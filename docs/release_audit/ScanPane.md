# Release audit — `ScanPane` (app/main.cpp:37701–38462)

## 1. What it is

`ScanPane` is the OCR pane: the surface where a scanned folio image becomes
Tibetan text. The user opens a page image (PNG/JPG/JPEG/TIF/TIFF), presses
**Run OCR**, and the pane runs the proven `allocr` pipeline end to end — BDRC
PhotiLines line detection → line building (deskew, mask-and-crop) → CTC
recognition against a BDRC recognizer → our own `wylieToUnicode` chain →
syllable-legality QC. The result lands in a results browser as per-line Wylie
plus Tibetan unicode with a per-line legality count, every word a clickable
anchor that highlights that word's box on the annotated page above. Its
audience is the input-centre / cataloguing side of the house, not the
translator: the pane is a first-pass typing aid whose whole contract is that
its output is **OCR-DERIVED review material**, banner-marked, never
corpus-bound. Output lands in `library/ocr_out/`, which the Overlay, the
Files pane and the Input pane all recognise and brand amber on open
(`openFile()` at main.cpp:9585).

It is small for this codebase — **762 lines**, roughly 0.03 of `app/main.cpp`
and about a fourteenth of `OverlayPane`. It is constructed in `main()` at
main.cpp:45538 and added as the tab `"OCR"` at 45539, inside the **Input**
ribbon group (`mkGroup("Input", {"Input", "OCR"})`, main.cpp:46062). Its
pane blurb — the hover line every pane must have — is
`{"OCR", "Reading scanned pages into text."}` at main.cpp:6007.

**The whole class, and the tab, live inside `#ifdef ALL_HAVE_OCR`**
(main.cpp:37531 opens it, 38463 closes it; the construction site is guarded
again at 45537–45540, and the session restore at 51616–51618).
`ALL_HAVE_OCR` is defined only when the `allocr` CMake target exists
(`app/CMakeLists.txt:63–65`), and `allocr` returns early unless **onnxruntime**
is found (`ocr/CMakeLists.txt:11–14`). So on a machine without onnxruntime
this pane does not exist at all — no tab, no menu entry, no Help chapter
target — and the app is otherwise identical. That has two release
consequences worth stating up front: (a) every screenshot, manual chapter and
pane count in the docs is build-conditional here, and (b) `ALL_HAVE_OCR` is
gated on onnxruntime **only**, while `allocr::buildLines` and
`allocr::TextRecognizer` are compiled only when **OpenCV** is also found
(`ocr/CMakeLists.txt:26–31`) — so a build with onnxruntime and no OpenCV
defines `ALL_HAVE_OCR`, compiles this class, and fails to link. The
configuration matrix is untested.

Its `selfTest(QStringList&)` (main.cpp:37708–37764) is ~56 lines and asserts
**7** `check(...)` conditions. Verified by running the battery for this audit
(`QT_QPA_PLATFORM=offscreen DiamondCutterTranslationTool --selftest`, this
build, exit 0): all seven pass —

```
[PASS] Scan: an unreadable image refuses politely and arms nothing
[PASS] Scan: a readable image arms Run OCR and reports ready
[PASS] Scan: save stays disarmed until a recognition has run
[PASS] Scan: pageX falls back to line origin + strip offset
[PASS] Scan: pageX clamps the column map at both ends
[PASS] Scan: a known-bad token is COUNTED among legal ones (TP-4)
[PASS] Scan: legal-only text counts zero flags (TP-4)
```

Its own comment (37703–37707) is honest about the shape of that battery: it
drives "the headless half" — open-state transitions and the strip→page
coordinate map — and leaves the recognition path, which needs the models, to
the guarded live run. The last two checks are the TP-4 mutation trap: the
legality counter was extracted out of two inline loops behind the real
recognizer precisely because zeroing it survived the whole battery from 23
August until it was pulled out and drilled.

It runs under the `app_selftest` ctest suite (`app/CMakeLists.txt:105`,
`DiamondCutterTranslationTool --selftest`), called at main.cpp:49022.

