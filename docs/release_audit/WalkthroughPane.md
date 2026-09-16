# Release audit — `WalkthroughPane` (app/walkthrough_pane.inc:225–1559)

## 1. What it is

"Walk me through it" is the answer to the problem Adam named on 2026-09-12 and
which the file's own header quotes: *"my main problem in reading/translating
Tibetan is understanding in what order to read the chunks and which direction to
read the chunks."* You paste a paragraph — ACIP, EWTS wylie or Tibetan script,
the pane works out which — press one button, and it prints one section per
clause, and inside each section seven numbered steps: (1) where the clause ends
and on what particle, (2) which word is the verb and the evidence for saying so,
(3) a table of the chunks with their particle, their case and what each one is
doing, (4) **what order to read them in**, engine's order and written order side
by side, (5) what each word means from Geshe Michael's glossary with its tier,
(6) those glosses assembled in the engine's order, and (7) Geshe Michael's own
translation when the passage is in the corpus.

It composes nothing. Every engine it drives already existed — `buildOverlay`,
`splitClauses`, `refineClauses`, `chunkClause`, `spotVerb`, `planReading`, the
case tables, the lattice, the corpus — and the pane's contribution is putting
them in a row and walking a paragraph down them. Step 6 assembles Geshe
Michael's own glosses in the engine's order and says in words that it is not a
translation; step 7 shows his actual English only when the corpus holds the
line. Rule 1 holds throughout: match, never compose.

It is small for what it does: 1,559 lines total, of which the class is
225–1559 and the free helpers in `namespace walk` are 59–223. There is no
ribbon (it registers none with `paneRibbons()`, main.cpp:6344), no menu of its
own, no file I/O, no network, no writes to the proposals or comment stores, and
no QSettings key except one session-memory slot for the pasted text. Its whole
control surface is four widgets and a splitter. Everything else on screen is
rendered report text.

The pane is unusually candid about its own weakest step, and the candour is
gated. `namespace walk`'s constants (lines 69–99) carry the reading-order
measurement from `docs/READING_ORDER_MEASUREMENT.md`: at this pane's level
(clause → phrases) the engine's order matched Geshe Michael on 51% of 112
aligned spans and plain left-to-right matched on 59%, with only 21 spans
separating the two — a trend, not a finding. Step 4 says so on the page, shows
both columns, and names the written order as "the simpler bet" rather than the
better one. Five selftest checks hold it to that wording (318, 320, 329, 336, 346).

Its `selfTest(QStringList&)` (269–696) has 59 `check(...)` call sites, one of
them inside a seven-iteration loop, and **65 assertions fired at runtime — all
PASS**, verified today by running `--selftest` (output lines 565–629). It is
driven under the `app_selftest` ctest suite (`app/CMakeLists.txt:105`,
`DiamondCutterTranslationTool --selftest`; invoked at main.cpp:49034). The
battery is almost entirely about **what the page says** — honesty gates, Rule 1,
Rule 3, Rule 10 — and almost not at all about the widgets: it calls `walk()`
directly and never clicks the button, never reads the detection label, and never
touches the splitter or the session memory.

Constructed at main.cpp:45275, added as the **first** tab at 45276 and listed
**first in the first ribbon group** (`mkGroup("Read", {"Walkthrough", "Overlay",
…})`, main.cpp:46054) — so on a profile with no remembered pane it is the pane a
new user lands on. It has no chapter in `data/help/USER_MANUAL.md` (the word
"Walkthrough" does not appear in that file at all; recorded in
`docs/punchlist_resurvey_2026-09-16.json`).

---

## 2. User-visible functions

