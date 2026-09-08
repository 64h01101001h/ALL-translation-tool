# Release audit — `ScansPane` (app/main.cpp:15785–15868)

## 1. What it is and how it is reached

`ScansPane` is the woodblock-scan utility pane (Adam's 2026-08-12 reorg comment at
main.cpp:15781): a thin launcher that owns no scan logic at all, only four buttons and a
status line, every one of which delegates to the `OverlayPane*` handed to its
constructor. It is a plain `QWidget` in the main tab widget, constructed in `main()` at
app/main.cpp:15852 (`new ScansPane(overlay)`) and added immediately after the Overlay tab
as `tabs.addTab(scansPane, "Scans")` (main.cpp:15853 — i.e. the Read group, sitting
between "Overlay" and "Export"). The user reaches it by clicking the **Scans** tab; it
operates on whatever document is loaded in the Overlay, and `showEvent` refreshes the
status label every time the tab is raised.

## 2. User-visible functions

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Banner label (top) | Static explanatory HTML: what the pane is, and that the cursor-following page lives in the Overlay | ctor main.cpp:15789–15795 | NONE | Word-wrapped `QLabel`; no interaction. |
| Status line (gold, `ux::chromeGold()`) | Shows `overlay_->scansStatus()`: document name · linked volume · current folio · count of cached pages, or "no document loaded" | `refresh()` main.cpp:15864; `scansStatus()` main.cpp:14423 | `"Scans: pane bound to the Overlay's scan state (%2)"` — main.cpp:15852-ish, the single assertion in `selfTest` (main.cpp:15845–15856); it only asserts the text is **non-empty** | Weak assertion: any non-empty string passes, including a wrong one. Themed via `ux::themedStyle` so it survives night-mode. |
| Button **"Find scans on BDRC / follow along…"** | `overlay_->scansAction()` — if `titleSearchMode_` opens the BDRC title-search dialog, else arms follow-along; then raises the Overlay pane via `g_raisePane` | ctor 15835–15836; `scansAction()` main.cpp:14415 | NONE for the button; BDRC URL construction is covered by `check(bdrcScanUrlChecked(...))` at main.cpp:6058 and 6065 (OverlayPane selfTest) | **Network**: queries `purl.bdrc.io` / `iiifpres.bdrc.io` / `library.bdrc.io` (main.cpp:9481, 9553). Errors surface through `bdrcErrorLine` (main.cpp:~2960) which distinguishes 401/403/404/429/5xx. Button leaves the pane you clicked it from (raises Overlay) — a jump the label does warn about. |
| Button **"Four-layer view (this folio)…"** | `overlay_->fourLayerView()` — opens the folio in four synced layers (scan · OCR · e-text · attested English) | ctor 15837–15838; `fourLayerView()` main.cpp:12478 | NONE (no `check(...)` in any selfTest names the four-layer view) | Guarded: with no `basePx_` or `curFolio_` it shows an information box telling the user to start Follow-along first. Parses `@0*(\d+)([AB])` folio markers out of the Overlay's text. |
| Button **"Illustration gallery (cached scans)"** | `overlay_->galleryCached()` — sweeps already-downloaded pages for miniatures/diagrams; candidates only | ctor 15839–15840; `galleryCached()` main.cpp:14294 | NONE | Guarded by a "No linked volume" information box when `scanCache_`/`scanWork_` are empty. Reads the on-disk scan cache. |
| Button **"Illustration gallery (whole volume)…"** | `overlay_->galleryWholeVolume()` — fetches every folio side of the linked volume (reduced size, cached), with a `QProgressDialog` + Stop, then sweeps all of them | ctor 15841–15842; `galleryWholeVolume()` main.cpp:14326 | NONE | **Heaviest action in the pane**: bulk network fetch from BDRC IIIF, unbounded by volume size, plus `QDir().mkpath` and many image writes into `volCacheDir()`. `fetchFails` is counted; confirm it is surfaced. Long-running; cancellable. |
| Raising the tab (showEvent) | Re-reads the Overlay's scan state into the status line | `showEvent` main.cpp:15859–15862 | Exercised implicitly by `selfTest` calling `refresh()` | Correct belt-and-braces: the pane cannot show a stale volume/folio. |

