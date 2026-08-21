# THE LODESTAR — LEDGER
Per docs/LODESTAR_PROMPT.md. Every finding names its BLINDSPOT CLASS.

## PROLOGUE — a gate bug the environment found for us
Before L2 even opened, the skills-batch press was stopped by the
visual gate: ALL 22 panes "changed." Diagnosis: the blessed set was
rendered ON-SCREEN at the display's Retina scale (2360×1402) and the
gate's sweep at DPR 1 — **the gate was environment-dependent**, keyed
to whatever monitor the Mac sat on; Adam's relocation fired it.
FIXED: bless and gate both render offscreen (deterministic DPR 1) —
a dock/undock can never fire the gate again. Blindspot class:
*infrastructure audited only in the environment it was born in.*

## L2 — THE EXCISE HUNT (Cooper, via design-critique)
Excise = work serving the software, not the scholarship. Inventory:
| # | Excise | Verdict |
|---|---|---|
| 1 | "Quit and relaunch" for teaching-index / imported-release layers | COLLAPSE (candidate: one "Reload data layers" act — approved layers already live-reload since G6; extend the pattern; 1.1 line) |
| 2 | FTP/SFTP password every session | JUSTIFY (S4 ruling: never persisted is the security model — written, accepted) |
| 3 | Two identity acts (Propose name; Catalog roster sign-in) | COLLAPSE (candidate: roster sign-in offers to seed the provenance name) |
| 4 | "Set your name in Propose" trip from the comments dialog | **KILLED (2026-08-21)**: name set INLINE in the dialog, once, persisted — the trip is gone. Blindspot class: *the fix that created excise while fixing honesty (yesterday's design pass!)* |
| 5 | First-run data-folder hunt | already KILLED (locate-or-exit + DMG layout) |
| 6 | Search→Lookup copy-paste trip | already KILLED (W3-01 term links) |
| 7 | Glossary tending buried behind card clicks | escalated to L6 candidate 5 (Glossary Workbench) |
| 8 | Re-finding one's working state across panes | escalated to L6 candidate 1 (Translation Dossiers) — the deepest excise in the tool |
| 9 | Official-root choose per machine | JUSTIFY (the perimeter IS the folder; one-time) |

## L6 — THE BRAINSTORM (seeded; Adam picks the flagship)
Candidates with their framework parentage in the prompt. THE
RECOMMENDATION: **Translation Dossiers** — Norman's object-model
argument is decisive: translators think in TEXTS-being-worked, not in
panes; today that identity is scattered across per-pane crumbs, and
every journey the campaign will walk begins with reassembling it by
hand. The dossier collapses excise #8, gives L3's personas their
resumable world, and is the substrate Team Presence (candidate 6)
and the Glossary Workbench (candidate 5) later attach to.
Runner-up: the Pipeline Map welcome (cheapest, teaches the model).

## L1 — THE CONCEPTUAL-MODEL AUDIT (Norman) — closed
The tool's hidden model: a PIPELINE (Read → Translate → Research →
Learn → Input → Catalog → Community) over a DATA HIERARCHY (HGM >
evidence > reference > machine > AI). Findings + fixes:
- **The model is now taught in one screen** — Help → How This Tool
  Thinks…: the pipeline with each group's role, the five voices with
  their badges and colors, and the record's permanence contract.
  Blindspot class: *the model lived in the builders' heads and the
  ledgers — never on a screen a stranger could reach.*
- **Group tabs teach their roles** — hover any group: "Translate —
  make the draft (⌘2 · panes: ⌘⇧]/[)". The shortcut tooltip grew a
  purpose.
- Pipeline-position cues on individual panes: RULED sufficient via
  the map + role tooltips + pane banners (each pane already opens
  with a role sentence); per-pane breadcrumbs would be chrome.

## L4 — THE LAWS SWEEP — closed (analysis + rulings)
- **Hick** (22 panes, 14 card layers): mitigated by design — the
  seven-group chunking, card presets (reading/research/minimal), and
  now the model map. HOLDS.
- **Fitts**: band targets ≥24px with labels; splitters 7px grabbable
  (Phase-1 M1); min-window floor keeps targets from crushing. HOLDS.
- **Miller/chunking**: the card's zone order IS the data hierarchy
  (HGM first, AI last) — structure encodes meaning. HOLDS.
