# ACI alignment campaign implementation plan

> **For agentic workers:** Use the project's subagent-driven-development and dispatching-parallel-agents workflows, adapted to the existing source-alignment production pipeline. Execute without asking for routine confirmations; the user authorized continuation through the deadline.

**Goal:** Extend carefully reviewed provisional ACI alignment from C05:166 as far as practical before 2026-09-15 07:00 America/Denver.

**Architecture:** Consume the existing spine and canonical generators in an isolated checkout. Two independent analyses feed reconciliation and adversarial review; only accepted three-segment pages enter the course registry and generated evidence bank. Preserve complete source specs and review records outside temporary scratch.

**Tech Stack:** Python generators and fidelity suites; SQLite source corpus; HTML alignment pages; C++/Qt and iOS consumers already in the application.

**Spec:** `docs/ALIGNMENT_LAYER_SPEC.md`, `docs/alignment_briefs/`, `data/alignment/C05_CAMPAIGN.md`, and the user's 2026-09-11 continuation authorization.

## Global constraints

- MATCH, never COMPOSE Geshe Michael Roach's English; no hgm_gloss promotion.
- Seven-depth schema and forward-only Tibetan cursor; d=7 members remain inside d=5 parents.
- Every quote is byte-exact, including source defects; record rather than repair the source.
- PROVISIONAL/TENTATIVE labels remain visible; correctness is reviewed, not claimed from fidelity tests.
- Three segments per landed page. Continue C05 and then C06 onward; C13 stays blocked.
- Independent readers use available Codex agents, with actual provenance recorded. Historical Claude-only role names are superseded by this Codex handover, not impersonated.
- Preserve the main checkout's unrelated work. Do not send, publish, purchase, or use paid external model APIs.
- Deadline: 2026-09-15T13:00:00Z. Preserve unfinished files at cutoff and pause the continuation automation.

### Task 1: Establish a checked and recoverable baseline

**Files:** campaign continuation record; durable baseline and handoff-check reports; `docs/handoff/HANDOFF_LOG.md` in this checkout.

- [x] Verify incoming handoff claims with command evidence, clearly distinguishing checked, corrected, and uncheckable claims.
- [x] Run relevant alignment suites against this checkout before landing any data.
- [x] Check raw course sources and source bank, record starting hashes and counts.
- [x] Record the independent-reader model substitution and the newer authorization to resume.

### Task 2: Produce and review the next three-segment batch

**Files:** durable `campaign-artifacts/C05-166-168/`, then equivalent directories for subsequent batches.

**Interfaces:** Each angle produces one `spec.json`, `body.html`, `errata.json`, and `report.md` per segment. The reconciler consumes both angles and the source independently, returning the same four files. No analyst changes landed project files.

- [x] Snapshot the next three original source rows and context.
- [x] Dispatch independent Tibetan-first and English-first analyses; require actual generator exit 0 and source evidence.
- [x] Reconcile each disagreement with the smallest supportable mapping. Explicitly record omissions and disputed numerals; do not turn uncertainty into an asserted no-exponent fact.
- [x] Have a separate reader challenge semantic correspondence, depth-5 dictionary units, over-capture, d=7 membership, and any proposed errata.
- [x] Correct upheld findings and re-run the generator. Preserve all original proposals and review decisions.

### Task 3: Land and prove the accepted page

**Files:** `data/alignment/pages_c05/c5p56.html` and navigation; `tools/build_alignment_layer.py` course registry; both generated alignment JSON files; `docs/geshe_michael_roach_dictionary.html`; course ledger; accepted specs and review reports.

- [x] Assemble exactly three reconciled segments; run `python3 tools/gen_alignment_page.py` with the assembled spec on stdin and save its body and exit status.
- [x] Wrap with the existing page wrapper; correct new-page producer attribution to the actual producer, preserving the provisional banner.
- [x] Register the page only after semantic review. Rebuild with `python3 tools/build_alignment_layer.py` and `python3 tools/build_dictionary_view.py`.
- [x] Run the existing alignment fidelity tests, generated-doc sync checks, and constitution check; inspect full outputs and reject incomplete or vacuous checks.
- [ ] Verify banked links for the new source rows and review the final rendered page. Append measured counts and decision records to the campaign ledger.
- [x] Commit only this campaign's reviewed files to the campaign branch. Do not modify the main checkout's unrelated files.

### Task 4: Verify consumption and continue through the deadline

**Files:** existing consumer paths, campaign continuation record, generated desktop/iOS assets only where the existing consumers actually use the changed data.

- [ ] Trace the desktop and iOS consumption paths, run appropriate integration checks, and report any unsupported propagation rather than claiming it occurred.
- [ ] Prepare narrow integration of accepted changes for the main project, checking its current state for concurrent edits before acting.
- [ ] Repeat Tasks 2–3 for the next three segments, preserving independent proposals and review. Do not skip ahead merely to increase counts.
- [ ] On every checkpoint, record the last verified segment, any open findings, test results, commits, and exact next action.
- [ ] At the deadline, stop new batches, preserve partial work, report measured coverage and limitations, and pause `aci-alignment-through-tuesday-morning`.

## Preflight consistency review

| Tasks | Shared interface | Finding and disposition |
|---|---|---|
| 1 → 2 | Source snapshot and initial course boundary | Registry and source bank determine next segment; no inference from stale ledgers. |
| 2 → 3 | Accepted specs and review decisions | Landing depends on generator evidence plus semantic review; proposals alone never suffice. |
| 3 → 4 | Bank, page, and consumers | Do not call work deployed until the consumer path is tested. |
| 4 → 2 | Continuation checkpoint | Resume by committed state and durable files; do not duplicate completed proposals after interruption. |
| 1 | Baseline scope | Existing suites suffice; no low-value tests that merely mirror data edits. |
| 2 | Source defects | C05:167's numeral discrepancy is preserved and reviewed, not silently fixed. |
| 3 | Producer label | Wrapper currently names Claude; new pages must identify the actual producer honestly. |
| 4 | Deadline vs completion | Deadline stops new work; unfinished work stays explicitly unfinished. |

## First continuation checkpoint — 2026-09-12

Tasks 1–2 and the non-visual Task 3 checks are complete for the accepted pages
through C05:177. The next batch starts at 178. Task 3’s rendered-page inspection remains
unverified because the browser URL policy refused local HTML preview. Source,
structure, bank and export checks passed; the unchecked box is intentional.
Task 4’s native test and signed phone install are verified through C05:171,
but the locked device prevented launch. Main integration and further campaign
work remain open. See the durable continuation record for current commits.

## Second continuation checkpoint — 2026-09-12

Accepted data now reaches C05:183 in `2f16dee` and `f30209b`. Both batches
passed all 16 targeted suites and separate semantic reviews; 205 analyst spans
were accepted after one overcapture correction. Two proposed spelling corrections
were refuted, and one new LOW/PROBABLE candidate was recorded without changing
the source. Previous data and citations are retained. Next is 184–186, with
source snapshots only. Rendered inspection, newer native/device execution and
main integration remain open; the earlier consumer checkpoint is still 171.

## Export repair found during continuation review

The course CSV used another course's depth metadata and collapsed several depths
into one count. A focused regression reproduced four failures before repair;
all five methods now pass. The exporter preserves separate course/Tibetan/English/depth
groups. Full-data regeneration and a source-ref oracle verify all 22,451 rows and
retention of prior course/pair aggregate counts. The other three CSVs, dictionary
HTML, banks, and master/source files are unchanged. The changes report remains
the 55-row delta against `2f16dee`. Private CMake registration is now 118 suites;
this is not a full-suite execution claim. See the append-only handoff ERRATA entry
and sibling integration evidence before continuing at C05:184.

## Third continuation checkpoint — 2026-09-12

Accepted C05 now reaches 189 in `bb77f01` and `883a4ee`: 168 reviewed analyst
spans plus six anchors. Every new page passed all 17 targeted suites, including
the course-export regression; every course/depth row and count matches actual
main CSV references. Previous links, citations, lexical fields and source hashes
are retained. The 185 la/to correction passed a separate scoped re-review, and
E-186 remains LOW/PROBABLE with exact committed digital witness evidence.
Independent fixed-commit reproduction passes for both banks, HTML and all four
CSVs, with 168 exact reviewed tuples. Read-only main preflight is recorded in the
final independent report; no main integration is inferred. Next 190–192 has source/context snapshots only. Task 3's visual limit and
Task 4's unfinished integration/campaign work remain explicit; no newer native or
phone execution is inferred. Resume from the latest sibling continuation record.

## Fourth continuation checkpoint — 2026-09-12

