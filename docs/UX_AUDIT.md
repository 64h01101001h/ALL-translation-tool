# UX AUDIT — ALL Translation Tool
*Reconnaissance pass per the Master UX/UI Oversight Prompt
(2026-08-20). No GUI was modified in producing this document.
Status: awaiting second-reviewer challenge before implementation.*

---

## A. Architecture discovered

**Framework.** Qt 6 Widgets (C++20), one executable; nearly all UI
lives in `app/main.cpp` (~31,600 lines). Domain logic is cleanly
separated into the `allcore` static library (engines, spine access,
catalog tools — 59 ctest suites); the UI layer only calls allcore.
OCR lives in `allocr` (ONNX Runtime). No QML, no web views.

**Shell (as of today's ribbon work).** `QMainWindow` → central
column: Quick Access row (★ pinnable command strip) → `QTabWidget`
with SEVEN group tabs (Read · Translate · Research · Learn · Input ·
Catalog · Community). Each group page = {ribbon band, inner
QTabWidget with its tab bar hidden}. The band = SURFACE switcher
(big checkable icon-over-label buttons per pane) + the active pane's
tool groups (RibbonBar/RibbonGroup/RibbonProxy). 8 of 22 panes have
tool ribbons (Overlay, Catalog, OCR, Library, Manuscript, Input,
Export, Draft); query-form panes (Search/Lookup/Convert/Sanskrit)
intentionally show surface-only bands. Menu bar: Edit (standard
roles) + a generically-derived mirror of every pane's controls +
View + Help. ⌘K Hunt palette (files/entries/corpus/persons/
apparatus/teachings/commands) + a visible corner button.

**Styling.** No central tokens. ~40 `setStyleSheet` calls plus
hundreds of inline styles inside HTML strings: 12+ recurring hex
colors (top: #B26B00 ×30, #B4540A ×27, #9A7A33 ×27, #8A8A8A ×27),
8 ad-hoc font sizes (11px ×52, 12px ×51 … 20px ×7). A cream
reading-surface rule (#FAF6EE) applies app-wide; dark mode is the OS
palette plus selected overrides (a prior night-mode audit exists;
a full pass is a known open TODO gated on live dark screenshots).

**State & persistence.** `sess::` namespace over QSettings (~17 key
families): active pane (by NAME), per-pane files/folders, display
toggles, splitter-adjacent state, QAT pins, ribbon-labels toggle.
Harness runs (`--selftest/--sweep/--screenshots`) are guarded from
reading/writing real session state.

**Testing culture (a real strength).** `app_selftest` = 200+ checks
including RENDERED-OUTPUT pins (the card must *show* the layer, not
merely load it); a sweep harness clicks every enabled control and
types probe strings; a fit pin fails any pane demanding >800px;
screenshot mode renders every pane to PNG. 59 ctest suites total.

**Reusable primitives that already exist.** RibbonBar/Group/Proxy;
`entryHtml` (the shared dictionary-card renderer used by Lookup,
Overlay, palette); `zoneLabel` (gold eyebrow); `miniIcon` (drawn,
license-clean, now 28 kinds with semantic color families);
CatalogTree; HuntPalette; `sess::remember` bindings; safe file
dialogs with per-caption memory.

**Keyboard.** ⌘K; arrow-key phrase walking in the Overlay (←→ walk,
↑↓ climb the nest); ⌘+/−/0 zoom (Overlay); standard edit shortcuts;
everything else mouse-first. No pane-switching keys, no
next/previous-unit keys outside the Overlay.

**Duplication worth noting.** Repeated eyebrow-label HTML snippets;
per-pane info-panel `QTextBrowser`s each with hand-rolled hint text;
anchor-scheme handling re-implemented per pane (`openfile:`,
`gloss:`, `catopen:`, `jae:`, `w:` …); two generations of toolbar
code now coexist (ribbon panes vs untouched rows in Files/Scans/
Apparatus/Review/Align/Trainer/Drills/Propose/Approval).

---

## B. Design-system inventory (what exists de facto)

- **Palette in practice:** paper #F2EAD9/#FAF6EE · card #F8F2E5 ·
  ink #2B2620 · gold eyebrow #9A7A33 · green (act/success) #1E6B4E,
  #3B7A3B, #2E7D32 (three greens!) · vermilion/amber (warn)
  #B4540A, #B26B00 (two ambers) · muted greys #8A8A8A/#6E675D/
  #9C948A · plum (people) #6E3E8E · blue (documents) #2E629E.
- **Type in practice:** Palatino/serif for content; system sans for
  eyebrows/labels; Tibetan via bundled OFL faces; sizes ad hoc
  (10–22px) with meaning by convention, not by role.
- **Spacing:** per-widget margins hand-set; no scale.
- **Iconography:** miniIcon fleet, 2.6px strokes, five semantic
  color families with soft washes (documents blue · actions green ·
  review/search gold · people/access plum · machines vermilion).
- **States:** hover/checked mostly Qt defaults; selection clarity
  varies; disabled = dimmed (was the visibility complaint's core).

---

## C. Current strengths — do not redesign these away

1. **Provenance culture is already in the pixels.** Tier labels
   (curated/glossary/PROVISIONAL), "reference only, never HGM",
   "OCR-DERIVED, review material", "machine-located candidates,
   not a ruling", ⟨flagged⟩ never guessed. This epistemic honesty
   is the app's soul; the audit's job is to make it *systematic*,
   never to dilute it.
2. **Rendered-truth testing.** The pin culture (assert the CARD
   shows it) catches what screenshots and compiles cannot. Any UX
   change inherits this safety net.
3. **Information density with real content.** No lorem anywhere;
   every pane earns its ink. The density is the product.
4. **⌘K Hunt + the menu mirror.** Command discovery already has two
   complete systems (palette + auto-derived menus); the QAT adds a
   third. Governance, not invention, is needed here.
5. **Session continuity.** Pane, files, toggles, and positions
   survive relaunches; harness runs can't clobber them.

---

## D. UX problem register (ranked)

### Critical
- **C1 · No design tokens.** 12+ hex colors and 8 font sizes are
  hand-scattered; three different greens and two ambers carry the
  same meanings. Impact: inconsistent contrast (the gray-icons
  complaint was a symptom), night-mode fragility, every future
  change re-rolls the dice. Fix: central token set (bg/surface/
  text/muted/accent/warn/error/success/link/selection + type roles)
  applied incrementally. Risk: low per step, wide surface.
- **C2 · Epistemic status is labeled but not visually systematic.**
  HGM binding material, reference dictionaries, machine candidates,
  and (soon) AI suggestions render at near-equal visual weight;
  the reader must read the labels to know the authority. Fix: a
  SourceBadge/visual grammar with 4–5 epistemic classes (binding ·
  primary evidence · reference authority · machine-located ·
  AI-labeled) applied across cards, ribbons of evidence, and
  dialogs. This is §48–§51 made concrete — the highest-value single
  idea in the framework for THIS app.

### High
- **H1 · Entry-card hierarchy.** Headword, glosses, evidence, and
  metadata compete; corpus shows 3 with no count ("3 of 41" board
  candidate already designed — 9j, board acd57e2b). Fix per 9j pick
  + C2 badges. (Adam's own directive; board awaits his letter.)
- **H2 · Overlay left column fragmentation.** 14 card-layer + 3
  document-mark checkboxes as one flat blue-noise list; Display
  grouping (framework §14) needs group headers, all/none, presets
  ("reading" / "research" / "minimal").
- **H3 · Ribbon governance.** Groups/icons were assigned by
  expedience, not a frequency×importance pass; some labels elide
  ("Typography c…sical rules"); checked-state on ribbon toggles is
  faint; 9 panes still carry legacy rows. Fix: per-pane command
  matrix, then finish rollout with intentional grouping.
- **H4 · Status/feedback inconsistency.** OCR is now async with
  Stop, but several long operations still block with
  processEvents (batch OCR, some corpus scans) and there is no
  global status surface (the mockups' status bar: release, signed
  identity, pending approvals, selftest state).
- **H5 · Link behavior is many things.** Anchors variously open
  panes, dialogs, external sites, insert text, or mutate state,
  with one visual style. Fix: link classes (internal-nav /
  dialog / external / action) with a consistent cue (e.g. external
  arrow, action underline style).

### Medium
- **M1 · Pane/splitter affordances.** 18 splitters, thin grips, no
  maximize/collapse/reset-layout, no keyboard pane focus/switching.
- **M2 · Typography roles.** Mixed-script sizing by eye; define
  roles (doc-tibetan, doc-translit, card-headword, card-gloss,
  metadata, eyebrow, status) and map the existing sizes onto them.
- **M3 · Empty states.** Some teach (Overlay info panel), others
  are bare (several results panes); adopt the teach-the-interface
  rule everywhere.
- **M4 · Control states.** Selected/checked emphasis (esp. ribbon
  toggles like "locate word", surface buttons) needs a visible
  treatment beyond platform default.
- **M5 · Keyboard-first gaps.** No shortcuts for pane switching,
  next/prev clause outside Overlay, copy-Tibetan/copy-translit,
  toggle-scan; palette lists commands but doesn't teach shortcuts.

### Low
- **L1 · Spacing scale** (adopt 4/8/12/16/24 and stop hand-tuning).
- **L2 · Scrollbar/overflow consistency** across QTextBrowsers.
- **L3 · Tooltip discipline** (many are excellent essays; some
  controls have none; add shortcut lines where they exist).
- **L4 · Dialog conventions** (button order, titles, ellipsis use).

### Polish
- **P1 · Ribbon visual refinement** (Word-like hover/checked tints,
  group caption weight, separator subtlety).
- **P2 · High-DPI icon crispness** audit.
- **P3 · Long-label truncation rules** (mid-label ellipsis reads
  badly; prefer end-truncate + tooltip).

---

## E. Top 10 highest-impact improvements

| # | Problem | Proposal | Benefit | Difficulty | Risk |
|---|---------|----------|---------|-----------|------|
| 1 | C1 tokens | Central palette+type tokens; migrate by area | Consistency, night-mode sanity | M | L |
| 2 | C2 epistemic grammar | SourceBadge classes across all evidence | Scholarly trust made visible | M | L–M |
| 3 | H1 card | Implement Adam's 9j pick w/ counts-first sections | The most-used surface, daily | M | M |
| 4 | H2 display presets | Grouped toggles + reading/research presets | Kills blue noise, faster setup | S–M | L |
| 5 | H3 ribbon matrix | Frequency×importance per pane; finish rollout | Coherent workflow story | M | L |
| 6 | H4 status bar + async sweep | Global status surface; de-block remaining ops | Trust during long ops | M | M |
| 7 | M4 states | Strong selected/checked treatment | "Is this on?" never asked | S | L |
| 8 | H5 link classes | Visual grammar for link behavior | Predictability | S–M | L |
| 9 | M5 keyboard | Pane-switch + copy shortcuts + palette hints | Expert speed | S–M | L |
| 10 | M1 pane affordances | Wider grips, maximize/reset, saved layouts | Long-session comfort | M | M |

## F. Proposed design direction

**"A scholar's instrument."** Keep the manuscript identity (paper/
ink/gold) as the token base; typographic hierarchy carries structure
(serif content, sans utility, tokenized roles for mixed scripts);
semantic color only (the five icon families extend into text/state
color); the epistemic badge grammar as the signature move — a reader
should *see* authority levels before reading a word. Compact, not
cramped: current densities kept, spacing regularized to a scale.
The ribbon stays (Adam's ruling) and matures via the command matrix;
a quiet status bar carries standing facts. Nothing that reads
"SaaS": no cards-for-everything, no oversized chrome, no icon-only
mystery controls.

## G. Implementation roadmap (small, reversible phases)

1. **Tokens & type roles** (C1+M2): introduce the token table +
   named constants; migrate entryHtml and the ribbon first; no
   layout changes; pins untouched. *Each later phase spends tokens.*
2. **Epistemic grammar** (C2): SourceBadge component + apply to the
   entry card and Draft evidence ribbon; wording of existing
   honesty labels preserved verbatim.
3. **Card hierarchy** (H1): implement the 9j letter Adam picks,
   with counts-first evidence sections and see-all into the
   concordance.
4. Display grouping + presets (H2) · 5. Ribbon matrix + finish
   rollout (H3) · 6. Status bar + async sweep (H4) · 7. States,
   links, keyboard, panes (M4/H5/M5/M1) · 8. Polish pass.

## H. Explicitly not to be changed

Honesty/provenance wording (rule 1/3 labels) · domain notation
(ACIP, EWTS, tier names, BDRC/toh identifiers, folio grammar) ·
information density · engine outputs and flags · the pin/sweep
culture · arrow-key reading · ⌘K · session persistence semantics ·
the charter texts in Catalog/Propose.

---

*Register maintenance: findings above carry IDs (C/H/M/L/P). The
UX decision log starts with: (1) ribbon = primary navigation (Adam's
ruling, screenshots, 2026-08-20); (2) query-form panes keep their
forms in-pane (Word semantics — forms are not commands); (3) color
icon families with washes (visibility ruling, 2026-08-20).*

### Decision log (continued)
- 2026-08-20 · **Review pane** joins the band (REVIEW group: source · draft · run). 59/59 green.
- 2026-08-20 · **Scans stays a launcher page.** Its four actions carry epistemic
  descriptions ("candidates only, never claimed complete") that the band's
  icon-over-short-label grammar cannot hold. Ruling: launcher pages whose button
  descriptions carry honesty wording keep the vertical layout. (§53 pushback.)
- 2026-08-20 · **Trainer and Drills stay compact rows.** Both are settings-dense
  learning forms (mode/script/course selectors interleaved with actions); the
  query-surface ruling applies.
- 2026-08-20 · **Files pane joins the band** (VIEW / GO / TRANSFER / TOOLS).
  Two new grammar pieces proved here: RibbonGroup::addColumn() — Word's
  stacked small-control idiom, for checkbox/pushbutton pairs that would
  otherwise blow the band's width — and RibbonProxy now carries QToolButton
  menus (Favorites/Workspaces). The quick-select field returned to the pane
  body per the query-surface ruling. Screenshot-verified before commit.
- 2026-08-20 · **Phase 3 ledger complete**: 84000 TM, both teaching tiers,
  and the reference zone now show true "N of M" totals; no cap poses as
  the whole anywhere on the card.
- 2026-08-20 · **M3 empty states**: the terminal lookup miss now names every
  fold it tried (exact · affix · ba/bo · verb-stem · pronunciation ·
  colloquial) and points at the layers below without promising them; the
  search zero-hit state offers the pane's REAL widening moves (combiner,
  NEAR window, folder scope, case) — first drafted with a typed-grammar
  description, corrected against the actual combiner control before commit.
