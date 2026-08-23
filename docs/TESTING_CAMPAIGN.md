# The Walkthrough — Adam's systematic test of the whole tool

*Rebuilt 2026-08-22 against what actually ships. The previous version
was written 13 August and covered an application that no longer exists:
no author search, no collections window, no Vinaya layer, no Treasury of
Lives links, no dossiers, no glossary workbench, no team comments, no
catalog workflow. Walking it would have tested a memory.*

**Measured inventory this campaign covers** (derived from source, not
estimated): **22 tabs · 20 pane classes · 43 ribbon groups · 294 named
controls.**

---

## How to use this

Each pane gets a block. Each block has four parts:

| Part | What it is |
|---|---|
| **DO** | The specific things to click, in order. Numbered so you can report "Overlay 3 is wrong" and I know exactly what you mean. |
| **STRESS** | The nasty case for that pane — the biggest file, the deepest outline, the longest name, no network, missing data. **Every one of today's four worst defects was found in exactly these conditions**, which is why this row is not optional. |
| **NEEDS YOU** | What no automated check can decide, so you know why you are the one looking. |
| **VERDICT** | Your three columns. |

Your three verdicts, unchanged from the version you set on 2026-08-11:

- **[T] Tested** — works as described, or a finding gets filed
- **[K] Keep / Pare** — does this information earn its place?
- **[F] Format** — is the kept information presented right?

**Protocol.** Report as you hit things; screenshots are the fastest
possible bug report and have been the most valuable input all week — the
84000 duplication, the cut-off "pending", the Hunt button placement and
the dead biography link all came from you looking at a screen. Fixes land
same-day with a pin that would fail if the bug came back. Presses batch
per pane.

**The app you are testing:** press the DMG first (`tools/package_macos.sh
team`) so you are walking the installed copy, not a build tree. A card
already open on screen does not re-render after a relaunch — click the
term again.

---

## PART 0 — Today's repairs, tested first

*These six were found by the bug bounty and fixed today, plus two you
found yourself. They are first because a fix nobody exercised is a
promise, not a result. Each one tells you exactly how to reproduce what
was broken.*

