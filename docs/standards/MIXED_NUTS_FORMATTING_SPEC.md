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
| 6 | Bum shad → line break | `;` → `^p` + `,` | **429** | **[verified]** |
| 7 | Folio marker opens | `@` → `[f. ` | ~400 | **[verified]** |
| 8 | Recto close + strip ornament | `A *,^p,` → `a] ` | **192** | **[verified]** |
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

### 3.1b The bum shad is a line break, not a comma

Verified at 13:18: find `;`, replace **`^p,`** — a hard return followed by
a shad — **MATCHES: Result 1 of 429**.

The narration says "replace all those with a comma", and an earlier draft
of this spec believed it. The pane says otherwise: the semicolon is
consumed and becomes a *line break plus a leading shad*, which is the
same shape as the verse rule. The surrounding lines on screen open with
`,RAB`, `,BSLA`, `,PHYA`, `,THU`, `,BSD` — each line beginning with its
shad, exactly as §3.1 describes for verse.

This is the clearest case in the whole recording where the spoken word
and the screen disagree. **Where they disagree, the screen wins.**

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
| C4 | `AAA;BBB` | `AAA` + line break + `,BBB` | stage 6 |
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

**D2 — FIXED 2026-08-25.** *(an unterminated bracket silently swallowed the rest of the file.)*
The scan for `]` runs to end-of-input and the remainder becomes one note,
with no error and no marker. A truncated file therefore produces
plausible-looking output whose tail has quietly become apparatus.

*Fix:* if no `]` is found, do not consume — emit the `[` literally and
record a flag on the result. This is inviolable rule 3: failed parses are
surfaced, never approximated.

**D3 — FIXED 2026-08-25.** *(nested brackets corrupted both text and note.)*
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

**S1 — FIXED 2026-08-25.** *(`, ,` was treated as a paragraph break; it is a verse line.)*
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

**S3 — FIXED 2026-08-25.** *(folio references kept their case and zero padding.)*
We emit `[f. 001A]`; the standard is `[f. 1a]`. Stages 8–10 exist
specifically to lowercase the recto letter and strip `00`.

**S4 — FIXED 2026-08-25.** *(the bum shad rule was not implemented at all.)* `;` should become
a line break plus a shad (`^p,`), 429 times on the demonstrated text.
The code does not touch `;`, so every bum shad stays inline.

**S5 — "brackets become footnotes" is unsourced, and its number came
from somewhere else entirely.** **[verified 2026-08-25]** At 22:58 the
find box holds `[f` and the pane reads **MATCHES: Result 4 of 383**.
**383 is the count of FOLIO REFERENCES**, not of bracketed corrections.
The older note took that figure from this operation and attached it to a
footnote step that does not occur anywhere in the recording. The older note
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


---

## 9. Status against the standard, 2026-08-25

| | |
|---|---|
| S1 paragraph vs verse | **fixed**, mutation-verified |
| S2 the note's swapped counts | **corrected in place**, note carries its banner |
| S3 folio form `[f. 1a]` | **fixed** |
| S4 bum shad `;` → `^p,` | **fixed** |
| S5 brackets → footnotes | **open — needs Adam's ruling** |

The formatter now reproduces the demonstrated output on the real opening
of *Destroying the Darkness*:

```
[f. 1a] *,
,TSAD MA SDE BDUN GYI RGYAN YID KYI MUN SEL ZHES BYA BA BZHUGS SO,,

[f. 1b] #,
,RGYA GAR SKAD DU, ... PHYAG 'TSAL LO,,

BDUD RTZI'I 'OD CAN COD PAN GYIS MDZES RAL PA'I KHUR 'CHANG BA BZHIN,
,GANG SKU RIN CHEN RI BO'I DPAL,
,'DAR BAR SEMS
```

**Two deliberate departures, both flagged rather than hidden:**

1. **Verso folios are normalised too.** He does the `B` side by hand,
   but only because Word made it awkward — "in this simplified system
   I've done". The stipulated *form* is lowercase and unpadded, so we
   apply it to both sides. If the intent was that verso stays raw, this
   is one line to reverse.

2. ~~**The recto ornamentation is retained.**~~ **Implemented
   2026-08-25.** Find string read at 21:05: `A *,^p,` → `a] `, 192
   matches. Measured here: 186 of 187 recto markers carry `*, ` and no
   verso marker does, so the rule is asterisk-only and recto-only, as
   his pattern has it. A `#` in the same position is left alone — he
   never touches one.

**Still open — S5, and it needs a decision, not a patch.** The code turns
`[...]` into numbered notes. That behaviour is not in this recording, and
the 383 that appeared to support it turns out to be the folio count. It
may well be right and sourced from elsewhere. Until somebody says where
from, it should not be described as GMR's demonstrated standard.


---

## 10. Validation against a real text, 2026-08-25

The recording gives ground-truth counts for one document, so the
formatter was run over the closest copy we hold:
`library/sungbum/.../S05501M_TSAD MA SDE BDUN GYI RGYAN YID KYI MUN
SEL_...MKHAS GRUB RJE...TXT` — 559 KB, 571,055 bytes.

**It is a different edition.** 120,884 words against his 144,160, and
its opening folios are `@001A @001A` where his were `@001A @001B`. Exact
count-matching is therefore not available, and no attempt was made to
force one.

| | ours | his | |
|---|---|---|---|
| verse breaks (`, ,` after the O pass) | **677** | **679** | 2 apart |
| paragraph breaks | 1,536 | ~1,737 | edition differs |
| folio references | 371 | 383 | edition differs |
| orphan `,,` left in running text | **0** | — | |
| stray `;` left unconverted | **0** | — | |

