# LibraryPane — release audit

*Source: `app/main.cpp`, class `LibraryPane` (lines 23202–26271). Audit written for a ship / hide / fix pass; no source was modified.*

## 1. What the pane is for

LibraryPane is the **Read → Library** surface: the user's on-disk collection of Tibetan texts (ACIP `.act`/`.txt` files under `<data root>/library`, plus `my_materials/` for imported personal documents and `ocr_out/` for OCR hand-offs). It is reached from the ribbon group **Read** in the main window; the pane itself carries its own four-group ribbon (SHELVE / STUDY / CARE / FIND). It shows a folder tree (or a flat catalog table) on the left and an info/card panel on the right; double-clicking a text opens it in the Overlay pane. It is also the home of the acquisition and maintenance pipeline — downloading and installing the official ACIP collections from asianlegacylibrary.org, importing a data release, rebuilding the FTS search index, legacy-font rescue, OCR hand-off — and of the People/author layer (author search, author ledgers, BDRC and Treasury of Lives links).

All coverage below is via `LibraryPane::selfTest()` (line 23744), which runs in ctest as **`app_selftest`** (`app/CMakeLists.txt:89`, `DiamondCutterTranslationTool --selftest`). Release-package discovery is additionally covered by the MainWindow-level selftest block at `app/main.cpp:5804` (same ctest suite).

## 2. User-visible functions

