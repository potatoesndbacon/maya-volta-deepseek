# Complete changed source hunk index

Generated from the complete patch, including the two previously untracked files. Hunk locations refer to the official tag and reconstructed source; the patch contains every exact line change.

## `CMakeLists.txt`

- `@@ -260,6 +260,10 @@ if(STRATA_BUILD_TESTS)`

## `serve/loop_guard.py`

- `@@ -301,6 +301,8 @@ TOOL_WINDOW = 12        # the last this many tool calls since the user's last me`
- `@@ -376,6 +378,27 @@ def _tool_calls(messages: list[dict]) -> list[tuple]:`
- `@@ -397,13 +420,25 @@ def tool_repeat(messages: list[dict]) -> ToolRepeat | None:`
- `@@ -414,8 +449,17 @@ def tool_blocked(messages: list[dict]) -> dict[tuple[str, str], ToolRepeat]:`

## `serve/server.py`

- `@@ -3327,7 +3327,18 @@ class Service:`
- `@@ -6206,6 +6217,9 @@ def make_handler(svc: Service):`
- `@@ -6522,6 +6536,68 @@ def make_handler(svc: Service):`

## `serve/web/monitor.js`

- `@@ -207,6 +207,31 @@ function ContextCard({m}) {`
- `@@ -616,6 +641,7 @@ export function MonitorPage() {`

## `src/ds4/ds4_gguf.cu`

- `@@ -912,6 +912,40 @@ __global__ void __launch_bounds__(kThreads) mx_down_kernel(const MxArgs a) {`
- `@@ -930,10 +964,10 @@ struct CombArgs {`
- `@@ -957,14 +991,44 @@ __global__ void __launch_bounds__(128) mx_combine_kernel(const CombArgs a) {`
- `@@ -1957,6 +2021,11 @@ void moe_down(const x3::MoeArgs& a, const float* h, const float* sh_out, float*`
- `@@ -1965,7 +2034,7 @@ void moe_down(const x3::MoeArgs& a, const float* h, const float* sh_out, float*`
- `@@ -2015,6 +2084,10 @@ bool win_experts(const x3::WinArgs& a, cudaStream_t s) {`
- `@@ -2023,6 +2096,11 @@ bool win_experts(const x3::WinArgs& a, cudaStream_t s) {`
- `@@ -2031,7 +2109,7 @@ bool win_experts(const x3::WinArgs& a, cudaStream_t s) {`

## `src/ds4/ds4_iq.cu`

- `@@ -1210,6 +1210,31 @@ __global__ void __launch_bounds__(kThreads) mv_kernel(const Jobs J) {`
- `@@ -1217,7 +1242,10 @@ __global__ void __launch_bounds__(kThreads) mv_kernel(const Jobs J) {`

## `src/ds4/ds4_iq_cpu.cpp`

- `@@ -1102,12 +1102,17 @@ void iq_cpu_gate_up(const IqCpu& f, const uint8_t* gate, const uint8_t* up, cons`

## `src/ds4/ds4_kernels.cu`

