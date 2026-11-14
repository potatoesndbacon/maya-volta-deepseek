# Feature flags and production state

Compared against official v1.1.60 `8b0a449c599fbb370a1dcc0979ae7c84d4310529`.
Values below were read from the running engine process; unset means not present
in that process, not proof a feature could never run under another configuration.
See `FLAG-OCCURRENCES.md` for every DS4/STRATA/MAYA name in added or removed
patch lines, exact source references, and the filtered live values. The source
has conditional paths, compile-time guards and aliases: setting a flag alone
is not proof a path executes. Some knobs are existing upstream knobs rather than
new implementation in this patch.

| Flag(s) | Meaning and default in reconstructed source | Live state |
|---|---|---|
| `DS4_UNDO_LOG` | Rewind horizon in tokens; default 0 disables per-layer log. | `16384` |
| `DS4_UNDO_HOST` | Put undo buffers in mapped pinned host memory rather than VRAM; default off. | `1` |
| `DS4_UNDO_VERIFY` | Read back and verify touched rewind slots; default off, diagnostic. | unset |
| `DS4_UNDO_FIRST` | Prototype in-place rewind before parking conversation slots; default off. | unset |
| `DS4_INDEX_ALL_FAST` | For eligible single-token, unrestricted indexer rows, select all when all positions fit top-k. Default off. | `1` |
| `DS4_INDEX_QUERY_SKIP` | Additionally skip query work in the applicable all-selected case; default off. | unset |
| `DS4_INDEX_CAND_ALL_FAST` | Extend fast selection to eligible candidate construction; default off. | unset |
| `DS4_INDEX_DIRECT_GATHER` | Direct row addressing for eligible selection; requires additional conditions including `DS4_INDEX_ALL_FAST` being present. Default off. | unset |
| `DS4_PACKED_ATTN` | Opt-in single-row packed FP4 attention path; source tests presence, so even `=0` is present. | unset |
| `DS4_PIPE_P2P` | Experimental direct pipeline peer copies; default off. | unset |
| `STRATA_DS4_AR_BLOCKS` | Reduce-scatter block count, bounded to existing flag layout; default original maximum 64. `MAYA_DS4_AR_BLOCKS` also present via aliasing. | `8` under both spellings |
| `STRATA_DS4_COMBINE_THREADS` | GGUF combine block size, rounded to warp multiples and capped at 256; invalid/small/unset falls back to 128. | unset: 128 |
| `STRATA_DS4_RESIDENT_BIAS`, `...BIAS_TOP` | Penalize nonresident expert selection, with separately configurable first pick. Defaults 0; first-pick default follows bias. | bias `0`; top unset |
| `STRATA_DS4_RESIDENT_ABS`, `...ABS_BLOCK` | Width-dependent nonresident expert selection penalty. Both default 0. Can change selected experts; mixture weights use unpenalized scores. | `1`, `2` (also mirrored as `MAYA_DS4_...`) |
| `STRATA_DS4_RESIDENT_TOPK`, `...SKIP_W` | Optional resident top-k/weight skip controls, both default 0. | both `0`, also mirrored |
| `STRATA_DS4_CACHE_SCORE` | Weight expert-cache heat by router weight; default off. | unset |
| `STRATA_DS4_NO_LANE_WAIT` | Diagnostic experiment that skips a CPU lane wait; default off. | unset |
| `STRATA_DS4_PROF` | Bounded plain-step CUDA profiler range `<start>[,<len>]`; default off. Not total speculative decode. | unset |
| `DS4_TRACE_DECODE`, `DS4_TRACE_WARMUP`, `DS4_TRACE_STEPS` | Main-loop profiler range and NVTX tracing; enable by presence of TRACE_DECODE; defaults warmup 8, steps 64. NVTX compile conditional on header. | unset |
| `MAYA_LOOP_TOOL_REPS_ANY`, `MAYA_LOOP_TOOL_BLOCK_ANY` | Whole-conversation repeated tool-call note/block thresholds. Source defaults 6/9. | `4` / `6` |
| `AR_TRACE`, `MX_TRACE`, `MAYA_HAVE_NVTX` | Compile-time trace switches/header detection, not ordinary runtime flags. AR/MX trace macros are commented out; NVTX availability depends on build headers. | no AR/MX trace build enabled in carried source |

Upstream runtime controls selected by this installation include `MAYA_DS4_HC_FUSE=2`,
`MAYA_DS4_ROUTE_FUSE=2`, `MAYA_DS4_DECODE_TRIM=15`, `DS4_IQ_RP=4`,
`DS4_IQ_AHEAD=1`, `DS4_TP_GGUF_UNEVEN=1`, `DS4_DRAFT_COST=1` and stock
DSpark. These are not automatically local source additions. The complete
filtered live process snapshot is in `production-settings.json`; it deliberately
omits unrelated process environment and credentials.

The executable was built using CUDA 12.8, SM70, Release `-O3`,
`DS4_CUDA_PEER_AR=1`, and `STRATA_NATIVE_EXPERTS=1`. The active configuration is
TP4, one serving slot, 1,048,576 context, prefill chunk 1024, DSpark on/confidence
0.5, vision on and four V100S at 150 W. These are machine-specific deployment
choices, not assumed upstream defaults.
