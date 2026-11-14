# Commit plan for maya-volta-deepseek

The original v1.1.60 source modifications are preserved uncommitted because the Git author identity was deferred. The original upstream tag is `v1.1.60`; the branch is `maya-volta-deepseek-1.1.60`. Suggested commits, with dependent pieces kept together:

1. **Carry V100 DeepSeek engine and undo-log changes:** `CMakeLists.txt`, `src/ds4/ds4_gguf.cu`, `src/ds4/ds4_iq.cu`, `src/ds4/ds4_iq_cpu.cpp`, `src/ds4/ds4_kernels.cu`, `src/ds4/ds4_kernels.cuh`, `src/ds4/ds4_main.cpp`, `src/ds4/ds4_model.cu`, `src/ds4/ds4_model.hpp`, `src/ds4/ds4_undo_log.hpp`, `src/kernels/cpu/iq_avx2.cpp`, and `tests/ds4_undo_log_test.cpp`. These files contain interdependent host, kernel and undo code; splitting them more finely requires an individual build and test of each resulting commit.
2. **Carry serving and monitoring changes:** `serve/loop_guard.py`, `serve/server.py`, and `serve/web/monitor.js`.
3. **Document the production source carry:** `fork-package/`, after reviewing its machine-specific paths and the generated patch for publication.

Before pushing, configure the desired author and add the fork repository as `origin`; `upstream` points at the official project. Run `git diff --check`, review `git diff` and the untracked package, and inspect the staged content before each commit. The carried historical patch is evidence and includes the original whitespace; the working `ds4_main.cpp` has one trailing-space-only cleanup.
