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

## 2. The six steps, and who does them

From GMR's screen recording (Sept 2020, *Destroying the Darkness*, 144k
words). Steps 2, 3 and 5 are automated; the rest are the editor's.

| # | Step | Automated? |
|---|---|---|
| 1 | Title the document in English | **No** — editor |
| 2 | Flow the text: remove input line breaks | **Yes** |
| 3 | Re-paragraph at every double shad `,,` | **Yes** |
| 4 | Verse lineation, one line per metered line | **No** — editor, needs judgement |
| 5 | Brackets → numbered notes, folio-tagged | **Yes** |
| 6 | Result: clean paragraphed ACIP, apparatus in notes | — |

Step 4 stays human deliberately: deciding what is verse is an editorial
reading, and guessing it would violate the project's never-guess rule.

## 3. The algorithm (normative)

A single left-to-right pass over the input. No lookbehind, no
backtracking, one character of lookahead (two for one case).

**State:** `out` (output text), `notes` (ordered list of strings),
`paragraphs` (integer, starts at 0), `folio` (string, starts empty).

**Helper — `trimTrailingSpaces()`:** remove spaces from the end of `out`.
Called before most emissions. It exists so that shads and note markers
attach tightly to the preceding syllable rather than floating.

Process each character `c` at position `i`:

### 3.1 `@` — page marker
Consume `@` then all following **alphanumeric** characters. That run is
the new `folio` (e.g. `@001A` → folio `001A`). Then:
- `trimTrailingSpaces()`
- if `out` is non-empty and does not end in newline, append one space
- append `"[f. " + folio + "] "`

The folio persists until the next `@`, and tags every note raised after it.

### 3.2 `[` — input-operator bracket
Scan forward to the **first** `]`. The text between is `content`.
- append to `notes`: `"folio " + folio + ": " + content`, or just
  `content` if no `@` has been seen yet
- `trimTrailingSpaces()`
- append `"[" + notes.length + "]"` (1-based, no surrounding spaces)
- resume **after** the `]`

### 3.3 `,` — shad
Look ahead, **skipping any run of whitespace — spaces, tabs, newlines,
carriage returns.** If the next non-whitespace character is `,`, this is
a **double shad**; consume through that second comma.

The whitespace skip is not a nicety. ACIP input is typed to a fixed
width, so the two halves of a nyis shad are regularly split by the hard
wrap. Deciding on the immediately-next character alone misses every one
of those (defect D1, fixed 2026-08-25).

**Double shad:** `trimTrailingSpaces()`, append `",,\n\n"`, increment
`paragraphs`.

**Single shad:** `trimTrailingSpaces()`, append `", "`, consume 1.

### 3.4 Newline (`\n` or `\r`) — flowed away
This is GMR's `^p^p` removal. `trimTrailingSpaces()`; if `out` is
non-empty and does not end in a newline, append one space; then consume
**all** consecutive newline characters.

### 3.5 Space or tab — collapsed
Append a single space only if `out` is non-empty and does not already end
in a space or a newline. Consume 1.

### 3.6 Anything else
Append the character verbatim. ACIP is passed through untouched — this
formatter never transliterates, never corrects, never normalises casing.

### 3.7 Finalisation
`trimTrailingSpaces()`. Then, if `out` contains any non-space,
non-newline character **and** does not already end with `",,\n\n"`,
increment `paragraphs` once more — the trailing passage has no closing
double shad but is still a paragraph.

## 4. Output shape

```
TranslationPrep {
  text:       string    // formatted ACIP with [n] note markers
  notes:      string[]  // note n is notes[n-1]
  paragraphs: int
}
```

The caller appends the notes to the saved file; the formatter does not
format them. Note markers are 1-based to match how they read on the page.

## 5. Worked example (verified output)

Input (`\n` shown literally):

```
@001A *, ,TSAD MA\nSDE BDUN, GYI RGYAN,\n,BZHUGS SO, ,\n@001B RGYA GAR [DD] SKAD DU,, DE NAS
```

Output:

```
[f. 001A] *,,

TSAD MA SDE BDUN, GYI RGYAN,,

BZHUGS SO,,

[f. 001B] RGYA GAR[1] SKAD DU,,

DE NAS
```

`paragraphs = 5`, `notes = ["folio 001B: DD"]`.

The split between `GYI RGYAN,,` and `BZHUGS SO,,` is the D1 fix working
on real text: in the source those two shads sit either side of a line
break, and they mark exactly where the title ends and *bzhugs so*
("herein contained") begins. Before the fix this ran on as one
paragraph carrying a `,,` in its middle.

## 6. Conformance tests

Port these first; they are the reference implementation's actual output.
`\n` is written literally.

| # | Input | Expected `text` | `paragraphs` |
|---|---|---|---|
| T1 | `AAA,,BBB` | `AAA,,\n\nBBB` | 2 |
| T2 | `AAA, ,BBB` | `AAA,,\n\nBBB` | 2 |
| T3 | `AAA,BBB` | `AAA, BBB` | 1 |
| T4 | `SO, ,\nNEXT` | `SO,,\n\nNEXT` | 2 |
| T5 | `[QUERY] AAA` | `[1] AAA` | 1 |
| T6 | `AAA,\n,BBB` | `AAA,,\n\nBBB` | 2 |
| T7 | `AAA,\n, BBB` | `AAA,,\n\nBBB` | 2 |

T5 also asserts `notes == ["QUERY"]` — no folio prefix, because no `@`
had been seen.

**Tests that pin defects rather than intent** — port them, but see §7:

| # | Input | Current output | Should be |
|---|---|---|---|
| D2 | `AAA [OOPS never closed` | `AAA[1]`, note `OOPS never closed` | flagged, not silently consumed |
| D3 | `AAA [outer [inner] tail] BBB` | `AAA[1] tail] BBB`, note `outer [inner` | flagged or nested-aware |

## 7. Known defects in the reference implementation

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
