# Technical notes for Joel — 25 August 2026

*Supplement to the Technical Brief of 22 August. Read the corrections
first: some of what that brief says is no longer true, and three
statements in it were not true when written.*

---

## 0. Corrections to the brief you may already have

I would rather you hear these from me than find them.

| The brief says | Actually |
|---|---|
| "aligned corpus of **35,601** segments" | **42,199.** 35,601 was the pre-v32 corpus. The shipped spine records `n_corpus_segments=42199`, and that is the number the app can demonstrate live. |
| "**70** automated test suites passing" | **85** as of today. |
| "`allcore` … `app/main.cpp`" as ~33k lines | **39,448** lines — 53% of the codebase in one file. Called out below. |
| "A constitution rule fails the build if any staged data directory lacks a licensing entry" | **Was not true when written.** That gate (L2) was a keyword match that never opened the payload. It is true now. |
| "Every component that ships is inventoried with its source, its license, and a redistribution verdict" | **Was not true when written.** The notices named 6 components; 35 upstream projects shipped. It is true now. |
| "Reference dictionary compilations whose provenance is uncertain are excluded from public builds entirely" | **Was not true when written.** 1,926 raw THL catalogue records were shipping in every DMG while the notices said that payload was not redistributed. Fixed by removal. |

The last three are the interesting ones, and they are why the rest of
this document exists.

---

## 1. What happened between 22 and 24 August

A full software-quality assessment was run against the product, then
re-run after remediation using the **same generator**, so the two are
comparable.

- **22 Aug baseline: grade C−.** 22 findings, 2 critical.
- Remediation of the 17 remaining high-severity findings across six
  tracks.
- **23 Aug re-measurement: grade C.** One notch. **No dimension grade
  moved** — the notch came from three measured firsts (the repository's
  first write-failure test, the first event-driving assertions, and two
  gates that open the payload instead of grepping it).
- The re-measurement found **2 new criticals**, both at the *sibling
  sites* of the two that had just been fixed. That is the finding I
  would most want to discuss.
- Both criticals and all five top-ranked items are now closed. 42
  commits, gate green at every step.

**Why the sibling-site pattern matters.** `LibraryIndex::search` was
capped; `goferSearchFiles` — the same evaluator, duplicated — was not,
and measured 115,589 ms with a 23.5 GB peak RSS on the *default* search
path of a fresh install. `ProposalStore` was taught to flush before
judging its own write; `GlossaryStore`, `DossierStore` and
`CommentStore` were not, and returned success over 64 of 2,842 bytes.
Two copies of a rule are how the copies drift.

**And one measurement lied.** The baseline probed those three stores at
600 rows — above the stream buffer, the one regime where the broken
shape works — recorded them as honest, and so the remediation excluded
them. The measurement manufactured the pass. That is worth more of your
attention than any individual bug.

---

## 2. New this week, and the parts you may want to poke

**Mutation testing as a standing measurement.** `tools/mutate.py`
plants a named bug, rebuilds, runs the battery and **requires** a red
result; green is the failure condition. `tools/mutation_sweep.json`
holds a committed mutation set with expected verdicts, so the score is
comparable run to run and reports DRIFT if any verdict changes.
Current score **5 of 6**, opened at 2 of 6.

Two things make that number quotable rather than decorative: a
**liveness control** that must be killed or the whole run is void, and
the rule that an entry moves from "survived" to "killed" only when
somebody writes a real pin. It caught three of my own tests that could
not fail.

**Payload manifest and licence enumeration.**
`docs/distribution/PAYLOAD_MANIFEST.txt` carries one row per shipped
path with a licence and a notices anchor. The press stages *from* it,
prunes to it, and is gated on it — 406 MB of unshipped ingest material
now comes out of every DMG. `tools/bundle_licenses.py` maps all 165
Mach-O objects to 35 upstream projects and **fails the press on any
binary it cannot attribute**.

**SBOM.** `tools/sbom.py` emits CycloneDX at press time. It covers
**data layers as well as binaries**, deliberately: the licensing
finding that cost us a day was data, not dylibs. It reports **24 of 115
components whose terms are OPEN rather than settled** — not new, but
counted per press now instead of discovered in an audit.

