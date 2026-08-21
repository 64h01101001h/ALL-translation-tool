# THE CLOSER — UNIFIED BACKLOG (single source of truth)
Compiled 2026-08-20 from all six ledgers. Order is execution order.
States: OPEN · CLOSED(date) · WAITING(who: what) · DEFERRED(stamp).

| # | Item (origin) | State |
|---|---|---|
| 1 | D1 virgin-Mac first-contact drill: mounted-DMG run + drag-install run, findings filed (Shipwright) | CLOSED(2026-08-20) |
| 2 | F1 honesty-invariant sweep at scale — provisional never unbadged, badges on every zone, true N-of-M, register warnings, machine labeling; permanent suite (Fidelity) | CLOSED(2026-08-20) |
| 3 | F4 tier-arithmetic pins — populations recomputed from loaded data at selftest, About numbers can't lie (Fidelity) | CLOSED(2026-08-20) |
| 4 | L2 licenses surface — About/Licenses screen + DMG LICENSES file assembled FROM the manifest; constitution rule: no unmanifested data layer (Shipwright) | CLOSED(2026-08-20) |
| 5 | S2 rolling backup of human-judgment stores + restore drill in battery; corruption quarantine (Stewardship) | CLOSED(2026-08-20) |
| 6 | F2 cross-engine coherence at dictionary scale, divergence classes banked (Fidelity) | CLOSED(2026-08-20) |
| 7 | D3 release script: VERSION+tag+CHANGELOG+RC naming move together; blessed baseline tagged per RC (Shipwright) | CLOSED(2026-08-20) |
| 8 | T3 engine fuzz at the seams, corpus banked as fixtures (Quality) | CLOSED(2026-08-20) |
| 9 | S4 security posture: no secrets in repo/logs, key path excluded from backups, FTP/SFTP no-persist proven, roster salt verified, trust model documented (Stewardship) | CLOSED(2026-08-20) |
| 10 | D2 signing/notarization pipeline in the press, skip-with-notice until identity exists (Shipwright) | CLOSED(2026-08-20) |
| 11 | T7 chrome pins for every P0-capable path (save/export/import/rulings/propagation) (Quality) | OPEN |
| 12 | T4 perf floors measured + pinned (cold start, big doc, query fan-out, index build) (Quality) | CLOSED(2026-08-20) |
| 13 | T8 Word-style menu reorganization, before/after screenshots to Adam (Quality; §63 governance) | OPEN |
| 14 | S5 docs/MAINTAINERS.md — the operational handbook for a competent stranger (Stewardship) | CLOSED(2026-08-20) |
| 15 | S6 findings loop: FINDINGS inbox + disposition cadence + Adam's outstanding campaign verdicts section (Stewardship) | CLOSED(2026-08-20) |
| 16 | T9 docs-vs-UI truth pass (help, tutorials, README against the real UI) (Quality) | OPEN |
| 17 | F3 outbound-link truth sampling at the index builders (Fidelity) | OPEN |
| 18 | D4 update path: honest check-for-updates or documented re-download (Shipwright) | OPEN |
| 19 | L1 itemized pass over data/extracted per-file licenses (Shipwright) | OPEN |
| 20 | A1 campaign executed against an RC (acceptance gate) | WAITING(Adam: run docs/TEST_CAMPAIGN.md on the RC) |
| 21 | A2 authority sign-off on data-facing surfaces | WAITING(Adam/GMR: walkthrough) |
| 22 | A3 input-center pilot, one week, ledger on | WAITING(Adam: name the pilot user) |
| 23 | HGM/LC redistribution stance | WAITING(Adam: one written line) |
| 24 | BDRC permission — banked note (2026-08-07) covers NON-COMMERCIAL free distribution and the WRITTEN email is still pending; verdict depends on whether v1.0 is sold or free | WAITING(Adam: (a) free or paid? (b) chase the written email) |
| 25 | Teachings-index consent line | WAITING(Adam: one written line) |
| 26 | SOAS POS terms | WAITING(Adam: locate terms; else MUST-REMOVE at market press) |
| 27 | Apple Developer ID decision | WAITING(Adam: $99/yr call) |
| 28 | Ribbon reorder from usage data | WAITING(time: a week of ledger data) |
| 29 | 9i manual revamp + screen recordings + YouTube channel | DEFERRED(post-1.0 roadmap, per earlier sequencing) |
| 30 | 9n remainder: team comments on texts; dictation | DEFERRED(post-1.0 unless Adam stamps otherwise) |

