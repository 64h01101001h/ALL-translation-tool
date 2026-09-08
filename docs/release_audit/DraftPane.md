# DraftPane — release audit

`class DraftPane` (app/main.cpp:19335–21432) is the translator's working bench: the user pastes ACIP/Wylie source into the top-left editor, presses **Load source**, and the text is split into clickable clauses; clicking (or merely arrowing into) a clause raises the "Evidence Ribbon" — colour-banded HGM anchors, a Wilson-reading-order scaffold, exact clause parallels from the corpus, quotation status, and 84000 reference comparanda. The bottom half holds the English draft, a live terminology guard, a shared-apparatus search (footnotes + bibliography), and the publishing exits (RTF export, bibliography composer, candidate footnote proposal, "Send to Manuscript →"). It is constructed at app/main.cpp:35862 and added as the top-level tab **"Draft"** (app/main.cpp:35863), sitting between "Drills" and "Manuscript" in the main `QTabWidget`; the user reaches it by clicking that tab. Two of its methods are also reachable from the Manuscript pane through the global seams `g_mssComposeBib` / `g_mssProposeNote` (app/main.cpp:35949–35952). Its `selfTest()` (line 19843) is invoked from the app-wide harness at app/main.cpp:38264, which is registered as the ctest suite **`app_selftest`** (`DiamondCutterTranslationTool --selftest`, app/CMakeLists.txt:89).

## User-visible functions