Accepted C05 now reaches 195 in `827fd9b` and `8cc5820`: 115 independently
reviewed analyst spans plus six sentence anchors. Both pages passed all 17
targeted suites, and all 22,622 course/depth rows match actual source refs and
counts. Previous data and source hashes survive; two old empty ACIP export
fields acquire spellings from new source-attested d5 links. No new errata.
The RJES candidate was refuted as underdetermined inflection, and the ambiguous
help construction stays omitted with the rejected passing attempt preserved.
Final integration review caught an evidence-description error in the semantic
review: the rejected attempt paired first Tibetan phan pa with first English help,
as reconciliation had correctly stated. The original and corrected review states
and scoped factual re-review are preserved; accepted specs remain unchanged.

Both data commits independently reproduce both banks, HTML and all four CSVs;
227,513 checks pass at the second commit, including all115 exact tuples and six
individual bodies. Scoped factual re-review approved the corrected help account.
The final report and companion evidence record final documentation review and
read-only main applicability. No main integration
or newer native/device/rendering verification is inferred. Next is196–198,
source/context snapshots only. Tasks 3 and 4 retain their stated limits;
the hourly campaign remains active through the user-authorized deadline.


## Fifth continuation checkpoint — 2026-09-12

Accepted C05 reaches 204 in ef8c0e4, 59de591 and 6665c77: 144 independently reviewed
analyst spans plus nine sentence anchors. Each of the three pages passed all 17
targeted suites, and all 22,751 course/depth groups match actual source references.
Prior data/citations and master/source hashes survive. One old phonetic export
row gains source-attested RING DU from 203; no old lexical content or depth changes.
No errata or allowances were added. The two 196 nulls remain distinct: fused
plurality and assertion closure; an appended ledger ERRATA corrects the earlier
fusion-only description without changing accepted specs or review evidence.

Independent fixed-head reproduction and the final report cover both banks,
HTML, all four CSVs, 144 reviewed tuples and nine individual bodies. Final committed
documentation and read-only main applicability are recorded in sibling
integration/continuation-through204-review.md. No main integration or newer
native/device/rendering verification is inferred. Next 205–207 has source/context
snapshots only. Tasks 3 and 4 retain their stated limits; the hourly campaign remains
active through the authorized Tuesday morning deadline.


## Sixth continuation checkpoint — 2026-09-12

Accepted C05 reaches 213 in cc7d690, 5aeb6e9 and a143082: 210 independently
reviewed analyst spans plus nine sentence anchors. All three pages passed all
17 targeted suites. Both JSON banks, HTML and all four CSVs independently
reproduce at each fixed data head; all 210 tuples and nine individual bodies
match. All 22,896 course/depth groups match actual source-reference counts.
Original source/master, full banked text, prior links and citations are retained.
Two prior empty ACIP export fields acquire NUB and LNGAR from new d5 source links.

The first landing exposed an incomplete numbered course-heading cut and a
legacy 40-row limit. A separately reviewed repair makes the following-heading
boundary explicit and validates every shortening against its cited source/depth.
It changes exactly eleven depth-1 display rows (ten earlier plus new206) while
retaining full source text and unchanged phonetic extraction. The existing gate
runs ten new literal regressions; RED/GREEN, mutations, exact migration records
and original failed attempts remain preserved. No general truncation permission
is introduced, and all other prior lexical content remains intact.

E-187 and E-188 are the only new errata, both LOW/PROBABLE after five-ground
review; two sogs closing-gesture allowances are explicitly supported. The false
ces-null cursor workaround, uncertain English rewrites and wrong-of attachment
remain rejected. An appended ledger ERRATA corrects a historical bar-gloss
attribution; final semantic review and E-187 already distinguish HGM between
from comparative-only interval. No source or accepted alignment was changed.

Final committed documentation, evidence and read-only main applicability are
recorded in sibling integration/continuation-through213-review.md. No main
integration or newer native/device/rendering verification is inferred. Next
214–216 has source/context snapshots, focused original-parallel checks and a
verified baseline at a143082, with no proposals yet. Tasks3/4 retain their
stated limits; hourly continuation remains ACTIVE through Tuesday morning.


## Seventh continuation checkpoint — 2026-09-12

Accepted C05 reaches 222 in f242fdc, 12b77ec and 21964bb: 329 independently
reviewed analyst spans plus nine sentence anchors. All three pages passed all
17 targeted suites. Every one of 23,090 course/depth export groups matches its
actual source references. All previous links, notes, trees, evidence, lexical
keys, citations and ACIP mappings survive. No old ACIP field changes. Original
source/master and the existing heading-trimming code remain unchanged.

Scoped correction rounds closed the 218 note/errata/narrative findings and the
220 topic-marker absence claim plus 222 copyedit disposition. The explicit ni/
Here is exponent is now aligned; the distinct 222 ni null remains justified.
Complete causal recasts preserve cause/result roles without equating converse
verbs atomically. E-189 polarity and E-190 affect/effect are the only new errata,
both LOW/PROBABLE with all five grounds checked and complete physical originals.
The 218 BAG/BAGS alternatives remain reviewed, unfiled variants. Seven precise
sogs allowances are added; prior errata/allowances and failed attempts survive.

Final fixed-head reproduction of both banks, HTML and all four CSVs, original
evidence checks, committed documentation and read-only main applicability are
covered by sibling integration/continuation-through222-review.md. Final checkpoint
approval requires that report to name the committed final HEAD and close all
required checks. No main integration or newer native/device/rendering verification
is inferred. Next 223–225 has a clean baseline at 21964bb, source/context and root
lexical preparation only; no proposals. Tasks 3 and 4 retain their stated limits.
Hourly continuation remains ACTIVE through the authorized Tuesday morning deadline.


## Eighth continuation checkpoint — 2026-09-12

Accepted C05 reaches 231 in 11a388a, 9f9edc6 and 1809556: 367 independently reviewed
analyst spans plus nine sentence anchors. All three pages passed all 17 targeted
suites, and every one of 23,325 course/depth groups matches actual source-reference
counts and metadata. Previous links, notes, trees, evidence, lexical keys and
citations survive. Three previously empty cig ACIP export fields gain CIG from
the newly accepted 226 d5 witness without changed wording, depth or citations.

The reviews retained positively attested whole units while rejecting reversed
causal roles, a false compound member, supplied apparatus and incomplete phrase
recasts. Five explicit nulls are independently justified; distributed/unresolved
content is omitted with reasons. No new erratum survives; the 230 punctuation
candidate is refuted under the five-ground protocol. Two exact sogs allowances
are added; all 190 older errata and previous allowances remain unchanged.
Original source/master, hgm_gloss and canonical generation/export code are intact.

Complete proposals, historical attempts and immutable builder versions preserve
the concurrent page-registration history. Final independent fixed-head output
reproduction, original-evidence audit, committed documentation and live-main
read-only patch applicability are recorded in
`campaign-artifacts/integration/continuation-through231-review.md`. Final approval requires that report to name the committed
final HEAD and close all required checks. No main integration or newer native,
device or rendered-page verification is inferred. Next 232–234 has source/context,
focused original-parallel checks, independent-role briefs and a clean baseline
at 1809556; no proposals. Tasks 3/4 retain their stated limits; hourly continuation
remains ACTIVE through the authorized Tuesday morning deadline.


## Ninth continuation checkpoint — 2026-09-12

Accepted C05 reaches240 in 6232214, aa580e8 and 499e907: 236 independently reviewed
analyst spans plus nine sentence anchors. Each page passed all 17 targeted suites;
every one of 23,471 course/depth groups matches actual source-reference metadata.
Previous links, notes, trees, evidence, lexical keys and citations survive. Two
previously empty ACIP fields gain CIS and SEMS TZAM from new d5 source witnesses.
Source/master, hgm_gloss and canonical generation/export code remain unchanged.

Supported whole units and lexical occurrences remain; supplied/distributed or
disputed phrase relations stay unwrapped. A false yin/null is corrected to is.
No new nulls are introduced. E-191 and E-192 are LOW/PROBABLE relation/spelling
candidates after all-five-ground review; actual HGM 25759 disappearance evidence
limits the 235 claim and refutes 236 under uncertainty. Five precise source-
licensed allowances are added. A scoped reviewer narrative correction restores
four source apostrophes without changing any accepted input or decision. All
originals, failures, superseded decisions and governing Git snapshots survive.

Final fixed-head output reproduction, original-evidence audit, committed
documentation and live-main read-only applicability are recorded in sibling
`campaign-artifacts/integration/continuation-through240-review.md`. Final
approval requires that report to name the committed final HEAD and close every
required check. No main integration or newer native/device/rendering verification
is inferred. Next 241–243 has a clean baseline at 499e907 and independent proposals
in preparation, with no reconciliation/acceptance. Resume their exact durable state.

