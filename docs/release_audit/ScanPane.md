# Release audit — `ScanPane` (the "OCR" tab)

Audit date: 2026-09-08 · Source: `app/main.cpp` (41,427 lines) · Class body
lines **28893–29862**, followed by `#endif // ALL_HAVE_OCR` (29863) and
`ProposePane` (29878). Read exhaustively; no source file was modified.

## 1. What it is

`ScanPane` is the app's page-image OCR surface: the user opens a scanned
folio (PNG/JPG/TIFF), presses **Run OCR**, and gets per-line Wylie +
Tibetan unicode in a results browser where every recognized word is a
clickable anchor that highlights that word's box on the annotated page
image above. It is reached as the top-level tab **"OCR"**, constructed in
`main()` at `app/main.cpp:36071–36072` (`auto* ocrPane = new ScanPane(checker,
root); tabs.addTab(ocrPane, "OCR")`), and the whole class — plus the tab — is
compiled only under `ALL_HAVE_OCR`, which `app/CMakeLists.txt:47–49` defines
only when the `allocr` target exists (onnxruntime found; OpenCV additionally
gates line building). On a machine without onnxruntime the tab simply does
not exist. Recognition itself needs two BDRC ONNX models on disk under
`<root>/library/ocr_models/`; the pane refuses politely and explains the
download when they are absent. Session restore reopens last session's image
at `main()` line 40594 (`ocrPane->restoreSession()`), and the pane's headless
battery runs from the `--selftest` block at line 38256
(`fails += ocrPane->selfTest(log);`).

The comment at **28895–28899** is the coverage note: *"TEST-7: this pane sat
at 22.2% coverage with no selfTest at all — the OCR entry point,
unexercised. This drives the headless half: open-state transitions and the
strip→page coordinate map. The recognition path itself needs models and stays
with the (guarded) live run."* That is an accurate self-description: the
battery covers open-state and two pure helpers, and **nothing else in the
pane is automatically covered**.

