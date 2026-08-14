# ALL Translation Tool — Release Notes
## Build of 2026-08-12 · app v0.9.0 · data release v0.27.2 (corpus v32)

Everything below is in the DMG and verified by the automated
batteries: 38 ctest suites plus the app's 96-check `--selftest`,
and — new this build — a mechanized click-through of every control
on every pane (17/17 panes crash-free).

## The written word, finally complete

- **The User Manual.** ~18,000 words, every pane, every control,
  click by click, with worked examples — inside the app (Help →
  User Manual…), in the distribution folder as a Word document,
  and searchable beside everything else in the Help window.
- **Help opens ready.** The Help window's left rail now lists the
  whole tool the moment it opens: every tutorial chapter, then
  every control grouped under its pane — so a feature can be found
  by scrolling even when its name is unknown.
- **Help → Troubleshooting.** Show Logs / Show Data Folder in
  Finder, Copy Installation Info, Generate Diagnostic Report… (a
  full health report on your Desktop — attach it to any bug
  report), Report a Problem… (pre-addressed email, template filled
  in), Storage… (cache sizes with safe clears), Verbose Logging,
  Reset Settings….

## Authors are people now

- Selecting a text in the Library names its author with links out
  to the Buddhist Digital Resource Center person record and a
  Treasury of Lives biography where one exists — and the author's
  other works in this Library are one click away. 184 of the 265
  catalog authors are resolved across three deterministic passes;
  every candidate is verified against BDRC's own labels, homonyms
  are all kept, and unresolved names are listed, never guessed.

## The teachings, linked

- Dictionary term cards link the moments Geshe Michael taught that
  term — timecoded into the original recordings of his own courses
  (Diamond Cutter Classics and the lam rim archive), with a second
  tier for hearing the spoken word itself. The recordings remain
  the authority; the app only opens them. A one-click report lists
  the teachings for the whole text you are reading.
- Chandra Das's classic dictionary opens inside the app at the
  right page for the word you looked up.

## Reading and scans

- **Session restore** — the app reopens exactly where you left
  off: same text, same scroll, same cursor.
- **Instant display toggles** — the Overlay's display checkboxes
  re-render in place without rebuilding the document or losing
  your card.
- **Whole-volume illustration gallery** — every folio side of a
  linked woodblock volume fetched (reduced size, cached, with
  progress and Stop) and searched for miniatures and diagrams;
  the cached-pages gallery now actually finds its cache (a
  never-could-match filter was found and fixed this build).

## The morning's three (added after the first press)

- **The Evidence Ribbon** — in Draft, the evidence follows your
  cursor: click into any clause and its terms, a scaffold of the
  master's attested glosses in Wilson reading order (nothing ever
  composed), "the master has translated this clause," and any
  quotation alert arrive with zero clicks.
- **Hunt Everywhere (⌘K)** — one palette, every source: wylie,
  ACIP, English, or phonetics in; dictionary, reverse index,
  phonetics fold, corpus, and teachings out; Enter jumps to the
  right pane with the query already run.
- **Four-layer view** — the folio you are reading as scan · OCR ·
  e-text · attested English, row-aligned, click-to-band on the
  carving.

## Search

- **This Mac (Spotlight)** is now an opt-in source row in the
  Search pane's own target list: each term searched everywhere
  macOS indexes, as typed and as Tibetan script; results open in
  their own application. (This closes the one known unreachable
  feature in the shipped app.)

## Fixes from the testing campaign

- Display toggles no longer rebuild the document (and no longer
  wipe the open card).
- Library links now survive apostrophes in Tibetan filenames and
  spaces in the install path.
- Every pane fits an 800-pixel window (enforced by a permanent
  self-test guard).
- Spelling doubts are raised only where dictionary coverage cannot
  attest the form, and never on folio markers.

## For the record

- The white paper, technical paper, feature guide, and this
  document were truth-passed against the shipped build; the
  testing campaign checklist and findings log ship in the
  distribution folder.

---

# Addendum — the August 12–13 wave

Since the notes above, eighteen further pressed builds landed. The
highlights, feature by feature:

## Reading & the dictionary
- **⌘D — the floating dictionary window.** Select a word or phrase
  in any pane and press ⌘D: the full lookup stack answers in a
  floating card that follows its own cross-reference links.
- **Entry cards redesigned** into a deliberate typographic order:
  identity (Tibetan headline with the ACIP form beside it, plus a
  "from:" note when you clicked an inflected form), pronunciation,
  the binding HGM layer, published apparatus, labeled reference
  layers, and teaching links — consistent spacing throughout.