The user's throughput refinement moves new proposal pairs to Astra/high, keeps
angles independent, overlaps preparation/review and stores identical immutable
evidence once with complete byte/hash provenance. Compact reports keep all
material decisions and contrary evidence. No semantic stage, canonical generator
or required fidelity test is removed; no measured overall speedup is claimed.
Tasks 3/4 retain their explicit limits; hourly continuation remains ACTIVE through
the authorized Tuesday 07:00 America/Denver cutoff.


## Tenth continuation checkpoint — 2026-09-12

Accepted C05 reaches249 in3a88d13 (audit supplement81dbbe4),885c766 and9aa617e:
286 independently reviewed analyst spans plus nine anchors. Each page passed
all17 targeted suites; every one of23,652 course/depth groups matches actual
source references and metadata. Prior links, notes, trees, evidence, lexical
keys, citations and ACIP metadata survive without enrichment. Source/master,
hgm_gloss and canonical generator/export implementation remain unchanged.

The 536 original proposal dispositions and286 final correspondences preserve
technical units, exact occurrences and whole supported recasts. Three affirmative
nulls are independently justified; distributed or uncertain content is omitted
with reasons. No errata added,192 older entries retained, one exact sogs allowance.
Shared-verse attribution remains unresolved. Corrections to structured mappings
and reviewer narratives retain exact histories and unchanged candidate tuples.
All source bytes, immutable Git pins and complete original evidence survive;
whitespace-audit recovery and helper-quoting failures are explicitly recorded.

Final fixed-head output reproduction, original-evidence/correction review,
committed documentation and full read-only live-main applicability belong in
sibling `campaign-artifacts/integration/continuation-through249-review.md`.
Final approval requires that report to name the committed final HEAD and close
all required checks; it is not inferred here. Main integration and newer native,
device or rendering proof remain open. Next250–252 has clean baseline9aa617e,
source/context, separate briefs and two independent proposal readers running.
No reconciliation or acceptance is implied; resume their precise frozen state.

Preparation, independent reading, reconciliation, review and integration overlap
while preserving angle independence. Evidence checks now compare each distinct
file once and retain all diagnostics. No semantic stage or required check is
removed, and no numerical overall speedup is claimed. Tasks3/4 retain their
explicit limits; hourly continuation remains ACTIVE to the authorized Tuesday
07:00 America/Denver cutoff.


## Eleventh continuation checkpoint — 2026-09-12

C05 reaches 255 in 0561829 and d428993: 115 independently reviewed analyst spans
plus six sentence anchors. Both pages pass all 17 required suites; all 23,719
course/depth groups match actual references and metadata. Prior links, notes,
trees, evidence, lexical keys, citations and ACIP metadata remain intact, without
new enrichment. Source/master, hgm_gloss and canonical generation/export logic
remain unchanged; the builder gains only two page registrations.

All 207 original dispositions and 115 final correspondences retain exact source
roles, repeated occurrences, full supported recasts and tighter compound members.
Fifteen affirmative nulls are explicitly justified; uncertain/distributed wording
is omitted with reasons. No errata added; 192 retained. Five precise source head
allowances are added. Six narrative apostrophe corrections and matching helper
literals preserve unchanged accepted data and complete pre-fix history. Root
helper failures and byte-safe whitespace classifier/schema repairs are recorded;
every warning was accounted for before the two private commits.

Final independent fixed-head output reproduction, original evidence/correction
review, committed documentation and read-only full-campaign main applicability
belong in sibling campaign-artifacts/integration/continuation-through255-review.md.
Final approval requires that report to name the final committed documentation
HEAD and close all required checks; it is not inferred from this entry. Main
integration and newer native/device/rendering verification remain open. Next
256–258 has clean baseline d428993, exact source/context, independent briefs and
root lexical/parallel preparation only; capacity-limited dispatches created no
readers or proposals. Resume durable state without contaminating original angles.

Preparation and independent review overlap while all gates remain in place.
No numerical overall speedup is claimed. Tasks 3/4 retain their existing limits;
the hourly heartbeat remains ACTIVE through Tuesday 07:00 America/Denver.

## Twelfth continuation checkpoint — 2026-09-12

Data commit48bd9d4587352461bc609fde85b5bfe374a05212 reaches C05:258/511,
86 pages. Since165,31 new pages contain2,624 independently reviewed analyst
spans and93 sentence anchors. This batch adds91 spans and3 anchors, no nulls,
errata or head allowances. All remain PROVISIONAL. All17 required suites
passed11.16seconds;23,769 course/depth groups match actual main references.
Prior lexical content/citations survive. One older mi'i/humans record gains
proven MI'I metadata through the new C05:256 source witness; original archives
and canonical generation/export logic stay unchanged.

Complete semantic decisions,172 original dispositions, nine five-ground
refutations and every failed/superseded attempt survive. The original reviewer's
scoped packaging fix preserves11 Python helpers as exact .py.txt copies and
retains all prior interfaces. Root verified599 mappings/66 historical Git
copy records,91 tuples and463 semantic file copies. The two actual builders
ran once each and exited0; a later overly strict wrapper stderr assertion
failed, with actual logs and original CSV seed preserved. The whitespace
preparation/v3 failures and scoped v4 proof survive; all44,036 diagnostics
were accounted for before commit against exact source/freeze/CRLF bytes.

A fresh fixed-head reviewer must close the complete checkpoint, reproduce
all seven outputs from Git with the actual previous CSV, verify original
evidence/corrections/retention/documentation and perform a read-only full
campaign patch preflight against current MAIN and all fixtures. Its report
belongs in sibling campaign-artifacts/integration/continuation-through258-review.md.
This entry does not itself establish final approval. MAIN integration and
newer native/device/rendering checks remain open. Native/phone proof remains
through171; locked launch remains unverified.

Next259–261 has clean data baseline48bd9d4, exact source/context and separate
original-angle briefs; no reader has been dispatched. Root evidence/logic
leads are withheld.262–264 and265–267 are source preparation only with no
baseline or proposals. Reuse preserved state without exposing one angle's
judgments to another. The larger campaign and hourly heartbeat remain ACTIVE
to Tuesday,September15,2026 at07:00 America/Denver. No speed multiplier claimed.

## Thirteenth continuation checkpoint — 2026-09-12

Data235bd2ebabc2278a91a8c06c95f8371f2e5a77fe reaches C05:261/511 and87pages.
Since165,32 new pages contain2,720 independently reviewed analyst spans and
96 sentence anchors. This batch adds96 spans and3 anchors, no nulls, errata
or allowances. All remain PROVISIONAL; source/master and hgm_gloss unchanged.
All17 required suites passed12.08seconds; all23,810 course/depth groups match
actual main refs and metadata. Prior links, notes, trees, lexical content,
citations and ACIP metadata survive without enrichment. The real previous
CSV seed is48bd9d4; both canonical builders ran once and exited0.

Root read177 original dispositions,96 final assessments,27 material omissions
and7 five-ground checks. All649 original mappings/21 historical Git records
and448 semantic copies verify. The corrected447-file freeze includes every
nested original freeze.json and preserves the failed enumeration attempt.
Root's original tuple-schema audit failure and corrected complete-object proof
also survive. No semantic or source bytes changed during these corrections.
The full HGM two types variant is preserved; the accepted two boundary is a
local judgment. All52,622 staged diagnostics are accounted for using original
physical source, canonical CSV CRLF and exact frozen review pins.

A fresh fixed-head reviewer must close this complete checkpoint, reproduce
all seven outputs from Git with the actual previous CSV, verify evidence,
retention and final documentation, and perform the read-only full original
campaign patch preflight against current MAIN and all1912 fixtures in each
checkout. Report: campaign-artifacts/integration/continuation-through261-review.md.
This entry does not establish final approval. Prior1448ea8 remains the closed
through258 checkpoint. MAIN integration remains open; native/phone proof is
through171; locked launch and rendered inspection remain unverified.

Next262–264 has clean baseline235bd2e and separate original-angle briefs;
265–273 are source/context preparation only. No new original readers have
started. Root leads remain withheld until both angles freeze. Use the corrected
semantic-review-brief-v2.md interfaces. Continue the preserved campaign under
the existing ACTIVE heartbeat, now every five minutes through Tuesday,
September15,2026 at07:00 America/Denver. No speed multiplier is asserted.


## 2026-09-12 continuation through264 — data accepted, checkpoint review pending

Private data2c1e0768629aff301b9f80a510d7ccbcf53d6ba7 lands c5p88 after independent
SPEC/QUALITY approval. Two original angles191 spans; corrected final108 spans,
11 nulls,75 non-null d5 pairs and3 automatic anchors. The second blood/pus
member was restored under the recorded contiguous-member ruling, with all
original/correction history retained. Separate non-authors were reused under
explicit platform-capacity rulings; no fresh-context claim or removed gate.

