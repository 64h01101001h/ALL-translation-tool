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

## Layer 2b — code inventory vs docs vs reality

The code agent enumerated every capability, hook, anchor scheme, and
shortcut across all 21 panes + 12 non-pane surfaces, with a ranked
reachability list. Cross-diff verdicts:

### Healthy (verified, no action)
- The in-app manual (data/help) matches the code closely — the drift
  is in the DISTRIBUTION copies (D1, fixed this audit).
- Both "data-gated" features the inventory flagged are actually fed:
  citation_web.json present (823KB), Das 1902 + Jäschke 1881 PDFs
  installed. Gates would only fire on a fresh install missing data.
- Link-only actions inside HTML reports (insert note/bib/candidate,
  approve/decline, concordance jumps) are the DESIGNED idiom for
  report surfaces — documented in the manual, not defects.
- Right-click-only actions (folio jumps, Files operations, viewer
  adjustments) are all documented in the in-app manual.

### C1. Translator's survey is one broken link from invisible
**FIXED 2026-08-19**: Maintenance menu gains "Translator's survey
(selected text)…", surveying the tree or list selection; polite
message when nothing is selected. The info-panel link stays.

Reachable ONLY through the `survey:` HTML anchor in the Library info
panel — no button, no menu. Same class as the scan-viewer finding.
QUEUED: give it a Library button or menu entry.

### C2. The mirrored menu bar has systematic blind spots
**FIXED 2026-08-19**: the generator now mirrors QToolButtons (menu-
owning ones become submenus) and non-editable QComboBoxes (exclusive
checkable choice submenus, two-way synced). Link-only actions inside
HTML reports remain unmirrored by design — they are content, not
chrome.

It enumerates only QPushButton + QCheckBox children, so it misses
every QToolButton (Files toolbar, viewer toolbar), every QComboBox
mode selector, and all link-only actions. The menu claims to mirror
the GUI; it mirrors about 80% of it. QUEUED: extend the generator
to QToolButtons and combos (as submenus), or soften the claim in
docs.

### C3. Undocumented-in-UI shortcuts (all ARE in the manual)
F3–F8 commander keys, Align Space=Link, viewer ⌘ keys, Hunt arrows,
Overlay arrow-key reading. Manual documents each; the UI itself
hints none. ACCEPTABLE — Help indexes every control — but tooltips
could carry the keys. Low priority, queued in the idea bank.

### C4. Today's features are undocumented in the manual
The newest wave — apparatus Tibetan anchors, ⌘K apparatus lane,
View-menu text sizing, session memory everywhere, the raised
defaults, Palatino enforcement — postdates the manual. QUEUED:
manual chapter updates (the truth pass, D2–D4, same sitting).

## Verdict

**The app is mechanically sound**: 43/43 suites, 172+ selftest
checks green, 21/21 panes sweep coherent with the harness now
actually exercising them. The audit found ZERO functional
regressions. What it found instead: two harness blind spots (fixed),
six documentation defects (two fixed, four queued as one truth
pass), one reachability defect (survey, C1), one honest gap between
the menu-mirror claim and its coverage (C2).

## Layer 3 — Adam's walkthrough (what only your hands can judge)

Do these in one sitting on the installed app; each is a yes/no.

1. **Session restore**: work somewhere real (document + scroll +
   highlight + a scan folder in Input on page 3), quit from the
   menu, relaunch. Everything back? (Input page memory is new
   today.)
2. **Reading defaults**: open a canonical text — is 18pt right for
   long reading? Card comfortable? (View → text size to taste; ⌘0
   returns to the audited defaults.)
3. **Arrow-key feel**: →/←/↓/↑ through a dense passage — the
   reverted stall. If it still catches, we do the joint session
   (you drive, one change at a time).
4. **Scans, live**: Follow along on a BDRC-linked text, folio
   turns, viewer window, Download images, Four-Layer view.
   (Network path — the harness can't prove it.)
5. **Apparatus anchors**: open Translate → Apparatus, find a note
   with a TIBETAN NAMED IN THIS NOTE block — does the candidate
   framing read right? Search "brtan pa" — hyphen-bridge works?
6. **⌘K end to end**: one query → dictionary, corpus, teachings,
   files, apparatus lanes all answering; Enter routes correctly.
7. **Proposal round trip**: file a proposal, rule on it as the
   authority, check the archive records it.
8. **Import data release** (when the next HGM release lands):
   Maintenance → Import — the full chain through spine rebuild.
9. **Pecha PDF**: print one folio side at 300dpi — the type budget
   was measured, your eye confirms it.
10. **Clean-Mac install** (already queued): the DMG on a machine
    that never saw the dev environment.
