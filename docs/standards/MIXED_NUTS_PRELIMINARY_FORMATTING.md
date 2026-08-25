# Mixed Nuts — Preliminary Formatting of a Raw ACIP Text
*(gleaned 2026-08-06 from Geshe Michael's screen-recorded demonstration,
"Preliminary formatting of a Mixed Nuts text.mp4" (28½ min, Sept 2020),
working on "DESTROYING DARKNESS working gmr 9 26 20" — the Pramāṇa
commentary "TSAD MA SDE BDUN GYI RGYAN YID KYI MUN SEL", 144k words. Frames
archived from the session; the workflow below is what the recording shows.)*

> **CORRECTION, 2026-08-25.** Step 3 below is **wrong**, and the C++ was
> built from it. A paragraph ends at `O, ,` (1,737 on screen), and the
> *remaining* `, ,` (679) are **lines of quoted verse**, which get a line
> break — not a paragraph break. This note reports those two counts
> swapped and says both are "handled the same way"; they are not. Step 5
> (brackets → footnotes) is **not in the recording at all** — the word
> "footnote" is never spoken and 383 is never said aloud.
> The corrected, evidence-tagged version is
> [MIXED_NUTS_FORMATTING_SPEC.md](MIXED_NUTS_FORMATTING_SPEC.md); read
> that instead. This page is kept as written because the implementation
> still follows it and because a note that quietly changes is worse than
> one that carries its correction.

## The goal
Turn a raw ACIP input-center file (page markers, hard line breaks, double
shads, input-operator brackets) into a clean working document a translator
can translate under, passage by passage.

## The observed steps (Word workflow)
1. **Title the document** in English at the top ("Destroying the Darkness").
2. **Flow the text**: Find & Replace `^p^p` (paragraph pairs from the raw
   input) — remove the input line breaks so the text runs continuously
   (his file: 2,668 replacements; 365 pp → 253 pp).
3. **Re-paragraph at double shads**: every `,,` (nyis shad — the end of a
   statement/section) becomes `,,` + paragraph break, so each passage is
   its own paragraph (679 replacements; sentence-final patterns like `O, ,`
   handled the same way, 1,737 instances). The translation will later go
   under each paragraph.
4. **Verse lineation**: root-text verses are set one line per verse line,
   each line beginning with its `,` shad (done with judgment — metered
   lines only; see the verse-meter tool for detection help).
5. **Brackets → footnotes**: input-operator brackets `[…]` (corrections,
   queries, damaged-block readings) are first marked `[f.` via Find &
   Replace (383 in his file), folio references like `2B`/`4a` attached, and
   then moved into real footnotes (Insert → Footnote), leaving clean
   running text with `[f. 4a]`-style folio references inline.
6. Result: clean paragraphed ACIP with folio refs inline, apparatus in
   footnotes — ready for the translation pass.

## In the app
`Overlay → "Prepare for translation (Mixed Nuts)…"` automates steps 2, 3,
and 5 deterministically (`allcore::formatForTranslation`): flows input
lines, breaks paragraphs at `,,`, converts `@NNN` page markers to inline
`[f. NNN]` references, and turns every bracket into a numbered, folio-tagged
note appended to the saved file. Verse lineation (step 4) and house style
stay with the human editor.

## Pending
The **Mixed Nuts style sheets / style guide** (from the team's editor) will
be uploaded and banked here; the formatter and the Draft workspace should
then be extended to enforce/assist its rules.
