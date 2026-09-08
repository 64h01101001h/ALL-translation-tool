# ReviewPane — release audit

`ReviewPane` (app/main.cpp:26272–26684) is the translator-oversight surface: the user pastes (or opens) the Tibetan source in ACIP on the left and the finished English draft on the right, presses **Run review**, and gets an HTML "Reviewer's report" listing register-sensitive terms, register spreads from the course alignment evidence, applicable published footnotes, honorific (zhe sa) terms, renderings resting on PROVISIONAL-tier glosses, possible collapsed distinctions, unmatched established terms, and sense-spread attention. Nothing is auto-corrected — the banner and the report header both say flags are attention guidance, not verdicts. It is a top-level tab of the main window: constructed at app/main.cpp:35954 and added as `tabs.addTab(reviewPane, "Review")` at 35955, sitting between "Manuscript" and "Align" in the single flat tab bar; the user reaches it by clicking the **Review** tab. Its `selfTest()` is invoked from the driver's self-test block at app/main.cpp:38266, between `manuscriptPane->selfTest` and `alignPane->selfTest`.

## User-visible functions

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Open source…** ribbon button (REVIEW group, big, "page" icon) | Opens a file dialog (`Text (*.txt *.act *.inc *.md);;All files (*)`), reads the whole file and sets it as the plain text of the source editor | ctor lambda `openInto` + `connect(openSrc,…)`, main.cpp:26310–26322 | NONE (selfTest sets `src_` text programmatically; never exercises the dialog path) | Disk read. `if (qf.open(ReadOnly))` with **no else** — an unreadable/locked file silently leaves the editor unchanged with no message to the user (contrast the repo's own `warnWriteFail` "never fail silently" rule at main.cpp:2866). No size cap: a huge file is loaded whole into memory and into the QSettings-backed session store (capped at 256 KB there, so oversized text is silently *not* persisted). Bytes are assumed UTF-8 (`QString::fromUtf8`), no encoding detection or replacement-char warning. Under `g_harnessRun`, `safeGetOpenFileName` (main.cpp:3356) returns the stub/empty path, so this button is inert in harness mode. |
| **Open draft…** ribbon button (REVIEW group, big, "page" icon) | Same as above, targeting the draft editor | ctor, main.cpp:26312, 26320–26322 | NONE | Same risks as Open source…. |
| **Run review** ribbon button (REVIEW group, big, "check" icon) | Calls `review()`: runs `allcore::checkTerminology` over source+draft and renders the report | ctor, main.cpp:26324; `review()` main.cpp:26397 | selfTest checks (all via direct `review()` calls, not the button): "register-sensitive term raises the register warning"; "unmatched established term is surfaced"; "an aligned register spread is surfaced for the reviewer (item 2)"; "the spread lists GMR's attested renderings with their counts (item 2)"; "the report says which attested rendering the draft chose (item 2)"; "spreads are flags, never verdicts (item 2)"; "honorific source term raises the respect advisory"; "unmatched auto-aligned term is surfaced"; "an unmatched auto-aligned gloss is tier-labeled PROVISIONAL, not attributed to HGM". Core engine: ctest `terminology_smoke` (core/CMakeLists.txt:144), `register_smoke` (core/CMakeLists.txt:302) | Runs synchronously on the GUI thread with a `QCoreApplication::processEvents()` after painting "reviewing…" — a long source freezes the UI and re-enters the event loop (re-entrancy: the user can press Run review again during that window; there is no busy guard). No cancel, no progress. The empty-input branch prints "both the source and the draft are needed" but does not say which. |
| **Source editor** (left `QPlainTextEdit`, placeholder "Tibetan source (ACIP)…") | Free text entry of the ACIP source; contents auto-restored/auto-saved to app settings | ctor main.cpp:26294–26296 | Indirectly, via every selfTest `setPlainText` | Persists user text to QSettings key `sess/review/source` (debounced 1.5 s, 256 KB cap — text over the cap is silently dropped, see `sess::remember`, main.cpp:3112). Suppressed when `g_harnessRun`. |
| **Draft editor** (right `QPlainTextEdit`, placeholder "The finished English draft…") | Free text entry of the English draft; same persistence | ctor main.cpp:26297–26299 | Indirectly, via selfTest | QSettings key `sess/review/draft`; same cap/debounce caveats. Confidential in-progress translation text is written to the settings file in plain text. |
| **Splitter drag** (source ⟷ draft, `QSplitter(Qt::Horizontal)`) | Resizes the two editors | ctor main.cpp:26293, 26300–26302 | NONE | Splitter sizes are **not** remembered across sessions (no `sess::remember` for the splitter, unlike the text). |
| **Report browser** (`QTextBrowser`, bottom) | Displays the placeholder italic explainer and then the generated HTML report; standard QTextBrowser selection/copy and scrolling | ctor main.cpp:26303–26310; content built in `review()` main.cpp:26537–26680 | Report text asserted by the selfTest checks listed above (`toPlainText()` / `toHtml()`) | **No `anchorClicked` connection and no links are emitted** — the report is read-only text; terms are not clickable through to Lookup/Overlay/corpus, which is a plausible reviewer expectation gap. No context-menu customisation (default QTextBrowser menu only), no export/save/print of the report, no copy-all button. |
| Keyboard shortcuts | none defined in the class | — | NONE | No `setShortcut` anywhere in 26272–26684; Run review is mouse-only (no Return/Cmd-R accelerator). |
| Tooltips | none defined in the class | — | NONE | No `setToolTip` in the range; the three ribbon buttons rely on their labels alone. Accessibility: no accessible names/descriptions set. |
| Drag & drop / double-click / context menu / custom link scheme | none implemented | — | NONE | No `dropEvent`, `itemDoubleClicked`, `customContextMenu`, or `addAction` in the class. Dropping a file on the pane does nothing (the two Open… buttons are the only import route). |

## Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `ReviewPane(allcore::Spine&)` | 26274 | Builds the banner, the REVIEW ribbon group (3 buttons), the source/draft splitter and the report browser; wires the three button handlers; registers session persistence for both editors | app/main.cpp:35954 (main window tab construction) |
| `selfTest(QStringList& log)` | 26327 | Drives `review()` over five scripted source/draft pairs and asserts nine properties of the resulting report; clears both editors afterwards; returns the failure count | app/main.cpp:38266 (driver self-test mode) |
| `review()` | 26397 | The whole engine call and report render: `allcore::checkTerminology`, honorific scan against `g_honorifics`, footnote scan against `g_appNotes`, per-term register/provisional/unmatched/sense-spread sections, alignment register spreads from `g_alignEvidence`, shared-English collapse section, then `report_->setHtml` | `Run review` handler (26324) and `selfTest` |
| `openInto` (ctor-local lambda, not a member) | 26310 | File-dialog + read + `setPlainText` helper shared by both Open buttons | Open source… / Open draft… handlers |
| Members: `spine_`, `index_` (`allcore::HeadwordIndex`), `src_`, `draft_`, `report_` | 26680–26684 | Spine reference, headword index built at construction, and the three widgets | throughout |

No other member functions exist; the class has no destructor, no event overrides, and no signals/slots beyond the three ctor `connect`s.

## Dependencies

**Core / data**
- `allcore::Spine&` (borrowed, not owned) and `allcore::HeadwordIndex index_` built from it in the ctor — the index construction happens eagerly at app start-up for this pane.
- `allcore::checkTerminology(spine_, index_, source, draft)` — the report's backbone (`rep.terms`, `rep.shared`); ctest `terminology_smoke` runs it against `build/hgm_spine_v27_2.db`.
- `allcore::tokenizeDocument` and `allcore::tokenToEwts` — used twice, once for the honorific scan and again, independently, for the alignment-spread scan (the same tokenisation is performed twice per review).
- Global tables loaded elsewhere in main.cpp and merely *read* here: `g_honorifics` (main.cpp:320), `g_alignEvidence` (main.cpp:553), `g_appNotes` (main.cpp:919). Each is null-checked; if a table failed to load the corresponding report section simply never appears — **silently**, with nothing in the report saying that honorific/spread/footnote checking was skipped.
- Spine DB path and library/alignment data folders are resolved by the app, not by this pane; the pane opens no data file of its own.

**Files read** — only the user-chosen file in the two Open… handlers (`QFile`, read-only). No `QSaveFile`, no `QProcess`, no `QDesktopServices`, no writes to disk from this class.

**Network** — none. No `QNetworkAccessManager`, no `TimedNam`, no hosts contacted.

**QSettings** (organisation "ALL", application "TranslationTool", via `sess::`)
- read + written: `sess/review/source`, `sess/review/draft` (debounced 1.5 s, 256 KB cap, skipped entirely when `g_harnessRun`).
- indirectly, `safeGetOpenFileName` records/restores the last dialog directory via `dlgRemember`/`dlgStartDir` under the caption "Open file" — a caption shared with other panes, so the remembered directory is not Review-specific.

**TODO/FIXME/XXX/HACK markers in 26272–26684:** none. The comments in the range are dated backlog/audit notes ("backlog item 2 (Adam 2026-08-26)", "DATA-2 / house rule 1"), all describing work already implemented.

## Release questions

- The two **Open…** buttons swallow open failures (no else branch on `QFile::open`) — decide whether an unreadable file must raise the same one-voice warning the write path uses. Manual verify: point at a file with no read permission.
- **No selfTest coverage for the file-open path at all**, and `safeGetOpenFileName` is stubbed out under the harness — decide whether `g_openDialogStub` should be used to give Review the same scripted-open coverage other panes get.
- If `g_honorifics`, `g_alignEvidence` or `g_appNotes` fail to load, the report quietly omits those sections. Should the report state which checks did *not* run? A reviewer currently cannot tell "no honorifics found" from "honorific data missing".
- `review()` runs on the GUI thread with a `processEvents()` re-entry point and no busy guard. Manually verify a large source (a full text, not a passage) for freeze duration and for double-click re-entrancy on Run review.
- Draft and source text are persisted verbatim into the user's QSettings file (unencrypted, 256 KB). Confirm this is acceptable for in-progress client/unpublished translation work, and that silent truncation above 256 KB is acceptable.
- The report is a dead end: no links to Lookup/Overlay/corpus, no save/export/print, no shortcut for Run review. Decide whether any of these are release blockers for the oversight workflow.
- The alignment register-spread section is labelled "AI-matched, TENTATIVE" and caps at 6 spreads / unmatched at 12 / sense-spreads at 10, silently. Confirm the caps and the "…first 12" wording are honest enough (the header counts are un-capped, so counts and listings disagree by design).
- Confirm `terminology_smoke` and `register_smoke` are in the release ctest run and that `build/hgm_spine_v27_2.db` (the fixture `terminology_smoke` needs) is present — CMakeLists.txt:97 notes suites are auto-skipped when an untracked fixture is missing, which could hide a Review regression.
