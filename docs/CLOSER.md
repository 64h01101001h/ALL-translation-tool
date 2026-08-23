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
| 40 | **⌘K Hunt button sits on the tab strip's baseline.** It is mounted as `tabs.setCornerWidget(huntBtn, Qt::TopRightCorner)`, which Qt draws flush inside the tab strip, so it reads as floating on the divider rather than as a control. Adam 2026-08-22: *"it looks like it is being displayed right on top of a line, and seems off."* Candidate home: the right end of the ribbon band, OUTSIDE the scrolling toolStack, so it stays put when the ribbon scrolls. | OPEN(mine) |
| 41 | **Rebuild the test campaign for the CURRENT app, then Adam walks it personally.** Adam 2026-08-22: *"a steady systematic test of all the features and an application wide audit of all functionality with me personally testing each and every function and stress testing it."* docs/TESTING_CAMPAIGN.md is dated 13 August — 284 lines covering NONE of the last nine days: no author search, no collections window, no Vinaya layer, no Treasury of Lives links, no dossiers, no glossary workbench, no team comments, no catalog workflow. Walking it as written would test an application that no longer exists. Rebuild it pane by pane against what actually ships, with (a) a K/F column Adam fills, (b) an explicit STRESS row per pane — the biggest file, the deepest outline, the longest name, the offline case, the missing-data case — because today's four worst defects were all found in exactly those conditions, and (c) a line per pane naming what CANNOT be verified without him. Sequence: after the bounty findings are closed, before #37's visual direction, so the redesign answers what the walkthrough finds. | WAITING(Adam: walk docs/TESTING_CAMPAIGN.md) |
| 42 | **Data-visualisation vocabulary, application-wide.** Adam 2026-08-22 supplied Taras Bakusevych's "choose the right chart" taxonomy (Relationship / Comparison / Composition / Distribution, each split snapshot vs over-time) from https://uxdesign.cc/20-ideas-for-better-data-visualization-73f7e3c2782d as the basis for how we display data across the app. Most of the chart vocabulary does NOT fit this tool — there is no time series in a canon and no pie chart worth drawing — but four mappings are real and grounded in data already on disk: **Distribution** — the meter census (333,802 verse lines; 7-syllable 61.1%, 5-syl 15.4%, 9-syl 8.5%, 6-syl mgur 6.2%) is a histogram currently rendered as a sentence; gloss counts per entry (median 1, mean 2.3, tail to 169) explain WHY the card crowds and would justify the layout to the reader. **Composition** — what a text is MADE of: how much is HGM-glossed vs unglossed vs reference-only, which is the single most useful thing a translator can know before starting; also the library's 33.7% subject coverage. **Comparison** — one term's renderings across the 75 courses (bsod nams: merit 323, goodness 50, good karma 40), which is the register evidence #31 needs a way to show. **Relationship** — the citation web (citation_web.json), the 1,768 person-text links, and term co-occurrence. Constraint: Qt rich text has no charting; anything drawn must be QPainter or composed from table cells and unicode blocks, and every chart must carry its N and its source like every other surface. | OPEN(Adam 2026-08-22) |
| 43 | **`div` borders have NEVER painted — eleven rules in this file are invisible.** Found by the card-design campaign 2026-08-22 and verified independently by rendering: a `div` with `border-left:6px solid #00FF00` paints **0 pixels**; the same border on a `<td>` paints **318**. Qt rich text supports per-side borders on TABLE CELLS only. Consequently `zoneLabel`'s hairline, the 84000 block's `border-left:2px`, the AI glossary's `border-left:3px`, the corpus segments' `border-left:2px` and **the entire five-tier left-rule channel I shipped for author search on 2026-08-22** have never been seen by anyone. I described that tier system as riding "four independent channels at once"; it rides three. Sweep every `border-*` declaration in app/main.cpp, convert the ones that must paint to single-cell tables, and DELETE the ones that were only ever decorative so the file stops implying rules that do not exist. Related measurement from the same campaign: `scaleCardPx` clamps font-size to `max(7, …)`, so the `font-size:1px` spacer-cell trick used by three of the four design proposals paints **8px**, not 1 — any future design that reaches for it is already broken. | OPEN(found 2026-08-22) |
| 44 | **Author portraits and woodblock etchings — a togglable visual layer.** Adam 2026-08-22: *"I really think we need some visuals here, not just text… togglable so it doesn't take away from serious research."* MEASURED, not speculated: the BDRC-pid → Wikidata (P2477) → Commons image (P18) join — the same join already proven for the Treasury of Lives links — resolves **62 of our 462 indexed people, 34 of them with texts in the local library**. Wikimedia's API returns a machine-readable licence per file, and real hits are Public domain / CC0 / CC BY-SA (a Met Museum Sakya Pandita is CC0). **Recommended path, in order:** (1) Wikidata→Commons for the 62, shipping only PD/CC0/CC-BY(-SA) with the licence and photographer recorded per image exactly as every other layer is; (2) museum open-access programmes (the Met, Rubin) for Himalayan art; (3) BDRC frontispieces, already in the licensing conversation; (4) WRITTEN PERMISSION for Treasury of Lives and Himalayan Art Resources, which hold the best Tibetan portrait collections but licence per-image — that is a letter like the BDRC one, and Adam's to send. **Design constraints:** OFF by default and toggled per Adam's instinct; every image captioned with source + licence, never bare; a portrait is REFERENCE, never evidence about a text; and 13% coverage means absence must read as normal, not as failure. **Do not bulk-download before Adam rules** — fetching and redistributing images is a licensing act, not an engineering one. | OPEN(Adam to rule; mine to build) |
| 45 | **Treasury of Lives biography links reached a 404.** Adam clicked Gyaltsab Je's *biography* 2026-08-22. Ids were never wrong (they match Wikidata P4138 exactly; BDRC confirms P65 → 9095) — the FORMATTER was: Wikidata P1630 still publishes `/biographies/view/$1` with a bare id, while TOL moved to `/view/<name-slug>/<id>`. The slug cannot be computed (Wikidata labels reproduce 2 of 6 known-good slugs), so slugs are now harvested DATA from the Wayback CDX index, held to reproducing six URLs confirmed independently by live search. 116/193 people resolve, incl. 43 of the 47 with texts; the other 4 get TOL's by-name search, labelled as a search. RESIDUAL: 77 people have no archived TOL page and cannot get a direct link until TOL is machine-reachable (their site 403s every automated request). | CLOSED(2026-08-22) |
| 46 | **84000 card repeated itself five ways.** Adam's "bcom ldan 'das" screenshot. Root cause worth remembering: the gloss dedup guard compared a trimmed needle against UNTRIMMED comma-components, so it had never fired for any entry in the layer while the commit that added it reported success. Also fixed: definitions identical for 756 chars rendering six times once elided at 400; one body republished under different openings (caught by a shared 200-char head OR tail — 33.0% of 71,296 definition lines across 26,382 entries are restatements); `ux::snip` truncating mid-word while its comment promised otherwise; and `<person>`/`<term>` merge tags reaching the reader incl. the Sanskrit line. Capped at 4 definitions with the remainder disclosed, since the layer is reference only. Pinned on the layer's worst case (43 definitions). | CLOSED(2026-08-22) |
| 47 | **Walk Bakusevych's "20 ideas for better data visualization" together, one idea at a time**, ruling on each: does it apply here, on which surface, at what cost. Adam 2026-08-22: *"bank this website link and put going through it together piece by piece on the todo list."* Link and constraints banked at docs/research/reference/DATA_VIZ_TAXONOMY.md. IMPORTANT: the article body could NOT be retrieved — Medium 403s every automated request and the Wayback snapshot (20250512161702) is an 11 KB JavaScript shell with no text — so the twenty ideas are deliberately NOT transcribed and must not be reconstructed from the title. The taxonomy chart Adam supplied IS transcribed and already mapped to four grounded candidates (#42). | OPEN(Adam 2026-08-22) |
| 48 | **Total Software Quality Assurance assessment.** Adam 2026-08-22: *"put running a total Software Quality Assurance assessment/audit on the list of todos."* Distinct from the bug bounty (which hunted defects) and the UX audits (which judged surfaces): this judges the ENGINEERING PROCESS and the product's fitness as software. Scope: test strategy and REAL coverage — the bounty already found `outline_smoke` covering a function the app does not call, and that no write-failure path is reachable under test at all; static analysis and warnings at full strictness; memory safety (bounty #10 was a 64-slot overrun reachable from library texts) and sanitizer runs; performance under real library sizes; failure-path behaviour; build reproducibility and release engineering; dependency and licence posture; data integrity and provenance guarantees; accessibility conformance; and documentation accuracy versus actual behaviour. Deliverable: a graded assessment with evidence per finding, a defect-density baseline, and a remediation ledger — not a pass/fail sentence. | OPEN(mine) |

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
- 2026-08-22 · **#45 CLOSED (biography links) / residual noted.** Adam
  clicked Gyaltsab Je's "biography" and got a 404. The ids were never
  wrong — ours match Wikidata's P4138 exactly and BDRC confirms
  P65 → 9095. The FORMATTER was: Wikidata's P1630 still publishes
  ".../biographies/view/$1" with a bare integer, and TOL has moved to
  /view/<name-slug>/<id>. The slug cannot be computed (deriving it
  from Wikidata labels reproduced 2 of 6 known-good slugs), so slugs
  are now DATA — harvested from the Wayback CDX index and held to
  reproducing six URLs confirmed independently by live web search.
  116 of 193 people resolve, including 43 of the 47 who have texts to
  click from; the other 4 get TOL's by-name search, labelled as a
  search. **Residual:** 77 people (4 with texts) have no archived TOL
  page, so no direct biography link exists for them. Not human-gated
  — it needs a reachable TOL index, and their site 403s every
  automated request. Revisit if TOL ever publishes a dump.
