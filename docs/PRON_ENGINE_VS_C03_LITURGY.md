# pron_engine.py vs the C03 liturgical phonetics — an observation, not a defect report

*Opened 2026-08-28 during C03 alignment batch 38 (model: Opus). Nothing
in the engine was changed. Inviolable rule 2 governs: the Python engines
are the authority, they are never reconstructed from spec, and a change
ships only against its own validation battery.*

## What prompted it

Batch 37 found that GMR's C03 phonetics encode the ergative case as a
vowel change (`tsong kha pa`→"tsongkapa" vs `tsong kha pas`→"tsongkape";
`rgyal ba`→"gyalwa" vs `rgyal bas`→"gyalwe"). Probing whether
`pron_engine.pronounce()` reproduces that, it does — correctly, both
pairs. But the probe surfaced other, systematic differences between the
engine's output and the prayer transcriptions in the C03 corpus.

## What this is NOT

**This is not a claim that the engine is wrong.** `pron_engine.py` is
validated at 42/42 on its spot battery and across 2,118 card lines in
`Handoff/data/card_phonetics.json`. Those card phonetics are dictionary
citation forms. The C03 material is a different genre: devotional verse
transcribed for chanting aloud. **The most likely explanation is that the
two use different conventions, and that both are correct for their
purpose.** Adjudicating that is a human call, not a machine one.

## Measurement honesty

Two earlier numbers produced during this probe were discarded as
unsound, and are recorded so nobody resurrects them:

- **"79.44% token agreement"** — computed by zipping engine and corpus
  syllables 1:1. Contaminated: where the two segment words differently
  (`le`/`lejung`, `la`/`lama`) the zip misaligns and reports phantom
  divergences.
- **"51.7% of segments differ"** — computed over segments selected by a
  function-word heuristic. Contaminated far worse: the heuristic
  misclassified English prose and section headings as phonetics
  (C03:138, 169, 218–264, 316, 539), so the engine's rendering of the
  wylie was compared against *English translation*. Those are not
  divergences at all.

The defensible figure, restricted to 79 hand-confirmed phonetic segments
and ignoring spacing and hyphenation, is **47 identical (59.5%), 32
differing**. Even this should be read as "how often two conventions
coincide", not as an error rate.

## The recurring classes, for adjudication

| # | Class | Engine | GMR corpus |
|---|---|---|---|
| 1 | Junction doubling not applied | `dak gi`, `tuka` | `dakki`, `tukka` |
| 2 | Final -b written -p | `rapkar`, `tserap`, `lapchen`, `drub la` | `rabkar`, `tserab`, `labchen`, `drupla` |
| 3 | Umlaut before -l | `gyelwa`, `gyelpo`, `gyelwe` | `gyalwa`, `gyalpo`, `gyalwe` |
| 4 | Epenthetic n before d/dz | `pungndray`, `chundzin`, `jomndze` | `pungdray`, `chunzin`, `jomdze` |
| 5 | Intervocalic b not spoken w | `chibor` | `chiwor` |
| 6 | Syllable joining across words | `kyap drosem kye` | `kyabdro semkye` |
| 7 | Hyphenation absent | `kunkyen`, `nyingkar`, `jinye` | `kun-kyen`, `nying-kar`, `ji-nye` |

Class 7 is orthographic only — the sounds agree. Classes 1 and 6 are
juncture. Classes 2–5 are genuine segmental rules where the two sources
disagree.

Note that the corpus is not internally uniform either: `sangs rgyas` is
"sangye" at C03:4/8/11 but "sanggye" at 18, and `chos kyi` is "chu-kyi"
at 57 and 114 but "chukyi" at 82 (C03 phonetics queue #1 and #2). Some
of the spread is in the source, not between source and engine.

## What is needed from Adam

1. Are the ACI prayer transcriptions and the dictionary card phonetics
   **meant** to follow the same convention? If not, this file closes as
   "two genres, both correct" and the engine is untouched.
2. If they are meant to agree, which is authoritative for classes 2–5?
3. Either way: **no engine change without its battery.** Any adjustment
   must be re-proved against 42/42 spot + 2,118 card lines before it
   ships, per rule 2.

## Reproducing

The probe is not a committed test — deliberately, since it measures a
convention question rather than a contract. Method: run
`pron_engine.pronounce(wylie)` over the hand-confirmed phonetic segment
list in `data/alignment/C03_CAMPAIGN.md` and compare against the spine's
`english` column with spacing and hyphens normalised out.
