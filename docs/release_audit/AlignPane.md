# AlignPane — release audit

*Source: `app/main.cpp` lines 26686–27297 (class `AlignPane`). Audited against the code only; nothing inferred beyond what the source states.*

## 1. What the pane is

`AlignPane` is the Hypercontext-style hypertext alignment workbench: the translator loads a Tibetan (ACIP or wylie) text on the left and its English translation on the right, clicks a Tibetan word (clicking again cycles the word-finder's candidate spans), selects the matching English, and presses **space** (or **Link**) to record the pair. Links are tinted in both panes (blue for a plain link, green when the link nests inside a longer one), persist per source text as a TSV under `<root>/library/links/`, and can be harvested out as PENDING dictionary candidates. The pane also hosts two file-comparison tools that are not strictly alignment: legacy `.hyp` import and a critical-edition collation dialog. It is a top-level tab in the main tab bar — constructed at `main.cpp:35956` and added as the tab **"Align"** at `main.cpp:35957`, between "Review" and "Input". `restoreSession()` is called at `main.cpp:40554` at startup, and `selfTest()` is invoked by the harness sweep at `main.cpp:38267`.

## 2. User-visible functions

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Open ACIP file…** (TEXTS ribbon) | File dialog rooted at `<root>/library`; reads the chosen file as UTF-8 into the Tibetan pane, records it as `docFile_`, and loads/tokenizes | ctor lambda `main.cpp:26753`, → `openFile()` `26841` | NONE (selfTest never calls `openFile`) | Open failure is reported via `warnOpenFail`; no file-type validation — any file is read as text. Writes QSettings `sess/align/lastFile`. |
| **Import .hyp…** (TEXTS ribbon, has tooltip) | Opens a legacy Hypercontext `.hyp`, parses via `allcore::parseHypFile`, replaces both texts, converts numbered links into Align links; sub-syllable links widen to whole syllables, unmappable links skipped, both counted in a status line | ctor `26706`–`26713`, → `importHyp()` `27046` | `hyp_smoke` ctest suite (`core/CMakeLists.txt:153`) covers the parser only; the pane-side token mapping has NONE | **Destructive**: on success `linksList_.clear()` (`27083`) discards all existing links for the document and immediately `saveLinks()` overwrites the on-disk TSV — no confirmation, no undo. If token mapping fails, links are left untouched but a red banner appears. |
| **Load pasted texts** (TEXTS ribbon) | If no file is open, sets `docFile_ = "untitled"`, then tokenizes the pasted Tibetan, re-renders it fixed-layout, sets both editors read-only, and reloads persisted links | ctor `26758`, → `loadTexts()` `26879` | `selfTest`: "Tibetan side tokenized", "wylie input detected", "harvest wylie stays lowercase for wylie docs" (`26794`–`26802`) | With `docFile_ == "untitled"` `linksPath()` returns empty, so links made on pasted text are **never persisted** and are silently lost — no user-facing warning (`26870`, `saveLinks()` early-returns at `27153`). |
| **Link (space)** button (LINKING ribbon) | Records the current Tibetan token span + the current English selection as a link, saves, repaints; disabled until a Tibetan word is selected | ctor `26716`/`26752`, → `makeLink()` `27013` | NONE | If no English is selected it replaces the links list HTML with an inline hint, wiping the visible link list until the next repaint (`27016`). |
| **Space bar** shortcut | Same as Link; `QShortcut` with `WidgetWithChildrenShortcut` context | ctor `26748`–`26751` | NONE | Editors are read-only after load, so space is unlikely to conflict — but it fires whenever focus is anywhere in the pane. |
| **Delete last link** (LINKING ribbon) | Pops the last link, saves, repaints; no-op on empty list | ctor `26762` | NONE | No confirmation, no redo. |
| **× link in the links list** (`del:<i>` anchor) | Deletes link *i* from the list, saves, repaints | ctor `anchorClicked` `26773`–`26783` | NONE | Only the `del:` scheme is handled; any other anchor is silently ignored. |
| **Click in the Tibetan pane** (cursor move) | Maps the caret to a token, selects the word-finder span at that token; clicking the same token again cycles through candidate spans and finally the bare token | ctor `26769`, → `onTibClick()` `26951` | NONE | Fires on any cursor move, not only mouse clicks (keyboard navigation also cycles). |
| **Export aligned pairs (PENDING)…** (HARVEST ribbon) | Creates `<root>/data/candidate_alignments/`, offers a save dialog defaulting to `<base>-pairs.tsv`, writes a header marking the file PENDING/never auto-ingested plus one wylie⇥english row per link | ctor `26768`, → `exportPairs()` `27181` | NONE | Silent no-op when the list is empty (`27182`) — button gives no feedback. Write is gated by `writeAllOrWarn` (DATA-4), partial file removed on failure. Creates a directory in the repo root even if the user then cancels. |
| **Compare editions…** (HARVEST ribbon, has tooltip) | Asks for witness A then witness B (txt/act/acip), converts wylie→ACIP, refuses empty/unreadable or >500k-char inputs, and opens a modeless dialog with the colored collation plus a numbered apparatus | ctor `26723`–`26732`, → `compareEditions()` `27216` | `selfTest`: "edition collation: sites, readings, apparatus" and "a null witness collates as empty, not as an error (MEM-2)" (`26805`–`26833`) — pure-function level only, dialog not exercised | An unreadable file and a genuinely empty file produce the same message (`readT` returns `QString()` on open failure, `27231`). Dialog is `WA_DeleteOnClose` and modeless — multiple can stack. |
| **Save apparatus (Markdown)…** (inside collation dialog) | Save dialog defaulting to `apparatus.md`; writes witness paths + apparatus via `saveOrWarn` | `compareEditions()` `27264`–`27279` | NONE | Default filename has no directory, so it lands wherever the dialog's remembered folder points. |
| **Session restore (automatic)** | On startup reopens `sess/align/lastFile` if still readable, which also reloads that document's saved links | `restoreSession()` `26856`, called `main.cpp:40554` | NONE | Reads and re-parses an arbitrary previously-opened file at launch with no user prompt. |
| **Links list panel** | Read-only HTML summary: link count plus `[wylie = english ×]` chips, English truncated to 40 chars | `repaint_()` `26965` | Indirectly via selfTest's load checks | `setOpenLinks(false)`, so only the `del:` handler acts — no external-link risk. |

**Count: 12 user-visible functions; 9 with NONE automated coverage.**

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `AlignPane(Spine&, root)` | 26688 | Builds banner, ribbon (TEXTS/LINKING/HARVEST), splitter with both editors, links browser, and all connections | `main.cpp:35956` |
| `selfTest(QStringList&)` | 26786 | Harness checks: tokenization, wylie detection, harvest-case rule, edition collation, MEM-2 null-witness guard | `main.cpp:38267` |
| `openFile(const QString&)` | 26841 | Reads a file into the Tibetan editor, sets `docFile_`, records session key, loads | Open button; `restoreSession()` |
| `restoreSession()` | 26856 | Reopens last session's file if readable | `main.cpp:40554` |
| `linksPath()` | 26869 | Per-document TSV path under `<root>/library/links/`; empty for untitled | `loadTexts()`, `saveLinks()` |
| `tokEwts(const std::string&)` | 26874 | Token → EWTS (identity for wylie docs, `acipToEwts` otherwise) | `makeLink()`, `importHyp()`, `selfTest()` |
| `loadTexts()` | 26879 | Detects wylie, builds the overlay doc, re-renders fixed-layout text with token offsets, makes editors read-only, reloads persisted links, resets selection | Load button, `openFile()`, `importHyp()`, `selfTest()` |
| `onTibClick()` | 26951 | Caret → token → cycling span selection; enables Link | `tib_` cursorPositionChanged |
| `linkColor(bool)` static | 26971 | Blue for plain link, green for nested | `repaint_()` |
| `repaint_()` | 26975 | Rebuilds both editors' extraSelections (offsets clamped to the live documents) and the links-list HTML | most mutating paths |
| `makeLink()` | 27013 | Builds a Link from the token span + English selection, saves, repaints | Link button, space shortcut |
| `importHyp()` | 27046 | `.hyp` import with honest widen/skip accounting | Import button |
| `saveLinks()` | 27152 | Writes the per-document TSV with a provenance header; on stream failure removes the partial file and warns (WP-4) | `makeLink()`, `importHyp()`, delete paths |
| `exportPairs()` | 27181 | PENDING candidate-pairs TSV export via `writeAllOrWarn` | Export button |
| `compareEditions()` | 27216 | Two-witness collation dialog + apparatus save | Compare button |
| `struct Link` | 27863 (26862 region) | Token span, wylie, English char range, English text | throughout |

## 4. Dependencies

**Reads (disk)**
- `<root>/library` — default folder for the Open dialog.
- `<root>/library/links/<basename>.tsv` — per-document persisted links (read in `loadTexts()`, written in `saveLinks()`).
- Arbitrary user-chosen files: ACIP/wylie text, `.hyp` files, and two collation witnesses (`*.txt *.act *.acip`).

**Writes (disk)**
- `<root>/library/links/<basename>.tsv` (directory created with `QDir().mkpath`).
- `<root>/data/candidate_alignments/` — created unconditionally by `exportPairs()` before the save dialog; the export TSV itself goes wherever the user chooses.
- Apparatus Markdown at a user-chosen path.

**Network**: none. No `QNetworkAccessManager`, `TimedNam`, `QDesktopServices`, or `QProcess` anywhere in the class.

**QSettings** (via `sess::`, org `ALL` / app `TranslationTool`, suppressed under `g_harnessRun`)
- `sess/align/lastFile` — written by `openFile()` (26852), read by `restoreSession()` (26857).
- Indirectly, the shared file-dialog folder memory `sess/dlg/<caption>` and `sess/dlg/last` via `safeGetOpenFileName` / `safeGetSaveFileName` (main.cpp:3302–3313).

**allcore**: `Spine`, `HeadwordIndex`, `buildOverlay`, `looksLikeWylie`, `acipToEwts`, `parseHypFile`. Shared UI helpers: `editionDiffHtml` (2710), `streamWriteOk` (2994), `writeAllOrWarn` (3008), `saveOrWarn`, `warnOpenFail`, `warnWriteFail`, `OverlayPane::wylieDocToAcip`.

**Relevant ctest suites**: `hyp_smoke` (core/CMakeLists.txt:153) for the `.hyp` parser. No ctest suite exercises AlignPane itself; its only automated coverage is the in-app `selfTest()`.

## 5. Release questions

- **`.hyp` import silently discards existing links** (`27083`) and overwrites the saved TSV. Should it confirm first, or import into a new document? Decide before release.
- **Links on pasted ("untitled") text are never saved** and the user is never told. Is a warning or a "save as" prompt required?
- Manually verify the full click→cycle→select→space loop and the `del:` × chips — nine of twelve user-visible functions have no automated coverage at all.
- Verify `exportPairs()` on an empty link list: today the button does nothing at all, with no message.
- Confirm the collation message wording is acceptable given that an unreadable witness and an empty witness are indistinguishable to the user (`27231`).
- Confirm auto-reopening the last file at startup (`restoreSession`) is desired behavior for the input centers, and that a large or malformed remembered file cannot stall launch.
- Confirm the PENDING header on exported pairs satisfies the data project's never-auto-ingest rule (Inviolable rule 1) and that the 500k-character collation cap is the right ceiling for real volumes.