**Correction to the audit brief:** `ScanPane` does **not** use
`ScanCanvasLabel` (defined 4508–4568, with click / drag-pan / ctrl-wheel-zoom
/ pinch-zoom hooks). Its page view `pageView_` is a plain `QLabel` inside a
`QScrollArea` (29040–29045). Consequently the OCR canvas has **no** click,
drag-pan, wheel-zoom, pinch-zoom, or context menu at all — the image renders
at 1:1 native pixel size (`adjustSize()`) and the only way to move around a
large folio is the scroll bars. `ScanCanvasLabel` is used by other panes
(instantiated at 10329 and 10557).

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor | Automated coverage | Notes / risks |
|---|---|---|---|---|
| **"Open scan image…"** button (ribbon PAGE, big, icon `image`) | `safeGetOpenFileName` under `<root>/library`, filter `*.png *.jpg *.jpeg *.tif *.tiff`; delegates to `openImagePath` | `ScanPane` ctor 28969–28970 + `openImage()` 29085–29092 | Indirect only — `selfTest` drives `openImagePath`, never the dialog | Writes QSettings session keys via the dialog helper (`dlg/last`, per-caption key, `sess::put` at 3312–3313) |
| Opening an **unreadable/missing** image | Sets results HTML `"<i>cannot read that image</i>"`, leaves Run disarmed, keeps previous image loaded | `openImagePath()` 29096–29101 | **YES** — `"an unreadable image refuses politely and arms nothing"` (28912) | Failure is a one-line italic string, no dialog; prior `file_`/`img_` silently retained |
| Opening a **readable** image | Converts to RGB888, stores `file_`, persists `sess::put("ocr/lastImage", f)`, shows pixmap 1:1, arms Run, disarms Save, results `"ready — Run OCR"` | `openImagePath()` 29102–29110 | **YES** — `"a readable image arms Run OCR and reports ready"` (28921) and `"save stays disarmed until a recognition has run"` (28923) | Disk/settings write of the last-image path (privacy-of-path only). No downscale: a 10k-px folio renders full size in the scroll area |
| **Session restore** (automatic at launch) | Reads `sess::path("ocr/lastImage")` and reopens it silently | `restoreSession()` 29079–29082 | NONE | Reopens a possibly-moved/renamed file; if gone, the pane silently shows "cannot read that image" at startup. Called from `main()` 40594 |
| **"deskew off (0°)"** checkbox (ribbon VIEW), default **checked** | Passes `&zero` to `allocr::buildLines` instead of letting the BDRC estimator run — a labeled deviation from the BDRC pipeline; used by both `runOcr` and `batchFolder` | ctor 28986–28997; consumed 29244 / 29284–29287 and 29477 | NONE (state never asserted) | Default-on is a documented **measured** deviation (18/40 real BDRC folios returned zero lines with deskew on). Not persisted to QSettings — resets to checked every launch, so a user working tilted scans re-unticks every session |
| **"Run OCR"** button (ribbon PAGE, big, icon `play`) | Repacks the image to tight RGB, spawns `std::thread` running `ocrWorker`; button text becomes "Stop" | ctor 28999–29001, `runOcr()` 29229–29255 | NONE (recognition path is model-gated; only arm/disarm state is checked) | Heavy: loads two ONNX models, runs a worker thread. Model load failure is surfaced as HTML, `catch(std::exception)` (29127–29133). No timeout |
| **"Stop"** (same button while running) | Sets `ocrStop_`, label becomes "stopping…"; worker breaks the per-line loop and the finish call appends a labeled partial-results banner | `runOcr()` 29233–29237; `ocrWorker` 29341, 29394; `ocrFinish` 29268–29275 | NONE | Stop is cooperative and only checked between lines — a single very slow line cannot be interrupted |
| **"models missing"** state | If `PhotiLines.onnx` or the chosen model's `model_config.json` is absent, prints the exact two download targets and refuses | `ensureModels()` 29112–29125 | NONE | Names Hugging Face URLs in prose only (no network from here) |
| **Click a recognized word** in the results browser (`w:<line>:<word>` anchor scheme) | Bounds-checks, redraws the page with that word highlighted amber/orange, and `scroll_->ensureVisible(pageX(line, x0), y+h/2, 120, 120)` | ctor 29056–29068; anchors emitted 29365–29373 | Partial — the *coordinate map* is covered (`pageX` checks, 28935/28940) but the anchor parse/scroll is NONE | `results_->setOpenLinks(false)`, so a malformed anchor is a silent no-op. The scheme is `w:i:k`, split on ':' with a strict size-3/prefix check |
| **"mark illustration candidates"** checkbox (ribbon VIEW) | On toggle, redraws the page with `illustrationCandidates()` regions outlined gold and labeled "image? (candidate)" | ctor 29003–29011; drawn 29200–29220 | Partial — the *geometry* is covered by the Scans-pane battery `"illustration candidates: side panel + gap found, …"` (~6077–6084); the toggle itself NONE | Toggle only redraws if `!pl_.lines.empty()`, i.e. it does nothing before a run — no feedback explaining that |
| **"Illustration gallery…"** button (ribbon VOLUME, big, icon `image`) | Picks a folder (no `root_` default dir), globs 6 image extensions, runs `runIllustrationGallery` | ctor 29013–29021; `illusGallery()` 29832–29850 | NONE | Runs the **line detector on every page in the folder synchronously on the UI thread**, driven by `processEvents()` — long freeze risk on a real volume. Per-page exceptions are silently `continue`d (28820–28822) |
| Illustration gallery: **double-click a crop** | Opens a modal full-size viewer dialog of that crop (scaled to 1000×700) | 28861–28874 | NONE | Nested `exec()`; no zoom, no save from this view |
| Illustration gallery: **"Save crops…"** | Picks a folder, writes `<basename>-illus<N>.png` for every crop, reports the count | 28875–28888 | NONE | **`QImage::save()`'s bool is discarded** — the "N crop(s) saved" message is not gated on a verified write, unlike every other write path in this pane. Overwrites same-named files without asking |
| Illustration gallery: **"Close"**; **"Stop"** on its progress dialog | Accept the dialog / break the scan loop | 28876/28869; 28802–28804 | NONE | — |
| **"OCR models…"** button (ribbon MODELS, big, icon `chip`) | Opens `showOcrModelManager(this, root_)` | ctor 29023–29030; dialog 400–520 | NONE | See the four rows below |
| Model manager: **"Download…" / "Re-download"** (per non-bundled model, 4 rows) | `mkpath`, then `ocrDownloadOne` for `config.json` → `model_config.json` and for the `.onnx`, from `https://huggingface.co/BDRC/<repo>/resolve/main/` | 456–487 (`showOcrModelManager`), `ocrDownloadOne` 375–423 | NONE | **Network + large disk writes.** Redirects allowed (`NoLessSafeRedirectPolicy`). Whole body buffered in RAM (100 MB-class models) then staged as `<dest>.downloading~` and renamed only on a verified full-byte write — the FAIL-13 fix. On failure it removes `model_config.json` but **can leave an orphaned `.onnx`** in the dir. No checksum/signature verification of downloaded model bytes |
| Model manager: **"Recognition model:" combo** | Lists only installed models; writes `QSettings("ALL","TranslationTool") scan/ocrModel` on change | 492–507 | NONE | `ocrModelDirFor()` (365–376) sanitizes (`no '/'`, no `..`) and falls back to bundled `BDRC_Woodblock` if the picked dir lacks `model_config.json`. `ensureModels()` resets `rec_` when the dir changed (29115–29116) — but **`det_ && rec_` short-circuits at 29113 only when both exist**, so the reset does happen; verify the first-run ordering |
| Model manager: **"Close"** | `dlg.accept()` | 516–519 | NONE | Modal `exec()` |
| **"Save to ocr_out…"** button (ribbon PAGE, big, icon `save`; disabled until a run produced text) | `mkpath library/ocr_out`, `safeGetSaveFileName` defaulting to `<base>-ocr.txt`, writes an `# OCR-DERIVED …` header + one Wylie line per detected line via `writeAllOrWarn` | ctor 29036–29038, `saveOut()` 29655–29688 | Only the **disarmed** precondition is checked (28923); the write itself NONE | Write is verified (`writeAllOrWarn` → `streamWriteOk`, 2994/3008) and the green "saved to" line is gated on it. Disk write outside the app bundle, into the data root |
| **"Batch folder…"** button (ribbon VOLUME, big, icon `stack`) | Requires models, picks a folder, globs 5 extensions, OCRs every page, writes one `<base>-ocr.txt` per page to `library/ocr_out/<foldername>/`, then prints a written/lines/flags/failures summary + per-page detail | ctor 29040–29048; `batchFolder()` 29411–29652 | NONE | **The single largest uncovered surface.** Fully synchronous on the UI thread with `processEvents()` pumping a `QProgressDialog` — a 300-page volume blocks the app. Carries two hard-won correctness fixes (mkpath return checked, DATA-5; `++pagesOk` moved inside the verified-write branch; truncated files removed) that **no test protects**. Per-page `catch(std::exception)` counts a failure and continues |
| Batch: **"Stop"** on the progress dialog | `prog.wasCanceled()` breaks the loop; the summary appends "· **stopped early**" | 29464–29466, 29644–29645 | NONE | Already-written pages remain on disk |
| **Results browser** (`QTextBrowser`, read-only) | Shows the header line (deskew angle + mode + line count), per-line numbered Wylie + Tibetan + "n/m syllable(s) fail legality", the total flag count and the BDRC credit | 29048–29055; HTML built 29296–29391 | Legality counting **YES** — `"a known-bad token is COUNTED among legal ones (TP-4)"` and `"legal-only text counts zero flags (TP-4)"` (28950/28954) | Selectable/copyable text; every export path carries the OCR-DERIVED header, and the banner (28962–28968) states CC BY-NC 4.0 + "never corpus-bound" |
| Implausible-deskew warning | When deskew is **on** and \|angle\| > 20°, prints "The deskew estimate (x°) is implausible" instead of trusting it | 29307–29318 | NONE (unreachable in the default deskew-off state) | STATIC-1 fix; only fires on the non-default path |
| "No lines found" explainer | Distinct advice depending on whether deskew was off (blank/faint/cropped) or on (tick 'deskew off (0°)') | 29320–29339 | NONE | — |
| **Page canvas** (`pageView_`) | Renders the deskewed page with purple line boxes, green word underlines, amber highlight for the clicked word, plus gold illustration candidates | `drawPage()` 29153–29227 | NONE | **No mouse interaction whatsoever** — not a `ScanCanvasLabel`. No zoom, no pan, no click-a-word-on-the-page (the follow-along is one-way, text → image). `adjustSize()` at native resolution |
| Keyboard shortcuts / context menus | **None defined** anywhere in the class | — | — | The pane is mouse-only; no accelerators, no `setShortcut`, no `customContextMenu` |
| Drag & drop of an image onto the pane | **Not implemented** (`setAcceptDrops` never called) | — | — | Likely user expectation on a "drop a scan here" surface |

