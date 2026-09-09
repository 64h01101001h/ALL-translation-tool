# Diamond Cutter Classics Style Guide — every rule, and where the tool stands on it

*Source of truth: `docs/standards/DCC_Style_Guide_2023-08-25.docx` (Venerable
Utpala, Managing Editor, 25 August 2023 — the latest version). Distilled for
readers in `data/help/STYLE.md` (six Help chapters). This page is the
engineering inventory: one row per rule, its kind, and its status in the
tool as of 2026-09-09.*

## Lineage of the document (found 2026-09-09 across mail, Dropbox and disk)

| Date | Document | Author | Where |
|---|---|---|---|
| 8 Feb 2017 | Style Sheet for Classics of Middle Asia 2 8 17.docx | Geshe Michael Roach | Gmail, "style sheet for translations" |
| 11 Feb 2017 | Style Sheet for Classics of Middle Asia 2 11 17.docx | Geshe Michael Roach | Gmail, "updating your draft translations"; Dropbox …/Style Sheet/Old |
| 1 Apr 2017 | Style Sheet for Classics of Middle Asia 4 1 17.docx | Geshe Michael Roach | ~/Documents; Dropbox (three copies) |
| 22 Nov 2021 | Mixed Nuts Style Guide.pdf (draft) | Utpala | Gmail, "The Golden Key: editing" |
| 12 Aug 2022 | Diamond Cutter Classics Style Guide_2022-08-12.pdf | Utpala | Gmail, "A Door to Emptiness: Let the editing begin!"; ~/Documents |
| 4 Sep 2022 | Diamond Cutter Classics Style Guide_2022-09-04.pdf | Utpala | Gmail, same thread |
| 5 Jan 2023 | Diamond Cutter Classics Style Guide_2023-01-05.pdf | Utpala | Dropbox …/For Our Reference/Style Guide (online-only placeholder) |
| **25 Aug 2023** | **Diamond Cutter Classics Style Guide (docx)** | **Utpala** | **`docs/standards/` — received from Adam 2026-08-14** |

Geshe Michael's 2017 sheet carried rules the 2023 guide no longer states
(footnote format, the `{%DGAS}` correction convention, folio-marker style,
bibliography section letters S/B/C/E, the shad-after-final-o rule). They are
listed at the end as *legacy* rules; Utpala should confirm which still hold.

## Kinds

- **mechanical** — a program can apply or detect it exactly.
- **judgement** — a person decides; the tool may show, never decide.
- **workflow** — how the editing team works in Word; not the tool's business.

## Status

- **applied** — Preferences ▸ House Style switch; Format ▸ Apply House Style
  Spacing applies it on demand, on save, or while typing.
- **checked** — the Draft's *House style check* flags it (never rewrites);
  each check is a switch on Preferences ▸ House Style.
- **documented** — in Help ▸ Style; left to the editor.
- **gap** — not yet in the tool; what to build.

## General