- **Pronunciation rulings, everywhere.** The authority's approved
  pronunciations now override the engine across every surface —
  whole-text display, entry cards (green ⟪ruled⟫ tag), and search —
  including phrase rulings that span the engine's own word
  boundaries (tshad ma rnam 'grel = "tsema namdrel"), and rulings
  embedded inside longer headwords.
- **Pronunciation (THL)** display mode: the THL Simplified
  Phonetic Transcription (Germano & Tournadre), implemented
  rule-for-rule from the published standard and proven against the
  standard's own complete example battery (139/139). The Convert
  pane shows both systems side by side.
- **Old-Tibetan spellings meet their classical forms** in every
  search (the archaic da-drag: gyurd finds gyur).

## The Pecha Maker, productionized
- Ornamental title folio (text begins on 1b), margin title
  (ya-yig) and volume letter, head marks, three rule weights.
- The Degé woodblock measure (68 × 10 cm) with the classical
  opening — first two text sides at five lines, larger letters.
- Two-up imposition on A4/US Letter with cut marks; an optional
  cover/label sheet; batch mode (a whole folder becomes a pecha
  set); and verse lineation (the poet's lines survive, blank
  lines become stanza gaps).
- An English interlinear that appears ONLY where the aligned
  corpus attests the exact segment — matched verbatim from
  published translation, never composed, proven end-to-end by a
  build-time self-test.

## Scholarship tools
- **Edition collation** (Align → Compare editions…): two witnesses
  of a text, every variant shown in a colored collation with a
  numbered, saveable apparatus. Nothing auto-resolved.
- **Search locations, TibetDoc-style**: folder search results roll
  up per file with hit counts; clicking opens the text in the
  Overlay at the first hit.
- **THL Places** joined the link-out row (~64,000 Tibetan places).

## The library, self-updating
- **Check for collection updates** (Library → Maintenance): the
  app asks asianlegacylibrary.org for the official releases,
  compares fingerprints with what is installed, and downloads and
  installs in place — with confirmation and progress, never
  touching your own materials.

## The dictionary program
- The **ALL Working Glossary** grew to 1,395 AI-drafted deep
  entries (senses, context, part of speech from hand-tagged
  evidence only, resolving cross-references) — shown only where
  Geshe Michael is silent, violet-bannered, and now with a
  **promotion lane**: review a draft, edit it, and file it to the
  Approval queue as your own proposal with full provenance.
- **The wall, enforced in code**: every machine-produced English
  is screened against the complete corpus of Geshe Michael's
  published English before it is kept. The screen's first audit
  proved all existing drafts clean — zero reproductions.
- The **84000 glossary** (24,576 terms, CC BY) as a toggleable
  layer; the in-app **Guidelines & Protocols** chapters distilling
  THL's published standards for print, digitization, scanning,
  and cataloging.

## The teaching archive
- The index grew to **263,188 timecoded moments** across 2,812
  recordings — including 554 caption files recovered from
  unlisted and forgotten uploads found through the archival
  program's link-probe.

## Text ↔ woodblock (the August 13 wave)

- **Jump from text to carving.** Right-click in any loaded text:
  the menu names your exact position (folio 94a · line 3/7, read
  from the text's own markers and preserved lineation — never a
  guess) and opens that folio in the reading panel, the Input
  workflow, or the full scan viewer with your selected words boxed
  on the woodblock itself (OCR used only to locate; misses named).
- **The scan viewer**, modeled on BDRC's BUDA viewer: thumbnail
  rail (self-completing from small preview images), dark canvas,
  click-to-zoom, grab-pan, pinch/⌘-scroll zoom, go-to-any-image,
  full screen, display-only brightness/contrast/invert, and a
  whole-work scroll view with an e-text toggle that lays each
  side's own keying beneath its image.
- **One offline store** joins reading and keying: download a
  volume once (folio-named files) — follow-along works offline and
  the Input workflow opens the same folder in folio order. A
  folio-faithful export writes one text file per woodblock side to
  sit beside the images.
- **In the Input workflow**: locate typed selections on the scan
  (your line searched first), jump to a typed folio's page, zoom
  hotkeys with anchored ⌘-scroll, a Recent-scans menu, and icons
  on the primary actions.

## Under the hood
- The authority's queue gained **Approve all in filter** for
  machine-seeded batches; the design audit closed every clipped
  label and unexplained empty surface; the whole application was
  re-certified by a 20-pane mechanized control sweep after the
  wave — zero anomalies, 39/39 engine test suites green.

---

# 0.10.0 — August 13, 2026 (the verification wave)

Versioning policy from here on (adopted 2026-08-13): the VERSION
file is the single source of truth; the minor number increments
with each shipped feature wave, the patch number for fix-only
presses, and 1.0.0 arrives with the first team installation.

Highlights since 0.9.0: the text↔woodblock jump and BUDA-style
scan viewer; the Lhasa scan-routing root-cause fix with
double-source verification (147/147); confirmed per-text author
links from the ACIP↔BDRC catalog; card typography v2 + Palatino
throughout; the Sanskrit-mantra programme (82,834-syllable census,
render guard); measured pecha presets + the 54-syllable authentic
type autofit; eKangyur + eTengyur acquired, indexed, and
collatable in one click (4,439 texts); pronunciation rulings
(tsema namdrel gyi tsik le'ur jepa; 'a-chung class); Input zoom,
recents, and locate lanes.
