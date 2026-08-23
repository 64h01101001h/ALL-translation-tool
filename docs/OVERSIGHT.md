# What needs Adam

*The single register of everything the tool cannot decide, author, or
send for itself. Created 2026-08-22 at Adam's instruction. Every row
says what it costs him and what it unblocks, so a spare ten minutes
can be spent on the highest-leverage thing rather than the loudest.*

*Rule for this document: an item leaves only when Adam rules, acts, or
explicitly defers it. Nothing is closed by inference, and nothing is
quietly dropped because it went stale.*

## How the lists relate (Adam asked, 2026-08-22)

There are **two durable lists and they are deliberately different
cuts of the same work** — not two backlogs.

- **`docs/CLOSER.md` is THE backlog.** Every piece of open work lives
  there, numbered, with its status and owner. An item not on it does
  not exist. That is the single source of truth.
- **This page is a VIEW of it** — the subset that no amount of
  engineering can move, sorted by what kind of act it needs, so a
  spare ten minutes can be spent well. Every row here cites its
  backlog number. Nothing lives here that is not also there.

A third list exists inside the session harness and is **scratch**: it
vanishes when the session ends and holds nothing from today. It is not
a source of truth and should not be treated as one.

**The rule that keeps them honest:** anything discovered to be
human-gated is written into CLOSER.md *and* surfaced here in the SAME
commit that discovers it. An item that lives only in a chat message
does not exist.

**As of 2026-08-22 every engineering item on the backlog is closed or
deferred. Everything that remains in the project is on this page.**

---

## A · Highest leverage — minutes of your time, large unblock

