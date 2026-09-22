# Backlog

Items Adam has put on the back burner, with the reason each one matters.

## The Geshe Michael Roach Dictionary becomes the spine (Adam, 2026-09-09)

> "ANY NEW DATA THAT COMES IN TO THE NEW GESHE MICHAEL ROACH DICTIONARY THAT
> WE'RE GENERATING SHOULD BE USED TO MAKE THE LEARN TAB MORE ROBUST, AS IT
> SHOULD BE INTEGRATED INTO THE DRILLS AND TRAINER AND ANYWHERE ELSE THAT IT
> CAN BE USED. ALMOST LIKE THIS WILL EVENTUALLY BECOME THE NEW SPINE OF
> EVERYTHING. IT SHOULD EVENTUALLY PROVE TO BE THE MOST AUTHORITATIVE GESHE
> MICHAEL ROACH DICTIONARY ON THE PLANET."

Every batch of alignment data should flow outward, not only into the export:

- **Drills and Trainer** draw their terms from the layer, tier-labelled, so a
  student is drilled on his actual renderings and never on a machine guess
  wearing his authority.
- **Per-course glossaries** already exist in the export
  (`geshe_michael_roach_dictionary_by_course.csv`) and are the natural
  syllabus for a student of that course.
- **Lookup, Overlay and the terminology check** should reach the same layer,
  so one dictionary answers everywhere.
- **The ruling queue** is the missing piece: until he rules, everything stays
  TENTATIVE. Authority comes from his decisions, not from volume.

The constraint that makes this safe: the tier travels with the data at every
hop. A layer entry may never arrive anywhere wearing the binding colour.

## The cursive scripts: kyuk-yik, bam-yik, dru-tsa (Adam, 2026-09-12)

> "we're going to integrate using the kyuk-yik script/font and the bam-yik
> script font and the dru-tsa script/font. in addition i'd also like to create
> a trainer for reading these scripts and find other creative ways to use them
> in our application."

Three Tibetan hands beyond the dbu-can (headed) printing script the app
renders everywhere today. Adam gave the names phonetically; the wylie is
recorded here as **unconfirmed** rather than guessed, because a wrong
transliteration banked in a spec is exactly the failure rule 3 exists to stop.
Confirm each spelling with Adam or against the spine before any of it reaches
code or a UI label.

| as Adam said it | almost certainly | what it is |
|---|---|---|
| kyuk-yik | ⟨'khyug yig⟩ *unconfirmed* | the running/fast cursive hand — what a scribe writes at speed |
| bam-yik | ⟨bam yig⟩ *unconfirmed* | a heavy formal headless book hand |
| dru-tsa | ⟨'bru tsha⟩ *unconfirmed* | an ornamental display hand |

### Why this is not just a font picker

The three tasks are different sizes and only the first is small.

**1. Render in these hands.** A font swap on the display side. The engines are
untouched: they work in ACIP and wylie, and unicode Tibetan is unicode Tibetan
whatever face draws it. The real work is sourcing fonts that are licensed to
ship — the same question the Kawachen audio raised, and it must be answered
BEFORE anything is added to the repo, not after.

**2. A trainer for READING them.** This is the substantial one, and it is a
genuinely different skill from everything the Learn tab does today. Every
existing drill assumes the reader can already decode the glyph and asks about
meaning, order or grammar. Reading a cursive hand is decoding, and it is the
skill that stands between a student and every manuscript that was never
block-printed.

The honest design problem: our drills are generated from the corpus and scored
against Geshe Michael's own text, which is what makes their answers Geshe Michael's rather
than ours. A script-reading drill would be scored against *the same syllable
rendered in another face* — so the answer key is the identity of the syllable,
which is attested, and the difficulty is entirely in the rendering. That works,
and it is worth noting that it is a different kind of drill from the other
fourteen: no corpus evidence is involved and none should be claimed.

**3. "Other creative ways."** Deliberately not designed here. Worth thinking
about with the pecha scans in the Input pane, where a real manuscript hand is
already on screen beside the typed text.

### What has to be settled first

- **Licensing, before a single font file lands in the repo.** This repository
  is public. Several good Tibetan cursive faces are free for use and not for
  redistribution, which is the Kawachen situation exactly.
- **Does the shaping hold?** Qt was chosen for this project because bundled
  HarfBuzz shapes Tibetan stacks correctly. A cursive face exercises shaping
  far harder than dbu-can does, and a face that renders a subjoined stack
  wrongly is worse than no face at all. Measure before promising.
