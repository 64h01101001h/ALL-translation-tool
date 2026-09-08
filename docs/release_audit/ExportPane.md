# Release audit — `ExportPane` (app/main.cpp:15872–16021)

## 1. What it is and how it is reached

`ExportPane` is the publishing station — "FORMAT & EXPORT promoted to its own Read-group
pane" (Adam, 2026-08-12; comment at main.cpp:15869). It is a plain `QWidget` built with a
`RibbonBar` group named **PUBLISH** carrying four big buttons, a gold "working document:"
label, and a `QTextBrowser` results area. It is constructed in `main()` at
app/main.cpp:15854 (`new ExportPane(overlay)`) and added as `tabs.addTab(exportPane,
"Export")` (main.cpp:15855), immediately after the Scans tab. The user reaches it by
clicking the **Export** tab. Three of its four actions operate on the document loaded in
the Overlay (Read → Overlay, or a Library double-click) and are refused by `guard()`
when nothing is loaded; the batch lane deliberately bypasses the guard because it reads a
folder instead. `showEvent` refreshes the document label on every raise.

## 2. User-visible functions

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Banner label | Static HTML: what the pane publishes and that a text must be loaded in Read → Overlay first | ctor main.cpp:15877–15882 | NONE | Non-interactive. |
| "working document:" label (gold `ux::chromeGold()`, `#9A7A33` night-audited) | Names the loaded file, or says "working document: none loaded" | `refresh()` main.cpp:16003–16016 | `"the document label tells the truth about what is loaded"` (`check(...)`, main.cpp:15975–15979) | This assertion replaced a tautological one (STATIC-5) and immediately caught a real bug: `documentLabel()` kept a stale filename after clearing (fixed at main.cpp:12654). |
| Ribbon button **"Prepare for translation (Mixed Nuts)…"** | `guard()` then `overlay_->prepareForTranslation()` — paragraphs at double shads, bracket notes folio-tagged, saved as a prep text; HTML report into the results view | ctor 15932–15935; `prepareForTranslation()` main.cpp:12694 | `"the dialog seam drives a real prep save end to end (FAIL-3's first consumer)"` (main.cpp:5989, OverlayPane selfTest via the `g_saveDialogStub` seam) | Opens a native save panel (stubbed in tests via `g_saveDialogStub`; `safeGetSaveFileName`). **Writes a file to a user-chosen path.** Success line gated on a verified write. |
| Ribbon button **"Make pecha (PDF)…"** | `guard()` then `overlay_->pechaExport()` — long-format pecha PDF: framed folio sides, rotated Tibetan folio numerals, chosen typeface, optional phonetics interlinear, preview before saving | ctor 15936–15939; `pechaExport()` main.cpp:13768 | `"Pecha maker v2: title folio + ya-yig + …"` (main.cpp:5189), `"pecha B13: the export report names the …"` (5268), `"Pecha maker: English interlinear is …"` (5327), `"Pecha verse lineation renders (lines + …)"` (5347) — all in OverlayPane's selfTest, none through this button | Native save panel + **PDF write to disk**. Reads a large body of `pecha/*` QSettings (page, rules, layout, interlinears, cover sheet). Font-dependent (Tibetan shaping); `font_smoke` (app/CMakeLists.txt:83) covers fonts separately. |
| Ribbon button **"Batch: folder of texts → pecha set…"** | **No guard** (deliberate, commented): `overlay_->pechaBatch()` — picks an input folder and an output folder, turns every `*.txt/*.act/*.acip` into its own pecha PDF using the saved Make-pecha options; per-text dress stays off | ctor 15940–15946; `pechaBatch()` main.cpp:14276; `pechaBatchDirs()` main.cpp:14185 | `"Pecha batch: folder of ACIP + wylie texts → 3 pecha PDFs"` (main.cpp:5387) and the B13 layout-clamp block (main.cpp:5446–5470, asserts saddle-stitch/three-up/label-sheet reporting) | **Two native folder pickers**; short-circuits under `g_sweepActive` with an explanatory italic (main.cpp:14277–14281). Writes N PDFs into a user-chosen folder — no overwrite prompt visible at this level. An empty return renders nothing at all (user cancel is silent, which is correct but indistinguishable from a no-op failure). The B13 selftest **writes real user QSettings keys** (`pecha/layout`, `pecha/coverSheet`) and restores them — a crash mid-test leaves the translator's settings changed. |
| Ribbon button **"Export print Tibetan (Unicode)…"** | `guard()` then `overlay_->exportPrintTibetan()` — `allcore::exportTibetanUnicode`, classical shad rules, unconvertible syllables flagged `⟨wylie⟩` and never guessed; reports syllable and failure counts | ctor 15947–15950; `exportPrintTibetan()` main.cpp:12776 | Engine-level: ctest suite **`tibexport_smoke`** (core/CMakeLists.txt:236). No `check(...)` drives this button or this method | Native save panel + file write through `saveOrWarn`; on failure prints an explicit "**Not saved.**… Nothing on disk has changed." Empty input returns an empty QString ⇒ `setHtml("")` blanks the results pane with no explanation. |
| Every button's tooltip | The long description lives on hover, not on screen (`b->setToolTip(what)`) | `mk` lambda main.cpp:15898–15904 | NONE | Accessibility/discoverability risk: the ribbon shows only a short title; the entire explanation is hover-only, unreachable by keyboard-only and touch users. |
| Results view (`QTextBrowser`) | Shows the HTML report of the last action; starts with an italic "Results appear here…" placeholder | ctor 15925–15931 | `"a refused export explains itself on the pane"` (main.cpp:15984–15986, only asserted when no document is loaded) | Links in the reports are NOT intercepted here (no `anchorClicked` connect) — default `QTextBrowser` link handling applies. |
| Raising the tab (showEvent) | Re-reads the Overlay's document label | `showEvent` main.cpp:15991–15994 | Exercised via `selfTest`'s `refresh()` | Results view is not cleared on raise — a stale report can sit under a newly-changed document label. |

