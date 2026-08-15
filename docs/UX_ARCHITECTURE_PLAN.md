# The ALL Translation Tool — Information Architecture & UX Plan
*(Adam's commission, 2026-08-12: "do the tools make sense where they
are… draw up a multi-step plan… this takes precedent." Baseline:
docs/CONTROL_INVENTORY_2026-08-12.txt — 145 controls, 19 panes.)*

## 0. Principles (the measuring sticks)

1. **The translator's minute is the unit of value.** Every layout
   question is settled by: does this shorten the path from
   "I need X" to "X is on screen"?
2. **A pane is a *place of work*, not a drawer.** Live, stateful
   surfaces (reading, typing, aligning) own panes. One-shot actions
   (exports, sweeps, reports) live in tool panes with descriptions —
   the Export/Scans pattern Adam established today.
3. **The viewer follows the work.** Anything that tracks the cursor
   (follow-along scans, evidence ribbon, live guard) stays embedded
   where the cursor lives.
4. **One obvious home per function; the glue is global.** ⌘K, the
   menu bar mirror, and Help search are the *deliberate* duplicates;
   nothing else appears twice.
5. **Progressive disclosure.** First glance = the 3–5 things a new
   user needs. Depth appears on demand (disclosure groups,
   descriptions, empty-state guidance).
6. **Consistency is a feature**: gold eyebrow sections, banner +
   description pattern for tool panes, empty states that teach.

## 1. Findings (what the inventory says)

**Healthy after today's reorgs:** Read = Overlay · Library · Scans ·
Export follows principle 2 cleanly. Learn (Trainer/Drills) and
Community (Propose/Approval) are coherent.

**F1 — Manuscript vs Draft is the biggest open question.** Two
writing surfaces. Draft is really a *workbench* (clauses, evidence
ribbon, structure tools, quotations, terminology) that also carries
publishing tools (RTF export, bibliography composer, footnote
proposal). Manuscript is the *composition* surface (rich text +
corpus sidebar) but lacks the apparatus. A translator asks "where do
I write?" and the answer is currently "both."

**F2 — Naming collision:** Read→"Scans" (woodblocks) vs
Input→"Scan" (OCR). One letter apart, different things.

**F3 — Duplication:** "Sanskrit OCR…" button exists in BOTH the
Sanskrit and Convert panes (inventory lines confirm).

**F4 — Convert is a drawer, not a place**: scripts + Sanskrit card +
Mahāvyutpatti + pronunciation + the entire Tibetan calendar. Each
excellent; the pile violates principle 2. The calendar especially is
a distinct task ("what date is this colophon?") hidden behind
"Convert".

**F5 — Overlay Display panel density:** 10 toggles + 3 combos in one
unlabeled run. They are really three families: *Script* (text
as / typeface), *Card layers* (phonetics, HGM defs, corpus,
Sanskrit, Hopkins, ref dicts), *Document marks* (grammar, Botok,
hints, doubts).

**F6 — Library's top row mixes daily acts with rare maintenance:**
Install/Import/List view are daily; "Legacy font rescue (UTFC)" and
"Send to OCR" and "Update search index" are occasional utilities.

**F7 — Propose carries setup:** identity fields (name, authority,
folder) duplicate Settings; the pane's job is *proposing*.

**F8 — Sparse panes lack self-explanation:** Analysis (1 button),
Review (3), Lookup — fine functionally, but empty states should
teach (the Export/Scans description pattern, applied everywhere).

**F9 — "Teachings for this text…" placement**: a one-shot report
living in Overlay's REVIEW section. Defensible (it acts on the
loaded text) but inconsistent with the Scans/Export precedent.

## 2. Target architecture (proposed)

READ        Overlay · Library · Scans · Export          (as today)
TRANSLATE   Workbench (=Draft) · Manuscript · Review · Align
RESEARCH    Search · Lookup · Sanskrit · Convert · Dates? · Analysis
LEARN       Trainer · Drills
INPUT       Input · OCR (renamed from "Scan")
COMMUNITY   Propose · Approval

