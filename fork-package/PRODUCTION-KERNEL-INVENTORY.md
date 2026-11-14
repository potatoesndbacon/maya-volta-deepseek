# Current production kernel-change inventory — 2026-11-13

> **Carried kernel A/B update (2026-11-13):** [Results](/home/oui/MAYA-CARRIED-KERNEL-AB-RESULTS-2026-11-13.md) supersede the earlier “no current isolated A/B” notes for Q6, FP4 unpack and one-row attention. Reverting Q6 was slower than both surrounding controls on all three short prompts, so the user chose to retain it. Output sequences varied; matched-input numerical parity and broad serving benefit remain unproven. See the report for FP4 and attention results.

## Scope and proof

This inventory covers **local code changes carried into the current production
Maya 1.1.60 SM70 executable**, plus upstream kernel paths selected by local
configuration. It is not a claim that every compiled branch executes for every
request or that a historical speedup carries to the current model. The exact
full patch against upstream 1.1.60 is
[custom-baseline.patch](/home/oui/archive/maya-checkpoints/20261112-q8-integer-ilp/custom-baseline.patch);
a [complete hunk index](/home/oui/MAYA-PRODUCTION-SOURCE-HUNK-INDEX-2026-11-13.md)
provides an exhaustive map, including host, header and non-kernel changes.

Production binary: `/home/oui/maya-v1.1.60-build/maya-ds4`, SHA-256
`a0cc17c94e17e398f7053947f3f062214d1e2fb410196f1cb8331add8a24429a`.
The [8CH migration record](/home/oui/MAYA-PHASE-INVENTORY.md:1420) says it
ported local work to upstream tag 1.1.60 `8b0a449`, resolved conflicts and
built for SM70. The [Q8 archive](/home/oui/archive/maya-checkpoints/20261112-q8-integer-ilp/CHECKPOINT.md)
records a reconstructed upstream 1.1.60 + custom patch and verifies six core
source files, while the later [NC5 checkpoint](/home/oui/archive/maya-checkpoints/20261113-nc5-stall/CHECKPOINT.md)
records this exact binary and a source archive. This is strong paper-trail
provenance, not an independent byte-for-byte reconstruction of every linked
object from source; the live git worktree has unresolved index conflicts and
must not be used as sole proof of the built binary. Original GGUF-object SASS
was independently audited against the NC5 control (68 existing functions
matched); the recent diagnostic's model object differs from production.

Current context: 4× V100S SM70, 150 W, TP4, stock DSpark, one serving slot,
1M configured context, Huihui UD-IQ2_XXS. Build: CUDA 12.8, `-O3`,
`compute_70,sm_70`, native experts and CUDA peer-AR compiled. Pack codecs are
mixed. Effective settings are from the service config/environment and the
checkpoint's effective process environment, not old 1.1.54 handoff settings.

## Local GPU kernel and launch changes in the carried source

