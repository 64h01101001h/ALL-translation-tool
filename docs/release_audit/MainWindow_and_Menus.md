# Release audit — `main()`, the main window, and the menu bar

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, lines **35515–41427** (`int main(int argc, char** argv)` to EOF).
Build/test wiring: `/Users/adamderickandrade/ALL-translation-tool/app/CMakeLists.txt`, `/Users/adamderickandrade/ALL-translation-tool/CMakeLists.txt`, `/Users/adamderickandrade/ALL-translation-tool/cmake/AllFixtureTests.cmake`.

## 1. What `main()` sets up, and how the user reaches the window chrome

`main()` is the whole application assembly in one function. In order it: pins `QCoreApplication::applicationName` to "Diamond Cutter Translation Tool" and performs a one-time migration of the legacy `…/ALLTranslationTool` Application-Support folder (35516–35543); sets `g_harnessRun`/`g_sweepActive` from the CLI *before any pane exists* so a headless run can never see the paid-API key (35544–35559); installs a global 250 ms **dialog reaper** that `reject()`s any visible `QDialog` under a harness run (35560–35587); sets the Palatino app font and registers five bundled + five user-disk Tibetan fonts (35588–35646); resolves the data root (`findDataRoot()`, main.cpp:29657) and opens the SQLite spine in a retry loop that offers a folder picker on failure (35652–35685); then loads ~15 optional side layers (spellcheck, RefDict, Mahāvyutpatti, Whitney, colloquial pron, SOAS POS, contractions, Progress, honorifics, idioms, teaching indices, Das/Jäschke page maps, AI glossary, alignment evidence + grammar, 84000) each of which silently degrades to `nullptr` if its file is absent; reads night mode from `QSettings` and applies the app-wide "night chrome, paper page" stylesheet (35706–35760). It then builds the window: a `QMainWindow`, a `HuntPalette` (`g_hunt`), a central `QVBoxLayout` of **Quick Access Toolbar row → ribbon `QStackedWidget` → `QTabWidget`** (35761–35820), adds **22 flat panes** as tabs, and then *re-groups them* (36417–36629) into seven ribbon-bearing group tabs (Read / Translate / Research / Learn / Input / Catalog / Community), each group a hidden inner `QTabWidget` fronted by a ribbon band whose left-hand `RibbonGroup` of big `QToolButton`s is the real pane switcher. Only after that is the **menu bar built** (36742–37429): File, Edit, then one menu per workflow group whose submenus are *generically derived* by walking each pane's `QPushButton`/`QToolButton`/`QComboBox`/`QCheckBox` children, then View and Help. So the user reaches chrome three ways — the ribbon band (mouse), ⌘1…⌘7 / ⌘⇧[ / ⌘⇧] (keyboard, 36959–36991), and the mirrored menu bar (which is also what the Quick Access pins and macOS Help-search index). Everything after the menus is CLI-mode dispatch (`--sweep`, `--survey`, `--teachbench`, `--selftest`, `--pasteprobe`, `--openprobe`, `--screenshots`, `--gauntlet`), geometry restore, drag-and-drop, the status bar, the first-run welcome card, the lifecycle log, and finally `app.exec()`.

---

## 2. User-visible functions

Coverage column: exact `check(...)`/`log <<` assertion string from the `--selftest` block (36393–40470) or the ctest suite name, else **NONE**.

