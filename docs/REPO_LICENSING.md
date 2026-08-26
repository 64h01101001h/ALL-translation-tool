# Repository licensing ledger

*What the DMG's payload manifest is to the disk image, this is to the
public GitHub repository: one row per bucket of tracked files, with its
terms and a status. `tools/repo_license_check.py` fails the battery if
any tracked file matches no row — adding material to the repo costs a
ledger row, deliberately. Created 2026-08-26, the day a licensing audit
found three in-copyright book texts public in this tree; the ledger
exists so that class of surprise has to pass a gate to happen again.*

Statuses: **own** (ALL/ACIP work, root LICENSE applies) ·
**settled** (third-party, terms verified and recorded) ·
**open** (terms not yet settled — listed, tracked, and honest).

| prefix | status | terms |
|---|---|---|
| core/ | own | ALL/ACIP, root LICENSE |
| app/ | own | ALL/ACIP, root LICENSE |
| ocr/ | own | ALL/ACIP, root LICENSE |
| tools/ | own | ALL/ACIP, root LICENSE |
| engines/ | own | canonical Python engines, ALL/ACIP |
| cmake/ | own | build glue |
| build/ | own | tracked build stubs |
| docs/ | own | project documentation |
| VERSION | own | — |
| README.md | own | — |
| TODO.md | own | — |
| LICENSE | own | the licence text itself |
| DIAMOND_CUTTER_TRANSLATION_TOOL_ROADMAP.md | own | — |
| .gitignore | own | — |
| Brewfile | own | dependency list |
| CHANGELOG.md | own | — |
| CLAUDE.md | own | project instructions |
| CLAUDE_CODE_BOOTSTRAP.md | own | — |
| CMakeLists.txt | own | build definition |
| .claude/skills/ | open | obra/superpowers MIT and levnikolaevich MIT — verified via GitHub API 2026-08-26, licence texts banked in .claude/skills/THIRD_PARTY_LICENSES/. Still open because anthropics/skills publishes NO licence, so those skills sit here on unrecorded terms. See S-1. |
| .claude/ | own | project settings, commands, hooks |
| third_party/kck_henning/ | settled | MIT, verbatim in every source header (read 2026-08-26); © 2009-2013 Edward Henning |
| third_party/tcg_henning/ | settled | MIT, verbatim in every source header; © Edward Henning |
| third_party/espel_lingua_bo_wylie/ | settled | Perl Artistic/GPL dual, © 2008-2010 Roger Espel Llima (stated in sources) |
| data/84000/ | settled | CC BY 4.0, 84000 project; see bucket README |
| data/abbreviations/ | own | in-house lists, sources cited inline |
| data/acip_codes/ | own | ACIP's own coding standard |
| data/ai_glossary/ | own | AI-provisional tier, in-house; README |
| data/botok/ | settled | Apache-2.0 derived (botok project); README |
| data/das/ | own | .gitignore only — no content tracked |
| data/extracted/ | open | 1,954 files under SHIP.md's blanket "SHIPS-WITH-ATTRIBUTION, pending the per-file itemization". Open item S-2 — the largest unfinished licensing job in the repo. |
| data/fonts/ | settled | OFL fonts with licence texts beside them; FONTS.md |
| data/goldstein/ | settled | derived factual findings only; source text removed from tree AND history 2026-08-26 (README) |
| data/hackett/ | settled | derived factual findings only; source text removed 2026-08-26 (README) |
| data/manual_authentic/ | settled | derived factual findings only; source text removed 2026-08-26 (README) |
| data/help/ | own | in-app help, house style |
| data/honorifics/ | own | in-house findings; README |
| data/hypercontext_legacy/ | own | ACIP's own legacy materials; README |
| data/idioms/ | own | in-house register, started 2026-08-10 |
| data/mahavyutpatti/ | settled | 9th-century text, public domain; README |
| data/mst/ | settled | 43 factual rows citing Tournadre & Sangda Dorje, Manual of Standard Tibetan, App. 3 — facts with citation, from Adam's own copy. Flagged for counsel as a judgement call (S-3), not treated as infringement: facts and short taxonomies are not the book's expression. |
| data/pron_colloquial/ | own | in-house; README |
| data/proposals/ | own | translator proposals, in-house |
| data/PENDING_REVIEW.md | own | empty review queue |
| data/PENDING_REVIEW.docx | own | ditto |
| data/reports/ | own | generated reports |
| data/research/ | own | in-house notes |
| data/rkts/ | open | abb.xml from rKTs (Vienna) — terms not yet verified at source. Open item S-4. |
| data/soas_pos/ | settled | CC BY 4.0, verified against Zenodo record 574876 on 2026-08-26; attribution in notices |
| data/spellcheck/ | settled | tibetan-spellchecker, CC0 (per that project); plus in-house exception lists |
| data/teaching/ | own | the org's own recorded teachings (captions only, no media). Formally gated on SHIP.md item 3, Adam's consent line — his own to give. |
| data/whitney/ | settled | Whitney 1885, public domain by age |

## Open items

- **S-1 · .claude/skills — half closed 2026-08-26.** Both MIT licence
  texts now travel with the copies (THIRD_PARTY_LICENSES/). Still open:
  anthropics/skills publishes NO licence, so those skills sit in a
  public tree on unrecorded terms. Adam's options: ask, or move them out
  of the public tree.
- **S-2 · data/extracted itemization.** 1,954 files under one blanket.
  The blanket was written for the DMG; the public repo inherits it.
  Needs the per-file pass the blanket has always promised.
- **S-3 · data/mst counsel note.** Defensible as cited facts; a lawyer
  should say so once.
- **S-4 · rKTs terms.** Verify at source, then move the row to settled.

*Corrections to this ledger belong in the row, not a side channel — a
reader must never trust a row the checker no longer enforces.*
