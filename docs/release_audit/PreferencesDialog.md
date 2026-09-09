# Preferences — release audit

## 1. Purpose and how it is reached

`PreferencesDialog` (app/main.cpp, in place of the former `SettingsDialog`) is the Word-style Preferences window Adam asked for on 2026-09-08: an icon grid of pages, a search box that narrows the grid, one page open at a time with "◀ Show All" to return, OK / Cancel / Apply. Reached from View ▸ Preferences… (⌘, — `QAction::PreferencesRole`, so macOS puts it in the app menu). Under the harness the action returns early. Every control is bound to a `QSettings` key that some code reads; pages that only display (Key Bindings) say so.

## 2. Pages

| Page | Controls → keys | Applies live? | Coverage | Notes |
|---|---|---|---|---|
| General | Night mode `app/nightMode` · ribbon labels `ui/ribbonLabels` · show the welcome tour again (`ui/welcomed` inverted) | night mode via `applyNight`; labels at next ribbon build | selftest | — |
| View | line numbers `view/lineNumbers` · word wrap `view/wordWrap` · Overlay displays text as `overlay/scriptMode` | numbers/wrap via `NumberedEdit::setAll*`; script mode at next start (as before) | selftest (keys) | — |
| **House Style** (Adam: every style-sheet rule a switch) | `style/twoSpacesSentence` · `style/twoSpacesColon` · `style/oneSpaceSemicolon` · `style/applyOnSave` · `style/live` | read on each save / keystroke by `editops::HouseStyle::fromSettings` | selftest: switches written and read back by the engine | Colon/semicolon defaults await the sheet; more rules join this page as the sheet is supplied |
| Lookup | `lookup/showRefs` · `lookup/stardictDir` (Choose…) | next lookup | — | HGM entry always shown and binding |
| Compare | default rules → `compare/defaultOptions` (JSON) | new comparisons (the Compare pane reads it at construction) | — | RULES in the pane change the current comparison only |
| Team | `team/name` · `team/admin` · `team/proposalsDir` (Choose…) | next proposal | selftest (name) | — |
| File Locations | `app/dataRoot` (Choose…) · `app/updatesDir` (Choose…) · Show in Finder for templates / snippets / compare sessions / properties | data root at next start | — | — |
| Privacy | `ui/usageLedger` (named store) · `troubleshoot/verbose` (default store — matches its readers) | next event | — | Text states nothing leaves the computer |
| Colour Scheme | Day / Night radio mirrored onto the General page's night-mode switch | via General | — | Installable schemes deliberately not offered: no theme may recolour a provenance label |
| Key Bindings | read-only table built from the live menu bar; filter; Copy as text | — | selftest: > 40 rows when a menu bar is supplied | Editing bindings not offered yet (says so) |
| Quick Access | pins list `qat/pins` with Remove | next start / QAT rebuild | — | — |

## 3. Honesty
- Nothing decorative: each control writes a key with a reader.
- The old `SettingsDialog` selftest (write path) is superseded by `PreferencesDialog::selfTest` (10 checks: page count, search, navigation, writes, engine read-back, key bindings).
