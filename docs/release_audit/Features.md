# Features — show or hide panes and groups (Preferences ▸ Features)

*Added 2026-09-09 at Adam's request: "all available features/tools/suites/functionality displayed or not displayed … so that users can turn off any features they don't use often and their GUI won't be crowded." Files: the page in `PreferencesDialog` (`app/main.cpp`), the registry and applier beside the pane groups in `main()`.*

## 1. Justification (steelman before test)
The suite now carries 25 panes in seven groups. A translator who only reads and drafts meets twenty surfaces they will never open, and an input-centre operator meets the translator's. Crowding is not cosmetic: it makes the pane bar a search problem and hides the two buttons a person actually uses. Every serious editor solves this by letting the person choose what is on screen.

The risk of a visibility switch is that a feature becomes unreachable and the person cannot work out why. That is answered below rather than by not building it.

## 2. What it does

| Function | Behaviour | Evidence |
|---|---|---|
| The list | Built from the panes that actually exist at run time, group by group, so it can never drift from the build or list something that is not there | selftest: the offered list equals the live pane registry |
| A pane off | Its button leaves the ribbon's pane bar. If it was the pane on screen, the first still-shown pane in its group takes its place | selftests (leaves, comes back) |
| A group off | The group's tab hides, and with it every pane in the group | selftest |
| Nothing left on | Refused: everything stays shown, because a window with nothing in it is not a preference, it is a fault | selftest |
| Workflows | Six ways of working — Translating, Editing, Reading and study, Cataloguing, Input and proofreading, Learning Tibetan — as tick boxes. Ticking several turns on the union of what they need. They only move the switches below; nothing is written until Apply, and every switch stays the person's afterwards | selftest: every shipped workflow names groups this build has |
| Save these choices as a workflow… | Names the current set of ticks and adds it to the list, stored per person | — |
| Show everything again | One button restores every switch on the page | — |
| Where it is stored | `features/pane/<name>` and `features/group/<name>` per person; default on, so a new pane appears for everyone until they turn it off | — |

## 3. Honesty
- The page says, in the first sentence a person reads: nothing is deleted, nothing is lost, the feature keeps working, its files are untouched.
- **Hiding is a view, not a removal.** A hidden pane stays reachable from the menus, so a feature used twice a year can be used twice a year without being switched back on. That is stated on the page.
- The refusal to hide everything is stated on the page before it can be hit, not only when it fires.
- **A workflow is a convenience, not a claim.** The shipped six are coarse, by whole group, and the page says they are a starting point rather than a rule about how anyone works. What each role actually needs on screen is what the editor's session and the translators' one-on-ones are for; those will correct these.

## 4. Open
- The shipped workflows are group-level guesses until the role interviews correct them.
- Ribbon groups within a pane, and individual ribbon buttons, are not switchable yet — only whole panes and groups. That is the next level of the same idea and is a bigger surface, so it waits for Adam's word on whether pane-level is enough.
- Whether hidden panes should also drop out of the menus. Today they do not, deliberately (see Honesty). Adam's ruling.