| Control / action | What it does | Code anchor (line) | Automated coverage | Notes / risks |
|---|---|---|---|---|
| Banner link "asianlegacylibrary.org/library" | Opens the ALL library page in the system browser (`setOpenExternalLinks`). | ctor, 23217–23238 | NONE | Network/external browser. Banner text branches on `installedCollectionSummary()`. |
| Orphaned-library warning banner | If this library has <50 texts and a `/Applications/*/…Tool Data/library` holds ≥50 more, shows a warning naming that folder. Changes nothing. | ctor, 23241–23299 | NONE | Scans `/Applications` subdirs at construction (up to 500 files each) — startup disk cost; hard-coded folder names "Diamond Cutter Tool Data" / "ALL Tool Data". |
| **Collections…** (SHELVE, big) | Runs `checkCollectionUpdates()` — HEADs the three official S3 ZIPs, scrapes the ALL library page, opens a dialog with per-collection status + "Download && install…". | 23315/23722 → `checkCollectionUpdates` 24839 | `officialCollectionUrls` (3 with no network; 4 with an extra link), `parseCollectionLinks`, "front-door button on the ribbon" | Network: `asianlegacylibrary.org` + `all-library-docs.s3.us-west-2.amazonaws.com`. Nested `QEventLoop` with no cancel — up to 6 s + 3×4 s of frozen UI. Writes QSettings. Disabled in sweep mode. |
| "Download && install…" button (per row in that dialog) | Confirms, downloads the ZIP to `QDir::temp()`, unzips via `installZipPath()`, stores the ETag fingerprint. | `downloadAndInstall` 25082 | NONE | Network GET; writes temp file; runs `/usr/bin/unzip`; nested event loop (cancel wired to `abort`). Partial file removed on failure. |
| **Install…** (SHELVE, big) | File picker taking a mixed selection: `.zip` → `installZipPath()`, everything else → `importPaths()` (copy into `my_materials/`). | 23319/23724 → `installAny` 24739 | NONE | `QProcess /usr/bin/unzip -o` with a 10-minute wait, on the GUI thread. Destination folder inferred from the ZIP filename (kangyur/tengyur/sungbum/varanasi), else the base name. |
| **Import…** (SHELVE, big) | File picker; copies chosen docs into `library/my_materials/`, converting `.docx`/`.rtf` via `textutil`. | 23320/23725 → `importFiles` 25148 → `importPaths` 25158 | NONE | Keep-both renaming (never clobbers); failed copies reported `[FAILED - not imported]`; runs `/usr/bin/textutil` (macOS-only). Refreshes tree + list. |
| **List view** toggle (FIND) | Switches the stack between folder tree and the 7-column flat catalog table; fills it on turn-on. | 23327/23702 → `fillList` 24686 | "catalog populated from the library" | Full recursive `QDirIterator` over the whole library on every toggle — synchronous, no progress, 8,988 rows in practice. |
| **Maintenance… → Check for collection updates…** | Same as **Collections…**. | 23357 | see Collections… | Duplicate entry point (deliberate). |
| **Maintenance… → Import data release…** | Full data-release import pipeline (see §5 / final report). | 23360 → `importDataRelease` 25758 | Release discovery + spine-pointer traversal check at 5804 (`app_selftest`); the copy/build/switch itself is **NONE** | Runs `python3 tools/build_spine.py` via QProcess on the GUI thread; deletes and replaces files in `data/`; writes `build/spine_current.txt`. Also exported globally as `g_importRelease`. |
| **Maintenance… → Update search index** | Proxy-clicks the hidden `indexBtn`. | 23506 → `updateIndex` 25993 | `indexResultHtml` 5 checks (completed / stopped / write-failures / remainder / cancelled refold) | Indexing runs on the GUI thread (measured 190 s over 8,988 files); modal progress with Stop; writes `library/.index.db`. Re-entrancy guarded by a static bool. |
| **Maintenance… → Send to OCR…** | Proxy-clicks the hidden `ocrBtn`. | 23509 → `sendToOcr` 26194 | NONE | Scans `/Applications` for a "…Tib…OCR…" bundle; `QProcess::startDetached("/usr/bin/open")`; otherwise shows the BDRC download link. |
| **Maintenance… → Legacy font rescue (UTFC)…** | Proxy-clicks the hidden `utfcBtn`. | 23511 → `convertLegacy` 25314 | NONE | Requires a locally built `build/utfc/utfc` (GPL v3, external); prompts for the UTFC-master folder; `QProcess` with a 5-minute wait; writes `my_materials/*.utfc.txt`; remembers `utfc/dir` in QSettings. |
| **Maintenance… → Legacy font rescue (py-tiblegenc, 28+ encodings)…** | Runs the optional Python venv converter, asks for encoding + file, saves the rescued text, offers to open it in Input. | 23513 → `convertLegacyTiblegenc` 25215 | NONE | Path guessing `…/../../../../build/tiblegenc_venv/bin/python`; runs Python via QProcess; dev-oriented setup message ("run `bash tools/setup_tiblegenc.sh` in Terminal") shown to end users. |
| **Maintenance… → Translator's survey (selected text)…** | Surveys the current tree/table selection via `g_surveyFile`. | 23522 → `surveySelected` 25436 | NONE | No-op with an information box if nothing/only a folder is selected. |
| **Survey…** (STUDY, big) | Same as the menu entry. | 23531–23538 | NONE | Duplicate entry point (deliberate, audit C1). |
| **By author…** (STUDY, big) | Builds the author index, focuses the author box, shows the primer or the index error. | 23539–23556 → `buildAuthorIndex` 24569 | "the author index loads, or says why it could not"; "the author front door exists on the ribbon" | Reads `data/extracted/author_index.json`; error surfaced, never silent. |
| Author search box (Enter) | `showAuthorCandidates()` — tiered candidate list (exact / phonetic etc.), honest "No person of that name". | 23557–23566 → 24412 | "a person spelled six ways is ONE candidate"; "an unknown name is answered honestly"; tier-skin checks | Session-remembered? No (`sess::remember` not applied to this box). |
| Library name search box (Enter) | `searchNames()` — filename substring search + the three filters, capped at 60 hits. | 23570/23729 → 26159 | NONE | Recursive directory walk per query; cap disclosed in the output. |
| Collection / status / language combo boxes | Re-run `searchNames()` as filters. | 23578–23596, `passesFilters` 26120 | NONE | Filters imply ACIP-catalog files: unrecognized filenames are excluded entirely when any filter is on. Persisted via `sess::remember`. |
| Tree selection change | `showInfo()` — catalog card: size/date, survey link, English title, subjects, Vinaya banner, person card, ACIP decode, BDRC scan link, 700-byte preview. | 23730 → `showInfo` 25663 | Vinaya banner: 4 checks; person card: 3 checks | Reads the file head; BDRC scan URL is verified against a local table (`bdrcScanUrlChecked`). |
| Tree **double-click** | Opens the file in the Overlay (`open_`) and records it in Progress. | 23734 | NONE | — |
| Table **double-click** | Same, from the flat catalog row. | 23706 | NONE | — |
| Table selection change | `showInfo()` for that row. | 23715 | NONE | — |
| Tree expand / collapse | Persists the expanded-path set to QSettings `library/expanded`. | 23632–23652 | NONE | A QSettings read+write on **every** expand/collapse; list grows unbounded (stale paths are only filtered at load). |
| Info-panel link `openfile:` | Opens that file in the Overlay, logs it. | 23670–23675 | "a bibliography link href resolves to a file that is really on disk"; "…has visible text to click" | — |
| Info-panel link `authortable:` | Fills the catalog table with one person's works and flips to List view. | 23676–23680 | NONE | — |
| Info-panel link `authorback:` / `authorname:` / `author:` (pid) | Back to candidates / by-name ledger / person ledger. | 23681–23686 | by-name ledger link check; "arrived here by sound" / no-false-doubt checks | — |
| Info-panel link `survey:` | Hands the file to the survey pane. | 23687 | NONE | Silently does nothing if `g_surveyFile` is unset. |
| Info-panel link `inputrescue:` | Opens a rescued text in the Input pane. | 23689 | NONE | Silently does nothing if `g_openTextInInput` is unset. |
| Info-panel link `http…` | `QDesktopServices::openUrl` — BDRC person/scan pages, Treasury of Lives biographies/searches, ALL library page. | 23693 | 8 TOL/BDRC URL-shape checks (bare-id 404 never emitted, unharvested id → search, Wikidata-matched flagged) | External network/browser. |
| "N works by this author" link on the Overlay person badge | `g_openAuthorByPid` raises this pane and shows the ledger. | 23494–23503 | NONE | Cross-pane global hook. |
| Overlay person card / author badge (`g_personCardByName`, `g_personBadgeForFile`) | Renders the PERSON block and the author line consumed by other panes. | 23370–23499 | person-card checks (no RDF-API hrefs; links resolve) | Bound at construction (was previously lazy — a real past defect). |
| Recently-opened list (initial info panel) | Up to 10 previously opened texts, titled from the catalog. | `showRecents` 26097 | NONE | Reads Progress; skips files no longer on disk. |

