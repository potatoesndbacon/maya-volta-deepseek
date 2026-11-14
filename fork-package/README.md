# maya-volta-deepseek

Source changes carried on the official Maya v1.1.60 base for DeepSeek inference
on NVIDIA Volta (SM70). The branch is `maya-volta-deepseek-1.1.60`. The full
source patch is [`production-v1.1.60.patch`](production-v1.1.60.patch), and the
source-level descriptions are in [`DIFFERENCES.md`](DIFFERENCES.md).

## Changes and measured performance impact

The table covers every changed or added source file in the patch. “No isolated
measurement” means the code change is present, but available benchmark evidence
does not establish a separate performance gain for it. Historical measurements
are labeled and should not be read as predictions for a different model,
workload, or host.

| File / change | Performance impact |
|---|---|
| `CMakeLists.txt`, `src/ds4/ds4_undo_log.hpp`, `tests/ds4_undo_log_test.cpp` — register and define the undo-log index/rewind contract and its tests. | No inference-speed gain measured. |
| `serve/loop_guard.py` — detect repeated tool calls across the conversation and add configurable warning/block thresholds. | No inference-speed gain measured. |
| `serve/server.py` — report undo/slot reuse metrics and add non-streaming `/v1/sweep` for shared-prefix tails. | No throughput gain measured. |
| `serve/web/monitor.js` — display prefill reuse, undo rewinds, and misses. | No inference-speed gain measured. |
| `src/ds4/ds4_gguf.cu` — stage MoE combine metadata, allow configurable combine launch size, and adjust launch geometry. | Historical combine microbenchmark: 43.4→4.2 µs. The carried call path overwrites the staged mask pointer, so the figure is not an end-to-end gain for this fork. 32-thread option: no gain reported; current default is 128 threads. |
| `src/ds4/ds4_iq.cu` — add a full-width NC=16 quantized GEMV branch and Q6_K read-only loads. | NC=16 historical diagnostic: 2916.0→2877.1 ms over 192 tokens (1.3% faster). Q6 reversion comparison: retaining Q6 was 3.8–6.4% faster in short prompts. |
| `src/ds4/ds4_iq_cpu.cpp` — reuse per-thread gate/up layout until worker or row count changes. | No isolated gain measured. |
| `src/ds4/ds4_kernels.cu` — implement undo logging and rewind/commit kernels; configurable all-reduce blocks; FP4 unpack read-only loads; resident expert selection controls; attention dispatch and indexing options; device-position/row-gather groundwork; and optional profiler instrumentation. | **All-reduce:** historical code prompt 75.6→82.3 tok/s (+8.9%); prose unchanged. **Undo log:** specific 2.3K-token tail-edit test, 4.9× lower TTFT. **Indexer shortcut:** historical 512-token API test 32.99→36.09 tok/s (+9.4%). **FP4 unpack:** prompt-dependent; no consistent gain figure. **One-row attention preload:** prompt-dependent; no consistent gain figure. **Resident routing:** no isolated speed gain reported; changes expert selection. **Other options/capture groundwork:** no gain reported. |
| `src/ds4/ds4_kernels.cuh` — extend kernel argument structures and declarations for undo, device positions, direct gathers, and batched rewind/commit. | No isolated gain measured. |
| `src/ds4/ds4_main.cpp` — add bounded decode/NVTX capture, undo metrics, and optional undo-first restore selection. | No isolated gain measured. |
| `src/ds4/ds4_model.cu` — manage undo buffers and lifecycle; add indexer shortcuts, optional score cache/P2P/packed-attention paths, device-position capture groundwork, resident routing, and related dispatch. | Undo log: 4.9× lower TTFT in the specific tail-edit test. Indexer shortcut: +9.4% in the historical API test. Other optional paths: no gain reported. |
| `src/ds4/ds4_model.hpp` — expose undo counters in model statistics. | No inference-speed gain measured. |
| `src/kernels/cpu/iq_avx2.cpp` — add a compiler unroll hint to the IQ1_M row-dot loop. | No isolated gain measured. |

### End-to-end comparison

A bounded three-prompt API comparison measured median decode throughput as
follows. Each arm used the same model pack, stock DSpark, TP4, and up to 400
generated tokens; the production build was measured before and after the fork
build to expose run-to-run drift.

| Prompt | Production before | Fork build |
|---|---:|---:|
| Code | 96.3 tok/s | 98.3 tok/s | 
| Prose | 67.5 tok/s | 66.9 tok/s | 
| Workflow | 83.2 tok/s | 84.0 tok/s | 

The fork was slightly faster than the first production run on code and workflow
and slightly slower on prose. Production control rates drifted between runs;
response hashes also differed. With three measured requests per prompt, this
does not establish a repeatable overall speedup or output equivalence. Full
method and test limits are in [`BUILD-VALIDATION.md`](BUILD-VALIDATION.md).

## Build

The validated configuration used CMake 3.31.6, CUDA 12.8.93, GCC 12.2, Release
mode, `CMAKE_CUDA_ARCHITECTURES=70`, native experts, CUDA, and tests enabled.
For example:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CUDA_ARCHITECTURES=70
cmake --build build --target maya-ds4
```

Resolve upstream dependencies and review the configured options for your host.
The package contains no model weights, private service configuration, or
production executable. Stock source licensing is retained.

## Package files

- [`DIFFERENCES.md`](DIFFERENCES.md): source-level overview by changed file.
- [`FEATURE-FLAGS.md`](FEATURE-FLAGS.md): flags, defaults, and captured values.
- [`FLAG-OCCURRENCES.md`](FLAG-OCCURRENCES.md): mechanical map of identifiers
  found in changed lines, with source links.
- [`HUNK-INDEX.md`](HUNK-INDEX.md): index of changed patch hunks.
- [`BUILD-VALIDATION.md`](BUILD-VALIDATION.md): build, selected tests, and
  bounded API comparison details.
- [`production-settings.json`](production-settings.json): filtered runtime
  settings snapshot; not a complete environment dump.
- [`source-file-identities.json`](source-file-identities.json) and
  [`verification.json`](verification.json): source and patch audit metadata.
