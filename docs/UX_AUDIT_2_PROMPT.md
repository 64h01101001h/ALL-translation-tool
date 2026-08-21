# MASTER UX/UI OVERSIGHT PROMPT — PHASE 2: THE LIVED-EXPERIENCE AUDIT
*(successor to the 64-section Master UX/UI Oversight Prompt; supersedes nothing —
all Phase-1 rulings, the §53 pushback authority, and the §63 standing governance
carry forward verbatim. Phase 1 audited what the screen SAYS. Phase 2 audits what
the tool is like to USE.)*

## PART I — MANDATE AND STANCE

**§1. Role.** You remain the senior UX architect, but your method changes: Phase 1
was an expert review of rendered surfaces; Phase 2 is a field investigation of the
tool in motion. Every claim in this phase must be verified by *operating* the tool
— clicking through the flow, resizing the window, killing the network, deleting the
data file — not by reading the code that draws it. Code reconnaissance tells you
what was intended; only operation tells you what is lived.

**§2. The Phase-1 inheritance.** The design register (ux:: tokens, epistemic
badges, counts-first ledgers, the ribbon band grammar, honest elision, designed
empty states) is now settled law. Phase 2 never relitigates it — it stress-tests
it. A Phase-1 pattern that fails under Phase-2 conditions (a badge unreadable in
dark chrome, a band that collapses at laptop width) is a finding, not a reversal.

**§3. The blindspot list is the syllabus.** Phase 1 had specific, now-identified
blindspots. Each is a numbered workstream below. Do not add generic best-practice
filler; every workstream exists because the first audit demonstrably could not see
it.

**§4. Verification discipline, upgraded.** Phase 1's standard was
screenshot-the-installed-binary. Phase 2 adds: (a) *flow scripts* — a written
click-by-click path, executed against the running app, with a screenshot at each
state transition; (b) *sabotage tests* — deliberately break a precondition
(no network, missing index, empty folder, locked file) and capture what the user
sees; (c) *dimension sweeps* — the same pane at 1000px, 1400px, and full width.
A finding without its reproduction script is a rumor.

**§5. Honesty rules are load-bearing law.** Rule 1 (match, never compose), rule 3
(never guess), provenance wording verbatim, reference-vs-HGM separation — as
always. Any Phase-2 change that would soften an honesty surface is REJECTED at
finding stage, before implementation.

## PART II — THE TEN WORKSTREAMS (each: investigate → findings with repro →
ruling or fix → verify → log in docs/UX_AUDIT_2.md → commit; batteries green at
every commit; announced presses only)

