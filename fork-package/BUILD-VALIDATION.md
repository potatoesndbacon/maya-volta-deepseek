# Clean build and bounded validation — 2026-11-13

The source fork configured and built successfully on the four-V100S host with CMake 3.31.6, CUDA 12.8.93, GCC 12.2, Release mode, `CMAKE_CUDA_ARCHITECTURES=70`, `STRATA_NATIVE_EXPERTS=ON`, `STRATA_ENABLE_CUDA=ON`, and `STRATA_BUILD_TESTS=ON`. The existing production build's llama.cpp dependency checkout was reused at revision `3cf03257f219afbe7334045ff7c6a06ac68c627d`. This was a new build directory, but is not an independent dependency fetch or a reproducible-binary proof. The system CMake 3.25.1 failed to configure CUDA C++20; CMake 3.31.6 succeeded. The generated executable SHA-256 was `2179f17e1fd89991e8f7eb6c17b9c1f734a431a9a8c20fae39a9146a72779a77`, different from the original production executable `a0cc17c94e17e398f7053947f3f062214d1e2fb410196f1cb8331add8a24429a`.

Five CTest checks passed: `ds4_undo_log_test`, `ds4_logprobs_test`, `ds4_iq512_rows`, `ds4_iq512_rows_default`, and `ds4_iq_host_test`. These exercise selected CPU and logical paths, not all GPU kernels or full-model numerical equivalence.

A bounded API comparison used the unchanged production binary/frontend, then the clean fork binary/frontend, then production again. Each load ran the same three code, prose and workflow prompts, with a warmup and three measured requests of up to 400 tokens per prompt, stock DSpark, TP4, the same model pack and frozen expert profile, and 150 W GPU limits. Diagnostic copy pinning and prompt warmup were disabled for all arms. Each ran in a systemd scope with `MemorySwapMax=0` and zero scope swap observed. These are diagnostic settings, not an exact production configuration benchmark. Median observed decode tok/s:

| Prompt | Production before | Clean fork | Production after |
|---|---:|---:|---:|
| Code | 96.3 | 98.3 | 92.0 |
| Prose | 67.5 | 66.9 | 68.2 |
| Workflow | 83.2 | 84.0 | 81.8 |

All measured requests returned 400 tokens. All three arms answered the arithmetic smoke test and generated the required tool call. Generated response hashes differed across every measured control/fork pair; controls themselves varied. A three-sample rate comparison is indicative, not a performance equivalence proof, and the smoke tests do not establish numerical equivalence or model quality. No NCU/NSYS replay was used. Raw request logs are not included in this package.
