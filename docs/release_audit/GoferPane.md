# Release audit — `GoferPane` (app/main.cpp:19091–20532)

## 1. What it is

`GoferPane` is the **Search** pane: the classic ACIP input-center "Gofer"
search, rebuilt on the C++ `allcore` engine that ports Ted Lemon's
lex/expr/near semantics. Its own banner (main.cpp:19110–19114) describes it as
"the classic input-center search, rebuilt: up to eight terms across the corpus,
the published apparatus, Spotlight, and your own folders, combined with OR /
AND / NEAR."

Who uses it: the translator or cataloguer who wants to know *where a phrase
occurs* rather than *what a word means* — across Geshe Michael Roach's aligned
corpus, the published footnote/bibliography apparatus, arbitrary folders of
ACIP/Wylie text files on disk, and (opt-in) everything Spotlight can see on the
Mac. It is the pane the app's Find-in-Files (⇧⌘F) and Goto Concordance
(⌃⌥⇧↓) route to, so it is also the general-purpose "find this string in my
files" surface for the whole application.

Size: **1,442 lines** (19091–20532), of which the constructor is 19101–19301
(~200 lines), `find()` — the whole search, render and disclosure pass — is
20038–20469 (~430 lines, the pane's hot path and the bulk of its risk), and
`selfTest()` is 19385–19919 (~535 lines).

It is constructed at main.cpp:45448 and added to the tab set as
`tabs.addTab(goferPane, "Search")` (45453); the ribbon places it first in the
**Research** group (`mkGroup("Research", {"Search", "Lookup", "Sanskrit",
"Convert", "Analysis", "Study", "Compare"})`, 46059–46060). Inside the pane a
nested `QTabWidget` carries three sub-tabs: **Search Setting** (index 0, added
19214), **Saved Search** (1, added 19228) and **Search Results** (2, added
19247); `find()` force-switches to index 2 the moment a search starts
(20055/20060).

Query construction is `buildQuery()` (19992–20005): every non-empty term is
wrapped in double quotes and the list is joined by `OR` (combiner index 0),
`NEAR/1000000` (index 1, "AND (same file)") or `NEAR/<proximity>` (index 2,
with a silent fallback to 3 when the box parses to ≤ 0). That one string is
handed to `allcore::goferSearch` (corpus, 20068), `allcore::LibraryIndex::search`
(folders with a `.index.db`, 20267) and `allcore::goferSearchFiles` (unindexed
folders, 20296). The apparatus branch (20113–20156) and the Spotlight branch
(20157–20231) do **not** use the query string at all — they re-derive their own
matching from the raw term list.

**What its selfTest already asserts.** `selfTest(QStringList&)` (19385–19919)
runs **51** `check(...)` conditions under the `app_selftest` ctest suite
(`app/CMakeLists.txt:105`, invoked at main.cpp:49132). Only about **17** of
those are about this pane; the remaining ~34 are cross-cutting house-rule pins
that happen to be hosted here — the reserved-green binding-authority tokens
(19415–19424), WCAG AA contrast for every paper ink and every chrome ink
(19442–19489), the backup/restore safety drill (19504–19517), the `saveOrWarn`
write-failure branches and the TP-5 short-write fixture (19529–19644), the
`TimedNam` 20-second transfer timeout (19654), `jsonObjectOrFlag` HTML-vs-JSON
(19665–19668), the Library banner's META-companion count (19690), the
ribbon-caption re-inking on palette change (19709/19714), and the
diagnostic-report redaction (19748–19767).

The pane-specific checks are: eight term fields present (19394); corpus row
present and checked (19395); the Spotlight row present and named (19398);
`citation_web.json` loads with > 100 edges (19400); the four DATA-1
`corpusCountLine` honesty checks (19779–19793); the three PERF-1
`goferSkipNotice` checks (19801–19808); "Find reaches the corpus through the
Gofer engine" driven through `findB_->click()` (19815–19816); the DATA-5 ban on
"hit(s) shown" (19824); and the six-check fold drill over two real temp files
that also pins Enter-in-a-term-box and the `goferopen:` anchor payload
(19864–19899). `GoferPane::goferFoldKeep` is additionally pinned as a pure
predicate from the app-level Lookup block at main.cpp:49222–49229.

---

## 2. User-visible functions

### 2a. Search Setting tab — query controls (constructed 19108–19172)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Term fields 1–8** (8 × `QLineEdit`) | Eight "go for" terms, placeholder `term N — ACIP or English`; every non-empty one is quoted and joined into the query | 19120 (fields 1–4), 19152 (fields 5–8) → `buildQuery()` 19992 | selftest 19394 "eight go-for fields present"; driven with real values at 19810 and 19871 | Case picks the reading (ACIP uppercase vs wylie lowercase) and that is stated **only** in the no-match hint (20441–20443) — never in the placeholder or a tooltip. No field has a label or accessible name beyond the placeholder. |
| **Enter** in any term field | `returnPressed` → `find()` | 19128 / 19155 | selftest 19891–19896 ("Enter in a term box runs the search (the returnPressed connect itself)") | The check fires `emit fields_[0]->returnPressed()`, not a real key event — a broken focus/event path would still pass. The comment at 19122–19127 records that Enter was inert for the whole prior life of the pane. |
| **Combiner combo** (OR / AND (same file) / NEAR) | Picks the join operator: `OR`, `NEAR/1000000`, `NEAR/<n>` | 19134, items 19135, tooltip 19136 → `buildQuery()` 19999–20004 | index 0 only — set at 19812 and 19875 inside other drills. **Indexes 1 and 2 have no assertion anywhere in this pane.** | This is the Gofer grammar itself, the pane's headline feature, and it rests entirely on the engine-level `gofer_smoke`. Also: the combiner is **ignored by the Spotlight branch** and only crudely honoured by the apparatus branch (`anyMode = currentIndex()==0`, 20117), where "AND" means "all terms somewhere in the same note", not same-file NEAR. The combo reads as global; it is not. |
| **"within lines (NEAR)"** box | Proximity N for `NEAR/N`; `toInt()` of the text, and ≤ 0 becomes 3 | 19141 (label), 19142, tooltip 19143 → 20002–20004 | NONE | A free-text `QLineEdit`, not a `QSpinBox`: `abc` silently becomes `NEAR/3`, and the default text is `0` which the tooltip says means "the same line" but which the code turns into 3. The tooltip and the behaviour disagree. |
| **Fold combo** (ignore space+caps / ignore space / ignore nothing) | Strict post-filter over hit windows: mode 1 requires case match on whitespace-simplified text, mode 2 requires the exact substring | 19160, items 19161–19163 → `goferFoldKeep()` 20017, call site 20313–20327 | selftest 19876–19890 (call-site drill over two real files: "the default fold keeps both cases", "\"ignore nothing\" DROPS the case-mismatched file", "the fold discloses how many windows it hid"); predicate pinned again at 49222–49229 | **Applies only to folder hits** (the loop at 20313 sits inside the per-directory block) — corpus, apparatus and Spotlight results ignore it entirely, while the combo sits at the top of the pane reading as a global setting. The comment at 19860–19863 records that a mutation disabling the call site (`if (false && foldMode > 0)`) left all 72 suites green, which is why the call-site drill exists. The combo has no label. |
| **Live-query session memory** | The eight term boxes, the combiner, the proximity box and the fold combo are bound to `sess::remember`, so a half-built search survives a quit | 19166–19171 → `sess::remember` 3378/3384 | NONE | Writes on **every keystroke** (`textChanged` → `sess::put`, 3380–3381). A term typed into the Search pane is persisted to the settings file; on this project a search term can itself be sensitive (see the diagnostic-report redaction rule the same selfTest pins at 19763). No cap, no debounce on these bindings. |

### 2b. Search Setting tab — scope list and action buttons (constructed 19173–19214, handlers 19250–19300)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Scope row 0 — "Geshe Michael Roach aligned corpus (all courses)"** (checkbox) | Searches the FTS5 spine through `allcore::goferSearch`, fetch cap 60, render cap 20 | row added `loadDirs()` 19952 (default **on**); read `find()` 20066; persisted `saveDirs()` 19977 | selftest 19395 — presence and default-checked **only** | No test asserts that *unchecking* it actually excludes the corpus. The row's display name is the only place the corpus is named; it is not disclosed which release/spine is behind it. |
| **Scope row 1 — "Published apparatus (footnotes + bibliography)"** (checkbox) | Substring-matches the in-memory `g_appNotes` / `g_appBib` | `loadDirs()` 19953 (default **on**); `find()` 20113–20156 | NONE | No check names this row at all. Its rendering is capped at 20 notes and then 40 cumulative entries (20134, 20146) — the header prints the **true** count `n` but silently stops rendering, and the two caps interact (25 matching notes leave only 15 bibliography rows). That cap is never disclosed, which is the exact shape of the DATA-1 finding this same selfTest pins for the corpus row. |
| **Scope row 2 — "This Mac (Spotlight — files open in their own app)"** (checkbox) | Fans out one `mdfind` process per probe term (term as typed + wylie→unicode + ACIP→EWTS→unicode), 6 s shared deadline, capped at 40 documents | `loadDirs()` 19956 (default **off**), tooltip 19960–19965; `find()` 20157–20231 | selftest 19398 — the row exists and carries the expected label. The branch itself is never executed by any test. | **Unescaped interpolation**: the term goes into `kMDItemTextContent == "*%1*"cd` (20186–20190) with no quoting of `"` or `\`. Up to 8 terms × 3 forms = **24 concurrent `QProcess`es**. The 40-document cap and the 6 s deadline are **not disclosed** in the result header (20217–20220 prints a bare "N document(s)") — a cap presented as a total, house rule 3. macOS-only path `/usr/bin/mdfind`, hard-coded. |
| **Scope rows 3+ — your own folders** (checkable rows) | Each ticked folder is searched: `LibraryIndex` if `<dir>/.index.db` exists, else a bounded file walk | added `addDirRow()` 19941; seeded `loadDirs()` 19966–19970 (default `<root>/library`); searched `find()` 20244–20409 | selftest 19868–19899 — the fold drill adds a real temp dir, ticks it and searches it end to end | `find()` calls `saveDirs()` (20058) before searching, so pressing Find silently **persists** whatever the scope list currently says. Row text is the folder path, editable only by Remove+Add. |
| **Add folder…** (button) | Directory picker → appends a ticked row → persists | 19177, tooltip 19178, handler 19250–19254 | NONE | Uses `safeGetExistingDirectory`. No de-duplication: the same folder can be added many times and will then be searched many times. |
| **Remove** (button) | Deletes the selected row when its index ≥ `kFirstDirRow` (3) → persists | 19181, tooltip 19182, handler 19255–19258 | NONE | Silently does nothing on rows 0–2 with no feedback about why. No confirmation. |
| **Duplicate** (button) | Copies the selected user row with its check state → persists | 19183, tooltip 19184, handler 19259–19265 | NONE | Tooltip says "to search the same folder with a different setting", but there is **no per-row setting** — rows carry only a path and a tick, so a duplicate row just searches the same folder twice and doubles its hit counts in the total. This control's stated purpose does not exist. |
| **Save search** (button) | Prompts for a name, writes 8 terms + combiner + proximity + fold under `gofer/saved/<name>` | 19187, tooltip 19188, handler 19289 → `saveSearch()` 20471–20485 | NONE | **Overclaims**: the tooltip promises "terms, operator, **scope**", and `saveSearch()` does not store the scope list at all (20475–20481). Loading a saved search therefore silently searches whatever folders happen to be ticked now. Also: the name is used verbatim as a QSettings group, so a name containing `/` creates nested groups that `refreshSaved()` will not list back correctly; an existing name is overwritten with no warning. |
| **Stop** (button) | Sets `stopped_`; aborts the Spotlight wait loop and the folder loop between pumps | 19191, disabled at birth 19192, tooltip 19193, handler 19196–19197; honoured 20198, 20249, 20269, 20294 | NONE | Cannot interrupt the corpus phase (20068) or the apparatus phase (20113) — both run to completion. The tooltip says "stop the search after the current folder", which is accurate for folders and silent about the two phases it cannot touch. Re-disabled at 20467. |
| **Find** (button) | Runs the whole search and switches to Search Results | 19198, handler 19300 → `find()` 20038 | selftest 19815–19816 "Find reaches the corpus through the Gofer engine" — driven via `findB_->click()` (the SQA TEST-5 rule: the button's own signal, not the slot) | Re-entrancy guarded by `finding_` + `FindGuard` (20043–20049, PERF-4) because the scans pump the event loop. The guard covers a second Find; it does **not** stop the user driving *other* panes' destructive actions during a pump. |
| **Citation web…** (button) | Loads `data/extracted/citation_web.json` and opens a modeless dialog of connected work-pairs and their shared passages | 19199, tooltip 19200–19205, handler 19206–19207 → `showCitationWeb()` 19323, `loadCitationWeb()` 19314 | selftest 19400 "Citation web loads (edges from the batch build)" — **the loader only**; the dialog is never opened by any test | Absence of the JSON is reported honestly in a message box (19325–19331). The tooltip's claim "Attested reuse only — no fuzzy matching" is a property of the Python batch builder, not of anything this pane verifies. |

### 2c. Citation web dialog (`showCitationWeb()`, 19323–19384)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Edge list** (rows: `A ↔ B — N shared`) | Selecting a row renders that pair's detail; row 0 auto-selected | list 19341, rows 19348–19355, `currentRowChanged` 19381, auto-select 19382 | NONE | No search/filter over the list; a large `citation_web.json` produces an unbounded flat list. |
| **Detail view** (`QTextBrowser`) | Shows up to 5 example shared passages: the shared Tibetan line plus Geshe Michael Roach's published English from both sides, with seq numbers | 19344, HTML 19357–19379 | NONE | The dialog is modeless with `WA_DeleteOnClose` (19334) and is **not** re-bound to the pane — nothing refreshes or closes it when a new search runs or the data reloads. The "up to 5" cap is stated in the body text (19363), which is good; the total `count` is printed, which is also good. Links are not enabled, so nothing here jumps into a text. |

### 2d. Saved Search tab (constructed 19216–19229, handlers 19289–19299)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Saved-search list** (rows = QSettings group names) | Lists every `gofer/saved/<name>` group; empty state is a disabled hint row "no saved searches yet — build one on the Search Setting tab and press Save search" | 19219 → `refreshSaved()` 20500–20513, hint 20507–20512 | NONE | The list is only repopulated on construction, save and delete — a group written by another window or by hand is not picked up. |
| **Double-click a row** | Loads that saved search and returns to Search Setting | 19291–19292 → `loadSearch()` 20486 | NONE | The handler ignores the item it was given and uses `saved_->currentItem()`; harmless today but it means the double-click and the Load button are literally the same code path with no distinction. Double-clicking the empty-state hint row is refused only because the row has `Qt::NoItemFlags` (20510). |
| **Load** (button) | Restores the 8 terms, combiner, proximity and fold, then switches to Search Setting | 19222, handler 19290 → `loadSearch()` 20486–20499 | NONE | Restores **no scope** (see Save search above). Silently no-ops when nothing is selected (20488). Does not run the search — the user must press Find again, which is defensible but undisclosed. |
| **Delete** (button) | `QSettings::remove("gofer/saved/<name>")` and refresh | 19223, handler 19293–19299 | NONE | **No confirmation dialog and no undo** — one click permanently destroys a saved search. Irreversible action, zero coverage. |

### 2e. Search Results tab (constructed 19231–19247)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Results view** (`QTextBrowser`, read-only) | Renders the whole result document; `setOpenLinks(false)` so anchors are routed to the pane's own handler instead of Qt's | 19232–19233 | selftest reads it at 19816, 19824, 19877, 19880, 19885, 19894, 19897 | HTML is built by string concatenation into one `QString h` and set in a single `setHtml` at 20468 — a very large result set is fully materialised in memory before anything renders. No progressive display, no "searching…" progress beyond the one-shot placeholder at 20059. |
| **Replace in these folders…** (button) | Collects the ticked user-folder rows and hands `(term, folders)` to the Replace-in-Files window via `g_replaceInFolders` | 19237, handler 19239–19243; `g_replaceInFolders` installed at 47162 | NONE at this button. (`replf::selfTest`, run at 49123, drives `seedFolders()` **directly** — it never touches this button or its gate.) | Sends **only `fields_[0]`** (19242), which is consistent with the gate, but the button lives on the Results tab where the user has just seen hits from the corpus and apparatus too; those sources cannot be replaced and the refusal message (19241) is the only place that is said. This is the pane's one route into a bulk **write** operation and it has no pane-level test. |
| **Replace gate** (enable/disable + tooltip swap) | Enables the button only for one plain term (field 1 set, fields 2–8 empty, combiner ≠ NEAR); tooltip explains the refusal | `replaceGateOpen()` 19929–19933, `updateReplaceGate()` 19934–19939, wired 19244–19246 | NONE | The gate lets **OR mode with a single term** through (it only blocks `currentIndex()==2`), so a query the user believes is an OR query can be replaced — harmless with one term, but the rule is "no NEAR" rather than the stated "one plain term". The disabled tooltip is the only explanation, and a disabled button's tooltip is not reliably shown on all platforms. |

### 2f. Result-link schemes (`results_` `anchorClicked`, 19266–19288)

The handler is a **capture-less lambda** — it holds no `this`, so it cannot
report anything back into the pane.

| Scheme | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| `goferopen:<pct-encoded path>\|<line>` | Splits at the **last** `\|`, decodes the path and calls `g_openAtLine(path, line)` — opens the file in the Overlay at that hit | emitted 20384–20396; handled 19269–19279 | selftest 19897–19899 "a result link carries the file's real path — the anchor payload survives rendering" | The comment at 20371–20383 records the TEST-5 defect this pins: chained `.arg()` re-scans its own output and `anchorEnc()` writes `%2F`, so `.arg(r.firstLine)` was eaten by the `%2` inside the encoded path and **every file row this pane ever drew was mangled** with no assertion able to see it. Any future edit that re-chains `.arg()` here reintroduces it. Silently no-ops when `g_openAtLine` is null. |
| `lookup:<pct-encoded term>` | Sends the term to the Lookup pane via `g_lookupQuery` | handled 19280–19285; emitted 20430–20432 | NONE | **Dead code.** The emitting loop iterates `lastTerms_` (20424), which is declared at 20520 and read at 20424 and **assigned nowhere in the file** (verified: the only two occurrences are the declaration and the read). The "look up:" row can never render, so this scheme is unreachable from the UI. |
| `file://…` (Spotlight rows) | `QDesktopServices::openUrl` — opens the document in its default application | emitted 20226–20230; handled 19286–19287 | NONE | Launches an arbitrary local file in whatever app the OS associates with it, straight from a search result, with no confirmation and no path display beyond the row's own label. The gate is only `u.isLocalFile()`. |
| *(any other scheme)* | Nothing — the handler falls off the end | 19286–19288 | NONE | A future `http`/`term:`/`propose:` anchor added to this result HTML would silently do nothing with no error. Worth knowing before anyone extends the renderer. |

### 2g. Entry points from outside the pane

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Search" ribbon tab** (Research group) | Raises the pane | `tabs.addTab(goferPane, "Search")` 45453; `mkGroup("Research", …)` 46059–46060 | NONE at pane level. `gauntlet_walk` (`app/CMakeLists.txt:108`) is a **random** walk whose seed is now `auto` = the Julian day (52384–52386), so it is a different walk every day and cannot be cited as coverage for any named control. | The pane can also be hidden entirely by the Features mechanism (`releaseHidden`, 3473) — confirm whether "Search" is ever in `g_releaseHidden` at release. |
| **File ▸ Find in Files… (⇧⌘F)** | Takes the editor selection (or the Find dialog's needle), raises this pane, seeds term 1 and searches | 47137–47143 → `searchFor()` 19095–19100 | NONE | `searchFor()` sets **only** `fields_[0]` and does not clear fields 2–8 or reset the combiner (19095–19100), so a leftover second term or a stale NEAR setting silently narrows a Find-in-Files the user believes is a plain search. This is the difference from `runQuery()`, which does clear and reset. |
| **File ▸ Find Results** | Raises the pane (no query) | 47144 | NONE | Lands on whatever sub-tab is current, not necessarily Search Results. |
| **Goto ▸ Goto Concordance (⌃⌥⇧↓)** | Word under the caret → raises this pane → `searchFor(word)` | 47634–47639 (menu created 47618) | NONE | Same non-clearing behaviour as ⇧⌘F. Reports "Put the caret on a word first." to the status bar when there is no word (47636). |
| **Hunt palette (⌘K) action kind 1** | `g_goferQuery(payload)` → `runQuery()` → raises the pane | 44595–44597 → 45449–45452 → `runQuery()` 19304–19310 | NONE | `runQuery()` clears all eight fields and forces combiner index 0 before searching (19305–19308) — correct, and the opposite of `searchFor()`. Two public entry points with different reset semantics is a drift risk. |
| **Overlay citation `citefind:`** | A citation anchor in the Overlay sends up to 8 words as one quoted phrase to this pane | 15241–15247 → `g_goferQuery` | NONE at the arrival end. The Overlay's citation battery covers the Overlay side only. | Builds the query by hand (`"\"" + first 8 words + "\""`) rather than through `buildQuery()`, so it bypasses the combiner and the fold entirely. |
| **`searchFor(const QString&)`** (public API) | Seed term 1 and search | 19095–19100 | NONE | Two callers (47142, 47638). |
| **`runQuery(const QString&)`** (public API) | Clear, seed term 1, force OR, search | 19304–19310 | NONE | One caller, via `g_goferQuery` (45449). |
| **`GoferPane::goferFoldKeep(int, QString, QStringList)`** (public static) | The fold predicate, exposed so other code and tests can use it | 20017–20034 | app selftest 49222–49229 ("Gofer fold: strict modes enforce case/space, default folds") | Public and `static`, but the only external caller is the test; it is effectively a test seam. |
| **Session restore on launch** | The eight terms, combiner, proximity and fold come back from the last session | 19166–19171 | NONE | A restored `NEAR/12` or a restored term in field 5 is invisible unless the user scrolls to it, and will silently shape the next Find-in-Files (see ⇧⌘F above). |

### 2h. Result-page disclosures (rendered by `find()`, 20038–20469 — not clickable, but the pane's honesty surface)

| Output | What it says | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| "enter at least one term" | Empty-query refusal; jumps to the Results tab | 20053–20057 | NONE | |
| "searching…" | One-shot placeholder before the work starts | 20059–20061 | NONE | The only progress feedback for the whole run; a multi-minute library sweep shows this and nothing else. |
| `query: <code>…</code>` | Echoes the exact string handed to the engine | 20062–20064 | NONE | Good disclosure (the user can see `NEAR/3` when they typed `0`), but it is small grey text at the top and is the *only* place the proximity fallback surfaces. |
| **Corpus source line** | `<corpus row name> — ` + `corpusCountLine(rendered=20, fetched=60, realTotal)` | 20086–20089; helper `app/textreport.h:84` | selftest 19779–19793: all three numbers printed (DATA-1); an unmeasurable total says **unknown**; "60 hit(s)" must not stand in for an unknown total; and the control case that a fitting result does not manufacture a cap notice | The strongest-pinned line in the pane. The comment at 20074–20083 records the finding: 1,191 real hits printed as "60 hit(s)" under the binding layer's own name. |
| Corpus error line | `corpus: <exception message>` in error red | 20109–20112 | NONE | The raw `what()` of an SQLite/FTS5 exception is shown to the user verbatim — FTS5 rejects this pane's own `NEAR/N` grammar at `step()` in some shapes (noted at 19776–19778), so this is a *reachable* path, not a theoretical one. |
| Apparatus source line | `<apparatus row name> — N hit(s)` | 20154–20156 | NONE | `N` is the true match count, but rendering silently stops at 20 notes / 40 cumulative entries (20134, 20146). An undisclosed render cap under a truthful count — the same defect class DATA-1 fixed for the corpus row. |
| Spotlight source line | `<Spotlight row name> — N document(s); searched as: <each probe>` | 20217–20223 | NONE | Naming every probe form is genuinely good (the user sees what was actually asked). But `N` is capped at 40 (20200, 20213) and the 6 s deadline (20190) can truncate the fan-out, and **neither cap is said** — a partial answer presented as a count. |
| Per-folder header (at cap) | "first **N** hit(s) found, in M file(s) so far (display limit — not the total)" | 20350–20366 | NONE | Correct honesty wording, entirely untested. |
| Per-folder header (normal) | "N hit(s) in M file(s)" | 20360–20366 | NONE | |
| "…and N more file(s) — narrow the terms to see them" | Discloses the 30-row-per-directory rollup cap | 20397–20402 | NONE | |
| **Total line** | "**N hit(s) found**" (+ "— a display limit was reached; this is not the corpus total" when capped) | 20413–20420 | selftest 19816 (contains "hit") and 19824 (DATA-5: never "hit(s) shown") | The banned wording is pinned so it cannot creep back through a copy edit; the *presence* of the display-limit clause is not pinned. |
| **"look up: …" row** | Should render each searched term as a `lookup:` link to the Lookup pane | 20421–20436 | NONE | **Never renders** — `lastTerms_` is never assigned (see 2f). Dead advertised feature: the code, the comment ("W3-01: the searched terms link straight to their cards") and the anchor handler all exist; the wiring does not. |
| No-match widening hint | "nothing matched… switch the combiner to OR; raise the NEAR window; check which folders are searched. ACIP is UPPERCASE, wylie lowercase — case picks the reading…" | 20437–20445 | NONE | Only shown when `total == 0 && !stopped_`. This is the **only** place the ACIP/wylie case rule is stated, and it appears exactly when the user has already failed. |
| Fold-hidden notice | "N window(s) hidden by the fold setting (<combo text>)" | 20446–20451 | selftest 19884–19887 "the fold discloses how many windows it hid" | |
| Skip notice | `goferSkipNotice(skipped, scanned)` — names both the unread count and what was read, says plainly the sweep was partial, silent when nothing was skipped | 20452; helper in `app/textreport.h` | selftest 19801–19808 (three PERF-1 checks) | |
| Index-cap notice | "This term is common enough that the index scan reached its `kScanCap`-window ceiling, so these hits are drawn from part of the library rather than all of it…" | 20453–20463 | NONE | Honesty-critical and untested; it is the only signal distinguishing "no more matches" from "we stopped looking". |
| "search stopped — partial results" | Shown when Stop was pressed | 20464–20466 | NONE | Does not say *which* phases completed — the corpus and apparatus always finished, folders may not have. |

---

**Counts: 55 rows; 40 with NONE.**

---

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `searchFor(const QString&)` | 19095 | Seed term 1 and search (Find in Files / Goto Concordance) | 47142, 47638 |
| `GoferPane(Spine&, root)` (ctor) | 19101 | Builds the three sub-tabs, all wiring, session and scope persistence | `main()` 45448 |
| `runQuery(const QString&)` | 19304 | Clear all fields, seed term 1, force OR, search | `g_goferQuery` 45449 (Hunt palette 44597, Overlay `citefind:` 15244) |
| `loadCitationWeb()` | 19314 | Lazy-load `data/extracted/citation_web.json` edges; returns edge count | `showCitationWeb`, selfTest 19400 |
| `showCitationWeb()` | 19323 | Build and show the modeless citation-web dialog | Citation web… button |
| `selfTest(QStringList&)` | 19385 | 51 checks (≈17 pane-specific, ≈34 cross-cutting house-rule pins) | `main()` 49132 (`app_selftest`) |
| `kCorpusRow` / `kApparatusRow` / `kSpotlightRow` | 19921/19923/19925 | The three fixed scope-row labels | `loadDirs`, `find`, selfTest |
| `replaceGateOpen()` | 19929 | One-plain-term, no-NEAR precondition for Replace | `updateReplaceGate` |
| `updateReplaceGate()` | 19934 | Enable/disable the Replace button and swap its tooltip | ctor 19244–19246 |
| `replaceButton()` | 19940 | Accessor for the Replace button | **no caller found** — dead accessor |
| `addDirRow(text, checked)` | 19941 | Append a user-checkable scope row | ctor, `loadDirs`, Add/Duplicate, selfTest 19868 |
| `kFirstDirRow` (= 3) | 19949 | Boundary between fixed sources and user folders | Remove, Duplicate, `saveDirs`, `find`, Replace |
| `loadDirs()` | 19951 | Seed the three fixed rows from QSettings + saved user folders (default `<root>/library`) | ctor 19174 |
| `saveDirs()` | 19974 | Persist the three source ticks + the user folder list and its ticks | Add/Remove/Duplicate handlers, **and `find()` 20058** |
| `buildQuery()` | 19992 | Quoted terms joined by `OR` / `NEAR/1000000` / `NEAR/N` | `find` 20052 |
| `goferFoldKeep(mode, window, terms)` (public static) | 20017 | Pure strict post-filter over a hit window | `find` 20324, selfTest, app selftest 49222 |
| `find()` | 20038 | The entire search: guard, query, corpus, apparatus, Spotlight, folders, fold, rollup, render, disclosures | Find button, Enter, `runQuery`, `searchFor` |
| `saveSearch()` | 20471 | Write `gofer/saved/<name>` (terms, combiner, proximity, fold — **not** scope) | Save search button |
| `loadSearch()` | 20486 | Restore a saved search into the controls and return to Search Setting | Load button, list double-click |
| `refreshSaved()` | 20500 | Repopulate the saved list, with a disabled empty-state hint | ctor, `saveSearch`, Delete |

Members of note: `lastTerms_` (20520) — declared, read at 20424, **never
assigned**; `citEdges_` (20527) — cached forever once loaded, so a rebuilt
`citation_web.json` is not picked up without a restart; `stopped_` (20529) and
`finding_` (20531) — the Stop flag and the PERF-4 re-entrancy guard.

---

## 4. Dependencies

**Core / engine**
- `allcore::Spine` (SQLite FTS5) via `allcore::goferSearch` (20068) and
  `spine_.corpusCount` (20077).
- `allcore::LibraryIndex` (20267) with `SearchStats` and its `kScanCap`
  (20463); `allcore::goferSearchFiles` + `allcore::GoferScan` (20289–20299).
- `allcore::wylieToUnicode` and `allcore::acipToEwts` — the Spotlight probe
  conversions (20164–20171). A failed conversion simply adds no probe (no
  silent roman fallback), which is rule-3 clean.

**Data / files read**
- `<root>/data/extracted/citation_web.json` — citation-web edges; absence is
  reported honestly in a message box (19325–19331).
- `<root>/library` — the default user-folder scope row on a fresh install
  (19969).
- `<dir>/.index.db` per scope folder — chooses the indexed branch (20265).
- Globals `g_appNotes` / `g_appBib` — the published apparatus, searched in
  memory (20113).
- selfTest fixtures under `QDir::temp()`: `all_selftest_restore`,
  `all_selftest_no_such_dir_xyzzy`, `all_selftest_tp5_short.txt`,
  `all_selftest_banner_root`, plus a `QTemporaryDir` with `upper.txt` /
  `lower.txt` for the fold drill.

**Processes / network**
- `/usr/bin/mdfind` — one `QProcess` per probe term, all started concurrently
  under one 6-second deadline, killed at the end (20183–20215). macOS-only,
  hard-coded path.
- **No network host is contacted by this pane.** `TimedNam` appears only inside
  `selfTest` (19653) as a FAIL-4 timeout probe; no request is issued.

**Cross-pane globals**
- Reads: `g_openAtLine` (19273), `g_lookupQuery` (19281), `g_appNotes`,
  `g_appBib`, `g_replaceInFolders` (19242).
- Written by `main()`: `g_goferQuery` (45449).

**QSettings (`ALL` / `TranslationTool`)**
- Scope: `gofer/corpusChecked`, `gofer/apparatusChecked`,
  `gofer/spotlightChecked`, `gofer/dirs`, `gofer/dirsChecked` (read 19952–19970,
  written 19977–19990, and written again on every Find via 20058).
- Saved searches: `gofer/saved/<name>/{f0..f7, combiner, proximity, fold}`
  (20475–20481), enumerated by `refreshSaved()` (20504), removed by Delete
  (19297).
- Live session: `gofer/f0..f7`, `gofer/combiner`, `gofer/proximity`,
  `gofer/fold` via `sess::remember` (19166–19171) — written on every keystroke.
- The selfTest saves and restores the five scope keys around its fold drill
  (19865–19918), so the battery does not leave the user's scope mutated.

**ctest suites**
- `app_selftest` (`app/CMakeLists.txt:105`) — the 51 checks, via
  `DiamondCutterTranslationTool --selftest`, called at main.cpp:49132.
- `gauntlet_walk` (`app/CMakeLists.txt:108`) — random UI walk, seed `auto` =
  Julian day (52384), so it certifies nothing specific about this pane.
- `gofer_smoke` (`core/CMakeLists.txt:147`) — the engine behind the pane,
  against `build/hgm_spine_v27_2.db` + `core/tests/fixtures/acipdocs`.
- `gofer_bound_smoke` (`core/CMakeLists.txt:508`) — the unindexed search's NEAR
  product and file walk are bounded and say so.
- `replf::selfTest` (main.cpp:49123) covers the Replace-in-Files **window**, not
  this pane's Replace button or its gate.

---

## 5. Release questions

- **The "look up:" row is dead.** `lastTerms_` (20520) is read at 20424 and
  assigned nowhere, so the row at 20421–20436 can never render and the
  `lookup:` anchor handler (19280–19285) is unreachable. Either wire it in
  `find()` or delete the block, the member and the handler — shipping code that
  advertises a cross-pane jump the user can never see is worse than not
  offering it.
- **"Save search" overclaims.** Its tooltip (19188–19190) promises "terms,
  operator, **scope**"; `saveSearch()` (20475–20481) stores no scope at all. A
  saved search silently runs against whatever folders are ticked at load time.
  Fix the code or fix the sentence.
- **"Duplicate" has no purpose.** Its tooltip (19184–19186) says "to search the
  same folder with a different setting", but rows carry only a path and a tick —
  there is no per-row setting. A duplicate row searches the same folder twice
  and double-counts into the total.
- **The combiner is not global, but reads as global.** Spotlight ignores it
  entirely (20157–20231); the apparatus branch reduces it to any/all substring
  matching (20117). Same for the fold combo, which applies **only** to folder
  hits (20313). Decide whether to scope these controls visually or to disclose
  the exception.
- **Two undisclosed caps, both the DATA-1 shape the pane already fixed once.**
  Apparatus rendering stops at 20 notes / 40 cumulative entries (20134, 20146)
  under a truthful count; Spotlight is capped at 40 documents and a 6-second
  deadline (20190, 20200) and prints a bare "N document(s)" (20217–20220). The
  corpus line was fixed for exactly this; these two were not.
- **Spotlight interpolates the raw term into an `mdfind` query string**
  (20186–20190) with no escaping of `"` or `\`, and starts up to 24 concurrent
  processes. Review before release; it is also the pane's only macOS-only code
  path, which matters for the promised Windows/Linux input-center builds.
- **`file://` result rows open arbitrary local files in their default
  application** on a single click (19286–19287), with no confirmation.
- **"Delete" on a saved search is irreversible, unconfirmed and untested**
  (19293–19299); "Save search" silently overwrites an existing name, and a name
  containing `/` writes a nested QSettings group that `refreshSaved()` will not
  list back.
- **The Replace lane has no pane-level coverage.** The button (19237) and the
  gate (19929) are this pane's only route into a bulk file **write**, and
  `replf::selfTest` reaches the window through `seedFolders()` directly, never
  through this button. The gate also admits OR-with-one-term despite its "one
  plain term" wording.
- **Proximity is free text.** `0` (the shipped default) becomes `NEAR/3`, and
  `abc` becomes `NEAR/3`, both silently; the tooltip says 0 means the same
  line. Either make it a `QSpinBox` with the real minimum or say what was used.
- **Live search terms are persisted to QSettings on every keystroke**
  (19166–19171). The same selfTest pins that a diagnostic report must leak no
  file title because "a text's title can be the sensitive part" (19763–19766) —
  the settings file holds the search terms in clear.
- **Stop cannot interrupt the corpus or apparatus phases** (only 20198, 20249,
  20269, 20294 honour it), and the "search stopped" notice (20464) does not say
  which phases had already completed.
- **`NEAR` and `AND (same file)` — the Gofer grammar, the pane's headline
  feature — have no assertion at pane level.** Every drill sets combiner index
  0. The grammar rests entirely on `gofer_smoke` at the engine layer.
- **`gauntlet_walk` can no longer be cited as coverage for anything.** Its seed
  is `auto` = the Julian day (52384–52386), so the walk differs every day. Any
  audit row that leans on it is leaning on chance.
- **`replaceButton()` (19940) has no caller** — dead accessor, decide whether it
  was meant to be a test seam.
- **`citEdges_` is cached for the life of the process** (19315), so a
  regenerated `citation_web.json` is invisible until restart, and the dialog is
  modeless with `WA_DeleteOnClose` and no re-binding.

---

*Carried across from the 2026-09-09 version of this file when the inventory was regenerated on 2026-09-16. The regeneration rewrote the machine inventory above (the old one cited the class at main.cpp:16022-17313; it is now 19091-20532), but this section was a later hand addition and is not something a re-read of the code would reproduce.*

## F3/F4 (2026-09-09)
- Search Results tab: Replace in these folders… (F4) — enabled only for one plain term (fields 2–8 empty, no NEAR), seeding the needle and the ticked user folders. Evidence: app selftests F3-17…23, F4-1…18c; `Normalize.md`, `ReplaceInFiles.md`.
