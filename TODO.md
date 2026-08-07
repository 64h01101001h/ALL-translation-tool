# TODO — ALL Translation Tool

**Vision (Adam, 2026-08-06):** a total all-in-one translation tool — every
function and tool one may need in order to work with a Tibetan text.

**Scope directive (Adam, 2026-08-06):** the project stays DETERMINISTIC —
definitive understanding of every aspect of the Tibetan, from particles
upward, per Wilson's designations. No heuristic guessing (no invented POS
tags); units the rules/data cannot decide are labeled unresolved. AI features
remain optional and clearly labeled, never part of the deterministic core.
**Corollary (same day):** within that limit, surface as MUCH information to
the translator as the data genuinely supports — richness through layers and
display toggles, not through invention.

## Waiting on Adam (prompt him when the task starts)

- [ ] **Collection title pairs → corpus/dictionary** (Adam 2026-08-06;
      docs/TITLES_INGEST_DESIGN.md): isolate every Kangyur/Tengyur/Sungbum
      work with BOTH Tibetan and English titles and ingest the pairs.
      Best source = the ACIP Master Catalog spreadsheet — **Adam to upload
      it** (or install the collection ZIPs for inventory). One decision
      needed from Adam/data project first: are catalog English titles
      binding-tier (HGM's English) or a labeled reference layer? Corpus-
      side ingestion is safe either way (course TITLES, full provenance).
      Bonus after ingest: the Library shows English titles for installed
      files by catalog number.
- [ ] **Footnote & bibliography reuse program** (GMR directive via Adam,
      2026-08-06 — STD-008: released Mixed Nuts footnotes can and should
      be reused; never redo performed work):
      — DONE: notes bank v1 (344 endnotes extracted from the three
      volumes; Draft-pane search + cited insertion).
      — DONE 2026-08-06: bibliography bank (138 published entries, 118
      w/ ACIP catalog numbers); centralized MAIN documents
      (docs/apparatus/MAIN_FOOTNOTES.md + MAIN_BIBLIOGRAPHY.md, official
      tier = published + GMR-approved ONLY, regenerated per release via
      tools/build_main_apparatus.py); pending queue in the Draft pane
      (Propose footnote — candidates clearly PENDING, never mixed in).
      — REMAINING: rerun the three apparatus tools per new release;
      confirm STD-008 in GMR's own words for the record; dedupe/merge
      notes covering the same lemma across volumes.
- [x] **Quotation detection → auto-recommend citations + bibliography
      entries** (Adam, 2026-08-06) — SHIPPED 2026-08-06: allcore
      detectQuotations (exact corpus match ≥7 syllables, greedy-longest,
      attested-only; quotation_smoke proves detection/attribution/
      below-threshold silence/ACIP path); Draft pane "Detect quotations"
      lists matches w/ published English + one-click published
      bibliography entries when the course id matches the bank's ACIP
      refs. LATER: cross-segment-boundary extension; footnote-lemma
      recommendations from quoted spans.
- [x] **Scan follow-along v1** (Adam, 2026-08-06) — SHIPPED 2026-08-06:
      opening a recognized Derge/Lhasa Kangyur or Derge Tengyur text arms
      "Follow along in scans (bdr:MW…)" in the Overlay; BDRC's IIIF
      manifest labels its canvases with folio sides ("94a"/"94b") so the
      @folio↔image mapping is BDRC's own — NO calibration needed (better
      than designed); the woodblock page tracks the cursor's @folio
      marker, with prev/next, disk cache (library/scan_cache/), license
      line shown (public domain mark), browser-link fallback when
      offline. Line-band highlight SHIPPED same day (exact newline
      arithmetic on the file's preserved line structure; uniform band
      placement labeled approximate). LATER:
      word-level after coordinate OCR (user option); Sungbum once a
      BUDA mapping exists (SPARQL title search, user-confirmed).