- **Which hand first.** Kyuk-yik is the one a reader meets most in
  manuscripts, so it is the obvious first, but Adam's call.

## THL Kangyur and Tengyur catalog (Adam, 2026-09-09)

Harvest what we can from `old.thlib.org/encyclopedias/literary/canons/kt/`
and fold it in **only where it is verifiably in accord with our own data** and
matches an entry we choose to align it with. Two prerequisites before any
harvesting: the licence and terms of that site, and a matching gate that
refuses a join rather than guessing one. We already hold KL-to-BDRC and
KL-to-Lhasa concordances, which are the natural join keys to test against.

## Drills and Trainer as a standalone iOS app (Adam, 2026-09-09)

> "take the drills and the trainer function of the learn tab and bring those
> into a strictly stand alone iOS mobile app ... once we get the drills, the
> decks, the words, and all of the development design features, and basically
> just debug the entire tool."

Not now — the gate Adam himself set is that the desktop Drills and Trainer are
debugged first. He expects that to go quickly, and wants this pushed up in
priority when it does.

What already helps, and what does not:

- **The core is already portable.** The drill logic is C++20 in `allcore`
  (`core/src/drills.cpp`, `core/include/allcore/drills.h`) with no Qt in it.
  It compiles for iOS as-is. Only `app/main.cpp`'s `DrillsPane` and
  `TrainerPane` are Qt, and those are the view layer.
- **The data is the question.** The spine is a 100 MB+ SQLite file. A phone
  build wants a slice: the drillable segments, the vocabulary deck, and the
  Geshe Michael Roach Dictionary layer — not the whole library.
- **Progress must sync or it is useless.** `Progress` is local today. Two
  devices means deciding where the deck lives.
- **The honesty rules travel with it.** Tier labels, the PROVISIONAL colour,
  "Engine guidance is labeled guidance" — a phone screen is smaller, not more
  forgiving.

### Measured 2026-09-09, so the estimate is not a guess

| Question | Answer | How it was measured |
|---|---|---|
| Does the core compile for a real iPhone? | **Yes. All 77 files, zero source changes.** | `clang++ -std=c++20 -target arm64-apple-ios17.0 -isysroot iPhoneOS.sdk` over every file in `core/src/`; 77 compiled, 0 failed |
| Is the toolchain on this Mac? | **Yes.** Xcode 26.6, with both `iPhoneOS.platform` and `iPhoneSimulator.platform` | `/Applications/Xcode.app`, platform listing |
| Anything blocking? | `xcode-select` points at CommandLineTools, so `xcodebuild` will not run | needs `sudo xcode-select -s /Applications/Xcode.app` — Adam's password, nobody else's |
| How big is the data? | Full spine 238 MB. A Drills-and-Trainer slice (meta, entries, variants, segments, pron) is **182 MB, 33 MB gzipped** | built the slice and vacuumed it |
| Is there a smaller way? | **Yes: 682 bytes per pre-built drill.** 20,000 drills = **13 MB**, and the phone then needs no spine at all | generated 3,000 real cloze drills and measured the serialized pack |

### Two paths, both real

**A — the pack.** Generate drills on the Mac, ship a 13 MB pack, SwiftUI reads
it. No C++ on the phone. Fastest to something he can hold. Costs the adaptive
draw and live generation.

**B — the core on the device.** Link `allcore` (proven to compile), ship the
182 MB slice, generate drills live, keep adaptive targeting and the SRS in the
same SQLite. Costs bundle size and a day or two more.

The UI is the bulk of the work either way — `DrillsPane` is roughly 700 lines
of Qt that becomes SwiftUI. The honesty rules travel with it: tier labels, the
PROVISIONAL colour, "engine guidance is labeled guidance", and the title badge.

### What Adam has to do himself

1. `sudo xcode-select -s /Applications/Xcode.app` (his password).
2. Signing identity. A free Apple ID sideloads to his own phone but the build
   expires every 7 days; the $99/yr Developer account gives a year, and is
   needed anyway to put it on the input centres' and students' phones.
3. On the iPhone: Settings ▸ Privacy & Security ▸ Developer Mode, then trust
   the Mac when plugged in.
4. Tell me the iPhone model and iOS version so the deployment target is right.

## An input-centre suite of its own (Adam, 2026-09-09)

> "create the application that could be installed on our machines overseas at
> our input centers that our input operators use to type in our text ... to
> deliver us our ACIP files, our a, b, c, d, and e files, and the raw files ...
> it will be used by a different class of user."

