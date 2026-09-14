# The auto-aligned tier, and the picker that preferred it

**Found by Adam, on the page**, 13 September 2026. Step 5 of the Walkthrough
showed him:

| span | tier | gloss |
|---|---|---|
| `rgyal ba'i yon tan` | auto-aligned | farthest shore |
| `ma lus` | auto-aligned | individual analysis and which |
| `rab rdzogs` | auto-aligned | sadang lamgyi yunten rabdzok |

The third is not English. It is a pronunciation line — the sound of
`sa dang lam gyi yon tan rab rdzogs` — paired with Tibetan by the aligner and
banked as though it were a meaning.

There are two faults here and they are worth keeping apart.

## 1. The picker, which is ours

`walk::bestGlossSpan` ranked candidate spans by LENGTH alone. So a four-token
auto-aligned span beat the curated two-token span inside it, and
`rgyal ba` — curated, "the Victors" — was passed over for "farthest shore".

Measured over the whole corpus:

| | |
|---|---|
| glossed picks | 877,004 |
| …that land on an auto-aligned entry | 97,913 (11.2%) |
| …**passing over a GMR-tier gloss underneath** | **63,956 (7.3%)** |

That is rule 1 inverted. `hgm_gloss` is binding; the curated and glossary tiers
are his English; auto-aligned is the machine's match, provisional by
construction. Preferring the machine's longer guess to his own shorter word is
the one trade this project may not make.

**Fixed.** The picker now takes the longest span among GMR-tier glosses, and
falls back to the longest provisional one only when nothing better covers the
token — which is still 33,957 picks, still labelled PROVISIONAL, because there
a provisional gloss is the best there is. Splitting a term into two of his
words beats keeping it whole with a guess: `ma lus` now shows as `MA` + `LUS`
from the glossary rather than whole with a wrong gloss.

Gated in the Walkthrough selftest against this very sentence, and the gate is
mutation-proved: restoring length-only ranking fails all three checks, with the
run completing (`app selftest: FAIL (3 failure(s))`) rather than crashing.

## 2. The tier itself, which is the data project's

The spine carries 3,910 auto-aligned entries against 7,980 glossary and 114
curated. A read of them:

| | |
|---|---|
| auto-aligned glosses | 3,912 |
| that begin or end mid-sentence — a slice, not an equivalent | 165 (4.2%) |
| whose "English" is actually a pronunciation line | 5 |

The 4.2% is a **floor**: it only catches glosses left hanging on a function
word (`kun spyod` → "how one should"; `khyab pa khas` → "doesn't necessarily
follow but"). It cannot catch the ones that are fluent and simply wrong —
`chibs` → "crimson", `chag med` → "angel in her sindhura", `gsung ba` →
"people who".

Plenty of the tier is sound: "distinction between", "flies", "consistent",
"fine wisdom", "entire body". It is not junk. But it is not dependable either,
and the honest description is that it is what its label says — provisional.

**For the dictionary project, not this repo.** Two suggestions worth passing on:

- the aligner is taking mid-sentence spans of English as equivalents; a check
  that refuses a candidate opening or closing on a dangling function word
  would remove 165 of them mechanically, today
- five entries have a pronunciation line in the `hgm_gloss` field, which means
  a pronunciation column was read as an English one somewhere upstream; those
  are worth tracing, because one such crossing tends to mean a batch

Neither is detectable from inside this app, which is why they are written down
here rather than gated.

---

## 3. Where it came from: the corpus's English column

Chasing the fix above turned up the source. `rab rdzogs` is glossed "sadang
lamgyi yunten rabdzok" in the spine, and that string is not invented — it is a
line of the corpus. **702 of the 42,013 segments carrying an English field
(1.67%) hold something that was never a translation**, and the auto-aligner
learned from them.

Two kinds:

**The chanted prayers.** Course materials print the opening verses in
transliteration so a student can chant along. `[C01:2] "sashi pukyi jukshing
metok tram,"` is the *sound* of its own Tibetan, sitting in the English column.
Detectable precisely: it is ≥60% inside the pronunciation the engine computes
from that same segment's ACIP. 234 segments.

**The mantras.** `[TCS12:365] "Om argham praticha sva ha."` — Sanskrit carried
over in transliteration, untranslated on purpose. No pronunciation test finds
these, because their sound is not the Tibetan's; what finds them is that four
or more words go by without one English function word.

`isDrillable` already refused mantras whose *Tibetan* will not convert
(2026-09-10, Adam, on a Kali Wang text). This is the mirror case: the Tibetan
converts perfectly and only the English side is wrong, which that test cannot
see.

### What it was costing

Every surface showing one of these labels it "Geshe Michael Roach's English for
this segment". **64 drills in the shipped phone pack did exactly that**, and 23
of the reading-order cards did it under a badge reading ATTESTED.

`allcore::englishIsNotEnglish` now unions both tests; `isDrillable` and the
reading-order builder both consult it. The pack is at **0 of 7,700**.

The cost of a false positive is one drill not built, and there is a measured
one: an outline heading, "b2. Mistaken deceptive reality", has no function word
either. That is the right side to err on.

### For the data project

These 702 are not defects in the *source* — the transliteration genuinely
belongs in the course booklet next to the Tibetan, and the mantras genuinely
are not translated. The defect is that the ingest put them in the column
labelled English, and then the aligner treated that column as ground truth.

Two suggestions, both upstream of this repo:

- mark these segments at ingest rather than having each consumer re-detect
  them: a `not_a_translation` flag would be exact where every downstream test
  is a heuristic
- and re-run the auto-alignment with them excluded. The tier's worst entries
  are drawn from this population, so a fair part of the 4.2% in §2 should
  simply disappear
