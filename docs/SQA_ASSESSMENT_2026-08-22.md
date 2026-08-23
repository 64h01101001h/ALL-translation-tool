# TOTAL SQA ASSESSMENT — Diamond Cutter Translation Tool

**Date:** 2026-08-22 · **Commit:** `f752f65` (2026-08-22 22:39:16 -0600) · **Version:** 1.0.0-rc.1
**Subject under test:** `app/main.cpp` (37,493 lines, md5 `30e3f0036feecbd43f2f503754edd579`), `allcore`, `allocr`, the press, the gates, the docs.
**Scope:** the ENGINEERING PROCESS and the product's fitness as software. Distinct from `docs/BUG_BOUNTY_2026-08-22.md` (which hunted defects) and the UX audits (which judged surfaces).
**Method:** seven independent dimension audits, each followed by an adversarial verification pass that tried to refute its own findings. Findings a verifier refuted, or found already closed, are excluded below and named in §4.0 so you can see what was thrown away.

---

## 1. The verdict

**Overall grade: C−**

Start with what is real, because it is unusual. `allcore` is measurably excellent — 94.03% line and 98.92% function coverage under `llvm-cov`, and every deliberate mutation planted in it was caught, some by two suites. The press is a properly gated pipeline: I confirmed that a failing `ctest` genuinely aborts it under `set -euo pipefail`, that the Homebrew dylib fixup leaves 0 rpath leaks across all 152 shipped Mach-O objects, and that the installed binary is byte-compared against the staged one before the press claims success. The project encodes past incidents as executable rules (`tools/constitution_check.py`, nine of them), keeps an honest backlog (`docs/CLOSER.md`), pins rendered output rather than internals in its best tests, and shipped `tools/verify.sh` the same day the stale-binary hole was noticed. Several audits closed a finding they had opened because the fix was already in the tree. That is a serious engineering culture and it should be said plainly before anything else.

The grade is C− because the assurance machinery does not reach the half of the product where the house rules live. `app/main.cpp` is 3× the size of the library and sits at 56.81% line / 47.67% function / 40.91% branch coverage; 3 of 9 mutations planted there were caught, against 4 of 4 in the engines. Six mutations survived a full green `ctest` — including three separately-ranked bug-bounty fixes reverted at their exact defect sites. Not one test in the repository can detect a write failure: five independent "report success when the write failed" mutants all survived 72/72, and the sole write-failure assertion is a pure formatter called with literal arguments. Two live criticals remain: a two-word library search compiles to an uncapped cross product that I measured at 18.0 GB and 5m46s on the GUI thread with a Stop button that cannot interrupt it, and `ProposalStore::save()` returns `true` after writing 4% of the shared proposal record while the app discards the return and prints "N proposal(s) approved and stamped."

The pattern underneath both halves is the same one the bug bounty named: **the rules are correct and they hold exactly where their author remembered them.** Four of the nine constitution rules were measured and all four encode the incident rather than the invariant — R1 is blind to 34% of regex construction sites, R4 is a five-string blocklist that misses 149 sub-AA text sites, L2 passes 102 MB of payload on the substring "84000", C2 cannot tell a deleted gate from a disarmed one. Fixes for closed bounty findings have 4th and 5th surviving surfaces. Closure evidence is repeatedly a green suite that provably cannot see the change it is quoted for. That is the finding of this assessment, and it is a process finding, not a code one.

---

## 2. Grades by dimension

| Dimension | Grade | One-line justification |
|---|---|---|
| Test strategy and real coverage | **C** | `allcore` 94.03% lines / 4-of-4 mutations caught; `app/main.cpp` 47.67% functions / 3-of-9 caught, and three bounty fixes revert at 72/72 green. |
| Memory safety and undefined behaviour | **B** | Zero sanitizer reports across ~1.35M adversarial invocations and 0 clang-analyzer findings — but the one memory-safety pin is `bool survived = true;` and no sanitizer runs in any gate. |
| Static analysis and compiler strictness | **C** | 0 warnings across 147 TUs with 0 suppressions anywhere — and 0 `[[nodiscard]]` across 59 headers, no clang-tidy installed, no CI, and the first analyzer run produced three data-integrity defects in one 240-line file. |
| Failure paths and error handling | **C** | 27 of 27 production `open(WriteOnly)` returns are checked and `saveOrWarn` is correct — but 0 of 5 write-failure mutants were killed and two trust-bearing stores were measured reporting success over bytes that never landed. |
| Performance and responsiveness at real scale | **D** | Engine work is excellent (1.75 µs/token overlay, 0.024 ms lookups); the product freezes for 5.6 s on a checkbox, 196 s on an index build, and up to 5m46s / 18 GB on an ordinary three-term search, uncancellable. |
| Build, release, dependencies, licensing | **C** | Eight ordered press gates that genuinely fire and a verified dylib fixup — against zero dependency pinning, no CI, 37 of 72 suites that only run on one laptop, and notices naming 6 of ~32 shipped upstream projects. |
| Data integrity, provenance, documentation accuracy | **C** | Every reference layer is labelled with its licence at the render site and the entry card computes true denominators — but three evidence surfaces print a cap as a total (200 vs 6,273) under a manual that promises "every". |

Weighting note: no dimension is failing outright, two carry live criticals, and the two strongest (memory safety, and the engine half of testing) are strong because of the code rather than because of the machinery guarding it. C− is the honest blend for a release candidate.

---

## 3. What is genuinely strong

Not padding. Each item is a measurement.

**3.1 `allcore` is covered, not merely tested.** Instrumented build (`-fprofile-instr-generate -fcoverage-mapping`), full `ctest` under `LLVM_PROFILE_FILE`, merged with `llvm-profdata`: `core/src` 12,208 / 12,983 lines = **94.03%**, 730 / 738 functions = **98.92%**, 90.35% regions, 79.13% branches. `allocr`: 851/910 lines = 93.52%. `pron_engine.cpp` is at 100.00% lines. An independent verifier reproduced these from a clean configure and landed within 0.1 percentage point on every figure.

**3.2 Engine mutations are caught, at real data scale.** Widening Gofer's NEAR window by 2 lines at both evaluator sites → `gofer_smoke` red. Making the ba→wa lenition rule skip the second syllable → `engines_battery` AND `pron_standard_smoke` both red. Collapsing the OCR CTC beam from 100 to 1 → `ocr_smoke` red. 4 of 4. These are oracles over the real 105,634-entry spine and the real 9,012-file library, not shape checks.

**3.3 No silent-skip pattern in the batteries.** `forward_battery /nonexistent/file.tsv` → exit 2 with an explicit "cannot open". `engines_battery` with missing reference TSVs → exit 1, "ENGINES BATTERY FAILED (2 failures)". `botok_smoke` prints "syllable oracle: 0/0 = 0.000%" and **fails** rather than reporting a vacuous pass. The corrosive failure mode of skipping-as-passing is largely absent (two exceptions are filed as BUILD-7).

**3.4 App pins are load-bearing where they exist.** Neutering `ApparatusPane`'s search → `app_selftest` red. Reverting the bounty #10 depth clamp → the selftest dies with SIGTRAP, i.e. it really opens a deep sa bcad tree on real library text. Removing the PROVISIONAL branch from `entryHtml` → the F1 sweep caught it quantitatively: `[FAIL] … prov-unmarked 161`. The weakness is placement, not craft.

**3.5 The F1 honesty sweep is a real oracle for the render path.** `app/main.cpp:36207-36270` renders `sampleEntries(23, 4600)` plus the whole `build/weird_top.tsv` slice and asserts five invariants per card over 4,592 cards. A verifier tried to prove it toothless by mutating the render site and it went red with an exact count. It has one narrow blind spot (TEST-1), and that spot is itself guarded by `spine_smoke`.

**3.6 The press gates fire.** `set -euo pipefail` at `tools/package_macos.sh:17` makes the `ctest` pipe at line 46 abort the release — verified by reproducing the exact shell construct. Proven-fatal gates: build failure, ctest, Homebrew rpath leak (explicit `exit 1`), staged-layout launch test, installed≠staged byte compare (`cmp -s`, `exit 1`), constitution (exit 8), shot_diff with a populated baseline (exit 7). `tools/release.sh:48-53` presses before it tags and `git reset --hard HEAD~1` unwinds the version commit on failure.

**3.7 The dylib fixup works and proves itself.** Independent scan of every Mach-O in the staged bundle: `scanned=152 homebrew_rpath_leaks=0 homebrew_dep_leaks=0`. The script does not assert this — it verifies it in a hard-fail loop at `package_macos.sh:104-116`. The OMP Error #15 class is genuinely closed.

**3.8 The wall is real and unbought.** All 147 translation units compile with **0 warnings**, and there are **0** `-Wno-*` flags, **0** `#pragma GCC/clang diagnostic`, **0** `NOLINT` comments anywhere in the tree. Most projects reporting zero warnings have paid for the zero. Adopting `-Wextra -Wpedantic` today costs 15 warnings at 8 unique sites, and all 8 were checked and are false positives on aggregates.

**3.9 Clang's path-sensitive analyzer finds nothing.** 128 `clang-analyzer-*` checks over all 68 `core`/`ocr` TUs plus the 37,493-line `main.cpp` as one TU, 0 compile errors, full ASTs: **0 findings**. No null derefs, no uninitialized reads, no leaks, no divide-by-zero, across ~53,500 lines of C++20 with heavy manual index work.

**3.10 The code is sanitizer-clean everywhere it could be driven.** ASan+UBSan build of the whole tree: 0 errors, 0 warnings, 3m20s. 70/70 core suites green under it in 98.43s; `--selftest`, 7 gauntlet seeds (2,700 randomised steps), 22 pane sweeps, 159 real library documents, 11 hostile documents, ~1.35M adversarial engine-API invocations, 911 corrupt-file parses, 6,000 hostile queries — **0 sanitizer reports** from all of it. Zero `strcpy`/`strcat`/`sprintf`/`memcpy`/`alloca` in the tree; all 7 fixed char buffers are `snprintf`+`sizeof`-bounded; 0 unions, 0 type punning.

**3.11 Provenance labelling is complete at the render site, not just in a README.** Every reference layer names its tier and licence where it is drawn: Hopkins "(reference only)", `84000 GLOSSARY · CC BY 4.0 · reference only`, Mahāvyutpatti "(DILA TEI, CC BY-SA 3.0 … reference only)", StarDict "local display only", 84000 TM "reference only, never HGM". No unlabelled reference layer was found. `tibetan_source` survives the port and renders as `[generated script]` on all 79,316 generated entries; `spine.cpp:120` keeps `raw_json` verbatim so no release field is destroyed.

**3.12 The entry card computes true denominators.** `spine_.corpusCount('"' + e.wylie + '"')` with a `-1 ⇒ "total unknown"` degradation path, backed by a real uncapped `SELECT COUNT(*)`. Verified: `sems can` = 1,566, so the card renders "3 of 1566 · show more" where the bounty measured "200 of 200". The recorded-teachings ledgers build their denominator from a set over the whole index while capping display at 3. That is the correct shape.

**3.13 `saveOrWarn` is a correct honest-save helper.** `app/main.cpp:2612-2624` checks open, checks `f.write(body) != body.size()`, checks `flush()`. Compiled in isolation and run against a chmod-555 directory, a missing directory, and a genuinely full HFS+ volume: it returns `false` and warns in every case, including the ENOSPC short-write that `QFile::error()` cannot see. 13 call sites converted.

**3.14 Four bounty fixes hold under independent re-measurement.** `attestedEnglish` recovered the English on 300/300 real corpus lines with 0 false attributions. `corpusCount` is used in both card branches. The QC intake green now carries its denominator and refuses with amber "Nothing was compared." when the bank is empty. `applyComposedName` (`main.cpp:29655-29685`) is the gold standard in the repo — `NewOnly` open, byte-count and flush checked, partial file removed, rename undone, and the undo's *own* failure reported to the user.

**3.15 The suite is fast enough to run constantly.** Clean out-of-tree configure and build: 1m51s at `-j10`. All 72 suites: 59.3s at `-j6`. Startup to all panes built: 1.95s, reproducible across three runs. Speed is a real quality attribute and this project has it.

**3.16 The bibliography audit is the model for how a verdict should be stated.** `main.cpp:29890-29907` refuses outright when no banks load, states its denominator, and volunteers its own epistemic limit in prose: "the app's installed subset, a LOWER BOUND: missing here may still exist in ACIP's master tree, and presence here is not proof of presence there." Nothing else in the file argues against itself that well. It should be the template.

---

## 4. Live findings, ranked by severity

**94 live findings: 2 critical · 22 high · 46 medium · 24 low.**

Every finding below survived an adversarial verification pass or was not selected for one. Each carries its verification status:

- **confirmed** — a verifier reproduced it and could not refute it.
- **downgraded** — reproduced, but a claim was overstated; it carries the corrected severity and the correction is stated inline.
- **reported** — the dimension audit measured it, but adversarial verification sampled other findings and did not re-test this one. Treat the evidence as one measurement, not two.

### 4.0 What was thrown away

Named so you can see the discard pile, not to be acted on:

- **TEST-4 (ctest not coupled to build success)** — *already closed.* `tools/verify.sh` landed at `f752f65` on 2026-08-22 and `docs/CLOSER.md` #49 records it. Pointed at a deliberately broken tree it exits 1 with "VERIFY FAILED: the build did not succeed / Nothing below this line was run. Do NOT quote a test count." The residual (a ctest stale-artifact tripwire; `DEVELOPER_ONBOARDING.md` still documents the bare two-command loop and still says "55 suites") is carried in the remediation ledger, not as a finding.
- **BUILD-1's dyld mechanism** — refuted by construction. A `minos 99.0` executable and dylib both ran to exit 0 on this machine; `minos` is not the runtime gate the audit claimed. What survives is arm64-only plus an undocumented, unstated requirement.
- **BUILD-3's "not notarised" headline** — already tracked in four places (`CLOSER` #27 WAITING on a $99 decision, #10 CLOSED, `PACKAGING.md`, `SHIP.md`). Only the stale first-launch instruction and the unsigned DMG survive.
- **BUILD-5's "the Das PDF ships with zero recorded provenance"** — refuted. `docs/SHIP.md` carries `| data/das (Das 1902 PDF) | Published 1902 — public domain | SHIPS |`.
- **DATA-3's reference-dict "6 of 100" ceiling at `main.cpp:8195`** — refuted as a live defect. Max definitions per headword in `reference.db` is 18, so the ceiling never binds. Latent idiom only.
- **FAIL-9's "corrupt archives install green"** — refuted. `unzip` exits 9 / 2 / 50 for truncated, bit-flipped, read-only and full-destination cases, and `installZipPath`'s `exitCode()` check catches all of them.
- **FAIL-8's "4 of 5 network waits have a Stop button that does nothing"** — refuted. Three of the four wire `QProgressDialog::canceled` straight into `abort()` or `loop.quit()` and were measured exiting in ~2.1s. One lane is genuinely uncancellable.
- **PERF-6's "all 13 QProgressDialogs are on network downloads"** — refuted. Five are compute-bound (`main.cpp:12944`, `23469`, `26043`, `26559`, `31900`), four with a working Stop and per-item progress.
- **STATIC-11's 11 unchecked `std::optional` dereferences** — 11 of 11 are false positives today; kept only at low, as a fragility note.
- **PERF-1's contrived headline query** (`PA NEAR/3 PA`) — replaced below by the ordinary-usage query a verifier found, which is far worse.

---

### CRITICAL

---

#### PERF-1 · critical · `LibraryIndex::search` materialises the whole match set before applying `limit`; NEAR is an uncapped pairwise product

**Verification:** confirmed — and the verifier found a worse, ordinary-usage trigger than the one reported.
**Location:** `core/src/libindex.cpp:325-414` (TERM case 336-349, NEAR case 355-372, `limit` first consulted at 395); called from `app/main.cpp:15207-15210` (`GoferPane::find`); query built at `app/main.cpp:14976-14989`.

**Evidence.** The TERM case runs `WHERE lines_fts MATCH ?` with no `LIMIT` and pushes every row into a vector. Asking for one hit costs the same as asking for 400 — interleaved warm reps on the 2.36 GB index:

```
query=[PA] limit=1   -> 1 hits in 1999.7 ms      query=[PA] limit=400 -> 400 hits in 1960.2 ms
query=[PA] limit=1   -> 1 hits in 1958.8 ms      query=[PA] limit=400 -> 400 hits in 1930.9 ms
```

Corpus: `SELECT count(*) FROM lines_fts WHERE lines_fts MATCH '"PA"'` → **7,854,758** of 14,077,690 lines materialised to return 60. The pane's own "AND (same file)" combiner compiles to `NEAR/1000000`, so every same-file pair passes the gap test — the full cross product:

```
"CHOS" NEAR/1000000 "SANGS RGYAS"                        -> 16,014.5 ms,  6.28 GB peak RSS
"BSOD NAMS" NEAR/1000000 "SEMS CAN" NEAR/1000000 "SANGS RGYAS"
                                                          -> 346,116.1 ms (5m46s), 18.0 GB peak RSS
"PA" NEAR/3 "PA"                                          ->  55,690.9 ms, 1.96 GB, 51.3 s user CPU
"CHOS" (plain single term)                                ->   6,338.2 ms
```

Reachability is not hypothetical: the user's own `com.ALL.TranslationTool.plist` has `gofer.dirs` and `gofer.dirsChecked` both set to `…/ALL-translation-tool/library`, so a Find click enters the index. The Search pane has **eight** term fields. `stopped_` is checked only *between directories* (`main.cpp:15200`); `LibraryIndex::search` takes no cancellation token and calls no `processEvents`, so the Stop button is lit and inert for the entire call.

No test reaches this. `libindex_smoke` writes at most 4 tiny files. `--sweep Search` clicks Find *before* it types into the term fields (`[OK] button Find` precedes all eight `[OK] lineedit term N` lines in the sweep log) and `findB_` is connected only to `clicked`, so the sweep has never executed a non-empty library search.

**Impact.** Three ordinary Buddhist Tibetan content words — merit, sentient being, buddha — freeze the whole application for nearly six minutes and allocate 18 GB on a machine the app already occupies 441 MB of. On the 16 GB laptops this ships to, that is an OOM kill or a swap storm, not a stall, and the work is lost. macOS marks the process Not Responding long before it returns.

**Recommendation (smallest change that closes it).** Push the bound into SQL: give the TERM case `LIMIT ?` with a generous over-fetch, and make NEAR a merge over per-file sorted line lists (O(|A_f|+|B_f|)) instead of a materialised cross product. Cap the materialised window and **disclose the remainder** (house rule 3). Give `LibraryIndex::search` a `std::stop_token` and run it off the GUI thread so the existing Stop button tells the truth. Then pin it: one ctest case asserting `search("CHOS NEAR/1000000 SANGS RGYAS", 400)` returns under a budget against a real-scale fixture.
**Effort:** large. **Ship-blocker: yes.**

---

#### FAIL-2 · critical · `ProposalStore::save()` returns `true` after a partial write, and the app discards the return before printing "approved and stamped"

**Verification:** confirmed.
**Location:** `core/src/proposals.cpp:197-211` (`return true;` unconditional); `app/main.cpp:27500`, `27575` (return discarded), `27580` (success modal). Identical shape at `core/src/catalog_actions.cpp:121` (`ActionLedger::save`), `core/src/regenreg.cpp:41`, `core/src/colloquial.cpp:107`.

**Evidence.** The function checks the open and then ends with a bare `return true;` after an unchecked `ofstream`. Measured against the **real shipped `allcore`** on a full HFS+ volume with a 16,384-byte hole:

```
ProposalStore   rows=600  save()=TRUE   bytes on disk=16384   <-- lies      (healthy: 123576)
GlossaryStore   rows=600  save()=FALSE  bytes on disk=-1      <-- honest
DossierStore    rows=600  save()=FALSE  bytes on disk=-1      <-- honest
```

The two stores immediately beside it in the same probe report honestly because they end with `return (bool)f`. The live file is `data/proposals/proposals.tsv`, 82,953 bytes / 228 rows — far above the `ofstream` buffer, so a partial write is exactly the shape measured. The app never reads the return at all:

```
27575:  store.save();
27580:  QMessageBox::information(this, "Approve all",
          QString("%1 proposal(s) approved and stamped.").arg(applied));
```

The correct twins exist in the same file at `main.cpp:26822` (`if (store.save())`), `26945` (`if (!store.save())`) and `30157`.

**Impact.** `proposals.tsv` is the PROPOSE/APPROVE channel — where a translator's proposed rendering and the authority's ruling on it live. A truncated write drops the tail, which is where the newest rulings sit. The user is shown a green count, the registers are regenerated from an in-memory store that no longer matches disk, and on the next launch the approvals are simply gone with no error ever having been shown. Constitution rule R5 protects this file from last-writer-wins; nothing protects it from a write that never happened. The more *reachable* half is the discarded return: a refused open already returns an honest `false`, and the app throws it away and prints the modal anyway — that needs only a read-only or missing proposals folder, no ENOSPC.

**Recommendation.** Three lines. (1) `return (bool)f;` in `proposals.cpp:210` and `catalog_actions.cpp:121` (and `regenreg.cpp:41`, `colloquial.cpp:107`). Note this is only a partial fix — for a file small enough to fit the stream buffer the failure surfaces at destructor time, so add an explicit `f.flush(); if (!f) return false;` before the return. (2) Consume the return at `main.cpp:27500` and `27575`, and gate the success modal on it. (3) On `false`, say plainly that the rulings are **not** on disk.
**Effort:** small. **Ship-blocker: yes.**

---

### HIGH

---

#### DATA-3 · high · Three evidence surfaces print a display cap AS the corpus total — by 30×, 80× and 278× — and the shipped manual promises completeness over two of them

**Verification:** confirmed by execution through `allcore` itself (not by parallel SQL).
**Location:** `app/main.cpp:18199-18207` (`DraftPane::showConcordance`), `19184-19188` (phrase memory), `647-658` (`tm84000Html`); manual claims at `data/help/USER_MANUAL.md:1248-1250` and `1311-1313`.

**Evidence.** Each fetches N and prints `segs.size()` as the total. Probe linked against the repo's own `liballcore.a`:

```
chos          showConcordance prints "200 corpus hit(s)" (shows 8)  |  TRUE = 6273
sems can      ... "200 corpus hit(s)"                              |  TRUE = 1566
sangs rgyas   ... "200 corpus hit(s)"                              |  TRUE = 2104
chos kyi      phrase memory prints "12 segment(s)"                 |  TRUE = 967
rnam par      ... "12 segment(s)"                                  |  TRUE = 1610
chos          tm84000Html renders "5 of 200"                       |  TRUE = 55720 (of 400,745 TM rows)
byang chub    ... "5 of 200"                                       |  TRUE = 53111
```

`sqlite3 … "select count(*) from entries where corpus_n_segments > 200"` → **1,532** headwords are affected on the concordance path alone. The per-course breakdown printed beneath is computed over the capped 200, so the distribution is wrong too, and only 8 rows are shown with no "8 of" note. `tm84000Html`'s comment reads *"counts-first ledger (UX audit phase 3): the cap never hides the true total"* directly above a re-query with a literal `200`.

The manual: *"its **full corpus concordance** — every corpus segment containing it"* and *"every prior rendering of those words"*. `docs/UX_AUDIT.md:292` additionally asserts "P3 counts-first ledger, ALL card evidence sections ✓" — false for all three.

The existing selftests at `main.cpp:17837/17850` **do** drive both panes; they assert only that the strings "corpus hit" and "HGM corpus" appear. They never assert the count is a count.

**Impact.** A translator asks how the master renders *chos*, is told 200 attestations exist, is shown 8, and settles the term with 6,073 unseen. The 84000 case is worse in kind: a reference layer holding 55,720 segments presents as holding 200, which reads as false scarcity in the published canon. House rule 3 violated on the two panes used to decide a rendering, under a contract that says "every".

