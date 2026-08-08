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

- [ ] **Send the two license-unblock emails** (drafted 2026-08-07,
      ready to personalize in docs/drafts/): EMAIL_84000_license.md
      (resolves the CC-BY-4.0 vs BY-NC-ND discrepancy — unlocks the
      best-licensed aligned Ti–En dataset in existence) and
      EMAIL_BDRC_ocr_models.md (model-weights license — unlocks OCR
      stage 2 and word-level scan following). Each is one reply away
      from a major integration.

- [~] **Collection title pairs → corpus/dictionary** (Adam 2026-08-06;
      docs/TITLES_INGEST_DESIGN.md) — CORPUS SIDE DONE 2026-08-07: the
      catalogs were already on disk in the Thursday meeting folder
      (ACIP Catalog.TXT 2,587 pairs · 00. KANGYUR CATALOG.xlsx 184 ·
      New Master Catalog 5 18 26.xlsx Sheet2 130). v0.29 corpus wave:
      +2,955 title segments (TITLK 349 / TITLT 359 / TITLS 2,091 /
      TITLR 156), corpus 37,807 → 40,762, 802 skips logged, full
      provenance (catalog_no + catalog_source); HANDOFF.md wave note;
      app spine rebuilt, 16/16. Library now shows catalog English
      titles (info panel + list-view column, 1,899 numbers joined).
      v0.30 WAVE 2026-08-07 (Adam: all title translations + subject
      headings): ༠༠ Sungbum Catalog.xlsx → +1,055 NEW pairs (1,837
      deduped vs v0.29), corpus 41,817; 2,416 texts → hierarchical
      Tibetan subjects (1,155 paths, 34 top-level) in
      sungbum_subjects.json; Library shows subject + English title;
      catalog_titles join now 2,352 numbers. Subjects are
      TIBETAN-ONLY in that source. v0.31 WAVE same day: the English
      pairing was in Adam's own Dropbox — the ACIP Release 6 web
      edition's subject browser names every node bilingually
      (Vowed_Morality--'DUL_BA): +367 unique subject-pair segments
      (course SUBJ), corpus 42,184; 1,424 works (incl. 322 Derge
      Tengyur) carry subject memberships shown in the Library. LATER:
      set-correspondence cross-check of R6 English subjects vs the
      Sungbum catalog's Tibetan-only hierarchies.
      REMAINING — **the tier ruling is Adam's**: are catalog English
      titles binding HGM English (→ dictionary-side attestation) or a
      labeled reference layer? Also: confirm the Thursday-folder
      catalogs are the current/authoritative versions.
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
      refs. Footnote-lemma recommendations from quoted spans SHIPPED
      2026-08-06 (official notes whose lemma appears in the matched
      segment's published English ride along with the match).
      Cross-segment merge SHIPPED 2026-08-06: consecutive lines of the
      same course matched back-to-back merge into one whole-span
      quotation; the battery seeds a pair whose boundary phrase exists
      in NO single segment, forcing the merge path.
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
      word-level after coordinate OCR (user option). Sungbum SHIPPED
      2026-08-06 via title search: BLMP Lucene template on
      purl.bdrc.io (verified live), query pre-filled from the file's
      own opening line via acipToEwts, candidates listed, USER confirms
      (never auto-asserted), link persisted in library/bdrc_links.json,
      then folio follow-along as normal (chosen edition's own folio
      labels).
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
      current PDF is missing BOTH the "ACIP Tibetan Input Code
      Standards" chart pages (153–160, empty placeholders) AND the
      entire printed appendix catalogs (verified 2026-08-07: the
      APPENDICES title page is followed only by two chart pages, then
      end of file). Equivalent ask: the Release IV CD-ROM's
      Acipdocs/Briefcat (WordPerfect) or Rtf abbreviated-catalog files
      (manual p. 83) — the ingested ACIP Catalog.TXT appears to be the
      same lineage (same field set), so this is confirmation +
      possible Sanskrit-title column, not new ground.
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
      front matter / translator notes. STATUS 2026-08-07: eleven
      sourced standards compiled (STD-001…011; the volumes' prose is
      mined dry under two pattern families); the pending-queue review
      sheet generator shipped (tools/build_pending_review.py →
      data/PENDING_REVIEW.md/.docx) so candidates can be brought to a
      GMR session.

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

- [ ] Review the 13 pyewts source-defect candidates
      (Handoff/data/qc_pyewts_defect_candidates_2026_08.json, filed
      2026-08-07 w/ HANDOFF QC note; standout: bsgrub bya stored with
      the Tibetan of bsgrub byed). Source check each before
      correcting.
- [ ] The pending attestation recount now also covers the three title/
      subject waves (v0.29 +2,955 · v0.30 +1,055 · v0.31 +367;
      corpus 42,184) and should honor the v0.29-addendum flags
      (85 truncated "…" titles, 7 "@" uncertainty marks).

- [ ] Bank the LC pair-selection script behind the historical 92.2%/4,582-pair
      sanskrit battery (for the record; the C++ port is already proven
      stronger — 100.000% identical to the Python oracle on all 79,452 LC
      terms).

## Integration menu — Tibetan tools survey (Adam: "scour… and put
## your findings into the todo list", 2026-08-07; full report w/
## licenses: docs/TIBETAN_TOOLS_SURVEY.md; internet-wide part 2 pending)

- [~] **Botok C++ port** — deterministic trie/maximal-match word
      segmentation (Apache-2.0), port-and-prove with its own test suite
      as battery; lexicon = ours + Monlam lists. The Overlay's missing
      segmentation backbone.
      — INCREMENT 1 SHIPPED 2026-08-07: the chunking layer
      (charcategories + BoString + ChunkFramework + Chunks/TokChunks +
      ChunkTokenizer) in allcore/botok (UTF-32 internal, every upstream
      quirk preserved and commented); char table + Apache-2.0 license
      banked at data/botok/. Battery (19th suite): expected values
      GENERATED from the live Python oracle over botok's own test
      inputs (upstream pytest run green first), plus corpus-scale
      diff-oracle — get_syls identical on all 105,546 distinct spine
      Tibetan strings (tools/build_botok_reference.py, regenerate per
      release).
      — INCREMENT 2 SHIPPED 2026-08-07: trie + maximal-match tokenizer
      (BasicTrie/Trie w/ affix inflection via SylComponents+BoSyl —
      SylComponents.json banked; has_skrt_syl char classes expanded to
      exact codepoint ranges via sre_parse; Tokenize w/ non-max
      backtracking and upstream's in-place pos-injection side effect
      preserved). Battery (20th suite, botok_tok_smoke): scenario ops
      replayed verbatim vs oracle (upstream test operations incl.
      deactivate/skrt/bare-form-data quirk/injection persistence) +
      corpus scale: trie from all 105,546 spine words, inputs = corpus
      segments via canonical ewts_unicode, token streams identical
      field-for-field (tools/build_botok_tok_reference.py; segments the
      canonical engine refuses to convert are counted and skipped,
      never approximated).
      — INCREMENT 3 v1 SHIPPED 2026-08-07: allcore::botok::Segmenter
      facade (lexicon-fed trie + tokenize; word = cleaned syllables
      reach a lexicon leaf — checked against the trie, since pos labels
      can't distinguish matched from backtracked) + Overlay Display
      toggle "Botok segmentation (reference)" (default off, persisted):
      click card shows the clicked CLAUSE segmented into words, lexicon
      = HGM dictionary headwords via the battery-proven conversion
      chain (lazy one-time build, timing shown), out-of-lexicon runs
      ⟨bracketed⟩, non-Tibetan grayed — labeled reference display only,
      spans stay lattice-bound. Facade checks in botok_tok_smoke.
      — REMAINING: Monlam lists as segmenter lexicon (needs a compact
      trie — current node layout would be ~GB-scale at 449k words ×
      affix inflection; deferred honestly), affixed-particle SPLIT
      display layer (modifytokens/splitaffixed), possible deeper
      Overlay integration with Adam's direction.
- [~] **Monlam word lists** — BANKED + FIRST INTEGRATION 2026-08-07
      (Apache-2.0 verified; 107,108 + 342,340 unique forms; allcore
      RefLexicon, 18th suite proves the real conversion path matches
      the lists' storage form exactly): Overlay click card shows
      "attested in the Monlam (Grand) Dictionary word list (reference)"
      for known spans AND for words the HGM dictionary doesn't have —
      real-word vs possible-typo signal, display only. REMAINING: use
      as the segmenter lexicon when the Botok port lands; word-level
      spellcheck pass in loadDoc (bulk, needs perf care). Definitions
      are NOT open — lists only.
- [x] **CC0 verbs database** — INTEGRATED 2026-08-07: 2,491 paradigm
      rows / 3,676 distinct stems banked (tools/import_verbs_db.py →
      verb_stems.tsv; misaligned cells flagged+skipped, ༼X༽ variants
      expanded); allcore VerbStems (17th suite: byas=past, byed=present
      via our own converters, bad cell proven absent); Overlay click
      card shows "Verbs DB (CC0 — reference only): past stem of
      pres|past|fut|imp [sources]" under the grammar toggle — display
      only, NEVER parse-binding (parse stays on dictionary tense
      evidence).
- [x] **pyewts diff-oracle** — RUN 2026-08-07 over all 105,634
      dictionary pairs (tools/pyewts_oracle.py; harness only, engines
      stay canonical). Verdict: 91.4% exact agreement; of the rest,
      7,398 are the master's ⟨wylie⟩ honesty flags (our engine refuses,
      pyewts converts blindly — even literal hyphens inside Tibetan
      output; our philosophy validated), 459 formatting conventions
      (comma/slash/ellipsis), 83 IAST-flavored wylie (we convert, they
      can't), 13 independently-confirmed source-defect candidates
      (data/extracted/pyewts_defect_candidates.json → FOR THE DATA
      PROJECT), leaving 1,141 letter-level divergences (1.08% —
      matching the battery's documented ~1.12% source-defect rate).
      Rerun per release as a permanent cross-check.
- [ ] **diff-match-patch (C++/Qt, Apache-2.0) + antx logic** → the
      input-center double-keying proofreading pane (roadmap item A).
- [ ] **Tibetan collation** — try ICU/CLDR (official rules since 2021)
      from Qt; fallback tibetan-sort-js/Apache Java collator. Needed for
      any sorted headword view.
- [ ] **eKangyur/eTengyur PD corpora** (Esukhia/OpenPecha) — citable
      diff-proofread canon beside the HGM corpus.
- [ ] **py-tiblegenc** (Apache-2.0) — legacy-font→Unicode rescue for
      input-center documents; external tool first, port maps later.
- [~] **lucene-bo affix rules** — ENGINE + LOOKUP SHIPPED 2026-08-07
      (allcore affixnorm: faithful TibAffixedFilter port, battery
      through our own converters incl. the needsAA guard; wylie-level
      projection defined BY the unicode port; Lookup pane: po'i finds
      po with a labeled "affixed particle stripped" note). FTS DONE
      2026-08-07 the clean way — not query expansion but an INDEX-side
      wylie_norm column in corpus_fts (built by the C++ authority via
      the wynorm CLI at spine-build time; 30,046/40,762 segments carry
      affixed forms; FTS matches all columns by default so every
      existing search became affix-tolerant with zero API change;
      measured: "bden pa" +87 segments, po +1,587; 22/22 suites on the
      rebuilt spine). Library index gained the same
      column 2026-08-07 (per-token ACIP→ewts→strip at index time, same
      authority; external-content FTS delete-sync updated for both
      columns; pre-norm indexes migrate by one-time rebuild; battery:
      BDEN PA'I line answers a bden pa search).
- [ ] **Steinert public dictionary CSVs** — Lookup-pane layers AFTER
      per-dictionary license review (code is GPL — data only).
- [ ] **bophono** (MIT) — optional non-GMR phonetics schemes toggle
      (GMR convention stays binding). Only if requested.
- [ ] **84000 TM: link-out only** — CC BY-NC-ND forbids ingestion
      without permission; concordance "view on 84000.co" links are safe.
      Consider writing 84000 for permission.
- [ ] **84000 glossary + translation memory** — CC BY 4.0 VERIFIED
      (glossary/TM/metadata; the translations are BY-NC-ND, different
      asset). Import as a provenance-tagged reference wave: glossary →
      Lookup layer, TM → concordance comparanda (never HGM tier).
      Biggest license-clean data win of the whole survey. ⚠ Resolve
      first: the TM GitHub README says BY-NC-ND 3.0 while 84000's
      terms-of-use says CC BY 4.0 — confirm from the files/84000
      before ingesting.
- [~] **Tibetan calendar engine** — YEAR LEVEL SHIPPED 2026-08-07
      (allcore tibcal, 60-cycle arithmetic pinned to five documented
      anchors incl. Tsongkapa's own 1357 fire-bird / 1419 earth-pig;
      Convert-pane colophon helper works both directions; pre-1027
      honest). Henning's MIT KCK sources banked in
      third_party/kck_henning. ORACLE HARNESS READY
      2026-08-07: Henning's original 2011 C compiles and RUNS on
      macOS/clang (third_party/kck_henning/build_macos.sh + DOS-header
      shims; sources untouched save void→int main); 76 reference
      fixtures captured (kck_fixtures.json: tshes/month/year → zla-dag,
      gza-dag, nyi-dag, daybreak, spyi zhag, Julian day, western date —
      every intermediate diffable; sweep even caught a duplicated lunar
      day, JD delta 0). PORT SHIPPED 2026-08-07
      (allcore tibcal_day: kckModernKarana(year, month, tshes) →
      zla-dag, gza-dag, nyi-dag, spyi zhag, Julian day; faithful
      transcription of T2/UTILS radix arithmetic + T4 driver order;
      23rd suite: ALL 76 fixtures match the original program's own
      outputs EXACTLY on every quantity; pre-epoch dates honestly
      invalid). Full-date UI SHIPPED
      2026-08-07 (Convert pane: tshes month year ↔ ISO western date;
      chad/lhag honesty; inverse + western round-trip proven 76/76 in
      the battery). REMAINING: adj_zla intercalary state machine for
      month-sequence display; the documented KCK-vs-published-Phugpa
      question.
- [ ] **rKTs abbreviations DB (6,000+ bskungs yig) + canonical IDs** —
      deterministic abbreviation expansion for input-center texts;
      verify the data dump's license first (unclear).
- [ ] **THL kmaps place/subject lookup** — user-initiated JSON lookup
      from the Overlay for place/person names (21k+ gazetteer; no-auth
      API; bundling license unclear so lookup-only).
- [ ] **Mahavyutpatti TEI (DILA)** — offline Skt⇄Tib table beside the
      Sanskrit converter.
- [~] **Bundle fonts** — Noto Serif Tibetan SHIPPED 2026-08-07 (OFL
      verified, variable font all weights, banked with OFL.txt in
      data/fonts/; registered at startup; the Overlay's Tibetan-script
      mode prefers it, ACIP/wylie stay in the default face, graceful
      if missing). REMAINING: BabelStone Tibetan (verify its exact
      license text first) for exotic Sanskrit stacks; visual pass of
      the Sanskrit-stack battery output in both faces.
- [ ] **Lotsawa House** — CC BY-NC: write them for bulk/permission;
      prayer-register corpus for register warnings.
- [ ] Link-out-only tier: Bibliotheca Polyglotta, Adarsha, WTS Munich,
      Dharmamitra (optional labeled AI aid at most), RY/Waldo/Valby
      (permission via Tsadra first).
- [ ] Red-flag list (no license / restrictive): see survey doc — never
      ship any of it without resolution.
- [ ] Survey's negative findings = our opportunities: no Tibetan
      verse-meter tool, no honorific-register data (beyond Monlam
      markings), no sum-cu-pa rule engines exist anywhere — our verse
      pane + Wilson tables lead the field; a zhe-sa honorific layer
      from Monlam data would be another first.

## Build queue

- [ ] **White paper v5** — v4 predates most of 2026-08-07: the three
      catalog/subject corpus waves (42,184 segments), the calendar
      engine (year + day level, 76/76 oracle-proven, Convert-pane
      dates both directions), the affix program (Lookup fallback +
      affix-tolerant search everywhere), Botok segmentation in the
      Overlay, five reference layers, bundled font, list view,
      pending-review sheet. Refresh both .md and .docx when the dust
      settles (coordinate: phone session authored v4).
- [~] **Release 6 zip — unexplored assets** (Adam's Dropbox,
      acip-release6-wylie.zip): (a) ACIP_Master_Catalog.html INSPECTED
      2026-08-07 — it is the Brief Catalog, record-identical to the
      ingested TXT (2,875; lineage proven, no Sanskrit column); (b)
      author trees HARVESTED — v0.32 wave: English×Tibetan author
      pairs by catalog-number join (≥2 works, ≥80% consistent) → 15
      pairs (course AUTH; 32 thin/conflicted excluded+logged), corpus
      42,199; (c) flat/ INSTALLED 2026-08-07: 2,723
      files → library/acip_release6/ (355MB; one true-corruption file
      quarantined as .corrupt, one benign CRC warning kept); library
      index rebuilt: 2,716 files, 8.68M lines, ~97s (headless
      libindex_cli shipped for scripted installs). The install
      surfaced and fixed three real defects: wylieToUnicode could
      THROW on exotic raw tokens (now flags ⟨…⟩, never throws —
      engine boundary contract); libindex norm ran ACIP→EWTS on
      already-wylie files (mangled case; now uppercase-detected —
      ACIP is defined uppercase); lines_fts lacked the apostrophe
      tokenchars the corpus uses (pa'i split as [pa,i]; schema v2
      aligns tokenizers, migrates old indexes once). Verified: 5,287
      lines match "bden pa" only via true affix normalization.
      CLOSED 2026-08-07: the by-title trees hold 359 titles — every
      one already paired via the Brief Catalog waves (0 new). The
      Release 6 archive is fully harvested.
- [x] **Composer pending-bibliography bank** — SHIPPED 2026-08-07:
      "Save as candidate (pending GMR approval)" in the compose
      dialog → data/candidate_bib.json; the review sheet
      (build_pending_review.py) now formats both footnote and
      bibliography candidates with decision checkboxes.
- [ ] **affixnorm: Old-Tibetan rules** (da-drag strip, medial འ) —
      present in lucene-bo's TibAffixedFilter behind convertOldTib,
      deliberately not ported 2026-08-07; port if Old-Tibetan
      material ever enters the library.
- [x] **Calendar month view** — SHIPPED 2026-08-07, fixtures-first:
      allcore kckYearMonths (intercalary enumeration + month names
      ported from T4.C incl. the male-year m11/12 exception and the
      tsurlug animal indexing; cycanim reconstructed from code +
      fixtures) + kckDayForTrueMonth (the only route to an intercalary
      month's days). Battery: 25/25 month headers and 721/721 days
      match the original's printed years kc_2010+kc_2012, with all 17
      doubled lhag days verified as exact JD-1 pairs. Convert pane:
      "2012 months" lists the year (ᵢ marks intercalary), "2012 m3i"
      tables an intercalary month's 30 days with chad/lhag notes.
      Fixture generators banked (gen_kck_year_fixtures.py; print-mode
      driving: menu 2, start/finish year on separate lines).

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
      BDRC-app → Import workflow; Stage 1 COMPLETE 2026-08-06 (Library
      "Send to OCR" hand-off; ocr_out live-watched by the tree; files
      there tagged ocr-derived in the info panel; opening one banners it
      and runs the syllable-legality first-pass QC with a fail count);
      Stage 2 = embedded ONNX pipeline + Scan pane (license check
      first). OCR output always tagged,
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
- [x] **List-view mode** — SHIPPED 2026-08-07: the Library's "List
      view" toggle shows a flat sortable catalog table (file, collection,
      number, verification, language, size) across all folders; numeric
      sort on catalog number, double-click opens, selection drives the
      info panel; ocr_out and my_materials labeled.

## Done highlights (details in README / memory)

- Spine, 5-pane GUI (Overlay · Analysis · Search · Convert · Lookup),
  all four canonical engines ported + battery-proven, Wilson particle layer
  (fused splits + suffix-agreement diagnostics), spellcheck, Gofer search
  (corpus + file trees), reference dictionaries, analysis pipeline + QC.
