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
