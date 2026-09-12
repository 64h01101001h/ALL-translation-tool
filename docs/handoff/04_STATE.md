# Where things stand

**Snapshot taken 2026-09-11 evening. Everything here was checked, not
remembered. Re-check anything you are about to act on — this file ages.**

---

## Repository

| | |
|---|---|
| branch | `main` · HEAD **4778e12** · 1,505 commits |
| version | **1.0.0-rc.1** |
| remotes | `origin` → github.com/64h01101001h/ALL-translation-tool (private) · `backup` → an 8 TB local mirror |
| tests | **115 suites, 100% passing**, 312.81 s (verified 2026-09-11) |
| constitution | all rules hold |

The working tree is **dirty** at handoff time (`TODO.md` modified,
`docs/handoff/` untracked) — that is this handoff being written.

## What the project is doing right now

Adam **paused the alignment campaign on 2026-09-08** and redirected everything
at shipping a public 1.0. That is the governing context: the scanning work you
are picking up is *resuming a paused campaign*, not continuing an active one.

`docs/RELEASE_PUNCHLIST.md` is the working list — generated from a survey of
every unchecked item in `TODO.md`, `docs/BACKLOG.md` and
`docs/OPEN_TASKS_2026-09-10.md`, classified by what actually blocks each.

| | count |
|---|---|
| open items | **191** |
| unblocked (no one else needed) | 85 |
| need Adam's ruling | 47 |
| need Adam to email or introduce someone | 43 |
| waiting on an outside party | 16 |
| **release-blocking** | **17** — 5 engineering, 12 Adam's |

The twelve release blockers that are Adam's are mostly single decisions: the
code LICENCE, the Qt Multimedia/FFmpeg licensing question, the diagnostic
report's privacy profile, and a pane-by-pane SHIP/HIDE/FIX walk. **None can be
resolved by a model.** Do not attempt them; do not wait on them either — there
are 85 items that need nobody.

## Landed on 2026-09-11 (the most recent session)

So you do not redo it or contradict it:

- **The Learn tab plan is complete** — all twelve tools built, eleven of them on
  the phone. The twelfth ("What you can read next") is **deliberately** not on
  the phone: it divides a word deck by each work's full vocabulary, and the
  phone has no surface that can grow such a deck, so it would report every text
  as a fraction of one percent readable.
- **Boundary Hunt was marked BUILT in the plan and did not exist.** Found by
  auditing the plan's claims against the code. Built, and the false mark left
  in place with a note. *Treat this as the house habit: check claims against the
  program, not against the document that makes them.*
- **The menu bar went from 19 top-level menus to 11.**
- **`progress.db` is now schema-versioned** — the one file holding work a user
  cannot get back. Old records upgrade with every row intact; a record from a
  newer release is refused rather than written to. `docs/DATA_MIGRATION.md` is
  the user-facing promise.
- **Right-click → Pronounce** now works on Tibetan and ACIP selections.
- **Constitution rule R9** added (a background with no text colour); it found
  eight more instances of the same defect immediately.
- **`allcore::placeBlank`** — blank placement had been implemented twice and had
  drifted; now one function, with the packer counting what it refuses.

## The data assets

| | |
|---|---|
| dictionary entries | **105,634** (12,004 glossed · 93,630 not) |
| aligned corpus segments | **42,199** across 75 course/text codes |
| alignment layer coverage | **1,547 segments**, C01–C05 only — **3.7%** of the corpus |
| layer links | 41,759, of which **11,777 (28.2%)** have no English exponent |
| evidence headwords | **6,756** / 14,528 pair records |
| AI glossary drafts | **1,395** of 88,369 eligible — a stalled pilot |

**The layer's tier is `TENTATIVE (machine-matched, unreviewed)`.** Nothing in it
is binding. Nothing in it has been promoted into `hgm_gloss`, and nothing may be
without a human ruling.

## Known-stale things that will mislead you

Four were found on 2026-09-11 and **fixed the same day** — recorded here
because the habit of checking is the point, not the four instances:

- ~~`README.md` and `docs/DEVELOPER_ONBOARDING.md` said "86 suites"~~ → 115.
- ~~`docs/FIXTURES.md` said "37 of 73"~~ → 51 of 115.
- ~~`docs/DEVELOPER_ONBOARDING.md` pointed at `cmake-build`~~ → the scripts use
  `cmake-build-release`; the stale tree still had 93 tests registered.

**Still stale, not fixed:**

- `data/alignment/C01_CAMPAIGN.md`'s header table still lists closed sections as
  "open". Left alone deliberately: campaign ledgers are append-only work
  records, and rewriting one would falsify the history it exists to hold.
- Four test fixtures have **no generator in this repository** and exist on one
  laptop only (`docs/FIXTURES.md` admits this).

## Standing orders in force

- **Desktop first, then the phone, before moving on.** Every change lands on the
  desktop trainer and then on iOS in the same session, so neither holds an
  update the other lacks. When a change genuinely should not go to the phone,
  **say why** — Adam asked to be told each time.
- **C03..C18 in order, three segments per batch**, resuming at C05:166.
- **Do not scan C13** until the upstream column offset is fixed.
- **Nothing is sent from Adam's accounts.** Drafts and holds only.
- **The eighteen-course scan is meant to become the main GMR dictionary**
  eventually (`docs/DICTIONARY_STRATEGY.md`), but `hgm_gloss` stays binding
  until Adam rules otherwise.

## Newest item on the backlog

Added 2026-09-11 at the top of `TODO.md`: **frequency-ordered vocabulary.**
Process all of GMR's known translations, count word frequency *at each alignment
depth*, and produce a learner's priority list (top 2,000, then 4,000). Adam's
reasoning is the specification and is quoted there in his own words. Not started.