No checkboxes, combos, line edits, shortcuts, context menus, drag/drop, double-click or
anchor schemes exist in this class.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ExportPane(OverlayPane*)` (explicit) | 15874 | Builds banner, doc label, the PUBLISH `RibbonBar` group with four buttons, the results browser; wires the four click handlers | `main()` main.cpp:15854 |
| `mk` (ctor-local lambda) | 15898 | Makes a ribbon button, puts the description in its tooltip, adds it to the PUBLISH group with an icon (default `"page"`) | ctor, ×4 |
| `selfTest(QStringList&)` | 15953 | Three falsifiable assertions (label truth, guard admits iff a document is loaded, refusal explains itself); returns fail count | `main()` selftest driver, main.cpp:38254 (`fails += exportPane->selfTest(log);`) |
| `check` (selfTest-local lambda) | 15959–15964 | Logs `  [PASS/FAIL] Export: <what>` and counts failures | `selfTest` ×3 |
| `showEvent(QShowEvent*)` | 15991 | Refresh-on-raise | Qt |
| `guard()` | 15997 | Refreshes, returns true iff `overlay_ && overlay_->hasDocument()`; otherwise writes the "No document loaded" italic into the results view and returns false | three of the four button handlers; `selfTest` |
| `refresh()` | 16003 | Sets the doc label; explicitly handles the null-overlay case ("none loaded") — a bug STATIC-5's new assertion caught (blank label) | ctor path via `selfTest`, `showEvent`, `guard` |
| `overlay_`, `doc_`, `results_` (members) | 16018–16020 | Non-owning Overlay pointer; the gold label; the report browser | throughout |

## 4. Dependencies

- **Owns no data.** All work is delegated to `OverlayPane`; the pane reads
  `hasDocument()` / `documentLabel()` (main.cpp:12651, 12654).
- **Files read (via Overlay)**: the loaded document; for batch, every `*.txt`, `*.act`,
  `*.acip` in the chosen input folder.
- **Files written (via Overlay)**: prep text (user-chosen path), pecha PDF (user-chosen
  path), N pecha PDFs into the chosen batch output folder, print-Tibetan `.txt`
  (`tibetan_unicode.txt` default). Writes go through `saveOrWarn` / `writeAllOrWarn`
  (main.cpp:~2870/~3060), which verify byte counts and flush and delete truncated
  remnants.
- **Network**: none.
- **QSettings**: none read/written directly by this class. Via `pechaOptsFromSettings()`
  the pecha lanes read `ALL/TranslationTool` → `pecha/layout`, `pecha/coverSheet` and the
  rest of the `pecha/*` family (see main.cpp:5446–5470). No `sess::` key is registered —
  the pane does not remember the last export folder or format.
- **Fonts**: the pecha PDF depends on the bundled Tibetan typeface (`font_smoke`,
  app/CMakeLists.txt:83).
- **ctest**: `app_selftest` (app/CMakeLists.txt:89) runs `ExportPane::selfTest` and the
  OverlayPane pecha/prep batteries; `tibexport_smoke` (core/CMakeLists.txt:236) covers the
  Unicode export engine.

## 5. Release questions

- **No button in this pane is driven by a test through the button.** All four are only
  covered at the Overlay-method level (and "print Tibetan" only at engine level). Decide
  whether that is acceptable, or add click-level coverage.
- `exportPrintTibetan()` returns an empty string on empty input, blanking the results
  pane silently. Should that say "nothing to export"?
- The batch lane writes N PDFs into a chosen folder: confirm the overwrite policy (does
  it clobber an existing `*.pecha.pdf` without asking?) and what happens when one text in
  the folder fails to parse.
- The B13 selftest mutates real `pecha/layout` and `pecha/coverSheet` user settings and
  restores them at the end. Decide whether a probe may touch live settings at all — the
  `g_harnessRun` rule in `sess::` exists precisely to forbid this elsewhere.
- All four descriptions are tooltip-only. Confirm against the accessibility audit
  (WCAG 2.2, hover-only content) before release.
- `guard()` refuses on an empty document, but "empty" is `input_->toPlainText().trimmed()`
  — a whitespace-only document reads as "none loaded" while its filename is still on
  screen. Confirm that is the intended message.
- Manually verify each export on a read-only volume / full disk: the pane must print the
  "Not saved" line, not a success count (this was bounty #6 / DATA-4).
- Decide whether the results view should be cleared when the loaded document changes.
- Confirm the four ribbon icons (all default `"page"` except as passed) are the intended
  art for release.
