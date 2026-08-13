# Working with Tibetan: Guidelines & Protocols

These chapters are the house protocols of the ALL Translation Tool
for working with Tibetan across formats and mediums — print,
digital text, transliteration, pronunciation, scans, catalogs, and
audio. They combine the project's own inviolable rules (ACIP/ALL
practice and Geshe Michael Roach's conventions) with the published
standards of the Tibetan & Himalayan Library (THL, thlib.org),
whose site-wide survey is in progress; THL-sourced sections cite
their pages and will deepen as the survey completes.

## Print & pecha work

The traditional pecha (dpe cha) is a long, loose-leaf folio format;
producing one is a typographic act with fixed conventions, which
the Pecha Maker (Read → Export) implements:

- **Folio, not page.** A folio has two sides; the folio numeral is
  written in Tibetan digits, rotated on the left margin of the
  recto (side A) only.
- **Margin furniture.** The recto margin traditionally carries the
  ya-yig (short margin title) toward the top and the volume letter
  (ka, kha, ga…) below — so a loose folio can always be returned
  to its text and volume.
- **The head mark ༄༅། ། (yig mgo)** opens a text. The first side
  (1a) of a formal pecha is an ornamental title folio; the text
  itself begins on 1b.
- **Framing rules** (the double rule around the text block) come
  in weights; choose by output size — fine for personal copies,
  classic to match common woodblock prints, bold for large sheets.
- **Line count**: seven lines per side is the classical look; 5–9
  are accepted.
- **The Degé measure** (THL's Degé cataloging documentation): a
  Degé folio runs about 68 × 10 cm at 7 lines per side — and the
  first two sides of a volume are set at **5 lines** with
  proportionally larger letters (the "classical opening"), the
  text settling into its regular count from the third side. The
  first folio of a volume is side 1b. The Pecha Maker offers the
  Degé preset and the classical opening as options.
- **Printing protocol**: true-size folio sheets go to a print
  shop; for office printers, impose two-up on A4/US Letter with
  corner cut marks and trim. Whole folders batch-render to a
  pecha set (Export → Batch). Partner printing (Vimala Treasures)
  takes the native-size PDF.
- **Script integrity in print is absolute**: every syllable passes
  the battery-proven conversion chain; a syllable that cannot be
  proven prints as ⟨wylie⟩ — flagged, never guessed.
- *Source: THL Toolbox (Tibetan Texts pipeline, Degé deep
  cataloging form) — full notes in docs/research/THL/. THL
  publishes no complete pecha point-size/margin spec; where their
  documents are silent, this tool's measured defaults apply and
  say so.*

## Digital Tibetan text

Protocols for keeping digital Tibetan trustworthy:

- **Transliteration is data; script is presentation.** Master
  files live in transliteration (ACIP or EWTS); Unicode Tibetan is
  generated through proven engines, never hand-patched.
- **Never guess.** A failed conversion is flagged (⟨wylie⟩
  markers, review flags) and surfaced. Silent approximation is how
  corpora rot.
- **Unicode normalization matters**: composed characters (e.g.
  U+0F73) and their decomposed forms (base + U+0FB7) are
  canonically equal but byte-different; compare NFC-normalized,
  and never mix forms within one file.
- **The tsheg is part of the text**, not decoration — preserve it
  in round-trips; shads mark clause barriers and drive
  segmentation.
- **Provenance is sacred.** Every generated field carries its
  source tag (engine version, wave, review flags); tags survive
  every schema change.

THL's Tibetan Text Input Manual (their complete typing and
proofreading standard) adds protocols worth adopting wherever we
key or receive keyed text:

- **Pagination travels inside the text**: inline markers like
  `[231]` or `[24b.3]` (folio + side + line), with strict
  no-extra-space rules, so any passage is citable to its witness.
- **Never expand abbreviations** in the transcription; record
  what the page shows.
- **Illegibility is marked, never smoothed over** — THL uses an
  explicit `{ILLEGIBLE…}` convention plus a required image of the
  page region; the same spirit as our ⟨wylie⟩ flags.
- **Corrections are attributed**: original reading, corrected
  reading, and who made the call — a critical apparatus in
  miniature, with sigla for witnesses.
- **Proofing doctrine**: the proofreader is never the inputter;
  proof from printouts; read the original first; double-input
  with machine comparison is the gold standard.
