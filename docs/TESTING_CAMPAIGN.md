# Release Testing Campaign — the working checklist
*Adam's directive, 2026-08-11: pane by pane, workflow by workflow,
EVERYTHING. Each feature gets THREE verdicts:*
- **[T]** Tested — works as described (or a finding is filed below)
- **[K]** Keep / Pare — does this information earn its place?
- **[F]** Format — is the kept information presented right?

*Protocol: test in the order below; report findings as you hit them
(screenshots welcome); fixes land same-day with a selftest guard;
presses batch per pane. Curation rulings get recorded here and
applied. Findings log at the bottom.*


---

# READ


## Overlay

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Loading text — two ways
- [ ] T · [ ] K · [ ] F — Reading the display
- [ ] T · [ ] K · [ ] F — The card
- [ ] T · [ ] K · [ ] F — Display panel (left)
- [ ] T · [ ] K · [ ] F — REVIEW (left column, under its gold heading)
- [ ] T · [ ] K · [ ] F — SCANS (left column, under its gold heading)
- [ ] T · [ ] K · [ ] F — FORMAT & EXPORT (left column, under its gold heading)

## Library

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# TRANSLATE


## Manuscript

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Draft

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use
- [ ] T · [ ] K · [ ] F — STRUCTURE tools (under the gold heading)
- [ ] T · [ ] K · [ ] F — EVIDENCE tools
- [ ] T · [ ] K · [ ] F — PUBLISH tools

## Review

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Align

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# RESEARCH


## Search (Gofer)

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Lookup

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Sanskrit

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Convert

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Analysis

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# LEARN


## Trainer

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Drills

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# INPUT


## Input

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Scan

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# COMMUNITY


## Propose

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

## Approval

- [ ] T · [ ] K · [ ] F — What it's for
- [ ] T · [ ] K · [ ] F — Use

---

# APP-WIDE


## Settings

- [ ] T · [ ] K · [ ] F — Appearance
- [ ] T · [ ] K · [ ] F — Reading
- [ ] T · [ ] K · [ ] F — Team & proposals
- [ ] T · [ ] K · [ ] F — Data

---

# WORKFLOWS


## Suggested Workflows

- [ ] T · [ ] K · [ ] F — Where do I start?
- [ ] T · [ ] K · [ ] F — Read a text (the daily bread)
- [ ] T · [ ] K · [ ] F — Translate a text end to end
- [ ] T · [ ] K · [ ] F — Check someone else's finished draft
- [ ] T · [ ] K · [ ] F — Study session (a student's hour)
- [ ] T · [ ] K · [ ] F — Hunt a term across everything
- [ ] T · [ ] K · [ ] F — Input-center day (ACE reborn)
- [ ] T · [ ] K · [ ] F — Team proposal cycle
- [ ] T · [ ] K · [ ] F — Sanskrit deep-dive


---

# Cross-pane workflows (from Suggested Workflows — walk each recipe
end to end exactly as written)

- [ ] Read a text (the daily bread)
- [ ] Translate a text end to end
- [ ] Check someone else's finished draft
- [ ] Study session (a student's hour)
- [ ] Hunt a term across everything
- [ ] Input-center day (ACE reborn)
- [ ] Team proposal cycle
- [ ] Sanskrit deep-dive

# App-wide

- [ ] Menu bar mirrors every pane (spot-check each group menu)
- [ ] Help window: search 10 random features, verify locations
- [ ] Night mode + system dark/light combinations
- [ ] Window resizing on every pane (small window, big window)
- [ ] Fresh-install experience from the DMG on a clean account

---

# FINDINGS LOG
*(date · pane · what happened · status)*

- 2026-08-11 · Overlay · rapid clicks line-selected via Qt
  triple-click — FIXED (dd68ef4, guard check 85)
- 2026-08-11 · Overlay · control column crushed in short windows —
  FIXED (8569dd7, scroll area)
- 2026-08-11 · Draft · same crush class found by the new fit sweep
  (823px demand) — FIXED (scroll area); permanent guard: every pane
  must fit an 800px window (selftest check 86)
- 2026-08-11 · Library · recently-opened links dead in the installed
  copy — QUrl percent-encoded the spaces in '/Applications/ALL
  Translation Tool/…'; ALL text-carrying anchors (openfile, gloss,
  propose, Help chapters) now decode payloads — FIXED (guard 87)
- 2026-08-12 · Overlay · display toggles rebuilt the whole document
  and wiped the card (slow + card disappears) — card-only toggles
  now re-render the card in place; doc-affecting ones (grammar,
  hints) rebuild then restore the card — FIXED (guard 92)
- 2026-08-12 · Library · recents STILL dead for Tibetan filenames —
  apostrophes (DPA'I) terminated single-quoted href attributes; all
  link payloads now percent-encoded at construction — FIXED (guard 94)
- 2026-08-12 · Overlay · session restore added: same file, same
  scroll, same cursor on relaunch (guard 93)
- 2026-08-12 · Research/Search · makeSearchPane (Spotlight "Search
  this Mac" + Library-index search) is UNWIRED — GoferPane ships as
  the Search tab. Not a bug, but those two features are unreachable.
  CURATION DECISION NEEDED: merge the two buttons into GoferPane, or
  drop them. Marked [[maybe_unused]] meanwhile.
  → RESOLVED 2026-08-12 (merge, per the recommendation Adam waved
  on): Spotlight is now an opt-in source ROW in GoferPane's own
  target list ("This Mac (Spotlight)"), each term probed as typed +
  as Tibetan unicode, results clickable; the Library-index button
  was already redundant (GoferPane's library folder row uses the
  index). The superseded makeSearchPane deleted (234 lines).
  Selftest guards the row; manual + tutorials corrected to match.
- 2026-08-12 · Overlay · MECHANIZED CLICK-THROUGH (Adam's kickoff:
  "run tests with each one of the buttons/fields on the overlay
  page"): new `--sweep <Pane>` harness loads a real passage
  offscreen, clicks every enabled button, toggles every checkbox on
  and back, cycles every combo, auto-dismissing and recording any
  dialog. Overlay result: 26 controls exercised, 0 crashes, all
  empty-state edges graceful (folio nav w/o volume, OCR locate w/o
  follow-along, galleries w/o cache all no-op politely with
  guidance dialogs). Sweep is repeatable per pane; heavier panes
  need per-button stall guards before sweeping (Library's index
  rebuild would block minutes).

# CURATION RULINGS
*(pane · what stays / what pares / format decision)*

(none yet — first pass pending)
