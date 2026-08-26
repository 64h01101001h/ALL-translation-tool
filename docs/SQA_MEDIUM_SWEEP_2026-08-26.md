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
| FAIL-3 (dialog seam, zero consumers) | OPEN |
| FAIL-8 (rollback asserts unverified state) | CLOSED — both META-guard undos verify via restore() |
| FAIL-9 (99 discarded I/O bools) | TRIAGED — the two with teeth fixed (overwrite-became-skip; surviving truncated stub); rest recorded benign |
| FAIL-10 (no I/O rule in the process gate) | TRIAGED-SUBSUMED — the mechanical rule now exists: Qt6 open() is nodiscard and the STATIC-1 wall reds every ignored one (it convicted this sweep author twice tonight); composed writes route through saveOrWarn/streamWriteOk with their own drills. A broader ban on bare write() would outlaw legitimate uses to catch none that these two nets miss. |
| FAIL-11 (short-write drill under-pins) | OPEN — its false-lesson history is why FAIL-9 was not blanket-"fixed" |
| FAIL-12 (read-only root collapses drills) | OPEN |
| STATIC-2 ([[nodiscard]] on saves) | CLOSED — convicted the audit's site AND one it missed |

## Network handling
| FAIL-4 / PERF-10 (no timeout, no cancel) | CLOSED — every manager born with 20 s; PERF-4's pump gives mid-scan cancel |
| FAIL-6 (unchecked JSON) | CLOSED — three network parses distinguish "empty" from "not JSON" |
| FAIL-7 (dead stream renders as finished) | CLOSED — message_stop tracked; fragments say TRUNCATED, never QC'd/saved |

## Test coverage and pin quality
| TEST-2 (saveOrWarn branches dead) | CLOSED — real drill, finding's own mutation killed |
| TEST-5 (44% mutation detection) | ONGOING — every sweep fix lands mutation-verified; sweep file now 10 entries |
| TEST-6 (0.60 floor passed a 21-pt regression) | CLOSED — floors ratchet at 85%/55% with the rule written beside them |
| TEST-7 (SettingsDialog 0%, ScanPane 22%) | OPEN |
| TEST-9 (ctest over a failed build) | CLOSED earlier (verify.sh build-exit gate) |
| TEST-10 (outline jump targets unasserted) | CLOSED — pinned to the announcing ordinal |
| TEST-11 (restore path unreachable) | CLOSED — Help menu door + safety-copy rule drilled |
| TEST-12 (flapping wall-clock pins) | CLOSED — micro-ceilings at regression scale; sanitized runs derate ×4 |

## Memory and UB
| MEM-3 (NULL → SEGV in progress readers) | CLOSED — reproduced at exit 139, then guarded |
| MEM-4 (unchecked indexing in botok) | CLOSED — .at() = the canonical IndexError, 26 sites |
| MEM-5 (sanitizer gate never triggered) | CLOSED — first FULL CLEAN run in the project's history: all 86 suites green under ASan+UBSan (2026-08-26, 55.7 s); the only maiden-run failure was a wall-clock ceiling tripped by instrumentation tax, now derated ×4 under ALL_SANITIZED; the release ritual runs the sanitized battery before any tag |
| MEM-6 (widget read off-thread) | CLOSED — the worker uses its captured value |

## Static analysis
| STATIC-1 (-Wall wall broken) | CLOSED — clamp wired as a warning; verify.sh reds on own-code warnings |
| STATIC-4 (no analyzer, 2,432 sites) | OPEN — standing clang-tidy adoption is its own project |
| STATIC-5 (ExportPane can't-fail assertion) | CLOSED — replacement went red twice on two REAL defects |
| STATIC-6 (loop that always breaks) | CLOSED — contract pinned first, then the honest one-liner |

## Performance
| PERF-2 (open blocks 4–9 s; guards fixture-dead) | GUARDS CLOSED — both self-provision and now run (1,053 ms / 2,682 ms measured); the freeze half stays open honestly |
| PERF-3 (limit never reached the scan) | CLOSED — 1,838 ms → 0.56 ms on the finding's own case |
| PERF-4 (GUI-thread search, dead Stop) | CLOSED — sqlite progress pump; Stop aborts mid-scan |
| PERF-5 (590 MB build in a render path) | CLOSED (acknowledgement) — wait cursor over exactly the cold build |
| PERF-6 (refold mechanics) | CLOSED — WAL + post-refold optimize; wording sliver awaits Adam |
| PERF-7/8/9/11/12/13/14/15 | OPEN |

## Build and release
| BUILD-8 (codesign failure rolls on) | CLOSED |
| BUILD-9 (absent baseline = pass) | CLOSED — explicit first-run escape only |
| BUILD-10 (C2 blesses disarmed gates) | CLOSED — liveness check; both disarm shapes mutation-proven; first draft convicted by a log filename |
| BUILD-12 (spine hash unlinked) | OPEN |
| BUILD-13 (press is a deployment) | OPEN |
| BUILD-15 (com.appelegant, version 1.0) | CLOSED — org.asianlegacylibrary + VERSION-file truth |
| BUILD-16 (DMG never verified) | CLOSED — hdiutil verify every press |
| BUILD-21 (no lockfile, no check) | CLOSED — roll-call: missing fails, drift notes; MANIFEST stays the version authority |
| BUILD-22 (two rc.1 DMGs, 2 GB, stale tag) | HYGIENE CLOSED — superseded images banked with a pointer; the tag is the release ritual's act |
| BUILD-23 (LGPL text absent) | CLOSED — canonical texts banked and shipped; zero LICENCE TEXT MISSING |
| BUILD-24 (manifest dodges its gate) | CLOSED — written before the gate, with rows |
| BUILD-3 (unsigned DMG guidance) | OPEN — blocked on the Apple Developer ID (Adam/ALL) |

## Data and documentation
| DATA-3 (nothing prevents the next untiered gloss) | OPEN — mechanical prevention is design work |
| DATA-4 (8,985 "texts" incl. 1,399 companions) | CLOSED — 7,586 truth, pinned |
| DATA-5 ("shown" vs rendered) | CLOSED — "found", old wording banned by pin |
| DATA-6 (771 hidden, no remainder) | CLOSED — remainder line |
| DATA-7 (84000 dedup under-reports) | OPEN |
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