**Recommendation.** Add `long long matchCount(const std::string&) const` to `allcore::Tm84000` — the same `SELECT COUNT(*) … WHERE tm MATCH ?` shape as `Spine::corpusCount`, which already exists and is proven. Use `corpusCount` at 18199 and 19184 and render the card's own wording: *"showing 200 of 6273 · capped — refine the term"*. Say that `byCourse` describes the shown page. Correct `USER_MANUAL.md:1249` and `:1312`, and `UX_AUDIT.md:292`. Mechanical guard: a CI grep for `QString::number(...size())` / `.arg(...size())` within two lines of the words "hit", "segment" or " of " would have caught all three.
**Effort:** medium. **Ship-blocker: yes.**

---

#### DATA-2 · high · `TrainerPane` prints an untiered auto-aligned gloss under the literal label "HGM has:"

**Verification:** confirmed, proved by execution.
**Location:** `app/main.cpp:17160-17173` (`TrainerPane`), and the same omission at `23964` (`ReviewPane`, unmatched branch). Correct twin at `19121-19135` (`DraftPane`). Correct twin **in the same pane** at `16559`.

**Evidence.** A probe linked against `liballcore.a` and run on the real spine:

```
./term_probe build/hgm_spine_v27_2.db "KA BA DANG BUM PA" "my translation mentions nothing relevant"
  bum pa   tier=glossary      prov=0  matched=0  gloss0=vase
  ka ba    tier=auto-aligned  prov=1  matched=0  gloss0=pillar
```

So `17172` renders `○ ka ba — HGM has: pillar` with no tier, and `23964` renders "none of HGM's equivalents (pillar…)" with no tier. The tier is in hand and discarded: `allcore::TermUse` carries both `std::string tier` and `bool provisional`, set at `core/src/terminology.cpp:96-97`. `checkTerminology` applies no tier filter. `DraftPane`, on the identical report, appends `[PROVISIONAL]`.

Population: 3,910 of 12,004 glossed entries are auto-aligned (**32.6%**). A real Kangyur text measured live: `--survey` on `KL00044E3C…` reports `PROVISIONAL (auto-aligned — amber): 7.1%` of 4,632 tokens. Mode 5 ("Translate & compare") is a user-selectable drill.

A second defect at the same site: the intended 6-row cap `un.count('\n') < 6` can never fire, because `un` is built entirely with `<br>` (filed separately as DATA-18).

**Impact.** In the drill where a student writes a translation and presses Check, a machine guess is presented as *what HGM has* — the strongest authority claim in the app, on the weakest tier, to the user least able to tell. This is house rule 1's exact prohibition. It is the 4th and 5th surface of bounty #2, which the project itself rated critical and closed.

**Recommendation.** Extract the `DraftPane` idiom into one helper — `static QString glossWithTier(const allcore::TermUse&, const QString&)` — and use it at `17172` and `23964`. Then close the class: extend the F1 sweep to drive `TrainerPane::check()`, `ReviewPane`'s report and `HuntPalette::runSearch` against a known auto-aligned headword and assert PROVISIONAL is present, so the twelfth emission site cannot ship untagged like the fourth did.
**Effort:** small (trivial for the two sites; small for the sweep extension). **Ship-blocker: yes.**

---

#### DATA-4 · high · Four export paths print green success strings that no code path verified — proved undetectable by execution

**Verification:** confirmed. One correction: the audit's `ulimit -f 8` transcript does **not** reproduce (SIGXFSZ kills the process, exit 153); the verifier reproduced it properly on a mounted 2 MB volume, which is the real-world case anyway.
**Location:** `app/main.cpp:24536-24538` (`AlignPane::exportPairs`), `27696-27701` (`writeApprovedExport`), `26682-26685` (`OcrPane::saveOut`), `26613-26621` (batch OCR). Corroborated by FAIL-5's census.

**Evidence.** All four check only the open, write through a live `QTextStream`, and compose the success HTML before the stream flushes. Compiled and run against a genuine ENOSPC:

```
SUCCESS STRING COMPOSED: exported 200000 candidate(s) to ".../mnt/out.tsv"
  ts.status() at that moment = 3   (0=Ok, 3=WriteFailed)
  f.error()   at that moment = 0
after flush: ts.status()=3  f.error()=0  "Unknown error"
bytes actually on disk = 1,912,832  of ~7,000,000
tail: ...\trow\t106520\tsome pa\nrow        <-- truncated mid-record
```

So a post-hoc `QFile::error()` check would *also* miss it; `QTextStream::status()` is the only signal, and the app checks it nowhere: `grep -c 'QTextStream ts(&' app/main.cpp` → **9**; `grep -n '\.status()'` → **2**, both `QPdfDocument::status()` at 4515 and 4600. The house helper, on the same volume: `saveOrWarn returned false` with "No space left on device".

**Impact.** House rule 4 verbatim. On a full disk, ejected volume or Dropbox quota stop the translator sees green "exported 412 approved dictionary candidate(s)" — in `writeApprovedExport`'s case in `#1E6B4E` — while a TSV truncated *mid-record* sits on disk. Because the cut lands mid-line rather than at a record boundary, the file re-imports as valid data with one corrupt row: silent corruption of the approved-candidates handoff, which is the one artefact this app produces that feeds the master dictionary.

**Recommendation.** Extend the existing helper rather than adding a fifth idiom: `bool writeAllOrWarn(QWidget*, const QString& path, const std::function<void(QTextStream&)>&, const QString& what)` — open, run the writer, `ts.flush()`, check `ts.status() == QTextStream::Ok && f.flush()`, remove the partial file on failure, warn. Convert all four and gate the success HTML on its return. Add the CI grep: fail on any `QTextStream ts(&` in `app/main.cpp` outside that helper. Note `writeApprovedExport` already takes a path parameter and is already driven by a selftest at `main.cpp:27310-27334` with a temp dir — a failure case needs only that hook pointed at an unwritable path.
**Effort:** medium. **Ship-blocker: yes.**

---

#### DATA-5 · high · Batch OCR counts a page as "written" on the branch where the file never opened

**Verification:** confirmed at code level; a live failing batch was not executed (see §7).
**Location:** `app/main.cpp:26613-26622` (counter), `26643-26652` (report), `26558` (`mkpath` bool discarded).

**Evidence.** The counter sits outside the guard — verified directly at HEAD:

```
26620:            for (const QString& w2 : wylieLines) ts << w2 << "\n";
26621:        }                       <-- end of  if (of.open(...))
26622:        ++pagesOk;
26623:        lineCount += static_cast<long>(wylieLines.size());
26624:        flagCount += pageFlags;
```

`grep -n '++pagesOk'` returns exactly one line: there is no failure branch. `failed` (the `%6` in the report) increments only for an unreadable *image* (26573) or a thrown exception (26640) — never for a write. `QDir().mkpath(outDir);` at 26558 discards its bool, and `outDir` is under the user-settable `app/dataRoot` (editable at `main.cpp:30952-30983`), so a read-only or full root is a supported configuration. On that root, mkpath fails, every `of.open()` fails, and a 300-page batch reports *"300/300 page(s) written to <outDir> · 41,000 line(s) · 0 failure(s)"*. Even when both succeed, the body writes through an unchecked `QTextStream`, so ENOSPC yields a silently truncated `-ocr.txt` counted as fully written.

**Impact.** The cataloger is told an entire batch landed, with line and legality-flag counts computed from data that only existed in memory, and given the directory path as if it held the files. Same rule-4 violation as DATA-4, but the lie is a *count*, which is harder to disbelieve. Structurally identical to bounty #9, which the project rated high and fixed elsewhere: `git show --stat` on the B9 fix touches `main.cpp` at 4665 and 8496-8561 only.

**Recommendation.** Move `++pagesOk` inside the guard and add the missing `else { ++failed; detail += "could not write: " + out + " (" + of.errorString() + ")"; continue; }` so line and flag counters do not accumulate for a page that produced no file. Check `mkpath`'s return before the loop and refuse the batch with `warnWriteFail` rather than discovering it 300 times. Route the body through `writeAllOrWarn` (DATA-4).
**Effort:** trivial. **Ship-blocker: yes.**

---

#### FAIL-1 · high · `catalogRosterSave()` returns TRUE having written zero bytes, and the caller signs the user in

**Verification:** downgraded from critical. The mechanism reproduces exactly; two impact claims were refuted.
**Location:** `app/main.cpp:28223-28241`; callers at `30736`, `30759`.

**Evidence.** A probe carrying the function's body verbatim, on a 4 MB HFS+ volume filled to 8,192 bytes free:

```
control (healthy):  returned TRUE,  52,202 bytes,  400 rows on disk
full volume:        returned TRUE,       0 bytes,    0 rows on disk
$ ls -la /Volumes/TINYFULL/CATALOG_TEAM.tsv  ->  -rw-r--r--  0  Aug 22 22:49
```

The body is `if (!f.open(...)) return false; QTextStream ts(&f); ts << …; return true;` — the open is checked, `ts.status()` never is, and there is no flush or close check. The callers are exemplary: on `false` they roll the member back, restore the prior status and refuse the sign-in. They are simply fed a lie.

**Corrections the verifier required.** (a) An empty roster does **not** lock everyone out — `signIn()` has an explicit "No roster yet. Team… creates the first admin entry" branch at `30544-30550`, so the next person becomes admin. That is an integrity problem, not a lockout. (b) The approvals record is **not** lost: `CatalogMember` carries name/initials/roles/salt/hash/addedBy/addedOn/status only; approvals live in `ActionLedger`'s `decided_by`/`decided_on`. (c) The destruction happens at the **truncating open**, before any return value exists — so no return-check fixes the data loss, and the audit's recommended `QFile::remove(path)` on failure would make it worse.

**Impact.** On ENOSPC or a mid-write I/O error the access-control roster — who may catalog, and as whom, with salt+hash per member — is truncated to zero while the function reports success and the new member is signed in. Read-only folder, missing folder and ejected volume all fail at `open()` and are handled correctly; only the write itself escapes.

**Recommendation.** Write to a temp file and `rename()` over the roster, so a failed write cannot destroy the existing one; end with `ts.flush(); if (ts.status() != QTextStream::Ok || !f.flush()) return false;`. Better still, build the `QByteArray` first and route through `saveOrWarn`, which already checks open, byte count and flush. Apply the same to the other seven production `QTextStream`-over-`QFile` sites (2650, 24498, 24526, 26614, 26675, 27678, 29825).
**Effort:** small. **Ship-blocker: yes.**

---

#### STATIC-1 · high · Zero `[[nodiscard]]` in the codebase against ~271 discarded status returns — and the first analyzer run found three data-integrity defects in one 240-line file

**Verification:** confirmed, and the verifier found the defect is worse than filed.
**Location:** `core/src/catalog_actions.cpp:190` and `:216` (defects); `core/include/allcore/catalog_actions.h:51` (missing annotation); `app/main.cpp:30842` (caller that reports success); 59 headers in `core/include/allcore/` with zero annotations.

**Evidence.** `grep -rc nodiscard core/include/allcore/*.h` summed → **0** across 59 headers; `grep -c nodiscard app/main.cpp` → **0**. `clang-tidy bugprone-unused-return-value` with Qt/std I/O configured: **211** discarded returns in `main.cpp` (88 `QDir::mkpath`/`removeRecursively`, 67-69 `QIODevice::write`, 33-34 `QFile::remove`, 15 `waitForFinished`, plus `rename`/`mkdir`/`copy`), 51 more on mutating `allcore` APIs, 9 in core/ocr. The compiler already catches this class the moment the attribute exists — adding `[[nodiscard]]` to `save()` and rebuilding with the **already-configured** `-Wall`:

```
catalog_actions.cpp:190:5: warning: ignoring return value of function declared with 'nodiscard' attribute [-Wunused-result]
catalog_actions.cpp:216:5: warning: ignoring return value ... [-Wunused-result]
```

Runtime reproductions on `ActionLedger::approve` (staging dir writable, `ACTIONS.tsv` chmod 400):

```
approve() returned: ''            <-- "" means SUCCESS to the caller
file on the shelf?   1            file still staged?   0
ledger on disk after approve: id=A0001 status=pending decided_by=''
second approve() -> 'staged file is missing'    <-- the row is now UNCLEARABLE
```

Two further defects found in the same function: (a) the META companion rename at the `// ec never read` line **silently clobbers** an existing shelf META with no permission trickery at all (`metaCompanionFor` truncates the stem at the first `_`, so any two items numbered 001 collide) — under a test file whose own header comment says "nothing ever overwrites"; (b) `save()` itself returns TRUE on write failure — under `RLIMIT_FSIZE=1024` a 16,693-byte ledger was truncated to 1,024 bytes, `save()` returned TRUE, and reload recovered 1 row of 20. So `[[nodiscard]]` alone is **necessary but not sufficient**: `stage()`, which does check the return, is fooled the same way.

`approvalsDialog()` is reachable from the Catalog pane's "Approvals…" button (28465/28495) and the root is a user-chosen folder (28489) — a Dropbox/official shared root.

**Impact.** The ledger and the official shelf diverge permanently and silently: the file is on the shelf, the ledger still says pending, `decided_by` is empty forever, and the approver can never record the decision. The audit trail for what Geshe Michael approved is lost with a success return. Plus a silent overwrite of years of cataloging META on a name collision.

**Recommendation.** (1) Annotate the ~30 mutating `allcore` APIs with `[[nodiscard]]` — one attribute per declaration converts the class into `-Wunused-result` errors under the wall that is already on. (2) Fix `catalog_actions.cpp:190`/`:216` by copying `stage()`'s existing rollback pattern. (3) Make `save()` honest: check the stream after flush, not just the open. (4) Open the META companion with `NewOnly` (the `applyComposedName` pattern at `main.cpp:29655`) so a collision refuses instead of overwriting. (5) Add a repo `.clang-tidy` with the `CheckedFunctions` list so the 211 Qt sites become a ratcheting baseline. Keep `(void)` as the documented escape for genuine don't-cares.
**Effort:** medium. **Ship-blocker: yes** (items 2–4; the annotation sweep can follow).

---

#### FAIL-3 / TEST-3 · high · No test in the repository can detect a write failure: 5 of 5 mutants survive, and the one write-failure pin is refuted by mutation

**Verification:** confirmed independently, twice, at full-suite scale.
**Location:** `core/tests/` (64 files, 0 with failure injection); the 17 selfTest bodies in `app/main.cpp` (2,296 lines); the single pin at `app/main.cpp:4826`.

**Evidence.** Baseline 72/72. Then, one mutant per build, each followed by a full rebuild and full `ctest`:

```
M1  revert bounty #9's DEFECT SITE (if (opened) {++got;}), folioBatchReport untouched  -> 100% of 72
M2  saveOrWarn returns true on refused open AND ignores short writes                   -> 100% of 72
M3  GlossaryStore::save  returns true on refused open                                  -> 100% of 72
M4  ProposalStore::save  returns true on refused open                                  -> 100% of 72
M5  DossierStore::save   returns true on refused open                                  -> 100% of 72
```

`glossary_smoke`, `proposals_smoke`, `dossier_smoke`, `comments_smoke` all "Passed" throughout. The pin that supposedly covers M1 is `folioBatchReport(240, 12, 0, 3, 7, "/nowhere/scans")` — a pure formatter called with **literal arguments**, so reverting the code that computes those arguments cannot move it. That is the B13 refutation shape, on a fix that shipped the same day.

Coverage confirms it independently: under an instrumented build running all 72 suites, `saveOrWarn`'s happy path executes exactly once and **both failure branches execute zero times**; `warnWriteFail` (`main.cpp:2575-2594`) is **0% covered end to end**; 12 of its 13 call sites never execute at all.

```
$ grep -rlnE "chmod|permissions\(|perms::|/dev/full|unwritable|ENOSPC" core/tests/*.cpp | wc -l   -> 0
$ grep -rn "CHECK(!.*save|!store.save" core/tests/*.cpp                                          -> no output
$ grep -nE 'setPermissions|QFile::Permission|chmod' app/main.cpp core/tests ocr/tests             -> no output
```

Not one test anywhere asserts that a save **fails**.

**Impact.** The bug bounty's dominant defect family — "reports success when the write failed", findings #4, #6, #9 — has zero regression protection. Every fix shipped for it can be reverted tomorrow and `ctest` still says 72/72. This is why the family keeps recurring: the process has no way to notice.

**Recommendation.** Add the seam (FAIL-4), then write one failing-write drill per store and per export lane that (a) points the write at an unwritable path, (b) asserts the function returns `false`, and (c) asserts the UI string contains the refusal and no success wording. **Validate each drill by mutation before it lands:** revert the defect site, confirm the drill goes red, restore. A drill that stays green under its own mutation is not a drill.
**Effort:** medium. **Ship-blocker: yes** (this is what stops the family recurring).

---

#### FAIL-4 · high · No injection seam exists: the harness stubs the save dialog to empty, which SKIPS the write instead of redirecting it

**Verification:** confirmed; every number reproduced to the digit.
**Location:** `app/main.cpp:2937-2951` (`safeGetSaveFileName`) and the three sibling wrappers at `2902-2936`.

**Evidence.** `if (g_harnessRun) return QString();` — under every headless mode the dialog answers empty and all 19 call sites do `if (fn.isEmpty()) return;`. There is no stub, no env override, no settings key. But the audit corrected the bounty's own account of the scope:

```
PRODUCTION open_write+write sites:                                  55
  gated by a harness-stubbed file dialog within 60 lines:           12
  NOT gated (computed path — reachable under harness):              43
```

The other 43 *are* reachable — proven by running the real selftest with the data root made read-only:

```
read-only data root:                 EXIT=1  PASS=284  FAIL=6
read-only data root + read-only TMP: EXIT=1  PASS=239  FAIL=51
```

…but every one of those 57 failures is the selftest's own **fixture creation** breaking ("Overlay: opening a second document moves docFile_ with it"), not an assertion that the app refused honestly. The harness cannot distinguish "the app correctly refused" from "my setup broke".

**Impact.** There is no way, today, to write the test that would have caught bounty #4, #6 and #9 — and no way to keep them caught. Every future fix in this family is unfalsifiable by the battery. This is the enabling cause of FAIL-3.

**Recommendation.** Six lines, one file: add `static std::function<QString(const QString&,const QString&,const QString&)> g_saveDialogStub;` beside the wrappers and change the harness branch to `if (g_harnessRun) return g_saveDialogStub ? g_saveDialogStub(caption, dir, filter) : QString();`. Do the same for `safeGetExistingDirectory`. That single seam unlocks all 12 dialog-gated write paths at once. For the 43 computed-path sites, pass `root_` through a testable accessor so a drill can point one pane at a read-only root without making the whole harness fail its fixtures.
**Effort:** small. **Ship-blocker: yes** (it is the cheapest item on this list and it unblocks the most).

---

#### TEST-2 · high · Three separately-ranked bug-bounty fixes can be reverted at their exact defect sites with `ctest` still at 72/72

**Verification:** confirmed, and the verifier applied all three reverts **simultaneously** and still got 100% of 72.
**Location:** `app/main.cpp:11436` (`fourLayerView`), `7669-7671` (`teachingsReportHtml`), `31604-31610` (`HuntPalette` lane 3).

**Evidence.** Three independent mutations, each reverting only the landed fix, each with a full rebuild and full `ctest`:

```
MUT12_fourlayer_english   (rank-1 attested-English defect restored verbatim)  ctest_exit=0  100% of 72
MUT3_prov_label           (deleted the block whose comment reads "BOUNTY #2, third surface")  100% of 72
MUT14_hunt_lane3          (the site whose comment reads "BOUNTY #2: bare gloss here…")        100% of 72
all three at once                                                                             100% of 72
```

Why they survive: `fourLayerView` appears exactly twice in `main.cpp` — its definition at 11329 and one UI connect at 14472 — with **zero** test references; the only pins on `attestedEnglish` call the *helper* at 4235/4242/4246, never the call site at 11436. `teachingsReportHtml` has exactly one ctest-reachable pin, `check(teachingsReportHtml().contains("youtube.com"))` at 5304 — a link substring. The lane-3 suffix has none.

Sharper still: `docs/BUG_BOUNTY_2026-08-22.md` **prescribed these pins in its own Fix sections** (line 21: "Add a selftest that builds the four-layer table … and asserts column 3 is empty"; line 29: "extend the F1 honesty sweep … and assert PROVISIONAL is present"). They were never written — and the closure commit `ba0fda7` quotes "72/72 ctest green; constitution holds" as its verification. That green has zero sensitivity to the change it was quoted for.

**Impact.** The three highest-ranked honesty defects in the 22 Aug bounty are held in place by nothing but the diff that landed them. Any refactor, merge or fan-out patch touching these lines reintroduces a fabricated-authority defect and the gate stays green. It is also a house-rule-4 problem in the project's own closure record.

**Recommendation.** Every bounty fix gets a pin at the **defect site**, not the helper, and the pin is validated by mutation before merge: revert only the fix, confirm the pin fails, restore. Concretely: assert the four-layer table's English cell is empty for an unattested line drawn from a real folio; assert a known auto-aligned wylie produces a PROVISIONAL row in the teachings and Hunt surfaces and a curated one does not. Make "reverting the fix must turn a pin red" the merge criterion for the whole bounty backlog.
**Effort:** medium. **Ship-blocker: no** (the fixes are correct today) **— but do it before the next fan-out.**

---

#### TEST-5 · high · The assertion-carrying suite never traverses a signal, a click or a keystroke

**Verification:** confirmed by execution-count measurement rather than grep. One phrasing correction required.
**Location:** `app/main.cpp:15226` (Gofer fold call site) vs `34754-34762` (its pins); `app_selftest` generally.

**Evidence.** Disabling the call site while leaving the pinned static helper untouched:

```
MUT9:  if (foldMode > 0)  ->  if (false && foldMode > 0)     ctest_exit=0   100% of 72
```

The pins "covering" the feature call `GoferPane::goferFoldKeep(0, "sem dpa", terms)` directly. Measured from the instrumented build, **execution counts, not greps**: under `--selftest`, every click/trigger/key line in `main.cpp` has count **0** — lines 3793, 21298, 33554, 33578, 34569, 36807, 36946, 36963, 36978, 36991, 36996, 37356, 37368, 37415, 37480 all zero. Under `--gauntlet 1 300` the same lines run (3793→27, 37356→70, 37368→14, 37415→7, 37480→1) — but the gauntlet's only invariants are structural (`main.cpp:37299-37314`: window visible, tab count, menu-action count, no unclosable modal). Its measured reach: 37 distinct tool buttons, 7 menu actions, 13 word-clicks, over a UI of 197 `QPushButton`, 76 QActions, 11 `QShortcut`, **466** `connect(` sites.

**Correction:** the audit's claim that the gauntlet "never clicks a QPushButton" is wrong — its tool-button clicks route through the ribbon mirror at `main.cpp:3793` (`src->click()`), executed 27 times. The substance is unaffected: the *assertion-carrying* driver traverses no event at all.

**Impact.** Every wiring defect is invisible: a dropped `connect`, a guard inverted at the call site, a handler that early-returns, a shortcut bound to the wrong slot. The correct helper and the broken product both score 72/72. This is the B12 refutation shape institutionalised.

**Recommendation.** Introduce one drive-the-real-path idiom and use it for new pins: locate the widget by `objectName`, call `click()` / `setText()`+`returnPressed`, assert on the resulting pane state. Backfill it first where helpers are already pinned (Gofer fold, lookup, apparatus search, catalog actions) — the cheapest conversion of nominal coverage into real coverage. Then give the gauntlet one behavioural invariant beyond structure, e.g. every rendered gloss row from an auto-aligned entry carries its tier.
**Effort:** large. **Ship-blocker: no.**

---

#### TEST-6 / MEM-6 · high · The product's UI is at 47.7% function and 40.9% branch coverage while the library beneath it is at 98.9%