**The finding that shapes the rest of this file.** Every control this pane
has — all six buttons and both checkboxes — is built into a `RibbonBar`
that is *detached from the pane* (`ribbon->attachTo(this)`, main.cpp:37851,
which only records the pane→ribbon mapping; the band later reparents the
ribbon into its own `QScrollArea` at main.cpp:46018–46019). The pane widget
therefore owns **no** `QPushButton`, `QCheckBox` or `QComboBox` at all. Three
separate mechanisms walk `pane->findChildren<...>()` and consequently see
nothing here:

- the **`--sweep` harness** (main.cpp:48878–48944). Verified by running it
  for this audit: `--sweep OCR` prints
  `SWEEP COMPLETE: OCR — 0 control(s) exercised, app coherent` — zero
  buttons, zero toggles, zero combos, not even a `[SKIP]` line;
- the **Panes-menu mirror** (main.cpp:46762–46851), which is what fills
  `Panes ▸ OCR` with the pane's commands. With no children to mirror, that
  submenu carries only its **Show pane** action;
- the **geometry inquisition** in `--screenshots` (main.cpp:52227), which
  audits clipping/overlap per pane.

And because the Quick Access pin resolver works by matching a ribbon button's
label to a *menu* path (`qatPathForLabel()`, main.cpp:6121–6143, exactly-one-
match or nothing), an empty `Panes ▸ OCR` submenu means every one of this
pane's ribbon buttons answers **"Cannot pin this one"** on right-click
(main.cpp:6177–6184). None of that is a crash; all of it is a pane that is
invisible to the house's own coverage machinery, and it is why so many rows
below read NONE.

---

## 2. User-visible functions

