# GoferPane — release audit

## 1. What the pane is

`GoferPane` (app/main.cpp:16022–17313) is the "Search" tab: the classic ACIP
Gofer input-center search rebuilt on the C++ `allcore` engine (the proven port
of Ted Lemon's lex/expr/near semantics, backed by the `gofer_smoke` ctest
suite). Its own banner describes it as "up to eight terms across the corpus,
the published apparatus, Spotlight, and your own folders, combined with OR /
AND / NEAR." It is a top-level tab in the main window's `tabs` widget, added at
main.cpp:35986 as `tabs.addTab(goferPane, "Search")` — immediately after
"Library" and "Files" and before "Convert" — so the user reaches it by clicking
the Search tab. It is also reachable programmatically: the global hook
`g_goferQuery` (main.cpp:35982) calls `runQuery()` (main.cpp:16210) and raises
the pane, driven from the Hunt palette (main.cpp:35178) and from another pane's
"search this" affordance (main.cpp:12184). Inside the pane there are three
sub-tabs on a nested `QTabWidget`: **Search Setting** (index 0), **Saved
Search** (1), **Search Results** (2); `find()` force-switches to index 2 when a
search starts.

Query grammar: `buildQuery()` (main.cpp:16736) quotes each non-empty term and
joins them — `OR` for combiner index 0, `NEAR/1000000` for "AND (same file)",
and `NEAR/<proximity>` (default 3 when the box is <= 0) for NEAR. That string
goes to `allcore::goferSearch` (corpus), `allcore::LibraryIndex::search`
(indexed folders) and `allcore::goferSearchFiles` (unindexed folders).

## 2. User-visible functions

