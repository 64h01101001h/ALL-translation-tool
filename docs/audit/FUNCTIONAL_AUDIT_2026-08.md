# TOTAL FUNCTIONAL AUDIT — 2026-08-18

*Adam's directive: "we're feature rich at this point... application
wide audit of all functionality in all workflows and all panes.
Nothing should slip through the cracks." Method: (1) mechanical
battery, (2) code-vs-docs inventory diff, (3) Adam's hands-on
checklist. This document is the running record.*

## Layer 1 — mechanical battery: GREEN, with two harness findings

| battery | result |
|---|---|
| ctest suites | **43/43 pass** |
| in-app selftest | **ALL PASS (0 failures)** |
| sanskritcheck | 0 missing-glyph cases across 2 fonts |
| pane sweeps | **21/21 coherent** (Overlay 31 controls · Search 19 · Draft 14 · Input 14 · Files 13 · Manuscript 13 · Library 8 · Trainer 8 · Drills 8 · OCR 6 · Align 6 · Propose 6 · Approval 6 · Scans 4 · Export 4 · Lookup 4 · Review 3 · Sanskrit 3 · Apparatus 2 · Convert 2 · Analysis 2) |

**Findings about the HARNESS itself (both fixed same day):**
1. Analysis swept "coherent" having exercised ZERO controls — its
   only button enables on pasted text. A pass that touches nothing
   proves nothing.
2. Convert exercised one control — its converters are
   QPlainTextEdits, a widget type the sweep never probed.

Fix: the sweep now primes every enabled, editable text box with an
ACIP probe before walking controls (commit 70b9690). Convert's
paste-to-convert wiring is now genuinely exercised. Analysis's
remaining skip is honest and printed: Analyze is gated on
ANTHROPIC_API_KEY **by design** (the API-credits item on Adam's
list); its wiring (both text boxes) probes clean.

## Layer 2a — what the documentation claims (docs sweep verdicts)

Full claim inventory captured by the audit agent; the DEFECTS:

### D1. The shipped manual is the stale one
`docs/distribution/USER_MANUAL.md` (and its .docx) has diverged ~310
lines behind `data/help/USER_MANUAL.md` — the in-app copy documents
Files, verse tools, citations, StarDict, OCR models, weak spots,
pecha booklet, TISE keys, Jäschke links; the distribution copy lacks
all of it. ACTION: regenerate distribution copy from the in-app one.

### D2. Pane-count contradictions everywhere
CFG says "seventeen in all" AND "Twenty panes" in the same document;
WP and UM say seventeen while UM documents an 18th (Files) plus
Scans and Export. Reality: **21 sweepable panes**. ACTION: one truth
pass over the four documents' structure sections.

### D3. Features documented in their old homes
Export/format tools documented under Overlay (they moved to Read →
Export); scan one-shots under Overlay (moved to Scans); PUBLISH
tools under Draft (moved to Manuscript); Library maintenance items
listed as top-row buttons (now under Maintenance…). Pane-name drift:
"Input → Scan" vs "Input → OCR". ACTION: same truth pass.

### D4. Count drift (docs vs docs, and docs vs reality)
- Test suites: docs say 36 or 38 — reality **43**.
- Selftest checks: docs say 89 or 96 — reality **170+**.
- "text as" modes: CFG says 3 — reality 5 (both pronunciations).
- Display toggles: "nine" prose vs 11 tabled.
- Proposal kinds: 8 vs 9 vs 10 across docs.
- Typeface picker: "thirteen faces" (CFG) vs 6+system (UM).
ACTION: truth pass with live numbers; consider generating counts
into docs from the batteries rather than hand-writing them.

### D5. "Only two network features" claim is stale
BDRC scans, collection updates, teaching links, link-outs, and the
Files pane's remote connections all reach the network. The honest
claim is "fully offline for all reading/translation; network only
for scans, updates, link-outs, remote files, and the two AI
features." ACTION: reword in README + WP + CFG.

### D6. UM self-contradiction
Files chapter footer says tags/workspaces/sync/remotes are "still
scheduled" while the same chapter documents them shipped. ACTION:
delete the stale footer.

### Aspirational items correctly documented as such (no action)
WP §8: input-center formatter, Das re-download, author letters,
Windows/Linux builds, cataloging set-aside. These match the master
board's gated items.

## Layer 2b — code inventory cross-diff
(pending — agent running)

## Layer 3 — Adam's hands (pending assembly)
