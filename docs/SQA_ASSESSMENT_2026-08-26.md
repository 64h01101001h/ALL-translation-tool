# Software Quality Assessment — 26 August 2026 (post-sweep re-measurement)

**Method.** Same dimensional structure as the 22 and 23 August
assessments, so the three are comparable. Eight dimension reviewers
(agentic, independent, file:line evidence required) measured the
current tree WITHOUT assuming the sweep's ledger was true; the top
finding(s) of every dimension then went to an adversarial verifier
instructed to REFUTE them against the actual code. 15 agents total.
Where a reviewer credits an improvement, it cites the code, not the
ledger.

**Headline: grade B− (from C on 23 Aug, C− on 22 Aug).** No dimension
below C+. The sweep's work is measurably real — and the re-measurement
still found live defects the ledger wrongly recorded as closed, which
is the finding to respect most.

**Adversarial verification: 7 of 7 top findings CONFIRMED, 0 refuted.**
The three most serious were fixed the same day (see Disposition).

| Dimension | Grade | High | Med | Low | Improvements credited |
|---|---|---|---|---|---|
| Gate scripts (tools/*.py ctest gates): sil… | **B-** | 1 | 7 | 5 | 7 |
| Static analysis and strictness | **B-** | 0 | 4 | 5 | 8 |
| Test and pin quality (core/tests, app self… | **B-** | 1 | 4 | 4 | 10 |
| build and release integrity | **B-** | 1 | 4 | 6 | 11 |
| data honesty and docs | **B-** | 1 | 3 | 5 | 12 |
| memory and UB | **C+** | 0 | 4 | 3 | 11 |
| performance | **C+** | 1 | 4 | 4 | 11 |
| write-path honesty | **C+** | 2 | 8 | 8 | 9 |

## Same-day disposition (fixed before this document was written)

* **WP-1** `ManuscriptPane::save` — the silent-loss shape (unchecked
  write, dirty_ cleared, "saved HH:mm" composed) — CONFIRMED live
  despite a "CLOSED earlier" ledger row. Fixed: routes through
  saveOrWarn; failure keeps dirty_ and says NOT SAVED; pin +
  pretend-success mutant killed. (commit cb0857e)
* **WP-2** "Save input work" — silent return on open failure,
  unverified write, plus the block-combine loop's unchecked writes.
  Fixed, both quadrants pinned through the real dialog stub,
  mutation-killed. (cb0857e)
* **G3 vacuous** — the PROVISIONAL-never-red rule matched three exact
  substrings that C++ string-literal splits guaranteed could never
  match. The honest proximity rule convicted FOUR live red-PROVISIONAL
  sites on its first run (app/main.cpp 8313, 14234, 17758, 17766 —
  all repainted kMachine amber), gained a liveness clause (fewer than
  3 PROVISIONAL tokens = the rule refuses to pass), and a standing
  mutation entry (G3-VARIANT) proving it catches formats the old
  patterns could not see.

## The finding to respect: ledger drift

Two shapes the disposition ledger recorded as closed were live. The
lesson is structural, not personal: a ledger row is testimony, and
the only evidence is a pin the harness runs. Every same-day fix above
therefore landed WITH its pin and its kill, and the re-measurement
habit (same generator, adversarial verify) is the standing control.

## Full findings by dimension

### Gate scripts (tools/*.py ctest gates): silent-failure quadrants, subprocess honesty, and mutation-sweep anchor freshness — grade B-

**Credited improvements (evidence-cited):**
* STATIC-3 is genuinely closed: constitution_check.py now routes L3 through the shared shell_verdict contract (tools/constitution_check.py:333-365) that fails on 'exited N with no parsable finding', the except branch appends to fails printed after (line 316-320), and constitution_selftest (core/CMakeLists.txt:522) pins all three quadrants; on 23 Aug deleting reconcile_lists.py left the gate green.
* The mutation harness and a committed sweep exist at all, vs. nothing mechanical on 23 Aug (TEST-1: 13/21 survived by hand-run): tools/mutate.py enforces anchor-uniqueness (rc 3 vs rc 1), object deletion before rebuild, build verification, restore re-proof, and a LIVENESS control; tools/mutation_sweep.json commits 10 named mutations with one honestly-recorded survivor (TERMINOLOGY-NESTING, resolved as an equivalent mutant pinned by LATTICE-EXTENT-UNIQUE). Verified today: all 10 anchors match current source exactly once (grep, no stale entries), and three former TEST-1 survivors were converted to killed pins (commits 97037b5, 15bd0f7, 5831f8f).
* Six new gate suites registered since 23 Aug: vuln_check_selftest, sbom_selftest, scratch_gc_selftest, mutate_selftest, constitution_selftest (core/CMakeLists.txt:504-527), repo_licensing (core/CMakeLists.txt:575-577, commit 015cc92) and payload_manifest --selftest with BUILD-20 absence checking (walk_stage complete=True, tools/manifest_check.py:227-257) — all ran green read-only today, and repo_licensing reports 2,713 tracked files all mapped with 1 open row disclosed.
* C2 hardened from substring to liveness (commit 268bcf8): tools/constitution_check.py:269-301 now requires an uncommented invocation carrying no '|| true'/'|| :' swallower and excludes log-filename false matches; the 23 Aug version was satisfiable by a commented-out gate.
* no_vacuous_pass_check.py gained crash discrimination (signal / rc>=128 at lines 77-82), a required-shape rule so an unknown binary fails rather than being skipped (lines 60-64), and a self-guard failing if zero suites were examined (lines 98-101); verified today against the three real built binaries: all exit 1 with no fabricated measurement.
* BUILD-19: the written-vs-registered suite-count comparison at configure time (CMakeLists.txt:113-131) makes a Python3-less configure a FATAL_ERROR instead of silently dropping 12 gates — with the Qt6 hole noted in the findings.
* STATIC-8 (commit fc117c0, 26 Aug): a ruff pass over the eleven gate scripts plus an audit of all 14 subprocess.run sites without check=; sbom.py git_head() now records UNRESOLVED instead of a blank commit field on git failure (tools/sbom.py:80-87).

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| GATE-1 | high | `core/CMakeLists.txt:477` | link_truth's SKIP_REGULAR_EXPRESSION 'with no caption source' is a substring of the gate's own genuine-failure message (tools/link_truth_check.py:45), so ctest reports a real failure as SKIPPED regardless of the exit-1. |
| GATE-2 | medium | `tools/mutate.py:261` | cmd_sweep's stale-anchor report indexes the expect string as a dict (e['id'] where results tuples are (id, expect, got)), crashing with TypeError on any stale anchor — demonstrated live with a synthetic sweep. |
| GATE-3 | medium | `tools/mutate.py:249` | The sweep's liveness guard is 'if live and ...' — a mutation_sweep.json with the LIVENESS entry deleted or renamed runs the whole sweep with no control and can pass, though _meta declares the control mandatory. |
| GATE-4 | medium | `tools/constitution_check.py:132` | The constitution gate writes baseline files into the source tree during a test run and silently re-installs deleted baselines at the current count — running it today rewrote tools/constitution_hgm_baseline.txt from the stale committed 33 to 32 (demonstrated, then restored). |
| GATE-5 | medium | `tools/vuln_check.py:366` | An empty or wrong-delimiter BUNDLED_COMPONENTS.tsv yields zero components, zero OSV queries, and exit 0 ('asking OSV about 0 bundled component(s)') — a scan of nothing reads as clean, the exact rule-1 family the file's own header forbids. |
| GATE-6 | medium | `tools/reconcile_lists.py:43` | If CLOSER.md's table format drifts so the row regex matches nothing, rows={} and gated={} and the L3 gate exits 0 printing 'nothing human-gated is invisible' — a vacuous empty-input pass in the very gate STATIC-3 was about. |
| GATE-7 | medium | `CMakeLists.txt:120` | The BUILD-19 written-vs-registered guard depends on ALL_BAT_WRITTEN, which is computed inside app/CMakeLists.txt:20 and only set when Qt6 is found — a core-only configure (explicitly supported at CMakeLists.txt:89-93) gets no gate-loss comparison at all. |
| GATE-8 | medium | `tools/sbom.py:52` | read_components_tsv and read_payload_manifest silently drop rows with too few fields ('if len(parts) < 5: continue' / '< 6: continue'), contradicting the file's own rule that an unmappable component is a failure, never a blank row. |
| GATE-9 | low | `tools/mutate.py:105` | run_tests invokes ctest without --no-tests=error, so a -R filter matching zero tests returns rc 0: the mutant is misreported as SURVIVED and the restore re-proof 'restored, rebuilt, green' is vacuous. |
| GATE-10 | low | `tools/mutate.py:200` | mutate_selftest (the registered ctest gate) only pins str.count on a fixture it just wrote plus objects_for on a missing name — it never exercises mutate()'s anchor-refusal rc 3, cmd_sweep parsing, or the stale branch that actually crashes. |
| GATE-11 | low | `tools/repo_license_check.py:52` | Ledger matching uses bare f.startswith(p) with no path-boundary check, so a row like 'VERSION' or 'data/extracted/thl_' credits any tracked file sharing the prefix. |
| GATE-12 | low | `tools/link_truth_check.py:54` | The das branch prints nothing at all when data/extracted/das_pages.json is absent, though the docstring promises a skip notice and the teaching branch prints one. |
| GATE-13 | low | `tools/constitution_check.py:214` | R6 only fires when 'conflicted copy' appears in proposals_smoke.cpp fewer than 3 times but at least once — deleting the conflict drill wholesale passes the rule silently. |

### Static analysis and strictness — grade B-

**Credited improvements (evidence-cited):**
* STATIC-1 wall is now mechanical: tools/verify.sh:61-72 fails the gate on any warning from own code (path-anchored to app|core|ocr|tools, both third_party spellings excluded), verified workable because Unix Makefiles compile with absolute source paths (checked cmake-build-release/core/CMakeFiles/allcore.dir/build.make); the formerly-dead deskew clamp is wired at app/main.cpp:28495-28496 (commit 1c9810d). On 23 Aug the wall was broken, unconsumed, and contradicted by two docs.
* [[nodiscard]] now covers every core store save path: proposals.h:91, glossary.h:25 (the site the 23 Aug audit convicted as live STATIC-2), catalog_actions.h:55, dossier.h:34, plus regenreg.cpp:34 writeLines — up from 3 annotations with GlossaryStore::save missing; both GlossaryStore call sites are checked (app/main.cpp:14498, 14785), and the core stores' write bodies now flush-and-close before judging (dossier.cpp:83-87, comments.cpp:111-115, colloquial.cpp:102-106).
* STATIC-3 closed with machinery, not prose: shell_verdict is shared by rules L2 and L3 in tools/constitution_check.py, has a cmd_selftest, and that selftest is a registered ctest suite (core/CMakeLists.txt:520-523) — deleting reconcile_lists.py can no longer print 'all rules hold'.
* STATIC-6 closed: the always-break loop in core/src/sanskrit.cpp is now a straight conditional with the anusvara contract pinned in a comment (sanskrit.cpp:601-616); my fresh always-break scan across all of core/src and app found zero remaining defects (the single unconditional-break hit, app/main.cpp:4615, is a deliberate take-first-lookup idiom inside a selftest fixture).
* STATIC-9 closed: a contraction row whose syllable counts fail to parse is skipped whole instead of loading with silent zeros (core/src/contractions.cpp:39-47).
* STATIC-10/STATIC-11 closed as claimed: rd = NULL at app/thirdparty/diff_match_patch.cpp:1480, the dmp TU compiles -w (app/CMakeLists.txt final stanza), vendored SQLite include marked SYSTEM (core/CMakeLists.txt).
* Four new constitution rules with real teeth landed since 23 Aug (commits be095bb, 57e1aac, 85582ea, 268bcf8): G1 frozen green vocabulary, G2 hgm_gloss consumer census, G3 provisional-never-error-red, C2 press-gate liveness (mutation-derived, rejects '|| true' disarming); plus BUILD-19's written-vs-registered battery comparison in the top CMakeLists that FATAL_ERRORs a configure that silently dropped Python gates.
* Strictness remains suppression-free and honest: zero NOLINT, zero -Wno-*, zero #pragma diagnostic, zero #if 0 in own code, and the constitution's ruled-exception escape hatch has zero uses in the tree; constitution_check.py on today's tree: 'all rules hold', rc=0.

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| STATIC-R1 | medium | `tools/verify.sh:66` | The warning wall only sees TUs that recompile in its own invocation, and the press never checks warnings at all (package_macos.sh:86 pipes the build to /dev/null; constitution rule C2 requires ctest/shot_diff/constitution live in the press but no warning gate). |
| STATIC-R2 | medium | `tools/constitution_hgm_baseline.txt:1` | The G2 binding-gloss ratchet carries one site of slack right now: the committed baseline says 33 but the tree has 32 hgm_gloss sites (proved live — running the gate auto-lowered the file to 32 and left the tree dirty; I restored it). |
| STATIC-R3 | medium | `app/main.cpp:30355` | The [[nodiscard]] sweep stopped at the core stores: Qt-side saves have no static net and live discards exist — ManuscriptPane::save writes unchecked, clears dirty_, prints 'saved HH:mm', and its bool is discarded at both call sites (30327 autosave, 30375 exportRtf); sibling shapes at 27846 (silent return, no warnWriteFail, 30 lines from the branch that warns), 27824/27847, 3566-3571, 24836 also print success over unchecked f.write(). |
| STATIC-R4 | medium | `app/main.cpp:37073` | The R3 modal census (constitution_check.py:97-103) counts only static-call forms of QMessageBox/QInputDialog, so constructor-form dialogs are structurally invisible — one live uncounted site exists: QMessageBox box(&win) ... box.exec() at 37073/37091. |
| STATIC-R5 | low | `tools/constitution_check.py:44` | R1 still uses the narrow pattern the 22 Aug audit told it to widen: QRegularExpression\s*\( misses the named-constructor form, leaving 24 static-const plus dozens of local sites (e.g. app/main.cpp:4230) unpoliced — my widened-net probe over core+app found no live collapsed escape today, but the gate cannot see that form, and its scan scope also excludes analyze_cli.cpp and ocr/. |
| STATIC-R6 | low | `app/ux_tokens.h:41` | G1's frozen-green census reads app/main.cpp only, while the color tokens — including the binding green kAct #1E6B4E — live in app/ux_tokens.h, which is exactly where the R7 extraction direction moves color decisions. |
| STATIC-R7 | low | `CMakeLists.txt:47` | STATIC-4 remains fully open (accepted, grouped with CI on Adam): -Wall only — no -Wextra, no -Werror in the toolchain, no .clang-tidy, ruff absent from PATH (only a stale .ruff_cache), and the exported compile_commands.json still has zero consumers. |
| STATIC-R8 | low | `tools/constitution_check.py:131` | The constitution gate mutates the tree on every run where a census count drops (R3 and G2 baselines auto-lower and are rewritten in place), and nothing forces the lowered file to be committed — this is the mechanism that produced STATIC-R2's slack. |
| STATIC-R9 | low | `ocr/CMakeLists.txt:19` | The ocr vendor includes (onnxruntime at :19, OpenCV at :30) are still plain -I, not SYSTEM — the sweep's STATIC-11 closure covered only vendored SQLite and the dmp TU, not the original ocr sites named in the 23 Aug report. |

### Test and pin quality (core/tests, app selftests, mutation sweep coverage) — grade B-

**Credited improvements (evidence-cited):**
* A standing mutation harness with a committed sweep now exists and was run on the current tree: tools/mutate.py (unique-anchor refusal, object deletion, build-exit check, restore-and-reprove) + tools/mutation_sweep.json (10 entries, liveness control, drift-as-failure); scored 9 killed + 1 documented-equivalent survivor on 2026-08-26 (commit 206b2a7). Baseline had no harness at all.
* The four same-day-revertable critical fixes from the 23 Aug TEST-3 finding are each pinned by a sweep entry that was verified to kill today (TEST3-BOUNTY8, TEST3-PERF1-SKIP, TEST3-FAIL2, TEST3-STATIC1-META).
* Three of five TEST-1 boundary-flip survivors converted to killed with real pins (core/src/affixnorm.cpp len>4, core/src/title_xlat.cpp n>=2 via a dedicated two-title bank, core/src/searchnorm.cpp inclusive range end); terminology.cpp proven equivalent-by-invariant with a 42,469-span LATTICE-EXTENT-UNIQUE sweep in lattice_smoke rather than hand-waved.
* Arranged write-failure drills grew from 2 to at least 6: storeflush_smoke (RLIMIT_FSIZE, both directions, small-store regime chosen deliberately), the export drill at app/main.cpp:29495-29549 (asserts failure reported, no green success line, truncated file removed), the roster RLIMIT-0 drill at app/main.cpp:38258-38290 (size and reload-count unchanged), the chmod-500 read-only-directory drill at app/main.cpp:15902-15916, plus shortwrite_probe and the streamWriteOk unit pins.
* The two zero-coverage surfaces from TEST-7 are drilled through real events: SettingsDialog round-trips edited widgets through the real Save button click into QSettings and back (app/main.cpp:33367-33395, with user-settings save/restore); ScanPane pins refusal state, arming, and exact strip-to-page coordinate math (pageX==125, clamps 9/7; app/main.cpp:28122-28164).
* The exact substring pins the 23 Aug audit named were rebuilt with numbers, absence clauses, and negative controls: corpusCountLine pins all three numbers plus the unknown branch plus a no-manufactured-cap control (app/main.cpp:16032-16047); the concordance pins use live spine_.corpusCount as the oracle with fixture-holds guards and a banned-string pin (19319-19338); corpusCount 0-vs--1 distinction pinned both ways (4406-4422).
* Ratchet floors with written rules replaced decorative floors: title_xlat_smoke floors at 85%/55% sitting five points under the measured 90.2%/62.0% with the ratchet rule in a comment (commit f6812af); forward_battery kFloor=99.95 with total>30000 anti-vacuous clause; towylie_battery requires match==total over 100k+ oracle rows.
* The gauntlet seed is unfossilized: ctest runs '--gauntlet auto 300' (app/CMakeLists.txt:93) seeded by Julian day (app/main.cpp:40114), reproducible within a day and printed on every run.
* Fixture honesty holds end-to-end: configure-time ***Skipped with the missing path named (cmake/AllFixtureTests.cmake), fixture_gate_check self-tests the wrapper hermetically, no_vacuous_pass holds the three formerly-vacuous suites red, engines_battery counts a missing reference file as a FAILURE not a skip, and the FAIL-4 dialog seam gained its first real consumer (app/main.cpp:5676).
* First memory assertion in the tree: peak physical footprint measured at selftest end with both a 5.5 GB ceiling and a 0.05 GB sanity floor (app/main.cpp:39449-39468); wall-clock ceilings derate x4 under ALL_SANITIZED, exported by tools/sanitized_battery.sh:31.

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| TP-1 | high | `tools/mutation_sweep.json:1` | The standing mutation set holds 10 entries across 8 files while the 26 Aug sweep closed ~57 findings across 15+ production files, and at least 8 fixes the ledger calls 'mutation-verified/proven' (FAIL-3, FAIL-11, FAIL-12, TEST-7 both drills, MEM-7, BUILD-10, DATA-3/G2, PERF-9) have no recorded sweep entry, so those verifications are one-off claims that cannot be re-run. |
| TP-2 | medium | `core/CMakeLists.txt:518` | No gate ever runs the mutation sweep: only 'mutate.py --selftest' (harness plumbing) is registered in ctest, and the sweep appears nowhere in tools/verify.sh, tools/package_macos.sh, or tools/release.sh, so the drift-detection ratchet built into cmd_sweep only fires when a human remembers to invoke it. |
| TP-3 | medium | `tools/mutate.py:261` | The stale-anchor reporting path added in commit 206b2a7 crashes with TypeError: the loop unpacks (id, expect) tuples and then indexes the expect STRING with e["id"], verified by executing the same code shape. |
| TP-4 | medium | `app/main.cpp:28681` | Several named TEST-1 survivor sites remain unpinned AND unrecorded in the sweep despite the file's stated rule that known gaps get expect=survived entries: the OCR legality counters (app/main.cpp:28552, 28681 — no test asserts a nonzero flag count from a known-bad fixture), the pane-level indexCapped disclosure wiring (16511/16517/16536 — core cut flags are pinned in gofer_bound/libindex but nothing drives an over-cap search through the pane and asserts the notice renders), the lattice.cpp:30 boundary flip, and the Draft tier label. |
| TP-5 | medium | `app/main.cpp:2639` | saveOrWarn's short-write branch (f.write(body) != body.size() ·· !f.flush() after a successful open) is exercised by no drill: the TEST-2 drill covers only open-failure (nonexistent dir, chmod-500 dir) plus the success path, and all three RLIMIT_FSIZE drills route through writeApprovedExport/streamWriteOk or catalogRosterSave, never through saveOrWarn. |
| TP-6 | low | `core/tests/verbstems_smoke.cpp:41` | Two can't-fail pins: line 41's CHECK(true, "hits carry their attestation sources") sits inside 'if (!h.sources.empty())' so it silently never executes when NO hit carries sources (pass either way), and line 46's condition 'vs.lookup(...).empty() ·· true' is a literal tautology; gofer_smoke.cpp:77 similarly CHECK(true)s a mixed query whose hit count is printed but never asserted. |
| TP-7 | low | `app/main.cpp:37502` | Two skip idioms coexist in the selftest and one lies to the tally: sk(true, "library index absent — skipped") logs a [PASS] for work not done, while the Hunt palette correctly logs a [SKIP] line with no PASS (33948). |
| TP-8 | low | `app/main.cpp:38917` | The T4 perf ceilings were re-loosened 10x from the 23 Aug retightening (kLookupMax 25→250, kCorpusMax 70→400, kUniMax 30→250, kPronMax 20→200) under TEST-12's anti-flap rationale, but unlike title_xlat there is no written ratchet-at-measure rule beside them, only the printed measured numbers. |
| TP-9 | low | `tools/mutate.py:246` | cmd_sweep maps every non-zero, non-3 exit to 'survived', including the build-failed-on-mutant case whose own printout says the run 'proves NOTHING', so an uncompilable sweep entry is scored as a surviving mutant (drift → regression) rather than reported as an invalid mutation. |

### build and release integrity — grade B-

**Credited improvements (evidence-cited):**
* Payload gate is real and passes on the real artifact: `python3 tools/manifest_check.py --press . dist/stage team` re-run in this audit returns rc=0, 'every staged path is manifested', with honest GATED notes for the spine and teaching index; the gate walks the finished stage (not a keyword), and its selftest (11 checks incl. UNPRUNED/WRONG MODE/UNRESOLVED TERMS/README stamp) is a registered ctest suite (core/CMakeLists.txt:580). Aug 23 this was BUILD-4's keyword match.
* Absence is now judged, not just presence (BUILD-20 closed): manifest_check.py --press runs walk_stage(complete=True) (tools/manifest_check.py:646) and fails MISSING PAYLOAD for any shipping row not staged (OPTIONAL rows exempt, marked in the licence field); staging copies are de-&&'d with explicit failure exits (tools/package_macos.sh:360-364); the reference.db/das/ocr_models rows carry no OPTIONAL, so their `|| true` copies are backstopped.
* codesign --verify is fatal (tools/package_macos.sh:262-263) and hdiutil verify gates every DMG (615-616) — BUILD-8/BUILD-16 closed as the sweep claims, verified in code under set -euo pipefail.
* A missing visual baseline FAILS instead of passing (tools/shot_diff.py:31-48), with a single explicit printed escape (SHOT_DIFF_ALLOW_EMPTY=1) — BUILD-9 closed.
* The press gates on the FULL battery: skip counting stops packaging (tools/package_macos.sh:102-108), and the Aug 26 press log (dist/ctest_press.log) shows 86/86 passed, 0 '***Skipped' — BUILD-7's press side holds with fresh evidence.
* The sanitized (ASan+UBSan) battery now has a trigger and it gates tagging: release.sh:51-55 runs tools/sanitized_battery.sh and unwinds the version commit on failure BEFORE `git tag` at release.sh:66 — MEM-5's dead gate is wired (caveats: findings REL-2/REL-5; no tag has yet passed through it, the only tag v1.0.0-rc.1 predates the wiring).
* The press can run as pure verification: ALL_PRESS_VERIFY_ONLY=1 exits after all gates, before install and DMG (tools/package_macos.sh:573-576) — BUILD-13 closed in the script (no CI consumer exists yet).
* Dependency roll-call: missing Brewfile formulas stop the press, drift is a note, exact versions recorded per press (tools/package_macos.sh:54-64) — BUILD-21 partially closed (still no Brewfile.lock.json, no CI).
* BUILD_MANIFEST.{json,txt} is written BEFORE the payload gate and has manifest rows (tools/package_macos.sh:513-520 then 533) — BUILD-24 closed; it honestly records git_commit and git_tree=DIRTY (tools/build_manifest.py:208-226), which is exactly what let this audit prove finding REL-1.
* Gate-on-gate exists for the press: constitution C2 verifies the battery, shot_diff and constitution invocations in package_macos.sh are live and un-swallowed, with the log-filename false-positive fixed (tools/constitution_check.py:270-301) — BUILD-10 closed for the press.
* verify.sh remains exit-status honest and added the warnings wall and skip accounting (tools/verify.sh:60-74, 96-107).

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| REL-1 | high | `tools/package_macos.sh:41` | The artifact's version comes solely from the VERSION file with no clean-tree or tag-consistency check, and step 7 (lines 608-609) deletes and re-cuts dist/Diamond-Cutter-Translation-Tool-$VERSION.dmg, so any later press silently replaces a tagged release's artifact with different bits under the same name — proven live: the DMG in dist/ (2026-08-26 13:22) was pressed from commit 8d5d60f with git_tree=DIRTY per its own BUILD_MANIFEST.json, while tag v1.0.0-rc.1 points at a290bb1 (2026-08-21). |
| REL-2 | medium | `tools/sanitized_battery.sh:71` | The sanitized battery prints 'SANITIZED: every suite green under ASan + UBSan' on bare ctest exit 0 with no '***Skipped' count — the exact BUILD-7 lie the project already named and fixed in verify.sh and the press, unapplied to its newest gate. |
| REL-3 | medium | `tools/package_macos.sh:489` | In the staged-launch gate, `wait $LPID; LEXIT=$?` under set -euo pipefail aborts the whole press the instant the staged app exits nonzero (reproduced: a bash repro of the idiom exits with the child's code before the next line), making the three-attempt retry, the per-attempt diagnostic, and the log tail at lines 490-495 unreachable dead code for every nonzero exit. |
| REL-4 | medium | `tools/package_macos.sh:85` | Release presses build incrementally into the shared everyday tree (cmake-build-release) with no clean-first and no freshness tripwire, while verify.sh's own header documents the cp-restore mtime hazard (a restored file older than its object makes make skip the rebuild and run the stale code) — recommendation #35's stale-binary tripwire is not implemented anywhere (no mtime/R9 check in constitution_check.py). |
| REL-5 | medium | `tools/release.sh:51` | No gate-on-gate covers release.sh: constitution C2's liveness check inspects only package_macos.sh, so deleting or `·· true`-disarming the sanitized-battery block (lines 51-55) or the press-before-tag ordering fails no check — the exact decay mode MEM-5 recorded (a written gate with no trigger) reintroduced one level up. |
| REL-6 | low | `tools/package_macos.sh:86` | The press's Release build discards compiler output (`cmake --build ... >/dev/null`) and applies no STATIC-1 warnings wall, so a warning in our own code that fails tools/verify.sh:66-74 sails through the press unexamined. |
| REL-7 | low | `tools/package_macos.sh:17` | The header documents `--skip-build` but the mode case at lines 35-37 exits 2 on it, making the flag impossible and the conditional at line 83 dead code — and because the battery, its skip gate, and the build all live inside that same `if`, a maintainer who 'fixes' the case to match the docs silently disables the build AND the full-battery gate together. |
| REL-8 | low | `tools/package_macos.sh:79` | The step-0 release gate sends validate_release.py's stdout — where all its [FAIL] reasons print — to /dev/null, so a firing gate reports only 'RELEASE GATE FAILED' with zero evidence. |
| REL-9 | low | `tools/release.sh:43` | The release commit hardcodes a Co-Authored-By trailer, contradicting the repo's recorded no-trailer authorship rule (recent history carries none; the CHANGELOG generator at line 32 even greps trailers out, acknowledging the convention the commit line violates). |
| REL-10 | low | `tools/release.sh:63` | Neither script has a failure trap: a press that fails after step 6c (install, package_macos.sh:578-604) or mid-hdiutil leaves /Applications updated and possibly a partial version-named DMG in dist/ while release.sh `git reset --hard`s the version commit away. |
| REL-11 | low | `tools/package_macos.sh:573` | No CI exists (no .github, no Brewfile.lock.json): every gate in this dimension runs only by hand on one machine, and ALL_PRESS_VERIFY_ONLY — written expressly as 'the mode a CI runs' — has zero consumers. |

### data honesty and docs — grade B-

**Credited improvements (evidence-cited):**
* Corpus-count honesty swept where it is a claim: commit 31cc9eb (2026-08-26) replaced 35,601 with 42,199 in DEMO_TUESDAY.md, README, and TODO history was deliberately preserved; the shipped spine reproduces it (meta n_corpus_segments=42199 in build/hgm_spine_v27_2.db).
* Every headline README number now reproduces against the spine DB, verified by direct SQL: 105,634 entries, 12,004 glossed, tier split 114 curated / 7,980 glossary / 3,910 auto-aligned, 105,576 distinct wylie (the pronounce battery denominator), 105,546 distinct Tibetan (the botok denominator).
* Suite-count honesty is now self-enforcing: 86 add_test in source CMakeLists equals 86 registered tests in the freshly generated cmake-build CTest files; README:86 quotes 86 dated 2026-08-26; the BUILD-19 configure guard (CMakeLists.txt:98-125) refuses a build that silently drops gate suites; the About pane quotes the build-time ALL_TEST_BATTERIES macro (app/main.cpp:36435), not a hardcoded number.
* DATA-4 closed for real: installedCollectionSummary (app/main.cpp:22612-22641) skips ' META.' companions with a red-first pin (app/main.cpp:15950-15972); measured on the real library: 927+3,423+3,236 = 7,586, matching the sweep's claim.
* DATA-9 closed for real: all three Hunt-palette lanes render the reader's label [PROVISIONAL] instead of the raw storage token, with keystroke-driven pins asserting both presence on the auto-aligned row and absence on the glossary row (app/main.cpp:33851-33881, 34057-34090).
* DATA-5 closed with anti-pins banning the old wording ('hit(s) shown' / bare '60 hit(s)') at app/main.cpp:16026-16077; counts now say 'found' with honest so-far semantics.
* DATA-7's visible half verified: both 84000 cut classes disclose counts ('N further definitions ... not shown — this layer is reference only' and 'N duplicate lines ... not shown') at app/main.cpp:1541-1555.
* New mechanical honesty gates exist and two of three work: G2 hgm_gloss consumer census with baseline ratchet (tools/constitution_check.py:111-139; count 32 vs baseline 33, self-lowering) and G1 frozen green vocabulary (tools/constitution_check.py:141-166) — an independent re-scan of app/main.cpp found zero unvocabularied saturated greens.
* The strongest authority claim in the app ('HGM has:') now travels with its tier: hgmGlossPhrase/equivalentsOwner helpers (app/main.cpp:17679-17694) make it impossible to print that phrase over an auto-aligned gloss without '[PROVISIONAL], not HGM's own English'.
* The three USER_MANUAL copies (data/help, docs/distribution, dist/stage) diff to zero — the 23 Aug 'diverged 58→78 lines' low finding is closed.
* Red-painted provisional marks shrank from the 23 Aug measured 18 sites to 5.
* Reference/binding separation reads clean everywhere sampled: AI glossary block labeled 'AI-DRAFTED, PROVISIONAL · not Geshe Michael's English' (app/main.cpp:1240), machine-located teachings wear the MACHINE badge with N-of-M disclosure and 'the recording is the authority' (app/main.cpp:1619-1631), Hopkins/84000/MVP consistently 'reference only'.

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| DH-1 | high | `tools/constitution_check.py:171` | The G3 'PROVISIONAL never wears error-red' gate is vacuous: its three literal substring patterns match zero occurrences in the current tree because every real violation splits the string literal across source lines, and cmd_selftest (line 368) exercises only the L2/L3 shell-out rules, never G3. |
| DH-2 | medium | `app/main.cpp:17683` | Five surfaces still paint provisional marks error-red #b00 — hgmGlossPhrase (17683) and equivalentsOwner (17691) used by the Drills answer key and Reviewer report, the teachings report gloss (8240), the carving hover tooltip (14160), and the Draft scaffold chip '?' (19571) — against USER_MANUAL.md:375's promise that provisional 'looks provisional (amber) wherever it appears'. |
| DH-3 | medium | `app/main.cpp:19571` | The Draft scaffold's chip renders the provisional marker '?' in red #b00 while the legend 38 lines below (19609) tells the reader the provisional mark is the amber '?' (#B4540A) — one HTML surface contradicting itself. |
| DH-4 | medium | `docs/DEMO_TUESDAY.md:253` | The demo speaking script still coaches the presenter to say '8,985 texts installed — Kangyur 1,011, Tengyur 3,439, Sungbum 4,535' — the exact 18% overstatement DATA-4 was closed for — and the line survived today's stale-numbers sweep commit (31cc9eb) that edited this very file. |
| DH-5 | low | `README.md:81` | README:81 and docs/MAINTAINERS.md:28 both state '37 of the suites' read untracked data, but the current generated CTest tree (cmake-build, regenerated 2026-08-26 13:56) labels 40 unique suites 'fixture'. |
| DH-6 | low | `docs/TOTAL_QUALITY.md:41` | The T5 monolith map claims app/main.cpp = 32,896 lines with per-class anchors, and MAINTAINERS.md:12 repeats '~33k lines' while instructing editors to read the map 'before any large edit' — the file is 40,382 lines and the anchors are off by thousands (map: HuntPalette 28212, actual: 33745; CatalogPane 25480, actual: 30799). |
| DH-7 | low | `docs/TESTING_CAMPAIGN.md:77` | Adam's current walkthrough (rebuilt 2026-08-22) instructs the tester to check tier badges as 'HGM vs PROVISIONAL (#b00) vs reference', codifying the forbidden error-red as the expected provisional colour in direct contradiction of the manual's amber promise and constitution G3. |
| DH-8 | low | `core/tests/engines_battery.cpp:6` | The battery's header comment still says the acipToEwts oracle runs over '35,601 pairs' while the battery iterates the shipped spine's 42,199 segments and README:88 claims 42,199 at 100.000%. |
| DH-9 | low | `docs/FEATURE_STATUS.md:1` | The 'Feature status register — everything discussed' presents live ✅/🟡/⬜ statuses 'verified against the app source and test suites (30/30 green)' but was audited 2026-08-07 (last touched 08-14), against today's 86 suites and a substantially larger app. |

### memory and UB — grade C+

**Credited improvements (evidence-cited):**
* MEM-1 (23 Aug high, ship-blocker) closed and verified: ocr/src/recognize.cpp:21-36 adds ocrCheckVocabCovers(), called at recognize.cpp:282 before the decode loop - a model whose output width exceeds charset+1 is refused with an actionable error instead of appending ~10KB of heap-overread bytes to the recognized Tibetan.
* MEM-2 (23 Aug high, ship-blocker) closed and verified: app/main.cpp:2444-2465 normalizes null QStrings and wraps diff_main in catch(...) in the one shared function (editionDiffHtml); compareWith at app/main.cpp:27586-27599 refuses null partners with status text plus a 500k size cap; both are pinned in the selftest (app/main.cpp:27045-27058, 26094-26100).
* MEM-3 closed: core/src/progress.cpp:122-128, 211-216, 229-235 null-guard sqlite3_column_text with skip-not-fabricate semantics and a comment recording the reproduced SEGV.
* MEM-4 closed at the public API: botok's classifiers index via .at() (canonical-IndexError rationale at core/src/botok_chunks.cpp:369-370; 21 .at() sites across the four botok files); internal loops keep raw [] but with local loop bounds.
* MEM-6 closed: app/main.cpp:28434 reads the deskew checkbox on the UI thread and the worker uses the captured bool; ocrStop_ is std::atomic<bool> (28819); ~ScanPane joins the worker before teardown (28282-28286); worker-to-UI publication goes through queued QMetaObject::invokeMethod with `this` as context (28533, 28589).
* MEM-7 partially closed: paren-nesting recursion capped at 200 with a friendly refusal, core/src/gofer_ast.h:93,136-142 (the OR/NEAR-chain axis is NOT capped - see finding MEM-N1).
* MEM-9 closed: connect(qApp, aboutToQuit, this, ...) carries the pane as receiver (app/main.cpp:7159-7164) and the 0ms singleShot carries idBox as context (app/main.cpp:29136); an audit of all 16 QTimer::singleShot calls and the sole connect(qApp,...) found no remaining context-less shape whose sender outlives its pane (the four TimedNam network managers are pane value-members, so replies die with their pane).
* MEM-5 substantially closed: tools/release.sh:51 runs tools/sanitized_battery.sh (ASan+UBSan over the full ctest battery) as a hard release gate - the 23 Aug state had the script with no trigger at all. Still absent: CI, TSan, leak detection.
* MEM-8 sites finally named (the 23 Aug finding's text never identified them and docs/SQA_MEDIUM_SWEEP_2026-08-26.md:47 explicitly deferred to this report): (1) the entry_ix sentinel family - core/include/allcore/lattice.h:22 declares `int entry_ix = -1` and reader.cpp:163/306/328 plus ~12 app/main.cpp sites (7656, 8494, 8551, 8571, 17972, 17987, 19436, 19447, 19514, 19555, 19565) index doc.entries[span.entry_ix] unchecked, safe only because every producer in lattice.cpp (lines 154, 165, 302, 304) interns; (2) the Gofer parser's t[i] reads (gofer_ast.h:98, 105, 115, 130, 146), bounded only by lex() appending an END sentinel at gofer_ast.h:80 and no production consuming it.
* STATIC-9 closed: core/src/contractions.cpp:39-47 skips a row whose syllable counts fail to parse instead of loading silent zeros.
* STATIC-10 closed: app/thirdparty/diff_match_patch.cpp:1480-1481 initializes rd/last_rd to NULL so the delete[] on early exit paths never sees garbage.

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| MEM-N1 | medium | `core/src/gofer.cpp:38` | The MEM-7 recursion fix capped only paren nesting; both Gofer evaluators (evalNode at gofer.cpp:38-45 and the self-recursive eval lambda at libindex.cpp:418-465) recurse down the lhs of the left-leaning OR/NEAR chain the parser builds iteratively, and ~Node's unique_ptr chain destruction (gofer_ast.h:88) is recursive too, all with no depth bound. |
| MEM-N2 | medium | `core/src/libindex.cpp:194` | Three sites construct std::string straight from an unguarded sqlite3_column_text (libindex.cpp:194, 529, 538) - the identical NULL-to-SEGV shape the sweep fixed in progress.cpp, whose own comment (progress.cpp:211-212: 'declared NOT NULL, but the same MEM-3 guard costs one branch and outlives the next schema edit') condemns exactly these reads. |
| MEM-N3 | medium | `core/src/hypfile.cpp:65` | parseHypFile calls std::stoi on a digit run validated only for being digits, not for magnitude, and its one caller (app/main.cpp:26336, importHyp) wraps nothing in try/catch, so std::out_of_range crosses app.exec() and aborts - the same crash class as the closed MEM-2. |
| MEM-N4 | medium | `core/src/qc.cpp:26` | splitSections calls std::stoi on an unbounded digit run from report markdown, violating qc.h:29's explicit 'Never throws on malformed reports' contract, and the caller (app/main.cpp:3553) feeds it text accumulated from a network reply with no try/catch. |
| MEM-N5 | low | `app/main.cpp:27603` | The 23 Aug prescription 'wrap both diff_main sites in catch(...)' is half-applied: editionDiffHtml got the catch but compareWith's diff_main at 27603 only guards isNull, so diff_match_patch's remaining bare throws (diff_match_patch.cpp:1135's internal-invariant 'Previous diff should have been an equality') would still abort the app. |
| MEM-N6 | low | `core/src/tibcal_day.cpp:240` | STATIC-7 is unchanged since 23 Aug: monthName leaves e = -1 when yrElem falls outside 0..4 and then reads kMElem[e % 5] = kMElem[-1], protected only by the 'year < 2010' floor in the different function kckYearMonths (tibcal_day.cpp:250) - no local guard was added and the finding appears nowhere in the sweep doc. |
| MEM-N7 | low | `core/src/lattice.cpp:67` | OverlayDoc::coverDepth writes d[t] for every token position of every span into a vector sized tokens.size() with no clamp, and the span-fill loop 300 lines into app/main.cpp (7546) does clamp the same iteration - the bound rests entirely on every lattice producer keeping span.end <= tokens.size(), the emergent-invariant shape MEM-8 describes. |

### performance — grade C+

**Credited improvements (evidence-cited):**
* PERF-1/PERF-3 indexed path genuinely fixed: the 34x ORDER BY regression is gone - the TERM scan now uses 'ORDER BY f.rowid LIMIT ?' with a bare-term scanLimit of limit+1 (core/src/libindex.cpp:416-448), and a unit pin proves O(limit) behavior: term_rows_visited asserted at 5-6 rows for limit 5 (core/tests/libindex_smoke.cpp:202).
* PERF-4 closed at the indexed site with a real mechanism, not a pattern claim: sqlite3_progress_handler pump every 2,000 VM steps with abort-on-false (core/src/libindex.cpp:380-401), wired to processEvents + Stop in the Search pane (app/main.cpp:16505-16510), and pinned by tests that assert the pump fires during a scan and a false return aborts it (core/tests/libindex_smoke.cpp:208-219).
* goferSearchFiles is now memory-bounded and disclosing where it was a 23.5 GB / 115 s uncapped cross product: kWindowCap=200000 on the NEAR join (core/src/gofer.cpp:73,144-146), the file walk counts skipped files instead of silently truncating (gofer.cpp:97), and the pane reports scanned/skipped and the window ceiling (app/main.cpp:16690-16696).
* PERF-10/FAIL-4 closed structurally: all five QNetworkAccessManager members are TimedNam with a 20 s transfer timeout set in the constructor (app/main.cpp:39-43; instances at 370, 3594, 14306, 20868, 25604) - the measured 59.8 s black-hole freeze is now bounded at 20 s.
* PERF-9: the tree's first memory assertion exists - task_vm_info ledger_phys_footprint_peak measured at selftest end against a 5.5 GB ceiling (app/main.cpp:39452-39468); the 23 Aug finding was '0 memory assertions anywhere in the tree'.
* PERF-2 guards half: both Overlay perf guards self-provision their fixture (largest installed library text) instead of skipping on a hardcoded path, so they now execute on this machine (app/main.cpp:6115-6190) - both were fixture-dead on 23 Aug.
* PERF-15 closed: the duplicate 1.7 s COUNT(*) is gone - the index report reuses st.lines paid by update() (app/main.cpp:25426-25433, core/src/libindex.cpp:355).
* PERF-5 partially addressed: ensureSegmenter has a modal progress dialog, Stop, wait cursor, and cancel-keeps-nothing semantics (app/main.cpp:14836-14935), and it now fires only when the attestation toggle is checked rather than on every document open (app/main.cpp:7576-7577); tm84000's cold build got a wait cursor (app/main.cpp:570-577).
* PERF-14 closed: the library title index and the title pair bank are lazily built once and cached (app/main.cpp:11418-11441 libIndexBuilt_, 31941-31962 pairBankBuilt_).
* The process caught its own regression: an O(notes^2 x glosses) footnote recount introduced during the sweep timed out the release selftest at 300 s and was found and fixed within a day (commit 8d5d60f; the one-pass fix with its confession is at app/main.cpp:1293-1300).
* PERF-13 did NOT get worse: 22 top-level panes eagerly constructed then and now (23 tabs.addTab occurrences in both the 23 Aug snapshot 61399cf and HEAD; the 23rd is the regrouping call at app/main.cpp:35643).

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| PERF-R1 | high | `core/src/gofer.cpp:68` | The unindexed library search - the DEFAULT path on a fresh install with no .index.db - runs entirely on the GUI thread with no pump, no cancel hook, and no progress: goferSearchFiles takes no callback, and the pane checks stopped_ only between directories (app/main.cpp:16487), so with the whole library as one checked directory the Stop button is dead for the full scan. |
| PERF-R2 | medium | `core/src/gofer.cpp:120` | The two Gofer evaluators have already re-diverged despite the comment at gofer.cpp:71-74 claiming they 'cannot drift apart again': LibraryIndex::search caps TERM at scanLimit and OR at kScanCap, but goferSearchFiles caps only the NEAR join - its TERM node (line 120-130) and OR node (line 131-135) accumulate unbounded windows. |
| PERF-R3 | medium | `app/main.cpp:565` | tm84000() still performs its ~5.6 s / 590 MB cold FTS5 build (27.8 MB tsv.gz -> 293 MB db) synchronously on the GUI thread inside a card render, with only a wait cursor - no progress dialog, no Stop - while its sibling ensureSegmenter got the full house pattern. |
| PERF-R4 | medium | `app/main.cpp:35020` | PERF-13 unchanged (known/parked, explicitly not worse): all 22 panes are constructed eagerly before win.show() (app/main.cpp:35020-35505), the 23 Aug measurement was 2.3-3.7 s and 446 MB before the user does anything, and startup time remains unpinned by any test. |
| PERF-R5 | medium | `app/main.cpp:7206` | Document open (openFile -> loadDoc -> buildOverlay) is still one synchronous GUI-thread pass with no progress dialog and no cancel - the honestly-open freeze half of PERF-2 (4.4 s warm / 9.3 s on the largest text at the 23 Aug measurement) - and the guards that now cover it assert ceilings of sanDerate(15000) and sanDerate(20000) ms (app/main.cpp:6156, 6181) against measured 1.0-2.7 s, roughly 7x headroom. |
| PERF-R6 | low | `app/main.cpp:25367` | updateIndex() and the refold still run on the GUI thread (progress dialog + working Stop + per-file commits are present, and the single-threaded design is documented as deliberate at app/main.cpp:25374-25381), so a full first index of 8,988 files still occupies the UI for minutes with the event loop kept alive only by processEvents. |
| PERF-R7 | low | `app/main.cpp:3513` | The AI-analysis stream re-renders the ENTIRE accumulated report on every received chunk - accum_ += t then report_->setPlainText(whole accum_) - making total layout work quadratic in report length. |
| PERF-R8 | low | `app/main.cpp:9543` | The folio download loop's per-request QEventLoop connects only finished->quit with no canceled->quit wiring (the ledger item 15 fix was applied elsewhere but not here), so Stop during a stalled fetch waits out the 20 s transfer timeout before the loop's cancel check at line 9540 can run. |
| PERF-R9 | low | `app/main.cpp:39463` | The new PERF-9 memory assertion's 5.5 GB ceiling sits 6.5x above the 0.85 GB observed peak - generous enough that a 4 GB accumulation regression (half the 23 Aug PERF-1 shape) would pass the tree's only memory gate. |

### write-path honesty — grade C+

**Credited improvements (evidence-cited):**
* All seven core store save paths (GlossaryStore, DossierStore, CommentStore, colloquial, ProposalStore, ActionLedger, regenreg writeLines) now flush+close and judge !fail() before returning true — core/src/glossary.cpp:56-59, dossier.cpp:92-95, comments.cpp:119-121, colloquial.cpp:112-114, proposals.cpp:219-222, catalog_actions.cpp:131-134, regenreg.cpp:50-52 — closing the 23-Aug FAIL-1/FAIL-2 criticals; regenreg's writeLines carries [[nodiscard]] proven by mutation (comment at regenreg.cpp:30-33).
* The fixes are pinned by real registered tests: core/tests/storeflush_smoke.cpp and core/tests/shortwrite_probe.cpp driven by tools/test_shortwrite.sh, wired in core/CMakeLists.txt:419-445 (commits 054752a, f9a15e0, f91f32f, all post-23-Aug).
* saveOrWarn now checks byte count AND flush before returning true (app/main.cpp:2634-2645), and its failure branches — dead in the 23-Aug tree — are exercised by an in-app selftest (TEST-2, app/main.cpp:15886-15921).
* writeAllOrWarn/streamWriteOk exist for streamed exports (app/main.cpp:2726-2769); the FAIL-9 fix verifies removal of the truncated remnant and names a surviving stub (app/main.cpp:2759-2765); four export paths route through it (26466, 28794, 29985).
* STATIC-2 closed: the one call site that discarded GlossaryStore::save's bool now warns via warnWriteFail (app/main.cpp:14494-14503 and 14780-14789).
* FAIL-13 closed: the OCR model download stages beside the working model, verifies every byte, and swaps only on proof (app/main.cpp:396-416).
* Folio scan cache writes verify byte count, flush, and delete stubs so a truncated JPEG never scores 'already present' (app/main.cpp:9449-9465 and 9553-9570), and the batch report counts write failures separately.
* Catalog approve/reject file moves check fs::rename error codes with explicit rollback and honest rollback-failure messages (core/src/catalog_actions.cpp:202-311); the roster saves staged-beside with atomic rename (catalogRosterSave, app/main.cpp:30568-30600).
* Data-release import stages beside and swaps instead of truncating the working release first (app/main.cpp:25230-25252).

**Findings:**

| id | sev | site | summary |
|---|---|---|---|
| WP-1 | high | `app/main.cpp:30355` | ManuscriptPane::save() discards f.write()'s return, never flushes, clears dirty_ at 30356, and prints 'saved HH:mm' — the exact shape named in both the 22-Aug (FAIL-5) and 23-Aug (FAIL-2) assessments, which the 26-Aug sweep table records as 'CLOSED earlier'. |
| WP-2 | high | `app/main.cpp:27846` | 'Save input work' still returns SILENTLY on open failure, discards the unchecked f.write(), and sets status to 'saved <path>' — the 22-Aug FAIL-16 finding (then at line 25875, marked trivial) surviving verbatim. |
| WP-3 | medium | `app/main.cpp:25299` | The spine pointer write after a release import discards both open and write results, then unconditionally shows 'Release %1 imported — the spine %2 is built and selected'. |
| WP-4 | medium | `app/main.cpp:26439` | saveLinks() writes the translator's alignment-links TSV through a bare QTextStream with no streamWriteOk verdict (void function; only the open is checked) even though writeAllOrWarn exists for exactly this. |
| WP-5 | medium | `app/main.cpp:24602` | importPaths() discards QFile::copy's bool and appends every filename to the report, which is then shown as 'Imported into library/my_materials/'. |
| WP-6 | medium | `app/main.cpp:20304` | The STD-004 RTF export checks open but discards f.write()'s return and never flushes, then renders '<b>Exported</b> …' with counts. |
| WP-7 | medium | `app/main.cpp:24836` | The UTFC legacy-font rescue checks open via warnWriteFail but discards the f.write() of the converted text, then reports 'Rescued to Unicode: … %1 characters'. |
| WP-8 | medium | `app/main.cpp:31973` | generateListHtml() — whose own comment says 'report honestly' — discards f.write()'s return and answers '<h3>ASCII catalog list written</h3>' with full statistics. |
| WP-9 | medium | `app/main.cpp:32226` | The witness-hunt export streams via QTextStream and returns 'wrote %1 missing work(s)' without ever calling streamWriteOk — the exact DATA-4 idiom the sweep fixed at four sibling exports. |
| WP-10 | medium | `core/src/libindex.cpp:272` | Index build discards every write-side sqlite3_step result (files insert/update at 272/281, line and FTS inserts at 327/333), stamping the file row with full mtime/size/nlines even when its line rows failed. |
| WP-11 | low | `app/main.cpp:20180` | The apparatus 'Candidate saved' report (and its twin 'Bibliography candidate saved' at 20500) renders unconditionally after a discarded saveOrWarn verdict. |
| WP-12 | low | `app/main.cpp:3573` | The analysis auto-save discards five f.write() returns, never flushes, appends '· saved <file>' to the status on unverified bytes, and is silent when the open itself fails. |
| WP-13 | low | `app/main.cpp:31780` | The line-slash cleaner discards out.write()'s return and reports success (empty error string) while setting lastFile_ to the CLEANED copy. |
| WP-14 | low | `app/main.cpp:13884` | The illustration-gallery fetch loop discards f.write()'s return and counts the page into paths on the open alone. |
| WP-15 | low | `core/src/progress.cpp:83` | Every write method of the drill-progress store (touchWord:83, reviewWord:111, recordDrill:140, recordSegmentRead:154) discards its sqlite3_step result. |
| WP-16 | low | `app/main.cpp:34641` | The survey-markdown save checks open via warnWriteFail but discards f.write()'s return with no flush, so a short write is fully silent. |
| WP-17 | low | `app/main.cpp:25244` | The release-import swap deletes the live file (QFile::remove(dst), bool discarded) before the rename, then on rename failure claims 'Nothing was imported' — but the previous release file was already destroyed. |
| WP-18 | low | `app/main.cpp:36925` | 'File a Finding' appends to docs/FINDINGS.md with a silent if-open-succeeds and an unchecked write (the diagnostic-report write at 37028 likewise discards its write bool before revealing the file in Finder). |

---
*Generated from the 26 Aug agentic re-measurement (15 agents,
adversarial verify stage). Reproduce: the workflow script is archived
in the session workflows directory; the standing gates are
`tools/verify.sh`, `tools/mutate.py --sweep`,
`tools/constitution_check.py`.*
