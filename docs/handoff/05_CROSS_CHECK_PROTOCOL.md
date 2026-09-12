# The cross-check protocol

**Adam, 2026-09-11:** *"I would like the models to stay as closely aligned on
their tasks. However, I would like the error-checking capabilities enabled by
each model. So I need to have you checking each other's work."*

Two models work this project in alternation — Claude and ChatGPT (Astra) —
because Adam pays for both and swaps whenever one budget runs low. The swap is
the risk. Each handover is a chance to introduce noise into the signal: a
figure repeated without checking, a rule half-remembered, a decision re-litigated
because the reason for it was not written down.

This file is how that is prevented. It is short on purpose.

---

## The three rules

### 1. The incoming model's FIRST task is to verify, not to build

Before writing a line of new work, open `HANDOFF_LOG.md`, read the most recent
entry, and check its **Claims to verify** list. That list is short and specific
by construction — the outgoing model is required to name what it did not
verify, and anything it asserted that a reader would act on.

Verify each one against the repository. Then write your verdict into the log:
`CONFIRMED`, `CORRECTED` (with what it actually is), or `COULD NOT CHECK` (with
why). Only then start new work.

**Why this way round.** The natural instinct on picking up a project is to
trust the handover note and get moving; that is exactly how a wrong number
survives five sessions. A wrong figure costs an hour to catch on the day it is
written and a release to catch later.

### 2. Nothing is asserted without its evidence beside it

Every factual claim in the handoff files and in the log carries **how it was
established** — a file path, a line number, a command and its output, or a
count. A claim with no evidence is written as `UNVERIFIED:` in front of it.

This is not bureaucracy. It is the only thing that lets the other model check
you cheaply: `UNVERIFIED: roughly 3,000 recordings` takes ten seconds to
resolve; "we have about three thousand recordings" takes twenty minutes to
trace back to whoever said it first.

### 3. A disagreement is recorded, never silently resolved

If you find the other model's work wrong, do not simply fix it and move on.
Append an **ERRATA** entry to `HANDOFF_LOG.md` saying what was claimed, what is
true, and how you established it.

Two reasons. Adam needs to see the error rate between models to know how much
to trust the arrangement at all. And a silent correction that is itself wrong
is undetectable — the record is what makes a second correction possible.

If you *think* the other model is wrong but cannot establish it, say that, and
leave the work alone.

---

## What the outgoing model must write

Before handing back, append an entry to `HANDOFF_LOG.md` with these five
headings, in this order. Nothing else is required and nothing may be omitted.

```
## <date> — <which model> — <one-line subject>

### Done
What actually changed, with paths. Only finished work.

### Verified
What you checked and how — the command, the count, the file:line.

### Claims to verify
Everything you asserted but did NOT verify, and anything the next model
would reasonably act on without re-checking. Be generous here: this list
being long is not an embarrassment, it is the mechanism working.

### In flight
Work started and not finished, and precisely where it stops. If nothing
is in flight, write "nothing".

### Do not
Things the next model would plausibly try that are already ruled out,
with the reason. This is the anti-noise section.
```

---

## What neither model may do

These hold regardless of which model is at the keyboard. They are not style
preferences; each one exists because breaking it has cost this project
something.

- **Never compose an English equivalent.** The machine may MATCH Geshe Michael
  Roach's English from corpus evidence; it may never invent it. This is the
  project's first rule and the reason it exists at all. See
  `01_INVIOLABLE_RULES.md`.
- **Never mark work complete that has not been run.** "Should pass" is not
  "passes". The gates exist to be run.
- **Never repeat a number you have not checked.** Several figures in this
  project's own documents have been wrong, and each was propagated by a
  careful reader who assumed the previous one had checked.
- **Never re-litigate a decision recorded in `HANDOFF_LOG.md` or `docs/ADR.md`**
  without new evidence. If the reason is not written down, ask Adam rather
  than deciding again.
- **Never send anything from Adam's accounts.** Drafts and calendar holds only,
  always, without exception.
- **Never guess at Tibetan.** A failed conversion is flagged (`⟨wylie⟩`), never
  silently approximated. A plausible wrong reading is worse than a visible gap.

---

## A note on tone between the models

Check the work, not the worker. "The count at `data/alignment/C05_CAMPAIGN.md`
line 40 says 165; I counted 167 files — here is the command" is useful.
"The previous session was sloppy" is not, and it makes the next handover worse.
