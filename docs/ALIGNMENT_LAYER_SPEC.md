# Alignment Layer v1 — the specification as built

*Written 2026-09-04 to satisfy condition 3 of `docs/PHASE3_GATE_ALIGNMENT_V1.md`:
one document stating the page-spec schema, the depth-nesting rules, the gate
inventory with what each proves and cannot prove, the audit protocol, and
Rule 10. Every statement below is drawn from the tools and briefs it cites;
where the code is the authority, the code wins and this file is corrected.*

## 1. What the layer is

A machine-authored, PROVISIONAL alignment of Geshe Michael Roach's (GMR's)
published English against the ACIP Tibetan of the same segment, at seven
Wilson depths, for every segment of a scanned course. It is **attestation,
not definition**: it records which English GMR used for which Tibetan in
which segment. Nothing in it is promoted into `hgm_gloss` (ADR-003); nothing
in it is BINDING until the human-review preconditions of the gate document
§5c are met.

Artefacts:

| artefact | path | built by |
|---|---|---|
| pages (both sides wrapped in place from the spine) | `data/alignment/pages/` (C02), `pages_c01/`, `pages_c03/`, `pages_c04/` | `tools/gen_alignment_page.py` + `tools/wrap_alignment_page.py` |
| evidence sidecar (depth-5 word pairs, ACIP-keyed, cited) | `data/alignment/alignment_evidence_v1.json` | `tools/build_alignment_layer.py` |
| full bank (every span at every depth, notes, trees) | `data/alignment/alignment_full_v1.json` | same |
| dictionary view | `docs/michael_dictionary.html` | `tools/build_dictionary_view.py` |
| errata register | `docs/errata_register.json` → `docs/ERRATA_REGISTER.md` | `tools/merge_errata.py`, `tools/build_errata_register.py` |
| campaign ledgers | `data/alignment/C0N_CAMPAIGN.md` | by hand, one entry per batch |

Courses locked complete (coverage-gate proved): C01 496/496, C02 68/68,
C03 620/620 (2026-09-03), C04 198/198 (2026-09-04). C05–C18 unscanned. C13 is under standing order not
to be scanned until the upstream column offset is resolved
(`docs/upstream/C13_COLUMN_OFFSET.md`).

## 2. Page-spec schema (input to `gen_alignment_page.py`, JSON on stdin)

```json
{ "course": "C03",
  "segments": [
    { "seq": 160,
      "title": "short lowercase title",
      "clauses": [ {"id":"c1","tib":"...","eng":"..."} ],          // optional, d=1..4
      "spans": [
        {"id":"w1","d":5,"tib":"zhi gnas","eng":"quietude"},
        {"id":"m1","d":7,"tib":"gnas","eng":"quiet"},               // member of w1
        {"id":"p1","d":6,"tib":"kyis","eng":null,"cls":"case","nul":"instrumental; unrendered"},
        {"id":"w2","d":5,"tib":"ma","eng":"n't","subword":true}
      ],
      "eng_order": ["w2","w1"],                                       // span ids in ENGLISH order
      "note": "AI NOTE body, may contain HTML" } ] }
```

Span object keys: `id`, `d` (1–7), `tib`, `eng` (string or `null`), optional
`cls`, `nul` (why the null), `subword` (bool). The top-level keys
`generator` and `errata` that analyst deliverables carry are NOT part of the
spec and are stripped before landing.

**The deliverable is a file.** A reconciler's output is a `spec.json` on disk
that has exited 0 from the generator, never a returned object. Reason: on
2026-08-28 a structured-output schema silently dropped `eng_order`, and three
passing specs were reported REFUSED (commit 900d1b6). The transport is the
filesystem; the proof is the exit code.

## 3. The ten rules the generator and the briefs enforce

Verbatim from `docs/alignment_briefs/RECONCILE_BRIEF.md`, condensed:

1. **VERBATIM.** Every `tib` is a contiguous substring of the spine wylie;
   every `eng` a contiguous substring of GMR's English, byte for byte,
   including errors. Never silently correct while quoting.
2. **TIBETAN ORDER** by a forward-only cursor over every span of depth 1–6.
3. **DEPTH.** d=1 sentence · d=2 clause · d=3 phrase · d=4 embedded phrase ·
   d=5 word or compound (the dictionary layer) · d=6 standalone particle
   that starts a syllable · d=7 bound morpheme or compound member. **A d=7
   span must immediately follow the d=5 span it lives inside**, resolves
   within that parent's range, and does not advance the flat cursor. A d=7
   hung off a d≤4 clause is refused. Position, not containment, is the test.
   Prefer the flat shape (d=5/d=6 at top level, d=7 inside compounds, no
   clause layer unless the clause is the thing being aligned).
4. **MEMBERS** are unique inside the parent, strictly tighter than it, and
   their English lies inside the parent's English.
5. **NO INVENTED ENGLISH.** A morpheme with no exponent gets `eng: null`.
6. **AMBIGUOUS ENGLISH:** equal counts resolve, unequal do not. When English
   crosses Tibetan order, supply `eng_order`; each side then has its own
   forward-only cursor.
7. **NO OVER-CAPTURE** (the dominant measured defect). Articles, hinges and
   pronouns GMR supplied stay unwrapped.
8. **NO BROKEN WORDS**, except the four confirmed negative-affix forms
   ("n't", "un", "im", "less") with `subword: true`. A fifth form
   (`ma` → "not" inside "cannot"; `ma` → "fail to" at C03:539) awaits Adam's
   ruling (TODO.md).