**Counts: 24 user-visible controls/actions listed above; 19 have NONE automated coverage** (5 have full or partial coverage: unreadable-open, readable-open, save-disarmed, `pageX` mapping ×2 checks, legality counting ×2 checks, plus the partially-covered illustration geometry and word-anchor coordinate map).

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ScanPane::selfTest(QStringList&)` | 28900 | Headless battery: 7 `check(...)` assertions (bad open, good open, save disarmed, `pageX` fallback, `pageX` clamping, TP-4 illegal count, TP-4 zero count). Writes and removes `QDir::temp()/all_selftest_scan.png` | `main()` 38256, under `--selftest` |
| `ScanPane::ScanPane(SyllableChecker*, root)` | 28958 | Builds the banner, the four-group `RibbonBar` (PAGE/VIEW/VOLUME/MODELS), the vertical splitter (scrollable page view over results browser), and all signal wiring | `main()` 36071 |
| `~ScanPane()` | 29074 | Sets `ocrStop_`, joins `ocrThread_` so the worker never posts into a dead pane | destruction |
| `restoreSession()` | 29079 | Reopens `ocr/lastImage` | `main()` 40594 |
| `openImage()` | 29085 | File dialog wrapper | Open button |
| `openImagePath(const QString&)` | 29095 | Load / validate / arm / persist | `openImage`, `restoreSession`, `selfTest` |
| `ensureModels()` | 29112 | Lazily constructs `allocr::LineDetector` (PhotiLines) and `allocr::TextRecognizer` (the picked dir); reports missing/failed models as HTML | `runOcr`, `batchFolder` |
| `pageX(size_t, double) const` | 29140 | Maps a word's strip-x back to deskewed-page x through the line's `colMap` (inverse of mask-n-crop), clamped; falls back to `line.x + stripX` | `drawPage`, the anchor handler, `selfTest` |
| `drawPage(int hlLine, int hlWord)` | 29153 | Paints line boxes, word underlines, the highlight, and the illustration candidates onto a copy of the deskewed page | anchor click, `ocrFinish`, illustration toggle |
| `runOcr()` | 29229 | Stop-or-start; repacks pixels; launches the worker thread | Run button |
| `ocrFinish(...)` | 29260 | UI-thread publication of worker results + the "stopped after N line(s)" partial banner; re-arms Save/Run | `ocrWorker` via `QMetaObject::invokeMethod` |
| `static countIllegalWylie(wylie, checker, int* toksOut)` | 29284 (decl ~29279) | Pure syllable-legality counter extracted for testability (TP-4 conversion — its mutant survived the battery from 23 Aug) | `ocrWorker`, `batchFolder`, `selfTest` |
| `ocrWorker(rgb, w, h, deskewOff)` | 29290 | Worker thread: detect → `buildLines` → per-line `recognize` → build the whole results HTML → post `ocrFinish` | `runOcr`'s `std::thread` |
| `batchFolder()` | 29411 | Whole-volume OCR to `library/ocr_out/<folder>/` | Batch button |
| `saveOut()` | 29655 | Verified single-page text export | Save button |
| `illusGallery()` | 29832 | Folder picker → `runIllustrationGallery` | Gallery button |

File-scope helpers this pane depends on (outside the class):

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `struct OcrModelInfo` / `kOcrModels[5]` | 341 / 347 | The BDRC model registry (dirName, repo, onnx, description) | `ocrModelDirFor`, `showOcrModelManager` |
| `ocrModelDirFor(root)` | 365 | Resolves `scan/ocrModel` to a real installed dir or the bundled default; rejects `/` and `..` | `ensureModels`, and the Input pane's OCR pre-fill (28031, 28110) |
| `ocrDownloadOne(parent, url, dest)` | 375 | Single-file HTTP download with progress + cancel, staged and byte-verified before swap (FAIL-13) | `showOcrModelManager` |
| `showOcrModelManager(parent, root)` | 425 | The model dialog | OCR models button |
| `illustrationCandidates(w, h, lines)` | 28735 | Pure geometry: side panels (> w/8) and inter-line gaps (> 2× median line height) | `drawPage`, `runIllustrationGallery`, the Scans-pane battery at 6077 |
| `runIllustrationGallery(parent, root, paths, title)` | 28773 | Shared gallery runner (also used by the Overlay at 14322/14412) | `illusGallery` |
| `ScanCanvasLabel` | 4508 | Click/drag/zoom canvas — **not used by ScanPane**; used at 10329, 10557 | other panes |

## 4. Dependencies

**Data folders / files (all under the resolved data `root_`):**
- `library/` — default directory for the Open and Batch dialogs.
- `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx` — line detector; **required** for Run OCR, Batch, and the illustration gallery. Not in `kOcrModels`, so the model manager cannot download it — it must be installed by hand or bundled.
- `library/ocr_models/BDRC_Woodblock/{OCRModel.onnx,model_config.json}` — bundled default recognizer and the fallback for every unresolved pick.
- `library/ocr_models/<dirName>/model_config.json` — installed-marker for each of the 5 registry models.
- `library/ocr_out/` — single-page saves; `library/ocr_out/<foldername>/<base>-ocr.txt` — batch output. Created with `mkpath` (return value checked in `batchFolder`, **not** checked in `saveOut`, where `writeAllOrWarn` catches it downstream).
- Illustration-crop saves go to any user-picked folder as `<base>-illus<N>.png`.
- `ocr/CMakeLists.txt:38–42` — the only ctest suite in this area, `ocr_smoke`, which drives the `allocr` library (PhotiLines + `build/ocr_ref` + BDRC_Woodblock) directly, **not** `ScanPane`.

**Models (`OcrModelInfo`, 341–356):** `Woodblock` (bundled default, Derge Kangyur woodblock), `LhasaKanjur`, `DergeTenjur`, `BigUCHAN_v1`, `ModernBookFormat`. All BDRC, CC BY-NC 4.0, used with permission, credited in the pane banner, in every results footer, and in every written file header.

**Network hosts:** `huggingface.co` only — `https://huggingface.co/BDRC/<repo>/resolve/main/config.json` and `…/<onnx>`, via `TimedNam` with `NoLessSafeRedirectPolicy` (so redirects to the CDN, typically `cdn-lfs*.huggingface.co`, are followed). Downloads are user-initiated from the model dialog; the pane makes no other network calls.

