# The idiom bank and register — release audit

*Added 2026-09-09 at Adam's request: "a bank of previously used idioms, and a way of adding new ones to the idiom register that need to be approved by Geshe Michael. Idioms can be big in our translation group — I feel like Geshe Michael uses them a lot." Files: `core/include/allcore/idioms.h`, `core/src/idioms.cpp`, `core/tests/idioms_smoke.cpp` (17 checks), `app/idioms.inc`; data `data/idioms/idioms.json` (shipped seed, started 2026-08-10) and `<dataRoot>/library/idioms.json` (the team register).*

## 1. Justification (steelman before test)
An idiom is the one thing a word-by-word reader gets exactly wrong. The group's English is Geshe Michael's, and he uses fixed expressions heavily, so two questions come up daily: is this string an idiom, and how has he rendered it before? Both were answered from memory. The register answers the first and the corpus answers the second.

The register file already existed with the rule written into it and nothing reading it. This closes that gap rather than inventing a scheme.

## 2. The line this feature does not cross
The register marks WHICH Tibetan forms are idioms. It holds no English of its own. English is Geshe Michael's (rule 1): the app may match his English from corpus evidence, never compose it. So an entry is a Tibetan form, a status, and its evidence. The renderings shown beside an idiom are fetched at the moment of asking from his own parallel corpus, counted, and labelled his. A translator's English lives in exactly one place — the payload of a proposal awaiting his ruling — and is labelled as theirs wherever it appears.

## 3. What it does

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| `idioms::Register` + longest-match walk | core | Forms are matched over syllables, so `pa` never matches inside `pad ma`; the longest registered form wins; an idiom is consumed whole, so repeats are counted once each; a form ruled NOT an idiom is kept as a record but never marked again | idioms_smoke 1–10 (17 checks) |
| Two register files | `data/idioms/idioms.json` read-only seed, `<dataRoot>/library/idioms.json` team register | The team file loads second, so a ruling always overrides the seed and survives a release that rewrites it. The app never edits `data/` | selftest (seed forms present, no ruling invented) |
| The bank | Draft ▸ EVIDENCE ▸ **Idioms**, and **Tools ▸ Idioms…** from any pane | Filter, status filter, "only the forms in the open text", occurrence counts, and per form: who put it forward and why, the ruling and its note, and Geshe Michael's own renderings from his courses with their count and context | selftests |
| Propose | **Propose an idiom…** | Files a proposal of kind `idiom` into the shared folder with the form, the translator's English (labelled theirs), the passage as evidence, and who proposed it. Refused, with the reason, when no name or shared folder is set | selftests |
| Bring across rulings | **Update from rulings** | Reads the shared proposals folder; approvals and declines land in the team register with his name and date; a pending proposal changes nothing and is counted as still waiting | selftest (pending → nothing; ruled → approved with his name) |

## 4. Honesty
- A form with no ruling reads "awaiting Geshe Michael's ruling" everywhere, and the detail says it is never presented as settled.
- The bank states that it lists the register's forms, not every idiom in the text.
- An empty corpus result says the corpus is 42,199 segments of the courses we hold, not everything he has said — absence is not evidence against the idiom.
- An unreadable register file is reported, never read as an empty list.

## 5. Open
- Marking idioms inline in the Overlay's reading view (they are listed and counted today, not highlighted in the text).
- The matcher is its own small trie; when the Entities automaton lands (batch 5 F6) both should share one machine.