**Verification:** confirmed; reproduced from a clean instrumented configure with several figures exact.
**Location:** `app/main.cpp` (39,508 instrumented lines) vs `core/src/*`.

**Evidence.** Instrumented build, full `ctest` under `LLVM_PROFILE_FILE` (70 `.profraw`, 72/72 pass), merged with `llvm-profdata`:

```
app/main.cpp    regions 50.89%   functions 47.67% (662 of 1265 never executed)
                lines   56.81% (17,065 of 39,508 missed)   branches 40.91% (7,644 missed)
core/src        lines 94.03%   functions 98.92%   regions 90.35%   branches 79.13%
ocr/src         lines 93.52%   functions 94.29%
per driver:  --selftest alone      47.18% regions / 39.53% funcs / 52.71% lines / 38.42% branches
             --gauntlet 1 300      14.60% / 22.21% / 27.04% / 10.34%
             all 22 pane sweeps    27.84% lines / 24.03% funcs
             everything combined   54.47% regions / 53.36% funcs / 59.05% lines / 43.99% branches
```

So 60.5% of `main.cpp`'s functions are never executed with any assertion about their output, and roughly 8 percentage points of function coverage exist only under the gauntlet's structural oracle. The sweep's own output explains part of it: 17 of 22 panes exercise 4 controls or fewer, and Align, Approval, Export and OCR exercise **zero**.

**Corrections:** the honest like-for-like pairings are 47.7% vs 98.9% *functions* or 40.9% vs 79.2% *branches* — not branch-vs-function. The "8.14 points reached only by the gauntlet" figure is arithmetic on two runs, not a direct measurement of gauntlet-only functions.

**Impact.** The 72-suite count reads as broad coverage; measured, it is deep coverage of the engine layer and thin coverage of the product. The engines were never where the bounty found the honesty, I/O and identity defects — those live in the 17,065 unexecuted lines and 7,644 unexercised branches of `main.cpp`, which is where the user is.

**Recommendation.** Adopt a per-layer coverage floor checked mechanically rather than aspired to: hold `allcore` at ≥90% lines and ratchet `app/main.cpp` branch coverage up from 40.91% by a fixed step per release, driven by the panes named above rather than by whatever is easy. Give `--sweep` a document argument so Export/Align/OCR/Approval come up with content. Keep the instrumented-build recipe in `tools/` so the number is reproducible in one command (stock cmake + `llvm-profdata`/`llvm-cov`, no extra dependencies). Publish the function-coverage number next to the suite count.
**Effort:** medium (the floor and the recipe; raising coverage is ongoing). **Ship-blocker: no.**

---

#### MEM-1 · high · The one memory-safety regression pin is tautological: it prints PASS on the defective code in the shipping build

**Verification:** confirmed, including the mutation proof, plus two additions from the verifier.
**Location:** `app/main.cpp:4187-4189` (the pin); `app/main.cpp:10162-10175` (the code it guards).

**Evidence.** The pin is `bool survived = true; showSaBcad(); check(survived, "an outline nested far past the old 64-slot bound does not write past the end of its vector");` — `survived` is never reassigned. An independent scan of all 148 `bool X = true/false;` declarations in `main.cpp` found exactly one such tautology, at 4187→4189. Removing **only** the two-line clamp at 10165-10166 (restoring the historic defect) in a private copy:

```
(a) ASan build:     ==99484==ERROR: AddressSanitizer: heap-buffer-overflow
                    WRITE of size 8 ... in OverlayPane::showSaBcad()
                    EXIT=134
(b) Release build:  EXIT=0,  FAILS=0
                    "[PASS] Overlay: an outline nested far past the old 64-slot bound
                     does not write past the end of its vector"
                    "app selftest: ALL PASS (0 failure(s))"
```

**Two additions.** (1) The Release mutant is not perfectly silent: over 11 runs of the identical defective binary, **9 exited 0 printing PASS and 2 died** (exit 139 / 138) with no `[FAIL]` line. So the suite would flake at ~18%, failing as a bare crash rather than as the pin — arguably worse than deterministic silence, and the report should say so. (2) A **second** tautological pin exists at `main.cpp:17869`: `check(true, "ladder hand-off invoked (Manuscript receives the draft)")`, which calls `g_sendToManuscript` and asserts nothing about receipt. Both print house-rule-4 PASS strings.

The pin also never asserts an outline was *produced*: replacing `extractSaBcad()` with an empty vector gives 3/3 runs EXIT=0, ALL PASS — deterministically. And `extractSaBcad` (`main.cpp:10057-10122`) still has **no bound on its stack**, so the two-line resize is the only remaining barrier. The fix commit `ab06ec0`'s own message says the pin should "require the pane to survive it and still produce an outline". It does neither.

**Impact.** The repo's single memory-safety regression test cannot fail on the defect it was written for. If the clamp is ever refactored away, `ctest` reports 72/72 in 9 runs out of 11 and the selftest prints a sentence explicitly claiming the memory property being violated.

**Recommendation.** Make the pin assert something the defect changes: `const auto nodes = extractSaBcad(); check(!nodes.empty() && maxDepth(nodes) <= kMaxOutlineDepth && treeNodeCount(dlg) == nodes.size(), …)`. Delete or rewrite the PASS string in the meantime — it currently asserts an unverified memory-safety claim. Fix `17869` the same way. And bound `extractSaBcad`'s stack so the clamp is a belt beside braces.
**Effort:** small. **Ship-blocker: no** (the fix is in and correct) **— but the string must stop lying.**

---

#### MEM-2 · high · No sanitizer build is wired into any gate; the one sanitized run on record was a manual one-off at 59 suites (there are now 72)

**Verification:** confirmed, with one sentence that must be corrected before it is quoted.
**Location:** `CMakeLists.txt`, `core/`, `app/`, `ocr/CMakeLists.txt`, `tools/` (absence); `docs/STEWARDSHIP.md:29-34`.

**Evidence.** Re-measured at HEAD:

```
$ grep -rn 'fsanitize' --include='*.txt' --include='*.cmake' --include='*.sh' --include='*.py' . | grep -v cmake-build   ->  0
$ grep -rn 'sanitiz|ASAN|UBSAN' tools/     ->  no output
$ ls -a .github                            ->  No such file or directory
$ grep -E '^CMAKE_CXX_FLAGS' cmake-build-release/CMakeCache.txt
    CMAKE_CXX_FLAGS:STRING=
    CMAKE_CXX_FLAGS_RELEASE:STRING=-O3 -DNDEBUG
suite count today: core 66 + app 5 + ocr 1 = 72   (the recorded sanitized run covered 59)
```

Cost is not the obstacle: a from-scratch ASan+UBSan configure and build of the whole tree took **3m20s** on 10 cores with 0 errors and 0 warnings, and the 70 core suites ran under it in **98.43s**, all green.

**Required correction.** The audit's sentence "app/main.cpp has never been under a sanitizer in any automated run" is false and refutable on sight: `git show 66a04e8:app/CMakeLists.txt` contains `add_test(NAME app_selftest COMMAND ALLTranslationTool --selftest)` under "suite 38", so the 59-suite Q:T2 battery did put it under ASan once. The defensible wording is **"never in any *gated* run, and not under one at all since 2026-08-20."**

**Impact.** Every memory-safety property in the product is enforced by memory rather than by machinery. Thirteen suites have been added since the only sanitized run. MEM-1 is the direct consequence: a pin that only fires under ASan is dead code when nothing runs ASan.

**Recommendation.** `option(ALL_SANITIZE "ASan+UBSan build" OFF)` in the root `CMakeLists.txt` appending `-fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=undefined`, plus `tools/sanitized_battery.sh` that configures a throwaway dir with it and runs the full `ctest`. The battery is already green under it today, so this lands clean and becomes a ratchet rather than a backlog. Leak detection off (macOS/arm64 has no usable LSan).
**Effort:** small. **Ship-blocker: no — but it is the cheapest high on this list.**

---

#### PERF-2 · high · "Update search index" blocks the GUI thread for 196 seconds with no progress, no cancel and no worker thread

**Verification:** downgraded from critical on consequence; the measurement reproduced with a byte-identical output database.
**Location:** `app/main.cpp:23514-23537` (`LibraryPane::updateIndex`); `core/src/libindex.cpp:137-320`.

**Evidence.**

```
$ rm -f fresh.db && /usr/bin/time -l ./bench_update fresh.db .../library
LibraryIndex::update  190,491.9 ms  added=8988 updated=0 removed=0 lines=14,077,690
      190.72 real   137.69 user   8.83 sys
$ ls -la fresh.db  ->  2,357,227,520 bytes     (from 642,907,695 bytes of source text = 3.67x)
```

(Audit measured 196,487 ms; the verifier 190,492 ms under load. Same number.) The UI wrapper is one synchronous call with a single `processEvents` **before** it and a static italic sentence. Concurrency census over the whole 37,493-line UI: `QtConcurrent` 0, `moveToThread`/`QRunnable`/`QThreadPool` 0, `QFutureWatcher` 0, `setOverrideCursor` 0, `std::thread` 1 (the OCR worker). `update()` runs inside one `BEGIN` (`libindex.cpp:171`), so a force-quit rolls the whole thing back.

The project routed the test suite around it rather than fixing it — `app/main.cpp:34530-34535`:

```
const QStringList stallGuard = {
    "Update search index",   // minutes-long full rebuild
```

and the "covering battery" it names, `libindex_smoke`, writes at most 4 tiny files.

**Impact.** The first thing a new user is told after installing a collection is "Run Maintenance → Update search index next" (`main.cpp:22774`). Doing so hangs the app for over three minutes; macOS marks it Not Responding; a reasonable user force-quits and gets a rolled-back journal and a fresh three-minute wait. Downgraded from critical because it is user-invoked, announced ("first run over a full collection can take a while"), and loses no data.

**Recommendation.** Run `update()` on a worker thread with a `QProgressDialog` wired to a per-file callback and a real cancel that commits the files completed so far — the schema is already incremental by path+mtime+size, so a partial index is valid and resumable. The project already has this exact pattern working in five compute dialogs (`12944`, `23469`, `26043`, `26559`, `31900`); copy one.
**Effort:** medium. **Ship-blocker: no** (but it is the worst first-run experience in the product).

---

#### PERF-5 · high · Clicking one checkbox freezes the GUI for 5.65 seconds

**Verification:** confirmed; every structural count reproduced to the unit, RSS within 0.3%.
**Location:** `app/main.cpp:14038-14077` (`ensureSegmenter`), triggered from `loadDoc` at `7015-7016`; the toggle is declared `docAffecting` at `6445-6447`.

**Evidence.** The function body rebuilt faithfully against the shipped `liballcore.a`:

```
SegTrie ctor                   0.6 ms
allAcipHeadwords              11.6 ms  (105,634)
HGM headwords -> trie      1,171.4 ms  (98,502 words)
RefLexicon load              260.0 ms  (449,445 forms)
Monlam -> trie             3,813.3 ms  (+449,444)
TOTAL ensureSegmenter      5,256.8 ms  (words=547,946; nodes=3,511,730)   RSS 363,528,192
```

Single-click reachability verified in source: `showAttest_ = mkToggle("attestation", "unattested-word hints (segmenter + Monlam)", false, /*docAffecting=*/true)` and `mkToggle`'s handler calls `loadDoc()` on every change, which calls `ensureSegmenter()`. The handler also does `s.setValue("overlay/attestation", on)`, so the cost recurs on the next launch's first document load. App-level corroboration: `--sweep Overlay` (which toggles that checkbox and prints `[OK] toggle`) ran 15.41 s at 795,885,568 RSS against `--sweep Search` at 2.59 s / 447,610,880. Live sampling during `--selftest` attributed 403 of 1,423 main-thread samples (~28%) to this call. The app has no busy-cursor affordance anywhere: `setOverrideCursor` count is **0**.

**Impact.** A translator ticks "unattested-word hints" and the application stops responding for nearly six seconds with an unchanged pointer and no indication anything is happening. The sweep exercises the toggle and prints `[OK]` with no timing assertion.

**Recommendation.** Build the trie once on a worker thread at idle and cache it process-wide rather than per-`OverlayPane`; better still, serialise the built trie to disk so subsequent launches memory-map it instead of re-syllabifying 548k forms. At absolute minimum, wrap the call in a busy cursor and a status line. Pin the build time.
**Effort:** medium (trivial for the busy cursor alone). **Ship-blocker: no.**

---

#### PERF-3 · high · "T4 perf floors" is marked CLOSED but pins the four cheapest operations in the app, with 95×–1200× headroom against a comment claiming 4×

**Verification:** confirmed and strengthened — the verifier found the closure claims the very things that are unpinned.
**Location:** `app/main.cpp:36118-36149` (the T4 block); `docs/CLOSER.md:18` and `130-135`.

**Evidence.** The comment, verified verbatim at HEAD: *"Budgets are ~4x the observed numbers on Adam's machine … a real regression (an accidental O(n²), a lost index) blows through 4x instantly."* The actual pin output:

```
[PASS] T4 perf floors: 300 lookups 6ms (<2000) · 20 corpus 21ms (<2000)
                      · 1k unicode 9ms (<1200) · 1k pron 1ms (<1200)
headroom: 333x · 95x · 133x · 1200x        comment claims: 4x
```

The lookup pin hits only **two** keys 300 times (`i % 2 ? "bsod nams" : "byang chub sems dpa'"`), so it measures a warm cache: against 1,957 distinct real headwords the same operation is 4× slower at median, 9× at p95 and 54× at max. And `docs/CLOSER.md:18` reads:

> `| 12 | T4 perf floors measured + pinned (cold start, big doc, query fan-out, index build) (Quality) | CLOSED(2026-08-20) |`

**Query fan-out** and **index build** are named in the closure. Neither has any pin. The four pins touch no `LibraryIndex`, no `buildOverlay`, no `ensureSegmenter`, no directory walk — while a real O(n²) blow-up of exactly the shape the closure note describes (PERF-1: 18 GB, 5m46s) sits in `LibraryIndex::search`.

**Impact.** The backlog and the selftest both assert that performance is guarded. It is not. A 100× regression in spine lookup or a 300× regression in corpus search would pass green, and the operations users actually wait on are unmeasured.

**Recommendation.** Retighten the four budgets to ~3× the warm observation (≈25 / 80 / 40 / 5 ms) so they can fail; replace the two-key loop with a distinct-headword sample. Add pins for the paths a user waits on: `LibraryIndex::search` over a real-scale fixture including a NEAR of two high-frequency terms, `buildOverlay` on a full volume, `ensureSegmenter`. **Reopen #12** with this evidence.
**Effort:** small. **Ship-blocker: no — but the CLOSED must be reversed, because it is being cited.**

---

#### BUILD-7 · high · The battery the press gates on cannot run from a clean checkout: 37 of 72 suites need gitignored fixtures, and at least two pass vacuously without them

**Verification:** confirmed and understated — the verifier found a second vacuous pass.
**Location:** `core/CMakeLists.txt` (37 fixture-dependent `add_test` blocks); `.gitignore` (`build/`, `library/`, `data/extracted/`).

**Evidence.** Independently re-derived by parsing all 72 `add_test` blocks: **37 of 72** depend on a gitignored tree (build/ 21, library/ 12, data/extracted/ 7), leaving **35** runnable from a clean clone. Behaviour when the fixture is absent, run against nonexistent paths:

```
catalog_id_smoke      exit=0    "[SKIP] library battery (no library at ...)"     <- VACUOUS PASS
catalog_qc_smoke      exit=0    "library: 0 mismatch flag(s) · 0 shared-title group(s)"
                                "[PASS] the installed library's own mismatch rate stays low"  <- VACUOUS PASS
shelf_suggest_smoke   exit=139  "held-out: 0 queries · top-1 0 (nan%)"
                                "[PASS] the real shelf leads the list for at least a
                                 quarter of held-out files (measured 31%)"       <- FABRICATED NUMBER
spine/lattice/reader/drills/terminology/quotation   exit=134 (uncaught std::runtime_error)
```

`core/tests/shelf_suggest_smoke.cpp:79-84` shows why: `CHECK(top1 * 100 >= queries * 25, "… (measured 31%)")` — vacuously true at `queries == 0`, and "(measured 31%)" is a **string literal in the assertion label**, printed on PASS regardless of what was measured. `catalog_qc_smoke`'s green all-clear over zero comparisons is the test-side twin of bounty finding #5.

Mitigating: `docs/DEVELOPER_ONBOARDING.md:24` does tell a newcomer to run `tools/build_spine.py` first — though its input is the external HGM data project on Adam's Desktop, and `library/` and `data/extracted/` get no such instruction. `docs/MAINTAINERS.md:27` says "ALL suites must pass" with no fixture caveat, and onboarding still says "55 suites" against 72.

**Impact.** The press's step-2 gate is the battery, and the battery is only meaningful on one laptop. On a clean clone the "72/72" would not mean what it says. `shelf_suggest_smoke` prints a fabricated success measurement over zero data — a house-rule-4 violation *inside the quality gate*.

**Recommendation.** Commit small real fixtures for the `library/` and `data/extracted/` cases so those suites run anywhere; label the genuinely corpus-scale ones so `ctest -LE corpus` is the CI gate and the press runs the full set. Make an absent fixture a hard **FAIL**, never a SKIP-that-exits-0. Replace hardcoded "(measured N%)" labels with the computed value and add `CHECK(queries > 0, …)` so ratio pins cannot go vacuous.
**Effort:** medium. **Ship-blocker: partly** — the two vacuous passes and the fabricated label are trivial and should go now; the fixture work can follow.

---

#### BUILD-4 · high · L2, the gate built to stop unmanifested payload, is a keyword match — and never inspects the payload that motivated it

**Verification:** confirmed, and the verifier found the blind spot is wider than reported.
**Location:** `tools/constitution_check.py:153-183`; `tools/package_macos.sh:151-222`.

**Evidence.** The rule maps each staged dir to a token and passes if the token appears **anywhere** in the notices: `credit = { … "extracted": "84000", … }`. So all of `data/extracted` (102 MB, 3,219 files) passes forever because the unrelated string "84000" occurs in `OPEN_SOURCE_NOTICES.md`. What that admits:

```
19M  thl_dege_records   (1,118 raw THL catalog HTML records, no provenance file beside them)
6.6M thl_lhasa_records      22M sanskrit_cluster_census.tsv
8.0M bdrc_toh_labels        7.4M wvpp_rows
```

Cross-checking the ~31 files under `data/extracted` that `app/` and `core/` actually open: **none of those five is read by the product**. `grep -rl` resolves every one of them only to ingest-side Python in `tools/`. That is ~63 MB of scraped/derived third-party material shipping to translators that the app never reads — exactly the class L1 stripped for `thl_dicts.jsonl` and `tm_84000.db`, missed here because the gate cannot see inside a credited directory.

Worse, the rule regexes only the press's `for d in …; do` loop. `grep -n 'PRESS_MODE' tools/package_macos.sh` → 24, 25, 26, 27, **163** — one mode guard in the entire press. Everything staged by explicit `cp` is invisible to L2, including `build/reference.db` (80,756,736 bytes) — and the rule's own comment reads *"Incident: reference.db and CC BY-NC models were shipping by habit."*

In fairness the rule is not toothless: a *new* directory added to the loop with no credit-map key does hard-fail. The defect is granularity and coverage, not absence.

**Impact.** The project identified unmanifested payload as a real incident, wrote a gate for it, and the gate covers neither the incident that caused it nor the largest unreviewed payload in the DMG. Bug-bounty theme 7 (tests pin substrings) reproduced in the licensing gate.

**Recommendation.** Invert L2: enumerate the staged tree after step 6 and require every top-level file and directory in the payload to match an explicit entry in a machine-readable manifest (path glob → licence → notices anchor), failing on anything unmatched. Drive the press's staging list *from* that manifest so a path cannot be staged without being manifested. Include the explicit-`cp` paths.
**Effort:** medium. **Ship-blocker: yes** (for any distribution beyond the immediate team).

---

#### BUILD-5 · high · Payload the notices say is "NOT redistributed" is redistributed in the DMG

**Verification:** confirmed, with one sub-claim refuted and two stronger pieces of evidence added.
**Location:** `docs/distribution/OPEN_SOURCE_NOTICES.md:42`; `tools/package_macos.sh:163-166`, `175-177`.

**Evidence.** Notices line 42: *"THL dictionary payload … LOCAL-ONLY display pending review — marked '(local only)' in the interface and NOT redistributed."* Actually staged:

```
dist/stage/Diamond Cutter Tool Data/build/reference.db     80,756,736 bytes
dist/stage/.../data/extracted/thl_dege_records             1,118 HTML records (19M)
dist/stage/.../data/extracted/thl_lhasa_records            6.6M
```

Stronger than the notices wording: `tools/build_reference_db.py`'s own docstring says of `reference.db`, verbatim, *"The DB never ships, never merges into HGM releases."* And `docs/SHIP.md` claims *"Market mode OMITS reference.db and the OCR models and stamps the README accordingly"* — but the README heredoc (`package_macos.sh:224-239`) has no mode conditional and `PRESS_MODE` never appears near it, so the README is **not** stamped. The generated `dist/stage/README.txt` is 8 lines with no confidentiality or non-redistribution notice, in a 424,670,599-byte file that can be forwarded to anyone. Market mode excludes exactly one payload.

**Refuted sub-claim:** "the 67 MB Das PDF ships with zero recorded provenance" is wrong. It is absent from `OPEN_SOURCE_NOTICES.md` (0 mentions, reproduced) but `docs/SHIP.md` carries `| data/das (Das 1902 PDF) | Published 1902 — public domain | SHIPS |`. The project does have a distribution manifest; it is `SHIP.md`.

**Impact.** The document governing redistribution is contradicted by the artifact it ships inside, and by the builder script's own header. A team member forwarding the DMG redistributes material the project declared non-redistributable, with nothing in the package saying otherwise.

**Recommendation.** Either mark `reference.db` and the THL record trees as team-only and put a plain non-redistribution paragraph in `README.txt` for team-mode DMGs, or complete the licensing review the notices say is pending. Make market mode actually exclude every layer whose terms are unresolved. Reconcile `SHIP.md`'s "stamps the README accordingly" with the heredoc that does not.
**Effort:** small. **Ship-blocker: yes.**

---

#### BUILD-2 · high · The licence notices name 6 software components; ~32 distinct upstream projects ship, with no LGPL or GPL text and no project LICENSE file

**Verification:** downgraded from critical; the headline metric was recomputed on an honest unit.
**Location:** `docs/distribution/OPEN_SOURCE_NOTICES.md` (Software components table); `dist/stage/…/Contents/Frameworks/`.

**Evidence.**

```
Mach-O objects shipped:  152   (13 Qt frameworks + 129 dylibs + 22 plugin dylibs)
Named in the notices:      6   (Qt 6, HarfBuzz, SQLite+FTS5, ONNX Runtime, Botok, tesseract)
Distinct upstream projects bundled (~76 libabsl_* collapsed to Abseil, 13 Qt frameworks to Qt): ~32
  of which named: 3 (Qt, HarfBuzz, ONNX Runtime)  ->  roughly 9% coverage
Licence/COPYING/OFL files in the DMG: 6  — all data-layer (whitney Apache, abbreviations CC0,
  three font OFLs, botok).  No LGPL text. No GPL text.
$ ls LICENSE* COPYING*   ->  no matches   (the project has no licence file)
```

Shipped and unnamed: `libgfortran.5`, `libquadmath.0`, `libgcc_s.1.1` (GPLv3-with-Runtime-Library-Exception, pulled in transitively — I traced `libopenblas.0.dylib → @executable_path/../Frameworks/libgfortran.5.dylib`), `libglib-2.0` / gobject / gio (LGPL-2.1), `libssl.3` / `libcrypto.3`, OpenCV, TBB, RE2, protobuf-lite, ICU, freetype, QtPdf/QtQml/QtQuick. The in-app "Licenses & Data Sources" item (`main.cpp:34072-34089`) simply renders that markdown, so it is the whole licence surface.