All17 suites passed13.57seconds; both actual canonical builders ran once with
the235bd2e CSV seed and exited0. Root verified765 exact semantic copies,
689 source mappings/38 Git pins, all23840 course-depth groups, retained prior
links/evidence/metadata, unchanged source archives and119316 narrowly classified
staged warnings. The whitespace helper gained only a constrained actual-freeze
filename input. Its complete sources and failed metadata history are preserved;
the full helper diff is external with exact SHA. Data remain provisional.

Coverage264/511 over88 pages. Bank44686/null11906/notes1600/trees11; evidence
7065heads/15415pairs; CSV22311main/21971reverse/23840course/23changes. No added
errata or allowances. Root's final-stage check found830 scoped files, no tracked
unstaged changes, and identical full warning output before commit.

A fixed data/documentation checkpoint review is still required, including seven
output reproductions, all1912 fixtures per checkout and a new read-only full
original-base MAIN patch check. Native/device/rendering/MAIN limits persist.
265–267 baseline is2c1e076, no original proposals yet; preparation extends through
285. Continue the five-minute campaign until2026-09-15 at07:00 America/Denver.

## 2026-09-12 — Codex private continuation through C05:267

Private data commit f3c1ca804a933d53f84214d6968a84e7e0193c6e lands C05:265–267
as page89: 92 analyst spans (19/38/35), four explicit nulls (2/0/2), 61 non-null
d5 pairs and three sentence anchors. C05 reaches267/511 across89 pages. Since
segment165,34 new pages contain2920 analyst spans and102 anchors. All remain
PROVISIONAL machine attestations; source/master bytes and hgm_gloss are unchanged.

The primary Codex reader authored the Tibetan original, disclosing its prior
source preparation, and froze all three drafts before opening English proposals.
A separate reader authored English; two distinct non-authors reconciled and
semantically reviewed the result. The explicit capacity ruling preserves these
four roles while reusing existing child records. No fresh-context claim is made,
and the primary's mechanical verification is not independent semantic approval.

The independent semantic report approves SPEC and QUALITY with zero findings.
Root read its complete29641 bytes and all92 adjudications; report SHA-256
16d8c1b797e7920e566b2f696d2932b4fabef8caee44cc6fdde383b14e6b94f8.
All171 original span identities survive with dispositions. Reviewers reopened
186 primary and five supplemental SQL sets,65 full master records,75 raw corpus
records and24 complete physical extents. All nine original/final canonical bodies
reproduce exactly. The current freeze lists525 files;526 including the freeze
were copied exactly, with527 original-copy mappings and19 historical Git pins.

Material decisions preserve265's second provides occurrence, active agentive
nulls and lexical sogs/and so on;266's collected root, three, matrix is/not,
contracted n't and two blood/pus members;267's whole rigs 'dra phyi ma/continuation,
look root, no/fullness/agentive distinctions, causal Since, experiencing root and
two blood/pus members. Positive glossary variants were considered alongside full
source syntax. One exact allowance, pages_c05/c5p89/s265w15, licenses the internal
and in sogs/and so on using HTG2016 HGM20919 and the local enumeration. No new
errata were accepted; all192 previous entries and previous allowances survive.

Both actual canonical builders ran once and exited0, using the real previous
main CSV from2c1e0768629aff301b9f80a510d7ccbcf53d6ba7. All17 targeted suites passed
in10.77seconds. Prior links, notes, trees, lexical rows, evidence references and
ACIP metadata were retained without enrichment. Bank44781 links/11910 nulls/
1603 notes/11 trees; evidence7067 heads/15430 pairs. CSV rows:22341 main,
22001 reverse,23875 course-depth and30 changes. Every course-depth group matches
actual references and metadata. ACIP forms are proven for7064 of7067 headwords;
the builder's rounded100.0% display does not mean every headword has a proven form.

Root verification errors are preserved. The CTest process passed, but its wrapper
then asserted a different footer and failed; root verified every numbered Passed
record and the actual footer without rerunning tests. Parent python -B did not
disable bytecode in CTest children: three tracked engine caches changed. Their
changed bytes were preserved before restoring only those caches to exact HEAD
bytes in the private checkout. Future subprocesses must explicitly inherit
PYTHONDONTWRITEBYTECODE=1. No source or engine code changed.

The first whitespace classifier refused three original physical lines inside a
frozen English collection log. Root mistakenly launched the dependent v5 audit
after that failure; it also failed because the pins file had not been written.
Both failures and full raw diagnostics survive. Strict v6 adds exact original
physical-line witnesses: frozen log hash, indexed/live equality, warning line
set, full original-file hash, byte ranges and actual line boundaries must agree.
No source whitespace was normalized and no general exception was introduced.
All59823 warnings are accounted for:37076 whole physical-source warnings,
22731 canonical CSV CRLF,three exact physical-source log lines,nine empty diff
contexts and four blank EOF warnings. The complete25478080-byte raw log has SHA
f870c2d24362d44014be0ae88ea1ef24b8a5b8529073223bd050599434a34a10;
its full bytes stayed identical after final proof staging. Both whole helper
versions and all failure history are retained; the complete helper diff is
external with a SHA pin. The private data commit contains597 scoped files.

The fully closed preceding checkpoint is7d3d232, through264. Fixed through267
data/documentation review is still required, including all seven exact output
reproductions, all1912 fixtures per checkout and a fresh read-only complete
original-base MAIN patch preflight. This entry does not claim those future checks
passed. No changes have been applied to MAIN. Native/installed-phone verification
remains through171; rendered inspection, locked launch and MAIN integration
remain open. C13 remains blocked pending its upstream correction.

Next268–270 baseline was captured from clean dataf3c1ca8 with all nine current
hashes and eight required references checked. No next original proposals exist
at this entry. Source preparation is complete through294;295–297 is partially
prepared and explicitly unfinished. The authorized five-minute continuation
remains active until2026-09-15 at07:00 America/Denver. No additional files needed.

## 2026-09-12 — Codex private continuation through C05:270

Private data commit 32472a829c3336f13667d19c3df92538cca3340f lands C05:268–270 as page90: 123 analyst spans (25/34/64), five explicit nulls
(0/1/4), 75 non-null d5 pairs and three sentence anchors. The corrected builder
proves270/511 across90 pages. Since segment165,35 pages contain3043 reviewed
analyst spans and105 anchors. These remain PROVISIONAL machine attestations;
source/master bytes and hgm_gloss remain unchanged.

Root authored the Tibetan originals, disclosed its earlier source preparation,
and froze all three before reading the separate English proposals. Distinct
non-authors reconciled and semantically reviewed the batch. Contexts were reused
with disclosed roles; root performed mechanical acceptance, not semantic approval.
Every227 original identity retains its disposition:200 retained,26 recast and
one rejected. All123 exact final tuples, optional fields and occurrences survive.

The initial full semantic review returned one blocking finding: the retained
sweet-to-clear erratum candidate did not defeat uncertainty. The non-author
reconciler removed that candidate from current errata and corrected its note.
Both dvangs and sweet remain unwrapped; refuting an editorial allegation does
not establish a lexical correspondence. Positive and contrary dictionary
records, original sources, all five refutation grounds and historical proposals
remain intact. No source text was corrected and no new erratum was accepted.
All192 previous errata remain unchanged.

The independent scoped re-review resolved C05-268-ERRATA-01 and approved SPEC
and QUALITY with zero open findings. It combines the completed initial full
alignment review with exact verification of the note-only correction; it does
not claim a fresh full semantic pass. Final report31823bytes SHA-256
cff14cd1bf28cc2d25b6c55444df31ea7e44149e2d1f251b578ba6f909bf4fba.
Current freeze285239bytes SHA-256
dd97122be4eaf14d7c848ebddf1f87cf97f9eadead09ee21196ed2eac06faa25
lists1394 files;1395 including itself are copied exactly. Current manifest has
1926 mappings and78 historical Git pins. Initial governing repository objects
remain pinned to2a1b9a8; changed live paths use explicit immutable-history
transport. All573 initial review files,923 root-preserved historical files,
936 transports and20 nested freezes remain verifiable. Initial packages retain
their initial status; no old NEEDS_CORRECTION result was relabeled APPROVE.

Material alignments preserve268's repeated human/world/karma/taken references,
created by as a complete phrase, and lexical sogs/and so on. Its single specific
head allowance is pages_c05/c5p90/s268w21, licensed by HGM20919 and source ranges
Wylie[311,315), English[426,435). Segment269 distinguishes all four spirit
references, outer-domain phrase expansion and omitted inner-domain explanatory
prose; three obstacle/with pairs and six compound members have distinct ranges.
Segment270 distinguishes repeated threes, two complete mantra-of-steel names
and a later abbreviated name, the second provides, two sets of application/hand/
contact predicates, contracted n't and the final coordination-licensed myong
null. Supplied words and unresolved correspondence stay unwrapped.

