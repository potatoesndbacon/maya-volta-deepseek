# maya-volta-deepseek

Source changes carried on the official Maya v1.1.60 base for DeepSeek inference
on NVIDIA Volta (SM70). The branch is `maya-volta-deepseek-1.1.60`. The full
source patch is [`production-v1.1.60.patch`](production-v1.1.60.patch), and the
source-level descriptions are in [`DIFFERENCES.md`](DIFFERENCES.md).

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
