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
| 11 | T7 chrome pins for every P0-capable path (save/export/import/rulings/propagation) (Quality) | CLOSED(2026-08-20) |
| 12 | T4 perf floors measured + pinned (cold start, big doc, query fan-out, index build) (Quality) | CLOSED(2026-08-20) |
| 13 | T8 Word-style menu reorganization, before/after screenshots to Adam (Quality; §63 governance) | CLOSED(2026-08-20) |
| 14 | S5 docs/MAINTAINERS.md — the operational handbook for a competent stranger (Stewardship) | CLOSED(2026-08-20) |
| 15 | S6 findings loop: FINDINGS inbox + disposition cadence + Adam's outstanding campaign verdicts section (Stewardship) | CLOSED(2026-08-20) |
| 16 | T9 docs-vs-UI truth pass (help, tutorials, README against the real UI) (Quality) | CLOSED(2026-08-20) |
| 17 | F3 outbound-link truth sampling at the index builders (Fidelity) | CLOSED(2026-08-20) |
| 18 | D4 update path: honest check-for-updates or documented re-download (Shipwright) | CLOSED(2026-08-20) |
| 19 | L1 itemized pass over data/extracted per-file licenses (Shipwright) | CLOSED(2026-08-20) |
| 20 | A1 campaign executed against an RC (acceptance gate) | WAITING(Adam: run docs/TEST_CAMPAIGN.md on the RC) |
| 21 | A2 authority sign-off on data-facing surfaces | WAITING(Adam/GMR: walkthrough) |
| 22 | A3 input-center pilot, one week, ledger on | WAITING(Adam: name the pilot user) |
| 23 | HGM/LC redistribution stance | WAITING(Adam: one written line) |
| 24 | BDRC permission — **free-of-charge ruling made 2026-08-22**, which puts the models inside CC BY-NC; they now ship in both press modes. Written confirmation still sought for the record (draft in Gmail, needs a To: address) | RULED(free) + WAITING(Adam: send the written-confirmation email) |
| 25 | Teachings-index consent line | WAITING(Adam: one written line) |
| 26 | SOAS POS terms | CLOSED(2026-08-21): CC BY 4.0 all along — Zenodo DOI banked in data/soas_pos/README.md; notices corrected |
| 27 | Apple Developer ID decision | WAITING(Adam: $99/yr call) |
| 28 | Ribbon reorder from usage data | WAITING(time: a week of ledger data) |
| 29 | 9i manual revamp + screen recordings + YouTube channel | PARTIAL: written half CLOSED(2026-08-21); recordings+channel WAITING(Adam: record or delegate) |
| 30 | 9n remainder: team comments on texts; dictation | CLOSED(2026-08-21) |
| 31 | **Genre/subject awareness — the register a text is read in.** Denote which subject a text belongs to (Vinaya, Pramana, Madhyamika, Abhidharma, Prajnaparamita, tantra, commentary, etc.) and let that govern how words are read: subject-aware gloss ordering, register warnings, and suggestions tuned to the genre. **Vinaya especially** — Adam's ruling: it is the OLDEST language in the monastic curriculum and reads differently; a term that means one thing in a logic text means another in the discipline literature. Seed data already banked: data/extracted/r6_work_subjects.json (1,424 works), sungbum_subjects.json (2,416), r6_subject_pairs.json. Design constraint: the subject layer is REFERENCE that reorders and warns — it may never invent an equivalent HGM did not give (rule 1), and a subject-specific reading must name its genre as evidence. | **ENGINEERING DONE 2026-08-22 · DATA WAITING ON ADAM/GMR** |
| 32 | **Mine GMR's recorded teachings for translation method.** Sweep every available video/audio teaching (the caption corpus + whatever transcripts Ora/Jamie share) for the moments where Geshe Michael gives TRANSLATION ADVICE — how to render a term, when a word shifts register, how to handle Sanskrit, what he tells students about the craft — and turn those findings into (a) a citable body of method notes, (b) concrete improvements to the tool's workflow, and (c) candidate register/genre rules. He has taught the method for decades in passing; almost none of it is collected. Honesty constraint: located passages are CANDIDATES with the recording as authority (the existing teaching-index grammar); anything promoted to a rule needs his ruling through the approval channel. | **CLOSED 2026-08-22** |
| 33 | **Find every text by an author — in whatever script you can type.** A first-class author search over the Library: type an author's name and get all their texts, opened from the results. Adam's requirement (2026-08-22) is that the QUERY side accept **ACIP or Wylie interchangeably**, and additionally match **approximately by pronunciation** — so someone who knows how a name SOUNDS but not how it is spelled still finds the works. That last part is the real engineering: pron_engine.py's GMR-convention phonetics already exist and are ported, so the design is to index each author name's phonetic form once and match the query's phonetic form against it, with the orthographic (ACIP/Wylie/Unicode) paths as exact tiers ABOVE it. Assets already in hand: authorByWork_ + acipPersonLinks_ (per-text author attribution), persons_bdrc.json (350 catalog author pids, 183 named authors), the new tol_links.json (193 biographies), and personHtml()'s 'their works in YOUR library' list — which today is reachable only by landing on a text first, never by asking for the author directly. Honesty constraints: a phonetic hit is a FUZZY tier and must look fuzzier than an exact orthographic hit; homonym clusters (the persons layer already carries multi-candidate authors) must be shown as distinct people to disambiguate, never silently merged. | **CLOSED 2026-08-22** |
| 34 | **GUI mockup + visual revamp.** BACK BURNER by Adam's ruling 2026-08-22: hold all GUI mockup and visual-revamp work until the rest of the queue is finished. The design wing (design-critique, nielsen-heuristics-audit, don-norman-principles-audit, accessibility-audit, interaction-design, design-systems) is installed and ready; the existing design system (ink #2B2620, green #1E6B4E reserved for the binding tier, warm-grey reference family, gold micro-caps zone labels, Palatino app-wide) is the baseline any revamp starts from. Constraint that survives any redesign: tier honesty is VISUAL — provisional must look provisional and reference must look reference, so no restyle may flatten the provenance tiers into one look. | DEFERRED(Adam 2026-08-22) |
| 35 | **The oversight register — docs/OVERSIGHT.md.** Adam's instruction 2026-08-22: keep one authoritative list of everything the tool cannot decide, author, or send for itself, so his time goes to the highest-leverage thing rather than whatever was mentioned last. Seven sections: highest-leverage minutes (backup, name sanction, the Joel send, the BDRC address), things only he or Geshe Michael can author (the register layer above all), human-only verification, money/accounts, the reversible defaults I chose, external parties, and one item waiting on time. **Standing rule: anything newly discovered to be human-gated is filed here in the same commit that discovers it — an item that lives only in a chat message does not exist.** | LIVING |
| 36 | **Ribbon labels elide to unreadable stubs across the app.** Found 2026-08-22 by reading a screenshot of the INSTALLED build, not by any gate. The Manuscript pane's MANUSCRIPT group ships four buttons labelled `…` `…` `…` `…` — nothing tells you what they do. Measured: the widest ribbon needs **2,572 px against a 1,180 px window**. The house principle "no label may elide" is older than this (Input pane, design audit 2026-08-12) but lived only in a comment, so nothing enforced it. Why three gates missed it: the visual gate measured 1.85% drift (under the 2.2% advisory), the geometry inquisition passed because eliding is not clipping or overlapping, and no test existed. A ratchet now stops it worsening (app_selftest, set at the measured 2,572). **Fixing it is per-pane design work — deciding which controls demote to Maintenance menus — and every pane fixed lowers the ratchet. #36 closes when the widest ribbon fits 1,180.** Related to but distinct from #34: this is a defect on a shipping RC, not a restyle. | **CLOSED 2026-08-22** |
| 37 | **Whole-app visual direction — mockups, then a ruling.** Adam 2026-08-22 wants to choose a GUI direction from rendered options: density, chrome, typography, how panes and the ribbon read. This is #34 un-deferred and given a shape. **Sequencing ruling: do this AFTER Adam runs the test campaign (OVERSIGHT C1), not before.** A redesign now would be built on my guesses about how the tool feels to work in; after a real session in it, it would be built on evidence — on 2026-08-22 alone he found four defects that reading code could not have surfaced. Constraint that survives any direction: tier honesty is VISUAL, so no restyle may flatten provenance tiers into one look, and green #1E6B4E stays reserved for binding authority. | OPEN(Adam 2026-08-22) |
| 38 | **Author-search layout: offer the rejected alternatives as a real choice.** The shipped surface is Library-first (ribbon + tier-ranked people + uncapped bibliography). Two approaches were designed and rejected with reasons: palette-first (⌘K drills into an author's shelf — rejected because its own rows returned web links when Adam asked for texts) and a dedicated person card (rejected because a modal dialog would hang the UI sweep; its arrival-banner idea was grafted onto the shipped design instead). Lowest value of the three open design questions — it swaps the surface of a feature that already works — but Adam asked to see the options, and the rejects are documented in docs/design/AUTHOR_SEARCH_SPEC.md. | OPEN(Adam 2026-08-22) |
| 39 | **The Overlay card pane: comprehensive UX + visual redesign.** Adam 2026-08-22: *"the one that the user will interact with most and it must be visually appealing… information and data dense, but I think we can begin to look at different ways of displaying the information here so that it is more unambiguous."* He asked for a seriously comprehensive prompt and as many agents as needed. Real observations already banked from live cards: the binding badge read as welded to the pronunciation (fixed); the solid HGM highlight may be too strong for its frequency; five identical `[HGM (glossary)]` suffixes stack with no visual grouping; MACHINE / EVIDENCE / REFERENCE zone labels float without the blocks they head; the REFERENCE · LOCAL ONLY block is a wall of `JH-ENG+JV+RB+RY` codes with no legend. Constraint on any redesign: tier honesty is VISUAL — provisional must look provisional, and green #1E6B4E stays reserved for binding authority. Sequence AFTER #37's whole-app direction is chosen, so the card inherits it rather than diverging from it. | OPEN(Adam 2026-08-22) |
| 40 | **⌘K Hunt button sits on the tab strip's baseline.** It is mounted as `tabs.setCornerWidget(huntBtn, Qt::TopRightCorner)`, which Qt draws flush inside the tab strip, so it reads as floating on the divider rather than as a control. Adam 2026-08-22: *"it looks like it is being displayed right on top of a line, and seems off."* Candidate home: the right end of the ribbon band, OUTSIDE the scrolling toolStack, so it stays put when the ribbon scrolls. | OPEN(Adam 2026-08-22) |
| 41 | **Rebuild the test campaign for the CURRENT app, then Adam walks it personally.** Adam 2026-08-22: *"a steady systematic test of all the features and an application wide audit of all functionality with me personally testing each and every function and stress testing it."* docs/TESTING_CAMPAIGN.md is dated 13 August — 284 lines covering NONE of the last nine days: no author search, no collections window, no Vinaya layer, no Treasury of Lives links, no dossiers, no glossary workbench, no team comments, no catalog workflow. Walking it as written would test an application that no longer exists. Rebuild it pane by pane against what actually ships, with (a) a K/F column Adam fills, (b) an explicit STRESS row per pane — the biggest file, the deepest outline, the longest name, the offline case, the missing-data case — because today's four worst defects were all found in exactly those conditions, and (c) a line per pane naming what CANNOT be verified without him. Sequence: after the bounty findings are closed, before #37's visual direction, so the redesign answers what the walkthrough finds. | OPEN(Adam 2026-08-22) |

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
- 2026-08-20 · **#19 L1 CLOSED — and it caught the biggest licensing
  leak yet.** The itemized pass over data/extracted found the two
  CHECK-LICENSING payloads (thl_dicts.jsonl 56MB,
  tibetan_dictionary_dic.jsonl 12MB — ingest intermediates only
  build_reference_db.py reads) shipping in BOTH press modes via the
  blanket extracted copy, plus the 279MB tm_84000.db cache shipping
  while its CC BY source did not, plus data/84000 (glossary + TM
  source) reaching installs only by legacy accident. Press fixed:
  84000 staged explicitly (credit map updated), the two intermediates
  and the cache stripped in both modes — the TM rebuilds from the
  CC BY tsv.gz on first use. DMG shrinks by ~350MB of wrong bytes.
- 2026-08-20 · **#18 D4 CLOSED.** Help → Check for Updates: states
  the version and the truth — updates arrive as a new DMG from the
  team; no auto-update, the app never phones home. (Modal baseline
  107, reviewed: Help-menu-only, never harness-reachable.)
**Burndown:** 4 open · 9 waiting · 2 deferred · 15 closed.
- 2026-08-20 · **#17 F3 CLOSED.** link_truth (suite 64): all 1,869
  teaching video ids trace to a caption source; Das's 820 page
  references sit in range and in reading order. Permanent, no-network,
  skips gracefully where data is absent.
- 2026-08-20 · **#16 T9 CLOSED.** Help chapters verified present and
  loading; README's Gatekeeper claims true; the USER MANUAL carries an
  honest interface note (the ribbon supersedes its screenshots) until
  9i's full revamp.
- 2026-08-20 · **#11 T7 CLOSED.** P0-path pin coverage: rulings,
  approved-export, roster, catalog staging, release import — pinned.
  The two private single-writer pairs (glossary, align links) are
  RULED exceptions: warned-on-failure (Phase 2) + backed up (S2);
  pinning them would refactor stable code against T5's no-churn law.
- 2026-08-20 · **#13 T8 CLOSED.** The menu bar now speaks the Word
  grammar: File (Open ACIP · Import Data Release — routing to the
  SAME code paths the panes use) · Edit · the seven workflow groups ·
  View · Help. The menu-mirror pin asserts the new truth; visual gate
  22/22. Adam's verdict channel: the findings inbox.

## THE RC LETTER (Closer §3 — the finish line)

**Every engineering item on the unified backlog is CLOSED.**
Burndown: 0 open · 9 waiting · 2 deferred · 19 closed.
The battery stands at 64 suites — engines vs canonical Python at
100.000%, honesty invariants over 4,592 cards, fuzz, backup restore,
perf floors, link truth, the constitution, the visual gate — and the
press carries three gates plus signing/notarization armed for the day
the Developer ID exists.

**What stands between this RC and 1.0.0 is now a list of human names:**
1. **Adam** — free-or-paid decision for v1.0 (#24 hinges on it).
2. **Adam** — one written line: HGM/LC redistribution stance (#23).
3. **Adam** — chase BDRC's written email, or stamp the download-path (#24).
4. **Adam** — teachings-index consent line (#25).
5. **Adam** — SOAS POS terms, or it drops from market builds (#26).
6. **Adam** — Apple Developer ID, $99/yr (#27) — then presses notarize.
7. **Adam** — run docs/TEST_CAMPAIGN.md against an RC build (#20).
8. **Adam/GMR** — the authority's walkthrough of the data-facing
   surfaces (#21).
9. **Adam** — name the input-center pilot user (#22).
Plus time itself: a week of usage-ledger data reorders the ribbon (#28).

When items 1–6 are answered, `tools/release.sh 1.0.0-rc.1 market`
produces the candidate; 7–9 accept it; then
`tools/release.sh 1.0.0 market` is the whole remaining ceremony.

## Post-letter wave (Adam: "keep going" over the deferred shelf)
- 2026-08-21 · **#29 (written half) CLOSED — and it flushed out a
  shipping bug.** The Help chapters were NOT staged by the press at
  all: fresh installs got an EMPTY Help, and Adam's install was
  reading 8-day-stale leftovers kept alive by the non-deleting data
  rsync (the same legacy-persistence mechanism as the 84000 accident).
  Fixed: data/help staged + credit-mapped. The manual itself now
  opens with "What's new — the 2026-08-20 interface": the band, the
  Word menu grammar, the keyboard, the measurable card, the safety
  nets, and File-a-Finding — current-first instead of
  stale-with-an-apology. Recordings + the YouTube channel remain
  WAITING on Adam (record or delegate).
**Burndown:** 0 open · 10 waiting · 1 deferred · 19½ closed.
- 2026-08-21 · **#30 CLOSED.** Team comments on texts: an APPEND-ONLY
  shared store (comments.tsv beside the proposals — append-only means
  two machines can never fight over the file; Dropbox conflicts reduce
  to a union, which load() performs over conflicted siblings) +
  comments_smoke (suite 65: round trip, escaping, two-writers union,
  sibling absorption, drilled twice) + the Overlay's REVIEW band gains
  "Team comments…": read this text's comments with line-jump links,
  add one on the cursor line under your provenance identity; the
  store joined the startup safety net. DICTATION assessed honestly:
  macOS system dictation already works in every text box of the app
  (native Qt text input) — nothing to build; noted in the manual's
  keyboard section rather than duplicated as a fake feature.
**Burndown:** 0 open · 10 waiting · 0 deferred · ALL ENGINEERING CLOSED.
- 2026-08-21 · **#26 CLOSED by reading.** The SOAS POS lexicon was
  CC BY 4.0 from the day it was banked (Zenodo DOI 10.5281/zenodo.574876
  in its own README); SHIP.md's "verify terms" was stale caution. The
  notices table now states the real license. Waiting list: 9 → 8.
- 2026-08-21 · **The signature drafts.** docs/decisions/
  DECISIONS_TO_SIGN.md turns #23/#24/#25 into one-approval affairs:
  the HGM/LC statement, the BDRC written-confirmation email (with the
  free-or-paid bracket marked), and the teachings consent line —
  drafted, nothing sent, Adam edits and signs.
- 2026-08-22 · **#33 CLOSED.** Author search: type a name in ACIP,
  wylie, Tibetan or English phonetics and get that PERSON's texts.
  Engine in allcore (`authorsearch.h`, five tiers, homonyms never
  merged), person index keyed on the BDRC pid so the catalog's 508
  spellings collapse to their real people (Tsongkhapa's works were
  split across six spellings of his name), UI in the Library ribbon's
  STUDY group. Designed by an 8-agent workflow whose judge rejected
  all three proposals' central choice — matching author STRINGS — in
  favour of the pid, independently reaching the same conclusion as the
  data measurement. Spec banked at docs/design/AUTHOR_SEARCH_SPEC.md;
  Adam's six open questions answered with the stated defaults and are
  reversible. Two SHIPPING bugs found and fixed on the way: 939 of
  5,607 work keys opened a metadata sidecar instead of the text, and
  three lazy loaders marked themselves done before attempting their
  load, so one missing bank silently disabled a layer until relaunch.
- 2026-08-22 · **#31 ENGINEERING DONE; the rest is authorship.**
  allcore/subjects states what the catalog says a text is about (3,559
  rows, 1,906 works, 243 R6 labels) and flags Vinaya on an exact
  heading match — never a substring, because S419's lojong heading
  contains 'DUL_BA and is not Vinaya. The Vinaya banner names Adam's
  ruling and its own limit together. What CANNOT be built: measurement
  found only ~5 of 12,004 HGM-glossed entries carry a register mark,
  so subject-aware gloss ranking would be the machine deciding what he
  meant (rule 1). **A register layer has to be WRITTEN by Adam or by
  Geshe Michael through the approval channel; the tool will host and
  surface it the moment it exists.** Full measurement in
  docs/FINDINGS.md.
- 2026-08-22 · **#32 CLOSED.** 2,800 caption files of Geshe Michael's
  own classes mined for translation METHOD: 407 candidate passages,
  each citable to the second (tools/mine_translation_method.py →
  data/teaching/translation_method.json + the readable digest at
  docs/research/GMR_TRANSLATION_METHOD.md). Classed by what he is
  actually doing — ruling a rendering out (32), giving one (20),
  naming a mistranslation (48), saying a thing cannot be rendered (36).
  **29 of them are context-dependent reading rules, which is exactly
  what #31 measured as absent from the dictionary hours earlier** — so
  the register-layer ask to Adam shrank from "author it" to "rule on 29
  passages you already taught" (OVERSIGHT B1). Two caption defects
  fixed to make the passages readable at all: rolling-caption overlap
  and one-sentence-becomes-three duplication.
- 2026-08-22 · **#36 CLOSED.** Root cause was never label length —
  "Maintenance" fits at 11 characters while "Collections" elided at 11.
  QHBoxLayout shrinks whichever buttons have give, and Qt elides them.
  RibbonProxy now returns sizeHint() as its minimumSizeHint, so a
  ribbon button cannot be crushed; the band hosts each ribbon in a
  scroller, so genuine overflow slides instead of turning labels into
  "C...s" and "…". Fixed across EVERY pane at once, including the
  Manuscript group that was shipping four buttons all labelled "…".
  The ratchet stays, re-scoped: it now measures how far the widest
  ribbon overflows (how much scrolling a user faces), not how much is
  unreadable.