- **Born-digital pagination** (for texts with no printed
  witness): THL freezes 20 syllables to the line and 15 lines
  (300 syllables) to the page at publication, so citations stay
  stable forever.
- *Source: THL Toolbox → Tibetan Text Input Manual and TEI/XML
  manuals (recovered captures; full notes in
  docs/research/THL/toolbox_standards.md).*

## Transliteration schemes

The schemes in daily use, and the tool's conventions:

- **ACIP** — the input-center standard this project was born from;
  uppercase Latin, with the official Sanskrit input code (a#/r%/n~
  style) for Tibetanized Sanskrit. The Document box shows ACIP.
- **EWTS (THL Extended Wylie)** — the scholarly interchange
  standard, and the tool's internal pivot: engines convert
  ACIP ⇄ EWTS ⇄ Unicode, with round-trips proven by batteries.
- **IAST** — for Sanskrit headwords and citations (Lokesh Chandra
  layer).
- **Protocol**: convert with the engines, never by hand; when a
  scheme cannot express something, flag rather than improvise.
- **The EWTS specification is captured in full** (v2.0, Germano;
  all sixteen rules, all charts, and the complete 246-entry
  Unicode↔EWTS character map with usage notes) in
  docs/research/THL/language_tools.md, together with THL's
  normative Standard Tibetan Stacks list — the reference behind
  the `+` explicit-stack rules. Any future engine question about
  an edge of EWTS is settled against that capture, not from
  memory.
- **Font licensing** (for shipping type): Tibetan Machine Uni is
  GPL; Jomolhari is SIL Open Font License — the OFL font is the
  safe embed, the GPL font is fine to *use* but its license terms
  govern redistribution.

## Pronunciation systems

Spoken renderings differ by system and by register — the tool
treats pronunciation as derived data with an authority lane:

- **GMR convention (binding for this project)**: Geshe Michael
  Roach's phonetics, produced by the battery-proven engine
  (42-item spot battery + 2,118 card lines). This is the
  convention of the courses and cards.
- **Register is real**: a term can carry different renderings in
  prayer versus prose (bsod nams: "sönam" contexts differ) —
  single-register terms carry warnings, never auto-resolved.
- **Derived registers (e.g. prenasalized km-/kd- forms) enter
  spoken output only through the Approval lane** — proposed,
  reviewed, approved; never silently.
- **THL Simplified Phonetic Transcription** (Germano & Tournadre):
  the scholarly standard for Standard Tibetan, planned as an
  additional display mode. Its published rules are now captured
  COMPLETE in docs/research/THL/language_tools.md — the
  30-consonant mapping, all sixteen special rules with examples,
  the exceptional-pronunciation classes, the 33-example battery,
  and the 19-word exception list — so the port can be proven
  rule-for-rule against the standard's own examples, with THL's
  live converter as a second oracle. Never approximated.
- **ACIP pronunciation standard**: banked at
  docs/standards/ACIP_PRONUNCIATION_STANDARD.md (Catalog Release
  IV, pp. 173–178); engine agreement 51/52 on pure Tibetan.

## Scans, images & OCR

- **Scans are witnesses.** A woodblock or manuscript image is
  evidence for the e-text, never the reverse; the Four-Layer view
  (scan · OCR · e-text · English) keeps the layers honest and
  aligned.
- **Link by durable identifier**: BDRC work and image-group IDs
  (note: image URLs carry image-group ids, not work ids — cache
  per work).
- **OCR output is provisional by definition** — it enters the
  corpus only through review, and OCR text is never presented as
  the e-text without saying so.
- **THL's scanning standard** (adopted here as the reference for
  any scanning we do or receive): uncompressed TIFF masters at
  300–600 dpi, one image per folio side; JPEG derivatives around
  1200–1600 px for reading copies; and two separate archival
  copies of the masters, kept apart.
