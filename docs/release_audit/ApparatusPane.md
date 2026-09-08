# ApparatusPane — release audit

*Audited 2026-09-08 against `app/main.cpp` lines 15516–15779 (class body), 896–921 (supporting structs), 35865–35941 (wiring), 38261 + 38705–38779 (self-test).*

## 1. What it is and where it sits

`ApparatusPane` is the read-only browser over the **published apparatus**: the
footnotes and bibliography entries mined from the released, GMR-approved
volumes (per `docs/APPARATUS_DESIGN.md` §1, the OFFICIAL tier — 344 footnotes +
138 bibliography entries from SSL/OSE/SVN; pending translator candidates live
elsewhere and are not shown here). It is a plain top-level tab, added in
`main()` at `app/main.cpp:35941` as `tabs.addTab(apparatusPane, "Apparatus")` —
no sub-tab group, no menu gate — immediately after the apparatus JSON banks are
loaded into the file-static `appNotes` / `appBib` vectors and published through
the globals `g_appNotes` / `g_appBib` (`main.cpp:35869–35934`). The user reaches
it by clicking the **Apparatus** tab, or indirectly from the ⌘K command palette:
a palette row of `kind == 4` calls `g_apparatusQuery(payload)`
(`main.cpp:35175–35177`), and that lambda (`main.cpp:35937–35940`) raises the
pane via `g_raisePane` and calls `searchFor()`. The pane itself is a banner + a
search row (line edit + kind combo) + a `QSplitter` holding a `QListWidget` on
the left and a `QTextBrowser` detail view on the right.

## 2. User-visible functions

