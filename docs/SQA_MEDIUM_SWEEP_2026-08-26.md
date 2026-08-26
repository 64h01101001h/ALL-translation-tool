# The medium-severity sweep — running disposition ledger

*Opened 2026-08-26 during the autonomous /loop sweep. One line per
finding from the 2026-08-23 assessment's 60-medium compact index, with
its disposition and the commit that carries the evidence. CLOSED means
fixed with a red-first pin and (where the harness can reach it)
mutation-verified; TRIAGED means examined and deliberately left, with
the reasoning recorded; OPEN means not yet reached.*

## Write-path honesty
| finding | disposition |
|---|---|
| FAIL-2 (9 unverified success strings) | CLOSED earlier (streamWriteOk + selftest pins) |
| FAIL-3 (dialog seam, zero consumers) | CLOSED — first consumer drives the real Mixed Nuts prep save through the stubbed dialog and reads the file back; mutation-verified |
| FAIL-8 (rollback asserts unverified state) | CLOSED — both META-guard undos verify via restore() |
| FAIL-9 (99 discarded I/O bools) | TRIAGED — the two with teeth fixed (overwrite-became-skip; surviving truncated stub); rest recorded benign |
| FAIL-10 (no I/O rule in the process gate) | TRIAGED-SUBSUMED — the mechanical rule now exists: Qt6 open() is nodiscard and the STATIC-1 wall reds every ignored one (it convicted this sweep author twice tonight); composed writes route through saveOrWarn/streamWriteOk with their own drills. A broader ban on bare write() would outlaw legitimate uses to catch none that these two nets miss. |
| FAIL-11 (short-write drill under-pins) | CLOSED PRIOR, verified tonight — the audit's close-only mutant is KILLED by the read-only-handle drill, and every comment now teaches ts.status(), not QFile::error() |
| FAIL-12 (read-only root collapses drills) | CLOSED — a chmod-500 directory driven through the real save path; the app reports, the drill stands |
| STATIC-2 ([[nodiscard]] on saves) | CLOSED — convicted the audit's site AND one it missed |

## Network handling
| FAIL-4 / PERF-10 (no timeout, no cancel) | CLOSED — every manager born with 20 s; PERF-4's pump gives mid-scan cancel |
| FAIL-6 (unchecked JSON) | CLOSED — three network parses distinguish "empty" from "not JSON" |
| FAIL-7 (dead stream renders as finished) | CLOSED — message_stop tracked; fragments say TRUNCATED, never QC'd/saved |

