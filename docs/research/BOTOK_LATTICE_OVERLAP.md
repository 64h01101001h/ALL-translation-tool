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
