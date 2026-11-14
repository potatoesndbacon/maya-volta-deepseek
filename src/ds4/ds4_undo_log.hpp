// src/ds4/ds4_undo_log.hpp - maya-ds4's undo log (DS4_UNDO_LOG, Phase 8BL): the contract the device kernels and
// Impl::restore() implement.  A restore point is a read stop (8BJ); an undo log snapshots without stopping the read:
// each write into a circular buffer (the window ring, the compressor's partial group) first records the overwritten
// slot's old value, so restore() can rewind to any position within the horizon H of the end instead of copying a full
// Check.  Pure logic, no GPU (tests/ds4_undo_log_test.cpp): slot contents are opaque values (a ring's row bytes, a
// compressor's raw row), positions are token indices.  The real code mirrors this exactly with win = g.win (ring) or
// ratio (compressor), H = DS4_UNDO_LOG (0: off), and batched launches for the rewind loop.
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace strata::ds4 {

// A circular undo log over one buffer of `win` slots, horizon H tokens (0: off).  write(p, v) is the buffer's write at
// token p; it records the overwritten slot's old value into log[p mod H], then stores v.  rewind(limit, pos) undoes the
// writes limit .. pos-1 in reverse, restoring the exact buffer state as it was after token limit-1 (before token limit).
// The invariant restore() relies on: rewind is exact iff every undone write's record is still in the log, i.e.
// covered(limit, pos) (pos - limit <= H); the engine falls back to a full Check when it is not.
template <class V>
struct UndoLog {
    int win = 0;               // the buffer's slots (g.win for the ring, ratio for the compressor)
    int H = 0;                 // the horizon in tokens (0: off, no record kept)
    std::vector<V> slot;       // the live buffer (win values)
    std::vector<V> log;        // H overwritten values, index (p mod H); empty when H == 0

    UndoLog(int win_, int H_) : win(win_), H(H_), slot((size_t) std::max(win_, 0)),
                               log(H_ > 0 ? (size_t) H_ : 0) {}

    bool on() const { return H > 0; }
    bool covered(int64_t limit, int64_t pos) const {
        return on() && limit >= 0 && pos >= limit && pos - limit <= (int64_t) H;
    }

    // the write at token p (the overwritten slot's old value recorded first)
    void write(int64_t p, V v) {
        const size_t s = (size_t) ((p % win + win) % win);
        if (on()) log[(size_t) ((p % H + H) % H)] = slot[s];
        slot[s] = v;
    }
    // undo the single write at token p (must be undone in reverse, highest p first)
    void undo(int64_t p) {
        const size_t s = (size_t) ((p % win + win) % win);
        slot[s] = log[(size_t) ((p % H + H) % H)];
    }
    // rewind the writes limit .. pos-1 (reverse order); requires covered(limit, pos)
    void rewind(int64_t limit, int64_t pos) {
        for (int64_t p = pos - 1; p >= limit; --p) undo(p);
    }
    // The device kernels' formulation (ring_undo_batch / comp_undo_batch): undoing in reverse leaves each slot holding
    // the earliest undone write's overwritten value - i.e. slot s's final value is log[p_min % H], p_min the smallest
    // position in [limit, pos-1] with p_min % win == s (none: the slot is untouched).  Equivalent to rewind(); the test
    // pins the equivalence.
    void rewind_direct(int64_t limit, int64_t pos) {
        for (int s = 0; s < win; ++s) {
            const int64_t pmin = limit + ((s - limit) % win + win) % win;
            if (pmin > pos - 1) continue;
            slot[(size_t) s] = log[(size_t) ((pmin % H + H) % H)];
        }
    }
};

}  // namespace strata::ds4
