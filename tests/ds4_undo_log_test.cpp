// maya-ds4's undo log (src/ds4/ds4_undo_log.hpp): the replay contract Impl::restore() will implement.  No GPU, no
// model: the window ring (win slots) and the compressor's partial group (ratio slots) are buffers of opaque values,
// written one token at a time; the undo log records the overwritten slot so a rewind reconstructs the state at an
// earlier position exactly - the thing the token-equivalence gate (§5 of MAYA-PHASE8BL) turns into 64 identical ids.
// Proves: (1) rewind is exact at arbitrary positions, group-aligned or not (the "8-token group" is a replay batch, not
// a correctness limit); (2) the horizon bound: covered() is exactly pos - limit <= H; (3) H=0 keeps writes bit-identical
// (no log, same slots).
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/ds4/ds4_undo_log.hpp"

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

using strata::ds4::UndoLog;

namespace {

// a content value that is a function of the position (the "row" written at token p).  kEmpty is 0 because the real
// ring and compressor are zeroed before the first token (reset() memsets them), so an unwritten slot's value is 0 - and
// content(p) = p + 1 never collides with it (a real row's bytes need not be non-zero; the log stores the actual bytes,
// which is why there is no sentinel ambiguity in the engine).
using V = int64_t;
constexpr V kEmpty = 0;
V content(int64_t p) { return p + 1; }

// the state at a position: the ring (win slots) and the compressor (ratio slots), each slot the content of the latest
// token that wrote it (0: none yet).  The compressor's fold output (the pooled latent / ckv rows) is indexed by
// position, not captured here - exactly what Check/restore capture today (the partial group only).
struct State {
    UndoLog<V> ring, comp;
    int64_t pos = 0;

    State(int win, int ratio, int H) : ring(win, H), comp(ratio, H) {}

    void write(int64_t p) { ring.write(p, content(p)); comp.write(p, content(p)); pos = p + 1; }

    bool rewind(int64_t limit) {
        if (!ring.covered(limit, pos) || !comp.covered(limit, pos)) return false;
        ring.rewind(limit, pos);
        comp.rewind(limit, pos);
        pos = limit;
        return true;
    }

    // the exact slot content after tokens 0 .. pos-1, in closed form (the truth model, independent of the log): the
    // latest token p < pos with p % win == s, i.e. p = pos-1 - ((pos-1-s) mod win), if >= 0.
    V slot_truth(const UndoLog<V>& b, int s) const {
        if (pos <= 0) return kEmpty;
        const int64_t d = ((pos - 1 - s) % b.win + b.win) % b.win;
        const int64_t p = pos - 1 - d;
        return p >= 0 ? content(p) : kEmpty;
    }
    bool matches_truth() const {
        for (int s = 0; s < ring.win; ++s)
            if (ring.slot[(size_t) s] != slot_truth(ring, s)) return false;
        for (int s = 0; s < comp.win; ++s)
            if (comp.slot[(size_t) s] != slot_truth(comp, s)) return false;
        return true;
    }
};

void check(int win, int ratio, int H, int64_t P) {
    State st(win, ratio, H);
    for (int64_t p = 0; p < P; ++p) st.write(p);
    assert(st.matches_truth());

    // rewind to every position the horizon covers, and a few it does not
    for (int64_t limit = 0; limit <= P; ++limit) {
        State re = st;
        const bool ok = re.rewind(limit);
        const bool expect = H > 0 && P - limit <= H;
        assert(ok == expect);
        if (expect) {
            assert(re.pos == limit);
            assert(re.matches_truth());
            // the device kernels' direct (one block a slot) formulation must equal the sequential reference
            State rd = st;
            rd.ring.rewind_direct(limit, P);
            rd.comp.rewind_direct(limit, P);
            assert(rd.ring.slot == re.ring.slot);
            assert(rd.comp.slot == re.comp.slot);
        }
    }
    // a couple of positions well past the horizon (must refuse, never silently approximate)
    if (H > 0) {
        State re = st;
        assert(!re.rewind(P - H - 1));
        assert(re.pos == P && re.matches_truth());   // untouched on refusal
    }
}

}  // namespace