**Count: 30 user-visible controls/actions listed; 20 have NONE automated coverage.**

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `LibraryPane(root, progress, open)` | 23204 | Builds the pane, ribbon, tree/table/info split; installs the global hooks `g_importRelease`, `g_personCardByName`, `g_personBadgeForFile`, `g_openAuthorByPid`. | MainWindow |
| `selfTest(log)` | 23744 | 30+ assertions over index messaging, collection-URL parsing, TOL/BDRC link shapes, person cards, author search, Vinaya banner. | `--selftest` / ctest `app_selftest` |
| `bestFile(paths)` | 24062 | Picks a readable file for a work key, avoiding `META.txt` sidecars. | `showAuthorWorks` |
| `ensureFileIndex()` | 24083 | One walk of the library building `filesByWork_` (`.ACT/.INC/.ACE` included). | `buildAuthorIndex` |
| `skinFor(tier)` | 24118 | Tier visual skin (ink/wash/glyph) for author-match confidence. | `authorRowHtml`, selfTest |
| `tierName(tier)` | 24140 | Human label for a match tier. | `authorRowHtml` |
| `authorRowHtml(hit)` | 24144 | One candidate row in the author-candidates list. | `showAuthorCandidates` |
| `authorPrimerHtml()` | 24185 | Explains how to type a name; shown before any query. | By-author button, `showAuthorCandidates` |
| `showAuthorWorks(pid)` | 24207 | Full bibliography ledger for one person, uncapped; "arrived here by sound" strip on weak arrivals. | anchor `author:`, `g_openAuthorByPid`, selfTest |
| `fillListForPerson(pid)` | 24337 | Restricts the catalog table to one person's local works. | anchor `authortable:` |
| `showAuthorWorksByName(name)` | 24359 | Ledger for a catalog author name with no person record. | anchor `authorname:`, selfTest |
| `showAuthorCandidates(query)` | 24412 | Runs `allcore::matchPeople`, renders tiered candidates. | author box, anchor `authorback:` |
| `personRecord(pid)` | 24536 | Lazy read of `data/extracted/author_index.json` → one person. | badges, ledgers |
| `invalidateAuthorIndex()` | 24555 | Drops all author caches after new texts arrive. | `installZipPath`, `importPaths` |
| `buildAuthorIndex()` | 24569 | Loads the people bank; records `authorIndexError_` instead of failing silently. | By-author button, `showAuthorCandidates`, selfTest |
| `englishTitle(fileName)` | 24633 | Lazy-loads catalog titles + Sungbum/R6 subject banks; maps a filename to a title. | `fillList`, `showInfo`, `recentLink`, ledgers |
| `fillList()` | 24686 | Rebuilds the 7-column flat catalog from a full recursive walk. | List-view toggle, installs/imports |
| `installAny()` | 24739 | Mixed ZIP/document install entry point. | Install… button |
| `installZipPath(zip)` | 24765 | `unzip -o` into `library/<name>/`, refreshes tree + table, reports counts. | `installAny`, `downloadAndInstall` |
| `parseCollectionLinks(html)` | 24811 | Extracts `.zip` hrefs from the ALL library page. | `officialCollectionUrls`, selfTest |
| `officialCollectionUrls(html)` | 24828 | The three official S3 URLs always, plus anything else published. | `checkCollectionUpdates`, selfTest |
| `checkCollectionUpdates()` | 24839 | HEAD each collection, compare stored ETag, build the collections dialog. | Collections… button, Maintenance menu |
| `downloadAndInstall(parent,url,name,etag,bytes)` | 25082 | Confirm → GET to temp → install → store ETag. | Collections dialog |
| `importFiles()` | 25148 | Document picker for personal materials. | Import… button |
| `importPaths(files)` | 25158 | Copy-with-keep-both into `my_materials/`, `textutil` conversion, refresh. | `importFiles`, `installAny` |
| `convertLegacyTiblegenc()` | 25215 | py-tiblegenc external-engine rescue lane. | Maintenance menu |
| `convertLegacy()` | 25314 | UTFC external-binary rescue lane (16 encodings, UTF-16LE→UTF-8). | Maintenance menu / hidden button |
| `surveySelected()` | 25436 | Resolves the current selection and hands it to `g_surveyFile`. | Survey button, Maintenance menu |
| `loadPersons()` | 25458 | Loads `catalog_works.json`, `persons_bdrc.json`, `acip_person_links.json`; flag set only on success. | badges, `personHtml`, `buildAuthorIndex` |
| `personHtml(workKey)` | 25504 | Person block on a text's catalog card. | `showInfo`, selfTest |
| `vinayaBannerHtml(workKey)` | 25638 | States the Vinaya genre (and its own limits) for catalogued works. | `showInfo`, selfTest |
| `showInfo(path)` | 25663 | The whole right-hand catalog card. | tree/table selection |
| `importDataRelease()` | 25758 | See §5. | Maintenance menu, `g_importRelease` |
| `indexResultHtml(stats,files,lines,libTotal)` | 25921 | Pure function: what the pane says after an index run. | `updateIndex`, selfTest |
| `updateIndex()` | 25993 | Modal, stoppable FTS index update over `library/.index.db`. | Update-search-index button/menu |
| `logOpen(path)` | 26068 | Records an open into Progress (drives Recents). | all open paths |
| `fileLink(path,base)` | 26074 | `openfile:` anchor for search results. | `searchNames` |
| `recentLink(path)` | 26085 | One-line recents entry with the path in the tooltip. | `showRecents` |
| `showRecents()` | 26097 | Initial info-panel content. | ctor, `searchNames` |
| `passesFilters(path)` | 26120 | Applies the three combo filters via `decodeAcipFilename`. | `searchNames` |
| `searchNames()` | 26159 | Filename search / filtered browse. | search box, filters |
| `sendToOcr()` | 26194 | BDRC OCR app hand-off or download link. | Send-to-OCR button/menu |