A tab, a workflow and a preset for the operators, not for translators.

**What already exists and must be surveyed before anything new is designed:**
the app already has an **Input** pane (page-scan typing, double keying against
a partner file, per-page files under `library/input_work/`) and an **OCR** pane
(page image → OCR-DERIVED text, batch folder). Roadmap item A is already "the
input-center formatter". So this is consolidation and extension, not a green
field — the first task is an honest inventory of what those two panes already
do.

**Research Adam asked for, none of it done yet:**
- Asian Legacy Library's own pages on how input is actually run.
- The **ACIP Catalog Release IV** PDF on his Mac — `~/Desktop/Thursday
  Database-Cataloging 6 4 26 Meeting/ACIP Catalog Release IV.pdf`. Note the
  known defect: its Tibetan Input Code chart, manual pp. 153–160, is EMPTY
  image placeholders in that copy, so an intact copy may be needed.
- Anything else public on the ACIP input scheme.
- **Still owed by Adam** (already on the list from earlier): input-centre raw
  data samples, and the GMR/publishing-team formatting guidelines.

**The a/b/c/d/e file question** is the centre of it and is not yet understood
here: what each letter means, who produces which, what the handoff looks like,
and what a finished delivery is. That has to be answered from the manual and
from Adam before a single pane is drawn.

**Naming.** Adam floated "input center tab", "ACIP input", "ALL input" and was
not satisfied with any. Deferred to him.

**The steelman rule applies with force.** A different class of user means the
temptation to ship everything translators have. Every tool in this tab has to
justify itself against an operator's actual day — typing from a scan, keeping
pace, catching their own errors, and delivering a clean file — or it is cut.

## DECIDE: attestation, the segmenter ladder, or both permanently (Adam, 2026-09-10)

Adam's ruling for now is **ship both as two views** — attestation on by default
because it is a fact, the Botok vocabulary ladder as a labelled opt-in because
it is a judgment. He asked that the *ultimate* choice be filed rather than left
to drift: do we keep both forever, or does one win?

**This entry exists so the question can be CLOSED, not merely remembered.**

### What we already know (measured 2026-09-10, not assumed)

- **The segmenter exists.** Botok is ported to C++, battery-proven
  (`botok_smoke`), Apache-2.0, already a user toggle in the Overlay, stocked
  with 98,449 GMR + 449,444 Monlam forms, and builds in 4.7 s. `segment()`
  returns a flat word sequence, so it *does* decide.
- **The 2.5× instability was not Botok.** That spread (199 vs 380 items for 50%
  coverage) came from two improvised tokenizations. Botok returns one
  consistent answer. The instability objection is dead; the "it makes choices
  the corpus does not force" objection is not, and is handled by labelling it
  reference-only, exactly as the Overlay already does.
- **Attestation is already in the spine.** `entries.corpus_n_segments`,
  populated for 29,512 entries, referenced by **zero** lines of C++. It needs no
  segmentation decision because it counts segments containing a form, not
  tokens.
- **Attestation alone produces a sane ladder:** thams cad 3,128 · sangs rgyas
  2,007 · rang gi 1,878 · de ltar 1,824 · byang chub 1,663.

### They answer different questions, which is why "both" may be the right end state

- Attestation: **how central is this to his teaching?**
- Frequency: **how often does this token occur?**

For a corpus that is one teacher's course material rather than a language
sample, the first is arguably the more useful question. That asymmetry is the
strongest argument that this is not a contest at all.

### What would SETTLE it — the point of this entry

1. **Do the two orderings actually differ where it matters?** Compute both
   rankings over the same headwords and measure the rank correlation. If the
   top 500 are largely the same words in a similar order, the ladder adds
   little and attestation wins on honesty alone. **This is cheap and should be
   run first — it may close the question outright.**
2. **Does Botok's segmentation survive inspection on OUR text?** Sample 200
   segments, have a qualified reader mark each segmentation sound or wrong.
   A high error rate kills the ladder regardless of its usefulness.
3. **Does anyone turn it on?** Once both ship, the usage ledger answers it. If
   the ladder toggle sits unused for a term, cut it under the steelman rule.
4. **Does a learner ordered by one outperform a learner ordered by the other?**
   The honest answer is that we will never have the sample size for this, and
   nobody should wait for it.

### Trigger

