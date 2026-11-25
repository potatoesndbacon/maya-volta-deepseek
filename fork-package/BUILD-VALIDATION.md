# Clean build and bounded validation — 2026-11-13

The source fork configured and built successfully on the four-V100S host with CMake 3.31.6, CUDA 12.8.93, GCC 12.2, Release mode, `CMAKE_CUDA_ARCHITECTURES=70`, `STRATA_NATIVE_EXPERTS=ON`, `STRATA_ENABLE_CUDA=ON`, and `STRATA_BUILD_TESTS=ON`. The existing production build's llama.cpp dependency checkout was reused at revision `3cf03257f219afbe7334045ff7c6a06ac68c627d`. This was a new build directory, but is not an independent dependency fetch or a reproducible-binary proof. The system CMake 3.25.1 failed to configure CUDA C++20; CMake 3.31.6 succeeded. The generated executable SHA-256 was `2179f17e1fd89991e8f7eb6c17b9c1f734a431a9a8c20fae39a9146a72779a77`, different from the original production executable `a0cc17c94e17e398f7053947f3f062214d1e2fb410196f1cb8331add8a24429a`.

Five CTest checks passed: `ds4_undo_log_test`, `ds4_logprobs_test`, `ds4_iq512_rows`, `ds4_iq512_rows_default`, and `ds4_iq_host_test`. These exercise selected CPU and logical paths, not all GPU kernels or full-model numerical equivalence.

## Refresh — 2026-11-24

The package was regenerated from the production commit `c0e574b` (branch `maya-volta-deepseek-1.1.60-prod` in
`/home/oui/maya-v1.1.60-port`), which records the tree the running engine was built from. All 16 changed/added
files in this fork match that commit byte for byte (`source-file-identities.json`), and the patch applies to a
clean `8b0a449` export (`git apply --check`). The production executable is `3235039e…`, built 2026-11-22 12:50 in
`/home/oui/maya-v1.1.60-build`; no source file in the tree is newer than it. No rebuild of the production binary
was performed for this refresh. One separate build of the opt-in `bae1086` branch was made in
`/home/oui/maya-v1.1.60-pmask-build` (2 min 38 s, CMake 3.31.6, same cache options); it is not used by the service.

## Refresh — 2026-11-25

Regenerated from production commit `7215099` (adds `06fd953`, the vectorized undo commit, and the split drafter). All 16
changed/added files match the commit byte for byte, and the patch applies to a clean `8b0a449` export. The production
executable `71ef5c38…` was built in `/home/oui/maya-v1.1.60-build` from that tree. Checks run before promotion: undo commit
with `DS4_UNDO_VERIFY=1` (3 rewinds through speculatively committed rows, 1,180 slots, 0 bad); split drafter probe 391,
draft acceptance unchanged (80–88 % by position), quality suite 21/24 (twice) against 21/24 for a control. Measurements:
`/home/oui/MAYA-DECODE-CAPTURE-2026-11-24.md` §6–7.