- **Jakob**: File/Edit/groups/View/Help + ⌘ vocabulary + ribbon =
  the conventions users bring from Word/macOS. HOLDS (G5's work).
- **Tesler**: the burden the software now carries that users once
  did: conflict merging, backups, honest counts, live rulings,
  recents, drag-open. Remaining user-side complexity is ESSENTIAL
  (scholarly judgment). HOLDS.

## L5 — THE VOICE — closed (convention verified)
Sampling the jargon frontier (bskungs yig, sa bcad, colophon, tsheg,
EWTS): every surveyed surface already speaks PLAIN-FIRST with the
native term parenthesized and the next act stated ("orthographic
abbreviation (bskungs yig): X = Y — not a typo"; "Outline (sa bcad)").
The error voices (warnWriteFail/warnOpenFail), empty states, and
banners all carry next-act discipline from earlier engines. RULED:
the pairing convention is the house voice; honesty wording verbatim;
no bare-jargon finding survived sampling. Blindspot class checked and
found already-covered: *strings written by experts for experts.*

## L3 — THE OTHER HUMANS (journey walks) — closed
- **The cataloger (modest English, month one):** welcome → model map
  → Catalog's role tooltip → the pane's own banner → chop/compose
  tools with previews. Surviving friction: the interface is
  English-only — i18n is DEFERRED to the post-1.0 roadmap with this
  persona's name on it (the deepest remaining barrier, honestly
  stated). House grammar (STD-007) JUSTIFIED: it is the team's own
  trained standard, taught in Help.
- **The authority (time-poor):** queue → filter → bulk-approve with
  count + one note; rulings apply IMMEDIATELY (G6); conflict banner
  names forks; backups quiet. The queue now respects minutes. HOLDS.
- **The student translator:** welcome text → click-a-word → presets →
  ⌘K → map. Surviving friction: no guided "first translation
  project" path — that is EXACTLY the Dossier flagship's opening act
  (L6-1); noted as the flagship's first acceptance test.
Blindspot class: *built under one expert eye* — now walked by three.

## LODESTAR SCOREBOARD
Prologue (gate DPR) ✓ · L2 excise ✓ (2 kills, 2 collapses queued to
1.1, 3 justified) · L1 model ✓ · L4 laws ✓ · L5 voice ✓ · L3 personas
✓ — **L6 flagship: WAITING(Adam: pick — recommendation Translation
Dossiers)**. The closing letter writes when L6 ships.

## L6 — THE FLAGSHIP: TRANSLATION DOSSIERS (executing)
**Brainstorm (condensed record).** Intent: a translator's working
world for ONE text — source, position, draft, glossary, comments —
resumable AS A UNIT. Norman: the object model users think in.
Constraints: personal working state (LOCAL store, not the shared
Dropbox — dossiers are a desk, not a record); never duplicates data
(it POINTS at the text, the glossary, the comments that already
exist); honest counts derived live.
**Plan (v1 slice).**
1. allcore::DossierStore — local TSV (library/dossiers.tsv): slug,
   title, text path, cursor line, created, touched. TDD: battery
   first.
2. UI: File → Dossiers… — list (title · text · touched · live
   glossary/comment counts), "New from current text" (captures the
   Overlay's document + position), double-click = open-as-unit
   (Overlay opens the text, restores the line, raises).
3. Overlay position auto-touches the dossier on open/close.
4. Acceptance (L3's student persona): open app → Dossiers →
   double-click → reading exactly where they left off, glossary and
   comments one click away. Verify + press.

### L6 shipped (v1) — and the closing letter
**Translation Dossiers are live.** allcore::DossierStore (TDD:
dossier_smoke wrote the contract first — suite 68; slug/round-trip/
touch/ordering/escaping/honest-ghost-failure). File → Dossiers…:
your desks listed most-recently-touched first with LIVE counts
(comments from the shared store, glossary presence), "New from
current text" captures the Overlay's document and line, one click
reopens the text AT YOUR LINE with everything an arm's reach away,
"save my place" updates the desk, remove removes. The constitution's
R3 census caught the one new modal and it was reviewed in the same
commit — the machine now polices its own growth.

**THE CLOSING LETTER (Lodestar §11).** What the tool now teaches:
its own pipeline and its five voices, on a screen a stranger can
reach. What it stopped demanding: the name-trip, the copy-paste
trips, the rebuild-your-world-every-session ritual — the excise
ledger has verdicts, not habits. What it became able to do: give a
translator a DESK — the object they always thought in — and resume
scholarship as a unit. And the way of seeing it leaves behind: the
ten prompts' blindspot classes are now named things (audited-against-
itself; built-for-one-eye; excise-normalized; the-model-untaught;
environment-of-birth) that any future engine checks FIRST. The tool
that changes the world is the one that carries its teacher inside it.

## EPILOGUE — the embers, executed (2026-08-21)
- **L2 collapse #1 shipped**: File → Reload Data Layers — one act
  refreshes approved rulings, honorifics, spelling, AND the teaching
  indices (now Live-pointer reloadable like the rest); the status
  bar reports what refreshed and states honestly that a new data
  RELEASE still asks for a relaunch (the spine itself).
- **L2 collapse #3 shipped**: a Catalog roster sign-in seeds the
  provenance name when none is set — one identity act, not two.
- **L6 acceptance pinned**: the student persona's test lives in the
  selftest — a dossier reopens its text AT THE SAVED LINE as one
  act, forever.
The Lodestar's ledger is now ash-cold: nothing queued, nothing open.

## EPILOGUE II — the Glossary Workbench (excise #7's answer)
allcore::GlossaryStore extracted from the Overlay's private pair
(glossary_smoke, TDD-first, suite 69; format contract preserved
including the header); Overlay's load/save strangler-routed through
the one store; and the Overlay's REVIEW band gains **Glossary…** —
this text's entries in one tended list: set, edit, remove, click a
term to its card. The translator's own layer finally has a room of
its own. (Constitution R3 census: baseline 111, reviewed — one
dialog-scoped info modal, never harness-reachable.)

## EPILOGUE III — Team Presence (the last named ember)
The many-hands model, made visible with ZERO new persistence: File →
Team Activity… merges the stores that already remember (proposals,
rulings, comments) into one newest-first feed — who proposed, who
ruled, who commented, when. Pinned in the selftest (seeded stores →
3 events, newest first, all voices). Gauntlet-checked before commit.
Every candidate the Lodestar's brainstorm ranked buildable-now is now
BUILT: dossiers, workbench, presence, the map, the feedback bar, the
reload act. The substrate list is empty. The engine's work is truly
done — what remains anywhere is human: signatures, verdicts, and the
week of usage data.