**W1. Cold start and first contact.** Quit the app, simulate the new-cataloger
machine: no QSettings, no built caches, no session state (use a scratch
QSettings scope or a temp HOME — never destroy Adam's real state). What greets a
brand-new user? Is there a first-run path that says what this tool IS, where data
comes from, and what to click first — or a wall of 22 panes? Audit: the no-release
state (spine missing), the no-teaching-index state, the no-catalog-login state.
Deliverable: a designed first-run experience proposal (mock in the app or as an
artifact) — proposal first, Adam rules before it ships.

**W2. Failure-path UX (the sibling of Phase 1's empty states).** Empty states are
"nothing matched"; failure states are "something broke." Inventory every external
dependency (BDRC scans, 84000 links, Dropbox official root, FTP/SFTP connections,
OCR models, teaching index, release imports) and sabotage each: network down,
file missing, folder unwritable, malformed data. Every failure must tell the user
three things: what broke, whether their work is safe, and the one next action.
Silent failures and console-only errors are P0 findings.

**W3. Flow friction — the top ten journeys.** Write the ten highest-value user
journeys as flow scripts (e.g. "look up a term from a pasted passage → read card
→ open a teaching link"; "stage a catalog action → approve as authority →
verify Dropbox propagation"; "import a new release"; "OCR a scan → correct →
export"). Execute each. Count: clicks, pane-switches, modal interruptions, points
where the user must remember something the screen no longer shows, and points
where work could be lost. Every journey gets a friction score and its worst step
gets a finding.

**W4. Keyboard and traversal integrity.** Phase 1 added shortcuts; Phase 2 audits
the whole keyboard story: tab order in every pane (does focus walk logically?),
Escape/Enter behavior in every dialog, whether every band action is reachable
without the mouse, shortcut collisions across panes, and a printed shortcut map
(→ Help). Note but do not yet port: ⌘-based bindings must have a defined
Ctrl-mapping story for the future Windows/Linux input-center builds.

**W5. Accessibility beyond the focus ring.** (a) Measure contrast ratios of every
token pair actually in use (including the gold-on-dark chrome and the muted
grays) against WCAG AA; fix failures within the palette's spirit. (b) Color-blind
safety: the Approve-green/Decline-red pair and the five epistemic badge colors
must survive deuteranopia simulation — color must never be the ONLY carrier
(glyphs ✓/✗/⏸ and badge text already help; verify every case). (c) Hit targets:
nothing interactive under 24px. (d) VoiceOver: at minimum, accessible names on
the band's proxies and the card's links.

**W6. Tibetan script as first-class typography.** The generic type scale is not a
Tibetan type policy. Audit: stacked-glyph clipping at every zoom level (line
height on multi-stack Sanskrit transliterations like sat+t+wa), the minimum size
at which the bundled font's stacks remain legible (rule, then enforce as the
floor for kFsTibetan), tsheg-wrapping behavior in narrow cards, and mixed
Tibetan/Latin baseline alignment in evidence rows. Verify shaping with the
existing --sanskritcheck battery plus a rendered-pin sweep at zoom extremes.

**W7. Theme coherence.** The chrome is dark; the card is paper. Rule on the
intent (a deliberate "manuscript in a dark reading room" contrast is a legitimate
design), then audit every surface against the ruling: hard-coded hex colors that
assume the wrong ground, unreadable combinations in whichever macOS appearance
Adam does NOT run, and the settings to commit: follow-system vs fixed. Whatever
is ruled, every color must resolve readably in both macOS appearances.

**W8. Consistency and language.** Build the terminology table from the actual UI
strings: every user-facing name for the same concept (wylie/EWTS, master/HGM/
the authority, corpus/evidence, press/release/package...). One concept, one word,
everywhere — with the honesty vocabulary (provisional, candidate, reference,
machine) frozen verbatim. Same audit for interaction idioms: confirmation style,
progress style, Stop buttons, dialog button order, title-case vs sentence-case.

**W9. State, undo, and the cost of a mistake.** Inventory every destructive or
hard-to-reverse action (file ops in Files, catalog stage/approve, register
rulings, trash, sync). For each: is there confirm-with-specifics (names the
thing), undo, or staged-preview? Then the persistence audit: list what survives
relaunch (zoom, toggles, session, presets, logins) and what silently resets;
anything a user re-does every session is a finding. Approval rulings and ledger
entries must be verified append-only.

**W10. Measurement — ending government-by-guess.** Phase 1's H3 ordering was
judgment. Build the minimal, local, privacy-respecting usage ledger: an opt-in,
plain-TSV counter of pane raises and band-action clicks, stored beside the
session file, viewable in-app (Help → Usage), never transmitted anywhere.
After a week of Adam's real use, the frequency×importance matrix gets re-derived
from data and the bands re-ordered where the data contradicts the guess. This
also feeds the test campaign and the future manual's "start here" ordering.

## PART III — PROCESS UPGRADES (fixing how Phase 1 worked, not just what it saw)

**§10. The adversarial second reviewer, for real this time.** Phase 1 let the
implementer self-review. In Phase 2, after each workstream's findings are drafted,
spawn an independent review agent whose brief is to REFUTE: reproduce each finding
from its script alone, and attack each proposed fix for regressions against the
Phase-1 register and the honesty rules. Findings that fail reproduction are
downgraded to PLAUSIBLE and do not ship fixes. Log both verdicts.

**§11. Visual regression, systematized.** The --screenshots sweep exists; make it
a gate. Bank a blessed screenshot set per pane; each press compares current
against blessed (perceptual diff, not byte-diff) and fails loudly on unexplained
change. Blessing a new baseline is an explicit, logged act.

**§12. Findings triage protocol.** Every finding carries: ID (W2-03 style),
severity (P0 data-loss/dishonesty · P1 blocks-a-journey · P2 friction · P3
polish), repro script, affected honesty rule (or "none"), and proposed fix with
cost. P0s interrupt the loop and ship immediately; P1s batch per workstream;
P2/P3 accumulate into a final polish press. Adam's explicit verdicts are only
required for: new surfaces (W1's first-run), anything touching honesty wording,
and anything that would retrain his muscle memory (band reorder from W10).

**§13. Cadence.** /loop-driven, self-paced. Order: W2 → W9 → W1 → W3 → W5 →
W7 → W6 → W4 → W8 → W10-instrument (the W10 re-derivation waits for real usage
data), with §11 built immediately after W2. Announced batch presses; batteries
green at every commit; memory banked at every workstream close; docs/UX_AUDIT_2.md
is the single deliverable ledger, closing with a scoreboard like Phase 1's.

**§14. Exit criteria.** Phase 2 is complete when: all ten workstreams show every
finding fixed, ruled, or explicitly deferred-with-reason; the visual-regression
gate is in the press; the usage ledger is live and opt-in; and a final
"walkthrough letter" for Adam summarizes what changed in plain language, keyed to
the journeys of W3 — so the human the tool serves can verify the lived
improvement himself, journey by journey.
