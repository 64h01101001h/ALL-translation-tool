# Auto-linking our texts to BDRC scans — design note
*(banked 2026-08-06; Adam's request: link our ACIP files to the
corresponding pecha scans on BDRC, using BDRC's APIs where/when we can)*

## Why this is very feasible
Our ACIP file names already carry the linking key. The Release IV
nomenclature (decoded by `allcore::decodeAcipFilename`) gives us, for
Kangyur/Tengyur texts, the **Tohoku catalog number** (e.g. `TD4211…` =
Derge Tengyur, Tohoku 4211) — and BDRC's BUDA database cross-references
Tohoku numbers on its work records. So the chain is:

    ACIP filename → collection + Tohoku number → BDRC work ID (bdr:…)
                  → scans page / IIIF manifest → images in-app or in-browser

## BDRC's API surface (verified to exist, 2026-08-06)
- **purl.bdrc.io** — the Linked Data Server (`buda-base/lds-pdi`): SPARQL
  plus templated query endpoints (e.g. `/query/table/<template>`),
  JSON-LD by default, content-negotiated. Resource URLs are persistent
  (`purl.bdrc.io/resource/<ID>`).
- **IIIF** — BUDA serves scan images via IIIF (presentation manifests per
  volume; standard Image API tiles), so any IIIF viewer — or a simple
  QImage fetch of a page URL — can display scans.
- **library.bdrc.io** — the human-facing library; `show/bdr:<ID>` pages
  are the right "View on BDRC" browser target.

## Transliteration boundary (Adam's note, 2026-08-06)
**BDRC works in Wylie (EWTS — their language tag is `bo-x-ewts`); our file
names and texts are ACIP.** Every name-based interaction with BDRC must
therefore cross the boundary through the battery-proven converter:

    our ACIP  →  allcore::acipToEwts  →  EWTS  →  BUDA query (bo-x-ewts)

This is already exact machinery (100.000% verified on all 37,807 corpus
conversions), so the cross-reference costs nothing — the rule is simply:
NEVER send raw ACIP to a BDRC endpoint; convert first. (The Tohoku-number
links shipped in stage 1 sidestep this entirely — numbers need no
transliteration — but the Sungbum title search and any SPARQL name lookup
depend on it.)

## Build plan
1. **Probe (first step, small):** live-test the templated queries /
   SPARQL for "work by Tohoku number" against a handful of knowns
   (Heart Sutra, Pramāṇavārttika = Tohoku 4210, etc.); identify the exact
   identifier predicate BUDA uses; cache findings here.
2. **Resolver in core:** `bdrcResolve(collection, tohoku)` → work ID +
   scans URL (+ IIIF manifest URL), with a tiny on-disk cache
   (library/.bdrc_links.db) so lookups are one-time. Network use is
   user-initiated (rule: the deterministic core stays offline; this is a
   lookup convenience, clearly network-labeled).
3. **Library integration:** the info panel for a recognized K/T file gains
   a "View scans on BDRC" link (browser) once resolved; unresolved =
   honest "no BDRC match found".
4. **Later:** an in-app scan viewer (IIIF page images beside the ACIP
   text — pecha image over e-text, the classic proofreading layout), and
   the OCR pipeline (docs/OCR_DESIGN.md) consuming those same IIIF images.
   Sungbum texts lack Tohoku numbers; matching those needs title-based
   search (BUDA name query templates) — flagged fuzzy, user-confirmed,
   never auto-asserted.

## Sources
- https://github.com/buda-base/lds-pdi (+ its API.md)
- https://www.bdrc.io/buda-archive/ (IIIF + Linked Open Data architecture)
- https://github.com/buda-base/public-digital-library (library.bdrc.io)
