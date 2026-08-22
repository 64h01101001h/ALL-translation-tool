# THL Toolbox — Standards & How-To Documentation (Research Notes)

**Researched:** 2026-08-12, for the Diamond Cutter Translation Tool (Pecha Maker QA reference, input/proofing standards, cataloging, scanning, markup).
**Method note / provenance:** The live `thlib.org/tools/` URL now returns 404. The Toolbox survives in two places: (1) `https://old.thlib.org/tools/` — but its wiki pages are empty JavaScript shells (content was AJAX-loaded from the Sakai wiki at `collab.its.virginia.edu`, whose DNS no longer resolves); (2) the **Wayback Machine**, which holds full server-rendered captures of `www.thlib.org/tools/wiki/*.html` (mostly 2010) and of `collab.its.virginia.edu/wiki/toolbox/*.html` (2017–2025). Everything below was read from those archived captures. Nothing in this file is reconstructed from general knowledge; unreachable pages are listed at the end.

**Key URL patterns for re-fetching:**
- `https://web.archive.org/web/2010*/http://www.thlib.org/tools/wiki/<page name>.html` (2010 captures have full content)
- `https://web.archive.org/web/2019*/https://collab.its.virginia.edu/wiki/toolbox/<Page Name>.html`
- Toolbox home (24 categories): `https://old.thlib.org/tools/` → `/tools/wiki/home.html`

---

## 1. The Toolbox at a glance

Source: `collab.its.virginia.edu/wiki/toolbox/Home.html` (arch. 2019); `old.thlib.org/tools/`.

The THL Toolbox is THL's how-to/standards wing: ~24 top-level categories including **Tibetan Texts** (cataloging, input, markup), **Scanning & OCR**, **Tibetan Scripts & Fonts** (a separate sub-wiki), **Essays** (editorial/formatting standards), **Reference → Dictionaries**, Developers' Zone (TEI/XML DTDs), Workflow Issues, Equipment, Licenses/Credit/Review. The category most relevant to us is **Tibetan Texts**, whose hub page routes to: Overview of Tibetan Text Processing, Cataloging a Tibetan Text, Inputting a Tibetan Text, Marking up Tibetan Text in Word with Styles, Batch Converting Input Tibetan Texts into XML, Markup of English Translations, Outlining a Tibetan Text, Photographing Tibetan Texts, Scanning & OCR, Sigla ID Assignments, Summaries, Translations, and a Tibetan Text Cataloging Glossary.

**THL's canonical text-processing pipeline** (source: *Overview of Tibetan Text Processing*, thlib 2010 capture): catalog → outline (sa bcad) → scan → input + proof → (optionally) critical edition → structural markup in Word styles → batch conversion to TEI-based XML → summaries/translations/scholarly essays layered on top. One master computer file is explicitly meant to be re-renderable as "a Tibetan-style pecha, a Western-style book, a CD, or an Internet webpage."

**Relevance to ALL Tool:** This is the same architecture our tool implements (spine → panes → pecha PDF output); THL's docs are the closest thing to a published industry standard for each stage.

---

## 2. Text input standards (the THL Tibetan Text Input Manual)

Source: `thlib.org/tools/wiki/tibetan text input manual, english version.html` (arch. 2010-10-13) — the master typing/proofing standard; exists in English, Tibetan, and Chinese versions. Supporting: *Inputting a Tibetan Text* (hub).

### Core principles
1. Input in a well-made **Unicode font** (Tibetan Machine Uni).
2. Save in a durable, convertible format.
3. **Input the text exactly as it is, errors and all** — a faithful diplomatic transcription of a known print edition. Corrections must preserve the original reading (see below).
4. No added elements (no extra spaces, ornamentation).
5. Insert the **pagination and lineation of the original print copy** into the text stream.
6. Careful proofreading is non-negotiable.

### Page/line number conventions (critical for our Pecha Maker & corpus tooling)
- Page numbers are inserted **inline** in square brackets at the exact break point: `ཞེས་[231]པ་དང༌།`. **No space before or after the number** (both `… ་ [200b.3]…` and `…[200b.3] …` are named as common errors).
- Folio-style texts use folio number + `a` (front, mdun) / `b` (back, rgyab): `1a`, `1b`. Some modern prints have Western numbers per side; the page-numberer macro supports both ("number on each side" vs. "front side only").
- Line numbers: `[page.line]` — e.g. `200b.3`; inserted immediately after the final tsheg of the preceding line, before the first letter of the new line.
- **Digital pagination** (for born-digital/stabilized editions): every **20 syllables = 1 line, 300 syllables = 1 page, so 15 lines per digital page**. Once published, digital page/line numbers are frozen even if corrections later shift the counts slightly.
- File chunking: start a new file every **150 pages**, split at an original page boundary; each file repeats the metadata table.
- Kangyur project file naming: `ka_0001a.doc` = volume letter + start folio + side; a second text starting on the same page gets an `x` suffix (`ka_0001xa.doc`).