| # | What | Cost | What it unblocks |
|---|---|---|---|
| **A1** | **`gh auth login`**, then `tools/backup.sh --github` | ~90 seconds | The only offsite copy of 790+ commits. External drive and Dropbox are current; GitHub is not. This is the copy that survives the house burning down. |
| **A3** | **Send the Joel Crawford email** (draft `r-7864908769855474259`). Drag `~/Desktop/ALL-Translation-Tool-Documentation.zip` in first — I can attach the three .md files but not the binary archive. | 2 minutes | The BDRC letter, the Apple Developer ID, the GitHub org, the catalog question, and the input-center question are ALL stalled behind his answer. |
| **A4** | **A To: address for the BDRC letter** (backlog #24), and the decision whether it goes from you or from ALL. Hold until Joel answers A3 — approaching them twice would be worse than approaching them late. | 1 minute after A3 | Written confirmation for the OCR models. The free-of-charge ruling already places them inside CC BY-NC; this is for the record. |

---

## B · Only you (or Geshe Michael) can author this

| # | What | Cost | Why nobody else can |
|---|---|---|---|
| **B1** | **The register layer — and the ask just got much smaller.** Measured 2026-08-22: only ~5 of 12,004 glossed entries carry a register mark, so the tool has nothing to surface. But mining your own recorded classes that same day turned up **29 passages where you already state a context-dependent reading out loud** — *"it doesn't mean wisdom here… it always means corrupt intelligence"*, *"the four higher truths, mistranslated as noble truths"*. They are in docs/research/GMR_TRANSLATION_METHOD.md, each linked to the second in the recording. **The ask is no longer "author a register layer" but "rule on 29 passages you have already taught."** | **~29 rulings**, each with the recording to check against | Rule 1. Machine work may MATCH his English from evidence, never COMPOSE it. Deciding he means X in Vinaya and Y in Pramāṇa is composition. The approval channel already exists to carry exactly this, stamped and permanent. |
| **B2** | **HGM/LC redistribution stance** — one written line | 1 minute | Backlog #23. Blocks what the public build may carry. |
| **B3** | **Teachings-index consent line** — one written line | 1 minute | Backlog #25. The teaching links ship on his material. |
| **B4** | **Screen recordings / YouTube channel** for the manual. Written half closed 2026-08-21. | hours, or delegate | Backlog #29. The written manual stands alone meanwhile. |
| **B5** | **An authoritative author-name list**, if you want one. Display names are currently the catalog's own most-frequent spelling (`RJE TZONG KHA PA BLO BZANG GRAGS PA`), never invented. | open-ended | A publishing-house decision about preferred forms, not an engineering one. |

---

## C · Verification only a human can perform

| # | What | Cost | Note |
|---|---|---|---|
| **C1** | **Run `docs/TESTING_CAMPAIGN.md` against the RC.** The K/F columns are yours. | a working session | Backlog #20. The acceptance gate. 72 automated suites prove the machine's claims; they cannot tell you the tool is *good to work in*. |
| **C2** | **Authority sign-off on data-facing surfaces** (you and/or GMR walk the panes) | a sitting | Backlog #21. Nothing machine-checkable replaces someone entitled to say the tiers read honestly. |
| **C3** | **Clean-Mac install test** | 30 minutes | The most important release step, per RELEASE_READINESS.md. Everything here has only ever run on your machine. |
| **C4** | **Name a pilot user** at an input center, one week, ledger on | 1 decision | Backlog #22. Also answers whether Windows builds are needed. |

---

## D · Money / accounts

| # | What | Cost | Consequence of not doing it |
|---|---|---|---|
| **D1** | **Apple Developer ID** — $99/yr, and whether it lives with ALL or with you | one call | Backlog #27. Until then the app is ad-hoc signed and every user must right-click-open on first launch. The signing pipeline is built and dormant. |

---

## E · Reversible defaults I chose — check when convenient

*These are running. Nothing is blocked. Judge them by using the tool.*

| # | Default taken | Flip it by saying |
|---|---|---|
| **E1** | Author search shows people **with texts first**; ~300 biography-only people fold behind one line. | "show everyone" / "hide the biography-only ones" |
| **E2** | The 442 name-only attributions **are** searchable, in a weaker zone that names its own limitation. | "leave them out" |
| **E3** | The 36 catalogued people with no recorded name stay unreachable by name, disclosed rather than hidden. | — |
| **E4** | Text S6387's two conflicting attributions are **both** shown with the disagreement named. | "pick one" |
| **E5** | Biography links live on the person's page, not in ⌘K. | "put them back in ⌘K" |
| **E6** | Data folder renamed to "Diamond Cutter Tool Data"; the old name still works, so your existing install keeps running. | — |

---

## F · Waiting on other people (you are the contact)

| # | Who | For what |
|---|---|---|
| **F1** | Joel Crawford | BDRC relationship, Apple ID, GitHub org, the real catalog, input centers, succession |
| **F2** | BDRC | Written OCR-model confirmation (gated behind A3/A4) |
| **F3** | Dr. Laine | rKTs export (CC BY-SA) |
| **F4** | Ora Maimes / Jamie | Whisper transcripts — she offered. backlog #32 is CLOSED on the 2,800 auto-caption files; her transcripts are cleaner than YouTube's auto-captions (which demonstrably mishear "sutra" as "citrus"), so they would EXTEND the method mining and sharpen B1's 29 passages, not unblock anything. |
| **F5** | THL · 84000 · PKTC · CIHTS | Licensing lines for reference layers currently local-only |

---

## G · Time, not effort

| # | What |
|---|---|
| **G1** | Backlog #28 — ribbon reorder from real usage data. Needs a week of ledger, not a decision. |

---

## H · Design decisions waiting on your ruling

*Filed 2026-08-22. Each is a real fork where my choosing for you would
be guessing at how you work. None blocks other work.*

| # | Decision | Backlog |
|---|---|---|
| **H1** | **Whole-app visual direction.** Choose a direction from rendered options — density, chrome, typography, how panes read. I have sequenced this AFTER your test campaign, on the argument that a direction chosen before you have worked in the tool is chosen from my imagination. Overrule me if you disagree. | #37 |
| **H2** | **Author-search layout.** Two rejected alternatives exist with documented reasons. I do not think either beats what ships, and said so, but you asked to see the options. | #38 |
| **H3** | **The Overlay card redesign.** A four-direction campaign has ruled: authority zones, ruled, with an apparatus grammar inside each. It also proved by rendering that div borders have NEVER painted in this app. Awaiting your look before implementation. | #39, #43 |
| **H5** | **Author portraits.** Measured: 62 of 462 people have a Commons portrait reachable by the join already built (34 with local texts), licences machine-readable and including PD and CC0. I can build the togglable layer; **fetching and redistributing images is a licensing act and yours to rule on**, and the two best Tibetan portrait collections (Treasury of Lives, Himalayan Art Resources) need a letter like BDRC's. | #44 |
| **H6** | **Data-visualisation vocabulary.** Which of the four grounded mappings to build first — meter histogram, a text's tier composition, one term across the courses, or the citation web. | #42 |
| **H6** | **GUI mockup + visual revamp**, the original deferral that #37 and #39 now give shape to. | #34 |

---

## I · Living

| # | What | Backlog |
|---|---|---|
| **I1** | **The oversight register itself** — this page. Standing rule: anything found to be human-gated is written into CLOSER.md AND surfaced here in the same commit that discovers it. Reconciled mechanically by `tools/reconcile_lists.py`, which runs in the constitution gate, so an item that needs you can no longer be invisible to you. | #35 |
| **I2** | **The test campaign** — mine to rebuild (the existing one is dated 13 August and covers none of the last nine days), yours to walk once it exists. | #41 |

---

## CLOSED

| # | What | Ruled |
|---|---|---|
| **A2** | **Name sanction — SETTLED.** Adam, 2026-08-22: *"Holy Geshe Michael doesn't mind having his name on the dictionary. He's given me his blessing to do so in person. And DCC Diamond Cutter Classics is the translation group I work for."* Both halves of the name are therefore authorised at their source: **Geshe Michael Roach Dictionary** by his own in-person blessing to Adam, and **Diamond Cutter Translation Tool** by Adam's standing as a translator for Diamond Cutter Classics. Recorded here because a name carried by the app bundle, the install path, every document and the Joel email should be traceable to who authorised it and when — not merely to the fact that nobody objected. This supersedes the caution raised on 2026-08-22, which asked the question precisely because it had not been asked. | Adam, 2026-08-22, on Geshe Michael's in-person blessing |

---

*Maintenance: when an item is done, move it to a Closed section with
the date and who ruled. When something new becomes human-gated, it
goes here in the same commit that discovers it — an item that lives
only in a chat message does not exist.*
