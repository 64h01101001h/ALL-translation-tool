# Getting the dictionary to near-100%: what the ACIP method buys, and what it does not

*Written 2026-09-09 in answer to Adam's question: should we run the AI alignment
twice, the way ACIP double-keys a text into its a/b/c/d/e files, and reconcile
the two into an authoritative version?*

**Short answer: the instinct is right, half of it is already built, and the
other half will not work for the reason you would expect. The fix is not "run
it twice" — it is "attack what we have already called correct."**

---

## 1. Why double keying works, exactly

Two operators type the same page. Operator A's typos are **statistically
independent** of operator B's. If each makes an error at rate *p*, the chance
they both err *at the same place, in the same way* is roughly *p²*. At a 0.5%
keying error rate that is 0.0025% — one in forty thousand. So the diff between
the two files catches essentially every error, and a human only has to
adjudicate the places they differ.

The whole method rests on that one word: **independent**.

## 2. Where the analogy holds for us

It holds, and we are already doing it. From `docs/ALIGNMENT_LAYER_SPEC.md` §7,
the production pipeline per batch is:

> Propose (**two independent analysts per segment, Tibetan-first and
> English-first angles**, Claude Opus 5) → Reconcile (one agent per segment,
> **Claude Fable 5.1**, writes a spec that exits 0) → Refute (every kept
> erratum to a skeptic, Fable 5.1) → Land → Ledger → Commit.

That is your a/b/c/d/e in miniature: two passes from deliberately different
angles, a different model reconciling them, and a third adversarial pass. It
was your own ruling of 2026-09-01.

## 3. Where the analogy breaks, and this is the important part

**A typing error is random. An alignment error is systematic.**

A typo comes from a slipped finger, and no two fingers slip alike. An alignment
error comes from a *misunderstanding* — and a misunderstanding is exactly the
thing that repeats. Run the same model over the same segment twice and you do
not get two independent samples; you get the same weights, the same priors and
the same blind spots, twice. Where I am confidently wrong about a particle, I
will be confidently wrong the same way both times, the diff will show
agreement, and that agreement will read as confirmation when it is nothing of
the kind.

**So: two runs of me is close to worthless. Two runs of me and ChatGPT is
genuinely worth something** — different training, different failure modes — but
it is not *p²* either, because we have both read a great deal of the same
Buddhist English and will inherit the same wrong glosses where the internet
carries one.

## 4. The proof that this is not theoretical

We already have the case, and it is in `docs/AUDIT_2026-08-29_LAYER_CORRECTNESS.md`:

> **C13:63–82** — twenty segments that pair Tibetan with the English of an
> entirely different passage. **Five separate mechanical gates were built for
> this class and all five were falsified.**

And on why more alignment passes cannot see it:

> An auditor handed such a segment would validate each span faithfully against
> a source that is wrong, find every licensor exactly where it should be, and
> return SOUND; and any auditor who did flag it would be refuted by all three
> lenses for lack of in-segment evidence.

Twenty passes would have returned SOUND twenty times. The error is not *in* the
alignment; it is in the material the alignment is faithful to.

## 5. What the numbers actually are today

From the same audit, over 100 sampled triples:

- **~4% defect rate**, wide interval, and it is a rate of *intra-segment
  mis-pairing only*.
- It is **C01-weighted** (66 of 100 draws), so it is a Course 1 number with
  Course 3 attached.
- It is **occurrence-level**, not per-record and not per-headword. A translator
  reading the `grub` card sees five renderings of which one is false — 20% of
  that card — and the layer-wide 4% does not describe that at all.
- **Only positives were attacked.** Ninety-one triples were called SOUND by one
  auditor and never challenged. In the audit's own words, *"4% is a floor
  produced by a method built to protect against overstatement."*

That last line is the single most important fact in this document.

## 6. What I recommend, in priority order

**First — attack the SOUND calls. This is the cheapest large win available.**
The current method sends three skeptics to every DEFECTIVE call and none to any
SOUND call. That is a method built to avoid crying wolf, and it succeeded at
that one job while leaving the other direction entirely unmeasured. Re-audit
the existing sample with skeptics on the SOUND calls and we find out whether
the true rate is 4% or 12%. Until that runs, no amount of new alignment
improves a number we cannot see.

**Second — use disagreement to target humans, not to declare truth.** Run a
second proposer, ideally a different model. Where the passes agree, that is
*not* proof — but where they disagree, that is a near-certain flag. Then the
human review is aimed at the 5–15% where the machines fight instead of at
40,000 pages. This is what makes the scale survivable, and it is the honest use
of cross-comparison: a **targeting device**, not a verdict.

**Third — build one check that looks outside the segment.** Every gate we have
is internal to the segment, which is structurally why the C13 offset survived
all five. A column offset is invisible within a segment and obvious across
them: the English stops flowing from one segment to the next. That is a
different kind of check and it is the one class of error that repeated
alignment can never find.

**Fourth — calibrate against a human-ruled gold set.** A few hundred segments
ruled properly by Geshe Michael Roach or a senior translator. Without it, every
cross-comparison produces an artefact whose accuracy nobody can state. With it,
you can finally say a number out loud and defend it.

**Fifth — a full second pass over all 40–50k pages is the *last* thing to buy,
not the first.** It is the most expensive option and it addresses the error
class we are already best at catching, while doing nothing for the class that
has actually bitten us.

## 7. What does not change

The tier. Nothing in this plan makes a single line binding. `TENTATIVE
(machine-matched, unreviewed)` stands until Geshe Michael Roach rules, and the
purpose of everything above is to make his review **cheap and well-aimed**, not
to replace it. Machine work may MATCH his English from corpus evidence; it may
never COMPOSE it, and it may never promote itself.

## 8. So: should we do a second full run?

Not yet, and not the way it was framed. In order:

1. Skeptics on the SOUND calls — measure what we actually have.
2. A second *differently-modelled* proposer on a stratified slice, to learn what
   the disagreement rate looks like and whether disagreement predicts defect.
3. Only if step 2 shows disagreement is a good predictor does a full second
   pass earn its cost — and by then it is a targeting run, not a truth run.

Doing (3) first would cost the most and teach the least.
