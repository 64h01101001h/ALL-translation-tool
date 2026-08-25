# Mixed Nuts preliminary formatting — implementation specification

*Normative spec for re-implementing the Mixed Nuts preliminary formatter
in another language (Python, TypeScript, or anything else). Written
2026-08-25 from the C++ reference implementation
(`core/src/tibexport.cpp:242`, `allcore::formatForTranslation`) and from
Geshe Michael's demonstrated workflow, recorded in
[MIXED_NUTS_PRELIMINARY_FORMATTING.md](MIXED_NUTS_PRELIMINARY_FORMATTING.md).*

**Every output in this document was produced by running the reference
implementation, not derived by hand.** Section 7 lists three places where
the reference implementation does not do what section 2 says it should;
a new implementation should read section 7 before copying section 3.

---

## 1. What this is for

An ACIP input-centre file is typed for *fidelity to the page*, not for
reading: hard line breaks at a fixed width, page markers, double shads
where a statement ends, and square-bracketed remarks from the input
operator. A translator needs the opposite — running text, one passage per
paragraph, with the operator's remarks moved out of the way but not lost.

This formatter is that conversion. It is deterministic and offline: same
input, same output, no model, no network, no judgement calls.

## 2. Provenance of this spec — read this first

Sections 3 onward were rewritten **2026-08-25** after transcribing the
source recording and reading frames from it. Everything marked
**[verified]** was read off a Find & Replace pane on screen, with its
match count. Everything marked **[inferred]** was not.

This matters because the older note,
[MIXED_NUTS_PRELIMINARY_FORMATTING.md](MIXED_NUTS_PRELIMINARY_FORMATTING.md),
**gets the central rule backwards**, and the shipped C++ implements the
backwards version. See §8.

Source: `Preliminary formatting of a Mixed Nuts text.mp4`, 28.6 min,
26 Sep 2020, Geshe Michael Roach, working on *DESTROYING DARKNESS
working gmr 9 26 20* — 144,160 words, 253 pages at the start.

## 3. The demonstrated pipeline

Order is load-bearing: stage 5 only means what it means because stage 4
already consumed the paragraph cases.

| # | Operation | Find → Replace | Count | Status |
|---|---|---|---|---|
| 1 | Flow out blank lines | `^p^p` → ` ` | 2,668 | **[verified]** |
| 2 | Flow out remaining returns | `^p` → ` ` | 9,143 | **[verified]** |
| 3 | *(house style)* full justification | — | — | [verified] |
| 4 | **Paragraph ends** | `O, ,` → `O,,` + `^p^p` | **1,737** | **[verified]** |
| 5 | **Verse lines** | `, ,` → `,` + `^p` + `,` | **679** | **[verified]** |
| 6 | Bum shad normalisation | `;` → `,` | 429 | [inferred] |
| 7 | Folio marker opens | `@` → `[f. ` | ~400 | **[verified]** |
| 8 | Recto close + strip ornament | `A *,^p,` → `a] ` | ~200 | [inferred] |
| 9 | Remaining recto letter | `A]` → `a]`, case-sensitive | ~200 | [inferred] |
| 10 | Strip leading zeros | `[f. 00` → `[f. ` | 16–18 | [inferred] |
| 11 | Verso (`B`) folios | — | — | **manual, by his own statement** |

### 3.1 The rule that matters — stage 4 vs stage 5

**A paragraph does not end at a double shad. It ends at `O, ,`.**

> "the mixed nuts standard is that after a **final O**, when it completes
> a paragraph, we just have two commas next to each other, we don't have
> a space between the commas"

The `O` is the final letter of the syllable closing the statement — `SO`,
`LO`, `TO`. On screen at 08:30 the search box holds `O, ,` and the pane
reads **MATCHES: Result 1 of 1737**; `BZHUGS SO, ,` and `PHYAG 'TSAL
LO, ,` highlight, while `BZHIN, ,GANG` and `SEMS, ,'PHRIN` do not.

**The remaining `, ,` are verse lines, not paragraphs.**

> "lines of quoted poetry from other texts are divided by a comma space
> comma... we show lines of poetry in separate lines"

At 11:40 the box holds `, ,` and reads **MATCHES: Result 1 of 679**, with
the stage-4 output already visible as tight `,,` separated by blank
lines. Replacement is `,` + hard return + `,` — the shads stay, the line
breaks between them.

He is explicit that this is approximate: *"we won't catch all the lines,
but we'll catch like 90%."* An implementation should say so too rather
than presenting stage 5 as exhaustive.

### 3.2 Case sensitivity is required

Stage 4 searches capital `O` and he warns against confusing it with zero.
Stage 9 needs Word's "ignore case" switched **off** — he demonstrates
getting this wrong first, and the replacement silently doing nothing.
Any implementation must be case-sensitive throughout.

### 3.3 Folio references

`@001A` becomes `[f. 1a]`, not `[f. 001A]`:
- stage 7 opens it (`@` → `[f. `)
- stage 8 or 9 closes it and lowercases the side letter
- stage 10 strips the leading zeros

`A` (recto) becomes `a`. `B` (verso) he leaves manual, so an
implementation that lowercases `B` is going beyond the demonstrated
standard and should say so.

## 4. Algorithm notes for an implementer

The stages above are Word find-and-replace, applied globally in order.
A single-pass scanner can produce the same result but must respect the
ordering: **classify `O, ,` before `, ,`**, or every paragraph end is
mis-read as a verse line.

Minimum state: current folio (for anything that tags by folio), and
whether the previous non-space character was a capital `O`.

