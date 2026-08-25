# SHIPWRIGHT ENGINE — LEDGER (v1.0 to market)
Per docs/SHIPWRIGHT_PROMPT.md. Stamps: SHIPS · SHIPS-WITH-ATTRIBUTION ·
MUST-REMOVE(market) · GATED-ON-GRANT · DEFERRED · DROPPED.

## L1 — THE DISTRIBUTION MANIFEST (first audit, 2026-08-20)
Read from tools/package_macos.sh staging (the manifest of record):

| Payload | Source / license | Market verdict |
|---|---|---|
| hgm_spine (dictionary+corpus) | ACIP/HGM data project (Adam's org) — includes the Lokesh Chandra layer extracted from Ven. Phil's payloads | **GATED-ON-GRANT**: one written line from Adam on the release package's redistribution stance; LC layer's provenance chain reviewed |
| build/reference.db | The "REFERENCE · LOCAL ONLY" compilations — the card itself says "unlicensed compilations — never for release data" | **P0 · MUST-REMOVE(market)** — team DMG only. FIXED: press gains a mode; market mode omits it |
| library/ocr_models | BDRC, CC BY-**NC** 4.0, verbal team permission | **P0 · MUST-REMOVE(market)** unless written distribution permission is banked; the app already has a download-it-yourself fallback, which becomes the market path. FIXED: market mode omits |
| data/das (Das 1902 PDF) | Published 1902 — public domain | SHIPS |
| teaching_moments_card.json | Derived index (timecodes+snippets) of the org's own recorded teachings | GATED-ON-GRANT (in-house consent — Adam confirms in one line) |
| fonts (Noto Serif Tibetan, Jomolhari: OFL; BabelStone: Free) | OFL 1.1 / freeware | SHIPS-WITH-ATTRIBUTION (OFL text must ride along — L2) |
| data/abbreviations (TibSchol, rKTs) | CC BY 4.0 (grants banked with the files) | SHIPS-WITH-ATTRIBUTION |
| data/spellcheck | tibetan-spellchecker, CC0 | SHIPS |
| data/extracted (84000 glossary CC BY · Monlam lists Apache-2.0 · verb banks CC0 · concordances derived from public catalogs · apparatus anchors from the org's own publications) | **itemised 2026-08-23** in `docs/distribution/PAYLOAD_MANIFEST.txt`: 33 files ship, 23 are `drop` and no longer reach a DMG (~62 MB, incl. 1,926 raw THL catalogue records and the BDRC RDF dumps — none read by `app/` or `core/`). 20 of the 33 still carry status `itemise`: they ship under this row's blanket and have no per-file terms recorded | SHIPS-WITH-ATTRIBUTION for the 33; a **market** press now FAILS on the 20 `itemise` rows and prints them |
| data/botok · soas_pos · whitney | botok-derived (Apache-2.0) · SOAS materials (**verify terms**) · Whitney 1885 (public domain) | soas_pos: GATED until verified; others SHIPS-WITH-ATTRIBUTION |
| Qt frameworks (macdeployqt) | LGPLv3, dynamically linked | SHIPS with LGPL notices in L2's licenses surface |
| Vendored: SQLite (public domain) · diff_match_patch (Apache-2.0) | — | SHIPS-WITH-ATTRIBUTION (Apache notice) |
| tools/build_spine.py (rides along) | in-house | SHIPS |

### Actions taken this wave
- **The press is now mode-aware**: `package_macos.sh [team|market]`
  (default team — today's behavior unchanged). Market mode OMITS
  reference.db and the OCR models and stamps the README accordingly.
  The two P0 payloads can no longer reach a market DMG by habit.

> **Correction, 2026-08-23 (SQA BUILD-5).** "Stamps the README
> accordingly" was not true when it was written. The heredoc that
> writes `README.txt` had no mode conditional and `PRESS_MODE` never
> appeared near it, so every DMG — team and market alike — carried the
> same eight lines of install instructions with no word about
> redistribution, inside a 424 MB file anyone can forward. It is true
> now: the press appends a mode block (team: "TEAM COPY — NOT FOR
> REDISTRIBUTION", naming `build/reference.db`; market: a line saying
> reference.db is omitted) and then re-reads the file it just wrote
> and fails the press if the stamp is not in it
> (`manifest_check.py --readme`). Also corrected in the same wave:
> `tools/build_reference_db.py`'s header said "The DB never ships"
> while 80,756,736 bytes of it shipped in every team DMG.

### Open, needing humans (surfaced per §10)
1. Adam: one-line written stance on redistributing the HGM release
   package (and the LC layer's chain) in a market build.
2. Adam: pursue BDRC WRITTEN distribution permission, or bless the
   download-on-demand market path (models fetched by the user).
3. Adam: confirm the teachings-index consent line.
4. SOAS POS materials: locate the original terms before any market DMG.
5. Apple Developer ID decision ($99/yr) for D2 notarization.
6. **These two also gate PUBLIC SOURCE HOSTING, not just a market
   DMG.** Both files are tracked in git and present throughout the
   history, so a public repository redistributes them to anyone who
   clones or forks — more broadly than a DMG, and not recallable:
   - `data/teaching/teaching_moments_card.json` (13.2 MB) — item 3
   - `data/soas_pos/classical-lexicon.txt` (0.6 MB) — item 4
   Decision 2026-08-25 (Adam), REVISED SAME DAY: first private, then
   **PUBLIC** at https://github.com/64h01101001h/ALL-translation-tool
   so the link could be shared with ALL's technical lead. Adam was
   told twice what publishing these two files means and chose it; that
   is his call to make and it is recorded here as his, not as an
   oversight.
   What that means now, stated plainly so nobody has to reconstruct it:
   the teachings index and the SOAS lexicon are published. Item 3 is
   Adam's own confirmation to give, so publishing effectively gives
   it. **Item 4 is not his to give** - the SOAS terms are unverified
   and belong to a third party, so that file is public under terms
   nobody has read. Closing item 4 is now urgent rather than
   pre-market, and if the terms turn out to forbid redistribution the
   fix is removal from the tree AND from history, plus asking GitHub
   to purge forks.
   Partial mitigation: the root LICENSE is all-rights-reserved
   (c) Asian Legacy Library / ACIP, so visibility is not a licence
   grant. Making it public before then repeats BUILD-5 — notices
   saying material is not redistributed while it ships — except a
   public git history cannot be withdrawn. Clearing items 3 and 4 is
   the whole cost of going public; nothing else in the repo blocks it
   (history carries no credentials — checked 2026-08-25).


## THE FREE-DISTRIBUTION RULING (Adam, 2026-08-22)
**The Diamond Cutter Translation Tool is distributed FREE OF CHARGE and is not
sold.** It is given to translators, monastic input centers, and
students of the tradition. This is the decision the licensing track
was waiting on, and it resolves the manifest's hardest question.

Consequences, applied:
- **BDRC OCR models (CC BY-NC 4.0 + BDRC's direct okay, banked
  2026-08-07): SHIP IN BOTH MODES.** Free, attributed, non-commercial
  redistribution is what the NC license permits; the banked note
  calls this case "doubly covered." The press no longer strips them
  from market builds. Written confirmation is still being sought —
  it upgrades certainty, it does not unlock anything.
- **reference.db still MUST-REMOVE from market builds** — unchanged,
  and for a different reason: those are unlicensed compilations the
  card itself labels "local lookup only, never for release data."
  Commercial-vs-free was never the issue there.
- Everything else on the manifest is unaffected: OFL fonts, CC BY
  layers (84000, rKTs, SOAS), CC0 data, Apache/PD vendored code, and
  the public-domain scans all permit free redistribution with
  attribution, which the licenses surface provides.
- Apple Developer ID (#27) remains a separate decision: notarization
  costs $99/yr whether or not the tool is sold.

## L2 — THE PAYLOAD MANIFEST (2026-08-23, SQA BUILD-2/4/5)

The distribution manifest above was prose. It is now also a table the
press reads and the battery checks: `docs/distribution/PAYLOAD_MANIFEST.txt`,
one row per path that may be in a DMG, with mode (both/team/drop),
status (ok/itemise/gated), licence, and the anchor in
OPEN_SOURCE_NOTICES.md that backs it.

- The press **stages from** it (`--list-data-dirs`), **prunes to** it
  (`--prune-extracted`), **stamps and verifies** the README
  (`--readme`) and is **gated on** it (`--press`, step 6d, the last
  thing to touch the stage before `hdiutil`).
- The constitution's **L2 rule is now that check** (`--static`): every
  anchor a row claims must really be in the notices, the press must
  still call all four entry points, and nothing may sit in
  `data/extracted` unclassified. The rule it replaced credited the
  whole of `data/extracted` — 102 MB, 3,219 files — because the string
  "84000" occurs somewhere in the notices.
- `tools/bundle_licenses.py` writes
  `Contents/Resources/licenses/BUNDLED_COMPONENTS.tsv` (component ·
  version · SPDX · licence texts · object count) from the staged
  bundle and **fails the press on any Mach-O it cannot attribute**.
  First run: 165 objects → 35 upstream projects, against the six the
  notices used to name.
- Qt's **virtual-keyboard plugin is no longer bundled** — Homebrew
  records `qtvirtualkeyboard` as `GPL-3.0-only AND Apache-2.0 AND
  BSD-3-Clause`, the one object in the bundle with strong copyleft and
  no alternative and no linking exception. A desktop tool with a
  hardware keyboard never selects that input context.

### Still open after this wave (needs humans)
6. **LGPL/GPL licence texts for Qt.** Homebrew's `qt`,
   `qtimageformats` and `qtvirtualkeyboard` kegs carry no licence file,
   so the DMG still ships Qt without the LGPL-3.0 text that LGPL-3.0 §4
   requires. `bundle_licenses.py` records `LICENSE_TEXT_MISSING` on
   those rows and prints them every press; drop the official texts at
   `docs/licenses/texts/LGPL-3.0-only.txt` (and `GPL-3.0-only.txt`,
   `GPL-2.0-only.txt`) from <https://www.gnu.org/licenses/> and they
   ship automatically. A market press with `--require-texts` refuses
   until they are there. **Nobody should transcribe these from memory.**
7. **The 20 `itemise` rows** under `data/extracted` — per-file terms,
   which is what blocks a market DMG now that everything else is
   settled.
8. **A project `LICENSE`.** One is now in the repo root stating only
   what this repo already records (the free-distribution ruling of
   2026-08-22, the team-only status of the ALL's data). It has not
   been reviewed by Adam or by counsel; it is a starting point, not a
   grant.