- `@@ -278,7 +278,7 @@ __device__ __forceinline__ void finish_vec(float* v, int len, const Finish& f, i`
- `@@ -340,8 +340,20 @@ template <bool kPack>`
- `@@ -957,18 +969,38 @@ __global__ void copy_regions_kernel(const CopyRegions r) {`
- `@@ -1825,7 +1857,7 @@ constexpr int kStage = 16;   // rows a pass stages (a thread: 32 values of one)`
- `@@ -1860,7 +1892,8 @@ __global__ void __launch_bounds__(kThreads) unpack_sel_kernel(const uint8_t* __r`
- `@@ -2446,8 +2479,9 @@ __device__ __forceinline__ void ring_row_to_ext(const uint8_t* __restrict__ ring`
- `@@ -2466,14 +2500,18 @@ __global__ void ring_to_ext_runs_kernel(const RingRuns rr, uint16_t* __restrict_`
- `@@ -2486,12 +2524,14 @@ __device__ __forceinline__ void ext_row_to_ring(const uint16_t* __restrict__ src`
- `@@ -2500,18 +2540,21 @@ __global__ void __launch_bounds__(kThreads) ext_to_ring_runs_kernel(const RingRu`
- `@@ -2520,7 +2563,8 @@ __global__ void ring_restore_batch_kernel(const RingBatch b, long long p0, int n`
- `@@ -2861,7 +2905,7 @@ __global__ void __launch_bounds__(512) route_publish_kernel(const float* __restr`
- `@@ -2876,15 +2920,20 @@ __global__ void __launch_bounds__(512) route_publish_kernel(const float* __restr`
- `@@ -2997,10 +3046,13 @@ __global__ void __launch_bounds__(512) route_pair_kernel(const RoutePair a) {`
- `@@ -3313,6 +3365,12 @@ __device__ __forceinline__ float4 sat4(float4 v) {   // (FP16's range: a partial`
- `@@ -3330,11 +3388,25 @@ __global__ void __launch_bounds__(kThreads) ar_rs16_kernel(float* x, int n4, flo`
- `@@ -3788,17 +3860,31 @@ bool pdl_on() {`
- `@@ -3812,8 +3898,14 @@ void route_publish(const float* logits, const float* bias, int n, int k, int T,`
- `@@ -3826,7 +3918,11 @@ bool route_pair(const RoutePair& a, cudaStream_t s) {`
- `@@ -4068,9 +4164,18 @@ bool copy_regions(const CopyRegions& r, cudaStream_t s) {`
- `@@ -4591,13 +4696,20 @@ void sparse_attn(const float* q, const uint16_t* win, const void* ckv, const int`
- `@@ -4651,8 +4763,8 @@ bool sparse_attn_rows(const float* q, const uint16_t* win, const AttnCkv& c, con`
- `@@ -4666,24 +4778,104 @@ void ext_to_ring_runs(const RingRuns& r, const uint16_t* ext, int n, int d, cuda`
- `@@ -4784,7 +4976,58 @@ void set_flag(unsigned* flag, unsigned v, cudaStream_t s) {`
- `@@ -4801,7 +5044,16 @@ void allreduce(float* x, int n, const ArGroup& a, int mi, unsigned seq, unsigned`

## `src/ds4/ds4_kernels.cuh`

- `@@ -49,6 +49,18 @@ void embed(const uint16_t* rows_bf16, int T, float* h, int dim, int hc, cudaStre`
- `@@ -174,12 +186,16 @@ struct CopyRegions {`
- `@@ -248,6 +264,7 @@ struct AttnRows {`
- `@@ -270,9 +287,10 @@ bool sparse_attn_rows(const float* q, const uint16_t* win, const AttnCkv& c, con`
- `@@ -281,6 +299,8 @@ void ext_to_ring(const uint16_t* ext, uint8_t* ring, int64_t p0, int n, int T, i`
- `@@ -302,7 +322,7 @@ struct IotaRows {`
- `@@ -310,7 +330,7 @@ struct RingBatch {`
- `@@ -323,7 +343,42 @@ struct CompBatch {`
- `@@ -416,6 +471,8 @@ struct RoutePair {`

## `src/ds4/ds4_main.cpp`

- `@@ -36,6 +36,33 @@`
- `@@ -1155,6 +1182,24 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -1422,6 +1467,12 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -1501,18 +1552,32 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -2062,6 +2127,7 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -2469,6 +2535,10 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -2483,6 +2553,12 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -2494,6 +2570,10 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`
- `@@ -2520,6 +2600,12 @@ int serve(strata::ds4::Model& m, const std::string& eos_list, bool spec, float c`

## `src/ds4/ds4_model.cu`