| Control/action | What it does | Code anchor (main.cpp line) | Automated coverage | Notes/risks |
|---|---|---|---|---|
| Source editor (`QPlainTextEdit`, "Source ACIP…") | Free-text input for the source; content persisted per session | ctor, 19488–19490 | indirectly, via `demo()` in selfTest ("clauses split") | `sess::remember(source_, "draft/source")` writes to QSettings (suppressed under harness runs) |
| Source cursor move → Evidence Ribbon auto-follow | 350 ms debounced timer re-tokenizes the text up to the cursor, finds the containing clause and calls `showAnchors()` — evidence with zero clicks | ctor lambda 19500–19524 | `"Evidence Ribbon scaffold renders for a clause"` (covers `showAnchors`, not the timer path) | The debounce/cursor path itself is never fired in selfTest; re-tokenizes the whole prefix on every keystroke-quiet moment (O(n) per idle tick on long texts) |
| **Load source** (ribbon WORKBENCH) | `load()`: detects Wylie vs ACIP, builds the overlay, refines clauses, renders them as `c:<n>` links | button 19495; `load()` 19941 | `"clauses split"` | — |
| **Extract outline (sa bcad)** (STRUCTURE) | `outline()`: extracts the topical outline tree, flags out-of-sequence nodes | button 19529; `outline()` 20363 | NONE in selfTest; core engine covered by ctest `outline_smoke` | Pane-level rendering unverified |
| **Structural units (bam po / le'u)** (STRUCTURE) | `structuralUnits()`: explicit BAM PO / LE'U markers plus labelled shloka/bampo *estimates* | button 19531; `structuralUnits()` 20418 | NONE | Estimates are labelled honestly in the HTML; no test asserts the label survives |
| **Verse meter** (STRUCTURE) | `verse()`: dominant meter detection, per-line syllable counts, irregular lines highlighted | button 19538; `verse()` 20482 | NONE in selfTest; engine covered by ctest `verse_smoke` | — |
| **House style check…** (STRUCTURE) | `showStyleCheck()`: runs `styleCheck()` (curly quotes, `--`, en-dash ranges, serial `&`, era style, house word list) into a modeless dialog; clicking a row jumps the draft cursor | button 19541; `showStyleCheck()` 19422; `styleCheck()` 19346 | NONE in DraftPane::selfTest; `DraftPane::styleCheck` is exercised elsewhere at main.cpp:40244 (sweep/report path) | Dialog is `WA_DeleteOnClose` and modeless; findings capped at 40 per rule with no "and N more" disclosure — a silent cap, contrary to the house cap-disclosure rule applied in `showConcordance` |
| **Verse reading order** (STRUCTURE) | `verseReading()`: groups lines into stanzas, runs verb-first reading plan per stanza, estimates dropped case particles | button 19551; `verseReading()` 20558 | NONE | Dropped-particle count is a heuristic (`~%1 case particle(s) likely understood`) shown in warning red |
| **Detect quotations** (EVIDENCE) | `detectQuotes()`: exact corpus matches ≥7 syllables; offers matching published footnotes and bibliography entries as insert links | button 19566; `detectQuotes()` 20773 | NONE in selfTest; engine covered by ctest `quotation_smoke` | ACIP-vs-Wylie decided by "any uppercase letter present" heuristic (20779–20781) |
| **Phrase memory** (EVIDENCE) | `phraseMemory()`: for the selected source phrase, all corpus renderings (binding) + Align-harvest TSV pairs labelled PENDING | button 19570; `phraseMemory()` 21287 | `"phrase memory finds corpus renderings"`, `"fixture holds: the phrase exceeds the 12-row fetch"`, `"phrase memory prints the UNCAPPED segment total"`, `"phrase memory never prints its page size as the total"` | Reads TSVs from `<root>/library/links` and `<root>/data/candidate_alignments`; unopenable files are skipped silently; harvest hits hard-capped at 12 with no remainder disclosure |
| **Check terminology** (EVIDENCE) | `check()`: matches source terms against the draft — unmatched, mixed renderings, PROVISIONAL-sourced, shared-English collapses | button 19675; `check()` 21221 | NONE directly (only the live-guard variant is asserted); engine covered by ctest `terminology_smoke` | — |
| Terminology live-guard status line | 1.5 s debounced re-run of `checkTerminology` after typing; green/amber chip counting unrendered terms, collapses, honorific-register terms | ctor 19629–19672 | `"terminology live-guard reports as you type"` | Runs the full terminology check on every typing pause — cost scales with draft+source size |
| **Send to Manuscript →** (WORKBENCH) | Calls `g_sendToManuscript(draft text)`, which sets the Manuscript text and raises that pane | button 19677; lambda 19683 | `"the ladder is wired…"` and `"ladder hand-off: the Manuscript now holds exactly the draft that was sent"` | Silently does nothing if `g_sendToManuscript` is unset |
| Apparatus search field (`QLineEdit`, Return) | `searchNotesBank()`: substring search over footnote bank, bibliography bank, and pending candidates; each hit renders `note:`/`bib:`/`cand:` insert links | field 19687; `searchNotesBank()` 20608 | NONE | `sess::remember(notesSearch_, "draft/notesSearch")`; missing bank files produce an instructional message, not an error; result counts capped at 10/8/5 silently |
| **Export draft (RTF, \*italics\*)…** (PUBLISH) | `exportRtf()`: file dialog, converts `*…*` to RTF italics, escapes non-ASCII as `\uNNNN?`, writes via `saveOrWarn` | button 19696; `exportRtf()` 21109 | NONE | Disk write to a user-chosen path; failed write reports "NOT exported" (WP-6 honest verdict); unclosed `*` closed automatically and warned |
| **Compose bibliography entry…** (PUBLISH) | `composeBibDialog()`: modal STD-007 form (7 fields + Sanskrit branch of 9 more), live preview, BDRC scan link, insert-into-draft or save-as-candidate | button 19702; `composeBibDialog()` 20872 | NONE | Writes `data/candidate_bib.json` via `saveOrWarn`; also reachable from Manuscript via `g_mssComposeBib` (35949) |
| ↳ **Sanskrit work** checkbox in that dialog | Reveals the 9 canonical-citation rows and switches the composer to `composeSanskritBibEntry` | 20918–20957 | NONE | — |
| ↳ **Auto-fill from catalog (by ACIP number…)** | `autoFillBib()`: fills author/dates/titles from `catalog_works.json`, falling back to `catalog_titles.json` | button 20958; `autoFillBib()` 21014 | NONE | Missing catalog record only sets a tooltip on the ACIP field — an easily-missed failure signal |
| ↳ **Pair-hyphenate author (STD-002 helper)** | Rewrites the author field through `hgmTechnicalSpelling` | 20989 / 21001 | NONE | Destructive in-place edit of the typed field, no undo |
| ↳ **Save as candidate (pending GMR approval)** | Appends the composed entry to `data/candidate_bib.json` with `status: pending GMR approval` | 20976 (lambda) | NONE | Disk write; honest "NOT saved" path present |
| **Propose footnote (pending GMR approval)** (PUBLISH) | `proposeNote()` → `proposeNoteText()`: saves the *selected draft text* as a candidate note in `data/candidate_notes.json` | button 19710; `proposeNote()` 20800; `proposeNoteText()` 20812 | NONE | Disk write; empty selection reports an inline hint; also reachable from Manuscript via `g_mssProposeNote` (35950) |
| **AI back-check (API, labeled AI)** | `aiCheck()`: streams a coverage diff from `api.anthropic.com`, prefixed as model output; disabled with an explanatory tooltip when no key is present | button 19714; `aiCheck()` 21089 | NONE (harness forces `anthropicKey()` empty, so the button is disabled under `--selftest`) | **Network egress** of the full source + draft to Anthropic; 60 s transfer timeout; truncated streams are labelled TRUNCATED (FAIL-7); the only entirely untested user path with side effects outside the machine |
| Clause list link `c:<n>` (click a clause) | `showAnchors(n)` — renders the clause in Tibetan script with hue-banded terms, glosses, scaffold, parallels, quotations, 84000 hits | 19771 (anchorClicked); `showAnchors()` 20000 | `"anchor hue bands render"`, `"HGM glosses listed"`, `"Evidence Ribbon scaffold renders for a clause"` | — |
| Anchors link `t:<wylie>` (click a term) | `showConcordance()` — uncapped corpus total, per-course breakdown over the fetched rows, 8 sample segments, 84000 + link-out tiers | 19806; `showConcordance()` 20326 | `"term concordance reaches the corpus"`, `"fixture holds: sems can exceeds the 200-row fetch"`, `"concordance prints the UNCAPPED corpus total"`, `"concordance never prints the fetch cap as the total"`, `"concordance discloses the cap and its remainder"`, `"the per-course breakdown says which rows it covers"` | Best-covered surface in the pane (DATA-3 regression) |
| Anchors link `ts:<wylie>` ("+ technical spelling") | Inserts `pronunciation (technical spelling: …)` at the draft cursor (STD-002) | 19778–19805 | NONE | — |
| Anchors link `back` | Returns the anchors panel to the last clause | 19808 | NONE | — |
| Anchors link `http…` | Opens the external link-out tier in the system browser via `QDesktopServices::openUrl` | 19810 | NONE | Opens an external browser; URLs come from `linkOutHtml`/`tm84000Html` |
| Report link `note:<i>` | `insertNote()` — inserts a published footnote with its reuse citation | 19725; `insertNote()` 20760 | NONE | — |
| Report link `bib:<i>` | `insertBibEntry()` — inserts a published bibliography entry with citation | 19727; `insertBibEntry()` 21068 | NONE | — |
| Report link `cand:<i>` | `insertCandidate()` — inserts a PENDING, not-GMR-approved note, labelled as such | 19729; `insertCandidate()` 21077 | NONE | Honesty-critical label lives only in this one string |
| Draft editor (`QPlainTextEdit`) | The English draft; persisted per session; target of every insert action | 19613–19616 | exercised throughout selfTest | `sess::remember(draft_, "draft/english")` |
| **File → Save / Save As** (menu, ⌘S / ⇧⌘S) — ADDED 2026-09-08 | `saveDraft()` writes the English draft as UTF-8 plain text to `draftPath_` (Save As asks, `.txt/.md`); status on the terminology line; failure says NOT SAVED via `saveOrWarn` | `saveDraft()` / `saveDraftAs()` before `selfTest()` | `"Save As writes the draft byte for byte…"`, `"Save reuses the Save As path…"`, `"a failed draft Save says NOT SAVED…"` | Draft only — the source box is not saved (it came from a file or the Overlay). No Open counterpart yet |

No `setShortcut`, `addAction`, context menu, drag/drop, or double-click handlers exist anywhere in the class — the pane is entirely button- and link-driven, and has **no keyboard accelerators of its own**.

## Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `styleCheck` (static, public) | 19346 | Pure mechanical house-style scanner returning `{pos, rule}` findings | `showStyleCheck()`; also main.cpp:40244 |
| `showStyleCheck` | 19422 | Builds and shows the style-findings dialog | House style check button |
| `DraftPane` (ctor) | 19480 | Builds the whole pane, ribbon, timers, and every connection | main.cpp:35862 |
| `demo` | 19835 | Screenshot/harness hook: set source, `load()`, open clause 0 | `selfTest()`; main.cpp:40768 (screenshot run) |
| `selfTest` | 19843 | 17 assertions over clauses, anchors, concordance caps, phrase memory, live guard, Manuscript ladder | main.cpp:38264 (`app_selftest`) |
| `tokEwts` | 19936 | Token → EWTS, honouring the detected Wylie/ACIP document mode | `showAnchors`, `phraseMemory`-adjacent rendering |
| `load` | 19941 | Overlay build + clause refinement + clause-list HTML | Load source button, `demo()` |
| `termSpans` | 19970 | Maximal glossed non-particle spans overlapping a clause | `showAnchors` |
| `showAnchors` | 20000 | Renders the full Evidence Ribbon for a clause | clause links, evTimer, `demo`, `back` link |
| `showConcordance` | 20326 | Term concordance card with disclosed caps | `t:` links, `selfTest` |
| `outline` | 20363 | sa bcad outline report | Extract outline button |
| `structuralUnits` | 20418 | bam po / le'u report with labelled estimates | Structural units button |
| `verse` | 20482 | Verse meter report | Verse meter button |
| `verseReading` | 20558 | Stanza-level reading order report | Verse reading order button |
| `loadNotesBank` | 20560s (20560–20600) | Lazily loads both footnote banks + bibliography bank + candidates | `searchNotesBank`, `detectQuotes` |
| `dataFile` | 20604 | Resolves a data file relative to the bundle, else CWD | all bank/candidate I/O |
| `searchNotesBank` | 20608 | Apparatus search rendering | notes search field |
| `loadCandidates` | 20744 | Reloads `candidate_notes.json` into the pending bank | `loadNotesBank`, `proposeNoteText` |
| `proposeNote` | 20800 | Selection guard, then `proposeNoteText` | Propose footnote button |
| `proposeNoteText` (public) | 20812 | Appends a candidate note to disk and reports the verdict | `proposeNote`, `g_mssProposeNote` |
| `insertNote` | 20760 | Insert published note at cursor | `note:` link |
| `detectQuotes` | 20773 | Quotation detection report + note/bib recommendations | Detect quotations button |
| `exportRtf` | 21109 | STD-004 RTF writer | Export draft button |
| `composeBibDialog` (public) | 20872 | STD-007 bibliography composer dialog | Compose button, `g_mssComposeBib` |
| `loadCatalogWorks` | 21005 | Lazily loads `extracted/catalog_works.json` | `autoFillBib` |
| `autoFillBib` | 21014 | Catalog-driven field auto-fill | Auto-fill button |
| `insertBibEntry` | 21068 | Insert published bibliography entry | `bib:` link |
| `insertCandidate` | 21077 | Insert PENDING candidate note | `cand:` link |
| `aiCheck` | 21089 | Streaming Anthropic coverage diff | AI back-check button |
| `check` | 21221 | Full terminology report | Check terminology button |
| `phraseMemory` | 21287 | Corpus + Align-harvest phrase memory | Phrase memory button |

## Dependencies

**Data files read** (via `dataFile()`, which tries `<bundle>/../../../../../data/<name>` then `<cwd>/data/<name>`):
- `data/extracted/mixed_nuts_notes.json`, `data/extracted/mixed_nuts_notes_older.json`
- `data/extracted/mixed_nuts_bibliography.json`
- `data/candidate_notes.json` (read **and written**)
- `data/candidate_bib.json` (read **and written**)
- `data/extracted/catalog_works.json`
- `<root>/data/extracted/catalog_titles.json` (read via `root_`, not `dataFile()` — a second, inconsistent path convention)
- `<root>/library/links/*.tsv` and `<root>/data/candidate_alignments/*.tsv` (Align harvest)
- The SQLite spine (`allcore::Spine&`), the 84000 TM (`tm84000()`), the honorifics set (`g_honorifics`), and `allcore::Progress*` (`touchWord` on every concordance view — a progress-DB write).

**Disk writes:** `data/candidate_notes.json`, `data/candidate_bib.json`, user-chosen `*.rtf`. All three go through `saveOrWarn`, and all three report an honest "NOT saved / NOT exported" verdict on failure.

**Network:** `https://api.anthropic.com/v1/messages` (POST, streaming SSE, 60 s transfer timeout) from `aiCheck()` only. Credentials come from `anthropicKey()` (main.cpp:2816): `ANTHROPIC_API_KEY` env, else `~/Library/Application Support/{Diamond Cutter Translation Tool,ALL Translation Tool}/anthropic_api_key`. Also outbound: `QDesktopServices::openUrl` for `http…` anchors, and BDRC scan URLs shown (not fetched) in the bibliography composer.

**QSettings** (organisation `ALL`, application `TranslationTool`, all under the `sess/` prefix, suppressed when `g_harnessRun`):
- `sess/draft/source` (read+written)
- `sess/draft/english` (read+written)
- `sess/draft/notesSearch` (read+written)

No `QProcess`, no `QSaveFile`, no explicit `QSettings` access beyond the `sess::` helpers.

## Release questions

- **The AI back-check has zero automated coverage and is the pane's only network egress** — it ships the entire source and draft to Anthropic. Confirm this is disclosed to translators in the UI (currently only the button label "labeled AI" and the result banner) and decide whether it should be off by default.
- **`styleCheck` silently caps at 40 findings per rule** while `showConcordance` and `phraseMemory` were explicitly fixed (DATA-3) to disclose caps. Decide whether the same disclosure rule applies here; likewise the 10/8/5 apparatus-search caps and the 12-hit Align-harvest cap.
- Nine buttons write to disk or compose publishable text with **no selfTest assertion at all** (outline, structural units, verse, verse reading, detect quotations, check terminology, export RTF, compose bibliography, propose footnote). Decide which need pane-level checks before release, or record the manual verification.
- `autoFillBib` reports "no catalog record for this number" **only as a tooltip** on the ACIP field — verify manually that a wrong ACIP number is noticeable to a user.
- `catalog_titles.json` is loaded from `root_` while every other data file goes through `dataFile()`. Verify both resolve in the shipped bundle layout, not just in a dev checkout.
- The Manuscript seams (`g_mssComposeBib`, `g_mssProposeNote`) call into DraftPane from another tab; confirm the composer dialog's parent/modality behaves correctly when raised from the Manuscript.
- `showAnchors` swallows FTS syntax exceptions on exotic clause text (20554-ish, `catch (const std::exception&)` with a "stay silent rather than wrong" comment). Confirm silence is the intended behaviour rather than a visible "could not search this clause" note.
- The pane defines no keyboard shortcuts and no context menus; confirm this is acceptable for the accessibility pass.
