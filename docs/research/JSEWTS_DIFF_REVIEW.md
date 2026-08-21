# jsewts as a third diff-oracle — findings (2026-08-14)

**Setup.** buda-base **jsewts** (Apache-2.0, npm) run as an
independent third implementation of EWTS→Unicode beside our
canonical `ewts_unicode.py` and the master's ground truth: all
**26,318 attested (wylie, tibetan) pairs** from
hgm_dictionary_v27_2 (`tibetan_source` not `generated-*` — the
count matches the historical battery exactly, confirming the
extraction). Comparison normalized only for trailing tsheks.

## Results

| comparison | agree | rate |
|---|---|---|
| canonical python vs ground truth | 25,872 | 98.31% |
| jsewts vs ground truth | 25,853 | 98.23% |
| jsewts vs canonical python | 26,014 | **98.84%** |

Two independent implementations agreeing at 98.84% — and each
agreeing with the human ground truth at ~98.3% — is strong
mutual corroboration. (The historical 98.88% battery figure was
variant-aware; this run is exact-match-only, hence the slightly
lower canonical rate.)

## jsewts defects found (our engine is right)

1. **Sanskrit long vowels pass through as Latin.** `kā ya` →
   `ཀā་ཡ` (Latin ā embedded in Tibetan!) where canonical and
   ground truth agree on `ཀཱ་ཡ` (a-chung length mark). Same for
   ī, ū, and standalone `ṇ` (`karṇa pa` → `ཀརṇཨ་པ`). This is
   the bulk of the disagreement class — jsewts does not accept
   precomposed diacritic input, only ASCII EWTS (`kA ya`).
   Our engine accepts both. **No action needed on our side;
   worth a courteous upstream issue someday.**
2. **Ellipsis handling**: `kho na re ... zer na` — jsewts emits
   a literal `་...་`; canonical drops it (ground truth uses a
   shad). Ours is closer; neither is exactly the ground truth's
   `ཁོ་ན་རེ། ཟེར་ན` (shad restoration from `...` is a judgment
   call — flagged, not guessed, per rule 3).

## Standing use

`scratchpad` harness (extract_pairs.py + jsewts_sweep.js) is
reproducible from this doc. When the canonical engine changes,
rerun the three-way as a cheap independent cross-check beside
the ground-truth battery. Remaining candidate oracles from the
TODO (NetWylie, Lingua::BO) add little beyond jsewts and are
deprioritized; pyewts was already mined separately (13 defects
filed).
