# Study pane (analysis suite batch 5) — release audit

*Opened 2026-09-09 with the F1 shell. Files: `app/study_pane.inc`; wiring in `app/main.cpp` (Research group, Tools ▸ Study ▸ …, Library ▸ Maintenance ▸ Statistics / Repeated passages for the selected text, Overlay ▸ "Where else does this passage appear?"); substrate `core/include/allcore/textspan.h`, `core/include/allcore/provenance.h`. Pages arrive with their engines: F5 Statistics, F2 Passages, F4 Similarity, F6 Entities; each adds its own rows here.*

## 1. Justification (steelman before test)
A translator with a text open has four questions the Compare suite does not answer: what will this text cost me (which forms recur, which my glossary and the dictionary do not cover, where the once-only readings are); what does it repeat, inside itself and from other texts; which texts on my shelf share material with it; and which people, works and dates it names, spelled the way the catalogue spells them. Each is a counting or matching job, deterministic, with a published definition — and each has been done with a pencil. One pane in the Research group holds the four, under one banner: Study measures, counts and matches; it never composes English and never proposes a reading.

Why one pane and not four (D1): one tab, one ribbon, one ratchet cost, and the page-switcher idiom translators already know from Compare. Why not inside Analysis: that pane is API-key-gated and deterministic counting must not sit behind a key gate. Why nothing renders `hgm_gloss` (D2): coverage columns are booleans plus a tier label; meanings are one click away in Lookup, so the binding-gloss census cannot grow from this batch.

## 2. What the shell does (F1)

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| One conversion at the boundary | `StudyPane::retokenize` | `textspan::toEwtsDocument` then `syllableSpans` under the chosen apparatus policy; the unconverted lines and the syllables refused for editorial marks are counted, listed and shown in a red strip; the scope label states syllables, lines, folio range and the encoding read | selftests (setText, scope label, apparatus switch) |
| Editorial marks control | SCOPE group | Strip the marked span (default) · Keep the letters inside the marks · Leave the text as it stands — re-runs the page and restamps provenance | selftest (refusal strip changes) |
| Text… / Use the front document / Refresh | STUDY group | File through `cmp::readText` (binary refused); the front document carries the Overlay's encoding; Use the front document is disabled with the tooltip "no document is open" when nothing is open | selftests |
| Save report… / Copy | REPORT group | Report opens with `provenanceBlock`; routed by the selected filter name; returns false with a status line under the harness | selftests |
| Entry points | Tools ▸ Study (four items, no accelerators — D7); Library Maintenance (two items); Overlay context menu on a selection | Hooks `g_studyText`, `g_studyPassage`, `g_studySay`, `StudyPane::g_studyFrontDoc` | wiring |
| Four pages | switcher | Each page states what it is and that its engine is not in this build yet; no number is a placeholder | selftests (four pages render) |

## 3. Honesty
- Banner on every page: measures, counts, matches; never composes English or proposes a reading.
- MACHINE badge on the status line and every page head.
- Refusals are never silent: the strip names the lines that could not be converted and the syllables refused, with the switch that would include them.
- No `hgm_gloss` anywhere in the pane (structural: the G2 census now covers `app/*.inc`).

## 4. Gate work recorded with this shell
- The binding-gloss census (G2) reads `app/main.cpp` and every `app/*.inc`; baseline 33 unchanged; mutation check: a planted `hgm_gloss` in a pane file fails the gate (33 → 34), reverted. Adam's ruling on moving the gate is requested (plan §11 Q8).

## 5. Open
- CLI flags `--stats` / `--passages` ship with their engines (F5 / F2), not with the shell — a flag that prints a stub is a dead control.
- Design-critique and accessibility findings: see `docs/FINDINGS.md` (Study pane, 2026-09-09).