Nine initial original/final canonical executions pass. The reviewer ran exactly
one new canonical268 execution for the changed note;269/270 remain exact prior
successful runs. Root independently regenerated268 and then the assembled page.
The assembled body is21400bytes SHA-256
9df2ab728dac4c87cfdc0ea3877b95cac1cd4d624f757e053a485be5b93f88b9;
combined spec SHA-256
f8035b43d6147b779c6d89fa040f52077cd7277e63cb4076129cdb2c8e987a72.

Root initially omitted page90 from the builder registry after updating the page,
index and allowance. Both builders exited0 but still reported267 coverage;
root stopped acceptance before testing or committing. All15 execution artifacts
and seven actual generated outputs were preserved, then all seven outputs were
restored byte-for-byte from baselinef3c1ca8. The sole builder-code change registers
c5p90:[268,269,270]. Corrected builders ran again and exited0 with270 coverage.
They each ran twice for this batch; canonical-builds-once.json describes only
its particular attempt. The historical first attempt remains explicitly separate.
Both attempts use the exact previous main CSV fromf3c1ca8. No algorithm changed.

All17 targeted CTest suites ran once and passed in12.29seconds with explicit
PYTHONDONTWRITEBYTECODE=1 inherited by children. All44781 prior links, earlier
notes/trees/evidence/ACIP,22341 prior main CSV rows and23875 course-depth groups
are retained without enrichment. Current bank44907 links/11915 nulls/1606 notes/
11 trees; evidence7073 heads/15452 pairs. CSV rows:22385 main,22045 reverse,
23932 course-depth and44 changes. All23932 course groups match exact main-CSV
references and metadata. ACIP is proven for7070/7073 heads; rounded100.0% does
not mean complete headword coverage. Source/master hashes remain exact.

The full staged whitespace audit accounts for141409 warnings:114913 complete
original physical-source warnings,22821 canonical CSV CRLF,3638 exact whole
ILL/original ZIP-member warnings,30 print-separator warnings,4 existing empty
EOF warnings,2 exact diff-context lines and1 exact root-log EOF. The complete
44112924-byte diagnostic SHA-256 is
ba7343bb6a19e47b73e66c2922e208db4fa079a0a8a7030236a6c20e282cd186.
It remained byte-identical after all final proof staging. No source or evidence
whitespace was normalized. The private data commit contains1862 scoped files.

The earlier pin preparer refused these new evidence forms. A bounded candidate
adds exact verification of the actual producer print call/default separator,
full SQLite/corpus field equality, complete original ILL bytes, original DOCX
member equality, and the specific root output's manifest/execution/empty EOF.
Root's initial inference that six trailing log spaces came from source fields
was wrong: they are print-separator bytes. An AST regression test exposed the
need to verify the actual producer call even if pins were updated. Full actual
failed attempts remain, including the candidate's initial refusal of the root
log and its final verifier's wrong source-line22 assumption corrected to23.
Sharedv6 and original root preparer remain unchanged. No general whitespace
exception or semantic-freeze relabeling was introduced.

Root independently verified all434 candidate files, both complete baseline
diffs and all16 exact pins against real staged/original bytes; fresh16 witness
tests and1 dispatch test pass. The dispatch test uses a test Git transport;
actual current staged verification is separate. The real full staged audit
passed at00:29:04UTC Sep13, writing only to a fresh external output directory.
Candidate freeze SHA-256
af8db137b011bf7accfdfda477ae506609a5acb80f73f1c6c2975049b00a9538
remains immutable. All434 full candidate files/818971825 bytes and actual test
fixtures/failure history remain available; banked code/proofs/metadata and
explicitly pinned external large/raw witnesses preserve every version.

The final root copy manifest has448 exact records and175 explicitly retained
external records. Earlier128-record before-whitespace and441-record before-
final-stage manifests remain exact historical snapshots. Initial staging proof
covers1855 files; a separate final supplement verifies the7 additional proof
records, the sole updated current manifest and unchanged1854 prior file pins,
then binds all1862 actual staged files. Its own last proof stays external to
avoid a self-referential hash. The immutable data commit binds the final tree.

The preceding fully closed checkpoint is2487bfc through267. A separate fixed
through270 checkpoint review, full seven-output reproduction and fresh read-only
original-base MAIN patch check are still required. No changes applied to MAIN.
Native/installed-phone verification remains through171; rendered inspection,
locked launch and MAIN integration remain open. C13 remains blocked upstream.
Source preparation through303 is not accepted alignment. Evidence-copy v2 is
available only for explicitly pinned future batches and was not used here; a
reading-index candidate remains unadopted. The active continuation is every five
minutes through2026-09-15 at07:00 America/Denver. No additional files are needed.

## 2026-09-13 — C05 through 273; private data commit 2e5d5159

Private data commit `2e5d515987c1b30d7812ab9d1ac7f42397b6ad37` adds
C05:271–273 as page 91. It contains 2,625 scoped files. Generated coverage is
273/511; this does not claim that C05 is finished. The preceding independently
closed global checkpoint is `7166534a9f47a201e5ab580a843c14ba2a910891`
through 270. This entry records page acceptance; the separate fixed-head global
through-273 review and fresh read-only MAIN preflight remain pending.

The page has 101 independently reviewed analyst spans (52/21/28), eight explicit
nulls (4/4/0), 63 non-null d5 pairs and three sentence anchors. Root authored
the Tibetan originals and froze them before reading separate English proposals.
Distinct non-authors reconciled and reviewed them with disclosed reused context.
All 198 original identities retain full dispositions: 180 retained, 16 recast
and two distributed omissions. Root's acceptance checks are mechanical;
semantic approval comes from the independent SPEC and QUALITY review.

The initial reviewer upheld one causal-scope finding in 273. The correction
removed only p22 and its English-order entry and appended an explanatory note.
The first phyir [196,201) and first for [91,94) remain verbatim and unwrapped:
they connect different premises in the two versions. Positive HGM11962 evidence
for for/because/since remains preserved. This is distributed restructuring,
not absent meaning or an explicit null. The distinct final phyir/for mapping at
[255,260)/[265,268) remains. Scoped independent re-review approved both gates
with zero open findings. All 101 surviving full spans, optional fields, parentage,
source ranges and rationales match the initial review after removing only p22;
the other 197 original dispositions remain unchanged. Six five-ground errata
refutations survive; no erratum or head allowance was added. All 192 prior
errata remain exact, and the initial NEEDS_CORRECTION package stays unchanged.

The final semantic freeze contains 2,032 files plus its top manifest, SHA-256
`b4f9b53d8cbc18385b006f7d8dff7a04cee73fa5bf61a98e9b031bcb32c1cf4c`.
Its 12,568 full source mappings retain 1,975 distinct copies, 9,145 historical
source-version transports and 283 Git aliases against 11 original objects.
Root reopened complete originals/copies before landing. The assembled spec is
17,245 bytes, SHA-256
`f77fb9eebff766ffc97db8c3290ba17c09cbc2b009920ef21e4c85ac3f56adbc`;
the canonical body is 17,220 bytes, SHA-256
`0d3ccc58baef88af484d202aaf5d6020101e733308c47e2c1db0acc47e922c33`.
The sole builder change registers c5p91:[271,272,273]; navigation and index
advance exactly. Source/master bytes and binding hgm_gloss are unchanged;
machine attestations remain PROVISIONAL.

Both live builders ran once, exited 0 and reported coverage 273. The view used
the exact prior main CSV seed from 7166534. All 17 targeted CTest suites passed
once in 12.16 seconds. All 44,907 prior links, earlier notes/trees/evidence/ACIP,
22,385 main CSV rows and 23,932 course-depth groups remain without enrichment.
Current totals: 45,011 links, 11,923 nulls, 1,609 notes, 11 trees, 7,078 evidence
heads and 15,469 pairs. CSV rows: 22,413 main, 22,073 reverse, 23,974 course-depth
and 28 changes. Every course group matches exact main CSV references/metadata.
Actual ACIP head coverage is 7,075/7,078; rounded 100.0% is not complete coverage.
The missing spellings and six unchanged prior empty-ACIP rows remain explicit.

An independent isolated run reproduced both JSON banks, dictionary HTML and all
four CSVs byte for byte with the preserved previous CSV seed and frozen date.
The retention oracle passed 223,906 checks. Its first ACIP diagnostic and scoped
recovery are preserved. Root reopened all 2,785 frozen reproduction files, 2,715
full input aliases, seven immutable prior Git outputs and seven current outputs.
Reproduction freeze SHA-256:
`6e4c34741e2712221a22806cf602440e99ab8d2ec1ce1ae3bcd8e89240d7a34f`.
No builder or CTest rerun was used to close staging.