- [~] **Bibliographies: collect + standardize + automate** (Adam
      2026-08-06): COLLECTED (138 published entries extracted → bank +
      MAIN_BIBLIOGRAPHY.md) · STANDARDIZED + AUTOMATED assembly
      2026-08-06 (allcore composeBibliographyEntry, exact B3 battery;
      Draft-pane compose dialog w/ live preview, STD-002 helper, BDRC
      link from ACIP number, pending-publication label). REMAINING:
      auto-fill from the Master Catalog title pairs when uploaded;
      other already-composed collections as Adam supplies them.

- [x] **Three new GMR translation volumes** — received readable copies
      2026-08-06 and staged with a provenance/ingestion README at
      `~/Desktop/HGM DICTIONARY - TRANSLATION APP/Input/GMR_volumes_2026-08/`
      (Spiritual Song ~42k w · Other Side ~40k w · 60 Verses ~158k w; all
      with parallel ACIP Tibetan).
      → INGESTED 2026-08-06 (Adam's direction, done in the data project's
      own structure): +2,206 aligned segments (SSL 427 · OSE 432 · SVN
      1,347), corpus 35,601 → 37,807 (`full_parallel_corpus_v28.json.gz`),
      HANDOFF.md v0.28 wave note added, app spine rebuilt, 14/14 suites.
      → **Remaining for a data-project session**: attestation recount +
      gloss miners over the new segments (deliberately not run — v0.27
      restraint), then the next full release package.
- [ ] An intact copy of the ACIP Release IV manual if available — the
      "ACIP Tibetan Input Code Standards" chart pages (153–160) are empty
      image placeholders in the current PDF.
