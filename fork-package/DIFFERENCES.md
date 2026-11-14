# Source differences from Maya v1.1.60

This package carries a reconstructed source patch against the official Maya
v1.1.60 commit `8b0a449c599fbb370a1dcc0979ae7c84d4310529`. The complete diff is
[`production-v1.1.60.patch`](production-v1.1.60.patch); its changed hunks are
indexed in [`HUNK-INDEX.md`](HUNK-INDEX.md). Performance observations for each
change are summarized in the [README](README.md#changes-and-measured-performance-impact).

| Changed file | Source change |
|---|---|
| `CMakeLists.txt` | Registers the undo-log contract test when project tests are enabled. |
| `serve/loop_guard.py` | Adds whole-conversation repeated-tool detection with configurable warning and block thresholds. |
| `serve/server.py` | Adds undo/slot-reuse metrics and a non-streaming `/v1/sweep` endpoint for multiple tails sharing a prefix. |
| `serve/web/monitor.js` | Adds a prefill reuse card distinguishing slot/shared-prefix reuse, undo rewinds, and misses. |
| `src/ds4/ds4_gguf.cu` | Adds combine metadata staging and configurable combine launch size. The current call path does not establish use of the staged mask pointer. |
| `src/ds4/ds4_iq.cu` | Adds an NC=16 full-width quantized GEMV branch and Q6_K read-only loads. |
| `src/ds4/ds4_iq_cpu.cpp` | Reuses per-thread gate/up layout until row count or worker count changes. |
| `src/ds4/ds4_kernels.cu` | Adds undo recording/commit/rewind kernels, configurable all-reduce blocks, FP4 unpack read-only loads, resident-expert selection controls, optional attention/indexing paths, device-position and gather groundwork, and profiling instrumentation. |
| `src/ds4/ds4_kernels.cuh` | Extends kernel data structures and declarations for device positions, gathers, undo buffers, batched rewind, and commit. |
| `src/ds4/ds4_main.cpp` | Adds bounded decode/NVTX capture, undo metrics, and optional undo-first conversation restore. |
| `src/ds4/ds4_model.cu` | Manages undo state and restore lifecycle; adds indexer shortcuts and optional scored-cache, P2P, packed-attention, device-position, and resident-routing paths. |
| `src/ds4/ds4_model.hpp` | Exposes undo counters in model statistics. |
| `src/ds4/ds4_undo_log.hpp` | Defines host-side circular overwrite/rewind index math and the shared undo contract. |
| `src/kernels/cpu/iq_avx2.cpp` | Adds a compiler unroll hint to the IQ1_M row-dot loop. |
| `tests/ds4_undo_log_test.cpp` | Tests the undo-log overwrite and restore index contract. |

The active captured settings include `DS4_UNDO_LOG=16384`, `DS4_UNDO_HOST=1`,
`DS4_INDEX_ALL_FAST=1`, `STRATA_DS4_AR_BLOCKS=8`, and resident routing controls.
Optional paths such as direct gather, packed attention, P2P, score-weighted cache,
and lane-wait bypass were unset in that snapshot. See
[`FEATURE-FLAGS.md`](FEATURE-FLAGS.md) and
[`production-settings.json`](production-settings.json) for the captured
configuration. Resident routing can change which experts are selected, so
output equivalence is not established by source inspection or throughput tests.
