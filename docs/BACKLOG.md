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

Shape when it starts: a thin SwiftUI shell over `allcore` compiled for arm64,
a generated slice of the spine shipped in the bundle, and the deck synced
through the same Dropbox folder the team already uses.