**External binaries / processes:** **none**. No `QProcess`, no `tesseract`, no shell-out anywhere in the pane. Recognition is in-process onnxruntime + OpenCV via the `allocr` static library.

**QSettings keys** (`QSettings("ALL","TranslationTool")`):
- `scan/ocrModel` — the chosen recognition model dir name (read by `ocrModelDirFor`, written by the combo).
- `app/dataRoot` — the data root the pane's paths hang off (written by `findDataRoot`, 519+).
- Session store (`sess::`): `ocr/lastImage`; plus the file-dialog memory keys `dlg/last` and the per-caption key (3312–3313).
- The deskew checkbox and the illustration-candidates checkbox are **not** persisted.

**Build gates:** `ALL_HAVE_OCR` (app/CMakeLists.txt:49, set only if `TARGET allocr`); `ALLOCR_HAVE_OPENCV` inside `allocr` gates `linebuild.cpp`/`recognize.cpp` — i.e. a build with onnxruntime but no OpenCV compiles the pane but has no line building.

## 5. Release questions

- **Ship, hide, or gate the OCR tab at all?** It is already absent on builds without onnxruntime, so the shipped DMG's behaviour depends entirely on the build machine's dependency set. Decide whether the release build is expected to have OCR, and whether a build without it should say so rather than silently dropping a tab.
- **PhotiLines is undownloadable from inside the app.** Every OCR feature dies without it, and the model manager lists 5 models, none of them PhotiLines. Is it bundled in the DMG data folder? If not, the model manager needs a PhotiLines row.
- **Batch folder is the biggest ship risk:** UI-thread, `processEvents`-driven, unbounded, with zero automated coverage, guarding two data-integrity fixes (DATA-5 mkpath, `++pagesOk` placement) that only a test can keep fixed. Decide: ship as-is, move it to the worker thread, or hide it for this release. At minimum, add a battery check for the "read-only output dir ⇒ 0 written, N failures" path.
- **"Save crops…" reports success without verifying the write** (`QImage::save()`'s bool discarded, 28881–28884) — the exact class of false-count bug the rest of the pane went out of its way to fix. Fix before ship or drop the success count.
- **Model downloads are unverified bytes.** Full-length verification is done, but there is no checksum or signature; a compromised/mirrored `resolve/main` URL installs an arbitrary ONNX file that is then executed by onnxruntime. Decide whether pinned hashes are required for release.
- **A failed download can leave an orphaned `.onnx`** (only `model_config.json` is removed on failure, 481–484). Harmless today because the config is the installed-marker, but it wastes ~100 MB silently. Confirm acceptable.
- **`deskew off (0°)` defaults to ON and is not persisted.** It is a deliberate, measured deviation from the BDRC pipeline. Confirm the label/tooltip satisfy the provenance rules, and decide whether the state should be remembered per session.
- **The page canvas has no zoom or pan** (plain QLabel at native resolution). A 6000-px folio is navigated by scrollbars only, and clicking a word can scroll it off-view on small screens. Is that acceptable for release, or should `pageView_` become a `ScanCanvasLabel` (the class already exists, at 4508, with the exact hooks needed)?
- **No drag & drop, no keyboard shortcuts, no context menu** on a pane whose primary gesture is "give me this image". Decide whether that ships.
- **Session restore silently re-opens the last image at launch** and prints "cannot read that image" if it moved — the very first thing a user sees in the tab. Decide whether restore should be silent-on-failure instead.
- **Illustration gallery freezes the UI** over a whole folder and silently skips per-page failures (28820–28822): a folder where every page fails is indistinguishable from a folder with no candidates. Decide whether failures must be reported.
- **The word-anchor scheme (`w:i:k`) is silently no-op on any malformed link.** Fine as designed, but confirm no other producer can inject anchors into `results_`.
- **Accessibility:** all state is conveyed by colour (purple/green/amber/gold boxes, `#B4540A` vs `#3B7A3B` for flag counts) with no text equivalent on the canvas; the pane has no `accessibleName`s. Needed for the accessibility-audit gate?
- **Coverage decision:** the TEST-7 comment explicitly parks the recognition path with "the (guarded) live run". Confirm that guarded live run exists and is executed before release, since `ocr_smoke` (ctest) exercises `allocr`, not this pane.