9. **AGENTIVE -S is not separable.**
10. **NEVER BANK A FALSEHOOD, even a verbatim one** (`docs/NEVER_BANK_A_FALSEHOOD.md`).
    A span that passes every mechanical check can still be false; it is
    nulled with a note. Rule 10 outranks coverage.

Campaign conventions settled during C03 (ledger entries name the rows):
`sogs` owns only the closing gesture, never GMR's supplied noun; `byin gyis
rlobs` → "Grant me" on majority precedent; a Tibetan word rendered twice by
GMR is banked once at the cursor position; a standing published epithet may
be banked whole at d=5 with a note, members only where a morpheme sits under
them; a proper-name title is banked whole; `dam tshig` alone may own "pledges
and vows" (HGM's gloss), so neither `dam` nor `sdom` claims one word of it.

## 4. Markers and apparatus

Corpus rows whose wylie field carries an English running head (part-title
markers) are not translation pairs. They get a hand-built page block
(`s<seq>hdr` / `s<seq>app`) with an AI NOTE naming the E-071 class, so the
coverage gate can prove the course complete. C03 markers: 22, 294, 303, 430,
476, 513, 558. Row-boundary spills (E-111), footnote fusions (E-076/E-121),
heading glue, the {SHl}/{lv}/{NGg} converter escapes (E-044 class) and the
1,500-character English cap (E-107) are registered classes; they are quoted
verbatim, never corrected, and not re-filed.

## 5. The gate inventory (`cd build && ctest -R <name>`)

| gate | proves | cannot prove |
|---|---|---|
| `gen_alignment_page` | generator refuses non-verbatim, out-of-order, broken-word, degenerate-member, orphan-d=7 and ambiguous specs; `eng_order` branch pinned | that an accepted span is the right pairing |
| `alignment_builder` | every wrapped fragment is a literal substring of its cited spine segment; refuses otherwise | correspondence |
| `no_broken_words`, `no_split_syllables` | no span cuts an English word or a Tibetan syllable | — |
| `no_degenerate_members` | no d=7 member equals or exceeds its parent | — |
| `no_supplied_span_head` | d=5/d=7 English spans beginning with a supplied article or hinge: ratchet on C01–C03 (frozen 2026-09-04 at 1,643 / 50 / 603), zero-or-allowlisted with a named licensor for C04+ | which of the frozen spans are licensed |
| `no_phonetics_in_layer` | d=7 sound-spelling pairs never enter the evidence layer | — |
| `layer_matches_spine`, `evidence_matches_spine` | every banked field occurs at every cited segment (NOT-IN-CORPUS hard zero) | correspondence |
| `view_matches_layer` | the dictionary view shows only what the layer banked | — |
| `builder_sees_every_span` | the builder parses every span the pages carry (closes the 2.7% silent drop) | — |
| `errata_quotes_hold`, `errata_name_their_witnesses` | every erratum's `found` still holds against the spine; entries with a witness name it; register prose matches sidecar | that a filed erratum is a real defect (that is the skeptic pass) |
| `generated_docs_in_sync` | committed generated docs match a fresh build | — |
| `fixture_gate`, `no_vacuous_pass` | the suites cannot pass on an empty or fixture input | — |
| `app_selftest` | the app loads the layer and its meta | — |
| COVERAGE GATE (in `build_alignment_layer.py`) | a course marked `complete` has a page for every spine segment; refuses otherwise | whole-segment mispairing at ingest (C13 class) |

Every suite is mutation-verified (ADR-007, `tools/mutate.py`). **None of
them tests correspondence** (RISK_REGISTER R10); see §6.

## 6. Correctness protocol (ADR-012)

Correctness is measured, never gated. The method (`docs/AUDIT_2026-08-29_LAYER_CORRECTNESS.md`):
sample (Tibetan, English, citation) triples from the evidence layer with a
recorded seed and stratum; each triple goes to one auditor WITH its page
siblings and the full source segment, who calls SOUND or DEFECTIVE with a
kind (OVER_CAPTURE, WRONG_MAPPING, UNDER_CAPTURE, INVENTED); every DEFECTIVE
call goes to three skeptics with different lenses (licence, style, evidence)
who default to refuted; only survivors count; the Wilson interval is
recomputed by hand; the sample is banked beside `audit_sample_20260828.json`.
Known limits of the 2026-08-29 audit: C01-weighted (66/100), occurrence-level
sampling, no second reader on SOUND calls. The gate document requires the
next audit to be C03-stratified, headword-sampled, with a second reader on
SOUND calls before C04 opens.

## 7. Production pipeline (per batch of 18 segments = 6 pages)

Propose (two independent analysts per segment, Tibetan-first and
English-first angles, Claude Opus 5) → Reconcile (one agent per segment,
Claude Fable 5.1, writes a spec that exits 0) → Refute (every kept erratum to
a skeptic, Fable 5.1) → Land (assemble three specs, generate, wrap, register
in `COURSES`, rebuild layer and view, run the gates) → Ledger → Commit.
Briefs: `docs/alignment_briefs/`. Model policy: Adam, 2026-09-01.

## 8. Honesty labels

The layer meta says `TENTATIVE (machine-matched, unreviewed)`; every page
carries the PROVISIONAL banner; the view carries it from the meta. No
artefact may claim more than the gates prove. GMR's English stays byte-exact
(Ruling 1); errata on his text are filed for his ruling, never edited
(E-120 class).
