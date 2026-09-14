# The HGM Dictionary Project
## Methodology, Pipeline, and Roadmap — v0.27.2

### v0.27.2 changelog (2026-08-04) — 🧹 PRE-HANDOFF HYGIENE (clean state for the Claude Code transfer)
- **4 phantom-fragment entries deleted** (old-parse artifacts with no headword, flagged v0.26.1; content fields already cleared). Unified 105,638 → **105,634**.
- **31 wylie aspiration typos fixed** — the *dkhyil/skhyil* class exposed by the v0.27.1 unicode validation (illegal onsets; ground-truth Tibetan script དཀྱིལ/སྐྱིལ is the authority). Wylie, variants, ACIP, and pronunciation regenerated; **all 31 now match their source script exactly** (`wylie_repaired: typo-v27-2`).
- **Last footer-pollution residue swept from wylie_variants** (1 entry). Corpus-wide audit: 0 footer text anywhere in wylie/variants/tenses.
- Package regenerated; ship check green. HGM layer untouched (12,004). This release is the handoff baseline for the ALL Translation Tool build (see CLAUDE.md / ALL_TRANSLATION_TOOL_ROADMAP.md at the project root).
- HOUSEKEEPING NOTE for Adam: superseded release files (v23_1 through v27_1) still sit in `Handoff/release/` and older corpus .gz files in `Handoff/data/` — drag them into an `_archive/` subfolder at your convenience so only the current v27_2 baseline is visible to tools.

