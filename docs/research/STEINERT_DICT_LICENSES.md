# Steinert public-dictionary CSVs — per-dictionary license review

**Reviewed 2026-08-20** (backburner item: "Lookup-pane layers AFTER
per-dictionary license review — code is GPL, data only").
Source: github.com/christiansteinert/tibetan-dictionary,
`_input/dictionaries/public/` (46 public dictionaries) + the webapp's
`dictlist.ts` source citations + the repo README.

**Steinert's own blanket statement (README):** "the dictionary data is
not my own and thus THE COPYRIGHT OF THE DICTIONARY DATA IS WITH THE
RESPECTIVE AUTHORS." So there is no repo-level grant; every dictionary
stands on its own license, and "it's in the public build" ≠ "freely
redistributable."

## Verdicts

### Already ingested through their own clean channels (nothing to do)
| Dict | Our channel |
|---|---|
| 43–46 · 84000 (dict/defs/synonyms/skt) | CC BY 4.0 per 84000's own Terms table; superseded 2026-08-20 by our direct GraphQL harvest + TM ingest |
| 51 · LaineAbbreviations | Dr. Bruno Lainé granted CC BY 4.0 directly (banked with the rKTs layer) |
| 21/63/64 · Mahāvyutpatti (+ sgra sbyor) | we carry the DILA TEI edition, CC BY-SA 3.0 (9th-century text itself PD) |
| 49 · LokeshChandraSkt | we extracted LC from ACIP's own materials |
| 65 · ChandraDas_Scan (headword→page index) | Steinert granted his index by email 2026-08-13 (banked) |
| 26 · Verbinator (Hill 2010 verb stems) | our verb layer is the CC0 Tibetan verbs database + our own engine work; Hill's book is © Bayerische Akademie — no grant, and no need |

### Not redistributable without permission — do NOT ingest
Copyrighted works with no open grant. Where the THL payload already
carries them they stay **local-only** (existing tier), which is the
correct maximum:
Hopkins/UMA 2015 family (01, 05, 06, 11–20, incl. Hackett defs) ·
Rangjung Yeshe 3.0 + Thomas Doctor + Jim Valby + Ives Waldo +
Richard Barron (all © their authors, distributed via the RY
dictionary) · Dan Martin "Tibetan Vocabulary" · Tsepak Rigdzin
(LTWA) · Bialek (Routledge 2022) · Berzin archive glossary ·
Heart of Tibetan Language (Dharma Publishing Anki cards) ·
Sera Textbook Definitions · GatewayToKnowledge (© RY) ·
NegiSkt (CIHTS) · ITLR (collaborative project, own terms) ·
CommonTerms-Lin · Misc (47, "mixed sources" — unattributable, so
unclearable by definition) · hotl1 · and every Tibetan-Tibetan /
Tibetan-Chinese published dictionary (25 tshig mdzod chen mo, 34
dung dkar, 37 dag yig gsar bsgrigs, 54–62 incl. sgom sde, brda
dkrol, chos rnam kun btus, li shi'i gur khang, bod yig tshig gter
rgya mtsho…).

### Outreach candidates (possible grants; queue for the email batch)
| Dict | Why plausible | Ask |
|---|---|---|
| 48 · TibTermProject | CTA Department of Education standardized-terms glossary (2013) — official terminology intended for adoption | CTA DoE: redistribution of the standardized glossary |
| 36 · ComputerTerms | China Tibetology Research Center IT-terms standardization (2006) — same class | likely via Steinert: what grant did he receive? |
| 38 · GaengWetzel | Buddhist Academy Berlin free PDF ("Buddhist Terms", 2004) | authors/academy: formal permission |
| 22 · Yogacharabhumi-glossary | Mahoney's edition of a terminology table; academic, freely circulated | Richard Mahoney (Indica et Buddhica) |
| 40 · tibetanlanguage-school | Nick Prior "making it available" per Steinert's credits | Nick Prior directly — colloquial vocab would be a real gain for the colloquial layer |
| 66 · Jaeschke_Scan index | Steinert already granted the Das index; the Jäschke ask is IN the queued thanks email | already queued (we built our own page map meanwhile, so nothing blocks) |

### Bottom line
Nothing in the public set is autonomously ingestable today that we
don't already carry through a cleaner channel. The review converts
the open-ended backburner item into: (a) a standing DO-NOT-INGEST
list, (b) six named outreach asks for the existing Adam-gated email
batch. No code work follows until a grant lands.