| Change and affected kernels | Production setting / observed status | Evidence and interpretation |
| --- | --- | --- |
| **Collective launch geometry** — `allreduce` makes `ar_rs16_kernel` / `ar_rs_kernel` block count configurable and clamps it to the existing flag layout. | `STRATA_DS4_AR_BLOCKS=8` (also mirrored under `MAYA_DS4_`). Applies when the relevant collective path is selected. | Phase 8BA measured 64→8 blocks, historical code 75.6→82.3 tok/s, prose unchanged; that result is from the earlier configuration, not a fresh current A/B. The release/acquire fence rewrite was reverted. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:1167). |
| **Combine metadata staging** — new `comb_flags_kernel` and per-device buffer; `mx_combine_kernel` supports dynamic block size. | Staging helper is compiled and was observed 3,935 times in one stock diagnostic trace; `STRATA_DS4_COMBINE_THREADS` is unset, so source default is 128 threads. | 8BM's older experiment reported 43.4→4.2 µs, but current carried GGUF call sites assign `c.cpu_pmask = a.cpu_pmask` **after** assigning the staged pointer, so that pointer is overwritten; flag staging remains, full two-array staging is not established. The IQ paths are not shown to receive this GGUF staging either. Do not repeat the historical 10.3× claim as a current end-to-end result. [Kernel investigation](/home/oui/MAYA-KERNEL-INVESTIGATION-2026-11-12.md). |
| **Quantized matvec full-width fast path** — `gguf::iq::mv_kernel<T,16>` skips per-column bounds checks for exactly 16 activation columns. | Compiled; conditional on NC=16 and actual `ncol=16`, not all decode calls. | Phase 7F promoted an earlier native diagnostic (2916.0→2877.1 ms for 192 tokens); no current-workload incremental A/B. The path was originally motivated by IQ1_M, but the template applies to supported types. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:98). |
| **Q6_K matvec read-only loads** — `mv_kernel` uses `__ldg` for quantized input words/scales when `T == kQ6_K`, both full-width and general path. | Present in the carried source patch; stock trace observes Q6_K matvecs on all four GPUs. Whether any individual instruction survives compilation requires separate binary-level inspection. | 7L had rejected the related change on the older setup. In the new short-prompt test, reverting the carried change increased median decode time 3.8–6.4% against both surrounding controls; generated sequences varied, so exact causal gain and parity remain unproven. User chose to keep the current path. [Results](/home/oui/MAYA-CARRIED-KERNEL-AB-RESULTS-2026-11-13.md). |
| **FP4 compressed-attention unpack loads** — `unpack32` uses `__ldg` for packed words. | Present in carried source, executed only where that unpack path is used. | 7N rejected a related change on the older setup. The new reversion was slower on code but faster on prose/workflow relative to both surrounding controls; sequences varied. The current incremental effect remains inconclusive. [Results](/home/oui/MAYA-CARRIED-KERNEL-AB-RESULTS-2026-11-13.md). |
| **One-row sparse-attention preload** — CUDA T=1 scratch dispatch selects `sparse_attn_kernel<true,4>` instead of upstream's `<true,8>`. | Present in carried patch; 2,240 `<true,4>` launches appear in the saved stock diagnostic trace. This is a diagnostic trace with the production kernel object, not proof of frequency over all serving. | 7O rejected preload 4 on the older model. In the new short-prompt test, preload 8 was faster on code/prose but mixed on workflow; code used substantially fewer speculative rounds and sequences differed, so the effect is not isolated from workload variation. [Results](/home/oui/MAYA-CARRIED-KERNEL-AB-RESULTS-2026-11-13.md). |
| **All-selected indexer shortcut** — host dispatch in `attn_sel` and `iota_rows` bypasses scoring/top-k for eligible single-row windows when all compressed positions fit the top-k, subject to existing candidate restrictions. | `DS4_INDEX_ALL_FAST=1`; eligibility varies by layer and context. | Phase 8B reported 32.99→36.09 tok/s in a historical 512-token API comparison; not a current A/B. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:150). |
| **Direct compressed-row unpack** — `AttnRows.direct` permits `unpack_sel_kernel` to use the row index directly. | Code is present; its extra `DS4_INDEX_DIRECT_GATHER` gate is not set in the recorded production environment, so do not call it an active default. | Phase 8E measured a regression and rejected this optional route. The ordinary `DS4_INDEX_ALL_FAST` shortcut is separate. |
| **Adaptive resident expert routing** — `route_publish_kernel` and `route_pair_kernel` apply resident penalties when selecting experts, including first-pick and width-dependent parameters; mixture weights derive from unpenalized scores. | `STRATA_DS4_RESIDENT_ABS=1`, `...ABS_BLOCK=2`, `...BIAS=0`, `...TOPK=0`, `...SKIP_W=0`. This is an active selection policy where the route paths and VRAM map apply. | It can alter *which* experts are selected despite keeping six picks; do not describe it as strictly identical to unbiased routing or as purely a performance-neutral kernel rewrite. Historical 8AV/8AZ evidence and user-observed quality are workload-specific. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:1163). |
| **Undo-log write and rewind** — `ext_row_to_ring` / `ext_to_ring*` log overwritten ring rows; `compress_kernel` logs overwritten compressor KV/score slots; `undo_commit_batch_kernel`, `ring_undo_batch_kernel`, `comp_undo_batch_kernel` commit accepted speculative rows and restore ring/compressor state. Per-sequence ownership and host-side lifecycle are part of the change. | `DS4_UNDO_LOG=16384`, `DS4_UNDO_HOST=1`; log hooks apply when enabled and relevant. Commit kernel was observed in current diagnostic traces. Slot-restored conversations and horizon limits constrain applicability. | 8BL/8BM/8BN measured and enabled the feature; historical 4.9× tail-edit TTFT was for a specific 2.3K-token edit, not general prefill or decode speed. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:1477). |
| **Device-position and compressed-row capture groundwork** — `finish_kernel` gains optional device position, row bound and destination offset; `ring_to_ext`, `ext_to_ring`, ring/compressor restore and undo-commit gain nullable position pointers; `compress_kernel` accepts device position; `rep_gather_kernel` computes source offset from device state. Host code publishes `pos_dev`/`ng_dev`, changes eligible finishes to padded/device-masked launches and stages replication. | Carried into source; individual branches are conditional and some position pointers are used by normal path. This is not a completed CUDA Graph implementation. The model may use a normal fallback for other shapes. | Phases 8BQ–8CC contain successive corrections and parity/needle checks; whole-token capture was attempted and reverted in 8CD because of per-layer host synchronization. Do not infer a current speed gain from the groundwork. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:1301). |
| **Combine block-size option** — `mx_combine_kernel` uses `blockDim.x`, launch bounds up to 256, and runtime `STRATA_DS4_COMBINE_THREADS`. | Unset: 128-thread source default, not the tested 32-thread candidate. | 8BE measured the 32-thread variant and rejected it; the option remains in source. [Ledger](/home/oui/MAYA-PHASE-INVENTORY.md:1200). |
| **AR and CPU-wait debug instrumentation** — conditional `AR_TRACE` / `MX_TRACE` device timestamps, counters and dumps. | Macros commented out in the carried source; disabled unless compiled in a different build. | Diagnostic infrastructure, not an active speed optimization. |