// Append and rewind interleaved, the way an edit-and-resubmit session actually drives the cache: write at pos, or
// jump back within the horizon and continue from there.  After every operation the slots must equal the closed-form
// truth, and the device formulation (rewind_direct) must equal the sequential one.  This is deliberately not a single
// forward pass - it exercises repeated rewinds without an intervening re-read, the case the engine's slot/undo tiers
// hit when a client edits twice in a row.
void check_random(int win, int ratio, int H, int steps, uint32_t seed) {
    State st(win, ratio, H);
    uint32_t x = seed;
    auto rnd = [&]() { x = x * 1664525u + 1013904223u; return x; };
    for (int i = 0; i < steps; ++i) {
        const uint32_t r = rnd();
        if ((r % 100u) < 55u || st.pos <= 0) {
            st.write(st.pos);                          // prefill / decode: append at pos
        } else if (H > 0) {
            const int64_t span = st.pos < H ? st.pos : H;
            const int64_t limit = st.pos - (int64_t) (rnd() % (uint32_t) (span + 1));
            if (st.pos - limit <= H) {
                State re = st;
                assert(re.rewind(limit));
                assert(re.pos == limit && re.matches_truth());
                State rd = st;
                rd.ring.rewind_direct(limit, st.pos);
                rd.comp.rewind_direct(limit, st.pos);
                assert(rd.ring.slot == re.ring.slot && rd.comp.slot == re.comp.slot);
                st = re;                                // continue from the rewound state
            }
        }
        assert(st.matches_truth());
    }
}

// A write the log did not record (a missed hook) restores the WRONG slot with no error at all - the silent fault
// DS4_UNDO_VERIFY catches on the device.  The proof here: the all-logged log and the one-miss log differ, and the
// rewind reports nothing.
void missed_hook_fails() {
    const int win = 4, H = 8;
    auto build = [](bool miss) {
        UndoLog<V> b(win, H);
        for (int64_t p = 0; p < 8; ++p) {
            if (miss && p == 5) {
                b.slot[(size_t) (p % win)] = content(p);   // the write happened; the log entry did not
                continue;
            }
            b.write(p, content(p));
        }
        return b;
    };
    UndoLog<V> good = build(false), missed = build(true);
    good.rewind(2, 8);                                 // limit 2: slot 1's p_min is 5, whose entry the miss skipped
    missed.rewind(2, 8);                               // returns void: no error, no signal
    assert(good.slot[1] == content(1));                // correct: the value before token 5
    assert(missed.slot[1] == kEmpty);                  // silently wrong - exactly what the device verify flags
    assert(good.slot[1] != missed.slot[1]);
}

int main() {
    // (win, ratio, H, P): a 1 M context with a 4096 window, ratio-8 compressor, horizons 0 / 1024 / 2048 / 4096
    check(4096, 8, 0, 5000);
    check(4096, 8, 1024, 5000);
    check(4096, 8, 2048, 5000);
    check(4096, 8, 4096, 23425);   // the 23 K handoff conversation, H = 4096
    check(512, 16, 4096, 2000);    // a smaller window, ratio 16 (a 16-token group)
    check(8, 8, 128, 300);         // win == ratio: rewind across many wraps

    // randomized append/rewind interleaving, against the truth model (several seeds, a small and a large window)
    for (uint32_t seed : {1u, 7u, 1234u, 99991u}) check_random(64, 8, 256, 4000, seed);
    for (uint32_t seed : {3u, 271828u}) check_random(512, 16, 4096, 4000, seed);

    // the silent-failure mode the device-side DS4_UNDO_VERIFY exists to catch
    missed_hook_fails();

    std::printf("ds4_undo_log_test: ok\n");
    return 0;
}
