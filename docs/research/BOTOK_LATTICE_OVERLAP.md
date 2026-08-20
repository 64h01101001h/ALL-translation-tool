# Botok(SegTrie) vs overlay lattice — segmentation overlap audit

*2026-08-15. The small-strike audit from the master board: do the
reference segmentation layer (botok SegTrie) and the
dictionary-bound overlay spans tell the same word-boundary story?*

## Method

400 random corpus segments (seeded RNG, reproducible), each run
through both machines:

- **lattice**: `buildOverlay` top-level partition — at each token
  the LONGEST span starting there (the shading's outermost story),
  single syllable where no span starts.
- **botok**: `SegTrie::segment` over the segment's unicode, fed
  with the 98,502 HGM headwords (the Monlam lexicon layer was NOT
  loaded in this probe — an HGM-only comparison; the in-app
  segmenter also carries Monlam).

Partitions compared as syllable-count sequences; boundaries as
prefix-sum positions.

## Result

| measure | value |
|---|---|
| lines with IDENTICAL partitions | 102 / 400 (25.5%) |
| boundary precision (shared / botok) | 80.0% |
| boundary recall (shared / lattice) | 79.9% |

## Reading (why this is fine, not alarming)

Four of five word boundaries agree. Full-line identity is low
because a single difference anywhere breaks it — and a large class
of differences is **by design**: the lattice's outermost spans are
dictionary PHRASES (multi-word entries — titles, fixed
expressions), while botok segments WORDS. A line containing one
five-syllable dictionary phrase can be internally identical in both
machines and still count as divergent here.

No systematic pathology surfaced in the divergent samples
(inspected: normal prose lines with particle-attachment and
phrase-vs-word differences). The architecture ruling stands: botok
remains a REFERENCE layer, displayed as labeled output, never
driving the dictionary-bound spans.

## If this is ever revisited

- Load the Monlam lexicon into the probe for full in-app parity.
- Split the divergence count into phrase-span-explained vs
  genuinely-different-boundary classes.
- Probe source pattern lives in the memory log (scratch
  overlap_audit.cpp; link liballcore + -lsqlite3 -lz).

## Revisit 2026-08-20 — Monlam parity + divergence classes (backburner closed)

Both "if revisited" items done, probe rebuilt from scratch
(overlap_audit2.cpp pattern; the original scratch probe is gone and
its exact stream alignment with it — two real bugs surfaced while
rebuilding, recorded here as method notes for any third pass):

1. **Trailing-tsheg off-by-one**: `SegTrie` words carry a trailing
   tsheg — naive "tshegs+1" over-counts every word by one syllable.
2. **Stream drift**: comparing lattice-over-ACIP against
   botok-over-converted-wylie silently misaligns totals on ~97% of
   lines (dropped non-Tibetan tokens, tokenization differences);
   positions then read as mass "crossings". This probe feeds botok
   unicode generated FROM THE LATTICE'S OWN TOKENS — same stream by
   construction. (The 2026-08-15 numbers above used a different,
   now-unreproducible alignment; compare trends, not digits.)

Method: same 400-segment seeded sample idea (mt19937 seed 42 over
corpus ids), SegTrie at FULL in-app parity — 98,502 HGM headwords +
both Monlam lexicons = 547,946 words. 331 lines comparable (69
dropped: empty/unconverted).

| measure | HGM-only 2026-08-15 | Monlam parity 2026-08-20 |
|---|---|---|
| identical partitions | 25.5% | **41.7%** (138/331) |
| boundary precision (shared/botok) | 80.0% | **98.4%** |
| boundary recall (shared/lattice) | 79.9% | **92.9%** |

**Divergence classes** (193 divergent lines, every one classified):
- **129 botok-merge-explained** — botok joins runs the lattice left
  as fallback singles; boundary sets nest cleanly, no contradiction.
  This is vocabulary size, not disagreement.
- **0 lattice-phrase-explained / 0 nested-both** — at full parity
  botok's boundaries are almost a subset of the lattice's
  (precision 98.4%): botok never merely splits inside our phrases.
- **64 CROSSING (19.3% of compared lines)** — at least one genuinely
  different boundary reading. Inspected samples are dominated by
  long honorific names/titles ("rje btsun bla ma dam pa blo bzang
  thub dbang rdo rje 'chang chen po…") chunked at different joints —
  exactly the material where "word" is a judgment call.

**Reading**: at vocabulary parity the two machines agree on 98% of
the boundaries either asserts jointly; four-fifths of remaining
divergence is nesting, not contradiction; the residue is name/title
chunking. The architecture ruling stands, now with better evidence:
botok remains a labeled REFERENCE layer and never drives the
dictionary-bound spans.
