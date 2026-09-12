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
