# This fork serves as a one-off checkpoint of kernel optimizations and other changes to improve V100 performance with Deepseek V4.1 Flash 1017 that I wanted to share, I do not intend to maintain this.
## It is EXTREMELY important to contribute or PR to upstream if you want to extend on optimizations or want to push some ideas up to upstream.

For the original information card of the main project, [click here](https://github.com/mw00/project-maya).

This checkpoint is made to get the most performance possible out of Maya, Volta and Deepseek without hurting quality or changing numerics much on outputs.
For that reason, top-k head pruning and expert tail skipping is not used, though can be a toggled option for even more speed, it hurts quality notably though.

## Context-length performance snapshot

Measured on a Proxmox VM with a Epyc 7452, 110gb of memory on the VM, four 150 W Tesla V100S GPUs with P2P TP4 passed through, stock DSpark, greedy
generation, and a 128-token output per request. The prompts were synthetic
natural-language contexts with unique prefixes. Rates are tokens per second;
each context/mode has one sample, so treat these as a workload snapshot rather
than a general benchmark. The saved run contains 2K, 8K, 32K, 64K, 128K, 256K,
and 512K contexts. The requested 16K and 1M results were not recorded.

Note that for 2 32gb cards, having more than 128gb of memory is mandatory for this quant.

For two simultaneous requests, each request's rate is shown as `A / B`, followed
by the measured aggregate rate in parentheses. Aggregate rates use the elapsed
pair-wide window and are not the sum of A and B.

## Recorded numbers on Unsloth DeepSeek-V4.1-Flash-1017-GGUF UD-IQ2_XXS, Weights 165.3gb, Engrams Q5_K / 135.2gb 
| Prompt tokens | Concurrency 1 prefill | C1 decode | Concurrency 2 prefill A / B | C2 aggregate prefill | C2 decode A / B | C2 aggregate decode |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| 2,048 | 725.6 | 111.3 | 506.4 / 333.8 | 666.6 | 31.3 / 64.2 | 62.6 |
| 8,192 | 1,123.9 | 88.7 | 586.3 / 679.9 | 1,169.3 | 75.8 / 31.0 | 62.1 |
| 16,384 | 1,211.9 | 92.7 | 702.2 / 647.7 | 1,293.1 | 33.2 / 69.1 | 66.5 |
| 32,768 | 1,255.2 | 112.9 | 607.9 / 633.0 | 1,213.7 | 63.9 / 31.5 | 60.7 |
| 65,536 | 1,225.2 | 107.1 | 614.7 / 601.8 | 1,202.0 | 28.7 / 69.8 | 57.4 |
| 131,072 | 1,142.9 | 89.8 | 565.0 / 571.0 | 1,129.1 | 67.9 / 29.2 | 57.7 |
| 262,144 | 1,004.8 | 77.0 | 496.6 / 493.7 | 986.6 | 23.8 / 60.1 | 47.6 |
| 524,288 | 801.8 | 73.8 | 379.2 / 378.4 | 755.8 | 19.7 / 52.3 | 39.3 |
| 1,048,440 | 542.9 | 68.3 | 257.2 / 257.9 | 514.1 | 63.1 / 67.4 | 38.5 |

The two-request test used expert placement that differed from the one-slot setup. 
Prefill aggregation spans the first request start through the later first output. 
Decode aggregation spans the first output through the later completion, so overlap and queue time affect both aggregates.

## Added an Undo Log: faster prefill for tail edits and repeated edits in context

An Undo Log complements the slot prefix cache, helping further prefill savings. 
During generation, the undo log with Maya keeps rolling attention-cache data. The undo log saves
the cache entries that are about to be overwritten. If you revise the end of a
prompt in the same live session, Maya with an undo log can restore those saved entries and
continue from the edit, instead of reading the unchanged prefix again. The
current setting rewinds up to 16,384 tokens from the tail in pinned host memory (about 2gb for 1 slot, 8gb for 4 slots).
If the Undo Log misses after the slot prefix cache, Maya falls back to its normal prefill recompute path.

In the Maya web interface, on the monitor page it will tell you how many prefill tokens are replayed by the undo log.

Measured hit cases on the V100 production setup:

- A 2.3K-token tail edit with production slots enabled reduced time to first
  token from 12.46 s to 2.56 s (4.9× faster).
- A 9.8K-token tail edit with slots disabled took 7.4 s with a log hit versus
  19.5 s for a full reread (2.6× faster).
- Across 10 tails on a roughly 20K-token shared prefix, streaming requests took
  0.36–0.49 s per later tail versus 13.37 s for the first full read (27–37×);
  all nine later tails hit the log.
- In a production six-tail sweep over a roughly 7.4K-token prefix, later tails
  took 0.34–0.47 s versus 5.80 s for the first read (12–17×), with six hits
  and no misses.

It’s most useful for repeated iterative editing, prompt variants, evaluation suites, and other workloads that keep a long prefix and change only
the ending. It may miss after restoring a conversation from a disk slot, since the undo history is in memory.  

In order to ensure no cache discrepancy, the log records the old value of each cache slot immediately before the slot is overwritten. When rewinding, Maya restores those saved
values in reverse logical order, so the ring and compressed-cache state match the earlier position. It also tracks which token positions
are covered; if the requested rewind is outside that recorded range, it reports a miss and uses the normal fallback.

Unset `DS4_UNDO_LOG` to disable.

## Expert-cache warm-up and GPU-count measurements

The expert swap cache learns which experts are being routed and moves frequently
used experts into VRAM. In this checkpoint, the router ramps FAST and AGGRESSIVELY as much as it can without cache trashing,
with the tuned promotion settings, `DS4_CACHE_MARGIN=1.0` and `DS4_CACHE_BONUS=0.25`.

| GPUs and layout | Prose decode | Code decode | Prefill at 20K / 80K |
|---|---:|---:|---:|
| 4, TP4 | 66.1 tok/s | 98.1 tok/s | 1,282 / 1,294 tok/s |
| 3, PP3xTP1 | 44.5 tok/s | 67.7 tok/s | 720 / 719 tok/s |
| 2, TP2 | 39.0 tok/s | 46.7 tok/s | Not measured |

For this model, the three-GPU result uses pipeline parallelism; TP3 does not
load successfully. 

Agents for install, read [AGENTS.md](https://github.com/potatoesndbacon/maya-volta-deepseek/blob/maya-volta-deepseek-1.1.60/AGENTS.md).