**Dependency scanning.** `tools/vuln_check.py` queries OSV for every
component, with triage: **302 raw candidates → 16 needing attention
across 9 components**. Labelled *candidates requiring triage*, never
"vulnerabilities found", because OSV has no Homebrew ecosystem and name
matching over-reports across distributions. A failed query exits
non-zero and says "This is NOT a clean result" rather than reporting
zero.

**Threat model** (`docs/THREAT_MODEL.md`) and **privacy statement**
(`docs/PRIVACY.md`), both grounded with file:line citations.

---

## 3. Your six questions, answered as far as I can

**1. BDRC.** Unchanged and still yours to advise on: the OCR models are
CC BY-NC 4.0 with a verbal okay from August. My reading is that free
distribution falls inside the licence; written confirmation is what we
want, and it should probably come from ALL rather than from me.

**2. Apple Developer identity.** Still the single highest-leverage
unblock. The app is **ad-hoc signed, `TeamIdentifier=not set`, not
notarised**. The signing and notarisation path is built and dormant.
One caveat found in audit: the armed path uses `--deep` with a hardened
runtime and no entitlements, which would fail on a QML/Quick bundle —
so when an identity arrives, budget an afternoon rather than a
`--sign` flag.

**3. Source hosting.** This is now urgent and I would like a decision.
Until 23 August **there was no off-machine copy of this project at
all** — no remote, and Time Machine's last reference to its external
volume predated the project's first commit. There is now a verified
mirror on an external drive plus a nightly job, but a drive beside the
laptop is not disaster recovery. Related: `.git` was 10 GB of committed
build artefacts; a history rewrite took it to **186 MB**, so hosting it
is now trivial rather than awkward.

**4. The catalog.** Unchanged — I would still rather feed ALL's real
catalog than build a parallel one.

**5. Input centres.** Unchanged, and now with a concrete stake: the
build is **arm64-only** with the macOS floor documented but no
universal binary, so we need to know what those machines are before
promising anything.

**6. Succession.** `docs/DEVELOPER_ONBOARDING.md` now exists. It has
**never been proven** by following it on a clean machine, and 37 of the
suites still cannot run from a clean checkout because they need
untracked fixtures. Until someone else builds this from scratch, the
bus factor is one and I would not claim otherwise.

---

## 4. What I would most like your judgement on

1. **The sibling-site pattern.** Two criticals appeared within 24 hours
   at the duplicate of code we had just fixed. Is there a structural
   answer beyond "delete the duplicate evaluator"?
2. **`app/main.cpp` at 39,448 lines**, 53% of the codebase. My plan is
   staged extraction of self-contained panes, one per press, each
   proven by the existing battery — not a rewrite. Sanity-check that.
3. **The 16 triaged advisories.** The open question is reachability:
   libpng is reached *only transitively* through Qt and OpenCV, so
   whether the vulnerable push-mode parser is ever entered depends on
   what those do with a scanned PNG. Nobody has traced it.
4. **CI.** Costed in `docs/CI_REQUIREMENTS.md`, deliberately not stood
   up. It is also what would have caught the clean-checkout failures.
5. **The root `LICENSE`.** A file now exists; Adam has read it, counsel
   has not, and the terms ALL wants to release under are an
   organisational decision nobody has made.

---

## 5. Numbers, for the record

| | |
|---|---|
| Test suites | 85 |
| Mutation score | 5 / 6, drift-detecting |
| SQA grade | C (from C−), 15 critical+high remaining |
| Bundled upstream projects | 35, all attributed, press fails otherwise |
| Data layers with open terms | 24 of 115 |
| Dependency advisories needing judgement | 16 across 9 components |
| `.git` | 186 MB (was 10 GB) |
| Corpus | 42,199 aligned segments |
| `app/main.cpp` | 39,448 lines |

*Everything above is reproducible from the repository: `tools/verify.sh`,
`tools/mutate.py --sweep`, `tools/vuln_check.py --triage-all`,
`tools/sbom.py`.*