| # | Was broken | Do this | Should now |
|---|---|---|---|
| **R1** | Catalog: "Move to shelf" + "Stamp the shelf" left the card saying **"not cataloged"** over a completed move | Catalog → pick a file → Move to shelf… → accept the stamp prompt | Say "Filed to \<shelf\> — the move is done and the file is verified on the shelf", in green |
| **R2** | Scans: "Download all folios" reported **0/0/0** when every save failed | Scans → set the data root to a read-only folder → Download all folios | Name the write failures and NOT promise offline folio jumps |
| **R3** | Citations report kept steering a document it no longer described | Overlay → Citations & quotations… → click a "LOCATED in the source" link (this swaps the document) → then click an "in your text" link | Refuse, naming the file the report was built for — not move the cursor |
| **R4** | ⌘K Enter was swallowed on first use, or opened the previous query | ⌘K → type → press Enter immediately, in one motion. Then reopen, type a second query fast, Enter | Go to the right entry **both** times, first press |
| **R5** | Pecha export said "two-up" after a three-up or booklet run | Export → choose Three-up, then A5 booklet → export | Name the layout it actually printed, with a real sheet count |
| **R6** | Witness-hunt BDRC links did nothing (cursor still changed — a convincing dead link) | Catalog → bibliography audit → click a BDRC link beside an absent work | Open BUDA in your browser |
| **R7** | Treasury of Lives "biography" 404'd | Library → any text by a known author → click **biography** | Land on a real biography. 43 of the 47 authors with texts resolve; the other 4 say "search Treasury of Lives" instead, on purpose |
| **R8** | 84000 block repeated itself six ways | Overlay → click `bcom ldan 'das` (the layer's worst case: 43 definitions, 13 glosses) | Show ≤4 distinct definitions, disclose the rest, no `<person>` markup, no mid-word cuts |

**VERDICT R1–R8:** T ☐ K ☐ F ☐ — findings:

---

# READ

## 1. Overlay — the pane you live in
*4 ribbon groups (DOCUMENT · REVIEW · SPELLING · WOODBLOCK), 30 controls.
The most-used surface in the tool and the one you have said must be
"visually appealing… information and data dense… unambiguous".*

**DO**
1. Open a text. Click a word — read the card top to bottom and ask whether the **binding HGM material** is the first thing your eye lands on.
2. Check the tier badges: HGM vs PROVISIONAL (#b00) vs reference. Does provisional *look* provisional?
3. Pronunciation line — when the surface form differs from the headword it says "as written in the text". Confirm that reads clearly.
4. Corpus concordance: does the attestation count look plausible, and does "show all" exist wherever a cap is shown?
5. Author area: name, dates, BDRC link, biography, "N works by this author".
6. Side panel: pin a card, keep working, confirm it stays useful.
7. Spelling pass, meter reader, citations, woodblock toggles.

**STRESS** — open the largest text in the library, then a text with a 1,000+ entry outline. Click a term with 169 glosses (the measured maximum). Click `bcom ldan 'das` (43 definitions).

**NEEDS YOU** — whether the card is *readable as scholarship*. No check can tell whether the hierarchy respects that HGM is binding and everything else is comparanda.

**VERDICT:** T ☐ K ☐ F ☐ —

## 2. Lookup
**DO** 1. Look up a common term, 2. a rare one, 3. one with no entry, 4. stacked multi-dictionary order, 5. q.v. links.
**STRESS** — a term present in every layer at once; then a term that exists only in a reference layer.
**NEEDS YOU** — whether the dictionary stacking order matches how you actually consult sources.
**VERDICT:** T ☐ K ☐ F ☐ —

## 3. Search (Gofer grammar)
*2 ribbon groups, 9 controls.*
**DO** 1. plain term, 2. OR, 3. NEAR-within-N-lines, 4. save a search, 5. re-run a saved search, 6. open a hit in context.
**STRESS** — a term with thousands of hits (`sems can` has 1,566+ attestations); confirm the count shown is the **real total**, not the page size. This was bounty #8.
**NEEDS YOU** — whether the grammar matches Gofer's behaviour as you remember it from the original tool.
**VERDICT:** T ☐ K ☐ F ☐ —

## 4. Library
*4 groups (SHELVE · STUDY · CARE · FIND), 13 controls.*
**DO** 1. browse, 2. open a text, 3. **By author…** (ACIP, Wylie, and an approximate/phonetic spelling), 4. Collections… — check for updates, 5. Install… a ZIP, 6. Import…, 7. subject/Vinaya layer.
**STRESS** — search an author by a badly-spelled phonetic guess; open the largest collection; run Collections… with the network off.
**NEEDS YOU** — the five confidence tiers in author search: do they rank the way a Tibetanist expects? Also E1–E4 in OVERSIGHT (defaults I chose for you).
**VERDICT:** T ☐ K ☐ F ☐ —

## 5. Scans
*4 groups (PAGE · VIEW · VOLUME · MODELS), 9 controls.*
**DO** 1. follow along in scans, 2. folio jump, 3. download all folios, 4. page navigation, 5. model selection.
**STRESS** — **no network**, then a read-only scans folder (R2). Note: BDRC's IIIF endpoint now returns 401 to everyone — it has moved behind their Auth0 login — so "BDRC unreachable" with a browser fallback is the CORRECT behaviour here, not a bug. It is in the BDRC letter.
**NEEDS YOU** — whether the fallback message tells you what to do next.
**VERDICT:** T ☐ K ☐ F ☐ —

---

# WRITE

## 6. Draft — the translation workbench
*4 groups (WORKBENCH · STRUCTURE · EVIDENCE · PUBLISH), 32 controls — the second-largest pane in the tool.*
**DO** 1. start a draft, 2. side-by-side with source, 3. provisional-tier warnings, 4. register warnings, 5. evidence panel, 6. structure/outline, 7. save, 8. reopen, 9. publish/export.
**STRESS** — a very long text; then close and reopen mid-draft and confirm nothing was lost. Then edit the source *underneath* an open evidence panel (this is the B11 hazard class).
**NEEDS YOU** — whether the oversight warnings fire where a translator actually needs them, and whether they are honest without being noisy.
**VERDICT:** T ☐ K ☐ F ☐ —

## 7. Input — the input-center formatter
*6 groups (PAGES · VIEW · TYPING · OCR · PARTNER · EXPORT), 15 controls.*
**DO** 1. type Tibetan live, 2. wylie→unicode as you type, 3. reverse (unicode→wylie), 4. page/folio structure, 5. partner mode, 6. export.
**STRESS** — paste a very large document; type fast enough to outrun the converter.
**NEEDS YOU** — this pane is for the input centers, not for you. Its real acceptance test is a pilot user (OVERSIGHT C4). Judge it as "could I train someone on this?"
**VERDICT:** T ☐ K ☐ F ☐ —

## 8. Convert
**DO** 1. wylie→unicode, 2. unicode→wylie, 3. ACIP↔EWTS, 4. round-trip a hard case, 5. failed-conversion flagging (must FLAG, never guess).
**STRESS** — malformed input, mixed scripts, a syllable the spellchecker rejects.
**NEEDS YOU** — whether flagged conversions are flagged in a way you would trust in production.
**VERDICT:** T ☐ K ☐ F ☐ —

## 9. Sanskrit
**DO** 1. IAST↔Tibetanized ACIP, 2. ACIP Sanskrit input code (a#/r%/n~), 3. next-letter style (a-/n.), 4. EWTS, 5. Devanagari, 6. pronunciation.
**STRESS** — the official standard doc's own examples (`prama#n%a` / `prama-n.a` / `{PRA M'A nA}`); then something outside the 92.2% battery.
**NEEDS YOU** — the pronunciation style is still **provisional** pending the ACIP pronunciation standard. Judge whether it is labelled as provisional clearly enough.
**VERDICT:** T ☐ K ☐ F ☐ —

## 10. Manuscript
*4 groups (MANUSCRIPT · TYPE · HOUSE STYLES · VIEW), 8 controls.*
**DO** 1. manuscript view, 2. type settings, 3. house styles, 4. view options.
**STRESS** — the longest title you can compose; a house style with unusual settings.
**NEEDS YOU** — house styles are a publishing decision (DCC's), not an engineering one.
**VERDICT:** T ☐ K ☐ F ☐ —

## 11. Export
*PUBLISH group.*
**DO** 1. pecha native folio, 2. two-up A4, 3. two-up Letter, 4. **three-up A4**, 5. **A5 booklet**, 6. translation prep, 7. print Tibetan, 8. citations document.
**STRESS** — export to a **read-only folder** and to a **full disk**. Until today, four export paths reported success or said nothing at all when the write failed (bounty #6). This is the check that proves it.
**NEEDS YOU** — whether the printed artifacts are usable by a real publishing workflow.
**VERDICT:** T ☐ K ☐ F ☐ —

---

# STUDY

## 12. Trainer · 13. Drills · 14. Analysis
**DO** Trainer: run a session. Drills: run one, get it wrong on purpose, read the miss-taxonomy report. Analysis: run an analysis and save it.
**STRESS** — Drills with zero history (the empty state must be honest, not a fake all-clear); Analysis on the largest text.
**NEEDS YOU** — whether drills teach the way you teach.
**VERDICT:** T ☐ K ☐ F ☐ —

## 15. Align
*3 groups (TEXTS · LINKING · HARVEST), 8 controls.*
**DO** 1. load a pair, 2. auto-align, 3. correct an alignment by hand, 4. harvest, 5. save.
**STRESS** — two texts that do NOT correspond; a pair with very different segment counts.
**NEEDS YOU** — harvested pairs are candidate evidence. Whether they are trustworthy enough to feed the corpus is your call and Geshe Michael's, never the machine's.
**VERDICT:** T ☐ K ☐ F ☐ —

## 16. Apparatus · 17. Review
**DO** Apparatus: anchors, variants, notes. Review: open a review, leave a comment, resolve it.
**STRESS** — a text with many variant witnesses; a review thread with many comments.
**NEEDS YOU** — whether the apparatus conventions match scholarly practice.
**VERDICT:** T ☐ K ☐ F ☐ —

---

# CATALOG & TEAM

## 18. Catalog — the largest pane in the tool
*5 groups (ACCESS · SUGGEST · INVENTORY · QC · HANDOFF), **90 controls** — nearly a third of every named control in the application.*
**DO**
1. Log in as a cataloger. 2. Scan a folder — read the census. 3. Compose a catalog filename. 4. **A name over 255 characters** (this must write a META companion or refuse — bounty #4). 5. Move to shelf (R1). 6. Stamp the shelf (R1). 7. QC intake — read the verdict carefully. 8. Bibliography audit → witness hunt → BDRC link (R6). 9. Export the missing list. 10. Propose an identity.
**STRESS** — QC intake on a machine **without** `data/extracted/catalog_works.json` (bounty #5: the pane printed a green "clean" verdict from a bank holding zero entries). Compose two files under the same catalog number. Move to a shelf that is itself the destination root.
**NEEDS YOU** — the QC verdicts are catalog judgements. A green "clean" from this pane must mean what a cataloger thinks it means.
**VERDICT:** T ☐ K ☐ F ☐ —

## 19. Propose · 20. Approval
*Propose: 20 controls. Approval: 3 groups (QUEUE · RULINGS · RELEASE), 15 controls.*
**DO** Propose: file a proposal, see it PENDING. Approval: open the queue, rule on one, export approved candidates.
**STRESS** — an empty queue; a proposal with a very long justification.
**NEEDS YOU** — **this is the Geshe Michael approval channel.** Nothing about it is machine-decidable. It is also the channel that would carry OVERSIGHT B1's 29 register rulings.
**VERDICT:** T ☐ K ☐ F ☐ —

## 21. Files
*4 groups (VIEW · GO · TRANSFER · TOOLS), 35 controls.*
**DO** 1. browse, 2. navigate, 3. FTP/WebDAV listing, 4. two-way sync plan, 5. batch rename plan, 6. tools.
**STRESS** — a sync plan with conflicts on both sides; a batch rename with collisions; **filenames with decomposed Unicode** (`Ārya … Sūtra.txt`) — these broke the release script earlier this week and `rsync` still cannot handle them.
**NEEDS YOU** — sync never deletes, by design. Confirm that matches your expectations before you trust it with real folders.
**VERDICT:** T ☐ K ☐ F ☐ —

## 22. OCR
**DO** 1. load a page, 2. recognise, 3. **Stop mid-recognition**, 4. read the result, 5. correct it.
**STRESS** — a bad scan; a huge image; press Stop immediately.
**NEEDS YOU** — OCR output quality against the models, and whether corrections flow where you need them.
**VERDICT:** T ☐ K ☐ F ☐ —

---

# CROSS-CUTTING

*Not a pane — these are the things that break BETWEEN panes, which is
where the bounty found its worst defects.*

| # | Check | Why |
|---|---|---|
| **X1** | Open text A, then open text B from a **different** pane. Does everything follow? | Bounty #3: Overlay's own open bypassed `openFile()`, so text B was read against text A's glossary and new entries were written into A's file |
| **X2** | With a report/dialog open, change the document underneath it | Bounty #11 (R3). Modeless windows outliving their document |
| **X3** | Every ribbon: does any button label elide to "…"? | This shipped app-wide with four buttons all reading "…" |
| **X4** | ⌘K from every pane | The headline affordance (R4) |
| **X5** | Menu bar mirrors the ribbons — pick three menu items and confirm they do the same thing as their buttons | The menu has lied before |
| **X6** | Quit and relaunch. Is everything where you left it? | |
| **X7** | Run with the data folder **renamed** | Should say so plainly, not crash or silently show nothing |
| **X8** | Run with **no network** for a full session | |

---

## Findings log

*File them here as you go: pane, number, what you saw, screenshot. I
convert each into a pin that would fail if it came back.*

| # | Pane | What you saw | Status |
|---|---|---|---|
| | | | |

---

## What this campaign deliberately does NOT cover

Being honest about the edges, so a clean walkthrough is not mistaken for
a clean bill of health:

- **Anything gated on other people** — the BDRC letter, licensing lines from THL/84000/PKTC/CIHTS, Dr. Laine's rKTs export. See `docs/OVERSIGHT.md`.
- **A clean-Mac install.** Everything here has only ever run on your machine (OVERSIGHT C3), and it is the single most important release step.
- **The register layer** (OVERSIGHT B1) — 29 passages where you already state a context-dependent reading out loud, awaiting your rulings.
- **Software-quality questions** — test coverage, memory safety, performance at scale. Those are backlog #48, running separately as a graded SQA assessment; this campaign asks "is it good to work in", which is a different question and the one only you can answer.
