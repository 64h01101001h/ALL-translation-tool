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
- **Printing protocol**: true-size folio sheets go to a print
  shop; for office printers, impose two-up on A4/US Letter with
  corner cut marks and trim. Partner printing (Vimala Treasures)
  takes the native-size PDF.
- **Script integrity in print is absolute**: every syllable passes
  the battery-proven conversion chain; a syllable that cannot be
  proven prints as ⟨wylie⟩ — flagged, never guessed.
- *THL's print-formatting guidance (THL Toolbox) merges here as
  the survey completes — including their specifications for
  formatting Tibetan texts for publication.*

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
- *THL Toolbox digitization standards (typing, proofreading,
  structural markup, TEI/XML practice) merge here as the survey
  completes.*

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
- *The full published EWTS specification (THL reference) is being
  captured by the survey and will be summarized here with its
  source pages.*

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
  additional display mode — port-and-prove from THL's published
  rules, with THL's own converter as oracle, never approximated.
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
- *THL imaging/scanning standards merge here as the survey
  completes.*

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
- *THL cataloging standards and their canon catalogs (Kangyur/
  Tengyur numbering) merge here as the survey completes.*

## Audio & teachings

- **Teachings are a citation layer**: the teaching index maps
  dictionary terms to spoken moments (243k+ moments); a moment is
  a pointer to the recording, never a substitute for it.
- **Spoken Tibetan in teachings follows the GMR convention** for
  search (pronunciation-folded lookup), with the same
  never-guess rule: unmatched spoken forms stay unmatched.
- *THL's audio-video archive practice (annotation, transcript
  tooling) merges here as the survey completes.*
