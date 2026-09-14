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
