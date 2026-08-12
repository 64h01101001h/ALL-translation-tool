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