| Control/action | What it does | Code anchor (main.cpp) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Search field (`search_`, `QLineEdit`, placeholder "search lemmas, note text, bibliography entries…") | On every `textChanged`, calls `refill()`, which case-insensitively substring-matches the trimmed query against each note's `lemma`, `text`, and each anchor's normalized `wylie`, and against each bib entry's `id` and `text`. | ctor `ApparatusPane()` :15605, :15608–15609 (`connect`); `refill()` :15672 | selfTest: `"search narrows the bank"` (:15654) and `"searching a normalized Tibetan anchor finds the notes that quote it hyphenated"` (:15667) | Its text is persisted via `sess::remember(search_, "apparatus/search")` (:15610), so the app reopens with the previous filter applied — the list can look partly empty on launch. No debounce: every keystroke rebuilds the whole list (~482+ rows) from scratch. Query is `trimmed()` but not normalized further, so only the anchor field bridges hyphen/space spellings. |
| Kind filter (`kind_`, `QComboBox`: "All" / "Footnotes" / "Bibliography") | Index 0 = notes + bibliography, 1 = footnotes only, 2 = bibliography only; `currentIndexChanged` calls `refill()`. | ctor :15606–15607, :15610–15611 (`connect` at :15610-15611 region, `refill` gating at :15679, :15701) | Partial — selfTest only ever sets index 0 (`kind_->setCurrentIndex(0)` at :15647). **No check exercises index 1 or 2.** | Persisted as `apparatus/kind`; a saved index of 1 or 2 silently hides half the bank at next launch. |
| Result list (`list_`, `QListWidget`) — selecting a row | `currentRowChanged` → `show(r)`, which renders the selected footnote (number, lemma, source, body, plus `anchorHtml()`) or bibliography entry (id, source, section, text) into the detail browser. | `connect` :15612–15613; `show(int)` :15748 | selfTest: covered only indirectly — `refill()` calls `setCurrentRow(0)` (:15725), so `show()` runs, but **no check asserts the detail HTML in `selfTest()` itself**. The main-window self-test block at :38705–38779 does pin rendered `anchorHtml()` output. | Rows are `📎 note N — lemma` / `📚 id — text`; both truncate (48 / 52 chars) with no tooltip, so long lemmas are unreadable in the list. |
| Automatic first-row selection / initial population | The constructor calls `refill()` (:15616); `showEvent()` re-runs `refill()` whenever the list holds ≤ 1 item, as a belt against the banks loading after construction. | ctor :15616; `showEvent` :15660–15665 | selfTest: `"bank arrives populated, no user action needed"` (:15645) — deliberately asserted *before* any refill; and `"bank lists the full published apparatus"` (:15650) | The `<= 1` guard is a heuristic: a legitimate one-result search leaves `count()==1`, so every re-show of the pane re-runs `refill()` needlessly (harmless, but it also re-selects row 0 and discards the user's selected row on tab switch). |
| Empty-state row + detail message | When nothing matches, inserts a non-selectable `QListWidgetItem` teaching either "no matches — clear the search or switch the filter" or "the bank is empty until a data release with apparatus is installed", and sets a matching italic message in the detail browser. | `refill()` :15726–15743 | NONE — no selfTest check drives the pane to zero results or to a missing-bank state | The hint item is added to `list_` but **not** to `rows_`, so `show()`'s bounds check (`r >= rows_.size()`) is what keeps a click on it inert — correct, but fragile if `rows_` and `list_` ever drift. |
| Detail pane (`detail_`, `QTextBrowser`) | Displays the rendered footnote or bibliography HTML. Read-only. | ctor :15602; `show()` :15748 | Rendered `anchorHtml()` is pinned by the main self-test block: "TIBETAN NAMED IN THIS NOTE", the sample's wylie, "candidates", "hgm_reverse_index", "WHERE HIS PUBLISHED ENGLISH", "not a term equivalence" (:38726–38747) | `QTextBrowser` defaults to `openExternalLinks == false` and no `anchorClicked` handler is connected, and the generated HTML contains no `<a>` — so there is **no link scheme** and no navigation. All inserted text goes through `toHtmlEscaped()` / `ux::snip`, so bank data cannot inject markup. |
| Anchor rendering — "TIBETAN NAMED IN THIS NOTE" block | For each `ApparatusAnchor`: converts wylie to Tibetan unicode via `allcore::wylieToUnicode`, shows the unicode + bold wylie, the "(written …)" variant when `asWritten` differs, and the evidence excerpt (ellipses only for non-sentence excerpts). Explicitly captioned as machine-located candidates, not a ruling. | `anchorHtml()` static, :15528–15592 | Main self-test block :38726–38735 (header, wylie present, "candidates" wording) | Public `static` — also called from the self-test at :38728/:38743/:38754. If `wylieToUnicode` returns `!ok`, the unicode is silently omitted and only wylie shows (honest, no fabrication — matches rule 3). |
| Anchor rendering — "WHERE HIS PUBLISHED ENGLISH USES THIS LEMMA (same work)" block | For anchors with `passage == true`, prints a separate green-headed section showing `asWritten`, the converted passage (truncated at 220 chars), and the evidence snippet (`ux::snip`, 260 chars), labeled "the passage, not a term equivalence". | `anchorHtml()` :15533–15556 | Main self-test :38749–38759 ("WHERE HIS PUBLISHED ENGLISH", "not a term equivalence") | Passage header emitted once, on first passage anchor; ordering therefore depends on the JSON's candidate order — a passage anchor appearing after term anchors puts the header mid-list. |
| ⌘K command-palette route into the pane (`searchFor`) | Forces kind to "All" (index 0) and sets the search text; `textChanged` does the refill. Raises the pane first via `g_raisePane`. | `searchFor()` :15520–15523; wiring :35937–35940; palette dispatch :35175–35177 | selfTest (main block): `"Apparatus: the ⌘K hunt lane routes a query into the pane"` (:38765–38773), verified through `currentSearchText()` | Silently overrides the user's saved kind filter. Writes the palette query into `QSettings` as `sess/apparatus/search`. |
| Splitter (list / detail) drag | Resizes the two panes; list has `setMinimumWidth(320)`, detail gets stretch factor 1. | ctor :15597–15604 | NONE | Splitter position is **not** remembered across sessions (unlike the search text and kind), an inconsistency with the rest of the pane's state handling. |

**Controls that do not exist in this pane** (verified by grep over 15516–15779):
no `QPushButton`, `QToolButton`, `QCheckBox`, `addAction`/menu entries,
`setShortcut`, `setToolTip`, context menu (`customContextMenu`),
`itemDoubleClicked`, `anchorClicked`, drag/drop (`dropEvent`), `QFileDialog`,
`QDesktopServices`, `QProcess`, `QFile`/`QSaveFile`, `QNetworkAccessManager` /
`TimedNam`, or direct `QSettings`. There is **no export, no copy-to-draft, and
no "insert citation"** action — despite `docs/APPARATUS_DESIGN.md` describing
the apparatus as "searchable and **reusable**" (the pane's own banner text says
"reusable" too, at :15583). Reuse currently happens only from other panes.
No `TODO`/`FIXME`/`XXX`/`HACK` markers appear inside the class.

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `currentSearchText() const` | 15518 | Returns the search field's current text. | Main-window self-test :38766 (⌘K routing check). Not used by production code. |
| `searchFor(const QString&)` | 15520 | Public entry point: sets kind to "All" and the search text (refill happens via signal). | `g_apparatusQuery` lambda :35939; self-test :38771. |
| `anchorHtml(const ApparatusNote&)` *(public static)* | 15528 | Builds the "Tibetan named in this note" / "where his published English uses this lemma" HTML block for a note's anchors. | `show()` :15757; self-test :38728, :38743, :38754. |
| `ApparatusPane()` ctor | 15594 | Builds banner, search row, splitter, wires the three connections, registers session persistence, runs the first `refill()`. | `main()` :35936. |
| `selfTest(QStringList&)` | 15632 | Four checks: pre-populated bank, full bank listed, search narrows, normalized-anchor search finds hyphenated notes. Returns fail count. Leaves the pane with an empty search. | `main()` :38261. |
| `showEvent(QShowEvent*)` *(protected override)* | 15660 | Re-runs `refill()` if the list holds ≤ 1 item, so late-loading banks still populate. | Qt, on tab show. |
| `refill()` *(private)* | 15670 | Clears and rebuilds `list_` + `rows_` from `g_appNotes` / `g_appBib` under the current query and kind filter; selects row 0 or installs the empty-state hint. | ctor :15616; `showEvent` :15663; the two `connect` lambdas :15609, :15611; `selfTest` :15649, :15653, :15659, :15676. |
| `show(int r)` *(private)* | 15748 | Renders row `r`'s note or bibliography entry into `detail_`; bounds-checked against `rows_`. | `list_` `currentRowChanged` lambda :15613. |

Member state: `search_`, `kind_`, `list_`, `detail_`, and
`std::vector<std::pair<bool,int>> rows_` (`true` = note, `false` = bib; the int
indexes into the corresponding global vector) — :15773–15778.

Supporting structs (`main.cpp:896–918`): `ApparatusAnchor` (`wylie`,
`asWritten`, `evidence`, `technical`, `passage`), `ApparatusNote` (`lemma`,
`text`, `source`, `num`, `anchors`), `ApparatusBib` (`source`, `section`, `id`,
`text`). Free/helper functions used: `allcore::wylieToUnicode` (engine port),
`ux::snip` (truncating excerpt helper), `QString::toHtmlEscaped`,
`sess::remember` (:3090 QLineEdit overload, :3096 QComboBox overload).

## 4. Dependencies

**Data files** — read in `main()` before the pane is constructed, *not* by the
pane itself (`main.cpp:35871–35934`), relative to the app's `root`:

- `data/extracted/apparatus_notes.json` — footnote bank; falls back to
  `data/extracted/mixed_nuts_notes.json` when absent. Fields: `lemma`, `text`,
  `source`, `num` (or legacy `note`).
- `data/extracted/apparatus_anchors.json` — `rows[]` of `{lemma, source,
  candidates[]}`; candidates carry `wylie`, `as_written`, `evidence`,
  `technical_spelling`, `passage`. Joined to notes on `lemma + \x1f + source`
  so a lemma reused across two books cannot cross-contaminate.
- `data/extracted/apparatus_bibliography.json` — bibliography bank; falls back
  to `data/extracted/mixed_nuts_bibliography.json`. Fields: `source`,
  `section`, `id`, `text`.
- Also referenced by the same paths at `main.cpp:32996` and `:33075` (other
  consumers of the bank).
- Documentary source of the banks: `docs/apparatus/MAIN_FOOTNOTES.md` +
  `MAIN_BIBLIOGRAPHY.md`, regenerated by `tools/build_main_apparatus.py`.

**Network**: none. No `QNetworkAccessManager`, `TimedNam`, or URL use anywhere
in the class; the `QTextBrowser` has no external links.

**Disk writes**: none from the pane (no `QFile`, `QSaveFile`, `QFileDialog`,
`QProcess`, `QDesktopServices`).

**QSettings** — organization `ALL`, application `TranslationTool`, all through
`sess::` which prefixes `sess/` and is a no-op under `g_harnessRun`:

| Key | Read | Written |
|---|---|---|
| `sess/apparatus/search` | ctor, via `sess::remember(QLineEdit*)` :3090 | on every `textChanged` :3093 |
| `sess/apparatus/kind` | ctor, via `sess::remember(QComboBox*)` :3096 | on every `currentIndexChanged` :3099 |

(The separate key `gofer/apparatusChecked` at :16628/:16730/:16754 belongs to
the Gofer pane's source checkboxes, not to this pane.)

**ctest suites**: no dedicated apparatus suite exists. The pane's `selfTest()`
runs only inside `add_test(NAME app_selftest COMMAND
DiamondCutterTranslationTool --selftest)` (`app/CMakeLists.txt:89`); the
Gauntlet walk (`app/CMakeLists.txt:92`) exercises the GUI generically. No suite
in `core/CMakeLists.txt` touches the apparatus banks.

## 5. Release questions

- **Reuse gap.** The banner and `docs/APPARATUS_DESIGN.md` both promise a
  "searchable and **reusable**" apparatus, but the pane offers no copy, export,
  insert-citation, or send-to-draft action. Is that intentional for this
  release (reuse lives in Draft/Overlay only), and should the banner wording be
  softened if so?
- **Kind filter is untested and sticky.** Nothing automated ever selects
  "Footnotes" or "Bibliography", and the choice persists across launches. Please
  manually verify both filter positions, and decide whether the saved kind
  should reset to "All" on launch.
- **Persisted search hides the bank on launch.** With `apparatus/search`
  restored, the user reopens the app to a filtered (possibly empty) apparatus.
  Confirm this is wanted, or clear the search at startup.
- **`>400` floor in `selfTest`.** The design doc states the official bank is 344
  notes + 138 bib = 482 rows. The check asserts only `count() > 400`, so a
  release that silently loses ~80 entries still passes. Decide whether to pin
  the exact expected counts per data release.
- **Silent bank-load failure.** If the JSON files are missing or unparseable,
  `main()` simply leaves `g_appNotes`/`g_appBib` null and the pane shows the
  polite "bank is empty until a data release with apparatus is installed"
  message. There is no error surfaced, no path named, and `app_selftest` would
  fail only via the `>400` check. Should a missing-bank condition be loud at
  startup?
- **Empty-state and missing-bank paths have zero automated coverage.** Worth a
  manual pass: search for gibberish (expect the "no matches" hint, inert row,
  "<i>no matches</i>" detail) and a run with the data folder renamed.
- **Selection loss on tab switch.** `showEvent`'s `count() <= 1` refill re-runs
  and re-selects row 0 whenever a single-result search is showing. Confirm
  acceptable.
- **Splitter position is not remembered** while search/kind are — decide whether
  to add it for consistency.
- **Provenance wording** (rule 1). The anchor blocks are carefully labeled
  "candidates, not a ruling" and "the passage, not a term equivalence"; those
  literal strings are pinned by the self-test at :38726–38759. Any copy edit to
  `anchorHtml()` must keep those substrings or the battery fails — flag this to
  whoever does the final wording pass.