## 4. Dependencies

**Folders created/written**
- `<dataRoot>/library/` (root), `library/my_materials/`, `library/ocr_out/` — created in the constructor.
- `library/<collection>/` — created and unzipped into by `installZipPath`.
- `library/.index.db` — the FTS search index (`allcore::LibraryIndex`).
- `<dataRoot>/data/` — release files replaced by `importDataRelease` (staged as `*.importing~`, then swapped).
- `<dataRoot>/build/spine_current.txt` — the spine pointer written by `importDataRelease`.
- `QDir::temp()/<name>-collection.zip` — download staging, removed afterwards.

**Data files read** (all under `<dataRoot>/data/extracted/`, all optional; absence is reported, not guessed)
`catalog_works.json`, `persons_bdrc.json`, `acip_person_links.json`, `author_index.json`, `catalog_titles.json`, `sungbum_subjects.json`, `r6_work_subjects.json`, `work_subjects.tsv`. Plus `<dataRoot>/tools/build_spine.py`, `<dataRoot>/tools/tiblegenc_convert.py`, `<dataRoot>/build/utfc/utfc` + `utfc_dir.txt`, `<dataRoot>/build/tiblegenc_venv/bin/python`.

**Network hosts**
- `https://asianlegacylibrary.org/library/` — GET, scraped for `.zip` links (6 s budget).
- `https://all-library-docs.s3.us-west-2.amazonaws.com/{KANGYUR,TENGYUR,SUNGBUM}.zip` — HEAD (4 s each) then GET on download.
- `https://library.bdrc.io/show/bdr:<pid>` — outbound links only.
- `https://treasuryoflives.org/...` (biographies + `search/by_name/`) — outbound links only.
- `https://buda-base.github.io/tibetan-ocr-app/` — outbound link only.
- BDRC scan URLs via `bdrcScanUrlChecked` — outbound links only.

