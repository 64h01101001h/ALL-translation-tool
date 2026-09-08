# Release audit — `HuntPalette`

Source: `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp`, class body lines
**34579–35197** (the global `static HuntPalette* g_hunt = nullptr;` follows at 35198; the code from
35200 onward is `translatorSurveyMarkdown`, a separate free function, and is **not** part of this
class).

## 1. Purpose and reach

`HuntPalette` is the app's ⌘K "hunt everywhere" command bar: a 760×460 non-modal `QDialog`
containing exactly two widgets — a `QLineEdit` and a `QListWidget` — plus a 220 ms single-shot
`QTimer` debounce. One typed query is fanned out across **seven** result lanes in a fixed order
(dictionary → English reverse index → pronunciation fold → aligned corpus → teaching index →
apparatus → persons), followed by an eighth **file-target** lane over the Library folder; each
result row carries an `Act{kind, payload}` that, on Enter or double-click, dispatches to the Lookup
pane, the Gofer search pane, the Apparatus pane, the Overlay/Input pane, or the system browser.

A single instance is created eagerly at startup — `g_hunt = new HuntPalette(spine, root, &win);`
(**35796**), parented to the main window — and reached two ways:
- **⌘K** — `View menu → "Hunt Everywhere…"` with `QKeySequence("Ctrl+K")` (**37238–37244**).
- **The "Hunt (⌘K)" corner button** — a flat `QPushButton` with a search icon installed as the
  main `QTabWidget`'s `Qt::TopRightCorner` widget (**36585–36601**), tooltipped "Hunt Everywhere —
  files, dictionary entries, corpus lines, persons, apparatus, teachings, commands. Same as
  pressing ⌘K."

Both call `openPalette()`, which clears the box, the list, and `acts_`, then shows/raises/focuses.
Note the tooltip promises **"commands"**, which no lane in `runSearch()` actually provides.

## 2. User-visible functions

### Entry points and window controls

