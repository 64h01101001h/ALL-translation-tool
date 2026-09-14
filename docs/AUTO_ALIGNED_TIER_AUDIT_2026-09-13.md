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

A second cut of the same tier, added 2026-09-14 because the first understates
it. Run `englishIsNotEnglish`'s function-word half over the auto-aligned
glosses themselves: **328 of 3,912 (8.4%) contain no English function word at
all**. Some are pronunciation lines (`kun tu rgyal` → "tserab kuntu gyalwa
tsongkape"); most are mid-sentence English with the function words outside the
span (`sku mdun` → "small table appeared before"; `rkyen dang phrad` →
"encountering some other influences").

Note these three counts measure three different things and should not be
compared: **5** glosses carry unmistakable Tibetan phonetic markers, **165**
dangle on a function word, and **328** contain none. The middle two overlap;
the largest is the best single indicator of how much of this tier is a slice of
a sentence rather than an equivalent.

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
line of the corpus. **1,066 of the 42,199 segments carrying an English field
(2.53%) hold something that was never a translation**, and the auto-aligner
learned from them.

*(This read 702 until 2026-09-14. That figure came from a probe that counted
one-letter words — "a", "I" — as English function words; the shipped C++ does
not, requiring two letters. The code was always flagging 1,066, so the number
in this document described something the code does not do. Corrected, and the
lesson is the session's own: a measurement and the implementation it describes
have to be the same measurement.)*

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

**How much each test is actually earning.** Measured, because "each catches
what the other cannot" is the kind of claim that sounds true and is rarely
checked:

| | |
|---|---|
| no English function word | 1,065 |
| sounds like its own Tibetan | 234 |
| both | 233 |
| **union — what `englishIsNotEnglish` flags** | **1,066** |

The pronunciation test adds **one** segment the cruder test does not already
catch. It is kept anyway, and the reasons are worth stating rather than
assuming: it is the only *principled* half — it compares the English against
the segment's own computed sound rather than against a hand-kept word list —
and it is the half that would still work if a chant line happened to contain
"the". But the honest summary is that the function-word test is doing almost
all of the work.

`isDrillable` already refused mantras whose *Tibetan* will not convert
(2026-09-10, Adam, on a Kali Wang text). This is the mirror case: the Tibetan
converts perfectly and only the English side is wrong, which that test cannot
see.

### What it was costing

Every surface showing one of these labels it "Geshe Michael Roach's English for
this segment". **64 drills in the shipped phone pack did exactly that**, and 23
of the reading-order cards did it under a badge reading ATTESTED.

*(That 64 was counted with the same looser tokenizer as the old 702 — it treated
"a" and "I" as function words, where the shipped rule requires two letters. The
true figure under the rule that now ships is therefore **at least** 64. It
cannot be recovered exactly, because the pack it describes has been rebuilt;
the current pack is at 0 of 7,700 under the shipped rule, which is the number
that matters.)*

`allcore::englishIsNotEnglish` now unions both tests; `isDrillable` and the
reading-order builder both consult it. The pack is at **0 of 7,700**.

### The false-positive rate, measured

The function-word test is crude by design, so its cost is worth a number rather
than an anecdote. Of 42,199 corpus segments it flags **1,065**:

| | |
|---|---|
| carrying an unmistakable mantra syllable (*om*, *hung*, *phat*, *svaha*…) | 649 (61%) |
| chant transliteration, by eye and by the pronunciation test | 407 (38%) |
| shaped like an outline heading | 9 (1%) |

And of those last nine, **two are correctly flagged** — `(4) Api tu khalu
punaḥ` is Sanskrit carried over untranslated, exactly what the test is for. The
genuine false positives are the other **seven**, all short English headings:
"II. Purifying Bad Deeds", "3) Your skin turns radiant.", "26) [Mental state
#8:] Attaining single-pointedness".

**Seven segments in 42,199 — 0.7% of what it flags, 0.017% of the corpus.** The
cost of each is one drill not built. That is the right side to err on, and it
is cheap enough not to be worth a cleverer test: the obvious discriminator, a
leading enumerator, would also exclude `Api tu khalu punaḥ`, which belongs.

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
