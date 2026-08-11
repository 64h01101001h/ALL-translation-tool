# ALL Translation Tool — Release Notes
## Build of 2026-08-11 · data release v0.27.2

This build collects the August 2026 work. Everything below is in the
DMG (`dist/ALL-Translation-Tool-0.27.2.dmg`) and verified by the
automated batteries: 38 ctest suites plus the app's own 80-check
`--selftest`.

## Reading the way Geshe Michael asked

- **Click-to-longest-phrase reading mode** (Geshe Michael's directive,
  relayed from Adam's class of 2026-08-10). The Overlay now opens
  clean, with no shading. Click any word and the LONGEST phrase the
  dictionary knows at that point highlights (gold band; amber-leaning
  when the entry is provisional); click again to step down phrase by
  phrase — dropping a particle at a time — to the single word. The
  back-off rides the proven match lattice, not new guessing. The old
  everything-shaded coverage view remains one click away as the
  "all matches (wash)" toggle in the Display panel.
- **Congruent line breaks.** The Overlay display now mirrors the
  source file's own line structure instead of reflowing it, so the
  screen matches the text as keyed.
- **Night mode fixed for reading surfaces.** Pages that show Tibetan
  text stay manuscript-cream regardless of the system appearance;
  the rest of the app follows night mode as before.

## Getting around

- **Six-workflow navigation.** The panes are grouped into six workflow
  sections with sectioned buttons, replacing the long flat tab row.
- **Mirrored menu bar.** Every pane's functionality is reachable from
  the menu bar dropdowns, matching the GUI grouping, plus a
  "Where do I start?" opener.
- **Settings dialog** — the app's options collected in one place.
- **Searchable Help + Suggested Workflows browser.** Help now has a
  search field and a tutorial for every pane; the Suggested Workflows
  chapter opens as its own window listing every workflow.

## New work surfaces

- **Manuscript pane** (task #56): a rich-text writing surface with a
  Gofer search sidebar, so drafting and corpus search sit side by
  side.
- **Spelling-doubts list** (Adam's request): a spell-checker
  independent of the red underline. The Overlay's REVIEW section
  collects potential input errors as a working list that files into
  the Approval queue as `spelling` proposals — the app never edits
  texts; the authority (GMR) rules. Doubts skip @-folio markers
  (structure, not spelling) and any form the dictionary itself
  attests, and forms the authority has ruled valid (declined flags)
  are excluded from future lists — an allowlist built from rulings.
- **Idioms register** (task #60): a banked idiom list, an `idiom`
  proposal kind for the channel, and idiom badges on entry cards.

## Packaging and the paper trail

- **OCR models bundled in the DMG** (BDRC, CC BY-NC 4.0, used with
  BDRC's permission, credited wherever output appears). Earlier
  packages omitted them and showed the download-it-yourself fallback;
  the packaging script now stages `library/ocr_models` every time.
- **Open-source & data notices ship with every release** —
  `OPEN_SOURCE_NOTICES.md` is staged into the DMG alongside the app.
- **Launch-test hardening.** The packaging gate launch-tests the
  staged layout up to three times and records exit codes; a
  lifecycle log (`~/Library/Logs/ALLTranslationTool-lifecycle.log`)
  now makes any silent startup death explain itself.

## Verification in this build

- 38 ctest suites green (engine ports at full-dictionary parity
  against their Python oracles, data layers, stores, mutations).
- 80 app selftest checks green (`./ALLTranslationTool --selftest`):
  the real panes constructed against the real spine, exercised
  offscreen, assertions on their actual state.

## Known limits

- The AI Analysis sections and BDRC scan viewing need a network;
  everything else runs offline.
- Ad-hoc signature only: first launch is right-click → Open (no
  Apple Developer ID yet).
- Day-level Tibetan calendar is Modern Karana, epoch 2009 onward.
- Live composing editor and Old-Tibetan affix rules are not built.
- Externally gated (waiting on other people, not code): 84000
  glossary/TM licensing, input-center formatter samples, Mixed Nuts
  style guide, the catalog-English tier ruling, Steinert license
  review, Lotsawa House bulk permission, BDRC's written OCR
  confirmation. Full register: `docs/FEATURE_STATUS.md`.


## Added after the initial notes (August 11)

- **Teaching moments**: entry cards link to Geshe Michael teaching
  the term in recorded classes — timecoded, machine-located from
  class captions (Diamond Cutter Classics channel, harvested with
  authorization), with a spoken-Tibetan tier matched on his own
  pronunciation convention. "Teachings for this text…" (Overlay →
  REVIEW) reports every taught term in a loaded text.
- **Das 1902 in-app**: every card links to Sarat Chandra Das's
  dictionary, opening inside the app at approximately the entry's
  page (from the team's own bookmarked PDF; public domain).
- **Woodblock illustration gallery**: single-folio overlay, whole-
  folder gallery with crop export, and cached-volume search from
  the Overlay.
- **Idioms register**: proposed/approved idiom badges on cards;
  idiom proposal kind; Approval filter.
- Spelling doubts refined (folio markers and dictionary-attested
  forms excluded); full-library open sweep recorded (2,716/2,716);
  self-test now 84 checks.
