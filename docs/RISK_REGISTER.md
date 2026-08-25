# Risk register

*Diamond Cutter Translation Tool · opened 2026-08-25 for the P2 gap
"No risk register". One row per risk that could cost the project
something it cannot get back. Reviewed when the list below says, or
when a press is cut, whichever comes first.*

**How to read it.** Likelihood and impact are judgements, stated so
they can be argued with. "Owner" is who can actually move it — several
are not me, and saying so is the point of writing them down. A risk
with no owner is a wish.

---

## Live risks

### R1 · The project exists in two places, both in this room
**Likelihood** low per year · **Impact** total · **Owner** Adam + ALL

Until 2026-08-23 there was **no off-machine copy at all** — no remote,
and Time Machine's last reference to its external volume predated the
project's first commit. There is now a verified mirror plus a nightly
job, but the mirror sits on a drive beside the laptop. Fire, theft, or
a bad `rm` takes both.

*Reduced by:* `tools/backup.sh`, nightly at 22:00, verified by
comparing HEADs rather than exit codes.
*Closes when:* the source lives somewhere neither this laptop nor this
room controls. That is Joel's "source hosting" question, and `.git` is
now 181 MB rather than 10 GB, so hosting it is trivial.
**Review: at the next conversation with ALL.**

### R2 · Bus factor is one
**Likelihood** certain over a long enough horizon · **Impact** severe ·
**Owner** ALL

Nobody but Adam has built this. `docs/DEVELOPER_ONBOARDING.md` was
*proved* on a clean clone on 2026-08-25 and three defects in it were
fixed, so a competent stranger can now get to a green
`ctest -LE fixture`. They still cannot run 33 of 85 suites without
untracked fixtures, and four of those fixtures **have no generator
anywhere in the repository**.

*Reduced by:* the onboarding doc, now tested rather than asserted.
*Closes when:* somebody who is not Adam builds and presses this from
scratch.
**Review: when a second developer exists.**

### R3 · The product can lose a translator's work while reporting success
**Likelihood** moderate · **Impact** severe · **Owner** Adam

The defect family this project has found most often: a store returning
TRUE over a write that never landed. FAIL-1 had three of them; FAIL-2
had rulings. All are ENOSPC-shaped, which means **disk pressure is a
correctness risk, not an inconvenience** — and the volume went from
14 GiB to 327 MiB in one working day on 2026-08-24.

*Reduced by:* stores flush-and-close before judging, pinned by
`storeflush_smoke`; `tools/scratch_gc.py` plus both nightly tasks
refusing to work below 5 GiB.
*Open:* FAIL-1's recommendation names **seven other**
`QTextStream`-over-`QFile` sites that have not had the same treatment.
**Review: next SQA pass.**

### R4 · Payload ships whose terms are not settled
**Likelihood** present today · **Impact** institutional · **Owner** ALL

The SBOM counts **24 of 115 components with OPEN terms** (4 gated, 20
itemised) — the HGM spine, the teaching index, SOAS POS and others.
BUILD-5 was the version of this that already happened: 1,926 THL
records shipping while the notices said they were not.

*Reduced by:* `PAYLOAD_MANIFEST.txt` gates the press and a market press
refuses to build while terms are unresolved; the SBOM counts it every
press instead of leaving it to an audit.
*Open:* the 24 need per-row answers, and a **team DMG carries
`reference.db`** — nothing technical prevents someone forwarding it.
**Review: before any market press.**

### R5 · The build cannot be verified by its recipients
**Likelihood** present today · **Impact** moderate · **Owner** ALL

Ad-hoc signed, `TeamIdentifier=not set`, **not notarised**. A recipient
cannot distinguish our build from a modified one, and the printed
first-launch instruction was the pre-Sequoia bypass until BUILD-3.

*Open:* waits on an ALL-owned Developer ID. The armed notarisation path
also uses `--deep` with a hardened runtime and no entitlements, which
will fail on a QML bundle — budget an afternoon, not a flag.
**Review: when a Developer ID exists.**

### R6 · A green suite is weaker evidence than it looks
**Likelihood** demonstrated · **Impact** moderate · **Owner** Adam

TEST-1 re-injected 21 defects and **13 survived the full battery**.
Worse, the 2026-08-22 baseline *mis-measured* three stores — it probed
at 600 rows, above the stream buffer, the one regime where the broken
shape works — recorded them honest, and the remediation therefore
skipped them. **The measurement manufactured the pass.**

*Reduced by:* `tools/mutate.py` and a committed mutation set, currently
**5 of 6** with the sixth explained as equivalent-by-invariant; a
liveness control that voids the run if it cannot go red.
*Open:* six mutations is a small sample of a large codebase.
**Review: monthly, via `--sweep`.**

### R7 · One file is 39,581 lines
**Likelihood** certain to worsen · **Impact** moderate · **Owner** Adam

`app/main.cpp` is 39,581 lines, over half the codebase, and it
grew by 133 lines today alone - which is the risk stating itself. It is why patches miss their
anchors, why two remediation tracks had to be warned off colliding in
it, and why review is hard.

*Plan:* staged extraction of self-contained panes, one per press, each
proven by the existing battery. Not a rewrite.
**Review: each press.**

### R8 · Untrusted input reaches C++ parsers
**Likelihood** moderate · **Impact** moderate · **Owner** Adam

OCR decodes arbitrary images through OpenCV, libpng, libtiff, freetype
and jasper. MEM-1 proved it is not hypothetical: a ~10 KB heap overread
whose bytes were appended to OCR output **as recognised Tibetan**.

*Reduced by:* MEM-1 closed; `tools/vuln_check.py --triage-all` reduces
302 raw advisories to **16 needing judgement across 9 components**.
*Open:* those 16, and libpng's reachability is genuinely unresolved —
it is reached only transitively through Qt and OpenCV, and nobody has
traced whether the vulnerable push-mode parser is ever entered. Our own
engines have never been fuzzed against hostile ACIP or `.dic` input.
**Review: next SQA pass.**

### R9 · A user hitting a defect has nowhere to send it
**Likelihood** certain at an input centre · **Impact** low but
compounding · **Owner** Adam + ALL

There is no defect-intake path and **no crash handler at all** — no
`qInstallMessageHandler`, no `std::set_terminate`, no signal handler.
An abort leaves nothing behind.

*Reduced by:* Help → Save Diagnostic Report gives a user something to
attach, with redaction pinned.
*Open:* nowhere to send it, and nothing captures the moment of failure.
**Review: before any input-centre deployment.**

---

## Closed, kept for the record

| | Closed |
|---|---|
| No off-machine backup ever | 2026-08-23 |
| `.git` at 10 GB; ENOSPC broke a press | 2026-08-24, history rewrite → 186 MB |
| Fan-out scratch filling the volume | 2026-08-24, `scratch_gc.py` with pinned safety |
| Two SQA criticals (PERF-1, FAIL-1) | 2026-08-24 |
| No LICENCE, no SBOM, no vuln scan, no threat model, no privacy statement | 2026-08-24 |
| Documented clean-checkout gate was red | 2026-08-25 |

---

## What this register does not cover

Anything requiring ALL's decision is recorded as *owner: ALL* rather
than solved: the licence ALL releases under, the Developer ID, source
hosting, the defect-intake alias, and whether CI gets stood up. Naming
the owner is the only honest thing a register can do about a risk it
cannot move.
