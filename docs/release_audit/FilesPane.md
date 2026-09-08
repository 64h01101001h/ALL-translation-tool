# FilesPane — release audit

## 1. What it is

`FilesPane` (app/main.cpp:21460–23201) is a dual-pane file manager built into the Diamond Cutter Translation Tool — a Total Commander / Path Finder / ForkLift hybrid for moving Tibetan text files around: two independently tabbed `QTreeView`s over a shared `QFileSystemModel`, breadcrumb navigation, a persistent "Drop Stack" shelf, copy/move between the panes, batch rename, folder compare, folder sync, archive browsing, and remote FTP/FTPS/WebDAV/SFTP connections. It is constructed once in `main()` at app/main.cpp:35974 with the data root and an `openText` callback that hands text files to the Overlay pane, and is added to the single top-level `tabs` group at app/main.cpp:35981 as the tab **"Files"** (between "Library" and "Search"). The user reaches it by clicking that tab; opening a text file from it switches to Overlay via `g_raisePane`, and opening a scan image jumps to Input via `g_openScanInInput`.

**Coverage headline: `FilesPane` has NO `selfTest()` member.** A grep for `selfTest` across lines 21447–23201 returns nothing, and `main()`'s `--selftest` block (app/main.cpp:38250–38283) never calls `filesPane->selfTest(...)` — `filesPane` is not referenced there at all. What exists instead is three inline blocks inside the same `--selftest` body (app/main.cpp:40353, 40402, 40428) that exercise only the four **pure static planners** (`parseFtpList`, `parseDavList`, `computeSyncPlan`, `computeRenames`). Every widget, dialog, menu, shortcut and file-mutating path in this pane is untested except incidentally by `gauntlet_walk` (`DiamondCutterTranslationTool --gauntlet auto 300`, app/CMakeLists.txt:92), a 300-step random-monkey UI walk that may or may not touch any given control on any given run. Other ctest suites (`dmp_smoke`, `collation_smoke`, `font_smoke`) are unrelated to this pane.

## 2. User-visible functions