The verse figure is the meaningful one: it is the count the O-rule
*predicts* once paragraphs are removed, and it lands two away from his
on a text that differs by 19% in length. That is corroboration the rule
is right, not a coincidence of tuning.

### What the real text taught that the recording could not

**Tight pairs exist.** 2,168 pairs are spaced `, ,`; **9 are tight
`,,`** — and every tight one follows a final O (`BZHUGS SO,,`, `PHYIR
RO,,`, `ZIN TO,,`). His Find & Replace required the space literally, so
on this text his method would silently drop 9 real paragraph ends. Our
scanner skips zero-or-more whitespace and catches them. **This is a
deliberate departure from his literal method in service of his stated
rule.**

**Four-comma runs are section breaks.** 26 runs of `,, ,,`, a doubled
nyis shad. Splitting one into two pairs strands a `,,` alone on a line —
the exact shape step 3 exists to remove. Both doubled shads are now
kept and the break goes between them.

Three of the 29 four-runs do **not** follow an O (after `MTSAR`,
`ZIN LA`, `NUS SAM`) — and each is followed by `DA NI`, "now, next",
opening a new section. So a doubled nyis shad breaks **on its own
account** and does not consult the O; the O-test distinguishes paragraph
from verse only for a plain pair. Mutation-verified.

**Runs of 3, or 5 and more, are left alone.** Two instances in 559 KB.
The standard does not address them and no break is invented — rule 3.


## 11. Corpus sweep, 2026-08-25

The formatter was run over **1,198 real ACIP files, 149,847,998 bytes**
of the Kangyur, Tengyur and Sungbum holdings.

| | |
|---|---|
| notes extracted | 3,760 |
| files flagged (malformed brackets) | **3** |
| orphan `,,` in running text | **0** |
| stray `;` left unconverted | **0** |
| crashes / hangs | 0 |

The 3 flagged files are why D2 and D3 were worth fixing rather than
documenting: malformed brackets are a live condition in the real
corpus, not a theoretical one. Example flag:

```
unterminated bracket at byte 15329 (folio 6b)
  - left as written, not converted to a note
```

`TranslationPrep::flags` is the channel. A malformed bracket is now
reported with its byte offset and folio, the text is left exactly as
written, and no note is fabricated from it. Previously an unterminated
bracket ran to end-of-input and turned the remainder of the document
into a single note, silently.


## 12. Where it stands

Everything demonstrated in the recording is now implemented and pinned,
except the parts that are house style (justification, Word page numbers,
Palatino) or explicitly the editor's judgement (the English title,
splitting over-long paragraphs).

Output on the real opening:

```
[f. 1a] TSAD MA SDE BDUN GYI RGYAN YID KYI MUN SEL ZHES BYA BA BZHUGS SO,,

[f. 1b] #,
,RGYA GAR SKAD DU, ... PHYAG 'TSAL LO,,

BDUD RTZI'I 'OD CAN COD PAN GYIS MDZES RAL PA'I KHUR 'CHANG BA BZHIN,
,GANG SKU RIN CHEN RI BO'I DPAL,
,'DAR BAR SEMS
```

Corpus sweep after every change: **1,198 files, 156,045,513 bytes**,
3,879 notes, 3 files flagged, 0 orphan `,,`, 0 stray `;`, no crashes.

**The one open item is S5** and it is not a coding question. The
bracket-to-notes behaviour is not in this recording, and the 383 that
appeared to support it is the folio count. It needs a source, or a
decision to keep it as a labelled house extension.


## 13. S5 resolved as far as evidence can take it — 2026-08-25

Three measurements, and they point one way.

**1. The demonstrated text has no brackets at all.** *Destroying the
Darkness* (our edition, 559 KB) contains **zero** `[...]`. He never
demonstrates bracket handling because there was nothing to demonstrate.
That is the whole explanation for the silence in the recording, and it
means the absence is *not* evidence that he opposes the behaviour.

**2. The 383 was never a bracket count.** Read off the pane at 22:58
with `[f` in the find box: 383 is the number of **folio references**.
The older note took it from that operation.

**3. What brackets actually contain, measured over 400 real files:**

| content | count | meaning |
|---|---|---|
| `[?]` | 972 | input operator: reading uncertain |
| `[DD]` `[DD1]` `[DD2]` | 39 | damaged / doubtful block |
| `[!]` | 14 | operator's note |
| `[D]` `[N]` | 7 | ditto |
| longer strings | ~60 | actual remarks |

So brackets are **input-operator apparatus**, which is a real thing a
translator wants flagged. But **~88% of them are a bare `?`**, and
turning each into a numbered endnote produces a thousand notes reading
`folio 12a: ?` — an apparatus that says nothing, a thousand times.

### Recommendation

Keep the marks, drop the endnote. A `[?]` means "the operator was
unsure *here*" and that meaning is positional — moving it to the back
of the document is what destroys it. Leave short operator marks inline
where they sit, and reserve the numbered-note treatment for brackets
carrying actual prose (~5% of them).

This is a recommendation, not a change. It alters what a translator
sees, and that is Adam's call. Recorded as backlog #50.

### One defect found while measuring

Running the prep over an **already-prepped** file turns its `[f. 1a]`
folio references into notes — the formatter cannot tell its own output
from raw input. Only 1 file in 400 of the library is pre-formatted, but
a translator re-running the action on a saved file would silently lose
every folio reference into the notes list. Also #50.