| Control/action | What it does (from code) | Code anchor (function + line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **⌘K / View → "Hunt Everywhere…"** | `if (g_hunt) g_hunt->openPalette();` | `main()` lambda 37241–37244 | NONE (selfTest drives `box_`/`runSearch()` directly, never the menu action) | Guarded by a null check on a global; if `g_hunt` construction ever moves after the menu build, ⌘K becomes a silent no-op. |
| **"Hunt (⌘K)" corner button** (top-right of the tab bar) | Same `g_hunt->openPalette()` | `main()` lambda 36597–36600 | NONE | Tooltip advertises "commands" — the palette has no command lane. Misleading affordance. |
| **`openPalette()` reset behaviour** | Clears box, list, and `acts_`, then `show/raise/activateWindow/setFocus` | `openPalette` 34627 | NONE | Deliberately **discards the previous query** on every open — no "resume last hunt". Confirm intended. |
| **Escape key (in the box)** | `hide()`, consumed by the event filter | `eventFilter` 34808–34811 | NONE | `hide()`, not `reject()` — the palette keeps its state object alive; harmless since `openPalette` clears. |
| **Down arrow / Up arrow (in the box)** | Moves `list_` current row ±1, `qBound`ed to the list, and **consumes the key** (`return true`) so the caret never moves | `eventFilter` 34798–34807 | Indirectly by BOUNTY #12 check (c) which calls `setCurrentRow(alt)` — but the **key handling itself is not exercised** | Consumes Up/Down unconditionally while the box has focus, so text-caret navigation in the box is impossible (acceptable for a palette). No Home/End/PageUp/PageDown handling. No wrap-around. |
| **Typing in the box** | `textChanged` → `timer_->start()` (220 ms single-shot) → `runSearch()` | ctor 34614–34618; `runSearch` 34838 | Every `check(...)` in `selfTest` reaches `runSearch()`, most by calling it directly | Debounce is **restarted on every keystroke**, so a continuous typist never sees results until they pause. |
| **Queries shorter than 2 characters** | `runSearch` clears the list and returns early (`if (q.size() < 2) return;`) | `runSearch` 34845 | NONE | Silent: no "keep typing" hint; a one-character query looks like "no results". |
| **Enter / Return (in the box)** | `submitCurrent()` — if the debounce is still pending, runs the search first, then `activate(list_->currentRow())` | ctor 34621–34622; `submitCurrent` 35151 | **BOUNTY #12 block, three checks** (34739–34796): `"Enter on a first-ever query runs the search and acts on it, instead of being swallowed"`; `"Enter answers the query in the box, not the one the debounce still had on screen"`; `"Enter on settled results honours the row the arrow keys chose, not row 0"` (this third may log `[SKIP] Hunt: arrowed row survives Enter — this spine offers no second lookup row for "sunam"`) | The best-covered path in the class — the fix is pinned at the `returnPressed` **signal**, not just the handler. The third check is **conditionally skipped** and depends on spine data; it can silently stop testing anything. |
| **Double-click / activate a result row** | `itemActivated` → `activate(list_->row(it))` | ctor 34623–34626; `activate` 35156 | NONE for the mouse path (selfTest only ever reaches `activate` through `returnPressed`) | Double-click and Enter go through different entry points — `itemActivated` **bypasses** `submitCurrent`'s debounce settle. Clicking a row rendered from a stale query acts on the stale payload. |
| **Single-click a row** | Selects only; `runSearch` also force-selects row 0 after every search (`list_->setCurrentRow(0)`) | `runSearch` 35148 | NONE | An arrow-key or mouse selection is overwritten by any late `runSearch` — the reason `submitCurrent` guards on `timer_->isActive()`. |

### Typed-query grammar (what the box accepts as user-visible syntax)

There is no operator grammar — no quoting, no `AND`/`OR`, no field prefixes. The query is
interpreted by **case** and then broadcast to every lane. These are the user-visible input modes:

| Query form | What it does (from code) | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **All-uppercase input → treated as ACIP** | `hasLower` is false → `allcore::acipToEwts(raw)` converts to wylie before the dictionary/corpus lanes | `runSearch` 34848–34855 | NONE (no selfTest check uses an uppercase ACIP query; `"TD04156"` is uppercase but is asserted only for the **file** lane) | The **case of the query silently changes its meaning.** A user typing an English word in caps ("MERIT") gets it run through the ACIP converter. No indicator of which interpretation was chosen. `acipToEwts` is called unguarded — no try/catch on this line. |
| **Any lowercase present → treated as wylie/English as typed** | `q.toLower()` used for wylie, reverse index, and file matching | `runSearch` 34848–34856 | `"bsod nams"`, `"kawa"`, `"pillar"`, `"sunam"`, `"gzhon nu dpal"`, `"byang chub"` queries in `selfTest` | Mixed-case input is treated as wylie — a proper-noun English query behaves as lowercase. |
| **Phonetic (GMR-convention) spelling** e.g. `sunam`, `kawa` | Lane 3 calls `spine_.lookupByPronunciation(raw, 3)` on the **raw** (un-lowercased) string | `runSearch` 34953–34980 | `check(pron, "phonetics query reaches the fold (sunam)")` (34650–34655) plus the whole `kawa` tier block | Uses `raw`, not the case-normalised form — an uppercase phonetic query hits both the ACIP branch and this lane. |
| **English word/phrase** e.g. `pillar` | Lane 2 `spine_.reverseIndex(q.toLower())` — exact phrase in the reverse index | `runSearch` 34918–34946 | `check(!engRow.isEmpty(), "the English lane answers 'pillar' with ka ba")` (34712–34714) | Exact-phrase only; near-misses return nothing with no "did you mean". |
| **Filename fragment** e.g. `TD04156` | File lane matches `QFileInfo(p).fileName().toLower().contains(ql)` over the indexed Library walk | `runSearch` 35088–35128 | `check(file, "filename lane answers (TD04156 → Library file target)")` (34719–34726) | Substring on the **filename only**, not the path — a folder name is not searchable. |
| **Person name / Tibetan name** e.g. `gzhon nu dpal` | Persons lane uppercases the query and substring-matches the key, the `eng` field, and every `candidates[].names[]` entry | `runSearch` 34995–35085 | `check(person, "persons lane answers (Gö Lotsawa reaches the authors layer)")` (34727–34734) | Uppercase substring matching over a JSON blob — no diacritic folding, no transliteration normalisation. |
| **Apparatus term (footnote lemma / bibliography id or text)** | Counts matching footnotes and bibliography entries and emits **one summary row** | `runSearch` 35001–35047 (lane 6) | NONE | Lane 6 has **zero coverage.** It also emits a row whose payload is the raw query and whose kind is `4`. |

### Result-row lanes (each row is itself a user-visible action)

| Row type | What it does when activated | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **📖 dictionary row** — `wylie — gloss [tier]` or `[PROVISIONAL]`; up to 4 rows, exact `spine_.lookup(wylie)` first, else FTS `spine_.headwordSearch("\"wylie\"", 4)` | `kind 0` → `hide()` then `g_lookupQuery(wylie)` — jumps to the Lookup pane with the query run | `addEntry` lambda 34861–34880; fallback 34882–34890; `activate` 35163–35166 | `check(list_->count() > 0 && !acts_.empty(), "wylie query answers")` (34641–34644) and `check(dict, "dictionary row present for a known headword")` (34645–34650) | Shows `"(no HGM gloss — reference layers in Lookup)"` when `hgm_gloss` is empty — correct honesty behaviour. The FTS fallback is wrapped in a **bare `catch (const std::exception&) {}` that swallows the error silently** (34888–34890). |
| **🔁 English-reverse row** — `english "q" → wylie [tier]`; up to 3 | `kind 0` → Lookup pane | lane 2, 34918–34946 | `check(engRow.contains("[PROVISIONAL]") && !engRow.contains("auto-aligned"), "the English lane wears the reader's label, never the storage token (DATA-9)")` (34715–34718) | DATA-9 fix: `auto-aligned` is normalised to `PROVISIONAL` here (34934–34938). Whole lane wrapped in a **silent `catch`** (34944–34946). |
| **🗣 pronunciation row** — `said "q" → wylie — gloss [tier]`; up to 3 | `kind 0` → Lookup pane | lane 3, 34953–34980 | Three checks (34693–34710): `"Enter on a phonetic query fills the pronunciation lane with an auto-aligned AND a glossary row"`, `"pronunciation lane tags its auto-aligned gloss — for a phonetic query it is often the only row on screen"`, `"pronunciation lane names the true tier of a non-provisional gloss"` | BOUNTY #2 site: previously printed a bare untagged gloss. The tier suffix is emitted **only when `hgm_gloss` is non-empty** (34966–34979) — a gloss-less provisional row still shows no marker. **Not** wrapped in try/catch, unlike its neighbours. |
| **📜 corpus row** — `[COURSE:seq] english…` truncated to 90 chars; `corpusSearch("\"wylie\"", "", 3)` capped at **2** shown | `kind 1` → `hide()` then `g_goferQuery(wylie)` — jumps to the Gofer search pane | lane 4, 34982–34999; `activate` 35180–35182 | NONE | Fetches 3, shows 2 — **no "…and N more" disclosure** on this lane, unlike the persons and files lanes which explicitly say what they cut. Inconsistent with the house honesty rule. Silent `catch` (34997–34999). |
| **🎧 teaching row** — "Geshe Michael says this word — <title truncated to 70>" | `kind 2` → `hide()` then `QDesktopServices::openUrl(url)` — **opens the system browser** | lane 5, 35001–35012; `activate` 35183–35186 | NONE | **Network / external browser launch, zero coverage.** URL comes from `g_teachingTib` (the teaching index data file). Shows only `it->second.front()` — the first match; no disclosure that other teachings mention the word. |
| **📎 apparatus row** — "N footnote(s), M bibliography entr(y/ies) — open in Apparatus (first: lemma)" | `kind 4` → `g_apparatusQuery(payload)` then **`accept()`** | lane 6, 35014–35049; `activate` 35177–35180 | NONE | Only row that calls `accept()` instead of `hide()` — an inconsistency in dismissal semantics. Note the odd inner loop at 35026–35031 (`for (…) if (hit) break; else if (…) hit = true;`) — reads as a mis-shaped anchor scan; verify it does what was intended. |
| **👤 person row (BDRC)** — `Name (dates) — N work(s) in the catalog · opens BDRC`; up to 4 people | `kind 2` → opens `https://library.bdrc.io/show/bdr:<pid>` in the system browser | lane 7, 35054–35075 | `check(person, "persons lane answers …")` only — asserts the 👤 glyph is present, **not the URL, not the opening** | **Hard-coded external host `library.bdrc.io`.** Label/destination were previously mismatched (fixed: the biography now gets its own row). Skips any candidate with an empty URL via `if (url.isEmpty()) continue;` — silently. |
| **👤 person row (Treasury of Lives biography)** — `Name — biography · opens Treasury of Lives` | `kind 2` → opens the Treasury of Lives URL from `tolBiographyUrl(...)` or `tolLinkForPid(pid, root_)` | lane 7, 35061–35082 | NONE | Second hard-coded external destination. Emitted only when a URL can be built. |
| **"… and N more person(s) match — the Library's "By author" search ranks them properly"** | `kind -1` → `activate` returns early; a disclosure line, not a target | 35077–35085; guard `activate` 35159–35161 | NONE | Good honesty behaviour (total counted before the cap is applied, 35029–35033). Not selectable-into-action but **is** selectable — Enter on it does nothing with no feedback. |
| **📁 file row** — `<filename>   — open this file`; up to **6** shown | `kind 3` → `hide()`, then by extension: `png/jpg/tif` → `g_openScanInInput(path)`, otherwise `g_openAtLine(path, 1)` | lane 8, 35088–35128; `activate` 35167–35177 | `check(file, "filename lane answers …")` — asserts the glyph only, **not the activation, not the extension routing** | Indexes `<root>/library` recursively for `*.txt *.act *.inc *.TXT *.ACT *.INC *.png *.jpg *.tif` via `QDirIterator` on **first query** — an unbounded synchronous disk walk on the UI thread. `filesIndexed_` is set only if the walk found something (deliberate: a late-mounting library must not kill the lane), so **an empty library folder re-walks the disk on every keystroke-batch**. |
| **"… and N more file(s) match — narrow the search, or use the Library's list view"** | `kind -1` disclosure row | 35124–35128 | NONE | Same honesty pattern as persons. |
| **"— nothing exact; try the Lookup pane's fuzzier paths"** | `kind -1` empty-state row when no lane produced anything | 35143–35147 | NONE | Enter on it does nothing silently. |

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `HuntPalette::HuntPalette(allcore::Spine&, const QString& root, QWidget*)` | 34581 | Builds the two-widget UI and stylesheet, creates the 220 ms single-shot debounce timer, wires `textChanged`→timer, timer→`runSearch`, `returnPressed`→`submitCurrent`, `itemActivated`→`activate`, installs the event filter on the box | `main()` 35796 |
| `openPalette()` | 34627 | Clears box/list/`acts_`, `show`, `raise`, `activateWindow`, focuses the box | ⌘K action 37242; corner button 36598 |
| `selfTest(QStringList& log)` | 34634 | The palette's whole automated coverage: wylie query, dictionary row, phonetics fold, the `kawa` tier block (lane 3), the `pillar` DATA-9 block (lane 2), file lane, persons lane, and the three-part BOUNTY #12 Enter/debounce block; restores `g_lookupQuery` around each borrowed section and clears state at the end | `main()` `--selftest`, **38426** (`if (g_hunt) fails += g_hunt->selfTest(log);`) |
| `check` lambda in `selfTest` | 34636–34641 | Logs `[PASS]/[FAIL] Hunt: <what>` and counts failures | `selfTest` |
| `pronRow` lambda in `selfTest` | 34670–34681 | Finds a 🗣 row containing a given wylie + gloss | the `kawa` block |
| `eventFilter(QObject*, QEvent*) override` | 34797 | Intercepts Down/Up (move list selection, consume) and Escape (hide) on `box_` | Qt, via `box_->installEventFilter(this)` (34628) |
| `struct Act { int kind; QString payload; }` | 34817 | Row → action mapping. `kind`: `-1` disclosure/no-op, `0` Lookup, `1` Gofer search, `2` open URL, `3` open file, `4` Apparatus | `acts_`, `add`, `activate` |
| `add(const QString& text, int kind, const QString& payload)` | 34821 | Appends a list item and its parallel `Act` | every lane in `runSearch` |
| `runSearch()` | 34826 | Stops the debounce, clears list/`acts_`, normalises the query by case (ACIP vs wylie), then runs all eight lanes in order and force-selects row 0 | timer timeout 34619; `submitCurrent` 35152; `selfTest` directly (several sites) |
| `addEntry` lambda in `runSearch` | 34861–34880 | Renders one 📖 dictionary row with gloss and tier/PROVISIONAL suffix | lane 1, both the exact and FTS paths |
| `matches` lambda in `runSearch` (persons) | 35010–35022 | Uppercase substring test over key, `eng`, and every candidate name — lifted out so the **total** can be counted before the cap | lane 7, both the counting pass and the display pass |
| `submitCurrent()` | 35151 | BOUNTY #12: `if (timer_->isActive()) runSearch();` then `activate(list_->currentRow())` — settles a pending debounce *only* while pending, so settled results keep the arrow-chosen row | `returnPressed` 34621 |
| `activate(int r)` | 35156 | Range-guards, skips `kind < 0` disclosure rows, stops the debounce, then dispatches by kind to `g_lookupQuery` / `g_openScanInInput` / `g_openAtLine` / `g_apparatusQuery` / `g_goferQuery` / `QDesktopServices::openUrl` | `submitCurrent` 35153; `itemActivated` 34625 |

Members (35189–35197): `spine_` (reference), `box_`, `list_`, `timer_`, `acts_`, `root_`,
`filePaths_`, `filesIndexed_`, `persons_` (`QJsonObject`), `personsLoaded_`.
Global: `static HuntPalette* g_hunt = nullptr;` (**35198**).

## 4. Dependencies

**Data folders / files**
- `<root>/library` — recursive `QDirIterator` walk for `*.txt *.act *.inc *.TXT *.ACT *.INC *.png
  *.jpg *.tif`, lazily on first query (35091–35102). Full paths cached in `filePaths_` for the
  session; never invalidated.
- `<root>/data/extracted/persons_bdrc.json` — read once (`QJsonDocument::fromJson`, `authors`
  object) on first query (35000–35009). `personsLoaded_` is set from `pf.exists()` **before** the
  open, deliberately, so a missing bank is retried rather than becoming a dead lane.
- The **spine** (`allcore::Spine&`, injected): `lookup()`, `headwordSearch()`, `reverseIndex()`,
  `lookupByPronunciation()`, `corpusSearch()`.
- In-process global data banks: `g_teachingTib` (`TeachingMap*`, 774), `g_appNotes`
  (`std::vector<ApparatusNote>*`, 919), `g_appBib` (`std::vector<ApparatusBib>*`, 920) — all
  null-checked before use.

**Network hosts** (all via `QDesktopServices::openUrl`, `activate` kind `2` — the system browser)
- `https://library.bdrc.io/show/bdr:<pid>` — hard-coded (35070–35072).
- Treasury of Lives — URL supplied by `tolBiographyUrl(banked)` (3939) or `tolLinkForPid(pid, root_)`
  (3964); the host is not literal at this site.
- **Any** URL held in the teaching index (`m.url`, 35010) — unvalidated, unfiltered, no scheme check.

**QSettings keys:** **none.** `HuntPalette` reads and writes no settings — no persisted geometry,
no query history, no "recent hunts", no lane toggles.

**Callbacks it dispatches into** (all `main()`-installed globals; each null-checked at the call site)
| Global | Installed at | Used by kind |
|---|---|---|
| `g_lookupQuery` | 3604 | `0` (dictionary, English, pronunciation rows) |
| `g_goferQuery` | 35982 | `1` (corpus rows) |
| `g_openScanInInput` | 35960 | `3` (image files) |
| `g_openAtLine` | 35969 | `3` (text files) |
| `g_apparatusQuery` | 35937 | `4` (apparatus row) |

**Disk writes:** none. **Automated harness:** `ctest` test **`app_selftest`**
(`add_test(NAME app_selftest COMMAND DiamondCutterTranslationTool --selftest)`,
`app/CMakeLists.txt:89`); `HuntPalette::selfTest` is invoked at `app/main.cpp:38426`.

## 5. Release questions

- **Three lanes launch an external browser and none is tested.** The teaching row (kind `2`) opens a
  URL taken verbatim from a data file, with no scheme validation. Confirm the teaching index is
  trusted content, or validate to `https` + an allow-list before `openUrl`.
- **Six of the eight lanes have no activation coverage.** `selfTest` asserts that rows *appear* for
  corpus, teaching, apparatus, persons, and files, but never that pressing Enter on them does the
  right thing. The only fully drilled path is Enter → Lookup (kind `0`). Decide whether to extend
  `selfTest` or to accept.
- **The corpus lane cuts results with no disclosure.** It fetches 3 and shows 2 (34985–34988) with
  no "…and N more" line, while the persons and files lanes explicitly announce what they cut
  (house rule 3 — a capped list that does not say what it cut reads as complete). Fix, or accept
  the inconsistency?
- **Query meaning is decided by letter case, silently.** All-uppercase → ACIP conversion;
  anything else → wylie/English (34848–34855). There is no indicator of which interpretation ran,
  and `acipToEwts` is called with no try/catch. Should the palette show the interpretation, or
  offer both?
- **Four `catch (const std::exception&) {}` blocks swallow errors with no user or log signal**
  (34888, 34944, 34997, and the FTS fallback). A spine that fails mid-query renders as "nothing
  found". Should these at least log?
- **Double-click bypasses the debounce settle.** `itemActivated` → `activate` directly (34623),
  while Enter goes through `submitCurrent`. A user who clicks a row while the 220 ms debounce is
  still pending acts on the previous query's payload — the exact class of bug BOUNTY #12 fixed for
  the keyboard. Route `itemActivated` through `submitCurrent`-equivalent logic?
- **The library file walk is unbounded, synchronous, and on the UI thread** (35091–35102), and if
  the library is empty it **re-walks on every subsequent query** because `filesIndexed_` stays
  false. Measure on a full library; consider a bounded/async index or a "walked but empty" flag.
- **The file index never invalidates.** Files added, renamed, or deleted after the first query are
  invisible (or stale targets) for the rest of the session. Acceptable?
- **The apparatus lane's anchor loop** (35026–35031) is written as
  `for (…) if (hit) break; else if (an.wylie.contains(q, …)) hit = true;` — verify this expresses
  the intended "any anchor matches" test; it reads like a refactor artefact.
- **Kind `4` calls `accept()`; every other kind calls `hide()`** (35177–35180). Decide one
  dismissal semantic.
- **The corner button's tooltip promises "commands"** (36592–36596) — there is no command lane.
  Either add one or fix the tooltip before release.
- **Disclosure rows are selectable but inert.** Enter on "… and N more …" or on "— nothing exact"
  returns silently from `activate` (35159–35161). Should they be `Qt::NoItemFlags` like
  `HelpWindow`'s section headers?
- **The BOUNTY #12 check (c) can silently skip** (34776–34783) when the spine offers no second
  lookup row for "sunam". It logs `[SKIP]` honestly, but a data change could disable the
  arrow-row-survives-Enter assertion without any failure. Pin it to a fixture instead?
- **Only the first teaching match is shown** (`it->second.front()`, 35005–35012) with no count of
  the rest — the same silent-cap issue the persons and files lanes were fixed for.
- **`selfTest` borrows and restores the real `g_lookupQuery` global three times** (34666, 34702,
  34746). Correct on the happy path; a throw mid-block would leave the app's Lookup dispatch
  replaced by a no-op. Worth an RAII guard?
- **Persons matching is uppercase substring over a JSON blob** with no diacritic or transliteration
  folding (35010–35022) — confirm the expected name spellings actually match.
- **No query history, no persisted geometry, and `openPalette()` deliberately clears the box.**
  Confirm that "every ⌘K starts from nothing" is the intended feel for the app's headline
  keyboard affordance.