- 2026-08-22 · **#46 CLOSED.** The 84000 card was repeating itself
  five different ways (Adam's "bcom ldan 'das" screenshot). The one
  that matters most: the gloss dedup guard compared trimmed needles
  against UNTRIMMED comma-components, so it had never fired for any
  entry in the layer — the dedup committed earlier the same day
  reported success while doing nothing for glosses. Also fixed:
  definitions identical for 756 characters rendering six times once
  elided at 400; the same body republished under different openings
  (now caught by a shared 200-char head OR tail — 33.0% of 71,296
  definition lines across 26,382 entries are restatements); ux::snip
  truncating mid-word while its own comment promised otherwise; and
  84000's "<person>"/"<term>" merge tags reaching the reader,
  including on the Sanskrit line. The layer is reference only, so it
  is capped at 4 definitions with the remainder disclosed and the
  tier named. Pinned on the layer's worst case (43 definitions).
- 2026-08-22 · **#47 OPEN (joint, Adam + me).** Walk Bakusevych's
  "20 ideas for better data visualization" together, one idea at a
  time, ruling on each: does it apply to this tool, on which surface,
  what it costs. Link and constraints banked at
  docs/research/reference/DATA_VIZ_TAXONOMY.md. NOTE: the article body
  could not be retrieved (Medium 403s automation; the Wayback snapshot
  is a JS shell with no text), so the twenty ideas are NOT transcribed
  and must not be reconstructed from memory — Adam reads, we rule
  together. The taxonomy chart he supplied IS transcribed and stands
  on its own. Surfaced in OVERSIGHT (needs Adam in the room).
- 2026-08-22 · **#48 OPEN (mine to run).** Total Software Quality
  Assurance assessment — Adam's directive. Distinct from the bug
  bounty (#that hunted defects) and from the UX audits (which judged
  surfaces): this judges the ENGINEERING PROCESS and the product's
  fitness as software. Scope: test strategy and real coverage (72
  suites — of what? the bounty already found outline_smoke testing a
  function the app does not call); static analysis and compiler
  warnings at full strictness; memory safety (the bounty's #10 was a
  64-slot overrun reachable from library texts); undefined behaviour
  and sanitizer runs; performance and responsiveness under real
  library sizes; error handling and failure paths (the bounty found
  no write-failure path is reachable under test at all); build
  reproducibility and release engineering; dependency and licence
  posture; data integrity and provenance guarantees; accessibility
  conformance; documentation accuracy versus behaviour. Deliverable:
  a graded assessment with evidence per finding, a defect-density
  baseline, and a remediation ledger — not a pass/fail sentence.
- 2026-08-22 · **BOUNTY B7, B9, B11, B12, B13, B14 ALL CLOSED.** The six
  open findings, specced and adversarially verified by a fan-out (one
  agent per finding, one independent verifier per patch), then applied
  and pinned. The verifiers earned their place: they **refuted three of
  the six patches**, and none of the three refutations was cosmetic.
  B14's edit anchor matched ZERO times (the file writes non-ASCII in
  string literals as `\u` escapes; the patch carried literal glyphs), so
  it would have failed or half-applied. B13's headline pin was refuted
  by MUTATION — the verifier reverted only the defect site, left the new
  helpers in place, and watched my pin still pass; it was asserting the
  helper, not the bug. B12's pins called the helper directly and so
  could never have failed when Enter was broken. All three are instances
  of the bounty's own systemic finding #7, which is the strongest
  argument for #48. Two mechanical lessons banked: a "corrected" edit
  that re-emits its own anchor line lets the superseded edit match
  again (this duplicated code twice, in B11 and B14 — the compiler
  caught both), and **ctest reports green against the STALE binary when
  a build fails**, so a suite that passes right after a broken build has
  proved nothing.