Independent review of the first whitespace candidate found discarded supplied
root aliases, rejection of 33 exact physical CRLF EOF fragments, and rejection
of a complete 2,176-byte diff with blank context at EOF. The corrected helper
preserves and rechecks aliases, proves complete physical-source ownership, and
parses balanced full diffs; ordinary LF EOF checks remain strict. Author 60
tests and independent 63 boundary/changed-fragment checks pass. Both actual
prepare and audit ran once and exited 0. The raw Git diagnostic itself exits 2;
all 95,906 warnings in 58 files have exact source evidence: 73,208 physical-byte,
22,691 canonical CSV CRLF, five diff-context and two blank-EOF warnings. Its full
36,268,143 bytes, SHA-256
`ce93fc36cce920a0d24660ef2ac1b7f735059a96beb3b9e4e4ee82921e02a766`,
remain identical after final staging. No source/evidence whitespace was changed.
All failed, rejected and superseded tooling versions and full witnesses survive.

Ordinary banking preserved 261 source/copy and 2,981 external records. Final
banking preserves 575 source/copy and 6,959 external records; its manifest is
4,159,097 bytes, SHA-256
`89a9f4b919aa32c27af1fd4ffa550771ce7dba1e1c6ac1da65d21f278b28d9e4`.
The 576 bank files include that final manifest. The original pre-banking raw
untracked capture is preserved: 2,039 of its paths became staged, 576 later
root-bank paths were added, and ten already tracked changes give 2,625 staged
files. Only the intentional dcc_captions directory remains untracked.

The full staging v2 execution stopped on an incorrect Git-tracked-source
assumption: the dictionary gzip exists as a local source alias, not an object
in the incoming Git commit. The actual failed git-show command, exit 128 and
complete 1,478-file run are retained. The minimal v3 correction checks canonical
alias/target identity, baseline hashes, unique non-Git provenance, the frozen
semantic copy and complete original/copy bytes. The rest of v2 is byte-exact.
Fifteen focused author tests and fifteen fresh independent tests pass; the
initial dispatch-test failure is preserved. Actual full v3 execution passed.
Root reopened all 24,467 consumed file pins (2,814,784,746 bytes), all 2,625
current staged files, all 2,935 complete command records and the current index.
The final stage proof is 1,015,312 bytes, SHA-256
`53bb499f5c36f626bf184220c8987a05361676efc23eb1bb6d7ddfd7bb5079e4`.
The final bank is immutable. Its later execution, staging v2 failure/v3 fix,
reviews, actual v3 run, root acceptance and commit records remain complete
external evidence under campaign-artifacts/integration; they are not claimed
inside the earlier bank. This avoids changing an input to include its own proof.

The current through-273 global checkpoint retains its full original-base patch,
lossless archive, fixture census and fresh read-only MAIN preflight commitment.
Root verified the 49-file frozen preparation, 39 original/copy mappings and 43
exact workspace copies. The unchanged helper is approved to execute only in
the separate unfrozen workspace after the actual data/documentation heads are
supplied; the old preparation must remain immutable. No global verdict is
claimed by this ledger entry.

After that checkpoint closes, the future global cadence is six accepted pages
(18 segments), next scheduled through 291, following ALIGNMENT_LAYER_SPEC §7.
Every three-segment page still requires independent original angles,
reconciliation, semantic review, original evidence preservation, canonical
generation, all 17 fidelity suites, source/prior-data retention, independent
seven-output reproduction, exact staging/whitespace checks and a page ledger
and commit. Only the repeated whole-history archive census, complete fixture
census and full campaign-to-MAIN applicability check move to the outer cadence.
Run the global checkpoint earlier for a course boundary, deadline, material
handoff, integrity discrepancy or a substantive source/schema/algorithm/proof
helper change. A routine reviewed page-registry addition retains its page gates.
Any actual integration still requires fresh applicability against current MAIN.
The evidence-backed cadence recommendation is retained at
campaign-artifacts/integration/checkpoint-cadence-review/report.md. No measured
speedup or APFS copy optimization is claimed. Earlier commitments stay intact.

Native and installed-phone verification remain through 171; rendered inspection,
locked launch and MAIN integration remain open. No changes were applied to MAIN.
C13 remains blocked upstream. Source preparation through 303 is not accepted
alignment. Finish all 511 C05 source segments before starting C06; 177 contiguous
C06 rows are available locally. The five-minute continuation remains active
through 2026-09-15 at 07:00 America/Denver. No additional files are currently needed.

## 2026-09-13 — C05 through 276; private data commit 2602b7d7

Private data commit `2602b7d7966f3ee332393e860770f25a6b3a3f79` adds
C05:274–276 as page 92, with 2,446 exact scoped files. The previous independently
closed global checkpoint is `81942cfc6022a785894d3cc2a20de81dc4c6f599`
through 273; its final root closure is retained under
campaign-artifacts/integration/through273-root-closure/final/proof.json.
Current generated and page-verified coverage is 276/511, with 235 segments open
(277–511). The early through276 global checkpoint is still pending here.

Independent proposal, reconciliation and semantic review accepted 112 analyst
spans (42/59/11), three explicit nulls (one per segment), 81 non-null D5 pairs
(31/43/7), and three sentence anchors. All 211 original proposal identities
retain dispositions: 198 retained, 12 recast and one distributed omission.
Uncertain correspondence remains unbanked with its reason. The initial review
required three explanatory metadata corrections in 275; no approved lexical
span, source range, optional field, occurrence, note or canonical body changed.
The corrected explanations distinguish zer/claim, the residual genitive and
the negative clause's D5 correspondence from its quoted D3 parent. Complete
initial review and pre-correction originals survive. Scoped independent final
SPEC and QUALITY both APPROVE, with 16 checks passing and zero open findings.
The verdict is model review, with role/context disclosures preserved.

The final semantic freeze contains 2,286 files, SHA-256
`694dceeb5248484e702d1973c84be78b7f6b34750fa75708113338e0887fee8a`.
All 3,234 original/copy mappings, 14 governing Git objects, source archive
aliases and nine baseline pins were reopened. The canonical landing helper
omits 24 extensionless raw execution files; a separately reviewed supplement
copied exactly those 6,646 bytes with a persistent attempt/completion journal.
The complete 2,287-file semantic tree, including freeze, is byte-identical.
Rejected copy helper and failed root suffix preflight remain preserved. No new
errata or head allowances were added. GMR wording, master/source bytes and
binding hgm_gloss remain unchanged; machine attestations stay PROVISIONAL.

Both canonical builders ran once, with the exact incoming main CSV seed; all
17 targeted CTest suites passed once in 11.32 seconds. Complete actual commands,
stdout, stderr and exits are retained, including the view's normal trim output.
The bank contains 45,126 links, 11,926 nulls, 1,612 notes, 11 trees, 7,083 evidence
heads and 15,500 pairs. All 45,011 prior links and prior notes, trees, evidence,
ACIP spellings and lexical export fields remain exact. The 115 new links are
the 112 spans plus three anchors. CSV rows are 22,457 main, 22,117 reverse,
24,024 course-depth and 44 changes. All course-depth groups match main references
and metadata. Original CSV BOM/CRLF and source-page digest semantics survive.
ACIP head coverage is 7,080/7,083 (99.957645%); rounded 100.0% remains incomplete.
Three missing heads and six unchanged historical empty-ACIP rows are explicit;
five of those rows have source segments, and one historical row has seg=null.

Independent isolated reproduction used one layer build and one correctly
seeded view build. All seven complete outputs are byte-identical; 224,556
retention checks passed. Its freeze covers 3,051 files, SHA-256
`672ca1b2f1eae5c72d6259d552ecc810e177595fb73975e10661aaa4f5272280`.
Root verified all 2,979 input mappings, seven prior Git outputs, all current
source/output aliases and all twelve frozen staged payloads. No passed builders
or CTest suites were repeated for banking or staging.

The first actual whitespace prepare failed on four aliases of one preserved
historical Python SyntaxError log. Its clipped source echo ends in an original
ASCII space; the complete physical source line does not. The failed helper,
full diagnostic and all partial evidence remain unchanged. A narrowly reviewed
new predicate binds the original failed source, invocation, full streams,
manifest aliases and exact source ranges. Compile-only exception rendering
reproduces all 2,220 stderr bytes; the failed source body is never executed.
Eleven focused author tests, actual fallback dispatch and fourteen independent
negative provenance checks passed. The code and descriptor were independently
approved and fully root-verified before fresh actual prepare/audit each passed.