| Control/action | What it does | Code anchor (main.cpp) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Term fields 1–8 (`QLineEdit`) | Eight "go for" terms, placeholder "term N — ACIP or English"; non-empty ones are quoted into the query | ctor, 16037 / 16070; `buildQuery` 16736 | selfTest: "eight go-for fields present"; values pinned in the fold drill | Case picks the reading (ACIP uppercase / wylie lowercase) — only stated in the no-match hint |
| Enter in any term field | `returnPressed` runs `find()` (was inert before the fix; comment at 16044 documents it) | ctor 16050 / 16074 | selfTest: "Enter in a term box runs the search (the returnPressed connect itself)" | Fired via `emit` in the test, not a real key event |
| Combiner combo (OR / AND (same file) / NEAR) | Selects the join operator for `buildQuery` | ctor 16057; `buildQuery` 16742 | Indirectly (set to 0 in every drill); no test exercises index 1 or 2 | **AND and NEAR modes have NO assertion at pane level** |
| "within lines (NEAR)" box | Proximity N for `NEAR/N`; <= 0 falls back to 3 | ctor 16066 | NONE | Free-text `QLineEdit`, not a spinbox: "abc" silently becomes 3 |
| Fold combo (ignore space+caps / ignore space / ignore nothing) | Strict post-filter over hit windows via `goferFoldKeep` | ctor 16082; `goferFoldKeep` 16776; call site 17069 | selfTest: "the default fold keeps both cases", "\"ignore nothing\" DROPS the case-mismatched file…", "the fold discloses how many windows it hid"; plus app_selftest lines 38351-38358 pinning `goferFoldKeep` directly | Applies to folder hits only — not to corpus, apparatus or Spotlight results |
| Scope list rows 0–2 (corpus / apparatus / Spotlight checkboxes) | Toggle the three fixed sources; persisted to QSettings | `loadDirs` 16700, `saveDirs` 16719; branches in `find` 16840/16887/16925 | selfTest: "corpus row present and checked"; "Spotlight source row present (opt-in)" | Only presence/default-checked is pinned; no test that unchecking actually excludes a source |
| Scope list rows 3+ (user folders, checkable) | Each checked folder is scanned (indexed via `.index.db`, else file walk) | `loadDirs` 16712; `find` 16999 | Exercised by the fold drill (a temp dir is added and searched) | |
| "Add folder…" | Directory picker, appends a checked row, persists | ctor 16155 | NONE (no test clicks it) | Uses `safeGetExistingDirectory` |
| "Remove" | Deletes the selected row if row >= 3, persists | ctor 16160 | NONE | Fixed rows are protected by `kFirstDirRow` |
| "Duplicate" | Copies the selected user row with its check state | ctor 16165 | NONE | |
| "Save search" | Prompts for a name, stores 8 terms + combiner + proximity + fold under `gofer/saved/<name>` | ctor 16192 → `saveSearch` 17245 | NONE | Name is used verbatim as a QSettings group; no overwrite warning |
| "Citation web…" | Loads `data/extracted/citation_web.json`, opens a modeless dialog listing connected work-pairs and their shared passages | ctor 16126 → `showCitationWeb` 16241, `loadCitationWeb` 16229 | selfTest: "Citation web loads (edges from the batch build)" (loader only) | The dialog itself is never opened by any test |
| "Stop" | Sets `stopped_`; aborts folder scans and Spotlight fan-out between pumps | ctor 16118; honored 16941/17002/17023 | NONE | Disabled until a search runs; corpus/apparatus phases are not interruptible |
| "Find" | Runs the whole search and switches to Search Results | ctor 16204 → `find` 16813 | selfTest: "Find reaches the corpus through the Gofer engine" (drives `findB_->click()`, the SQA TEST-5 rule); gauntlet_walk | Re-entrancy guarded by `finding_` (PERF-4) |
| Saved Search list — "Load" button and double-click on a row | Restores terms/combiner/proximity/fold and returns to Search Setting | ctor 16193/16195 → `loadSearch` 17261 | NONE | Double-click ignores which item, uses `currentItem()` |
| Saved Search — "Delete" | Removes `gofer/saved/<name>` from QSettings | ctor 16197 | NONE | No confirmation dialog — irreversible click |
| Result link `goferopen:<enc path>\|<line>` | Opens the file in the Overlay pane at that line via `g_openAtLine` | anchorClicked 16172; emitted 17150 | selfTest: "a result link carries the file's real path — the anchor payload survives rendering" | Payload built with one-pass multi-arg `.arg()`; the SQA TEST-5 comment at 17147 records that chained `.arg()` previously mangled **every** file row because `anchorEnc` writes `%2F` and ate the `%2` marker. Regression risk if anyone re-chains `.arg()` here |
| Result link `lookup:<pct-encoded term>` | Sends the term to the Lookup pane via `g_lookupQuery` | anchorClicked 16185; emitted 17203 | NONE | **Dead in practice:** the "look up:" row iterates `lastTerms_`, which is declared (17294) and read (17199) but never assigned anywhere — the row can never render |
| Spotlight result link (`file://…`) | `QDesktopServices::openUrl` opens the document in its default app | anchorClicked 16189 | NONE | Opens arbitrary local files from a search result |
| Tooltips (combiner, proximity, Add/Remove/Duplicate/Save/Stop, Citation web, Spotlight row) | Explanatory hover text | 16060, 16068, 16099–16130, 16706 | NONE | |
| Result footer / cap notices | Discloses rendered vs fetched vs total, skipped files, fold drops, display caps, "search stopped" | 17178–17240; `corpusCountLine`, `goferSkipNotice` | selfTest: five DATA-1 checks on `corpusCountLine`, three on `goferSkipNotice`, and "the result footer never claims counts were SHOWN…" (DATA-5) | Honesty-critical (house rule 3) |

Counts: **22** user-visible controls/actions listed; **12** with NONE.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `GoferPane(Spine&, root)` | 16024 | Builds all three sub-tabs, wiring and persistence | main.cpp:35981 |
| `runQuery(term)` | 16210 | Clears fields, sets term 1, forces OR, runs `find()` | `g_goferQuery` (35982), Hunt palette (35178), 12184 |
| `loadCitationWeb()` | 16229 | Lazy-loads `data/extracted/citation_web.json` edges | `showCitationWeb`, selfTest |
| `showCitationWeb()` | 16241 | Builds the citation-web dialog | "Citation web…" button |
| `selfTest(QStringList&)` | 16294 | 40-odd pane and cross-cutting assertions | main.cpp:38270 (`app_selftest`) |
| `addDirRow(text, checked)` | 16686 | Appends a checkable scope row | ctor, `loadDirs`, Add/Duplicate, selfTest |
| `loadDirs()` | 16700 | Seeds the three fixed rows + saved user folders | ctor |
| `saveDirs()` | 16719 | Persists scope rows and their check states | Add/Remove/Duplicate, `find` |
| `buildQuery()` | 16736 | Terms → quoted OR / NEAR/1000000 / NEAR/N string | `find` |
| `goferFoldKeep(mode, window, terms)` (public static) | 16776 | Pure strict post-filter for the fold combo | `find`, selfTest, app_selftest 38351 |
| `find()` | 16813 | The whole search: corpus, apparatus, Spotlight, folders, render | Find button, Enter, `runQuery` |
| `saveSearch()` | 17245 | Writes the named saved search | Save search button |
| `loadSearch()` | 17261 | Restores a saved search | Load button, list double-click |
| `refreshSaved()` | 17276 | Repopulates the saved list (with an empty-state hint) | ctor, save, delete |