### 2a. The pane's own controls (constructed 227–266)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Paragraph box** (QPlainTextEdit `src_`) | Where the passage is pasted; accepts ACIP, EWTS wylie or Tibetan script | 241 → placeholder 242 | selftest drives it 27 times via `setPlainText` (283, 289, 296, …, 694) | **No size cap.** Measured today: 300 KB of ACIP → 62,091 syllables, 13,340 clause sections, **64.3 s of synchronous work** with no progress bar and no cancel. In the GUI that is a hard freeze. |
| **Session memory for the pasted passage** | `sess::remember` seeds the box from last session and writes it back, debounced 1.5 s, capped 256 KB | 245 → `sess::remember(QPlainTextEdit*)` main.cpp:3400–3413; key `sess/walk/source` | NONE — `sess::put` is disabled under the harness (main.cpp:3125), so no check can reach it | On relaunch the box is full and the report still shows the idle "Paste a paragraph on the left" page, because `idle()` runs at 265 and nothing re-walks. The pane looks like it lost the reader's work until they press the button again. |
| **Walk me through it** (button `go_`) | The one action: reads the box, detects the script, builds the whole report | 248–250, `connect` 264 → `walk()` 727 | **NONE for the click path.** Every selftest call goes to `walk()` directly; the gauntlet clicks only QToolButton / QCheckBox / QLineEdit (main.cpp:52503–52546), never a QPushButton | `setDefault(true)` (249) is **inert** — `autoDefault` only fires inside a QDialog and this is a QWidget in a tab; the codebase already knows this (main.cpp:19124). So there is **no keyboard route** to run the walk: Return in the box inserts a newline. |
| **Detection label** (`detect_`) | Prints "read as **ACIP** / **EWTS wylie** / **Tibetan script**", or an amber "could not read this: …" | 251–253, set at 731–733 → `walk::detect()` 135–159 | NONE — no check ever reads `detect_` | The ACIP branch (`upper*10 > letters*6`, 151) and the Tibetan-script branch (140–146) are **never exercised by any automated check**; I confirmed the ACIP branch only by running `--walk` on a Kangyur file by hand. A mixed-case document read as the wrong script is labelled confidently as the wrong script. |
| **Report view** (`out_` QTextBrowser) | Renders the whole walkthrough as HTML | 257–259; filled at 777 | Every one of the 65 selftest assertions reads `out_` | `setOpenExternalLinks(false)` at 258 and **no `anchorClicked` connection anywhere in the file** — the report contains no links at all. Step 7's corpus hits name a course and a segment but cannot be opened; nothing jumps to Overlay, Lookup or the corpus. |
| **Idle instruction page** | The seven-step explainer shown before any walk, and again whenever the box is emptied | `idle()` 708–725; called 265 and 729 | selftest 285 ("an empty box shows the instruction, not a blank page") | Its closing line claims "Steps 1–3 and 5 cite their evidence" — steps 1 and 3 cite the particle tables implicitly but print no source; only 2, 5 and 7 name real evidence. |
| **Splitter** (paragraph pane / report pane) | Drag to rebalance; 2:5 stretch by default | 237, 260–261 | NONE | The position is **not persisted** (Overlay persists its equivalent as `overlay/inputSplit`); every launch resets it. |
| **Paragraph box context menu** (Qt standard) | Undo / Redo / Cut / Copy / Paste / Delete / Select All | Qt default — no `setContextMenuPolicy` anywhere in the file | NONE | Paste is the pane's primary input route and is only reachable through this menu or ⌘V. |
| **Report context menu** (Qt standard) | Copy / Select All on the rendered report | Qt default | NONE | This is the only way to get the walkthrough out of the app — there is no Save, Export or Print for the report. |

### 2b. The report's frame — header, refusal, footer (`walk()` 727–778, `header()` 780–844)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **"Nothing to walk through"** refusal page | Shown when `detect()` finds no letters, or Tibetan that will not read back to wylie; names the reason | 734–741 | selftest 291 | Honest (rule 3) — it refuses rather than walking an empty document. |
| **Counts strip** | "N syllables · N clauses · read as X" | 798–802 | NONE (the numbers are never asserted) | The only place the detection result appears inside the report; duplicates `detect_`. |
| **Green "Clause order is reliable" strip** | Tells the reader to leave clause order alone: 5% of 972 measured pairs come out backwards | 821–828, constants 84–85 | selftest 342 (`kClauseBackPct` + `kClausePairs` on the page), 349 ("Read the clauses as written") | Colour is load-bearing here (green = attested); `greenBg` `#1C3E24` is vocabulary-checked by the `constitution` ctest suite's G1 rule over `app/*.inc` (tools/constitution_check.py:36–44, 254). |
| **Amber "Order INSIDE a clause is not settled" strip** | Warns that neither column in step 4 is established; "Try the written one first" | 829–835 | NONE for this strip's own text (the step-4 checks at 318/320/329 read step 4's table, not this strip) | Says the same thing as step 4's caveat line in different words; a reader meets the argument twice. |
| **Dropped-`na`-break notice** | When `refineClauses` merged clause breaks at *na* on absent verb evidence, says so and says the split may belong there after all | 836–842 | NONE — no fixture in the battery produces a merged `na` | An honesty notice that has never been seen firing in an automated run. |
| **Per-clause heading + the clause's own syllables** | "Clause N of M" and the clause text in monospace | 853–856 | selftest 305 (`clauses_ >= 2`), 306 (one section per clause) | |
| **Footer honesty note** | Says no translation was composed, that PROVISIONAL glosses are the machine's match, and that the chunking / cases / verb / order are the engine's | 770–776 | selftest 582, 586 (both negative — the retracted overclaims may not return), 589 (the claim that is true) | Rule 10 scar tissue: the earlier footer claimed every English word on the page was Geshe Michael's, on a page carrying auto-aligned glosses. The three gates exist to keep that fixed. |

