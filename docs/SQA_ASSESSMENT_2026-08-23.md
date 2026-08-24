# TOTAL SQA ASSESSMENT — Diamond Cutter Translation Tool

**Date:** 2026-08-23 · **Commit:** `a5f2e09` (2026-08-23 16:15:51 -0600) · **Version:** 1.0.0-rc.1
**Subject under test:** `app/main.cpp` (39,238 lines, md5 `43b8012319cc9e5673f7aab6244c746a`), `allcore`, `allocr`, the press, the gates, the docs, and the shipped artifacts.
**Baseline:** `docs/SQA_ASSESSMENT_2026-08-22.md` — commit `f752f65`, `app/main.cpp` 37,493 lines, md5 `30e3f0036feecbd43f2f503754edd579`, graded **C−**, 94 live findings.
**Scope:** the ENGINEERING PROCESS and the product's fitness as software. Distinct from `docs/BUG_BOUNTY_2026-08-22.md` (which hunted defects) and the UX audits (which judged surfaces).
**Method:** seven independent dimension audits, each followed by an adversarial verification pass whose job was to refute its own side's findings. Statuses below are those verifiers' verdicts. A claim a verifier refuted is struck and named; a claim a verifier corrected carries the corrected severity. On top of that, every load-bearing number in §0 was re-run by hand for this document; those commands are shown inline.

---

## 0. Movement against the 2026-08-22 baseline

The baseline's §4 lists **24 headline findings — 2 critical and 22 high** (two are dual-labelled: `FAIL-3 / TEST-3` and `TEST-6 / MEM-6`). The brief for this assessment says "22 findings: 2 critical, 20 high" and "17 remaining high-severity findings"; I am reporting the count I measured in the file, not the count in the brief. The remediation commit `a5f2e09` claims six tracks against "the 17 high-severity findings left after the two criticals" and self-reports **52 items NOT DONE**. Those are claims. Below is what I re-ran.

### 0.1 The two criticals

| # | Baseline finding | Status | Evidence I re-ran |
|---|---|---|---|
| 1 | **PERF-1** · `LibraryIndex::search` uncapped; 346,116 ms / 18.0 GB on three ordinary words | **CLOSED at the named site · REGRESSED elsewhere** | `core/src/libindex.cpp:358,387,392,401,419` now carry `kScanCap`, a `cut` flag and `LIMIT ?`. The PERF audit re-measured the exact baseline query at **487 ms / 43 MB** (from 346,116 ms / 18.0 GB). But the fix's own `ORDER BY l.file_id, l.line_no` (`libindex.cpp:384`) defeats FTS early termination: `PA` with `ORDER BY` = 2.158 s vs 0.064 s without — **34×**, verified twice by the PERF verifier. And the sibling `goferSearchFiles` was never touched: see live **PERF-1** below. |
| 2 | **FAIL-2** · `ProposalStore::save()` returns `true` after a partial write | **CLOSED at the named site · class OPEN at three siblings** | `core/src/proposals.cpp:219-221` is now `f.flush(); f.close(); return !f.fail();` with the incident recorded above it. `tools/test_shortwrite.sh` + `core/tests/shortwrite_probe.cpp` exist and were mutation-verified by the TEST audit (revert to `return true;` → `shortwrite: FAIL`, exit 1). But `core/src/glossary.cpp:47` and `core/src/dossier.cpp:83` still end `return (bool)f;` with no flush — see live **FAIL-1**. |

### 0.2 The 22 highs