### 2a. Ribbon buttons and toggles (constructed 37777–37852; groups PAGE / VIEW / VOLUME / MODELS)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Open scan image…** (button, PAGE) | File dialog filtered to `*.png *.jpg *.jpeg *.tif *.tiff` rooted at `<root>/library`; loads the image, converts to RGB888, shows it at 1:1, arms **Run OCR**, disarms **Save**, remembers the path in the session store | 37782 (`gPage->addBig` 37785), connect 37885 → `openImage()` 37902 → `openImagePath()` 37912 | selftest 37716–37733 — `"an unreadable image refuses politely and arms nothing"` and `"a readable image arms Run OCR and reports ready"` (both drive `openImagePath()` directly, not the button) | Writes session key `ocr/lastImage` (37920) **before** any user consent to remember it. A very large TIFF is loaded whole into a `QLabel` pixmap with no downscale and no size cap — a 600 MB uncompressed folio is a plausible input here. The dialog wrapper (`safeGetOpenFileName`, 3704) returns empty under every harness mode, so no automated run ever goes through the button. |
| **Run OCR** / **Stop** (button, PAGE) | First click: repacks the image, spawns the worker thread, sets the label to **Stop**, streams `recognizing line N/M…` into the results panel. Second click while running: sets `ocrStop_`, label becomes `stopping…`, and partial results are published **labelled** as partial | 37802 (`addBig` 37807), connect 37886 → `runOcr()` 38025 → `ocrWorker()` 38103 → `ocrFinish()` 38059 | **NONE** for the button and the whole recognition path. Engine-level only: ctest `ocr_smoke` (`ocr/CMakeLists.txt:38`) pins line detection, deskew angle, line bboxes, line images and CTC recognition against the canonical BDRC oracle on three banked folios | The tri-state (Run → Stop → stopping…) has no test at all. `run_` is **not disabled** during `stopping…`, so a third click re-enters `runOcr()`, sees `ocrRunning_` true, and re-sets `ocrStop_` — harmless, but the label then lies about the state. The worker reads `det_`/`rec_` (owned by the UI thread) and `ocrStop_` (atomic) — model pointers are not re-checked after `ensureModels()`, so a model swap from the OCR-models dialog mid-run is a data race in principle; `recDir_` is only compared on the next `ensureModels()` call. |
| **Save to ocr_out…** (button, PAGE) | Save dialog proposing `<root>/library/ocr_out/<page>-ocr.txt`; writes the OCR-DERIVED header + one Wylie line per recognized line through `writeAllOrWarn` | 37837 (`addBig` 37842), connect 37887 → `saveOut()` 38394 | selftest 37734–37735 — `"save stays disarmed until a recognition has run"`. That covers the **disabled state only**; the write itself is **NONE**. The shared helper is covered indirectly: `"streamWriteOk reports a refused stream write as a FAILURE"` (ApprovalPane selfTest, main.cpp:39112) | **Disk write.** Gated on a verified write (DATA-4) — a failed write warns, removes the partial, and prints nothing green. The success line claims "opening it in the Overlay runs the ocr-derived banner + first-pass QC"; that claim is **true** (OverlayPane `openFile()` branches on `/ocr_out/` at main.cpp:9585–9596). `QDir().mkpath` at 38396 discards its bool — unlike the batch path, which was fixed for exactly this (38248). |
| **deskew off (0°)** (checkbox, VIEW) | Forces the deskew angle to 0° instead of using the BDRC estimator, for both single-page and batch runs. **Checked by default** | 37795–37801 (`gView->add` 37801); read at 38042 (single) and 38289 (batch) | **NONE** | Default-ON is a **labelled deviation from the canonical BDRC pipeline** — justified in a 14-line measured comment (37786–37794: 18 of 40 real folios returned zero lines with deskew on), but it means the shipped default is not the reference pipeline, and the header text that discloses it (`"(deskew off — the default for pecha)"`, 38120) is the only place a user learns this. Not persisted: `sess::remember` is never called on it, so it silently resets to ON every launch. **The user manual is stale here on three counts** (`data/help/USER_MANUAL.md:2684–2691`): it calls the control "override deskew to 0° (DEVIATION…)", says "Leave it off unless a straight page comes out skewed" (it is on), and says the header brands the run "(override — deviation)" (it does not). |
| **mark illustration candidates** (checkbox, VIEW) | Redraws the page with amber boxes over regions *not* covered by detected text lines — side panels wider than w/8 and inter-line gaps taller than 2× the median line height — each labelled `image? (candidate)` | 37808–37815 (`gView->add` 37815), connect 37834–37836 → `drawPage()` 38001–38020, geometry `illustrationCandidates()` 37543–37576 | **NONE** for the toggle. The geometry beneath it is covered: `"illustration candidates: side panel + gap found, clean page yields none"` (OverlayPane selfTest, main.cpp:8041–8043, verified PASS) | Toggling before any run is a **silent no-op** — the handler returns unless `pl_.lines` is non-empty (37835), with no message. Label and tooltip are honest ("CANDIDATES only … never claimed complete"), and the gallery dialog repeats it. The geometry is pure heuristic with no confidence figure. |
| **Illustration gallery…** (button, VOLUME) | Folder picker, then runs line detection over every image in the folder and shows the candidate crops as an icon gallery with page provenance | 37816–37823 (`addBig` 37823), connect 37832–37833 → `illusGallery()` 38438 → `runIllustrationGallery()` 37581 (shared with OverlayPane/ScansPane, called there at 17382 and 17472) | **NONE** | Folder picker has **no default directory** (38439–38440 passes no `dir`), unlike every other picker in the pane which starts at `<root>/library`. Synchronous, on the GUI thread, one ONNX detection per image, `processEvents()`-pumped; a 600-page volume is a long unbounded run. Every crop is held in memory as a full `QImage` (`crops` vector, 37605) with no cap. |
| **Batch folder…** (button, VOLUME) | Loads the models, folder picker, then OCRs every page image in that folder → one `<page>-ocr.txt` per page into `library/ocr_out/<folder>/`, each with the OCR-DERIVED + BDRC-licence header; reports written/total, line count, legality flags, failures | 37843–37849 (`addBig` 37849), connect 37852 → `batchFolder()` 38228 | **NONE** | **Bulk disk write + long synchronous run on the GUI thread.** Two SQA DATA-5 fixes are recorded in place and worth verifying survive: `mkpath`'s bool is now checked (38248) and `++pagesOk` now sits *inside* the write guard (38300–38355), because a read-only data root used to report "300/300 written" over an empty directory. A truncated file is removed rather than left as a stub (38342). But: models are loaded **before** the folder dialog (38229), so cancelling still pays the model-load cost; there is no per-page thread, no deskew disclosure in the per-file header (only the OCR-DERIVED + models line, 38321–38325) even though the batch honours the deskew checkbox at 38289; and an existing `-ocr.txt` is **overwritten without asking**. |
| **OCR models…** (button, MODELS) | Opens the model manager dialog (see 2d): download the four non-bundled BDRC recognizers and pick which one Run OCR uses | 37824–37829 (`addBig` 37829), connect 37830–37831 → `showOcrModelManager()` 521 | **NONE** | The pane banner and the manual both frame OCR as local ("they run entirely on your machine — no network", `USER_MANUAL.md:2668–2670`) — this button is the exception and it is not mentioned in the manual's control list (§13.1, `USER_MANUAL.md:2681–2708`); its only mention is a stray paragraph before the chapter's own opening (`USER_MANUAL.md:2659`). |
| **right-click any ribbon button** → Pin / Unpin to Quick Access | Context menu on each `RibbonProxy` offering a pin into the Quick Access strip | `RibbonProxy` ctor 6169–6205; path resolver `qatPathForLabel()` 6121–6143 | **NONE** for this pane's buttons (the mechanism itself is audited in `RibbonBar.md`) | Because `Panes ▸ OCR` carries no mirrored actions (see §1), `qatPathForLabel()` finds no match and **all six of this pane's buttons show "Cannot pin this one"** — the one ribbon in the app where the advertised pinning does nothing. Worth confirming by hand before release. |