### 2c. The seven steps (`oneClause()` 846–1277 and its helpers)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Step 1 — Where it ends** | Names the closing particle and its function, or "the end of the text — nothing closed it" | 859–866 | selftest 301 (step present, 7× loop); the boundary text itself NONE | `c.boundary_function` comes from the engine; the pane prints it unlabelled. |
| **Step 2 — ATTESTED verb** | Green ATTESTED badge plus the banked-tense evidence string | 882–885 | selftest 460 ("spotVerb really is confident here") | |
| **Step 2 — UNVERIFIED candidate** | Amber UNVERIFIED, "a candidate, not a finding", and "the clause may have NO verb at all" | 903–911 | selftest 442, 446, 450 | The gates are deliberately unconditional (see 432–438) because an earlier `if (contains("UNVERIFIED"))` wrapper meant they never ran. |
| **Step 2 — "no verb found"** branch | When `verb.chunk < 0`, says the clause may be a noun phrase or the verb is not in the glossary | 878–880 | NONE | `spotVerb` "almost never" returns < 0 (the file says so at 914–917), so this branch is close to unreachable and has never been proven to render. |
| **Step 2 — fragment note (`ni` / `dang`)** | A clause ending in the topic particle `ni` or the conjunctive `dang` is named an opening or a continuation, with the corpus figures (764 `ni`-final segments, 64% verbless vs 19% overall) | `walk::fragmentNote` 102–120; called 875 and 918–923 | selftest 557, 560 (all three constants on the page), 567, 571 (not offered when a verb is attested) | |
| **Step 2 — Wilson class line** | "Wilson class: X — expects …" from `verb.cls` | 924–927 | NONE directly; the class strings are covered by `cases_smoke` at core level | |
| **Step 2 — "an agent, not a subject"** | Reads transitivity off the class and keeps Preston's agent/subject split | 941–950 | selftest 396 | |
| **Step 2 — specialized-verb sentence** | For Necessity / LocativeNom classes prints `specializedNote()` instead of the transitive/intransitive sentence | 936–940 | NONE at pane level (`cases_smoke` covers `specializedNote` in core) | No fixture in the pane battery uses a specialized verb. |
| **Step 3 — chunk table (CHUNK / PARTICLE columns)** | One row per chunk, particle shown bold and marked "(fused)" when fused | 963–1097 | selftest 359, 362 (rows read correctly on Preston's own sentence) | |
| **Step 3 — CASE cell, single case** | "3rd byed sgra" etc., with the gloss hoisted once into the legend | 1032–1043 | selftest 359 ("third case" + AGENT), 362 ("first case") | |
| **Step 3 — CASE cell, la don unnarrowed** | Prints `2/4/7` in amber and says the particle cannot choose | 1017–1031 | selftest 386 | |
| **Step 3 — CASE cell, narrowed by the verb** | When `narrowLaDon` resolves it, prints the single case and says the **verb** narrowed it | 1008–1016 | selftest 379, 382 (and no `2/4/7` left over); `table_conformance` ctest holds `narrowLaDon` to `wilsonParse`'s answer (core/tests/table_conformance.cpp:157–216) | Exists because the Wilson Parse pane and this pane used to contradict each other about the same chunk. |
| **Step 3 — CASE cell, not a case particle** | Grey "not a case" | 994–995 | selftest 490 (the paired function cell) | |
| **Step 3 — WHAT IT IS DOING, single function** | Green, when the grammar really gives one answer | `functionCell()` 1314–1322 | selftest 485 ("qualifier of the verb") | |
| **Step 3 — WHAT IT IS DOING, several** | Prints the whole set separated by `·` and hoists the reason to the legend rather than picking | 1317–1351 | selftest 368 ("subject readings are out because the verb is transitive") | The refusal to name one is the point; a reader still gets up to five readings on one row. |
| **Step 3 — "qualifier of the verb" (syntactic particle)** | `dang` under a conjunctive/disjunctive verb is named from the verb's own class, not from a case | 1293–1298 | selftest 485, 490; `cases_smoke` for `syntacticParticleMarksQualifier` | |
| **Step 3 — predicate split row `[Nb]`** | Splits the final chunk so the verb gets its own row, as Preston separates verb from object | 1057–1064, 1098–1125 | selftest 389 ("transitive verb, ending the clause"), 415 (never "intransitive" on an unclassified verb), 418 ("transitivity is unknown"), 421 | **The split only happens when `verb.tok > chunks[i].beg`.** Verified by hand on `'phags pa'i bden pa bzhi la slob`: the chunk that *is* just the verb (`SLOB`) is printed as an ordinary first-case row reading "object of the transitive verb · subject of the intransitive verb · …" — a verb dressed as a noun, in the same view where step 2 called it ATTESTED. No check covers this shape. |
| **Step 3 — legend block** | The case glosses and the exclusion reason, once per clause instead of once per row | 1128–1140, `note` 985–987 | selftest 368 (the exclusion text appears) | |
| **Step 3 — unstated-agent caution** | "the verb is transitive but no third-case chunk appears, so the agent is UNSTATED" | 1143–1148 → `allcore::unstatedAgentCaution` | selftest 522; `cases_smoke` for the engine | |
| **Step 3 — attributive-syntax note** | Offers the nom-loc reading when a normally transitive verb has no agent and a la don chunk is present, and says both readings are live | 1151–1157 | selftest 502, 505, 511 (not offered when the agent is there) | |
| **Step 3 — implied linking verb note** | "every chunk sits in the first case … the shape of an IMPLIED LINKING VERB" | 1165–1179 | **NONE at pane level** — `cases_smoke` proves the engine string, nothing proves the pane renders it | The `beyondFirst` computation at 1165–1172 is pane-local logic with no test of its own. |
| **Step 3 — implied existence / VERBAL NOUN note** | Fires on a `pa`/`ba`/`pa'o`/`ba'o` ending: an implied *yod*, with HAVE-vs-EXIST left to the reader | 1189–1213 | selftest 464 ("VERBAL NOUN"), 467 ("your judgement"), 473 (not on a finite verb) | The `seventhNoVerb` branch (1201–1206) adds nothing the `endsVerbalNoun` branch does not already trigger; untested separately. |
| **Step 3 — "recovered from the SURROUNDING DISCUSSION" line** | The standing caveat printed under either implied-verb note | 1214–1218 | NONE | |
| **Step 4 — the caveat line** | "neither column is established at this level — the engine matched him 51% and the written order 59% of 112 aligned spans, and with only 21 … the gap does not hold up" | 1223–1232, constants 73–82 | selftest 329 ("does not hold up"), 336 (sample size **and** discordant count), 346 (both percentages) | Rule 10's sharpest gate in the app: it forbids both overclaiming *and* over-hedging. |
| **Step 4 — "the engine says" column** | `planReading`'s numbered order plus its reason per chunk; unnumbered chunks fall to the bottom with a dash | 1235–1257 | selftest 318 (both columns present); `reader_smoke` exercises `planReading` in core (core/tests/reader_smoke.cpp:117–125) | |
| **Step 4 — "as written (the simpler bet)" column** | The chunks in document order | 1258–1263 | selftest 318, 320 ("the simpler bet") | |
| **Step 5 — gloss table** | Innermost glossed span per token, up to three equivalents each | `glossTable()` 1364–1402, picker `walk::bestGlossSpan` 212–221 | selftest 603, 606, 609 (Rule 1: curated "the Victors" beats the longer auto-aligned "farthest shore"; no pronunciation lines), 665, 669 (Buddha, Dharma); `lattice_smoke` covers `OverlayDoc::bestSpan` (core/tests/lattice_smoke.cpp:250–278) | Two Rule-1 inversions have already been found here by eye, not by gates (the file's own notes at 181–211). |
| **Step 5 — unglossed syllable row** | `⟨SYLLABLE⟩ not in the glossary` | 1371–1376 | selftest 631 (angle brackets), 634 ("not in the glossary") | Rule 3, working. |
| **Step 5 — tier badge** | `[GMR curated]` / `[GMR glossary]` green, `[PROVISIONAL — auto-aligned]` amber, `[no tier recorded]` grey | 1386–1393 | selftest 646 (every gloss renders with a tier), 650 (an auto-aligned gloss is never dressed as GMR), 670 | G2. |
| **Step 5 — "nothing to gloss"** | Fallback when the clause produced no rows | 1400 | NONE | |
| **Step 6 — the assembled line** | Geshe Michael's first equivalent per span, in the engine's order, chunks separated by a grey pipe | `assembled()` 1406–1474 | selftest 621 (refuses the word "translation"), 686 (no angle brackets when every word is glossed) | Concretely, on `sangs rgyas kyis rgya gar la chos bstan` the line reads "Dharma (the teaching) mentioned / Buddha with / india to" — `bstan`'s first banked equivalent is "mentioned", not "taught". It is labelled raw material, not English, but it is the most quotable thing on the page. |
| **Step 6 — PROVISIONAL superscript** | A provisional gloss is marked `prov` in amber **at the point of use** | 1450–1455 | NONE directly (646/650 read step 5's badges, not step 6's superscript) | This is the exact site G2 was written for; the gate that guards it reads the other block. |
| **Step 6 — "no chunk got its own number"** | When `planReading` numbered nothing, says there is no order to put together | 1410–1412 | NONE | |
| **Step 6 — caveat note** | "⟨angle brackets⟩ = a word the glossary does not have"; admits it took the FIRST of several equivalents | 1464–1472 | selftest 690 ("FIRST one") | |
| **Step 7 — ATTESTED corpus hit** | Up to three corpus segments, green ATTESTED, course + §seq + Geshe Michael's English | `corpusHit()` 1477–1530 | selftest 544 (an ordinary hit is not mis-badged) | Strongest claim on the page; the hits are plain text — no link back to the corpus or the course. |
| **Step 7 — NOT A TRANSLATION badge** | A chant transliteration or untranslated mantra in the English column is badged as what it is | 1508–1528 → `allcore::englishIsNotEnglish` | selftest 534; `drills_smoke` covers the engine predicate (core/tests/drills_smoke.cpp:308–311) | 1,066 corpus segments hold such lines; this is the worst place in the app to mis-badge one. |
| **Step 7 — closing "His English. Compare its order" line** | Or, when every hit is a chant line, says there is nothing here to check step 4 against | 1535–1547 | selftest 537 (the closing line agrees with the badge above it) | |
| **Step 7 — "this clause is not in the corpus"** | Says so, and says step 4 therefore has nothing to be checked against | 1492–1498 | NONE | The ordinary case for new material — i.e. the branch a real translator sees most — is untested. |
| **Step 7 — corpus search failure** | `catch (...)` → "the corpus could not be searched for this clause" | 1486–1491 | NONE | Swallows the exception; nothing is logged and nothing distinguishes a broken spine from an honest miss. |

### 2d. Entry points from outside the pane

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Read ▸ Walkthrough ribbon switcher** | Raises the pane; tooltip carries the pane blurb | main.cpp:46054 (`mkGroup`), blurb at main.cpp:6000, tooltip at 45985 | selftest "Hover: every pane says what it is (25 pane(s))" (main.cpp:51073–51083, verified PASS) | Selecting this pane leaves the Read band's tool area **blank** — the pane registers no ribbon, so the ribbon section that was showing Overlay's DOCUMENT/REVIEW buttons empties. |
| **Walkthrough tab** (inner tab of the Read group) | The pane itself | main.cpp:45276 | selftest "Fit: every pane fits an 800px window" — Walkthrough needs 223 px (output line 998) | |
| **Landing pane on a fresh profile** | First pane of the first group, so it is what a new user sees before anything is remembered; thereafter `sess/ui/pane` restores the last pane | main.cpp:46054 + 46185–46192 | NONE | The app's front door has no manual chapter (see §5). |
| **Panes ▸ Read ▸ Walkthrough ▸ Show pane** | Raises the pane from the menu bar | main.cpp:46753–46757 | NONE for this pane (the submenu-count assertion at 51142–51157 checks Overlay only) | |
| **Panes ▸ Read ▸ Walkthrough ▸ Walk me through it** | The generic button mirror: raises the pane and calls `b->click()` | main.cpp:46762–46775 | NONE | **This is the only route that actually exercises the button's `clicked` connection** — and nothing tests it. |
| **Features ▸ show/hide Walkthrough** | Turns the pane (and its menu entry) off and on | main.cpp:46073–46110 | selftest "Features: every pane that exists is offered" (main.cpp:49052, verified PASS); the hide/show probe itself uses a Learn pane, not this one | |
| **`--walk "<passage>"` (CLI)** | Drives this same pane and prints `out_->toPlainText()` to stdout | main.cpp:45866–45881 → `setSourceForTest` 702 / `walkForTest` 703 / `outText` 705 | NONE (no ctest registers `--walk`) | The documented way to re-check the reading-order measurement against a grown alignment bank (`tools/reading_order/README.md`). It is the pane's own text, so it cannot drift from the GUI — but nothing gates the flag itself. |
| **`--walk-html`** | Same, but prints `outHtml()` so the layout can be looked at | main.cpp:45877–45880 → `outHtml` 704 | NONE | |
| **`--sweep Walkthrough`** | The generic pane sweep can target this pane by title | main.cpp:48788–48800 | NONE | |
| **Quick Access strip pin** | "Walk me through it" is a unique menu label, so `qatPathForLabel` can pin it | main.cpp:6120–6141 | NONE for this label | |
| **⌘K Hunt palette** | Does **not** reach this pane — Hunt indexes dictionary entries, corpus lines, persons, files and teachings, not menu commands (main.cpp:44271–44549) | — | n/a | A reader who knows the feature by name cannot find it with the app's own "hunt everywhere" search. |

### 2e. Keyboard, mouse and window-level

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| **Return / Enter in the paragraph box** | Inserts a newline. It does **not** run the walk | 241 (plain QPlainTextEdit); `setDefault(true)` at 249 is inert outside a QDialog | NONE | The pane's single action has no keyboard shortcut at all — not ⌘Return, not Enter. Tab-then-Space is the only keyboard route. |
| **Tab / Space** | Focus the button and press it | Qt default focus chain | NONE | |
| **Edit menu — Undo / Redo / Cut / Copy / Paste (⌘Z ⌘X ⌘C ⌘V)** | Routed to whichever editor has focus; `QTextBrowser` is a `QTextEdit`, so Copy works on the report too. This is how a passage gets in and how the walkthrough gets out | main.cpp:46623–46647 (`route` 46625, `add` 46633) | NONE for this pane | |
| **Edit ▸ Paste Special ▸ (script conversion)** | Detects clipboard text as ACIP / Wylie / Tibetan Unicode and re-expresses it on insert into the focused editor — including this pane's paragraph box | main.cpp:46658 onward | NONE for this pane | Converts on paste *and* the pane then re-detects the result; two detections with different rules on the same text. Unverified whether they can disagree. |
| **View ▸ Zoom — ⌘+ / ⌘− / ⌘0** | **Routes to the Overlay pane, not this one** (only the Input pane is special-cased) | main.cpp:47804–47829 | NONE | Pressing ⌘+ while reading a walkthrough silently resizes Overlay's text. The report is fixed-size: an 11 px caveat line and a 10 px table header (975–981) cannot be enlarged. |
| **Appearance Day / Night / Match System** | The two header strips re-pick their ink and grounds from `ux::darkChrome()` | 814–820 | `constitution` ctest (G1 green vocabulary, R4 forbidden inks) over `app/*.inc` (tools/constitution_check.py:36–44, 254, 318–326) | The rest of the report uses hand-written `#666`, `#888`, `#aaa` greys that do **not** react to dark chrome (e.g. 1083, 1102, 1135, 1494) — the constitution's R4 list covers specific forbidden inks, not every hand-written grey. |
| **Window resize / small window** | The splitter and both panes reflow | 237, 260–261 | "Fit: every pane fits an 800px window" — 223 px (selftest, verified) | Step 3's five-column table and step 4's two-column table have **no horizontal scroll area**; on a narrow window they compress rather than scroll. |

### 2f. Public API / test hooks (702–705, 1556–1558)

| Control/action | What it does | Code anchor | Automated coverage | Notes/risks |
|---|---|---|---|---|
| `setSourceForTest(QString)` | Sets the paragraph box from outside | 702 | NONE — the battery calls `src_->setPlainText` directly; only `--walk` (main.cpp:45872) uses this | Public and unguarded; named "ForTest" but shipped as the CLI's real entry point. |
| `walkForTest()` | Runs the walk from outside | 703 | NONE — used only by `--walk` (main.cpp:45873) | Same. |
| `outHtml()` | The report as HTML | 704 | NONE — used only by `--walk-html` (main.cpp:45879) | The only way anyone has looked at the rendered layout; no check reads it, so nothing catches a table that renders as black boxes (the failure recorded at 805–812). |
| `outText()` | The report as plain text | 705 | every one of the 65 selftest assertions; main.cpp:45880 | |
| `clauses_` (public member) | Clause count of the last walk | 747, 1557 | selftest 305, 306 | Public mutable state on a shipped class. |
| `provisional_` (public member) | Counts provisional glosses used in the last step 6 | 1409, 1455, 1558 | NONE | **Written and never read** — not by the battery, not by the pane, not by main.cpp. Dead state; if it was meant to drive a "N provisional glosses on this page" disclosure, that disclosure does not exist. |

---

## 3. Internal functions

| Name | Line | Purpose | Called from |
|---|---|---|---|
| `walk::kOrderSpans` … `kClauseBackPct` | 69–85 | the reading-order measurement, as named constants | step 4, the green strip, the gates |
| `walk::kVerblessPct` / `kNiFinalTotal` / `kNiFinalVerblessPct` | 97–99 | the `ni`-final corpus measurement | `fragmentNote` |
| `walk::fragmentNote` | 102–120 | names a `ni`/`dang` ending as an opening or a continuation | step 2 (875, 918) |
| `walk::pct` | 122–124 | formats `kOrderAgree / kOrderSpans` | **nothing — dead code** |
| `walk::Detected` / `walk::detect` | 129–159 | script detection (Tibetan / ACIP / EWTS) | `walk()` 730 |
| `walk::ord` | 163–167 | 1 → "1st" | case cells |
| `walk::esc` / `walk::slice` | 169–179 | HTML escaping; token-range join | everywhere |
| `walk::bestGlossSpan` | 212–221 | longest span **carrying a GMR-tier gloss**, else the longest provisional one; −1 when nothing is glossed | steps 5 and 6 |
| `WalkthroughPane` (ctor) | 227–266 | builds the four widgets and the splitter | `main()` 45275 |
| `selfTest` | 269–696 | 59 check sites → 65 assertions | main.cpp:49034 |
| `kEngineHere` / `kPlainHere` | 699–700 | the constants the step-4 gate quotes | `selfTest` |
| `setSourceForTest` / `walkForTest` / `outHtml` / `outText` | 702–705 | drive the pane from outside | `--walk`, `--walk-html`, battery |
| `idle` | 708–725 | the seven-step instruction page | ctor, `walk()` on empty input |
| `walk` | 727–778 | detect → build → one section per clause → footer | the button, the CLI, the battery |
| `header` | 780–844 | counts strip, green strip, amber strip, `na`-merge notice | `walk` |
| `oneClause` | 846–1277 | all seven steps for one clause (the pane's core, ~430 lines) | `walk` |
| `functionCell` | 1282–1352 | Preston's function set for a chunk, narrowed by the verb; writes the exclusion reason into the legend | step 3 |
| `step` / `body` | 1354–1360 | heading and indented-block markup | throughout |
| `glossTable` | 1364–1402 | step 5's table with tier badges | `oneClause` |
| `assembled` | 1406–1475 | step 6's line, with provisional glosses marked at the point of use | `oneClause` |
| `corpusHit` | 1477–1549 | step 7's corpus search, badges and closing line | `oneClause` |
| members `spine_` / `src_` / `out_` / `go_` / `detect_` / `clauses_` / `provisional_` | 1551–1558 | the pane's whole state | |

---

## 4. Dependencies

**allcore engines called** (all read-only): `buildOverlay`, `splitClauses`,
`refineClauses`, `chunkClause`, `spotVerb`, `planReading`, `OverlayDoc::spansAt`
/ `bestSpan`, `caseOf`, `narrowLaDon`, `functionsFor` / `functionsForCase`,
`functionName`, `transitivityOf`, `isTransitive`, `actorWord`,
`specializedNote`, `unstatedAgentCaution`, `attributiveSyntaxOpen`,
`impliedLinkingVerbLikely`, `impliedExistenceVerbLikely`,
`impliedElementNeedsContext`, `syntacticParticleMarksQualifier`,
`englishIsNotEnglish`, `unicodeToWylie`, `ewtsToAcip`, `Spine::corpusSearch`.

**Data**: the allcore Spine (FTS5) only — the dictionary spans, entry tiers and
`hgm_gloss`, plus the corpus for step 7. **No other file is read. Nothing at all
is written**: no proposals, no comments, no glossaries, no exports, no cache.

**Network**: none. This is one of the few panes with no outward host at all.

**QSettings** (`ALL` / `TranslationTool`): a single key, `sess/walk/source`
(via `sess::remember`, main.cpp:3400). No `walk/*` preferences exist.

**App-level helpers**: `sess::remember`, `ux::darkChrome()`, `ux::kFaint`.

**Documentation the constants answer to**:
`docs/READING_ORDER_MEASUREMENT.md` and `tools/reading_order/` — the README there
instructs that a re-measurement updates `namespace walk` in this file.

**ctest suites relevant here**: `app_selftest` (the 65 assertions, all PASS
today) · `constitution` (R4 forbidden inks, G1 green vocabulary, now widened to
`app/*.inc`) · and, at engine level only, `cases_smoke`, `reader_smoke`,
`lattice_smoke`, `table_conformance`, `drills_smoke`. `gauntlet_walk` reaches
the tab but never the button.

---

## 5. Release questions

- **The pane's one button has never been clicked by a test.** 65 assertions call
  `walk()` directly; the gauntlet clicks QToolButtons, QCheckBoxes and
  QLineEdits and skips QPushButtons; the only click path that exists is the
  menu mirror. A broken `connect` at line 264 would ship green.
- **There is no keyboard route to run the walk.** `setDefault(true)` (249) does
  nothing outside a QDialog — a fact this codebase already learned and wrote
  down at main.cpp:19124 — and Return in the box makes a newline. ⌘Return is the
  obvious fix; decide before release.
- **A large paste freezes the app.** Measured today: 300 KB of ACIP → 64.3 s
  synchronous, 13,340 clause sections, no progress, no cancel, and the session
  cap allows 256 KB to be remembered and re-pasted next launch. A 74-clause
  Kangyur text is fine (≈0.5 s), so the question is only where to put the cap
  or the progress dialog.
- **On relaunch the box is full and the report says "Paste a paragraph on the
  left."** `sess::remember` restores the passage but nothing re-walks. Either
  re-walk on restore or clear the box.
- **Step 7's hits are dead text.** The pane finds Geshe Michael's own English for
  the clause and cannot take the reader to it — no link to the corpus, the
  course, Overlay or Lookup. `out_` has no `anchorClicked` handler at all.
- **⌘+ / ⌘− / ⌘0 zoom the Overlay while this pane is on screen** (main.cpp:47812)
  and the report's smallest text is 10 px. For a pane aimed at readers working
  through a passage, that is an accessibility question as much as a routing bug.
- **A chunk that is nothing but the verb is printed as a first-case noun.**
  Verified by hand on `'phags pa'i bden pa bzhi la slob`: `SLOB` gets "1st ming
  tsam" and the noun function list, in the same clause where step 2 badges it
  ATTESTED. The split row at 1098 only fires when `verb.tok > chunks[i].beg`.
  Nothing tests this shape.
- **The measured figures where the engine actually wins are shown nowhere.**
  `kWordEngine` 36% vs `kWordPlain` 23% (77–78) and the overall 724-span figures
  (69–71) are referenced only by the dead `walk::pct()` (122). The reader is told
  the engine is not established at *this* level and never told it is clearly
  better one level down. Decide whether that is honest minimalism or a missing
  half of the story.
- **Untested honesty branches**: the `na`-merge notice (836), the implied linking
  verb note (1173), the "no verb found" branch (878), "this clause is not in the
  corpus" (1492), the corpus-search `catch` (1486), and step 6's PROVISIONAL
  superscript (1450). Each of these is a claim about evidence, and each is
  exactly the kind of text this project gates everywhere else.
- **The corpus-failure branch swallows the exception** (1488). A spine that
  cannot be searched reads to the user like a clause that is not in the corpus.
- **No manual chapter.** "Walkthrough" appears nowhere in
  `data/help/USER_MANUAL.md`, and this is the **first pane of the first group** —
  the front door of the app (recorded in
  `docs/punchlist_resurvey_2026-09-16.json`).
- **The press screenshot of this pane is empty.** The `--shots` demo seeding
  (main.cpp:51872–51970) populates Lookup, Search, Sanskrit, Convert, Trainer,
  Analysis, Apparatus and Catalog, and not this one — so the capture shows the
  idle instruction page, not the tool at work.
- **`provisional_` is dead state** (1558) and `walk::pct()` is dead code (122).
  Either wire them up or remove them; both are the residue of a disclosure that
  was planned and not built.
- **Two strips say the same thing.** The amber header strip (829) and step 4's
  caveat line (1223) both explain that the inside-clause order is unsettled, in
  different words, and a multi-clause paragraph repeats the step-4 line once per
  clause.