### 2.1 File menu (built at main.cpp:36749)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| File → **Open ACIP File…** (⌘O, `QKeySequence::Open`) | `safeGetOpenFileName` filtered to `*.txt *.act *.inc *.ace`, then `overlay->openFile(f)` | lambda at 36752–36765 | Indirect: `app_selftest` "MenuBar: mirrors the workflow groups"; the open path itself via `overlay->selfTest` | Uses the harness-stubbed file panel (R3); no error path if `openFile` fails |
| File → **Open Recent** (submenu, no shortcut) | Rebuilds on `aboutToShow` from `QSettings "file/recents"`; unreadable entries shown **disabled**, never removed | 36768–36794 | NONE | Reads a list written by `OverlayPane::openFile`; no cap enforced here |
| File → **Dossiers…** | Opens `allcore::DossierStore` over `<root>/library`; lists dossiers with comment counts + glossary tick; anchors `open:` / `save:` / `drop:` reopen at a saved line, re-stamp, or remove; every mutation calls `ds.save()` and warns on failure | 36797–36866 | `app_selftest` → "L6 acceptance: dossier reopens the text at line 37 as one act" | Writes to `<root>/library`; a read-only data folder makes every action fail (warned, at least) |
| File → **Team Activity…** | Renders `teamActivityRows(60)` (main.cpp:3152) — proposals + rulings + comments, newest first | 36868–36892 | `app_selftest` → "Team Presence: 3 events, newest first, all voices present" | Reads the shared proposals folder; empty state names the cause honestly |
| File → **Reload Data Layers** | `reloadApprovedLayers()` + `reloadTeachingIndices(root)`; status-bar message for 6 s | 36894–36909 | Seam proven by "G6 seam: a ruling made NOW is live NOW (and cleanly restored)" | Does **not** reload the spine — the message says so |
| File → **Close Window** (⌘W, `QKeySequence::Close`) | `QMainWindow::close` | 36911–36916 | NONE | On macOS closing the only window quits after `lastWindowClosed` |
| File → **Import Data Release…** | Calls the global `g_importRelease` if set | 36918–36921 | NONE | Silent no-op if the hook was never installed |
| **File → Save** (⌘S) / **Save As…** (⇧⌘S) — ADDED 2026-09-08 (Adam's request) | Routed to the front pane: Manuscript `save()/saveAs()`, Draft `saveDraft()/saveDraftAs()`, Overlay `saveDocument()/saveDocumentAs()`; elsewhere a 6 s status-bar message naming what Save applies to | main.cpp File menu block after Open Recent (`saveRoute`) | Overlay: 4 selftest checks; Draft: 3; Manuscript: existing WP-1 checks. The routing lambda itself is untested | Front pane = focus ancestor, else `isVisible()` |
| **File → New Document** (⌘N) · **New from Template…** (⇧⌘P) · **Close** (⌘W; Close Window moved to ⇧⌘W) · **Save as Template…** · **Move…** · **Rename…** · **Properties…** (⌥⌘P) — ADDED 2026-09-08 (Word parity, Adam) | All routed to the front pane. Overlay: new/close reset the Document box (confirm if dirty), templates in `library/templates/*.txt` + two built-ins, Move/Rename move the file (Rename carries glossary + properties sidecars), Properties = General/Summary/Statistics/Content/Custom with a JSON sidecar in `library/properties/`. Draft & Manuscript: new/close/move/rename/properties (no templates). | `docprops` namespace before OverlayPane; `OverlayPane::newDocument…showProperties`; `DraftPane::newDraft…`; `ManuscriptPane::newManuscript…` | Overlay 10 checks (stats, new, dirty, templates ×2, rename+sidecar, move ×2, revision, round-trip), Draft 3, Manuscript 1. Dialogs themselves return early under the harness | 8 new modal sites, all `g_harnessRun`-guarded; constitution baseline 146→154 |
| **Edit → Paste and Match Formatting** (⌥⇧⌘V) · **Paste Special ▸ as ACIP / as Wylie / as Tibetan script** · **Clear** (was Delete) · **Find ▸ Find… ⌘F · Find Next ⌘G · Find Previous ⇧⌘G · Replace… ⌥⌘F** — ADDED 2026-09-08 | Plain-text paste into the focused editor; Paste Special detects the clipboard's script (Unicode block / looksLikeWylie / else ACIP) and re-expresses it, refusing (status-bar message) when a conversion fails or the reverse engine warns; Find is a non-modal Tool window bound to the last-focused editor, wrapping once, with Replace / Replace All (one undo step) | `editops` namespace (after `docprops`), Edit menu block in `main()` | 6 selftest checks (find/wrap, miss, replace-all count, undo, ACIP↔Wylie↔Unicode conversion, Unicode round-trip) | No new modals. Hunt ⌘K unchanged |
| **File → Reopen with Encoding ▸ · Save with Encoding ▸ · Save All (⌥⌘S) · Print… (⌘P) · Revert File · Close All Files (⌥⇧⌘W)** — ADDED 2026-09-08 (Sublime parity) | Encodings: UTF-8, UTF-8 BOM, UTF-16 LE/BE, Windows-1252, MacRoman, Latin-1 — decode counts undecodable bytes (shown, never guessed), encode REFUSES unmappable characters; the chosen encoding becomes the file's for Save. Save All saves every dirty document across Overlay/Draft/Manuscript. Print → QPrintDialog on the front pane's document. Revert reloads from disk after the discard confirmation. | `enc` namespace; `OverlayPane::reopenWithEncoding/saveWithEncoding/revertDocument/printDocument`; Draft/Manuscript revert/print | 8 selftest checks (tables, refusal, BOM, reopen, re-encode) | Qt6::PrintSupport now linked. No new modals |

### 2.2 Edit menu (36922–37027)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Edit → Undo / Redo / Cut / Copy / Paste / Delete / Select All | `route()` dispatches to whichever of `QPlainTextEdit`/`QTextEdit`/`QLineEdit` holds focus; Delete uses `textCursor().removeSelectedText()` or `del()` | `route` 36923–36931, `add` 36932–36940, actions 36941–37027 | NONE | **All seven are registered with an empty `QKeySequence()`** and `Qt::WidgetShortcut` — the menu shows no ⌘Z/⌘X/⌘C/⌘V/⌘A. Comment says the widgets' native shortcuts still work, but the menu is unlabelled and macOS Edit-menu conventions are not met |

### 2.3 Derived group menus (Read / Translate / Research / Learn / Input / Catalog / Community)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| One top menu per group tab, one submenu per pane | 36992–36999 | 36992–37225 | `app_selftest` → "MenuBar: mirrors the workflow groups (%2 menus)" (asserts `menus.size() == tabs.count() + 4`) | Count assertion is the only structural guard |
| "**Show pane**" in every pane submenu | `g_raisePane(pane)` | 37001–37005 | Same as above | — |
| Every `QPushButton` on the pane, mirrored | Raise pane, then `b->click()` | 37006–37019 | `app_selftest` → "MenuBar: Read > Overlay submenu carries its actions (%2)" (>8) | Mirrors labels blindly — a destructive button becomes a menu item with no confirmation of its own |
| Every `QToolButton`; menu-owning ones become submenus | 37020–37045 | 37020–37045 | Same | — |
| Every `QComboBox` (2–24 items, non-editable) → exclusive checkable submenu, two-way synced | 37046–37085 | 37046–37085 | NONE specifically | Submenu title guessed from tooltip or item 0 — can produce meaningless titles |
| Every `QCheckBox` → checkable action, two-way synced | 37086–37103 | 37086–37103 | NONE specifically | — |

### 2.4 Quick Access Toolbar (★)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| ★ pin menu | Enumerates every group menu → pane submenu → action as `Group>Pane>Action`; toggling writes `QSettings "qat/pins"` | `qatFind` 37107–37122, `rebuild` 37123–37147, `pinMenu` 37148–37196 | NONE | Pins are matched by **display text**; renaming any button orphans the pin silently (`if (!a) continue;`) |
| Pinned buttons strip | Each pinned button triggers the mirror's own `QAction` | 37136–37146 | NONE | Hint label hidden once any pin exists |

### 2.5 View menu (37197 onward)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| View → **Ribbon: show labels** (checkable) | Writes `QSettings "ui/ribbonLabels"`, calls `applyRibbonLabelStyle()` (main.cpp:4239) | 37198–37209 | Indirect: "Fit: no ribbon grew past the recorded worst (%2 px, ratchet 2572)" | — |
| View → **Larger Text** (⌘+ and ⌘=) | Routes zoom to the Input pane's scan if focus is inside it, else `overlay->zoomRouted(+1)` | `route` 37212–37223, action 37224–37228 | NONE | Silently does nothing when focus is in any third surface |
| View → **Smaller Text** (⌘−) | `route(-1)` | 37229–37233 | NONE | — |
| View → **Actual Text Size** (⌘0) | `route(0)` | 37234–37238 | NONE | — |
| View → **Night mode** (checkable, default **on**) | `applyNight(on)` sets `QStyleHints::setColorScheme(Dark/Light)`; persists `QSettings "app/nightMode"` | toggle 37240–37247; `applyNight` lambda 37749–37756 region (defined 35755–35760); initial read 35707–35709 | `app_selftest` → "Night: reading surfaces cream in all modes" | Chrome only — reading surfaces are hard-pinned cream `#FAF6EE` in the app stylesheet. Setting stored under org **ALL / TranslationTool** |
| View → **Hunt Everywhere…** (⌘K) | `g_hunt->openPalette()` | 37248–37254 | `g_hunt->selfTest(log)` (38424) + "Apparatus: the ⌘K hunt lane routes a query into the pane" | Also surfaced as the top-right corner button (36637–36654) |
| View → **Look Up Selection…** (⌘D, ApplicationShortcut) | Pulls the selection from the focused text widget and runs `g_lookupPopup->run(sel)` | 37255–37275 | `app_selftest` → "⌘D popup answers with the full stack" | Runs with an empty string when nothing is selected |
| View → **About Diamond Cutter Translation Tool** (`AboutRole` → macOS app menu) | See §2.5.1 | 37277–37357 | NONE (the battery **count** shown comes from `ALL_TEST_BATTERIES`, guarded at CMakeLists.txt:117–160 BUILD-19) | `WA_DeleteOnClose`, non-modal `show()` |
| View → **Check for Updates…** (`ApplicationSpecificRole`) | See §2.5.2 | 37358–37416 | NONE | **Duplicated** by a second, contradictory Help-menu item (§2.7) |
| View → **Settings…** (⌘,, `PreferencesRole`) | `SettingsDialog dlg(applyNight, &win); dlg.exec();` | 37417–37423 | `SettingsDialog::selfTest` at 38255–38258 | — |

#### 2.5.1 About box contents (37277–37357)

Fixed 520 px wide, cream `#FAF6EE`, all `QLabel`s centred with `TextBrowserInteraction` + `setOpenExternalLinks(true)`:

1. Title "Diamond Cutter Translation Tool" (Iowan Old Style, 24 px, `#7C2D26`).
2. `APP <ALL_APP_VERSION> · BUILT <date> · HGM DICTIONARY DATA v<spine meta release_version>` — build date parsed from `__DATE__` (BUILD-25 comment: says *built*, deliberately not *released*).
3. Divider dots.
4. "Created by **Adam Derick Andrade**" / "(Loppun Pawo, a.k.a. StaticSky)".
5. "for **Geshe Michael Roach** · the Asian Legacy Library · Diamond Cutter Classics".
6. Mission italic: "…with Geshe Michael Roach's English as the binding layer. The machine may match the master's English; it may never compose it."
7. "Developed in C++20 with Qt `qVersion()` · **`ALL_TEST_BATTERIES`** automated test batteries · runs fully offline".
8. `mailto:adam.derick.andrade@gmail.com`.
9. "Third-party components and data sources are credited in OPEN_SOURCE_NOTICES, shipped with every release."

#### 2.5.2 "Check for Updates…" — exact behaviour (37358–37416)

- Reads `QSettings("ALL","TranslationTool") "app/updatesDir"`. If empty or non-existent, asks a `QMessageBox::question` ("Updates are distributed by the ALL team as DMGs in a shared folder (e.g. the team Dropbox)"), then `safeGetExistingDirectory(&win, "Team updates folder")` and **remembers the choice** in `app/updatesDir`.
- **No host and no network access.** It is a plain local/mounted-filesystem directory scan: `QDir(dir).entryList({"*.dmg"}, QDir::Files)` matched against the regex `ALL-Translation-Tool-([0-9.]+)\.dmg`, best version chosen with `versionLess()` (main.cpp:242).
- If a newer version than `ALL_APP_VERSION` exists → `QMessageBox::information` naming version and filename, offering **Open** → `QDesktopServices::openUrl(QUrl::fromLocalFile(dir))` (opens the folder in Finder). Nothing is downloaded, mounted, verified, or installed.
- Otherwise → "You are running version X — the newest in the team folder" (+ "(no DMGs found there)").
- **Filename mismatch risk:** the app is now *Diamond Cutter* Translation Tool but the regex still requires the legacy `ALL-Translation-Tool-<v>.dmg` name. A DMG named after the current product is invisible to this scanner, which then reports "up to date".

### 2.6 Keyboard shortcuts registered outside the menus (36959–36991)

| Shortcut | What it does | Anchor | Coverage |
|---|---|---|---|
| ⌘1 … ⌘7 | `tabs.setCurrentIndex(k)` — jump to a workflow group | 36960–36968 | NONE (documented in Help → Keyboard Shortcuts) |
| ⌘⇧] / ⌘⇧[ | step to next/previous pane inside the current group (`step` lambda) | 36969–36991 | NONE |

### 2.7 Help menu (37425–38050)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Help → **Diamond Cutter Help && Tutorials…** (`QKeySequence::HelpContents`) | Lazily constructs `HelpWindow(&tabs, root, &win)`, shows/raises | 37428–37437 | `app_selftest` → "Help: tutorial chapters for every pane (%2)", "Help: feature auto-index built (%2 entries)", "Help: search finds a feature by name", "Help: browse list populated on open (%2 rows)" | `helpWin` leaks intentionally for the process life |
| Help → **Restore a Shared Store from Backup…** | Lists `proposals.tsv`, `comments.tsv`, `CATALOG_TEAM.tsv` newest backups from `<AppData>/backups`; `QInputDialog::getItem`; `restoreStoreWithSafety()` (main.cpp:2974) backs up the current file first | 37447–37530 | `backup_smoke` (core suite) drills the restore path | Early-returns when `g_sweepActive`; **destructive-ish** but reversible; tells the user to restart |
| Help → **Save Diagnostic Report…** | See §2.7.1 | 37531–37594 | NONE for the menu action; `buildDiagnosticReport` itself lives in `app/textreport.h:55` | Writes to a user-chosen path |
| Help → **Suggested Workflows…** | HelpWindow → `openWorkflows()` | 37595–37603 | `app_selftest` → "Help: Suggested Workflows chapter present" | — |
| Help → **User Manual…** | HelpWindow → `openManual()` | 37604–37612 | "Help: User Manual chapters loaded (%2 chapters total)" | — |
| Help → **Check for Updates…** (second one) | Static `QMessageBox` stating "there is no auto-update and the app never phones home… Ask Adam for the current DMG, or check the team's shared folder." | 37614–37630 | NONE | **Contradicts the View-menu item of the same name**, which does scan a folder and remember it. Two identically-named commands in one menu bar |
| Help → **How This Tool Thinks…** | Static HTML dialog: the seven-group pipeline and the HGM/EVIDENCE/REFERENCE/MACHINE/AI data hierarchy | 37631–37700 | NONE | Hand-written duplicate of the tier doctrine — can drift from the card renderer |
| Help → **Licenses && Data Sources…** | Renders `<root>/docs/distribution/OPEN_SOURCE_NOTICES.md`, falling back to `applicationDirPath()/../../../OPEN_SOURCE_NOTICES.md`; honest message if absent | 37702–37736 | `repo_licensing`, `payload_manifest` (ctest) | The fallback path is DMG-layout-specific |
| Help → **Keyboard Shortcuts…** | Static HTML sheet (⌘K, ⌘1–⌘7, ⌘⇧[/], ⌘=/⌘−/⌘0, ⌘D, Files F2–F8, "standard macOS Undo/Redo/Cut/Copy/Paste/Select All") | 37738–37785 | NONE | Claims standard Edit shortcuts that the Edit menu itself does not display (see §2.2) |
| Help → **Usage Ledger…** | Checkbox writes `QSettings "ui/usageLedger"`; renders counts from the TSV at `<AppData>/usage_ledger.tsv`; **Reveal file** → `/usr/bin/open -R` | 37787–37878 | NONE | Opt-in, local-only, never transmitted (`usage::` at main.cpp:3050) |
| Help → Troubleshooting → **File a Finding…** | Appends a timestamped stub (`WHAT I SAW / WHAT I EXPECTED / DISPOSITION: (pending)`) to `<root>/docs/FINDINGS.md`, then opens it | 37884–37916 | NONE | **Writes into the repo/data folder**; warns via `warnWriteFail` + `streamWriteOk` |
| Help → Troubleshooting → **Show Logs in Finder** | `open -R ~/Library/Logs/DiamondCutterTranslationTool-lifecycle.log`, else opens `~/Library/Logs` | 37917–37927 | NONE | — |
| Help → Troubleshooting → **Show Data Folder in Finder** | `open <root>` | 37928–37932 | NONE | — |
| Help → Troubleshooting → **Copy Installation Info** | Clipboard ← `"Diamond Cutter Translation Tool v… · Qt … · <OS> · data: <root>"` | 37933–37938 | NONE | Leaks the data-root path (may contain the user's name) to the clipboard by request |
| Help → Troubleshooting → **Generate Diagnostic Report…** | See §2.7.2 — a **second, much less private** report | 37940–37962 | NONE | Writes to Desktop unprompted; contains **all** QSettings values and 60 lines of the lifecycle log |
| Help → Troubleshooting → **Report a Problem…** | Opens `mailto:adam.derick.andrade@gmail.com` with a percent-encoded subject/body template | 37963–37982 | NONE | Hands the user's mail client a personal address hardcoded in the binary |
| Help → Troubleshooting → **Storage…** | Sizes `<root>/library/scan_cache`, `<root>/library/.index.db`, `<root>/library/ocr_out`; buttons **Clear scan cache** (`QDir::removeRecursively`) and **Clear search index** (`QFile::remove`) | 37984–38031 | NONE | **Irreversible deletes with no confirmation step** beyond clicking the button in the report box |
| Help → Troubleshooting → **Verbose Logging** (checkable) | `QSettings()` (default org!) `"troubleshoot/verbose"` | 38032–38038 | NONE | Uses the **default-constructed** `QSettings`, not `("ALL","TranslationTool")` — a second settings domain |
| Help → Troubleshooting → **Reset Settings…** | `QMessageBox::question`, then `QSettings().clear(); sync();` | 38039–38050 | NONE | Also default-domain `QSettings` — it therefore does **not** clear `app/nightMode`, `qat/pins`, `ui/geometry`, `files/stack`, `app/dataRoot` etc., which live in `("ALL","TranslationTool")`. The dialog promises "every preference … to defaults" |

#### 2.7.1 "Save Diagnostic Report…" (37531–37594)

- Gathers `QSettings() "scan/recent"` (default domain).
- Builds the report via `buildDiagnosticReport(ALL_APP_VERSION, ALL_GIT_COMMIT or "unknown", QSysInfo::prettyProductName(), QSysInfo::currentCpuArchitecture(), ALL_TEST_BATTERIES, recents)` — `app/textreport.h:55`. Contents: header, the "no text you have written and no file names / nothing is transmitted" statement, version, git commit, macOS version, architecture, test-suite count, and recent activity **reduced to file extensions and counts only**.
- Destination: `safeGetSaveFileName(&win, "Save Diagnostic Report", ~/Desktop/diagnostic-report.txt, "Text (*.txt)")` — user chooses.
- Write judged twice (`open` failure and `f.error()` after flush) via `warnWriteFail`; success dialog suppressed under `g_harnessRun`.
- Nothing is transmitted anywhere.

#### 2.7.2 Troubleshooting → "Generate Diagnostic Report…" (37872–37962)

Distinct from §2.7.1 and far more revealing. `buildDiagnostic` (37871–37939) writes: header + ISO timestamp; installation info **including the absolute data root**; the app binary's absolute path; presence/size/mtime for seven key data files (`build/hgm_spine_v27_2.db`, `library/.index.db`, `data/help/tutorials.md`, `data/help/USER_MANUAL.md`, `data/teaching/teaching_moments_card.json`, `data/das/das_pages.json`, `data/fonts/NotoSerifTibetan.ttf`) plus `library/ocr_models`; `QStorageInfo` free/total GB and root path; **every key/value in the default-domain `QSettings`, truncated to 120 chars each**; and the **last 60 lines of the lifecycle log**. Saved unprompted to `~/Desktop/ALL-Tool-Diagnostic-<yyyyMMdd-HHmmss>.txt` and revealed with `open -R`.

### 2.8 Window chrome, lifecycle and other user-visible behaviour

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Window geometry restore | `QSettings "ui/geometry"` → `restoreGeometry`; first run `resize(1180,760)` + `showMaximized()`; saved on `aboutToQuit` | 40484–40501 | NONE | — |
| Minimum window size | `setMinimumSize(640,480)` then **overridden** to `(820,560)` — the G3 proven-clean floor | 40483 and 40545 | `--screenshots` geometry inquisition (exit 4); gauntlet `resize:` steps | Two different minimums set 60 lines apart; the first is dead |
| Tab bar overflow | scroll buttons on, `ElideRight`, tabs size-policy `Ignored` | 40479–40482 | "Fit: every pane fits an 800px window" | — |
| **Drag & drop a text file onto the window** | `DropRoute` event filter accepts `txt/act/inc/ace`, raises the Overlay and opens the file | class 40503–40542 | NONE | Only the first URL is used; other suffixes silently rejected |
| Session restore of panes | `overlay->restoreSession()`, `inputPane->restoreSession()`, `alignPane->restoreSession()`, `ocrPane->restoreSession()` (OCR only under `ALL_HAVE_OCR`) | 40546–40549, 40601–40603 | "Session: the Input pane resumes its working folder AND page, and ignores a folder that has gone away" | — |
| Last-pane restore | Pane remembered **by name** in `sess "ui/pane"`; restored after grouping | `currentPaneName`/`notePane` 36676–36700, restore 36701–36710 | NONE | `usage::tick("pane", …)` fires here too |
| Status bar | Permanent labels: `release v<spine release_version>`, identity (`<name> (authority)` or "no identity set"), and `<n> proposal(s) pending` for admins | 40551–40600 | "Approval: tab badge shows pending count" (adjacent) | Size grip disabled; padding fix documented |
| First-run **welcome card** | Once only, guarded by `QSettings "ui/welcomed"` and `!g_harnessRun`; loads and sanitises `demoTextPath(root)` into `/tmp/S0134I_welcome.txt`, opens it in the Overlay, floats a dismissible card | 40605–40667 | NONE | Writes a temp file on first launch |
| Lifecycle logging | See §5 | 40941–41141 | NONE | Appends on **every** launch |

---

## 3. Internal functions, lambdas and helpers in or used by this range

Defined *inside* `main()` unless a file-level line number is given.

| Name | Line | Purpose | Called from |
|---|---|---|---|
| legacy-AppData migration block | 35516–35543 | Renames `…/ALLTranslationTool` → `…/Diamond Cutter Translation Tool` once | top of `main()` |
| harness-guard block | 35544–35559 | Sets `g_harnessRun`, `g_sweepActive` from argv before panes exist | top of `main()` |
| `g_reaper` timer lambda | 35573–35586 | 250 ms: `reject()` every visible `QDialog` under a harness run | Qt timer |
| Palatino font block | 35588–35602 | App font selection with fallback | `main()` |
| `applyNight` | 35755–35760 | `styleHints()->setColorScheme(Dark/Light)` | night toggle 37244, SettingsDialog 37421, initial 35760 |
| `paneIcon` | 36444–36462 | Pane title → ribbon icon key | `mkGroup` |
| `mkGroup` | 36463–36578 | Builds one workflow group: inner `QTabWidget` (tab bar hidden), ribbon band, `RibbonGroup` surface buttons, per-pane ribbon `QScrollArea` hosts, `syncBand` | 36579–36592 (seven calls) |
| `syncBand` | 36560–36571 | Keeps the tool `QStackedWidget` and surface-button check state in sync with the inner tab | `currentChanged` + once at build |
| group tooltip block (`kRoles`) | 36594–36629 | Per-group role text + ⌘N and pane-stepping hints | `main()` |
| Hunt corner-button block | 36631–36654 | Top-right "Hunt (⌘K)" button | `main()` |
| `g_raisePane` (global assigned here) | 36655–36664 | Raise a pane: select its group page then the pane in its inner tabs | ~30 call sites app-wide |
| `currentPaneName` | 36676–36684 | Which flat pane is on screen | `notePane` |
| `notePane` | 36685–36692 | `sess::put("ui/pane")` + `usage::tick("pane", …)` | both `currentChanged` signals |
| `route` (Edit) | 36923–36931 | Dispatch an editor op to the focused text widget | `add` |
| `add` (Edit) | 36932–36940 | Register one Edit action with `Qt::WidgetShortcut` | seven Edit actions |
| `step` (pane stepping) | 36969–36980 | ±1 inside the current group's inner tabs | ⌘⇧] / ⌘⇧[ |
| `qatFind` | 37107–37122 | Resolve a `Group>Pane>Action` path to a `QAction` | `rebuild` |
| `rebuild` (shared_ptr<function>) | 37123–37147 | Rebuild the Quick Access strip from `qat/pins` | pin toggles, once at build |
| `route` (View text size) | 37212–37223 | Zoom the Input scan or the Overlay | Larger/Smaller/Actual |
| `add` (About dialog) | 37288–37298 | Append a centred, link-enabled `QLabel` | nine About lines |
| `installInfo` | 37820–37827 | Version/Qt/OS/data-root one-liner | File a Finding, Copy Installation Info, Report a Problem, `buildDiagnostic` |
| `buildDiagnostic` | 37871–37939 | The verbose Desktop diagnostic (see §2.7.2) | Generate Diagnostic Report… |
| `dirSize` | 37986–37995 | Recursive byte count | Storage… |
| sweep `settle` | 38116–38121 | 12 × `processEvents(50 ms)` | every sweep step |
| sweep `note` | 38122–38131 | Print `[OK] <kind> <label>` + any dialogs seen | every sweep step |
| sweep reaper lambda | 38100–38114 | Record + `reject()` modal dialogs during a sweep | Qt timer |
| `lifeLog` | 40946–40955 | Append `<ISO ms>  <msg>` to the lifecycle log | startup, `lastWindowClosed`, `aboutToQuit`, verbose pane switches |
| `DropRoute` (local class) | 40503–40542 | Drag-enter filter + drop → open in Overlay | installed as window event filter |
| screenshots `settle(ms)` | 40780–40786 | Pump events for N ms | every screenshot demo step |
| screenshots `paneByTitle` | 40773–40778 | Flat-pane lookup by title | demo population, Catalog demo |
| geometry-inquisition block (`inScroll`, clip/ovl/out counters) | 40988–41082 | Widget-tree clipping/overlap/escape detector; nonzero → exit 4 | inside the `--screenshots` loop |
| gauntlet `denied` | 41180–41184 | Reject world-facing/destructive control labels | every gauntlet action lap |
| gauntlet `note` | 41262–41269 | Journal + optional `[gNNNN]` stderr trace | every gauntlet step |
| gauntlet `stepper` timeout lambda | 41186–41417 | The random walk: modal closing, structure invariants, then one of group-switch / pane-switch / ribbon tool / checkbox / menu action / word click / resize / lookup / Escape | 15 ms timer |
| gauntlet `post` | 41394–41404 | Synthesise press+release `QMouseEvent`s on the reading viewport | word-click lap |

File-level helpers this range depends on (defined earlier in `main.cpp` or in headers):
`versionLess` (242) · `loadHonorifics` (1974) · `resolveSpinePath` (1993) · `anthropicKey` (2816) · `sess::put/get/str` (2847) · `warnWriteFail` (2869) · `restoreStoreWithSafety` (2974) · `sanDerate` (2989) · `streamWriteOk` (2994) · `usage::on/path/tick` (3050) · `sess::path/remember` (3074) · `g_isAdmin` (3143) · `loadIdentity` (3146 decl) · `teamActivityRows` (3152) · `reloadTeachingIndices` (3211) · `reloadApprovedLayers` (3244) · `safeGetExistingDirectory` (3341) · `safeGetOpenFileName` (3356) · `safeGetSaveFileName` (3384) · `miniIcon` (3993) · `ribbonHost` (4221) · `paneRibbons` (4226) · `ribbonProxies` (4230) · `ribbonLabelsOn` (4234) · `applyRibbonLabelStyle` (4239) · `RibbonGroup` (4313) · `isDataRoot` (29649) · `findDataRoot` (29657) · `HuntPalette` (34579) · `demoTextPath` (35491) · `buildDiagnosticReport` (`app/textreport.h:55`) · `latestBackup`/`backupFile` (allcore).

---

## 4. Pane / tab registration — the ordered list the user sees

**Flat registration order** (35812–36416): each `tabs.addTab(...)` below is later re-parented by `mkGroup`.

| # | Tab label | Class | Line |
|---|---|---|---|
| 1 | Overlay | `OverlayPane` | 35812 |
| 2 | Scans | `ScansPane` | 35814 |
| 3 | Export | `ExportPane` | 35816 |
| 4 | Analysis | `AnalysisPane` | 35817 |
| 5 | Trainer | `TrainerPane` | 35820 |
| 6 | Drills | `DrillsPane` | 35822 |
| 7 | Draft | `DraftPane` | 35824 |
| 8 | Apparatus | `ApparatusPane` | 35937 |
| 9 | Manuscript | `ManuscriptPane` | 35953 |
| 10 | Review | `ReviewPane` | 35955 |
| 11 | Align | `AlignPane` | 35957 |
| 12 | Input | `InputPane` | 35959 |
| 13 | Library | `LibraryPane` | 35973 |
| 14 | Files | `FilesPane` | 35980 |
| 15 | Search | `GoferPane` | 35986 |
| 16 | Convert | `makeConvertPane(mvp, whitney)` | 35987 |
| 17 | Lookup | `makeLookupPane(spine, refdict, mvp, whitney, colloq)` | 36068 |
| 18 | Sanskrit | `SanskritPane` | 36073 |
| 19 | OCR | `ScanPane` (only `#ifdef ALL_HAVE_OCR`) | 36076 |
| 20 | Catalog | `CatalogPane` | 36418 |
| 21 | Propose | `ProposePane` | 36420 |
| 22 | Approval / `Approval (N)` | `ApprovalPane` — **only when `g_isAdmin`** | 36429 |

**Grouped order actually presented** (36579–36592) — seven top tabs, ⌘1…⌘7:

1. **Read** — Overlay, Library, Files, Scans, Export
2. **Translate** — Draft, Manuscript, Apparatus, Review, Align
3. **Research** — Search, Lookup, Sanskrit, Convert, Analysis
4. **Learn** — Trainer, Drills
5. **Input** — Input, OCR
6. **Catalog** — Catalog
7. **Community** — Propose, Approval

Also constructed but not a tab: `LookupPopup lookupPopup` (36070, the ⌘D floating window, `g_lookupPopup`) and `HelpWindow* helpWin` (37426, lazily).

---

## 5. CLI flags parsed by `main()`

Harness detection (35550–35558) sets `g_harnessRun` for: `--selftest`, `--sweep`, `--screenshots`, `--pasteprobe`, `--sanskritcheck`, `--survey`, `--teachbench`, `--gauntlet`. Note `--openprobe` is **absent** from that list.

| Flag | Arguments | What it does | Line | Driven by ctest? |
|---|---|---|---|---|
| *(positional)* `argv[1]` | spine db path (if it doesn't start with `-`) | Overrides `resolveSpinePath(root)` | 35647 | No |
| *(positional)* `argv[2]` | analysis template path | `PASSAGE_ANALYSIS_TEMPLATE.md` override | 35649 | No |
| *(positional)* `argv[3]` | spellcheck data dir | `SyllableChecker` root | 35690 | No |
| `--sanskritcheck` | — | Shapes 23 Tibetanized-Sanskrit stacks in two fonts, reports missing glyph indexes; exits 0/1 | 36293–36390 | **No** |
| `--png <path>` | path | Only with `--sanskritcheck`: renders a 900 px visual sheet PNG | 36355–36389 | No |
| `--sweep <Pane>` | pane title (default `Overlay`) | Redirects `g_proposalsDir` to a temp folder, raises the pane, primes text boxes, then clicks every enabled button, toggles every checkbox, cycles every combo (≤6), and types 4 probes into every line edit, dismissing modals; prints `SWEEP COMPLETE`; exit 0 (2 if the pane isn't found) | 38053–38222 | **No** |
| `--survey <file>` | file | Prints `translatorSurveyMarkdown` + elapsed ms; exit 0/1 | 38224–38235 | **No** |
| `--teachbench <file>` | file | Opens the file, times `overlay->teachingsReportHtml()`; exit 0/1 | 38237–38246 | **No** |
| `--selftest` | — | Runs ~19 pane `selfTest`s plus ~60 in-file checks (Lookup, Search index, Hunt, Working Glossary, Survey, Help, idioms, Das/Jäschke, teachings, Input session, Apparatus, API-key blindness, ~20 Catalog lanes, Team Presence, dossier L6, G6 seam, T4/T4b/T4c perf floors, F1/F2/F4 fidelity, ALIGN-1/ALIGN-2, night cream, pane fit, ribbon ratchet, menu-bar mirror, Approval badge, Files stack, Draft style, Convert TISE/reverse/THL, Drills misses, Files FTP/WebDAV/sync/rename, PERF-9 peak footprint); prints each line and `app selftest: ALL PASS/FAIL (n failure(s))` | 36391, 38248–40477 | **Yes — `app_selftest`** (app/CMakeLists.txt:89; `QT_QPA_PLATFORM=offscreen`, TIMEOUT 300, `LABELS fixture`, `SKIP_REGULAR_EXPRESSION "cannot open spine db"`) |
| `--pasteprobe <text>` | text | Pastes into the Overlay's editable box, clicks "Load into overlay", prints load ms + char counts; exit 0 (1 if the button is missing) | 40668–40696 | No |
| `--openprobe <file>` | file | Times `overlay->openFile`, prints the token hint; exit 0 | 40697–40720 | No; **and it does not set `g_harnessRun`** |
| `--screenshots <dir>` | output dir | Fixed-size window, opens/cleans a demo text, populates Lookup/Search/Sanskrit/Convert/Trainer/Analysis/Apparatus/Catalog demos, grabs one PNG per flat pane, then runs the **geometry inquisition**; exit 0, or **4** on any clip/overlap/escape violation | 36287–36291, 40721–40940 | **No** |
| `--shotsize WxH` | e.g. `820x560` | Only with `--screenshots`: sweep another geometry | 40721–40732 | No |
| `--gauntlet <seed\|auto> <steps>` | seed (`auto` = Julian day) + step count | Deterministic random walk (15 ms tick) over group/pane switches, ribbon tools, checkboxes, menu actions, reading-view mouse clicks, resizes, lookups, Escape; invariants = window visible, tab count, menu count; exit 0 survived, **3** invariant violation or unclosable modal | 41148–41419 | **Yes — `gauntlet_walk`** (app/CMakeLists.txt:92, `--gauntlet auto 300`, offscreen, TIMEOUT 420, `LABELS fixture`, same skip regex) |
| `--gauntlet-verbose` | — | Heartbeat + per-step + modal tracing on stderr | 41199, 41208, 41252 | No |

Environment variables read (harness only): `DCT_SHOT_WORD` (default `SHES RAB`), `DCT_SHOT_NEST`, `DCT_SHOT_CARD_HTML` (writes card HTML + a rendered `.png`) — 40790–40848; `ALL_SANITIZED` via `sanDerate` (main.cpp:2989).

**There is no `--teachings-report` flag.** The nearest thing is `--teachbench`.

Only **two** of the twelve flags are wired into ctest (`app_selftest`, `gauntlet_walk`). The other app-level ctest suites are `dmp_smoke`, `collation_smoke`, `font_smoke` (separate binaries). Core/OCR suite names available for cross-reference include `spine_smoke`, `forward_battery`, `towylie_battery`, `backup_smoke`, `constitution`, `no_vacuous_pass`, `fixture_gate`, `build_manifest`, `link_truth`, `payload_manifest`, `repo_licensing`, and ~90 more.

---

## 6. Global singletons and window lifecycle

- **`g_hunt`** (`static HuntPalette*`, main.cpp:35198) — allocated at 35762 as `new HuntPalette(spine, root, &win)`, parented to the window. Used by the ⌘K action, the corner Hunt button, and `--selftest` (`g_hunt->selfTest(log)` at 38424).
- **`g_harnessRun`** (main.cpp:35544–35558) — set from argv *before* any pane construction (the comment records the near-miss: a sweep would otherwise have seen the Anthropic key and fired paid API calls). It gates the dialog reaper, the launch backups, `sess::` writes, `usage::` ticks, the welcome card, and the diagnostic success dialog. `--selftest` pins the invariant: `"Safety: harness runs never see the Anthropic API key"` (38781–38789). It is temporarily flipped to `false` inside the Input-session selftest (38650–38700) and restored.
- **`g_sweepActive`** (2802, set 35558) — the narrower "never dialog" guard, checked by Restore-from-Backup at 37449.
- Other globals assigned by `main()`: `g_spineForAbout` (35806), `g_surveyFile` (35763), `g_raisePane` (36655), `g_sendToManuscript` / `g_manuscriptText` / `g_mssComposeBib` / `g_mssProposeNote` (35944–35952), `g_apparatusQuery` (35938), `g_openScanInInput` (35965), `g_openAtLine` (35969), `g_goferQuery` (35982), `g_lookupPopup` (36071), `g_registersRoot` / `g_honorifics` / `g_honorificsLive` (35988–35993), `g_idioms` (36008), `g_dasSections` / `g_dasPdfPath` / `g_dasHeadwords` (36024–36042), `g_jaeSections` / `g_jaePdfPath` (36060–36065), `g_spellingValid(Live)` (36121), `g_pronApproved(Live)` (36143), `g_aiGlossary` (36180), `g_alignEvidence` (36219), `g_alignGrammar` / `g_alignGrammarIx` (36262), `g_84000` (36285), `g_tm84000Root` / `g_tolRoot` (36286), `g_isAdmin` / `g_proposalsDir` / `g_userName` / `g_identityPinned` (36395–36400 for harness runs; `loadIdentity()` at 36078 otherwise), `g_appNotes` / `g_appBib` (35934). Most of these point at function-local `static` storage inside `main()` and live for the process.
- **Window lifecycle:** `QMainWindow win` is a stack object in `main()`. Geometry restored from `ui/geometry` and saved on `QApplication::aboutToQuit` (40484–40501). File → Close Window is `QKeySequence::Close`. `lastWindowClosed` and `aboutToQuit` are both logged (41120–41129). There is **no** `closeEvent` override, no "unsaved work" prompt, and no explicit Quit action of the app's own (macOS supplies it; the gauntlet's deny-list excludes anything matching "Quit").
- **Startup/lifecycle logging (every launch, 40941–41141):** appends to `~/Library/Logs/DiamondCutterTranslationTool-lifecycle.log`, one line per event, `ISODateWithMs + "  " + msg`:
  - `startup ok — entering event loop (pid N)` on every real launch;
  - `lastWindowClosed fired`;
  - `aboutToQuit — window visible=X active=Y`;
  - `pane: <group tab text>` on every group-tab change **when `troubleshoot/verbose` is on**.
  The file is never rotated or size-capped and the append failure is silently ignored. Note the log block sits *after* every CLI early-return, so harness modes write nothing.
- **Launch backups (35948 region, 36080–36105):** on every non-harness launch, `allcore::backupFile` copies `proposals.tsv`, `comments.tsv` and `<officialRoot>/CATALOG_TEAM.tsv` into `<AppData>/backups` with a `yyyy-MM-dd-HHmmss` stamp, keeping 14.

---

## 7. Dependencies

### Data folders and files READ (all relative to the resolved `root`)
`build/hgm_spine_v27_2.db` (or the name in `build/spine_current.txt`) · `build/reference.db` · `build/weird_top.tsv` · `data/extracted/verb_lemmas.tsv` · `data/fonts/{NotoSerifTibetan,BabelStoneTibetanSlim,Jomolhari-Regular,MonlamBodyig-Regular}.ttf` · `~/Library/Fonts/{tibusrfa2,TibetanChosgyalUni_2014-12-23-shipped,TibetanCalligraphicUnicode.1.0,TibetanSambhotaYigchung,TibetanUnicode}.ttf` · `data/spellcheck/` · `data/extracted/mahavyutpatti.tsv` · `data/whitney/whitney_roots.tsv` · `data/pron_colloquial/colloquial_pron.tsv` · `data/soas_pos/classical-lexicon.txt` · `data/extracted/bsdus_tshig.tsv` · `progress.db` · `data/extracted/apparatus_notes.json` (fallback `mixed_nuts_notes.json`) · `data/extracted/apparatus_anchors.json` · `data/extracted/apparatus_bibliography.json` (fallback `mixed_nuts_bibliography.json`) · `data/honorifics/honorific_register.tsv` · `data/idioms/idioms.json` · `data/teaching/teaching_moments_card.json` · `data/extracted/das_pages.json` · `data/extracted/das_headwords.tsv` · `data/das/das_1902_bookmarked.pdf` · `data/extracted/jaeschke_pages.json` · `data/das/jaeschke_1881.pdf` · `data/ai_glossary/ai_glossary.json` · `data/alignment/alignment_evidence_v1.json` · `data/alignment/alignment_full_v1.json` · `data/84000/g84000.json` · `library/.index.db` · `library/` (dossiers, glossaries, demo texts, `library/tengyur` for the Catalog demo) · `docs/distribution/OPEN_SOURCE_NOTICES.md` (fallback `<appdir>/../../../OPEN_SOURCE_NOTICES.md`) · `data/help/tutorials.md`, `data/help/USER_MANUAL.md` (via HelpWindow; also probed by the verbose diagnostic) · `docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md`.

### Files/folders WRITTEN
- `<AppData>/backups/` — dated backups of `proposals.tsv`, `comments.tsv`, `CATALOG_TEAM.tsv` (every real launch, keep 14).
- `<AppData>/usage_ledger.tsv` — only when `ui/usageLedger` is on.
- `~/Library/Logs/DiamondCutterTranslationTool-lifecycle.log` — every real launch (append, never rotated).
- `~/Desktop/ALL-Tool-Diagnostic-<stamp>.txt` — Troubleshooting → Generate Diagnostic Report.
- User-chosen path (default `~/Desktop/diagnostic-report.txt`) — Help → Save Diagnostic Report.
- `<root>/docs/FINDINGS.md` — appended by File a Finding.
- `<root>/library/…` — dossier store saves; `library/glossaries/*.tsv` read.
- **Deletes:** `<root>/library/scan_cache` (recursive) and `<root>/library/.index.db` — Troubleshooting → Storage.
- Restores: shared store files overwritten by Restore-from-Backup (current file backed up first).
- Temp files: `/tmp/S0134I_welcome.txt` (welcome card), `/tmp/S0134I_inc_t.txt` (screenshots), `/tmp/all_sweep_passage.txt`, `/tmp/all_sweep_proposals/`, plus ~12 selftest probe folders under `QDir::temp()` (`all_input_probe`, `all_catalog_probe`, `all_catalog_demo`, `all_presence_probe`, `all_dossier_accept`, `all_seam_probe`, `all_survey_fixture.txt`, `all_files_selftest_probe.txt`) — all removed at the end of their blocks.
- The app-data folder rename (legacy `ALLTranslationTool` → pinned name) at first launch after the rename.

### Network hosts touched
**None from this range.** `Check for Updates…` (View) reads a local/mounted directory only. `Report a Problem…` hands `mailto:adam.derick.andrade@gmail.com` to `QDesktopServices`. The Catalog selftest exercises a `https://library.bdrc.io` link **dispatch** through an injected opener, not a real request. Card link-outs to `online.adarshah.org`, `bdrc.io`, `youtube.com` are strings rendered in HTML for the user to click. The Anthropic API is reachable elsewhere in the file (`anthropicKey()` at 2816, request at 3746) but is provably invisible under any harness run.

### Team-folder paths
- `QSettings "app/updatesDir"` — an arbitrary user-chosen directory ("e.g. the team Dropbox") scanned for `ALL-Translation-Tool-*.dmg`.
- `g_proposalsDir` — the shared proposals folder (`proposals.tsv`, `comments.tsv`); defaults to `<root>/data/proposals` under harness modes.
- `sess::path("catalog/officialRoot")` — the official catalog tree holding `CATALOG_TEAM.tsv`, `AWAITING APPROVAL/`, the roster and the action ledger.

### QSettings keys

Organisation **"ALL" / application "TranslationTool"** (explicit):

| Key | R/W | Where |
|---|---|---|
| `app/dataRoot` | R/W | `findDataRoot` 29659; spine-recovery picker 35682 |
| `app/nightMode` | R/W | 35708 (read), 37245 (write) |
| `app/updatesDir` | R/W | 37361, 37375 |
| `file/recents` | R | 36775 (written by `OverlayPane::openFile`) |
| `qat/pins` | R/W | 37129, 37154, 37183 |
| `ui/ribbonLabels` | W (R via `ribbonLabelsOn`, 4234) | 37205 |
| `ui/usageLedger` | R/W | 37793, 37761; `usage::on` 3052 |
| `ui/geometry` | R/W | 40487, 40499 |
| `ui/welcomed` | R/W | 40608, 40663 |
| `ui/pane` (as `sess/ui/pane`) | R/W | 36687, 36701 |
| `files/stack` | R/W (selftest) | 40437–40465 |
| `scan/ocrModel` | R/W (selftest) | 38334–38341 |
| `sess/input/{mode,folder,page}` | R/W (selftest) | 38660–38690 |
| `sess/catalog/intakeDir` | R (selftest) | 39707 |
| `sess/*` generally | R/W | `sess::` namespace, 2847 / 3074 — suppressed entirely under `g_harnessRun` |

Organisation **default (empty `QSettings()`)** — a *second, separate* domain:

| Key | R/W | Where |
|---|---|---|
| `scan/recent` | R | 37535 (Save Diagnostic Report) |
| `troubleshoot/verbose` | R/W | 38034, 38037, 41134 |
| *(all keys)* | R | 37913 — the verbose diagnostic dumps `QSettings().allKeys()` |
| *(all keys)* | W (clear) | 38047 — Reset Settings… |

---

## 8. Release questions — for the owner to decide or verify by hand

- **Two "Check for Updates…" commands** exist in one menu bar (View 37358, Help 37614) and they say opposite things: one scans and remembers a team folder, the other states flatly that updates arrive only by asking Adam. Decide which ships and delete the other.
- **The update scanner's regex is stale.** It matches `ALL-Translation-Tool-<version>.dmg` only. Verify the actual DMG filename produced by `tools/package_macos.sh`; if it now carries the Diamond Cutter name, this feature silently reports "up to date" forever.
- **"Reset Settings…" almost certainly does not do what it says.** It clears the *default-domain* `QSettings`, while night mode, geometry, QAT pins, data root, recents, welcome flag, drop stack and every `sess/*` key live in `("ALL","TranslationTool")`. Same split affects `troubleshoot/verbose`, `scan/recent`, and the verbose diagnostic's settings dump. Decide on one domain and migrate.
- **Two diagnostic reports with very different privacy profiles.** Help → Save Diagnostic Report is carefully redacted and matches `docs/PRIVACY.md`. Troubleshooting → Generate Diagnostic Report writes the absolute data root, the binary path, disk layout, **all** settings values and 60 lines of the lifecycle log to the Desktop unprompted. Confirm both are intended to ship, and that the second's contents are acceptable to hand to a tester.
- **Storage… deletes without a second confirmation.** "Clear scan cache" is `QDir::removeRecursively` on `<root>/library/scan_cache` and "Clear search index" removes `library/.index.db` (a 2.36 GB rebuild measured in minutes) from a plain button in an information box. Decide whether these need a confirm step.
- **The Edit menu shows no shortcuts.** Every Edit action is registered with an empty `QKeySequence`, yet Help → Keyboard Shortcuts advertises "standard macOS Undo/Redo/Cut/Copy/Paste/Select All". Verify by hand on a real build that ⌘Z/⌘X/⌘C/⌘V/⌘A reach text widgets, and consider labelling the menu.
- **The lifecycle log grows unbounded** and has no rotation. Confirm that is acceptable for long-lived installs; it is also quoted (60 lines) into the Desktop diagnostic.
- **Quick Access pins are matched by display text.** Renaming any button or pane orphans a user's pins with no notice. Decide whether pins need stable identifiers before the feature is advertised.
- **Ten of the twelve CLI modes have no ctest suite.** `--sweep`, `--survey`, `--teachbench`, `--screenshots` (including the geometry inquisition, which can exit 4), `--sanskritcheck`, `--png`, `--pasteprobe`, `--openprobe`, `--shotsize`, `--gauntlet-verbose` are all run by hand only. If the geometry inquisition and the sanskrit glyph check are release gates, register them.
- **`--openprobe` does not set `g_harnessRun`,** unlike every other probe flag. Confirm that is deliberate — it means the reaper, the API-key blindness and the `sess::`/`usage::` suppression are all off during that run.
- **`win.setMinimumSize` is called twice** (640×480 at 40483, then 820×560 at 40545). Confirm 820×560 is the intended floor and drop the dead first call.
- **File → Import Data Release… is a silent no-op** if `g_importRelease` was never installed. Verify the hook is wired in a shipped build.
- **The About box quotes `ALL_TEST_BATTERIES`** as "automated test batteries". The BUILD-19 gate at `CMakeLists.txt:117–160` protects it, but a build configured with `-DALL_ALLOW_REDUCED_BATTERY=ON` would let the About pane overstate. Confirm no release build uses that escape hatch.
- **Menu mirroring is generic and unfiltered** — every `QPushButton` on every pane becomes a menu item, including destructive ones, with no deny-list (unlike the gauntlet, which has `kDeny`). Decide whether the mirror needs the same list.
- The **status bar and About box both quote the spine's `release_version`**; confirm the shipped DMG's data folder carries v0.27.2 so the F4 exact pin (`n == 105634 && hgm == 12004`) stays live rather than falling through its version gate.