All 62,470 warnings in 56 staged files are accounted for: 39,554 original
physical-byte warnings, 22,912 canonical CSV CRLF warnings and four recorded
SyntaxError source-echo warnings. The full raw diagnostic is 27,652,514 bytes,
SHA-256 `cb2a68b32f29fc54d10dae2f745aff0599110ff01dab868789fe2aaed353c1dd`.
It remained byte-identical during final staging verification. No source or
evidence whitespace was normalized. This substantive predicate change requires
an early global checkpoint through276 before starting 277–279; the regular
six-page global checkpoint remains scheduled through291 afterward.

Ordinary banking preserved 123 copy records and 11,063 external records. Final
banking preserves 142 copy records and 12,611 complete external records; its
6,634,433-byte manifest has SHA-256
`0c7a9f21ac4f3e61431a9854644f3da2cfbb625d10fdcef2827c3b35673149bf`.
The original pre-bank untracked stream is retained unchanged. Exactly 2,427
initial files plus 19 later bank files were staged. The complete final verifier
passed; root reopened 15,211 consumed inputs totaling 2,227,467,786 bytes,
all 2,487 complete command records and every staged blob, then checked a fresh
identical index and unchanged HEAD. Final-stage proof SHA-256 is
`3e1d221662d00067fcb1c0534f1237c0ce98755ec8b90ee93f4548f80eb5d507`.
Only the intentional dcc_captions alias remains untracked. The final bank and
landing directory are immutable. Later final-bank/staging/root/commit/global
execution evidence is retained externally under campaign-artifacts/integration;
it is not falsely claimed inside its earlier input bank.

The pending global276 checkpoint will bind the existing reproduction to actual
data/documentation commits, produce full canonical review packages, review all
current source/history/data/documentation obligations, and measure a fresh
read-only original-base-to-final MAIN applicability conjunction. The exact
accepted whole-file partition method will be used from the outset; the prior
oversized unsplit failure stays history. No patch has been applied to MAIN.
Private JSON, HTML and CSV consumption artifacts are current through276;
native/installed-phone verification remains through171. Rendered inspection,
locked launch, current native/device consumption and MAIN integration remain
open and must not be described as deployed. C13 remains blocked upstream.
Finish all 511 C05 segments before C06 (177 contiguous local sources). The
five-minute continuation remains active through 2026-09-15 at 07:00
America/Denver. No additional files, paid API generation or release is needed.

### 2026-09-13 — Correction to the through276 checkpoint entry

Independent global review found two inaccuracies in the preceding entry's
description of the metadata correction. There was one blocking correction and
two nonblocking explanatory clarifications. The phrase "quoted D3 parent" is
incorrect: C05:275 w4, mi 'thad par / mistaken (D5), and r14, mthong med /
don't see (D3), are separate alignments. Their Tibetan ranges [28,40) and
[146,156), and English ranges [78,86) and [283,292), do not contain one another.
The intended description is "the negative evaluation's D5 correspondence from
the separate quoted D3 predicate." This corrects the ledger prose only; the
accepted source, specs, canonical bodies, ranges and generated data are unchanged.
The original entry, private commit bbf32652 and its full review packages/patch
remain preserved. The early global276 checkpoint must use the subsequent
documentation commit and newly bound packages/applicability evidence.


### 2026-09-13 — C05 segments 277–279 (page 93)

Independent proposal, reconciliation and skeptical semantic review approved
146 correspondences (61/21/64), including eight explicit null correspondences.
Uncertain correspondences remain unbanked with recorded reasons. GMR's source
wording and the master sources are unchanged; all new alignments remain
provisional and none were promoted to hgm_gloss. The complete source specs,
original proposals, review decisions, omissions and actual execution history
are retained in reviews_c05/c5p93 and the external campaign evidence manifests.

One root layer/view build and all 17 relevant fidelity suites passed. The
independent isolated rebuild reproduced all seven generated outputs byte for
byte. Full retention checks preserved all 45,126 prior links and added 149
links, including three sentence anchors. Coverage is now 279 of 511 C05
segments, with 232 remaining. The full bank has 45,275 links, 11,934 nulls,
7,084 heads and 15,517 evidence pairs. ACIP is present for 7,081 of 7,084
heads; the three missing heads remain an explicit limit. Main/reverse/course
CSV row counts are 22,491/22,151/24,061; the changes CSV has 34 data rows.

Actual prepare/audit accounted for every preserved source, CSV and historical
formatting warning without changing the evidence bytes. Final bank and staged
source checks precede this append. Complete actual later bank, stage, commit
and closure outputs are retained externally; they cannot be inside their own
earlier input manifests. The fixed DATA and documentation commit identities
are recorded in the subsequent root closure proof.

Private JSON, HTML and CSV artifacts are current through 279. Native and
installed-phone consumption remain verified only through 171. The separately
reviewed phone parser correction is still external and unadopted; a fresh
production build and complete pack fidelity check remain pending. MAIN
integration is open; its last successful read-only applicability check was
2026-09-13 at 17:25:51 UTC, before later unrelated native edits. No MAIN patch
or release was applied. Finish all 511 C05 segments before C06, then courses
in order; C13 remains blocked upstream. The next regular outer checkpoint is
291. The five-minute continuation ends 2026-09-15 at 07:00 America/Denver.

## C05 through279 — parser code verified; private phone pack withheld

Page DATA746a6f549207a4254049aab747cb898f530d1b44 and page documentation f7686dce07f03c356f565ec71c6bdbaa18cf11e7 remain valid. Separate private CODE59d668e5407b085494220f9f4d4601659ebe5dd0 changes only tools/build_drill_pack.cpp and tools/drill_evidence_json.h. These are the exact independently reviewed CP276-01 sources: JSON decoding now preserves complete English strings and integer counts for second/vocab. No source wording, source archive, spine, hgm_gloss or seven accepted page outputs changed. All machine alignments remain provisional. Code/source/object binding: campaign-artifacts/integration/through279-parser-code-commit/proof.json in the campaign root.

The original58 failures and72 passing corrected controls remain retained. A fresh current verification separately passed the unchanged historical72 checks and three complete current modes:7084 heads/15517 English-count rows in exact source order; second300 heads/1237 rows with exact source multiplicities/ranks; vocab6138 pre-conversion/FTS candidates, not6138 emitted drills. Root acceptance is through279-parser-current-root-acceptance/proof.json; actual current freeze1255234a61bc9258c3c44efbd221a4c2460d967d6746d1d1b119a56805d98c84. Existing page279 builders,17 fidelity suites and224711 independent reproduction/retention checks were not rerun for this code bookkeeping.

A fresh normal private CMake configure retained118 written/registered suites and fixtures. One target build compiled80 units and linked79 allcore object members plus the Darwin archive symbol table. Full compile/link/SDK/dependency provenance is retained at through279-private-pack-build-provenance-complete/proof.json. The new production binary is488656 bytes, SHA6be6273004f31dbda69e8ae3118de93d68a16b926be048d1e1accbb185496822. A provenance assertion originally counted the archive symbol table as an extra object; its failure and corrected retention are preserved. No successful build/test was repeated.

Exactly one external DIAGNOSTIC phone pack was produced in136.73005429 seconds:10208944 bytes, SHA6592c01f45d52621b32556155d247003d1c8380f7d4f03312c02b5db5489e6c1. Counts: cloze4000, order1000, particle1000, script1200, debate200, silent300, second300, peel250, boundary144, vocab400, trainer1000. A separately compiled, source-reviewed read-only native comparator ran once in121.01812517 seconds and matched every pack byte and ordering choice;39037 conversion entries cover18580 requests. This is observed behavior evidence, not an independent semantic oracle.

The independent strict-UTF8 source checker ran once with that trace, preserving complete schemas, raw source ownership and structured silent/peel checks. Its bounded PASS cannot approve the pack. A discovered UTF8 boundary error in the original checker was corrected through an exact one-function change and20 passing targeted tests after9 actual negative failures; all earlier failures remain. Full comparison to the old through171 pack records6775 changed pointers:2416 second,4358 vocab,1 metadata. Other arrays are structurally unchanged. Four inherited TAB-to-space English display transformations retain their complete original strings; original-byte equality is not claimed. Source checker, native trace, complete difference and review evidence are retained externally under campaign-artifacts/integration.

Independent semantic review found four INHERITED C01 standalone vocabulary failures, all also present in the old through171 pack: 'da' bar -> “gressing” (C01:180/s180w9); 'ong rgyu -> “a” (C01:170/s170w7); bden par ma grub pas -> “has any true existence” (C01:412/s412w5); bden par med pa -> “exists truly” (C01:422/s422w2). One cuts an English word, one assigns an article to distributed predication, and two lose Tibetan negation when shown alone. Exact links, complete source paragraphs/physical byte intervals and old/current exposure are preserved. No replacement English or C01 correction was composed. Separate bounded source review is required before correction or quarantine.

