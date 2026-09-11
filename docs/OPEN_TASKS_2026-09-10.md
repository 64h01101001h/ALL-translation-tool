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

- **The blanked word's meaning after answering** — **BUILT, both apps** — via `allcore::checkTerminology`
  — which already exists and already has a battery, so this is a call site, not
  new engine code. Shown identically whether the answer was right or wrong.
  Evidence check passed: his equivalents hit his English for THIS segment 50.3%
  of the time against 14.6% for a random other segment — +35.7 points over
  chance, so the match is real and not coincidence.
- **A hint button** — "Rule out one". **BUILT, both apps.** Revealing the grammatical role instead
  would solve 48–65% of drills outright, so the weak hint is the right one.
- **A lookup button, reachable only after answering** — **BUILT 2026-09-11,
  both apps.** The phone expands the equivalents its pack carries; the desktop
  opens the floating dictionary on the blanked word, so the drill is not
  navigated away from mid-session. Five gates hold the *sequence*, which is the
  whole design: out of reach before the answer, offered after it, the hint
  withdrawn once there is nothing left to hint at, and out of reach again on
  the next drill.
- **Red/green on the English: recommend against.** Colour already carries tier
  meaning here, colour alone fails WCAG, and the item above is what was
  actually wanted.

## 5 — Nineteen top-level menus — **DONE 2026-09-11**

Was twelve hand-written plus one generated per pane group, so the bar grew a
menu every time a group was added. Now **eleven, fixed**: File · Edit · Panes ·
Find · Insert · Format · Tools · Goto · View · Window · Help.

- **Selection → Edit.** Every item acts on the current editor's selection.
  Nothing in it was pinnable (only commands inside submenus could be pinned,
  and Selection had none), so the move strands nothing.
- **Project → File.** A dossier is opened, recent-listed, saved and closed —
  File's whole vocabulary.
- **Every pane group → one "Panes" menu**, each group's name carried as a
  greyed header rather than another submenu. A submenu per group would have
  put every pane command four levels down and added a hover-and-wait to
  reaching any of them; as a header the depth is unchanged, so a pin made
  before the regrouping has the same shape after it.

**The constraint was real and is handled.** Pins resolved by walking exactly
three levels, so anything deeper would have been invisible and every such pin
would have reported itself dead. The resolver now walks the whole bar to any
depth (`qatWalk`), and a one-time migration rewrites a stale pin **only when
its command name matches exactly one command in the new tree** — two matches
or none and it is left to show as "not in this build", because a pin that
quietly starts running a *different* command is worse than one that says it is
broken. Five gates cover it, including that a moved command resolves at its
new path.

## 6 — Divergence between the desktop and the phone

Two fixed today (the unattested verb, the clause-coverage disclosure). Still
open, from the audit:

- **Blank placement is implemented twice** — `app/main.cpp:22415` and
  `tools/build_drill_pack.cpp:95` — and is not in `allcore`. They already
  differ on failure: the desktop explains, the packer drops the drill silently.
- ~~The phone re-sorts the reading plan~~ — **FIXED 2026-09-11.** The Mac hands
  the plan over in chunk order with each step carrying where it is read; the
  phone sorted by that number and silently turned it into a different reading.
  Same data, different lesson.
- ~~A nested-bracket bug in the Swift~~ — **FIXED 2026-09-11.** The flush at an
  opening bracket carried `false` instead of the depth it was leaving, so in
  "[a [b] c]" the text "[a " rendered in ordinary ink — and ordinary ink here
  means *this is his own English*. Proved against the desktop's
  `englishWithSupplied` on eight cases, including that an unterminated bracket
  errs toward "supplied" rather than claiming it as his. The old code was run
  against the same cases to confirm it genuinely failed them.
- The desktop Trainer and the iOS Trainer are **different programs sharing no
  code**. Worth deciding whether that is intended.

## 7 — Smaller, real

- ~~His English plaque sets a background with no text colour~~ — **FIXED
  2026-09-11**, and it was worse than four places. Eight plaques, plus eight
  more grounds found by the new rule: a white breadcrumb strip, a near-black
  overview panel (the same defect pointing the other way — dark text on
  near-black in Day mode), two legend swatches, a gloss highlight, a footnote
  chip, an AI-diff banner and a library warning. Constitution rule **R9** now
  refuses any hand-written background that has no text colour in its block,
  because on a theme-independent ground the ink must be fixed too.
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