| # | Baseline finding | Status | Evidence I re-ran |
|---|---|---|---|
| 3 | **DATA-3** · three surfaces print a cap as the corpus total | **CLOSED at the named sites · OPEN at a new one** | `spine_.corpusCount` is used in both card branches; `Tm84000::matchCount` exists at `core/src/tm84000.cpp:193-205` with an honest `-1`. But `GoferPane` prints its 60-row cap as the total in 2 of 3 modes — live **DATA-1**. |
| 4 | **DATA-2** · `TrainerPane` prints an untiered auto-aligned gloss under "HGM has:" | **CLOSED** | `hgmGlossPhrase` (`app/main.cpp:16986-16992`) binds label to tier so the caller cannot separate them; `equivalentsOwner` does the same for the possessive. This is the correct structural shape and is the template for the rest. |
| 5 | **DATA-4** · four export paths print unverified green success | **CLOSED at those four · 9 similar sites remain** | `writeAllOrWarn` and `streamWriteOk` now exist (4 call sites each); `warnWriteFail` at 12. `grep -cE '\.open\(\s*QIODevice::(WriteOnly\|Append\|ReadWrite)' app/main.cpp` → **73**, of which 28 are production and 21 discard the write-side return. |
| 6 | **DATA-5** · batch OCR counts a page written on the branch where the file never opened | **CLOSED** | Fixed in `fc461b6` ("DATA-5, actually applied this time — and how I came to claim it wasn't"). No audit reopened it. |
| 7 | **FAIL-1** · `catalogRosterSave()` returns TRUE having written zero bytes | **CLOSED** | `app/main.cpp:29699-29716` now writes temp, checks `write()==size && flush() && error()==NoError`, removes the temp on failure, renames atomically. Both callers (`32233`, `32256`) consume the bool. The FAIL audit calls it textbook. |
| 8 | **STATIC-1** · zero `[[nodiscard]]` against ~271 discarded status returns | **PARTIAL** | `grep -nE '^\s*\[\[nodiscard\]\]' core/include/allcore/*.h` → **3** annotations (`catalog_actions.h:55`, `dossier.h:34`, `proposals.h:91`) against **63** bool-returning declarations. `GlossaryStore::save` was missed, and its one discarding call site is a live unflagged write failure — live **STATIC-2**. |
| 9 | **FAIL-3 / TEST-3** · no test in the repository can detect a write failure (0 of 5 mutants killed) | **PARTIAL — the headline is refuted, the class is not closed** | Two real drills now exist: the RAM-disk short write (`tools/test_shortwrite.sh`) and an in-process `RLIMIT_FSIZE`/`EFBIG` export drill in `ApprovalPane::selfTest` (`app/main.cpp:28629-28675`) that asserts refusal, no green line, and removal of the truncated file. The FAIL verifier proved `warnWriteFail`'s body and `writeAllOrWarn`'s short-write branch each execute once under the battery. **But** `saveOrWarn`'s own two failure branches are dead (llvm-cov 0 executions) and mutating both to `return true` leaves the battery green. Baseline: 0 of 64 test files arranged a write failure. Now: 2. |
| 10 | **FAIL-4** · no injection seam for the save dialog | **PARTIAL — the seam shipped unwired** | `app/main.cpp:3004/3006/3012` declare the three stubs; `3018/3034/3047/3062` read them. `grep -rnE 'g_(save\|dir\|open)DialogStub\s*=' --include='*.cpp' --include='*.h' .` returns **nothing** — zero assignments in the repository. `docs/CLOSER.md:420` records FAIL-4 as fixed. The seam is functional (file-statics in the same TU as the selftest bodies); it simply has no consumer. |
| 11 | **TEST-2** · three bounty fixes revert at their defect sites with 72/72 green | **OPEN — and it recurred within 24 hours** | The TEST verifier reverted **four fixes committed today under critical/ship-blocker labels** (bounty #8 cap-as-total, PERF-1 cap disclosure, FAIL-2 ruling save, STATIC-1 META rollback) in one combined run: build rc=0, main.cpp recompiled and relinked, `ctest` "100% tests passed out of 76", `--selftest` "ALL PASS", 346 PASS / 0 FAIL. A liveness control (`tibcal_phugpa.cpp:292`) went red, so the green is not a stale-binary artifact. |
| 12 | **TEST-5** · the assertion-carrying selftest never traverses a signal, click or keystroke | **CLOSED** | Baseline measured **0**. I measured **8** event-driving calls inside `selfTest` bodies: `app/main.cpp:15420, 15465, 15468, 15488` (`GoferPane::selfTest`, starts 15397) and `32918, 32977, 32989, 33014` (`HuntPalette::selfTest`, starts 32884). They assert on rendered widget state after the event, including the lane-3 `[PROVISIONAL]` tier. |
| 13 | **TEST-6 / MEM-6** · UI at 47.67% functions while the library is at 98.92% | **OPEN — moved 1.7 points** | `app/main.cpp` now 58.22% lines / 49.38% functions / 42.24% branches (baseline 56.81 / 47.67 / 40.91). `core/src` 93.77% lines / 98.06% functions. `SettingsDialog` 0.0%, `ScanPane` 22.2%, and neither has a `selfTest`. |
| 14 | **MEM-1** · the memory-safety pin is tautological (`bool survived = true`) | **CLOSED** | `app/main.cpp:4337-4348` records the defect and now asserts three things the bug changes. `saBcadShapeOf` (`10397-10430`) grows its depth table dynamically with `.at()`. Restoring the fixed 64-slot table produced `Segmentation fault: 11` in `app_selftest` — the pin kills its mutant. |
| 15 | **MEM-2** · no sanitizer build in any gate | **PARTIAL** | `CMakeLists.txt` has `option(ALL_SANITIZE …)` and `tools/sanitized_battery.sh` exists. `grep -rn 'sanitized_battery\|ALL_SANITIZE' tools/*.sh tools/*.py` finds **only the script itself** — no press step, no ctest suite, no CI. `docs/STEWARDSHIP.md` records the sanitized run "at 73 suites" in the same commit that made it 78. |
| 16 | **PERF-2** · index update blocks the GUI for 196 s with no progress or cancel | **PARTIAL** | `updateIndex()` now has a modal `QProgressDialog` with per-file progress and a working Stop; `core/tests/libindex_cancel_smoke.cpp` pins cancel, exact file set, monotonic progress and cancel-then-resume byte equivalence. Still on the GUI thread. New: a cancelled **refold** makes zero progress while `app/main.cpp:24587` promises "the files already done are not redone" — live **PERF-6**. |
| 17 | **PERF-5** · one checkbox freezes the GUI for 5.65 s | **PARTIAL** | `ensureSegmenter` (`app/main.cpp:14485-14500`) now has `QProgressDialog` + "Stop" + `setOverrideCursor(Qt::WaitCursor)`. Still ~5 s and still fires on document open rather than on the tick. Its twin `tm84000()` (5,624 ms / 590 MB) got nothing — live **PERF-5**. |
| 18 | **PERF-3** · T4 perf floors pin the four cheapest operations, 95×–1200× headroom | **PARTIAL** | Budgets retightened: `kLookupMax=25, kCorpusMax=70, kUniMax=30, kPronMax=20` (`app/main.cpp:37811`), measured headroom now **4×–21×** rather than 95×–1200×. The two operations that dominate a session — document open and library search at real arity — are still unpinned, and both Overlay perf guards are fixture-dead. |
| 19 | **BUILD-7** · the battery cannot run from a clean checkout; two suites pass vacuously | **CLOSED** | Configure prints "45/78 suites can run here; 33 will SKIP". `ctest -N -L fixture` → **37**; `ctest -N -LE fixture` → **41**. A fixture-less suite reports `***Skipped` and the press counts skips. `no_vacuous_pass` holds the three formerly-vacuous suites red. |
| 20 | **BUILD-4** · L2 is a keyword match that never opens the payload | **CLOSED** | `tools/manifest_check.py` opens the stage and walks 82 manifest rows. Its own selftest: `python3 tools/manifest_check.py --selftest .` → **"manifest selftest: 11 checks passed"**. Run against the shipped image it names 20 forbidden paths with byte counts. |
| 21 | **BUILD-5** · payload the notices call "NOT redistributed" is redistributed | **PARTIAL — fixed in the repo, live in the artifact** | The pruning and the gate landed at 15:55-16:15; the DMG was cut at 13:21. `du -sh '/Applications/Diamond Cutter Translation Tool/Diamond Cutter Tool Data/data/extracted/thl_dege_records'` → **19M**, still installed. Live **BUILD-18**. |
| 22 | **BUILD-2** · notices name 6 of ~32 upstream projects; no LGPL/GPL text; no LICENSE | **PARTIAL** | `tools/bundle_licenses.py` maps **165 Mach-O objects → 35 projects, 0 unmapped**, and fails the press on any it cannot attribute. A root `LICENSE` exists. But `ls docs/licenses/texts/` → **`README.md` only**, and a team press (the only mode that can currently build) exits 0 with `LICENSE_TEXT_MISSING` for qt (26 objects, LGPL-3.0-only), qtimageformats and qtvirtualkeyboard. |
| 23 | **BUILD-6** · zero dependency pinning, no CI | **PARTIAL** | `Brewfile` exists and its own header tells the reader to commit the lockfile. `ls Brewfile.lock.json` → **No such file**. `ls -d .github` → **No such file**. `docs/CI_REQUIREMENTS.md` exists and opens "**Status: not stood up.**" — and is already stale (it says "36 of 73 suites"; measured 41 of 78). |
| 24 | **BUILD-1** · arm64-only with the macOS minimum documented nowhere | **CLOSED (BUILD-1) · OPEN (BUILD-15)** | `plutil -p …/Info.plist` → `"LSMinimumSystemVersion" => "26.0"`, and the press prints `lipo -archs` / `otool minos` and refuses an empty value. The bundle identifier is still `com.appelegant.DiamondCutterTranslationTool` and `CFBundleShortVersionString` is `1.0` against a VERSION of `1.0.0-rc.1`. |

**Tally: 8 CLOSED · 14 PARTIAL or OPEN · 2 with a same-class regression at a sibling site (PERF-1, FAIL-2) · 1 regression introduced by a fix (the `ORDER BY`).** The self-reported "52 NOT DONE" is honest as far as it goes — every PARTIAL above is one the tracks themselves disclosed, and `docs/CLOSER.md` records "REMAINDER OPEN: 89 findings" rather than claiming closure. Nothing in the remediation record was found to overstate what was done, with one exception: `docs/SHIP.md:18` says in the present tense that the 23 `drop` rows "no longer reach a DMG", while the only DMG in existence holds 20 of them.

### 0.3 Did the grade move, and why

**C− → C.** One notch, and it must be said exactly what moved it, because **no dimension grade changed** — all seven verifier-corrected grades are identical to the baseline's (C, B, C, C, D, C, C).

What moved the overall grade is three measured firsts, none of which existed on 2026-08-22:

1. **The repository now contains a test that can detect a write failure.** Baseline: "Test files that arrange a write failure: 0 of 64." Now 2, and the RAM-disk one kills its mutant.
2. **The assertion-carrying selftest now drives real Qt events.** Baseline: "Clicks / key events / `QTest::` calls in the assertion-carrying selftest: 0." Now 8, at two panes, asserting rendered state.
3. **Two gates that open the payload rather than grep it.** `manifest_check.py` (11-check mutation-designed selftest, passing) and `bundle_licenses.py` (165 → 35, 0 unmapped, fatal on any unmapped binary).

Plus both named criticals closed at their named sites, and BUILD-7 and BUILD-4 fully closed.

What holds it at C rather than higher: **two live criticals of the same class as the two that were closed, at sibling sites, found within 24 hours** (`goferSearchFiles` never got `LibraryIndex::search`'s cap; `GlossaryStore`/`DossierStore`/`CommentStore` never got `ProposalStore`'s flush). The `ORDER BY` regression means the PERF-1 fix's net effect on the single-term path was a 34× slowdown that shipped green. Four fixes committed **today** under critical or ship-blocker labels revert with the whole battery green.

**Measurement changes to declare, because a grade that moved on a changed ruler has not moved:**

- `app/main.cpp` grew 37,493 → 39,238 lines (+4.7%), so per-file coverage and pin-density figures are not exactly comparable.
- Registered suites 72 → 78, so "72/72 green" and "78/78 green" are different statements about different sets.
- The mutation experiment this round used 21 mutations across 12 subsystems against the baseline's 13 code + 3 gate probes, and — importantly — **12 of the 21 were adversarially chosen at sites the auditor already believed unpinned**. The headline 38.1% survival is therefore a mixed sample and is not comparable to the baseline's 33% for `app/main.cpp`. The only unbiased arm is the seeded-random sample: **4 of 9 killed = 44%**, n=9, 95% CI roughly 14–79%. Quote that, with its interval, not 38.1%.
- The baseline measured `dist/stage`; this round measured the shipped DMG **and** the copy installed in `/Applications`. That is a wider scope and it is why BUILD-18 exists.

---

## 1. The verdict

**Overall grade: C**

Say the strong part first, because it is real and it is unusual. The remediation of 2026-08-22 was not cosmetic: I reverted to the source and re-ran it, and `ProposalStore::save` genuinely checks the stream after `close()`, `LibraryIndex::search` genuinely caps and discloses (346,116 ms / 18.0 GB → 487 ms / 43 MB on the identical query), `catalogRosterSave` writes temp-then-rename with every return consumed, the tautological memory pin now dies on its own mutant, and the payload gate opens the bundle and names 20 forbidden paths with byte counts. The project encodes rules as executable tests rather than prose, keeps a backlog that records its own failures (`CLOSER.md` #48 states plainly that the author's first write-failure test could not fail and was only caught by planting a mutant), and shipped the repository's first event-driven pins and first write-failure drill in the same 24 hours. `allcore` measures 93.77% lines / 98.06% functions under `llvm-cov`, and 200,000 hostile-input mutants across 38 engine entry points under ASan+UBSan produced zero reports.

The grade is C because the assurance machinery still cannot see the class of defect it was built for. Two live criticals are the **sibling sites** of the two that were just closed: `goferSearchFiles` — the branch a fresh install takes, because nothing builds the search index for a new user — reached 115,590 ms and 23.5 GB peak RSS on three ordinary Tibetan words with a Stop button wired to nothing; and `GlossaryStore`, `DossierStore` and `CommentStore` return `true` having landed 64 of 2,842 bytes, with zero entries surviving reload. Four fixes committed today under critical or ship-blocker labels can each be reverted with `ctest` at 76/76 and the app selftest at 346 PASS. The `-Wall` wall has been broken since 11:22 today, survived a 16:05 rebuild, and the constitution reports "all rules hold" over it — because no script in `tools/` greps for the word `warning:`. The 424 MB DMG and the 1.4 GB install both carry 61.9 MB of payload the project's own manifest marks `drop`.

**The baseline's central finding is unchanged: the rules are correct and they hold exactly where their author remembered them.** What is new, and what earns the notch, is that three of them now hold mechanically instead — the payload gate, the fixture gate and the short-write drill are machinery, not memory. Three out of roughly forty.

---

## 2. Grades by dimension

| Dimension | Grade | 2026-08-22 | One-line justification |
|---|---|---|---|
| Test strategy and real coverage | **C** | C | 78 suites in ~65 s with 93.77% `allcore` line coverage and one exemplary defect-site pin — against 13 of 21 mutations surviving, four same-day critical fixes revertable green, and `app/main.cpp` at 49.38% functions reachable by 2 of 78 suites. |
| Memory safety and undefined behaviour | **B** | B | Zero sanitizer reports across 78 suites, 8,100 gauntlet steps, 25 real library texts and 200,000 hostile mutants — against a heap-buffer-overflow in the OCR recogniser, a `SIGABRT` on an empty partner file, and a sanitizer gate with no trigger. |
| Static analysis and compiler strictness | **C** | C | 1 warning across 148 TUs with **0** `-Werror`, `-Wno-*`, `#pragma diagnostic` and `NOLINT` anywhere — against a broken wall nothing consumes, 3 of 63 `[[nodiscard]]`, no `.clang-tidy`, no analyser installed, no CI, and a constitution rule that can be deleted while the gate prints "all rules hold". |
| Failure paths and error handling | **C** | C | Two real write-failure drills now exist and one is mutation-proven, and `saveOrWarn`/`writeAllOrWarn`/`streamWriteOk`/`catalogRosterSave` are correct — against 21 of 28 production write sites discarding their return, three core stores judging the stream before flush, and 0 HTTP status reads in 39,238 lines. |
| Performance and responsiveness at real scale | **D** | D | Spine lookup medians 0.021 ms and `buildOverlay` is linear to 10 MB — against a default-path search at 115,590 ms / 23.5 GB, a 34× regression introduced by yesterday's fix, a 3.7 s document open whose two guards are both fixture-dead, and 0 memory assertions anywhere in the tree. |
| Build, release, dependencies, licensing | **C** | C | A payload manifest gate with a passing 11-check selftest, a per-binary licence enumerator with 0 unmapped objects, an honest fixture gate, and a byte-reproducible executable — against a shipped artifact 61.9 MB out of compliance, a suite count nothing asserts, no lockfile and no CI. |
| Data integrity, provenance, documentation accuracy | **C** | C | 11 of 11 reference layers name tier and licence at the render site and the AI layer is structurally gated on `hgm_gloss.empty()` — against a search pane printing its cap as the corpus total, a 2,000-segment silent recall cap, and 6 documents still quoting a corpus figure corrected yesterday. |

---

## 3. What is genuinely strong

Each item is a measurement, taken this session.

**3.1 The two named criticals really are fixed, and I checked the fix rather than the claim.** `core/src/proposals.cpp:219-221` reads `f.flush(); f.close(); return !f.fail();` under a comment recording the exact byte counts of both failures. `core/src/libindex.cpp` carries `kScanCap` at five sites plus a `cut` out-parameter. The PERF audit re-ran the baseline's own worst query and got 487 ms / 43 MB against 346,116 ms / 18.0 GB.

**3.2 The repository's first write-failure drill exists and kills its mutant.** `tools/test_shortwrite.sh` stages a 2 MB RAM disk, fills it, and proves `ProposalStore::save()` returns FALSE on a real short write: `full volume -> CLAIMED=FALSE BYTES=155648`. Reverting the fix flips it red with exit 1. Its header records that the author's *first* attempt could not fail, was mutation-tested, and was discarded — that is the discipline, written down at its own expense.

**3.3 A second, in-process write-failure drill covers an app-level path.** `app/main.cpp:28629-28675` arms `RLIMIT_FSIZE=4096` with `SIGXFSZ` ignored to force a genuine `EFBIG` mid-record short write, runs the real approved-candidates export, and asserts three things at once: the export returns `-1`, the green success-string count did **not** increase, and the truncated file was removed. House rule 4 is mechanically enforced on that path today.

**3.4 The selftest now drives real Qt events.** Baseline: zero. I measured 8 event-driving calls inside `selfTest` bodies — `app/main.cpp:15420, 15465, 15468, 15488` and `32918, 32977, 32989, 33014` — asserting on `results_->toHtml()` and `list_->item(i)->text()` after the click or the Return. `HuntPalette`'s lane-3 pin asserts `[PROVISIONAL]` present on the auto-aligned row and absent on the glossary row, at the defect site, through the keystroke.

**3.5 The payload gate opens the payload.** `python3 tools/manifest_check.py --selftest .` → *"manifest selftest: 11 checks passed — the gate fails on every kind of unmanifested payload, on team payload in a market press and on unresolved terms, the README stamp rejects the pre-fix README, and the manifest matches this repo."* Run against the shipped DMG it names all 20 forbidden paths with byte counts and reasons. This replaced a keyword match that passed 102 MB on the substring "84000".

**3.6 Every shipped binary is now attributed.** `tools/bundle_licenses.py` maps **165 Mach-O objects to 35 upstream projects with 0 unmapped**, from local Homebrew metadata with no network, and hard-fails the press on any object it cannot attribute. The notices went from 6 named components to 33 rows.

**3.7 The fixture gate turns an unrunnable suite into an honest SKIP.** Configure prints "45/78 suites can run here; 33 will SKIP for want of untracked fixtures". `ctest -N -L fixture` → 37; `ctest -N -LE fixture` → 41. `no_vacuous_pass` holds the three formerly-vacuous suites red when handed a missing path. The corrosive skipping-as-passing pattern the baseline found is closed.

**3.8 `allcore` is covered, and the oracles kill.** 13,156 lines / 820 missed = **93.77%**; 775 functions / 15 missed = **98.06%**; branches 79.15%. Mutations were caught by `engines_battery` (pron_engine voiceless set), `gofer_smoke` (NEAR window +3), `phugpa_smoke` (a calendar boundary), `botok_tok_smoke` (a syllable-count guard) and `tibexport_smoke` (a loop bound). These are diffs against canonical references at real data scale, not shape checks.

**3.9 The tree is clean under ASan+UBSan everywhere it could be driven.** Full build in 3m21s with one compiler warning. 76 of 78 suites green under sanitizers in 100.28 s with **zero** sanitizer output; 343 selftest pins, 22 pane sweeps, 8,100 gauntlet steps, `--survey`, 25 real library texts through the document pipeline including all three deep-outline files from bounty #10, and 200,000 hostile-input mutants across 38 engine entry points — **0 reports from all of it**.

**3.10 The wall is unbought.** `grep -rn "Werror"` → nothing. `#pragma GCC/clang diagnostic` → 0. `NOLINT` → 0. `-Wno-*` → 0 in any build file. `grep -rn compile_options` finds exactly two directives in the whole tree: `-Wall` and the opt-in sanitizer block. Raising to `-Wextra -Wpedantic` costs 15 warnings at 8 sites, all verified false positives on aggregate initialisers; strict ISO `-std=c++20` adds zero.

**3.11 Compilation is deterministic.** Two independent configure+build runs of identical HEAD produced a byte-identical app executable (same sha256, 5,686,976 bytes) and byte-identical object files inside `liballcore.a`; the archives differ only in `ar`'s embedded mtimes.

**3.12 One structural honesty fix landed in the right shape.** `hgmGlossPhrase` (`app/main.cpp:16986-16992`) makes the authority label and the tier travel together, so "HGM has:" cannot be printed for an auto-aligned gloss by any caller. That is the shape the whole dimension needs; it exists in exactly one place, and it should be the template.

**3.13 `tools/verify.sh` is the best-written file in the tooling.** The build's own exit status decides (`if ! cmake --build … | tee`), the failure message reads "Nothing below this line was run. Do NOT quote a test count.", skips are counted into the success line, and the header documents four distinct disguises of the stale-binary hazard including the mtime-after-`cp` trap.

**3.14 The gate is fast enough to be run constantly.** 78 suites in 18.55 s at `-j8` (114.99 s serial); `--selftest` 14–26 s; `--gauntlet 1 300` 14.1 s. A full verification is about a minute on a 10-core machine.

---

## 4. Live findings, ranked by severity

**100 live findings: 2 critical · 13 high · 60 medium · 25 low.** Verification statuses: **confirmed** (a verifier reproduced it and could not refute it), **downgraded** (reproduced, a claim was overstated, corrected severity shown), **already-known** (real but a restatement of an open baseline item), **reported** (measured once by its dimension audit, not selected for adversarial verification — treat as one measurement, not two).

### CRITICAL

---

#### PERF-1 · critical · confirmed · The unindexed library search — the DEFAULT path on a fresh install — never got `LibraryIndex::search`'s cap

**Location:** `core/src/gofer.cpp:68` (`goferSearchFiles`, NEAR case ~line 128); called from `app/main.cpp:15843`; the default search folder is added checked at `app/main.cpp:15550-15553`.

**Evidence.** The verifier built `allcore` in an isolated tree from HEAD and drove the public entry point:

```
$ /usr/bin/time -l ./livescan library '"BSOD NAMS" NEAR/1000000 "SEMS CAN" NEAR/1000000 "SANGS RGYAS"'
live q=...   115589.7 ms  hits=401  peakRSS=22431 MB
     115.60 real  84.57 user  21.10 sys
     23520706560  maximum resident set size
     32715119920  peak memory footprint
```

`Node::NEAR` in `gofer.cpp` is a bare nested cross product with no cap of any kind, and `limit` is first consulted after `eval()` returns. Reachability was traced, not assumed: `app/main.cpp:15836` takes the indexed branch only `if (QFileInfo::exists(dir + "/.index.db"))`; the index is built **only** by the button (`grep -n "updateIndex()" app/main.cpp` → the sole call site is `22451`); and `tools/package_macos.sh:244-296` ships an empty `library/` (OCR models only). `buildQuery` (`app/main.cpp:15584`) compiles the pane's own second combiner option to `NEAR/1000000`. `stopped_` is tested only at `app/main.cpp:15818`, between directories, and the default configuration has exactly one.

The verifier also found a second defect inside the same function that the audit only recorded in a metrics line: `gofer.cpp:97` breaks at `files.size() >= 4000`, while `find library -type f` with the same extension set returns **8,988** — 4,988 files are silently excluded from every unindexed search, and the pane's cap disclosure only fires on the 400-hit ceiling.

**Impact.** This is the first library search a new user runs. On the 16 GB laptops this ships to, 22.4 GB of transient allocation is a swap storm or a jetsam kill, not a stall. On a 64 GB machine macOS marks the app Not Responding for nearly two minutes with a lit, inert Stop. And a rare phrase in the 4,988 unread files returns "no matches" — a performance cap converted into a claim about the world, which is house rule 3 in its exact terms.

**Recommendation.** Give `goferSearchFiles` the treatment `LibraryIndex::search` already got: `kScanCap` on every node including the NEAR output, and a `truncated` out-parameter. Better, delete the duplicate evaluator and route both paths through one bounded implementation so they cannot diverge again. Then either build the index automatically after `installZipPath` or refuse the unindexed multi-term search with an offer to build it. Return the scanned-file count and disclose it.

**Effort:** medium.

---

#### FAIL-1 · critical · confirmed · `GlossaryStore`, `DossierStore` and `CommentStore` report success over a lost write — the verdict is taken before the buffer is flushed

**Location:** `core/src/glossary.cpp:47`, `core/src/dossier.cpp:83`, `core/src/comments.cpp:111`. Caller: `app/main.cpp:14419-14424`.

**Evidence.** The verifier compiled the repository's own unmodified store sources into a probe and drove the real APIs under `RLIMIT_FSIZE=64`:

```
== real allcore stores under RLIMIT_FSIZE=64 ==
GlossaryStore rows= 3  save()=TRUE  landed=64 of 271   entries_after_reload=0
GlossaryStore rows=10  save()=TRUE  landed=64 of 642   entries_after_reload=0
GlossaryStore rows=50  save()=TRUE  landed=64 of 2842  entries_after_reload=0
CommentStore  adds=10  all_returned_TRUE=TRUE          rows_after_reload=0
DossierStore  rows=6   save()=TRUE  landed=64          rows_after_reload=0
```

Root cause, isolated: the failbit does not appear until the buffer is pushed, and a small store never pushes it.

```
rows=  5  | glossary/dossier shape claimed=TRUE  | proposals shape claimed=FALSE | failbit after writes=FALSE flush=TRUE close=TRUE
rows=100  | glossary/dossier shape claimed=TRUE  | proposals shape claimed=FALSE | failbit after writes=FALSE flush=TRUE close=TRUE
rows=400  | glossary/dossier shape claimed=FALSE | proposals shape claimed=FALSE | failbit after writes=TRUE  flush=TRUE close=TRUE
```

I confirmed the source myself: `grep -n 'return (bool)f;' core/src/*.cpp` → `dossier.cpp:83`, `glossary.cpp:47`; `comments.cpp:111` is `if (!f) return false;` placed after the writes and before any flush. The caller is **correct** and still cannot help — `app/main.cpp:14421` does `if (!gs.save(…)) { QFile f(p); warnWriteFail(this, f, "The glossary"); }`, and the warning can never fire.

**This corrects a recorded mis-measurement.** The baseline (`docs/SQA_ASSESSMENT_2026-08-22.md:149-150`) probed these stores at 600 rows — above the stream buffer, the one regime where the shape works — recorded them as "`<-- honest`", and consequently ledger item 2 enumerated four stores to fix and excluded these three.

**Impact.** A per-text glossary is the artefact the Overlay labels "the translator's own — outranks the general dictionary here", and it is small, which is exactly the regime this shape cannot see. On a full disk, an ejected volume or a read-only share, the term is added in memory, `save()` returns TRUE, no modal appears, and the work is gone at next launch with no signal at any point. `CommentStore` is explicitly designed for a shared Dropbox folder (`comments.h:4-10`), which puts a disconnected share squarely in scope. Its header promises the record is "append-only; never rewritten".

**Recommendation.** Give all three the `proposals.cpp` ending: `f.flush(); f.close(); return !f.fail();`. Measurement says `flush()` is the load-bearing call; keep `close()` for symmetry. Better, delete the shape: route every store through one `writeAllOrFail(path, body)` in `allcore` so there is one place to be right. Write the drill **before** the fix and mutation-verify it — point `shortwrite_probe`'s technique at `GlossaryStore` with `RLIMIT_FSIZE` (cheaper than the RAM disk, runs in-process, and `app/main.cpp:28646` already does exactly this).

**Effort:** small.

---

### HIGH

---

#### DATA-1 · high · confirmed · The Search pane prints its 60-row display cap as the corpus total in 2 of its 3 combiner modes

**Location:** `app/main.cpp:15640-15659` (the fix and its comment); `core/src/spine.cpp:318` (`return 0`); `app/main.cpp:15575-15589` (`buildQuery`).

**Evidence.** I reproduced this at the SQL layer myself:

```
$ sqlite3 build/hgm_spine_v27_2.db "SELECT COUNT(*) … WHERE corpus_fts MATCH '<q>';"
"sems can"                             1566
"sems can" OR "byang chub"             3142
"sems can" NEAR/1000000 "byang chub"   Error: stepping, fts5: syntax error near "/"
"sems can" NEAR/3 "byang chub"         Error: stepping, fts5: syntax error near "/"
```

`buildQuery` emits FTS3/4 infix `NEAR/N` grammar and hands it to `Spine::corpusCount`, which binds it as an FTS5 `MATCH`. FTS5 rejects it at `step()`, not `prepare()`, so nothing throws and the function falls through to `return 0` — never the `-1` its own comment promises. Rendered header becomes `HGM aligned corpus (all courses) — 60 hit(s)`. The verifier added two things the audit missed: the pane renders only 20 of the 60 rows (`if (++shown > 20) break;` at `15663`), and the grand-total footer's cap notice is gated on `indexCapped`, which is declared at `15815` — *after* the corpus branch — and set only by the library-index branch, so a corpus search prints "60 hit(s) shown" with no notice at all.

Unpinned by construction: all four `combiner_->setCurrentIndex` calls in the file are index 0, and the one corpus assertion is `check(results_->toPlainText().contains("hit"), …)` — a substring pin that passes on "60 hit(s)".

**Impact.** A translator running the pane's advertised multi-term search is told the master's aligned corpus contains 60 hits for a term that may have thousands, under the binding layer's own name, with no cap notice in header or footer. Commit `e4e179e` wired Enter into all eight term boxes yesterday, so multi-term queries are now the pane's fast path. The correct twin is 280 lines below at `15942-15947`.

**Recommendation.** Make `corpusCount` honest first: `long n = -1; if (step==SQLITE_ROW) n = …; return n;` — matching `Tm84000::matchCount`, which already does this — so an unanswerable query degrades to "total unknown" instead of a zero it did not measure. Then translate the Gofer AST to FTS5 for the count (`NEAR("a" "b", N)`). Track the rendered count separately from the fetched count and print all three numbers. Pin by mutation: assert a 2-term AND header contains "of" and a number greater than the row count, revert `return 0`, confirm red.

**Effort:** small.

---

#### DATA-2 · high · confirmed · The Gofer engine silently searches only the first 2,000 attestations of any term

**Location:** `core/src/gofer.cpp:29-34`.

**Evidence.** `for (const auto& seg : spine.corpusSearch(q, course, 2000))` in the TERM leaf, before any OR/NEAR join, under a comment calling it a "generous candidate limit". I confirmed the scale myself: `SELECT COUNT(*) FROM entries WHERE CAST(corpus_n_segments AS INTEGER) > 2000 AND wylie NOT LIKE '%~~%'` → **113**. The verifier re-implemented `evalNode` with a parameterised cap and measured the join loss:

```
termA    termB          NEAR/n |  A@2000  A@uncap | join@2000 join@uncap | lost
chos     sangs rgyas         3 |    2000     6273 |      833      2720 | 1887 (69.4%)
phyir    stong pa nyid       3 |    2000     7194 |      107       411 |  304 (74.0%)
pa       bden pa             3 |    2000    36770 |      305      2866 | 2561 (89.4%)
```

And — decisively — because `goferSearch` pages by `(course, seq_lo)` while the cap keeps the top 2,000 by FTS **rank**, the cap changes the first page, not only the tail:

```
chos  NEAR/3 sangs rgyas   : shipped page=60  true page=60  IDENTICAL rows=19  ROWS THE USER NEVER SEES=41
pa    NEAR/3 bden pa       : shipped page=60  true page=60  IDENTICAL rows=4   ROWS THE USER NEVER SEES=56
```

*Correction to the audit's evidence:* its top-8 headword table is not what its stated query returns. The pairs are individually correct but the real top 8 is `pa|26053, la|18839, dang|17009, de|15545, ba|15451, ma|13487, na|12600, du|12374` — which makes the worst case 7.6% recall, not the 31.9% claimed.

**Impact.** Unlike a display cap, this one cannot be worked around by scrolling — the evidence never enters the join. 41 to 56 of the 60 rows on screen are an artifact of the cap, and the same number of true rows are unreachable. Nothing in the API, the pane or the UI says so. `core/tests/gofer_smoke.cpp` exercises only rare phrases that never approach 2,000.

**Recommendation.** Return the truncation upward: add `truncated` / `candidates_seen` to the result, set it when any TERM leaf hits 2,000, and have the pane say "this term has N attestations; the search examined the first 2,000 — narrow it". The corrected twin already exists in the sibling module: `core/tests/libindex_smoke.cpp:138` passes a `bool* cut`. Add a test that runs a NEAR query on `chos` and asserts the reported search space equals `corpusCount` or is explicitly disclosed.

**Effort:** medium.

---

#### MEM-1 · high · confirmed · `TextRecognizer::recognize` indexes the vocab table by the model's output dimension without comparing it to the charset it loaded — heap-buffer-overflow

**Location:** `ocr/src/recognize.cpp:331` (bind) and `:335` (first use); vocab built at `:272-275`; `vocabSize` computed at `:265-266` and used only to decide transposition.

**Evidence.** The verifier compiled the repository's own `recognize.cpp` with ASan and drove it against the real banked `BDRC_Woodblock OCRModel.onnx`:

```
(a) unmodified config (charset 83)      -> recognized, EXIT=0, no report
(b) same ONNX, charset truncated to 40:
==87230==ERROR: AddressSanitizer: heap-buffer-overflow  READ of size 1
    #3 allocr::TextRecognizer::recognize(...) const recognize.cpp:335
  0x6190000d83a7 is located 10319 bytes after 984-byte region
  allocated by ... vector<std::string>::reserve ... recognize.cpp:273
(c) model_config.json truncated to 738 of 1055 bytes (parser accepts it):
==87685==ERROR: AddressSanitizer: heap-buffer-overflow ... recognize.cpp:335
  0x61a00004e2a7 is located 10103 bytes after 1200-byte region
```

The code carries two notions of vocabulary size and bounds-checks against neither. Reachability is **stronger** than the audit claimed: beyond the hand-download path `ensureModels()` prints (`app/main.cpp:27473-27479`), `showOcrModelManager` (`app/main.cpp:485-548`) fetches `config.json` and the `.onnx` as two independent GETs, and `ocrDownloadOne` (`452-483`) discards the return of `f.write(r->readAll())` and validates only `QFileInfo(dest).size() > 0` — a short write leaves exactly construction (c) on disk. `tools/package_macos.sh:295-296` ships `library/ocr_models` into the DMG. `ocr/tests/ocr_smoke.cpp:296-316` exercises the config parser on two well-formed shapes and never pairs a real ONNX with a mismatched charset.

**Impact.** The read is ~10 KB out of bounds and what it reads is treated as a live `std::string` — compared at `:335`, appended at `:344`. When the wild bytes decode as a short string, they are **appended to the recognized text** and presented as OCR output: garbage surfaced as recognized Tibetan, which is house rule 2 on top of the UB.

**Recommendation.** Immediately after `V` is settled (`recognize.cpp:270-276`), refuse rather than guess: throw a named `runtime_error` ("model output layer has %lld classes, model_config.json charset has %zu") so `ensureModels()`'s existing catch turns it into the honest banner it already knows how to print. Add an `ocr_smoke` case pairing the banked ONNX with a truncated charset and run it under `ALL_SANITIZE`.

**Effort:** trivial.

---

#### MEM-2 · high · confirmed · "Compare with partner" aborts the whole process on a 0-byte partner file

**Location:** `app/main.cpp:26777` (read) and `:26794` (`diff_main`); throw at `app/thirdparty/diff_match_patch.cpp:196`.

**Evidence.** Three measurements, all reproduced by the verifier on both `offscreen` and the real `cocoa` platform:

```
$ ./nullq
QFile::readAll() on 0-byte file: isNull=1 isEmpty=1 size=0
QString::fromUtf8(that): isNull=1 isEmpty=1        (Qt 6.11.1)

$ ./emptypartner2      # QApplication + QPushButton::clicked slot + real event loop
entering event loop
libc++abi: terminating due to uncaught exception of type char const*
EXIT=134
```

`diff_match_patch.cpp:195-197` throws a bare `const char*`. `grep -n 'app.exec()' app/main.cpp` → one site, `39237`, with no enclosing try and no `QCoreApplication::notify` override. `compareWith`'s only guard is the >500,000-character size check at `26785`.

The correct twin is ninety lines away: `app/main.cpp:25725-25730` does `if (A.isEmpty() || B.isEmpty()) { QMessageBox::warning(… "One of the files was empty or unreadable."); return; }`. The trigger is an unconditional ribbon button (`app/main.cpp:26169`), and `docs/DEMO_TUESDAY.md:341` walks a presenter through pressing it. The existing pin calls `compareWith` with two non-null literals — the B12 pattern exactly.

**Impact.** `SIGABRT` of the whole application, no dialog, no log line, taking the translator's unsaved double-keyed input with it. The trigger is ordinary: a placeholder the partner has not typed into yet, a failed copy, a `touch`ed file.

**Recommendation.** Mirror the sibling: `if (partner.isEmpty()) { status_->setText("the partner file is empty — nothing to compare against"); return; }` at the top of `compareWith`. Independently wrap both `diff_main` call sites (`2475`, `26794`) in `catch(...)` — the vendored library throws `const char*` and `QString` from eight sites, none derived from `std::exception`. Pin it: call `compareWith(QString())` and assert the status line.

**Effort:** trivial.

---

#### STATIC-3 · high · confirmed · Constitution rule L3 can be lost outright and the gate still prints "all rules hold"

**Location:** `tools/constitution_check.py:213-224` (L3, no returncode guard) against `:186-197` (L2, which has it); `:208-209` vs `:224`.

**Evidence.** The verifier built a shadow tree of symlinks (one real file: a copy of `tools/`) and ran four mutations. The strongest needed no tampering with the checker at all — deleting the very file L3 exists to police:

```
$ mv <root>/docs/OVERSIGHT.md /elsewhere
$ python3 <root>/tools/reconcile_lists.py <root>
FileNotFoundError: … 'docs/OVERSIGHT.md'        exit=1
$ python3 <root>/tools/constitution_check.py <root>
constitution: all rules hold                    exit=0
```

Deleting `reconcile_lists.py`, making it exit 3 with unparsable output, and making it raise on import all produce the same green. The asymmetry is provable in one pair of commands: removing `manifest_check.py` turns the gate **RED** with a named failure ("FAIL L2 manifest_check.py --static exited 2 with no parsable finding"); removing `reconcile_lists.py` leaves it **GREEN**. Same file, 28 lines apart. L3's `except` branch appends to `notes`, and the `for x in notes: print(...)` loop sits at `:208-209` — fifteen lines *above* — so that message can never reach the terminal.

I confirmed the consumers: `core/CMakeLists.txt:456` registers it as a ctest suite and `tools/package_macos.sh:85` runs it as press gate 2a with `|| exit 8`. C2 cannot see it, because C2 is `if gate not in press` over three literal strings.

**Impact.** C2 exists to guarantee "a press that silently lost a gate is a failed press". L3 is a gate that can be silently lost. The failure mode it encodes — human-gated backlog items invisible on the page Adam reads — returns undetected the moment `reconcile_lists.py` is renamed, moved, breaks on an import, or changes its output shape.

**Recommendation.** Give L3 the guard L2 already has: after the subprocess, if `returncode != 0` and no `- ` line was parsed, append to `fails` naming the exit code and stderr; change the `except` branch from `notes.append` to `fails.append`; move the notes print below the L3 block. Then mutation-test it the way the verifier did. Generalise: every rule that shells out must assert the subprocess **ran**, not merely that it did not complain.

**Effort:** small.

---

#### TEST-1 · high · downgraded from critical · 13 of 21 deliberately re-injected defects survive the full battery

**Location:** whole battery.

**Evidence.** Two independent runs, in two separate clones, with a **liveness control** proving the harness can go red (`core/src/tibcal_phugpa.cpp:292`, `< 0` → `< -1`, produced "The following tests FAILED: 16 - phugpa_smoke"). Against that control, eleven mutations applied in two combined runs — both builds `rc=0` with `main.cpp` genuinely recompiled and relinked — left the battery at "100% tests passed out of 76" and "app selftest: ALL PASS (0 failure(s))", 346 PASS / 0 FAIL.

Killed (8): `pron_engine.cpp:291` voiceless set → `engines_battery`; `gofer.cpp:55,136` NEAR window → `gofer_smoke`; `main.cpp:11722` `attestedEnglish` equality → 2 selftest FAILs; `main.cpp:10552` fixed 64-slot table → `Segmentation fault: 11`; `tibexport.cpp:69`; `lattice.cpp:67`; `tibcal_phugpa.cpp:292`; `botok_tokenize.cpp:139`.

Survived (13): sa bcad jump position zeroed; Draft tier label removed; `saveOrWarn` always true; catalog META-clobber guard deleted; both OCR legality counters zeroed; `corpusTotal = segs.size()`; both `indexCapped` assignments removed; FAIL-2 ruling-save guard bypassed; and five boundary flips in `affixnorm.cpp:100`, `lattice.cpp:30`, `title_xlat.cpp:127`, `terminology.cpp:83`, `searchnorm.cpp:141` — each proved non-equivalent by a probe linked against pristine vs mutant `liballcore`.

**Read the number correctly.** 12 of the 21 were adversarially chosen at sites the auditor already believed unpinned. The only unbiased arm is the seeded-random sample: **4 of 9 killed = 44%**, n=9, 95% CI roughly 14–79%. Quote that with its interval; 38.1% to one decimal is not supported.

**Impact.** The green suite is not evidence that behaviour is preserved. Seven survivors sit on lines the battery executes hundreds of times and simply does not assert; six sit on branches the battery never reaches.

**Recommendation.** Adopt mutation testing as a standing gate. Keep a `tools/mutate.sh` that applies a named patch, rebuilds, runs the battery and **requires** a red result, and make "a pin proved by a mutation" a merge requirement for every new pin — the discipline `app/main.cpp:4430` already documents and that `payload_manifest` and the shortwrite drill already followed. Start with killing pins for the 13 survivors.

**Effort:** large.

---

#### TEST-3 · high · downgraded from critical · Four fixes shipped today as critical or ship-blocker have no pin — reverting each leaves the battery fully green

**Location:** `app/main.cpp:8491` (bounty #8), `app/main.cpp:15835/15847` (PERF-1), `app/main.cpp:28846` (FAIL-2), `core/src/catalog_actions.cpp:218-222` (STATIC-1).

**Evidence.** All four reverted in one combined run, build `rc=0`, 0 errors, `main.cpp` rebuilt and relinked → `ctest` "100% tests passed out of 76"; `--selftest` "ALL PASS (0 failure(s))", 346 PASS / 0 FAIL. Commit dates confirmed: `f7acf98` / `25ad21b` / `18c4926` are all 2026-08-23. `llvm-cov` on the instrumented build: `8491|11|`, `8492|11|`, `8493|0|` … `8499|0|` (the corpus card's print block never renders under the battery); `15847|0|`; `28846|0|` (the ruling path is never entered).

*Correction to the audit:* it says of M8 "line 8490 executes 11 times, so the card IS rendered — nothing asserts the number it prints." The print block executes zero times. M8 survives because of a coverage gap, not an assertion gap.

**Impact.** The remediation loop does not close. A fix can be written, reviewed, committed with a critical label, and silently regressed by the next refactor with the battery certifying the regression. Three of the four are honesty-rule violations.

**Recommendation.** Make "a pin that fails on the pre-fix code" a merge requirement, proved by mutation before commit. Retrofit these four first: assert the rendered count for a high-frequency term equals `corpusCount()`; assert the capped-result HTML contains the remainder; assert `rule()` refuses on an unwritable proposals directory; add a META-collision case to `catalog_actions_smoke`. The right pin shape already exists at `app/main.cpp:18637` (asserts the rendered concordance contains `QString::number(semsTotal)` and never prints "200 corpus hit").

**Effort:** medium.

---

#### PERF-3 · high · confirmed · `LibraryIndex::search` is bounded but still not limited: asking for 1 hit costs the same as 400, and yesterday's `ORDER BY` costs 34×

**Location:** `core/src/libindex.cpp:336-349` (TERM case), `limit` first consulted at `:453`; the `ORDER BY` at `:384`.

**Evidence.** Reproduced twice, warm, on an independent build:

```
warm pass 1: PA limit=1  1837.7 ms | PA limit=400 1842.3 ms
warm pass 2: PA limit=1  1832.7 ms | PA limit=400 1834.0 ms
             CHOS limit=1 428.1 ms | CHOS limit=400 429.5 ms
first (cold) touch: PA limit=1  8079.2 ms

$ sqlite3 … EXPLAIN QUERY PLAN … ORDER BY l.file_id,l.line_no LIMIT 200001
|--SCAN f VIRTUAL TABLE INDEX 0:M2
|--SEARCH l USING INTEGER PRIMARY KEY (rowid=?)
`--USE TEMP B-TREE FOR ORDER BY

with ORDER BY    -> 2.158 s   |   without -> 0.064 s   (34×)
SELECT count(*) FROM lines_fts WHERE MATCH '"PA"'  -> 7854758  0.237 s
the unlimited join (no LIMIT)                      -> 7854758  1.954 s   <- the real cost
```

The header comment (`core/include/allcore/libindex.h:57`) states the defect the fix was meant to remove — "asking for 1 hit cost the same as asking for 400" — and it is still true. The mechanism is not principally the temp B-tree: `ORDER BY` defeats early termination, so SQLite visits all 7.85 M matching rows through the rowid join before picking 200,001. The SQL sort is redundant for output order — `search()` already does its own `std::sort` at `libindex.cpp:441`; the `ORDER BY` only decides *which* 200,000 rows survive the cap.

**Impact.** Every single-term library search pays a 200,000-row scan it does not need. `PA` takes 1.9 s warm and up to 8.1 s cold to return 60 rows the FTS index can hand over in 15 ms. This is the operation three places in the product describe as answering "instantly".

**Recommendation.** Take the first `kScanCap` FTS rowids in rowid order (which *is* `(file, line)` order for a freshly built index) and sort the kept window in C++ — the code already sorts it. If a stable window across incremental updates is genuinely required, get it from a monotonic key rather than a full scan. Then make `limit` reach the scan.

**Effort:** medium.

---

#### PERF-4 · high · confirmed · Every library search runs on the GUI thread with a Stop that cannot fire; the pane's own eight fields build a 7.5 s query

**Location:** `app/main.cpp:15816-15850` (`GoferPane::find`), `15584` (`buildQuery`), `15222-15223` (Stop).

**Evidence.**

```
8 fields, "AND (same file)" -> NEAR/1000000 chain :  7512.2 ms  hits=60  trunc=1  peakRSS=38 MB
8 fields, "OR" (the DEFAULT combiner)            :  6922.7 ms  hits=60  trunc=1  peakRSS=36 MB
"PA" NEAR/3 "PA"                                 :  4209.5 ms
"sems can" NEAR/1000000 "bsod nams"              :   284.0 ms   <- the ONLY pinned shape (ceiling 6000 = 21×)
```

`LibraryIndex::search(query, limit, truncated*)` takes no stop token. `stopped_` is tested only at `app/main.cpp:15818`, between directories; `kFirstDirRow = 3` (`15532`) and the default adds exactly one directory. The Stop tooltip is honest about this — "Stop the search after the current folder" — which with one folder means never. The pin at `37840` covers only the two-term shape at `kIxMax = 6000 ms`; both eight-field queries the UI can build exceed it.

**Note for the ledger:** this is the same open defect as PERF-1 measured on the indexed branch. `docs/CLOSER.md:433` already carries "the rest of PERF-1 (merge join, stop_token, off-thread)". Do not count them as two independent items.

**Recommendation.** Move `LibraryIndex::search` off the GUI thread (a `QFutureWatcher`; the OCR pane already proves one worker thread is acceptable here), give it a `std::stop_token`, and wire the existing Stop to it. Retighten `kIxMax` toward the warm observation and add a pin at the pane's real maximum arity — eight fields, both combiners.

**Effort:** medium.

---

#### PERF-6 · high · confirmed · The installed index is stamped for a different fold generation, so the next "Update search index" is a 2.6-minute full reindex — and a cancelled refold makes zero progress while the dialog promises otherwise

**Location:** `core/src/libindex.cpp:172-179` (refold decision), `226-236` (delete+reinsert), `334-336` (the stamp skipped on cancel); `app/main.cpp:33818` (`configureLemmaFold` at startup), `app/main.cpp:24587` (the promise).

**Evidence.** Both halves of the trigger reproduce:

```
$ ./foldprobe data/extracted/verb_lemmas.tsv
before configure: globalLemmaFold()=0x0
after  configure: globalLemmaFold()=0x100c6c008  -> foldGen would be 1
$ sqlite3 'file:library/.index.db?mode=ro' 'PRAGMA application_id;'   -> 0
```

Same 1,011-file subtree, four passes:

```
PASS 1 fresh, fold ON : added=1011   28.98 real -> 518,340,608 B, lines_fts_data 38,586
PASS 2 incremental    : unchanged=1011  0.09 real
PASS 3 refold (OFF)   : updated=1011  36.50 real -> 626,593,792 B (+20.9%)
PASS 4 refold (ON)    : updated=1011  40.21 real -> 811,327,488 B (+29.5%), lines_fts_data 57,098
```

`grep -nE 'optimize|VACUUM|merge' core/src/libindex.cpp` → only a comment. `PRAGMA journal_mode` → `delete`. `df -g /` → 13 GB available.

The verifier found a defect the audit missed and proved it by experiment: because `libindex.cpp:334-336` skips the `application_id` stamp when the pass was cancelled, a cancelled refold makes **zero** progress — stop at 20 of 64 (`updated=20 canceled=1`), resume, and the resumed pass reports `updated=64`. `app/main.cpp:24587` tells the user "the files already done are not redone". House rule 4, in its exact terms.

**Impact.** The next click of "Update search index" on this machine — expecting the 0.09 s incremental pass — gets a 2.6-minute full rebuild that grows a 2.36 GB file toward 3.0 GB, on a volume with 13 GB free, with an interruption that silently discards all of it. Repeated refolds compound without bound because nothing ever optimizes the FTS index or vacuums the file.

**Recommendation.** Detect the refold **before** opening the transaction and say so in the dialog ("the word-fold changed; this rebuilds all 8,988 files, about 3 minutes"). Fix the promise at `app/main.cpp:24587` or make it true by stamping incremental progress. Run `INSERT INTO lines_fts(lines_fts) VALUES('optimize')` and a `VACUUM` after a full refold, or rebuild into a temp file and rename. Set `PRAGMA journal_mode=WAL`.

**Effort:** medium.

---

#### BUILD-18 · high · downgraded from critical · The shipped DMG and the installed copy violate the project's own payload manifest by 61.9 MB and carry no bundled-licence surface

**Location:** `dist/Diamond-Cutter-Translation-Tool-1.0.0-rc.1.dmg`; `/Applications/Diamond Cutter Translation Tool`; `tools/package_macos.sh:515` vs `:524`.

**Evidence.** I re-ran the artifact checks myself:

```
$ du -sh '/Applications/Diamond Cutter Translation Tool/Diamond Cutter Tool Data/data/extracted/thl_dege_records'
 19M
$ ls '…/Diamond Cutter Translation Tool.app/Contents/Resources/'
AppIcon.icns  qt.conf                                  # no licenses/, no BUNDLED_COMPONENTS.tsv
$ ls '…/Contents/PlugIns/platforminputcontexts/'
libqtvirtualkeyboardplugin.dylib                       # GPL-3.0-only AND Apache-2.0 AND BSD-3-Clause
$ ls docs/licenses/texts/
README.md
```

The verifier ran the gate against both artifacts: 20 `FAIL UNPRUNED` rows, exit 1, **61,866,439 bytes** (the audit's 61,934,399 is 0.11% high and unexplained), including 24.1 MB of raw THL catalogue records that the shipped `OPEN_SOURCE_NOTICES.md:42` declares "NOT redistributed". `--readme` returns 3 FAILs. DMG cut 13:21; the fixes committed 15:55–16:15.

Three things keep this at high rather than critical or medium:

1. **Downgraded from critical** because "the image the ALL/ACIP team is holding right now" is unmeasured — I found no record of distribution, only `docs/RELEASE_READINESS.md:47` listing "Distribute the DMG" as a future step. A licensing violation requires conveyance.
2. **The install cannot self-heal.** `package_macos.sh:515` installs the `.app` with `rsync -a --delete` (so a future press *does* remove the GPL plugin), but `:524` installs the data tree with `ditto` and no delete, so no press can remove staged payload from an installed data tree.
3. **The licence-text half is NOT fixed**, contrary to the audit's own "all of this is already fixed in the repository". A team press — the only mode that can currently build — exits **0** with `LICENSE_TEXT_MISSING` for qt (26 objects, LGPL-3.0-only), qtimageformats (8 objects) and qtvirtualkeyboard.

**Recommendation.** Drop the FSF texts into `docs/licenses/texts/` (`bundle_licenses.py` already picks up hand-supplied texts by SPDX id) and make `--require-texts` unconditional — five minutes, and it removes the sharpest licence question from every future image. Press once in team mode before anything else and diff the new stage against the old. Add a one-shot removal list so paths the manifest marks `drop` are deleted from the install target, or stage the data tree into a versioned subfolder. Correct `docs/SHIP.md:18`, which says in the present tense that those rows "no longer reach a DMG".

**Effort:** medium.

---

#### BUILD-19 · high · confirmed · Nothing asserts how many suites are registered: dropping one optional dependency silently deletes six process gates, and the About pane keeps quoting 78

**Location:** `tools/package_macos.sh:70` (greps the count, asserts nothing); `core/CMakeLists.txt` (six `add_test` under `if(Python3_FOUND)`); `ocr/CMakeLists.txt:12`.

**Evidence.**

```
$ cmake -S <repo> -B bnopy -DCMAKE_DISABLE_FIND_PACKAGE_Python3=TRUE
-- ALL battery: all 72 suites have their fixtures present
$ (cd bnopy && ctest -N | grep 'Total Tests')          -> Total Tests: 72
   (constitution, payload_manifest, fixture_gate, no_vacuous_pass, build_manifest, link_truth: all absent)

$ cmake -S <repo> -B bnoocr -DCMAKE_IGNORE_PATH="…onnxruntime;…opencv"
-- onnxruntime not found — skipping allocr (OCR stage 2)
-- ALL battery: all 77 suites have their fixtures present
$ ctest -N | grep 'Total Tests'  -> 77 ;  grep -c ocr_smoke -> 0
   ALL_HAVE_OCR undefined -> the entire Scan pane compiled out

$ grep -o 'ALL_TEST_BATTERIES[^ ]*' bnopy/…/flags.make   -> ALL_TEST_BATTERIES=\"78\"
$ sed -n '35474p' app/main.cpp   -> "Qt %1 · " ALL_TEST_BATTERIES " automated test batteries "
```

I confirmed the counts myself: `ctest -N` → 78; `add_test(` per CMakeLists → core 72, app 5, ocr 1 = 78. `grep -rnE 'Total Tests|tests passed|ALL_TEST_BATTERIES' tools/ cmake/` finds one line — `package_macos.sh:70`, which prints and asserts nothing.

**Impact.** This is the press-side form of the stale-binary lie. BUILD-7 closed the *skip* case; the *unregistered* case is wide open. On a machine where `find_package(Python3 …)` fails, configure says "all 72 suites have their fixtures present", ctest reports 100% passed, the press prints "72 tests passed", and six gates including the constitution and the payload manifest have vanished. Worse, `ALL_TEST_BATTERIES` is a static source count that stays 78 and is printed to the user in About — a degraded build tells its user it has 78 batteries when 72 ran. Without onnxruntime the app ships with the whole Scan pane compiled out and the press still prints PACKAGE COMPLETE.

**Recommendation.** Have the press parse ctest's "Total Tests: N" and fail unless it equals `ALL_TEST_BATTERIES` (which is computed correctly and stayed 78 in both degraded builds). Add `-DALL_RELEASE=ON` that turns the `if(Python3_FOUND)`, onnxruntime and Qt6Pdf soft-failures into `FATAL_ERROR`, and have the press pass it.

**Effort:** small.

---

#### BUILD-20 · high · confirmed · The payload gate can only see what is present: `--press` returns 0 on an empty stage, and the staging copies swallow their own failures

**Location:** `tools/manifest_check.py:177` (`walk_stage`); `tools/package_macos.sh:305-309`.

**Evidence.**

```
$ python3 tools/manifest_check.py --press <repo> ./emptystage team; echo EXIT=$?
payload manifest (team press): every staged path is manifested
EXIT=0

$ bash -c 'set -euo pipefail; d=/etc; [[ -d "$d" ]] && cp -R /nonexistent/src /tmp/dst && mkdir -p /tmp/zzz && mv /tmp/dst /tmp/zzz/final; echo "SURVIVED cp failure"'; echo exit=$?
cp: /nonexistent/src: No such file or directory
SURVIVED cp failure
exit=0
```

`package_macos.sh:305-309` is exactly that shape inside the `DATA_DIRS` loop, so a failing `cp -R` is exempt from `set -e` because it is not the final command in the AND-list. The manifest documents a row that has already stopped staging: `candidate_alignments` — "(directory absent from the current tree; the pre-manifest press named it, so the row is kept)". The only size check in step 6 is `teaching_moments_card.json`; the Das PDF and teaching-index copies carry a bare `|| true`.

**Impact.** The step the press calls "the last thing to touch the stage before the DMG is cut" proves only that nothing forbidden is present. A DMG missing an entire data directory — fonts, botok, spellcheck, 84000, the OCR models — passes the payload gate and passes the 8-second launch test (which asserts only `kill -0 $LPID` and never inspects what data was found), and the press prints PACKAGE COMPLETE.

**Recommendation.** Add a required-presence pass to `walk_stage`: every manifest row whose mode ships in this press and which is not glob-optional must be matched by at least one staged path, or the gate fails naming the row. Replace the `[[ -d … ]] && cp && mkdir && mv` chains with `if [[ -d … ]]; then … fi` so the copies are subject to `set -e`, and drop the bare `|| true`s.

**Effort:** small.

---

### MEDIUM (60) and LOW (25) — compact index

Full evidence for each is held in the dimension reports this document consolidates. Grouped by theme rather than by id, so the shape is visible.

**Write-path honesty (10 medium, 2 low).** `FAIL-2` — 9 production paths print an affirmative success string over a write whose bytes were never verified, measured landing 64 of 600,000 bytes on four shapes including `ManuscriptPane::save`, which also clears `dirty_` (already-known: baseline FAIL-5/FAIL-16, ledger item 8). `FAIL-3` — the FAIL-4 dialog seam has zero consumers. `FAIL-9` — 61 of 64 `mkpath` bools and 38 of 41 `QFile::remove` bools discarded, against 3 checked twins that carry the lesson in their comments. `FAIL-8` — the move-to-shelf rollback discards its own rename bool and asserts a disk state it did not verify, 350 lines from the honest twin. `FAIL-10` — the process gate encodes no I/O rule. `FAIL-11` — the short-write drill under-pins its own fix (the `close()`-only mutant survives) and enshrines a false lesson in two places. `FAIL-12` — an unwritable data root is already injectable through cwd, but every drill writes its fixture first, so a read-only root collapses the tests instead of testing the app. `STATIC-2` — `GlossaryStore::save` has no `[[nodiscard]]` and its one discarding call site (`app/main.cpp:14138`) is a live unflagged write failure; adding the attribute to a scratch header makes the already-configured `-Wall` produce the warning. `FAIL-13`, `MEM-8`, `STATIC-9` (low) — the OCR model download reports "✓ installed" on a short write and truncates a working model; two container reads whose bounds are emergent; a numeric parse failure silently substituting 0.

**Network handling (4 medium).** `FAIL-4` — the folio loop has no cancel connection and no transfer timeout; measured `transferTimeout() = 0` and a request still unfinished at 25 s (already-known: baseline FAIL-8, ledger item 16). `FAIL-6` — 42 of 43 JSON parses unchecked, so an HTTP 200 carrying HTML renders as "BDRC lists no scan volumes". `FAIL-7` — an AI analysis whose stream dies mid-report is rendered as finished, QC'd green and auto-saved. `PERF-10` — no transfer timeout on the folio fetch; measured 59,754 ms blocked on a black-holed address. `FAIL-5` (low) — HTTP status is never read: 401, 404, 429 and 500 all render as "BDRC unreachable".

**Test coverage and pin quality (8 medium, 4 low).** `TEST-5` — 93.8% line coverage buys 44% mutation detection; five random operator flips in 83–100%-covered files survive, all boundary-value gaps. `TEST-6` — `title_xlat_smoke` prints its own 21.3-point regression (90.2% → 68.9% mean fragment coverage) and passes, because its floor is 0.60. `TEST-7` — `app/main.cpp` is 62% of production code, reachable by 2 of 78 suites, with `SettingsDialog` at 0.0% and `ScanPane` at 22.2%, neither with a `selfTest`. `TEST-2` — `saveOrWarn`'s two failure branches are dead and mutating both to success leaves the battery green. `TEST-9` — plain `ctest` reports 76/76 over a build that failed to compile; `verify.sh` prevents it but nothing enforces it. `TEST-10` — `outline_smoke` covers `allcore::extractOutline` while the Overlay runs `extractSaBcad`, whose jump targets are unasserted. `TEST-11` — five `allcore` functions are tested but reachable from nowhere, including the entire backup **restore** path. `TEST-12` — two wall-clock perf pins passed and failed on an unchanged binary in one session. `TEST-4` (low) — the B13 anti-pattern persists at the bounty-#8 pin, though the report's "exists in exactly one place" is **refuted**: the defect-site pattern exists in at least three, two driving real Qt events. `TEST-8` (low) — 42% of 274 pins are substring-presence only, though 34% of that bucket carries an absence clause or a number, so the honest ceiling on genuinely-weak pins is 27.7%. `TEST-13`, `TEST-14` (low) — the gauntlet is a fossilised seed-1 walk adding 3.9 coverage points; seven pins are coupled to one machine's 2.2 GB index.

**Memory and UB (4 medium, 3 low).** `MEM-3` — `allcore::Progress` builds `std::string` from a possibly-NULL `sqlite3_column_text` at `progress.cpp:122/218`, reproducible SEGV, still open from the baseline and — notably — **absent from `CLOSER.md`, `FINDINGS.md` and `OVERSIGHT.md` entirely**. `MEM-4` — `botok::ChunkFramework`'s nine public int-index overloads have no bounds check. `MEM-5` — the sanitizer gate has no trigger and its recorded run was 5 suites stale in the commit that wrote it. `MEM-6` — the OCR worker reads a `QCheckBox` from a non-GUI thread while the correctly captured value sits unused. `MEM-7`, `MEM-9`, `STATIC-7` (low) — unbounded recursive descent in the Gofer parser (safe today only by a Qt default nobody chose); two Qt lifetime shapes tied to objects that outlive their capturing pane; `kMElem[-1]` guarded only by a year floor in a different function.

**Static analysis and strictness (6 medium, 4 low).** `STATIC-1` — the `-Wall` wall is broken (`app/main.cpp:27579`, a dead OCR deskew clamp the author documented as a guard rail), has been since 11:22, survived a 16:05 rebuild, and nothing in `tools/` greps for `warning:`; `docs/TOTAL_QUALITY.md:26` and `docs/MAINTAINERS.md:26` both assert the count is zero. `STATIC-4` — no analyser installed, no `.clang-tidy`, no CI, and `compile_commands.json` has zero consumers; when asked, clang-tidy returns 2,432 unique sites. `STATIC-5` — `ExportPane`'s only selftest assertion cannot fail and its FAIL branch is unreachable-by-crash. `STATIC-6` — `sanskrit.cpp:603` has a loop whose body always breaks, contradicting the anusvara contract its own header documents. `STATIC-8`, `STATIC-10`, `STATIC-11`, `BUILD-25` (low) — 629 ruff findings across the Python the gates are written in, 16 in the seven ctest-registered gates; `delete[]` of an uninitialized pointer in vendored `diff_match_patch` (unreachable); third-party includes not marked SYSTEM; two `__DATE__` sites making the About pane's "RELEASED" date the last compile of `main.cpp`.

**Performance (8 medium, 3 low).** `PERF-2` — opening a document blocks the GUI for 4.4 s warm / 9.3 s on the largest library text, and **both** selftest guards that cover it are fixture-dead while the suite prints ALL PASS. `PERF-5` — `tm84000()` is a 5,624 ms / 590 MB lazy build on the GUI thread inside a rendering path with no dialog, no cancel and no cursor. `PERF-7` — the unindexed search reads 4,000 of 8,988 files and the disclosure names the wrong cap. `PERF-8` — "answers instantly" survives at three sites including a success string printed unconditionally after a three-minute build. `PERF-9` — 446 MB before the user does anything, 2.11 GB on a 10 MB document, and **0 memory assertions anywhere in the tree**. `PERF-11` — six timing numbers in one suite with 4×–21× headroom; `ctest` has one time assertion in 78 suites; the constitution has none. `PERF-12` — the Search sweep clicks Find before typing, so the whole `find()` path has zero automated coverage. `PERF-13`, `PERF-14`, `PERF-15` (low) — 22 panes constructed eagerly (2.3–3.7 s, 446 MB); repeated full-library walks and an O(citations × 7,097) resolver inside `openFile`; `lineCount()` is a 2.44 s `COUNT(*)` run to print a number on the fast path.

**Build and release (12 medium, 3 low).** `BUILD-8` — a failing `codesign --verify` does not stop the press (proven by executing the idiom) and PACKAGE COMPLETE prints anyway. `BUILD-10` — C2 reports "all rules hold" against a press whose battery, visual and constitution gates are all disarmed (mutation-proven). `BUILD-21` — no lockfile has ever been generated, the press never runs `brew bundle check`, and `docs/CI_REQUIREMENTS.md` was already stale one day after it was written to close this. `BUILD-9` — `shot_diff.py` returns 0 when the untracked baseline is absent. `BUILD-23` — Qt's LGPL-3.0 text is not in the bundle and a team press does not fail on it. `BUILD-15` — the bundle identifier is still `com.appelegant.…` and `CFBundleShortVersionString` is `1.0`. `BUILD-22` — one tag, 108 commits behind HEAD, two distinct DMGs both named 1.0.0-rc.1, 2.0 GB in `dist/`. `BUILD-3` — the DMG is unsigned, Gatekeeper rejects the app, and the printed first-launch instruction is the pre-Sequoia bypass on a build whose floor is macOS 26. `BUILD-13` — the press is a deployment, so it cannot be run as a verification step. `BUILD-12` — the release gate validates the source JSON against itself; nothing cryptographically links the 249 MB database that ships to it. `BUILD-24` — `BUILD_MANIFEST.*` is written after the payload gate and has no manifest row. `BUILD-16` — the DMG is never opened or verified by the press; `hdiutil verify` costs 4 seconds. `BUILD-14`, `BUILD-17` (low) — docs send an operator to a flag that exits 2 and still say SQLite is vendored (it is the system library); six objects carry absolute `/opt/homebrew` install names, though **0 true dependency leaks** remain.

**Data and documentation (12 medium, 6 low).** `DATA-3` — nothing mechanically prevents the next surface from printing an untiered binding gloss; already-known (bounty systemic theme 2). `DATA-4` — the Library banner shipped today announces "8,985 texts installed"; I measured 1,463 of them are RTF META companions, so the true count is **7,522** (19.4% overstatement, and it is the line the demo leads with). `DATA-5` — the Search header says "showing 60" while 20 render and the footer says "60 hit(s) shown". `DATA-6` — the survey hides 771 of 786 unknown vocabulary forms with no remainder, twelve lines above a section that discloses its own. `DATA-7` — the 84000 duplicate notice under-reports on 34.1% of the layer (10,450 unreported suppressions). `DATA-8` — the teachings report shows 2 of up to 5 moments (82.9% of keys hold more) under a comment claiming "every". `DATA-9` — the Hunt palette prints `[PROVISIONAL]` on two lanes and the raw `[auto-aligned]` on the third, across 3,912 rows. `DATA-10` — the pecha batch lane clamps the preset to 3 while the dialog offers 5, four lines above the comment describing B13's fix. `DATA-11` — the four-layer view shows 1 of N attested renderings with no signal that others exist (26.5% of woodblock-length lines carry more than one). `DATA-12` — seven greens over 43 sites and four documents giving `#1E6B4E` four different meanings. `DATA-13` — 55 suites claimed against 78 actual, in four places including the demo script; and **6 documents still quote 35,601 corpus segments** against the shipped spine's 42,199 (I measured 8 files; `CLAUDE.md` carries the ruling and `DEVELOPMENT_CHRONICLE.md` is historical). `DATA-14` — zero accessible names app-wide, four mouse-only links including the author-search entry point, and colour as sole carrier at two sites `UX_AUDIT_2.md:134-136` certifies "verified by design". Low: the provisional tier is painted `#b00` at 18 sites while the manual calls it amber; 19 truncations cut mid-word against the code's own invariant; the two `USER_MANUAL` copies diverged from 58 to 78 lines in a day; four provenance columns are loaded into the process and unreachable from any surface; the footnote block caps at 2 with no count; the "[generated script]" mark on 75.1% of entries is the lowest-contrast ink on the card (3.29:1).

---

## 5. The defect-density baseline

Measured **2026-08-23** at commit **`a5f2e09`**, `app/main.cpp` md5 **`43b8012319cc9e5673f7aab6244c746a`** (39,238 lines), VERSION `1.0.0-rc.1`, on a 64 GB / 10-core Apple M1 Max, macOS 26.5.2, Apple clang 21.0.0, Qt 6.11.1, SQLite 3.51.0. Baseline column is 2026-08-22 at `f752f65`. `†` = re-run by hand for this document; the command is in §0 or §4.

### Suites, pins, mutation

| Metric | 2026-08-22 | 2026-08-23 |
|---|---|---|
| Registered ctest suites (`ctest -N`) † | 72 | **78** (core 72 · app 5 · ocr 1) |
| Suites labelled `fixture` † | — | **37** |
| Suites runnable with no untracked fixture (`-LE fixture`) † | 35 | **41** |
| Suites that compile `app/main.cpp` | 2 | **2** |
| Full battery wall clock | 59.3 s at `-j6` | **18.55 s at `-j8`** · 114.99 s serial |
| `app --selftest` assertions | — | **346 PASS / 0 FAIL** |
| Suites proved to pass vacuously without their fixture | 2 | **0** |
| Mutations applied | 13 code + 3 gate probes | **21 code** (+ 4 gate probes) |
| Mutation score, `allcore` + `allocr` | 4/4 = 100% | **8 killed of 21 = 38.1% overall** (mixed sample) |
| — unbiased random arm | — | **4 of 9 = 44%**, 95% CI ≈ 14–79% |
| — adversarially-targeted arm | 3/9 = 33% | **4 of 12 = 33%** |
| Fixes revertible at their defect site with the battery green | 3 | **4** (all committed the same day, under critical/ship-blocker labels) |
| Test files that arrange a write failure | **0 of 64** | **2** (`tools/test_shortwrite.sh`; `ApprovalPane::selfTest` RLIMIT_FSIZE drill) |
| Write-failure mutants killed | 0 of 5 | proposals: **killed**; `saveOrWarn` both branches: **survived** |
| Event-driving calls inside `selfTest` bodies † | **0** | **8** (`main.cpp:15420,15465,15468,15488,32918,32977,32989,33014`) |
| Pins resting on substring/HTML presence | 125/217 = 57.6% | **115/274 = 42.0%** (34% of that bucket carries an absence clause or a number → genuinely weak ceiling **27.7%**) |
| Core-test `CHECK` sites / test LOC | 939 / 8,751 | **946 / 9,517** (test:production 1:6.5) |
| Time assertions in all of `ctest` | — | **1** (`tm84000_smoke.cpp:46`) |
| Memory assertions anywhere in the tree | — | **0** |

### Coverage (llvm-cov, instrumented, full battery)

| Target | 2026-08-22 | 2026-08-23 |
|---|---|---|
| `core/src` lines / functions / branches | 94.03% / 98.92% / 79.13% | **93.77% / 98.06% / 79.15%** |
| `app/main.cpp` lines / functions / branches | 56.81% / 47.67% / 40.91% | **58.22% / 49.38% / 42.24%** |
| `app/main.cpp` functions never executed | 662 of 1,265 | **657 of 1,298** |
| `--gauntlet 1 300` alone (lines) | 27.04% | **27.14%** (marginal contribution over selftest: **+3.92 pts**) |
| Worst panes (lines) | — | `SettingsDialog` **0.0%** · `ScanPane` **22.2%** · `FilesPane` 25.6% · `AnalysisPane` 30.3% |
| Panes with a `selfTest` method | — | **16 of 20** (`AnalysisPane`, `FilesPane`, `ScanPane`, `CatalogPane` have none) |
| `saveOrWarn` failure branches | 0 executions | **0 executions** |

### Unchecked I/O and error handling (`app/main.cpp` production scope)

| Metric | 2026-08-22 | 2026-08-23 |
|---|---|---|
| Write-mode `open()` sites, all † | — | **73** (28 production · 45 fixture) |
| Production write sites fully verified / discarding the write-side return | — | **7 / 21** |
| Production sites printing success over an unverified write | 9 | **9** (audit said 11; 2 overcounted) |
| `saveOrWarn` / `writeAllOrWarn` / `streamWriteOk` / `warnWriteFail` call sites † | 13 / 0 / 0 / 13 | **13 / 4 / 4 / 12** |
| Core stores whose `save()` verdict precedes any flush † | 4 | **3** (`glossary.cpp:47`, `dossier.cpp:83`, `comments.cpp:111`) |
| `mkpath` bools discarded | 24 of 26 | **61 of 64** |
| `QFile::remove` bools discarded | — | **38 of 41** |
| `setTransferTimeout` calls † | **0** | **2** (of 12–14 request sites) |
| `HttpStatusCodeAttribute` reads † | 0 | **0** |
| `QJsonDocument::fromJson` / real `QJsonParseError` checks † | 43 / 1 | **43 / 1** |
| `[[nodiscard]]` in `allcore` headers / bool-returning decls † | 0 / — | **3 / 63** |
| Dialog-stub assignments (the FAIL-4 seam) † | n/a | **0** |

### Compiler, sanitizers, analysis

| Metric | 2026-08-22 | 2026-08-23 |
|---|---|---|
| Translation units compiled | 147 | **148** |
| Warnings at the standing wall (`-Wall`) † | **0** | **1** (`app/main.cpp:27579`, live since 11:22, survived a 16:05 rebuild) |
| `-Werror` / `-Wno-*` / `#pragma diagnostic` / `NOLINT` † | 0 / 0 / 0 / 0 | **0 / 0 / 0 / 0** |
| Cost of `-Wextra -Wpedantic` | 15 at 8 sites, all false positives | **15 at 8 sites, all false positives** |
| Cost of strict ISO `-std=c++20` on top | — | **0** |
| clang-tidy unique sites (when installed into a scratch venv) | 514 + 413 | **2,432** (core+ocr 1,187 · app 1,245) |
| `clang-analyzer-*` findings | 0 | **9** (5 first-party, 2 real, 3 false positives, 4 vendored) |
| ASan+UBSan build of the whole tree | 0 errors, 3m20s | **0 errors, 1 warning, 3m21s** |
| Sanitizer reports across all dynamic runs | 0 | **0** (78 suites, 343 pins, 22 sweeps, 8,100 gauntlet steps, 25 real texts, 200,000 hostile mutants across 38 entry points) |
| Confirmed crash/UB defects found by targeted probing | 3 | **5** (2 new: `recognize.cpp:335`, `diff_match_patch.cpp:196`) |
| CI configurations † | 0 | **0** |
| Dependency lockfiles † | 0 | **0** (`Brewfile` exists; `Brewfile.lock.json` does not) |
| Constitution rules / rules about provenance, caps, I/O or docs † | 9 / 0 | **9 / 0** |
| Constitution rules provably losable | C2 (disarm-blind) | **C2 + L3** (L3 mutation-proven losable four ways) |

### Performance at real scale

| Operation | 2026-08-22 | 2026-08-23 |
|---|---|---|
| `Spine::lookup` median / p95 | 0.024 / 0.031 ms | **0.021 / 0.059 ms** |
| `corpusCount` uncapped, worst | 7.592 ms | **21.19 ms** |
| Three ordinary terms ANDed, **indexed** path | 346,116 ms · 18.0 GB | **487 ms · 43 MB** |
| Three ordinary terms ANDed, **unindexed** path (the default) | not measured | **115,590 ms · 23.5 GB max RSS · 32.7 GB peak footprint** |
| `LibraryIndex::search "PA"` warm / cold | 1,960 / 18,828 ms | **1,833 / 8,079 ms**; `limit=1` costs the same as `limit=400` |
| Eight-field query (the pane's own max arity) | not measured | **7,512 ms** AND · **6,923 ms** OR — both above the 6,000 ms pin, neither pinned |
| Document open, largest library text (2.6 MB) | not measured | **9,303 ms** first · ~4,400 ms warm · 1,026 MB max RSS |
| `ensureSegmenter` | 5,257–5,647 ms | **~5,100 ms** (now with progress + Stop + WaitCursor) |
| `Tm84000::ensureBuilt` cold / cached | not measured | **5,624 ms / 590 MB** vs **107 ms** |
| Full index build (8,988 files) | 190,492–196,487 ms | **156,630 ms**, 2.62 GB from 643 MB (4.08×) |
| Incremental no-change pass (1,011 files) | not measured | **0.09 s** |
| Refold pass vs fresh, same content | not measured | **+26% to +39% slower**, file **+20.9%** then **+29.5%**, no VACUUM, no FTS optimize |
| RSS: before first user action / 10 MB document | 440 MB / — | **446 MB / 2,115 MB** |
| Worker threads / `setOverrideCursor` sites | 1 / 0 | **1 / 1** |

### Distribution and provenance

| Metric | 2026-08-22 | 2026-08-23 |
|---|---|---|
| Mach-O objects shipped / mapped to an upstream project | 152 / — | **165 / 165 (0 unmapped)** |
| Distinct upstream projects bundled / named in the notices | ~32 / 6 | **35 / 33 rows** |
| Components with `LICENSE_TEXT_MISSING` in a team press † | — | **3** (qt 26 objects · qtimageformats 8 · qtvirtualkeyboard) |
| Licence texts in `docs/licenses/texts/` † | 0 | **0** (`README.md` only) |
| Project `LICENSE` † | absent | **present** (read by Adam 2026-08-23; not by counsel) |
| Payload-manifest rows / `drop` rows | none | **82 / 23** |
| `drop` bytes present in the shipped DMG and the install † | not measured | **61,866,439** across 20 paths |
| Homebrew rpath / dependency leaks | 0 / 0 | **0 / 0** (6 absolute `LC_ID_DYLIB` remain) |
| `LSMinimumSystemVersion` † | **empty** | **26.0** |
| Bundle identifier † | `com.appelegant.…` | **`com.appelegant.…`** |
| Git tags / commits since the only tag † | — | **1 / 108** |
| DMGs in `dist/` claiming version 1.0.0-rc.1 † | — | **2** (distinct sha256) |
| App binary byte-identical across two independent builds | not measured | **yes** |
| Library: files counted as "texts installed" / actual texts † | — | **8,985 / 7,522** (1,463 META companions) |
| Spine: entries / corpus segments † | 105,634 / 42,199 | **105,634 / 42,199** |
| Documents still quoting the superseded 35,601 † | — | **6** (plus `CLAUDE.md`'s ruling and one historical chronicle) |
| Documents still quoting "55 suites" † | — | **4** (`README.md:86`, `DEMO_SCRIPT.md:130`, `DEVELOPER_ONBOARDING.md:36`, `APP_FUNCTIONS_AND_DESIGN.md:401`) |

---

## 6. Remediation ledger

Ordered by (severity × reach) ÷ effort. **Ship-blocker** means: do not press 1.0.0 without it.

### Do before 1.0 ships

| # | Item | ids | Effort | Why now |
|---|---|---|---|---|
| 1 | Give the three unflushed stores the `proposals.cpp` ending (`flush(); close(); return !f.fail();`), and write the `RLIMIT_FSIZE` drill for `GlossaryStore` **first**, mutation-verified | FAIL-1 | **small** | **Ship-blocker.** A live critical, and the one that destroys the translator's own binding vocabulary silently. Three lines each, and the drill technique already exists at `main.cpp:28646`. |
| 2 | Cap and disclose `goferSearchFiles` — same `kScanCap` + `truncated` treatment `LibraryIndex::search` got — and disclose the 4,000-file scan limit | PERF-1, PERF-7 | **medium** | **Ship-blocker.** A live critical on the path a fresh install takes: 23.5 GB and 115 s with an inert Stop. Merge the two evaluators so they cannot diverge a third time. |
| 3 | Refuse a charset/output-layer mismatch in `TextRecognizer::recognize`, and check the write in `ocrDownloadOne` | MEM-1, FAIL-13 | **trivial** | **Ship-blocker.** A ~10 KB heap overread whose bytes are appended to OCR output as recognized Tibetan, reachable through the app's own model downloader. One `if`. |
| 4 | Guard the empty partner file in `compareWith`, and wrap both `diff_main` sites in `catch(...)` | MEM-2 | **trivial** | **Ship-blocker.** `SIGABRT` on an ordinary click, taking unsaved input with it. The correct guard is ninety lines away; the demo script walks a presenter through the trigger. |
| 5 | Make `corpusCount` return `-1` on a query it could not answer, and fix the Search pane header/footer to print shown, fetched and total | DATA-1, DATA-5 | **small** | House rule 3 with user-visible numbers, in the pane the demo uses, in 2 of 3 modes. `Tm84000::matchCount` is already the correct twin. |
| 6 | Drop the FSF licence texts into `docs/licenses/texts/` and make `--require-texts` unconditional | BUILD-23, BUILD-18 | **trivial** | Five minutes. Removes the sharpest licence question — LGPL-3.0 across 26 shipped Qt objects — from every future image. |
| 7 | Press once in team mode, diff the stage, re-issue, and add a removal list so `drop` paths are deleted from an existing install | BUILD-18 | **medium** | The artifact contradicts its own notices by 61.9 MB and `ditto` means no future press heals it. Also correct `SHIP.md:18`, which claims the present tense. |
| 8 | Wire the FAIL-4 seam: assign `g_saveDialogStub` from the selftest and land one write-failure drill in the same commit | FAIL-3, TEST-2 | **small** | The seam shipped today with zero consumers and `CLOSER.md:420` records it closed. A seam with no consumer is an unproven seam. |
| 9 | Fix L3's missing returncode guard and move the notes print below it | STATIC-3 | **small** | A press gate that can be deleted while the gate prints "all rules hold". Three lines, and it is mutation-testable. |
| 10 | Assert the registered-suite count in the press (`ctest -N` vs `ALL_TEST_BATTERIES`) and add `-DALL_RELEASE=ON` making the optional dependencies fatal | BUILD-19 | **small** | Otherwise one missing dependency silently deletes six process gates or a whole pane, and About keeps quoting 78. |
| 11 | Add a required-presence pass to the payload gate and de-`&&` the staging copies | BUILD-20 | **small** | The press's self-described final check returns 0 on an empty stage. |
| 12 | Wire `kMaxPlausibleSkew` or delete it, and add a constitution rule that greps the teed build log for `warning:` | STATIC-1 | **small** | The wall is broken now, and two shipping documents assert it is not. `verify.sh` already tees the log. |
| 13 | Exclude META companions from the Library banner and pin it | DATA-4 | **trivial** | The demo's opening line overstates the corpus by 1,463 phantom texts. One filter. |
| 14 | Correct the four "55 suites" documents and the six "35,601" documents; derive both at press time | DATA-13 | **small** | The demo script hands Adam two numbers to say out loud and both are wrong. |
| 15 | `setTransferTimeout` on all five `QNetworkAccessManager` members; wire `canceled → loop.quit()` at `main.cpp:9288` | FAIL-4, PERF-10 | **trivial** | A minute-long freeze per folio on conference wifi with a Stop that fires only after it. Two lines, both present verbatim elsewhere. |
| 16 | Retighten `kIxMax`/`kSegMax`, add pins at the pane's real arity, and ratchet `title_xlat_smoke`'s floor to the measured value minus a named tolerance | PERF-11, TEST-6 | **small** | A suite that prints its own 21.3-point regression and passes. The ribbon-width ratchet at `main.cpp:4354` is the pattern. |
| 17 | Commit or generate the two Overlay perf fixtures; make an absent perf fixture FAIL, never `[info]` and never silent | PERF-2 | **small** | Both guards on the product's central interaction have never executed in this checkout. Carried from baseline ledger item 29. |
| 18 | Null-guard `progress.cpp:122/204/218` with the existing `columnText()`; add a NULL-row pin | MEM-3 | **trivial** | Reported 2026-08-22 at trivial effort with the fix already written three times in the same library, and **absent from `CLOSER.md`, `FINDINGS.md` and `OVERSIGHT.md`** — a delivery failure, not a code one. |
| 19 | Make `codesign --verify` fatal; add `hdiutil verify` and a mount-and-check after `hdiutil create` | BUILD-8, BUILD-16 | **small** | The press prints PACKAGE COMPLETE having never opened the file it wrote. Under ten seconds of wall clock. |
| 20 | Set an ALL-owned bundle identifier and wire the plist versions from `VERSION` | BUILD-15, BUILD-22 | **trivial** | Fixed the moment a Developer ID is issued, and every install makes it more expensive to change. |

### Do soon after 1.0

| # | Item | ids | Effort |
|---|---|---|---|
| 21 | Adopt mutation testing as a standing gate: `tools/mutate.sh`, and "a pin proved red by a mutation" as the merge criterion. Write killing pins for the 13 survivors, starting with the four same-day fixes | TEST-1, TEST-3 | **large** |
| 22 | Push `limit` into the scan and remove the redundant `ORDER BY`; then `stop_token` + off-thread for `LibraryIndex::search` | PERF-3, PERF-4 | **medium** |
| 23 | Detect and announce the refold before the transaction; fix the false "not redone" promise; add `optimize` + `VACUUM` + WAL | PERF-6 | **medium** |
| 24 | Return `truncated` from the Gofer TERM leaf and disclose it in the pane | DATA-2 | **medium** |
| 25 | Generate `Brewfile.lock.json`, add `brew bundle check` as press step 0b, and stand up the cheapest CI on the 41 fixture-independent suites | BUILD-21 | **medium** |
| 26 | Split `package_macos.sh` into press and install — the prerequisite for CI and for a gate-probe C2 | BUILD-13, BUILD-10 | **small** |
| 27 | Constitution R7: census of discarded write-side returns, baseline file, in R3's exact shape. R8 for the network primitives (status reads, timeout coverage, unchecked `fromJson`) | FAIL-10 | **small** |
| 28 | Give `Entry::hgm_gloss` the `hgmGlossPhrase` treatment — display text unobtainable without the tier — and extend the F1 sweep beyond `entryHtml` | DATA-3, DATA-9 | **medium** |
| 29 | Finish the `[[nodiscard]]` sweep across the other 60 bool-returning `allcore` declarations; commit a `.clang-tidy` with the `CheckedFunctions` list as a ratcheting baseline | STATIC-2, STATIC-4 | **medium** |
| 30 | Give `ScanPane`, `SettingsDialog`, `AnalysisPane` and `FilesPane` a `selfTest`; set a per-pane coverage floor | TEST-7 | **large** |
| 31 | Give `tm84000()` the progress+Stop treatment `ensureSegmenter` got; move the segmenter build to the tick | PERF-5 | **small** |
| 32 | Disclose the remainders: survey 15-cap, teachings 2-of-5, 84000 duplicate notice, footnote 2-cap, pecha preset clamp, four-layer 1-of-N | DATA-6, DATA-7, DATA-8, DATA-10, DATA-11, DATA-19 | **trivial each** |
| 33 | Record source sha256 in the spine's `meta` and assert it in the press | BUILD-12 | **small** |
| 34 | Clamp the `botok` public index API; bound the Gofer parser's recursion; guard `kMElem[-1]` | MEM-4, MEM-7, STATIC-7 | **small** |
| 35 | Track the shot_diff baseline and make an absent baseline FAIL; add a stale-binary tripwire (binary mtime vs newest source) as constitution R9 | BUILD-9, TEST-9 | **small** |
| 36 | Add ruff on the seven ctest-registered gate scripts (16 findings, 10 unchecked `subprocess.run`) | STATIC-8 | **small** |
| 37 | Fix the Gatekeeper instruction and say the DMG is unsigned; correct the `--skip-build` and "SQLite vendored" claims | BUILD-3, BUILD-14 | **small** |
| 38 | Accessibility: `Qt::TextBrowserInteraction` on the four mouse-only link labels; a non-colour carrier in the links ledger and the double-keying comparator; replace R4's blocklist with a computed WCAG rule; correct `UX_AUDIT_2.md:134-136` | DATA-14, DATA-12 | **medium** |
| 39 | Wire `tools/sanitized_battery.sh` to a trigger (the press, or a weekly run) and derive its recorded suite count from `ctest -N` | MEM-5 | **small** |
| 40 | The trivial one-liners: OCR worker's `deskewOff` parameter, the context-less `QTimer::singleShot`, the `connect(qApp, …)` capture, `diff_match_patch.cpp:1480`, `contractions.cpp:42`, `sanskrit.cpp:603`, the `#b00` tier ink, the `#888` provenance mark, the SYSTEM include keyword, `lineCount()` | MEM-6, MEM-8, MEM-9, STATIC-6, STATIC-9, STATIC-10, STATIC-11, DATA-15, DATA-20, PERF-15 | **trivial each** |

### Not worth doing before 1.0 — and in some cases not at all

- **`-Wextra -Wpedantic` adoption.** Re-measured: 15 warnings at 8 sites, all false positives on aggregate initialisers; strict ISO adds zero. The *gate* (item 12) is worth more than the flag. Same conclusion as the baseline.
- **A universal or lower-floor macOS build.** Unchanged: 128 of 129 bundled dylibs are macOS 26 Homebrew bottles. The requirement is now documented where a user reads it, which was the actual gap.
- **Raising `app/main.cpp` coverage as a project.** It moved 1.7 points in a day of directed work; a coverage push is a programme, not a fix. Items 21 and 30 buy far more per hour.
- **The 19 mid-word truncations, the ink-token migration, the `SYSTEM` include change.** Real, mechanical, and better done in one pass after the ship.
- **A worker-thread refactor of the UI.** The remediation commit's own reasoning is right: "no worker thread, by design, in a file with no threading discipline to lean on." Item 22 puts one function on a `QFutureWatcher`; that is the right size.
- **Rewriting anything.** Nothing here recommends a rewrite. Every item is a clamp, a return value, a disclosure sentence, a guard, or a test.

---

## 7. What could not be measured

Stated plainly, with the reason. Nothing here is an estimate dressed as a measurement.

**The press was never run end to end.** `tools/package_macos.sh` quits and `pkill`s the running app, `rsync -a --delete`s over a 1.4 GB `/Applications` install, and relaunches. Doing that would have mutated Adam's machine and raced the other agents in this tree. So every claim about what a *fresh* press would produce — that steps 4d, 4e, 6a, 6d, the README stamp and the build manifest work together in sequence — is reasoned from the script and from running each gate individually. **That inability is itself BUILD-13.**

**The full 78-suite `ctest` was not run in the repository's own build directory.** Several suites take `${CMAKE_SOURCE_DIR}/library` as an argument and `cleanup_smoke` could write into Adam's own text library; other agents were building in `cmake-build-release`. The suite results quoted here come from independent out-of-tree builds made by the dimension audits and their verifiers. `ctest -N` (a listing, not a run) was executed in the repo build directory and returned 78.

**Behaviour on the deployment target.** Everything was measured on a 64 GB / 10-core M1 Max, much of it under load averages of 5–58 from parallel agents. A 16 GB machine was not tested and swap was not measured, so the statement that PERF-1's 23.5 GB allocation causes a jetsam kill on a 16 GB laptop is extrapolation from a measured allocation, not an observation.

**Cold page cache.** `sudo purge` needs a password no agent has. Cold/warm spreads reported here are opportunistic, not controlled.

**On-screen rendering.** Every application timing and memory figure was taken with `QT_QPA_PLATFORM=offscreen`, which excludes font rasterisation, compositing and window-server round trips. Real on-screen numbers are higher by an unmeasured factor. No rendered HTML in this report was captured from a screen; render findings are source-plus-data, replicated against the shipped databases or through probes linked against `liballcore.a`.

**Thread-sanitizer coverage.** TSan cannot be combined with ASan, and the OCR cross-thread widget read needs a page image plus the ONNX models through a real GUI session. `MEM-6` is reported from static evidence only. Leak detection was also off throughout (`detect_leaks=0`, no usable LSan on macOS/arm64), so nothing in this report says anything about leaks.

**`allocr` mutation strength.** `ocr_smoke`'s mapping onto what `ScanPane` calls was verified by inspection and `ocr/src` line coverage was measured (linebuild 95.82%, linedet 97.88%, recognize 89.54%), but no mutation was applied there.

**The 84000/BDRC/Anthropic network paths.** `anthropicKey()` returns empty under any harness flag, and no API key was used. Network behaviour was measured against local fault servers and one black-holed address, not against the real endpoints. The brief's premise that BDRC returns 401 to everyone did **not** reproduce: the URL form the app builds (`wio:bdr:MW…`) answered 200 for three real ids.

**Gatekeeper first-launch behaviour.** Measured: `spctl -a -t exec` rejects the app, the DMG carries no signature and no stapled ticket. Whether macOS 26 still offers the Control-click→Open path is reasoning from the documented macOS 15 change, not an observation. The notarisation path could not be exercised — no `ALL_DEV_IDENTITY` and no notary profile exist on this machine.

**Whether the shipped spine derives from the validated release package.** No checksum links them. The counts agree (105,634 / 42,199 / 16,465 in both), and agreement of three integers is not proof of derivation. A 249 MB rebuild-and-diff was not attempted. This is BUILD-12.

**The four fixtures with no generator.** The remediation commit reports "four of their fixtures have no generator anywhere in this repository — a worse finding than the one reported". That claim was not independently verified here.

**Windows and Linux buildability.** Not tested at all. The SQLite finding (the system library, not the vendored amalgamation the docs assume) is relevant to it, but no cross-platform configure was attempted.

**Screen-reader behaviour.** The accessibility findings are API-level (interaction flags, focus policy, `setAccessibleName` counts, computed WCAG ratios). VoiceOver was not run.

**Repo hygiene, for the record.** `git status` was clean at session start apart from a `LICENSE` edit by a parallel agent (adding "Adam read and accepted it on 2026-08-23"). No agent contributing to this assessment edited any repository file; every build and probe was made in scratch directories, and nothing was built in `cmake-build-release`.

---

## 8. What this says about the process

The baseline's finding was: **the rules are correct and they hold exactly where their author remembered them.** The remediation was directed squarely at that. So the question this section has to answer is not "did the fixes land" — they did, and §0 shows it — but "does the process now catch the class?"

**It catches it in three named places and nowhere else.**

### 8.1 Three rules became machinery. Roughly forty did not.

Before 2026-08-22 the payload rule was "remember not to stage ingest by-product". It is now 82 manifest rows walked by a gate with an 11-check mutation-designed selftest. Before, a suite with no fixture might pass vacuously; now it SKIPs by name and the press counts skips. Before, no test in the repository could see a write failure; now a RAM disk gets filled and the mutant dies. Those three are genuine conversions from memory to mechanism, and they are why the grade moved.

Everything else is still memory. The constitution has nine rules and **zero** touch provenance, tier labels, display caps, I/O returns, documentation figures or compiler warnings — the six classes that produced most of this report. `ctest` contains **one** time assertion in 78 suites and **zero** memory assertions in the whole tree. `grep -rn "warning:" tools/*.sh tools/*.py` returns nothing, which is why a broken wall has stood for six hours across a rebuild.

### 8.2 The fix visits the site the report named. The class survives at its sibling.

This is the sharpest finding in the assessment, because it happened twice in twenty-four hours to the two items the project treated as most urgent:

- `LibraryIndex::search` was capped. `goferSearchFiles`, the other branch of the same `if` statement at `app/main.cpp:15836`, was not — and it is the branch a fresh install takes, because `tools/package_macos.sh` ships an empty library and only the button builds an index. Measured: **115,590 ms, 23.5 GB.**
- `ProposalStore::save` learned to flush before judging. `GlossaryStore`, `DossierStore` and `CommentStore` — the same shape, in the same directory — did not. Measured: **TRUE returned with 64 of 2,842 bytes on disk and zero entries surviving reload.**

And the fix wave's enumeration was not merely incomplete; it was **misled by its own measurement**. The baseline probed those three stores at 600 rows, above the stream buffer, in the one regime where the broken shape works, recorded them as "honest", and consequently excluded them from ledger item 2. A measurement taken at the wrong scale became evidence of correctness.

The cure is structural and the project already knows it: `hgmGlossPhrase` (`app/main.cpp:16986-16992`) makes the authority label unobtainable without the tier, so no caller can print one without the other. That is a class fixed rather than a site fixed. It exists in exactly one place in 39,238 lines.

### 8.3 A fix landing green is still not evidence the fix will stay.

Four fixes committed **today**, three of them labelled critical or ship-blocker, revert with `ctest` at 76/76 and the app selftest at 346 PASS. A liveness control proves that is not a stale-binary artifact. Meanwhile the PERF-1 fix introduced a 34× regression on the single-term path — the operation three documents call "instantly" — and shipped green.

The instrument that would catch all of it exists and was used correctly twice this week: the shortwrite drill and the STATIC-1 rollback pin were both mutation-verified before commit, and `CLOSER.md` #48 records that the author's first write-failure test could not fail and that only a planted mutant revealed it. That is exactly the right instinct. It has not become a rule. **"A pin that fails on the pre-fix code" as a merge requirement is the single highest-leverage change available to this project**, and it costs a shell script.

### 8.4 The gates now report on themselves, and two of them cannot.

C2 exists so that "a press that silently lost a gate is a failed press". It is a substring match, so a disarmed gate reads as a present one — mutation-proven again this round. And L3 turns out to be a gate that can be *lost outright*: delete `docs/OVERSIGHT.md`, watch `reconcile_lists.py` die with `FileNotFoundError`, and the constitution still prints "all rules hold". The asymmetry is provable in one pair of commands, because L2 twenty-eight lines above has the guard L3 lacks.

The same shape appears in the build: nothing asserts how many suites are registered, so a missing `python3` deletes six process gates and the press prints "72 tests passed" — while the About pane, which reads a static source count, keeps telling the user there are 78.

**Green means "nothing objected". It does not yet mean "the check ran".** That sentence describes the stale-binary incident, C2, L3, the suite count, the payload gate's blindness to absence, and `saveOrWarn`'s dead branches. It is one defect wearing six costumes.

### 8.5 What the process gets right, and must keep

Three habits are worth more than any finding in this report.

**It writes down its own failures.** `CLOSER.md` #48 says the first fix was insufficient and the first test could not fail. The remediation commit lists **52 NOT DONE items** rather than claiming them. `LICENSE` says it has not been reviewed by counsel. `CI_REQUIREMENTS.md` opens "Status: not stood up." `docs/SHIP.md` keeps a superseded ruling in place with a dated correction above it. Almost nothing in the remediation record was found to overstate what was done — the single exception is `SHIP.md:18`'s present-tense claim about the DMG.

**It fixes classes when it thinks to.** `saBcadShapeOf` replaced a magic number with a dynamically-grown table and `.at()`. `hgmGlossPhrase` bound label to tier. `catalogRosterSave` writes temp-then-rename. `streamWriteOk` rests on a measured fact (`ts.status()=3` while `f.error()=0` on a full volume) and pins both halves. Those are not patches; they are shapes that cannot regress the same way.

**It runs its gate.** 78 suites in 18.55 seconds. Cheap gates get run, and every finding in this report was found *because* the machinery was fast enough to drive.

### 8.6 The one measurement to watch

Between now and the next assessment, one number tells you whether the process changed: **how many defects fixed in the previous round can be re-injected with the battery green.** It was 3 on 2026-08-22. It is 4 today — and all four were committed the same day they were fixed.

If that number is not lower next time, nothing structural has changed, whatever else moves.

---

*No file in this repository was modified in the course of this assessment except the creation of this document. The 2026-08-22 baseline is unchanged and remains the record this comparison depends on.*