QUALITY is NEEDS_CORRECTION for the private pack. It remains external and UNACCEPTED; it has not replaced WT/iOS or installed content. Root disposition is through279-private-pack-root-disposition/proof.json; independent source review freeze50b9c2a12b03d8432305d67826525d093e1b456f398809a09460ae0a9ca4545b. These inherited consumer failures do not invalidate accepted C05:277–279/page93 evidence. Seven selected output locations use current page93 alignment refs; the sole direct C05:278 cloze example is corpus/factory sourced and is not evidence that new alignment generated that drill. There are278 explicit C13 addresses, zero in63–82, and no selected alignment refs toC13. Hidden dependencies are not fully attributed, and no universal C13 refusal is claimed. C13 alignment remains blocked upstream.

Coverage remains279/511 C05,232 segments remaining; full layer45275 links,11934 nulls,1615 notes,11 trees,7084 heads/15517 pairs. All45126 prior links remain. Desktop HTML/CSV data are current279; native rendering, installed phone and QSettings routing remain unverified beyond the recorded through171 boundary. MAIN integration remains OPEN; no patch, release or device operation occurred. Failed/deferred consumer disposition is recorded separately from fixed page/code acceptance. Mandatory early279 global packages/history/fixtures/partitioned read-only MAIN checks and independent/root closure remain outstanding before280 originals. The regular outer checkpoint through291 remains scheduled; it does not waive this early trigger.

Complete consumer evidence is retained at campaign-artifacts/integration/through279-parser-final-ledger-preparation/complete-consumer-evidence-manifest.json. It lists complete full-byte external originals, including source specs/reviews, actual commands/results, corrected and failed tooling, binaries, data, dependencies and current measured coverage. This ledger and later checkpoint callers are subsequent evidence, not falsely self-banked. Continue C05 beforeC06; no spend, paid API, account messages, release or overwrite of others' work. Standing local authorization requires no further user confirmation.


### 2026-09-14 UTC — C05 segments 280–282 (page 94)

Independent proposal, reconciliation and semantic review approved 124
provisional correspondences (54/46/24), including four grammatical nulls and
102 nonnull depth-5 correspondences. All 225 original proposal dispositions,
15 material omission reasons and 11 investigated/refuted suspicions survive.
Exactly two source-specific sogs head licenses were added; no errata were
added. Uncertain correspondences remain unbanked. GMR's wording and master
sources are unchanged; none of this machine work enters hgm_gloss. Complete
sources, specs, proposals, decisions, generator results and semantic review
are retained in reviews_c05/c5p94 and the external campaign evidence records.

One root layer/view run and all 17 relevant fidelity suites passed; the suite
took 12.62 seconds. The independent isolated reproduction matched all seven
generated outputs byte for byte with the immutable incoming main CSV seed.
Full retention preserved 45,275 prior links and added 127, including three
sentence anchors. Coverage is 282 of 511 C05 segments, with 229 remaining.
The full bank has 45,402 links, 11,938 nulls, 1,618 note blocks, 11 trees,
7,092 heads and 15,546 evidence pairs. Proven ACIP covers 7,089 of 7,092
heads; three missing heads and five source-linked spans remain explicit
limits. Main/reverse/course CSV rows are 22,528/22,188/24,105; changes has
37 rows. Producer metadata date is September 13 local time; the execution
records are September 14 UTC. No date-driven regeneration was performed.

The actual 13-file payload includes the two-license sidecar and preserves
all prior sidecar values. Staged whitespace checks account for the original
source and CSV bytes without normalization. Complete current bank and stage
checks precede this append. Later bank, stage, commit and closure results
remain external at their true boundaries; they are not claimed inside their
own earlier input manifests. Source-preparation failures and corrections
are preserved. Three retention-preparation caller results are explicitly
labeled conversation transcriptions: original raw streams were unavailable.
This limitation does not replace the full actual generator/build/test or
independent reproduction captures. Fixed DATA and documentation commit
identities are recorded by the subsequent root closure.

Private JSON/HTML/CSV outputs are current through 282. MAIN remains
OPEN_NO_APPLICATION; the September 13 23:40–23:42 UTC compatibility check
passed all 13 chunks but applied nothing. The new private phone pack remains
FAILED_UNACCEPTED for four inherited C01 source-correspondence defects and
must not be installed. The existing through171 phone resource is unchanged;
no current native rendering, device installation or saved data-root acceptance
is claimed. No release, paid API, spending, account message or source/gloss
correction occurred. C13 remains blocked upstream. Finish all C05 through511
before C06, then proceed in order. Next regular outer checkpoint is291.
The existing one-minute continuation ends September 15, 2026 at07:00
America/Denver (13:00 UTC); safely checkpoint and pause at the deadline.

### 2026-09-14 UTC — C05 segments 283–285 (page 95)

Independent proposal, reconciliation and semantic review approved 76
provisional correspondences, with no grammatical nulls and 61 nonnull
depth-5 correspondences. All 145 original proposal dispositions survive:
129 were retained and 16 recast. The review preserves 77 material omission
reasons and four investigated/refuted suspicions. Exactly two source-specific
sogs head licenses were added; no errata were added. Uncertain correspondences
remain unbanked. GMR's wording and master sources are unchanged; none of this
machine work enters hgm_gloss. Complete sources, specifications, proposals,
review decisions and actual generator/test results are retained in
reviews_c05/c5p95 and the external campaign evidence records.

One root layer/view run and all 17 relevant fidelity suites passed; the suites
took 11.25 seconds. The independent isolated reproduction matched all seven
generated outputs byte for byte with the immutable incoming main CSV seed.
It passed 225,597 retention checks with 3,031 input aliases. Full retention
preserved 45,402 prior links and added 79, including three sentence anchors.
Coverage is 285 of 511 C05 segments, with 226 remaining. The full bank has
45,481 links, 11,938 nulls, 1,621 note blocks, 11 trees, 7,093 heads and
15,557 evidence pairs. Proven ACIP covers 7,090 of 7,093 heads; three missing
heads and five source-linked spans remain explicit limits. Main/reverse/course
CSV rows are 22,546/22,206/24,126; changes has 18 rows. Producer metadata date
is September 13 local time; execution records are September 14 UTC. No
date-driven regeneration was performed.

The 13-file payload includes the two-license sidecar and preserves all prior
sidecar values. Actual whitespace prepare and audit account for all 93,740
native warnings in 112 files through exact physical/source/CSV predicates;
the original Git diagnostic and exit 2 are retained without normalization.
Complete final bank and staging verification precede this append. Later bank,
stage, commit and closure results remain external at their actual boundaries;
they are not claimed inside their own earlier input manifests. All failed
preparations, superseded sources and corrections are preserved, including the
failed first reproduction preparation, three whitespace source-analysis
failures, the root intake recipe-reader failure, and the independently reviewed
STAGE-001 variable-shadowing correction. Native captures distinguish source
preparation from actual execution. Fixed DATA and documentation commit
identities are recorded by the subsequent root closure.

Private JSON/HTML/CSV outputs are current through 285. MAIN remains
OPEN_NO_APPLICATION; the September 13 23:40–23:42 UTC compatibility check
passed all 13 chunks but applied nothing. The private phone pack remains
FAILED_UNACCEPTED for four inherited C01 source-correspondence defects and
must not be installed. The existing through171 phone resource is unchanged;
no current native rendering, device installation or saved data-root acceptance
is claimed. No release, paid API, spending, account message or source/gloss
correction occurred. C13 remains blocked upstream. Finish all C05 through511
before C06, then proceed in order. Next regular outer checkpoint is291.
The existing one-minute continuation ends September 15, 2026 at07:00
America/Denver (13:00 UTC); safely checkpoint and pause at the deadline.

Final verification exposed a source-archive ownership lookup mismatch; a
separately reviewed correction verifies both complete original-to-copy chains.
Its two real archive checks and twenty rejection controls passed. A later full
verification attempt then stopped on internal-disk exhaustion. Its partial
captures remain unaccepted; unavailable target exit and outer streams were not
reconstructed. A fresh complete final-stage run is required before this append.

Five closed C05 campaign patch records, totaling 5,053,241,758 bytes, were copied
to the attached Oct2024(8TB) drive and compared byte for byte. Their existing
filenames now resolve to those verified copies through individual symlinks.
The first temporary-link creation itself failed for lack of space; the first
path was restored after an explicitly recorded brief gap, and the remaining
four replacements were atomic. Original bytes and historical physical metadata
are preserved. No worktree, MAIN, semantic, master-source or index file moved.
Recovery evidence is under the external campaign archive's
storage-recovery-20260914T1200Z directory. Keep that drive connected for evidence
verification. These later storage and verifier records remain outside their own
earlier input manifests, with both failures and successful recovery retained.
