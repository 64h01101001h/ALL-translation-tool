# Integrating the manual HGM dictionary with the scan layer

*Adam's ruling, 2026-08-28: keep the original HGM dictionary intact and
integrate the new scan-derived dictionary alongside it; the scan data
will most likely become what the application uses once all eighteen
courses are rescanned. This document records what the two actually
contain, measured rather than assumed.*

## What each one is

**The manual dictionary** (`entries.hgm_gloss`, 11,966 glossed
headwords): humans reading Geshe Michael's translations and linking
Tibetan to English by hand. Tiered `curated` > `glossary` >
`auto-aligned`.

**The scan layer** (`alignment_evidence_v1.json`, 4,039 headwords):
machine-matched spans from the corpus, every one letter-exact against
the spine, ACIP-keyed, each carrying its attesting segments.

## Measured agreement (2026-08-28)

| | count | share |
|---|---|---|
| headwords in both | 2,178 | — |
| exact gloss match | 903 | 41.5% |
| share a content word | 634 | 29.1% |
| **no overlap — review queue** | **641** | **29.4%** |
| headwords only in the scan | 1,861 | new coverage |

So the two corroborate each other, fully or partly, on **70.6%** of
shared headwords, and the scan already adds 1,861 headwords the manual
set does not have — from three courses out of eighteen.

## The honest reading of the 641 disagreements

Adam's note says he trusts the machine linking more than the manual
work. **The measurement supports that only partly, and it should be
said plainly.** The disagreements are a mix, in at least three kinds:

**1. The manual entry is clearly wrong.** `zhus pa` glossed "tilopa"
(tier: auto-aligned) against the scan's "asked / requested"; `snga
phyir` glossed "out any apparent inconsistencies" (auto-aligned)
against "a number of times". These are broken links, and they cluster
in the weakest manual tier.

**2. The scan's render is weaker AS A DICTIONARY EQUIVALENT.**
`'phel` — the manual has "increasing", which is what the word means.
The scan has "may it help" and "the higher good", which is what Geshe
Michael *wrote at those two places*, rendering the whole dedication
formula freely. My span is correct as an attestation and wrong as a
definition.

**3. Genuine register difference, both right.** `spro ba` "joy /
pledge" against "the act of emanating"; `zin` "already / believed"
against "fill / filled with"; `sdod pa` "to stay" against "live".

## The structural conclusion

**The scan layer is an ATTESTATION layer, not a DEFINITION layer.** It
records what Geshe Michael actually wrote at a specific place, with the
citation. The manual dictionary records what a word *means*. Those are
different kinds of claim and the integration must not flatten one into
the other — case 2 above is exactly what flattening would cost.

This also answers risk R10 far better than anything else available.
R10 says no machine gate can tell whether a pairing is *correct*. The
manual dictionary is a partial human oracle for precisely that: where
the two agree, a human already judged the link and the machine
reproduced it. That converts "review all 9,000 segments" into "review
641 disagreements", and the queue shrinks per course rather than grows.

## Recommended integration

1. **Both layers stay separate and both stay keyed on ACIP.** Neither
   overwrites the other. `hgm_gloss` remains binding until Adam rules
   otherwise; the scan layer stays PROVISIONAL until reviewed.
2. **Corroboration is recorded as a field, not a merge.** A scan pair
   whose gloss matches a manual entry is marked corroborated, and that
   is the strongest evidence tier the layer can carry without a human.
3. **The 641 disagreements become the review queue**, partitioned by
   manual tier — disagreements against `auto-aligned` entries are
   likely manual defects; disagreements against `curated` entries are
   likely scan defects and should be read first.
4. **Attestation and definition stay distinguishable in the app**, so a
   contextual render like `'phel` → "may it help" can never be
   presented as the word's equivalent.

## Status

Measurement is reproducible from `alignment_evidence_v1.json` and the
spine's `entries` table. Nothing has been merged; this is the design
record for the integration Adam ordered, pending his review of the
recommendation above.