Whitespace: because stages 1 and 2 have already flattened every line
break to a space by the time stage 4 runs, `O, ,` and `O,,` and
`O,` + wrap + `,` are all the same thing. A scanner that skips
whitespace runs inside the shad lookahead reproduces this; one that
looks only at the next character does not (defect D1).

## 5. Output shape

```
TranslationPrep {
  text:       string    // formatted ACIP
  notes:      string[]  // see the caveat in §8
  paragraphs: int
}
```

## 6. Conformance tests

Derived from the verified rules, **not** from current behaviour. Several
of these fail against the shipping C++ — that is the point.

| # | Input | Expected | Why |
|---|---|---|---|
| C1 | `BZHUGS SO, ,NEXT` | paragraph break after `SO,,` | stage 4 |
| C2 | `BZHIN, ,GANG` | verse line break, **not** a paragraph | stage 5 |
| C3 | `PHYAG 'TSAL LO, ,` | paragraph break | stage 4, capital O |
| C4 | `AAA;BBB` | `AAA,BBB` | stage 6 |
| C5 | `@001A` | `[f. 1a]` | stages 7–10 |
| C6 | `o, ,` (lowercase) | **not** a paragraph break | §3.2 |

## 7. Defects found in the reference implementation

All three were found by running it on 2026-08-25. **Do not reproduce them
in a new implementation.**

**D1 — FIXED 2026-08-25.** *Kept here because the spec is also the record
of what went wrong; T6 and T7 pin it.*

A double shad split across an input line break used to lose its paragraph
break: `AAA,\n,BBB` yielded `AAA,, BBB` in one paragraph. The scanner
sees `,` followed by `\n`, classifies it as a single shad, flows the
newline away, then meets the second `,` and treats it as another single
shad. The two commas end up adjacent in the output — so the text
*contains* `,,` while the paragraph break that `,,` is supposed to
trigger never happens.

This matters more than it looks. ACIP input files wrap at a fixed width,
so a nyis shad landing across a line boundary is ordinary, not exotic.
Every occurrence is a passage boundary the translator does not get.

*Fix, applied:* the shad lookahead now skips any whitespace run before
deciding. Mutation-verified — reverting the newline case turns T6 and T7
red by name.

**D2 — an unterminated bracket silently swallows the rest of the file.**
The scan for `]` runs to end-of-input and the remainder becomes one note,
with no error and no marker. A truncated file therefore produces
plausible-looking output whose tail has quietly become apparatus.

*Fix:* if no `]` is found, do not consume — emit the `[` literally and
record a flag on the result. This is inviolable rule 3: failed parses are
surfaced, never approximated.

**D3 — nested brackets corrupt both text and note.**
`AAA [outer [inner] tail] BBB` gives note `outer [inner` and leaves a
stray `]` in the running text. Neither the note nor the text is right.

*Fix:* either track depth, or detect a `[` before the closing `]` and
flag. Flagging is preferable until somebody confirms whether nested
brackets legitimately occur in input-centre files — a question for the
input centres, not for a programmer to assume.

## 8. Deliberately out of scope

- **Verse lineation** (step 4) — editorial judgement.
- **The English title** (step 1) — the editor's.
- **House style.** The Mixed Nuts style sheets are still pending from the
  team's editor. When they arrive, they constrain the editor's passes,
  not this formatter.
- **Any transliteration.** ACIP in, ACIP out, byte-for-byte apart from the
  transformations above.

---

## 8. Where the shipped implementation diverges from the standard

Found 2026-08-25 by checking the recording rather than the older note.
None of these are fixed yet; they are listed so nobody ports the current
behaviour believing it is the standard.

**S1 — `, ,` is treated as a paragraph break. It is a verse line.**
The single most consequential divergence. `formatForTranslation` makes a
paragraph at any `,` + whitespace + `,`, with no test for a preceding
capital `O`. On the demonstrated text that is 679 verse-line divisions
converted into paragraph breaks — the opening homage verses become
prose. Stage 4 of the workflow (which the older note called "editorial
judgement, deliberately human") is in fact ~90% automated by one
find-and-replace, and we automate the wrong half of it.

**S2 — the older standards note has the two counts swapped.** It reads
"every `,,` ... becomes a paragraph (679 replacements; sentence-final
patterns like `O, ,` handled the same way, 1,737 instances)." On screen,
**1,737 is the `O, ,` paragraph count** and **679 is the `, ,` verse
count**, and they are not handled the same way at all. The
implementation follows the note, so the note is where the defect entered.

**S3 — folio references keep their case and their leading zeros.**
We emit `[f. 001A]`; the standard is `[f. 1a]`. Stages 8–10 exist
specifically to lowercase the recto letter and strip `00`.

**S4 — `;` → `,` is not implemented at all.** The bum shad normalisation
(stage 6) has no counterpart in the code.

**S5 — "brackets become footnotes" is unsourced.** The older note
describes a step where `[...]` corrections are marked `[f.` and moved
into Word footnotes, citing 383 instances, and the C++ implements a
version of it. **The recording contains no such step.** The word
"footnote" is never spoken, the only use of the Insert menu is for page
numbers, and 383 is never said aloud. The bracket handling may be
correct and come from elsewhere — but it does not come from here, and
the note should not have said it did. Until a source is produced, treat
`notes[]` as a house extension, not as GMR's demonstrated standard.

*Caveat on all of the above:* the transcript is machine-generated and has
known defects; the frame readings are direct but sampled. Where this
document says **[verified]** it means a Find & Replace pane was read on
screen with its count. Where it says [inferred] it means the transcript
described it and no frame was checked.
