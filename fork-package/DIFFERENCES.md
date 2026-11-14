# Production source differences from stock Maya v1.1.60

> Follow-up: [isolated reversion A/B](/home/oui/MAYA-CARRIED-KERNEL-AB-RESULTS-2026-11-13.md) measured Q6, FP4 and single-row attention separately. The Q6 reversion was slower against both surrounding controls on the three short prompts; Q6 remains in production at the user’s direction. See the report for FP4/attention results and limitations.

This is a source review of the archived custom baseline reconstructed on the
actual upstream v1.1.60 tag, not a claim that every conditional path is active
or that every optimization was independently requalified on the current model.
The complete reproducible diff is `production-v1.1.60.patch`, including both
new files; `HUNK-INDEX.md` lists every one of its 115 hunks. For exact
instructions, edge conditions and ordering, review the patch and source.

| Changed file | Source-level change and behavior |
|---|---|
| `CMakeLists.txt` | Registers the new undo-log test when `STRATA_BUILD_TESTS` is enabled. |
| `serve/loop_guard.py` | Counts repeated tool calls across the full conversation and adds configurable warning/block thresholds, alongside existing recent-window checks. This changes tool-loop behavior. |
| `serve/server.py` | Adds undo and slot reuse counters to metrics and a non-streaming `/v1/sweep` endpoint for multiple tails sharing a prefix. |
| `serve/web/monitor.js` | Adds the prefill reuse card, separating slot/shared-prefix reuse from undo rewinds and misses. |
| `src/ds4/ds4_gguf.cu` | GGUF MoE combine gains dynamic launch size and device-side staging of host-mapped flag/mask metadata. The current carried call sites overwrite the staged `cpu_pmask` with the original pointer after assignment, so full two-array staging is not established. |
| `src/ds4/ds4_iq.cu` | Quantized GEMV adds a full-width `NC=16,ncol=16` branch that avoids column checks and Q6_K read-only `__ldg` loads in both branches. Conditional on actual type and width. |
| `src/ds4/ds4_iq_cpu.cpp` | Reuses per-thread gate/up layout until row count or worker count changes. Applies to that CPU IQ path. |
| `src/ds4/ds4_kernels.cu` | Main CUDA changes: undo ring/compressor recording, accepted-row commit and rewind kernels; optional device position/row-bound addressing and replication gather; FP4 unpack read-only loads; direct row gather and optional packed attention; resident expert selection controls; configurable all-reduce blocks; T=1 sparse attention preload dispatch 8→4; optional compiled trace instrumentation. |
| `src/ds4/ds4_kernels.cuh` | Extends kernel argument structs and function declarations for device positions, direct gathers, undo buffers/batched rewind and commit. |
| `src/ds4/ds4_main.cpp` | Adds conditional bounded decode/NVTX capture, exposes undo metrics, and optional in-place undo-first conversation restore selection. |
| `src/ds4/ds4_model.cu` | Allocates per-device/per-sequence undo state, manages ownership and restore coverage, commits accepted speculative rows, rewinds or falls back to checkpoints and counts outcomes. Also adds indexer shortcuts, optional scored cache and P2P pipeline, optional lane-wait bypass, profiler ranges, device-position/capture groundwork, staged replication, and associated dispatch/lifecycle updates. |
| `src/ds4/ds4_model.hpp` | Exposes undo counters in model statistics. |
| `src/ds4/ds4_undo_log.hpp` | New host-side circular overwrite/rewind index math and shared contract. |
| `src/kernels/cpu/iq_avx2.cpp` | Compiler unroll hint in the IQ1_M row-dot loop. Actual impact depends on CPU dispatch and quantization. |
| `tests/ds4_undo_log_test.cpp` | New standalone tests of overwrite/restore index contract. |

Important status distinctions:

- `DS4_UNDO_LOG=16384` and `DS4_UNDO_HOST=1` are live. Undo-first is a separate
  unset opt-in. Capture groundwork is present; whole-token CUDA graph capture
  was not completed or promoted.
- Resident ABS and ABS_BLOCK are active and can change expert selection. It is
  not correct to describe this entire configuration as numerically identical to
  unbiased stock routing. Output/intelligence equivalence is not established.
- The carried Q6_K `__ldg`, FP4 unpack `__ldg`, and T=1 sparse-attention preload
  of 4 appear in the patch despite old 7L/7N/7O rejection notes. A current
  isolated reversion A/B is recorded above; results vary by workload and
  generated sequence, so same-input parity and broader serving effects remain
  unqualified. Treat the historical mismatch explicitly during upstream review.
- Packed attention, direct gather, P2P, scored cache and no-lane-wait are
  compiled/available but unset in the captured production process. Several are
  experimental and should be evaluated independently before proposing upstream.
- Recent Q8 integer-ILP, NC5/NC8, tensor-core, AR/HC, Markov partial and EAGLE
  work was not promoted and is not included as production changes.

`FEATURE-FLAGS.md` records switches and effective settings. `FLAG-OCCURRENCES.md`
is a mechanical map of every project-prefixed identifier appearing in changed
lines. `production-settings.json` provides the filtered live configuration.


### Combined and preload-8 reversion qualification — 2026-11-13

[Combined FP4/preload reversion](/home/oui/MAYA-COMBINED-REVERSIONS-AB-2026-11-13.md) did not establish a broad gain. A [fixed-input preload-8 test](/home/oui/MAYA-ATTENTION-PRELOAD8-FIXED-2026-11-13.md) found 0.58–1.20% shorter replay time with some verdict variation. In the larger [decode bracket](/home/oui/MAYA-ATTENTION-PRELOAD8-DECODE-2026-11-13.md), preload 8 was slower than the final code control and both workflow controls; prose improved slightly. Control drift and differing speculative outputs constrain attribution. No reversion was promoted; production and fork retain Q6, FP4 and preload-4 paths.
