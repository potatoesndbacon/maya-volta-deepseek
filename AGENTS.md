# Installation guidance

Use this guide when preparing an installation from this fork or reproducing
its recorded runtime configuration.

## Source patch

The fork is based on the official Maya v1.1.60 commit
`8b0a449c599fbb370a1dcc0979ae7c84d4310529`. Its complete local source change is
[`fork-package/production-v1.1.60.patch`](fork-package/production-v1.1.60.patch).

- If working in this fork checkout, the patch is already applied. Do not apply
  it a second time.
- To reproduce the fork from an upstream checkout, start from the exact commit
  above and apply the patch. Check it first:

  ```sh
  git apply --check /path/to/fork-package/production-v1.1.60.patch
  git apply /path/to/fork-package/production-v1.1.60.patch
  ```

- Do not apply this patch to another Maya release. Resolve upstream dependency
  versions as that release documents, then review the resulting build options.

## Build reference

The validated build used CMake 3.31.6, CUDA 12.8.93, GCC 12.2, Release mode,
CUDA architecture 70, native experts, CUDA support, and project tests enabled:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CUDA_ARCHITECTURES=70 \
  -DSTRATA_NATIVE_EXPERTS=ON \
  -DSTRATA_ENABLE_CUDA=ON \
  -DSTRATA_BUILD_TESTS=ON
cmake --build build --target maya-ds4
```

See [`fork-package/BUILD-VALIDATION.md`](fork-package/BUILD-VALIDATION.md) for
what was built and checked. The build reused an existing llama.cpp dependency
checkout, so a clean dependency fetch has not been qualified by this package.

## Runtime settings reference

Use [`fork-package/production-settings.json`](fork-package/production-settings.json)
as the reference when reproducing the recorded settings. Its `settings` object
is a filtered snapshot of process environment variables; it is not a complete
service configuration or a ready-to-install service unit. Translate its
key/value pairs into the target service's environment and review
[`fork-package/FEATURE-FLAGS.md`](fork-package/FEATURE-FLAGS.md) for defaults,
aliases, and which settings are local additions versus upstream controls.

The captured setup was four Tesla V100S GPUs at 150 W each, TP4, one serving
slot, and 1,048,576-token context. The model pack, weights, Engram files, service
unit, and host-specific paths are not included. Do not treat its settings as
validated for a different GPU count, model pack, memory budget, or operating
system. In particular, the active resident-expert routing penalty can change
which experts are selected and therefore can change outputs. The full fork has
not been qualified for bit-for-bit output equivalence.
