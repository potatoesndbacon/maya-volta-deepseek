# Flag and symbol occurrence index

Mechanical inventory of DS4, STRATA and MAYA names appearing in added or removed patch lines (regenerated 2026-11-24 from the production commit). It includes compile-time symbols and comments as well as runtime variables; use FEATURE-FLAGS.md and the referenced source for semantics. "Configured" is the value the service would start with (`production-settings.json`), not a live process read.

## `DS4_INDEX_ALL_FAST`

Configured: `DS4_INDEX_ALL_FAST=1`.

- added in [src/ds4/ds4_model.cu:21631](../src/ds4/ds4_model.cu#L21631), [src/ds4/ds4_model.cu:21853](../src/ds4/ds4_model.cu#L21853)

## `DS4_INDEX_CAND_ALL_FAST`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:21641](../src/ds4/ds4_model.cu#L21641)

## `DS4_INDEX_DIRECT_GATHER`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:21850](../src/ds4/ds4_model.cu#L21850)

## `DS4_INDEX_QUERY_SKIP`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:21635](../src/ds4/ds4_model.cu#L21635)

## `DS4_PACKED_ATTN`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_kernels.cu:4701](../src/ds4/ds4_kernels.cu#L4701)

## `DS4_PIPE_P2P`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:1417](../src/ds4/ds4_model.cu#L1417), [src/ds4/ds4_model.cu:3785](../src/ds4/ds4_model.cu#L3785), [src/ds4/ds4_model.cu:3787](../src/ds4/ds4_model.cu#L3787), [src/ds4/ds4_model.cu:21408](../src/ds4/ds4_model.cu#L21408), [src/ds4/ds4_model.cu:21419](../src/ds4/ds4_model.cu#L21419), [src/ds4/ds4_model.cu:21462](../src/ds4/ds4_model.cu#L21462)

## `DS4_TRACE_DECODE`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:48](../src/ds4/ds4_main.cpp#L48)

## `DS4_TRACE_STEPS`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:50](../src/ds4/ds4_main.cpp#L50)

## `DS4_TRACE_WARMUP`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:49](../src/ds4/ds4_main.cpp#L49)

## `DS4_UNDO_FIRST`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:1185](../src/ds4/ds4_main.cpp#L1185), [src/ds4/ds4_main.cpp:1557](../src/ds4/ds4_main.cpp#L1557), [src/ds4/ds4_main.cpp:1560](../src/ds4/ds4_main.cpp#L1560), [src/ds4/ds4_main.cpp:1561](../src/ds4/ds4_main.cpp#L1561)

## `DS4_UNDO_HOST`

Configured: `DS4_UNDO_HOST=1`.

- added in [src/ds4/ds4_model.cu:9881](../src/ds4/ds4_model.cu#L9881), [src/ds4/ds4_model.cu:9886](../src/ds4/ds4_model.cu#L9886)

## `DS4_UNDO_LOG`

Configured: `DS4_UNDO_LOG=8192`.

- added in [CMakeLists.txt:263](../CMakeLists.txt#L263), [serve/server.py:3330](../serve/server.py#L3330), [serve/server.py:6562](../serve/server.py#L6562), [serve/web/monitor.js:212](../serve/web/monitor.js#L212), [serve/web/monitor.js:231](../serve/web/monitor.js#L231), [src/ds4/ds4_kernels.cu:997](../src/ds4/ds4_kernels.cu#L997), [src/ds4/ds4_kernels.cu:2511](../src/ds4/ds4_kernels.cu#L2511), [src/ds4/ds4_kernels.cu:4801](../src/ds4/ds4_kernels.cu#L4801), [src/ds4/ds4_kernels.cuh:347](../src/ds4/ds4_kernels.cuh#L347), [src/ds4/ds4_kernels.cuh:367](../src/ds4/ds4_kernels.cuh#L367), [src/ds4/ds4_main.cpp:1470](../src/ds4/ds4_main.cpp#L1470), [src/ds4/ds4_model.cu:965](../src/ds4/ds4_model.cu#L965) (+10 more)

## `DS4_UNDO_VERIFY`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:9891](../src/ds4/ds4_model.cu#L9891), [src/ds4/ds4_model.cu:9896](../src/ds4/ds4_model.cu#L9896), [src/ds4/ds4_model.cu:10002](../src/ds4/ds4_model.cu#L10002), [tests/ds4_undo_log_test.cpp:130](../tests/ds4_undo_log_test.cpp#L130), [tests/ds4_undo_log_test.cpp:166](../tests/ds4_undo_log_test.cpp#L166)

## `MAYA_DS4_RESIDENT_SKIP_W`

Configured: `STRATA_DS4_RESIDENT_SKIP_W=0`.

- removed in [src/ds4/ds4_kernels.cu:3863](../src/ds4/ds4_kernels.cu#L3863), [src/ds4/ds4_kernels.cu:3880](../src/ds4/ds4_kernels.cu#L3880)

## `MAYA_DS4_RESIDENT_TOPK`

Configured: `STRATA_DS4_RESIDENT_TOPK=0`.

- removed in [src/ds4/ds4_kernels.cu:3863](../src/ds4/ds4_kernels.cu#L3863), [src/ds4/ds4_kernels.cu:3880](../src/ds4/ds4_kernels.cu#L3880)

## `MAYA_DS4_THP`

Configured: `MAYA_DS4_THP=0`.

- added in [src/ds4/ds4_os.hpp:504](../src/ds4/ds4_os.hpp#L504), [src/ds4/ds4_os.hpp:507](../src/ds4/ds4_os.hpp#L507)

## `MAYA_HAVE_NVTX`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:41](../src/ds4/ds4_main.cpp#L41), [src/ds4/ds4_main.cpp:43](../src/ds4/ds4_main.cpp#L43), [src/ds4/ds4_main.cpp:2539](../src/ds4/ds4_main.cpp#L2539), [src/ds4/ds4_main.cpp:2557](../src/ds4/ds4_main.cpp#L2557), [src/ds4/ds4_main.cpp:2574](../src/ds4/ds4_main.cpp#L2574), [src/ds4/ds4_main.cpp:2604](../src/ds4/ds4_main.cpp#L2604)

## `MAYA_LOOP_TOOL_BLOCK_ANY`

Configured: `MAYA_LOOP_TOOL_BLOCK_ANY=6`.

- added in [serve/loop_guard.py:305](../serve/loop_guard.py#L305)

## `MAYA_LOOP_TOOL_REPS_ANY`

Configured: `MAYA_LOOP_TOOL_REPS_ANY=4`.

- added in [serve/loop_guard.py:304](../serve/loop_guard.py#L304)

## `MAYA_TOKEN_STEP`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_main.cpp:2540](../src/ds4/ds4_main.cpp#L2540), [src/ds4/ds4_main.cpp:2575](../src/ds4/ds4_main.cpp#L2575)

## `STRATA_DS4_AR_BLOCKS`

Configured: `STRATA_DS4_AR_BLOCKS=8`.

- added in [src/ds4/ds4_kernels.cu:5047](../src/ds4/ds4_kernels.cu#L5047), [src/ds4/ds4_kernels.cu:5052](../src/ds4/ds4_kernels.cu#L5052)

## `STRATA_DS4_CACHE_SCORE`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:1795](../src/ds4/ds4_model.cu#L1795), [src/ds4/ds4_model.cu:1801](../src/ds4/ds4_model.cu#L1801)

## `STRATA_DS4_COMBINE_THREADS`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_gguf.cu:915](../src/ds4/ds4_gguf.cu#L915), [src/ds4/ds4_gguf.cu:922](../src/ds4/ds4_gguf.cu#L922)

## `STRATA_DS4_NO_LANE_WAIT`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:22338](../src/ds4/ds4_model.cu#L22338), [src/ds4/ds4_model.cu:22342](../src/ds4/ds4_model.cu#L22342)

## `STRATA_DS4_PROF`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_model.cu:17817](../src/ds4/ds4_model.cu#L17817), [src/ds4/ds4_model.cu:17822](../src/ds4/ds4_model.cu#L17822)

## `STRATA_DS4_RESIDENT_ABS`

Configured: `STRATA_DS4_RESIDENT_ABS=1`.

- added in [src/ds4/ds4_kernels.cu:3875](../src/ds4/ds4_kernels.cu#L3875), [src/ds4/ds4_kernels.cu:3886](../src/ds4/ds4_kernels.cu#L3886)

## `STRATA_DS4_RESIDENT_ABS_BLOCK`

Configured: `STRATA_DS4_RESIDENT_ABS_BLOCK=2`.

- added in [src/ds4/ds4_kernels.cu:3876](../src/ds4/ds4_kernels.cu#L3876), [src/ds4/ds4_kernels.cu:3887](../src/ds4/ds4_kernels.cu#L3887)

## `STRATA_DS4_RESIDENT_BIAS`

Configured: `STRATA_DS4_RESIDENT_BIAS=0`.

- added in [src/ds4/ds4_kernels.cu:3864](../src/ds4/ds4_kernels.cu#L3864), [src/ds4/ds4_kernels.cu:3883](../src/ds4/ds4_kernels.cu#L3883), [src/ds4/ds4_kernels.cuh:474](../src/ds4/ds4_kernels.cuh#L474)

## `STRATA_DS4_RESIDENT_BIAS_TOP`

Configured: not set under the listed spelling(s)..

- added in [src/ds4/ds4_kernels.cu:3885](../src/ds4/ds4_kernels.cu#L3885), [src/ds4/ds4_kernels.cuh:475](../src/ds4/ds4_kernels.cuh#L475)

## `STRATA_DS4_RESIDENT_SKIP_W`

Configured: `STRATA_DS4_RESIDENT_SKIP_W=0`.

- added in [src/ds4/ds4_kernels.cu:3863](../src/ds4/ds4_kernels.cu#L3863), [src/ds4/ds4_kernels.cu:3881](../src/ds4/ds4_kernels.cu#L3881)

## `STRATA_DS4_RESIDENT_TOPK`

Configured: `STRATA_DS4_RESIDENT_TOPK=0`.

- added in [src/ds4/ds4_kernels.cu:3863](../src/ds4/ds4_kernels.cu#L3863), [src/ds4/ds4_kernels.cu:3882](../src/ds4/ds4_kernels.cu#L3882)