## CPU kernel modifications in the carried patch

* `ds4_iq_cpu.cpp`: caches per-thread gate/up buffer layout until thread count or
  row count changes, avoiding repeated pointer setup; conditional on the CPU IQ
  path. This is host-side CPU kernel plumbing, not a CUDA kernel.
* `src/kernels/cpu/iq_avx2.cpp`: compiler unroll hint on the IQ1_M row-dot loop.
  Whether this affects current CPU-lane experts depends on their actual codecs
  and CPU dispatch; no current whole-model incremental win is established.

## Existing upstream paths enabled by this installation

These are meaningful kernel choices, but the 1.1.60 local patch does **not** by
itself establish that we authored those upstream kernels:

* `MAYA_DS4_HC_FUSE=2`, `MAYA_DS4_ROUTE_FUSE=2`,
  `MAYA_DS4_DECODE_TRIM=15`: supported HC, route and handoff fusion/trim paths
  selected in the service config. The older v1.1.49 fusion qualification is not
  a stand-alone measurement of each flag on this 1.1.60 installation.
* `DS4_IQ_RP=4`, `DS4_IQ_AHEAD=1`: IQ kernel work scheduling/prefetch settings;
  their effect depends on codec, actual path and default/alias handling.
* `DS4_TP_GGUF_UNEVEN=1`: uneven per-member GGUF FFN splitting for the active
  TP4 installation; this is not evidence that the separately investigated
  Maya-XS uneven experimental branch passed its numerical checks.
* Four-way TP output-head projection and gather, Q8 dense GEMVs, existing MoE
  grouping, sparse attention, 1M cache/ring support and stock DSpark are part
  of the production baseline. They are not automatically attributable to a
  local kernel patch merely because the engine executes them.

## In source but inactive / excluded experiments

* `DS4_PACKED_ATTN` sparse packed-attention shortcut is opt-in and not set;
  phase 7P rejected it on the earlier workload. `DS4_INDEX_DIRECT_GATHER` is
  also unset. `DS4_PIPE_P2P`, `STRATA_DS4_NO_LANE_WAIT` and scored-cache
  opt-ins are not set; do not treat those experimental branches as active.
* Integer-ILP Q8 NC1/NC4, NC5, paired-column NC8, online tensor-core Q8,
  expert-codec serialization, HC overlap, AR→HC fusion and Markov partial
  reduction were standalone or isolated model experiments; none was promoted.
  EAGLE remains deferred and the live drafter is stock DSpark. [Census results](/home/oui/MAYA-CENSUS-LEADS-RESULTS-2026-11-13.md),
  [NC5](/home/oui/MAYA-NC5-DRAFTER-RESULTS-2026-11-13.md).
* The recent warmup startup join and head/cycle NVTX labels belonged solely to
  diagnostics; neither was installed in production. [Investigation archive](/home/oui/archive/maya-checkpoints/20261113-head-cycle-investigation/CHECKPOINT.md).

The paper trail contains historical claims and superseded deployments. When a
phase verdict conflicts with the 1.1.60 carried patch, this inventory reports
both rather than silently inferring the old verdict describes the current
binary. To prove exact source-to-binary identity for every kernel would require
a reproducible rebuild and comprehensive SASS audit; this inventory does not
claim one has been completed.
