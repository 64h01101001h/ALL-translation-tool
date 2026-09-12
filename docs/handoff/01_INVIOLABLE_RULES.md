# The inviolable rules, and the gates that catch you

Four rules live verbatim in `CLAUDE.md`. A fifth governs the alignment layer.
They are not style preferences. Each is enforced by automated gates, and
breaking one fails the build.

---

## Rule 1 — hgm_gloss is binding; everything else is reference

> The dictionary's `hgm_gloss` field carries Geshe Michael Roach's (HGM's)
> English equivalents — the whole point of the project. Hopkins, {C}, {PH},
> {GD}, {LC} material is reference comparanda only and must never be presented
> as, or promoted into, HGM equivalents. **Machine work may MATCH HGM's English
> from corpus evidence, never COMPOSE it.**

### What "MATCH never COMPOSE" forbids, concretely

The machine may select a **contiguous range of English that GMR already wrote**
in the corpus segment being aligned, and attach it to a contiguous range of the
Tibetan in that same segment. That is matching.

Everything else is composing, including all of these:

- writing an equivalent that does not appear verbatim in that segment;
- paraphrasing, normalising or tidying his English;
- stitching two non-contiguous fragments into one exponent;
- importing a gloss from a dictionary, from Hopkins, from Lokesh Chandra, or
  from your own knowledge of Tibetan;
- promoting a machine-derived equivalent into `hgm_gloss`.

**The gate:** the constitution's G2 census counts every site in the app that
touches `hgm_gloss`, pinned at a baseline (currently **42**). Adding one
reference — even in a comment, even a variable name — fails the build until the
baseline is deliberately moved. The point is that no one touches the binding
field by accident.

---

## Rule 2 — never reconstruct canonical engines from spec

> The Python engines are the authority. Port from the file and prove the port
> with its validation battery. A port that fails its battery does not ship.

You may not rewrite a converter from its description, however well you think
you understand Tibetan orthography. Port the file; run the battery.

---

## Rule 3 — never guess

> Failed conversions and parses are flagged and surfaced, not silently
> approximated.

A conversion that fails emits `⟨wylie⟩` markers and the surface says so. A
plausible wrong reading is worse than a visible gap, because a gap is
actionable and a wrong reading is invisible.

---

## Rule 4 — provenance is sacred

> Generated data is tagged (`tibetan_source: generated-ewts-v27-1`,
> `scaffold_added`, `source_wave`, `{LC}`). Preserve these fields through any
> schema work.

---

## Rule 10 — never bank a falsehood, even a verbatim one

From `docs/ALIGNMENT_LAYER_SPEC.md`:

> A span that passes every mechanical check can still be false.

This outranks coverage. It is better to leave a segment unaligned than to bank
a pairing you cannot support. `docs/NEVER_BANK_A_FALSEHOOD.md` carries the
full statement.

---

## The asymmetry you must internalise

**Every gate proves fidelity. Not one proves correspondence.**

The gates prove the text is verbatim, in order, from the cited source, and
honestly labelled. They do **not** prove that a Tibetan↔English pairing is
right. The evidence file's own metadata says so, in the file:

> a wrong pairing passes every one of them (risk R10)

`docs/ADR.md` records that five mechanical correctness gates were built and all
five were falsified. Correctness is **measured by sampled audit**, never gated.

Two audits are on record, and they disagree by roughly five times because they
measure different things. Quote the right one:

| audit | rate | 95% CI | what it sampled |
|---|---|---|---|
| 2026-08-29 | **4.0%** | [1.6%, 9.8%] | occurrence-level, C01-weighted |
| 2026-09-04 (C03 stratified) | **19.0%** | [12.5%, 27.8%] | 100 headword-level pairs, seed 20260904 |

**Passing the suite does not mean your work is correct. It means your work is
honest about what it is.**

---

## The gates, and what trips them

Run `python3 tools/constitution_check.py .` — it must print
`constitution: all rules hold`.

| rule | fails when |
|---|---|
| **R1** | a `QRegularExpression("…")` plain-string literal contains a collapsed escape (`\s`, `\(`, `\d`…). Use `R"(…)"`. Incident: literal parens became capture groups |
| **R2** | the harness flag marker appears other than exactly once in `app/main.cpp` |
| **R3** | the modal-dialog census moves off its baseline (**161**) |
| **R9** | a hand-written 6-digit `background:#RRGGBB` has no `color:` within 300 chars. On a theme-independent ground the ink must be fixed too |
| **G1** | a new saturated green appears in `app/main.cpp`. The green vocabulary is frozen at three named meanings |
| **G2** | the `hgm_gloss` consumer census moves off its baseline (**42**) |
| **G3** | a hardcoded colour appears in any `app/*.inc` stylesheet (must stay at zero) |
| — | PROVISIONAL painted in any red-family colour |

Other standing gates: `tools/repo_license_check.py` (every tracked file
mapped to a licence), the fifteen alignment-layer fidelity suites, and the
mutation traps that prove the gates can actually fail.

---

## Rules of conduct that are not in the code

- **Never send anything from Adam's accounts.** Drafts and calendar holds only.
  No exceptions, ever.
- **Do not name the tooling in outgoing work.** Commits, emails and documents
  are Adam's, authored by `vajra108`, with **no `Co-Authored-By` trailer**.
  *(Counted 2026-09-11: of the 642 commits since the rule was set on
  2026-08-25, **184 carry a trailer** — but they are historical, and the twelve
  most recent commits are all clean. The backlog of 184 is not worth rewriting
  history over; just do not add the 185th.)*
- **Never use, store or enter an API key** Adam pastes, and never handle his
  Apple ID password, App Store Connect keys, or 2FA codes.
- **The Kawachen audio recordings are in-house only.** Gitignored, and they
  must not ship in any distributed build — distribution is not in-house use.
