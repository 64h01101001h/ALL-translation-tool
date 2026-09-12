# Handoff — read this first

You are picking up work on the **Diamond Cutter Translation Tool** (the Asian
Legacy Library's Tibetan translation suite) from another AI model. Adam Derick
Andrade alternates between Claude and ChatGPT as each budget runs down, so this
folder exists to make the swap cost nothing.

**Read in this order. It is about twenty minutes and it is not optional — the
whole point of this folder is that you do not have to guess, and guessing here
is expensive.**

| | file | what it gives you |
|---|---|---|
| 1 | `00_ORIENTATION.md` | what the project is and why it is built the way it is |
| 2 | `01_INVIOLABLE_RULES.md` | the rules that cannot be broken, and the gates that catch you |
| 3 | `02_THE_TASK.md` | the work being handed over, precisely scoped |
| 4 | `03_HOW_TO_RUN.md` | the commands, verified, with the traps named |
| 5 | `04_STATE.md` | exactly where things stand right now |
| 6 | `05_CROSS_CHECK_PROTOCOL.md` | how you and the other model check each other |
| 7 | `HANDOFF_LOG.md` | the baton. Read the last entry; it names what to verify first |

---

## The one-paragraph version

This repository builds a desktop application (C++20 + Qt 6) and an iOS
companion for reading, translating and teaching classical Tibetan. Its
distinguishing asset is **42,199 aligned Tibetan–English segments carrying one
translator's consistent English** — Geshe Michael Roach's — plus a word-level
alignment layer that records, span by span, which Tibetan words his English
came from. The project's first rule follows from that asset: **the machine may
MATCH his English from evidence; it may never COMPOSE it.** Almost every design
decision in the codebase is downstream of that sentence.

## The five things that will bite you

Stated up front because each has already cost this project time.

1. **Every number in this project's own documents is suspect until you check
   it.** Not carelessness — drift. Several figures were wrong for weeks and
   were propagated by careful readers who assumed the previous one had
   checked. This folder marks unverified claims as `UNVERIFIED:`. Do the same.
2. **No gate tests correctness.** Roughly twenty-three automated gates prove
   *fidelity* — the text is verbatim, in order, from the cited source, honestly
   labelled. Not one proves that a Tibetan↔English pairing is *right*. The
   layer's own metadata says so. Passing the suite does not mean the work is
   correct; it means the work is honest about what it is.
3. **"Unattested" means three different things here** and conflating them will
   send you down the wrong road. `02_THE_TASK.md` separates them.
4. **The tests need data that is not in the repository.** A fresh clone cannot
   run most of the suite. `03_HOW_TO_RUN.md` explains what is missing and why.
5. **Nothing is ever sent from Adam's accounts.** Drafts and calendar holds
   only. This is absolute.

## What to do in your first fifteen minutes

1. Read this folder in the order above.
2. Open `HANDOFF_LOG.md`, find the newest entry, and work its
   **Claims to verify** list. Record `CONFIRMED` / `CORRECTED` / `COULD NOT
   CHECK` for each.
3. Only then start new work.

If something here contradicts what you find in the code, **the code wins** —
and say so in the log, so the next model does not trip over the same line.
