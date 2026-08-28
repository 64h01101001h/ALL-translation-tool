# Upstream data defect: the Tibetan column is offset one row in C13

*Found 2026-08-28 by an alignment agent reading C03:299, which cited C13 as a
cross-reference. Verified independently. **This is a report for the HGM data
project, not a change to be made here** — the spine is consumed, never edited,
by this repo.*

## The defect

Across **C13:63–82** the `wylie` column sits **one row below** the `english`
it belongs to. Every segment in that run pairs a Tibetan text with the English
of a *different* text.

| seq | wylie | english on the same row | the English it belongs to |
|---|---|---|---|
| 63 | `dga' ldan lha brgya ma` | "3) …Lady of Liberation" | 62: "2) The Thousand Angels of the Heaven of Bliss **(Ganden Hlagyama)**" |
| 70 | `shes rab snying po` | "10) …Face of a Lion" | 69: "9) Three repetitions of the **Essence of Wisdom ('Heart Sutra')**" |
| 71 | `seng gdong ma` | "11) The Tree of Sandalwood…" | 70: "10) …the Angel with the **Face of a Lion**" |
| 72 | `tsan dan ljon shing` | "12) The Servant of Gentle Voice" | 71: "11) **The Tree of Sandalwood**…" |
| 81 | `'jam dpal mtshan brjod` | "3) …Woman of Total Victory" | 80: "2) **Chanting the Names** of Gentle Voice" |
| 82 | `rnam rgyal ma'i gzungs ring` | "4) …Essence of Wisdom" | 81: "3) …the **Woman of Total Victory**" |

## How it was verified

Geshe Michael prints the Tibetan name parenthetically inside much of this
English — "(Ganden Hlagyama)", "('Heart Sutra')", "Face of a Lion". Those
parentheticals are independent anchors: they say which Tibetan text each
English line is about, without reference to the wylie column.

Testing eight such anchors across all of C13: **7 land one row above their
wylie, 3 land on the same row.** The three aligned cases fall outside the
63–82 run. Repeating the test across every course in the spine found no other
systematic run — TITLS scores 43 aligned against 3 stray, which is noise, not
an offset.

## Why it matters more here than a typo

This is not a spelling defect that a reader would catch. **Every segment in
the run reads as perfectly coherent Tibetan next to perfectly coherent
English**, and only someone who knows both texts would notice they are not the
same text.

For this project specifically: **any alignment run over C13:63–82 would align
unrelated texts.** The generator's verbatim gates would all pass — the wylie is
genuine wylie and the English is genuine English, both byte-exact against the
spine. Nothing in the alignment machinery could detect it. It has to be fixed
in the data.

C13 has not been scanned by the campaign yet. **It should not be, until this
is resolved.**

## Recommended action

1. Re-derive C13:62–83 from the source files. The offset most likely entered
   when one column gained or lost a row during ingest — check whether a row
   was dropped from the English side at ~62 or inserted into the wylie side.
2. **Add an ingest check.** Where the English contains a parenthetical Tibetan
   name, it should match the wylie on the same row. That check is cheap, it is
   the one used to find this, and it would have caught the defect at build time.
3. Re-run that check across the whole corpus after any future re-ingest. This
   run was found only because an agent happened to cross-reference C13 while
   working on C03; nothing was looking for it.