There are **no** checkboxes, combos, line edits, menu actions, context menus, shortcuts,
drag/drop, double-click handlers or anchor schemes in this class — the buttons are the
whole surface.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ScansPane(OverlayPane*)` (explicit) | 15787 | Builds banner, status label, and the four buttons via the local `mk` lambda (button + grey description label, 34px min height); connects each to an Overlay method | `main()` main.cpp:15852 |
| `mk` (ctor-local lambda) | 15801–15811 | Makes a titled button plus its wrapped grey caption and adds both to the layout | ctor, ×4 |
| `selfTest(QStringList&)` | 15845 | Calls `refresh()` and asserts the status label is non-empty; returns fail count | `main()` selftest driver, main.cpp:38255 (`fails += scansPane->selfTest(log);`) |
| `showEvent(QShowEvent*)` | 15859 | Refresh-on-raise | Qt |
| `refresh()` | 15864 | `status_->setText(overlay_->scansStatus())` | ctor path (`selfTest`), `showEvent` |
| `overlay_` (member) | 15865 | Non-owning Overlay pointer; never null-checked in this class | all handlers |
| `status_` (member) | 15866 | The gold status label | `refresh`, `selfTest` |

## 4. Dependencies

- **Owns no data.** Everything is reached through `OverlayPane`.
- **Files/folders read (via Overlay)**: the scan cache directory (`scanCache_`/`scanWork_`,
  `volCacheDir()`), the Overlay's loaded document text, OCR review material for the
  four-layer view.
- **Files written (via Overlay)**: `galleryWholeVolume()` does `QDir().mkpath(dir)` and
  writes fetched folio images into the volume cache.
- **Network hosts (via Overlay)**: `purl.bdrc.io` (main.cpp:9481), `iiifpres.bdrc.io`
  (main.cpp:9553), `iiif.bdrc.io` thumbnails (`iiifThumbUrl`, main.cpp:4500),
  `library.bdrc.io` for the human-readable record links.
- **QSettings**: none read or written by this class. It registers no `sess::` key —
  "which scan was last opened" is remembered by the Overlay, not here.
- **Theming**: `ux::themedStyle` + `ux::chromeGold()`.
- **ctest**: the class is only reached by `app_selftest`
  (`add_test(NAME app_selftest COMMAND DiamondCutterTranslationTool --selftest)`,
  app/CMakeLists.txt:89). No dedicated core suite exists for it.

## 5. Release questions

- Three of the four buttons have **no automated coverage at all**. Decide whether the
  whole-volume gallery (network + bulk disk writes) ships without a driver-level test.
- The one existing assertion passes on any non-empty string. Should it assert the *shape*
  of `scansStatus()` the way `ExportPane` asserts "working document: " / "none loaded"?
- `overlay_` is dereferenced unguarded in all four button lambdas and in `refresh()`
  (`ExportPane` null-checks the same pointer). Confirm the construction order in `main()`
  can never hand this a null — or add the check.
- Manually verify each button's empty-state path: no document, document but no linked
  volume, linked volume but no folio outline. Each should produce a message, never a
  no-op.
- Whole-volume fetch: confirm the page cap / progress / Stop behaviour on a large volume,
  what happens on a mid-fetch network drop, and that `fetchFails` is reported to the user
  rather than only counted.
- Confirm BDRC use is acceptable for offline/air-gapped input-center installs, and that
  the rate-limit (429) message from `bdrcErrorLine` actually reaches this pane's user.
- Confirm the "returns you to the Overlay" jump (`g_raisePane`) is the desired behaviour
  rather than a surprise tab switch.
