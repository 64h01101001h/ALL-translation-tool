# HGM DICTIONARY PROJECT — HANDOFF & CONTEXT
### Session handoff document · v0.27.2 · 2026-08-04
**Upload this file (plus the files listed in the manifest) at the start of the new conversation.**

> **QC INPUT (2026-08-07): 🔍 pyewts oracle defect candidates.** The app project ran the ecosystem-standard EWTS converter (OpenPecha pyewts) over all 105,634 (wylie, tibetan) master pairs as an independent diff-oracle (91.4% exact agreement; full classification in the app repo's `data/extracted/pyewts_oracle_report.json`). **13 entries where the stored tibetan is letter-true-prefix-inconsistent with its own wylie** are banked at `data/qc_pyewts_defect_candidates_2026_08.json` for review in the next data session — e.g. `khongs sa` stored without its final ས, and `bsgrub bya` stored as བསྒྲུབ་**བྱེད**. Candidates, not verdicts: each needs a source check before correction.

> **QC (2026-08-07): subject systems cross-validated.** The v0.31 bilingual subject pairs checked against the ༠༠ Sungbum Catalog's independent Tibetan hierarchy by SET CORRESPONDENCE (which texts each classifies; 1,020 works classified by both): 79 catalog paths show a strong R6 correspondence (≥3 shared works, ≥70% consistency — most are 100%: 12/12, 11/11, 10/10), and the correspondences confirm the '--' pairs linguistically (སྒྲུབ་ཐབས། ↔ SGRUB_THABS, རབ་གནས། ↔ RAB_GNAS, རྫོགས་རིམ། ↔ RDZOGS_RIM). 529 paths are support-thin (1–2 works — the deep hierarchy is sparse), not contradicted. Banked: `data/qc_subject_correspondence_2026_08.json`.

> **v0.32 CORPUS WAVE (2026-08-07): ✍️ AUTHOR-NAME PAIRS (by join).** The Release 6 web edition's Sungbum-by-author tree (English pronunciation-spelled names per work) joined against the ༠༠ Sungbum Catalog's Tibetan author column BY CATALOG NUMBER (`tools/ingest_r6_authors_2026_08.py`): pairs admitted only at ≥2 shared works and ≥80% consistency — **15 pairs** (top: Changkya Ngawang Lobsang Chunden = LCANG SKYA NGAG DBANG BLO BZANG CHOS LDAN, 102 works; Kyabje Trijang Rinpoche = SKYABS RJE KHRI BYANG RIN PO CHE, 48), 32 thin/conflicted joins EXCLUDED. Deterministic set evidence, no linguistic pairing. Course AUTH, `join_support` provenance. **Corpus 42,184 → 42,199** (`full_parallel_corpus_v32.json.gz`). Also confirmed: `ACIP_Master_Catalog.html` in the R6 zip = the Brief Catalog, record-identical to the ingested ACIP Catalog.TXT (2,875 records — lineage proven, no Sanskrit column there either).

> **v0.31 CORPUS WAVE (2026-08-07): 🏷️ ACIP SUBJECT-HEADING PAIRS.** Adam's subject-headings directive, source found in his own Dropbox: the **ACIP Release 6 web edition** (`acip-release6-wylie.zip`, HTTrack mirror of aciprelease.org, 2010) names every node of its subject browser as a bilingual pair by construction — `Vowed_Morality--'DUL_BA`, `Buddhist_Logic_And_Perceptual_Theory--TSAD_MA`. Parser over 767 subject pages (`tools/ingest_r6_subjects_2026_08.py`): 758 named nodes → **+367 unique Tibetan–English subject segments** (course SUBJ, `source_wave: r6-subjects-2026-08`; duplicates across tree branches deduped; 0 conversion failures). **Corpus 41,817 → 42,184** (`full_parallel_corpus_v31.json.gz`). Sidecars: `r6_subject_pairs.json` (the pair list) and `r6_work_subjects.json` (**1,424 works → their subject memberships**: S 1,089 · TD 322 · others few) — the app's Library joins these; also a future cross-check against the Sungbum catalog's Tibetan-only subjects by set correspondence. These are ACIP's own published equivalents (English titles of subject pages), reference tier as always.

> **v0.30 CORPUS WAVE (2026-08-07): 🗂️ SUNGBUM CATALOG PAIRS + SUBJECTS.** Adam's directives (all title translations into the corpus; extract the subject headings): `༠༠ Sungbum Catalog.xlsx` (Thursday meeting folder; double-row header) ingested via `tools/ingest_sungbum_catalog_2026_08.py` (report `data/ingest_report_sungbum_catalog_2026_08.json`): 2,949 rows → **+1,055 NEW title segments** (course TITLS, `source_wave: sungbum-catalog-2026-08`, w/ catalog_no + subject_bo provenance; 1,837 rows correctly deduped as already-covered by v0.29 on normalized number or identical pair; 57 other skips logged). **Corpus 40,762 → 41,817** (`full_parallel_corpus_v30.json.gz`). Also emitted `data/sungbum_subjects.json`: **2,416 texts → hierarchical Tibetan subject** (1,155 distinct paths, 34 top-level categories: རྒྱུད། 1,257 · ཉམས་ལེན། 375 · དྲི་ལན། 292 · ལམ་རིམ། 133 …). Subjects are TIBETAN-ONLY in this source — no English composed (rule 3); pairing awaits the Master Catalog's empty "ACIP/ALL Subject Category" column or the collection ZIPs' folder names. Dictionary untouched; tier ruling still open.

> **v0.29 ADDENDUM (2026-08-07): title-wave data conventions from the Release IV manual.** The manual (pp. 82–84) documents two conventions that appear in the ingested title pairs: **elision marks (…)** = title truncated at the Master Catalog's 255-char field limit (full title lived in the catalog's Notes field, absent from our TXT export) — **85 of the 2,955 ingested segments** carry this and should be treated as partial titles; **@ marks** = data ACIP staff could not verify — **7 segments**. Neither was flagged at ingest; noted here so dictionary-side use treats them accordingly. Also confirmed: the manual's printed appendix catalogs are ABSENT from the available PDF copy (appendices title page, then two chart pages, end of file); the same abbreviated catalog shipped on the Release IV CD as Acipdocs/Briefcat (WordPerfect) + Rtf — not found on Adam's disk; the ingested ACIP Catalog.TXT carries the same field set and is presumed the same lineage.

> **v0.29 CORPUS WAVE (2026-08-07): 🏷️ CATALOG TITLE PAIRS.** Adam's directive (isolate every K/T/S work with BOTH Tibetan and English titles, bring the pairs into the corpus) executed from the catalog files in `~/Desktop/Thursday Database-Cataloging 6 4 26 Meeting/` (parser `tools/ingest_catalog_titles_2026_08.py`, report `data/ingest_report_catalog_titles_2026_08.json`): **ACIP Catalog.TXT** (2,587 pairs, richest), **00. KANGYUR CATALOG.xlsx** (eKangyur, English titles as filenames), **New Master Catalog 5 18 26.xlsx Sheet2** (130 pairs) — merged on catalog number, TXT priority. **Corpus 37,807 -> 40,762** (`full_parallel_corpus_v29.json.gz`; +2,955 segments in courses TITLK 349 / TITLT 359 / TITLS 2,091 / TITLR 156; tagged `source_wave: catalog-titles-2026-08` + `catalog_no` + `catalog_source`); 802 skips logged (records missing a title / no catalog number / unconvertible). Wylie via canonical acip_to_ewts. **Dictionary untouched — the tier ruling (are catalog English titles binding HGM English or a labeled reference layer?) remains OPEN for Adam**; these segments are corpus evidence labeled as catalog titles. App spine rebuilt on v29.

> **v0.28 CORPUS WAVE (2026-08-06): 📚 GMR VOLUMES.** Three published translation volumes ingested (staged in `Input/GMR_volumes_2026-08/`, parser `tools/ingest_gmr_volumes_2026_08.py`, report `data/ingest_report_gmr_volumes_2026_08.json`): **SSL** *A Song of My Spiritual Life* (Tsongkapa + Choney Lama comm., tr. Chang w/ GMR, +427 seg), **OSE** *The Other Side of Emptiness* (Ngulchu Dharma Bhadra, tr. Chang w/ GMR, +432), **SVN** *Sixty Verses of Nagarjuna* (Gyaltsab Je comm., tr. Lashaw w/ GMR, +1,347). **Corpus 35,601 -> 37,807** (`full_parallel_corpus_v28.json.gz`; segments tagged `source_wave: gmr-volumes-2026-08`); 91 unpairable blocks skipped & logged. Alignment = the volumes' own [N]-unit adjacency; IAST/romanized-Sanskrit echo lines excluded; folio refs stripped both sides; {%corrections} kept in acip, stripped from wylie input; wylie via canonical acip_to_ewts. Dictionary untouched (hgm_gloss 12,004). **Next data session: attestation recount + miners over the new segments** (deliberately not run — same restraint as v0.27). App spine rebuilt on v28; all 14 app suites green.

> **v0.27.2 PATCH NOTE (2026-08-04):** Pre-handoff hygiene. 4 phantom entries deleted (unified -> 105,634); 31 dkhyil/skhyil wylie typos fixed against ground-truth script (all verified); last footer residue swept. **This is the handoff baseline for the ALL Translation Tool build — see CLAUDE.md + ALL_TRANSLATION_TOOL_ROADMAP.md at project root.** Old release files archived to Handoff/release/_archive/.

> **v0.27.1 PATCH NOTE (2026-08-04):** NEW CANONICAL ENGINE tools/ewts_unicode.py (wylie -> Tibetan Unicode; validated 98.88% against the dictionary's own 26,318 ground-truth pairs; ~300 residuals are source defects, logged). **79,316 entries filled with generated Tibetan script** (tagged tibetan_source; 7,655 flagged tibetan-review, failed syllables shown as <wylie> markers, never guessed). Tibetan coverage 26,321 -> 105,637/105,638. hgm_gloss untouched (12,004).

> **v0.27 SESSION NOTE (2026-08-04): 🕉 LOKESH CHANDRA LAYER.** LkChDictionary.dic (Ven. Phil's ACIP reader) parsed directly: 88,973 Tibetan-Sanskrit headwords. **8,364 existing entries gained {LC}-tagged Sanskrit; 79,305 new awaiting entries created (unified 26,333 -> 105,638)**; 15,151 of the new entries corpus-attested (+26,194 contexts; in-corpus -> 29,512). Miners deliberately not run (79k blind influx would flood review). hgm_gloss untouched (12,004). Standalone strip in Input/LokeshChandra/.

> **v0.26.1 PATCH NOTE (2026-08-04):** Hopkins realignment completed. {GD:###} tag regex fixed (colon) -> 338 entries carry Dreyfus glosses; 41 wylie headwords cleaned of embedded page-footer text (acip/pronunciation regenerated); 163 duplicate source headwords content-merged; +546 further realignments; 4 phantom fragments flagged; **11 missing scaffold headwords added (unified 26,322 -> 26,333)**. hgm_gloss untouched (12,004). Hopkins coverage 21,730.

> **v0.26 SESSION NOTE (2026-08-04): 📖 HOPKINS SCAFFOLD REALIGNMENT.** Source Uma/Hopkins PDF re-parsed style-aware (new tools/hopkins_parser.py: baseline clustering, 16pt-margin anchors, font-style field assignment). 21,730 entries parsed; **13,360 dictionary entries realigned**: 2,480 {PH} English blocks moved out of sanskrit, 1,531 Sanskrit terms recovered out of hopkins_english, ~4,800 entries regained line-wrap-lost English (hopkins coverage 16,874 -> 21,681). hgm_gloss untouched (12,004 before and after). Ship check green.

> **v0.25.1 PATCH NOTE (2026-08-04):** TCS09-18 rerun -- 9 of 12 files byte-clean; 924 segments repaired in place (page-number pollution: TCS16KaliWang 692, TCS18LamaChupa 122, TCS18Commentary 78), 32 contexts refreshed, +2 new segments; **corpus -> 35,601**. Full TCS series now re-audited end-to-end (2,391 repairs total). No gloss changes.

> **v0.25 SESSION NOTE (2026-08-04): 😈 DEVIL DEBATES AN ANGEL.** New DCC Volume 111 ingested (DDA, +516 segments, First Panchen Lama Mahamudra debate poem, entirely new material). TCS re-parse: 1,467 v0.10-era segments repaired in place (page-number pollution class), +32 new segments, 34 contexts refreshed. **Corpus -> 35,599; in-corpus 14,361; HGM-glossed 12,004** (+1: ngoms pa med = never satisfied; 39 candidates banked). Ship check green.

> **v0.24.1 PATCH NOTE (2026-08-04):** C03-C18 ASCII readings re-parsed under current rules -- originals near-complete; +24 debate-register micro-segments recovered ('dod / yod de / med de / ga re / yang dialogue lines, C13/C14/C18); 13 folio-ref false hits rejected; **corpus -> 35,051**; no new glosses (particles already glossed) -- see README v0.24.1.

> **v0.24 SESSION NOTE (2026-08-04): 🪷 DAILY PRACTICE WAVE — the banked Practice batch decode SHIPPED.** Ten PDFs received (P1, P3–P10 + Diamond Cutter Sutra pecha). Pecha excluded per rule 4 (English-only text layer). New `practice_parser.py` dual-channel layer (pypdf glyph runs × pypdfium2 clean text; T/X interleave never guessed; 11 mismatch pages skipped & logged). **242 prayer cards decoded** (new validated glyph alias `baĸ`→bkr; yig-mgo→shad rule); `decoded_all_courses.json` → **2,286 cards / 27 sets**. **+782 unique passage segments appended (corpus 34,245 → 35,027**, `wylie_method: glyph-decode`, mean lexicon score 0.993, gates logged). **284 corpus-dupes = character-exact matches of glyph-decoded wylie against ACIP-derived segments (C03/C05/C15…) — independent end-to-end pipeline validation.** Attestation recount: in-corpus → **14,338**, +131 contexts, 170 newly P-evidenced. Tokenizer standing rule: unicode preserved (*paṇ* ≠ *pa*) — killed a support-inflation class in-session. Miners (ratio ≥0.25 triage): **+24 practice-v24 glosses; HGM-glossed → 12,003**; 76 auto-candidates. v0.23 recon correction: harvest() on P6 = 54 pairs, not 156; structural parser supersedes. Next: tier review (224+146+77+24+3,438) · fill-from-contexts · card-derived pass (~275) · banked-only (79) · 887 unmatched · engine battery (7 pairs) · disambiguation → translation tool.

---

## 1. WHAT THIS PROJECT IS

A Tibetan–English dictionary **strictly locked to Geshe Michael Roach's (HGM's) English equivalents**, formatted as an Illuminator × Hopkins hybrid, built to eventually power a Tibetan→English translation tool in HGM's voice.

**Founding rules (inviolable):**
1. **Hopkins is reference only, never authority.** The `hgm_gloss` field is binding; Hopkins glosses are context.
2. **No English is ever invented.** An entry stays `awaiting-hgm-equivalent` until the term is attested in HGM's own translations. Machine work may *match* HGM's English, never *compose* it.
3. **Single-register evidence never generalizes.** (Lesson of *bsod nams*: "goodness" in prayers, "merit/good deeds" in the corpus.) Register variants are recorded, not resolved.
4. **Deviations are logged, not forced.** (DEV-001: ASCII homage line reads *rje btsun 'jam pa'i dbyangs* where HGM's English reads "high and holy lamas" — textual variant, no pairing extracted.)

**Standing release rule (in user memory):** every update ships as a package — `.json` master · `.tsv` · reverse index `.json` · full `.md` dictionary · README — extended with the **compact edition** and **attested edition** (7 files total).

**Pronunciation rule (in user memory):** every entry carries `acip` + `pronunciation` fields per ACIP Release 4 Catalog pp. 173–178, refined by GMR card conventions, with GMR word segmentation (details §4).

---

## 2. CURRENT STATE (v0.18.0)

| Metric | Value |
|---|---|
| Unified entries | **105,638** (26,333 core + 79,305 Lokesh Chandra layer v0.27) |
| — with HGM English | **11,532** (114 curated · ~7,980 glossary · ~3,438 auto-aligned) |
| — awaiting equivalent | 14,367 (thousands carry `attestation_contexts` and/or `auto_candidate`) |
| Parallel corpus | **35,601 aligned Tibetan–English segments** (incl. 782 glyph-decoded practice v0.24; 516 DDA v0.25; 1,467 TCS segments repaired v0.25) |
| Corpus coverage | 12,467 entries attested somewhere in-corpus |
| Pronunciation engine | validated 42/42 battery; **1,308 entries card-attested** ⟪card⟫; 2,118 card lines banked (C01–C05, C07–C17) in card_phonetics.json; 47 v0.16 segmentation-override pairs banked as engine-rule evidence, engine untouched pending regression battery |
| Glyph decode (prior) | **2,044 decoded cards** across ALL 18 COURSES in `decoded_all_courses.json` (1,561 validated · 392 lexicon-validated · 55 validated-prefix · 29 unvalidated · 7 undecodable) |
| Glyph harvest (NEW, this session) | **C13: 152 · C14: 80 · C15: 104 (control) · C16: 211** raw (glyph_run, phonetic) pairs in `harvested_c13_c14_c15_c16.json` — **DECODED & MERGED in v0.18** (C15 regression 99/99) |

**Evidence tiers** (in `hgm_evidence.tier`): `curated` > `glossary` > `auto-aligned` (methods: `direct-segment` / `intersection` / `quote-pattern`; **provisional pending Adam's review**). 449 weak candidates demoted to `auto_candidate`.

---

## 3. SESSION LOG (2026-08-04, harvest session)

1. Verified full v0.17.0 package integrity on load (26,322 / 115 / 8,028; card_phonetics 2,118 lines).
2. Received `C13Tibetan.pdf`, `C14Tibetan.pdf`, `C15Tibetan.pdf`, `C16Tibetan.pdf`. All four expose TibetanMachine glyph-name runs via pypdf — the exact-spelling channel is live.
3. Ran `glyph_decoder.harvest` (standalone; no engine needed) on all four:
   - **C15 as extraction control:** 104 cards harvested = exact match to the 104 previously decoded C15 cards in `decoded_all_courses.json` (99 distinct-phonetic overlap; remainder are within-set duplicates). Extraction layer verified sound.
   - **C13:** 152 cards; 134/145 distinct phonetics match banked card_phonetics lines; 11 harvest-only (new); 27 banked-only (likely multi-line/layout edge cases — inspect during decode).
   - **C14:** 80 cards; 68/78 match; 10 harvest-only; 28 banked-only.
   - **C16:** 211 cards; 199/201 match; 2 harvest-only (multi-term cards: *ngunsum tsema ngungyur*; *jepak tsema kokgyur shintu kokgyur*); 6 banked-only.
4. Banked everything in `harvested_c13_c14_c15_c16.json` (format: `{course: [[glyph_run, phonetic], …]}`).
5. **DECODE NOT RUN — blocker (RESOLVED in batch-1 reconciliation):** `pron_engine.py` was not uploaded during the harvest session; the decision then was **do not reconstruct the engine** (a rebuilt engine risks silent divergence from the validated 42/42 version and would mislabel `validated` statuses). The canonical `pron_engine.py` has since landed and passed a 10/10 convention spot battery. Decode now awaits only `decoded_all_courses.json` (regression control + merge target) and `hgm_dictionary_v27_2.json` (lexicon source), plus the current `card_phonetics.json`.

---

## 4. SOURCES INGESTED (complete inventory)

1. **C01 Reading PDF** (Three Principal Paths; Tibetan font unextractable — superseded by ASCII).
2. **Hopkins Tibetan-Sanskrit-English Dictionary** (June 2015, 880 pp) — span-level parse → **21,751-headword scaffold** with Sanskrit, Hopkins English, verb tenses.
3. **Hypertext Glossary (2016-05-16)** — **8,028 ACIP→HGM pairings** (bulk attestation layer).
4. **C01–C18 ASCII/ACIP course readings** — the full 18-course ACI curriculum.
5. **TCS01–TCS18 practice series** (27 files) — prayers, sadhanas, Lama Chupa root + commentary, etc.
6. **Sunlight on the Path to Freedom** (docx; DCC vol. 72) — 1,098 segments; **template for the whole DCC series** (normalize curly apostrophes; filter CJK lines + [K#]/folio markers).
7. **ACIP Release 4 Catalog** (pronunciation standard, pp. 173–178) + **ACIP Sanskrit Input Code Standards** + **26 GMR vocabulary-card screenshots** (pronunciation ground truth).
8. **ACI Tibetan Language Study Guides, Courses I–XVII** (PDFs; Tibetan font-locked; phonetic lines extracted via font-anchored layout parsing) — card-attested pronunciations stamped on 797 entries; engine-vs-card segmentation overrides applied (card governs). Glyph-name runs decoded for C01–C05, C07–C12, C15 (1,185 cards); **C13/C14/C16 harvested this session, decode pending engine**. ALL 18 guide PDFs ingested and decoded (series complete v0.20).

---

## 5. CONVENTIONS LOCKED

**ACIP→EWTS:** TS→tsh, TZ→ts; consonant+'+vowel = long vowel (K'A→kA); vowel+' = achung (BA'I→ba'i); lowercase = Sanskrit retroflex. (Code: `hgm_tools.py: acip_to_ewts`.)

**Pronunciation** (`pron_engine.py: pronounce`), word-segmented per GMR cards:
- Lexemes joined (*ngenjung, semkye*); formatives attached (*gelukpa, malupar*); genitive *'i* fused (*yangdakpay*).
- Standalone particles: *kyi/gyi/gis…, dang, la, na, nas, las, ni, yang, kyang, du, ru, te, ste, de, rnams, tsam*. `tu` fuses solid (*chiktu*); `su` hyphen-fuses (*se-su*). Negation fuses forward (*masong*). Adverbial *gyis* fuses after {rim, zil}.
- o+n→u general (*dunche, ngunsum*), exceptions {ston→ton, dkon→kon}; a→e before d/n/l/s; o→u before d/s.
- Prenasal only after non-p finals (*delnjor, tangok*; but *kyabdro*). Junction sandhi: *sangye, tsokyi, dakki, tangok*. Lexical devoicers {thob→top, grub→drup, sgrib→drip}; *chub/rdzob* keep b. Hyphen compounds curated list {chos sku→chu-ku, gzugs sku→su-ku, …}.
- Known lexical-segmentation limits (*lha mi…*, *blo gnod med*) → flagged `pronunciation-review` (244 entries), never guessed.

**Glyph decoding** (`glyph_decoder.py`): harvest → per-syllable candidate decode → round-trip validate against the card's own phonetic via `pronounce` → fallback lexicon corroboration. Statuses: `validated` > `validated-prefix` > `lexicon-validated` > `unvalidated` (kept, flagged) > `undecodable-glyph`. **Never guesses.**

**HGM signature equivalents** (sample; full table in README): kun rdzob bden pa = deceptive truth · nges/drang don = literal/figurative · schools = Detailist/Scripturalist(Sutrist)/Mind-Only/Independent/Implication · dngos smra ba = Functionalist · theg chen/dman = greater/lesser way · sgom lam = path of habituation · lam rim = Steps of the path · 'jig lta = destructible/perishable view · bka' gdams pa = Seers of the Word · las = deeds · thar pa = freedom · thub pa = the Able · rdo rje 'chang = Holder of the Diamond · 'jam dbyangs = Gentle Voice · mkha' 'gro ma = Angel · bsod nams = merit/good deeds (goodness in prayer register).

---

## 6. FILE MANIFEST (bring these to the new conversation)

| File | What it is |
|---|---|
| `HANDOFF.md` | This document. |
| `pron_engine.py` | **✅ IN HAND (batch 1).** `pronounce(wylie)`, `wylie_to_acip`, `CLUSTERS`, `SIMPLE` — validated 42/42; 10/10 convention spot battery re-passed on receipt. Required by `glyph_decoder.best_decode`. |
| `glyph_decoder.py` | harvest / decode_run / best_decode for TibetanMachine glyph runs. |
| `harvested_c13_c14_c15_c16.json` | **NEW.** Raw (glyph_run, phonetic) pairs: C13 152 · C14 80 · C15 104 (control) · C16 211. Awaiting decode. |
| `decoded_all_courses.json` | 2,286 decoded cards across 27 sets (all 18 courses + P1, P3–P10 practices) with statuses. |
| `card_phonetics.json` | 2,118 banked GMR card phonetic lines (C01–C17), incl. 887 unmatched ground-truth lines for calibration. |
| `hgm_dictionary_v27_2.json` | **THE MASTER.** meta + `unified_entries` (26,322; 1,308 card-attested; 12,004 HGM-glossed) + `curated_entries` (115) + `glossary_entries` (8,028). |
| `hgm_dictionary_v27_2.tsv` | Flat table for filtering/review. |
| `hgm_reverse_index_v27_2.json` | English→{wylie, pronunciation, tier} (~15k keys). |
| `hgm_dictionary_v27_2.md` | Full rendering (Part I curated; Part II unified). |
| `hgm_dictionary_v27_2_compact.md` | Adam's preferred browse format. |
| `hgm_dictionary_v27_2_attested.md` | HGM-only edition (11,532 entries). |
| `README.md` | Methodology + changelog + signature table + release-package standard. |
| `full_parallel_corpus.json` | 35,601 aligned segments — the evidence base (needed for ingestion/attestation work). |
| `hgm_tools.py` | `acip_to_ewts`, `is_acip_line`, `parse_aligned` (needed for new-text ingestion). |

Minimum set for the **decode session**: HANDOFF.md · pron_engine.py · glyph_decoder.py · harvested_c13_c14_c15_c16.json · decoded_all_courses.json · card_phonetics.json · hgm_dictionary_v27_2.json (lexicon source). Plus `C17Tibetan.pdf` if available.

---

## 7. NEXT STEPS (immediate → roadmap)

1. **DECODE PASS (engine in hand — fires as soon as `decoded_all_courses.json` + `hgm_dictionary_v27_2.json` + current `card_phonetics.json` arrive, expected batch 2):** run `best_decode` over the 443 harvested C13/C14/C16 cards (lexicon = all wylie forms + variants from unified entries). Re-run C15 as regression control against `decoded_all_courses.json`. Report validation stats per course; merge into `decoded_all_courses.json`.
2. **Guide harvest COMPLETE** — all 18 courses decoded; item closed v0.20.
3. **Match decoded wylie back to the dictionary:** stamp `pronunciation_source: card` + exact card spellings on matched entries; attack the 887 unmatched ground-truth lines; inspect the banked-only lines (27 C13 · 28 C14 · 6 C16) for multi-line card merging.
4. **Version bump to v0.18** after decode+stamp: regenerate the full 7-file release package.
5. **Review the auto-aligned tier** (~3,438 glosses): filter TSV, confirm against `attestation_contexts`, promote.
6. **Fill from contexts:** thousands of awaiting entries carry bilingual contexts (C13/C16/C18 richest).
7. **Ingest more DCC volumes** (Sunlight recipe) → rebuild attestation → rerun miners.
8. **Disambiguation layer** for multi-gloss entries — needed before the translation tool.
9. **Translation tool architecture** (end goal): segment → longest-match → rank by tier + corpus breadth → assemble in HGM register.

## 8. REBUILD RECIPES
- **Decode harvested cards:** `from glyph_decoder import best_decode` → build `LEX_WORDS` = set of all syllable-tokens from unified `wylie` + `wylie_variants` → for each (run, phon): `best_decode(run, phon, LEX_WORDS)` → bank {phonetic, wylie, status} per course.
- **Ingest a new text:** `segs = parse_aligned(path, 'ID')` → append to corpus → attestation pass (n-gram match over segment tokens; store courses + ≤2 contexts) → miners (intersection: support≥2, df ≤ max(3, 1% segs), unigrams support≥3; quote-pattern: "what we call/known as/called" + exactly one awaiting term) → bump version → regenerate 7-file package.
- **Never** auto-fill without tagging `tier: auto-aligned` + method + confidence. **Never** reconstruct `pron_engine` from spec — always require the canonical file.
