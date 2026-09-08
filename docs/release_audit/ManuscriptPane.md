# ManuscriptPane — release audit

`app/main.cpp` lines 30835–31296 (class body), header comment at 30830.

## 1. What it is

ManuscriptPane is the translator's **writing surface** — the pane where the finished translation is actually composed, as opposed to the Draft/Workbench pane where the source is understood. It is a rich-text editor (`QTextEdit`, `setAcceptRichText(true)`) with a ribbon (MANUSCRIPT / TYPE / HOUSE STYLES / VIEW groups), a collapsible Gofer corpus-search sidebar whose hits carry an `insert` link that drops the master's attested English at the cursor, a PUBLISH row that hands work to the Draft pane's bibliography/footnote composers, and a status line. Manuscripts are stored as HTML (lossless Qt round-trip); RTF export shells out to macOS `textutil`. It is constructed at `main.cpp:35864` and added to the main window's top-level tab bar as the tab **"Manuscript"** at `main.cpp:35953`, sitting between "Apparatus" and "Review". The user reaches it by clicking that tab, or automatically when another pane calls `g_sendToManuscript` (wired at 35942–35945), which sets the text and raises the pane — the Draft pane's "ladder" hand-off does exactly this.

## 2. User-visible functions

| Control/action | What it does | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| "Open manuscript…" button (ribbon MANUSCRIPT, big) | Opens `safeGetOpenFileName` filtered to `Manuscript (*.html)`; if a name comes back calls `openFile(fn)` | ctor lambda, 31058–31063 | Indirect only — `openFile` is exercised by "manuscript saves and reopens" (31251), but the dialog path is not | `openFile` **returns silently** if `QFile::open` fails (31137): a corrupt/unreadable file produces no message at all. Disk read. |
| "Save" button | `save()` — writes `editor_->toHtml()` UTF-8 via `saveOrWarn`; on failure sets "NOT SAVED — <path>" and keeps `dirty_` | ctor 31064; `save()` 31148–31165 | `"manuscript saves and reopens"` (31251) and `"a failed save keeps dirty_ set and never claims 'saved' (WP-1)"` (31268) | Disk write. Well covered; failure path deliberately hardened (WP-1). |
| "Save as…" button | `saveAs()` — `safeGetSaveFileName`, appends `.html`, records `manuscript/lastFile`, then `save()` | ctor 31065; `saveAs()` 31167–31177 | NONE for the dialog path (selfTest sets `path_` directly) | Writes QSettings **before** the save is known to succeed — a failed save still repoints `manuscript/lastFile` at a file that may not exist. |
| "Export RTF…" button | If unnamed, forces `saveAs()`; if dirty, saves; asks for an `.rtf` path; runs `QProcess textutil -convert rtf <html> -output <rtf>` with a 15 s wait; status reports success or "RTF export failed — is this a Mac?" | ctor 31066; `exportRtf()` 31179–31196 | NONE | **macOS-only** external process; contradicts the project's cross-platform (Windows/Linux input-centre) goal. `waitForFinished` timeout is not distinguished from a non-zero exit — both render the same message. No check that `textutil` exists. Disk write + process spawn. |
| "B" toggle button (TYPE) | Toggles bold on the selection and the current char format | ctor 31083–31087 | `"bold applies to the selection"` (31237) | Checkable buttons never re-sync to the cursor's actual format — moving the caret into bold text leaves B unchecked. |
| "I" toggle button | Toggles italic on the selection | ctor 31088–31090 | NONE | Same stale-state issue as B. |
| "U" toggle button | Toggles underline on the selection | ctor 31091–31094 | NONE | Same stale-state issue. |
| Font-size spin box (9–48, default 14) | Sets font point size on the selection | ctor 31101–31105 | NONE | Does not reflect the selection's actual size. |
| HOUSE STYLES "Body" | Clears block margins, sets Normal weight, non-italic, base point size, on the selected paragraphs | `styleBtn` 30895–30923 | NONE | Whole gallery is uncovered. |
| HOUSE STYLES "Heading" | Top margin 14, bold, non-italic, base+5 pt | 30924–30937 | NONE | — |
| HOUSE STYLES "Section" | Top margin 10, bold, base+1 pt, 106 % letter spacing | 30938–30954 | NONE | — |
| HOUSE STYLES "Verse" | Left margin 28 / right 20, non-italic, base pt | 30955–30971 | NONE | — |
| HOUSE STYLES "Colophon" | Left margin 36, top 8, italic, base−1 pt | 30972–30986 | NONE | — |
| "Search sidebar" checkbox (VIEW, default on) | Shows/hides the corpus sidebar widget (`side_->setVisible`) | ctor 30987, 31056–31057 | NONE | State is not persisted across sessions. |
| "Compose bibliography entry…" button (PUBLISH row) | Calls global `g_mssComposeBib()` → `draftPane->composeBibDialog()` (wired 35949) | ctor 30996, 31013–31016 | NONE | Null-guarded; if the hook is unset the button is **silently inert** — no feedback to the user. |
| "Propose footnote from selection" button | Calls `g_mssProposeNote(selectedText())` → `draftPane->proposeNoteText()` (35950) | ctor 31000, 31017–31020 | NONE | Same silent-inert risk. Also fires with an **empty selection** — no guard. |
| Gofer query line edit | Free-text Gofer query; session-remembered under key `manuscript/query`; Return runs the search | ctor 31035–31040, 31107 | `"Gofer sidebar reaches the corpus"` (31243, sets text + calls `find()`) | Return-pressed connection itself untested. |
| "Find in corpus" button | `find()` — `allcore::goferSearch(spine_, q, "", 30)`, renders windows with course, wylie, English, and an `insert` link per segment | ctor 31106; `find()` 31228 area / 31198–31218 | `"Gofer sidebar reaches the corpus"` (31243); engine itself by ctest **gofer_smoke** (core/CMakeLists.txt:119) | Result cap hard-coded at 30, unstated in the UI. Parse errors are rendered as "Query not understood: …" (caught `std::exception`) — reasonable, but any engine failure is reported the same way. |
| `ins:<n>` link click in results | `anchorClicked` handler: parses index after `ins:`, bounds-checks against `inserts_`, inserts the English at the cursor | ctor 31109–31118 | NONE | Only `ins:` is honoured; other schemes silently ignored (`setOpenLinks(false)` so no external navigation). This is the pane's only link scheme. |
| Typing in the editor | `textChanged` marks `dirty_ = true` | ctor 31105–31106 | Indirect via WP-1 check | — |
| Autosave (every 60 s when named and dirty) | `QTimer` fires `save()` | ctor 31121–31128 | NONE | Timer starts unconditionally and runs for the app's lifetime; a repeatedly failing autosave can pop `saveOrWarn`'s warning **once a minute**. |
| Session restore on construction | Reads `manuscript/lastFile` from QSettings and, if the file exists, `openFile`s it | ctor 31129–31134 | NONE (selfTest removes the key at 31283 to avoid polluting it) | Reopens a user file at startup with no prompt. |
| Text hand-off from other panes | `setManuscriptText()` replaces the editor content as **plain text** and raises the pane | 31220–31222; wiring 35942 | DraftPane selfTest: `"ladder hand-off: the Manuscript now holds exactly the draft that was sent"` (19931) | **Destructive**: overwrites the entire manuscript, including unsaved rich text, with no confirmation and no undo checkpoint. Highest-risk item in the pane. |
| Keyboard shortcuts | None defined in the class (`setShortcut` does not appear in the range) | — | NONE | No ⌘S; Save is mouse-only within this pane. |
| Context menu / drag-drop / double-click | None overridden (`customContextMenu`, `dropEvent`, `itemDoubleClicked`, `addAction` absent from the range) — inherited QTextEdit behaviour only | — | NONE | Inherited QTextEdit drop accepts arbitrary rich text/files; unaudited. |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ManuscriptPane(const allcore::Spine&, QWidget*)` | 30837 | Builds ribbon, styles gallery, editor, sidebar, PUBLISH row, status line; wires all signals; starts autosave timer; reopens last file | `main.cpp:35864` |
| `openFile(const QString&)` | 31136 | Reads a file as UTF-8 HTML into the editor, sets `path_`, clears `dirty_`, records `manuscript/lastFile`, updates status | Open button (31062), ctor restore (31134), `selfTest` (31250) |
| `save()` | 31148 | Saves editor HTML through `saveOrWarn`; delegates to `saveAs()` when unnamed; truthful failure status | Save button, autosave timer, `saveAs()`, `exportRtf()`, `selfTest` |
| `saveAs()` | 31167 | Prompts for a path, forces `.html`, records the QSettings key, calls `save()` | Save-as button, `save()` when `path_` empty, `exportRtf()` |
| `exportRtf()` | 31179 | Ensures a saved HTML file exists, then converts to RTF via `textutil` | Export RTF button |
| `find()` | 31198 | Runs the Gofer query against the spine, renders result HTML, rebuilds `inserts_` | Find button, query Return, `selfTest` |
| `setManuscriptText(const QString&)` | 31220 | Replaces editor content with plain text (inbound hand-off) | `g_sendToManuscript` lambda, 35943 |
| `manuscriptText() const` | 31225 | Returns editor plain text (MEM-1 receipt reader) | `g_manuscriptText` lambda, 35947; DraftPane selfTest 19931 |
| `selfTest(QStringList&)` | 31227 | Four checks: bold, Gofer sidebar, save/reopen round trip, WP-1 failed-save honesty; cleans up temp file, `path_`, editor, and the QSettings key | `main.cpp:38265` (app-wide self-test sweep) |

Private state: `spine_`, `editor_`, `side_`, `query_`, `results_`, `status_`, `boldB_`, `italB_`, `undB_`, `sizeSpin_`, `path_`, `dirty_`, `inserts_` (31286–31296).

## 4. Dependencies

**Files / folders read and written**
- Any user-chosen `*.html` manuscript file (read at 31137, written at 31156).
- Any user-chosen `*.rtf` export target, written by the external `textutil` process (31192).
- `QDir::temp()/all_manuscript_selftest.html` — created and removed by `selfTest` (31245, 31255).
- The spine database, read-only via the injected `allcore::Spine&` in `find()`.

**Network**
- **None.** No `QNetworkAccessManager`, `TimedNam`, `QDesktopServices`, or URL fetch appears anywhere in 30835–31296. `results_->setOpenLinks(false)` also prevents the browser from following any anchor.

**External processes**
- `textutil` (macOS system binary), 15 s timeout.

**QSettings (organisation "ALL", application "TranslationTool")**
- `manuscript/lastFile` — written by `openFile` (31142) and `saveAs` (31174); read in the ctor (31129); removed by `selfTest` (31283).

**Session memory (`sess::remember`, separate store)**
- `manuscript/query` — the Gofer query box, seeded and written on every keystroke (31040).

**Cross-pane globals**
- Reads `g_mssComposeBib`, `g_mssProposeNote` (declared 3416–3417, bound to DraftPane at 35949–35952).
- Provides `g_sendToManuscript`, `g_manuscriptText` (bound 35942–35948).

**Helpers outside the class**: `safeGetOpenFileName` / `safeGetSaveFileName` (harness-stubbable, 3384), `saveOrWarn` (2902), `RibbonBar`, `ux::themedStyle`, `sess::remember` (3090), `allcore::goferSearch`.

**ctest suites**: no ManuscriptPane-specific suite exists. The only related suite is `gofer_smoke` (core/CMakeLists.txt:119), which covers the search engine, not this pane. All pane-level coverage is the in-app `selfTest` invoked at 38265.

## 5. Release questions

- **Undo-less overwrite:** `setManuscriptText` wipes the whole manuscript (and any unsaved rich formatting, since it inserts plain text) when another pane hands off. Should it refuse when `dirty_`, insert at the cursor, or prompt? Decide before release.
- **RTF export is macOS-only.** Is shipping a Mac-only export acceptable for the Windows/Linux input-centre builds, or should the button be hidden/disabled off-macOS? The failure message ("is this a Mac?") is the only signal today.
- **Manually verify the two file dialogs.** Open… and Save as… are never exercised by any automated check; verify pick/cancel/overwrite/extension-append behaviour by hand.
- **Manually verify the five HOUSE STYLES buttons and the I/U/size controls** — zero automated coverage, and the styles are claimed to survive RTF export; confirm that claim end to end.
- **Silent `openFile` failure:** should an unreadable or non-HTML file report an error rather than doing nothing? Currently the pane looks like it ignored the click.
- **`saveAs` writes `manuscript/lastFile` before knowing the save succeeded** — a failed save leaves a startup restore pointing at a bad path. Confirm intended.
- **Autosave warning storm:** a persistently failing autosave can raise `saveOrWarn`'s dialog every 60 seconds. Decide on a back-off or a one-shot warning.
- **No ⌘S / keyboard shortcuts** in this pane. Confirm this is covered by a global menu action elsewhere, or add one.
- **Silently inert PUBLISH buttons** if the DraftPane hooks are ever unset (e.g. a pane-construction reorder). Consider disabling the buttons instead of no-oping.
- **Startup auto-reopen** of the last manuscript happens without asking. Confirm this is the desired behaviour for shared/input-centre machines.