| Rule (guide wording, abridged) | Kind | Status |
|---|---|---|
| Edit only in Word, Track Changes on; corrections in the manuscript; queries as Comments (a footnote's query on its marker) | workflow | documented (Help ▸ Style ▸ Manuscript Workflow). The editor's-tool session decides whether queries-as-objects replaces this inside the tool |
| Save as `NAME date_YourName date.docx` | workflow | documented; **gap** — the Manuscript's Save As could propose this name pattern (small) |
| Ignore pagination, header, layout; flag oddities in a Comment | workflow | documented |
| Palatino Linotype 12 pt, fully justified; ṝ ḹ in Times New Roman 12 pt | mechanical | documented; **gap** — a Manuscript house style "DCC body" (Palatino 12 justified) and an export check for the two Times letters |
| Section numbers Palatino 10 pt; removed from English-only manuscripts | mechanical | documented; the Mixed Nuts formatter handles section numbering separately |
| Header Palatino 10 pt bold centred; page numbers 12 pt bottom centred (top outer for print) | mechanical | documented; page furniture is the publisher's |
| Headings consistent within a manuscript; hand-built table of contents checked against headings | judgement / mechanical | documented; **gap** — "headings vs table of contents" consistency report is cheap once headings are styled |
| Chicago Manual of Style; US English, Merriam-Webster | judgement | documented; **gap** — a US-spelling check (colour/honour/-ise) is mechanical and worth adding to the checker |
| Curly quotes, never straight; never Find/Replace All | mechanical | **checked** (straight double quotes flagged; apostrophes as one aggregate) |
| En dash for number ranges | mechanical | **checked** |
| Em dash in text, never `--` or a spaced en dash | mechanical | **checked** (both forms, since 2026-09-09) |
| No tabs or spaces for indentation (table of contents excepted) | mechanical | **checked** (since 2026-09-09) |
| Ending punctuation followed by two spaces; colon, comma, semicolon by one | mechanical | **applied** (both switches) and **checked** (both, since 2026-09-09). Note: the earlier "two spaces after a colon" switch contradicted the guide and was removed |
| Serial commas / semicolons | mechanical-ish | **gap** — flagging "A, B and C" needs a conjunction heuristic; propose flagging only clear three-item lists, as a "check" not an error |
| Commas and semicolons: preserve as written unless an obvious consistency issue | judgement | documented |
| Paragraphs fully justified unless under one line | mechanical | documented; publisher's layout |
| Footnote lead: italics, colon in italics, one space | mechanical | documented; **gap** — the Draft's footnote bank could emit leads in this form |
| No vertical space between footnotes | mechanical | documented; layout |
| Foreign words in closest normal English pronunciation; diacritics / ACIP only when a technical word is treated in detail | judgement | documented; the Convert pane produces all three forms (pronunciation, diacritics, ACIP) so the writer can choose |
| First citation "bibliography entry B25 (ACIP digital text S25004)", thereafter "(S2, TD03786)" | mechanical | documented; **gap** — Insert Citation (analysis-suite batch 6) should emit exactly these two forms |
| `%` correction or provisional number; `%?` possible correction; `%%` placeholder; leave provisional markers unless asked | mechanical | documented; **gap** — the checker could list every `%`, `%?`, `%%` remaining in a draft so none reaches publishing unseen |
| Source-language page numbers in square brackets; "page"/"folio" spelled out first, then "p."/"f."; removed from English-only manuscripts | mechanical | documented; the Mixed Nuts formatter owns folio markers; **gap** — check first-then-abbreviated sequence |
| Spell out single-digit numbers; numerals otherwise; keep the immediate context consistent | mechanical with judgement | documented; **gap** — flag lone digits 1–9 in prose, excluding folios, dates, section numbers and ranges, as a "check" |
| Bibliography alphabetical in source language by author (ignoring parenthesised parts) then title (ignoring Arya, a, an, the) | mechanical | documented; **gap** — a bibliography sorter belongs with Insert Citation |
| Sanskrit / Tibetan bibliography entry formats (the two templates) | mechanical | documented; **gap** — templates for Insert Citation |

## Word Use

| Rule | Kind | Status |
|---|---|---|
| accurate perception; autocommentary; ebook; mindstream; partless; wood-carver; Middle Way School; Mind Only School; aka | mechanical | **checked** |
| the Jewel of Realizations (not Ornament; lowercase *the*) | mechanical | **checked** |
| Independent branch / Consequence branch (not group) | mechanical | **checked** (since 2026-09-09) |
| Enlightened Being (ruling 11/22) | mechanical | **checked** — lowercase form flagged (since 2026-09-09) |
| in between / between (adverb, preposition) vs in-between (adjective, noun) | judgement | documented — part of speech is not decidable mechanically |
| Prefix compounds closed, with the five hyphenation exceptions | judgement | documented |
| Ampersand for pairs, never in a serial list | mechanical | **checked** |

## Capitalization

| Rule | Kind | Status |
|---|---|---|
| Chicago headline style for titles and subtitles (seven rules) | mechanical-ish | documented; **gap** — a title-case checker for lines styled as headings (articles, short conjunctions, prepositions lowercased; first and last words capitalised) is feasible and worth adding |
| Subtitle after a colon begins with a capital; after an em dash need not | judgement | documented |
| Sanskrit titles: GMR word division; capitalise every word | judgement / mechanical | documented; the Sanskrit engine splits — **gap** — apply capitalise-every-word to a Sanskrit title on request |
| Tibetan titles: dashes join word parts; root letter of main words capitalised (GMR standard) | mechanical | documented; **gap** — the Convert pane can produce this form from ACIP |
| No capital after a colon except lead, quotation, subtitle, proper noun, or two-plus sentences | judgement | documented |
| ad / bc / ce small caps, no preceding space | mechanical | **checked** (spaced era flagged); small caps is a formatting attribute the Manuscript could apply — **gap** |
| aka, not a.k.a. | mechanical | **checked** |

## Italics

| Rule | Kind | Status |
|---|---|---|
| Punctuation after an italicised word is italicised — unless italics mark root-text words, then only if the root text has a similar mark | judgement | documented |
| Parentheses and brackets italicise with their surroundings, not their contents | mechanical in a rich-text editor | documented; **gap** — a Manuscript check for roman brackets inside italic runs |
| Footnote markers never italicised | mechanical | documented; **gap** — Manuscript check |
| Footnote lead and its colon italicised | mechanical | documented; **gap** — footnote bank emission |
| Translators' notes italicised; book titles roman inside an italic paragraph | judgement | documented |
| Book titles italicised | judgement | documented |

## Quotation Marks

| Rule | Kind | Status |
|---|---|---|
| Closing mark inside ending punctuation for a word or phrase; outside for a full sentence | judgement | documented |
| Periods and commas inside quotation marks | mechanical | **checked** (`”.` and `”,` flagged, since 2026-09-09) |
| Semicolons and colons outside | mechanical | **checked** (`;”` and `:”` flagged, since 2026-09-09) |
| Question marks, exclamation points, em dashes by sense | judgement | documented |

## Word Settings

| Rule | Kind | Status |
|---|---|---|
| Word ▸ Preferences: View (drawings, object anchors), Ribbon (Review), AutoCorrect (smart quotes), Language (English US, no auto-detect) | workflow | documented — Word's own settings, outside the tool |

## Legacy rules from Geshe Michael's 2017 sheet not restated in 2023 (confirm with Utpala)

- File name: short English title + date (`Fulfilling the Hopes of the Fortunate 2 8 17`).
- Original Tibetan in ACIP transcription, divided by section; removed from the printed English.
- Errors in the Tibetan: `DAGS {%DGAS}` confirmed, `DAGS {%DGOS?}` suspected — the `%` and `%?` conventions survive in 2023 in generalised form.
- Section numbers `[1]` in 10 pt, continuous through the book.
- Folio markers `[folio 1a]` first, then `[f. 1b]`; lowercase side letter, no leading zeros — survives in 2023 as the page-number rule.
- Footnotes: 12 pt, one space after the number, no space between footnotes, lead in italics ending in a colon, foreign names in pronunciation, technical transcription where detailed.
- Dates: `500bc`, `650ad` — survives.
- Bibliography in four sections (Sanskrit S, Tibetan B, Chinese C, other E), provisional numbers with `%`, renumbered alphabetically at the end.
- Transcription vs pronunciation vs input code — the conceptual rule behind the 2023 "closest normal English pronunciation" rule.
- Final `O, ,` shad handling when splitting into sections; `GO ,,` spacing — owned by the Mixed Nuts formatter (`docs/standards/MIXED_NUTS_FORMATTING_SPEC.md`).

## Build list that falls out of this inventory

1. Checker additions (done 2026-09-09): sentence spacing, punctuation spacing, indentation characters, spaced en dash, quote-punctuation placement, two word-use rows, Enlightened Being.
2. Checker additions (next): US spelling; lone single digits in prose; stray `%`, `%?`, `%%` markers; title case on heading lines; serial-comma "check".
3. Manuscript: a "DCC body" house style (Palatino 12 justified), footnote-lead emission, roman-brackets-in-italics and italic-footnote-marker checks, small-caps era.
4. Insert Citation (analysis-suite batch 6): the two citation forms, the two bibliography templates, the source-language sort.
5. Save As name pattern for the editing hand-off.
