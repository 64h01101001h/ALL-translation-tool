# Where we stopped — paused 2026-09-22, 09:30 MDT

Adam: *"Pause everything and make note of where we stopped."* Everything below
is state, not plan. Nothing is running: the background alignment workflow was
stopped mid-agent and the self-paced loop is ended.

## Stopped cleanly

- **Course 5 alignment: 366 of 511 segments landed.** Bank at **50,370
  attested links across 578 pages**. Last page landed: `c5p122` (C05:364-366).
- All page gates green at the pause: no supplied span heads, no broken words,
  no degenerate members, no split syllables, builder sees every span, layer
  matches spine, evidence matches spine, view matches layer.
- Errata register at **298 entries** (202 document errata, 7 HIGH).
- Gate audit `docs/GATE_AUDIT_2026-09-17.md`: **13 of 17 findings closed**,
  each with a Disposition line. The four still open are named in its Counts
  section.

## Stopped MID-WORK — read this before resuming

**C05:367, 368 and 369 are reconciled but NOT landed.** Their specs are in the
session scratchpad at
`/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05/`
as `36{7,8,9}-reconciled.json`. They were held back because a page is three
segments and 369 is not trustworthy — see below.

**C05:369 IS NOT VERIFIED AND MUST BE RE-RUN.** The workflow that produced it
reported `[reconcile:369] failed: You've hit your weekly limit`. A
`369-reconciled.json` and `369-body.html` exist on disk anyway — the agent got
far enough to write them before it died — but `369-errata.json`, which every
successful reconcile in this campaign writes, is **absent**. A deliverable
whose producer reported failure is not a deliverable. Do not land it. Re-run
the reconcile for 369 and compare.

The scratchpad is session-local and will not survive indefinitely. If those
three specs matter, the cheap recovery is to re-run 367-369 from scratch
rather than to hunt for the files.

## Five requests from Adam, 2026-09-22 — NOW FILED, see below for where

They arrived in conversation and existed nowhere durable until this file;
Adam then asked for them to be put where they belong. They are filed, and
**this section is no longer the record** — the three source lists are, and
`docs/RELEASE_PUNCHLIST.md` regenerates from them.

| | request | filed to | punchlist section |
|---|---|---|---|
| 1 | title-translation protocol | `TODO.md` §AA | code |
| 2 | hover-for-English read-along | `TODO.md` §AA | code |
| 3 | folio -> English page estimator | `TODO.md` §AA | code |
| 4 | Himalayan Art Resources | `docs/BACKLOG.md` | waiting on someone outside |
| 5 | Jev AI | `docs/BACKLOG.md` | waiting on Adam |

None has been designed, scoped or costed. Each entry carries the
measurement that says how far off it is, so the next reader does not have
to guess — and the two in the backlog carry the reason they are gated
rather than merely deferred.

1. **A title-translation tutorial / walkthrough / protocol** — ornamental
   versus descriptive titles, and which comes first.
2. **A Tibetan/English hypertexted, highlighted read-along for all of Geshe
   Michael's translations** — hover a word, see its English equivalent. (This
   is what the alignment campaign has been building the data for: 50,370
   attested links and rising.)
3. **A Tibetan folio -> English page and volume predictor/estimator.**
4. **Bring the Himalayan Art Resources website into the project** —
   licensing and permission are the first question, not the last.
5. **Whether "Jev AI" can complete the dictionary more time- and
   cost-efficiently.** NOTE: I do not know what Jev AI is and did not guess.
   Adam to identify it before anyone assesses it.

## What was NOT done, deliberately

- **Nothing was pushed to GitHub at the pause.** The landing of C05:361-366 is
  committed locally only.
- The corrected `data/help/` prose ships in the DMG payload, so Adam's
  installed copy still shows the old wording. Release work is paused; no press
  was run.
