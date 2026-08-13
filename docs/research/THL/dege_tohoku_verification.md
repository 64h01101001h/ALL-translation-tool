# THL Degé Kangyur numbering vs. Tohoku (Toh) — spot-check verification

**Date:** 2026-08-12 · **Verdict: REFUTED** (THL D.N = Toh N does **not** hold; variable, growing offset)

Follow-up to the PROBABLE-UNVERIFIED flag in `collections_catalogs.md` ("THL d-number ≈ Toh number").
Spot check: 20 THL Degé Kangyur records (+4 diagnostic records) fetched live from old.thlib.org,
compared against live Tohoku assignments from 84000 (read.84000.co per-Toh pages + `84000/data-rdf`
GitHub, CC0) and BDRC (purl.bdrc.io `MW22084_NNNN` — 84000's own RDF asserts
`owl:sameAs bdr:MW22084_0361` for Toh 361, so BDRC's Degé Kangyur part numbers ARE Toh numbers).

## How the records were fetched (works, no browser needed)

The `?_escaped_fragment_=` trick does NOT render catalog records (search shell only). But the JS
app's backend endpoint is plain PHP and serves fully rendered record HTML:

```
https://old.thlib.org/encyclopedias/literary/php/cat_reader.php?pg=kt/d/NNNN   (NNNN zero-padded to 4)
```

Returns the complete record: Tibetan + Wylie title, Text ID (`D.N`), volumes, pagination,
"Master Catalog Number", cataloger metadata. Mild rate-limiting: consecutive rapid requests come
back empty — space requests a few seconds apart. (Found by reading
`/encyclopedias/literary/js/thl-catalog-controller.js`: `readerUrl = "/encyclopedias/literary/php/cat_reader.php?pg="`,
built as `pg=<coll>/<ed>/<text>` with `catId = "kt"`, edition `d`.)

Human-facing hash URL for a record is `#cat=d/NNNN` (per the app's `parseHash`), e.g.
`https://old.thlib.org/encyclopedias/literary/canons/kt/catalog.php#cat=d/106` — **not** the
`#cat=d/k/d0001` form guessed in the earlier research pass.

## Checked pairs

| THL ID | THL title (Wylie) | Actual Toh # of that text | Authority for Toh side | D.N = Toh N? | Offset |
|---|---|---|---|---|---|
| D.1 | 'dul ba gzhi | Toh 1 (Vinayavastu) | 84000 | YES | 0 |
| D.2 | so sor thar ba'i mdo | Toh 2 (Prātimokṣasūtra) | 84000 | YES | 0 |
| D.7 | 'dul ba gzhung bla ma | Toh 7 (Uttaragrantha; 84000: "Ultimate Section on Vinaya" spans 7/7a) | 84000 | YES | 0 |
| D.8 | 'dul ba gzhung dam pa | **Toh "7a"** — Tohoku sub-numbers it, THL counts it as a full text | (Tohoku convention) | **NO — first insertion** | — |
| D.9 | shes rab ... stong phrag brgya pa ('bum) | Toh 8 (Perfection of Wisdom, 100,000) | 84000 | NO | +1 |
| D.10 | shes rab ... stong phrag nyi shu lnga pa | Toh 9 (PW in 25,000; Toh 10 is the 18,000) | 84000 | NO | +1 |
| D.44 | bkra shis chen po'i mdo | Toh 43 (Sūtra of Great Fortune; Toh 44 = Avataṃsaka) | 84000 | NO | +1 |
| D.100 | ... ye shes rgyas pa'i mdo sde rin po che mtha' yas pa mthar phyin pa | Toh 99 (Blessed One's Extensive Wisdom...) | 84000 | NO | +1 |
| D.106 | 'phags pa dkyil 'khor brgyad ... | Toh 105 (Eight Maṇḍalas; Toh 106 = Saṃdhinirmocana) | 84000 | NO | +1 |
| D.112 | snying rje chen po'i pad ma dkar po | Toh 111 (White Lotus of Great Compassion) | 84000 | NO | +1 |
| D.113 | snying rje pad ma dkar po | Toh 112 (White Lotus of Compassion; Toh 113 = Saddharmapuṇḍarīka) | 84000 | NO | +1 |
| D.175 | 'jig rten 'dzin gyis yongs su dris pa | Toh 174 (Inquiry of Lokadhara) | 84000 | NO | +1 |
| D.250 | chos bzhi bstan pa | Toh 249 (Sūtra Teaching the Four Factors) | 84000 | NO | +1 |
| D.361 | dum bu zhe gnyis pa'i mdo (Sūtra in 42 Sections) | **none** — absent from Toh 345–362 (BDRC sweep) | BDRC | **NO — THL insertion** | — |
| D.543 | pad ma'i spyan zhes bya ba'i gzungs | **none** — absent from Toh 525–543 (BDRC sweep) | BDRC | **NO — THL insertion** | — |
| D.555 | bcom ldan 'das kyis 'jam dpal rnon po la bstod pa | Toh 551 (Blessed One's Praise of Sharp Mañjuśrī) | 84000 + BDRC | NO | +4 |
| D.676 | dpal lha mo nag mo'i bstod pa rgyal po'i rgyud | Toh 671 (exact Wylie match) | BDRC | NO | +5 |
| D.746 | 'phags pa dpal chen mo'i mdo | Toh 740 (exact Wylie match) | BDRC | NO | +6 |
| D.800 | las grub pa'i gzungs | Toh 794 (exact Wylie match) | BDRC | NO | +6 |
| D.846 | dpal he ru ka snying rje rol pa'i rgyud ... | Toh 840 (exact Wylie match) | BDRC | NO | +6 |
| D.1000 | dbyig dang ldan pa'i gzungs | Toh 991 (exact Wylie match) | BDRC | NO | +9 |
| D.1100 | nad kyi bdag mo la bstod pa | Toh 1090 (exact Wylie match) | BDRC | NO | +10 |
| D.1108 | stong chen mo rab tu 'joms pa las gsungs pa'i smon lam | Toh 1098 (exact Wylie match) | BDRC | NO | +10 |

## Verdict and offset pattern

**REFUTED.** THL's Degé Kangyur `D.N` equals Toh N only for D.1–D.7. From D.8 onward THL runs
ahead of Tohoku by a **variable, monotonically growing offset**: THL numbers as full texts several
items that the Tohoku catalogue either sub-numbers (Toh 7a = 'dul ba gzhung dam pa) or omits
entirely (e.g. the Sūtra in 42 Sections, the pad ma'i spyan dhāraṇī). Each insertion bumps all
subsequent THL numbers.

Observed offsets (THL − Toh): 0 (D.1–7) → +1 (D.9–~D.250+) → +4 (D.555) → +5 (D.676) →
+6 (D.746–D.846) → +9 (D.1000) → +10 (D.1100–D.1108). At least 10 THL-only insertions across the
catalog; exact insertion points between the sampled records were not located (not needed for the
verdict).

**There is NO safe formula from a Toh number to a THL record.** Do not wire
`#cat=d/<toh>`-style links.

## Consequences for the ALL Tool

1. **Kill the "d = Toh, link-out for free" idea** in `collections_catalogs.md` §Cross-linking #1.
   THL cannot join the Toh hub (ACIP KL/TD, 84000, BDRC) by number alone.
2. **A full concordance is cheap to build if ever wanted:** iterate
   `cat_reader.php?pg=kt/d/0001 … ~1118` (throttled), take each record's Wylie title, and match
   against a Toh-keyed title list — BDRC `purl.bdrc.io/resource/MW22084_NNNN.json` (Toh-numbered,
   Wylie prefLabels) or 84000's CC0 `data-rdf` repo. Exact-string Wylie matches worked for every
   non-inserted record sampled above. Only ~1118 fetches + fuzzy review of a handful of variants.
3. **Deep-link template (THL-number keyed, verified working):**
   `https://old.thlib.org/encyclopedias/literary/canons/kt/catalog.php#cat=d/<THL-number>` — but
   only via a concordance mapping Toh → THL number.
4. Useful side discovery: `https://purl.bdrc.io/resource/MW22084_<toh 0-padded 4>.json` is a
   clean, unauthenticated Toh-keyed title service (84000's RDF asserts the sameAs equivalence),
   and 84000's `https://read.84000.co/translation/toh<N>.html` serves og:title for published
   translations.

## Sources checked live (2026-08-12)

- `old.thlib.org/encyclopedias/literary/php/cat_reader.php?pg=kt/d/NNNN` — 24 records
- `read.84000.co/translation/toh<N>.html` — ~40 Toh pages (og:title)
- `raw.githubusercontent.com/84000/data-rdf/master/toh361.rdf` — Toh↔BDRC sameAs assertion
- `purl.bdrc.io/resource/MW22084_NNNN.json` — ~70 Toh-numbered Degé Kangyur part labels
