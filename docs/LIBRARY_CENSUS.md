# Library Health Census — full collection
*2026-08-11. Every file opened through the real Overlay pipeline
(`--openprobe`), 90s timeout. Method + caveats at bottom.*

## Headline

- **11,700 files, all opened** (the 1,439 "no-stats" rows are files
  the dictionary matcher found nothing in — predominantly English
  translation files (`*_E.TXT`), incomplete-input files, and other
  non-Tibetan content; they open fine and show the honest
  "no dictionary matches" hint).
- **Tibetan corpus: 204.6M tokens · 4.32M spelling doubts (2.11%)
  · 184.6K particle-agreement flags.**

## The input-error map — most-doubted Tibetan files
*(the priority list for input-team review / the authority's
spelling-flag sittings; doubts are uncovered-token legality
failures, folio markers and dictionary-attested forms excluded)*

| doubts | tokens | agreement | file |
|---|---|---|---|
| 92064 | 390361 | 3071 | TD04420A_LEGS BSHAD NOR BU 'DREN PA'I SHING RTA_@.txt |
| 58062 | 246532 | 1820 | TD04155-1_BYANG CHUB SEMS DPA'I RTOGS PA BRJOD PA DPAG BSAM GYI 'KHRI  |
| 52136 | 225240 | 1608 | TD04155-2_BYANG CHUB SEMS DPA'I RTOGS PA BRJOD PA DPAG BSAM GYI 'KHRI  |
| 51870 | 248927 | 1950 | TD10169_T.TXT |
| 46416 | 228158 | 1819 | TD10170_T.TXT |
| 44245 | 58598 | 0 | S00034E_RDO RJE 'CHANG PHA BONG KHA PA DPAL BZANG POS LAM GTZO'I ZAB K |
| 41409 | 45126 | 3 | TOHOKU KANGYUR index.txt |
| 38013 | 273324 | 2531 | TD10196_T.TXT |
| 36330 | 46923 | 0 | S00069F_YON TAN GZHIR GYUR MA'I ZAB KHRID GNANG SKABS KYI BRJED BYANG  |
| 36073 | 280950 | 1589 | TD10195_T.TXT |
| 33823 | 179727 | 236 | R1003A1_T.TXT |
| 32924 | 173568 | 276 | R1003A24_T.TXT |
| 32598 | 137489 | 153 | R1003A48_T.TXT |
| 32457 | 153196 | 174 | R1003A8_T.TXT |
| 32207 | 136438 | 280 | R1003A2_T.TXT |
| 32176 | 115708 | 134 | R1003A10_T.TXT |
| 32176 | 142234 | 193 | R1003A23_T.TXT |
| 32018 | 143792 | 194 | R1003A27_T.TXT |
| 31928 | 117073 | 186 | R1003A4_T.TXT |
| 31870 | 218776 | 1021 | TD04286_LUNG STON PA KA L'A PA'I MDO'I 'GREL PA SLOB MA LA PHAN PA ZHE |

## No-stats rows by collection

- acip_release6: 1,175 (mostly _E English + _inc incomplete files)
- sungbum: 175 · kangyur: 74 · tengyur: 15

## Method & caveats

- Doubt = token not covered by any dictionary match AND failing the
  CC0 classical syllable rules; a raw rate, not a verdict — L-level
  unproofed texts dominate the top of the list, as expected.
- English files sometimes register doubts when the wylie detector
  matches English words; use the Tibetan-only numbers above.
- Full per-file data: data/reports/library_census.tsv.
