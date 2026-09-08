# Release audit — function-by-function inventories (generated 2026-09-08)

One file per pane/class, produced by reading `app/main.cpp` (not from
memory). Each lists every user-visible control with its code anchor,
its automated coverage (selfTest check / ctest suite / NONE) and its
risks, then the internal functions, dependencies and release questions.
The walk with Adam adds a `Disposition` (SHIP / HIDE / FIX / LABEL) per
row. Plan: `docs/RELEASE_PLAN_2026-09.md`.

Counts are the auditors' own tallies; "NONE" = no automated test
exercises the control.

| Group | Pane | User-visible fns | NONE coverage | Headline risk |
|---|---|---|---|---|
| Read | [Overlay](OverlayPane.md) | 118 | 61 | reference-dictionary layer's own tooltip says "unlicensed compilations, never release data"; `proposeai:` lane untested; unbounded BDRC bulk fetches |
| Read | [Library](LibraryPane.md) | 30 | 20 | Import-data-release pipeline has zero coverage and deletes live files before the new ones land; nested event loops on GUI thread |
| Read | [Files](FilesPane.md) | 58 | 53 | remote names joined into local paths without traversal check; silent trash/mkdir/remove; blocking I/O |
| Read | [Scans](ScansPane.md) | 6 | 5 | whole-volume BDRC fetch unbounded; unguarded overlay pointer |
| Read | [Export](ExportPane.md) | 9 | 4 | selftest mutates real pecha QSettings; empty input is a silent no-op |
| Translate | [Draft](DraftPane.md) | 27 | 17 | AI back-check POSTs source+draft to api.anthropic.com, no consent, no coverage; undisclosed caps |
| Translate | [Manuscript](ManuscriptPane.md) | 24 | 18 | setManuscriptText destroys formatting without prompt; RTF via macOS-only textutil |
| Translate | [Apparatus](ApparatusPane.md) | 10 | 3 | bank-load failure silent; selftest only asserts count>400 |
| Translate | [Review](ReviewPane.md) | 11 | 7 | Open… fails silently; missing tables silently omit report sections |
| Translate | [Align](AlignPane.md) | 12 | 9 | importHyp overwrites the document's TSV with no confirm; untitled links never persist |
| Research | [Search](GoferPane.md) | 22 | 12 | dead `lookup:` row (never assigned); NEAR combiners untested at pane level; unescaped term into mdfind |
| Research | [Lookup](LookupPane.md) | 18 | 12 | Browse button dies after one click; `proposeai:` untested; six external hosts on click |
| Research | [Lookup popup ⌘D](LookupPopup.md) | 12 | 6 | ⌘D focus extraction untested; duplicated stepper handler |
| Research | [Sanskrit](SanskritPane.md) | 17 | 6 | IPA row unlabelled as provisional; Sanskrit OCR blocks 60 s, hard-coded tessdata path |
| Research | [Convert](ConvertPane.md) | 21 | 16 | Tibetan-year conversion proven only for 2015–2035 but shown without caveat; four Sanskrit converters cannot show the ⟨…⟩ failure marker; no selftest |
| Research | [Analysis](AnalysisPane.md) | 10 | 10 | paid, data-sending action with no consent gate, zero coverage, plaintext key file |
| Learn | [Trainer](TrainerPane.md) | 11 | 2 | silent wylie→unicode fallback (rule 3); "clicks join the deck" not implemented |
| Learn | [Drills](DrillsPane.md) | 14 | 7 | modes 3/4 write progress.db untested; silent ACIP fallback |
| Input | [Input](InputPane.md) | 31 | 22 | page-change autosave untested (data-loss point); CC BY-NC OCR model |
| Input | [OCR](ScanPane.md) | 24 | 19 | model download without checksum; batch OCR synchronous; save-crops ignores result |
| Catalog | [Catalog](CatalogPane.md) | 27 | 8 | SIDELINED per TODO yet shipped unconditionally; irreversible renames on shared material; weak admin bootstrap |
| Catalog | [Catalog tree](CatalogTree.md) | 11 | 8 | uncapped recursive walk on UI thread |
| Community | [Propose](ProposePane.md) | 14 | 12 | self-elevation checkbox; fields cleared even after failed save |
| Community | [Approval](ApprovalPane.md) | 15 | 10 | "Approve all in filter…" irreversible, untested; Defer is one-way |
| — | [Main window & menus](MainWindow_and_Menus.md) | 62 | 41 | Reset Settings clears the wrong QSettings domain; two contradictory Check-for-Updates; unredacted diagnostic report to Desktop |
| — | [Settings](SettingsDialog.md) | 14 | 11 | admin gate is a checkbox; updatesDir unvalidated |
| — | [Help](HelpWindow.md) | 17 | 13 | any http anchor opens externally; stale pane index |
| — | [Hunt palette](HuntPalette.md) | 31 | 22 | external URLs from index unvalidated; swallowed exceptions read as "nothing found" |
| — | [Ribbon](RibbonBar.md) | 13 | 13 | menu-bearing ribbon buttons excluded from all automated clicking; no overflow |

Cross-cutting (from the auditors):
- **Two panes send text to api.anthropic.com** (Analysis, Draft back-check) with no consent gate and no coverage; the harness forces the key empty, so no AI path is ever tested. Key is a plaintext file, not Keychain.
- **Rule 3 violations in three places** (Trainer, Drills, Draft converters): failed `wylieToUnicode` silently falls back to roman text instead of flagging.
- **`proposeai:` link scheme** (Overlay, Lookup, popup) moves AI-drafted English toward the authority queue with zero tests — rule 1 exposure.
- **Licensing**: reference.db layers labelled "unlicensed compilations" in-app; BDRC Woodblock OCR model is CC BY-NC; 24 data layers TERMS OPEN in the SBOM.
- **Update channel**: View → Check for Updates scans for `ALL-Translation-Tool-<v>.dmg`, a filename the press no longer produces, so it reports "up to date" forever.
