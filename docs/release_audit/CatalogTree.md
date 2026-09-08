# Release audit — `CatalogTree` (app/main.cpp, lines 31459–31613)

## ⚠️ SCOPE FLAG: CATALOGING IS SIDELINED

> "## SCOPE DECISION (Adam, 2026-08-08): cataloging is SIDELINED"
> — `/Users/adamderickandrade/ALL-translation-tool/TODO.md`, line 471

> "A separate in-house cataloging tool/app will be created as its own
> project in the future. All cataloging-tool discussion and
> implementation is sidelined HERE from this date."
> — `TODO.md`, lines 473–475

> "SIDELINED (banked knowledge kept for the future project): … any
> catalog-editing/catalog-building features in this app."
> — `TODO.md`, lines 476–481

> "STAYS IN SCOPE here (serves translators, not cataloging): the
> Library pane's catalog DISPLAY (decoder, titles, subjects,
> verification levels) …"
> — `TODO.md`, lines 484–487

`CatalogTree` exists **only** to serve `CatalogPane`, which is a
catalog-BUILDING surface, not the Library pane's catalog DISPLAY. By the
2026-08-08 ruling it is on the sidelined side of the line. Note the
countervailing evidence: `TODO.md` line 812 records item 9g
("CATALOGING WORKFLOW", Adam 2026-08-19) shipping v1 "same day" —
i.e. work continued AFTER the sideline decision, and `TODO.md` line 1389
still restates "Cataloging sidelined (own future project, SCOPE
DECISION …)". The two statements are unreconciled in the repo. That
contradiction is itself a release question (below).
`docs/FEATURE_STATUS.md` does not list a Catalog pane row; its three
"catalog" hits (lines 13, 65, 148) are all about Library-pane catalog
*display*, not this pane.

---

## 1. Purpose and how the user reaches it