- `@@ -10,6 +10,7 @@`
- `@@ -625,6 +626,10 @@ struct Dev {`
- `@@ -680,6 +685,9 @@ struct Dev {`
- `@@ -954,6 +962,12 @@ struct Layer {`
- `@@ -1400,6 +1414,7 @@ struct Model::Impl {`
- `@@ -1521,6 +1536,10 @@ struct Model::Impl {`
- `@@ -1545,12 +1564,15 @@ struct Model::Impl {`
- `@@ -1770,6 +1792,17 @@ struct Model::Impl {`
- `@@ -3749,6 +3782,10 @@ struct Model::Impl {`
- `@@ -4345,8 +4382,10 @@ struct Model::Impl {`
- `@@ -6847,6 +6886,18 @@ struct Model::Impl {`
- `@@ -9626,6 +9677,9 @@ struct Model::Impl {`
- `@@ -9636,6 +9690,9 @@ struct Model::Impl {`
- `@@ -9643,6 +9700,17 @@ struct Model::Impl {`
- `@@ -9684,6 +9752,7 @@ struct Model::Impl {`
- `@@ -9696,6 +9765,8 @@ struct Model::Impl {`
- `@@ -9716,6 +9787,9 @@ struct Model::Impl {`
- `@@ -9729,7 +9803,7 @@ struct Model::Impl {`
- `@@ -9741,6 +9815,7 @@ struct Model::Impl {`
- `@@ -9794,6 +9869,35 @@ struct Model::Impl {`
- `@@ -9831,6 +9935,10 @@ struct Model::Impl {`
- `@@ -9865,7 +9973,167 @@ struct Model::Impl {`
- `@@ -9886,6 +10154,9 @@ struct Model::Impl {`
- `@@ -10270,6 +10541,7 @@ struct Model::Impl {`
- `@@ -15245,7 +15517,7 @@ struct Model::Impl {`
- `@@ -17467,6 +17739,7 @@ struct Model::Impl {`
- `@@ -17481,6 +17754,7 @@ struct Model::Impl {`
- `@@ -17540,6 +17814,25 @@ struct Model::Impl {`
- `@@ -17878,7 +18171,7 @@ struct Model::Impl {`
- `@@ -17908,7 +18201,8 @@ struct Model::Impl {`
- `@@ -21085,6 +21379,11 @@ struct Model::Impl {`
- `@@ -21102,7 +21401,26 @@ struct Model::Impl {`
- `@@ -21135,7 +21453,20 @@ struct Model::Impl {`
- `@@ -21147,13 +21478,17 @@ struct Model::Impl {`
- `@@ -21216,6 +21551,13 @@ struct Model::Impl {`
- `@@ -21265,7 +21607,11 @@ struct Model::Impl {`
- `@@ -21277,8 +21623,34 @@ struct Model::Impl {`
- `@@ -21287,17 +21659,25 @@ struct Model::Impl {`
- `@@ -21313,7 +21693,12 @@ struct Model::Impl {`
- `@@ -21367,31 +21752,44 @@ struct Model::Impl {`
- `@@ -21418,7 +21816,7 @@ struct Model::Impl {`
- `@@ -21429,7 +21827,9 @@ struct Model::Impl {`
- `@@ -21446,6 +21846,13 @@ struct Model::Impl {`
- `@@ -21479,10 +21886,13 @@ struct Model::Impl {`
- `@@ -21925,7 +22335,14 @@ struct Model::Impl {`
- `@@ -22047,7 +22464,7 @@ struct Model::Impl {`
- `@@ -22404,6 +22821,7 @@ struct Model::Impl {`
- `@@ -22423,9 +22841,38 @@ struct Model::Impl {`

## `src/ds4/ds4_model.hpp`

- `@@ -148,6 +148,11 @@ struct Stats {`

## `src/ds4/ds4_undo_log.hpp`

- `@@ -0,0 +1,64 @@`

## `src/kernels/cpu/iq_avx2.cpp`

- `@@ -475,6 +475,9 @@ inline void row_dot_iq1m(const uint8_t* row, int nblocks, const block_q8_K* cons`

## `tests/ds4_undo_log_test.cpp`

- `@@ -0,0 +1,171 @@`
