# This fork serves as a one-off checkpoint of kernel optimizations and other changes to improve V100 performance with Deepseek V4.1 Flash 1017 that I wanted to share.
## It is EXTREMELY important to contribute or PR to upstream if you want to extend on optimizations or want to push some ideas up to upstream.

For the original information card of the main project, [click here](https://github.com/mw00/project-maya).

## Context-length performance snapshot

Measured on four 150 W Tesla V100S GPUs with TP4, stock DSpark, greedy
generation, and a 128-token output per request. The prompts were synthetic
natural-language contexts with unique prefixes. Rates are tokens per second;
each context/mode has one sample, so treat these as a workload snapshot rather
than a general benchmark. The saved run contains 2K, 8K, 32K, 64K, 128K, 256K,
and 512K contexts. The requested 16K and 1M results were not recorded.

For two simultaneous requests, each request's rate is shown as `A / B`, followed
by the measured aggregate rate in parentheses. Aggregate rates use the elapsed
pair-wide window and are not the sum of A and B.

| Context | 1 request prefill | 1 request decode | 2 requests prefill A / B (aggregate) | 2 requests decode A / B (aggregate) |
|---:|---:|---:|---:|---:|
| 2K | 738.2 | 63.2 | 479.9 / 315.7 (630.1) | 25.9 / 43.9 (49.8) |
| 8K | 1,145.2 | 72.5 | 592.6 / 687.7 (1,181.9) | 51.5 / 30.9 (57.8) |
| 32K | 1,277.9 | 62.3 | 699.5 / 669.2 (1,336.7) | 27.0 / 51.0 (54.0) |
| 64K | 1,275.1 | 70.8 | 602.8 / 616.2 (1,204.5) | 47.7 / 25.0 (49.8) |
| 128K | 1,185.3 | 61.7 | 573.1 / 566.9 (1,133.0) | 24.9 / 50.1 (49.7) |
| 256K | 1,027.6 | 61.6 | 492.5 / 494.4 (982.9) | 47.3 / 21.7 (43.3) |
| 512K | 818.2 | 62.4 | 396.1 / 397.0 (791.5) | 38.4 / 18.5 (35.1) |

The two-request test used a separate two-slot engine; its expert placement
differed from the one-slot setup. Prefill aggregation spans the first request
start through the later first output. Decode aggregation spans the first output
through the later completion, so overlap and queue time affect both aggregates.

## Undo log: faster edits when it has the needed history

During generation, Maya keeps rolling attention-cache data. The undo log saves
the cache entries that are about to be overwritten. If you revise the end of a
prompt in the same live session, Maya can restore those saved entries and
continue from the edit, instead of reading the unchanged prefix again. The
current setting keeps up to 16,384 tokens of this history in pinned host memory.
It only helps when the requested rewind is covered by entries recorded in that
live session. A conversation restored from a disk slot may not have matching
undo history; then Maya falls back to its normal restore/recompute path.

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

These are edit/reuse workloads; they do not mean ordinary first-time prompt
prefill is 2.6–37× faster. When the log misses, it provides no reuse and a
fallback can cost more than a clean hit-path comparison. For example, one
long-edit run with three misses out of five was 25% slower on prompt time than
its no-log control. Results depend on how much of the edited context is covered
by the current session's log.