### 2b. The page viewer and results panel (constructed 37853–37883)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **word anchor click** (`w:<line>:<word>`) | Clicking a Wylie word in the results redraws the page with that word's box filled amber and outlined, dims the others to green underlines, and scrolls the page view to it | anchors emitted 38183–38188; handler 37868–37879 → `drawPage()` 37970, `pageX()` 37961, `scroll_->ensureVisible()` 37877 | **NONE** for the handler. The coordinate map it depends on is covered: selftest 37736–37746 — `"pageX falls back to line origin + strip offset"` and `"pageX clamps the column map at both ends"` | The handler bounds-checks `li`/`wi` (37872–37874) but then indexes `pl_.lines[li]` (37876) on the assumption that `words_` and `pl_.lines` are the same length — true today because `ocrFinish` assigns them together, but nothing asserts it. Malformed anchors are silently ignored (37870), which is right. Direction is one-way only: **the page image is not clickable** — there is no `mousePressEvent` on `pageView_`, so a user cannot go from a box on the folio back to its line of text. |
| **page viewer** (`QLabel` in a `QScrollArea`) | Shows the open image, then the deskewed page with violet line boxes, green word underlines and (optionally) amber illustration candidates | 37854–37860; `pageView_` 37855, `scroll_` 37860; redraw `drawPage()` 37970 | **NONE** | **No zoom, no fit-to-window, no rotate** — the image renders at 1:1 in a scroll area, which for a real BDRC folio means constant two-axis scrolling. (The Overlay's woodblock viewer has zoom, brightness, contrast and invert; this pane has none of it.) `adjustSize()` on every redraw (38022) resets the scroll position implicitly on large pages. |
| **vertical splitter** (page ⟷ results) | Drag to rebalance the page viewer against the results panel | 37853, stretch factors 37881–37882 | **NONE** | Not persisted — unlike the Overlay's `overlay/inputSplit`, this splitter forgets its position on every launch. |

### 2c. Batch-OCR progress dialog (`batchFolder()`, 38257–38269, 38375)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Stop** (cancel button on the window-modal progress dialog) | Breaks out of the page loop; the summary then appends **stopped early** and reports the honest written/total split | 38257–38261 (dialog), 38269 (`wasCanceled()`), 38389–38390 (the disclosure) | **NONE** | Cancellation is only checked between pages, so Stop cannot interrupt the current page's recognition — on a dense folio that is a multi-second wait with the button already pressed. Pages already written stay on disk; nothing tells the user which ones, beyond the per-file `detail` list. |

### 2d. OCR models dialog (`showOcrModelManager()`, main.cpp:521–615)

Reached only from the **OCR models…** ribbon button (37830). Lists five BDRC
models from `kOcrModels` (441–452): Woodblock (bundled), Lhasa Kangyur, Derge
Tengyur, BigUCHAN, Modern book format.

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Download… / Re-download** (×4, one per non-bundled model) | `mkpath`s `library/ocr_models/<dir>`, fetches `config.json` → `model_config.json` then the `.onnx` from `https://huggingface.co/BDRC/<repo>/resolve/main/`, and flips the row to "✓ installed" | 552–583; the fetch itself `ocrDownloadOne()` 468–519 | **NONE** | **Network + ~100 MB disk write, with no checksum and no declared size.** FAIL-13 is fixed and documented in place (498–515): the download is staged beside the target, the byte count is verified, and the swap happens only on proof, so a failed download leaves the working model untouched. What is still missing is any **integrity** check — no hash, no signature, no expected length — so a valid-length wrong file installs cleanly. The whole body is read into memory (`r->readAll()`, 502) before staging. On a partial failure the dialog removes `model_config.json` but **leaves a downloaded `.onnx` behind** (576), so the directory is left half-populated and `installed()` correctly reports "not installed" while the bytes stay on disk. |
| **Cancel** (on the per-file download progress dialog) | Aborts the in-flight `QNetworkReply` | 475–481 | **NONE** | Abort makes `r->error()` non-`NoError`, so the staged file is removed and nothing is switched — correct. A nested `QEventLoop` (487–490) runs on the GUI thread for the duration. |
| **Recognition model** (combo) | Lists only the *installed* models; the pick is written to `QSettings scan/ocrModel` immediately on change | 588–601; consumed by `ocrModelDirFor()` 456–466 | **NONE** at the control level. The resolver it feeds *is* covered: `"OCR model pick falls back to the bundled model; traversal refused"` (main.cpp:49203–49216, logged under the `Lookup:` block, verified PASS) | The picked name is path-sanitised (`!contains('/')`, `!contains("..")`, 460–461) and falls back to the bundled Woodblock model when the pick is missing — never a guess. But the pick **takes effect only at the next `ensureModels()`**, and `ScanPane::ensureModels()` resets `rec_` when the directory changed (37933) while **`det_` is never reset**, so `if (det_ && rec_) return true` at 37929 is false and both are rebuilt — correct by accident, not by design. Note the setting is written on `currentTextChanged`, so a combo rebuild would also write. |
| **Close** | Accepts the dialog | 610–613 | **NONE** | The dialog has no Cancel semantics: the model pick is already persisted by the time Close is pressed, so there is no way to back out of a switch. |

### 2e. Illustration gallery dialog (`runIllustrationGallery()`, main.cpp:37581–37699)

Shared code: also reached from OverlayPane (17382, 17472) and ScansPane, so a
disposition here affects three panes.

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Stop** (cancel on the scanning progress dialog) | Stops the page walk; crops found so far are still shown | 37600–37610 | **NONE** | The window title then reports the crop count without saying the walk was cut short — a partial gallery is presented as if it were the whole folder. |
| **thumbnail double-click** | Opens a modal preview of the crop scaled to 1000×700 | 37670–37683 | **NONE** | The preview is a bare `QDialog` with no Close button, no shortcut wired, and no window title beyond the source filename — the user closes it through the window chrome. |
| **Save crops…** | Folder picker, then writes every crop as `<source base>-illus<N>.png` | 37662, 37684–37697 | **NONE** | **Disk write with no verification** — `QImage::save()`'s bool is discarded (37690), then the dialog reports "%1 crop(s) saved" counting attempts, not successes. This is the exact class of defect the batch path was fixed for (DATA-5, 38300–38308: "A false COUNT is worse than a false sentence"). Existing files are overwritten silently. Also has no default directory. |
| **Close** | Accepts the dialog | 37663, 37668–37669 | **NONE** | |

### 2f. Window-level, lifecycle and cross-pane entry points

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"OCR" ribbon surface button** (Input group) | The pane switcher — the way a user actually reaches this pane; the inner tab bar is hidden and the ribbon band *is* the switcher | built 45977–45996; group membership `mkGroup("Input", {"Input", "OCR"})` 46062; blurb 6007 | `app_selftest` — `"Hover: every pane says what it is (25 pane(s))"` (main.cpp:51078, verified PASS) and `"Hover: every ribbon button says what it does (135 of 135)"` (51087, verified PASS) | Both gates assert that the button *says* something, not that it does anything. The pane can also be switched off entirely by the user via Preferences ▸ Features (`featureOn("pane", "OCR")`, 3477–3481); it is **not** release-hidden (`g_releaseHidden` is empty in shipped builds, 3472). |
| **Panes ▸ OCR ▸ Show pane** (menu bar) | Raises the pane | 46753–46757 | **NONE** | This submenu is **empty apart from Show pane** — the mirror at 46762/46781/46802/46837 walks `pane->findChildren<...>()` and this pane owns none of its controls (verified: `--sweep OCR` exercises 0 controls). Compare `Panes ▸ Overlay`, which carries 27 actions and has a selftest floor at main.cpp:51154 (`overlayActions > 8`). There is no equivalent assertion for any other pane, so this gap is invisible to the battery. |
| **session restore at launch** | On startup, reopens last session's page image if the path still exists | `restoreSession()` 37896–37899, called at main.cpp:51617 (inside `#ifdef ALL_HAVE_OCR`); key written at 37920; `sess::path()` 3369–3372 | **NONE** | A vanished path restores as nothing rather than an error (good, and the `sess::path` contract says so). But this decodes and renders a full-resolution scan **on the launch path**, before the window is shown (51617 precedes `win.show()` at 51619) — a slow launch that is invisible in every harness mode, since the harness never writes the key. No way for a user to clear it except opening a different image. |
| **quit / close the window while a run is in progress** | The destructor sets `ocrStop_` and joins the worker so it can never post into a dead pane | `~ScanPane()` 37890–37894 | **NONE** | The join is unbounded: quitting mid-line waits for the current line's CTC decode to finish, with no UI feedback. Correct, but untested — and this is the one path where a bug is a crash at exit rather than a visible fault. |

---

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `selfTest` | 37708 | the 7-check battery | `main()` 49022 (`app_selftest`) |
| `ScanPane` (ctor) | 37766 | banner, ribbon, split, page view, results browser, all wiring | `main()` 45538 |
| `~ScanPane` | 37890 | stop + join the worker | Qt teardown |
| `restoreSession` | 37896 | reopen last session's page | `main()` 51617 |
| `openImage` | 37902 | file dialog → `openImagePath` | Open scan image… |
| `openImagePath` | 37912 | load, convert, display, arm/disarm, remember | `openImage`, `restoreSession`, selfTest |
| `ensureModels` | 37928 | lazy-load PhotiLines + the picked recognizer; explains itself when either is missing | `runOcr`, `batchFolder` |
| `pageX` | 37961 | strip-x → deskewed-page x through the line's `colMap` (the exact inverse of mask_n_crop) | `drawPage`, the word anchor handler |
| `drawPage` | 37970 | annotate the deskewed page: line boxes, word underlines, highlight, illustration candidates | `ocrFinish`, illus toggle, anchor click |
| `runOcr` | 38025 | Run/Stop state machine; repack the image; spawn the worker | Run OCR button |
| `ocrFinish` | 38059 | publish worker results on the UI thread; append the partial-run disclosure | `ocrWorker` (queued) |
| `countIllegalWylie` | 38084 | the pure syllable-legality counter (TP-4 extraction — it was inline in two loops and its mutant survived the battery) | `ocrWorker`, `batchFolder`, selfTest |
| `ocrWorker` | 38103 | detect → build lines → recognize per line → unicode + legality → HTML; implausible-skew and no-lines-found disclosures | the worker thread |
| `batchFolder` | 38228 | whole-folder OCR → one headered `-ocr.txt` per page | Batch folder… |
| `saveOut` | 38394 | write the single-page OCR text, verified | Save to ocr_out… |
| `illusGallery` | 38438 | folder picker → `runIllustrationGallery` | Illustration gallery… |

**Free functions in the same `#ifdef` block, owned by this pane's surface:**

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `illustrationCandidates` | 37543 | pure geometry: regions not covered by text lines | `drawPage` 38006, `runIllustrationGallery` 37627, OverlayPane selfTest 8034 |
| `runIllustrationGallery` | 37581 | the shared gallery dialog | this pane 38453, OverlayPane 17382/17472 |

**Free functions elsewhere in the file that only this pane's button reaches:**

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `OcrModelInfo` / `kOcrModels` | 435–452 | the five-model table (repo, dir, onnx, description) | model manager, `ocrModelDirFor` |
| `ocrModelDirFor` | 456 | which recognizer directory to use — the pick when installed, else the bundled default | `ScanPane::ensureModels` 37932, InputPane 17599, OverlayPane 36651 |
| `ocrDownloadOne` | 468 | staged, byte-verified single-file download (FAIL-13) | model manager 568/570 |
| `showOcrModelManager` | 521 | the model manager dialog | this pane 37831 (only caller) |

---

## 4. Dependencies

**Data files / folders read** (relative to `root_`, the data root, which is
user-settable):
`library/` (the start directory of both file pickers) ·
`library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx` (line detector, 86 MB
installed) · `library/ocr_models/BDRC_Woodblock/` + `model_config.json`
(bundled recognizer, 25 MB installed) · `library/ocr_models/<picked>/` for the
four downloadable recognizers (none installed in this working copy).

**Written:** `library/ocr_out/<page>-ocr.txt` (single page, 38400) ·
`library/ocr_out/<folder>/<page>-ocr.txt` (batch, 38297–38299) ·
`library/ocr_models/<dir>/{model_config.json,<model>.onnx}` (downloads,
558–571, staged through `<dest>.downloading~`) · illustration crops into any
folder the user picks (37690).

**Libraries:** `allocr::LineDetector` (onnxruntime), `allocr::buildLines` and
`allocr::TextRecognizer` (OpenCV + onnxruntime), `allcore::wylieToUnicode`,
`allcore::SyllableChecker` (passed in by `main()`, 45538).

**Network hosts:** `huggingface.co` — `https://huggingface.co/BDRC/<repo>/
resolve/main/config.json` and `/<model>.onnx` (563–571), through `TimedNam`
with `NoLessSafeRedirectPolicy`. This is the pane's **only** network use;
recognition itself is entirely local.

**QSettings** (`ALL` / `TranslationTool`): `scan/ocrModel` (457–459,
591–599). **Session store** (`sess::`): `ocr/lastImage` (37920, 37897).
Neither checkbox is persisted.

**ctest suites relevant here:** `app_selftest` (the 7 Scan checks at
37719–37760, plus the two global hover gates at 51078/51087 and
OverlayPane's `illustrationCandidates` check at 8041) · `ocr_smoke`
(`ocr/CMakeLists.txt:38` — the allocr pipeline against the canonical BDRC
oracle; needs `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx`,
`build/ocr_ref/` and `library/ocr_models/BDRC_Woodblock`) · `gauntlet_walk`
(reaches this pane's ribbon buttons only as no-op cancel paths, since
`safeGetOpenFileName`/`safeGetExistingDirectory`/`safeGetSaveFileName` all
return empty under `g_harnessRun`, main.cpp:3689–3748).

---

## 5. Release questions

- **This pane is invisible to the house's coverage machinery.** `--sweep OCR`
  exercises **0 controls** (verified this session), `Panes ▸ OCR` mirrors none
  of its commands, none of its buttons can be pinned to Quick Access, and the
  `--screenshots` geometry inquisition audits an empty widget tree — all
  because the ribbon is detached from the pane before those walkers look. Two
  fixes are possible (reparent the source buttons under the pane, or teach the
  three walkers to follow `paneRibbons()`); one of them should land before
  release, because otherwise every future control added here inherits the same
  blindness. **This is the headline finding.**
- **Model downloads have no integrity check.** ~100 MB fetched over the
  network and installed as executable model weights, verified only by byte
  count against what the server sent (468–519). The staging fix (FAIL-13)
  protects the *working* model; it does not establish that the new one is what
  BDRC published. Decide whether a pinned hash per entry in `kOcrModels` is a
  ship condition.
- **The build matrix is untested and can fail to link.** `ALL_HAVE_OCR` is
  gated on onnxruntime alone (`app/CMakeLists.txt:63`), while `buildLines`
  and `TextRecognizer` need OpenCV (`ocr/CMakeLists.txt:26–31`). onnxruntime
  without OpenCV compiles this class and fails at link. Either gate
  `ALL_HAVE_OCR` on both, or prove the configuration is impossible on the
  supported machines.
- **The user manual's OCR chapter is stale on the pane's most consequential
  control.** It documents the deskew checkbox by the wrong label, states the
  wrong default, and quotes a header string the code no longer emits
  (`USER_MANUAL.md:2684–2691` vs main.cpp:37795, 37796, 38119–38121). The
  chapter also frames the pane as "no network" (2668–2670) while the OCR
  models button downloads from Hugging Face. There is a selftest gate that
  every pane has a Help chapter (main.cpp:49365, PASS, 126 chapters) — none
  that a chapter is *true*.
- **Batch OCR runs synchronously on the GUI thread** and overwrites existing
  `-ocr.txt` files without asking, for a whole volume. The single-page path
  was moved to a worker thread in August; the batch path was not. Confirm the
  Stop-between-pages granularity is acceptable, and whether an existing
  output folder should be a prompt.
- **"Save crops…" counts attempts, not successes** (37689–37696): `QImage::
  save()`'s return is discarded and the dialog then reports N saved. This is
  the same defect class the batch writer was explicitly fixed for, in the same
  file, with a comment saying why ("A false COUNT is worse than a false
  sentence"). It lives in shared code, so OverlayPane and ScansPane carry it
  too.
- **The recognition path itself has no pane-level test at all.** Seven
  selftest checks cover open-state transitions, the coordinate map and the
  legality counter; the Run/Stop state machine, the partial-results
  disclosure, the implausible-skew warning (38128–38134), and the
  no-lines-found guidance (38138–38154) are each written carefully and
  exercised by nobody. The battery never loads the ONNX models at all —
  decide whether a guarded live single-page run (models present ⇒ run it,
  absent ⇒ skip loudly) belongs in `--selftest`, or whether this pane's
  recognition path is a manual-pass item for 1.0.
- **Session restore decodes a full folio scan on the launch path**
  (51617, before `win.show()`). Measure it on a real 8000×2000 TIFF before
  shipping; if it costs seconds, defer it to first activation of the pane.
- **Half-built / heuristic surfaces to judge:** the illustration-candidate
  detector (pure geometry, no confidence figure, "candidates only" label
  carried honestly in four places) and the page viewer (no zoom, no fit, no
  rotate, no click-back from image to text — markedly poorer than the
  Overlay's woodblock viewer on the same kind of image).
