# Open from today — 2026-09-10

*Everything begun today that is not finished, in the order I would take them.
Written after Adam asked for a running list. Anything marked DONE is here only
so the list is complete and nothing looks forgotten.*

## 1 — Commit the day (5 minutes, and it should be first)

**52 files uncommitted on `main`.** A whole day of work — the audio core, the
iOS project, the Quick Access fixes, the two honesty fixes — exists only in the
working tree. Suites are green. This is pure risk with no upside.

## 2 — The audio makes no sound yet

The core is finished and proven: 3,002 recordings labelled and indexed,
`allcore::SpeakBank` resolving syllables with honest tiers, battery-proven
against the Python oracle across 11,939 syllables, 96.5% corpus coverage, the
two-build split proven both ways. **But nothing in the app plays anything.**
Qt Multimedia is not linked; there is no surface.

- **a. Right-click any Tibetan → Pronounce.** Adam's primary ask. Needs the
  playback service (decode-to-PCM over 22.3 MB, driven through QAudioSink —
  chaining QMediaPlayer gives audibly variable start latency).
- **b. The Kawachen reader, recreated offline.** Adam's explicit ask, with
  their own spec captured: the Alphabet syllable builder, the History roster
  you can replay, and the listening Quiz. Placement decided: builder and quiz
  into the Learn tab beside Drills and Trainer; the roster becomes the
  builder's own playlist.
- **c. Settle the Qt Multimedia dependency deliberately** — the FFmpeg backend
  puts LGPL/GPL dylibs into a DMG whose licensing manifest is currently clean.
  Decide before it ships, not after.

## 3 — The drill's real defect: 72.2% guessable without reading

Measured over the 4,000-drill pack: in 72.2% of drills exactly one option's
case particle fits the slot, so the answer can be picked without reading the
Tibetan or the English. Only 22.6% require meaning. Distractors are drawn as
"marked chunks from other random segments" with no requirement to share the
answer's marker (`core/src/drills.cpp:212`).

This is the single highest-value change in the app, and it explains Adam's own
report — answering correctly and still not knowing what the chunk meant.

## 4 — What Adam asked for on the drill card

Designed, not built. Both apps.

- **The blanked word's meaning after answering**, via `allcore::checkTerminology`
  — which already exists and already has a battery, so this is a call site, not
  new engine code. Shown identically whether the answer was right or wrong.
  Evidence check passed: his equivalents hit his English for THIS segment 50.3%
  of the time against 14.6% for a random other segment — +35.7 points over
  chance, so the match is real and not coincidence.
- **A hint button** — "Rule out one". Revealing the grammatical role instead
  would solve 48–65% of drills outright, so the weak hint is the right one.
- **A lookup button, reachable only after answering.** Before answering it
  removes the retrieval effort that makes a drill work.
- **Red/green on the English: recommend against.** Colour already carries tier
  meaning here, colour alone fails WCAG, and the item above is what was
  actually wanted.

## 5 — Nineteen top-level menus

Twelve hand-written plus seven generated one-per-pane-group
(`app/main.cpp:41789`), duplicating a ribbon and a tab bar that already do that
job. macOS expects 6–9. Design in progress.

**Constraint found before touching it:** saved Quick Access pins resolve by
walking the menu bar three levels deep, so any change to depth or a top-level
name breaks every saved pin. A migration or a resolver shim is part of the job.

## 6 — Divergence between the desktop and the phone

Two fixed today (the unattested verb, the clause-coverage disclosure). Still
open, from the audit:

- **Blank placement is implemented twice** — `app/main.cpp:22415` and
  `tools/build_drill_pack.cpp:95` — and is not in `allcore`. They already
  differ on failure: the desktop explains, the packer drops the drill silently.
- **The phone re-sorts the reading plan** the Mac computed; the desktop renders
  it in chunk order.
- **A nested-bracket bug in the Swift** would show translator-supplied text as
  Geshe Michael Roach's own English. Real code, zero instances in the shipped
  pack — latent, not live.
- The desktop Trainer and the iOS Trainer are **different programs sharing no
  code**. Worth deciding whether that is intended.

## 7 — Smaller, real

- **His English plaque is `background:#EEF6EE` with no text colour**, in four
  places. In Night mode the default light text may land on a near-white ground.
  Unverified — check before assuming.
- **`app_selftest` times out under `ctest -j4`** but passes alone in 270s. The
  default timeout is too tight; the suite reports a failure that is not one.
- **TestFlight upload** — the paid programme is active and the app is signed
  and installed on Adam's phone, but nothing has been uploaded for testers.

## Filed to the backlog, correctly not started

- A colloquial ("spoken") Tibetan trainer. None of the audio we hold is
  colloquial speech — it is isolated literary syllables — so this needs its own
  recordings by a speaker of the modern language.
- The remaining audio ideas beyond the read-aloud: search by sound, the
  minimal-pair ear trainer, the big-type reading mode.

## Done today, listed so nothing looks lost

Kawachen harvest, labelling and renaming · the engine cross-check · the C++
speak core and its battery · the two-build split · the Xcode project and the
app on the phone · one-command redeploy · the pack build stamp · Quick Access
right-click pin/unpin/reorder · the two iOS honesty fixes · `available()` now
proving a file opens rather than counting rows · the digest.