## 4. Dependencies

**Data / files read**
- `allcore::Spine` (the SQLite FTS5 spine) via `allcore::goferSearch` and `spine_.corpusCount`.
- `<root>/data/extracted/citation_web.json` — Citation web edges (generated by `tools/build_citation_web.py`; absence is reported honestly in a message box).
- `<root>/library` — default user-folder row on a fresh install.
- `<dir>/.index.db` per scope folder — if present, `allcore::LibraryIndex`; otherwise a bounded file walk (`allcore::goferSearchFiles`).
- Globals `g_appNotes` / `g_appBib` — the published apparatus, searched in memory.
- selfTest side-files under `QDir::temp()`: `all_selftest_restore`, `all_selftest_tp5_short.txt`, `all_selftest_ro_dir`, `all_selftest_test2_ok.txt`, `all_selftest_banner_root`, plus a `QTemporaryDir` fold fixture.

**Processes / network**
- `/usr/bin/mdfind` — one `QProcess` per probe term, concurrent, 6 s shared deadline, capped at 40 documents (main.cpp:16957).
- No network host is contacted by this pane. `TimedNam` appears only inside `selfTest` as a FAIL-4 timeout probe (no request is issued).

**QSettings ("ALL"/"TranslationTool")**
- Read+written: `gofer/corpusChecked`, `gofer/apparatusChecked`, `gofer/spotlightChecked`, `gofer/dirs`, `gofer/dirsChecked`.
- Read+written: `gofer/saved/<name>/{f0..f7, combiner, proximity, fold}`; group list enumerated by `refreshSaved`; removed by Delete.
- Session restore (`sess::remember`, ctor 16087): `gofer/f0..f7`, `gofer/combiner`, `gofer/proximity`, `gofer/fold`.
- selfTest saves and restores the five scope keys around its fold drill.

**Test suites** — `app_selftest` (app/CMakeLists.txt:89, calls `goferPane->selfTest` at main.cpp:38270), `gauntlet_walk` (300-step random UI walk, app/CMakeLists.txt:92), and, for the engine behind the pane, `gofer_smoke` (core/CMakeLists.txt:119, run against `build/hgm_spine_v27_2.db` + `tests/fixtures/acipdocs`) and `gofer_bound_smoke` (core/CMakeLists.txt:427). `dmp_smoke`, `collation_smoke`, `font_smoke` do not touch this pane.

## 5. Release questions

- `lastTerms_` is never assigned, so the "look up:" link row (main.cpp:17199) can never render. Wire it in `find()` or delete the block and the `lookup:` anchor handler — shipping dead code that advertises a cross-pane jump is worse than not offering it.
- No pane-level assertion covers combiner modes 1 ("AND (same file)" = `NEAR/1000000`) or 2 (`NEAR/N`), nor the proximity box. That is the Gofer grammar itself — the headline feature — resting entirely on `gofer_smoke` at the engine layer.
- Proximity is a free-text `QLineEdit`; any non-numeric text silently becomes `NEAR/3` with no feedback. Should it be a `QSpinBox`, or at least say what it used?
- "Delete" on a saved search has no confirmation, and Save silently overwrites an existing group of the same name.
- Spotlight scope shells out to `mdfind` with the user's term interpolated into a `kMDItemTextContent == "*%1*"cd` query string; no quoting/escaping of `"` in the term. Worth a look before release.
- The fold filter applies only to folder hits; corpus, apparatus and Spotlight results ignore the fold setting while the combo reads as global. Is that the intended (Gofer-heritage) behaviour, and is it disclosed?
- `Stop` cannot interrupt the corpus or apparatus phases — only folder scans and the Spotlight wait loop.
- Add folder / Remove / Duplicate / Save / Load / Delete / Citation-web dialog have no automated coverage beyond whatever `gauntlet_walk` happens to hit at seed 1.