`CatalogTree` is a self-contained folder-browser widget: a bold title, a
"Choose folder…" button, a one-line honest census of what the folder
holds, and a `QTreeView` over a `QFileSystemModel` whose first column is
painted by `IdentitySuffixDelegate` (line 31436) so each filename is
followed by its decoded ACIP identity or the word "uncataloged". It
remembers its own root under its own `sess::` key. It is **not** a tab
and has no menu entry — the user never reaches it directly. It is
instantiated exactly twice, both inside `CatalogPane`'s constructor
(lines 31773 "Uncataloged intake" and 31776 "Destination — the
library"), side by side in a horizontal `QSplitter`. `CatalogPane`
itself is added unconditionally in `main()` at
`app/main.cpp:36402–36403` (`new CatalogPane(root); tabs.addTab(…,
"Catalog")`) — no setting, no admin gate, no feature flag: **the Catalog
tab is visible to every user on every launch**, and these two trees are
the top half of it.

## 2. User-visible functions

| Control/action | What it does (from code) | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| "Choose folder…" button | Opens `safeGetExistingDirectory` with the per-instance `prompt_` ("Choose the uncataloged folder" / "Choose the destination folder"); a non-empty pick calls `setRoot(d)` which **persists** the path to `sess::put(sessKey_)` | ctor lambda, 31496–31499 | NONE directly. The persistence side is proved negatively by `"  [%1] Catalog: harness probe left no session residue"` (app selfTest, abs. line ~39665) which asserts the harness does NOT write `sess/catalog/intakeDir` | Writes a QSettings key on every pick. Modal native dialog. No validation beyond "non-empty and exists". |
| Single click on a file row | `emitFile` fires `onFile(filePath)` → in `CatalogPane` sets `lastFile_` and renders the whole file card (decoded identity, three states, provenance scan, full text) | 31498–31501 (`emitFile`), connected 31502 | Indirect: `"  [%1] Catalog: the panel shows the entire text, not a truncation"`; `"… a binary file gets an honest note, not a garbage dump"` (via `showFilePublic`, not via the tree) | Clicking a file triggers a **full read of up to 20 MB** plus a 32 MB `scanAcipCleanup` pass on the UI thread — no progress, no cancel. Directories are ignored silently. |
| Keyboard/arrow selection change | `currentChanged` runs the same `emitFile`, so arrowing through a folder re-renders (and re-reads) each file | 31503–31507 | NONE | Same synchronous cost as above, but fired per arrow-key press — a held-down arrow key reads every file in the folder. |
| Expand/collapse a folder node | `setRootIsDecorated(true)` / `setItemsExpandable(true)`: subfolders expand in place rather than replacing the view | 31487–31489 | NONE | Lazy `QFileSystemModel` population; `selectPath` has an explicit workaround for it (see internals). |
| Column header click (sorting) | `setSortingEnabled(true)`, initial sort column 0 ascending | 31483–31484 | NONE | — |
| Column visibility | "Type" column (index 2) permanently hidden; column 0 width 380 | 31485–31486 | NONE | Not user-configurable; the name column is where the identity suffix is painted. |
| Identity suffix rendering (`IdentitySuffixDelegate`) | Appends `"   — <collection> <number>"` for a recognized ACIP filename, else `"   — uncataloged"`. Paint-only; the model is untouched | `IdentitySuffixDelegate::initStyleOption`, 31440–31456 | Indirect only: the census counters it mirrors are proved by `"  [%1] Catalog: intake census counts identifiable vs uncataloged honestly"` | Calls `allcore::decodeAcipFilename` **per repaint, per visible row** — no memoization. Directories are skipped (`m->isDir` early return). Labels every unrecognized file, including PDFs/scans/`.DS_Store`, as "uncataloged". |
| Census line (read-only label) | After `setRoot`, states "N file(s) · X identifiable by name · Y uncataloged (Z announce a title in their own text) · <folder>" | `setRoot`, 31511–31546 | `"  [%1] Catalog: intake census counts identifiable vs uncataloged honestly"` (asserts 3 files / 1 recognized / 1 titled) | **Hard cap at 5,000 files** (31527). The cap is disclosed in the text ("first 5,000 counted") but the counts silently describe a subset of a large intake tree. |
| Drag and drop | Not enabled — `setDragEnabled`/`setAcceptDrops` are never called | — | N/A | A user dragging a file onto either tree gets nothing. Given that the pane's whole idea is moving files between trees, the *absence* of drag-and-drop is a discoverability gap, not a bug. |
| Double-click | No handler; Qt's default begins an inline rename edit attempt on an editable model. `QFileSystemModel` defaults to `readOnly=true`, so the edit is refused — but nothing here asserts that | — | NONE | Should be pinned: a non-read-only model here would mean silent in-place file renames with no confirmation. |
| Context menu | None (`contextMenuPolicy` never set) | — | N/A | — |
| Shortcuts | None registered | — | N/A | — |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `CatalogTree(title, sessKey, prompt, parent)` | 31461 | Builds the whole widget: title row, "Choose folder…", census label, `QFileSystemModel` + `QTreeView` + identity delegate; wires click and `currentChanged` to `onFile` | `CatalogPane` ctor, 31773 and 31776 |
| `setRoot(dir, remember=true)` | 31511 | Sets the model/view root, optionally persists to `sess::`, then walks the tree with `QDirIterator` (capped at 5,000) computing `files_`/`recognized_`/`titled_` and rewrites the census label | "Choose folder…" lambda (31498); `CatalogPane` ctor (31790, 31805, 31808); `CatalogPane::scanFolder` (32066); `setDestRoot` (32071); "Official library…" button (31682); `finishHandoffMove` (33452–33454); `approvalsDialog` approve handler (34039) |
| `selectPath(path)` | 31548 | Selects and scrolls to a path; when the lazy model has not listed the folder yet, defers via a one-shot `directoryLoaded` connection. Returns whether it selected immediately | `CatalogPane::handleInfoAnchor` `shelfsel:` branch (32046); `CatalogPane::selectFirstUncataloged` (32075) |
| `firstUncataloged()` | 31570 | Full recursive walk (**no 5,000 cap here**), collects every file the decoder cannot name, sorts, returns the first | `CatalogPane::selectFirstUncataloged` (32074) |
| `currentDir()` | 31586 | The selected folder (a selected file yields its parent); empty when nothing is selected | `CatalogPane` "Move to shelf…" handler (31856) |
| `root()` | 31593 | The current root path | `CatalogPane` several places (31817, 31860, 31896, 31912, 31951, 33453–33454) |
| `fileCount()` / `recognizedCount()` / `titledCount()` | 31594–31596 | Census accessors | `CatalogPane::fileCount/recognizedCount/titledCount` (32067–32069), which the app selfTest reads |
| `onFile` (public `std::function`) | 31597 | Callback seam for "a file was selected" | Assigned by `CatalogPane` ctor for both trees, 31793–31801 |
| `IdentitySuffixDelegate::initStyleOption` | 31440 | Paint-only identity suffix on column 0 | Installed at 31491–31492 |

## 4. Dependencies

**Code/library**
- `allcore::decodeAcipFilename` (delegate paint path + `setRoot` census + `firstUncataloged`).
- `allcore::extractAcipTitle` (census "announce a title in their own text" counter — reads the first 4,000 bytes of every unrecognized file).
- `ux::themedStyle`, `ux::chromeMuted` (census label colour).
- `safeGetExistingDirectory` (the house wrapper around `QFileDialog`).
- Qt: `QFileSystemModel`, `QTreeView`, `QDirIterator`, `QStyledItemDelegate`.

**Disk**
- Any folder the user points it at. Reads only — `CatalogTree` itself never writes a file. It opens each unrecognized file read-only for a 4,000-byte title sniff during the census.
- The two roots as configured by `CatalogPane`: intake = user's choice; destination defaults to `<root>/library` (`CatalogPane` ctor, 31807) or the official Dropbox root.

**Network**
- None.

**QSettings / `sess::` keys** (written via `sess::put(sessKey_, dir)` in `setRoot` when `remember=true`)
- `sess/catalog/intakeDir` — the intake tree's remembered root. **Written** on every "Choose folder…" pick and on `scanFolder()`.
- `sess/catalog/destDir` — the destination tree's key. In practice `CatalogPane` always calls the destination `setRoot(..., remember=false)`, so this key is **read (31805) but never written** by the current code — a latent inconsistency: the destination root is not actually remembered across launches, despite having a key for it.
- (`catalog/officialRoot` is owned by `CatalogPane`, not this class.)

## 5. Release questions

- **Should this ship at all?** It is a component of a pane the 2026-08-08 scope decision sidelines. If `CatalogPane` is hidden or removed, `CatalogTree` and `IdentitySuffixDelegate` go with it — they have no other caller (`grep` confirms: only lines 31773/31776 construct it).
- **Reconcile TODO.md line 471 vs line 812.** The repo simultaneously says cataloging is sidelined *and* records a shipped v1 cataloging workflow eleven days later. Until the owner rules, "sidelined" is the written policy and the pane contradicts it in the shipping tab bar.
- **The 5,000-file census cap.** Honest in wording, but a cataloger pointing this at a real intake drive gets a census of an arbitrary 5,000-file subset with no way to raise the cap. Ship as-is, raise the cap, or make it configurable?
- **`firstUncataloged()` has no cap at all** while `setRoot` does. On a large tree it is an unbounded synchronous recursive walk on the UI thread, invoked by `selectFirstUncataloged()` — which the screenshot/demo path calls (`app/main.cpp:40967`). Freeze risk on a real drive.
- **Synchronous full-file reads on selection.** Arrow-keying through a folder reads up to 20 MB + scans up to 32 MB per file with no debounce. Should selection be debounced before ship?
- **`sess/catalog/destDir` is read but never written.** Decide: honour the key (pass `remember=true` somewhere) or delete it.
- **Double-click behaviour is unpinned.** No test asserts the `QFileSystemModel` stays read-only. One line of setup drift would turn double-click into an unconfirmed rename of library files.
- **No drag-and-drop** between the two trees, in a pane whose thesis is moving files from the left tree to the right one. Intentional (moves must be confirmed) or a gap?
- **Zero direct automated coverage** of the widget's own interactions — every catalog selfTest check goes through `CatalogPane`'s public seams, never through a tree click, a folder pick, or the delegate's painted text.
