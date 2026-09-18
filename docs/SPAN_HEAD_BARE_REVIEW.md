# Bare function-word spans — seven that want a reader

*Found 2026-09-17 by auditing `test_no_supplied_span_head.py` against the
question "can it fail?". Its regex required a space AFTER the function word,
so a d=5 span whose English was EXACTLY `the` — `>the</span>` — never
matched. That is the purest form of the defect the gate exists to catch, and
**73 such spans were invisible.*

Reading them is what made the fix usable rather than noisy. Most are correct:
a pronoun rendering a pronoun (`bdag gis` → "I", `khyod` → "you"), the
disjunctive `'am` → "or", a conjunction rendering "and", `gcig` → "a". Those
are the word's own meaning, not English the translator supplied, and the gate
now licenses them **by class, checked against the span's own Tibetan column**
rather than assumed from the English.

These seven are not that. Each is a d=5 span — the layer the dictionary
harvests from — whose entire English is a function word that the Tibetan does
not obviously mean.

| page | span | Tibetan | English | why it is doubted |
|---|---|---|---|---|
| c1p03 | s46w18 | `lam` | the | `lam` is "path"; 38 landed links gloss it path/way, none "the" |
| c1p05 | s51w7 | `lugs` | the | `lugs` is "system, tradition, position" — 31 links, never "the" |
| c1p21 | s99w12 | `tshe` | the | `tshe` is "life" or the temporal "when" |
| c1p49 | s184w35 | `ston pa` | his | `ston pa` is "the Teacher" — a noun, not a possessive |
| c3p112 | s357y23 | `gzhan` | your | `gzhan` is "other, another"; "your" needs a second person |
| c1p38 | s151w47 | `yid` | you | `yid` is "mind" |
| c1p45 | s170w7 | `'ong rgyu` | a | "cause of coming" rendered as an article |

**Nothing has been changed.** All seven are in shipped Course 1 and Course 3
pages, which the campaign RATCHETS rather than re-cuts — the count is frozen
so they cannot multiply, and re-cutting a shipped page is a decision, not a
tidy-up. They are recorded here so the next person to open those pages has
the list, and so that "the gate is green" is not mistaken for "every span was
read".

Also counted but NOT doubted, listed so nobody re-derives them: `dang` and
`dang bcas pa` and `zhing` rendering "and" (5), `gcig`/`cig` rendering "a"
(3), and three genitives rendering "his" (`rang gi`, `dam pa'i`, `de'i`)
where a possessive reading is ordinary.
