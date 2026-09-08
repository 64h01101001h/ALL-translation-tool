# InputPane — release inventory

Source: `app/main.cpp` lines 27310–28892 (class `InputPane`). Next class `ScanPane` begins at 28893.
Audit date: 2026-09-08. Read exhaustively; no source file modified.

## 1. What the pane is

The Input pane is the input-center transcription workstation — the revival of ACE
(Aaron Cram's `acip-support` design, Apache-2.0) inside the Diamond Cutter tool. An operator
opens a page scan (or a whole folder of page scans, the "block" workflow), types the ACIP
transliteration into the editor below the image, and the scan auto-scrolls to follow the
typing cursor (proportional ACE estimate, or exact OCR line bands when the BDRC PhotiLines
model is installed). The pane also runs the double-keying correction pass: it diffs the
operator's text against a partner keyer's file and highlights every discrepancy region.
Live ACIP syllable-legality spellchecking underlines doubtful syllables. It sits as the
top-level tab named **"Input"**, added in `main()` at `app/main.cpp:35958–35959`
(after Align, before Library). There is no global keyboard shortcut to reach the tab;
other panes route into it programmatically via the `g_openScanInInput` hook (35960) and
the legacy-rescue `g_openTextInInput` hook (installed at ~27735). Its only menu-bar
integration is View ▸ Larger/Smaller/Actual Text, which routes to `scanZoomRouted()`
when focus is inside this pane (`app/main.cpp:37202–37210`).

## 2. User-visible functions

Coverage column names the exact `check(...)` string inside `InputPane::selfTest`
(starts line 27760) that exercises the control, or another suite, or NONE.

| Control/action | What it does | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Ribbon PAGES ▸ "Open scan…" button | File dialog for PNG/JPG/TIFF under `<root>/library`; loads it as the current scan | ctor connect 27594 → `openScan()` 27890 | NONE (only `gauntlet_walk` random clicking; dialog is stubbed away) | Dialog goes through `safeGetOpenFileName`; harness-safe |
| Ribbon PAGES ▸ "Open scan folder…" button | Directory picker; loads every image in the folder as an ordered page block, creates `library/input_work/<folder>/`, jumps to page 1 | ctor connect 27443 → `openFolder()` 28430 / `openFolderPath()` 28436 | Indirectly via the main() session test "Session: the Input pane resumes its working folder AND page…" (38700) | Writes to disk: `mkpath(workDir_)`. Non-recursive; ignores sub-folders silently |
| Ribbon PAGES ▸ "Recent ▾" menu button | Lazy-built menu of the last 10 opened scans/folders from QSettings; entry click reopens file or folder | ctor lambda 27351–27379 | NONE | Non-existent paths are silently skipped; shows "(nothing opened yet)" disabled item when empty |
| Recent menu item (per entry) | `openFolderPath(p)` if dir, else `openScanPath(p)` | 27371–27376 | NONE | — |
| Ribbon PAGES ▸ "◀ page" button | Previous page of the block (saves the leaving page first) | 27444 → `gotoPage(pageIx_-1)` 28489 | NONE directly; session test covers `gotoPage` | Disabled until a folder is open |
| Ribbon PAGES ▸ "page ▶" button | Next page of the block | 27446 → `gotoPage(pageIx_+1)` 28489 | NONE directly | Disabled at last page |
| Page label ("page N/M") | Read-only status of position in the block | set in `gotoPage` 28524 | NONE | — |
| Folio strip (thumbnail list) — click a thumbnail | Jumps to that page (`gotoPage(row)`) | connect 27404 → 28489 | NONE | Hidden until a folder is opened; ✓ marks pages with saved typing |
| Folio strip lazy thumbnail render | Background QTimer fills 8 thumbnails per 30 ms tick | 27409–27423 | NONE | `QPixmap(path)` per page; null pixmaps silently skipped (page shows no image, no message) |
| Ribbon TYPING ▸ "Predictive typing" checkbox | Toggles ACIP completion from spine headwords; persisted to QSettings `input/predict`; builds the inventory on first enable | 27424–27441, `buildPredict()` 27735 | `"predictive typing completes BSOD NAM from the dictionary"` | Builds a full set of all spine ACIP headwords in memory on toggle; one-shot (`predictBuilt_`), never rebuilt if the spine changes |
| Completion popup accept (Tab/Enter/click) | Replaces the typed prefix with the completion + a trailing space | connect 27686–27695 | `"predictive typing completes BSOD NAM from the dictionary"` (prefix/count only, not the insertion) | Insert path itself untested |
| Typing in the editor (any keystroke) | Fires `spellcheck()` + `predict()` on every `textChanged` | connect 27600 | NONE for spellcheck | Full-document rescan per keystroke; guarded at >200 000 chars with a "live spellcheck paused" message — that guard is untested |
| Cursor move in the editor | `follow()` scrolls the scan to the matching line/column | connect 27598 | NONE | Proportional mode is approximate by design |
| Ribbon VIEW ▸ "scan follows cursor" checkbox | Enables/disables the follow behaviour | 27448–27450 | NONE | Not persisted — resets to on each launch |
| Ribbon VIEW ▸ "−" zoom-out button | `zoomStep(-25)`, center-anchored | 27528 → 28644 | NONE | — |
| Ribbon VIEW ▸ zoom slider (25–400) | Sets zoom; re-renders; mirrors value into the % combo | 27509, connects 27516/27596 | NONE | Two `valueChanged` connections (label sync + `render()`) |
| Ribbon VIEW ▸ "+" zoom-in button | `zoomStep(+25)` | 27526 → 28644 | NONE | — |
| Ribbon VIEW ▸ zoom % combo (editable) | Preset or typed percentage; "Fit width"/"Fit page" call `fitZoom()` | 27488–27506 | NONE | Typed garbage parses to `0` via `toInt()` and is clamped to the slider minimum (25 %) — no error message |
| ⌘-scroll over the scan | Pointer-anchored zoom (±15), consumes the wheel event | `eventFilter()` 28676–28689 | NONE | Uses `Qt::ControlModifier` (⌘ on macOS with Qt's default mapping) |
| View menu ▸ Larger/Smaller/Actual Text (⌘+ / ⌘− / ⌘0) | When focus is inside this pane, steps or resets the scan zoom | `scanZoomRouted()` 27862; routed at 37202–37210 | NONE | Shared with the Overlay's text zoom; routing depends on focus-widget ancestry |
| Ribbon TYPING ▸ "@ next folio" button | Inserts the next ACIP folio marker (@001A → @001B → @002A), forcing column 1 | 27553 → `nextFolio()` 28546 | NONE | Scans the whole document each press; a malformed existing marker silently restarts numbering |
| Ribbon PARTNER ▸ "Compare with partner file…" | Suggests the same-stem file beside the scan, opens a file dialog, diffs it against the editor and highlights discrepancies | 27595 → `compare()` 28322, `compareWith()` 28348 | `"identical double-keying reports clean"`, `"planted difference is flagged"`, `"a null partner is refused, not crashed (MEM-2)"` | Read failure warns via `warnOpenFail`. Historic ship-blocker MEM-2 (crash on null partner) is fixed and pinned. >500 k chars refused with a message |
| Ribbon PARTNER ▸ "◀ disc" / "disc ▶" | Steps the cursor through discrepancy regions, wrapping | 27544/27546 → `jumpDiff()` 28409 | NONE | Enabled only after a compare finds discrepancies |
| Ribbon OCR ▸ "Detect lines (OCR)" *(only if `ALL_HAVE_OCR`)* | Runs BDRC PhotiLines on the open scan, draws line bands, switches follow to exact bands | 27472 → `detectBands()` 27998 | NONE | Requires `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx`; missing model or exception → status text only. Compile-gated: absent in non-OCR builds |
| Ribbon OCR ▸ "Pre-fill from OCR (draft)" *(`ALL_HAVE_OCR`)* | Recognizes the page (BDRC Woodblock, CC BY-NC) and fills an EMPTY editor with an ACIP draft to correct | 27488 → `prefill()` 28058 | `"pre-fill refuses a non-empty editor (never overwrites)"` and `"pre-fill allowed on an empty page"` (guard only, via `canPrefill()`) | The recognition path itself is untested. Long blocking loop with `processEvents()` — no cancel. Model licence is CC BY-NC: a licensing decision for shipping |
| Editor right-click ▸ standard context menu | Qt's own cut/copy/paste/select-all | 27612 | NONE | — |
| Editor right-click ▸ "folio NNNx" header item | Disabled label naming the folio marker in effect at the cursor | 27655–27662 | NONE | — |
| Editor right-click ▸ "Jump to this folio's scan (page N)" | Matches the folio token against page filenames and jumps to that page | 27664–27686 | NONE | Only appears in folder mode and only when a filename matches |
| Editor right-click ▸ "Locate selection on the scan (OCR)" *(`ALL_HAVE_OCR`)* | OCRs the page with word boxes and paints red rectangles over the selected ACIP | 27626–27644 → `locateSelection()` 28128 | NONE | Heavy: recognizes every line. Failure → status text. Only shown when a scan is loaded and text is selected |
| Ribbon EXPORT ▸ "Save…" | Block mode: flushes the current page then exports all typed pages combined in order. Single mode: saves the editor to `library/input_work/<stem>.txt` | 27596 → `save()` 28583 | `"input work lands its bytes and says so (WP-2)"`, `"an unwritable input-work save says NOT SAVED instead of returning silently (WP-2)"` | Single-file path is verified (WP-2 fix). Combined-block path has its own `wroteOk`/"NOT COMBINED" check but is **not** covered by any test |
| Implicit: page-change autosave | `gotoPage()` writes the leaving page's text via `saveOrWarn` | 28494–28499 | NONE (session test exercises navigation, not save failure) | BOUNTY #6 note in code: a silent failure here loses typing the user believes is safe. Warn path untested |
| Implicit: session restore on launch | Reopens last scan or last folder+page | `restoreSession()` 27840; called 40553 | main() log line `"Session: the Input pane resumes its working folder AND page, and ignores a folder that has gone away"` (38700) | Covered |
| External entry: Scan pane "open in Input" | `g_openScanInInput` → `openScanPath` + raise tab | 35960–35963 | NONE | — |
| External entry: legacy-rescue text | `g_openTextInInput` loads a rescued Unicode file into the editor, asking before replacing existing typing | 27737–27758 | NONE | Read failure `return`s **silently** — no status text, no dialog; user sees nothing happen |

**No anchor-href / link-scheme handling exists in this pane** (no `linkActivated` slots), and no
`setShortcut()` calls are made inside the class — the only keyboard accelerators reaching it are the
View-menu zoom actions and Qt's editor defaults. There is no drag-and-drop handler.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `InputPane(checker, root, spine)` | 27312 | Builds the whole pane: banner, ribbon, folio strip, splitter, editor, completer, hooks | `main()` 35958 |
| `buildPredict()` | 27735 | Loads all spine ACIP headwords (plus first-word prefixes) into the completer model | predict toggle, ctor, `selfTest` |
| `predict()` | 27753 | Computes the current ACIP token and pops the completion list | editor `textChanged` |
| `selfTest(QStringList&)` | 27760 | The pane's own PASS/FAIL battery (7 checks) | `main()` 38269 (`app_selftest` ctest) |
| `restoreSession()` | 27840 | Reopens the last file or folder+page from `sess::` settings | `main()` 40553; session test 38682/38688 |
| `scanZoomRouted(int)` | 27862 | View-menu zoom routing (0 = reset to 100 %) | 37210 |
| `currentScanFile()` / `currentPage()` | 27867/27868 | Accessors for the session test | 38683–38690 |
| `recordRecentScan(QString)` | 27870 | Prepends to QSettings `input/recentScans`, capped at 10 | `openScanPath`, `openFolderPath` |
| `openScan()` | 27890 | File dialog wrapper | Open-scan button |
| `openScanPath(QString)` | 27897 | Loads the pixmap, records session/recents, renders, sets status | `openScan`, recents, `g_openScanInInput`, `restoreSession` |
| `bandNote()` | 27923 | One-phrase description of follow accuracy (exact bands vs proportional) | `openScanPath`, `gotoPage` |
| `render()` | 27931 | Scales the pixmap to the zoom and paints line bands + locate rects | zoom, page change, detect, locate |
| `detectBands()` *(OCR)* | 27998 | PhotiLines line detection into `bands_` | OCR button, `prefill`, `locateSelection` |
| `canPrefill()` | 28054 | Named guard: editor must be empty | `prefill`, `selfTest` |
| `prefill()` *(OCR)* | 28058 | Recognizes the page and fills the empty editor with an ACIP draft | Pre-fill button |
| `locateSelection()` *(OCR)* | 28128 | Finds the selected ACIP words on the scan and highlights their boxes | Context-menu action |
| `follow()` | 28217 | Scrolls the scan to the cursor's line/column (band or proportional) | `cursorPositionChanged` |
| `spellcheck()` | 28246 | Underlines syllables the `SyllableChecker` rejects; pauses over 200 k chars | `textChanged` |
| `suggestPartner()` | 28300 | Finds a single same-stem `.txt/.act/.inc` beside the scan to preselect | `compare()` |
| `compare()` | 28322 | Partner-file dialog + read, then `compareWith` | Compare button |
| `compareWith(QString)` | 28348 | The dialog-free double-keying diff and highlighting | `compare`, `selfTest` |
| `jumpDiff(int)` | 28409 | Moves the cursor to the next/previous discrepancy | ◀ disc / disc ▶ |
| `openFolder()` | 28430 | Directory dialog wrapper | Open-folder button |
| `openFolderPath(QString)` | 28436 | Enumerates pages, creates the work dir, builds the folio strip, goes to page 1 | `openFolder`, recents, `restoreSession` |
| `pageTextFile(int)` | 28481 | Path of a page's per-page typing file (`page_NNN_<stem>.txt`) | `gotoPage`, `save`, folio strip ✓ |
| `gotoPage(int)` | 28489 | Saves the leaving page, loads the new page image + text, updates strip and buttons | page buttons, thumbnails, folio jump, `save`, `restoreSession` |
| `nextFolio()` | 28546 | Inserts the next `@dddX` marker in column 1 | @ next folio button |
| `save()` | 28583 | Combined-block export or single-file save | Save button, `selfTest` |
| `zoomStep(int)` | 28644 | Center-anchored relative zoom | ± buttons, `scanZoomRouted` |
| `fitZoom(bool)` | 28649 | Fit-width / fit-page zoom computation | zoom % combo |
| `setZoomAnchored(int, QPoint)` | 28659 | Applies a zoom keeping a chosen viewport point fixed | `zoomStep`, `fitZoom`, combo, wheel |
| `eventFilter(QObject*, QEvent*)` | 28676 | ⌘-wheel pointer-anchored zoom over the scan viewport | Installed on `scroll_->viewport()` 27597 |

## 4. Dependencies

**Data folders / files read**
- `<root>/library` — default directory for both file dialogs.
- `<root>/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx` — line-detection model (existence checked; absent → status message).
- `ocrModelDirFor(root_)` + `/model_config.json` — recognition model directory for pre-fill.
- `<root>/library/ocr_models/BDRC_Woodblock/OCRModel.onnx` — recognition model for locate-selection (hard-coded path, distinct from `ocrModelDirFor`).
- Page images: `*.png *.jpg *.jpeg *.tif *.tiff` in the opened folder.
- Partner files: `*.txt *.act *.inc` (plus uppercase) beside the scan.
- The spine (`allcore::Spine::allAcipHeadwords()`) for predictive typing.

**Files written**
- `<root>/library/input_work/` (created by `save()`).
- `<root>/library/input_work/<folder>/page_NNN_<stem>.txt` — per-page typing, written on every page change.
- User-chosen `.txt` from the Save dialog (single file or combined block).

**Network** — none. No `QNetworkAccessManager`, no `TimedNam`, no `QProcess` anywhere in the class. All OCR is local.

**QSettings ("ALL", "TranslationTool")**
- `input/recentScans` — read (Recent menu) and written (`recordRecentScan`), max 10 entries.
- `input/predict` — read in the ctor, written on toggle.
- `sess/input/mode`, `sess/input/folder`, `sess/input/scan`, `sess/input/page` — via the `sess::` helpers, written on open/page-change and read by `restoreSession()`.
- `ui/ribbonLabels` — read indirectly by the shared `RibbonBar`.

## 5. Release questions

- **BDRC Woodblock model is CC BY-NC.** Pre-fill and locate-selection depend on it. Confirm the shipping licence/permission statement, or hide both OCR buttons for the public build.
- **Two different recognition-model paths.** `prefill()` uses `ocrModelDirFor(root_)` while `locateSelection()` hard-codes `BDRC_Woodblock`. Decide which is canonical; a user who installed the model elsewhere gets "recognition model missing" from one button and not the other.
- **Combined-block export is untested.** The `save()` block path has a real `wroteOk` guard and a "NOT COMBINED" message, but no `check(...)` pins it. Given WP-2's history, decide whether to add a battery check before ship.
- **Page-change autosave failure is untested.** `gotoPage()`'s `saveOrWarn` is the single point where a translator's typing can be lost. Manually verify the warn path (e.g. read-only `input_work` folder).
- **`g_openTextInInput` fails silently** on an unreadable rescued file — nothing happens, no message. Fix or accept.
- **"scan follows cursor" is not persisted** while predictive typing is. Decide whether that inconsistency ships.
- **Zoom % combo accepts garbage** and silently clamps to 25 %. Decide whether to validate.
- **Live spellcheck rescans the whole document per keystroke** up to 200 k chars. Verify typing latency on a long page on the slowest input-center machine.
- **`buildPredict()` builds the full ACIP headword set once and never refreshes.** Confirm no in-session data reload can stale it.
- **No shortcut reaches the Input tab.** Decide whether input-center operators need one.
- **In non-`ALL_HAVE_OCR` builds the whole OCR ribbon group disappears**, leaving an empty "OCR" group heading. Verify how that renders in the shipped configuration.