### v0.27.1 changelog (2026-08-04) — ཨ TIBETAN UNICODE COMPLETION (new canonical engine + 79,316 entries filled)
- **New canonical engine: `ewts_unicode.py`** — wylie → Tibetan Unicode, handling native onset grammar (prefix/superscript/root/subscript decision table, g.y disambiguation), Sanskrit CV-group stacking (karṇa → ཀརྞ), IAST-flavored wylie (ā/ṇ/ṣ/ṃ… as the HGM fields use), explicit `+` stacks, diphthongs, anusvara/visarga. **Validated against the dictionary's own 26,318 (wylie, tibetan) ground-truth pairs: 98.88% exact (variant-aware).** The residual ~300 mismatches are predominantly SOURCE defects the engine exposed, now logged as review candidates: wylie typos (*dkhyil/skhyil* for དཀྱིལ/སྐྱིལ), one GT/wylie disagreement (*rab tu zlum po* vs ཀུན་ཏུ་ཟླུམ་པོ), footer pollution inside `wylie_variants`, and a GT double-vowel anomaly (ཀོོ).
- **79,316 entries filled with generated Tibetan script** (the LC layer + the 11 scaffold additions), tagged `tibetan_source: generated-ewts-v27-1` so generated script is always distinguishable from source-attested script. Failed syllables are never guessed: they render as ⟨wylie⟩ markers and the entry is flagged `tibetan-review` — **7,655 flagged** (mostly Sanskrit-transliteration headwords with clusters outside the engine's tables). **Tibetan coverage: 26,321 → 105,637 of 105,638** (the one holdout is a flagged phantom entry with no headword).
- `hgm_gloss` and all HGM-layer fields untouched: 12,004 before and after.
- **The translation-tool unicode converter now exists and is battle-tested** — the same engine slots directly into the DCC desktop tool's conversion pane (wylie/ACIP → unicode), per the architecture discussion.
- Package regenerated; ship check green. NOTE: full .md now 22.3MB (over the 20MB device-write cap) — committed gzipped alongside the master json.gz.

### v0.27.0 changelog (2026-08-04) — 🕉 THE LOKESH CHANDRA LAYER (88,973 Tibetan–Sanskrit headwords merged)
- **New source ingested: the Lokesh Chandra Tibetan-Sanskrit Dictionary** (main edition + 1992 Supplementary), extracted from `LkChDictionary.dic` — the compiled data file of Ven. Phil Baker's open-source ACIP reader app (see `Input/LokeshChandra/` for the standalone strip: 88,973 headwords, 83,884 with IAST Sanskrit, citations and source-line provenance preserved). The .dic's tagged-text format was parsed directly; **zero ACIP→wylie conversion failures** through the canonical `acip_to_ewts`.
- **8,364 existing entries gained `{LC}`-tagged Sanskrit** appended to their `sanskrit` field — same tagged-reference convention as {C}/{PH}/{GD}; matching was variant-aware on both sides (multi-headword LC entries split on commas). **Founding rule 1 holds: reference layer only; zero hgm_gloss/status/pronunciation changes; HGM-glossed exactly 12,004 before and after.**
- **79,305 new awaiting entries created** for LC headwords absent from the dictionary — each with wylie (+variants), ACIP, engine pronunciation (0 failures), `{LC}` Sanskrit, source tags (`lc_source`), and up to 4 citations (`lc_refs`). **Unified entries: 26,333 → 105,638** — the dictionary is now 4× its former size, with the HGM layer unchanged at its core.
- **Attestation over the new layer: 15,151 of the new LC entries are attested in the 35,601-segment corpus** (+26,194 bilingual contexts); **in-corpus coverage 14,361 → 29,512**. These 15k corpus-attested LC terms are the prime mining frontier for future waves. **Miners deliberately NOT run this wave** — a 79k-entry influx mined blind would flood the review queue; mining resumes per-wave on new corpus evidence.
- Part III of the full rendering now carries the LC additions; generator headers updated. Package regenerated via validated generators; ship check green.
- **Review queue unchanged:** 224 + 146 + 77 + 24 + 1 + 3,438 provisional glosses; plus 4 phantom entries pending deletion decision.

### v0.26.1 changelog (2026-08-04) — 🔎 HOPKINS REALIGNMENT COMPLETION (residual classes from the v0.26 audit, all repaired)
- **Parser gap fixed: `{GD:###}` Dreyfus glosses.** The tag regex rejected colons, so *Recognizing Reality* material was dropped at parse time — exactly the class behind the 119 "matched-but-empty" entries. Full book re-parsed; **338 entries now carry {GD} glosses**, many fuller than the old parse ever had (e.g. *lkog na mo* regained its complete bracketed explanation; *skye ba'i sgo bcom pa* also gained its tense set *gzhom 'joms bcom choms*).
- **41 entries had the running page footer ("Tenses: future, present, past, imperative") embedded inside their WYLIE headword** — old-parse page-break artifacts. Wylie cleaned, `acip` and `pronunciation` regenerated via the canonical engine (failures flagged `pronunciation-review`, never guessed).
- **Duplicate source headwords merged:** the Uma dictionary repeats 163 headwords with complementary content (e.g. the three *wa ra ṇa si* entries on p. 641); the realign index now unions their sanskrit/hopkins content instead of keeping only the first.
- **546 additional entries realigned** on the re-run (GD recoveries + duplicate merges + rematches after wylie cleaning).
- **4 phantom fragment entries flagged** (`flags: phantom-fragment`) — old-parse artifacts with no headword at all, one carrying a stray slice of *ka 'khor ba*'s gloss; reference fields cleared, entries retained pending Adam's decision on deletion.
- **11 genuine scaffold headwords recovered** that the original span parse never captured (e.g. *sgras brjod rigs kyi sgo nas* "terminologically", *rgyu'i kun nas nyon mongs pa* "causal affliction") — appended as awaiting entries (`scaffold_added: uma-v26-1`, Part III). **Unified entries: 26,322 → 26,333**; generator header updated accordingly.
- `hgm_gloss` untouched throughout: 12,004 before and after. Hopkins coverage now **21,730**. Residual audits: 0 {PH}-in-sanskrit, 0 footer text in any field.
- Package regenerated via validated generators; ship check green.

### v0.26.0 changelog (2026-08-04) — 📖 HOPKINS/UMA SCAFFOLD REALIGNMENT (all 866 pages re-parsed style-aware)
- Adam re-supplied the source Hopkins/Uma Institute Tibetan-Sanskrit-English Dictionary PDF (June 2015, 880 pp) and asked for a precise realignment of the `sanskrit` and `hopkins_english` fields. **A new style-aware parser (`hopkins_parser.py`) replaced the original span-level parse**: entries anchored on 16pt Tibetan headwords at column margin (x0 ≤ 76 / 306–316 — excludes inline debate quotes, which are also 16pt); lines clustered by the **text-matrix baseline** (the 16pt Tibetan glyph box spans two Latin lines, so geometric clustering fails); fields assigned by font style — *italic* = wylie/tenses/Sanskrit, **bold** = Hopkins English, roman `{TAG}` blocks = tagged English glosses ({PH} Hackett, {C} Conze…), smallcaps = prose blocks (Definitions/Divisions/examples), excluded.
- **Parse: 21,730 entries** from 866 body pages (scaffold had 21,751 — 0.1% delta). Spot-verified character-exact against print (p.1, p.641 *wa ra ṇa si* cluster).
- **21,562 dictionary entries matched; 13,360 realigned.** The misfiling classes the audit exposed and fixed: **2,480 `{PH}` English blocks stored as `sanskrit`** (now moved to `hopkins_english`); **1,531 entries with Sanskrit leaked into `hopkins_english`** (e.g. *yang dag par*: "samyak" now in `sanskrit`, "correctly; really; completely" in English); **~4,800 entries recovered English the old parse dropped at line wraps** (with-hopkins coverage 16,874 → **21,681**; e.g. *ka 'khor ba* recovered "cycling from the beginning; cycling from the first letter…"); 34 tense corrections; 17 unmatched stragglers manually moved (incl. one carrying the running page footer glued into its sanskrit field).
- **Founding rule 1 untouched:** Hopkins remains reference-only; **zero `hgm_gloss`, status, or pronunciation fields changed** (HGM-glossed exactly 12,004 before and after). Residual audits: 0 `{PH}` blocks in sanskrit fields, 0 letter-spacing artifacts.
- 119 matched entries where the fresh parse found no content were left untouched (flagged class, likely layout edge cases); 176 dict entries with Hopkins content found no match (old-parse truncated headwords) — review-pass candidates.
- Package regenerated via validated generators; ship check green.

### v0.25.1 changelog (2026-08-04) — 🧹 TCS09–TCS18 HYGIENE REPAIR (the back half of the practice series, rerun & repaired)
- Adam re-supplied the remaining 12 TCS files (TCS09–TCS18). **Nine of twelve diffed byte-clean against the corpus** — the newer-era ingests were sound. Three carried the same v0.10-era **page-number pollution class found in v0.25**: TCS16KaliWang (692 segments), TCS18LamaChupa (all 122), TCS18Commentary (78).
- **924 segments repaired in place** (English + acip/wylie refreshed from the clean files; positional alignment; 1 count-mismatched key group skipped). Residual audit: 0 trailing page numbers. **32 polluted attestation contexts refreshed.**
- **+2 genuine new segments** (a TCS12 ritual connective and a TCS16KaliWang mantra rubric). **Corpus 35,599 → 35,601.**
- Attestation/miners rerun: no new glosses or contexts — expected for a pure hygiene patch. Counts unchanged: in-corpus 14,361; HGM-glossed 12,004.
- **The full TCS01–TCS18 series has now been re-audited end-to-end against the current toolchain** (v0.25 + v0.25.1): 2,391 segments repaired, 34 recovered, all sources clean.
- Package regenerated via validated generators; ship check green.

### v0.25.0 changelog (2026-08-04) — 😈 THE DEVIL DEBATES AN ANGEL (Volume 111) + TCS HYGIENE REPAIR
- **New volume ingested: *The Devil Debates An Angel* (DDA)** — by the First Panchen Lama, translated by Geshe Michael Roach, Diamond Cutter Classics Volume 111, ©2026. The Great Seal (Mahamudra) debate poem: Wisdom versus "The Tendency to Think that Things are Real" — core Mahamudra/emptiness register. Docx ingest on the Sunlight/ILL recipe (TOC page-number lines filtered; [P###] markers scrubbed; one front-matter false ACIP hit dropped). **+516 unique segments (0 corpus duplicates — entirely new material). Corpus 35,051 → 35,599.**
- **TCS re-parse & hygiene repair:** Adam re-supplied all 15 TCS01–TCS08 ASCII files. Diff against corpus exposed a **systematic page-number pollution class in the v0.10-era TCS ingests** — e.g. corpus English read "…A Light for Finding Ecstasy **1**" where the source reads clean. **1,467 segments repaired in place** (English + acip/wylie refreshed from the clean files; positional alignment for repeated keys; 15 count-mismatched key groups skipped, never guessed). One TCS04Initiation segment recovered a lost phrase the old ingest had replaced with a bare "86". **+32 genuine new segments appended** (TCS07 ritual-instruction lines, TCS03 outline entries, TCS08PabongkaSadhana verses); folio-reference false hits rejected per the v0.24.1 rule. **34 polluted attestation contexts refreshed** from the repaired segments.
- **Attestation:** in-corpus 14,338 → **14,361**; +42 contexts; 72 awaiting entries newly evidenced by the wave.
- **Miners (v0.24 triage): +1 applied gloss** — *ngoms pa med* → "never satisfied" (4/9, `source_wave: dda-v25`) — and **39 auto-candidates** banked with evidence (incl. *ja* → "tea"). The triage held the line on a debate-poem register that mines noisily. **Total HGM-glossed: 12,003 → 12,004.**
- Package regenerated via validated generators; ship check green (per-source counts DDA 516 · TCS repairs audited to 0 residual pollution · zero wave duplicates · marker/IAST audits clean).
- **Review queue:** 224 books-v21 + 146 ill-v22 + 77 w3-v23 + 24 practice-v24 + 1 dda-v25 + 3,438 prior auto-aligned, on a 35,599-segment corpus.

### v0.24.1 changelog (2026-08-04) — 🔁 C-ASCII RE-PARSE PATCH (all 16 available course readings rerun under current rules)
- Adam re-supplied `C03–C18ReadingASCII.txt` (C01/C02 did not arrive in the upload) and asked for a rerun with the current toolchain. Full re-parse via canonical `parse_aligned` + normalized-key diff against the 35,027-segment corpus.
- **Verdict: the original ingests were near-complete.** 16 files → 45 differing segments, of which **13 were false ACIP hits on folio references and syllabus headers** (`13b`, `(9a)`, `650 ad.`, `ad).`) — correctly rejected, never appended.
- **+24 genuine micro-segments recovered (32 raw, 8 within-course repeats of identical exchanges collapsed — repeated identical pairs would inflate miner support counts, per the v0.23 lesson) (`reparse_wave: c-ascii-v25`, shipped as v0.24.1): the debate-dialogue particles** from the C13/C18 collected-topics readings — *'dod* → "Du! Right!", *yod de* → "Yu de B: Why so?", *med de* → "Me de A: Why not?", *ga re* → "Gare shak? Then show me something like that!" — plus the C14/C18 quotative *yang* → "He also said:" lines. The old parser folded these one-line exchanges into neighboring segments. **Corpus 35,027 → 35,051.**
- **Attestation/miners rerun (v0.24 triage): no new glosses and no new contexts** — the particles are already richly glossed high-frequency terms (*'dod* 31 HTG glosses, 2,728 occurrences; *ga re* already carries its direct-quote gloss). 8 entries received occurrence-count updates. Honest small patch; the debate register now exists as standalone corpus evidence for the future disambiguation layer.
- Package regenerated via validated generators; ship check green (per-source counts C13 +9 · C14 +3 · C18 +12 · zero wave duplicates · marker/IAST audits clean).

### v0.24.0 changelog (2026-08-04) — 🪷 DAILY PRACTICE WAVE: nine glyph-encoded Practice PDFs decoded & ingested
- **Ten Daily Practice PDFs received (P1, P3–P10 + `Diamond_Cutter_Sutra_Pecha.pdf`) — the banked v0.23 roadmap item fires.** All nine Practice readings are TibetanMachine glyph-encoded; the pecha is **excluded per rule 4**: its text layer is English-only (no glyph channel, no parallel content — same class as *Door to the Diamond Way*).
- **New extraction layer: dual-channel page parser** (`practice_parser.py`). pypdf supplies the glyph-name runs (exact-spelling channel); pypdfium2 supplies clean text (English with real spaces, phonetics, stanza line breaks; Tibetan appears as control-char garbage). The channels are zipped per page by Tibetan block-count; the T/X interleave is **never guessed** — 11 channel-mismatch pages (of ~800) were skipped and logged rather than risk space-less or misattached English. Passage walk is document-level, so a Tibetan passage at a page bottom picks up its English overleaf.
- **Recon correction logged:** verbatim `harvest()` on Practice6 reproduces **54** pairs, not the 156 logged in the v0.23 reconnaissance note (the practice PDFs are prose-interleaved, not card-formatted; only the shared opening prayers + P10 carry phonetic lines). The structural parser supersedes `harvest()` for this class: P6 alone exposes 491 glyph lines.
- **Prayer cards decoded: 242 harvested** (the shared mandel/kyabdro-semkye/ngowa/chupa set ×9 + P10's *A Thousand Angels* prayer book). Two new decode rules, both validation-gated: **yig-mgo ornament** (`zla.tshes.gnyis`) treated as shad; **glyph alias `baĸ` → bkr**, which round-trip **validated** the mandala line *sa gzhi spos kyis byugs shing me tog bkram*. Per shared set: 14 validated · 5 lexicon-validated · 1 undecodable (the `ma n` title stack); P10: 51 · 28 · 2 unvalidated · 1. **`decoded_all_courses.json` now 2,286 cards across 27 sets** (18 courses + 9 practices): 1,724 validated · 460 lexicon-validated · 55 prefix · 31 unvalidated · 16 undecodable.
- **Passage decode & ingest:** 1,108 Tibetan–English passage segments built; per-syllable lexicon decode (3,110-token lexicon from unified wylie+variants), mean lexicon score 0.993. Gates: score ≥0.9 (8 dropped), wave-dedupe (34), corpus-dedupe (**284** — see below), undecodable-run drops logged per source (89 doc-wide, mostly mantra/dharani and title-stack lines). **+782 unique segments appended; corpus 34,245 → 35,027.** New segments carry `wylie_method: glyph-decode` + `decode_score`; English filtered by the standing mantra/transliteration line rules.
- **🎯 Independent end-to-end decode validation, free of charge:** the 284 corpus-duplicate rejects are glyph-DECODED segments whose wylie + English matched existing ACIP-derived segments **character-for-character** after normalization — 113 against C03, 53 C05, 46 C15, 15 C16, plus C06–C18/SOS scatter. The glyph pipeline independently reproduces the ACIP ground truth at passage length.
- **Attestation (token recount over the full 35,027-segment corpus):** 14,315 entries re-attested; in-corpus 13,945 → **14,338** (incl. 23 prior-method holdovers whose evidence was preserved untouched); **+131 contexts** on awaiting entries; **170 awaiting entries newly P-evidenced**. **Tokenizer fix logged as standing rule:** unicode letters are preserved in wylie tokenization — *paṇ* no longer collapses to *pa*. The first mining pass exposed the resulting support-inflation class (junk glosses on *ko/ya/ra/li/haṃ* and high-frequency single syllables); caught **in-session before ship** per the v0.23 lesson, miners rerun from the pristine v23.1 master.
- **Miners with tightened triage** (support/segments ratio ≥ 0.25 for auto-apply; single-syllable entries auto-apply only when total attestation ≤ 30 segments; boundary/stopword rules as before): **+24 provisional glosses (`source_wave: practice-v24`)**, incl. *yi dwags* → "craving spirit" (22/28), *rten gyi stobs* → "power of the foundation", *chos bzhi bstan pa'i mdo* → "four practices", *'dab chags* → "winged ones", *bar bar du* → "time to time". **71 intersection + 5 quote-pattern demotions to `auto_candidate`** with evidence. **Total HGM-glossed: 11,979 → 12,003.**
- Package regenerated from master via the validated generators; ship check green (per-source counts vs parse report · zero duplicate keys introduced by the wave — 360 pre-existing normalized-key collisions in older sources are repeated liturgical refrains, not doubled appends, and involve no P source · marker-residual 0 — the single hit is [1932], a year in HGM's own English; audit regex now excludes 4-digit years · IAST-density 0 on the new wave).
- **Review queue:** 224 books-v21 + 146 ill-v22 + 77 w3-v23 + **24 practice-v24** + 3,438 prior auto-aligned, one pass, on a 35,027-segment corpus.

### v0.23.1 changelog (2026-08-04) — 🧹 CORPUS HYGIENE PATCH (no reupload required — audited & repaired in place)
- Adam asked whether earlier documents needed reuploading so newer parser rules could catch what older ingests missed. **Answer: no** — all extracted texts remain on disk and the corpus was audited directly. The audit found three defect classes, all in ingests that predate the rules: **(1) Sunlight v0.10:** unscrubbed `[N]` markers in ACIP and Sanskrit passages joined into English fields; **(2) MB2 v0.21:** romanized-Sanskrit (IAST) passages in English fields — the Medicine Buddha books carry transliterated sutra text; **(3) books-v21 glosses** never passed through the v0.22 boundary/stopword triage. All other sources audited clean.
- **Repairs (corpus-wide, in place):** 399 segments marker-scrubbed with wylie regenerated · 347 Sanskrit suffixes stripped from English via new windowed IAST-density rule · 159 segments dropped entirely (hollow after cleaning) · **corpus 34,404 → 34,245** · residual audit: 0 markers, 0 IAST-dense English. **551 attestation contexts cleaned** with the same rules. Gloss-text IAST audit: **0 leaks** (mining had not absorbed Sanskrit).
- **Retroactive triage:** 245 books-v21 glosses demoted to `auto_candidate` under the boundary/stopword rule (books-v21 now 224 kept). **Total HGM-glossed: 12,224 → 11,979** — smaller and honester; every demotion preserved as a candidate with evidence for the review pass.
- **Standing rule added to ship check:** marker-residual and IAST-density audits now run alongside per-source segment counts.

### v0.23 changelog (2026-08-04) — 🌅 THIRD BOOK WAVE + Practice-reading reconnaissance
- **Three more HGM volumes ingested:** *Emptiness Meditations* (EM, +1,146 segments — entirely new), *Sunlight on Suchness* (SOS, 4-language, +391), *Sunlight on the Path to Freedom* (SPF, 4-language, +482 new / 488 duplicates — confirming ~half overlaps the original v0.10 Sunlight ingest; the new half is material the first pass missed). **New parser rule logged:** IAST-diacritic density filter strips romanized-Sanskrit lines in 4-language editions (608 lines in SPF); `[P###]` page markers added to the embedded-marker scrub (257 segments re-scrubbed). **Corpus: 32,385 → 34,404 segments (+2,019 unique after dedupe).**
- **Attestation:** 5,070 entries attested in the wave; in-corpus 13,777 → 13,945; +302 contexts on awaiting entries.
- **Miners** over 399 newly-evidenced awaiting entries with the full triage stack: initial run applied 174, but the ship check exposed a **doubled wave in the corpus** (an interrupted session had part-written the append; the continuation appended again), inflating support counts — a single context could masquerade as support-2. **Incident handled per founding rules:** corpus deduplicated (36,423 → 34,404), all 174 w3 glosses reset, miners rerun on clean evidence. **106 of 174 turned out to be inflation artifacts and no longer qualify; 22 changed; net +77 verified provisional glosses (`source_wave: w3-v23`)**, incl. *dkon mchog brtsegs pa* → "pile of jewels" (the Ratnakūṭa); 141 demoted to `auto_candidate`. **Total HGM-glossed: 12,147 → 12,224.** Lesson logged: ship checks MUST verify per-source segment counts against parse reports — this is now a standing checklist item.
- **Practice-reading reconnaissance:** `Practice4/6/8Reading.pdf` are glyph-encoded Tibetan in the Study Guide legacy font — `harvest()` **works** (156 card-pairs recovered from Practice6). Full decode of long liturgical lines exceeds interactive budget; **banked as roadmap item** (batch decode pass; liturgical register would extend pronunciation coverage into recitation texts).
- Package regenerated with validated generators; ship check pending below.
- **Review queue:** 469 books-v21 + 146 ill-v22 + 77 w3-v23 + 3,438 prior auto-aligned = one pass on a 34,404-segment corpus.

### v0.22 changelog (2026-08-04) — 🕯 ILLUMINATION OF THE TRUE THOUGHT (Volume 101)
- **`ILLUMINATION_5_28_26.docx` ingested — Tsongkapa's *dgongs pa rab gsal*, translated by Geshe Michael Roach, Diamond Cutter Classics Volume 101, ©2026.** The largest single source in the project's history: 8,385 ACIP lines → 5,731 parsed → 4,911 kept → **4,907 new unique segments** (only 4 duplicates vs. corpus) — one volume larger than the entire six-book wave. **Corpus: 27,478 → 32,385 segments.** This is the core Prāsaṅgika register: Candrakīrti's *Madhyamakāvatāra* through Tsongkapa's commentary, precisely the terminology base the translation tool needs.
- **Attestation:** 6,658 entries attested in ILL; in-corpus coverage 13,233 → 13,777; 873 awaiting entries newly evidenced; +910 contexts.
- **Miners:** 392 intersection candidates; 18 demoted at source (1-syl weak-majority). Dense philosophical prose degraded capture quality (fragments like "that because the"), so a **new boundary/stopword triage rule** was added and logged: demote when the gloss starts or ends with a function word or is ≥50% stopwords — 228 further demotions to `auto_candidate`. **Net +146 provisional glosses (`source_wave: ill-v22`), incl. *rdza mkhan* → "potter", *lang kar gshegs pa* → "journey to langka", *dpyad bzod* → "stand up under". Total HGM-glossed: 12,001 → 12,147.** All 246 ILL demotions preserved as candidates for the review pass.
- Package regenerated with the v0.21-validated generators; ship check green.
- **Review queue now:** 469 books-v21 + 146 ill-v22 + 3,438 prior auto-aligned glosses, one pass, on a 32,385-segment corpus.

### v0.21 changelog (2026-08-04) — 📚 THE BOOK WAVE: six HGM-translated volumes ingested
- **Seven books received; six ingested** (Diamond Cutter Classics / HGM-translation format, ACIP–English aligned): *The Two Sutras of the Medicine Buddha* (MB2, +760 segs), *Medicine Buddha Long Sutra* (MBL, +3 — the manuscript is contained almost entirely within MB2; 665 exact duplicates skipped), *Great Ideas of the East* (GIE, +1,282), *All Karma* (AK, +890; 787 CJK lines filtered per Sunlight precedent), *A Door to Emptiness* (DTE, +1,264 — the debate-manual register, "coffee cup" convention attested in the wild), *The Golden Key* (GK, +371; verse format; translated "Word Smith with Geshe Michael Roach" — co-translation credit logged). **Excluded per rule 4:** *Door to the Diamond Way* — zero parallel Tibetan–English content (English teaching text + bibliography; apparent ACIP hits were ISBN/catalog false positives). 659 segments scrubbed of embedded [G###]/[f. ###a] markers with wylie regenerated.
- **Corpus: 22,908 → 27,478 segments (+4,570).** Attestation rebuilt once over the final corpus per the books-first decision: **6,310 entries attested in the new volumes; in-corpus coverage 12,467 → 13,233; 1,163 contexts added** to awaiting entries.
- **Miners rerun** (intersection support ≥2 phrases / ≥3 unigrams, df ≤ 274; quote-pattern) over the 1,011 newly-evidenced awaiting entries: 502 candidates applied, then **quality triage demoted 33 to `auto_candidate`** (9 quote-pattern junk captures — the regex caught "the", "heaps", "warmth"; kept only *ldog pa ngos 'dzin* → "Identifying Reverses of All That Are Not", *brda sprod pa* → "Linguists", *'phags pa'i rigs* → "The Elder Group" — and 24 single-syllable weak-majority intersection hits, e.g. the liturgical co-occurrence artifact *ra* → "saparivara"). **Net +469 provisional glosses (tagged `source_wave: books-v21`). Total HGM-glossed: 11,532 → 12,001.**
- **Full package regenerated from master via validated generators** (surgical patching impossible at this change volume). Generators were round-trip validated against v0.20: every file-format rule recovered (csv.writer TSV with minimal quoting; `card` pron-source convention; gloss unwrap-if-fully-quoted; sanskrit[:200] TSV / [:110] full-md caps; hopkins[:150] full-md cap; compact item-caps gloss 4 / hopkins 3 / sanskrit 2 with {tag}-strip, bracket-filter, trailing-period strip, case-insensitive dedup; reverse-index keys 2–60 chars lowercased; Part III sorted by wylie). All residual diffs classified as corrections, not generator error: **(a) TSV newline bug FIXED** — 21 card-derived rows were joined with bare `\n`, hiding 19 entries inside 2 mega-rows, invisible to row-level filtering; every entry now occupies its own record (26,322 rows verified). **(b) 16 pron-source cells normalized** to the `card` convention (my v0.18–v0.20 surgical patches had written full source strings). **(c) ~490 stale pronunciations refreshed** — v0.17-era renderings predated later card stamps (e.g. *kunnjung* → card-stamped *kunjung*, C09); master is authority. **(d) 16 junk reverse-index keys dropped** ('(1)', '--', etc.).
- Auto-candidate schema normalized (string gloss + `auto_candidate_evidence` side-field); full-md now renders ⟨candidate: …⟩ for demoted awaiting entries.
- **Review queue note:** all 469 book-wave glosses are provisional pending Adam's tier review, which now happens once, on the final corpus.

### v0.20 changelog (2026-08-04) — 🏁 C17 + C06: THE GUIDE SERIES IS COMPLETE (all 18 courses)
- **`C17Tibetan.pdf` and `C6Tibetan.pdf` received — the last two missing guides.** Both harvested and decoded in one pass: **C17: 183 cards** (145 validated · 33 lexicon-validated · 3 prefix · 1 unvalidated · 1 undecodable) · **C06: 90 cards** (68 · 17 · 3 · 1 · 1). Harvest↔banked-phonetic cross-check: C17 179/187 distinct overlap, C06 84/94 — banked-only remainders are the known multi-line/layout class.
- **Milestone: `decoded_all_courses.json` now covers the entire 18-course ACI curriculum — 2,044 decoded cards** (1,561 validated · 392 lexicon-validated · 55 validated-prefix · 29 unvalidated · 7 undecodable; ~95.5% fully validated tiers). Failure classes stable and consistent across every course: Sanskrit-stack names (*kamalashila* joins *sakya pandita*), multi-line verse cards (the *semkye pa ni* verse fails identically in C17 and C06 — same signature as its C02 sibling), g.y-orthography edges (*g.yang sa*). Never guessed.
- **C17: 0 new stamps** — pure late-curriculum review; 135 of its decoded terms were already card-attested from earlier courses. 44 non-headword lines banked.
- **C06: 2 new stamps, both phoneme-level card corrections** (banked as engine-rule evidence, engine untouched pending regression battery): *rabtu jungwa* (card retains b before fused *tu*, against the devoicer) · *sanggyepa* (card writes the full junction geminate, against the *sangye* sandhi rule). Both readable directly on the card. 27 non-headword lines banked. **Total card-attested: 1,308.**
- Release regenerated by verified surgical patch (2 entries + headers + changelog); every replacement hit exactly once.
- **The glyph-decode roadmap item is CLOSED.** Remaining pronunciation work: banked-only line inspection (27 C13 · 28 C14 · 6 C16 · 8 C17 · 10 C06), 887 unmatched ground-truth lines, engine-rule regression battery over the accumulated card-correction evidence (now 7 phoneme-level pairs from v0.18–v0.20), card-derived-entry pass over ~275 banked non-headword decoded lines.

### v0.19 changelog (2026-08-04) — C18 arrives (glyph-decode pass, 16 courses)
- **`C18Tibetan.pdf` received** — one of the two guide PDFs never previously seen (C06 and C17 still outstanding). TibetanMachine glyph-name channel live as expected; **143 cards harvested and decoded in one pass**: 94 validated · 33 lexicon-validated (89%) · 10 validated-prefix (multi-line BCA/lojong verse cards, phonetic covers the opening — known class) · 5 unvalidated · 1 undecodable.
- **Cross-course consistency check for free:** C18's failures are the *same cards* that failed in C14 (*kadampa geshe chekawa*, *chekawa chekepa*, *sempa chenpo langri tangpa…*, the *nga'am* debate formula, Sanskrit-stack *sakya pandita*) — identical failure signatures on identical content across independent PDFs corroborate that the decoder fails honestly and deterministically, never by guess.
- `decoded_all_courses.json` now **1,771 cards across 16 courses** (1,348 validated · 342 lexicon-validated · 49 validated-prefix · 27 unvalidated · 5 undecodable).
- **3 new card stamps** (headword-exact; C18 is late-curriculum review, so 74 of its decoded terms were already card-attested from earlier courses): *nyerdok* (pron unchanged, source stamped) + 2 segmentation-only overrides, card governs: *michokme*, *samten dangpo*. **Total card-attested: 1,306.** No phoneme-level corrections needed — the engine matched every C18 card at the phoneme level.
- **51 non-headword decoded lines banked** (`stamp_evidence` c18 set): Śāntideva verse quotations, the eight-verses colophon, definition-length clauses — candidates for the card-derived-entry pass.
- Release regenerated by verified surgical patch (3 entries + headers + changelog); every replacement hit exactly once.
- Outstanding: **C06 and C17 guide PDFs** (the last two); banked-only line inspection (27 C13 · 28 C14 · 6 C16); 887 unmatched ground-truth lines; auto-aligned tier review.

### v0.18 changelog (2026-08-04) — glyph-decode pass complete through C16 (15 courses)
- **The v0.17 decode blocker cleared:** the canonical `pron_engine.py` (validated 42/42) arrived and re-passed a 10-item convention spot battery on receipt; no reconstruction was ever attempted, per standing rule.
- **C15 regression control: 99/99 distinct cards reproduced the banked decode exactly** (same wylie, same status, zero drift) — extraction + decoder + engine + lexicon chain verified end-to-end before any new decoding.
- **C13/C14/C16 decoded: 443 cards** (lexicon = 3,110 syllable-tokens from unified wylie + variants). Results: C13 152 (125 validated · 25 lexicon-validated · 1 prefix · 1 unvalidated) · C14 80 (52 · 18 · 1 · 7 unvalidated · 2 undecodable — the Sanskrit-stack names *sakya pandita*, *dharma rakshita*) · C16 211 (170 · 38 · 1 · 2 unvalidated). Merged into `decoded_all_courses.json`: **1,628 decoded cards across 15 courses** (1,254 validated · 309 lexicon-validated · 39 validated-prefix · 22 unvalidated · 4 undecodable). Failures are the known classes: Sanskrit stacks, multi-line verse cards (phonetic covers only the opening → validated-prefix), missing-tsheg junction glitches. Never guessed.
- **8 new card stamps** (headword-exact matches only; matches routed through `wylie_variants` are excluded from pronunciation stamping after one such mismatch was caught in review — the *ngenjung* card would have overwritten the long-phrase entry). 3 segmentation-only overrides (card governs): *chu-nyi, den nyi, hlenchik mi-ne gel*. **5 phoneme-level card corrections** banked as engine-rule evidence, engine untouched pending regression battery: *drupjay chu* (devoicer on sgrub), *dakdzin* (no prenasal after -k here), *sunjinpa*, *pentsun panggel*, *loppon chukyi drakpa*. **Total card-attested: 1,303.**
- **153 decoded non-headword card lines banked for review** (multi-term cards, lineage names, verse quotations — candidates for card-derived entries at next pass, per the v0.17 recipe). 1 misaligned harvest pair flagged (*tokpa* phonetic paired with *rtag/rtog* ambiguity), not stamped.
- Release regenerated by **surgical patch** of the v0.17 package (8 entries + headers + changelog), every replacement verified to hit exactly once — zero format drift.
- Still outstanding: C17 guide PDF (harvest + decode); C06/C18 guide PDFs never received; 887 unmatched ground-truth phonetic lines; banked-only line inspection (27 C13 · 28 C14 · 6 C16).

### v0.17 changelog (2026-08-04) — glyph-decode pass (12 courses) + C12
- **New channel: `glyph_decoder.py`.** The "font-locked" guide PDFs embed the Tibetan as TibetanMachine glyph-name runs; the decoder recovers exact EWTS wylie AND Unicode Tibetan per card, validated by pronounce() round-trip against the card's own phonetic, with unified-entry lexicon corroboration as second channel. 12 PDFs decoded (C01–C05, C07–C12, C15): 1,185 cards, ~96% validated. The glyph stream preserves the g.y/gy orthographic distinction invisible to phonetics.
- **C12 (BCA Part III, the wisdom chapter) newly banked**: 141 phonetic lines (card_phonetics.json → 2,118 lines), 13 pron-channel stamps.
- **62 wylie-channel stamps** on entries the pronunciation channel could not reach (card contradicts engine: dukngel, sukkam, nabsem, dzute, mi-shepa…) + **6 entries now carry `pronunciation_variants`** where GMR's own cards disagree across courses (nyunmong C01 vs nyonmong C07/C10; kundzob vs kuntzob C15; gyelwa vs gyalwa) — recorded, never resolved, per founding rule 3. **Total card-attested: 1,295.**
- **423 card-derived new entries** (unified: 25,899 → 26,322): ≤8-syllable single-clause cards with no existing headword; tibetan (Unicode from glyphs) + wylie + ACIP + pronunciation all card-attested; status awaiting-hgm-equivalent; flag `card-derived-entry`; rendered as Part III in the full .md pending interleave at next full regeneration. Longer verse/phrase cards stay banked in decoded triples.
- **DEV-002 logged**: C09 guide prints 'pqmpa' for pham pa — card typo, excluded from variants.
- **Pending**: C13/C14/C16/C17 glyph decode (PDFs displaced by later uploads — re-upload to complete); C06 and C18 guide PDFs never received; engine-rule regression pass over banked counterevidence (k+ng junction: dukngel; final-'o: lo-o; k+gy: lok-yem).

### v0.16 changelog (2026-08-04) — card pass III (Courses VII–XVII)
- **Language Study Guides for Courses VII–XI and XIII–XVII ingested (10 PDFs): 1,436 more GMR phonetic lines banked** (card_phonetics.json now 1,977 lines across C01–C17). Extraction upgraded to font-anchored layout parsing (Palatino-Italic/BoldItalic phonetic lines, x-gap card splitting), validated to 440/444 exact reproduction of the C01–C05 bank.
- **369 entries newly card-attested**: 322 exact engine matches, 47 segmentation overrides applied (card governs: *gan-gu, si-tse, du-hla druk, tsa-nyel gye, hlamayin, nyomle, chiyang me, tsok druppa, ma sepa, geway chu dupay tsultrim*, …). **Total card-attested pronunciations: 797** ⟪card⟫. Zero pronunciation-text corrections on exact matches — the engine held on every direct hit.
- The 47 override pairs are banked as evidence for future engine-rule extensions (hyphen-compound list, fusion rules); `pron_engine.py` deliberately untouched this release pending a regression battery.
- 887 unmatched lines (names, verse quotations, multi-term composites) remain banked ground truth for calibration.
- Missing from this batch: C06 (already ingested v0.15), C12, C18 — decks XII and XVIII process identically on upload.

### v0.15 changelog (2026-08-04) — card pass II (Courses V–VI)
- **Language Study Guides for Courses V (How Karma Works) and VI (Diamond-Cutter Sutra) ingested: 218 more GMR phonetic lines.** 131 exact engine matches, 37 segmentation overrides applied (card governs). **Total card-attested pronunciations: 428** ⟪card⟫.
- The C06 deck is register-precious: it card-attests the Diamond Cutter analytical vocabulary (*kundzob semkye*, *dundam semkye*, the three/four bodies *ngowo nyiku / longku / trulku / yeshe chuku*, the four-extremes lines) — exactly the terms the Sunlight corpus feeds on the gloss side.
- Unmatched lines (110) are again names, multi-term composites, and full verse quotations — banked in `card_phonetics.json`.

### v0.14 changelog (2026-08-04) — card-attested pronunciations (Language Study Guides I–IV)
- **Four ACI Tibetan Language Study Guides ingested** (Courses I–IV vocabulary decks; Tibetan layer font-locked as in the C01 PDF, but **323 GMR phonetic lines extracted** as pronunciation ground truth).
- **Engine validation at scale:** of the card lines matching dictionary entries, the engine's output was phoneme-identical in every case and segmentation-identical on ~91%; the **27 segmentation differences were overridden by the card forms** (card is ground truth), and a comma-split second pass matched 39 more pieces. **288 entries now carry `pronunciation_source: GMR card`**, marked ⟪card⟫ in the .md — the highest pronunciation-evidence tier, above engine derivation.
- **Source variance logged, not resolved:** the C01 deck itself writes both *suk-ku* and *su-ku* for གཟུགས་སྐུ within two pages, and *kuntzob* beside our *kundzob*; era/deck variance in GMR materials remains documented in §v0.8. Our normalized standard stands; card stamps mark direct attestation only.
- Remaining unmatched card lines are proper names (lineage figures), composite multi-term lines, and sentence-length examples — banked in `card_phonetics.json` for the future pronunciation-review pass and for validating the engine against Courses V–XVIII decks when uploaded.

### v0.13 changelog (2026-08-02) — Sunlight on the Path to Freedom
- **First book-length HGM translation ingested:** *Sunlight on the Path to Freedom*, Choney Lama Drakpa Shedrup's complete commentary on the Diamond Cutter Sutra (Diamond Cutter Classics vol. 72, 2019). The docx's body is ACIP–English paragraph pairs throughout ([1]–[676]) plus the quadrilingual Diamond Cutter Sutra appendix (Sanskrit / Chinese / Tibetan / English, [K#] sections). Curly-apostrophe normalization applied; Chinese lines and folio markers filtered. **1,098 new aligned segments.**
- **Corpus: 22,908 segments** (ACI curriculum + first Diamond Cutter Classics volume). Attestation rebuilt: **12,467 entries attested in-corpus.**
- **Miners rerun over the grown corpus: +378 new auto-aligned glosses** (370 intersection, 8 quote-pattern) — sutra-commentary register vocabulary the courses didn't cover. **Total HGM-glossed: 11,532.**
- Release package now standing at **seven files**: JSON master, TSV, reverse index, full MD, compact edition, attested edition, README.
- Pipeline note: this ingestion doubled as the template for the whole Diamond Cutter Classics series — any volume with the ACIP–English body format drops straight in.

### v0.12 changelog (2026-08-02) — the fill phase begins (auto-aligned tier)
- **HGM English extracted from the full 21,810-segment corpus and brought into the dictionary: 11,154 entries now carry HGM glosses, up from 8,094** (+3,060 net). Every new gloss is verbatim HGM English from his own translations — machine-*matched*, never machine-*invented*.
- **Three extraction methods, tagged per-entry in `hgm_evidence`:** (1) *direct-segment* — short aligned segments pairing a term with its rendering; (2) *intersection mining* — an English phrase recurring across ≥2 of a term's bilingual contexts, filtered for global rarity (majority-support scoring); (3) *quote-pattern* — "what we call the 'X'" constructions in segments containing exactly one unglossed term (52 high-confidence hits, e.g. ཚོགས་ཞིང་ → "Collection Field").
- **The auto-aligned tier is provisional by design**: distinct from curated and glossary tiers, marked `[HGM·auto]` in the compact edition, with method + confidence + support counts recorded. Sample quality is genuinely mixed at the medium tier — gold (*thod pa* → "cup of skull", *zung 'jug* → "secret union") beside noise — so **449 weakly-supported single-word glosses were demoted** to an `auto_candidate` field rather than allowed to occupy the gloss position; their entries remain officially awaiting.
- **Review workflow:** filter the TSV on `auto-aligned`, confirm or correct against the entry's `attestation_contexts`, and promotions become permanent. The compact edition (now a standing sixth file in the release package) shows all four evidence tags for at-a-glance triage.

### v0.11 changelog (2026-08-02) — the complete ACI corpus
- **TCS10–TCS18 integrated (11 files)**, completing the entire Course Series: including the **Lama Chupa** (*Offering to the Lama*) root text (122 segments) and its full commentary (395), the **Kali Wang** initiation text (737), and the large TCS13 (1,053) and TCS17 (1,367) supplements.
- **The corpus is now the complete ACI curriculum: 21,810 aligned Tibetan–English segments** across 18 formal courses + 27 practice/supplement files. Reshipped as `full_parallel_corpus.json`.
- **Attestation totals: 12,360 entries attested in-corpus (48% of the entire dictionary); 4,862 awaiting-entries carry bilingual contexts** — nearly five thousand pre-loaded fill targets. Newly context-rich: ཚོགས་ཞིང་ (tshogs zhing, contexts from C01/TCS01/TCS04-Initiation), ཟུང་འཇུག་ (zung 'jug).
- Milestone note: with the Great Dictionary scaffold, the Hypertext Glossary layer, the eighteen courses, and the complete practice series all integrated, the project's source-acquisition phase for the core ACI corpus is essentially **complete**. What remains is the fill phase (4,862 context-bearing entries → curated equivalents), the disambiguation layer, and ingestion of HGM's book-length translations as they become available.

### v0.10 changelog (2026-08-02) — the practice corpus
- **Sixteen TCS practice-series files integrated**: TCS01–TCS09 course supplements plus the standalone practice texts — *The Thousand Angels* (dga' ldan lha brgya ma), *The Source of All My Good* (yon tan gzhir ma), Naropa's Vajra Yogini sadhana, the Pabongka sadhana, Naropa's Twelve Trials, and the Khen Rinpoche long-life supplication. Largest: TCS04-Initiation (1,009 segments), TCS07 (809), TCS06 (603), TCS09 (570).
- **Combined corpus now 14,861 aligned segments** (courses + practice texts), reshipped as `full_parallel_corpus.json`. The practice texts add the liturgical and Vajrayana registers the formal courses lack — lama-devotion, sadhana, and long-life vocabulary.
- **Attestation rebuilt over the combined corpus: 11,666 entries attested (up from 10,431); 4,215 awaiting-entries now carry bilingual contexts (up from 3,075).** Newly context-rich examples: རྣལ་འབྱོར་མ་ (rnal 'byor ma) with contexts from C14/C18/TCS01/TCS02; མཁའ་འགྲོ་མ་ (mkha' 'gro ma) from the initiation and prayer-book texts.
- The TCS files also carry hundreds more GMR phonetic lines (e.g. *ganden hlagyay gun gyi tukka ne*) — additional calibration ground truth banked for future pronunciation-engine refinement.
- Note for the fill phase: prayers repeat across TCS files (mandala, refuge), so occurrence counts in liturgical vocabulary run high; the `courses` list per entry disambiguates breadth from repetition.

### v0.9 changelog (2026-08-02) — the full eighteen-course corpus
- **All seventeen remaining ACI course readings (C02–C18, ASCII/ACIP editions) parsed and integrated in one pass**, using the C01 pipeline unchanged (identical format throughout). Combined with C01, the parallel corpus now spans **8,983 aligned Tibetan–English segments across all 18 courses** — the complete formal-study curriculum. Shipped as `full_parallel_corpus.json`. Densest courses: C18 (1,158 segments), C13 (1,042), C16 (1,021), C17 (757).
- **Corpus attestation pass over every unified entry** (n-gram matching on all wylie variants): **10,431 entries occur in the corpus.** Attested entries gain `corpus_occurrences` (which courses, how many segments) — evidence breadth for the translation tool's ranking.
- **3,075 awaiting-entries now carry `attestation_contexts`**: up to two real bilingual segments each (course, source Tibetan, HGM's English) embedded directly in the JSON master. These entries are no longer merely "awaiting" — each holds the exact raw material from which its equivalent can be curated. Example: ཀ་བ་ (ka ba, "pillar") arrives with the very passage from the labeling discussion where HGM renders it.
- **The gloss-extraction phase is now unblocked at full scale:** the fill pipeline is (1) read an entry's contexts, (2) identify HGM's rendering, (3) promote to `hgm-attested` with citation. This can proceed course-by-course in future sessions, with the deviation log catching textual variants as in DEV-001.

### v0.8 changelog (2026-08-02) — card-calibrated pronunciation
- **Fifteen additional GMR vocabulary cards ingested as ground truth**; the engine now passes a **42/42 combined battery** (new cards + original screenshots + ACIP catalog examples).
- **Rules confirmed or newly derived from the cards:** *o+n → u* restored as the general rule (*dunche*, *shendun*, *nyundrip*, *ngunsum*) with lexical exceptions *ton/kon*; la-don particles split by form — *tu* fuses solid (*chiktu*, *raptu*), *su* fuses with hyphen (*se-su*), *du/ru/te/ste/de* stand alone (*tomen du gyurwa*, *madrup te*); *rnams* and *tsam* stand alone (*nam kyi*, *shak tsam*); negation absorbs following formatives (*malupar*); prenasal insertion is blocked only after p-final syllables (*delnjor*, *tangok*, but *kyabdro*); junction *k+ng → ng* (*brtags 'gog* → *tangok*); lexical devoicers *thob→top*, *grub→drup*, *sgrib→drip* (while *jangchub*, *kundzob* keep b).
- **Documented source variance (normalized, not hidden):** GMR materials alternate *ge-ba/ge-wa*, *sang-gye/sangye*, hyphen-heavy vs. solid era-styles, *druppay/topar* geminates, *yin pay* split genitives, and *so-sor/soso* reduplication — we normalize to the solid/dominant forms and keep the hyphen list curatorial.
- **Known lexical-segmentation limits flagged, not guessed:** juxtaposed monosyllabic nouns (*lha mi…* "gods and men"), head-modifier odd groupings (*blo gnod med* → *lo nume*), and enumeration-style splitting (*shi lam nam sum*) can't be derived from surface form; such entries stay in `pronunciation-review` (244 currently).

### v0.7 changelog (2026-08-02) — word-segmented pronunciation
- **Pronunciation output is now word-segmented per GMR's own convention** (modeled on his printed vocabulary cards): the flagged example དངོས་པོ་ལ་ལྷར་མ་སོང་ཡང now reads ***ngupo la hlar masong yang*** instead of one unreadable join.
- **Segmentation rules:** lexemes joined (*ngenjung*, *semkye*, *sherab*); formative suffixes (pa/ba/po/bo…) attached (*gelukpa*, *tekpa chenpo*); genitive *'i* fused into its word (*yangdakpay tawa*); connective particles standalone (*kyi, dang, la, yang, ni, nas, las*); negation *ma/mi* fused forward onto the verb (*masong*, precedent: *lame* = bla med, *marikpa*); la-don enclitics *tu/du/su/ru/ste/te* fused backward (*raptu tsawa*); noun-noun compounds hyphenated via a curated list (*chu-ku*, *su-ku* — extensible as more of GMR's cards are ingested).
- **Validation: 11/11 against the GMR vocabulary-card screenshots, 17/17 against the ACIP catalog examples.** Known ambiguity: *yang/la/dag* are particle-or-lexeme homographs; segmentation is positional and flagged cases go to review (244 entries carry `pronunciation-review`).
- Engine (`pron_engine.py`) reshipped with the segmentation layer; all 25,899 pronunciations regenerated.

### v0.6 changelog (2026-08-02) — pronunciation + ACIP on every entry
- **Every entry now carries two new fields:** `acip` (ACIP transliteration, standing rule) and `pronunciation` (simplified pronunciation per the ACIP Release 4 Catalog standard, pp. 173–178).
- **The pronunciation engine** (`pron_engine.py`, shipped with this release) implements the full catalog chart — silent prefixes, umlauting suffixes, prenasal insertion, suffix devoicing, genitive *'i* → *y* — **validated 41/41 against the catalog's own worked examples and 22/22 against GMR's course-phonetics conventions.**
- **Where catalog and GMR course usage diverge, GMR governs** (documented refinements): *o* umlauts only before *d/s* (hence *tonpa*, *konchok*, not tunpa/kunchok); suffix *-b* stays voiced before voiced onsets and word-finally (*jangchub*, *kyabdro*) but devoices before voiceless (*raptu*) and with second-suffix *s* (*chumbep*); prenasal insertion only after open syllables (*Ganden*, *tanjuk* — but *kyabdro*, not kyabndro); junction sandhi *sangs rgyas* → *sangye*, *tshogs kyi* → *tsokyi*, *bdag gis* → *dakki*, *nyi zlas* → *nyinde*. Known residual variance: geminate vs. single stops at some junctions (source itself alternates *druppar* / *topar*); geminate kept.
- 240 entries flagged `pronunciation-review` (exotic Sanskrit transliterations, non-syllabic headwords).
- Curated entries retain the old hand phonetics as `phonetics_legacy`; the standardized `pronunciation` field is authoritative.
- Sample: ངེས་འབྱུང་ ▫ nges 'byung ▫ NGES 'BYUNG ▫ *ngenjung* — "renunciation."

### v0.5 changelog (2026-08-02) — the C01 ASCII source layer
- **The ASCII/ACIP edition of the Course I reading removes the project's biggest bottleneck.** The PDF's Tibetan was locked in legacy fonts; the ASCII edition supplies 889 machine-readable ACIP Tibetan lines, parsed into a **497-segment aligned Tibetan–English parallel corpus** (`c01_parallel_corpus.json`, shipped alongside this release).
- **102 of 115 curated entries are now attested in real source Tibetan** (upgraded from "canonical vocabulary" status). The remaining 13 are section-title or note-derived terms.
- **22 new curated entries** mined from newly legible passages (grace prayer, dedication, commentary colophon and opening) — 21 of 22 independently confirmed by the Hypertext Glossary. New HGM signatures captured: *rdo rje 'chang* = "the Holder of the Diamond," *'jam dbyangs* = "Gentle Voice," *mkhyen brtse nus gsum* = "knowledge, love, and power," *bla med* = "matchless / higher than any other," *dkon mchog gsum* = "the Three Jewels," *sku gnyis* = "the two ultimate bodies."
- **Correction:** the dedication's actual English is "complete the collection of **merit and wisdom**" (ASCII-attested), fixing the earlier reconstructed example and further confirming the *bsod nams* register finding.
- **Deviation log opened (DEV-001):** the ASCII edition's homage line reads *rje btsun 'jam pa'i dbyangs la phyag 'tshal lo* (to Gentle Voice) where HGM's English reads "I bow to all the high and holy lamas" (*rje btsun bla ma rnams la…*) — a known textual variant between editions; no lexical pairing forced from that line.
- **648 "awaiting" unified entries occur in the C01 Tibetan corpus** — each is now fillable by aligned reading of this single course. The parallel corpus is the template for every future course upload: request ASCII/ACIP editions wherever they exist.

### v0.4 changelog (2026-08-02) — the Great Dictionary scaffold
- **The Hopkins Tibetan-Sanskrit-English Dictionary ("the Great Dictionary") is now the master headword scaffold.** All 880 pages parsed span-level (16pt headword font as the entry anchor; two-column reading order reconstructed; internal verb-tense blocks and embedded examples handled). Yield: **21,751 unique headwords**, 16,874 with Hopkins English, 14,950 with Sanskrit, 4,817 with tense forms.
- **Unified dictionary: 25,898 entries** = Great Dictionary scaffold + 4,147 HGM-attested terms not found in Hopkins.
- **8,093 entries carry a locked HGM equivalent** (89 matched to curated, 3,857 to the Hypertext Glossary, plus the HGM-only appendix); **17,805 await one**, holding Tibetan, Wylie, Sanskrit, tenses, and the Hopkins reference gloss until an equivalent surfaces in HGM's translations. Per project rule, no English is invented for these — the field stays empty until attested.
- Master JSON now carries three views: `unified_entries` (the working dictionary), plus the original `curated_entries` and `glossary_entries` tiers preserved intact.
- Wylie normalization added (curly-apostrophe achung ‘ ’ → ', case, whitespace) so Hopkins variants match the HGM layers; all `wylie_variants` are match keys.

### Standard release package (every update, standing rule)
Every dictionary update ships as exactly five files, versioned together:
1. `hgm_dictionary_vNN.json` — machine-readable master (curated + glossary tiers); the translation tool's input.
2. `hgm_dictionary_vNN.tsv` — flat inspectable table, both tiers, with a `tier` column.
3. `hgm_reverse_index_vNN.json` — English → Wylie index across both tiers (v0.3: 14,569 keys).
4. `hgm_dictionary_vNN.md` — the human-readable HGM Dictionary: Part I, curated entries in full Illuminator × Hopkins hybrid style; Part II, the complete glossary layer in compact listing.
5. `README.md` — methodology, changelog, signature table, roadmap.

### v0.3 changelog (2026-08-02)
- **Full pass through Course I completed** (Readings Two–Ten; v0.1 had covered Reading One only). The Pabongka commentary yielded 48 new curated entries, 35 of them independently cross-confirmed against the Hypertext Glossary — frequently word-for-word.
- **The Reading Nine harvest** (correct view) locked HGM's full Prāsaṅgika register: *rang ngos nas grub pa* = "to exist from its own side," *bden grub* = "true/solid existence," the labeling-theory vocabulary ("basis to receive the name"), the four-school names, the five paths, "literal/figurative" for *nges don / drang don*, and the two-truths pair "deceptive truth / ultimate truth."
- **New register variants logged:** *'jig lta* "destructible view" (C01) ⇄ "perishable view" (corpus); *mdo sde pa* "Scripturalist" (C01) ⇄ "Sutrist" (corpus); *theg dman* "lesser way" ⇄ "lower way."
- Master file: `hgm_dictionary_v03.json` — 93 curated + 8,028 glossary entries.

### Expanded HGM signature table (v0.3 additions)
| Tibetan | Field standard | **HGM locked** |
|---|---|---|
| ཀུན་རྫོབ་བདེན་པ | conventional/relative truth | **deceptive truth / deceptive reality** |
| ངེས་དོན་ / དྲང་དོན་ | definitive / interpretable | **literal / figurative** |
| བྱེ་བྲག་སྨྲ་བ | Vaibhāṣika | **the Detailist school** |
| མདོ་སྡེ་པ | Sautrāntika | **the Scripturalist school / Sutrists** |
| སེམས་ཙམ་པ | Cittamātra / Mind-Only | **the Mind-Only school** |
| རང་རྒྱུད་པ | Svātantrika | **the Independent group** |
| ཐལ་འགྱུར་པ | Prāsaṅgika | **the Implication group** |
| ཐེག་ཆེན་ / ཐེག་དམན་ | Mahayana / Hinayana | **the greater way / the lesser way** |
| སྒོམ་ལམ་ | path of meditation | **the path of habituation** |
| ལམ་རིམ་ | stages of the path | **the Steps of the path (to Buddhahood)** |
| འཇིག་ལྟ་ | view of the transitory collection | **the destructible view / perishable view** |
| བཀའ་གདམས་པ | Kadampa | **Seers of the Word** |
| འཇིགས་བྱེད་ / བདེ་མཆོག་ | Vajrabhairava / Chakrasamvara | **the Frightener / Highest Bliss** |

### v0.2 changelog (2026-08-02)
- **Ingested the Hypertext Glossary (2016-05-16):** 8,028 ACIP→HGM pairings parsed, converted ACIP→EWTS Wylie→Tibetan Unicode (rule-based converter, validated; 76 entries flagged for exotic transliteration).
- **New files:** `hgm_dictionary_v02.json` (master: 45 curated + 8,028 glossary entries), `hgm_glossary_layer.tsv` (inspectable), `hgm_reverse_index.json` (English→Wylie, 14,500 keys — for the translation tool's candidate ranking).
- **Corpus-driven corrections.** Cross-validation of the 45 curated entries against the glossary produced 32 direct agreements and several honest revisions:
  - **bsod nams** — v0.1 claimed "goodness (never merit)." Wrong: the wider corpus shows *merit / good deeds / good energy* dominant, with *goodness* in the C01 prayer register. Now marked register-variant.
  - **rten 'brel** — "interdependence" primary; "dependent origination" also attested.
  - **de nyid** — "reality" in the wisdom formula; "thusness / this very" elsewhere.
  - **dben pa** — "isolation" (verse) alongside "deep solitude" (corpus).
- Lesson encoded into the method: verse-register evidence alone must not be generalized; every locked equivalent needs corpus-wide attestation before "verified" status.

### What this is
A Tibetan–English lexicon whose English equivalents are **strictly locked to the renderings used by Geshe Michael Roach (HGM)**, formatted as a hybrid of two models:

- **Illuminator style** (per the app screenshots): headword in Tibetan script → grammatical tag (`<noun>`, `<verb><phrase>`, etc.) → numbered senses → usage examples with translation → `q.v.` cross-references → record ID.
- **Hopkins structure** (per the uma-tibet.org dictionary): Tibetan ▫ Wylie ▪ Sanskrit ▪ glosses, with source-tagged comparanda.

The result: each entry carries Tibetan Unicode, Wylie, HGM-style phonetics (as printed in the ACI readings, e.g. *nge-jung*, *kyabdro semkye*), Sanskrit, POS tag, the **locked HGM gloss with verse-level citation**, an Illuminator-style definition, the Hopkins gloss *as reference only*, and cross-refs.

### The governing rule
> **The `hgm_gloss` field is authoritative. Hopkins and Illuminator inform structure and comparanda, never the equivalent itself.**

Where HGM's rendering diverges from the field standard, the divergence is the point — these are his signatures:

| Tibetan | Field standard | **HGM locked** |
|---|---|---|
| ལས་ *las* | karma | **deeds** |
| བསོད་ནམས་ *bsod nams* | merit | **goodness** |
| ཐར་པ་ *thar pa* | liberation | **freedom** |
| འཁོར་བ་ *'khor ba* | samsara / cyclic existence | **cyclic life; the cycle** |
| བྱང་ཆུབ་ཀྱི་སེམས་ | bodhichitta | **the wish for enlightenment; the Wish** |
| རྟེན་འབྲེལ་ *rten 'brel* | dependent arising | **interdependence** |
| ཐུབ་པ་ *thub pa* | the Muni / Subduer | **the Able** |
| ཞིང་ *zhing* | buddha-field / pure land | **paradise; the Pure World** |
| རྒྱལ་བ་ *rgyal ba* | Conqueror | **the Victors** |

### Technical note on the source PDFs
`C01Reading.pdf` (2002) uses legacy **TibetanMachine Type 1 fonts with custom encoding and no Unicode mapping** — the English and phonetics extract perfectly, but the Tibetan script layer is unrecoverable by text extraction. The Hopkins PDF has full Unicode Tibetan. Consequence for the pipeline:

1. English translations + phonetics are mined programmatically from the course readings.
2. Tibetan headwords are supplied in Unicode from the canonical root text and **verified visually against rasterized pages** (spot-checked; e.g. p. 5 mandala offering confirmed character-for-character).
3. Entries carry a `status` flag: `verified` (equivalent directly attested with citation), `provisional` (inferred, needs confirmation), `reference` (structural placeholder).

### File inventory
- `hgm_dictionary.json` — machine-readable master. This is the format the future translation tool consumes.
- `hgm_dictionary.md` — human-readable rendering in the hybrid entry style.
- `README.md` — this file.

### Roadmap
**Phase 1 (done, v0.1):** Schema locked; 45 verified entries seeded from Course I (root text of the *Three Principal Paths* + opening prayers).

**Phase 2:** Ingest remaining Course I readings (Pabongka commentary excerpts, Readings 2–10) — the commentary prose is a far richer source of one-to-one pairings than the verse. Then Courses II–XVIII as uploaded.

**Phase 3:** Collision handling. One Tibetan word can carry two locked equivalents by register (ཆོས་ = "Dharma" as Jewel, "objects/things" as phenomena; both བསོད་ནམས་ and དགེ་བ་ = "goodness"). The JSON schema already supports multiple glosses per entry; the translation tool will need a context-disambiguation layer.

**Phase 4:** Reverse index (English → Tibetan) and frequency data from the corpus for the translation tool's candidate ranking.

### What would accelerate the build
1. **Unicode Tibetan sources.** Any ACI/DCI materials in Unicode (later PDFs, .doc files, the ACI transcripts site exports) eliminate the visual-verification bottleneck entirely.
2. **HGM's own glossaries.** The vocabulary lists from the ACI homework/quiz materials and the glossaries in *The Principal Teachings of Buddhism* (Mahayana Sutra and Tantra Press) are pre-made one-to-one pairs — the highest-value input possible.
3. **The remaining 17 course readings.** Each adds several hundred attested pairings.
4. **A ruling on collisions** as they arise (e.g., when the same Tibetan term gets different English in different courses, which wins — earliest, latest, or most frequent?). I'd suggest logging them and deciding per-term.