- [ ] **Mixed Nuts style sheets / style guide** — Adam will get it from the
      team's editor and upload it here; then extend the translation-prep
      formatter and Draft workspace to assist/enforce its rules
      (docs/standards/MIXED_NUTS_PRELIMINARY_FORMATTING.md holds the
      workflow gleaned from GMR's demonstration video).
- [ ] **HGM translation standards — gather & compile** (Adam, 2026-08-06):
      collect translation standards from Geshe Michael himself and the
      Mixed Nuts translators (Gibson Chang, Nicholas Lashaw, AN, the
      editor); every entry sourced (who/when/medium), never composed.
      Receiving scaffold ready at docs/standards/
      HGM_TRANSLATION_STANDARDS.md with six categories each pre-mapped to
      its app integration hook (terminology checker, register warnings,
      Draft workspace, Sanskrit/pron engines, prep formatter, apparatus
      conventions). Integrate each standard into the app as acquired.
      First mining source available now: the three ingested volumes'
      front matter / translator notes.

- [ ] **Input-center conversion standards** — Adam will upload the standards
      for turning raw input documents (as received from the input centers)
      into full Tibetan Unicode texts. Unblocks the input-center formatter
      (roadmap item A). *He announced the upload 2026-08-06 — ask for it at
      next session start.*
- [ ] Input-center raw data samples (same item).
- [ ] "ACIP Standards for Simplified Tibetan and Sanskrit Pronunciation" doc —
      validates/replaces the provisional Sanskrit pronunciation style.
- [ ] Adam's reference Xcode projects for the overall tool vision.
- [x] Git init — done 2026-08-06 at Adam's "next most logical step":
      initial commit 686d148, 103 files; generated DBs/artifacts/user data
      ignored (all rebuild from releases). Optional next: a private GitHub
      remote for backup (needs Adam's say-so — pushing publishes).

## Waiting on the data project (Cowork sessions)

- [ ] Bank the LC pair-selection script behind the historical 92.2%/4,582-pair
      sanskrit battery (for the record; the C++ port is already proven
      stronger — 100.000% identical to the Python oracle on all 79,452 LC
      terms).

## Build queue

- [x] Battery H: the official pronunciation-chart pairs as a permanent C++
      battery — pure-Tibetan 50/51 asserted, Sanskrit-in-Tibetan informative.
      *Shipped 2026-08-06.* VOC dots also shipped (interjection closed list:
      kye / kyi ma / kyi hud / kye ma / e ma / e ma ho).
- [x] Print export: ACIP file → digital Tibetan Unicode (allcore/tibexport —
      @page markers, [bracket] Text-only stripping, shad print rules incl.
      the nga-tsheg exception, ⟨…⟩ failure flags; Overlay button + ACIP
      file-nomenclature provenance decoding). *Shipped 2026-08-06.*

- [ ] **Wilson full-designation program** (the "complete access on every
      grammatical level" goal). DONE so far (2026-08-06): per-unit parse w/
      contextual eight-case numbers; negation particles; infinitive-marker +
      auxiliary verb phrases; postposition constructions; ambiguity guards
      (shes/de licensing); the Science of the Dots (S · C n · NOM · SP · V ·
      VB per tsheg — reproduces Wilson's own diagrams; Trainer layer ⑥).
      REMAINING: OM contraction dots (needs an abbreviation table), VOC
      (interjection list), ADV (adverbial-identity detection), UP understood-
      particle inference, NA/NN/APP disambiguation (needs POS data — only
      with a real source, never heuristic).

- [x] **Translation Trainer v1** (docs/TRAINER_DESIGN.md): clause splitter,
      phrase chunker, verb spotter (HGM tenses + "to …" glosses),
      reading-order planner, Trainer pane with progressive reveal + corpus
      answer key. *Shipped 2026-08-06.*
- [x] Trainer deepening: na noun/verb disambiguation (63.7% conditional /
      36.3% locative over the corpus, remaining merges verified as fixed
      phrases), Tibetan-script rendering in the trainer, chunk click-through
      to full entries. *Shipped 2026-08-06.*
- [x] Wilson verb-class rule tables (8 classes from Appendix Four of Adam's
      copy of the book; class-aware chunk roles in the trainer; ~55 verbs).
      *Shipped 2026-08-06.*
- [x] Sanskrit authentic pronunciation: Devanagari→IAST input (100.000%
      round-trip over 73,437 LC renderings), classical IPA with anusvāra
      assimilation, FPMT letter guide — all in the Convert pane.
      *Shipped 2026-08-06.*
- [x] Grow the verb-class tables from chapters 17 & 19 (~70 verbs; corpus
      coverage 69.5% confident verb / 29.9% Wilson-classed over 147,902
      clauses). *Shipped 2026-08-06.*
- [ ] **Pedagogy roadmap** (docs/PEDAGOGY_ROADMAP.md — Adam's translator-
      training question, 2026-08-06). Build order: ① corpus drill generator
      (cloze, chunk-ordering, parallel reading — HGM answers only),
      ② local progress/SRS layer, ③ draft workspace + terminology
      consistency + concordance, ④ sa bcad outline extractor, ⑤ verse
      meter, ⑥ miss taxonomy, ⑦ AI back-translation QC.
      — ① SHIPPED 2026-08-06: Drills pane (chunk-order, cloze, particle,
      parallel reading; seedable DrillFactory in allcore; drills_smoke).
      — ② SHIPPED 2026-08-06: local progress/SRS layer (allcore::Progress →
      progress.db; SM-2-lite; self-building deck from Overlay clicks;
      Vocabulary-review drill mode; drill/reading logging + stats line;
      peeked-segment resurfacing + coverage() ready for difficulty scoring).
      — ③ SHIPPED 2026-08-06: Draft pane (clause anchors + concordance-first
      term view + terminology checker in allcore/terminology — maximal
      glossed spans, gloss-alternative matching, mixed-rendering +
      shared-English + provisional flags; terminology_smoke).
      — ④ SHIPPED 2026-08-06: sa bcad extractor (allcore/outline —
      announcement + ordinal matchers incl. compound teens, stack tree with
      irregular flags; validated on GIE.txt: 160-node coherent tree, lexical
      ordinals correctly rejected; outline_smoke; Draft-pane button).
      — ⑤ SHIPPED 2026-08-06: verse meter (allcore/verse — syllabic lines,
      dominant-meter detection ≥70%, irregular-line flags; Draft-pane
      button; verse_smoke).
      — ⑥ SHIPPED 2026-08-06: miss taxonomy (order misses classified by the
      misplaced chunk's role: verb-position / genitive-attach / agent-chunk /
      ladon-chunk; particle misses by family; cloze misses by blanked role;
      Progress::topMisses → "weak spots" line in Drills stats).
      — ⑦ SHIPPED 2026-08-06: AI back-translation QC (buildBackTranslation-
      Prompt in allcore/analysis — diff-only instructions: omissions /
      additions / anchor mismatches / verdict, never retranslates; Draft-pane
      "AI back-check" button, streamed, output banner-labeled AI; disabled
      without ANTHROPIC_API_KEY).
      **The pedagogy roadmap build order ①–⑦ is complete.**
- [ ] Input-center formatter (blocked on the two uploads above).
- [ ] Live composing editor idea (type Wylie/ACIP, edit rendered Tibetan) —
      possible fit for the formatter workflow; noted from the Jskad review.

- [x] **Library pane v1** (Adam's request 2026-08-06): install collection
      ZIPs from asianlegacylibrary.org/library, import personal materials
      (.docx→.txt auto-conversion), sortable tree browser with catalog
      provenance + preview + open-in-Overlay. *Shipped 2026-08-06.*
- [ ] **Tibetan OCR for pecha scans** (docs/OCR_DESIGN.md; Adam 2026-08-06,
      "eventually"): base = BDRC's open-source tibetan-ocr-app models
      (5 script styles incl. Woodblock). Stage 0 = document the manual
      BDRC-app → Import workflow; Stage 1 = Library "Send to OCR" hand-off +
      watched ocr_out folder w/ ocr-derived tags; Stage 2 = embedded ONNX
      pipeline + Scan pane (license check first). OCR output always tagged,
      review-flagged, never corpus-bound without verification.
- [x] Library browsing deepening (first tranche, 2026-08-06, commit
      a876cb8): name search with clickable results, recently-opened list,
      "search the Library" button in the Search pane, OCR Stage 1 hand-off
      ("Send to OCR…" + library/ocr_out/ convention).
- [x] Prebuilt library full-text index (2026-08-06): allcore::LibraryIndex
      (SQLite FTS5, line-per-row, incremental by mtime/size, purge on
      delete), Library "Update search index" button, Search pane auto-uses
      the index when targeting the library; .act/.inc/.ace now searchable
      in both indexed and live modes; libindex_smoke 12 checks.
- [x] Library filters (2026-08-06): decoder-driven browse by collection /
      verification status / language, combinable with the name search.
- [x] **BDRC scan auto-linking, stage 1** (Adam 2026-08-06; docs/
      BDRC_LINKING_DESIGN.md): live-probed BUDA — Derge/Lhasa outline IDs
      encode Tohoku numbers directly (bdr:MW23703_4210 = Pramanavarttika
      verified); deterministic bdrcScanUrl (KD→MW22084, KL→MW26071,
      TD→MW23703) + "view the original scans on BDRC" link in the Library
      info panel. *Shipped 2026-08-06.*
- [ ] BDRC linking, remaining: templated-query/SPARQL fallback for
      editions without ID-encoded Tohoku (KX, TS), title-based candidate
      search for Sungbum works (fuzzy — user-confirmed, never auto-
      asserted), in-app IIIF scan viewer beside the e-text, and the OCR
      pipeline consuming the same IIIF images. NOTE (Adam 2026-08-06):
      BDRC is Wylie/EWTS (bo-x-ewts); our names are ACIP — every
      name-based BDRC query converts through acipToEwts first (exact,
      battery-proven); never send raw ACIP to a BDRC endpoint.
- [ ] Library deepening, remaining: list-view mode (low priority).

## Done highlights (details in README / memory)

- Spine, 5-pane GUI (Overlay · Analysis · Search · Convert · Lookup),
  all four canonical engines ported + battery-proven, Wilson particle layer
  (fused splits + suffix-agreement diagnostics), spellcheck, Gofer search
  (corpus + file trees), reference dictionaries, analysis pipeline + QC.