### What NOT to type (ornamental vs. textual)
- The **yig mgo (༄༅།) at the head of each folio side is ornamental** — type it only at the very beginning of the text (and reproduce exactly what the first page has, e.g. ༄༅༅།); never on subsequent folios, and not at the start of every file.
- Do not type **tsheg strings used as line-fill** (`…དང༌༌༌༌༌༌` → type `…དང༌`).
- Do not type the **mchan rtags** (dot-chains marking annotations); annotations (mchan 'grel) go into footnotes styled `Annotation Block,anb` instead, to be re-rendered traditionally when a pecha is generated.

### Faithfulness rules
- Errors in the original are reproduced. If an editor with authority corrects, the macro wraps it TEI-style: `<sic corr="corrected" resp="initials">actual reading</sic>` — original reading always preserved.
- **Contractions/abbreviations (bskung yig) in dbu can are NOT expanded** (`ལཌ་` stays `ལཌ་`, not `ལགས་`; `ཉམསུ་` not `ཉམས་སུ`). Cursive-script texts using extensive bskung yig are input expanded (dbu can can't reproduce them) — flagged per project.
- **Illegible text:** `{ILLEGIBLE[12-3]}` (line 3 of folio side 12); multiple per line get `a`, `b`… suffixes; each illegible span must be **scanned** and the image named `SIGLA-ILL-12-3a.jpg`. Style `Illegible,il`.
- **Unclear text:** best-guess input marked with `Unclear,uc` style (displays red).
- Special glyph traps called out: visarga ཿ vs. gter shad ༔; avagraha ྅ vs. ཉ; che mgo ༸ vs. digit ༧; 'og skor (circles under root-text syllables in commentaries); all shad types (༎ ༏ ༐ ༑ ༈) reproduced exactly; rin chen spungs shad ༑ typed as-is (it marks a tsheg bar that starts a new line).
- Non-breaking space: EWTS underscore `_`; typing uses paragraph returns (not manual line breaks) and non-breaking spaces after shad.

### Metadata table (per file — "an electronic dpe cha label (dpe mtshan)")
Bilingual (Tibetan/English) rows: Title of Text; Cover Page; Title on Cover; Title on Spine; **Margin Title** ("usually appears on the front-side of each folio for dpe cha style books"); Author; Collection name; Publisher name/place/date; ISBN; Library call number; Other IDs; Volume number; **Pagination of Text** (format `58.4-103.7` or `24b.3-78a.6` depending on numbering style); Pages in this file; Inputter name + dates + place + input method; Proofreader name + dates + place; Markup person + dates; Problems/Anomalies; Converter + dates. Dates always `YYYY-MM-DD`. (The later Kangyur workflow moved most bibliographic metadata to a pre-made catalog keyed by unique text ID, leaving the inputter only name/date/location/volume-letter/ID.)

### Critical editions / variant readings
- Each edition gets a **siglum** from a central Authority List of Sigla (with a sigla-manager system feeding an XML authority feed).
- Base edition is marked up; variants recorded in Word **footnotes**, converted to TEI `<app>` on XML conversion.
- Format: `{bracketed base reading}` + footnote `PL (152.4): བརྙན་འདི་` (siglum, page.line in parens, colon, variant). Multiple differing editions separated by `;`; identical readings by `,` sharing one entry. Omission: footnote `PL (405.3): omits`. Insertion: no braces, footnote at insertion point with added text. Preferred reading marked `*` before the siglum.
- Precise footnote placement rules relative to tsheg/shad (after the tsheg, after first shad of a double shad, special cases for final ng/g) — spelled out with examples.
- Never combine annotations and variant readings in one footnote (they must remain separately filterable).

### Proofreading standard (THL's QA doctrine)
- "Double input" (two independent typists + Word compare) endorsed as the strongest method.
- **At least one proofer must be a different person than the inputter.**
- Proofing must be done **against a printout, never on-screen**, and **against the original text**.
- Turn on formatting marks (¶, spaces) while proofing.
- THL explicitly rejects the read-aloud two-person method (homonyms uncatchable) and is skeptical of letter-by-letter spelling aloud.

**Relevance to ALL Tool:** This is the de-facto standard our input-center formatter and QA checks should mirror: inline `[231]`/`[24b.3]` pagination grammar, folio a/b conventions, yig mgo policy, non-expansion of abbreviations, `{ILLEGIBLE[..]}` flags, sic/corr preservation. Good candidate rules for an automated lint pass over input texts.

---

## 3. Structural markup in Word styles (pre-XML)

Source: `thlib.org/tools/wiki/marking up tibetan text in word with styles.html` (arch. 2010); *Using Word Styles for THL Markup* (restricted; not readable — see dead list).

- Texts divide into **front (klad ཀླད་) / body (gzhung གཞུང་) / back (mjug མཇུག)**; these three get Heading 2, their subdivisions Heading 3, etc. Title of the work = Heading 1. Editor-added subhead numbering `[1]`, `[2.1]`, `[2.1.1]`… is bracketed and styled **Added by Editor** so it can be stripped.
- Subheads are always **editor-added text** (never the author's words), written in Tibetan, ending in a shad.
- Body text styles: `Paragraph` (prose), `Verse 1`/`Verse 2` (first/subsequent lines of a stanza; return after the second shad of a double shad), `Citation Prose 1/2`, `Citation Verse 1/2`, `Paragraph Continued` (after a quote, no topic change). Close-quote markers (ces so / zhes so) are NOT part of the citation.
- `Sa bcad` is a **character style** for topical-outline statements (include the final shad).
- Name styles: `Author` (in colophons), `Text Title`, `Colophon Text Title`, `Colophon Chapter Title`, `Name Personal Human`, `Name Buddhist Deity`, `Speaker …` variants, `Epithet`, `Speaker Epithet`. Oblique references ("rtsa rgyud las", "de nyid las") are marked as text titles too, excluding the particle las.
- Character styles include the final tsheg but not a final shad (except phrase-level styles like sa bcad, which include the shad).
- **Pecha round-trip rule:** carriage returns go **after the shad + the following white space**, and that space must never be deleted — "the idea here is that an electronic edition should be able to be converted into traditional pecha formatting… as this space is intrinsic to the text, if you remove it, the pecha formatting will not appear correctly."
- Introductory scenes (gleng gzhi) belong in the body, not front matter; break them out with their own subhead if the text doesn't.
- All formatting must be via styles (anything else is lost at XML conversion).

**Relevance to ALL Tool:** The style inventory maps 1:1 onto the structural elements a pecha layout engine must handle (prose vs. verse vs. citation vs. annotation), and the shad+space rule is a concrete reflow invariant our Pecha Maker must respect.

---

## 4. Outlining a Tibetan text (sa bcad)

Source: `collab…/toolbox/Outlining a Tibetan Text.html` (arch. 2024) — same content in 2010 capture.

- Texts are analyzed as front/body/back; classical texts additionally carry **topical outlines (sa bcad)** — nested hierarchies of 10, 20, "even more" levels, hundreds of sections. When chapter structure and sa bcad conflict/overlap, **the sa bcad takes priority** (the other is captured via milestones).
- THL Word outline template (THLOutlineStyles) supports **25 heading levels**; h1 = front/body/back; deeper levels indented.
- **Header format:** `chos thams cad stong pa/ [The emptiness of all phenomena] (44.5-56.4)` — Tibetan first, English translation in brackets (first letter capitalized), then pagination range as `page.line-page.line`. Multiple editions: `(SG 44.5-56.4; DG 56.4-63.4)` — 2–3-letter sigla + space before each range, semicolon-separated.

**Relevance to ALL Tool:** ready-made grammar for outline/TOC records and for a future navigable-outline feature over texts in the Library.

---

## 5. TEI/XML markup standards

Sources: *XML Markup Manual for THL* (collab arch., large), *Text Markup – Overview* and *Text Markup – Structural Divisions* (thlib 2010), *XML Markup in THL* (collab arch.), *Our Guide to TEI XML Markup* (index page).

- DTDs: **xtib2 / xtib3** (essays & texts) and **xtibbibl3** (stand-alone catalog records) — all TEI P4 derivatives built with TEI PizzaChef/Roma; DTD was hosted at `texts.thlib.org/xtib3.dtd`.
- Document = metadata (teiHeader-style) + `<text>` with `<front>/<body>/<back>` (only body required).
- **Divisions:** essays use numbered `<div1>…<div3>`; **Tibetan texts use unnumbered `<div>` (infinitely nestable, 20+ levels)** with `type` ∈ {chapter, section, outline} (outline = sa bcad) and `n` = number. Divs nest but never overlap; when chapter and outline breaks overlap, divs carry the outline and `<milestone>` carries the chapters.
- **Pagination milestones:** `<milestone unit="page" n="2a"/>` and `<milestone unit="line" n="2a.1"/>`; units `digpage`/`digline` for born-digital pagination. Example given inline in running Tibetan.
- An earlier line-referencing scheme also documented: `<seg type="shad" n="3.24">` — shad-delimited segments, **100 shad-lines per digital page** (superseded in practice by the 20-syllable/300-syllable milestone scheme in the Input Manual).
- Verse: `<lg>` stanza / `<l>` line, with `<seg>` shad-units inside `<l>`. Citations: `<title>` + `<quote>` (textual) vs. `<persName>` + `<q>` (speech); `rend="inline"` vs. default block. Lists via `<list>/<item>` with HTML-like `rend` values. Links: `<ptr/>/<ref>` internal by ID, `<xref>` external; images as `<xref type="img">` inside `<p rend="img">`.
- Editorial elements: `<sic corr resp>`, `<corr sic resp>`, `<unclear reason cert resp>` (e.g. `reason="smudged" cert="40%"`), `<add resp>` for editor-added headers, `<app>` for variants.
- **Minimum required metadata:** e-text title, transcriber, editor, converter/lineator, markup person (each with resp/name/date `yyyy-mm-dd`), THL location links, summary; source document: title, author, editor, translator, **pagination**, place, publisher, date.
- Language markup + ISO language codes have dedicated pages (`iso language codes used in thl xml markup`, `language markup in xml documents`).

**Relevance to ALL Tool:** if we ever export/import TEI (e.g. interchange with BDRC/84000 or archival exports from the Library), THL's milestone and div conventions are the reference vocabulary; the `unit="page" n="2a"` pattern matches our folio-side conventions.

---

## 6. Cataloging standards (descriptive cataloging of Tibetan texts)

Sources: *Cataloging a Tibetan Text* (hub, thlib 2010), *Introduction to Cataloging System* (= "Summary of THDL catalog mark-up scheme"), *Kangyur-Tengyur Cataloging Manual*, *Kangyur-Tengyur Cataloging Form* ("KT cataloging in Word template"), **Degé Kangyur-Tengyur Deep Cataloging Form** (collab arch. 2019 — the most detailed document recovered), *Long Cataloging Form*, *Markup of a Collection Catalog*, *More Issues Related to Translator's Colophon*.

### System design
- Catalog records are entered in **Word table templates** → converted by VBA macro to **TIBBIBL** XML (a THL/IATH DTD) → wrapped in TEI → assembled into a collection catalog (GDMS DTD) → published via XSLT. TIBBIBL covers: identification (IDs), **physical description** (pagination, page size, script), intellectual description (genre/doxography), provenance (author/translator/editor/reviser), **title information** (title page, title line, margin, end-of-section titles), table of contents (chapters + pagination).
- **Shallow vs. deep cataloging**: shallow = one table (title, author, pagination); deep = multiple tables (metadata; text info; front/body/back chapter-level elements; text titles; provenance).

### Concrete conventions (Degé deep form — highest-value specifics)
- **Pagination format:** `234b.3-456a.5` — folio side (a/b) + "**always a period before the line number**". Multi-volume spans: `011:378a.5-012:12b.4` (volume always 3 digits). No commas in numbers ≥1000.
- **First folio of a volume has text on one side only: it is side 1b** (Degé and similar editions).
- **Standard lines per side: 7 for Degé; the first two sides of each volume generally have 5 lines.** Line-fraction tables for partial sides: 7-line texts 1=.143, 2=.286, 3=.428, 4=.571, 5=.714, 6=.857; 8-line texts 1=.125 … 7=.875. A "page differential" field reconciles non-standard sides, duplicate/skipped folio numbers, and multi-volume spans; "total sides" is macro-computed to the line.
- **Inserted duplicate folios**: gong (གོང་)/'og (འོག་) pages become `66Aa/66Ab` and `66Ba/66Bb`, lines as `66Ba.5`.
- **IDs:** master text number = 4 digits across all Kangyur/Tengyur editions (links records of one text across editions); per-edition text number 4 digits; volume number 3 digits; concordances kept to Peking (P) and Tohoku numbers; references like `THL-D317.A3` = 3rd front section of Degé text 317.
- **Volume letter** (ka, kha…, restarting per genre section); **genre category as printed in the left margin of the front side of each folio** (with authority lists of Kangyur categories — 'dul ba, sher phyin, mdo tshan bcu gsum po, phal chen, dkon brtsegs, mdo sde, myang 'das, rgyud, rnying rgyud, dus 'khor 'grel bshad, gzungs 'dus, smon lam — and Tengyur categories); **script field** (normally `dbu can/`); illustrations yes/no.
- **Chapter-level elements (CLEs):** front (A), body (B), back (C) tables, each row one section. Canonical front sections: **title page (mtshan byang), title line (mtshan / skad gnyis shan sbyar gyi mtshan), translator's homage ('gyur phyag), author's homage, expression of worship (mchod brjod), statement of intent (rtsom par dam bca' ba)**. Chapter titles recorded exactly as printed (chapter number as printed even if wrong; drop final 'o/rdzogs so: `le'u ste gsum pa'o/` → `le'u ste gsum pa/`); section-name field records le'u/skabs/bam po etc.; three paginations per chapter (title, whole section, end-of-chapter text title). Untitled texts: "Untitled." with pagination and `NT`.
- **Wylie hygiene:** straight quotes (not smart quotes) for all Extended Wylie; everything else smart quotes. Common citation titles only from authoritative sources (never composed by catalogers) — with source attribution.
- Emendation markup in Word: `{original[corrected]}` → XML `<corr sic="…" resp="initials">…</corr>`; unclear: `<unclear reason cert resp>`.
- **Cataloging QA:** cataloger and proofer must be different people; both work from printouts; **always read the original text first, then check the record** (never the reverse); short-span alternation; conversion errors logged centrally (TMU Conversion Errors page); status reports updated per volume (`initials + working` → `initials + YYYY-MM-DD`).
- **Sigla rules** (*Sigla ID Assignments*): collected works = 2-letter author initials + `CW`; Kangyur-Tengyur = established field sigla prefixed `KG-`/`TG-`; Nyingma Gyubum prefixed `NG-`; real IDs ultimately numeric ("one to a billion") with sigla as mnemonics; central sigla manager with XML authority feed.
- Translator's colophon caveat (Phil Stanley): ~70 author-attribution variations vs. ~500 translator/reviser-attribution variations across Kangyur/Tengyur editions — translator colophons are unreliable and were widely tinkered with; treat attributions cautiously.

**Relevance to ALL Tool:** the folio/pagination grammar (a/b sides, `.line`, `vol:page`, 1b first side, 7-line standard with 5-line openers, gong/'og) is exactly the QA vocabulary a Pecha Maker needs for validating folio numbering and line counts, and the CLE section list is a ready checklist for auto-detecting front/back matter in texts. The master-number/edition-sigla model parallels our master catalog design.

---

## 7. Pecha-specific formatting facts (collected)

The Toolbox has **no single "pecha print layout" page** (no margin/point-size/paper spec was found — see dead list; the closest was never published). What it does give, scattered:

- **Physical:** "for Derge paper it is usually **680 mm long × 100 mm tall**" (Scanning Texts — scanner page-size setting). Degé standard **7 lines per side**; first two sides of a volume **5 lines**; first folio of a volume printed on one side only, numbered **1b**. Some Kangyurs printed in **red ink** (harder to scan). Genre/section abbreviation printed in the **left margin of the front side**; **margin title** on the front of each folio (metadata field). Tibetan folio number written in the margin of the front side; some editions add Western numerals per side.
- **Layout semantics for regeneration:** page numbers stripped from the text stream and re-placed "on the side of the page (in the case of pechas), at the bottom of the page (in the case of books)"; mchan 'grel re-rendered "in the traditional way" from footnotes; yig mgo ༄༅། regenerated ornamentally on each folio front (which is why it isn't typed); tsheg line-fill regenerated (not stored); shad+space preserved so pecha lineation reflows correctly; rin chen spungs shad ༑ used where a tsheg bar begins a new line (an artifact of print line breaks — replaceable by plain shad in non-diplomatic editions).
- **Digital derivative sizing for pecha page images:** JPGs 1000–1300 px wide (published Nyingma Gyübum pages = **1150 px**); no thumbnails (too small to be useful); general catalog scans resized to 1200–1600 px wide (~200–250 px tall), JPEG quality 7–8, target < 150 KB/image; bitonal TIFFs converted to grayscale before JPEG to avoid jagged text.

**Relevance to ALL Tool:** these are the only hard numbers THL publishes that bear on pecha QA: Degé proportions (~6.8:1 page ratio), 7/5-line conventions, 1b start, margin-title and margin-genre placement, side-number placement. Our Pecha Maker QA checklist can adopt them as the "Degé profile" defaults; everything else (font sizes, margins in mm) must come from other sources (e.g. GMR/publishing-team guidelines).

---

## 8. Scanning & imaging standards

Sources: *Scanning & OCR* (hub, collab arch.), **Scanning Texts** (collab arch. — main spec), *Scanning Books and Documents*, *Scanning Equipment* (dated 2007), *Photographing Tibetan Texts*, *Scanning Guidelines for Images*, *Digital Image Formats*, *Processing Finished Scans for Inclusion into Online Catalog*, *Creating Web-Viewable Derivatives of Images of dpe cha Pages for Catalogs*.

### Text scanning (dpe cha)
- **Master format: uncompressed TIFF. Never save compressed TIFF as master; zip/7z only as temporary storage; archival = uncompressed on dedicated drives, two entirely separate copies.**
- **Resolution: 300 dpi minimum; test at 600 and use it if detail improves.** For OCR-bound pecha scans: **600 dpi, black-and-white, TIFF, one file per page, no compression** (Canon DR-M160 sheet-feed workflow; counter-numbered filenames, 3–4 digits). Color: don't go below 400 dpi; never JPEG masters. B/W TIFF can use **GROUP4** compression for small files (in the OCR workflow context).
- Color mode by material: 24-bit color for e.g. yellowed paper/faint writing and original manuscripts ("you can't go wrong scanning in color"); bitonal/grayscale fine for clean modern prints. Litmus test: does a print of the scan read well.
- **Duplex long-page scanning**: set exact page size (Derge 680×100 mm); duplex so front/back captured in one pass; feed pages one at a time by hand; adjustable guides; **one image per folio side (not recto+verso combined)** for delivery flexibility.
- Conservation: auto-feeders damage manuscripts — "any serious conservator will strongly resist" them for unique/uncommon manuscripts; use flatbeds or cameras for fragile items; wipe Tibetan paper (grit scratches the glass → line artifacts); humidity can relax creased folios; watch for light/red-ink pages, missing/mis-ordered folios (check before scanning); number files in sync with the text's own page numbers.
- Bleed-through: scan at lightest brightness; severe cases: photocopy at light setting, then scan the photocopy.
- Training/consistency: closely supervise initial work; forbid mid-project settings changes.

### Photographing texts (camera-based digitization)
THL's Library of Congress Degé Kangyur shoot (~79 volumes): Canon EOS 1DS Mark II (16.7 MP), 50 mm compact-macro lens, copy stand with camera at **95 cm**, two light banks, remote release, autofocus. **RAW (.cr2) + highest-quality JPG per shot; compression 10 (lowest); ISO 100; tungsten white balance (~3200 K)**. File naming: `kt-d-v###-p###.jpg` (edition-version-volume-folio side); one camera folder per volume. RAW → TIFF → JPG derivation pipeline (Photoshop).

### Image scanning (photos/negatives/line art)
- Color: **3000 px on the long side, 24-bit (8 bits/channel), Adobe RGB 98, TIFF, LZW ok, uncorrected (gamma before scan only), crop to the image** (~20 MB/scan).
- Grayscale line art: **1200 dpi, 8-bit**.

### Web derivatives of page scans
- Goals: readable, fits ~1200–1400 px screens, **< 150 KB per image**. Pipeline (Photoshop batch): bitonal→grayscale mode change; rotate if needed; conservative whitespace crop; brightness/contrast tuned on samples per edition; resize to 1200–1600 px wide (some bordered editions 2000–2200); save JPEG quality 7–8. Dpe cha derivatives 1000–1300 px wide, no thumbnails.

**Relevance to ALL Tool:** direct QA reference for any scan-import features and for advising input centers; the "TIFF master / JPEG derivative / one image per folio side / filenames in folio order" doctrine matches what our catalog and page-viewer would consume.

---

## 9. Editorial, translation & essay standards

Sources: *Markup of English Translations of Tibetan Texts* (thlib 2010), *THL Translation Guide* (collab arch. 2016), *Formatting: Notes, Numbers, Dates, Punctuation* (collab arch.), *Summaries*, *Translations*, *Scholarly Essays* (hubs).

- **Aligning translation pagination to Tibetan pagination:** page numbers embedded in curly braces in both languages. Because Tibetan/English syntax inverts clause order, the standard is: **place the page number at the end of the split clause in the English** (`des cang mi smra {22} bas khas blangs te/` → "He accepted by not speaking at all. {22}"). Exception: if nearly all of the sentence is on the next page, the number goes before the sentence. Explicitly an "acceptable arbitrary standardization."
- **THL Translation Guide** (brief): translate faithfully but intelligibly; preserve the format of the original transcription (line-by-line stays line-by-line); neither mechanical word-for-word nor free "poetic license."
- **Essay/JIATS formatting standard** (detailed, Chicago-like): footnotes/endnotes with Arabic numerals, full citations in notes (no author-date in body); spell out numbers ≤ 100; page-span digit rules (e.g. 101–8, 321–28); fractions spelled out; "percent" spelled out; BCE/CE; centuries spelled out lowercase; decades "1960s"; **all Tibetan in THL Extended Wylie — shad = `/`, and after each shad an underscore `_` (renders as space); every space in Wylie is a tsheg; space between nga and shad**; no italics except English emphasis; smart quotes for prose (but straight quotes in Wylie data-entry contexts); serial comma; spaced en dashes.
- *Summaries* and *Translations* Toolbox pages were **stubs** ("still under development") even in 2010 — THL never published full markup specs for those two.
- Critical-edition practice lives in the Input Manual's variant-readings section (see §2) — footnote-based apparatus with sigla, page.line, omission/insertion/preferred-reading conventions.

**Relevance to ALL Tool:** the `{page}` alignment rule is directly reusable in our translator-oversight pane (aligning HGM translations to Tibetan folio references); the Wylie punctuation conventions (`/_`) already match our EWTS handling.

---

## 10. Tibetan scripts, fonts & input tools (sub-wiki)

Source: `collab.its.virginia.edu/wiki/tibetan-script/*` (arch. 2019–2025) — a separate ~120-page wiki ("Tibetan Scripts & Fonts").

- **Font catalog** (Unicode): CTRC-Betsu/Drutsa/Uchen/Tsumachu (China Tibetology Research Center, free), **Jomolhari / Jomolhari ID** (Chris Fynn, open, Unicode + GB/T 20524-2006), **Tibetan Machine Uni** (THL's own, **GPL**, OpenType dbu can with "full support for the Sanskrit combinations found in chos skad texts"), Kailasa, Kokonor, Marpa, Terton Sogyal Uni, XenoType families (incl. **TB Woodblock**, HorYig, sPungsChen), Yagpo, Dedris, plus legacy (Tibetan Machine, Tibetan Machine Web, Sambhota, Tibetan!, LTibetan) with a **Tabular Survey of Legacy Tibetan Fonts** and a **Tabular Survey of Converters & Reverters** (Universal Tibetan Font Converter, UDP, Wylie2Uni, TB Youtso, Jskad, ATTU).
- Input: TISE (Wylie), Keyman (Sambhota), Wylie Word, per-OS input-tool pages (Windows/Mac/Linux/browser), *Tibetan Input System Principles*.
- Transliteration/transcription: pages on EWTS, ACIP transliteration, LOC transliteration, THL Simplified Phonetics, Chinese partial Wylie, Sanskrit/Prakrit schemes; glyph-by-glyph script documentation (consonants, vowels, numerals, punctuation, Sanskrit letters, "other marks and signs") — though the archived Punctuation table's cells did not survive capture.
- Practical rendering pages: *HTML for Displaying Tibetan*, *Making Web Pages in Tibetan Unicode*, CSS for TMU, sorting Tibetan in MySQL/PostgreSQL, Word line-breaking ("Use Line Breaking Rules" compatibility option so lines don't break mid-syllable).

**Relevance to ALL Tool:** authoritative license info for candidate pecha fonts (TMU is GPL; Jomolhari open; CTRC free) and the historical converter landscape our Convert pane supersedes. XenoType TB Woodblock is worth evaluating for pecha-facsimile output.

---

## 11. Dictionary & translation-tool documentation (adjacent but valuable)

Sources: *Dictionaries* (hub), **THL Tibetan Dictionaries Editorial Manual** (thlib 2010 capture, long), *Tibetan Translation Tool* technical docs (collab arch.).

- The **Editorial Manual** specifies THL's dictionary information architecture: hierarchical entries where every datum (definition, sub-definition, pronunciation, etymology, spelling variant, literary quotation, oral quotation, translation equivalent, model sentence, register/metadata, source) attaches either to the head term or to another datum; explicit aspiration to OED-style historical citations; head-term formatting rules; verb handling; controlled vocabulary/knowledge-map linkage; bibliographic sourcing per definition.
- **Tibetan Translation Tool** (the ancestor of our Lookup/Overlay panes; by Andrés Montano Pellegrini, script input by Edward Garrett): takes pasted Unicode, typed Wylie, or Tibetan script; **segments the passage into component phrases and words and stacks corresponding dictionary definitions**; runs online, or offline via local install; separate pages for installation, available dictionaries, user instructions, and developer notes. (This is the same tool whose `thl.wrd`/`thl.def` payloads we already extract per the roadmap.)

**Relevance to ALL Tool:** the editorial manual's entry model (typed relationships, citations, register) is a useful benchmark for how far our Geshe Michael Roach Dictionary schema could grow; the TTT docs confirm the segmentation→stacked-definitions UX pattern our Overlay pane implements.

---

## 12. Workflow / project-management (back pocket)

Sources: *Workflow Issues* (hub), *Cataloging Workflow*, KT manual general principles.

- Tooling THL used: Subversion, Sakai worksites, JIRA (staff task tracking), status-report wiki tables per volume/edition, sigla manager, roster IDs (`per3141`) for every worker embedded in records and notes.
- Recurring QA doctrine across all manuals: **separate cataloger/proofer, work from printouts, original-first checking order, centralized error logs, per-volume status updates with initials + ISO dates, zip deliverables with structured names** (`N-Kg-v009.zip`).

---

## DEAD / UNREACHABLE / NOT RECOVERED

Confirmed dead or not readable; do **not** cite content from these without recovering them first:

- `https://www.thlib.org/tools/` — **404** on the current (WordPress) thlib.org. `https://thlib.org/tools/toolbox/` — 404.
- `collab.its.virginia.edu` — **DNS no longer resolves** (the entire Sakai wiki, canonical home of the Toolbox).
- `old.thlib.org/tools/wiki/*.html` — pages load but are **empty navigation shells** (content was AJAX-fetched from the dead collab server).
- **"Using Word Styles for THL Markup"** — the 2010 thlib capture returns "**Restricted Area** … not available for public viewing." Not recovered in any open capture found.
- **"Tibetan Text Section Labels for Structural Markup"** — archived Sakai page returns only "closed." (Its content likely equals the "section names" / Tibetan Text Cataloging Glossary list, of which the Degé form quotes the front-section subset — see §6.)
- **"Section names" / Tibetan Text Cataloging Glossary** (`…/site/c06fa8cf-…/section names.html` and `provenance figures.html`) — not recovered (site-UUID wiki pages sparsely archived).
- **"Scanning Pages"** (`/tools/wiki/scanning pages.html`, linked from the Overview) — no capture found under that name (superseded by *Scanning Texts*, which was recovered).
- **"THL Word To XML Manual"**, **"Scanning Microfiche"**, **"Scanning Journals"**, **"Structure of the Word Document"**, **"How to OCR a PDF"**, **"Kangyur-Tengyur TIFF to JPG Conversion Instructions"** (canons-site UUID wiki), **"Technically Processing Images"** — captures exist in the CDX index for some of these but downloads failed or were not attempted in this pass; recoverable later via the URL patterns above.
- **"Cataloging the Structure of a Tibetan Text"** (`thdl.org/xml/show.php?xml=/collections/literature/xmldoc/techntsfull.xml`) — no Wayback capture found for the XML view.
- **Punctuation glyph table** (tibetan-script wiki) — page recovered but the table cells are empty in the archive.
- **"Summaries"** and **"Translations"** Toolbox pages — recovered, but are stubs ("still under development"); THL never published those specs.
- Downloadable payloads referenced everywhere (TibetanLanguageTemplate.dot / .zip, THLOutlineStyles, WordToXML converter `w2xmlconv-v1.3.2`, cataloging entry forms .doc, `outlineConv.zip`, xtib3.dtd at texts.thlib.org) — **not downloaded/verified** in this pass; some may survive in Wayback's file captures if ever needed.

---

## Local copies

Raw HTML + extracted text of every recovered page are in the session scratchpad (`scratchpad/thl/*.html|*.txt`) — scratchpads are ephemeral, so re-fetch from the Wayback URLs above if needed later. The CDX inventories (`cdx_toolbox.txt`, `cdx_thlib.txt`, `cdx_all.txt`) list every archived Toolbox page (≈240 toolbox pages, ≈120 tibetan-script pages, ≈860 thlib.org/tools/wiki captures).