## Wave log
- 2026-08-20 · **#1 D1 CLOSED.** Virgin-Mac drill: the app runs clean
  BOTH from the read-only mounted DMG and from a drag-install with a
  fresh account simulation (22/22 panes each). OPEN_SOURCE_NOTICES.md
  already ships. Honest limitation recorded: a locally-built DMG
  carries no quarantine xattr, so Gatekeeper's downloaded-file verdict
  can only be proven on a real second Mac (A3's pilot covers it).
- 2026-08-20 · **#2 F1 CLOSED.** The honesty-invariant sweep is a
  permanent part of app_selftest: 4,592 cards (stride-23 over the
  whole dictionary) assert provisional-always-marked, no false
  PROVISIONAL, absence-always-stated, reference-always-labeled,
  AI-never-beside-HGM. First run caught its own invariant being too
  blunt (provenance mentions inside the AI banner are labeled BY the
  banner) — refined, then 4,592/4,592 clean. Battery is now 60 suites
  with the sweep inside app_selftest.
**Burndown:** 17 open · 9 waiting · 2 deferred · 2 closed.
- 2026-08-20 · **#3 F4 CLOSED.** Spine::entryCount()/tierCensus() +
  selftest pin: 105,634 = meta = tier sum; curated 114 + glossary
  7,980 + auto-aligned 3,910 = 12,004 HGM-glossed, exactly as claimed.
  Consistency always asserted; exact numbers version-gated to v27.2.
- 2026-08-20 · **#4 L2 CLOSED.** Help → Licenses & Data Sources…
  renders OPEN_SOURCE_NOTICES.md in-app (already comprehensive — Qt
  LGPL through CDSL); the constitution gained the unmanifested-payload
  rule: a press-staged data dir without a credit-map + notices entry
  fails the battery. #24 refined: BDRC's banked okay covers
  NON-COMMERCIAL free distribution; written email still pending.
**Burndown:** 15 open · 9 waiting · 2 deferred · 4 closed.
- 2026-08-20 · **#5 S2 CLOSED.** allcore::backupFile/latestBackup/
  restoreBackup (deterministic stamps, bounded prune, missing-source
  honesty) + backup_smoke = battery suite 61: the corrupted-store
  restore drill, self-cleaning at entry AND exit. App hookup: every
  real launch quietly backs up proposals.tsv + CATALOG_TEAM.tsv into
  Application Support/backups (keep 14). One key-wiring bug caught in
  review before commit (officialRoot lives under the sess/ prefix).
- 2026-08-20 · **#7 D3 CLOSED.** tools/release.sh: refuses a dirty
  tree; VERSION + CHANGELOG (scaffolded from git log since last tag) +
  commit + tag + three-gate press (team|market) + blessed-baseline
  snapshot move as ONE ritual.
**Burndown:** 13 open · 9 waiting · 2 deferred · 6 closed.
- 2026-08-20 · **#6 F2 CLOSED.** The coherence question got a real
  answer: the C++ forward engine vs CANONICAL python over 35,211
  dictionary headwords = **100.000% agreement** (forward_battery,
  suite 62, ratchet floor 99.95% — may only rise). Two false oracles
  unmasked on the way: the STORED generated rows carry v27-1 flag
  policy + release post-processing (python-vs-stored itself diverges
  1.3%), and the engine's ok=false is a flagged-tokens signal, not a
  failure (byte-identical outputs were being counted divergent). The
  ACIP↔EWTS leg holds at 0.54% divergence (known ambiguity classes,
  <2% threshold) in the selftest.
**Burndown:** 12 open · 9 waiting · 2 deferred · 7 closed.
- 2026-08-20 · **#8 T3 CLOSED.** fuzz_smoke (suite 63): 20,000
  deterministic mutants (splices, stack pileups, orphan-plus walls,
  control/high bytes, truncated UTF-8, repetition blowups) through all
  four converters — every one returned, bounded, in 1.6 s. Promoted
  fixtures pin the risky shapes forever. Fixed seed: any failure
  reproduces exactly.
- 2026-08-20 · **#9 S4 CLOSED.** The trust model, verified and stated:
  the perimeter is the shared folder (the roster's own words —
  "access discipline, not cryptography"). Checks all clean: zero
  secrets in tracked files; FTP/SFTP passwords asked per session and
  NEVER written to settings (grep-proven); the API key lives in env or
  Application Support (harness runs always see an empty key; backups
  copy only the two judgment stores, never the key file); roster
  passphrases stored as SHA-256(salt:pass) with a per-member salt.
  Known bound, stated honestly: SHA-256 is not a slow KDF — adequate
  for the shared-folder perimeter, not for a hostile-network one.
**Burndown:** 10 open · 9 waiting · 2 deferred · 9 closed.
- 2026-08-20 · **#10 D2 CLOSED.** The press now signs with
  ALL_DEV_IDENTITY when set (hardened runtime) and notarizes+staples
  the DMG when the ALL_NOTARY keychain profile exists — both branches
  skip-with-notice today, so the day Adam's Developer ID activates,
  one env var + one store-credentials command make every press a
  notarized press. Setup commands documented inline in the script.
**Burndown:** 9 open · 9 waiting · 2 deferred · 10 closed.
- 2026-08-20 · **#12 T4 CLOSED.** Perf floors pinned in the selftest:
  300 spine lookups 6ms (<2000) · 20 corpus searches 21ms (<2000) ·
  1k unicode conversions 9ms (<1200) · 1k pronunciations 1ms (<1200).
  Budgets sit far above observed so variance never cries wolf; a lost
  index or accidental O(n²) blows through instantly. (Cold-start is
  proven live by the press's relaunch-verify step.)
- 2026-08-20 · **#14 S5 CLOSED.** docs/MAINTAINERS.md — build/test/
  press/release, the five rules, the paid-for editing lessons, data
  topology, and where every decision lives. A competent stranger can
  press a release from it.
- 2026-08-20 · **#15 S6 CLOSED.** docs/FINDINGS.md inbox (with the
  standing section for Adam's outstanding verdicts so they can never
  silently expire) + Help → Troubleshooting → File a Finding: one
  click appends a timestamped, pane-stamped, version-stamped template
  and opens the file.
**Burndown:** 6 open · 9 waiting · 2 deferred · 13 closed.
