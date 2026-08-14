# LOOSE ENDS — full-project audit & attack board (2026-08-14)

*Four parallel audits: TODO.md line-by-line (1,218 lines), all 82
docs/help markdown files, the full code tree, and the session-memory
log's every forward promise checked against later completions.
Adam's directive: "find all of the loose ends that have built up
since the beginning of this project and let's start attacking."*

**Already struck during the audit itself:** About-pane stale
version + missing release date (root cause: VERSION wasn't a CMake
configure dependency) · THL phonetics gluing phrases into one word
(now Botok-word-grouped with syllable fallback — Adam's live
finding) · stale "see TODO" pointer in the eKangyur dialog (recipe
inlined) · quotation_smoke silent skip (now fails loudly) · two
checkbox-state mismatches (Pedagogy roadmap, Release-6 harvest) ·
jsewts third diff-oracle run (98.84% mutual corroboration; their
defect, not ours) · version bumped 0.12.0.

## A. ATTACK QUEUE — solo-doable, ranked

**0. THE DIGITALTIBETAN INTEGRATION PLAN (Adam 2026-08-14: "drive
   it up to the top of the list") —
   docs/design/DIGITALTIBETAN_INTEGRATION_PLAN.md. Shipped: P1
   typography engine; fonts P2 largely in (BabelStone bundled).
   Work top-down: P5 StarDict reader (SHIPPED) → P3 Espel oracles (SHIPPED — found+fixed our monosyllable-ba defect) → P6 booklet/3-per-A4 (SHIPPED) → P8 TISE keys (SHIPPED) → P4 py-tiblegenc (SHIPPED) → W3C full-doc pass on the
   typography rules.**

1. **Analysis item 14 suppressed** (`core/src/analysis.cpp:166`):
   the prompt still tells the model the library is NOT indexed —
   but LibraryIndex shipped. Route item 14 through
   `LibraryIndex::search()` hits; keep the refusal only as the
   no-index fallback. *A capability we built and then hid.*
2. **Design-audit fixes** — CLOSED 2026-08-14: re-verified by fresh screenshots; nearly all had landed in post-audit rounds (stale notes); the one real remainder (Propose field widths) fixed. (was: never applied)
   (docs/design/AUDIT_FINDINGS_2026-08-12.md): Input-pane toolbar
   clip (6 garbled labels) · empty-state teaching HTML on ~8 blank
   surfaces · Gofer pane face · missing banners (Lookup, Convert,
   Analysis) · Library dead parchment · Manuscript placeholder
   clip + B/I/U · Propose form widths.
3. **Trainer particle tables**: zhing/shing and ces/zhes/shes
   rows missing (TRAINER_DESIGN.md:43).
4. **Pronunciation battery H**: the 60 ACIP-standard pairs queued
   as a C++ ctest (ACIP_PRONUNCIATION_STANDARD.md:66).
5. **Anusvara keying sweep**: corpus grep for OM/H'UM syllables
   keyed with final ma instead of ཾ → review-candidate list.
6. **lucene-bo app-side remainder**: updateIndex lemma path +
   Lookup affix-fold alignment (TODO L451).
7. **Approved-register regeneration**: fold approved Proposal rows
   into canonical register TSVs at release time
   (PROPOSAL_APPROVAL_SYSTEM.md:78) — the one un-struck v2 item.
8. **Spine/data-release updater**: mirror the collection-updates
   dialog for release packages (TODO L790).
9. **Banked-corpus mining lanes** (data local, never mined):
   Goldstein glossary markings · Hackett verb classes ·
   Manual-of-Authentic-Tibetan modern-colloquial mode.
10. **Adarsha link-out recheck** (their new site broke old URLs).
11. **Small strikes**: Whitney last-four fields (dcs_freq/rank,
    warnemyr, apte_id) · Trainer honorific vocab tag · Input-pane
    partner auto-pairing + UTFC lane hookup · Spotlight async ·
    QTextCursor selftest warning · empty-input loadDoc guidance ·
    nesting-tones shading swap · per-text apparatus books (16) ·
    OCR async + extra script models · KX/TS scan lane · pecha
    margin presets · outline/verse wylie-parity probe · Botok
    overlap audit · citation side-pane streamline.
12. **Wow-list leftovers** (ideas Adam liked, never built):
    pronunciation trainer (whisper ready) · Text DNA strips ·
    team board · apprenticeship mode · English-on-the-carving
    hover · teaching-playlist karaoke · ⌘K persons/notes lanes.

## B. ADAM'S ONE-SITTING QUEUE (everything below is his, consolidated)

**Sends (drafts finished in Gmail):** Jamie Plant (#1 —
irreplaceable recordings; confirm address) · 84000 license +
partnership · THL (16 dictionaries + Solr + kmaps) · rKTs/Lainé ·
Lotsawa House · Vimala · PKTC · Jigme (needs his Dropbox link) ·
Joel zoom · Élie reply · Steinert thanks (Jäschke ask) · GMR
transcription appeal + GMR trust dialog (both need GMR's address) ·
fundraising.
**Approvals/money:** git filter-repo (~20GB; disk at 3.4GB free —
operational risk) · API credits (glossary stalled at 1,395) ·
ElevenLabs pilot <$20 · MITRA 18GB download · API key rotation
(pasted in chat twice).
**Rulings:** catalog-titles tier · prenasal queue (220 rows,
Approval pane, bulk-approve ready) · Sungbum workbook (963 yellow
cells) · Sanskrit render-sheet eyeball rows · unlisted-recordings
archive go · GitHub private remote say-so · TibetanDictionary.dic
licensing option.
**Sessions:** K/F testing-campaign walkthrough · clean-Mac install
(the release gate's most important step) · dark-mode screenshots ·
GMR: LE'UR class + trust hierarchy + STD-008/9 + what to ADD.
**Uploads:** input-center standards + samples (announced 08-06) ·
Master Catalog · intact Release IV manual/Briefcat · honorific
books · reference pecha prints · Das SCD re-download · Xcode
vision projects · Fall-2022/Winter-2021 link sheets.

## C. EXTERNAL WATCH (dated triggers)

New (DigitalTibetan census 2026-08-14): tibetanebook.com +
dharmaebooks.org — Buddhist e-text sources, license check before
any use; TiCode pre-Unicode spec PDF pointer (legacy lane
reference).

BDRC OCR release (~end Aug) → model refresh + deskew-bug report
upstream · Lainé reply → abbreviations layer + Wilson register
growth · THL reply → kmaps/Solr integration + dictionary licenses ·
84000 reply → TM ingest · PKTC → Das etext/Chodrak lane · CIHTS →
Negi · DILA site return → Mahavyutpatti TEI refresh · web team →
collections manifest JSON · ALL Developer ID → signed builds ·
input-center server → remote-lane shakedown · data project →
Sanskrit V/visarga/candrabindu/A'A fixes (177k+ occ), pyewts 13
regenerations, LC pair-script banking, attestation recount.

## D. Policy fences (not open work — do not resurrect)

Cataloging sidelined (own future project) · NEVER lane
(unlicensed dictionaries) · Monlam definitions closed ·
red-flag list ships nothing unresolved.