Revisit when **either** (1) has been run — it is a day's work and may end the
question — **or** the ladder has been shipped and toggled by real users for one
course cycle. Do not revisit before one of those; there would be nothing new to
reason about.

## The phone ignores the system text size (found 2026-09-13)

Every piece of type in `ios/DiamondDrills/DiamondDrills.swift` is set at a
fixed point size — `.font(.system(size: 15))` and its neighbours — so none of
it responds to the reader's Dynamic Type setting. The one exception is the
drill picker, whose `.menu` style label uses the scalable body font, and at a
large accessibility size that single control ran to five lines and pushed the
drill off the screen.

Shortening the drill names contained it (worst case is now two lines), but the
underlying state is worth fixing properly:

- the picker cannot be brought under control while it uses `.pickerStyle(.menu)`
  — neither `.font()` nor `.lineLimit()` affects its collapsed label; it wants
  a custom button plus a confirmation dialog, or `.navigationLink` style
- and the rest of the app should move to relative sizes so a reader who needs
  larger type gets it everywhere, rather than nowhere

Not urgent — at the default text size the app looks right — but it is the
difference between an app that merely ignores accessibility settings and one
that breaks under them.

## Himalayan Art Resources, somehow (Adam, 2026-09-22)

> "find someway to bring the himalayan art resources website into this
> project."

Back burner because the FIRST question is not technical and cannot be answered
from this repo: **what are we permitted to do with it.** himalayanart.org is
somebody else's collection — images, iconographic identifications, and
scholarly metadata, much of it photographed from works held by museums and
private collections that carry their own separate rights. Nothing about the
site being public makes any of it ours to ingest, and this project has a
clean licensing manifest that a careless ingest would ruin: every payload row
in `docs/distribution/PAYLOAD_MANIFEST.txt` names its source and its lane, and
`tools/manifest_check.py` refuses anything unclassified.

So the order of work is fixed, and it is not the order that feels natural:

1. **Ask.** Find out whether HAR has an API, a data-sharing arrangement, or a
   stated licence, and if it does not, ask them directly. This is an email
   Adam or the organisation sends, not a crawl anybody runs. Precedent inside
   this project: the rKTs abbreviation list came in CC BY 4.0 because
   Dr. Bruno Laine was asked and said yes in writing, and that permission is
   recorded at `docs/licenses/RKTS_ABBREVIATIONS.md`.
2. **Then decide the lane** — ingest, link-out, or never-ship — the same three
   lanes every other reference layer sits in. LINK-OUT is the likely answer
   and it is not a consolation prize: a deep link from a text to its
   iconographic entry costs nothing, ships nothing, and breaks no rights.
3. **Only then** ask what it would attach to. The honest candidates are the
   catalog (a work -> the images of its subject), the deity and lineage names
   that already appear in the corpus, and the Sanskrit/Tibetan name forms the
   converter already handles.

What it would be FOR, so this is not scope for its own sake: a student reading
a text about a figure they cannot picture is the ordinary case, and nothing in
the tool answers it today. That is a real gap. It is still gated on step 1.

## Jev AI for finishing the dictionary — UNIDENTIFIED (Adam, 2026-09-22)

> "Is there some way to use the new Jev AI to complete the dictionary in a
> more time and cost efficient way?"

**I do not know what Jev AI is and did not guess.** It is not something this
project has used, nothing in the tree mentions it, and it is not a system I
can describe from knowledge. Rule 3 applies to tools as much as to syllables:
a confident answer about a product I cannot identify would be worthless and
would sound exactly like a useful one.

**Waiting on Adam for one thing only: what it is** — a link, a vendor, or
where it was seen. Then it can be assessed properly.

What the assessment will have to answer, recorded now so it is not
re-invented later, and it is the same test any tool faces here:

- **Rule 1 is not negotiable.** `hgm_gloss` carries Geshe Michael's own
  English. A machine may MATCH it from corpus evidence and may never COMPOSE
  it. Any tool that would generate English equivalents is answering a question
  this project does not ask, however cheaply it answers it.
- **Where the actual cost is.** The expensive half of "completing the
  dictionary" is not generation — it is ATTESTATION and human ruling. 12,004
  of 105,634 entries carry an HGM gloss; the 79,305 Lokesh Chandra entries are
  awaiting, and what they await is a decision, not text.
- **The standing constraint**: no paid API generation, and the app imports a
  dictionary release rather than owning one (CLAUDE.md). A tool that produces
  data would be producing it for the DATA project, not for this repo.
