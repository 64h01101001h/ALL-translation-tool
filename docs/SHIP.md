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
| data/extracted (84000 glossary CC BY · Monlam lists Apache-2.0 · verb banks CC0 · concordances derived from public catalogs · apparatus anchors from the org's own publications) | itemized pass pending | SHIPS-WITH-ATTRIBUTION, pending the per-file itemization (next L1 wave) |
| data/botok · soas_pos · whitney | botok-derived (Apache-2.0) · SOAS materials (**verify terms**) · Whitney 1885 (public domain) | soas_pos: GATED until verified; others SHIPS-WITH-ATTRIBUTION |
| Qt frameworks (macdeployqt) | LGPLv3, dynamically linked | SHIPS with LGPL notices in L2's licenses surface |
| Vendored: SQLite (public domain) · diff_match_patch (Apache-2.0) | — | SHIPS-WITH-ATTRIBUTION (Apache notice) |
| tools/build_spine.py (rides along) | in-house | SHIPS |

### Actions taken this wave
- **The press is now mode-aware**: `package_macos.sh [team|market]`
  (default team — today's behavior unchanged). Market mode OMITS
  reference.db and the OCR models and stamps the README accordingly.
  The two P0 payloads can no longer reach a market DMG by habit.

### Open, needing humans (surfaced per §10)
1. Adam: one-line written stance on redistributing the HGM release
   package (and the LC layer's chain) in a market build.
2. Adam: pursue BDRC WRITTEN distribution permission, or bless the
   download-on-demand market path (models fetched by the user).
3. Adam: confirm the teachings-index consent line.
4. SOAS POS materials: locate the original terms before any market DMG.
5. Apple Developer ID decision ($99/yr) for D2 notarization.


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