**Correction:** the audit's "3.9% coverage" counted 76 `libabsl_*` dylibs as 76 uncredited components. On the honest unit it is ~9% — still damning, not 4%. Also, the notices *do* give Qt LGPL v3 prominent notice; the gap is the missing licence **text**, which LGPL v3 §4 requires be supplied with the distribution.

**Impact.** ~29 of ~32 shipped upstream projects have no recorded licence, in a project whose stated first-order rule is that provenance is sacred. The distribution boundary is where provenance is thinnest.

**Recommendation.** Generate the notices instead of maintaining them by hand: walk the staged bundle, map each Mach-O to its Homebrew formula (`brew info --json` gives the SPDX licence), and **fail the press** if any shipped binary has no mapping. Bundle the actual licence texts under `Contents/Resources/licenses/` (Homebrew keeps most in the Cellar). Add a project `LICENSE`. Decide whether `libgfortran`/`libquadmath` need to ship at all — OpenBLAS can be built without the Fortran runtime, which removes the sharpest question.
**Effort:** medium. **Ship-blocker: yes** for distribution outside the team.

---

#### BUILD-6 · high · Zero dependency pinning and no CI: the release is a snapshot of one laptop's Homebrew

**Verification:** confirmed; every absence re-checked including untracked-but-present.
**Location:** repo root (no lockfile, no CI config); `ocr/CMakeLists.txt:4-9`, `24-25` (bare Homebrew paths).

**Evidence.**

```
Brewfile · Brewfile.lock.json · .tool-versions · requirements.txt · conanfile.txt · vcpkg.json  -> all absent
.github · .gitlab-ci.yml · .circleci · Jenkinsfile                                             -> all absent
$ git ls-files | grep -iE 'brewfile|\.github/|gitlab-ci|circleci|jenkins|\.lock|requirements'   -> (empty)
$ brew list --versions glib   ->  2.88.3  2.88.2      <-- TWO versions in the Cellar
bundled, unpinned: qt 6.11.1 · onnxruntime 1.28.0 · opencv 5.0.0 · openblas 0.3.34
                   libomp 22.1.8 · openssl@3 3.6.3   (libcrypto strings confirm OpenSSL 3.6.3)
```

Which glib `macdeployqt` copies depends on what `brew` has linked at press time. There is no record in the DMG of what was bundled, so a shipped OpenSSL CVE cannot be answered with "which version did release X carry?". No documented dependency or CVE process exists.

Separately and worth flagging on its own: **SQLite is not vendored.** `CMakeCache` has `SQLITE3_LIB=…/MacOSX.sdk/usr/lib/libsqlite3.tbd`, `otool -L` shows the app linking `/usr/lib/libsqlite3.dylib`, and `find . -name sqlite3.c` outside build dirs returns nothing — while `CLAUDE.md` states the architecture as "SQLite via vendored amalgamation (FTS5 compiled in)" and `docs/PACKAGING.md:101` says "SQLite vendored". On Windows, `find_library(sqlite3 REQUIRED)` fails and the documented fallback `FATAL_ERROR`s because the amalgamation was never committed. The stated cross-platform readiness rests on a premise that is false today.

Corroborating artifact ambiguity: `dist/` holds two DMGs both claiming 1.0.0-rc.1 (417,499,799 and 424,670,599 bytes), differing only by the product rename.

**Impact.** No two presses are guaranteed to produce the same artifact, security updates reach users only when someone happens to run `brew upgrade`, and the press has only ever run on the one machine whose Homebrew masked the OMP Error #15 bug.

**Recommendation.** Add a `Brewfile` and `brew bundle --file` in the press, and emit a **build manifest** into the DMG (formula → version → SPDX licence → SHA) generated from the staged bundle — that one artifact answers the licence question (BUILD-2), the CVE question and the reproducibility question at once. Stand up CI on a clean runner that at minimum configures, builds and runs the 35 fixture-independent suites. Either vendor the SQLite amalgamation as `CLAUDE.md` claims or correct both documents.
**Effort:** medium. **Ship-blocker: no** for a team DMG; **yes** before any second machine presses a release.

---

#### BUILD-1 · high · The shipped app is arm64-only, with a macOS minimum documented nowhere

**Verification:** downgraded from critical — the causal mechanism was refuted by construction.
**Location:** `CMakeLists.txt` (no `CMAKE_OSX_DEPLOYMENT_TARGET` / `CMAKE_OSX_ARCHITECTURES` anywhere); `Contents/Info.plist`.

**Evidence.**

```
$ lipo -archs .../DiamondCutterTranslationTool          ->  arm64
$ otool -l ... | grep -A4 LC_BUILD_VERSION              ->  minos 26.0   sdk 26.5
$ plutil -p Contents/Info.plist | grep LSMinimum        ->  "LSMinimumSystemVersion" => ""
$ grep -iE '^CMAKE_OSX_(DEPLOYMENT_TARGET|ARCHITECTURES|SYSROOT)' cmake-build-release/CMakeCache.txt
    all three EMPTY
$ grep -niE 'requires macos|system requirement|apple silicon|intel|universal'
      docs/distribution/*.md README.md docs/PACKAGING.md docs/RELEASE_CHECKLIST.md
    -> two hits, both about a "Universal Tibetan Font Converter"
  dist/stage/README.txt and INSTALL_QUICK_START.md have no requirements section at all.
```

**Refuted mechanism.** The audit claimed dyld refuses to load a binary whose `minos` exceeds the running OS. Tested by construction on this machine: a `-mmacosx-version-min=99.0` executable *and* a `minos 99.0` dylib both ran to exit 0; the linker emitted only a warning. So `minos` is not the runtime gate claimed, and macOS 15's dyld behaviour is unmeasured. What survives is unambiguous: **arm64-only** (there is no reverse Rosetta — no Intel Mac can run this), an empty `LSMinimumSystemVersion` so Finder cannot warn, and no stated requirement anywhere.

**Also correcting the fix:** 128 of the 129 bundled dylibs are themselves `minos 26.0` Homebrew Tahoe bottles, so setting a deployment target on the app alone would change nothing. A universal or lower-floor build means rebuilding the whole dependency stack — that is a project, not a fix.

**Impact.** The DMG cannot run on any Intel Mac, and the team includes input centres on unknown hardware. Because `LSMinimumSystemVersion` is empty, the user gets a non-launch rather than a system-requirements message.

**Recommendation (smallest change).** Populate `LSMinimumSystemVersion` and set `MACOSX_BUNDLE_GUI_IDENTIFIER` while you are in there (BUILD-15), state "Apple Silicon Mac, macOS 26 or later" in `README.txt`, `INSTALL_QUICK_START.md` and the release checklist, and have the press print the resulting `lipo -archs` and `minos` so a change is visible. A universal build is a separate decision with a real cost; do not conflate the two.
**Effort:** trivial for the documentation and plist; large for a universal build. **Ship-blocker: yes** for the documentation half.

---

### MEDIUM

Compact format. Every entry carries location, evidence, impact, fix, effort and verification status.

#### Testing and the gates