**External processes**: `/usr/bin/unzip`, `/usr/bin/textutil`, `/usr/bin/open`, `python3` (spine builder), the venv python (tiblegenc), `build/utfc/utfc`.

**QSettings** (organisation `ALL`, application `TranslationTool`)
`library/expanded` · `collections/etag/<name>` · `collections/lastCheck` · `collections/lastResult` · `utfc/dir` · plus `sess::remember` keys `library/search`, `library/collection`, `library/status`, `library/language`.

## 5. Release questions

**Update / import pipeline (highest priority — the weekly dictionary update will be built on this)**
- `Import data release…` shells out to `python3 <dataRoot>/tools/build_spine.py`. **Does the shipped app bundle contain `tools/build_spine.py` and a `data/` folder at all?** In a signed, notarized `.app` this path resolves inside/next to the bundle; if it is absent, every import ends at "The spine builder is missing from the data folder."
- The pipeline depends on a **system `python3`** (`QStandardPaths::findExecutable`). On a clean Mac without Xcode CLT there is none. Decide: ship a bundled interpreter, port the builder into `allcore`, or ship prebuilt spines and make import a download-and-swap.
- The builder is invoked with `--release-dir <dataRoot>/data` and the app parses a single line `SPINE_DB=` from merged stdout/stderr. **This contract is untested.** A builder change that renames or reorders that line silently turns every successful build into "The spine build FAILED".
- **Validation is filename-only**: `discoverReleasePackage` requires an `hgm_dictionary_v*.json.gz` and picks the highest version number. There is no checksum, no signature, no schema check, no gz integrity test before the old release files are deleted. For an auto-update feature this needs a hash/signature step.
- The old `data/` files are **removed before** the new ones are renamed into place. The failure message for a failed rename is honest ("old gone, new stranded") but the user is left to fix it in Finder. Decide whether an automatic rollback is required before this becomes weekly.
- The new spine only takes effect **after a restart**, and the message says so. A weekly updater needs a decision on hot-swap vs. prompt-to-relaunch.
- Collections check writes `collections/etag/<name>` only after a *successful install*; a collection installed manually shows "installed, but not by this checker". Confirm this wording ships.
- The whole collections check runs in **nested event loops** on the GUI thread (up to ~18 s of unresponsive UI, no cancel), and the download and unzip likewise (unzip waits up to 10 minutes). Verify behaviour on slow/captive-portal networks before a demo.

**Ship / hide candidates**
- **Legacy font rescue (UTFC)** and **(py-tiblegenc)** both require a developer-only, terminal-run setup script and refer the user to `tools/setup_utfc.sh` / `tools/setup_tiblegenc.sh` in "the project folder". For an end-user release these should be hidden, or the message rewritten as a real user instruction.
- **Send to OCR…** finds the BDRC app by scanning `/Applications` for a folder whose name contains both "OCR" and "Tib". Confirm the shipped app's actual name matches, or the feature always degrades to the download link.
- The **orphaned-library guard** hard-codes the two historical data-folder names; confirm the release's folder name is one of them or the guard never fires.
- The `library/expanded` QSettings list is appended to on every expand and never pruned of nonexistent paths on write — verify it does not grow without bound across a long-lived install.

**Coverage gaps worth closing before release**
- No automated coverage at all for: install (ZIP), import (files), the data-release copy/build/switch, index invocation (only its *messages* are tested), OCR hand-off, both legacy-rescue lanes, filename search/filters, `fillList`, and every open path except the author-ledger links.
- `showInfo` writes a 700-byte raw preview of the selected file into rich text — escaped, but confirm behaviour on binary files that pass the suffix test (`.inc`, no-suffix).