- **Folio arithmetic is checkable**: THL's Degé cataloging form
  works in vol:page.line references (e.g. `011:378a.5`) with
  line-fraction rules per 7- and 8-line pages, and names the
  canonical front matter (title page, title line, translator's
  homage, opening verses, the author's pledge) — a ready
  checklist when validating a scanned volume against its e-text.
- *Source: THL Toolbox scanning/photography specs + Degé deep
  cataloging form (docs/research/THL/toolbox_standards.md).*

## Text ↔ scan synchronization (protocol)

- **The markers are the authority.** A text's position on the
  woodblock is derived from its own @folio markers and the
  preserved line breaks (one typed line per carved line — the
  input centers' convention). Never re-derive folio or line from
  OCR when markers exist; never edit markers to "fix" a mismatch
  without checking the scan — the marker is usually right.
- **Folio → image comes from the archive's catalog.** BDRC's IIIF
  manifests label each image with its folio side; the app maps
  through those labels and never assumes image order equals folio
  order (title pages and inserts break that assumption).
- **OCR locates; it never testifies.** Word boxes may be used to
  find text on a carving. Recognized text is review material only
  and is never displayed or exported as the text itself.
- **Downloads are folio-named** (`094a.jpg`) into
  `library/scans/<work>/` — one store serving both reading
  (follow-along, viewer) and keying (Input block). Keep it; it is
  the offline ground truth for the volume.
- **Folio-faithful exports** (one file per side, `001a.txt`) are
  derived from markers and therefore safe as keying and
  OCR-comparison targets. Regenerate them after edits; never edit
  the per-side files directly (the document is the source).
- **Attribution**: BDRC scan imagery is used with its own license
  terms (shown in the viewer's ⓘ); public-domain volumes are
  labeled as such. Thumbnails are fetched at reduced size, one
  request at a time — be a polite guest on the archive's servers.

## Catalogs & provenance

- **One text, many witnesses**: catalog identity (ACIP number,
  BDRC work ID, edition sigla like Derge/Lhasa numbers) is kept
  distinct from any single copy of the text.
- **Tier labels are honest**: curated > glossary > auto-aligned
  provisional; provisional data must look provisional everywhere
  it appears.
- **The binding layer is separable**: Geshe Michael's English
  (hgm_gloss) is binding; Hopkins, Lokesh Chandra, 84000 and all
  other layers are reference comparanda, each under its own
  license lane (ingest / link-out / never ship), each attributed.
- **AI never touches Geshe Michael's words.** Machine work may
  MATCH his published English from corpus evidence, never COMPOSE
  or edit it. AI-drafted reference entries live in their own tier,
  under an unmistakable banner, suppressed wherever he speaks.

What THL's catalogs add to the reference picture:

- **THL's canon catalogs** (alive at old.thlib.org) cover the
  Kangyur in six editions (Degé, Choné, Nartang, Peking, Lhasa,
  Urga) and the Tengyur in four, each searchable in Wylie,
  Unicode, and English — plus "Master" concordances aligning a
  text across editions, the only free multi-edition concordance
  we know of. IDs read sigla.number (e.g. `dg.0043`). View-only:
  link out, don't ingest.
- **Genre classification**: THL's Tibetan Literary Genres map
  (~200 nodes — Biography, Philosophical & Doctrinal with its
  sixteen commentary types, Ritual with ~28 text types, and so
  on) is captured in full in our research notes and is the best
  available taxonomy when tagging what kind of text a work is.
- **Places and subjects**: THL's gazetteer (~64,000 Tibetan
  places with script + Wylie names) answers live JSON queries —
  the licensing lane is *query and link out*, no bulk ingest.
- **License lanes are per-resource and never assumed**: THL's
  site-wide community license does not cover third-party media;
  everything THL is link-out until a specific resource's terms
  prove otherwise.
- *Source: docs/research/THL/collections_catalogs.md +
  knowledge_infrastructure.md.*

## Audio & teachings

- **Teachings are a citation layer**: the teaching index maps
  dictionary terms to spoken moments (243k+ moments); a moment is
  a pointer to the recording, never a substitute for it.
- **Spoken Tibetan in teachings follows the GMR convention** for
  search (pronunciation-folded lookup), with the same
  never-guess rule: unmatched spoken forms stay unmatched.
- **Auto-captions are machine-derived and labeled so** wherever
  quoted; the timecoded link to the original recording is always
  the authority. YouTube captions notoriously corrupt Tibetan
  terms — the re-transcription program (see
  docs/research/TRANSCRIBER_SURVEY.md) exists to replace them
  with lexicon-biased transcripts, and even those enter the index
  as evidence, not as text of record.