| Control/action | What it does | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| "hidden files" checkbox (VIEW) | Toggles `QDir::Hidden` in the shared model filter, affecting both panes | ctor lambda, 21601 | NONE (gauntlet_walk may toggle) | — |
| "Commander keys" checkbox (VIEW) | Opt-in for the F3–F8 key handling in `eventFilter`; persisted | ctor 21473 + 21624 | NONE | Persisted to QSettings `files/cmdkeys` |
| "sync browsing" checkbox (VIEW) | When on, entering a folder in one pane enters the same-named folder in the other if it exists | ctor 21482 + used in dbl-click 21723 | NONE | Persisted `files/sync` |
| "Drop Stack" checkbox (VIEW) | Shows/hides the drop-stack list widget | ctor 21521, connect 21598 | NONE | Not persisted — always re-checked on launch even if the user hid it |
| Favorites button (GO, instant popup) | Opens the favorites menu | ctor 21489, `rebuildFavMenu` 22947 | NONE | — |
| Favorites → "Add LEFT/RIGHT pane's folder" | Appends that pane's current path to `files/favs` | `rebuildFavMenu` 22948/22951 | NONE | Writes QSettings |
| Favorites → \<favorite name\> | `setPath(active_, f)` — jumps the active pane there | 22965 | NONE | No existence check; a deleted favorite silently shows an empty view |
| Favorites → "Remove favorite" → \<path\> | Removes it from `files/favs` and rebuilds the menu | 22972 | NONE | — |
| Connections button (GO) | Opens the remote-connections dialog | ctor 21499, `openConnections` 22083 | NONE | Entire remote subsystem is untested at the UI level |
| Workspaces button (GO, instant popup) | Opens the workspace menu | ctor 21506, `rebuildWsMenu` 22992 | NONE | — |
| Workspaces → "Save current layout as workspace…" | Prompts for a name; stores both panes' tab path lists under `files/ws_<name>_0/1` | 22993 | NONE | A duplicate name silently overwrites the old workspace |
| Workspaces → \<workspace\> | `applyTabs` on both panes, then `saveTabs` | 23036 | NONE | Non-existent paths are dropped silently (`applyTabs` 22983); a fully stale workspace collapses to $HOME |
| Workspaces → "Delete workspace" → \<name\> | Removes the name and both `files/ws_*` keys | 23050 | NONE | No confirmation |
| "Terminal" button (GO) | `QProcess::startDetached("/usr/bin/open", {"-a","Terminal", curPath_[active_]})` | ctor 21613 | NONE | Hard-coded macOS path/app; return value discarded — failure is silent, non-macOS is a no-op |
| "Copy →" / "← Copy" (TRANSFER) | `xferSel(from, false)` — copies the selection to the other pane's folder | ctor 21572/21574, `xferSel` 21931 | NONE | Recursive for folders; existing destination is skipped with an info box (never overwrites) |
| "Move →" / "← Move" (TRANSFER) | `xferSel(from, true)` — `QDir::rename`/`QFile::rename` to the other pane | ctor 21576/21578, `xferSel` 21931 | NONE | Cross-volume moves of folders fail (`QDir::rename` won't cross devices) → only a "Failed on X" warning |
| "Batch rename…" (TOOLS) | Opens the find/replace preview dialog over the active pane's selection | ctor 21580, `batchRename` 22780 | Planner only: app_selftest `[PASS] Files: batch-rename plan applies find/replace and reverts collisions` (40428) | Apply loop itself (rename + skip counting, 22820) untested |
| Batch-rename "regex" checkbox / find / replace fields | Live-refresh the preview via `computeRenames` | 22812–22817 | Same as above | Invalid regex silently leaves names unchanged (`re.isValid()` guard, 22761) |
| Batch-rename "Apply" | Renames each changed name; skips any name that already exists | 22820 | NONE for the apply path | Result reported only as a "N renamed, M skipped" box; no per-file detail |
| "Sync folders…" (TOOLS) | Opens the sync-plan dialog for the two shown folders | ctor 21590, `syncFolders` 22593 | Planner only: app_selftest `[PASS] Files: two-way sync plan — adds both ways, newer wins, never deletes` (40402) | — |
| Sync mode combo (L→R / R→L / two-way) | Recomputes the plan on change | 22636 | Same as above | Full recursive `collectFiles` scan on every change — blocking on large trees |
| Sync "Apply this plan" | Copies each planned file, `QFile::remove` first on overwrite rows | 22639 | NONE | Blocking (no progress, no cancel); `QFile::remove` return discarded — a failed remove turns into a counted "failed" copy; only aggregate counts are reported, not which files |
| "Compare panes…" (TOOLS) | Builds a text report: only-left, only-right, same-name-different-size | ctor 21602, `comparePanes` 22703 | NONE | Top level only, files only — stated in the report footer |
| Quick-select line edit + Return | Wildcard or regex select of matching names in the active pane; placeholder becomes "N selected"/"no names matched" | ctor 21617, `quickSelect` 22669 | NONE | Only the currently loaded rows of the active root are scanned (`QFileSystemModel` populates lazily) |
| Tree double-click (either pane) | Folder → `setPath` (plus mirrored folder when sync browsing is on); file → `openPath` | buildPanel lambda 21718 | NONE (gauntlet_walk may hit) | — |
| Tree context → "Add to Drop Stack" | Appends the path to the stack and persists it | 21759 | NONE | — |
| Tree context → "Open" | `openPath` dispatch | 21761 | NONE | — |
| Tree context → "Reveal in Finder" | `QDesktopServices::openUrl` on the containing folder | 21764 | NONE | Opens the parent folder, not the item itself — does not actually select the file |
| Tree context → "Open in Terminal here" | `open -a Terminal` on the item or its parent | 21770 | NONE | macOS-only, return discarded |
| Tree context → "Move to Trash" | `QFile::moveToTrash(p)` on the single item under the cursor | 21778 | NONE | **No confirmation** and the bool is discarded — a failed trash looks identical to success |
| Tree context → "Finder tags: …" (disabled row) | Runs `/usr/bin/mdls -raw -name kMDItemUserTags` and shows the tags as a disabled label | 21781 | NONE | Blocking 1.5 s `waitForFinished` **inside the context-menu build** — visible stall on slow/network volumes; macOS-only |
| Pane tab click | Switches to that tab's stored path | 21806 | NONE | — |
| Pane tab close (×) | Removes the tab (refuses when only one remains) and re-saves | 21810 | NONE | Silent no-op on the last tab — no feedback |
| "+" button beside each tab bar | New tab duplicating the current location | 21801 | NONE | — |
| Breadcrumb "/" and segment buttons | `setPath` to that ancestor | `setPath` 21856/21868 | NONE | — |
| Inline rename (F2 / click-rename) | Model is `setReadOnly(false)` with `EditKeyPressed` triggers | buildPanel 21694/21702 | NONE | Rename failures surface only as Qt's own model behaviour |
| Drop Stack double-click | Opens the stacked path via `openPath` | ctor 21592 | NONE | Path may no longer exist — no existence check at open time |
| Drop Stack context → Open / Remove from stack / Clear stack | Open path; delete the item and re-save; clear all and re-save | ctor 21565–21585 (menu block) | NONE | "Clear stack" has **no confirmation** |
| Commander key F3 / F4 | Opens the selected path (`openPath`) | `eventFilter` 23093 | NONE | Only when "Commander keys" is checked |
| Commander key F5 | Copy selection to the other pane | `eventFilter` 23098 | NONE | Same guard |
| Commander key F6 | Move selection to the other pane | `eventFilter` 23101 | NONE | Same guard; no confirmation |
| Commander key F7 | Creates "untitled folder" (numbered if taken) in the active pane | `eventFilter` 23104 | NONE | `d.mkdir` result discarded — a read-only folder silently does nothing |
| Commander key F8 | Moves selection to Trash; confirms only when more than one item is selected (W9-01) | `eventFilter` 23115 | NONE | **A single-item F8 trashes with no confirmation**; `moveToTrash` results discarded |
| Opening a .txt/.act/.inc/.acip/.md | Routed to the Overlay pane through `openText_` | `openPath` 21881 | NONE | — |
| Opening a .png/.jpg/.jpeg/.tif/.tiff | Routed to the Input pane via `g_openScanInInput` | `openPath` 21889 | NONE | Falls through to `QDesktopServices` if the global is unset |
| Opening a .zip/.tar/.tgz/.tar.gz | Opens the archive browser dialog | `openPath` 21894, `browseArchive` 22861 | NONE | — |
| Any other file type | `QDesktopServices::openUrl(file://…)` — hands it to the OS | `openPath` 21900 | NONE | Launches arbitrary external applications on the user's file |
| Archive "Extract selected & open" | Extracts chosen entries to `$TMPDIR/all_zip_view` and opens each | `browseArchive` 22903 | NONE | Writes to a shared fixed temp dir, never cleaned; `unzip -o` **overwrites** inside that dir; archive entry names are passed to `tar`/`unzip` as arguments (no path-traversal check on `../` entries) |
| Archive "Extract all into the other pane" | `tar -xkf` / `unzip -n` into the opposite pane's folder | `browseArchive` 22920 | NONE | Keep-existing flags are correct; 120 s blocking wait with no progress or cancel; success box shown even if the tool failed |
| Connections: profile combo | Lists profile names from `files/conns` | `openConnections` 22105 | NONE | — |
| Connections: "Add / edit profile…" | Sub-dialog for name/protocol/host/port/user/start path; saves to QSettings | 22163 | NONE | Names are used verbatim as QSettings key fragments (`files/conn_<name>_…`) — a name with `/` forges a nested key; no edit-load of an existing profile despite the "edit" label |
| Connections: "Connect" | Loads the profile, prompts for a password (non-SFTP), lists the remote path | 22354 | NONE | **Live network I/O to a user-supplied host**; blocking `waitForFinished(60000)` on the GUI thread |
| Connections: remote list double-click | Descends into a remote directory | 22383 | NONE | — |
| Connections: "Up" | Goes one remote directory up | 22393 | NONE | — |
| Connections: conflict combo (skip / keep both / overwrite) | Chooses the local landing rule for downloads | 22318 (`landing` 22400) | NONE | Documented FAIL-9 comment at 22406: an overwrite whose `QFile::remove` fails deliberately degrades to keep-both |
| Connections: "Download into the active pane" | Recursive FIFO download of selected remote items into `curPath_[active_]` | 22455 | Parsers only: app_selftest `[PASS] Files: FTP + WebDAV listing parsers (live-captured samples)` (40353) | **Writes remote content to disk**; remote-supplied names are joined into local paths with no traversal sanitisation; blocking, no cancel; errors only appear in the activity log |
| Connections: "Upload the active pane's selected file(s)" | Recursive upload, creating intermediate remote directories | 22536 | NONE | Blocking; failures only logged |
| Drag out of a tree view | `setDragEnabled(true)` on both views | buildPanel 21698 | NONE | Drag **out** only — the pane sets no `setAcceptDrops`, no `dragEnterEvent`, no `dropEvent`, so you cannot drop files into the panes or onto the Drop Stack (the shelf is add-by-menu only) |
| Keyboard shortcuts | None registered — no `setShortcut` anywhere in the class; F3–F8 are handled by `eventFilter`, gated on the opt-in checkbox | 23083 | NONE | Every operation is mouse-only unless Commander keys are enabled |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `FilesDirProxy::lessThan` | 21451 | Sort override keeping folders above files in both views | Qt sorting on `proxy_` |
| `FilesPane::FilesPane` | 21462 | Builds the ribbon, both panels, the Drop Stack; restores toggles, tabs, favorites, workspaces, stack | `main()` 35974 |
| `addToStack` | 21651 | Adds a path (with folder/file emoji) to the shelf and persists | context menu, stack restore, public API |
| `buildPanel` | 21663 | Constructs one side: tab bar, "+" button, breadcrumbs, tree view, shared model/proxy, restores saved tabs | ctor |
| `addTab` | 21820 | Adds a tab bound to a path and navigates to it | `buildPanel`, "+" button, `applyTabs` |
| `setPath` | 21833 | Sets a pane's root index, updates the tab label/data, rebuilds breadcrumb buttons | double-click, breadcrumbs, favorites, tabs, `addTab` |
| `selectedPath` | 21875 | Current-index path in a pane | `selectedPathsIn`, F3/F4 |
| `openPath` | 21881 | Extension dispatcher: text→Overlay, image→Input, archive→browser, else OS | double-click, context menu, stack, archive extract, F3/F4 |
| `selectedPathsIn` | 21903 | Selected rows as paths, falling back to the current index | `xferSel`, `batchRename`, upload, F8 |
| `copyRecursively` | 21915 | Static recursive directory copy | `xferSel` |
| `xferSel` | 21931 | Copy/move the selection to the other pane, skipping existing names | Copy/Move buttons, F5/F6 |
| `parseFtpList` | 21974 | Static parser for `ls -l`-style FTP/SFTP listings | `refresh`, `listAt`, app_selftest 40364 |
| `parseDavList` | 21996 | Static parser for WebDAV PROPFIND multistatus XML | `refresh`, `listAt`, app_selftest 40381 |
| `computeSyncPlan` | 22042 | Static, pure: the sync plan (adds both ways, newer wins, never deletes) | `syncFolders`, app_selftest 40403 |
| `collectFiles` | 22067 | Static recursive rel-path → (size, mtime) map | `syncFolders::refresh` |
| `openConnections` | 22083 | The whole remote-connections dialog (profiles, curl/sftp runners, list, download, upload, log) | Connections button |
| `syncFolders` | 22593 | Sync dialog: plan preview then Apply | Sync button |
| `quickSelect` | 22669 | Wildcard/regex selection in the active pane | quick-select Return |
| `comparePanes` | 22703 | Builds and shows the two-folder comparison report | Compare button |
| `computeRenames` | 22755 | Static, pure: find/replace over names; collisions revert | `batchRename`, app_selftest 40429 |
| `batchRename` | 22780 | Rename dialog with live preview and Apply | Batch-rename button |
| `browseArchive` | 22861 | Lists a zip/tar via `zipinfo`/`tar -tf`, offers selective/full extraction | `openPath` |
| `addFav` | 22939 | Appends a path to `files/favs` | favorites menu |
| `rebuildFavMenu` | 22947 | Rebuilds the favorites menu from settings | ctor, `addFav`, remove action |
| `applyTabs` | 22983 | Replaces a pane's tabs with a saved list (dropping missing paths) | workspace restore |
| `rebuildWsMenu` | 22992 | Rebuilds the workspaces menu (save/restore/delete) | ctor, save, delete |
| `saveTabs` | 23063 | Persists both panes' tab path lists | `addTab`, `setPath`, tab close, workspace restore |
| `saveStack` | 23074 | Persists the Drop Stack paths | `addToStack`, stack context actions |
| `eventFilter` | 23083 | Tracks the active pane on focus; implements F3–F8 when Commander keys are on | installed on both views (21704) |

## 4. Dependencies

**Filesystem**
- Reads the whole local filesystem: `model_->setRootPath("/")` (21690).
- Left pane's default start folder: `<root>/library` (21815), falling back to `QDir::homePath()`; right pane starts at `$HOME`.
- Writes: copies/moves/renames anywhere the user points it; `QFile::moveToTrash`; `QDir::mkdir` (F7); `QDir::mkpath` for sync and downloads; archive extraction into `QDir::tempPath() + "/all_zip_view"` (22905) and into the opposite pane's folder.

**External processes (all hard-coded absolute macOS paths)**
- `/usr/bin/open` (Terminal launch, 21615 and 21771)
- `/usr/bin/mdls` (Finder tags, 21784)
- `/usr/bin/tar`, `/usr/bin/zipinfo`, `/usr/bin/unzip` (archive browse/extract, 22865–22925)
- `/usr/bin/curl` (FTP/FTPS/WebDAV, 22289) — credentials fed via `-K -` on stdin, never argv
- `/usr/bin/sftp` (`-oBatchMode=yes -b -`, key auth only, 22309)
- `QDesktopServices::openUrl` for Reveal in Finder and unknown file types

**Network hosts**
- None fixed. Every host is user-configured in a connection profile; protocols `ftp`, `ftps`, `webdav` (http), `webdavs` (https), `sftp`. No `QNetworkAccessManager`/`TimedNam` — all traffic goes through the child processes above.

**QSettings (organisation "ALL", application "TranslationTool")**
| Key | R/W | Meaning |
|---|---|---|
| `files/cmdkeys` | R 21621 / W 21625 | Commander-keys toggle |
| `files/sync` | R 21623 / W 21630 | Sync-browsing toggle |
| `files/stack` | R 21645 / W 23079 | Drop Stack paths |
| `files/tabs0`, `files/tabs1` | R 21810 / W 23070 | Per-pane tab path lists |
| `files/favs` | R 22955 / W 22943, 22977 | Favorite folders |
| `files/workspaces` | R 23029 / W 23012, 23057 | Workspace names |
| `files/ws_<name>_<0\|1>` | R 23039 / W 23020, removed 23058 | A workspace's per-pane tab lists |
| `files/conns` | R 22107 / W 22222 | Connection profile names |
| `files/conn_<name>_{proto,host,port,user,path}` | R 22359 / W 22224 | Profile fields (no password — passwords are session-only, 22367) |

## 5. Release questions

- Ship a destructive pane with **zero widget-level tests**? Only four pure planners are covered by `app_selftest`; every mutating path (copy, move, trash, rename apply, sync apply, extract, upload/download) is proven by nothing but the random `gauntlet_walk`. At minimum a `FilesPane::selfTest` over a temp-dir fixture for `xferSel`, the batch-rename apply loop, and the sync apply loop.
- Trash without confirmation: the context-menu "Move to Trash" and single-item F8 delete with no prompt and discard the result. Is recoverability alone enough, given F8's own multi-select prompt already exists?
- Cross-platform: `open`, `mdls`, `tar`, `zipinfo`, `unzip`, `curl`, `sftp` are hard-coded `/usr/bin` paths. The roadmap promises Windows/Linux builds for the input centers — this pane degrades to silent no-ops there.
- Blocking GUI-thread I/O: the 1.5 s `mdls` call inside every context-menu build, the 60 s per-request remote calls, the 120 s archive extract, and the full recursive scan on every sync-mode change. No progress, no cancel.
- Remote-name trust: downloaded entry names from an FTP/WebDAV/SFTP server are concatenated straight into local paths (`localDir + "/" + e.name`), and archive entry names go straight to `tar`/`unzip`. Neither is checked for `..`. Do we accept that before shipping the remote feature?
- Connection profile names are used verbatim inside QSettings key strings — should they be sanitised or hashed?
- "Add / edit profile…" never loads an existing profile into the form; it only ever adds. Rename the button or implement editing.
- Fixed extraction scratch dir `$TMPDIR/all_zip_view` is shared, `unzip -o` overwrites inside it, and it is never cleaned up.
- Drop Stack visibility is not persisted, so it reappears every launch regardless of the user's last choice.
- No drop target: the pane advertises a shelf but accepts no drags from Finder or from itself. Is that acceptable for the release, or should it be labelled?