**TEST-7 · Cap-disclosure (house rule 3) is unpinned, and the pin that names the rule cannot fail.**
*Location:* `app/main.cpp:1539-1546` (the "%2 further definition%3 … not shown" block) vs the pins at `4135-4139` and `4152`.
*Evidence:* mutation `if (false && defsHeld > 0)` → `ctest_exit=0`, 100% of 72. The adjacent pins assert `card.count("class='g84def'") <= 4` and `>= 1` — both assert the cap, neither the remainder. The verifier found something sharper: `main.cpp:4152` reads `check(!card.contains("further definition") || card.contains("reference only"), "a held-back definition count is disclosed AND names the layer as reference only")` — an implication that is **vacuously satisfied the moment the disclosure is deleted**. A test that reads as enforcement and can never fail on the violation it names. Pinned term `bcom ldan 'das` carries 43 definitions, so 39 would vanish.
*Impact:* an inviolable rule has zero mechanical enforcement at the one site the suite claims to guard.
*Fix:* pin the pair everywhere a literal cap exists — `shown <= cap` **and** the rendered text contains the true remainder. One added line here: `check(card.contains("39 further definitions"), …)`. Sweep the other named caps (300 terms in `teachingsReportHtml`, the folio-batch report, HuntPalette's lanes).
*Effort:* small. *Verification:* downgraded from critical.

**TEST-8 · The suite is not hermetic: one `--selftest` run rewrites the developer's live application preferences.**
*Location:* `app/main.cpp:5710` (unguarded `setValue("overlay/lastFile")`), `5327-5344` (`g_harnessRun = false` mid-run), 20 `setValue` calls inside selftest bodies.
*Evidence:* `defaults read com.ALL.TranslationTool` before/after one selftest run → two real recents evicted, `overlay.lastFile` rewritten from a real Kangyur META to `/var/folders/…/T/all_selftest_wylie.txt`. The recents guard exists and is honoured at `5677` (`if (!g_harnessRun)`); `5710` has no guard. The session-restore pin does `g_harnessRun = false; restoreSession(); … g_harnessRun = guardKeep;` — disarming **every** harness guard in the process for its duration.
*Impact:* tests and product share one global mutable store, so the suite is order-dependent and cannot run in parallel with itself or a live app — exactly the fan-out pattern this project uses. `findDataRoot` reads `app/dataRoot` first, so a stale key could point `app_selftest` at a different checkout than the one just built. It also damages the translator's recents, which is the harm the guard was written to prevent.
*Fix:* under `g_harnessRun`, set a distinct `QSettings` organisation/application at startup so no harness run can touch `com.ALL.TranslationTool`; guard `5710` like its sibling; replace the global flag flip with a narrower seam (pass a bool into `restoreSession`).
*Effort:* small. *Verification:* reported.

**TEST-9 · Constitution rule R1 is blind to the commonest regex idiom in the file — 18 of 53 construction sites.**
*Location:* `tools/constitution_check.py:44-46` vs `app/main.cpp`.
*Evidence:* R3 works — adding one `QMessageBox::warning` produced `FAIL R3 app/main.cpp: modal call sites grew 130 -> 131`. R1 does not: adding a genuine collapsed-escape literal in the named-variable form `QRegularExpression re("\(x\)\s+");` produced `constitution: all rules hold`, exit 0. The detector matches only direct construction. Census: 35 direct `QRegularExpression("…")`, **18** named `QRegularExpression name("…")`. The incident R1 exists to prevent — the 84000 entity-link regex, repaired at `main.cpp:1522-1524` — is invisible in 34% of sites.
*Impact:* a gate that reports "all rules hold" while the exact incident class is unguarded in a third of the code is worse than no gate, because it is cited as proof.
*Fix:* widen to `QRegularExpression\s*(?:\w+\s*)?\(\s*"` (plus the `static const … name(` form), re-run over `app/` and `core/`, triage before bumping any baseline. Add the two probe files as fixtures so R1 itself has a test.
*Effort:* trivial. *Verification:* reported.

**TEST-10 · Pin composition: 57.6% are substring presence, `OverlayPane` holds 40% of them, two panes' only pins are tautologies.**
*Location:* the 217 `check()`/`lk()` sites in `app/main.cpp`; `ExportPane::selfTest` at 14586, `ScansPane::selfTest` at 14479.
*Evidence:* classifier over all 217 (local bools inlined 3 levels): substring/HTML-presence 85 (39.2%), substring+count 40 (18.4%), numeric/structural 41 (18.9%), other boolean 33, equality/value 18 (8.3%). Executed pin lines from a real run: 290. Per-pane: Overlay 115 · Catalog 30 · Library 29 · Lookup 12 · Draft 9 · … · Propose 1 · Export 1 · Scans 1. Thirteen of 21 panes have five or fewer. `ExportPane`'s entire test is `doc_ != nullptr && results_ != nullptr` — and the log line dereferences `doc_` regardless, so it can only PASS or crash. `ScansPane`'s is `!status_->text().isEmpty()`.
*Impact:* the 217/290 figure overstates the net. Over half the pins survive any change that keeps a string on the page, including every honesty regression that alters a label's meaning while keeping its words. `ExportPane` is the publishing station the bounty found skipping write-failure warnings on four paths.
*Fix:* retire the two tautologies and replace with contracts. New-pin rule: assert a value, a count or an absence — a `.contains()` on a label is acceptable only as a supplement.
*Effort:* medium. *Verification:* reported.

**TEST-11 · Test-only siblings: 69 `allcore` functions are exercised only by tests, and the sole pin on the provisional tier runs through a function the app never calls.**
*Location:* `core/tests/spine_smoke.cpp:45-63`; `core/tests/backup_smoke.cpp`.
*Evidence:* declaration scan across the 59 `allcore` headers (329 functions): used by app AND tests 179; **tests only 69**; used by app but absent from core tests 12 (7 also absent from `app_selftest`). `grep -c englishSearch app/main.cpp` → **0**, yet `englishSearch` is the only route by which `spine_smoke` exercises `provisional()`. `restoreBackup` and `latestBackup` are covered by `backup_smoke` and invoked by no app path. Confirmed and refined the bounty's outline example: `outline_smoke` drives `allcore::extractOutline` while the Overlay's own tree comes from the app-local `extractSaBcad` at `10057`.
*Impact:* MEM-1's catch by `spine_smoke` was an accident of a sibling entry point, not a design. Backup restore — a data-safety feature — is proven in the library and unreachable in the product.
*Fix:* publish the app-facing surface of `allcore` as an explicit list and require every entry to have a test entering through the same function the app calls. Decide for `restoreBackup`/`latestBackup` whether the feature is unfinished (file it) or dead (delete it and its suite).
*Effort:* medium. *Verification:* reported.

**TEST-12 · `docs/TEST_PLAN.md` certifies features against seven batteries that do not exist.**
*Location:* `docs/TEST_PLAN.md:1-18` and its per-feature table.
*Evidence:* of 21 named `*_smoke`/`*_battery` suites, seven are not registered anywhere: `acipfile_smoke`, `bampo_smoke`, `hypfile_smoke`, `ocr_ctc_smoke`, `segtrie_smoke`, `wilson_smoke`, `wilsondots_smoke` — `grep -rn` for each over `core app ocr tools CMakeLists.txt` returns 0 references outside `docs/`. The header says "runs in the 37-suite ctest" and "43 selftest checks" against a measured 72 suites and 290 executed pins, and asserts "The automation queue is empty as of 2026-08-09 — everything automatable is automated", which the six surviving mutations falsify. Several may be renames (`hypfile`→`hyp_smoke`, `segtrie`→`botok_tok_smoke`, `ocr_ctc`→`ocr_smoke`), unconfirmed feature-by-feature.
*Impact:* the document a reviewer consults to answer "is this feature covered?" cannot be checked against the build, and its ✅ marks are unverifiable.
*Fix:* generate the suite column from `ctest -N` plus the selftest's printed pin labels, and run that as part of the constitution gate so a battery named in the plan but absent from the build is a hard failure. At minimum correct the seven names and two stale counts now.
*Effort:* small. *Verification:* reported.

#### Memory safety

**MEM-3 · Two reproducible SEGVs: `allcore::Progress` builds `std::string` from a possibly-NULL `sqlite3_column_text`.**
*Location:* `core/src/progress.cpp:122` (`dueWords`) and `:218` (`recentKeys`).
*Evidence:* both do `out.push_back(reinterpret_cast<const char*>(sqlite3_column_text(s.p, 0)))` with no null check, against a schema (`progress.cpp:55-60`) that declares `key TEXT` with no NOT NULL. Under ASan, after inserting a NULL: `AddressSanitizer: SEGV on unknown address 0x000000000000 … #0 _platform_strlen #1 allcore::Progress::recentKeys progress.cpp:218` and the same at `:122`. Both are live app paths: `progress_->recentKeys("openfile", 10)` at `main.cpp:23571`, `progress_->dueWords(1, time(nullptr))` at `16999`. The correct twin is already written three times: `spine.cpp:13`, `refdict.cpp:31`, `tm84000.cpp:15-18` (`columnText()`).
*Impact:* `progress.db` is a plain user-writable SQLite file at the data root. One NULL from a hand edit, partial restore or sync conflict turns opening the Library pane or asking for a due word into an instant crash with no message.
*Fix:* hoist the existing `columnText()` helper into a shared internal header and use it at `progress.cpp:122`, `:204`, `:218` (and `libindex.cpp:178/400/409` as hardening). Add NOT NULL to `events.key` and `vocab.wylie` for new databases. Pin with a `progress_smoke` case that inserts a NULL row — a test that fails today.
*Effort:* trivial. *Verification:* reported.

**MEM-4 · `botok::ChunkFramework`'s public int-index API has no bounds check.**
*Location:* `core/include/allcore/botok.h:107-122` (nine overloads); `core/src/botok_chunks.cpp:370`, `227`, `252`, `414`, `562-564`.
*Evidence:* driving a 9-codepoint text under ASan+UBSan — `end=10`: `heap-buffer-overflow, READ of size 4 … isBoUnicode(int)`; negative `start`: `runtime error: applying non-zero offset 18446744073705551616 to null pointer`; `chunkString` with an out-of-range Chunk: `libc++abi: terminating due to uncaught exception of type std::out_of_range`. Enumerated over `{-1,0,8,9,10} × {-1,9,10,100}`: every `start<0` and every `end>len` aborts; in-range calls survive. Reachability today is limited — the app calls only the no-arg overloads, which pass `(0, len_)`.
*Impact:* a public header exposes ten entry points with unchecked raw indices and no documented precondition; the failure mode is a silent heap read or a hard `std::terminate`, not a refusal. One future caller off by one gets memory corruption instead of an error. No test probes any out-of-range index.
*Fix:* clamp at the entry point — `start = max(0, min(start, len_)); end = max(start, min(end, len_));` at the top of `chunkUsing`/`chunk`, and a clamped `substr` in `chunkString`/`readable`. Add a `botok_smoke` case hammering the `{-1, 0, len-1, len, len+1}` grid.
*Effort:* small. *Verification:* reported.

**MEM-5 · `ScanPane`'s OCR worker reads a `QCheckBox` from a non-GUI thread.**
*Location:* `app/main.cpp:26455` (the read); `26404` and `26411-26413` (the parameter and the thread start).
*Evidence:* `26404` captures the value correctly on the UI thread (`const bool deskewOff = deskewOverride_->isChecked();`) and passes it into the lambda — then `ocrWorker` reads the widget again at `26455` for the provenance line. `grep -n 'deskewOverride_'` returns 26164, 26165, 26169, 26404, 26455, 26591 — **no `setEnabled` anywhere**, so the user can toggle it while the worker reads it. Every other cross-thread touch in the same function is done correctly via `QMetaObject::invokeMethod(..., Qt::QueuedConnection)`. **Not dynamically confirmed** — TSan cannot combine with ASan and the path needs a page image plus the ONNX models through the GUI.
*Impact:* Qt documents `QWidget` as GUI-thread-only; this is a data race and therefore UB. The visible consequence is a provenance line claiming "(BDRC pipeline)" when the deskew override was applied, or vice versa — a statement about which pipeline produced the text.
*Fix:* one token — use the parameter that is already there: `.arg(deskewOff ? … : "(BDRC pipeline)")`. Disable `deskewOverride_` alongside `save_` for the run.
*Effort:* trivial. *Verification:* reported (static only).

#### Static analysis and compiler strictness

**STATIC-2 · The wall is honour-system: `-Wall` with no `-Werror`, no CI, and no warning gate anywhere.**
*Location:* `CMakeLists.txt:19-24`; `tools/constitution_check.py` (no warning rule); `tools/package_macos.sh:41-47`; `docs/TOTAL_QUALITY.md:5-29` vs `docs/TOTAL_QUALITY_PROMPT.md:43-47`.
*Evidence:* `add_compile_options(-Wall)` at line 23 is the only compile-flag directive in the entire tree. Zero `Werror` anywhere. No CI. `constitution_check.py` has exactly one hit for "warning", line 88, the unrelated modal census. `tools/verify.sh` tees the build log but greps only `error:`. The T1 directive said "-Wall -Wextra as the standing build, new warnings fail fast"; `-Wextra` was deferred — **and the deferral is written into both the ledger and `CMakeLists.txt:21`**, so the record matches the build. Measured cost of finishing: 15 warnings at 8 unique sites, all 8 verified false positives (`-Wmissing-field-initializers` on aggregates).
*Impact:* "the count is zero and stays zero" is an assertion with no mechanism. A warning introduced tomorrow scrolls past; ctest is green, the press packages, and the constitution — which encodes nine process lessons including the one a `-Wall` warning taught — has no rule for this one. Same shape as the stale-binary incident: a signal exists, nothing consumes it.
*Fix:* add `-Wextra -Wpedantic` (all 8 sites are false positives; rule the exception explicitly), an `ALL_WARNINGS_AS_ERRORS` option defaulting ON for the press build, and a constitution rule that greps the build log for `warning:`.
*Effort:* small. *Verification:* downgraded from high (the `-Wextra` half is disclosed and dispositioned; the no-gate half is novel).

**STATIC-3 · The About box's "72 automated test batteries" is a grep of CMakeLists, not a count of registered tests.**
*Location:* `app/CMakeLists.txt:3-16` (derivation); `app/main.cpp:33884` (the claim); `ocr/CMakeLists.txt:11-14` and `core/CMakeLists.txt:427-443` (the conditional tests it cannot see).
*Evidence:* the number comes from `string(REGEX MATCHALL "add_test\\(" …)` over the CMakeLists **files**. Baseline `ctest -N` → `Total Tests: 72`, matching. Without onnxruntime (an explicitly supported configuration — `allocr` `return()`s early): `Total Tests: 71`, compiled flag still `ALL_TEST_BATTERIES=\"72\"`. Without onnxruntime **and** Python3: `Total Tests: 69`, still `72`.
*Impact:* the comment says the derivation exists "so the About line can never go stale again (it drifted twice at 39)" — the fix removed one drift mode and installed another. Losing Python3 silently drops the **constitution gate itself**, and the About box still promises 72.
*Fix:* count through a project `add_test` wrapper that increments a counter, so conditional tests are excluded automatically; or bank `ctest -N` output at press time. Failing both, `FATAL_ERROR` if Python3 or onnxruntime is missing when packaging.
*Effort:* small. *Verification:* reported.

**STATIC-4 · The About box's RELEASED date is `__DATE__`, guarded only by a comment whose invariant the build system does not hold.**
*Location:* `app/main.cpp:33836-33841` and `:33850`; `tools/package_macos.sh:41-47`.
*Evidence:* the comment says "every press rebuilds main.cpp, so it tracks presses". Proved false in a private copy: appending a comment to `core/src/spine.cpp` and rebuilding the app target recompiled **only** `spine.cpp` and relinked a new binary while `main.cpp.o` — which carries the baked `__DATE__` — stayed at the previous timestamp. A `VERSION` bump *does* force it. The press never cleans and supports `--skip-build`. The date is readable in the shipped artifact (`strings -a` → `Aug 22 2026`). `-Wdate-time` would flag both sites and is in neither `-Wall` nor `-Wextra`.
*Impact:* for a same-version re-press — a core-only bug fix, exactly what this assessment will produce — the About box states a RELEASED date earlier than the build the user is running, and no test can notice because the value is baked at compile time. It also makes the binary non-reproducible by construction.
*Fix:* stamp the date from CMake (a `RELEASE_DATE` line in `VERSION`, or the git committer date of HEAD) passed as a compile definition and added to the existing `CMAKE_CONFIGURE_DEPENDS` list — the pattern that already works for the version string. Then enable `-Wdate-time`, which costs 0 once these two are gone.
*Effort:* small. *Verification:* reported.

**STATIC-5 · Unbounded recursive descent in the Gofer query parser: SIGSEGV at ~45,000 nesting depth, and `gofer.h` documents an exception it cannot throw.**
*Location:* `core/src/gofer_ast.h:95-142`; `core/src/gofer.cpp:68-79` (parse before validation) and `:26`; `core/include/allcore/gofer.h:28`; same pattern at `core/src/botok_trie.cpp:45/89/105`.
*Evidence:* `clang-tidy misc-no-recursion` flags `parseOr`/`parseNear`/`parseTerm`/`evalNode`; `parseTerm`'s LPAR branch has no depth counter. Bisected against the real `liballcore.a`: depth 40,000 → exit 0; 45,000 / 48,000 / 50,000 / 60,000 → **exit 139 (SIGSEGV)**, with `ulimit -s` = 8176 KB. The parse runs **before** `if (!fs::exists(root)) throw` — confirmed empirically: depth 10,000 against a nonexistent dir threw the directory error; depth 50,000 segfaulted first. The header says "throws std::runtime_error on malformed queries." Not reachable through today's UI: `buildQuery()` quotes every field so user parens lex as literal text, and the one raw-text call site is a `QLineEdit` whose default `maxLength` of 32767 caps depth at ~16,383.
*Impact:* a public entry point crashes the process on input its own header promises to reject. The guard is accidental (a Qt default and a quoting choice), not designed, and is exactly what a future "advanced query" box or a scripted consumer removes.
*Fix:* a depth counter in `gofer_ast::Parser` checked against a named constant, throwing "query nested too deeply"; the same for `JsonParser::value/object/array`; move `fs::exists(root)` above the parse; set an explicit `maxLength` on the query line edit so the accidental guard becomes deliberate. Pin at depth 1,000 asserting a throw.
*Effort:* small. *Verification:* reported.

**STATIC-6 · No analysis apparatus is installed or configured, and the sanitizer gap is free to close.**
*Location:* `CMakeLists.txt:17` (unused `compile_commands.json` export); repo root (no `.clang-tidy`); `tools/` (no analysis driver among 81 scripts); `docs/TOTAL_QUALITY.md` (no T2 entry).
*Evidence:* `which clang-tidy` → not found; nothing in Xcode or Homebrew; no `cppcheck`, `include-what-you-use`, `scan-build`; no `.clang-tidy`; `CMAKE_EXPORT_COMPILE_COMMANDS` is ON and **nothing reads the result** (`grep -rn compile_commands tools/ docs/ .claude` → nothing). When the tooling *is* asked (LLVM 22.1.8 installed into a scratch venv): 310 checks, 128 of them analyzer; 514 diagnostics in core+ocr, 413 in `main.cpp`, 0 analyzer findings. Sanitized subset built and run with **zero source changes**: 8/8 pass in 89.63 s.
*Impact:* the quality doctrine leans on mechanical enforcement (nine constitution rules, a modal census, a link-truth checker, an honesty sweep) yet the two standard C++ instruments are absent. Bounty #10 was a 64-slot buffer overrun reachable from six library texts in this repo — an ASan finding by construction, found by human audit instead. The T2 sanitizer wave has no ledger entry while task 15 is marked complete.
*Fix:* commit a `.clang-tidy` (bugprone-*, clang-analyzer-*, cert-*, performance-*, misc-*, plus the `bugprone-unused-return-value` `CheckedFunctions` list from STATIC-1) so the exported `compile_commands.json` becomes load-bearing and today's 927 diagnostics become a ratcheting baseline; add `ALL_SANITIZE` (MEM-2); record T2's actual state in `TOTAL_QUALITY.md`.
*Effort:* small. *Verification:* reported. Note for whoever does it: clang-tidy 22 needs `--extra-arg=-isysroot <sdk> --extra-arg=-nostdinc++ --extra-arg=-isystem <sdk>/usr/include/c++/v1` on this machine.

#### Failure paths

**FAIL-5 · 22 of 26 production `write()` returns and all 9 `QTextStream` writes are discarded; nine success strings are printed by paths that verified only the open.**
*Location:* `app/main.cpp` — worst sites `28040` (`ManuscriptPane::save`), `23503` (spine pointer), `29600`, `29407`, `18752`, `24526`, `26675`, `27678`, `3348-3353`.
*Evidence:* statement- and receiver-aware census, selfTest bodies and `main()` excluded — `open_write` 27 sites / **0 discarded**; `write` 26 / 22 discarded; `mkpath` 26 / 24; `remove` 15 / 15; `QTextStream` over `QFile` 9 / **0 status checks**. Against 13 `saveOrWarn` and 13 `warnWriteFail` call sites. `ManuscriptPane::save` additionally clears `dirty_`, defeating the unsaved-changes guard. The spine pointer case is `if (ptr.open(...)) ptr.write(...)` with **no else at all**, followed unconditionally by "Release %1 imported — the spine %2 is built and selected" — and `resolveSpinePath` (`main.cpp:1755-1767`) really does read that pointer at launch and silently fall back to the pinned default.
*Impact:* nine lanes print success on a path that proved only that a file could be opened.
*Correction (verifier):* 7 of the 9 lanes **do** check the open and warn or return an explicit failure string; they lie only on a short write. `3348` prints no success claim on failure (the " · saved" suffix is simply omitted) — silent, not falsely successful. And 27 of 27 production `open(WriteOnly)` returns *are* consumed, which catches the common failures.
*Fix:* convert the 22 to `saveOrWarn`, or to the `writeAllOrWarn` helper proposed in DATA-4 where streaming is genuinely needed; gate each success string on the return.
*Effort:* medium. *Verification:* downgraded from high.

**FAIL-6 · The collection update check reads its own 15-second timeout as "the website answered" and reports a phantom "UPDATE AVAILABLE".**
*Location:* `app/main.cpp:22486-22492` (`await`), `22527-22582` (the row loop).
*Evidence:* Qt probe reproducing the exact `await` idiom against an endpoint that sleeps 600 s — timed-out HEAD: `isFinished() = false`, `error() == NoError = TRUE`, `ETag = ''`. The caller treats `NoError` as answered, sets `row.live = true`, and with a stored ETag the `known == row.etag` test fails, so the branch taken is *"UPDATE AVAILABLE — the website's copy changed since yours was installed."* The stored ETag is written at `22766` after a successful in-app install, so the exposed user is one who installed through the app. The module's own comment reserves `live` for "a HEAD that answered during THIS session".
*Impact:* a slow link or captive portal makes the app assert in the present tense that a new release exists and offer a multi-hundred-megabyte download. The honesty mechanism the designers built is defeated by the one path that feeds it. Medium, not high: a refused, 401'd or reset HEAD errors and lands correctly on the honest branch, and the worst outcome is a wasted download the user chooses.
*Fix:* make `await` return `r->isFinished()` and treat `!finished` as a third state — the honest text for it already exists twelve lines below ("the website did not answer this row"). Leave `row.live` false. Call `req.setTransferTimeout(20000)` so a stall errors rather than dangles.
*Effort:* small. *Verification:* downgraded from high.

**FAIL-7 · 42 of 43 JSON parses are unchecked, so an HTTP 200 carrying HTML is reported as "no instances matched — edit the title and search again".**
*Location:* `app/main.cpp:8451`, `8518`, `8631` (network); 43 `fromJson` sites, one `QJsonParseError` check at `22218`.
*Evidence:* re-counted at HEAD: `QJsonDocument::fromJson` **43**, `QJsonParseError` **2** (one declaration, one real check). Probe against a local fault server — body `<html>…Sign in…</html>` at HTTP 200: `error()=0 (NoError)`, parse "illegal number", `object().isEmpty()=true`, rows seen by app **0**. Truncated JSON at 200: `error()=0`, "unterminated object", rows **0**. The one correct twin at `22217-22226` is textbook.
*Impact:* house rule 2 at the network boundary. A captive portal, WAF challenge or dropped connection is reported to the translator as a factual negative about the corpus — "no instances matched" (blaming their query) or "BDRC lists no scan volumes" (blaming the catalogue). The same silent-empty applies to 40 on-disk parses. Medium rather than high because the harm is a misleading empty state, not lost work or forged authority, and the real BDRC failure mode today is a 401 that `error()` catches honestly.
*Fix:* pass a `QJsonParseError` at all 43 sites — mechanical — and branch. For the three network sites the message must distinguish "the server answered with something that is not JSON" from "the server answered with zero results", and include `pe.errorString()` plus the first 120 bytes of the body.
*Effort:* medium. *Verification:* downgraded from high.

**FAIL-8 / PERF-9 · No network transfer timeout is ever set, and one blocking network wait is genuinely uncancellable.**
*Location:* `app/main.cpp:8944-8951` (`downloadAllFolios`, per-folio `QEventLoop`); correct sibling at `13111-13116`.
*Evidence:* `grep -c setTransferTimeout app/main.cpp` → **0**; measured default `nam.transferTimeout()` = 0, and a GET against a 600-second black hole was still unfinished after 8.4 s. `downloadAllFolios` creates a fresh `QEventLoop` per folio that exits **only** on `finished`, while `pd.wasCanceled()` is consulted between folios — so the user's click is recorded but cannot end the current wait.
*Correction (verifier):* the audit's "4 of 5 waits have a Stop that does nothing" is refuted. `main.cpp:450` connects `canceled → QNetworkReply::abort`, `13115` connects `canceled → QEventLoop::quit`, `22748` connects `canceled → abort` — each measured exiting in ~2.1 s. **One** lane is affected, not four.
*Impact:* one unreachable folio server hangs the application indefinitely with a progress dialog whose Cancel cannot fire. On a monastery link this is the routine failure.
*Fix:* `nam_.setTransferTimeout(30000)` on each manager (or per request) so a stall becomes `OperationCanceledError` the existing error branches already handle, and connect `pd.canceled` to `rep->abort()` at `8945` to match the sibling twelve pages later.
*Effort:* trivial. *Verification:* downgraded from high.

**FAIL-9 / PERF-13 · "Extract all" always says it succeeded; 13 of 13 `waitForFinished` bools discarded, 1 of 13 `exitStatus` checked.**
*Location:* `app/main.cpp:20779-20792` ("Extract all"), `22413-22440` (`installZipPath`), `23003-23006` (UTFC), `28065-28070`.
*Evidence:* `x.waitForFinished(120000);` followed unconditionally by `QMessageBox::information(this, "Extract", "Extracted into " + dst + " (existing files were left untouched).")` — no check of any kind. Measured `QProcess` semantics: a process that outlives `waitForFinished` returns `false` (discarded) while `exitCode()` reads `0`, so `ok = p.exitCode() == 0` evaluates TRUE. `grep -c waitForFinished` → 13; `grep -c exitStatus` → 1 (the correct twin at `23492`). Twelve sites total up to ~21 minutes of potential GUI-thread blocking; the two multi-minute ceilings are `22415` (10 min) and `23006` (5 min), both with a static italic sentence and no cancel.
*Corrections (verifier):* `installZipPath`'s `exitCode()` check **does** catch corrupt archives (unzip 9/2), read-only targets (50) and full destinations (50) — only an unzip that outruns the ten-minute wait escapes. `exportRtf`'s failure branch fires correctly because an absent binary yields exit 255. **Confirmed:** the "%1 file(s)" figure is a `QDirIterator` count over the whole destination, so it includes files from a previous install and is not a count of what arrived.
*Impact:* an unzip that outruns its wait prints "Installed." and writes an ETag fingerprint that makes the update checker report the collection current forever. "Extract all" reports success unconditionally. Force-quitting mid-unzip leaves a partial collection the next index build happily indexes.
*Fix:* `if (!p.waitForFinished(ms) || p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0) { report with p.errorString() and the tail of readAllStandardError(); return false; }` at all sites; count files before and after for the delta; convert the two multi-minute waits to the async `QProcess::finished` signal with a `QProgressDialog` and kill-on-cancel — the download code at `444-460` already shows the pattern.
*Effort:* small. *Verification:* downgraded from high.

**FAIL-10 · `importPaths` lists every file as imported without checking `QFile::copy`.**
*Location:* `app/main.cpp:22804`, report at `22826-22827`. *Evidence:* `QFile::copy(f, target); report << QFileInfo(target).fileName();` then `"<b>Imported into library/my_materials/:</b>"`. Census: 6 production `QFile::copy` sites, 2 with the bool discarded (22804, 19791); four other sites check it. *Impact:* an unreadable source, a full volume or a permissions refusal produces a list of filenames under "Imported" for files that are not there; the user cannot find their own material and has no error to act on. *Fix:* `if (!QFile::copy(f, target)) { failed << fi.fileName(); continue; }` and render the failures with `QFile(f).errorString()`, mirroring `folioBatchReport`. *Effort:* trivial. *Verification:* reported.

**FAIL-11 · The move-to-shelf rollback discards its own rename's bool while the identical rollback 350 lines later reports its failure.**
*Location:* `app/main.cpp:29310-29314` vs the correct twin at `29657-29665`. *Evidence:* `QFile::rename(destPath, srcFile);` with the bool discarded, then `return "the META companion could not move — the pair stays together in intake";`. The twin returns "The rename could NOT be undone — the file is now at X with no companion." Census: 9 production `QFile::rename` sites, 8 checked. *Impact:* when the undo itself fails, the main text is on the shelf and its META is still in intake while the message says they are together — the split-identity state the check exists to prevent is created **and** denied in one sentence. *Fix:* capture the undo's bool and copy the twin's message. Two lines. *Effort:* trivial. *Verification:* reported.

**FAIL-12 · A failed scan-volume manifest is dropped silently and the partial folio map is printed as the total.**
*Location:* `app/main.cpp:8626-8663` (no else branch), caption at `8606-8616`. *Evidence:* the harvest is inside `if (rep->error() == QNetworkReply::NoError) { … }` with no else and no counter, then the caption reads "Scans: Buddhist Digital Resource Center · %1 folio sides mapped". The live chain is multi-volume — MW22084 returns 14 sub-collections and 103 manifests — so a partial harvest is the normal failure shape. The correct twin is 300 lines away at `13124-13130` (`else ++fetchFails;`). *Impact:* house rule 3 — a partial count printed as a total. If three of fourteen volumes fail, the translator is told the edition is mapped and every `@folio` marker in a missing volume silently resolves to nothing. *Fix:* count manifest failures and print them: "N folio sides mapped from M of K volumes — K−M volume(s) did not answer; those folios will not resolve." Reuse the `folioBatchReport` shape. *Effort:* small. *Verification:* reported.

**FAIL-13 · A model stream that dies mid-report is rendered as a finished analysis, QC'd, and auto-saved with a "· saved" stamp.**
*Location:* `app/main.cpp:3331-3356`; same conjunction at `19088-19090`. *Evidence:* `if (reply_->error() != NoError && accum_.empty()) { status error } else { render; validateReport; mkpath; if (f.open(...)) { six writes discarded }; saved = " · saved …" }`. The `&& accum_.empty()` means any partial stream — a reset after three paragraphs, a proxy cutting the SSE — takes the success branch. *Impact:* a truncated analysis is presented as the finished article and `allcore::validateReport` runs QC over it, laundering the truncation into a green-looking verdict on a report that stops mid-sentence; it is then auto-saved, and if that save failed the stamp simply does not appear with nothing said. *Fix:* split the conditions — on `error() != NoError` with a non-empty accumulator, render under an explicit "INCOMPLETE — the connection failed after N characters; this is a fragment, not an analysis" banner, skip or label the QC verdict, and do not auto-save without the banner in the file. *Effort:* small. *Verification:* reported.

**FAIL-14 · 24 of 26 `mkpath` bools and all 15 `QFile::remove` bools are discarded, so writes proceed into directories that were never created.**
*Location:* `app/main.cpp` — mkpath at 512, 615, 2647, 3343, 8493, 9781, 13084, 13223, 20323, 20510, 20764, 21025-21027, 22409, 22793, 24492, 24512, 25718, 25833, 26558, 26663 (+3); the one checked twin is the bounty-#9 fix at `8919`. *Evidence:* that twin's comment states the lesson explicitly — *"the folder has to exist before a single byte is fetched: mkpath's bool was discarded, so a read-only dataRoot_ (a user-settable override) pulled the whole volume over the network and then reported nothing wrong."* Applied at one site out of twenty-six. The override is real and user-settable at `main.cpp:30952`. *Impact:* every downstream `open()` in those lanes then fails for a second-order reason and lands in whichever FAIL-5 site is next — mostly a success string. The 15 unchecked removes matter most in rollback paths (8830, 8866, 8971, 22762, 29679), where a failed cleanup leaves a truncated file the next run scores as valid cached content. *Fix:* `if (!QDir().mkpath(p)) { warn naming p; return; }` at the 24 sites, exactly as `8919` does; where a failed remove would be read back as valid (folio and volume caches), abort the optimistic path. *Effort:* medium. *Verification:* reported.

**FAIL-15 · The process gate encodes nine rules and none is about I/O return values.**
*Location:* `tools/constitution_check.py` (R1-R6, C2, L2, L3). *Evidence:* `python3 tools/constitution_check.py .` → `constitution: all rules hold`. Reading the file: R1 regex escapes, R2 harness-flag list, R3 modal census with a baseline, R4 forbidden inks, R5 `proposals.tsv` trunc discipline, R6 drill cleanup, L2 payload manifest, L3 backlog reconciliation, C2 press gates. Not one inspects an `open`/`write`/`rename`/`mkpath` return. The file's own preamble says *"A lesson in a ledger is a hope; a lesson in a gate is a guarantee"* — and the family that cost three of the bounty's top ten is in the ledger, not the gate. Meanwhile `warnWriteFail`'s own comment claims "One voice for every write failure" and it is honoured at 13 of 53 write-side sites. *Impact:* nothing stops the counts in FAIL-5 and FAIL-14 from growing back after they are fixed. *Fix:* add R7 in R3's exact shape — count production sites where a write-side return is discarded, store the number in `tools/constitution_baseline_io.txt`, fail when it grows, auto-lower when it shrinks. Ship it at today's number so it ratchets down rather than blocking the fix. *Effort:* small. *Verification:* reported.

#### Performance

**PERF-4 · The only perf guard on the document pipeline is silently skipped because its fixture does not exist, and the suite still reports ALL PASS.**
*Location:* `app/main.cpp:5615-5645`; fixture `library/acip_release6/TD10199_T.TXT`. *Evidence:* every selftest run prints `[info] Overlay: volume file absent — perf guard skipped` and then `app selftest: ALL PASS (0 failure(s))`; `test -d library/acip_release6` → DIR MISSING. The two disabled assertions (`tokens.size() > 100000`, `ms < 15000`) are the only automated statements about document-pipeline speed. The guard's own comment hands the display path to a manual checklist. *Impact:* zero automated coverage of document-pipeline performance, presented as a passing suite — the self-disabling test the bounty named. *Correction:* the guarded operation measures 699.9 ms against a 15,000 ms ceiling (21× headroom), so even a working guard is a very loose net; the exposure is "a slack guard is absent", not "a tight guard is absent". *Fix:* commit or generate a canonical volume fixture, or make an absent fixture a FAIL rather than an `[info]`. The suite must not be able to print ALL PASS while a named guard is inert. *Effort:* small. *Verification:* downgraded from high.

**PERF-6 · No worker-thread facility for compute, and zero busy-cursor affordance in 37,493 lines.**
*Location:* `app/main.cpp` (whole file); the single worker is `ocrThread_` at `26411`/`26694`. *Evidence:* re-measured at HEAD — `QtConcurrent` **0**, `moveToThread`/`QRunnable`/`QThreadPool` **0**, `QFutureWatcher` **0**, `setOverrideCursor` **0**, `QProgressBar` **0**, `QProgressDialog` 13, `std::thread` 1 worker, `QDirIterator` 23 sites, `processEvents` 31 sites. Every measurement in this report exceeding 2 s runs on the GUI thread. *Correction (verifier, required before quoting):* the audit's claim that all 13 `QProgressDialog`s are on network downloads is **false**. Five are compute-bound — `12944` "Making pechas…", `23469` "Building the spine database…", `26043` "Scanning pages for illustration candidates…", `26559` "Batch OCR — N page(s)", `31900` "Surveying the text…" — four of them with a working Stop, per-item `setValue` and `wasCanceled()` inside the loop. The true finding is narrower: the pattern exists and works, and was not applied to the four heaviest operations. *Impact:* there is no architectural place to put slow work, so every new slow feature lands on the GUI thread by default, and with zero `setOverrideCursor` every stall presents as an unexplained freeze. *Fix:* one facility (`QtConcurrent::run` + `QFutureWatcher`), route the four heavy operations through it, and adopt a rule with a test: any `allcore` call whose measured p95 exceeds ~200 ms at real scale runs off the GUI thread or behind a busy cursor. *Effort:* large. *Verification:* downgraded from high, with a refuted evidence claim struck.

**PERF-7 · Three separate places claim library search answers "instantly"; measurement refutes all three.**
*Location:* `core/include/allcore/libindex.h:3`; `README.md:36`; `app/main.cpp:23526`. *Evidence:* measured against the real 2.36 GB index — `PA` 1,960 ms warm / 18,828 ms cold; `PA NEAR/3 PA` 54,133 ms; `SANGS RGYAS` 229-819 ms; `BSOD NAMS` 60-147 ms. And the "instead of re-reading every file" half inverts for common terms: `PA` indexed 2,382/1,894/2,489 ms vs live scan 1,571/1,715/1,549 ms — the index **loses**. The `main.cpp:23526` string is printed unconditionally on a successful `update()`; no timing is taken anywhere in `updateIndex()`. *Impact:* house rule 4 in its exact terms — a success string asserting a property the code that printed it never verified. A user who runs the three-minute index build on that promise and then waits two seconds (or eighteen, or fifty-four) has been told something untrue by the software about itself. *Fix:* delete "instantly" from all three places; replace the post-index message with something the code produced (the observed time of a probe query), or state the honest shape. Earn the claim back by fixing PERF-1 and pinning it. *Effort:* trivial. *Verification:* reported.

**PERF-8 · First use of the Command Palette freezes the GUI for 1.47 s inside a 220 ms debounce handler.**
*Location:* `app/main.cpp:31772-31786`; `10751-10772` (`libTitleIndex`); `10841-10850` (citation match loop). *Evidence:* first-use library walk **1,470 ms** (9,541 paths), then 12-24 ms per keystroke against a 220 ms debounce — the steady state is fine, the one-off walk is not, and it happens inside `runSearch()` on the user's second typed character. `libTitleIndex()` build 453 ms (8,988 files, 7,097 title entries); the citation match loop runs the whole index per citation with two `QString::contains` tests each — 965 ms at N=2000 (14,194,000 title tests). Raw walk costs for the other GUI-thread `QDirIterator` sites: `ensureFileIndex` 398 ms, `fillList` 579 ms total (456 build + 122 `resizeColumnsToContents`). *Impact:* a command palette's entire value is that it appears instantly; the first use teaches the user it is slow. All figures are warm local SSD — the library root is user-selectable, so a Dropbox or SMB mount multiplies every walk by an unmeasured factor. *Fix:* kick the palette's walk off at idle after startup and cache it once process-wide instead of per-pane — `ensureFileIndex`, `libTitleIndex`, `fillList` and the palette all walk the same tree with different filters. Index the 7,097 titles by leading token so the per-citation cost is a hash probe. *Effort:* small. *Verification:* reported.

**PERF-10 · Idle footprint is 441 MB, peak 981 MB, and the app depends on a 2.36 GB index staying in page cache.**
*Location:* whole application; `core/src/libindex.cpp` (schema). *Evidence:* max RSS under `QT_QPA_PLATFORM=offscreen` — startup 440,582,144; sweep Overlay 797,786,112; gauntlet 672,808,960; selftest cold 981,155,840. Index amplification 642,907,695 → 2,357,227,520 bytes (**3.67×**). Page-cache sensitivity on the same query and file: fully resident 1,960 ms; partially evicted 5,169 / 7,507 / 8,906 ms; first touch 18,828 ms — a 10× swing purely on residency. Offscreen rendering **excludes** real font rasterisation and compositing, so on-screen figures are higher. *Impact:* on the 64 GB development machine the index stays resident and searches look tolerable; on a translator's 16 GB laptop the 2.36 GB index is routinely evicted, making the 9-19 s cold path the common path, and a NEAR query's multi-GB transient forces swapping. No stated memory budget, no test asserting one. *Fix:* state a budget and pin it (max RSS after the selftest is a one-line ctest assertion). Reduce the 3.67× amplification — the `lines` table stores full text alongside the FTS5 index, so raw text is effectively stored twice; `content=''` with on-demand re-read would cut it substantially. Most of all, fixing PERF-1 removes the need for the whole index to be resident. *Effort:* medium. *Verification:* reported.

#### Build, release and distribution

**BUILD-3 · The printed first-launch instruction is the pre-Sequoia Gatekeeper bypass, and the DMG itself is unsigned.**
*Location:* `tools/package_macos.sh:136-149`, README step 2 generated at `224-239`. *Evidence:* `codesign -dv` → `flags=0x2(adhoc)`, `TeamIdentifier=not set`, no entitlements; `spctl -a -vvv -t exec` → rejected (also with a quarantine xattr applied); `xcrun syspolicy_check distribution` → "Notary Ticket Missing — Severity: **Fatal**"; `codesign -dv <dmg>` → "code object is not signed at all"; `security find-identity -v -p codesigning` → 0 identities. Generated README step 2: *"First launch: RIGHT-CLICK the app and choose Open"* — the bypass Apple removed in macOS 15, while `CLOSER` #16 (T9, CLOSED) asserts "README's Gatekeeper claims true". *Impact:* the printed remedy does not work on any supported OS; users must use System Settings → Privacy & Security → Open Anyway, which the README does not mention. The unsigned DMG is rejected before the app is reached. **The notarisation gap itself is already tracked** (`CLOSER` #27 WAITING on a $99 decision, #10 CLOSED, `PACKAGING.md`, `SHIP.md`) — only the stale instruction and the unsigned DMG are new. *Fix:* correct the instruction the press prints to the Settings flow and state that the DMG is unsigned. *Effort:* trivial. *Verification:* downgraded from critical; core already known.

**BUILD-8 · A `codesign --verify` failure does not stop the press.**
*Location:* `tools/package_macos.sh:149`. *Evidence:* the line is `codesign --verify "$STAGE/$APPNAME.app" && echo "   signature ok"`; under `set -euo pipefail` a command that is the left operand of `&&` is exempt from `set -e`. Reproduced with the identical construct: the script continues and exits 0. Line 145 (the signing itself) **is** fatal but sends its stderr to `/dev/null`, so the reason is discarded. *Impact:* verification failure does not stop the press; it proceeds to build the DMG, rsync over `/Applications` and relaunch, the only signal being the absence of one line in a long log. *Fix:* `codesign --verify --strict --deep … || { echo 'SIGNATURE VERIFY FAILED - press stopped'; exit 1; }` and drop the `2>/dev/null` on 145. Audit every other `&&`-guarded check in the script. *Effort:* trivial. *Verification:* reported.

**BUILD-9 · The visual-regression gate silently passes when the blessed set is absent — and `build/` is gitignored.**
*Location:* `tools/shot_diff.py:31-34`; `tools/package_macos.sh:56-60`. *Evidence:* `if not os.path.isdir(blessed) or not os.listdir(blessed): print("… skipping"); return 0` — measured exit 0 for both an empty and a missing directory. The blessed set is 22 PNGs living only under gitignored `build/`. Separately, `package_macos.sh:58` ends the screenshot producer with `|| true`, discarding its exit code, and an unbaselined pane is a print-only note. *Impact:* on any machine other than Adam's the press's visual gate is a no-op that returns 0 while the log implies coverage; an app that crashes after emitting the 22 blessed panes also passes. *Fix:* version the baseline (22 small PNGs) or its hash manifest, make an absent baseline a hard failure unless `--bless-missing` is passed, capture the screenshot run's exit code, and make a new unbaselined pane fail with "bless to adopt". *Effort:* small. *Verification:* reported.

**BUILD-10 · C2, the gate that checks the press kept its gates, is a substring test that cannot detect a disarmed gate.**
*Location:* `tools/constitution_check.py:184-193`. *Evidence:* the rule is `if gate not in press: fails.append(...)` for the three strings `ctest`, `shot_diff.py`, `constitution_check.py`. Demonstrated against a press in which every gate is disarmed with `|| true`: all three strings are present, so C2 reports PASS. This is not hypothetical — `package_macos.sh:58` already appends `|| true` to the screenshot producer and line 149's verify is already non-fatal (BUILD-8), and `constitution_check.py` reports "all rules hold". *Impact:* deleting a gate is caught; disarming one is not — and disarming is what a developer does to get an urgent press out. *Fix:* make C2 execute rather than grep — run the press in a gate-probe mode against a deliberately failing fixture and assert a non-zero exit at each gate. *Effort:* medium. *Verification:* reported.

**BUILD-11 · The armed notarisation path will not work as written when the Developer ID arrives.**
*Location:* `tools/package_macos.sh:140-143`, `331-344`. *Evidence:* the armed branch is `codesign --force --deep --options runtime -s "$ALL_DEV_IDENTITY"`. `find . -name '*.entitlements'` → nothing exists. The bundle carries `QtQml`, `QtQmlMeta`, `QtQmlModels`, `QtQmlWorkerScript` and `QtQuick`. The DMG is submitted to `notarytool` unsigned (measured: "code object is not signed at all"). *Impact:* three known-bad ingredients are pre-committed — `--deep` is deprecated for distribution signing and does not apply entitlements to nested code (a common rejection for a 152-object bundle); `--options runtime` enables the hardened runtime with no entitlements while the QML/Quick JIT needs `com.apple.security.cs.allow-jit`; and the DMG is unsigned. The moment the external blocker clears, the press fails in a way that looks like an Apple problem. *Fix:* replace `--deep` with an inside-out signing pass driven by the same `find` loop step 4b already uses; add an entitlements plist and pass `--entitlements`; sign the DMG before submitting. Dry-run the whole path now against an ad-hoc identity. *Effort:* medium. *Verification:* reported.

**BUILD-12 · Step 0's "release gate" validates the source JSON against itself and says nothing about the 249 MB database that ships.**
*Location:* `tools/package_macos.sh:36-39`, `151-162`; `tools/validate_release.py:49-51`. *Evidence:* with no `--master` argument the script compares `BANKED_MASTER` to itself, so the no-shrink check is a tautology (`[ok] entries: 105,634 → 105,634`). Its closing line says "safe to rebuild the spine" — but the press does not rebuild it, it copies an existing file at line 161, and the two are **9 days apart** (`hgm_dictionary_v27_2.json.gz` Aug 5; `hgm_spine_v27_2.db` Aug 14). No `shasum`/`sha256`/`checksum` appears anywhere in the press or the release script. *Impact:* the gate the press opens with proves a static file is internally consistent with itself. An arbitrary or stale spine can ship while the press reports "banked release v1.0.0-rc.1 passes" — a success line not produced by a path that verified the shipped bytes. *Fix:* stamp the spine's `meta` table with the source master's version and SHA at build time, have the press require a match before staging, and record a SHA-256 manifest of the staged payload into the DMG. *Effort:* small. *Verification:* reported.

**BUILD-13 · The press mutates the developer's machine on every run and cannot be unwound.**
*Location:* `tools/package_macos.sh:281-322`, `348-364`; `tools/release.sh:48-53`. *Evidence:* the press quits the running app, `pkill`s it, `rsync -a --delete`s over `/Applications` (1.4 GB installed), `ditto`s the data folder, and relaunches. `release.sh` unwinds only git (`git reset --hard HEAD~1`), so a press that fails at step 8 leaves `/Applications` holding the new build while git is reset to the previous version. *Impact:* the press is a deployment, not a build step — it can never run on CI, never run twice concurrently, and cannot verify a candidate without installing it. The release ritual's unwind is partial, producing exactly the half-release state it was written to prevent. *Fix:* split it — `package_macos.sh` produces and verifies the DMG and stops; a separate `install_local.sh` does quit/rsync/relaunch; `release.sh` presses, tags, then installs. This is also the prerequisite for BUILD-6's CI. *Effort:* small. *Verification:* reported.

**BUILD-14 · Release-facing docs contradict the script and the code they describe.**
*Location:* `docs/PACKAGING.md:11-15`, `99-103`; `docs/RELEASE_CHECKLIST.md:9-11`, `19`; `README.md:70`; `tools/package_macos.sh:16`, `20-26`, `197-205`. *Evidence:* (1) `--skip-build` is documented in three places and is **unreachable** — `$1` is `PRESS_MODE`, and passing `--skip-build` exits 2 with "unknown mode"; the test at line 41 can never be false. (2) Suite counts are stale everywhere: `ctest -N` → 72; PACKAGING says "37-suite battery", README "55 suites", RELEASE_CHECKLIST "all suites, 55 as of 2026-08-20" and points at the wrong build dir. (3) SQLite is documented as vendored and is not (see BUILD-6). (4) The press contradicts itself in one file: its header says market mode omits the CC BY-NC BDRC OCR models, while lines 197-205 say "They therefore ship in BOTH modes" with no `PRESS_MODE` guard. *Impact:* an operator following the checklist reaches for a flag that exits 2, and the "SQLite vendored" claim is load-bearing for the stated Windows/Linux readiness. *Fix:* delete or implement `--skip-build`; derive the suite count from `ctest` or reference `ALL_TEST_BATTERIES`, which is already correct; reconcile the press header with its own OCR block. *Effort:* small. *Verification:* reported.

#### Data integrity and documentation

**DATA-1 · Four documents disagree about what the app's own semantic green means, and seven sites render reference/machine material in it.**
*Location:* `app/main.cpp:7859`, `8107` (reference layers), `1835`, `1853`, `1868`, `1889`, `1942` (machine folds); legend at `34040-34054`. *Evidence:* the in-app Help teaches `EVIDENCE #1E6B4E — the aligned corpus` and closes "*The order never lies: a lower tier is never dressed as a higher one.*" `grep -n "1E6B4E"` → 21 lines; among them an Apache-2.0 word list and a CC0 verb DB, both labelled "(reference)" in their own text, plus five machine routing notes. The verifier found something stronger: `7859`, `8107`, `8154` (Evidence badge) and `8201` (Reference badge) are **all inside one function**, `OverlayPane::onClick()`, so a single card shows two reference blocks in the evidence ink and a third in the reference ink at once. The documentation contradiction is the durable part: `docs/TECHNICAL_OVERVIEW.md:420` says "green `#1E6B4E` reserved for HGM authority", `docs/CLOSER.md` rows 34/37/39 say "reserved for the binding tier", `main.cpp:274` says "actions / success", and `sourceBadge()` assigns it to `Epistemic::Evidence` while Binding gets `kGold`. The 84000 TM heading uses a fourth green, `#1F5B4B`, measured at 1.23:1 against `#1E6B4E` — visually the same ink. *Impact:* the colour code the Help page teaches is contradicted on the same screen. Not high: every block names its own tier and licence **in text**, the binding ink is `kGold` not this green, `kAct` is dual-purpose by construction (8 of the 21 sites are genuine action/success), and `sourceBadge()` has only 13 call sites app-wide so the badge grammar is a weak teaching signal either way. *Fix:* route the seven blocks through `ux::sourceBadge` with the right token (Reference at 7859/8107, Machine at 1868), retire `#1F5B4B` for `kDoc`, and — the item that matters most — reconcile the four documents so one sentence defines the ink. **Note:** the audit's blanket "migrate all literals to `ux::` tokens" recommendation was explicitly ruled against on 2026-08-20 (`UX_AUDIT.md:284-289`, the M2 ruling); do not re-litigate it here. *Effort:* small (the seven sites and the docs). *Verification:* downgraded from high.

**DATA-6 · Constitution R4 encodes five past ink incidents instead of the invariant, so 149 sub-AA text sites pass.**
*Location:* `tools/constitution_check.py:113-121`; `app/main.cpp:267-269` (the forbidding comment); 21 `color:#9A7A33` text sites. *Evidence:* R4 is a five-entry blocklist (`color:#9C948A`, `#8A8A8A`, `#999;`, `#999'`, `#B26B00`) and `constitution: all rules hold`. Census of every literal text ink: 60 distinct colours over 450 sites; scoring each against `kPaper #FAF6EE` with the WCAG relative-luminance formula → 162 sites below AA-normal (4.5), of which 13 are dark-panel viewers by design, leaving **149** on the cream reading surface: `#777` at 4.15 (117 sites), `#9A7A33` at 3.74 (21), `#888` at 3.29 (8), `#c80` at 2.75 (1, bold body text at 18568). `#9A7A33` is the sharpest case because `main.cpp:266-270` forbids it in its own comment ("the chrome keeps #9A7A33 for borders/washes") and it is used as 10-11 px micro-caps text. *Impact:* the gate that exists to stop this class cannot see it, because it was written as a memorial to five strings rather than as the rule. `UX_AUDIT_2.md` W5-01's "failing text colors corrected in place" scoped to the `ux::` tokens only, and does not say so. *Fix:* replace R4's blocklist with the invariant — parse every `color:#RRGGBB`, compute the WCAG ratio against `kPaper` (and both chrome grounds for QLabel stylesheets), fail below 4.5 outside an explicit dark-panel allowlist. That one rule subsumes the five strings. Then migrate `#777`→`ux::kFaint` (4.66, mechanical), the gold zone labels→`kGold` (4.97), `#888`→`kSoft`. *Effort:* medium. *Verification:* reported.

**DATA-7 · "Check for Updates" cannot see the DMG the press produces, and reports "(no DMGs found there)".**
*Location:* `app/main.cpp:33916-33918` (regex), `33800-33809` (`versionLess`); `tools/package_macos.sh:326`. *Evidence:* the press writes `Diamond-Cutter-Translation-Tool-$VERSION.dmg`; the app matches `ALL-Translation-Tool-([0-9.]+)\.dmg`. Tested: the produced name → no match. `git log -S` shows the cause — commit `650343a` "Total rename" renamed the DMG and kept backward compatibility for `findDataRoot` but left the updater regex behind. With no match the app renders *"Up to date — You are running version 1.0.0-rc.1 — the newest in the team folder (no DMGs found there)"*, which is false. A second mechanism defeats it even if the regex is fixed: replicating `versionLess` exactly, `1.0.0-rc.1` parses as `[1,0,0,1]` and therefore ranks **above** the `1.0.0` final it is a candidate for. *Impact:* the one self-update affordance in a hand-distributed app is inert, and it fails in the direction that asserts rather than admits — so fixes, including the ones in this assessment, do not reach the input centres and nobody is told. *Fix:* match `(?:Diamond-Cutter|ALL)-Translation-Tool-([0-9][0-9A-Za-z.\-]*)\.dmg`; make `versionLess` pre-release-aware, or better have the press emit a small `latest.json` beside the DMG that the app reads instead of parsing filenames. Never print "(no DMGs found there)" from a branch that only knows the regex missed — count `entryList({"*.dmg"})` and say so. Add a press gate asserting the DMG name and the app's regex agree. *Effort:* small. *Verification:* reported.

**DATA-8 · The teachings report shows 2 of up to 5 moments per term with no remainder, under a comment claiming it lists every matched entry with its teaching moments.**
*Location:* `app/main.cpp:7626-7627` (the cap), `7536-7541` (the completeness comment), `7676-7679` (the 300-term cap, disclosed without its remainder). *Evidence:* `if (++n > 2) break;` with no remainder note, under a header comment reading "every matched entry in the loaded text with its teaching moments". Measured against the index it reads: 8,518 keys, 36,258 moments, **7,064 keys (82.9%) hold more than 2**, max 5. The file's own correct idiom exists at 29466, 30115, 32089 ("and %1 more"), and the card path over the same index discloses its cap with a seen-of-total counter at `1616-1622`. *Impact:* the companion sheet a translator prints to find where the master taught this vocabulary presents 2 of 5 recordings as the whole set — and because the moments are ranked by context fit, the dropped ones are the less-obvious matches, precisely the ones a translator would not find by searching. *Fix:* `if (n > 2) { rest = (int)cand.size() - 2; break; }` then "… and %1 more moment(s)". Give the 300-term cap its remainder too. Correct the comment. *Effort:* trivial. *Verification:* reported.

**DATA-9 · The translator's survey hides ~61% of a text's unknown vocabulary behind an undisclosed 15-item cap, while the section directly beneath it discloses its remainder correctly.**
*Location:* `app/main.cpp:32066-32073` vs `32079-32090`. *Evidence:* "Top unknown forms" breaks at 15 with no remainder; twelve lines below, "Canonical quotations" breaks at 12 and then prints `- … and %1 more`. Run live on a real Sungbum text: `Tokens: 20247 · covered: 97.4% · uncovered: 2.6%` (~526 uncovered instances) while the 15 listed forms account for 206 — so ~320 instances (61%) belong to forms the report never names. The 15th entry is at ×4, so the tail is entirely ×1-×3 forms, the hapax legomena a translator most needs warned about. (Same run: the report labels the tier "PROVISIONAL (auto-aligned — amber)" in a markdown artefact that has no colour.) *Impact:* the survey's stated purpose is to tell a translator what they are facing; the list understates the unfamiliar vocabulary, which is the one number it exists to establish. *Fix:* two lines copying the idiom from twelve lines below: "… and %1 more distinct form(s), %2 further occurrence(s)". Drop the colour word from a plain-text report. *Effort:* trivial. *Verification:* reported.

**DATA-10 · The pecha batch lane silently downgrades a saved preset 4 to preset 3 — B13's exact twin, four lines above the comment describing B13's fix.**
*Location:* `app/main.cpp:12556` (dialog clamps 0..4) vs `12900-12901` (batch clamps 0..3); the B13 comment at `12921-12923`. *Evidence:* the combo offers five presets and the dialog clamps to 4; the batch lane reads the same `pecha/preset` key and clamps to 3, so "Measured woodblock (46 × 8.2 cm, 8 lines)" silently becomes "Degé woodblock (68 × 10 cm)" — a different physical format. Four lines below sits the fix for the identical bug on the sibling key, with the comment *"all five layouts, not three (B13): the clamp stopped at 2, so a saved three-up or A5-booklet choice came out of the batch lane as two-up with nothing said about it."* `pechaOptsFromSettings` is the live batch path (called at 4691, 4693, 4720, 4723, 12943). *Impact:* a cataloger sets the measured-woodblock geometry — derived from 14 sampled sides of a real print volume — batch-renders a folder, and gets Degé geometry in every PDF with nothing said. Bounty theme 6 verbatim, surviving a commit whose subject was fixing the same bug on the adjacent line. *Fix:* clamp to 4 and add the preset-4 branch; better, define the ceiling once (`kPechaPresetMax` or derive it from `addItems`) and use it at both ends. Pin: write each of the five indices to QSettings and assert the returned geometry — a test that fails today. *Effort:* trivial. *Verification:* reported.

**DATA-11 · Colour is the sole carrier of meaning at two sites, refuting `UX_AUDIT_2`'s "W5-02 · verified by design".**
*Location:* `app/main.cpp:24290-24292` + `24313`/`24323` (AlignPane nested links); `25636-25657` (double-keying compare); claim at `docs/UX_AUDIT_2.md:135-137`. *Evidence:* the audit says "Color is never the sole carrier: rulings carry ✓/✗/⏸ glyphs, epistemic badges carry their text…". But `linkColor(bool nested)` returns green `#9ED9A0` vs blue `#ABCDEF` as a background with no glyph, no border and no text difference, the pane's banner instructs "Sublinks inside longer links show green", and the links ledger beneath prints `[wylie = english ×]` with **no nesting indicator at all**. The double-keying comparator marks DELETE amber `#F5C066` and INSERT green `#9ED9A0` — the classic deuteranopia-confusable pair — and the status line reports only a total, never which kind. *Impact:* WCAG 2.2 SC 1.4.1 fails at both sites, both in production workflows for the input centres. A red-green-deficient operator double-keying cannot tell "extra here" from "missing here". *Fix:* add a non-colour carrier — a dotted underline or a bracketing glyph for sublinks plus a leading "↳" in the ledger; strikethrough for DELETE and caret/underline for INSERT, and split the status line into "N extra here · M missing here". Correct `UX_AUDIT_2.md:135-137` from "verified by design" to a scoped statement, since "verified by design" is what let two cases through. *Effort:* small. *Verification:* reported.

**DATA-12 · Zero accessible names app-wide, and the audit titled "MEASURED ACCESSIBILITY" silently dropped two of the four items it was scoped to answer.**
*Location:* `app/main.cpp` (whole file); `docs/UX_AUDIT_2_PROMPT.md:77-84` (scope) vs `docs/UX_AUDIT_2.md:122-141`. *Evidence:* the prompt scoped four items — contrast, colour-blind safety, **(c) hit targets: nothing interactive under 24px**, **(d) VoiceOver: accessible names on the band's proxies and the card's links**. The delivered section reports W5-01, W5-02 and a deferral, with no (c), no (d), and no statement that either was skipped. Measured: `setAccessibleName` **0**, `setAccessibleDescription` **0**, `setBuddy` **0** — against 65 `QLineEdit`, 27 `QComboBox`, 31 `QCheckBox`, 11 `QToolButton`, 30 `QListWidget`, 19 `QTableWidget`; 61 `setPlaceholderText` calls carry the only labelling many fields have, and a placeholder is not an accessible name. For (c): `collapse->setFixedWidth(16)` at `9253` is an interactive `QToolButton` at 16 px, under both the audit's own 24 px rule and WCAG 2.2 SC 2.5.8. On the positive side, nothing **removes** keyboard access: `setFocusPolicy(Qt::NoFocus)` count is 0 and there is no `Qt::LinksAccessibleByMouse`, so the 20+ `setOpenLinks(false)` browsers keep Qt's default Tab-to-anchor. *Impact:* a screen-reader user meets 65 unlabelled text fields and 11 icon-only tool buttons. More corrosive for the process: a report headed "MEASURED ACCESSIBILITY" that answered half its brief without saying so becomes the record that accessibility was done. *Fix:* `setAccessibleName` on the ribbon's icon proxies and every `QLineEdit` without a label buddy; `setAccessibleDescription` on the card's browsers; widen the 16 px rail toggle to 24. Amend the W5 section to state that (c) and (d) were not performed and are open. *Effort:* medium. *Verification:* reported.

---

### LOW

Twenty-four items. Real, small, and worth recording so the next assessment can see whether the tail is growing or shrinking.

| id | title | location | evidence (measured) | fix | effort |
|---|---|---|---|---|---|
| **TEST-1** | The F1 sweep uses the same predicate as the code under test | `app/main.cpp:36203-36266` vs `core/include/allcore/spine.h:36` | Hard-wiring `Entry::provisional()` to `false` left the sweep printing `[PASS] … prov-unmarked 0` over 4,592 cards while 3,910 auto-aligned glosses lost their tier. **Downgraded from critical:** a counter-mutation at the render site produced `[FAIL] … prov-unmarked 161` and turned `app_selftest` red, and the predicate mutation itself was caught by `spine_smoke` (ctest_exit=8). No escape from both was constructible. | Give the sweep an SQL-derived oracle (`SELECT wylie FROM entries WHERE tier='auto-aligned'`) and one golden card checked byte-for-byte, so a predicate change breaks a literal | small |
| **TEST-13** | Half the suite is inert on a clean clone; four harness modes are ungated | `core/CMakeLists.txt`, `.gitignore`, `tools/package_macos.sh:59` | 37 of 72 suites point into gitignored trees; `--sweep`, `--survey`, `--screenshots`, `--teachbench` are registered as no ctest suite; the visual gate runs only in the press. Mitigating: the batteries fail loudly rather than skipping | one `tools/fixtures.sh` that runs every regeneration step and reports missing/stale artifacts; register `--survey` and `--teachbench` with a pinned digest | medium |
| **MEM-7** | StarDict `.idx` bounds guard is off by one | `core/src/stardict.cpp:97` | Guard is `z + 8 > idx.size()` where the record needs `z+9`. Measured: truncated by 1 byte → `ok=1 words=1` (same count as intact); by 2 → correctly refused | change to `z + 9 > idx.size()`; add the truncated-by-one case to `stardict_smoke`, which already builds its own byte-format fixtures | trivial |
| **MEM-8** | Two Qt connections omit the receiver object the rest of the file passes | `app/main.cpp:27008`, `6598` | 15 of 16 `QTimer::singleShot` calls pass a context object; `27008` does not and captures a raw child pointer. `6598` ties an `aboutToQuit` connection to `qApp` while capturing `this`. Both safe today (one instance each, owned by the stack `QMainWindow`) | pass the receiver at both; add a constitution rule failing on `QTimer::singleShot(`/`connect(qApp` with no receiver — the count is already nearly zero | trivial |
| **STATIC-7** | Three undocumented empty catch blocks drop whole Hunt search lanes | `app/main.cpp:31567`, `31582`, `31626` vs 621, 3272, 11324, 18122, 32338, 32345, 32400 | 10 `bugprone-empty-catch` sites; seven carry an explanatory comment ("index unreadable → honest fallback text"), three in one HuntPalette function do not. Same shape at `core/src/contractions.cpp:42`, where an unparseable count silently becomes a real-looking 0 | give the three the treatment the seven have, and better an honest row ("dictionary lane unavailable"), so a broken lane looks different from an empty one | trivial |
| **STATIC-8** | A loop that provably runs at most once in the Sanskrit IPA converter | `core/src/sanskrit.cpp:598-612` | `-Wunreachable-code-loop-increment`: the `for` body ends in an unconditional `break`. When the token after the anusvāra is a vowel, the comment's third case is unreachable. No canonical Python oracle exists (`grep -ci ipa` on both `sanskrit_convert.py` files → 0) and the battery is 3 hand-written CHECKs, none reaching the branch | decide the intent and write it plainly as an `if`; extend `engines_battery` to reach all three documented branches | trivial |
| **STATIC-9** | Third-party includes use `-I`, not SYSTEM, so vendor headers drown the house signal | `ocr/CMakeLists.txt:19`, `:29` | At the high-signal tier: house code 22 warnings / 15 sites vs **52** from `onnxruntime` headers; near-`-Weverything`: 85 house vs 138 external. Invisible at `-Wall`, which is why raising the wall *feels* expensive when it is not | add the `SYSTEM` keyword to both `target_include_directories` calls, then re-run the deferred `-Wextra` evaluation — it is 8 sites | trivial |
| **STATIC-10** | Vendored `diff_match_patch`: `delete[]` on an indeterminate pointer | `app/thirdparty/diff_match_patch.cpp:1480`, `1549` | `-Wconditional-uninitialized` (in neither `-Wall` nor `-Wextra`) produced exactly one hit across 147 TUs, and it is real UB: `int *rd;` assigned only inside a loop an empty pattern skips, then `delete[] rd;` unconditionally. Currently unreachable — `match_main` returns early for an empty pattern, and the whole match/patch half is dead code (`diff_main` is the only entry the app calls) | `int *rd = nullptr;` — one character. Then add `-Wconditional-uninitialized` to the standing flags; measured cost is this site plus STATIC-8 | trivial |
| **STATIC-11** | Eleven unchecked `std::optional` dereferences held up by an unasserted cross-table invariant | `core/src/sanskrit.cpp:138-142` + 10 sites; `core/src/botok_trie.cpp:673` | **11 of 11 are false positives today** — verified by hand that `C_ACIP`/`C_DEVA`/`IPA_C` share 33 keys and the four vowel tables share 14, and that `botok_trie.cpp:673` is guarded. The finding is fragility: four parallel tables must keep identical key sets forever and the requirement is written nowhere; violating it is silent UB, not a failed lookup | a battery case asserting the key sets are equal — a dozen lines that turn silent UB into a red test | small |
| **FAIL-16** | "Save input work" is the one production save that says nothing at all | `app/main.cpp:25875-25877` | `if (!f.open(...)) return;` — silent — then an unchecked write and `status_->setText("saved " + out)`. Every neighbouring export in the same class routes through `warnWriteFail` (25844, 23035, 24495, 24523, 26672) | `warnWriteFail(this, f, "The input work file")` and gate the status line on a checked write. Two lines; the helper is already in scope | trivial |
| **FAIL-17** | An authentication challenge is reported as "BDRC unreachable" | `app/main.cpp:8511-8515` | Measured against a 401 responder: `error()=204 (AuthenticationRequired)`, rendered as "BDRC unreachable: Host requires authentication". `grep -c HttpStatusCodeAttribute` → **0** — the app never reads an HTTP status code anywhere, so 401/403/404/429/500 all funnel through "unreachable" | read `HttpStatusCodeAttribute` and branch: 401/403 declined, 404 no such record, 429 rate-limited (do not invite an immediate retry mid-batch), 5xx server failure | trivial |
| **PERF-11** | A one-time 1,628 ms main-thread stall resolving the missing font family `-apple-system` | `app/main.cpp:319`, `21832` | Qt names it and tells you the fix: `qt.qpa.fonts: Populating font family aliases took 1628 ms. Replace uses of missing font family "-apple-system" with one that exists`. Fires on first render of that HTML, not at startup. Also contradicts the project's own Palatino ruling at `main.cpp:6434-6437` | replace with a family Qt resolves (or `qApp->font().family()`) at both sites; assert zero `qt.qpa.fonts` lines in the selftest as a permanent pin | trivial |
| **PERF-12** | The fast `buildOverlay` overload does not produce the identical span set the header promises | `core/include/allcore/lattice.h:52-55`, `79-81` | The header claims "~10x faster … and produces the IDENTICAL span set (asserted in lattice_smoke)". Measured: 227× faster (699.9 ms vs 158,838.4 ms on 1.89 MB) — and **not identical**: −10 spans at 118 KB, growing to −151 at 1.89 MB. `lattice_smoke` evidently asserts identity on an input small enough that the paths agree | either fix the divergence or amend the header to state the true relationship, and strengthen `lattice_smoke` to a document large enough to exercise the tiering logic — 118 KB already separates them | medium |
| **BUILD-15** | Bundle identity is a placeholder and the Finder version does not match the release | `app/CMakeLists.txt:45-48` | `CFBundleIdentifier = com.appelegant.DiamondCutterTranslationTool`; `CFBundleShortVersionString = 1.0` against `VERSION` = `1.0.0-rc.1`; `LSMinimumSystemVersion` and `NSHumanReadableCopyright` both empty. The About string is correct, so app and Finder disagree | set an ALL-owned reverse-DNS identifier **now**, before a Developer ID is issued and before users accumulate TCC grants and QSettings under the placeholder domain; wire the version properties from `VERSION` as `ALL_APP_VERSION` already is | trivial |
| **BUILD-16** | The DMG is never verified as an artifact, and `dist/` accumulates ambiguous releases | `tools/package_macos.sh:324-346` | No `hdiutil verify`, no attach-and-launch: the launch test at step 6b runs against `$STAGE`, so the artifact the user receives is never opened by the press. `dist/` holds three DMGs, two of them claiming 1.0.0-rc.1 (417,499,799 and 424,670,599 bytes); `dist/stage` (813 MB) is never cleaned; `dist` is 2.0 GB | add `hdiutil verify` plus an attach → launch-from-mounted-image check; clean `$STAGE` on success; prune superseded DMGs so exactly one file per version exists | small |
| **BUILD-17** | The Homebrew-leak proof checks `LC_RPATH` only | `tools/package_macos.sh:104-116` | Independent scan of all 152 objects: `homebrew_rpath_leaks=0`, `homebrew_dep_leaks=0` — the gate's claim is true. But two stale absolute install IDs survive (`libbrotlicommon.1.dylib`, `libgcc_s.1.1.dylib` still carry `/opt/homebrew/...` as `LC_ID_DYLIB`). Not exploitable today: every referrer already loads via `@executable_path` | rewrite `LC_ID_DYLIB` for every bundled dylib and widen the verification from LC_RPATH-only to "no `/opt/homebrew` or `/usr/local` in **any** load command" | trivial |
| **DATA-13** | The 84000 "N duplicate line(s) not shown" notice under-reports on 34.1% of the layer | `app/main.cpp:1547-1552` vs `1553-1567` | The notice is composed, and *then* the Sanskrit dedupe keeps incrementing the same `dropped` counter. Measured by replicating `norm84` over `data/84000/g84000.json`: 10,450 of 38,878 Sanskrit lines are dropped, and **10,010 entries (34.1%)** report a smaller remainder than the real one | move the notice below the Sanskrit block, or count `droppedSkt` separately and report both. Assert in the F1 sweep that the reported number equals the number suppressed | trivial |
| **DATA-14** | The honest-elision invariant is asserted in a comment and bypassed at 33 display sites | `app/main.cpp:328-330` (the invariant), `671` (the adjacent bypass) | 19 honest `ux::snip` sites against 33 raw `.left(n)` display truncations. The sharpest is one statement with two behaviours at `664-674`: the Tibetan gets `ux::snip(…, 160)` with an ellipsis, 84000's English on the next line gets `.left(220)` with no marker. Scope note in the docs' favour: the manual's "marks every truncation" claim is scoped to the word card, and the card range 978-1670 contains zero raw display `.left()` — the manual holds, the code comment does not | convert the content-bearing subset (671, 2008, 2044, 2136, 2182, 15055, 15059, 16605, 18117, 18144, 18186, 18253, 18676, 19194, 19198, 30148, 32086) to `ux::snip`; make the comment enforceable with a constitution rule and a `ruled()` escape hatch like R1's | small |
| **DATA-15** | `attestedEnglish` suppresses genuinely attested English on 1.08% of lines because its equality test sees only the top 5 ranked hits | `app/main.cpp:11317-11322`; claim at `data/help/USER_MANUAL.md:701-703` | Replaying the function's own normalisation against the spine: 1,200 random short corpus lines → 13 suppressed (1.08%); of 150 high-containment lines (≥20 containing segments) → 7 suppressed (**4.7%**). A 300-line control on ordinary lines recovered 300/300 with 0 false attributions, so the primary guarantee is clean. The manual says the English appears "**wherever** that exact line is attested" | ask the precise question instead of ranking: `SELECT english FROM corpus_segments WHERE wylie = ? LIMIT 1` against a normalised index, falling back to FTS. Pin both directions — one line that must yield English, one containing-but-unequal line that must yield empty (only the second half is guarded today) | small |
| **DATA-16** | Two divergent `USER_MANUAL` copies, and the stale one is the only place the real DMG name is documented | `docs/distribution/USER_MANUAL.md` (3,882 lines) vs `data/help/USER_MANUAL.md` (3,938, the one the app loads) | Different md5s; the 58-line diff is the whole "What's new — the 2026-08-20 interface (read this first)" chapter, missing from the docs copy — plus one contradiction: the shipped copy documents `ALL-Translation-Tool-<version>.dmg` (matching the broken updater regex), the stale copy documents the name the press actually writes | make one authoritative and derive the other at press time; add to constitution L2 an assertion that the two copies are byte-identical, so a divergence fails the press | trivial |
| **DATA-17** | The Hunt palette prints "[PROVISIONAL]" on two lanes and the raw tier string "[auto-aligned]" on the third | `app/main.cpp:31578-31580` vs `31551` and `31606-31608` | Lanes 1 and 3 translate the tier for the reader; lane 2 prints `h.tier` raw. Reverse-index rows affected: **3,912** auto-aligned of 27,736. Rows are plain `QListWidget` text, so no colour carries the difference either — "[auto-aligned]", "[glossary]" and "[curated]" are visually identical and only one is weak | give lane 2 the shared `glossWithTier` helper from DATA-2; add a `provisional()` accessor to `ReverseHit` so the three lanes cannot diverge again | trivial |
| **DATA-18** | A cap that never fires: `un.count('\n') < 6` on a string built entirely from `<br>` | `app/main.cpp:17162` | `un` is appended to at exactly one site and the appended literal contains `<br>`, never `'\n'`, so the guard is always true and the intended 6-row cap does not exist | delete the guard (arguably right for a drill) or make it real and disclosed — "… and %1 more term(s) without a match". Do not leave a dead guard; it reads as a bounded list to the next reader | trivial |
| **DATA-19** | `pronunciation_source` is compressed to a bool at the core boundary | `core/src/spine.cpp:113`; `core/include/allcore/spine.h:23`; rendered at `app/main.cpp:1118` | `e.pronunciation_card_attested = !columnText(s, 6).empty();` — the release names the card for **1,308 entries across 17 distinct cards** ("GMR card (C03 Language Study Guide)" ×139, C04 ×137, C10 ×133) and the card renders only "⟪card⟫". Nothing is destroyed on disk (`raw_json` keeps it), but the addressability is gone. `corpus_courses` is likewise absent from `kEntryCols` | widen the field to `std::string pronunciation_source` (keep `pronunciation_card_attested()` as an inline accessor so no call site breaks) and render "⟪card: C03⟫". Assert in a test that every column in `kEntryCols` reaches a distinct `Entry` field | small |
| **DATA-20** | The "[generated script]" provenance marker — on 75% of entries — is rendered in the worst-contrast ink in the app | `app/main.cpp:999-1000` | `<i style='color:#888'>[generated script]</i>` at **3.29:1** against `kPaper`, on 79,316 of 105,634 entries (75.1%). The mark that tells a reader the Tibetan was machine-generated is the least readable text on the card | move it to `ux::kMachine #B4540A` (4.62:1) — both AA-compliant and the app's own declared ink for machine-derived marks. Retire the other seven `#888` sites in the DATA-6 sweep | trivial |

---

## 5. The defect-density baseline

Raw numbers, so a future assessment can compare rather than re-argue.
**Measured 2026-08-22 at commit `f752f65`, `app/main.cpp` md5 `30e3f0036feecbd43f2f503754edd579` (37,493 lines), VERSION `1.0.0-rc.1`, on a 64 GB / 10-core Apple Silicon machine, Apple clang 21.0.0, Qt 6.11.1, macOS 26.5.**
Figures marked ‡ were reproduced independently by a second agent.

### Suites, pins, and what they catch

| Metric | Value |
|---|---|
| Registered ctest suites (`ctest -N`) ‡ | **72** (core 66 · app 5 · ocr 1) |
| Full battery wall clock ‡ | 59.3 s at `-j6` (51.3 s in a second run) |
| Clean out-of-tree configure + build ‡ | 1 min 51 s at `-j10` |
| Suites depending on gitignored fixtures ‡ | **37 of 72** (build/ 21 · library/ 12 · data/extracted/ 7) |
| Suites runnable from a clean clone | 35 |
| Suites proved to pass **vacuously** without their fixture | 2 (`catalog_id_smoke`, `catalog_qc_smoke`) |
| Suites proved to SIGSEGV after printing a fabricated "(measured 31%)" | 1 (`shelf_suggest_smoke`) |
| Mutations planted, overall | 13 code + 3 gate probes |
| **Mutation score, `allcore` + `allocr`** | **4 / 4 caught (100%)** |
| **Mutation score, `app/main.cpp`** | **3 / 9 caught (33%)** — verifier's independent ledger: 2 / 8 (25%) |
| Write-failure mutants killed by the suite | **0 of 5** |
| Bounty fixes revertible at their defect site with 72/72 green | 3 (individually and all three at once) |
| `check()`/`lk()` pin sites in `app/main.cpp` | 217 declared · 290 executed lines |
| Pins resting on substring or HTML presence | 125 / 217 = **57.6%** |
| Pins that are numeric or structural | 41 / 217 = 18.9% |
| Panes with ≤5 executed pins | 13 of 21 |
| Share of pins belonging to `OverlayPane` | 115 / 290 = 39.7% |
| Tautological pins (`bool x = true; … check(x)` or `check(true, …)`) | 2 (`main.cpp:4189`, `:17869`) |
| Clicks / key events / `QTest::` calls in the assertion-carrying selftest | **0** |
| Click/trigger/key line execution counts under `--selftest` | 0 for all 15 such lines |
| Distinct controls the gauntlet actually drives | 37 tool buttons · 7 menu actions · 13 word-clicks |
| Test files that arrange a write failure | **0 of 64** |
| Tests anywhere asserting a save **fails** | 0 |
| Core-test `CHECK` assertions / test source lines | 939 / 8,751 |
| App selftest source lines | 4,256 = 11.4% of `main.cpp` |

### Coverage (llvm-cov, instrumented RelWithDebInfo, full ctest, 70 `.profraw`)

| Target | Regions | Functions | Lines | Branches |
|---|---|---|---|---|
| `core/src` ‡ | 90.35% | **98.92%** (730/738) | **94.03%** (12,208/12,983) | 79.13% |
| `ocr/src` ‡ | — | 94.29% (33/35) | 93.52% (851/910) | — |
| `app/main.cpp` (all ctest suites) ‡ | 50.89% | **47.67%** (662 of 1,265 never executed) | **56.81%** (17,065 of 39,508 missed) | **40.91%** |
| `app/main.cpp`, `--selftest` alone ‡ | 47.18% | 39.53% | 52.71% | 38.42% |
| `app/main.cpp`, `--gauntlet 1 300` alone ‡ | 14.60% | 22.21% | 27.04% | 10.34% |
| `app/main.cpp`, all 22 sweeps merged | — | 24.03% | 27.84% | — |
| `app/main.cpp`, every harness combined | 54.47% | 53.36% | 59.05% | 43.99% |
| `saveOrWarn` failure branches | **0 executions across all 72 suites** |
| `warnWriteFail` | **0% covered end to end**; 12 of 13 call sites never execute |

### Unchecked I/O and error handling (production scope: `app/main.cpp` outside 17 selfTest bodies and `main()`)

| Metric | Value |
|---|---|
| `open(WriteOnly)` sites / returns discarded | 27 / **0** |
| `open(ReadOnly)` sites / returns discarded | 71 / 1 |
| `write()` sites / returns discarded | 26 / **22** |
| `QTextStream` over `QFile` sites / `status()` checks | 9 / **0** |
| `mkpath` sites / bools discarded | 26 / 24 |
| `QFile::remove` / `rename` / `copy` bools discarded | 15 / 1 / 2 |
| `saveOrWarn` call sites ‡ | 13 |
| `warnWriteFail` call sites ‡ | 13 |
| Total write-side sites / with discarded return | 53 / 22 |
| `clang-tidy bugprone-unused-return-value` on `main.cpp` | 211 (verifier's wider pattern: 245) |
| `[[nodiscard]]` in 59 `allcore` headers / in `main.cpp` ‡ | **0 / 0** |
| `QJsonDocument::fromJson` sites / `QJsonParseError` checks ‡ | 43 / **1** real check |
| `waitForFinished` sites / `exitStatus` checks | 13 / 1 |
| `setTransferTimeout` calls ‡ | **0** (measured default `transferTimeout()` = 0) |
| Blocking network event loops / with a working cancel | 5 / 4 |
| Core stores whose `save()` returns unconditional `true` | 4 (`proposals`, `catalog_actions`, `regenreg`, `colloquial`) |

### Compiler, sanitizers, analysis

| Metric | Value |
|---|---|
| Translation units compiled ‡ | 147 |
| Warnings at the standing wall (`-Wall`) ‡ | **0** |
| Suppressions in the repo (`-Wno-*`, `#pragma diagnostic`, `NOLINT`) ‡ | **0 / 0 / 0** |
| Warnings adding `-Wextra -Wpedantic` ‡ | 15 at **8 unique sites**, all verified false positives |
| Warnings, high-signal set (9 extra families) | core+ocr 22 · app 54 · tests 16 |
| Text sites below WCAG AA on the cream surface | 149 (of 450 literal-colour sites, 60 distinct colours) |
| `clang-analyzer-*` findings (128 checks, all TUs) | **0** |
| clang-tidy diagnostics (310 checks) | core+ocr 514 · `main.cpp` 413 |
| `-fsanitize` occurrences in any build file, script or CI config ‡ | **0** |
| CI configurations present ‡ | **0** (no `.github`, `.gitlab-ci.yml`, `.circleci`, `Jenkinsfile`) |
| Dependency lockfiles present ‡ | **0** |
| ASan+UBSan build of the whole tree | 0 errors, 0 warnings, 3 min 20 s |
| Sanitizer reports from every dynamic run (~1.35M API invocations, 159 real docs, 11 hostile docs, 911 corrupt files, 6,000 hostile queries, 2,700 gauntlet steps) | **0** |
| Reproducible crashes found by targeted probing | 3 (`progress.cpp:122`, `:218`, `gofer_ast.h` at ~45,000 depth) |
| Confirmed OOB read APIs | `botok::ChunkFramework` (9 overloads + `chunkString`) |
| Unsafe C string calls / unions / type punning ‡ | 0 / 0 / 0 |
| `reinterpret_cast` sites | 11-12, all sqlite `unsigned char*` → `char*` |

### Performance at real scale

| Operation | Measured |
|---|---|
| Spine open (cold / warm) | 2.08 ms / 0.33 ms |
| `Spine::lookup`, 1,993 distinct headwords | median 0.024 ms · p95 0.031 ms · max 0.225 ms |
| `corpusSearch` (limit 20 / 200) | median 2.889 ms / 2.786 ms — the limit costs nothing here |
| `corpusCount`, uncapped | median 1.406 ms · max 7.592 ms |
| `buildOverlay` indexed, 1.89 MB / 399,602 tokens | **699.9 ms** = 1.75 µs/token (un-indexed: 158,838 ms → 227×) |
| `extractOutline` / `extractStructure`, same file | 10.8 ms / 10.5 ms |
| tokenize + outline over the WHOLE library (8,988 files, 643 MB) | 11.46 s total, 1.27 ms/file, worst file 179 ms |
| `LibraryIndex::search "PA"` (warm / cold / evicted) | 1,960 ms / 18,828 ms / 5,169-8,906 ms |
| `"CHOS" NEAR/1000000 "SANGS RGYAS"` (the ordinary AND) ‡ | **16,014 ms · 6.28 GB RSS** |
| three ordinary terms ANDed ‡ | **346,116 ms (5m46s) · 18.0 GB RSS** |
| `LibraryIndex::update`, full rebuild ‡ | **190,492-196,487 ms**, index 2,357,227,520 B from 642,907,695 B (3.67×) |
| `ensureSegmenter` (one checkbox) ‡ | **5,257-5,647 ms**, 3,511,730 trie nodes, 547,946 words |
| HuntPalette first-use walk / per keystroke | 1,470 ms / 17 ms (debounce 220 ms) |
| `fillList` 9,011 rows / `libTitleIndex` build | 579 ms / 453 ms |
| Font-alias stall (`-apple-system`) | 1,628 ms, once |
| Startup to all panes built | 1.95 s (1.57 user) |
| Max RSS: startup / sweep Overlay / selftest cold | 440 MB / 798 MB / 981 MB |
| T4 pin headroom vs the comment's claimed 4× ‡ | 333× · 95× · 133× · 1200× |
| Concurrency: `QtConcurrent` / `QFutureWatcher` / `setOverrideCursor` / worker threads ‡ | 0 / 0 / 0 / **1** |

### Distribution and provenance

| Metric | Value |
|---|---|
| Mach-O objects shipped in the DMG | **152** (13 Qt frameworks · 129 dylibs · 22 plugins) |
| Distinct upstream projects bundled / named in the notices | ~32 / **6** (3 of them software components proper) |
| Licence text files in the DMG / LGPL or GPL texts / project LICENSE | 6 (all data-layer) / **0** / **absent** |
| Homebrew rpath leaks / load-dependency leaks / stale install IDs ‡ | 0 / 0 / 2 |
| Press gates proven fatal / proven non-fatal | 7 / 5 |
| Constitution rules / rules about I/O returns | 9 / **0** |
| Constitution rules measured / found blind to their own class | 4 / **4** (R1, R4, L2, C2) |
| App binary archs / `minos` / `LSMinimumSystemVersion` ‡ | arm64 / 26.0 / **empty** |
| Codesign state ‡ | adhoc, no TeamIdentifier, no entitlements; `syspolicy_check`: "Notary Ticket Missing — **Fatal**" |
| Spine populations ‡ | 105,634 entries · 12,004 glossed (glossary 7,980 · **auto-aligned 3,910** · curated 114) · 42,199 corpus segments |
| Entries with generated Tibetan ‡ | 79,316 = 75.1% |
| Headwords whose corpus count exceeds the 200 display cap | 1,532 |
| 84000 TM rows / segments matching `chos` | 400,745 / 55,720 (rendered as "5 of 200") |

---

## 6. Remediation ledger

Ordered by (severity × reach) ÷ effort. **Ship-blocker** means: do not press 1.0.0 without it.

### Do before 1.0 ships

| # | Item | ids | Effort | Why now |
|---|---|---|---|---|
| 1 | Add the harness save-dialog stub seam (6 lines beside the wrappers) | FAIL-4 | trivial | Cheapest item on the list and it unblocks the most: it is the only way to write any of the write-failure tests below. |
| 2 | Make the four `save()`s honest and consume their returns: `proposals.cpp:210`, `catalog_actions.cpp:121`, `regenreg.cpp:41`, `colloquial.cpp:107`; check the stream after flush, not just the open; gate `main.cpp:27580`'s modal on the result | FAIL-2, STATIC-1 | small | A live critical. The shared proposal record and the catalog ledger both report success over bytes that never landed. |
| 3 | Fix `ActionLedger::approve`/`reject`: roll back like `stage()` does, and open the META companion `NewOnly` so a name collision refuses instead of overwriting | STATIC-1 | small | Silent, permanent divergence between the ledger and the official shelf, plus a silent overwrite of existing cataloging META with no permissions trickery required. |
| 4 | Move `++pagesOk` inside its guard, add the `else { ++failed; … }`, and check `mkpath` before the batch | DATA-5 | trivial | A false count is harder to disbelieve than a false sentence, and it is one line. |
| 5 | `catalogRosterSave`: write temp + `rename`, and check `ts.status()` / `f.flush()` | FAIL-1 | small | The access-control roster is destroyed by the truncating open while the function returns TRUE. |
| 6 | Tag the tier at `main.cpp:17172` and `:23964` (shared `glossWithTier` helper) | DATA-2, DATA-17 | trivial | House rule 1, in the drill, to the user least able to tell. Two lines plus the Hunt lane. |
| 7 | Add `Tm84000::matchCount`, use `corpusCount` at `18199` and `19184`, correct the wording, and correct `USER_MANUAL.md:1249`/`:1312` and `UX_AUDIT.md:292` | DATA-3 | medium | House rule 3 on the two panes used to decide a rendering, under a manual that says "every". |
| 8 | Add `writeAllOrWarn` and convert the four export lanes; gate their success strings on it | DATA-4, FAIL-5 | medium | House rule 4, and the approved-candidates TSV is the artefact that feeds the master dictionary. |
| 9 | Bound `LibraryIndex::search`: `LIMIT` in SQL for TERM, a merge join for NEAR, a disclosed cap on materialised windows, a `stop_token`, and run it off the GUI thread | PERF-1 | large | The only measured OOM path in the product, reachable from three ordinary words with a Stop button that cannot fire. If the full fix cannot land, the interim is the SQL `LIMIT` plus a hard window cap with disclosure — that alone removes the 18 GB. |
| 10 | State the system requirement (Apple Silicon, macOS 26+) in `README.txt`, `INSTALL_QUICK_START.md` and the checklist; populate `LSMinimumSystemVersion`; set an ALL-owned bundle identifier | BUILD-1, BUILD-15 | trivial | Users on unsupported hardware get a non-launch with no explanation, and the placeholder identifier gets harder to change with every install. |
| 11 | Fix the Gatekeeper instruction the press prints, and say the DMG is unsigned | BUILD-3 | trivial | The printed remedy does not work on any OS this build supports. |
| 12 | Either put a non-redistribution notice in the team `README.txt` or stop staging `reference.db` and the THL record trees | BUILD-5 | small | The artifact contradicts the notices inside it and the builder script's own docstring. |
| 13 | Generate `OPEN_SOURCE_NOTICES.md` from the staged bundle and fail the press on any unmapped binary; bundle the licence texts; add a project `LICENSE` | BUILD-2, BUILD-4 | medium | ~29 of ~32 shipped projects unrecorded, no LGPL text, and the gate that should catch it passes on a keyword. |
| 14 | Delete the two vacuous passes and the fabricated "(measured N%)" label; add `CHECK(queries > 0, …)` | BUILD-7 | trivial | A house-rule-4 violation inside the quality gate. |
| 15 | Fix the fabricating PASS strings: rewrite the pin at `main.cpp:4189` to assert the data, and `:17869` to assert receipt | MEM-1 | small | Two shipped strings assert properties no code verified, one of them on heap corruption. |
| 16 | `setTransferTimeout` on the four managers; wire `pd.canceled → rep->abort()` at `main.cpp:8945` | FAIL-8, PERF-9 | trivial | One unreachable folio server hangs the app indefinitely with a dead Cancel. |
| 17 | Check `waitForFinished`/`exitStatus`/`exitCode` at the 13 sites, starting with "Extract all" | FAIL-9 | small | An unconditional "Extracted" on a batch that may not have happened. |
| 18 | Delete "instantly" from `libindex.h:3`, `README.md:36` and `main.cpp:23526` | PERF-7 | trivial | A success string asserting a property nothing measured — the same rule as everything else on this list. |
| 19 | Fix the updater regex and `versionLess`; stop printing "(no DMGs found there)" from a branch that only knows the regex missed | DATA-7 | small | Without it, none of the above reaches the input centres. |
| 20 | Reopen `CLOSER` #12; retighten the four T4 budgets to ~3× and sample distinct headwords | PERF-3 | small | The closure is being cited as proof that performance is guarded. |

### Do soon after 1.0

| # | Item | ids | Effort |
|---|---|---|---|
| 21 | Write the write-failure drills behind the new seam — one per store, one per export lane — and **validate each by mutation before it lands** | FAIL-3, TEST-3 | medium |
| 22 | Pin every bounty fix at its **defect site**, mutation-validated; make "reverting the fix turns a pin red" the merge criterion | TEST-2 | medium |
| 23 | `[[nodiscard]]` on the ~30 mutating `allcore` APIs; commit a `.clang-tidy` with the `CheckedFunctions` list as a ratcheting baseline | STATIC-1, STATIC-6 | medium |
| 24 | `option(ALL_SANITIZE …)` + `tools/sanitized_battery.sh`; the battery is already green under it | MEM-2 | small |
| 25 | Null-guard `progress.cpp:122/204/218` with the existing `columnText()`; NOT NULL in the schema; pin with a NULL-row test | MEM-3 | trivial |
| 26 | Constitution R7: census of discarded write-side returns with a baseline file, in R3's exact shape | FAIL-15 | small |
| 27 | Replace R4's blocklist with a computed WCAG rule; widen R1 to the named-variable idiom; make C2 execute a gate probe; invert L2 to a manifest match | DATA-6, TEST-9, BUILD-10, BUILD-4 | medium |
| 28 | Progress + cancel + worker thread for `LibraryIndex::update` and `ensureSegmenter` (copy one of the five working compute dialogs) | PERF-2, PERF-5, PERF-6 | medium |
| 29 | Commit or generate the Overlay perf-guard fixture; make an absent fixture FAIL, never `[info]` | PERF-4 | small |
| 30 | Disclose the remainders: teachings 2-of-5, survey 15-cap, 84000 duplicate notice, pecha preset clamp | DATA-8, DATA-9, DATA-10, DATA-13 | trivial |
| 31 | Clamp the `botok` public index API; bound the Gofer parser's recursion | MEM-4, STATIC-5 | small |
| 32 | `Brewfile` + `brew bundle` in the press; emit a build manifest (formula → version → SPDX → SHA) into the DMG | BUILD-6, BUILD-2 | medium |
| 33 | Split `package_macos.sh` into press and install; then stand up CI on the 35 fixture-independent suites | BUILD-13, BUILD-6 | medium |
| 34 | Make `codesign --verify` fatal; version the shot_diff baseline; add `hdiutil verify` | BUILD-8, BUILD-9, BUILD-16 | small |
| 35 | Point the harness at its own QSettings scope; guard `main.cpp:5710`; remove the global `g_harnessRun` flip | TEST-8 | small |
| 36 | Correct the stale documents: `TEST_PLAN.md`'s seven phantom batteries, `DEVELOPER_ONBOARDING.md`'s "55 suites" and missing `verify.sh`, `PACKAGING.md`'s `--skip-build` and "SQLite vendored", the two `USER_MANUAL` copies, `UX_AUDIT_2.md`'s W5 scope | TEST-12, BUILD-14, DATA-16, DATA-12 | small |
| 37 | The trivial one-liners: `stardict.cpp:97` off-by-one, `diff_match_patch.cpp:1480`, the two Qt receiver arguments, `-apple-system`, the OCR worker's widget read, `FAIL-16`'s silent save, `FAIL-11`'s discarded rollback bool, `FAIL-10`'s unchecked copy, `DATA-18`'s dead guard | MEM-5, MEM-7, MEM-8, STATIC-10, PERF-11, FAIL-10, FAIL-11, FAIL-16, DATA-18 | trivial each |

### Not worth doing before 1.0 — and in some cases not at all

- **`-Wextra -Wpedantic` adoption** (STATIC-2). All 8 sites are false positives; it demonstrably hides zero bugs today. The *gate* (a `warning:` check in the press) is worth more than the flag.
- **A universal / lower-floor macOS build** (BUILD-1's second half). 128 of 129 bundled dylibs are `minos 26.0` Homebrew bottles; this means rebuilding the whole dependency stack. Document the requirement instead and decide the build question separately.
- **Wholesale migration of colour literals to `ux::` tokens** (DATA-1). Explicitly ruled against on 2026-08-20 (`UX_AUDIT.md:284-289`, M2): "churn hundreds of pinned lines for zero visible change." Fix the seven mis-tiered sites and the four contradicting documents; leave the rest.
- **The 33 `.left()` display truncations** (DATA-14) and the `SYSTEM` include change (STATIC-9) — real, mechanical, and better done in one pass after the ship, not during a release.
- **Raising `app/main.cpp` coverage as a project** (TEST-6). Adopt the floor and the recipe now; raising the number is a programme, not a fix. The specific pins in items 21-22 buy far more per hour than a coverage push.
- **Rewriting anything.** Nothing in this assessment recommends a rewrite. Every item above is a clamp, a return value, a disclosure sentence, a receiver argument, or a test.

---

## 7. What could not be measured

Stated plainly, with the reason. Nothing here is an estimate dressed as a measurement.

**Rendering, painting, scrolling and typing latency.** Everything ran under `QT_QPA_PLATFORM=offscreen`, which skips real compositing and font rasterisation. Nobody has published a measurement of typing latency or scroll smoothness in a 1.89 MB document with 589,362 spans rendered, and none could be produced here. The Overlay's own selftest comment concedes the gap (`main.cpp:5617-5619`: "Display timing is a GUI concern … covered by the manual checklist"). **This is the single largest hole in the coverage of this assessment**, and it sits exactly where a translator's experience of the product lives.

**True cold-boot performance.** `purge` needs sudo, which was unavailable. Every "cold" figure means "first touch of that file within the session", so the 58.37 s cold selftest and the 18.8 s cold search are **lower bounds** on true cold cost. The index copy used for search benchmarks was an APFS clone (`cp -c`) sharing warm blocks with the original.

**Behaviour on the actual deployment target.** All figures are from a 64 GB / 10-core machine, several under a load average of 11-38 from parallel agents (RSS and user CPU are the trustworthy comparands there; wall times inflate). A 16 GB configuration was not tested, swap was not measured, and the reasoning in PERF-10 about page-cache eviction is extrapolation from a measured 10× residency swing — not a 16 GB measurement.

**Live GUI interaction.** No finding rests on driving the real GUI. The Search pane's Stop button was proved inert from source (no cancellation token, no `processEvents` inside `search`), not by clicking it mid-search. Card-link keyboard activation is unverified: nothing *removes* keyboard access (`setFocusPolicy(Qt::NoFocus)` = 0, no `LinksAccessibleByMouse`), so Qt's defaults should apply — but the app's `--selftest` writes fixtures into the repo's `library/` tree and the repo was not to be written to.

**VoiceOver output.** The zero counts for `setAccessibleName` / `setAccessibleDescription` / `setBuddy` are certain. What those controls actually announce (Qt derives some names from text and tooltips) was not heard.

**ThreadSanitizer, and therefore MEM-5.** TSan cannot combine with ASan, and the only thread in the product (`ocrThread_`) is unreachable headless — `--sweep OCR` reports "0 control(s) exercised". The cross-thread `QCheckBox` read is a source-inspection finding. No race in this codebase has been dynamically observed.

**Leaks.** `ASAN_OPTIONS=detect_leaks=0` throughout: LeakSanitizer is unreliable on macOS/arm64 and Qt's parent-ownership idiom generates large volumes of non-leaks. **No measurement of leaks was made and no claim about them is offered.**

**Real network behaviour.** No outbound calls were made for the failure-path findings; the 401/500/timeout/malformed traces come from local fault servers and a black-hole endpoint, not from BDRC. One correction worth recording: the brief's premise that BDRC's IIIF endpoint now returns 401 to everyone **did not reproduce** — collection, sub-collection, manifest and a real canvas image all returned 200 (30,487 bytes of PNG) from this machine on this date.

**A full press run.** `tools/package_macos.sh` rsyncs 1.4 GB over `/Applications`, kills the running app and relaunches it. Running it would have been destructive and would have collided with parallel agents. All press findings come from reading the script end to end plus isolated reproductions of its exact shell constructs. The wall-clock of the 13 `waitForFinished` sites is likewise the coded ceiling, not an observed duration — no real collection archive was unzipped.

**Concurrency of the test suite.** 20 `setValue` calls in the selftest write the shared `com.ALL.TranslationTool` domain and one run was proved to mutate it; two simultaneous `ctest` invocations were not run, so the hazard is established and a concrete flake is not.

**Coverage attribution edges.** `llvm-cov` attributes header-inline code (`Entry::provisional`, `DrillFactory::pickSegment`) to headers, so per-file percentages exclude some inline logic; moc/AUTOGEN sources were outside the per-file aggregation; `allocr` was not instrumented at all, so what `ocr_smoke` reaches inside the OCR line-builder and CTC decoder is unknown.

**Fuzzing depth.** The fuzz drivers are breadth-first mutation drivers, not coverage-guided. Without libFuzzer or AFL feedback, deep conditional branches inside the converters may never have been reached despite ~1.35M invocations. The clean result bounds the shallow input space well and says less about deep state.

**Per-library licence verification.** ~32 bundled projects were enumerated and ten spot-checked against the notices; each formula's SPDX identifier was not resolved. **The GPLv3-with-Runtime-Library-Exception question around `libgfortran`/`libquadmath`/`libgcc_s` is flagged as unconfirmed, not adjudicated** — it needs a licensing opinion, not a measurement. Likewise whether `reference.db` actually contains the THL and Hypercontext payloads was inferred from the press comment and the extractor scripts, not by opening the database.

**Whether the shipped spine was built from the validated master.** The two files are 9 days apart and no checksum links them (BUILD-12); no rebuild-and-diff was attempted.

**Build determinism in the strict sense.** The proxies were measured (no `__DATE__` in `main.cpp`/`core`, no leaked build paths in the binary) but two clean builds were not compared byte for byte. `main.cpp` *does* carry `__DATE__` for the About box (STATIC-4), which makes byte-identical verification impossible by construction.

**GCC, MSVC, Debug builds, and clazy.** Every compiler measurement is Apple clang 21.0.0 on arm64, Release. GCC's `-Wmaybe-uninitialized` has no clang equivalent and typically produces a first-run spike. **clazy — the Qt-specific analyser, whose checks (connect-non-signal, lambda-in-connect lifetime, detaching-temporary) cover exactly the defect classes clang-tidy has no model for — was not installed and not attempted. For a 37,493-line Qt UI that is the largest single analysis surface left uncovered.**

**Mutation sampling, not census.** 13 code mutations plus 3 gate probes, chosen deliberately at sites where coverage is claimed or where the bounty fixed something. The app-layer score (3/9, or 2/8 on the verifier's independent ledger) is a sample, not a systematic operator sweep; the core-layer 4/4 rests on four mutations and leans on the coverage measurement for its confidence.

**The other ~69 bounty findings.** `docs/BUG_BOUNTY_2026-08-22.md` states its own payload was truncated — "17 verified findings … arrived, not 86 … the remaining ~69 need to be re-sent to be placed." This assessment is independent of those and may duplicate or miss them.

**Side effects disclosed, not measured gaps.** Running the harnesses rewrote the gitignored `progress.db` at the repo root and modified the user's live `com.ALL.TranslationTool` preferences (recents, `overlay/lastFile`) — exactly as the project's own `ctest` does. No tracked file in the repository was edited by this assessment; `git status --porcelain` was clean before and after, and every build, mutation and probe lived under a scratch directory. This document is the only file created.

---

## 8. What this says about the process

The bug bounty's conclusion was that this codebase has correct rules that hold only where their author remembered them. This assessment set out to test whether the process now catches that. **It does not — and the reason is specific enough to fix.**

### 8.1 The gates encode the incident, not the invariant

Four of the nine constitution rules were measured. All four are blind to their own class:

| Rule | What it was written for | What it actually checks | Measured blind spot |
|---|---|---|---|
| **R1** | A collapsed-escape regex that silently broke every `[84000]` entity link | `QRegularExpression\s*\(\s*"` — direct construction only | 18 of 53 sites (34%) use the named-variable form; a genuine collapsed escape there produces `constitution: all rules hold` |
| **R4** | Five specific failing inks | those five strings | 149 sub-AA text sites pass, including 21 uses of an ink `main.cpp:267-269` forbids as text in its own comment |
| **L2** | "reference.db and CC BY-NC models were shipping by habit" | a keyword per staged directory | passes 102 MB on the substring "84000"; **never inspects `reference.db`**, the thing it was written for |
| **C2** | "a press that silently lost a gate is a failed press" | three substrings present in the file | cannot tell a deleted gate from one disarmed with `\|\| true` — and two gates in the press already are |

R3 (the modal census) is the counter-example that proves the shape is achievable: adding one `QMessageBox::warning` produced `FAIL R3 app/main.cpp: modal call sites grew 130 -> 131`. It works because it **counts** rather than matching strings. Every rule above could be rewritten in R3's shape — compute the contrast ratio, count the discarded returns, enumerate the payload against a manifest, execute the press against a failing fixture — and the difference in cost is a few hours.

### 8.2 Fixes visit the sites the report named, and stop there

Every closed bounty finding examined has surviving instances:

- **#2** (untiered provisional gloss): closed after three surfaces. A 4th is at `main.cpp:17172` and a 5th at `:23964` — and the *same pane* tags it correctly 600 lines earlier at `16559`.
- **#8** (cap printed as total): closed on the card. Three more at `18199`, `19184`, `648` — the last with the comment "the cap never hides the true total" directly above a re-query with a literal `200`.
- **#6** (unverified success string): ten sites converted to `saveOrWarn`. Four remain, plus `catalogRosterSave` and `ProposalStore::save`.
- **#9** (counters on the wrong branch): fixed in the folio downloader. The identical shape is in batch OCR at `26622`.
- **#13** (a clamp silently downgrading a choice): fixed for `pecha/layout`. The twin for `pecha/preset` is **four lines above the comment describing the fix**.

The diff for each fix touches only the line numbers the report gave it. The class was never swept.

### 8.3 Closure evidence is repeatedly a green suite that cannot see the change

This is the sharpest process finding, because it means the record itself is unreliable:

- Commit `ba0fda7` quotes "72/72 ctest green" as verification for the bounty #2 surfaces. Reverting all three simultaneously still gives 100% of 72. The green has **zero sensitivity** to what it was quoted for.
- The bounty **prescribed** the pins that would have caught them (doc lines 21 and 29). They were never written.
- `CLOSER` #12 is CLOSED as "T4 perf floors measured + pinned (cold start, big doc, **query fan-out, index build**)". Query fan-out and index build have no pin at all, and query fan-out is the 18 GB critical in this report.
- The only perf guard on the document pipeline prints `[info] … perf guard skipped` because its fixture does not exist, and the suite prints ALL PASS.
- `shelf_suggest_smoke` prints `[PASS] … (measured 31%)` over **zero** queries, then segfaults — the "measured" figure is a string literal in the assertion label. A house-rule-4 violation inside the quality gate.
- The single memory-safety pin is `bool survived = true;`. Restoring the heap overflow it names leaves the suite green in 9 runs out of 11.
- `docs/TEST_PLAN.md` certifies features against seven batteries that are not registered anywhere, and asserts "everything automatable is automated".
- `UX_AUDIT_2.md`'s "MEASURED ACCESSIBILITY" section silently answered two of its four scoped items; `UX_AUDIT_2.md:135` says colour is "never the sole carrier" while two production surfaces encode meaning in hue alone; `UX_AUDIT.md:292` marks "P3 counts-first ledger, ALL card evidence sections ✓" over three sections that never got it.

House rule 4 says nothing prints a success string that was not produced by the code path that verified the bytes landed. **The project applies that rule rigorously to the product and not at all to its own closure record.**

### 8.4 The two mechanical habits that would change the trend

Everything above reduces to two missing disciplines, and both are cheap:

1. **A pin must fail when the defect returns.** Revert only the fix; if no test turns red, the pin is decoration. Make that the merge criterion. It would have caught B12 and B13 before they were proposed, and it would have caught all three of the bounty #2 reverts, the `saveOrWarn` rewrite, the cap-disclosure deletion and the Gofer fold call site — six of the six mutations that survived a full green suite.
2. **A rule must state the invariant computably.** Not "these five colours are forbidden" but "compute the ratio and fail below 4.5". Not "the word `ctest` appears in the press" but "run the press against a failing fixture and require a non-zero exit". Not "`extracted` maps to `84000`" but "every staged path matches a manifest entry". R3 already demonstrates the shape.

Add to those one structural item: **assertion-carrying tests must traverse the real path.** Today, `--selftest` executes zero clicks, zero triggers and zero key events across a UI with 466 `connect` sites, while the only driver that does traverse events (`--gauntlet`) asserts nothing but window structure. That single gap explains why a call site can be disabled, a feature disconnected, or a guard inverted with 72/72 green.

### 8.5 What the process gets right, and should keep

Said last so it is not read as consolation. The habit of encoding lessons as executable rules is correct and rare — the failure is in how the rules are written, not in the idea. The backlog is honest: `CLOSER` #48 states outright that "no write-failure path is reachable under test at all", which is exactly what this assessment measured; `CLOSER` #27 records the notarisation blocker as a human decision rather than pretending it is done; `TOTAL_QUALITY.md` wrote down its own `-Wextra` deferral instead of claiming the stricter wall. `tools/verify.sh` landed the same day the stale-binary hole was noticed. The press gates genuinely fire. Several audits in this very assessment opened a finding and closed it because the fix was already in the tree.

That is a project that tells the truth about itself in prose. The remaining work is making the machinery tell the truth too.

---

*Compiled from seven independent dimension audits and their adversarial verification passes. Every number in this document was produced by a command whose output was read; figures marked ‡ in §5 were produced twice by different agents. Findings refuted or found already-closed during verification are listed in §4.0 and excluded from the count. No file in the repository was modified in the course of this assessment except this one.*
