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
