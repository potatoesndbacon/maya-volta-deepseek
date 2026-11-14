# maya-volta-deepseek

Maya 1.1.60 V100 production source fork

This checkout is the official upstream v1.1.60 tag on branch
`maya-volta-deepseek-1.1.60`, with the archived production source changes
applied as uncommitted changes. The official repository is configured as `upstream`; add an `origin` remote
pointing to your own fork before pushing. No upstream write has been made.

- `BUILD-VALIDATION.md`: clean build, tests and bounded production comparison.
- `COMMIT-PLAN.md`: suggested commit groups and publishing steps.
- `DIFFERENCES.md`: file-by-file source and kernel review.
- `FEATURE-FLAGS.md`: behavior, source defaults and captured production values.
- `FLAG-OCCURRENCES.md`: mechanically extracted identifiers and source links.
- `HUNK-INDEX.md`: complete index of all changed hunks.
- `production-v1.1.60.patch`: complete patch, including the two added files.
- `source-file-identities.json`: reconstructed vs live source SHA-256s.
- `production-settings.json`: filtered public runtime knobs, not a secrets dump.
- `verification.json`: clean-tag patch-application and source identity checks.

The 15 changed/added files were verified against the corresponding live source files.
After that verification, a trailing space was removed from `src/ds4/ds4_main.cpp`;
that whitespace-only cleanup is the one intentional source difference. The archived patch was applied to the verified upstream tag and reverse
application was checked. The live production binary SHA-256 is
`a0cc17c94e17e398f7053947f3f062214d1e2fb410196f1cb8331add8a24429a`.
The source paper trail is strong but the conflicted live Git index and external
build dependencies mean this is not proof of a bit-for-bit reproduction of every
object in that executable. A clean rebuild and numerical qualification are
needed before claiming binary equivalence or upstream readiness for any change.
A clean fork build and selected tests have since passed; see `BUILD-VALIDATION.md`.
The original captured patch preserves the live-source whitespace for provenance. The audit generator reads local production paths and is
included as evidence, not as a portable setup script.

The checkout has full history and tracks the official repository as `upstream`.
A typical isolated configuration is:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CUDA_ARCHITECTURES=70
cmake --build build --target maya-ds4
```

Use a CMake version with CUDA C++20 support (CMake 3.31.6 was used locally) and CUDA 12.8 for the tested SM70 build. Resolve upstream dependencies and inspect the resulting build options before use.
No model weights, private environment, service unit, expert profile or production
binary are included. Stock source licensing is retained. Review the changes in
small independently qualified commits before sending them upstream.