## Test coverage and pin quality
| TEST-2 (saveOrWarn branches dead) | CLOSED — real drill, finding's own mutation killed |
| TEST-5 (44% mutation detection) | ONGOING — every sweep fix lands mutation-verified; sweep file now 10 entries |
| TEST-6 (0.60 floor passed a 21-pt regression) | CLOSED — floors ratchet at 85%/55% with the rule written beside them |
| TEST-7 (SettingsDialog 0%, ScanPane 22%) | CLOSED — both drilled: open-state/coordinate pins and the settings round trip through the real Save button; both mutation-verified |
| TEST-9 (ctest over a failed build) | CLOSED earlier (verify.sh build-exit gate) |
| TEST-10 (outline jump targets unasserted) | CLOSED — pinned to the announcing ordinal |
| TEST-11 (restore path unreachable) | CLOSED — Help menu door + safety-copy rule drilled |
| TEST-12 (flapping wall-clock pins) | CLOSED — micro-ceilings at regression scale; sanitized runs derate ×4 |
| TEST-4 (B13 anti-pattern at bounty-#8) | RECORDED — the audit itself refuted its "exactly one place" premise (three sites, two driving real Qt events); the pattern is tracked by the no-vacuous-pass gate rather than chased pin-by-pin |
| TEST-8 (42% substring-only pins) | RECORDED — honest ceiling on genuinely-weak pins is 27.7% by the audit's own arithmetic; every pin the sweep touched was rebuilt with a number or an absence clause, and mutation entries police the class; a wholesale rewrite of 274 pins is design-scale, not sweep-scale |
| TEST-13 (gauntlet fossilised at seed 1) | CLOSED — ctest runs '--gauntlet auto': seed = Julian day, fresh walk daily, reproducible within the day, printed on every run (auto and explicit) so a corpse names its own reproduction; fuzz_smoke keeps its fixed seed deliberately (survivors promote to fixtures) |
| TEST-14 (seven pins coupled to one machine's 2.2 GB index) | VERIFIED COVERED — every index-touching pin now guards with exists() and skips out loud ('library index absent — skipped'); the diagnostics report says OK/MISSING; clean-checkout suites carry the fixture label with [SKIP] path + docs/FIXTURES.md pointer, proven by fixture_gate_check |

## Memory and UB
| MEM-3 (NULL → SEGV in progress readers) | CLOSED — reproduced at exit 139, then guarded |
| MEM-4 (unchecked indexing in botok) | CLOSED — .at() = the canonical IndexError, 26 sites |
| MEM-5 (sanitizer gate never triggered) | CLOSED — first FULL CLEAN run in the project's history: all 86 suites green under ASan+UBSan (2026-08-26, 55.7 s); the only maiden-run failure was a wall-clock ceiling tripped by instrumentation tax, now derated ×4 under ALL_SANITIZED; the release ritual runs the sanitized battery before any tag |
| MEM-6 (widget read off-thread) | CLOSED — the worker uses its captured value |
| MEM-7 (unbounded recursion in the Gofer parser) | CLOSED — reproduced first: 300k parens from the search box = exit 139, exactly as filed; parse depth now capped at 200 with a friendly 'simplify the grouping' refusal; ordinary nesting pinned still-parsing; mutation-verified (guard off → the pin crashes red) |
| MEM-8 (two container reads, bounds emergent) | NEEDS THE DIMENSION REPORT — the finding's only public text never names the sites, and the one-liners row maps other items; recorded so nobody closes it by guessing |
| MEM-9 (two Qt lifetime shapes outliving their pane) | CLOSED — both shapes: the context-less 0 ms singleShot now carries idBox (the very object it reads) as context, and the connect(qApp, aboutToQuit) lambda carries the pane as receiver, so Qt drops each when its object dies |

## Static analysis
| STATIC-1 (-Wall wall broken) | CLOSED — clamp wired as a warning; verify.sh reds on own-code warnings |
| STATIC-4 (no analyzer, 2,432 sites) | GROUPED WITH CI (P1, Adam) — an analyzer without an enforcement trigger is another MEM-5; it should arrive WITH the CI stand-up, armed |
| STATIC-5 (ExportPane can't-fail assertion) | CLOSED — replacement went red twice on two REAL defects |
| STATIC-6 (loop that always breaks) | CLOSED — contract pinned first, then the honest one-liner |
| STATIC-8 (ruff findings in the gate scripts) | CLOSED — 207 raw findings over the eleven registered gates; five correctness-class fixed (dead cutoff/ctx, unused import, semicolon, shadowed 'l'), six execute bits set, sbom's git failure now says UNRESOLVED; style classes deliberately left (gate diffs are audit trail); all 14 check=-less subprocess sites audited — none drops an exit code |
| STATIC-9 (numeric parse silently substituting 0) | CLOSED — a contraction row whose syllable counts fail to parse is skipped whole (rule 3), never half-loaded with zeros |
| STATIC-10 (delete[] of uninitialized pointer in vendored dmp) | CLOSED — rd initialised to NULL; unreachable per the audit, but shipped code does not get to carry garbage into delete[] |
| STATIC-11 (third-party includes not SYSTEM) | CLOSED — vendored SQLite include marked SYSTEM, dmp TU compiles -w; and the wall's own filter had a spelling hole ('third_party' but not 'thirdparty') that would have blamed upstream warnings on us — both spellings excluded, path-anchored |

## Performance
| PERF-2 (open blocks 4–9 s; guards fixture-dead) | GUARDS CLOSED — both self-provision and now run (1,053 ms / 2,682 ms measured); the freeze half stays open honestly |
| PERF-3 (limit never reached the scan) | CLOSED — 1,838 ms → 0.56 ms on the finding's own case |
| PERF-4 (GUI-thread search, dead Stop) | CLOSED — sqlite progress pump; Stop aborts mid-scan |
| PERF-5 (590 MB build in a render path) | CLOSED (acknowledgement) — wait cursor over exactly the cold build |
| PERF-6 (refold mechanics) | CLOSED — WAL + post-refold optimize; wording sliver awaits Adam |
| PERF-7 | CLOSED PRIOR — the R7 skip-notice reports real scanned/skipped counts |
| PERF-8 | CLOSED — 'instantly' became the measured claim |
| PERF-9 | CLOSED — first memory assertion: peak footprint at selftest end, 0.85 GB vs 5.5 GB ceiling, inverse-mutation proven |
| PERF-11 | ABSORBED by TEST-12 — ceilings re-derated with the ratchet rule; the sanitized derate covers instrumentation |
| PERF-12 | CLOSED PRIOR — the selftest types real terms before every Find click |
| PERF-13 (22 eager panes) | OPEN — startup-lazy construction is design-scale, grouped with PERF-2's freeze half |
| PERF-14 | CLOSED PRIOR — the title index and pair bank are built once and cached |
| PERF-15 | CLOSED — the duplicate 1.7 s COUNT(*) reuses st.lines |

## Build and release
| BUILD-8 (codesign failure rolls on) | CLOSED |
| BUILD-9 (absent baseline = pass) | CLOSED — explicit first-run escape only |
| BUILD-10 (C2 blesses disarmed gates) | CLOSED — liveness check; both disarm shapes mutation-proven; first draft convicted by a log filename |
| BUILD-12 (spine hash unlinked) | CLOSED — build_spine stamps source sha256s; validate_release cross-checks when present (arms on next spine build) |
| BUILD-13 (press is a deployment) | CLOSED — ALL_PRESS_VERIFY_ONLY=1 stops after all gates; proven to leave dist/ and /Applications untouched |
| BUILD-15 (com.appelegant, version 1.0) | CLOSED — org.asianlegacylibrary + VERSION-file truth |
| BUILD-16 (DMG never verified) | CLOSED — hdiutil verify every press |
| BUILD-21 (no lockfile, no check) | CLOSED — roll-call: missing fails, drift notes; MANIFEST stays the version authority |
| BUILD-22 (two rc.1 DMGs, 2 GB, stale tag) | HYGIENE CLOSED — superseded images banked with a pointer; the tag is the release ritual's act |
| BUILD-23 (LGPL text absent) | CLOSED — canonical texts banked and shipped; zero LICENCE TEXT MISSING |
| BUILD-24 (manifest dodges its gate) | CLOSED — written before the gate, with rows |
| BUILD-3 (unsigned DMG guidance) | OPEN — blocked on the Apple Developer ID (Adam/ALL) |

## Data and documentation
| DATA-3 (nothing prevents the next untiered gloss) | CLOSED — constitution G2: the hgm_gloss consumer census (32 baselined); growth fails the gate until the new site is reviewed as tiered; mutation-proven |
| DATA-4 (8,985 "texts" incl. 1,399 companions) | CLOSED — 7,586 truth, pinned |
| DATA-5 ("shown" vs rendered) | CLOSED — "found", old wording banned by pin |
| DATA-6 (771 hidden, no remainder) | CLOSED — remainder line |
| DATA-7 (84000 dedup under-reports) | NEEDS THE DIMENSION REPORT — both visible cut-classes disclose counts tonight; the audited 10,450 does not reproduce against current code |
| DATA-8 (2-of-5 under "every") | NOT FOUND — both live surfaces disclose N of M; recorded so nobody re-hunts |
| DATA-9 (raw tier token on one lane) | CLOSED — reader's label everywhere, storage token banned by pin |
| DATA-10 (preset clamped 3, offered 5) | CLOSED — worse than filed: preset 4 had no dims at all |
| DATA-11 (1 of N attested renderings) | CLOSED — ⟨+N more attested⟩, pinned on the sūtra opening |
| DATA-12 (7 greens, 4 meanings) | CLOSED — three named greens, G1 freeze; convicted 2 more strays on first run |
| DATA-13 (55/78 suites, 35,601 in 6 docs) | CLOSED — swept where claims, kept where history |
| DATA-14 (a11y: names/mouse-only/colour-only) | PARTIAL — accessible names shipped earlier (mkBtn); the rest tracked in the a11y audit |

*The incident record of the sweep itself — the stale-mutant object, the
three commit-before-verdict races, the vacuous-pin streak — lives in
the commit messages, deliberately: the sweep's failures are part of its
evidence.*

---

## Capstone — the sweep is complete (2026-08-26)

Every finding in the 60-medium compact index, plus the low-severity
tail the loop absorbed, now carries a disposition. The final census:

- **57 CLOSED** (+2 closed earlier, +1 verified covered by later
  work) — each with a red-first pin and, wherever the harness could
  reach it, a mutation entry proving the pin can fail.
- **2 OPEN by rule, not neglect**: PERF-13 (startup-lazy panes,
  design-scale, grouped with PERF-2) and BUILD-3 (blocked on the
  Apple Developer ID — Adam/ALL).
- **2 NEED THE DIMENSION REPORT** (DATA-7, MEM-8): their public text
  does not locate the defect and the sweep does not close findings by
  guessing.
- **2 RECORDED** (TEST-4, TEST-8): pattern-class findings policed by
  standing gates rather than chased pin-by-pin.
- **1 ONGOING** (TEST-5): mutation detection is a ratchet, not a
  checkbox — the sweep file grew from 6 to 10 entries and every fix
  in this ledger fed it.
- **1 PARTIAL** (DATA-14): tracked in the a11y audit.
- **1 NOT FOUND** (DATA-8): recorded so nobody re-hunts it.
- **1 TRIAGED into CI** (STATIC-4): an analyzer without an
  enforcement trigger is another MEM-5; it arrives WITH CI, armed.

What the sweep leaves behind that did not exist when it started: the
warning wall, the sanitized battery in the release ritual, the
mutation sweep with drift detection and stale-anchor honesty, the
constitution's G1/G2/G3 and C2-liveness rules, the fixture-label skip
machinery, a gauntlet that varies its walk and names its seed, and a
parser that refuses a paren-bomb politely. The failures are in the
commit messages, on purpose.
