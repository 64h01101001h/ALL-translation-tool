# TODO — Diamond Cutter Translation Tool

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

## SCOPE DECISION (Adam, 2026-08-08): cataloging is SIDELINED

A separate in-house cataloging tool/app will be created as its own
project in the future. All cataloging-tool discussion and
implementation is sidelined HERE from this date. What this means:

- SIDELINED (banked knowledge kept for the future project):
  catalog-toolchain follow-ups (ACIPMaintenance deep dives, further
  MakeGMRCatalog/MakeDergeCatalog mining), the ACIP MySQL dump hunt
  (aciptbl ~110 cols), acip-support's org.asianclassics.center.catalog
  module, any catalog-editing/catalog-building features in this app.
  docs/CATALOG_TOOLCHAIN_SURVEY.md and docs/ACE_RECOVERY.md remain the
  handoff dossiers for that future project.
- STAYS IN SCOPE here (serves translators, not cataloging): the
  Library pane's catalog DISPLAY (decoder, titles, subjects,
  verification levels), the Input pane build (#30 — input centers,
  not cataloging), OCR, registers, and everything else.

## Waiting on Adam (prompt him when the task starts)

- [x] **Pecha renderer v4 (W3C/r12a traditional justification)** — COMPLETE 2026-08-14: N1 tsheg-padding + N3 ༑ substitution + N5 ༈ NBSP block (core press #83), then N2 double-shad-GAP justification (closing shad drawn flush at the margin; pechaDoubleShadEnd pure helper, selftest-pinned) + N4 wrapped-shad leading-space discard. Letter-spacing fallback DROPPED with reason: lines break at tsheg/shad boundaries by construction so the case barely occurs, and stretching inside syllables is untraditional — tsheg-padding and shad-gap are the two attested methods, both shipped. (was:
      N1 tsheg-padding justification (dominant method; line-ends-in-
      tsheg → pad tshegs to margin), N2 double-shad gap justification
      (+1/4–1/3-space normal gap), N3 ༑ substitution on reflow (first
      shad only), N4/N6 wrapped-shad space discard + NBSP after
      swallowed shad, N5 ༈ three-NBSP unbreakable block. Source:
      docs/research/W3C_TIBETAN_LAYOUT_FULL.md.

- [ ] **MASTER STATUS BOARD (five-lane total audit, 2026-08-14)** —
      Adam's directive: "total audit… nothing falls through the
      cracks." Five parallel lanes (all docs · TODO verification ·
      memory-log promises · Adam's request ledger · code markers)
      reconciled here. THIS section is the single de-duplicated
      queue; body items below keep their history and detail — where
      an item appears in both, the body entry is the record and this
      is the index. Canonical facts fixed by the audit: prenasal
      queue = 225 pending in the store (164/205/220 were growth
      snapshots); Analysis item 14 and the Trainer particle rows are
      DONE (stale board claims corrected); code tree is clean (2 real
      markers, both people-gated).

      **A. SOLO BUILD QUEUE (ranked)**
      0. REVERTED — Overlay phrase-walk / nest-ladder change
         (2026-08-14, commit 058efd2, reverted same day at Adam's
         call: "working worse than before"). The DIAGNOSIS stands
         (probe-proven): a bare connective (GYI class) between two
         top-level phrases is covered only by itself + the whole
         line; old walk re-lights the whole line and up/down
         ring-cycles — the middle-syllable stall. The FIX attempt
         changed too much at once (walk target + ladder semantics).
         Revisit LATER (Adam's word) as a joint session with him
         driving the exact reproduction; keep the lattice probe
         (scratchpad/lattice_probe.cpp banked in the memory log) as
         the test harness; consider fixing ONLY the walk target
         first, leaving ↑/↓ untouched, then iterating live.
      1. Pecha programme remainder — RECONCILED 2026-08-20: the v4
         half of this line was STALE — N1-N5 all COMPLETE 2026-08-14
         (see the [x] v4 entry above; letter-spacing fallback DROPPED
         with reason there). Shipped conventions verified in code:
         ༄༅། ། opens every recto (shadConv), ༎ stanza close, yig-mgo
         head marks, classical 5-line opening, ya-yig margin title,
         volume letter, title folio. GENUINELY REMAINING, all gated:
         Sungbum-format syllable budget (needs the OCR-stats lane);
         margin presets + dbu-khyud fine conventions + print QA
         (correct values COME from Adam's reference prints — joint
         half; building presets before the prints would be guessing).
      2. Banked-corpus mining — Hackett verb classes FIRST HARVEST
         BANKED 2026-08-14: data/hackett/verb_classes.tsv, 344 rows
         (91 CC0-corroborated · 11 cc0-disagrees flagged for review
         · 7 ⟨?⟩); coverage pp. 58–71/86–105/114–127/142–179;
         REMAINING: crop re-pass for pp. 31–57, 72–85, 106–113,
         128–141 (scan resists current zoom — agents refused rather
         than guess); Goldstein 254 "(h.)" markings (vision pass);
         then extend spotVerb with the 31 Wilson-class causative
         pairs + the new class rows once reviewed.
      3. ~~THL Degé deep-link wiring~~ AUDIT CORRECTION 2026-08-14:
         already shipped with task #25 (setScanTarget routes through
         the verified 919-entry concordance, collisions dropped).
         Lhasa half CLOSED same day: kl_lhasa_concordance.json
         (258 title-matched, already built) wired into setScanTarget
         — KL files now carry "THL Lhasa catalog: H.N" links
         (cat=h token live-verified; selftest-pinned).
      4. ~~Gofer fold + Stop~~ DONE 2026-08-14: the fold combo now
         acts — strict modes post-filter hit windows (case /
         space-normalized case; pure goferFoldKeep, selftest-pinned
         — the pin caught a normalization bug before ship), hidden
         windows counted in the footer; Stop is real (chunked loop,
         partial results labeled).
      5. ~~People layer increments~~ DONE 2026-08-14: Lookup PERSON
         card (author-name query → dates + BDRC/Treasury-of-Lives
         links, same authority as the Library panel; pinned) +
         Overlay ✍-author badge with per-text BDRC link beside the
         THL catalog line. Remaining people work = the 81 unresolved
         authors → BDRC letter (send is Adam's).
      6. OCR: ~~model manager~~ DONE 2026-08-14 (Scan pane → "OCR
         models…": 4 downloadable BDRC models verified live on HF,
         license banner, progress downloads, config saved under the
         recognizer's expected name, picker with bundled-Woodblock
         fallback — resolver selftest-pinned; parser accepts BDRC
         string-form charsets, ocr_smoke-proven). ASYNC RECOGNITION
         DONE 2026-08-20: Run OCR runs on a worker thread (std::thread
         + queued invokeMethod posts; worker owns all data until the
         finish call — no cross-thread mutation), button becomes Stop,
         partial results labeled "stopped after N lines"; pane dtor
         joins the worker. REMAINING: model refresh when BDRC's
         ~end-Aug release lands (external).
      7. ~~True-Phugpa calendar port~~ CORE SHIPPED 2026-08-14:
         allcore tibcal_phugpa (Generalised Phugpa epoch of TCG,
         ported from the files) + suite 43 phugpa_smoke — 21 oracle
         years diff-proven (260 months incl. intercalary/delayed
         labels, 7,971 day lines, 171 duplicated pairs, 293 omitted
         days, 0 failures). Convert-pane wiring DONE same
         day: ISO dates answer in BOTH systems; "YYYY months" shows
         both month sequences (ᵢ intercalary, ᵈ delayed); and Adam's
         2026-08-15 ask shipped — Tibetan year name + month
         ("me bya 6 m3") → the month's western date span
         (probe: me bya rabjung 6 = 1357, m3 = 30 Mar–28 Apr 1357).
         Item COMPLETE.
      8. Draft-pane standards conventions: STD-006 translator-
         insertion marking · STD-009 variant-reading notes (build
         follows GMR ratification of the trust dialog).
      9. Small strikes: ~~Input partner auto-pairing~~ DONE
         2026-08-15 (suggestPartner: the shared page scan's folder +
         stem; single match preselects the compare dialog, user
         always confirms — the input centers' real naming convention
         still awaited) · ~~UTFC lane hookup~~ AUDIT-STALE, was already DONE 2026-08-15 (g_openTextInInput + inputrescue: anchor — rescued text flows into Input; verified in code 2026-08-20) · ~~Spotlight async~~
         DONE 2026-08-15 (concurrent mdfind fan-out, one shared 6s
         deadline, Stop honored) · ~~QTextCursor selftest warning~~
         ROOT-CAUSED + FIXED 2026-08-15 (lldb backtrace:
         AlignPane::repaint_ re-enters during clear() with stale
         offsets; spans now clamped to the live documents) ·
         ~~empty-input loadDoc guidance~~ DONE 2026-08-15 (teaches
         instead of blanking) · BONUS FIX: harness runs
         (--selftest/--sweep/--screenshots) no longer read or
         clobber the translator's real saved session (g_harnessRun
         guard; the session pin lifts it locally) · ~~apparatus expansion~~ DONE
         2026-08-15 — the real find was not the 16 book .txts but the
         OLDER compiled footnote collections (2018-05 + 2019-02
         docx, never ingested): tools/extract_older_apparatus.py
         (title-run heuristic, verse-marker rejection, roman-marker
         glue-splitting, dedupe vs main bank) → 1,569 notes from 5
         published books + one honestly-untitled 2018 opening
         section; bank 344 → 1,913; Draft pane loads both banks.
         The 16 Handoff/book_texts .txts remain a possible later
         per-book pass; the older BIBLIOGRAPHY compilations too · ~~KX/TS scan lane~~ CLOSED BY MEASUREMENT 2026-08-15 (library holds 1 KX file, 0 TS — no lane worth building; the one file gets the title-search fallback; revisit only if those collections ever install) ·
         ~~outline/verse wylie-parity probe~~ DONE 2026-08-15: probe FOUND a real defect — the sa-bcad outline matcher was uppercase-ACIP-only, so wylie files (Lhasa _inc, Release-6 wylie) got NO outline; fixed with a case-folded matching shadow (headings keep original script); verse already had parity; outline_smoke parity-pinned · ~~Botok-vs-lattice overlap audit~~ DONE 2026-08-15 (docs/research/BOTOK_LATTICE_OVERLAP.md: 400 corpus lines, 80% boundary agreement both directions, 25.5% identical partitions — the gap is largely phrase-spans-vs-words BY DESIGN; no pathology; reference-only ruling stands) · ~~citation side-pane streamline~~ DONE 2026-08-15 ("Pin to side panel" on the located report; cite anchors factored into handleCiteAnchor shared by dialog + side panel) · ~~sweep-harness line-edit coverage~~ DONE 2026-08-15 (every enabled QLineEdit gets typed probes — Tibetan string, wylie, junk edge, empty — with returnPressed fired; proven live on Lookup/Convert/Sanskrit/Search, all coherent). SMALL-STRIKES LIST COMPLETE ·
         ~~navigable-outline TOC~~ AUDIT-STALE, CLOSED 2026-08-15: the sa-bcad outline tree already ships (Overlay → Outline (sa bcad)…: QTreeWidget, click-jump, scan follow-along keeps pace). The banked THL outline-MARKUP grammar remains a parser-in-waiting for whenever THL-marked-up files enter the library — none do today · Sungbum route-2 (per-author outline harvest)
         verification · ~~THL follow-ups~~ BOTH DONE 2026-08-15 (GMR_VS_THL_PHONETICS.md engine table; studies_in_genre_chapters.tsv — 30 chapters recovered from the legacy reader's server-rendered contents) · ~~Zenodo Hill verb-stems license check~~ DONE 2026-08-15: record 4726991 = open ACCESS but license 'other-open' with NO explicit grant (no CC tag, no LICENSE file; derives from Hill 2010, Bayerische Akademie — a published book). VERDICT: in-house research comparandum at most; nothing ships without Hill's word (an email would clear it — optional, our CC0 bank already covers stems; Hill adds grammatical-tradition attestations) · About-dialog
         contact swap when a support@ address exists · vendor
         libtesseract if the team wants zero-setup Sanskrit OCR.
      9b. **ACIP anchor for every apparatus footnote (Adam,
         2026-08-15).** His words: "we can see the footnotes output
         but we don't know what Tibetan phrase/term they are
         associated with in the Translate > Apparatus pane." He is
         right and the gap is in the DATA, not the pane: all 2,234
         mined notes carry `lemma` = the ENGLISH catchword from the
         published translation, and no Tibetan field exists in the
         ApparatusNote struct at all (lemma, text, source, num).
         MEASURED FEASIBILITY 2026-08-15, before scheduling:
           · 2,010 of 2,234 lemmas are 1-6 words (matchable length)
           · 1,289 of those 2,010 (64%) appear verbatim inside an
             aligned English segment of full_parallel_corpus_v32
           · but naive first-match is WRONG often enough to be
             dangerous: 'Je Tsongkapa' resolved to a segment whose
             ACIP is "THE ASIAN CLASSICS INSTITUTE", because
             containment returns the WHOLE SEGMENT, not the phrase,
             and does not check that the segment even belongs to the
             note's source work. 'Namo Guru Manjughoshaya' resolved
             correctly. So the raw 64% is an upper bound on recall
             and says nothing about precision.
         WHAT THE REAL BUILD NEEDS: (1) constrain candidates to the
         note's own source work via the corpus `course` field before
         matching at all; (2) isolate the PHRASE inside the segment
         (word-level alignment or the overlay lattice), never hand
         back the whole segment; (3) a second, independent route for
         the 318 notes whose BODY already quotes the Tibetan inline
         ("the Tibetan word bum...") — those are self-anchoring and
         should be mined directly, they are the cheapest true wins;
         (4) hgm_reverse_index for single-term lemmas.
         RULE 3 APPLIES HARD: every anchor is MACHINE-LOCATED
         evidence, displayed as a labeled provisional candidate with
         its corpus segment shown as proof, never as a silent fact
         beside HGM's own footnote. Unresolved lemmas stay visibly
         unresolved rather than being approximated.
         SHIP ORDER: schema field first (so the pane can show what
         exists), then route 3, then routes 1+2, then the pane's
         display + a battery pinning known-good and known-bad pairs.
         PROGRESS: schema + route 3 SHIPPED 2026-08-18 (commit
         946e8a3: 117 body-quoted notes, evidence sentences carried,
         "TIBETAN NAMED IN THIS NOTE" block + rendered-detail pin).
         ROUTE 4 SHIPPED 2026-08-20: tools/build_apparatus_anchors_r4
         .py (banked — route 3's generator never was; lesson noted)
         resolves lemmas through hgm_reverse_index with the
         gloss-equality gate (a lemma anchors only when it is EXACTLY
         one of an entry's own hgm_glosses — rule 1 by construction);
         tier discipline: curated/glossary preferred, auto-aligned
         only alone, capped 3, labeled PROVISIONAL. +134 notes → 252
         of 2,234 anchored; pane label covers both methods; selftest
         floor 240 + route-4 evidence render pinned.
         ROUTE 1 SHIPPED 2026-08-20 (same day): the source-work→
         course mapping EXISTS in the corpus itself — SVN/SSL/OSE are
         whole published books as courses. tools/
         build_apparatus_anchors_r1.py: lemma verbatim in the
         published English of the note's OWN course → labeled PASSAGE
         candidates ("WHERE HIS PUBLISHED ENGLISH USES THIS LEMMA…
         not a term equivalence"), capped 3; the course constraint
         kills the wrong-work class the 2026-08-15 measurement warned
         about, and the passage label satisfies the never-hand-back-
         the-whole-segment rule. +189 notes → 444/2,234 anchored.
         Selftest floor 420 + passage-render pin (pin lesson: the
         raw-wylie sample check needs a TERM anchor). REMAINING:
         route 2 phrase isolation — MEASURED AND PARKED 2026-08-20:
         on the 19 ground-truth notes (r4 term + own-course passage),
         the term's wylie occurs in the matched passage only 11/19
         (58%) even affix-tolerant; the misses are epithets/
         abbreviations ("Arya Nagarjuna" where the Tibetan says klu
         sgrub alone) — i.e. the citation form ISN'T in the passage
         half the time, so lookup-based isolation would attach wrong
         or missing halves. Real isolation needs word-level
         ALIGNMENT — genuine research, parked with this measurement
         as its entry point. The two compilation sources (1,890
         notes) have no single course and stay honestly unmapped.
      9d. **GMR RULING — resolve Tibetan number through the
         Sanskrit (Adam relaying Geshe Michael, 2026-08-15).** His
         method: *when you are stuck on whether a Tibetan term is
         singular or plural, go to the Sanskrit if it is available,
         because Sanskrit REQUIRES number to be designated
         linguistically.* Tibetan marks number only optionally
         (rnams / dag / tso, and constantly omitted), so the
         information is genuinely absent from the Tibetan surface —
         it is not there to be recovered. Sanskrit inflection carries
         it obligatorily. NOTE THE THIRD VALUE: Sanskrit distinguishes
         singular / DUAL / plural, and the dual is the most valuable
         of the three here — it settles the pair-sets translators
         argue about (the two accumulations, the two truths) in a way
         no Tibetan reading can.
         WHAT WE HOLD, MEASURED 2026-08-15: 92,071 of 105,634 spine
         entries (87.2%) carry a Sanskrit form, plus 15,705 rows in
         84000's Skt file. BUT EVERY ONE IS A CITATION FORM — kāya,
         sarva, paryavasthāna — i.e. the lexicon lemma. Number in
         Sanskrit lives in the INFLECTIONAL ENDING, so a citation form
         says nothing about number in a given passage. The method
         needs the Sanskrit OF THAT PASSAGE, which we do not have.
         Do NOT build a number oracle on the lemma table; it would
         manufacture confident answers out of data that cannot carry
         them (rule 3).
         BUILDABLE NOW, in order:
         (1) Teach the method where it is needed — a number-ambiguity
             flag in Draft/Review when a Tibetan noun phrase carries
             no number marker AND the entry has a Sanskrit lemma,
             surfacing GMR's rule plus the lemma and a link out. It
             tells the translator WHERE to look; it never answers.
         (2) Bank the ruling in the in-app guidance layer (Help ->
             Working with Tibetan) as his named method, since it
             generalizes far past any one passage.
         (3) THE REAL UNLOCK — acquire aligned Sanskrit witnesses
             where they survive (84000 carries Sanskrit for some
             texts; GRETIL is the obvious candidate corpus, license
             to be checked BEFORE any ingest). With a passage-level
             Sanskrit alignment the ending can be parsed and the
             number read off honestly, tagged as evidence with the
             witness shown.
         (4) Only after (3): a number column on the card, always
             sourced to the witness, never asserted from the lemma.
         ASK ADAM: whether Geshe Michael gave this as a general
         principle or in a specific context (it came from the Diamond
         Way Translator Course, 2026-08-15 — the same session as the
         semicolon/spacing instructions already queued), and whether
         Ven. Utpala's style sheet already records it.
      9b2. ~~Zoom for the Overlay text pane AND the card info pane~~ DONE 2026-08-16 (A-/A+ both rows, cmd+/-/0 focus-routed, persisted, pinned) — original scoping kept below for the record: **Zoom for the Overlay text pane AND the card info pane
         (Adam, 2026-08-16).** Reader-controlled text size on the
         loaded document view and on the card/context browser.
         SCOPED 2026-08-16: the two halves are NOT the same job.
         · Text pane (view_, QPlainTextEdit): trivial — zoomIn/
           zoomOut exist on the widget; wire ⌘+/⌘−/⌘0 (the Input
           pane's zoom shortcuts are the in-app precedent), add a
           small control, persist via sess:: so the size survives
           relaunch.
         · Card pane (context_, QTextBrowser): NOT trivial — the
           entryHtml() renderer hard-codes px font sizes throughout
           (font-size:12px etc.), which ignore QTextEdit zoom. Needs
           a card-zoom factor threaded through entryHtml's px values
           at render time (single shared renderer, so one change
           covers Lookup/Overlay/popup), or a rem-style refactor of
           the card CSS. Same treatment then benefits ⌘D popup and
           Lookup pane free.
         Persist both sizes per-pane in sess::, label the control
         plainly (A− / A+), respect the existing typography audit
         item when it runs.
      9e. **From the DCC cataloging-agent demo (Lon & Tom,
         2026-08-18, analysis: docs/research/DCC_CATALOG_AGENT_DEMO.md).**
         Adoptable in our architecture, our rules: (1) title-vs-
         translation mismatch QC over the library's bilingual
         filenames (deterministic; their sweep found 6, ours will
         find some); (2) doctrinal-topic + life-application tag
         layers — machine-assigned, so PROVISIONAL-labeled through
         the proposal channel, never silent; (3) catalog-title gloss
         tier from GMR's own published title pairs (matched English,
         labeled reference); (4) colophon-aware same-title/duplicate
         detection; (5) GMR's 23-item Russian physical-catalog
         format incl. printed-area-as-woodblock-fingerprint — ADAM:
         request the list (GMR is sending it to Lon; it serves our
         scan/edition matching directly); (6) conversational
         librarian over OUR spine — gated on API credits, answers
         restricted to attested material. ON THE RECORD: GMR called
         the longest-match walk "a trick I taught Adam… it's going
         to revolutionize the dictionary" — that is our lattice's
         exact design, validated by the authority himself.
      9f. **Windows .exe packaging (Adam, 2026-08-18: "considering
         how hard it will be").** AUDITED SAME DAY — the stack was
         chosen for this (CLAUDE.md: C++20 + Qt 6 + CMake "macOS-
         first, cross-platform-clean"), and the audit bears it out:
         core/ and app/ have ZERO Q_OS_MAC / __APPLE__ / objc code;
         CMakeLists has no platform branches; SQLite is vendored;
         all URL/file opens go through QDesktopServices (portable);
         QSettings maps to the registry by itself. Only THREE real
         mac-isms, all small: (1) Files pane "open Terminal here"
         calls /usr/bin/open -a Terminal → Windows: start cmd /K;
         (2) "Send to OCR" scans /Applications for the TibOCR app →
         gate behind Q_OS_MACOS, hide the button elsewhere; (3) the
         data-root discovery mentions .app-relative layout → add the
         exe-relative equivalent. WORK LIST: Qt 6 (MSVC) + CMake
         build, fix the three call sites, windeployqt instead of
         macdeployqt in a package_windows.(bat|ps1) mirroring
         tools/package_macos.sh (stage → deploy → data manifest →
         launch test → installer), NSIS or Inno Setup for the
         installer (~380MB payload, same as the DMG), FONTS: bundled
         OFL faces carry over but Kailasa/Kokonor are Apple-only —
         the vetted list already degrades gracefully. TESTING: the
         batteries are ctest (portable) and --selftest/--sweep run
         headless-ish — GitHub Actions windows-latest runner can
         build+test without owning a Windows box; a real machine
         only needed for the eyeball pass. SIGNING: unsigned .exe
         hits SmartScreen warnings; an Authenticode cert (~$100-400/
         yr, OV) is the Windows sibling of the Apple Developer ID
         already on Adam's list — same sitting, decide both.
         ESTIMATE: days, not weeks, absent surprises — the surprise
         reserve is HarfBuzz/Tibetan shaping on Windows, which Qt
         bundles, so verify shaping FIRST on the CI runner
         (render bsgrubs-class stacks, compare screenshots) before
         any packaging work.
      9g. **CATALOGING WORKFLOW (Adam, 2026-08-19: "this is about
         to be a huge update... bring files from the inhouse
         uncataloged data into our official database/catalog").**
         This ACTIVATES what WP §8 had set aside as "its own future
         project". v1 SHIPPED same day: a Catalog tab group with the
         intake pane — folder census through the trusted ACIP
         filename decoder (identifiable vs uncataloged, capped
         5,000 files honest, per-file identity + preview + open in
         Overlay, intake folder session-remembered, pinned).
         Charter stated on the pane: machine LOCATES and SUGGESTS,
         humans approve, the official catalog changes only through
         data releases. ROADMAP (build in order, each its own
         press):
         (1) DONE 2026-08-19 — identity PROPOSAL lane for
             uncataloged files. allcore/catalog_id.h: two title-page
             rules taken from the collections themselves (the
             canonical "BOD SKAD DU," bilingual head incl. the
             Chinese-source RGYA'I SKAD DU variant; the Sungbum
             block closing in BZHUGS SO), matched by shared-syllable
             overlap against ~8,700 known titles (library filenames
             + catalog_works.json). Every candidate carries its
             evidence: extracted title, rule used, Sanskrit side of
             the head, shared-syllable counts, matching basis,
             source bank. A file that begins mid-text gets NO guess.
             MEASURED by catalog_id_smoke (suite 44) against the
             library itself, re-measured every ctest run: 240 of 414
             sampled files carry a title page; top-1 80.8%, within
             top-5 90.0%, wrong 4.6%, no match 5.4%. TWO THINGS
             SETTLED BY MEASUREMENT, not taste: an edit-distance-1
             "near syllable" tier LOWERED top-1 (81.8%→80.4%) and is
             not shipped; the 0.5 score floor beat 0.35 (costs 2
             correct, removes 4 confidently wrong). The corpus route
             is NOT built — the title-page banks answered well
             enough that corpus matching would add guesswork, not
             recall; revisit only if intake material turns out to
             lack title pages.
             Manual: new Chapter 14 (the old 14-16 shifted to 15-17);
             17.1's stale "six groups / seventeen panes" table fixed
             to seven/twenty-two on the way past (audit finding 3,
             partial).
             THE HONEST GAP: measured against LIBRARY files, whose
             title pages are clean. Adam's actual in-house intake
             folder has not been seen yet — when he supplies it, the
             first job is re-measuring on that material.

         (2a) DONE 2026-08-19 (loop 1, suite 45; re-measured under
             the uncapped-number rules in loop 10: 347 cited / 297
             present / 50 missing). BIBLIOGRAPHY AUDIT lane — GMR's
             own "first job"
             (cataloging session 1, Dec 22 2025, SBceNNuHtRc;
             docs/research/GMR_CATALOGING_SESSIONS.md): every ACIP
             number cited in the published Mixed Nuts
             bibliographies must exist in the database. MEASURED
             2026-08-19 against the installed library: 349 distinct
             cited numbers, 298 present, 51 MISSING — evidence
             banked at data/research/gmr_cataloging/
             bibliography_audit_missing.json. Build the report into
             the Catalog pane (per source volume, citation text as
             evidence). NOTE: installed library ≠ ACIP master tree;
             ours is a lower bound.
         (2) DONE 2026-08-19 (loop 15, suite 54): QC lanes shipped —
             title-vs-translation mismatch + colophon-aware
             same-title/duplicate detection; found 8 real flags and
             107 true-duplicate filings in the installed Sungbum;
         (2c) PHASE 3 ALSO COMPLETE 2026-08-19 (loop 17-19, suites
             55 + extensions): shelf suggestions at their measured
             ceiling (31% top-1/43% top-3 held-out — shelving stays
             human, numbers printed on the report), witness-hunt
             links (45/50 missing works yield a BUDA query from
             their own citation; export handout), worksheet
             enrichment (MD5, folio range, BDRC permalink).
         (2b) BUILD QUEUE COMPLETE 2026-08-19 — all ten items below
             SHIPPED via the build loop (suites 45-52, sweeps 1->9
             controls, manual 14.5-14.14; see
             docs/plans/CATALOG_WORKFLOW_LOOP.md for the measured
             checkoffs). Plus mid-loop: Adam's title translator
             (attestation workbench), the live Google-sheet
             worksheet/register read (52-column schema banked in
             the research doc), the no-5-digit-cap ruling + dashed
             sub-number key logic. FROM THE 13 SESSION VIDEOS
             (2026-08-19, all analyzed:
             docs/research/GMR_CATALOGING_SESSIONS.md — "The house
             rules, consolidated" section IS the build spec). The
             build queue, in value order:
             · VOLUME SPLITTER ("chop assist") — their #1 manual
               activity: detect title pages / BZHUGS SO blocks /
               kar-chag entries inside one file, propose cut points
               w/ evidence; never auto-cut; support cut-backwards
               and remove-as-you-extract; mother copy untouched.
             · FILENAME COMPOSER — NUMBER_TIB_ENG_AUTHOR + .TXT
               grammar, the punctuation rules, the length limit w/
               mid-word stop + "+" + companion _META file
               generation; validates against decodeAcipFilename.
             · ASCII MASTER CATALOG generator — GMR's session-3
               directive ("we don't have a LIST of what's in the
               folders"): walk the library, emit the field-coded
               ASCII catalog (S:/N:/T:/author/format/pages/dims/
               colophon where known); St. Petersburg format.
             · TREE DIFF — the divergence audit (GMR's copy vs
               Nick's): compare the two browser trees, list
               adds/removes/renames; the dual-tree pane is the UI.
             · TOHOKU-FIRST numbering support in the identity lane
               (we hold Toh concordances); never mint numbers.
             · COLOPHON FINDER — locate colophon candidates near
               text end (jar/mdzad/bris + translator-credit
               patterns), display for the author-determination
               step; translator ≠ author warning.
             · Cleanup lanes: line-slash stripper (Aug 4 directive),
               lowercase-run detector, double-space-in-title check.
             · Provenance signals surfaced per file: western-style
               pagination (no folio A/B), parenthesized small-print
               titles, computer-font-era markers → "typed = suspect"
               banner.
             · Register spreadsheet import/view (Nick's) w/ folio
               start/end columns; three-state display (numbered /
               input / cataloged).
             · Initials+date change-log helper on destination-tree
               folder renames (their Dropbox policy, in-app).
             NOTE Aug 11 session: GMR mentions "Adam's been working
             on" translation tools — this project is known to the
             class; Lioran Katz demo (AI translation tools) was
             scheduled for the following Monday.
             NOT CAPTIONED: Mar 24 session (bq8SfTu558c,
             "Mixed Nuts Translation with Geshe Michael (Xuanzang's
             Tower)") — whisper tooling ready; media RETRIED
             2026-08-20 (yt-dlp 2026.07.04, android_vr/tv/ios
             clients): metadata answers, media still 403 — the
             block is server-side; retry again later.
         (3) DONE 2026-08-19 (loop 13, suite 53): the worksheet form
             shipped on the team's live 52-column schema (Adam's
             Google sheet, banked in GMR_CATALOGING_SESSIONS.md) —
             prefilled from the file, sidecar-saved, CSV row export;
             the live spreadsheet stays the master. FROM
             SESSION 1: add witness census (how many versions, same
             works?), witness quality tier (carving > vetted ms >
             modern single-pass typeset), corruption evidence,
             input-fidelity note ("faithful reproduction of a
             corrupt version" is the standard), provenance leads
             (who to ask, by name, status); and the THREE separate
             states — number issued (by whom) / input exists /
             cataloged to a shelf — a work can have a number and
             exist nowhere;
         (4) DONE 2026-08-19 (loop 14): routing shipped —
             catalog-identity proposals in the existing channel with
             full evidence (rule, candidates, Tohoku, worksheet
             snapshot), Approval filter, existing candidates-file
             export; the app never writes the catalog, numbers stay
             registrar-issued, proposer+approver recorded by the
             channel itself;
         (5) DONE 2026-08-19 (loop 12): the handoff shipped — Move
             to shelf with move-not-copy, the META companion
             travelling (pair-undo if it cannot follow), collisions
             refused, nameless names sent to the composer, stamp
             offered, censuses refreshed. GROUNDWORK notes follow:
             Adam's directive ("dual file browser windows, both
             tree view") shipped same day — the pane is now two
             independent QFileSystemModel tree browsers (intake
             left, destination right, each with its own root
             chooser + census + sess-remembered root; destination
             defaults to the app library), identity painted per
             file IN the tree via IdentitySuffixDelegate, shared
             evidence panel below. The two trees are the two ends
             of this move.
      9c. **RECOVERED DEFERRALS (Adam, 2026-08-15: "look for any
         'saved for later' notes and add them to the updated todo
         list").** Swept every session transcript, all of docs/,
         data/help/ and the whole codebase for deferral phrasing.
         Code carries ZERO TODO/FIXME/LATER comments; docs were clean
         (the USER_MANUAL "deferred" hits are the proposal system's
         own DEFERRED status label, not project debt). 21 real
         deferrals surfaced in the transcripts, 13 were already on
         this board. These EIGHT were not, and are now:
         · **Arrow-key / highlighter revisit** — the shader stalling
           on long strings. My fix made it WORSE and was reverted
           2026-08-15; I said the queue would carry a note and it
           never landed here, which is why this went missing. Terms:
           a JOINT session with Adam driving the exact reproduction,
           ONE semantic change at a time (likely the walk-target fix
           alone first, leaving the up/down ladder untouched). The
           lattice probe is kept as evidence. LESSON: arrow-key FEEL
           cannot be signed off from a simulator.
         · **`.hyp` legacy importer** — DONE (verified 2026-08-20
           reconciliation): "Import .hyp…" ships in the Align pane,
           allcore::parseHypFile proven on tutorial.hyp (hyp_smoke).
           This line had gone stale-open.
         · **Word-level scan following** — blocked on coordinate OCR;
           today's following is line-band granularity.
         · **PPP-reverse lookup** — DONE (verified 2026-08-20
           reconciliation): allcore WhitneyRoots::byPpp shipped with
           battery pins (gata→gam, krta→kṛtá) and TWO app wiring
           sites (Lookup + Sanskrit). This line had gone stale-open.
         · **"Text reports" pane (option E)** — CONDITION MEASURED
           2026-08-20: the Overlay REVIEW section now holds SEVEN
           tools (Outline, Citations, Verse meter, Prose reader,
           Typography, Spellcheck, Teachings) — "outgrown" by any
           reading. But ruling E is Adam-ratified and tied to the
           joint walkthrough, so the pane is NOT built autonomously;
           the measurement goes on the walkthrough agenda for Adam's
           call.
         · **Night-mode + typography audit** — needs fresh
           screenshots against the current layout; pairs with the
           joint T/K/F walkthrough already in Adam's queue.
         · **Transfer-queue reordering + remote edit with
           auto-upload** — the two File Browser P3 items deferred
           with reasons.
         · **Botok/Monlam deeper revisit** — DONE 2026-08-20
           (docs/research/BOTOK_LATTICE_OVERLAP.md §Revisit): probe
           rebuilt at full in-app parity (547,946 words). Identity
           25.5%→41.7%, precision 98.4%, recall 92.9%; all 193
           divergent lines classified — 129 pure merges (vocabulary,
           not disagreement), 64 genuine crossings (19.3%), dominated
           by honorific name/title chunking. Ruling stands: botok
           stays a labeled reference layer. Two probe bugs recorded
           as method notes (trailing-tsheg off-by-one; ACIP-vs-wylie
           stream drift — same-stream by construction now).
      9h. **IN-HOUSE ACCESS + APPROVAL PIPELINE — SHIPPED 2026-08-20**
         (Adam's directive, same day: "in-house only… login… awaiting
         approval folder… all actions propagate through the linked/
         shared official dropbox"). allcore catalog_actions (suite 57,
         15 pins): ActionLedger over "<official root>/AWAITING
         APPROVAL/" — stage COPIES (mother copy untouched, META
         companion travels), plain-ASCII ACTIONS.tsv ledger (id, ts,
         actor, action, source, staged name, proposed shelf, status,
         decided-by/on, note), approve MOVES onto the proposed shelf,
         reject → REJECTED/ with reason, nothing overwrites, second
         decisions refused. App: CATALOG_TEAM.tsv roster IN the
         official root (salted SHA-256 passphrase hashes; roles
         admin/approver/cataloger; first entry bootstraps as admin =
         Adam; honesty: team discipline + attribution, the Dropbox
         share is the real perimeter); Sign in gate — all 10 action
         buttons disabled until login; cataloger's "Move to shelf"
         becomes STAGE for approval (identity evidence in the note);
         approver still moves directly; Approvals… dialog (pending
         table, approve/reject approver-only); Team… roster admin.
         Official root = sess catalog/officialRoot (the team's
         Dropbox folder; dest tree defaults there). Selftest pin:
         roster verify + lock holds + stage→approve lands on the
         shelf. Manual 14.20 (+ 14.15 role note). NOTE FOR THE TEAM:
         Dropbox propagation is file-level — the app never calls
         Dropbox; concurrent edits of ACTIONS.tsv on two machines
         resolve as Dropbox conflicted copies (rare in practice:
         approve/reject happen on one machine, GMR's).
      9i. **DOCUMENTATION & TEACHING MEDIA PROGRAM (Adam,
         2026-08-20 — "put on the todo list"):**
         (1) REVAMP + BEAUTIFY THE USER MANUAL — design pass over
             the 17-chapter manual (typography, layout, visual
             hierarchy, screenshots per chapter; possibly a styled
             HTML/PDF edition alongside the docx).
         (2) SCREEN RECORDINGS of how to use EVERY tool/function —
             one short demo per feature, mirroring the manual's
             chapter structure (the TEST_CAMPAIGN.md item list is
             the natural shooting list; docs/DEMO_SCRIPT.md exists
             as a starting script).
         (3) A YOUTUBE CHANNEL dedicated to demonstrating every
             aspect of the tool — channel setup is Adam's (account/
             branding); the machine can script each video, generate
             the demo material/screenshots, and draft descriptions.
         NOTE: sequence AFTER the test campaign — record features
         once they've survived the real-life pass, so the videos
         show the settled behavior.
      9j. **CARD FORMAT DESIGN PASS (Adam, 2026-08-20 — "figure out
         how to format the card info for each term"):** a deliberate
         design pass over the entry card's layout/typography/section
         order (the card has grown many layers: phonetics, glosses,
         corpus usage, honorific/idiom badges, 84000 block + toh
         attestations, Das/Jäschke links, reference stacks, teaching
         moments, footnotes…). AND: "a place to take and display
         MORE than just three of the corpus references" — the card
         currently caps corpus renderings; design where the full
         set lives (expander on the card, a "see all N corpus
         hits →" link into the concordance view, and/or a
         configurable cap). Candidate shape: card stays a SUMMARY
         with honest counts ("3 of 41 — see all"), one click to the
         complete evidence. Pair with the test campaign's 🤔/💡
         findings about the card before implementing.
         DESIGN BOARD PUBLISHED 2026-08-20 (interactive mocks, real
         bsod-nams content, app palette): claude.ai/code/artifact/
         acd57e2b-7ff3-4959-a2a5-bf838676e1c0 — card A baseline /
         B summary+ledger (counts first, expand in place) / C
         two-zone tabs. The SAME board holds the 9k toolbar
         candidates (A long row / B primary+menus / C eyebrow
         groups). Answer by letter ("card B + toolbar C").
      9k. **TOOLBAR/CONTROLS PRESENTATION REDESIGN (Adam,
         2026-08-20: "i don't like the way that the toolbar displays
         the functions/tools/options"):** rethink how each pane's
         toolbar presents its functions — today most panes carry a
         long row of text buttons (the Catalog pane alone has 10+,
         Overlay's REVIEW column keeps growing). Directions to
         explore WITH Adam's campaign findings in hand: grouped
         menu-buttons (the Library "Maintenance…" pattern), icons +
         text, a two-row or sidebar layout, per-pane "Tools" menus,
         progressive disclosure (common actions visible, the rest
         behind one button). Pairs with 9j (card format) as one
         UI-polish wave AFTER the test campaign surfaces which
         controls he actually reaches for.
         DIRECTION RULED 2026-08-20 (Adam, Word-ribbon screenshot:
         "start an extensive implementation of a toolbar across the
         top as seen in the screenshot"): a WORD-STYLE RIBBON —
         labeled groups of controls in a strip across the top,
         group captions beneath, tabs when a pane's tools warrant
         them. ROLLOUT: RibbonBar component + Catalog-pane pilot
         first (worst button row; groups: Access · Suggest ·
         Inventory · QC · Handoff), then pane by pane as the
         campaign settles each; the design board's B/C candidates
         are superseded by this ruling. ROLLOUT PROGRESS
         2026-08-20: Catalog (pilot) + OCR (PAGE · VIEW · VOLUME ·
         MODELS) + Library (SHELVE · STUDY · CARE · FIND, search
         box in the ribbon) converted; RibbonBar relocated early in
         the file so every pane can use it. Remaining big rows:
         Export, Input toolbar, Draft/Overlay tool columns (bigger
         re-plumb), pending Adam's whole-window layout pick.
      9l. **EDIT MENU + WORD-STYLE MENU-BAR REORGANIZATION (Adam,
         2026-08-20, with two reference screenshots: the standard
         macOS Edit menu, and Microsoft Word's menu bar — "we will
         need an edit menu and i like the way that microsoft word's
         menus are organized and displayed"):**
         (1) A proper EDIT MENU with the standard roles — Undo/Redo,
             Cut/Copy/Paste/Delete, Select All, Find in Page/Find
             Again — wired to whichever editor has focus; using
             native menu roles also brings the macOS services rows
             (Writing Tools, AutoFill, Dictation, Emoji & Symbols)
             for free on text widgets.
         (2) Reorganize the whole menu bar in the WORD style Adam
             likes: top-level verbs (File · Edit · View · Insert/
             Format · Tools · Window · Help) with Word's visual
             grammar — related items in separator-fenced groups,
             ellipsis for dialogs, submenu arrows for families,
             keyboard shortcuts shown. Today's menu bar mirrors
             every pane's controls (the C2 audit mirror) — keep
             that coverage but re-house it under Word-style
             top-level menus instead of per-pane dumps.
         Pairs with 9k (toolbars) as the menu half of the same
         presentation wave; sequence with the campaign findings.
      9m. **FULL GUI RELAYOUT IN THE MICROSOFT STYLE — MOCKUPS FIRST
         (Adam, 2026-08-20: "the tool bar is going to be key. maybe
         we should reconsider the way our gui is layed out. maybe
         you could give me some mockups of how a new gui layout that
         would follow the way that microsoft works"):** produce a
         MOCKUP BOARD (like the 9j/9k design board) showing the
         whole app re-imagined Word-style: ribbon tabs across the
         top as the PRIMARY navigation (possibly replacing the
         current two-level group/pane tabs), one document/work
         surface below, panels docking right (card/evidence) the
         way Word docks Styles/Comments, a status bar. 2-3 whole-
         window layout candidates with real pane content; Adam
         picks; then staged migration (the 9k ribbon pilot on
         Catalog is the first working taste and already shipped).
         Supersedes/absorbs the pane-by-pane framing of 9k if Adam
         chooses a global relayout.
         MOCKUP BOARD PUBLISHED 2026-08-20 (same session, Adam:
         "the mockup would be cool to see now"): claude.ai/code/
         artifact/c6826fd7-6c4f-4508-9118-e1aed75748aa — Layout 1
         ribbon-as-navigation (Word-faithful, tabs switch live) /
         Layout 2 left rail + ribbon (Outlook-style) / Layout 3
         quiet migration (today's shell + ribbon everywhere — the
         Catalog pilot is its first real pane). Awaiting Adam's
         pick.
      9n. **FEATURE SCAN 2026-08-20 (Adam: "scan for any
         functionality that you think we could implement to make our
         tool more useful" — Word/PPT ribbon screenshots as the
         model). The ten, awaiting his picks:**
         (1) House-style GALLERY in Manuscript/Export (Word styles
             analog): one-click STD formats w/ visual previews —
             verse stanza, prose, heading, colophon, apparatus;
             RTF honors them (STD-004/006/007).
         (2) 84000 TM HINTS IN DRAFT: matching published renderings
             beside corpus hits while drafting — reference-only,
             never HGM; the 400k FTS index already exists.
         (3) FOLIO THUMBNAIL STRIP (PPT slide-sorter analog) in
             Scans + Input block mode: click-to-jump, per-page
             keying-progress states.
         (4) QUICK ACCESS TOOLBAR: user-pinnable favorite commands
             above the ribbon.
         (5) VISIBLE SEARCH FIELD top-right (⌘K surfaced).
         (6) TEAM COMMENTS ON TEXTS: annotation threads stored
             beside the file on the shared Dropbox (ledger pattern).
         (7) BACKSTAGE/HOME VIEW: recents + pinned work.
         (8) DRAFT-HEALTH CHIP: live Review-advisory summary in a
             status bar.
         (9) DICTATION (English fields; free with 9l native roles;
             honest limit: no Tibetan dictation, rule 3).
         (10) PECHA PRESET GALLERY: visual preview tiles.
         RIBBON-GRAMMAR upgrades for RibbonBar noted from the
         screenshots: big icon-above-label primaries, split buttons
         with dropdowns, in-ribbon galleries, right feature cluster.
      10. Idea bank (liked, never scheduled): pronunciation trainer (DECLINED-AS-CONCEIVED 2026-08-20: whisper's Tibetan too weak to honestly score learner speech — rule 3; needs a design conversation) ·
         Text DNA strips DONE 2026-08-20 (Overlay REVIEW → Text DNA: one colored column per source line — meter/off-meter/prose/blank + quote-announcement ticks; hover + click-to-jump; allcore textdna, suite 58; summary+jump selftest pin) · team board · apprenticeship mode ·
         English-on-the-carving hover DONE 2026-08-20 (cache-only tooltip over the woodblock: OCR-read wylie + HGM gloss, review-material label; seeded-cache selftest pin) · teaching-playlist karaoke ·
         ⌘K persons lane DONE 2026-08-20 (authors layer in Hunt Everywhere; the apparatus/notes lane had already shipped 08-16); remaining: none · live composing editor · register
         studies · reverse-composition drills · sight-translation
         timer · worked-examples casebook.

      **B. ADAM'S ONE-SITTING QUEUE (his actions; canonical copy)**
      **ElevenLabs pilot (2026-08-16, plan at
      docs/ELEVENLABS_TEST_PLAN.md, task #43):** two inputs are
      his and everything else is on disk — (1) ElevenLabs account
      + ~$20 credit; (2) three pre-2010 AUDIO-era samples
      (cassette/minidisc quality, one deliberately bad) — the
      repo holds none, and no verdict from clean 2020s video
      generalizes to the era Phase One exists to rescue.
      **Donation rails (2026-08-15, after a donor said "I don't
      have a PayPal account"):** the live site was PayPal-only,
      and a PERSONAL PayPal account does not offer guest card
      checkout — switching to a free Business account and turning
      on "PayPal Account Optional" is what unlocks card-without-
      an-account. Adam chose Venmo + Zelle + Stripe. Zelle is
      LIVE (needs nothing but his email, already on the page).
      Still needed FROM HIM: his Venmo handle, and a Stripe
      Payment Link (dashboard.stripe.com -> Payment links -> new,
      "customer chooses price"; Stripe signup wants bank details
      + ID, so it cannot be done for him). Both drop into the
      donate block in ~5 minutes once supplied.
      **Sends (drafts finished in Gmail):** Jamie Plant (#1 —
      irreplaceable recordings; confirm address) · 84000 license +
      partnership registration (their ToU asks for it) · THL (16
      dictionaries + Solr + kmaps) · rKTs/Lainé · Lotsawa House ·
      Vimala · PKTC (Chodrak + Das SCD) · Jigme (needs his Dropbox
      link) · Joel zoom (agenda: text→BDRC-work linkage) · Élie
      reply · Steinert thanks (asks re Jäschke index) · BDRC letter
      (81 unresolved authors) · BDRC deskew-bug report · GMR
      transcription appeal + GMR trust dialog (both need GMR's
      address) · fundraising.
      **Approvals/money:** git filter-repo (~20GB; disk at 3.4GB
      free — operational risk) · API credits (glossary stalled at
      1,395/93,630) · ElevenLabs pilot <$20 · MITRA 18GB download ·
      API key rotation (pasted in chat twice) · **get an Apple
      Developer ID** (2026-08-14 — unlocks notarized builds;
      $99/yr, developer.apple.com/programs; enroll as ALL if
      possible) · Windows/Linux build machines or CI when wanted.
      **Rulings:** catalog-titles tier · prenasal queue (225
      pending; Approval pane bulk-approve ready) · Sungbum workbook
      (963 yellow cells) · Sanskrit render-sheet eyeball rows ·
      unlisted-recordings archive go (68 links) · GitHub private
      remote say-so · TibetanDictionary.dic licensing option ·
      humilific/double-honorific schema blessing · display-toggles
      retest verdict.
      **Sessions:** K/F testing-campaign walkthrough + the 9
      scenario walkthroughs · 5-step human release gate (reading
      pass, woodblock word-locate #31, Input on a real scan, Mixed
      Nuts export, clean-Mac install) · dark-mode screenshots · GMR
      session: LE'UR class + trust-hierarchy ratification +
      STD-008/9 in his words + "what sources would you ADD" ·
      MP3-archive path for the whisper pilot · fundraiser
      pre-presentation steps (Share the artifact, verify donate
      URL, pick version; next week domain + Cloudflare + Zeffy).
      **BDRC "ASSOCIATED PEOPLE" RELATION GRAPH (Adam, 2026-08-15)**
      — pull BDRC's person-to-person relational fields into the app
      so a person card shows not just who they were but *who they
      stood between*: teachers and students, incarnation lines,
      kinship, and the seat/monastery each was associated with.
      Feasible with what we already hold: we store BDRC person ids
      (`pid`) for 184 resolved catalog authors plus Elie's
      authoritative per-text links, and BDRC's Linked Data Server
      (purl.bdrc.io, `buda-base/lds-pdi`) exposes SPARQL and
      per-resource RDF — so the relations come from the same graph
      whose ids we already trust, no new authority and no scraping.
      Build shape: harvest relations per known pid (respect their
      rate limits, cache to `data/extracted/person_relations.json`),
      store edge type + direction + source id verbatim, and render
      a "connected people" block on the PERSON card with each edge
      clickable through to that person's own card. Two disciplines:
      an edge appears only when BDRC asserts it (no inferring a
      teacher from a shared monastery), and where BDRC marks a
      relation uncertain we surface the uncertainty rather than
      flattening it. **This is the natural join to the monasteries
      item below** — BDRC's seat/affiliation edges are exactly what
      ties a person to a house, so harvesting relations and places
      in one pass gives both layers at once.
      **Enrich with Treasury of Lives (Adam, 2026-08-15)** — ToL
      carries relational detail BDRC often lacks (who taught whom,
      incarnation lines, which house someone headed and when). What
      the earlier survey already established, and what it means
      here: ToL **metadata is CC0** and reusable, but the biography
      **essays are non-commercial — link out, never copy**; and
      ToL's API is **Cloudflare-gated against scripts** (a browser
      loads it fine, a harvester does not). So the practical route
      is the one already proven for identity: **BDRC carries
      `owl:sameAs` pointers to ToL resource ids**, so a single BDRC
      harvest yields the ToL identifier for free and its CC0
      metadata can be joined without fighting the gate. Where BDRC
      and ToL disagree on a date or a relation, keep BOTH and flag
      (their editorial judgments genuinely differ); where only ToL
      asserts an edge, label it ToL-sourced so provenance shows on
      the card. If we ever want the essays' depth rather than the
      metadata, that is an EMAIL to Treasury of Lives, not a
      scrape — add it to the sends queue if this layer is
      prioritized.

      **MONASTERIES LAYER (Adam, 2026-08-15)** — bring a total
      study of the monasteries into the app: which house held which
      lineage, who taught there, what was written there, and where
      each sits. Sources, in the order their licensing allows:
      (1) **THL Places / kmaps gazetteer** — the survey already
      found a *publicly queryable Solr core* behind the new site
      (`kmterms`, ~4.49M docs) plus per-feature JSON at
      `places.kmaps.virginia.edu/features/{id}.json`; the app
      already link-outs to it, so this is an upgrade from link to
      layer. Two gates: heavy 429 rate-limiting (single-digit
      requests/minute — harvest slowly, cache locally) and the
      per-dataset licence, which the THL email in Adam's sends
      queue is meant to settle. (2) **Treasury of Lives** — People-
      layer work already established its metadata is CC0-favorable
      and the app links persons to it; monastery/institution records
      cross-reference the same person IDs, so this joins the People
      layer to the Places layer for free. (3) **BDRC place records**
      (bdr:G…) — already our authority for texts and persons, so
      monastery IDs come from the same graph we trust. Build shape:
      a `data/extracted/monasteries.json` keyed by BDRC/kmaps id
      with name, location, founding, lineage, and person links,
      cross-checked between sources with disagreements FLAGGED not
      resolved; surface it as a Places/Monastery card in the Library
      and People panes. Adam's action: supply the **PDF of GMR's
      translation of the Je Tsongkapa biography** — that is the
      period-context source for which monasteries mattered in his
      lifetime, and it can seed the first curated subset (facts with
      citation only; nothing from the book ships verbatim).

      **Recordings to obtain & mine:** **Diamond Way Translator
      Course, 2026-08-15** — GMR gives explicit translation/
      formatting rulings in this session, including *removing
      semicolons* and *adding spaces*. Get the recording +
      transcript, mine the rulings into
      docs/standards/HGM_TRANSLATION_STANDARDS.md as new STD-
      entries, then wire the punctuation ones into the Draft
      pane's house STYLE CHECK (it already lints DCC rules, so
      these become new lint rules) and the Input-pane formatter.
      Adam's action: supply the recording/transcript. **ALSO contact
      Ven. Utpala** to check whether these punctuation rules are
      already on the house style sheet -- if they are, the sheet is
      the authority and we cite it; if they are not, GMR's session
      ruling is new and the sheet may need updating. Ask before
      implementing either way.

      **Uploads:** input-center standards + samples (announced
      08-06) · Master Catalog · intact Release IV manual/Briefcat ·
      honorific books · reference pecha prints · Das SCD
      re-download · Xcode vision projects · Fall-2022/Winter-2021
      link sheets · more already-composed bibliography collections.

      **C. EXTERNAL WATCH (dated/triggered)**
      BDRC OCR release ~end Aug → model refresh + selftest rerun ·
      BDRC written OCR permission to bank beside the verbal okay ·
      Lainé reply → rKTs abbreviations layer + OM-register growth ·
      THL reply → kmaps/Solr + dictionary licenses · 84000 reply →
      TM ingest (resolve BY-NC-ND-vs-BY-4.0 conflict first) · PKTC
      → Das etext + Chodrak lane · CIHTS → Negi · Tsadra →
      RY/Waldo/Valby · DILA site return → Mahavyutpatti TEI
      refresh · web team → collections manifest JSON · input-center
      server → remote-lane shakedown (P3 queue-reorder + remote
      editing wait on it) · data project → Sanskrit
      V/visarga/candrabindu/A'A fixes (177k+ occ), pyewts-13 +
      prenasal-defect regenerations, anusvara TSV ingest (2,172),
      oṃ→ॐ question, LC pair-script banking, attestation recount,
      register-annotation growth, then the NEXT RELEASE the new
      importer exists to consume · tibetanebook.com +
      dharmaebooks.org license checks · Banzhida MT weights ·
      Monlam Melong license statement · transcription enterprise
      quote.

      **D. DEFERRED BY DESIGN (reasons logged — not open work)**
      Files pane: share sheet (ObjC bridge) · 7z (p7zip dep) · tag
      writing (Finder stays editor) · P3 queue reorder + remote
      editing (need a real server) · P4 long tail (S3, embedded
      terminal, hex viewer). Espel's other four phonetics display
      conventions (port when wanted) · EPUB/e-reader export lane ·
      Old-Tibetan affixnorm rules (until Old-Tibetan enters the
      library) · Overlay incremental lattice rebuild + .hcprj/<NNN>
      legacy interop · Manual-of-Authentic colloquial Trainer mode
      (needs a labeled second corpus; parked unless wanted) · MST
      honorific rule-detector + "who is doing what" lesson · People
      browser under Research · NetWylie/Lingua::BO extra oracles ·
      Gate E text-reports pane (only if REVIEW outgrows itself) ·
      Chodrak scan-and-OCR fallback (until PKTC answers; US term
      blocks distribution) · py-tiblegenc native port (external
      tool shipped instead) · Lhasa gap-heal growth lane (optional).

      **E. POLICY FENCES (do not resurrect)**
      Cataloging sidelined (own future project, SCOPE DECISION
      above) · NEVER lane (unlicensed dictionaries; no paraphrase)
      · Monlam definitions closed (word lists only) · red-flag list
      ships nothing unresolved · pron-engine fence (external
      oracles may only cross-check THL phonetics; canonical
      pron_engine.py + GMR rulings are the sole authorities) ·
      hgm_gloss binding / never reconstruct engines / never guess /
      provenance sacred (CLAUDE.md rules 1-4).

- [ ] **SOURCE TRUST DIALOG WITH GESHE MICHAEL** (Adam, 2026-08-14,
      all-caps priority: "which sources to trust over others and
      why"). PREPARED: docs/standards/SOURCE_TRUST_HIERARCHY.md —
      the tool's ACTUAL current five-tier ranking, every open
      question enumerated (published-vs-spoken gloss precedence,
      Tier-III ordering, strike-list, 'a-chung class, imperial-vs-
      ACIP Sanskrit rules); ratified answers become STD-009 in his
      own words, then the tool is audited against it. EMAIL DRAFT
      r8445981399472347202 (needs GMR's address — same as the
      transcription appeal). Folds in the LE'UR item above.

- [x] **DISK: approve the git history cleanup + review the full drive.** DONE 2026-08-24 with Adam's explicit approval. `git filter-repo --invert-paths --path dist --path cmake-build-release`: **.git 10 GB -> 185 MB**. Verified rather than assumed - HEAD^{tree} is BYTE-IDENTICAL (1408eda1...), so not one tracked file changed; 3,892 of 3,894 non-artifact paths survive in history and the only two missing are `dist` and `cmake-build-release` themselves; gate 82/82 after. 61 commits dropped, all of them presses whose entire content was build output. The pre-rewrite history is preserved as ALL-translation-tool-PRE-FILTER-REPO-2026-08-24.git on the 8 TB drive - DELETE THAT ONLY when you are satisfied, it is the rollback. Mirror remade (hashes all changed, so a push was impossible) and the `backup` remote repointed. Free disk 326 MiB -> 14 GiB. Side effect worth knowing: macOS purged every APFS local snapshot under the pressure, so Time Machine local restore points are gone. (was:
      review the full drive.** 2026-08-13 postmortem: presses had
      committed every DMG/build into git → .git = 21GB; ENOSPC
      broke a press mid-rsync (recovered; installed app verified
      ALL PASS; artifacts untracked going forward). The 21GB
      HISTORY remains until you approve:
      `git filter-repo --invert-paths --path dist --path
      cmake-build-release` (local-only repo, no remote; sources
      untouched; old DMG blobs are worthless). Separately: the
      2TB drive is at 100% overall (3.4GB free) — worth a personal
      sweep beyond this repo.

- [ ] **BDRC OCR: new version lands ~end of August 2026** (Élie
      Roux's email, 2026-08-13: "a significant improvement").
      When released: refresh the bundled BDRC_PhotiLines /
      BDRC_Woodblock models, rerun the OCR selftests + the
      word-locate lanes, and re-check the deskew-0 deviation still
      holds. Also: thank-you/reply to Élie drafted in Gmail.

- [ ] **Confirm with Geshe Michael Roach how to address the LE'UR
      pronunciation exception** (Adam, 2026-08-13). "le'ur"
      ("lay-oohr") is CONFIRMED for display and is live in the app;
      what awaits GMR's word is how the exception class is to be
      addressed in the pronunciation system. With that ruling,
      also decide whether it generalizes: 465 dictionary entries carry
      an 'a-chung+u syllable of this class (le'ur, mtha'ur…) — if
      general, implement as a DISPLAY-layer transform (engine stays
      battery-locked canonical) and check against the banked ACIP
      pronunciation standard (docs/standards/
      ACIP_PRONUNCIATION_STANDARD.md). Ruling row:
      pronunciation-2026-08-13-adam-1 in data/proposals/proposals.tsv.

- [ ] **Send the 84000 license email** (drafted, docs/drafts/
      EMAIL_84000_license.md — resolves the CC-BY-4.0 vs BY-NC-ND
      discrepancy; unlocks the best-licensed aligned Ti–En dataset in
      existence). VERIFIED 2026-08-12 from their canonical
      all-data/Terms_of_Use.md (rev. 2023-06-14): the license TABLE
      says RDF metadata = CC-BY, **translation memory = CC-BY**,
      glossaries = CC-BY; translations = CC-BY-ND. The TM repo
      README still carries old BY-NC-ND 3.0 boilerplate — that's
      the contradiction to cite. NOTE their terms also ask data
      partnerships to REGISTER a written agreement (info@84000.co)
      — the email should both cite the table and register the
      partnership. ✅ BDRC OCR: Adam reports BDRC gave the OK
      (2026-08-07) — stage 2 UNBLOCKED; save the written confirmation
      to docs/licenses/ when available.

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
      — RERUN 2026-08-14: build_main_apparatus.py against the
        current release — output unchanged (344/138; v27.2 still
        current, so nothing new to fold). Next rerun at the next
        release. Original: rerun the three apparatus tools per new release;
      confirm STD-008 in GMR's own words for the record.
      — dedupe/merge RESOLVED BY MEASUREMENT 2026-08-13: zero
      verbatim lemma+text duplicates exist across the 2,234 notes;
      the 107 same-lemma multi-note cases are genuinely different
      annotations and must stay separate. Nothing to merge.
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
      placement labeled approximate). WORD-LEVEL COORDINATES SHIPPED
      2026-08-08 in the Scan pane (OCR word spans below); OVERLAY
      WORD-LOCATE SHIPPED same day — opt-in "locate word (OCR)" nav
      button OCRs the cached BDRC folio (deskew forced 0°, labeled)
      and boxes the cursor's word on the woodblock; locator only,
      text never surfaced; matcher verified on the PV fixture.
      Sungbum SHIPPED
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
      TITLE AUTO-FILL SHIPPED 2026-08-14: the compose dialog now
      falls back to catalog_titles.json (v29-31 waves + the ACIP
      catalog's own English titles via Élie's CSV) when the works
      record lacks an English title.

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
- [x] **Mixed Nuts style sheets / style guide** — ARRIVED 2026-08-14 (Diamond Cutter Classics Style Guide, 2023-08-25, via Adam's Dropbox link): original banked docs/standards/DCC_Style_Guide_2023-08-25.docx; distilled to data/help/STYLE.md (6 in-app Help chapters, 'Style: ' prefix); Draft pane HOUSE STYLE CHECK button ships the mechanical rules (quotes/dashes/ranges/ampersand/era/word-use list; flags never auto-fixes — the guide itself forbids Replace All; selftest 150). Subtler rules (italics-in-root-text, quotation placement, capitalization) documented, left to judgment. (was: Adam will get it from the
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
- [x] "ACIP Standards for Simplified Tibetan and Sanskrit Pronunciation" doc — FOUND + BANKED 2026-08-06 (docs/standards/ACIP_PRONUNCIATION_STANDARD.md; the still-missing Input Code CHART pages ride with the intact-Release-IV item) —
      validates/replaces the provisional Sanskrit pronunciation style.
- [ ] Adam's reference Xcode projects for the overall tool vision.
- [x] Git init — done 2026-08-06 at Adam's "next most logical step":
      initial commit 686d148, 103 files; generated DBs/artifacts/user data
      ignored (all rebuild from releases). Optional next: a private GitHub
      remote for backup (needs Adam's say-so — pushing publishes).

## Waiting on the data project (Cowork sessions)

- [ ] **[DATA-PROJECT-GATED, per the research doc: "the fixes
      belong to the data project"] Sanskrit conversion gaps
      (canonical engines — findings
      2026-08-13, docs/research/SANSKRIT_IN_TIBETAN_GLYPHS.md;
      CENSUS ADDENDUM same day: 82,834 attested syllables swept —
      V=wasur gap is 177k occurrences incl. everyday TIBETAN
      grva/zhva/rtsva/dvags; visarga ':' 56k; '%' candrabindu;
      fix-priority table in the doc; census file
      data/extracted/sanskrit_cluster_census.tsv):**
      (1) ACIP V in clusters unmapped: SV'AH'A/SATTVA/SARVA flag
      instead of yielding wasur stacks (fix: cluster V → EWTS w in
      hgm_tools.acip_to_ewts; target glyphs verified present);
      (2) A'A must merge to long ā (ཨཱ, EWTS "A") — A'ARYA renders
      standalone འ today; (3) sanskrit_convert's own IAST v→W table
      disagrees with its cluster output (SV'A H'A) — align. After
      fixing: extend batteries, then re-port to allcore per the
      inviolable rules. App guard: `--sanskritcheck` (23 stacks,
      exits non-zero on missing glyphs).
- [ ] Anusvara keying sweep (data-side): mantra syllables keyed
      with uppercase M (OM, H'UM) render final ma (ཨོམ) not ཾ
      (ཨོཾ); a corpus sweep for uppercase-M mantra syllables would
      list review candidates. SWEEP RUN 2026-08-14:
      data/extracted/anusvara_review_candidates.tsv — 2,172 flagged
      (H'UM 1,512 · LAM 346 · BAM 76 · …, mantra-context filtered).
      Handed to the data project; item stays open until ingested.

- [x] Review the 13 pyewts source-defect candidates — DONE
      2026-08-13 (docs/research/PYEWTS_DEFECT_REVIEW.md): ALL 13
      are stored-Tibetan defects (engine==pyewts against stored in
      every case): 10 truncated finals, 1 wrong-word (bsgrub bya
      stores bsgrub byed's Tibetan), 2 different-text. Correction
      handed to the data project (regenerate via the canonical
      engine); this repo applies nothing.
      Original: Review the 13 pyewts source-defect candidates
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
      = Geshe Michael Roach Dictionary headwords via the battery-proven conversion
      chain (lazy one-time build, timing shown), out-of-lexicon runs
      ⟨bracketed⟩, non-Tibetan grayed — labeled reference display only,
      spans stay lattice-bound. Facade checks in botok_tok_smoke.
      — INCREMENT 4 SHIPPED 2026-08-07 (COMPACT TRIE + FULL LEXICON):
      allcore::botok::SegTrie — interned syllable ids, one flat edge
      map, 2 bytes of flags per node (leaf + affix-type + aa); the
      SAME maximal-match walk minus node-data bookkeeping. Proven by
      cross-battery: SegTrie::segment == ported Segmenter::segment
      word-for-word (incl. affix info) on all 40,758 corpus segments
      (100.000%); memory ~20 MB at 105k words (vs hundreds of MB in
      the ported node layout — the ported path stays the oracle
      anchor). Overlay segmenter lexicon is now HGM headwords + BOTH
      Monlam lists (RefLexicon::eachWord), and matched affixed forms
      show their affix tag (+gi, +gis, ·འ for lost a-chung). En route:
      REAL BUG fixed — monlam_lexicon_1.txt is UTF-16LE and the old
      byte-wise RefLexicon reader banked it as garbage (its 15,349
      list-1-only forms could never attest; the old smoke's label
      check was blind because "Monlam" prefixes both labels);
      normalized on load + regression-locked (list-1-only ka ka ni).
      — AFFIX-SPLIT DISPLAY SHIPPED 2026-08-07 (splitaffixed adapted
      as display info, not stream mutation): segWordBaseForm/
      segWordAffixSurface strip the affix codepoints and restore a
      lost final འ; the Overlay card shows "མཐའི = མཐའ + འི (gi)".
      Battery: mtha'i→mtha' (aa restored) + der→de+r (no aa).
      **THE BOTOK PROGRAM IS COMPLETE** — remaining only: possible
      deeper Overlay integration with Adam's direction.
- [x] **Monlam word lists** — BANKED + FIRST INTEGRATION 2026-08-07
      (Apache-2.0 verified; 107,108 + 342,340 unique forms; allcore
      RefLexicon, 18th suite proves the real conversion path matches
      the lists' storage form exactly): Overlay click card shows
      "attested in the Monlam (Grand) Dictionary word list (reference)"
      for known spans AND for words the Geshe Michael Roach Dictionary doesn't have —
      real-word vs possible-typo signal, display only. SEGMENTER
      LEXICON DONE 2026-08-07 (Botok increment 4: both lists feed the
      compact SegTrie; UTF-16 list-1 loader bug fixed en route).
      WORD-LEVEL PASS DONE 2026-08-07: Overlay toggle "unattested-word
      hints" — maximal lattice-UNCOVERED, non-particle runs are
      segmented by the proven SegTrie over HGM + Monlam; only
      segmenter-OOV words get the slate dash-dot hint (single
      syllables of known words are never flagged; mapping back to
      tokens by syllable count; conversion failures skip the run
      honestly; count in the hint line). THE MONLAM ITEM IS COMPLETE.
      Definitions are NOT open — lists only.
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
- [x] **THL wylie.js diff-oracle** (second oracle) — RUN 2026-08-08
      over the same 105,634 pairs (tools/thl_wylie_oracle.py +
      thl_wylie_runner.js driving Espel's wylie.js from the ACIP dev
      folder via node; harness only). Verdict: 91.37% exact — WITHIN
      0.03% OF PYEWTS with identical class counts (7,398 ⟨wylie⟩
      flags, 83 IAST, 13 defect candidates), 1,135 letter-level after
      NFC. KEY ADDITION: comparison is NFC-normalized — the master
      stores precomposed DHA/DDHA (U+0F52 …, composition-excluded so
      NFC decomposes them), THL emits base+0FB7; canonically equal,
      7 pairs recovered. Two independent codebases now bracket the
      same ~1.1% master-side residue: our engine's divergences from
      strict EWTS are data variants, not conversion errors. Remaining
      sample classes: Sanskrit stack conventions (gupta = པྟ vs པཏ),
      final-cluster stacking (rgold), au-diphthong (kau = ཀཽ vs the
      master's double-o defect ཀོོ), and wylie↔tibetan field
      mismatches (rab tu / kun tu) — all dictionary-side findings.
      (NetWylie's Wylie.java skipped: same Espel lineage as wylie.js,
      a third run of the same algorithm adds nothing.)
- [x] **diff-match-patch (C++/Qt, Apache-2.0) + antx logic** → the
      input-center double-keying proofreading pane (roadmap item A).
      DONE by 2026-08-13: dmp semantic diff with inline orange/green
      highlights shipped earlier; ◀ disc / disc ▶ navigation added
      2026-08-13 (cursor jumps discrepancy to discrepancy with an
      i/N readout — no scroll-hunting). antx's transfer concept is
      N/A to double-keying (both texts are keyings, not annotation
      carriers).
- [x] **Tibetan collation** — SHIPPED 2026-08-07 via QCollator("bo")
      (brew Qt's ICU carries the CLDR Tibetan rules — probed first:
      REAL tailoring, e.g. ཀྵ inside the ka section against codepoint
      order, and full traditional root-letter sections — དཀའ/བཀའ sort
      under KA before ཁ). collation_smoke (24th suite, Qt6::Core)
      pins the traditional scheme pairwise: 30-letter alphabet, vowel
      order a-i-u-e-o, suffixes in alphabet order before vowels,
      bare→subjoined(incl ཀྵ)→prefixed→double-prefixed, multi-syllable
      splits, and the anti-codepoint tailoring witness — an ICU/Qt
      upgrade that changes ordering fails loudly. Lookup pane gained
      "Browse the dictionary (Tibetan order)": all headwords sorted by
      QCollatorSortKey (lazy, count shown), click = normal lookup.
      NOTE: Monlam's own list files are only ~80% in this order
      (Sanskrit-diacritic interleaving differs) — measured, not used
      as a fixture. No port needed; tibetan-sort-js fallback unused.
- [x] **eKangyur/eTengyur PD corpora** (Esukhia/OpenPecha) — citable
      diff-proofread canon beside the HGM corpus. eKANGYUR ACQUIRED
      2026-08-13: Esukhia derge-kangyur cloned (103 vols, 336MB,
      Public Domain per its README) to editions/ (gitignored, out
      of the press); tools/index_ekangyur.py → 1,108 Toh texts
      indexed with volume/offset/folio (ekangyur_index.json);
      CROSS-CHECK: Toh 16 starts folio 121a = BDRC manifest labels
      exactly. APP LANE SHIPPED 2026-08-13/14: compareWithEkangyur
      (KD via eKangyur AND TD via eTengyur — editionDir(bool),
      ekStripMarkers, dmp edition diff, menu offers on both);
      eTengyur indexed too (3,331 texts). Item CLOSED — remaining
      only eyeball spot-checks as texts get used.
- [x] **py-tiblegenc** (Apache-2.0) — SHIPPED 2026-08-15 (the UTFC/
      tiblegenc hookup: completion dialog offers open-in-Input;
      18 wiring sites in app). This line had gone stale-open;
      verified + checked 2026-08-20. Port maps later if ever needed.
- [x] **lucene-bo** — Adam approved full inclusion 2026-08-07
      ("Lucene is good to include"). Affix rules ENGINE + LOOKUP
      SHIPPED 2026-08-07 (below); INTEGRATED 2026-08-07 per
      Adam ("integrate what we need to integrate"): allcore searchnorm
      (25th suite) — PaBa fold (post-strip, upstream semantics);
      verb-lemma fold from OUR CC0 bank (1,697 unambiguous folds, 373
      ambiguous skipped; full table round-trips through our converter
      — pyewts bootstrap-only); normalizeTibetanUnicode (TibCharFilter
      strict+lenient maps, reorder, Old-Tib merged-syllable splits 1-3
      as upstream defaults, Sanskrit ts→c folds) for unicode entry
      points + future OCR text. Wired: wynorm + spine (38,556 folded
      segments; byed +7,334, pa +7,221) + libindex v3 (969k
      fold-reachable lines). Documented skips: TibEwtsFilter +
      SyllableTokenizer (covered natively). Pronunciation search
      SHIPPED 2026-08-07 (spine pron_index fold table over all 105,634
      GMR pronunciations; Spine::lookupByPronunciation exact+prefix;
      battery: sunam→bsod nams, spaced/cased input, nonsense silent;
      Lookup pane fallback labeled "matched by pronunciation") — OUR
      deterministic expression of the phonetics family; upstream's
      English-speller folding tiers (ee→i etc.) remain optional later
      lenience. REMAINING app-side CLOSED
      2026-08-14: app main() now configures the process-wide lemma
      fold from the bundled CC0 bank, so the in-app indexer builds
      the same text_norm as the offline v3 builder; libindex gained
      a fold-generation stamp (PRAGMA application_id) — an index
      whose norms were built without the fold full-reindexes ONCE
      on the next update, healing the 08-07..08-14 in-app gap;
      Lookup fallback walks the full analyzer chain (affix strip →
      pa/ba fold → verb-lemma fold), each step labeled (klogs →
      present stem klog, selftest-pinned). lucene-bo item COMPLETE.
      Affix rules detail:
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
- [x] **Steinert public dictionary CSVs** — REVIEW DONE 2026-08-20,
      docs/research/STEINERT_DICT_LICENSES.md: all 46 public dicts
      classified. Steinert's own README: copyright stays with the
      respective authors — no repo-level grant. Everything clean we
      already carry via better channels (84000 direct, Laine CC BY,
      DILA MVP, LC-from-ACIP, Das index by email grant); the rest is
      DO-NOT-INGEST (Hopkins/UMA, RY family, published Tib-Tib
      dictionaries…) or one of SIX named outreach candidates
      (TibTermProject/CTA, ComputerTerms, GaengWetzel, Yogacarabhumi/
      Mahoney, Nick Prior colloquial vocab, Jäschke index already in
      the queued Steinert thanks email) → folded into the Adam-gated
      email batch. No ingest work follows until a grant lands.
- [x] **bophono** — CORE REQUEST SATISFIED 2026-08-13 by our own THL Simplified Phonetics engine (139/139 battery; Overlay display mode + Convert row) — the 'THL scheme beside GMR' ask. bophono itself (Tournadre IPA etc.) stays optional/parked. Original: optional non-GMR phonetics schemes toggle
      (GMR convention stays binding). REQUESTED by Adam 2026-08-12:
      a THL-scheme option beside the new Pronunciation (GMR) display
      mode. RECON 2026-08-12: bophono = Esukhia/bophono (MIT, cloned
      to scratchpad) — emits Tournadre-MST IPA etc., NOT the THL
      romanization itself; needs per-word segmentation (pairs with
      pybo/botok — we have the botok port). REFINED PLAN: implement
      the THL Simplified Phonemic Transcription from THL's own
      published standard (Germano & Tournadre 2003, thlib.org),
      oracle = THL's own online converter sampled live for the
      diff-battery, bophono MST IPA as a phonological cross-check;
      display mode joins the existing "text as" combo beside
      Pronunciation (GMR). Port-and-prove, never approximate.
      **PORT-READY 2026-08-12**: the full standard is captured in
      docs/research/THL/language_tools.md — 30-consonant map, all
      16 rules with examples, exception classes, the 33-example
      battery, the 19-word exception list (b13 confirmed empty in
      the source; thun/thün internal contradiction flagged for
      port-time resolution). Battery-first port can start any
      session.
- [x] **THL FULL-SITE KNOWLEDGE SWEEP** — COMPLETE 2026-08-12 (4 reports + Guidelines + concordance) (Adam: "read
      the entirety of the Tibetan & Himalayan Library's website…
      everything useful to have in your back pocket" — documents,
      resources, digital-Tibetan practice, and especially their
      guidance on formatting Tibetan texts and pechas for print).
      Program: sweep thlib.org section by section (THL Toolbox =
      digitization/format/catalog standards; transliteration EWTS
      spec; THL Simplified Phonetics standard; fonts + input
      tools; catalogs incl. Kangyur/Tengyur; place/person
      gazetteers; TEI/XML markup practice; A/V + image
      collections; licensing per resource) → banked notes in
      docs/research/THL/ + synthesis docs/research/THL_KNOWLEDGE.md.
      Feeds: Pecha Maker v3 QA (their print-formatting docs), THL
      phonetics engine (item above), catalog cross-linking, future
      resource integrations (licensing lanes noted per item).
      STARTED 2026-08-12: four parallel research agents launched
      (Toolbox/standards · language tools · collections/catalogs ·
      knowledge-map/tech infrastructure).
- [x] **84000 TM: link-out only** — SHIPPED 2026-08-07 as part of the
      link-out tier: the Lookup pane and the Draft concordance carry a
      "search elsewhere (external sites, links only)" row — 84000,
      BDRC etexts, Bibliotheca Polyglotta, Lotsawa House — every URL
      format verified live before shipping; nothing ingested, so
      restrictive licenses are never touched. (The full-TM ingest
      still waits on the 84000 license email.)
- [x] **84000 glossary + translation memory** — SHIPPED 2026-08-20.
      License gate resolved from the files: 84000's canonical Terms
      table (all-data/Terms_of_Use.md rev. 2023-06-14, re-read live)
      grants CC BY for BOTH the TM and the glossary; the TM README's
      BY-NC-ND line is outdated boilerplate that itself defers to
      those Terms. (Published translations stay BY-ND — not touched.
      Courtesy/partnership email still queued for Adam, above.)
      · TM: full repo combined — 400,745 aligned Ti–En segments, 388
      texts (319 from json/ + 69 parsed from TMX) →
      data/84000/tm_84000.tsv.gz (27MB); allcore Tm84000 builds an
      FTS5 db on first use (data/extracted/); Overlay concordance now
      shows "84000 Translation Memory" comparanda beside HGM corpus
      hits, toh+folio attributed, linked to 84000.co/translation/<toh>
      (URL pattern verified live). 56th suite tm84000_smoke: build,
      cache-hit, Tibetan+English phrase search, nonsense-returns-
      nothing — all against the real shipped TSV. Selftest pin builds
      and answers sems can.
      · Glossary: official live harvest via graphql.84000.co (open,
      no auth) — 62,430 term instances across all 456 published works
      merged into g84000.json: 24,576 → 29,383 keys, +46,662
      definitions, +23,174 Sanskrit, 60,714 toh attestation links now
      rendered as "attested in" on cards. GOTCHA the selftest caught:
      the live harvest spells the wylie apostrophe U+2019; unfolded it
      would have silently duplicated 9,317 keys.
      · OPEN_SOURCE_NOTICES gained both rows (the 08-12 glossary ship
      had missed its row); manual §Overlay + Lookup-card table
      updated.
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
- [~] **Abbreviations (bskungs yig)** — FIRST TABLE SHIPPED 2026-08-07:
      the ERC TibSchol abbreviations table (CC0 1.0 verified, 136
      manuscript-attested forms; banked at data/abbreviations/ with
      license + provenance; the project's reference request honored in
      the UI label). allcore::AbbrTable (28th suite: khyadr→khyad par
      both lookup directions, stacked-vowel ku+ob, and the red-wave
      companionship — the squeezed form legally FAILS ordinary
      syllable rules, so the Overlay card now explains it as an
      abbreviation, not a typo). Overlay no-span click card shows the
      expansion (labeled, display only, never auto-expanded).
      SCOPE CLARIFIED FROM WILSON'S BOOK (App.2 re-read 2026-08-07):
      his OM dot marks the SURVIVING dot inside a SYLLABIC contraction
      (bsdus tshig: རྣམ་ཤེས for རྣམ་པར་ཤེས་པ) — a different animal
      from these orthographic squeezes (where the dot disappears).
      rKTs LICENSE VERIFIED 2026-08-14 (subagent research): site-wide
      CC BY-SA 4.0 on rkts.org/about.php ("If not indicated otherwise,
      data (other than images) provided through the rKTs website are
      placed under… CC BY-SA 4.0"); list now 6,709 entries (grown from
      3,000); NO bulk download — export must come from Bruno Lainé
      (brunogml@gmail.com; his GitHub catalog data is CC0 but does NOT
      include the abbreviations). EMAIL DRAFTED r-9190233297626946294
      (export request + license confirmation + CC0 invitation) — Adam
      sends. On arrival: separate labeled layer beside TibSchol (never
      merged — ShareAlike). (was: the rKTs 3,000+ list, license unverified —
      would grow this table) and a bsdus-tshig table for the OM dot
      proper.
- [x] **THL kmaps place lookup** — SHIPPED 2026-08-13 as a
      link-out row ("THL Places" beside 84000/BDRC/Polyglotta/
      Lotsawa; ?searchTerm= pattern verified live against their
      own searchbox). Deeper JSON integration (was: user-initiated JSON lookup
      from the Overlay for place/person names (21k+ gazetteer; no-auth
      API; bundling license unclear so lookup-only).
- [x] **Mahavyutpatti TEI (DILA)** — SHIPPED 2026-08-07: license
      verified CC BY-SA 3.0 (DILA/DDBC site statement; attribution in
      data/mahavyutpatti/README.md). Source TEI P5 zip BANKED in-repo
      (DILA's site was refusing connections — copy retrieved from the
      Internet Archive capture of the original download, SHA-1
      recorded; refresh from DILA if it returns).
      tools/extract_mahavyutpatti.py → all 9,379 entries (IAST +
      Devanāgarī + Wylie + Tibetan script + Chinese, chapter heads,
      standard MVP numbers; DDBC's own editorial additions carry a
      {ddbc} marker, never silently merged). allcore::Mvp (26th suite:
      sangs rgyas ↔ buddhaḥ both directions, case-fold, {ddbc} marker,
      exact-only honesty). In-app: Convert pane answers Sanskrit AND
      Tibetan inputs with the MVP block; Lookup matches show Sanskrit
      + MVP number — labeled reference, never HGM-tier.
- [x] **Bundle fonts** — Noto Serif Tibetan SHIPPED 2026-08-07 (OFL
      verified, variable font all weights, banked with OFL.txt in
      data/fonts/; registered at startup; the Overlay's Tibetan-script
      mode prefers it, ACIP/wylie stay in the default face, graceful
      if missing). BabelStone Tibetan Slim ADDED same day: OFL 1.1
      verified BOTH on the site and embedded in the font's own name
      table (v10.011; data/fonts/FONTS.md records provenance);
      registered at startup; the Display panel now has a typeface
      combo (Noto / BabelStone / system, persisted) — BabelStone's
      wide rare-stack + bskungs-yig coverage pairs with the
      abbreviations layer. SAMBHOTADEGE PREFERRED (Adam 2026-08-07):
      the system-installed SambhotaDege heads the combo and is the
      default when present, and the app-wide font falls back
      per-character to it so inline Tibetan in EVERY pane uses it;
      never bundled (not ours to redistribute) — OFL faces stand in
      when absent. Setting now persisted by family name
      (overlay/tibFontFamily). LATER (nice-to-have): visual pass of the
      Sanskrit-stack battery output in both faces.
- [~] **Lotsawa House** — CC BY-NC: OUTREACH DRAFTED 2026-08-13 (Gmail r918926364457655611, To: blank — Adam adds the address and sends). Ask: bulk/permission;
      prayer-register corpus for register warnings.
- [~] Link-out-only tier — Bibliotheca Polyglotta + Lotsawa House
      SHIPPED 2026-08-07 (with 84000 + BDRC etexts, above). Adarsha
      RESTORED 2026-08-14: the rebuilt site (adarshah.org) reads
      online.adarshah.org/search.html?text=<Tibetan unicode> and
      auto-runs the query — verified live in-browser, link-out
      wired back in (selftest-pinned). THL kmaps lookup BLOCKED upstream
      (probed 2026-08-07: search endpoints 500, searchterm ignored,
      SOLR index host NXDOMAIN). Remaining: WTS Munich, Dharmamitra
      (optional labeled AI aid at most), RY/Waldo/Valby (permission
      via Tsadra first).
- [ ] Red-flag list (no license / restrictive): see survey doc — never
      ship any of it without resolution.
- [ ] Survey's negative findings = our opportunities: no Tibetan
      verse-meter tool, no honorific-register data (beyond Monlam
      markings), no sum-cu-pa rule engines exist anywhere — our verse
      pane + Wilson tables lead the field; a zhe-sa honorific layer
      from Monlam data would be another first.

## ACIP Development folder finds (survey 2026-08-07 —
## docs/ACIP_DEV_FOLDER_SURVEY.md) — **ADAM APPROVED THE SIX
## 2026-08-07** ("yes, add these 6 things to the list"): per-text
## glossary · saved searches · Acipcode battery · font render suite ·
## UTFC lane · bampo/shloka layer

- [x] **Acipcode.txt → converter-completeness battery** — SHIPPED
      2026-08-07 (31st suite, acipcodes_smoke): banked at
      data/acip_codes/ with provenance; all 411 alphabetic codes
      parsed (363 core + 48 Sambhota extensions), **100.0% handled**:
      379 by the native ACIP→EWTS→Unicode chain, 32 by the Sanskrit
      converter, ZERO unhandled — the converters are provably
      complete against the official inventory.
- [x] **Per-text glossary** — SHIPPED 2026-08-07: TSV per opened text
      at library/glossaries/<basename>.tsv (human-editable); the
      Overlay click card shows "FROM THIS TEXT'S GLOSSARY:" ABOVE the
      general dictionary (clearly labeled as the translator's own);
      ＋add/edit links in every card via QInputDialog; loads on
      openFile.
- [x] **Saved searches** — SHIPPED 2026-08-07: Search pane combo +
      Save… button, named queries persisted in QSettings
      (search/saved), pick to refill the query box.
- [x] **Font render-test battery** — SHIPPED 2026-08-07 (32nd suite,
      font_smoke): Adam's 1,973-pair syllable table + fail-to-render
      list banked at data/hypercontext_legacy/; corpus = their full
      Tibetan-block codepoint set; the two bundled faces must always
      register + pass in full, each installed vetted face asserted,
      absent faces reported-skipped. (Gotcha: addApplicationFont
      needs an ABSOLUTE path under the offscreen platform.)
- [x] **UTFC legacy-font rescue** — SHIPPED 2026-08-07 (GPL v3 kept
      as an EXTERNAL PROCESS, never linked): tools/setup_utfc.sh
      compiles Converter.c (one shim: malloc.h → stdlib.h) into
      build/utfc/utfc; Library pane "Legacy font rescue (UTFC)…"
      picks source encoding from the 16 shipped tables (TMW, TM,
      LTibetan, Old/NewSambhota, Fz, Hg, …), runs the binary with
      cwd = UTFC-master (its .tbl lookups are cwd-relative,
      QSettings utfc/dir), normalizes the UTF-16LE output to UTF-8
      as my_materials/<base>.utfc.txt, labels it utfc-derived, and
      warns when no tsheg appears (wrong-encoding tell). Verified
      live: ACIP "BSOD NAMS KYI TSOGS," → བསོད་ནམས་ཀྱི་ཚོགས།
- [x] **Bampo/shloka structural layer** — SHIPPED 2026-08-07:
      allcore::extractStructure (outline.h) finds explicit BAM PO +
      numbered LE'U markers in both cases with the ordinal grammar
      the canon actually uses (dang po / N pa / bare N / bcu-teens /
      bco lnga·brgyad / decade pairs ± rtsa / nyer- and
      zhe-style contractions / brgya dang N / slar-bsdu closes);
      out-of-sequence numbers flagged irregular, unparsed ordinals
      kept raw with number 0 (rule 3). Syllable-derived shloka
      (÷30) and bampo (÷9000) figures are labeled ESTIMATES.
      Draft-pane "Structural units (bam po / le'u)" report.
      Real-canon validation on KL0009I1 (PoW 25k, Lhasa): 26
      markers, per-bampo spans ~10k syllables (definition: 9,000),
      and the source's own skipped `bam po bzhi pa` correctly
      flagged. 13 new outline_smoke checks.
- [x] **wysearch's Spotlight idea** — SHIPPED 2026-08-08: Search
      pane "Search this Mac (Spotlight)" — mdfind -literal
      kMDItemTextContent probes with the query as typed PLUS its
      Tibetan-unicode conversions (wylie chain + ACIP chain, both
      proven engines); Gofer operators stripped; 60-hit cap;
      results open in the default app. Both probe forms verified
      live against real files on this machine.
- [x] **OCRProcessing's batch-volume pattern** — SHIPPED 2026-08-08:
      Scan pane "Batch folder…" — every page image in a folder
      through the same canonical pipeline → one headered
      OCR-DERIVED -ocr.txt per page under library/ocr_out/<folder>/,
      QProgressDialog with Stop, per-page line + legality-flag
      counts, failures listed loudly, early-stop reported.
- [x] **Style-face registration** — SHIPPED 2026-08-08: the five
      inactive style-face FILES in ~/Library/Fonts (chos-rgyal,
      yig-chung, calligraphic, TCRC Youtso, Tib-US) registered
      per-session by app + font_smoke (never bundled), probed
      64/64 working-set coverage, appended to typeface combo +
      battery.
- [ ] **Sanskrit findings for the data project (indic_transliteration
      oracle, 2026-08-14)**: (a) standalone om/oṃ — consider the sacred
      ligature ॐ (U+0950) for Devanagari display (library's convention;
      mantra-authentic); (b) tisḥ-class degenerate inputs (bare
      consonant+visarga) — library inserts virama, ours doesn't; likely
      the LC entry itself is defective. 99.995% agreement over 63,667
      terms otherwise (docs/research/INDIC_TRANSLIT_ORACLE.md).

- [~] **Catalog-toolchain mining** (MakeGMRCatalog & co.) with the
      data project. DIFF-ORACLE CLAUSE LARGELY DONE 2026-08-14:
      jsewts (buda-base, Apache-2.0) run three-way over all 26,318
      ground-truth pairs — 98.84% agreement with our canonical
      engine, both ~98.3% vs ground truth (mutual corroboration);
      jsewts defect found (Latin diacritics pass through: kā→ཀā —
      ours is right); docs/research/JSEWTS_DIFF_REVIEW.md.
      NetWylie/Lingua::BO deprioritized (add little beyond jsewts;
      pyewts already mined). Remaining: the MakeGMRCatalog toolchain
      mining itself (data-project side).

## THE DICTIONARY PROGRAM (Adam, 2026-08-12 — docs/DICTIONARY_STRATEGY.md)

- [x] **84000 glossary layer** — SHIPPED 2026-08-12 (24,576 terms,
      CC-BY per 84000's own ToU table; via Steinert's public CSV
      mirror; green banner, entity links, never HGM).
- [~] **ALL Working Glossary (AI tier)** — APP-SIDE COMPLETE 2026-08-13 (deep-entry form + display + q.v. + PROMOTION LANE); scale-up credit-gated. (was: scaffolding + deep-entry
      form shipped; 45-entry pilot done. NEXT: scale batches (cost
      gate with Adam), Propose/Approval promotion lane, per-term
      suppression audit.
- [x] **Geshe Chodrak lane** — CLOSED 2026-08-13, see
      docs/research/GESHE_CHODRAK_LANE.md: d.1972 VERIFIED
      (1898–1972; dict compiled 1946, publ. 1949 Lhasa/Horkhang,
      w/ Gendun Chophel) → PD in life+50 countries since 2023; BUT
      US term restored to 2044 (URAA, 1949 publication) — not
      clearly free for a US-distributed app until 2045. Etext:
      only PKTC's commercial keying exists (Steinert's open set
      has none); lane parked on the pending PKTC outreach.
      Original line: verify d. 1972 (PD-2023 math), hunt
      the etext (BDRC/Monlam/THL), then AI-translate definitions
      ("AI-translated from a PD native definition" tier).
- [x] **Jäschke 1881 (PD)** — SHIPPED 2026-08-13: PD scan acquired (archive.org tibetanenglishdi00jsuoft, 81MB → data/das/), OUR OWN letter-section page map built from the scan itself (section-opening formulas + per-page root-letter voting; Steinert's index untouched — license stays pending), root-aware jaePageFor (prefix/superscript stripping), 'Jäschke 1881 · open the page scan' card line + jae: viewer on all three surfaces. Original: EVALUATED 2026-08-12: Steinert's
      Jaeschke_Scan = headword→page INDEX (154,112 rows), not text;
      index is Steinert's compilation (his pending license email
      covers it, same as ChandraDas_Scan). The 1881 text is PD on
      archive.org → "Jäschke at-page" layer needs the PDF (ask
      Adam before the download) + index license or our own map.
- [ ] **Das text** (etext): PKTC license answer → else DSAL Chicago terms →
      NOTE 2026-08-14: Das PAGE-SCAN lane upgraded to HEADWORD level —
      Steinert granted his ChandraDas_Scan index by email 2026-08-13
      (20,773 headwords banked w/ provenance + his '?' marks; ±page
      caveat kept; thanks drafted r-1310423263851445693, which also
      asks whether the grant extends to his Jäschke index).
      else the input-center OCR digitization project.
- [ ] **Steinert per-dictionary license verification** queue:
      Hopkins family (UMA terms), ITLR, TibTermProject, Berzin;
      RY/Waldo/Valby via Tsadra permission.
- [ ] **Negi Tib-Skt**: CIHTS licensing inquiry.
- [ ] NEVER lane: Tshigdzod Chenmo, Golden Mirror, Dungkar et al.
      without licenses; no paraphrase of copyrighted dictionaries.

## IN-APP COLLECTION UPDATES (Adam, 2026-08-12 — "strongly suggest")

- [x] **SHIPPED 2026-08-13** — Library → Maintenance → "Check for
      collection updates…": fetches the site's library page (S3
      fallback), HEADs the official ZIPs (size/date/ETag), compares
      ETag fingerprints against what this machine installed, and
      downloads+installs in-app with progress + confirm. (was:
      Check asianlegacylibrary.org for updates from inside the
      app**: a Library-pane action (and/or startup check) that
      probes the ALL website's library page for the current
      Kangyur/Tengyur/Sungbum collection releases, compares against
      the installed manifest (record installed release/version at
      install time), and reports what's new.
- [x] **SHIPPED 2026-08-13** (same dialog — per-collection
      "Download & install…" with confirm, progress, cancel;
      fingerprint recorded on success). (was: Load the .zip files
      directly from the website: download
      chosen collection ZIPs in-app (progress + resume + checksum),
      then feed the existing Install-collection pipeline — no
      browser round-trip. Needs: a probe of the site's actual
      download URL structure (and ideally a small manifest/JSON the
      website team could publish — one static file would make this
      robust forever; raise with the web team).
- [x] Same mechanism later for data releases (spine) alongside the
      team-folder Check-for-Updates that exists for the app itself —
      SHIPPED 2026-08-14: build_spine.py parameterized (--release-dir
      newest-version discovery, no-arg behavior unchanged); Library →
      Maintenance → "Import data release…" (validate, copy, run the
      canonical builder via QProcess, spine_current.txt pointer,
      fail-safe — nothing switches if the build fails);
      resolveSpinePath honors the pointer with traversal refused
      (selftest-pinned); the press ships the builder so an installed
      app can self-update.

## PECHA MAKER + TibetDoc workflows (Adam, 2026-08-12)

- [x] **Pecha Maker v1** — SHIPPED: Overlay FORMAT & EXPORT → PDF
      (traditional 42×9 / wide / A4; double-rule frames; rotated
      Tibetan folio numerals; battery-proven script chain; optional
      GMR-phonetics interlinear; selftest writes+verifies a PDF).
- [~] **PECHA MAKER → TRUE PRODUCTION TOOL** — 2026-08-13 additions: verse lineation; MEASURED woodblock preset (real-print geometry, 8 lines/side); 54-syl/line authentic type autofit (47,728 measured eKangyur lines); batch folder→PDFs. Remaining v3: dbu-khyud/double-shad conventions, eyeball QA vs prints, Sungbum-preset budget via OCR stats. (Adam, 2026-08-12:
      "super rudimentary as of now… focus on getting it ready for
      full production and deployment").
      **v2 TRANCHE SHIPPED 2026-08-12:** translation interlinear
      where corpus-attested (verbatim match from the aligned
      corpus, never composed — selftest proves it end-to-end);
      ornamental title folio (1a, text begins 1b); margin title
      (ya-yig) + volume letter in the rotated recto margin; yig-mgo
      head mark; rule-weight furniture (Fine/Classic/Bold);
      imposition two-up on A4/US Letter with cut marks + side
      labels; all options persisted (QSettings pecha/*). Internals:
      QPicture-per-side recording (NB: QPicture replay rescales by
      device-DPI ratio — record at the picture's own logicalDpi).
      **v3 progress:** batch mode SHIPPED 2026-08-12; Degé preset
      + classical 5-line opening SHIPPED (THL specs); cover/label
      sheet SHIPPED (office layouts; title/volume/count/date).
      verse lineation SHIPPED 2026-08-13 (source lines → pecha
      lines, blank = stanza gap; selftest + visual check).
      **v3 remaining:** per-side syllable budget tuned against real
      woodblock prints; head-letter (dbu khyud) and double-shad
      conventions; margin presets; QA pass comparing against
      traditional prints.
- [~] **PRINT PARTNERSHIP — Vimala Treasures** (Adam, 2026-08-12):
      OUTREACH DRAFTED 2026-08-13 (Gmail r1053591277282639853,
      To: blank — Adam adds the address and sends; pecha tool has
      matured: v2 + batch + verse lineation).
      partner/work with vimalatreasures.org (they print +
      distribute BDRC texts:
      vimalatreasures.org/collections/the-buddhist-digital-resource-
      center-bdrc) so texts/pechas produced by the tool can be
      PRINTED properly on request — the Pecha Maker's output as
      their input. Outreach draft when the pecha tool matures.
- [x] TibetDoc search-locations view — SHIPPED 2026-08-13 (per-file rollup + goferopen open-at-line): per-FILE hit counts across
      chosen locations + open-at-first-hit (Gofer extension).
- [x] In-document lookup popup — SHIPPED 2026-08-13 (⌘D LookupPopup, app-wide selection → floating entryHtml card, follows lookups). Original: In-document lookup popup (TibetDoc's dictionary window):
      select a word in Manuscript/Draft/Input → floating card
      (entryHtml) + ⌘D shortcut; the popup follows lookups.

## Build queue

- [x] **White paper v5** — SHIPPED 2026-08-12 (docx in Desktop distribution) (was: v4 predates most of 2026-08-07: the three
      catalog/subject corpus waves (42,184 segments), the calendar
      engine (year + day level, 76/76 oracle-proven, Convert-pane
      dates both directions), the affix program (Lookup fallback +
      affix-tolerant search everywhere), Botok segmentation in the
      Overlay, five reference layers, bundled font, list view,
      pending-review sheet. Refresh both .md and .docx when the dust
      settles (coordinate: phone session authored v4).
- [x] **RELEASE 6 RETIRED (Adam, 2026-08-12)**: "we don't need to
      rely on the ACIP release 6 files at all — the Kangyur, Tengyur,
      and Sungbum .zip files (manually installed) are the latest
      public releases of our database." library/acip_release6 moved
      to Trash; dev library index rebuilt (8,986 files, 14.08M
      lines); docs truth-passed. The harvest HISTORY below stands
      (title pairs, author trees, subject browser — all ingested to
      the corpus before retirement).
- [x] **Release 6 zip — unexplored assets** (Adam's Dropbox,
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

- [x] **Wilson full-designation program** — ALL TWELVE DOT DESIGNATIONS IMPLEMENTED (closed 2026-08-14; remaining clauses are externally gated: rKTs register growth awaits Lainé's answer r-9190233297626946294; letter-level fusions = research item). (the "complete access on every
      grammatical level" goal). DONE so far (2026-08-06): per-unit parse w/
      contextual eight-case numbers; negation particles; infinitive-marker +
      auxiliary verb phrases; postposition constructions; ambiguity guards
      (shes/de licensing); the Science of the Dots (S · C n · NOM · SP · V ·
      VB per tsheg — reproduces Wilson's own diagrams; Trainer layer ⑥).
      NA/NN/APP: the real POS source ARRIVED 2026-08-07 — the SOAS
      "Tibetan in Digital Communication" hand-tagged Classical Tibetan
      lexicon (CC BY 4.0, Zenodo 574876; banked at data/soas_pos/ with
      attribution). allcore::PosLexicon (27th suite; with/without-tsheg
      rows unioned so ambiguity is never hidden — 13,107 forms) wired
      into wilsonParse as an optional evidence layer: an UNAMBIGUOUS
      adj tag on the second nominal resolves "NA (adjective — SOAS
      lexicon)"; two unambiguous nouns narrow to "NN|APP (apposition
      undecidable)"; anything ambiguous stays undetermined, and
      without the lexicon the dot reads exactly as before (battery
      asserts both behaviors). VOC shipped earlier (interjection list).
      ADV shipped same day: an UNAMBIGUOUS SOAS adv.* tag on the
      pre-verb unit resolves "ADV (adverb — SOAS lexicon)" (battery:
      kha sang | bstan; 164 adverbs across 5 subtags).
      UP SHIPPED same day (attested-only, per Wilson p.573 verbatim —
      "the dot where the case-marking particle would normally have
      been seen"; his preferred label = the case number): between two
      bare nominals, the genitive family (kyi/gyi/gi/yi/'i) + dang are
      probed against OUR OWN dictionary entries and corpus; an
      attested uncontracted form resolves "UP (6 — understood kyi;
      attested uncontracted: …)" naming the evidence (battery: tshul
      khrims | phung po via 4 corpus hits of tshul khrims kyi phung
      po). Evidence order at the pair dot: NA(SOAS adj) > UP(attested)
      > NN|APP(SOAS nouns) > undetermined.
      OM SHIPPED 2026-08-07 (Adam's request: "a list or register to
      hold abbreviations as would be denoted by the OM dots"): the
      bsdus-tshig register is DERIVED FROM THE HGM DICTIONARY ITSELF —
      attestation only: 182 pairs where BOTH forms are glossed
      entries, the contraction follows Wilson's drop pattern with a
      required MID drop (the omission the OM dot stands for — a
      tail-only drop like ma yin/ma yin pa is excluded by rule), and
      the glosses agree; corpus counts ride as evidence
      (tools/derive_bsdus_tshig.py → data/extracted/bsdus_tshig.tsv,
      regenerate per release; human review doc
      docs/BSDUS_TSHIG_REGISTER.md — DERIVED tier, top ranks are the
      textbook set: stong nyid, bla med, theg chen, rnam shes,
      rten 'brel, 'jam dbyangs, grub mtha'). allcore::Contractions
      (29th suite) + wilsonParse: a registered 2-syllable unit's
      internal dot reads "OM (contraction of rnam par shes pa —
      derived register)" — the battery reproduces Wilson's own p.570
      OM illustration (rnam shes mi rtag pa yin) and asserts the
      honest S default without the register. Overlay span card shows
      the expansion under the grammar toggle.
      **ALL TWELVE DOT DESIGNATIONS NOW IMPLEMENTED** (S · OM · NA ·
      NN · APP · VB · V · ADV · NOM · VOC · C-with-number · UP · SP).
      LATER: grow the register from rKTs' 3,000+ list when its host
      answers and its license verifies; letter-level fusions (sher
      phyin ← shes rab kyi pha rol tu phyin pa) need squeeze rules
      beyond the drop pattern.

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
- [x] **Pedagogy roadmap** (docs/PEDAGOGY_ROADMAP.md — Adam's translator-
      training question, 2026-08-06). Build order: ① corpus drill generator
      (cloze, chunk-ordering, parallel reading — HGM answers only),
      ② local progress/SRS layer, ③ draft workspace + terminology
      consistency + concordance, ④ sa bcad outline extractor (SHIPPED 2026-08-14: enumeration-grammar parser + clickable tree dialog, selftested), ⑤ verse
      meter (SHIPPED 2026-08-14: block detection + deviation flags, selftested), ⑥ miss taxonomy (SHIPPED 2026-08-14 — 'My weak spots…' report in Drills), ⑦ AI back-translation QC (credit-gated).
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
- [x] Live composing editor — BOTH DIRECTIONS LIVE 2026-08-20. The
      typing half already existed (Convert renders as you type); the
      missing half was the REVERSE ENGINE: allcore unicodeToWylie,
      ported FROM THE FILE (pyewts.py::toWylie, OpenPecha master,
      Apache-2.0 — the same library the forward engine was
      cross-validated against; tables AUTO-GENERATED from the
      module's own runtime maps, tools/gen_unicode_wylie_tables.py).
      Suite 59 towylie_battery: 100.000% match on ALL 109,490
      fixtures (master ground-truth Tibetan + strided native 84000
      unicode; build/towylie_reference.tsv) + round-trip 97.3%
      (floor 96.5% — misses are FORWARD-engine boundaries: EWTS '_'
      explicit space, explicit '+' Sanskrit stacks; documented in
      the battery). Convert pane: paste/edit Tibetan SCRIPT → live
      wylie/ACIP/phonetics card, warns surfaced, escapes never
      guessed. Selftest pin round-trips through both engines.

- [x] **Library pane v1** (Adam's request 2026-08-06): install collection
      ZIPs from asianlegacylibrary.org/library, import personal materials
      (.docx→.txt auto-conversion), sortable tree browser with catalog
      provenance + preview + open-in-Overlay. *Shipped 2026-08-06.*
- [x] **Tibetan OCR for pecha scans** — STAGES 0-2 COMPLETE (see below) + 2026-08-13 additions: in-Input detect/prefill/locate-selection, Overlay word-locate, Four-Layer OCR column. RESIDUE: the other 4 tibetan-ocr-app script-style models (Uchan etc.) when needed; BDRC new OCR release ~2 wks triggers refresh. Original: (docs/OCR_DESIGN.md; Adam 2026-08-06,
      "eventually"): base = BDRC's open-source tibetan-ocr-app models
      (5 script styles incl. Woodblock). Stage 0 = document the manual
      BDRC-app → Import workflow; Stage 1 COMPLETE 2026-08-06 (Library
      "Send to OCR" hand-off; ocr_out live-watched by the tree; files
      there tagged ocr-derived in the info panel; opening one banners it
      and runs the syllable-legality first-pass QC with a fail count);
      Stage 2 COMPLETE 2026-08-07 (permission recorded; increments A-D:
      line detection oracle-proven, line building exact via OpenCV,
      CTC recognition = faithful pyctcdecode-beam port, Scan pane w/
      per-line QC + ocr_out hand-off + labeled deskew-override
      deviation; upstream deskew bug documented for BDRC). OCR output always tagged,
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
- [x] BDRC linking — LARGELY SHIPPED by 2026-08-13: templated-query fallback = WorkVersionPerPerson lane; Sungbum title-based candidates = 520 verified links + title-search dialog (user-confirmed); in-app IIIF viewer = the BUDA-style scan viewer + Four-Layer Page. RESIDUE: KX/TS editions still lack any lane. Original: BDRC linking, remaining: templated-query/SPARQL fallback for
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

## Registers (Adam's requests, 2026-08-08)

- [x] **Colloquial pronunciation register** — STARTED 2026-08-08
      (data/pron_colloquial/, 35th suite): 58 community spellings
      (gonpa/tulku/chenrezig/chorten…), all spine-verified; Lookup
      fallback accepts them, labeled, GMR stays canonical.
- [x] **Prenasal rule class** — HGM (via Adam): SKABS 'DIR = "kamdir";
      -b(s) nasalizes to m before 'a-chung syllables. Engine covers
      vowel-final (gendun/kangyur) but not -b(s); master itself attests
      kyamdro. 164 forms DERIVED with juncture-count guard (2 ambiguous
      skipped; 'b-cluster junctures left for review) — PENDING HGM/Adam
      REVIEW; engine untouched.
- [x] **Honorific (zhe sa) register** — STARTED 2026-08-08
      (data/honorifics/, 27 spine-verified pairs, level column with
      "high" available). HGM glosses mark honorifics in only ONE entry
      — levels need human marking. UI badge + oversight warnings =
      next increments.
- [ ] **INCOMING (Adam, 2026-08-08): a series of books/PDFs on the
      honorific + high-honorific registers and numerous Tibetan
      grammar books** — when supplied: mine for register rows
      (attestation-tier per source), mark high-honorific levels,
      and consider a grammar-references layer alongside Wilson.

## GMR RECORDINGS ARCHIVE (Adam, 2026-08-12)
- [x] **Master recordings inventory** — COMPLETE 2026-08-13: 4,010 recordings tracked w/ language+duration census + transcription cost tiers (Summary sheet). Preservation pass remains its own item. Original: every known recording of
      Geshe Michael Roach, audio + video: YouTube, The Knowledge
      Base, personal collections. STARTED 2026-08-12:
      "~/Documents/GMR Recordings Tracker.xlsx" seeded with 3,633
      recordings (2,901 The Knowledge Base + 732 Diamond Cutter
      Classics, from our caption-harvest metadata; 2,440 with
      captions captured), + "Leads and Unpublished" sheet +
      self-updating Summary. Next: Jamie Plant server inventory
      (below); personal-collection drive (pair with the GMR
      Translations drop folder); possible extra seeds in Adam's
      own ~/Documents YouTube-links spreadsheets (Fall 2022 /
      Winter 2021 DCC link sheets spotted 2026-08-12).
- [ ] **Jamie Plant / Knowledge Base server access** — ask for
      (read-only) server access to document unpublished/unreleased
      holdings ("things he has that no one else has"). Draft ready
      (Gmail draft r-8606637541006616502, recipient BLANK — Adam
      must confirm the address; best guess jamie@
      diamondcutterinstitute.com from GMR's publishing list);
      calendar reminder set 2026-08-13 09:00 Denver. On access:
      inventory → Leads sheet → All Recordings rows.

## OPENED 2026-08-12 EVENING (autonomous block bookkeeping)
- [x] **THL Pronunciation engine** — SHIPPED 2026-08-13 (139/139 battery, display mode, Convert row, credited): C++ port
      of THL Simplified Phonetics (spec captured complete), battery
      = the standard's own 33 examples + 19 exceptions + rule
      examples as ctest; display mode joins "text as" combo; UI
      credits "THL Simplified Phonetics (Germano & Tournadre, THL)".
      Policy decision documented at port: thun→tün (rules 2+3 over
      the source's own contradictory example).
- [x] **THL Degé concordance** — COMPLETE 2026-08-13: all 1,118
      records harvested (3 passes vs their throttle) + 1,118 BDRC
      Toh labels → data/extracted/thl_dege_concordance.json: 919
      verified mappings (543 exact / 374 guarded-containment / 2
      particle-fold), 33 ambiguous + 166 unmatched listed honestly
      (THL-only insertions incl. D.8/D.361/D.543), ZERO order
      inversions, offsets reproduce the live verification.
      NEXT (new item): wire THL catalog deep-links through the
      concordance (Toh/KD-numbered contexts: composer, citations,
      BDRC-linked Derge works — never numerically). NOTE
      2026-08-13: ACIP KL vs THL Lhasa (kt/h) numbering spot-check
      REFUTED at H.11 (8k vs 10k Lines) — the KL-numbered library
      files need their own title-matched Lhasa concordance first
      (same recipe as the Degé one) before any THL link touches
      them.
- [ ] **Transcriber pilot** (needs Adam's go + account): <$20 on
      ~10 Tibetan-dense videos, ElevenLabs Scribe v2 vs AssemblyAI
      (± Soniox/Speechmatics), scored on Tibetan-term recall;
      keyterms + phonetic→wylie map READY in data/teaching/
      (transcriber_keyterms.txt / transcriber_lexicon.tsv). Then
      full-archive run (~$840-1,080 at 4,000 hrs) + teaching-index
      rebuild from the better transcripts.
- [x] **THL outreach email** — DRAFTED (r2995543576256424105; Adam adds address + sends) (was: draft tonight if time; else next
      session): dictionary-content licensing for the 16 TTT
      dictionaries (the one documenting wiki page was never
      archived), the possibly-unintentional public Solr endpoint,
      data-partnership registration, and credit lines. Address:
      THL/UVa contact page.
- [~] **kmaps Places live-query integration** — link-out HALF
      SHIPPED 2026-08-13 (cards' search-elsewhere row). JSON half
      PROBED 2026-08-13: features.json?searchtext= is a TREE
      endpoint (returns root "Earth", ignores search params — all
      variants tried); real search runs through THL's Solr index,
      which we flagged to them as possibly-unintentional exposure —
      NOT building on it before they answer the outreach email.
      GATED ON: THL's reply. (query + link-out
      lane only, no bulk ingest; harsh 429s — cache per lookup):
      Overlay/Library place names → places.kmaps.virginia.edu
      JSON (~64k places, Tibetan script + Wylie names).
- [ ] **Unlisted-recordings preservation pass** (68 unlisted links
      now in the Recordings Tracker): confirm with Adam whether to
      archive captions/audio for these before they rot — unlisted
      videos vanish silently when channels reorganize.
- [x] Remaining design-audit shots — ALL 14 AUDITED + fixes applied 2026-08-12/13 (docs/design/AUDIT_FINDINGS_2026-08-12.md):
      Library, Scans, Manuscript, Review, Search, Lookup,
      Sanskrit, Convert, Analysis, Drills, Input, OCR, Propose,
      Approval (first pass found: clipped banners ALL FIXED,
      empty-state gaps FIXED for Draft/Trainer/Apparatus/Export).
- PENDING ADAM (standing): Jamie Plant draft recipient + send;
      PKTC / GMR-permission / 84000 / Jigme drafts (send);
      Dropbox share link → Jigme draft placeholder; transcriber
      pilot go; joint T/K/F walkthrough; kamdir/prenasal Approval
      queue rulings; Jäschke 1881 archive.org download go-ahead.
- [ ] **GLOSSARY BLOCKED — API credits exhausted** (2026-08-12
      night): batch 5 400'd on every call ("credit balance too
      low"). Adam: top up at console.anthropic.com → Plans &
      Billing, then rerun `python3 tools/build_ai_glossary.py
      --limit 1800` (resume-safe; BATCH=3 + max_tokens 8000 fix in
      place — verify 8000 is accepted once credits return).
      Current total: 1,395 entries.
- [ ] **Hugging Face MT lane** (Adam's question 2026-08-13; verdict:
      yes, narrowly, guarded): local batch scripts in tools/ (not
      in-app) using Buddhist-MT models (evaluate MITRA/Dharmamitra
      first; check model licenses). HARD GUARD required before any
      output is kept: n-gram screen against the FULL HGM corpus
      (quotation-detector machinery) — any overlap with Geshe
      Michael's published English is flagged/discarded, because MT
      weights may have memorized his translations (the wall,
      enforced in code for arbitrary models). Uses, ranked:
      (1) Geshe Chodrak PD definition translation lane,
      (2) divergence-flagging cross-check on Working Glossary
      drafts (evidence metadata only, never merged),
      (3) Align-pane segment-pair candidates (PENDING tier).
      SURVEY DONE 2026-08-13 (docs/research/HF_TIBETAN_MT.md):
      pilot = buddhist-nlp/mitra-qwen35-it (Apache 2.0, Wylie
      input, ~18GB BF16 on MPS, needs 32GB+ unified memory);
      cross-check juror = madlad400-3b-mt (Apache 2.0, CT2 int8);
      embedder = mitra-qwen35-2b/8b (alignment substrate, cannot
      emit prose). CONTAMINATION CONFIRMED IN PRINCIPLE: MITRA's
      paper admits rights-encumbered translations in its MT
      pipeline (2M undisclosed monlam.ai pairs) and ACIP Tibetan
      in pretraining — the mt_wall_screen guard is MANDATORY (and
      already proven on our own 1,395 drafts: 0 reproductions).
      MLotsawa = fully auditable data but chrF ~20, too weak.
      Watchlist: Banzhida 7B (ACL 2026), bdrc-mitra-ocr-qwen
      (OCR bake-off candidate). Mac has 64GB unified memory —
      the MITRA pilot is FEASIBLE here (18GB weights ≈ ~9GB
      headroom-friendly at int8); the ~18GB download awaits
      Adam's go (and the Chodrak etext, its real workload).
- [x] Meter reader phase 2 — SHIPPED 2026-08-14 same day: mgur even-count grouping (floor 6, best>=4 — 4-count runs are DANG-list noise, excluded by rule), per-block on-meter %, AND the full canon meter census (tools/meter_census.py → data/extracted/meter_census.json: 333,802 verse lines over all 3 collections; 7-syl 61.1%, 5-syl 15.4%, 9-syl 8.5%, 6-syl mgur 6.2%, 13-syl kavya 3.4%) wired into the reader headings ('· N% OF ALL CANON VERSE'). Possible later: per-collection shares in-app; census by century/author if metadata ever supports it.

## SOFTWARE ENGINEERING DISCIPLINE AUDIT (Adam, 2026-08-23: "are
## there other aspects like SQA I should be aware of… complete
## rundown… put your findings onto the todo list")

Scope note: this audit covers the disciplines AROUND quality
assurance. QA itself is in good shape — docs/SQA_ASSESSMENT_2026-08-22.md
carries 22+ measured findings with severity, effort and verification
columns, and the remediation wave is running. Nothing below duplicates
it; each item was checked against the repo on 2026-08-23 and each is
either absent from the SQA doc entirely or is a lifecycle concern the
SQA doc explicitly scopes out.

### P0 — DATA LOSS RISK, act before any further feature work

- [x] **THE PROJECT HAS NEVER BEEN BACKED UP OFF THIS LAPTOP.** CLOSED 2026-08-23/24: `git clone --mirror` to /Volumes/Oct2024(8TB)/ALL-translation-tool.git, verified by comparing HEADs and a full `git fsck` (exit captured directly, not through a pipe). A `backup` remote makes it `git push backup main`. Scheduled task `all-nightly-backup` runs it at 22:00 daily and reports which destinations were reachable. STILL OPEN as a separate decision: a copy on a drive beside the laptop is not disaster recovery - the durable answer (private remote, or an ALL-owned host) is the P1 'source hosting' question Joel was asked. (was:
      Measured 2026-08-23: `git remote -v` is EMPTY — there is no
      origin, no fork, no mirror. Time Machine IS configured to an
      external volume ("My Book") and the repo IS included, but the
      destination fails to mount and its last reference snapshot date
      is **2026-05-21** — eleven weeks BEFORE the project's initial
      commit (2026-08-06). The hourly APFS snapshots that do exist
      (09:44…14:45 today) are LOCAL — same physical SSD, no protection
      against drive failure, and macOS purges them under disk
      pressure, which this machine is already under (100% capacity,
      14 GiB free). Exposure: 19 GB working tree + 10 GB `.git`, 17
      days of work, ~72,000 lines of C++, the entire institutional
      deliverable. One SSD failure ends the project.
      Remedy available NOW: `Oct2024(8TB)` is mounted with 1.1 TiB
      free. Immediate: `git clone --mirror` onto it, today. Then
      decide the durable answer — private remote (GitHub/GitLab) vs
      an ALL-owned host — because a second copy on a desk in the same
      room is not disaster recovery.

- [x] **Disk pressure is itself a correctness risk, and it nearly bit today.** CLOSED 2026-08-24. Three parts: the ~32 GB of fan-out scratch was reclaimed; the git history cleanup took .git from 10 GB to 185 MB; and the recurrence is now guarded rather than trusted - tools/scratch_gc.py reports stale session scratch and reclaims it on request, with two rules pinned by scratch_gc_selftest and mutation-verified: it NEVER touches the running session, and NEVER touches anything modified inside the idle window. The nightly health task runs the report, leads with DISK CRITICAL below 5 GiB, and may reclaim only below 10 GiB; the nightly repair task refuses to start below 5 GiB. Free disk 326 MiB -> 14 GiB. (was: 2026-08-24: the volume fell from 14 GiB free to **327 MiB** during one working day. The cause was not the project - it was 32 GB of Claude workflow scratch (a 15-agent re-assessment and six remediation tracks, each copying the tree and building it) that nothing ever cleans up. Reclaimed to 12 GiB by hand. THE FIX IS NOT 'delete it again': a fan-out must clean its own scratch, and the nightly health task should fail loudly below ~5 GiB. Until then every long agent session walks the app back toward the ENOSPC regime where FAIL-1's family of bugs live. (was: Data volume is at 100% capacity / 14 GiB free.
      FAIL-1 in the SQA doc is `catalogRosterSave()` returning TRUE
      after writing zero bytes — the exact failure ENOSPC produces.
      The machine is currently sitting in the state that makes the
      app's worst data-integrity bug fire. Reclaimable immediately:
      3.5 GB of dead-session scratch under
      /private/tmp/claude-501/…/5588dc0d-…/scratchpad, plus 2.0 GB in
      `dist/`. Longer term, `library/` (2.9 GB) and the 10 GB `.git`
      want a deliberate home.

- [x] **Guard the disk automatically, since the 2026-08-24 near-miss was
      self-inflicted.** DONE same day: the nightly health task now checks
      free space FIRST and leads with **DISK CRITICAL** below 5 GiB,
      reporting what /private/tmp/claude-501 holds without deleting it;
      the nightly repair task REFUSES to start below 5 GiB rather than
      building into a full volume. Both say why in the terms that
      matter here — under ENOSPC the app is in the exact regime
      FAIL-1's family of "reported success over a lost write" defects
      live in, so working low on disk tests the product in its worst
      state while risking the machine. STILL OPEN: a fan-out does not
      yet clean its own scratch; that is a discipline the workflow
      scripts have to carry, and today's 32 GB proves asking nicely is
      not enough.

### P1 — needed before the app leaves this machine

- [ ] **No LICENCE for the project's OWN code.** `ls LICENSE* COPYING*`
      → no matches. ~32 upstream projects ship inside the bundle
      (SQA BUILD-2) and there is no statement of what ALL is releasing
      its own work under. Until that exists, nobody downstream — input
      centres, collaborators, the director — can lawfully know what
      they may do with it. Needs an ALL decision, not an engineering
      one; prompt Adam to get it from ALL.

- [ ] **No SBOM (software bill of materials).** BUILD-2 fixes the
      human-readable notices; an SBOM is the machine-readable artifact
      institutions increasingly require (CycloneDX or SPDX, generated
      at press time). It is also what makes the NEXT item possible.

- [ ] **No dependency vulnerability scanning.** Nothing watches the
      ~32 bundled upstreams for CVEs. A Tibetan translation tool is
      not a high-value target, but it parses untrusted files (OCR
      input, imported texts, .dic/.ndx payloads) with C++ — that is
      the classic memory-safety attack surface, and MEM-2 already
      records that no sanitizer build is wired into any gate.

- [ ] **No threat model, and the roster holds credential material.**
      SQA FAIL-1 notes the roster carries "salt+hash per member" —
      that is authentication data for the in-house identity list.
      There is no document stating what the app protects, from whom,
      or what happens on compromise. Small scope, but it should be
      written down once rather than assumed.

- [ ] **No privacy/data-handling statement.** The app will run at
      input centres with multiple named users, storing who did what.
      What is retained, where, for how long, and who may see it — none
      of that is recorded. Needed before institutional deployment,
      and cheap to write now.

- [ ] **No crash reporting or diagnostics channel.** When the app
      fails at an input centre in Kathmandu, nobody here learns of it.
      No crash handler, no log bundle the user can send, no version
      string in a report. Consider a "Save diagnostic report" action
      that writes a redacted bundle the user mails in — deliberately
      NOT telemetry, which would be wrong for this project.

- [ ] **No update channel.** There is no mechanism by which an input
      centre running 1.0.0 learns that 1.0.1 exists, let alone gets
      it. Decide now: manual DMG re-download (document it), or an
      in-app updater (Sparkle is the macOS standard, and its signing
      story interacts with the BUILD-3/11 notarisation work already
      queued — resolve them together, not separately).

- [ ] **No defect intake path for users.** Translators and input-centre
      staff have no way to report a bug that reaches this repo. An
      email alias plus a short template would do; the point is that
      one exists and is printed in the app's About box.

### P2 — sustainability of the work itself

- [ ] **Bus factor is one.** The project is one developer plus AI
      sessions. There is no onboarding document that would let a
      second engineer build, test and press this app from a clean
      machine — and BUILD-7 records that 37 of 72 suites cannot even
      run from a clean checkout. For an institutional deliverable that
      ALL will depend on, this is the highest long-term risk after
      backup. Deliverable: a DEVELOPER_ONBOARDING.md proven by
      actually following it on a clean checkout.

- [ ] **`app/main.cpp` is 38,007 lines — 53% of the entire 72,176-line
      codebase in one file.** This is why patches keep missing their
      anchors, why two remediation tracks have to be warned not to
      collide in it, and why review is hard. Not a rewrite: a staged
      extraction of self-contained panes into their own translation
      units, one per press, each proven by the existing battery. Book
      it as ongoing maintenance, not a project.

- [ ] **No CI.** Confirmed: no `.github/workflows`, no `.gitlab-ci.yml`,
      no `.circleci`. SQA BUILD-6 records "the release is a snapshot of
      one laptop's Homebrew". The REPRO track is instructed to record
      what CI would take rather than build it — that record should
      become a decision, because CI is also what would have caught
      BUILD-7's clean-checkout failures.

- [ ] **No architecture decision log.** CLAUDE.md carries the big
      as-built calls (C++20 + Qt 6 over Swift, 2026-08-06) but there is
      no running ADR record of why later choices were made. Cheap
      insurance against re-litigating settled decisions, and the first
      thing a second engineer reads.

- [ ] **No data-migration / schema-versioning plan.** The spine is
      v27.2 and the app "should IMPORT a release, never own the data"
      — but what happens to a user's local work, saved searches,
      glossaries and roster when v28 lands? Write the upgrade path
      before there are users with data to lose.

- [ ] **Accessibility conformance is unproven.** The design wing
      includes an accessibility-audit skill and CLAUDE.md says new
      surfaces get a heuristic pass, but no WCAG-referenced audit
      result is recorded in docs/FINDINGS.md for the shipped panes.
      Run one and record the dispositions — Tibetan script rendering
      at small sizes and the reserved-green colour semantics both
      deserve a contrast check specifically.

- [ ] **No user-facing documentation set distinct from dev docs.**
      docs/ is rich but written for the builder. An input-centre
      operator needs a task-oriented manual. DEMO_TUESDAY.md is a
      script, not a manual.

- [ ] **No risk register.** Every item above is a risk someone should
      own with a review date. A one-page register beats rediscovering
      them in the next audit.

### Process note — mid-turn message protocol (adhered to, 2026-08-23)

Recorded at Adam's request, so the audit trail states the working
protocol as well as the findings.

**The protocol.** Claude Code surfaces messages the user sends mid-turn
*within the running turn* — arriving alongside the next tool result
rather than as a separate conversation turn. The requirement is that
such a message is addressed as the turn continues, not deferred to the
next exchange and not silently absorbed.

**Adherence in this session — two instances, both honoured:**

1. **Mid-turn message during session reconstruction.** While reading
   the dead session's transcript to determine where work had stopped,
   Adam sent the original `/loop` command verbatim ("keep going use
   everything you can and all agentic teams…"). It arrived alongside a
   tool result. It was addressed inside that same turn: the command was
   treated as the standing authorisation for multi-agent orchestration,
   the six-track fan-out was relaunched from the recovered script, and
   the loop was re-armed — all before the turn closed. It was not
   deferred, and it was not answered with a fresh question.

2. **Mid-turn message during the backup/reclaim work.** While the
   memory files were being checked, Adam sent this documentation
   request. It arrived alongside a tool result and is being actioned
   within that same running turn — this note is the product of it.

**Why it is recorded here.** The project's standing habit is evidence
before assertion. A claim that a protocol was followed is itself an
assertion, so the two instances above are stated concretely — what
arrived, when, and what was done inside the same turn — rather than as
a bare "protocol followed".

**Scope, stated honestly.** This is a harness message-delivery
protocol governing how Claude handles user input arriving mid-turn. It
is not a software-engineering standard and does not belong in the
discipline table above; it is recorded as a working-process note so the
audit trail is complete on both axes — what was found, and how the work
was conducted while finding it.