With these moves:
- **Translate ladder (F1):** Draft is renamed in speech and banner
  as *the Workbench* — "understand the source, gather evidence."
  Its PUBLISH tools (Export RTF, Compose bibliography, Propose
  footnote) migrate to **Manuscript**, which becomes "write the
  translation, with the apparatus at hand." The path reads
  Workbench → Manuscript → Review → Export. (Alternative if Adam
  prefers: fold Manuscript INTO Draft as a second tab. Gate A.)
- **OCR rename (F2):** Input group = Input · OCR. (Gate B)
- **Sanskrit OCR dedupe (F3):** one home — the Sanskrit pane.
- **Convert split (F4):** Convert keeps script/pron conversions;
  the calendar becomes a labeled "Dates" section with its own
  eyebrow — or its own small pane if Adam wants it findable as
  "Dates". Sanskrit card in Convert stays as quick-answer, deep
  work points to the Sanskrit pane. (Gate C)
- **Overlay Display regrouped (F5):** three labeled sub-groups
  (Script · Card layers · Document marks), collapsed by default
  into the current footprint or a "Display…" popover. (Gate D)
- **Library utilities (F6):** a "Maintenance…" menu-button holding
  UTFC rescue, Send to OCR, Update index; the row keeps
  Install / Import / List view / search.
- **Propose slims (F7):** identity block collapses to one line
  ("filing as Adam (authority) — change in Settings").
- **Teach-everywhere (F8):** every pane gets the banner +
  description + empty-state treatment.
- **Teachings report (F9):** stays on the Overlay (it is about the
  loaded text and one click away from reading) — unless Adam wants
  a "Text reports" tool pane later gathering survey + teachings +
  structure. (Gate E)

## 3. The multi-step plan

- **Phase 0 — Gates.** Adam rules on A–E below. Nothing moves that
  we haven't both agreed on.
- **Phase 1 — Safe wins (no muscle memory broken):** OCR rename +
  Sanskrit-OCR dedupe + banners/empty states everywhere + Library
  Maintenance menu + Propose slim. Selftest/sweep/docs per item.
- **Phase 2 — Overlay Display regroup** (D), with the persisted
  settings preserved key-for-key.
- **Phase 3 — Translate ladder** (A): apparatus migration, banners,
  docs; the campaign checklist re-run on both panes.
- **Phase 4 — Convert/Dates/Sanskrit boundaries** (C).
- **Phase 5 — Design polish:** spacing/typography audit pane by
  pane, consistent section pattern, night-mode pass, fit sweep;
  screenshots refreshed; manual + tutorials truth pass.
- **Phase 6 — The joint walkthrough:** Adam + the machine walk
  every pane against the campaign checklist (T/K/F), and we don't
  call it done until we both feel it.

Each phase ships pressed and selftested before the next begins.

## 4. Gates — consultant rulings EXECUTED and RATIFIED
## (Adam approved all five, 2026-08-12: "I approve your 5
## executed rulings")

Adam converted Phase 0 to consultant authority ("you are the
expert… bring your expertise"); rulings shipped in Phases 1–5 and
ratified. The architecture below is now settled:

- **A. Translate ladder — RULED: ladder.** Draft is the Workbench
  (understand + gather evidence); publish tools live with
  Manuscript. Path reads Workbench → Manuscript → Review → Export.
- **B. OCR rename — RULED: yes.** Input group = Input · OCR; the
  Read group keeps "Scans" for woodblocks. Collision dissolved.
- **C. Calendar — RULED: labeled "Dates" section inside Convert**
  with its own gold eyebrow; promotes to a pane only if usage
  proves it.
- **D. Display regroup — RULED: inline labeled groups** (SCRIPT ·
  CARD LAYERS · DOCUMENT MARKS, gold eyebrows) — fewer clicks than
  a popover, settings preserved key-for-key.
- **E. Text reports pane — RULED: later.** Survey + Teachings stay
  one click from reading on the Overlay; revisit after the joint
  walkthrough if the REVIEW section outgrows itself.

Adam ratified A–E 2026-08-12 (see §4). Remaining for Adam (or overrule any), the Phase 6
joint walkthrough (T/K/F campaign), and the night-mode audit
(needs his live dark-mode screenshots — offscreen renders paint
light chrome).
