# The task being handed over

Adam's words, 2026-09-11: *"hand off the ACI course scanning and AI aligning of
the unattested terms."*

That is two related but separate jobs. Before either, one piece of vocabulary
has to be untangled, because getting it wrong sends you to the wrong files.

---

## "Unattested" means three different things here

Conflating these is the single most likely way to waste a session.

### A · An unattested **word** — a spelling judgement, in the app only

A Tibetan token that neither the 105,634 dictionary headwords nor the Monlam
word lists recognise. Computed at render time in `app/main.cpp`, **never
persisted**, shown as a slate dash-dot underline behind a toggle that is **off
by default**.

It is a hint that a token may be a typo or an unusual spelling. It is not data.
`core/include/allcore/lexicon.h` states its own limit: *reference only;
attestation is not correctness*.

**This is almost certainly not what the task means.**

### B · An un-**glossed** dictionary term — the big population

A dictionary row whose `hgm_gloss` is empty. This is where the numbers are:

| | count |
|---|---|
| entries in the spine | **105,634** |
| **with** an hgm_gloss | **12,004** (glossary 7,980 · auto-aligned 3,910 · curated 114) |
| **without** | **93,630** |
| …of those, having a comparandum (Sanskrit or Hopkins) — the machine-eligible pool | **88,369** |
| …having neither, so ineligible for any current lane | **5,261** |

### C · A no-**exponent** span — the alignment layer's recorded silence

A span of Tibetan that GMR's English **does not render at all**: `"eng": null`
on a link in `data/alignment/alignment_full_v1.json`. **11,777 of 41,759 links.**

These are deliberately excluded from the evidence sidecar — the builder's own
comment is *"no-exponent word: not evidence"* — which is why
`alignment_evidence_v1.json` holds exactly the depth-5 forms that **do** have
an English exponent.

**These are not gaps to be filled. They are findings.** A Tibetan morpheme with
no English exponent is one of the most valuable things the layer records, and
"fixing" one by inventing an exponent breaks Rule 1.

---

## Job 1 — ACI course scanning (the alignment campaign)

Reading ACI course material and recording, span by span at seven depths, which
Tibetan words GMR's English came from.

### Where it stands

| course | text | segments | pages | state |
|---|---|---|---|---|
| C01 | Pabongka, *Key that Unlocks the Door to the Noble Path* | **496/496** | 166 | ✅ LOCKED 2026-08-28 |
| C02 | Buddhist Refuge | **68/68** | 22 | ✅ LOCKED 2026-08-28 |
| C03 | Applied Meditation | **620/620** | 200 | ✅ LOCKED 2026-09-03 |
| C04 | The Proof of Future Lives | **198/198** | 66 | ✅ LOCKED 2026-09-04 |
| C05 | How Karma Works (Abhidharmakosha ch. IV) | **165/511** | 55 | ⏸ PAUSED 2026-09-08 |
| C06–C18 | — | 0 | 0 | not started |
| **C13** | — | — | — | ⛔ **BLOCKED — do not scan** |

**Totals:** 1,547 of 8,961 segments across C01–C18 — **17.3%**.

### The order of work

Resume at **C05 segment 166** (the gap is the contiguous run 166–511, 346
segments), then open **C06**. Standing mandate: *C03..C18 in order, three
segments per batch.*

### C13 is blocked, and you cannot gate around it

Across **C13:63–82** the wylie column sits one row below the English it belongs
to, so every segment in that run pairs a Tibetan text with the English of a
**different** text.

Five detector approaches were built and **all five failed** against the known
positive. `docs/COLUMN_OFFSET_UNDETECTABLE.md` exists specifically so nobody
re-attempts it. The verbatim gates all *pass* on the offset rows, because the
wylie is genuine wylie and the English genuine English — both byte-exact, just
belonging to different texts.

**The fix belongs upstream in the data project. Do not scan C13. Do not build a
sixth detector.**

### What "complete" means, mechanically

Not an English word in a document. `tools/build_alignment_layer.py` holds a
`COURSES` registry; on every build it unions the registered segment numbers and
compares them against the spine. A course marked `"complete": True` with any
unpaged segment makes the build **exit REFUSED, naming the gaps**. The gate was
liveness-tested: deleting a page made the build refuse.

Prose in a ledger saying a course is complete proves nothing. Check the flag
and run the build.

### Traps, verified

- **`C01_CAMPAIGN.md`'s header table is stale** and still lists closed sections
  as "open". The ledgers are append-only work records, not current state.
- **Every "N headwords / M pairs" line in a ledger is a snapshot** at that
  batch's landing, not today's value.
- **C05's unlanded work is probably gone.** Specs for 166–169 and proposals for
  170–175 lived only in session scratch and were never banked to disk. On
  resume, re-propose from 166.
- **`c4p1–c4p7` and `c5p1–c5p7` are cloned pages** copied verbatim from landed
  C03 phonetics pages, because those carry a span shape the generator no longer
  accepts. **They cannot be regenerated by anyone.** Do not delete them.

---

## Job 2 — "AI aligning" of un-glossed terms

Proposing English for dictionary entries that have none (population **B**).

### Where it stands — read this before planning anything

The lane exists and is **a stalled 1.6% pilot, not a layer**:

- **1,395** AI drafts written, of **88,369** eligible entries;
- generated **2026-08-12** and **blocked that night on exhausted API credits**;
- grounding: 927 on Lokesh-Chandra Sanskrit + Hopkins, 462 on Hopkins only,
  6 on LC Sanskrit only;
- **0** have been promoted into `hgm_gloss`. None ever may be, without a human
  ruling.

**Any plan assuming broad AI coverage of the dictionary is planning against a
pilot.** Restarting it needs Adam to resolve API billing — that is his, not
yours.

### The rule that governs this lane

A drafted equivalent is **a proposal, tiered and labelled**, never an
hgm_gloss. `tools/mt_wall_screen.py` — "the wall" — screens composed English
against GMR's published corpus before anything is banked. Run it.

### The honest finding about the scan's vocabulary

Of the **6,756** evidence headwords the scan has produced, **2,722 (40%) have
no spine entry at all**, and only **644** fill an actual no-gloss gap.

The scan is generating vocabulary the dictionary does not contain. That is the
"successor dictionary" intent (`docs/DICTIONARY_STRATEGY.md`) working as
designed — but it means **the scan is not primarily a gap-filling exercise**,
and reporting it as one would be wrong.

---

## What success looks like

For **Job 1**: more segments registered, every one gate-proved, with the page
files banked to disk and the campaign ledger appended. Coverage that the
builder refuses is not coverage.

For **Job 2**: nothing, until API billing is resolved. If you have credits,
resume `tools/build_ai_glossary.py --limit N` (it is resume-safe) and screen
everything through the wall.

**For both:** you will produce no correctness gate, because none is possible
here. Measure by sampled audit and report a rate with a confidence interval,
the way the two existing audits do.
