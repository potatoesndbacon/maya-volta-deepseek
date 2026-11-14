# This fork serves as a one-off checkpoint of kernel optimizations and other changes to improve V100 performance with Deepseek V4.1 Flash 1017 that I wanted to share, I do not intend to maintain this.
## It is EXTREMELY important to contribute or PR to upstream if you want to extend on optimizations or want to push some ideas up to upstream.

For the original information card of the main project, [click here](https://github.com/mw00/project-maya). Credits and thank you to [mw00](https://github.com/mw00) for making a good base for me and Deepseek to work on for this.

This checkpoint targets V100 performance while preserving quality. For that reason, it does not use attention-head
pruning or expert-tail skipping (`STRATA_DS4_RESIDENT_TOPK=0`,
`STRATA_DS4_RESIDENT_SKIP_W=0`). Adaptive resident-expert routing is enabled
and can change which experts are selected which may slightly change numerics, though the answers will always arrive to same conclusion.

## Context-length performance snapshot

Measured on a Proxmox VM with a Epyc 7452, 110gb of memory on the VM, four 150 W Tesla V100S GPUs with P2P TP4 passed through, stock DSpark, greedy
generation, and a 128-token output per request. The prompts were synthetic
natural-language contexts with unique prefixes. Rates are tokens per second;
each context/mode has one sample, so treat these as a workload snapshot rather
than a general benchmark. The recorded run covers 2K through 1M contexts at
2K, 8K, 16K, 32K, 64K, 128K, 256K, 512K, and 1,048,440 tokens.

For the recorded two-card run, the CPU expert lane used about 88 GB of pinned
host memory. A load completed once in 420 seconds; repeat loads were sensitive
to memory pressure and model page cache. This is a measured workload constraint,
not a universal minimum-RAM specification.

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

An undo log complements the slot prefix cache for tail edits and repeated prompt
variants. During generation, Maya updates its rolling attention caches; the undo
log saves each affected entry before it is overwritten. If you revise the end of
a prompt in the same live session, Maya can restore those saved entries and
continue from the edit instead of reading the unchanged prefix again. The
captured setting keeps up to 16,384 tokens of history in pinned host memory
(about 2 GB for one slot, 8 GB for four). If the log misses, Maya falls back to
normal prefill recomputation.

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

It’s most useful for repeated iterative editing, prompt variants, evaluation suites, events where a loop guard hits, and other workloads that keep a long prefix and change only
the ending. It may miss after restoring a conversation from a disk slot, since the undo history is in memory.  

Before a write overwrites a ring or compressed-cache slot, the log records that
slot's old value. A covered rewind restores those values to return the logged
cache state to its earlier position. The log tracks which token positions are
covered and falls back to normal prefill when a rewind misses. This establishes
the restore behavior for covered writes; it does not guarantee bit-identical
full-model output against a cold recomputation.

Unset `DS4_UNDO_LOG` to disable.

## Expert-cache warm-up and GPU-count measurements

The expert swap cache learns which experts are being routed and moves frequently
used experts into VRAM. The ramp is very aggressive at the patched settings, though does not thrash the cache.

| GPUs and layout | Prose decode | Code decode | Prefill at 20K / 80K |
|---|---:|---:|---:|
| 4, TP4 | 66.1 tok/s | 98.1 tok/s | 1,282 / 1,294 tok/s |
| 3, PP3xTP1 | 44.5 tok/s | 67.7 tok/s | 720 / 719 tok/s |
| 2, TP2 | 39.0 tok/s | 46.7 tok/s | Not measured |

For this model, the three-GPU result uses pipeline parallelism; TP3 does not
load successfully. The two- and three-GPU throughput figures are warmed runs;
they do not measure the cold-start ramp at those GPU counts.

For patch-based installation guidance, see [AGENTS.md](AGENTS.md).
