# Analysis & Research Suite — implementation plan

*How this was produced (2026-09-09).* Nine readers mapped the subsystems
each batch builds on (backup and document properties, quotation and text
DNA, the entity matchers, glossary and table formats, Draft/Manuscript
export, Files and Search, Analysis, the Compare suite, the catalogue
stores). For each batch three designers then wrote independent
specifications from three angles — translator-first, honesty-and-
provenance-first, engineering-risk-first — two judges scored them on
translator value, honesty, feasibility, Tibetan fit and testability, and a
synthesiser built the final specification from the winner while grafting
what the judges wanted kept. A completeness critic then checked all three
against every verdict in `docs/TEXT_ANALYSIS_SUITE_RESEARCH.md`.

The critic's reconciliation below is **binding** where it contradicts an
individual batch specification.

---

## Reconciliation (binding)

**Critic's verdict.** Coverage against the research doc is strong and the specs are unusually implementable. Every BUILD and ADAPT verdict in §§1–17 is either specified or explicitly deferred with a reason: §3(a)/(d), §5, §6, §7, §12, §13 in Batch 4; §8, §9, §10, §11, §14 in Batch 5; all eleven §17 rows in Batch 6. Nothing rated SKIP has crept back — branching, mirror-with-delete, FTP, syntax colouring, ed-script output, word cloud, readability, sentiment, machine summaries/chat, Manuscript Matcher and the AI Research Assistant are all absent and named with their reasons. No spec composes English, none presents generated data as attested, and the acceptance tests are concrete enough to write from: literal expected strings, FIPS/CRC published check values, byte-identity assertions against the shipped bibliography writer, five-seed estimator bounds, and a brute-force equivalence test that earns the blocking claim. Two deliberate deviations from the research doc are properly written down and raised with Adam (Batch 3's local-index-plus-shared-TSVs instead of §17's shared references.db; Batch 4's finding that §3(a)'s 'the Manuscript already exports .docx' is false and the OOXML writer must be built). The material problems are almost all cross-batch rather than within any one spec, which is what reading three plans together is for: the same constitution census is widened three times, the same folio primitive is unified twice into two different modules, and a handful of shared primitives (eligibility rule, apparatus spans, CSV writer, provenance struct, shingle builder, syllable tokenizer) are each specified more than once. Folding that substrate into Batch 4 F0 — where it is cheapest and earliest — removes both high-severity findings and most of the low ones. Beyond that there are three real honesty defects to fix before any code: one control offered for a capability the spec itself says does not exist (Statistics ▸ Distinctive terms ▸ 'This folder'), one control with no defined behaviour (References ▸ Save report…), and two pane shells that ship buttons for pages that stay empty for several steps — each contradicting its own spec's no-dead-controls rule. Two verdicts are also quietly under-delivered: §5's promised 'Compare with Backup' listing (backup.h still has no enumeration — verified) and §17's 'texts that quote this passage', deferred on a Batch 5 dependency that the recommended order delivers first. With those addressed the three batches are ready to sequence and build in the order given.

### Build order (19 steps, across the three batches)

1. Batch 4 F0, EXPANDED to absorb the cross-batch substrate — create allcore/textspan.h with the folio primitive and apparatusSpans(); create allcore/provenance.h; create allcore/filewalk.h (eligibleTextFile/listEligibleFiles) and rewire gofer.cpp + libindex.cpp; widen BOTH constitution censuses (R3 modal and G2 hgm_gloss) to app/*.inc once, with both reviewed baselines and the mutation check; fix DraftPane::dataRoot_; add textdiff::applySelected, the selectedFilter out-parameter, renameFileTo directory pairs, and the Manuscript plain-text compare fix. Reason: it is the earliest point in the whole programme, and today three specs each plan the census change and two plan the folio unification into different modules. Doing it once here removes two of the three high-severity findings and gives Batches 5 and 6 a substrate to consume rather than rebuild. Gate: quotation_smoke, textdiff_smoke, gofer_smoke, libindex_smoke green unchanged; baseline 154 held; mutation check recorded.
2. Batch 4 F1 Versions (core versions.h + versions_smoke + lineOrigins green BEFORE any UI). Reason: it is the one undo primitive every later write in all three batches leans on — pre-replace, pre-patch, pre-normalize, pre-restore — and Batch 6's record history idiom copies its per-field/per-version discipline. Add here the ~2 h allcore::listBackups + Tools ▸ Compare ▸ 'Compare with Backup…' that §5 promised and the spec dropped.
3. Batch 4 F2 changed-folios report. Reason: cheapest visible win, first consumer of the unified folio primitive from step 1, and it lands the selectedFilter report routing that Batch 4 F6 and Batch 5 F7 both reuse.
4. Batch 4 F3 Normalize (textnorm + textnorm_smoke first). Reason: F4's Normalize mode consumes the engine, and the shad controls must ship defaultless while Adam's ruling is outstanding.
5. Batch 4 F4 Replace in Files with mandatory preview. Reason: §7 names it 'the input-centre formatter's core (roadmap item A)' — the highest-value item in Batch 4 for the input centres — and it must land after the preview/confirm/undo idioms have settled in F1–F3.
6. Batch 4 F5 Apply Patch (textpatch + textpatch_smoke, then the dialog and --apply-patch).
7. Batch 4 F6 tracked-changes .docx, INCLUDING the Word verification gate before the Save Report… filter, the Tools item and the Versions 'Send Changes to Word…' button ship. Reason: it also lands zipstore, which is the first brick of §3(b)'s deferred .docx reader, so the LATER item stops being unreachable. Add here the ~1.5 h §7 'Bookmark all matches' item so the last unhomed BUILD verdict in §§1–14 gets a batch.
8. Batch 5 F7 Table Compare — may start in parallel with steps 3–7 and must land before Batch 6 F9. Reason: it depends on nothing in this cluster but Provenance, lands inside an existing pane and audit row with zero ribbon growth, closes the oldest filed item in COMPARE_SUITE_PLAN.md, and delivers allcore::table::readDelimited/writeCsv that Batch 5 F4 and Batch 6 F9 both need — landing it early removes the three-more-CSV-writers duplication.
9. Batch 5 F0 (EXTEND textspan with the syllable tokenizer, script routing and the three-mode apparatus policy — do not create a second module) + F1 Study pane shell, shipping with the Passages page only. Reason: fixes the empty-pages honesty defect and proves wiring, fit and the pane-local 1180 px ratchet before any algorithm exists.
10. Batch 5 F2 Repeated Passages, then F3 the .passages.db signature index immediately after. Reason: F2 must precede Statistics because Phrases calls passages::shingleForms (no placeholder n-gram counter is ever written), and F3 must follow F2 before anyone gets used to the slow folder scan.
11. Batch 5 F5 Statistics, whose first commit extracts unitSyllableCounts from verse.cpp and rewires analyzeVerse. Drop the unimplementable 'This folder' distinctive-terms option, or implement LibraryIndex::documentFrequency with it.
12. Batch 5 F4 Similarity studies (TRACER export first; the generic column-mapping importer last, and never labelled a TRACER profile until Adam supplies a real output file).
13. Batch 5 F6 Entities, last in Batch 5. Reason: most external banks, the only feature needing new accessors on an existing pane, and its first commit lifts the shared bank loader out of LibraryPane — which is also Batch 6's H4, so doing it here removes that hygiene item from Batch 6.
14. Batch 6 hygiene H1 (ApparatusBib::acip_refs), H2 (export titleOverlap), H5 (file the LibraryPane key-regex convergence). H3 is already done at step 1 and H4 at step 13; both should be struck from the Batch 6 commit.
15. Batch 6 F1 record engine, then F5 Insert Citation, then F6 regenerating bibliography — before the pane. Reason: the picker is a dialog over the record engine and needs no pane, so the most expensive daily task (retyping STD-007 entries by hand) ends two features into the batch. F5's locator prefill should call allcore::textspan::citeAt from step 1 rather than the app-side resolveFolioAt.
16. Batch 6 F2 References pane (single Records page), F3 collections/tags, F4 read state. Add each later page's switcher button and MORE ▾ entry only in the commit that implements it; add the ~3 h attachments.tsv so §17's 'records with attachments' is actually met.
17. Batch 6 F9 BibTeX/RIS import and export — reading and writing through allcore::table from step 8. Reason: no network, produces offline fixtures, and it populates the shelf that F7's duplicates engine then has to serve.
18. Batch 6 F7 duplicates finder (colophon shingles delegated to passages::shingleForms from step 10), then F8 BDRC refresh last — the only network feature and the only one that can be wrong because someone else's data is wrong.
19. Batch 6 F10 quotations facet, now shipping BOTH directions: the forward lane on quotation.h and the reverse lane on PassageIndex::candidatesFor from step 10, since Batch 5 has already delivered it. Keep the §8-dependency sentence only as the fallback when .passages.db is absent.

### Duplications to collapse before any code is written

- Folio primitive built twice: Batch 4 F0 puts folioMarkerEnd in allcore::textdiff; Batch 5 F0 puts folioAt/citeAt/folioRange/countFolioMarkers in a new allcore/textspan and rewires citeFor. Verified neither exists at HEAD — this is duplicate new work, and it would leave a fifth folio implementation rather than removing four.
- Constitution census widened three times: Batch 4 F0 (R3), Batch 5 F1 (R3 + G2), Batch 6 H3 (R3), each with its own baseline install on a gate Adam owns. Verified constitution_check.py reads app/main.cpp only today.
- Apparatus-span rules specified twice with different semantics: Batch 4 F3's balanced-bracket refusal vs Batch 5 F0's three-mode policy with intra-syllable refusal, ⟨ ⟩ handling and ( ) retention.
- Eligible-text-file rule stated a fourth time: Batch 5 F2's folder scope restates the extension/size rule instead of calling Batch 4 F0's allcore::eligibleTextFile, which Batch 4 has already rewired gofer.cpp and libindex.cpp onto.
- Three more CSV writers (Batch 4 changedFoliosCsv, Batch 5 table::writeCsv, Batch 6 F9 export) on top of the three that already exist (worksheet csvCell, regenreg csvField, textdiff apparatusCsv).
- Three provenance idioms for one house rule: Batch 4's per-version JSON meta, Batch 5's allcore/provenance.h Provenance struct, Batch 6's per-field RefField sources.
- Shingle builder re-implemented: Batch 6 F7's 5-syllable colophon shingles duplicate Batch 5 F2's passages::shingleForms, which lands earlier in the recommended order. (Batch 6's refusal to use MinHash/LSH is separately justified and correct; only the shingle builder is duplicated.)
- Syllable tokenizer still doubled after Batch 5's 'one tokenizer' commitment: catalog_id.cpp's private syllables() (verified at line 24) is untouched by Batch 5 and then built upon by Batch 6 H2's titleOverlap export.
- Delimited reader/writer: Batch 6 F9's CSV export path names worksheetCsvRow/apparatusCsv rather than Batch 5 F7's allcore::table reader/writer, which by the recommended order already exists and which Batch 5 F4's TRACER import already reuses.

### Honesty findings — fix in the specification, not later

- Batch 5 F5, Statistics ▸ Distinctive terms: the 'Against: This folder' radio option is a control with no implementation. The engine takes a single dfLookup, the app passes Spine::corpusCount, and the spec's own open question 6 states the folder document-frequency index does not exist. Offering it violates the no-dead-controls rule; remove it or implement LibraryIndex::documentFrequency with a battery.
- Batch 6 F2, References MORE ▾ ▸ 'Save report…': listed as a control with no output format, no destination, no engine function and no acceptance test anywhere in the feature, and duplicating the adjacent Export ▸ CSV. Define it or delete it.
- Batch 5 F1 and Batch 6 F2, sequencing: both pane shells ship page-switcher buttons and menu entries for pages that stay empty for three to five later steps ('four empty pages'; MORE ▾ carrying Duplicates…, Refresh from BDRC…, Import…, Export ▸). Each spec states its own rule — 'every control implemented; nothing greyed with a promise' — and then breaks it in its own sequencing. Add each page's button in the commit that implements it.
- No violations of the composition rule were found: no spec composes English, none renders hgm_gloss outside the Lookup hand-off (Batch 5 D2), none presents generated data as attested (Batch 5 D3's verified-before-reported rule and Batch 6's verbatim-published-entry rule are both explicit and battery-pinned), and provisional/machine tiers are badged throughout. This item is recorded so the absence is on the record, not inferred.

### Gaps, with the fix each requires

| Severity | Where | What is missing | Fix |
|---|---|---|---|
| high | Cross-batch: Batch 4 F0 (allcore::textdiff::folioMarkerEnd) vs Batch 5 F0 (allcore/textspan: folioAt, citeAt, folioRange, countFolioMarkers) | Both plans unify the app's four folio-marker regexes — but into two different modules with two different APIs. Verified at HEAD: neither textspan nor folioMarkerEnd exists yet, so this is duplicate new work, and if Batch 4 lands first Batch 5 inherits a fifth folio primitive rather than a unification. | Name the home once, now: the folio primitive lives in core/include/allcore/textspan.h. Batch 4 F0 CREATES textspan with only the folio pieces (folioAt/citeAt/folioRange/countFolioMarkers + the citeAt-vs-folioAt cross-check test) and rewires textdiff::citeFor, catalog_list::acipFolioRange and docprops::textStatistics; Batch 5 F0 EXTENDS the same header with the syllable tokenizer, script routing and apparatus policy. Delete folioMarkerEnd from Batch 4's textdiff additions. |
| high | Cross-batch: Batch 4 F0 gate work, Batch 5 F1 gate fix, Batch 6 H3 | All three plans widen tools/constitution_check.py's R3 modal census from app/main.cpp to app/*.inc, each with its own baseline commit. Verified: constitution_check.py reads only app/main.cpp today (line ~29). Only Batch 5 also widens the G2 hgm_gloss census; Batch 4 and 6 widen R3 alone. Three baseline installs on one gate is churn on a gate Adam owns. | Do it once, in Batch 4 F0, and widen BOTH censuses (R3 modal and G2 hgm_gloss) in that single commit with both reviewed baselines and the mutation check. Batch 5 F1 and Batch 6 H3 then assert the gate is already widened instead of changing it; delete the census work from both. |
| medium | Batch 5 F1 (Study pane shell, sequencing step 4) and Batch 6 F2 (References pane, sequencing step 4) | Both shells ship surfaces for features that land 3–5 steps later: Batch 5 F1 ships 'four empty pages' with Statistics/Similarity/Entities not implemented until steps 5/8/9; Batch 6 F2 ships a 3-page stack plus MORE ▾ entries for Duplicates…, Refresh from BDRC…, Import BibTeX/RIS… and Export ▸ that land at steps 7–9. This contradicts each spec's own rule ('every control implemented; nothing greyed with a promise') and the batch commitment that a stop after any unit leaves an honest surface. | Make the page-switcher button and the MORE ▾ entry part of the commit that implements each page, not of the shell commit. Batch 5 F1 ships the shell with the Passages page only (which lands next); Batch 6 F2 ships a single Records page. Add a selftest to each shell asserting that the switcher's button count equals the number of implemented pages. |
| medium | Batch 5 F5, Statistics ▸ Distinctive terms, control 'Against: The HGM course corpus · This folder' | The 'This folder' option has no implementation anywhere in the spec: df comes from Spine::corpusCount, the engine takes a single dfLookup, and the spec's own open question 6 states that folder-based document frequency 'needs a document-frequency index the library index does not expose today'. It is a control offered for a capability the plan says does not exist. | Remove 'This folder' from the radio for this batch and leave the question in open questions only; or implement it properly by adding LibraryIndex::documentFrequency(term, bool fold, long* nDocs) beside countTermByFile, with a libindex_smoke check pinning df over a 3-file index, and keep the option. |
| medium | Batch 6 F10 vs Batch 5 F3, against §17 row 'Web of Science citing/related' (ADAPT: BOTH 'texts that quote this passage' and 'texts this text quotes') | F10 ships only the forward direction and defers the reverse because 'it needs the collection-wide passage index of §8, which is Batch 5 and does not exist' — but the recommended order delivers Batch 5 F3 (.passages.db with candidatesFor) before Batch 6 starts. The verdict is deferred on a dependency that will already be met, so §17's row is left half-answered for no reason. | Add a second facet 'Quoted by (attested)' to F10 built on allcore::PassageIndex::candidatesFor + F2's exact seed-and-extend verification, with the same threshold wording and EVIDENCE badge. Keep the current deferral sentence only as the fallback shown when .passages.db is absent (Batch 6 pulled ahead of Batch 5). ~4 h on top of F10's 6 h. |
| medium | Batch 6 F2, MORE ▾ menu item 'Save report…' | No output format, no destination, no engine function and no acceptance test are specified for it anywhere in F2's UI/Data/Engine/Tests sections; and it appears to duplicate the adjacent 'Export ▸ CSV of the visible rows'. | Either delete the item (Export ▸ CSV already covers it) or define it: a Markdown report of the visible rows with the shelf's built_utc, banks_generation, counts and skip lists, written through saveOrWarn, with a selftest asserting the header line and the row count match the table. |
| medium | Batch 4, F1 Versions — against §5 line 93 ('it also gives "Compare with Backup" its listing') | §5's stated secondary benefit is dropped. Verified: backup.h still exposes only backupFile / latestBackup / restoreBackup — no enumeration — so 'Compare with Backup' for the three shared stores (proposals.tsv, comments.tsv, CATALOG_TEAM.tsv) stays blocked exactly as docs/COMPARE_SUITE_PLAN.md:99 records. The spec's deliberately-omitted note reinterprets §5 rather than delivering it. | Add allcore::listBackups(backupDir, basename) -> vector<{stamp,path,bytes}> to backup.h (≈10 lines, mirroring versions::list's sort), a backup_smoke case, and a Tools ▸ Compare ▸ 'Compare with Backup…' entry that lists the three shared stores' backups and routes the chosen one through g_compareTexts. ~2 h inside F1. |
| medium | Batch 6, F1/F2 — against §17 row 1 ('every text/edition/article/person/place is a record with attachments (scans, PDFs, notes)') | Attachments are derived links to existing sidecars only. There is no way to attach a user file — the article PDF behind an imported BibTeX record, a scan not in the cache, a colleague's notes — so the §17 row is only half met, and imported secondary literature (F9) can never carry the PDF it describes. | Add attachments.tsv (# record_key\tpath\tlabel\tby\tat\tstate) to F3's store: absolute paths, LINKS not copies (preserving the 'never a second store of texts' commitment), with a 'missing on disk' state shown rather than a silent absence, an 'Attach a file…' card action and a refshelf_smoke round-trip. ~3 h. |
| medium | Cross-batch: Batch 5 F2 folder scope vs Batch 4 F0 allcore::eligibleTextFile / listEligibleFiles | Batch 5 F2 restates the eligibility rule inline ('.txt .acip .md .act .inc .ace, ≤ 10 MB') instead of calling the shared helper Batch 4 F0 creates and rewires gofer.cpp and libindex.cpp onto. That is the fourth statement of one rule. | State in Batch 5 F2's Engine section that folder scope calls allcore::listEligibleFiles with its glob and file cap, and that the disclosed skip counts come from WalkStats — no local walk. |
| medium | Cross-batch: Batch 4 F3 Normalize (apparatus stripping) vs Batch 5 F0 Apparatus policy (StripSpan/KeepInner/AsWritten) | Two apparatus-span rules with different refusal semantics — Batch 4 refuses a line with unbalanced [ ] or { }, Batch 5 refuses a syllable whose mark falls inside it and additionally handles ⟨ ⟩ and retains ( ). They will disagree about what an apparatus span is on the same file. | Export one primitive in textspan (Batch 4 F0, since Normalize consumes it first): allcore::apparatusSpans(line) -> vector<{beg,end,kind}> + a balanced() predicate, built from textdiff::Options::ignoreApparatus's regex. Batch 4 F3 and Batch 5 F0 both call it; each keeps its own refusal policy on top, and both policies are named in the report headers. |
| medium | Cross-batch: Batch 6 H2 (export catalog_id's private overlap()/syllables()) vs Batch 5 F0 D4 ('one tokenizer') | Batch 5 promotes quotation.cpp's wylie syllable tokenizer and rewires quotation/textdiff/catalog_list, but leaves catalog_id.cpp's own syllables() (verified at core/src/catalog_id.cpp:24) untouched; Batch 6 then exports the function that sits on top of it. The 'one tokenizer' claim is not true after either batch, and nothing records why. | Add one sentence to Batch 5 F0 stating that catalog_id's syllables() operates on uppercase ACIP normalised titles and is deliberately NOT converged, with the difference documented in textspan.h; or converge it and prove it with title_xlat/catalog_id batteries. Either way the divergence must be written down, not left implicit. |
| medium | Cross-batch CSV writers: Batch 4 F2 changedFoliosCsv, Batch 5 F7 allcore::table::writeCsv, Batch 6 F9 CSV export | Three more RFC-4180 quoting implementations are specified on top of the three that already exist (worksheet.cpp csvCell, regenreg csvField, textdiff apparatusCsv) — the exact maintenance smell the subsystem maps flag. | Make Batch 5 F7's allcore::table::writeCsv the single writer; Batch 4 F2 keeps apparatusCsv's helper until then and Batch 6 F9 calls table::writeCsv (it lands earlier in the recommended order). File the converge-the-older-three item in TODO with the three anchors. |
| low | Cross-batch provenance: Batch 4 F1 per-version JSON meta, Batch 5 F0 allcore/provenance.h Provenance struct, Batch 6 F1 per-field RefField sources | Three provenance idioms for one house rule. Batch 5's Provenance is a superset shape and is the only one made mechanical; Batch 4's version records and Batch 6's report headers reinvent it. | Create allcore/provenance.h in Batch 4 F0 (it is the earliest consumer), have the version record embed it as its provenance block, and let Batch 5 and 6 extend the same struct. One report-header renderer, provenanceBlock(), everywhere. |
| low | Batch 6 F7 refdup (colophon similarity) | Re-implements 5-syllable shingles + Jaccard that Batch 5 F2's passages::shingleForms already provides and batteries. The spec justifies not using MinHash/LSH (correctly), but that argument does not cover the shingle builder itself. | Have refdup's colophon comparison call allcore::passages::shingleForms(doc, 5) once Batch 5 has landed (it does, in the recommended order); keep the exact Jaccard in refdup. One line in F7's Reuses section. |
| low | Batch 5 F1 acceptance tests 7–8, and Batch 6 F2 test 5 | Batch 5 states a ≤700 px design target for the Study ribbon but only asserts the ≤800 px pane width; Batch 6 asserts a ≤1180 px pane-local ribbon ratchet. Two new panes in the same Research group adopt two different pane-local ceilings, and one of them is not test-enforced. | Pick one pane-local ceiling for new panes — 1180 px (the app's launch width) — and assert it in both panes' selftests on the ribbon's own minimumSizeHint().width(), keeping the ≤800 px pane-height gate as it is. Record the number once in the audit rows. |
| low | Batch 5 F5 acceptance test 13 (log-likelihood) and Batch 6 F7 test 8 (2,000-record performance) | Test 13's expected value is described as 'computed by hand from Dunning (1993)' rather than pinned as a literal, so the battery cannot be written from the spec; test 8 asserts a 2-second wall-clock ceiling, which is flaky on a loaded CI machine or a debug build. | Put the literal G² value for a=10,b=90,c=100,d=9800 in the test text. Replace the wall-clock ceiling with a bounded-work assertion (scan.comparisons < N for the 2,000-record fixture), which is what the blocking claim actually needs. |
| low | Batch 4 F2 acceptance test 1 | The spec text carries an unedited self-correction: '…the Markdown contains @001A, @002A, 1 of 3… wait, 2 of 3 folios changed…'. An implementer reading it cannot tell which string is being asserted. | Fix the sentence to assert the literal '2 of 3 folios changed' and 'Unchanged folios: 1'. |
| low | §7 'Mark' / 'Bookmark all matches in the Find bar' — rated ADAPT/BUILD | Filed to TODO by Batch 4 with a reason (it is a Find-bar/NumberedEdit change, not one of the six features) — an acceptable deferral, but it is the only BUILD verdict in §§1–14 with no batch, and the gutter bookmarks it needs already exist. | Give it a home: one line in Batch 4's close-out step ('Bookmark all matches' button in editops::FindDialog calling allMatches + NumberedEdit::setBookmarks, plus one selftest that N matches yield N bookmarks). ~1.5 h, no ribbon growth. |
| low | All three specs — §16 ('The plan keeps these six verbs as the menu vocabulary') | No spec checks its placements against §16's Diff / Merge / Replace / Sync / Version control / Text analysis vocabulary. Batch 4 in particular scatters the verbs: Replace in Files under Find, Normalize under Format, Apply Patch under Tools ▸ Compare, Versions under File. | Add a one-table placement check to Batch 4's close-out step mapping each new action to its §16 verb, and state the deliberate exceptions (Replace in Files sits under Find because that is where Find in Files is, per Notepad++). Cheap, and it is the criterion the research doc set for findability. |
| low | Batch 5 F5, Particles tab — against §10's 'particle density' | The named metric is refused: freestanding and fused counts are shown 'never as one density number'. The refusal protects against over-reading, but it also drops the figure §10 asked for, which is honest when shown with its denominator. | Show 'particles ÷ tokens = 0.19 over 8,431 tokens' beside the split counts, with the denominator in the same line and the formula printed — the same discipline the TTR row already uses. |
| low | Batch 5 F6 Entities, control 'Report a name the index does not know…' | No acceptance test covers it, and it is the only control on that page that writes to a shared store (the proposals channel). | Add a selftest: under g_harnessRun the action composes a CatalogIdentity proposal whose evidence string contains the surface form and its cites, and writes nothing. |
| low | Batch 5 F6 Entities — against §11 ('listed with folio citations and links to the map/lineage work already filed') | Folio citations and person/ToL/BDRC links are specified; the link to the filed map/lineage work is not mentioned at all. | Either add the link where that work has a surface, or add one line to deliberatelyOmitted naming the filed item and saying it has no surface yet — so the verdict is answered rather than silently dropped. |

---

## Subsystem maps (what these batches build on)

### Backup + document properties + document save paths (allcore::backup, docprops sidecars, Overlay/Draft/Manuscript save) — the substrate a per-document Versions feature would attach to

| API | Purpose | Anchor |
|---|---|---|
| `allcore::backupFile` | Copies src to <backupDir>/<basename>.<stamp>.bak (overwrite_existing, idempotent per stamp), then prunes oldest same-basename .bak files beyond `keep` (prune failures ignored; keep<=0 disables pruning). Returns false if src missing or copy fails. No clock inside: caller supplies the stamp. Whole-file copy, NO compression. | core/include/allcore/backup.h:22-23; core/src/backup.cpp:15-39 |
| `allcore::latestBackup` | Returns the path of the lexicographically greatest <basename>.*.bak in backupDir, or empty. This is the ONLY query the module offers — there is no list-all-versions API; stamps must sort chronologically (ISO-like). | core/include/allcore/backup.h:28-29; core/src/backup.cpp:41-54 |
| `allcore::restoreBackup` | Copies the named backup over dst (overwrite_existing); never touches the backup. No safety copy of dst — that rule lives in the app wrapper. | core/include/allcore/backup.h:32-33; core/src/backup.cpp:56-63 |
| `restoreStoreWithSafety (app-side static)` | The app's restore rule in one testable place: if dst exists, first backupFile(dst, backupDir, "pre-restore-"+stamp, 14) (fail => abort), then restoreBackup. Drilled by the app selftest TEST-11 block (main.cpp:18553-18582). | app/main.cpp:3022-3032 |
| `saveOrWarn` | The single honest write predicate (WP-1): open\|Truncate, byte-counted write, flush; any failure -> warnWriteFail (one modal per path, logs under g_harnessRun) and returns false. Every document/sidecar save gates its success message on this. Streaming twin: writeAllOrWarn (3061+). | app/main.cpp:2952-2964; warnWriteFail 2919-2937 |
| `docprops::sidecarPath` | Returns <dataRoot>/library/properties/<completeBaseName(docPath)>.json; empty if either input empty. Keyed by BASE NAME ONLY (same keying as per-text glossaries library/glossaries/<base>.tsv). | app/main.cpp:3947-3951 |
| `docprops::load / docprops::store` | Read/write the sidecar JSON (Indented); store mkpaths the dir and writes via saveOrWarn("The document properties"). | app/main.cpp:3952-3965 |
| `docprops::noteSave` | Called after every SUCCESSFUL document save (3 call sites). Increments sidecar keys: revision (+1), editingSeconds (+= editSeconds), lastSavedBy (g_userName or $USER), lastSavedAt (ISO datetime). No-ops silently when sidecar is empty. Receives neither the document path nor the saved bytes — a Versions hook would need those added. | app/main.cpp:3966-3977; callers 8323 (Overlay), 22058 (Draft), 33537 (Manuscript) |
| `docprops::digest` | SHA-1 of UTF-8 text; savedDigest_ vs current digest is how Overlay/Draft compute isDirty()/draftDirty(). A Versions store could dedupe identical saves with the same digest. | app/main.cpp:3932-3935; isDirty 8498-8501; draftDirty 22063-22066 |
| `docprops::textStatistics` | Tibetan-aware counts (folios, syllables, shads, words, paragraphs, characters) used by the Properties Statistics tab; research §5 wants each version listed 'with the Properties statistics'. | app/main.cpp:3981-4011 |
| `docprops::showDialog / Input` | Word-style Properties window (General/Summary/Statistics/Content/Custom); Statistics tab shows 'Revision number' from sidecar revision and 'Last saved by'. Returns immediately under g_harnessRun. A 'Versions' tab or button would go here. | app/main.cpp:4085-4256; revision row 4159 |
| `docprops::renameFileTo` | Renames a file plus base-name-keyed sidecars given as (dir, ext) pairs. A versions folder keyed by base name must be added to the pair lists at the 3 callers or it orphans on rename (there is no dir-rename support in it — only file rename). | app/main.cpp:4048-4083; callers 8617-8619, 22121, 33642 |
| `OverlayPane::saveDocument / saveDocumentAs / writeDocumentTo` | Overlay Document-box save. saveDocument -> Save As when untitled or wasWylieFile_ (never overwrite a wylie source with ACIP); asks before overwriting a file under /library/. writeDocumentTo applies docLineEnding_ (LF/CRLF/CR) and enc::encode(docEncoding_) (refuses on unmappable chars), saveOrWarn, then savedDigest_, docprops::noteSave, editTimer_.restart(). The encoded `bytes` are exactly what lands on disk — the natural payload for a version copy. | app/main.cpp:8280-8326 |
| `DraftPane::saveDraft / saveDraftAs` | English draft as UTF-8 plain text; optional editops house-style spacing applied on save; saveOrWarn; status on termLive_; savedDigest_; noteSave(sidecarPath(dataRoot_, draftPath_)). | app/main.cpp:22045-22062; 22137-22145 |

**Reuse for:** Versions: keep a compressed copy on every save — Hook at the three successful-save sites immediately after saveOrWarn succeeds and beside docprops::noteSave — Overlay writeDocumentTo (main.cpp:8322-8325, payload = the encoded `bytes` + docEncoding_/docLineEnding_), Draft saveDraft (22057-22060, payload = toPlainText().toUtf8()), Manuscript save (33536-33539, payload = toHtml().toUtf8()). Cleanest: extend noteSave or add a sibling docprops::noteVersion(parent, dataRoot, docPath, bytes, meta) so the revision number written to the sidecar and the version folder name stay in lockstep (revision N == version N). Use docprops::digest to skip a version when bytes are identical to the previous one. Decide autosave policy explicitly: Manuscript autosave (33497-33503) calls save() every minute while dirty and would otherwise mint a version per minute. Compression: do it in the app layer with qCompress (inferred Qt API; nothing in repo uses it) or add a tiny core engine — allcore has no zlib; if the store lives in core it needs a vendored deflate + smoke test per house convention (core/src + core/include/allcore + add_test in core/CMakeLists.txt, cf. backup_smoke at core/CMakeLists.txt:452-454).; Versions: listing — backup.h has NO list function (only latestBackup, backup.cpp:41-54); write an allcore listVersions(dir, basename) -> vector<{stamp, path, bytes}> sorted by stamp (or reuse the '<base>.<stamp>.bak' naming so latestBackup keeps working). Per-version metadata (who = g_userName/$USER as noteSave already writes to lastSavedBy at 3971-3973; revision; encoding; digest; textStatistics) needs a per-version JSON — the compare_sessions per-file JSON idiom (compare_pane.inc:391-412) or a versions index inside the properties sidecar are the two existing patterns. Surface it as File ▸ Versions… (next to Save routing at 39394-39435, dispatched via the same active()/frontDoc lambdas) and/or a row/button in the Properties Statistics tab (4159), with the dialog returning early under g_harnessRun like showDialog (4086).; Versions: Compare with Version — Copy the 'Compare with Saved Version' action (main.cpp:40361-40366): frontDoc() -> (name, text, path); pick a version; decompress; decode with enc::decode(bytes, storedEncoding) for Overlay versions (namespace enc at 4945-5026), UTF-8 for Draft; then g_compareTexts(fileName + " (version <stamp>)", vText, fileName + " (editing)", text). For Manuscript compare plain text on both sides (load the version HTML into a QTextDocument and take toPlainText) — see risk on the existing feature. Blame ('which version introduced this line') = walk versions oldest->newest with allcore::textdiff::diffLines (textdiff.h:63).; Versions: Restore with confirm — Mirror restoreStoreWithSafety (3022-3032): bank the CURRENT on-disk file as a version first ('pre-restore-<stamp>' precedent at 3029), then write the chosen version's bytes to the document path via saveOrWarn (2952), then reload the pane using the existing revert paths (revertDocument 8481 re-runs openFile with pendingEncoding_; revertDraft 22090; revertManuscript 33613) so savedDigest_/dirty_/editTimer_ reset correctly. Confirm via QMessageBox guarded by g_harnessRun/g_sweepActive (constitution R3, cf. 40955, 4086). Do NOT call restoreBackup on documents unless the versions are stored uncompressed .bak files — restoreBackup is a raw copy.; Rename/Move must carry the versions folder — Add the versions dir to the (dir, ext) pair lists passed to docprops::renameFileTo at 8617-8619, 22121, 33642 — but renameFileTo only QFile::rename()s single files ('<dir>/<oldBase><ext>'), so a per-document DIRECTORY (library/versions/<base>/) needs either a small extension to renameFileTo (directory pairs) or a flat naming (library/versions/<base>.<stamp>.ext) that fits the existing pair mechanism.; Tests — Core: a versions_smoke in core/tests modelled on backup_smoke.cpp (deterministic stamps, temp dir cleaned at entry and exit, prune + missing-source + restore drills). App: extend the Overlay selfTest sidecar block (8003-8015) and the TEST-11 drill (18553-18582) — pin that a save mints exactly one version, identical re-save mints none, restore banks a pre-restore version first, and the listing order is chronological. Register in the selfTest sum at 41785-41807.

**Risks:** DraftPane::dataRoot_ (main.cpp:23759) is never assigned — the constructor initializes root_ from `root` (21675-21677) but not dataRoot_, and there is no setDataRoot for Draft (only Manuscript has one, 33543, called at 38494). Consequence, observed from code: docprops::sidecarPath("", draftPath_) returns empty, so noteSave at 22058 is a silent no-op (draft revision never increments), Draft Properties shows 'No data root: properties cannot be stored' (4116), and Rename at 22121 builds the sidecar dir as "/library/properties". A Versions hook placed beside noteSave would inherit this and silently keep no draft versions — fix dataRoot_ wiring first (or use root_).; Sidecar keying by completeBaseName only (3947-3951): two documents named the same in different folders share one properties/glossary sidecar; a versions store keyed the same way would interleave two documents' histories. Either accept for parity (and document it) or key versions by a hash of the absolute path — a design decision to put to Adam.; Manuscript autosave (33497-33503) calls save() every minute while dirty, so 'a version on every save' without an autosave flag means a version per minute of editing; also each autosave already bumps sidecar revision, so revision numbers for manuscripts do not correspond to deliberate saves.; No compression capability exists anywhere in the repo (no qCompress/zlib/miniz in app/ or core/, CMake links only Qt modules + allcore). The plan's 'compressed copy' therefore forces a choice: app-layer qCompress (Qt-only, so no core battery can drill it) or a new core dependency; putting it in core means vendoring a deflate implementation with its own smoke test.; Existing 'Compare with Saved Version' compares mismatched representations for manuscripts (observed, not yet verified at runtime): cmp::readText(path) returns the raw .html source (compare_pane.inc:20-29) while manuscriptText() returns editor_->toPlainText() (33724), so the left side is HTML markup and the right is plain text. A Compare with Version for manuscripts must normalise both sides or it will report every line as changed.; backup.h offers only latestBackup — no enumeration — and the Help-menu restore (40953-41016) restores the newest only; nothing in the existing module supports 'pick a version'. Do not stretch latestBackup into a picker; add a listing function.

### Tokenisation / search-normalisation / quotation-detection layer of allcore: quotation.h (verbatim corpus-attested quotation finder), textdna.h (per-line verse/prose/quote-mark strip), searchnorm.h + affixnorm.h (lucene-bo analyzer ports: affix strip, da-drag strip, pa/ba fold, verb-lemma fold, Tibetan-unicode normalisation), lexicon.h (Monlam word-list attestation, reference only). All C++20, no Qt, each with a CTest smoke.

| API | Purpose | Anchor |
|---|---|---|
| `allcore::detectQuotations` | Verbatim, corpus-attested quotation finder. Converts ACIP via acipToEwts if flagged, tokenises to syllables, then for every passage position builds an FTS5 phrase query of min_syllables consecutive syllables, calls spine.corpusSearch(phrase, "", 20), re-verifies and greedily extends the match against the segment's own syllables, keeps the longest, advances past it (non-overlapping), then merges back-to-back matches of consecutive seq in the same course into one QuotationMatch. min_syllables is clamped to >=3 (quotation.cpp:52). Below-threshold overlap is never reported (rule 3). | core/include/allcore/quotation.h:28-31; impl core/src/quotation.cpp:47-119 (loop 58-98, merge 99-118) |
| `allcore::QuotationMatch` | One attested match: source course id, seq (becomes the RANGE END after a cross-segment merge, start seq is lost — quotation.cpp:108), the corpus-form span, the segment's published English, and the passage-side syllable offset/length. | core/include/allcore/quotation.h:16-24 |
| `(file-private) syllables` | THE wylie-space syllable tokenizer that makes quotation matching exact on both sides. Splits on space/tab/CR/LF and on '/', '\|', ',', ';'; strips non-alphanumeric edge characters except the apostrophe; keeps '+', '.', '~', '\'' inside a syllable; lowercases A-Z; drops tokens starting with '@' (page markers) and all-digit tokens. NOT exported — cannot be reused without promoting it to a header. | core/src/quotation.cpp:11-43 |
| `allcore::textDna` | Per-source-line structure strip. Runs analyzeVerse (verse.h) for the document's dominant meter, splits each raw line on ',', ';', '/' into feet, tokenises each foot with lattice tokenizeDocument, sums syllables, classifies Blank / Prose (when !is_verse) / Verse (every foot == meter) / VerseIrregular, and sets quote_mark when the line has ZHES/CES/SHES followed within 4 tokens by GSUNGS/BSHAD/SMRAS/GSUNG/BYA/GRAGS (lineQuoteMark, textdna.cpp:15-41). Input is assumed ACIP (uppercase compare). | core/include/allcore/textdna.h:37; impl core/src/textdna.cpp:45-99; types textdna.h:14-35 |
| `allcore::DnaClass / DnaLine / DnaReport` | Result types; DnaLine::number is the 1-based SOURCE line (not the analyzer's shad-renumbered line) so a UI can jump by line. | core/include/allcore/textdna.h:14-35 |
| `allcore::searchFoldWylie` | The full per-token INDEX-side fold used by the library indexer: stripDaDragWylie -> stripAffixedParticlesWylie -> paBaFoldWylie -> LemmaFold::fold (only if configureLemmaFold succeeded). Search-layer only; the header forbids display/dictionary use of folded forms. | core/include/allcore/searchnorm.h:52; impl core/src/searchnorm.cpp:263-271 |
| `allcore::paBaFoldWylie` | Standalone ba->pa, bo->po only (exact-token compare); everything else unchanged. Meant to run AFTER affix stripping, as lucene-bo chains it. | core/include/allcore/searchnorm.h:32; impl core/src/searchnorm.cpp:225-229 |
| `allcore::LemmaFold` | Unambiguous non-present verb form -> present stem map loaded from data/extracted/verb_lemmas.tsv (first two tab columns). fold returns nullptr when no fold applies (present stems never fold). | core/include/allcore/searchnorm.h:34-43; impl core/src/searchnorm.cpp:231-253 |
| `allcore::configureLemmaFold / globalLemmaFold` | Process-wide singleton (g_lemmas / g_lemmasLoaded, searchnorm.cpp:220-221). Unconfigured or missing file = fold silently limited to affix + da-drag + pa/ba. libindex stamps the fold generation into PRAGMA application_id and forces a full reindex on mismatch. | core/include/allcore/searchnorm.h:48-49; impl core/src/searchnorm.cpp:255-261; consumer core/src/libindex.cpp:176-188, 300-330 |
| `allcore::normalizeTibetanUnicode` | Port of lucene-bo TibCharFilter (strict map always; lenient map adds retroflex folding, Sanskrit-stack respellings such as padma -> pad+ma) then TibPattFilter (vowel-sign/subscript reorder, MergedSyl 1-3 Old-Tibetan splits, ts->c Sanskrit folds). Operates on UTF-32 codepoints, longest-match-first. For UNICODE entry points (attested-layer lookups, OCR text). | core/include/allcore/searchnorm.h:57-58; impl core/src/searchnorm.cpp:273-276; tables 57-102; patterns 146-218 |
| `allcore::stripAffixedParticles` | Faithful port of BDRC TibAffixedFilter: strips 'i/'o/'is/'am/'ang, stacked 'i'o/'i'am/'i'ang/'o'am/'o'ang, and 'ur/'us endings from one UNICODE syllable, guarded by needsAA(prefix, main) so dga', mkha', bka' keep their own 'a. Unchanged if no rule applies; idempotent. | core/include/allcore/affixnorm.h:27; impl core/src/affixnorm.cpp:95-137 (needsAA table 50-88) |
| `allcore::stripAffixedParticlesWylie` | Wylie projection of the unicode rule: suffix-compares the LAST syllable against 12 particle spellings, converts it with wylieToUnicode, lets the unicode port decide, then finds the wylie prefix whose conversion equals the stripped unicode. Returns the input unchanged if the port declines or conversion fails (never guesses). Cost: up to O(len) wylieToUnicode calls per token. | core/include/allcore/affixnorm.h:34; impl core/src/affixnorm.cpp:157-189 |
| `allcore::stripDaDrag / stripDaDragWylie` | Old-Tibetan da-drag: drop a final ད/d after ན/ར/ལ (gyurd->gyur). Wylie variant is a 4+ char suffix test on the whole word string. Medial-འ rule deliberately unported (no source to port from, affixnorm.h:15-17). | core/include/allcore/affixnorm.h:40-41; impl core/src/affixnorm.cpp:139-155 |
| `allcore::RefLexicon` | Two-list Monlam word-list bank (Apache-2.0). load normalises UTF-16LE-with-BOM or UTF-8 to UTF-8, skips ASCII-lettered header lines, stores one unicode word per line in set a_ (first label) or b_. attested returns "" or the label(s) joined by " + ". eachWord visits both sets with no early exit. REFERENCE ONLY per header: never drives segmentation or parse decisions. | core/include/allcore/lexicon.h:20-35; impl core/src/lexicon.cpp:74-102 (decoder 31-64) |

**Reuse for:** Repeated Passages — syllable tokenisation for k-gram shingles — Reuse the semantics of quotation.cpp:13-43 syllables() (wylie space; splits on whitespace and / | , ; ; strips edge punctuation except apostrophe; keeps ' + . ~ inside; lowercases; drops @page markers and pure digits) — but it is file-private, so PROMOTE it to a shared header (e.g. core/include/allcore/syllables.h with a smoke test) and make detectQuotations call the promoted function. Do not write a fifth tokenizer: lattice tokenizeDocument (ACIP space + barrier flags, lattice.h:45 / lattice.cpp:12-50), analysis tokenizeAcip (analysis.h:56 / analysis.cpp:11-40), textdiff tokenize (private, byte-offset spans, textdiff.cpp:313-341) and catalog_id syllables (catalog_id.cpp:24-30) already exist.; Repeated Passages — script-agnostic input (ACIP / wylie / Tibetan unicode -> one EWTS form) — Route through the canonical converters exactly as textdiff::normalizeLine does with Options::scriptAgnostic (textdiff.cpp:84-101): unicode -> unicodeToWylie (fail the line when warns>0), all-uppercase -> acipToEwts, else pass through; or use engines.h looksLikeWylie/tokenToEwts (engines.h:30-39) per token. Unicode texts (OCR) may first go through normalizeTibetanUnicode(utf8, lenient) (searchnorm.h:57). Lines that fail conversion must be counted and reported (Result::unnormalised pattern, textdiff.h:50), never shingled as if converted.; Repeated Passages / similarity studies — optional 'near' tolerance layer — Apply searchFoldWylie (searchnorm.h:52) per syllable BEFORE hashing to make po'i ~ po, gyurd ~ gyur, ba ~ pa, bklubs ~ klub collapse. This is the same fold the library index already stores in text_norm (libindex.cpp:300-330), so results stay consistent with Gofer search. It is lossy and search-layer only (affixnorm.h:18-19): label any hit found only after folding as 'same after search normalisation', never as verbatim, and offer an unfolded (verbatim) mode as the default. Cache the fold per distinct syllable — stripAffixedParticlesWylie calls wylieToUnicode repeatedly (affixnorm.cpp:171-184).; Repeated Passages — 'does this passage quote HGM's corpus' lane — Call detectQuotations(spine, passage, isAcip, 7) directly (quotation.h:28) instead of shingling against the corpus: it is already exact, attested, cross-segment-merging and returns the published English + course:seq for citation. Reserve MinHash/LSH for text-vs-text and library-wide comparison where no FTS phrase index exists (research doc: docs/TEXT_ANALYSIS_SUITE_RESEARCH.md:122-133). For 'which corpus lines are similar but sub-threshold' the doc rules that this is the existing concordance, not a quotation (docs/APPARATUS_DESIGN.md:51-55).; Repeated Passages — folio/line citation and refrain-vs-quotation annotation — textDna(acip) gives per-SOURCE-line number, syllable count, Verse/VerseIrregular/Prose/Blank and quote_mark (textdna.h:21-29); map a shingle hit's syllable offsets back to DnaLine.number for citations and use cls/quote_mark to say whether a repeat sits in verse (refrain/formula) or follows a zhes/ces announcement. Folio markers (@012a) are already dropped by syllables() and by textdiff's ignoreFolioMarkers regex (textdiff.cpp:106-108).; Similarity studies — Jaccard on syllable shingles + longest common passages — Compute Jaccard over the promoted syllable shingles; for the 'shared passages highlighted' view reuse textdiff::diffLines / diffInline (textdiff.h:60, 66) on the two normalised texts rather than re-implementing an LCS. Document-pair overlap-by-syllable-set already exists in miniature in catalog_id.cpp:35-45 (shared distinct syllables / longer side) — cite it as precedent, do not duplicate its scoring for passages.

**Risks:** The only correct wylie syllable tokenizer for corpus comparison (quotation.cpp:13-43) is in an anonymous namespace; copying it into a passages engine would silently diverge from detectQuotations. Promote, don't copy.; detectQuotations issues one FTS5 phrase query per passage position with LIMIT 20 (quotation.cpp:58-67); on a canon-scale text this is O(n) database round-trips and unsuited to library-wide scanning. If more than 20 segments share a common 7-syllable formula, the true longest continuation may not be among the candidates (inferred from the limit; not tested).; The phrase query is built from raw syllables with no FTS5 escaping (quotation.cpp:60-65). A syllable containing a double quote would break MATCH; corpusSearch then logs to stderr and returns empty (spine.cpp:391-394) — honest absence, but silent. FTS unicode61 with tokenchars=' splits on + . ~ that syllables() keeps inside a token (build_spine.py:140), so candidate recall for Sanskrit-stack syllables depends on FTS tokenisation; the verify step (quotation.cpp:70-90) protects precision only. Inferred.; After a cross-segment merge, QuotationMatch::seq holds the range END and the start seq is lost (quotation.cpp:108); callers printing 'course:seq' cite the last line of the span.; ACIP-vs-wylie routing is done three different ways at call sites (docIsWylie_ in tokEwts app/main.cpp:9727; !hasLower app/main.cpp:22553; any uppercase app/main.cpp:23112-23115) instead of engines.h looksLikeWylie (engines.h:30-35). A new pane should use the engine's judgement.; textDna assumes ACIP input: lineQuoteMark uppercases and compares to ZHES/CES/SHES (textdna.cpp:20-37) which also works for lowercase wylie, but Verse/Prose classes come from analyzeVerse over ACIP tokens; unicode input is not handled. A mixed document with no dominant meter classifies every syllable-bearing line as Prose (textdna.cpp:89-90).

### Catalog matchers and text-adjacent engines in allcore: people/author matching (authorsearch.h), catalogued subjects (subjects.h), the catalog filename/register/ASCII-list trio (catalog_name.h, catalog_register.h, catalog_list.h), the title attestation workbench (title_xlat.h), Botok segmentation + SOAS POS lexicon (botok.h, poslex.h), and the Tibetan calendar ports (tibcal.h year-level, tibcal_day.h KCK karana, tibcal_phugpa.h Phugpa) — mapped against the question "which of these can populate an 'Entities in this text' study (names, places, texts, dates) with folio citations, and on what data."

| API | Purpose | Anchor |
|---|---|---|
| `allcore::matchPeople` | QUERY -> PERSON direction: runs matchAuthors over each person's aliases, keeps the best tier per person (one hit per pid, homonyms never merged), sorts by tier, then localWorks desc, then display. Persons with empty aliases are skipped (authorsearch.cpp:217). Loading PersonRef is the caller's job (no JSON in allcore). | core/include/allcore/authorsearch.h:137; core/src/authorsearch.cpp:213 |
| `allcore::matchAuthors` | Five tiers: Exact (canonical identity), Contains (substring, floor kMinContainsLen=3), Spacing (despaced substring), Phonetic (pronounce() fold equal/substring, floor kMinPhoneticFold=4), PhoneticNear (bounded Levenshtein, ~1 slip per 6 chars). Header states Phonetic tiers are GUESSES and must render weaker. | core/include/allcore/authorsearch.h:89; core/src/authorsearch.cpp:115 |
| `allcore::authorCanonical / authorPhoneticFold` | Canonical space: strip trailing life-dates parenthetical, collapse whitespace, Tibetan unicode -> wylie via unicodeToWylie, ACII-uppercase (ACIP) -> EWTS via acipToEwts, lowercase. Phonetic fold = pronounce(canonical) with spaces/hyphens removed. Exposed so callers can key their own indexes identically — this is the hook for scanning a TEXT for aliases. | core/include/allcore/authorsearch.h:80-84; core/src/authorsearch.cpp:96,105 |
| `allcore::Subjects` | Work-key -> catalogued subject headings (R6 bilingual + Sungbum Tibetan). Reference-tier facts; empty vector = 'no subject recorded' (a real answer). isVinaya set only on exact R6 heading match. Must NOT re-rank glosses by genre (rule 1). | core/include/allcore/subjects.h:45-62; core/src/subjects.cpp:8-44 |
| `allcore::acipFolioRange` | First and last '@NNN[AB]' folio marks in a body ('','' when none). Only EXPORTED folio helper in allcore; range only, not position->folio. | core/include/allcore/catalog_list.h:51; core/src/catalog_list.cpp:25-52 |
| `allcore::generateAsciiCatalog` | Walks a library tree (.txt/.act/.inc/.ace, META skipped) and emits field-coded ASCII records S/F/D/T(T*)/E/A/V/P including folio range. Not an entity matcher; shows how filename fields NUMBER_TIB_ENG_AUTHOR are split. | core/include/allcore/catalog_list.h:47; core/src/catalog_list.cpp:54-173 |
| `allcore::CatalogRegister` | Read-only CSV/TSV register parser; columns found by header substring (number/catalog/no/id, title, folio start/end, date, initials/who/by). RegisterEntry carries folio_start/folio_end as filed by the registrar — a per-WORK folio range, not per-occurrence. | core/include/allcore/catalog_register.h:28-43; core/src/catalog_register.cpp:57-110 |
| `allcore::parseChangeStamp / composeChangeStamp` | The '<base> - updated YYYY-MM-DD ADA' folder stamp. Not entity-related. | core/include/allcore/catalog_register.h:52-62 |
| `allcore::composeCatalogFilename et al.` | Filename grammar composer + META continuation. Never mints numbers. Not an entity matcher. | core/include/allcore/catalog_name.h:28-60 |
| `allcore::TitlePairBank / buildTitleWorkbench` | Attestation only: given a Tibetan title, whole-title matches (shared-syllable score) + greedy longest-first fragment cover using whole-syllable contiguous spans (spanIn, title_xlat.cpp:36-40) over normalizeAcipTitle'd bank entries. Direction is query-title -> bank; it does NOT scan a running text for titles. | core/include/allcore/title_xlat.h:36-81; core/src/title_xlat.cpp:91-157 |
| `allcore::botok::SegTrie` | Memory-lean maximal-match segmenter (proven == ported Segmenter word-for-word). SegWord{text, tibetan, word, affixType, affixAa}. REFERENCE ONLY in UI; Tibetan unicode input. Helpers segWordBaseForm/segWordAffixSurface. | core/include/allcore/botok.h:385-422; core/src/botok_segtrie.cpp |
| `allcore::botok::Segmenter / chunkTokenize / CharTable` | Faithful ports of Botok chunking/trie/tokenize (Apache-2.0). Segmenter is the anchor the SegTrie battery compares against. | core/include/allcore/botok.h:55-70,174,355-375 |
| `allcore::PosLexicon` | SOAS classical POS lexicon (CC BY 4.0), exact lookup on tsheg-stripped unicode form, tags UNIONed. The tagset includes n.prop (2,826 rows measured) — a proper-noun tag, but there is NO person/place distinction and no API isolates n.prop (unambiguousNoun folds n.prop with n.count/n.mass, poslex.cpp:80). | core/include/allcore/poslex.h:20-37; core/src/poslex.cpp:25-82 |
| `allcore::tibetanYear / yearsFor` | Year-level rabjung arithmetic. yearsFor accepts English OR wylie element/animal names case-insensitively (me/sa/lcags/chu/shing; yos/'brug/sbrul/rta/lug/spre'u/bya/khyi/phag/byi/glang/stag). NO text scanner exists — the name tables live in an anonymous namespace (tibcal.cpp:10-18) and are not exported. 'gregorian' = western year in which the Tibetan year BEGINS. | core/include/allcore/tibcal.h:36-44; core/src/tibcal.cpp:27-65 |

**Reuse for:** NAMES (people) in this text — Reuse the PersonRef bank + authorCanonical. matchPeople is query->bank, so the study needs the inverse loop: tokenize the text (lattice tokenizeDocument or the doc's own clause split), canonicalize candidate spans with authorCanonical, and test each person's canonical aliases with the Exact/Contains/Spacing logic (authorsearch.cpp:143-165). Reuse the AuthorTier enum and LibraryPane::skinFor/zone labels for display; one row per pid (never merged); link via personRecord(pid) to dates/ToL/BDRC (personHtml pattern). Restrict automatic text scanning to the orthographic tiers; the header (authorsearch.h:21-27) says Phonetic tiers are guesses — if offered at all, they must carry Epistemic::Machine and the evidence string.; TEXTS (titles cited) in this text — Build a TitleBank/TitlePairBank once (library filenames + catalog_works.json, as CatalogPane::titlePairBank does) and, for each banked tib_norm of >=3 syllables, test spanIn-style whole-syllable containment against normalizeAcipTitle(text) — same normalization on both sides (catalog_id.h:38). Show the PUBLISHED English (catalog_works.json eng / catalog_titles.json) as Reference; never compose. Use extractAcipTitle for the text's own title and findColophonCandidates for the colophon's self-naming. Work key -> local file via the filesByWork_ pattern; Toh -> eKangyur locator via ekangyur_index.json.; DATES in this text — Year level only: add a small allcore scanner for wylie/ACIP '<element> <animal>' pairs (optionally 'rab byung N') feeding yearsFor(element, animal, rabjung) which already accepts the wylie names; export the kElemWy/kAnimWy tables from tibcal.cpp:10-18 rather than re-typing them. Present as 'candidate years' (yearsFor returns every 60-year match unless a rabjung is given) with the Losar-straddle note (tibcal.h:14-16). Day-level (KCK epoch >=2009; Phugpa proven 2015-2035) does not apply to historical colophons and must be stated as unsupported.; PLACES in this text — No matcher and no data exist. Honest options: (a) offer the user-initiated THL Places lookup link per selected span (main.cpp:1971-1975 pattern), (b) surface PosLexicon n.prop hits as 'proper noun (SOAS lexicon)' — a reference hint with no person/place distinction (poslex.cpp:57-61 tags()). Do not label anything a 'place' without a source.; FOLIO CITATIONS for every entity occurrence — Export one allcore position->folio primitive instead of adding a fifth copy: folioBefore (catalog_id.cpp:302, byte offset -> 'NNNA'), resolveFolioAt (main.cpp:11972, char pos -> folio+line), citeFor (textdiff.cpp:569, '@012B.3'), and catalog_list.cpp:25 folioRange all re-implement the '@0*(\d+)([AB])' walk with slightly different regexes. Recommend an exported folioAt(body, offset) -> {folio, lineInFolio} using the resolveFolioAt semantics and the '@012B.3' cite format already documented at textdiff.h:91. For co-occurrence-within-folio (§11 Relationships), split the body on the same regex (splitFolioSides pattern, main.cpp:12000).; Genre header for the study — Subjects::forWork(workKey) lists catalogued headings as Reference; isVinaya drives the existing vinayaBannerHtml (main.cpp:28014). Empty = 'No subject recorded'.

**Risks:** Direction mismatch: matchPeople/matchAuthors/buildTitleWorkbench all take a QUERY and rank a bank; none scans a running text. The study needs an inverse loop; do not call matchPeople per token (596 aliases x pronounce() per token would be slow and phonetic-noisy).; Phonetic and PhoneticNear tiers are documented guesses (authorsearch.h:21-27) and Tibetan names are homonym-dense; used for bulk text scanning they will generate false positives. Ship orthographic tiers only, or badge phonetic hits Machine with their evidence string and never auto-select.; Homonyms are never merged (rule 3): a matched name can map to several pids (persons_bdrc 'candidates'); the study must list all candidates and let the human disambiguate (personHtml pattern main.cpp:27908-27915).; 36 of 462 people have no recorded name (aliases empty) and are skipped by matchPeople (authorsearch.cpp:217); OCR-grade alias spellings ('GR AGS') mean text spellings not in the alias list will be missed — report 'not found' honestly, never fuzz beyond the defined tiers.; author_index.json, catalog_works.json and library/ are gitignored and absent on ordinary installs (main.cpp:35289-35296). A missing bank must read as 'index could not be read', never as 'no entities' (the pattern at main.cpp:26789-26799).; PLACES have no data source; THL kmaps bundling license is unclear (docs/TIBETAN_TOOLS_SURVEY.md:133-137). Any 'place' label without a cited source violates rule 3; n.prop is 'proper noun', not 'place'.

### Structured-data stores and exports (glossary TSV, terminology checker, alignment evidence/full-bank JSON, release package + spine meta, build/payload manifests, app CSV/TSV exports) — the inputs a key-based Table Compare would read

| API | Purpose | Anchor |
|---|---|---|
| `allcore::GlossaryStore` | Per-text glossary store. load() skips blank and '#' lines, splits on the FIRST tab, trims, drops rows with empty wylie or gloss, last duplicate wins (std::map). save() writes one '# per-text glossary for <doc>' header line then 'wylie\tgloss' rows sorted by wylie; flush+close before reporting success. Missing file = empty store, true. | core/include/allcore/glossary.h:16-36; core/src/glossary.cpp:13-59 |
| `allcore::checkTerminology / glossMatches` | In-memory only (no file format): extracts maximal dictionary spans with binding hgm_gloss from an ACIP passage and reports which HGM equivalents the English draft uses (TermUse{entry_id, wylie, tier, provisional, occurrences, glosses, matched}; SharedEnglish collapse warnings). MATCHES, never composes. Not a Table Compare input; a TermReport could be tabulated (key: wylie) if ever exported. | core/include/allcore/terminology.h:21-48; CTest terminology_smoke core/CMakeLists.txt:143-146 |
| `tools/build_alignment_layer.py main()` | Builds the evidence sidecar from banked HTML pages, gated letter-exact against the spine; REFUSES (sys.exit) on any failure: tib not in cited wylie, eng not in cited english, pair count < FLOOR 150, wylie->ACIP not 1:1, ACIP recovered for <95% of headwords. Headword key preserves case (a capital is a different letter) and collapses whitespace. | tools/build_alignment_layer.py:461, 555-569 (key + battery), 588-600 (1:1 gate), 616-656 (doc schema), 658-685 (writers) |
| `allcore::textdiff::diffInline` | In-cell (per-field) difference at Tibetan-syllable / word / char granularity — the natural engine for Table Compare's per-field diffs. | core/include/allcore/textdiff.h:64-67 |
| `allcore::textdiff::normalizeLine / Options` | The comparison key for a string under the ignore rules (case, whitespace, Tibetan punct, script-agnostic via canonical converters; unconvertible = raw + flagged). Public 'for the folder compare's rules-based criterion' — reusable as the cell-equality rule. | core/include/allcore/textdiff.h:19-31, 56-58 |
| `allcore::textdiff::apparatusCsv` | The app's only core-side CSV writer: header 'cite,kind,a_line,b_line,a_reading,b_reading', RFC-style double-quote escaping of text cells. House style for any new CSV report. | core/src/textdiff.cpp:626-633; used app/compare_pane.inc:377 and app/main.cpp:41751 |
| `allcore::diffTrees / TreeDiffResult` | Existing added/removed/renamed/modified classification model keyed by relative path (with an honesty tier: sampled hash). The record-level vocabulary Table Compare should mirror. | core/include/allcore/tree_diff.h:28-39 |
| `ComparePane::addPage / showPage / installComparePages` | How Folder Compare (page 1) and Three-Way Merge (page 2) are hosted in the Compare pane's QStackedWidget with the 'Text Compare · Folder Compare · Three-Way Merge' switcher buttons. A Table Compare page would be page 3 via the same calls. | app/compare_pane.inc:146-155 (switcher), 213-214 (addPage/showPage), 1327-1334 (installComparePages); registered app/main.cpp:38613-38618 |
| `FolderComparePage::report / saveSnapshot / compareWithSnapshot` | CSV report 'path,status,left_size,right_size,left_modified,right_modified'; snapshot JSON {root, taken, entries:[{path,size,mtime,sha1}]} keyed by path; snapshot compare unions keys from both sides then classifies LeftOnly/RightOnly/Identical/Different — the exact key-union algorithm a Table Compare needs, at file granularity. | app/compare_pane.inc:1008-1023, 953-983 |
| `allcore::CatalogRegister::loadText` | The app's only generic CSV/TSV table reader: delimiter auto-detected from the header (tab wins, else comma), quoted CSV fields handled (splitRow, anonymous namespace), columns found by case-insensitive header substring (number/catalog, title, folio, date, initials/by/who); key column = number, normalized padding-insensitively (S464 == S00464). Rows with no real number are dropped. | core/include/allcore/catalog_register.h:28-43; core/src/catalog_register.cpp:19-55 (splitRow), 60-117 |
| `allcore::worksheetSchema / worksheetCsvRow / serializeWorksheet / parseWorksheet` | 52-column live-sheet schema (first column 'ACIP Number'); CSV row writer with RFC 4180 quoting (csvCell, anonymous namespace); sidecar is 'Field\tValue' with \t \n escaping and ';' comments. | core/include/allcore/worksheet.h:19-33; core/src/worksheet.cpp:97-107 (csvCell) |
| `allcore::AbbrTable::load` | Reads the abbreviation CSVs: skips the first line (header + BOM), naive split on ',' (no quote handling), needs >= 4 columns, 5th = provenance. Loaded twice (TibSchol base + approved layer). | core/src/abbr.cpp:35-63; app/main.cpp:11001-11010 |
| `allcore::ProposalStore + tsvEscape/tsvUnescape` | Folder-backed TSV store (proposals.tsv) with a '# id\tkind\tstatus\t...' header; row identity = id; absorbs Dropbox 'conflicted copy' siblings by id, counts divergent same-id rows and never merges by guess. tsvEscape/Unescape are the only exported TSV field escapers in allcore. | core/include/allcore/proposals.h:81-127; core/src/proposals.cpp:94, 201-206 |
| `allcore::Spine::metaValue / corpusSegment` | Release identity of the loaded spine (release_version, source_master, source_master_sha256, n_entries, n_corpus_segments...) and the (course, seq) join point that alignment refs 'COURSE:seq' cite. | core/include/allcore/spine.h:63, 91-92; tools/build_spine.py:266-287 |

**Reuse for:** Table Compare: record classification (added / removed / changed / identical) — Mirror FolderComparePage::compareWithSnapshot's key-union loop (compare_pane.inc:970-982: union keys from both sides, sort case-insensitively, classify LeftOnly/RightOnly/Identical/Different) and TreeDiffResult's vocabulary (tree_diff.h:28-35). Keep the Status enum names the folder page already uses so the Show ▸ combo and CSV/HTML report pattern (compare_pane.inc:1008-1023) carry over.; Table Compare: per-field diffs and cell equality rules — allcore::textdiff::diffInline(a, b, Mode::Auto) for the highlighted in-cell spans (syllables for Tibetan cells, words for English) and normalizeLine(cell, Options) as the equality key so the pane's existing RULES ribbon (ignore case/whitespace/punct/script-agnostic) applies per cell; unconvertible cells compared raw and flagged, exactly as the line diff does (textdiff.h:41, 58, 66).; Table Compare: reading CSV/TSV inputs — Lift catalog_register.cpp's anonymous splitRow (quoted CSV, delimiter param) into an allcore header (e.g. a new core/include/allcore/table.h with its own smoke test in core/CMakeLists.txt) rather than writing a second parser; reuse its header-driven delimiter detection (tab in header wins, else comma). For the release TSV expect CRLF + QUOTE_MINIMAL (generate_package.py:6). For '#'-headed two-column TSVs (glossaries, links, pairs) supply implicit column names from the header comment and treat '#' lines as comments (glossary.cpp:20).; Table Compare: reading JSON tables — Follow the app's own QJsonDocument loaders: alignment_evidence pairs object -> rows keyed by object key with acip as a declared alternate key (main.cpp:38822-38852); alignment_full links array -> composite key course+page+id (main.cpp:38857-38875); BUILD_MANIFEST.json components[] keyed by path; snapshot entries[] keyed by path (compare_pane.inc:966-970).; Table Compare: release-to-release dictionary diff (docs/TEXT_ANALYSIS_SUITE_RESEARCH.md §14 'which entries changed in v0.27.3') — Key on wylie but treat the 56 duplicated wylie groups explicitly: group rows by key and diff the groups as multisets (or offer composite key wylie+tibetan, still 10 collisions) and surface 'ambiguous key' rows as a flag rather than pairing by position (rule 3). Diff hgm_glosses as a '; '-split list so provisional-tier gloss changes read per gloss; hgm_evidence/pron_source/status columns carry the provenance and MUST be shown (rule 4). Do not use spine entries.id across releases (per-release ordinal, build_spine.py:83). Show the two release identities from master meta{version,date} / Spine::metaValue('release_version') in the header.; Table Compare: glossary version compare (COMPARE_SUITE_PLAN.md:63) — Two GlossaryStore paths (library/glossaries/<A>.tsv vs <B>.tsv) or a glossary vs a spine lookup: load with allcore::GlossaryStore::load() and diff all() maps by wylie — one value column (gloss). The Glossary Workbench (main.cpp:17105) is the natural launch point ('Compare with…'); reuse g_compareFiles-style globals (compare_pane.inc:7-9) to route into the Compare pane.

**Risks:** wylie is NOT a unique key in the release master (56 duplicate wylie values in 105,634 unified_entries; (wylie,acip) also 56; (wylie,tibetan) 10). A Table Compare that assumes uniqueness would silently pair the wrong rows — violates rule 3 unless duplicates are flagged as ambiguous-key groups.; Spine entries.id is a per-release ordinal (build_spine.py:83 'stable ordinal in release order', :178 enumerate) — must never be used as a cross-release key.; Corpus segments carry no seq in full_parallel_corpus_v32.json.gz; 'COURSE:seq' is positional per course (build_spine.py:238-242). Any alignment ref key is invalidated by a corpus insertion; a corpus-to-corpus table compare needs a content key (course + acip text), not seq.; Release TSV drops provenance fields present in the JSON (tibetan_source, wylie_variants, corpus_occurrences detail; hgm_evidence reduced to a tier string) — comparing TSVs can hide provenance changes (rule 4). Prefer the JSON master for a rule-4-faithful compare, or say on the report that the TSV view is lossy.; Release TSV is CRLF with QUOTE_MINIMAL (generate_package.py:6); a naive line splitter leaves '\r' in the last cell (GlossaryStore strips it, glossary.cpp:19; AbbrTable strips it, abbr.cpp:45 — a new reader must too).; AbbrTable::load splits on ',' with no quote handling (abbr.cpp:50) and regenreg writes csvField-quoted cells (regenreg.cpp:79, 196-200) into a file named .tsv but comma-separated — an abbreviation containing a comma would misparse today; a Table Compare must not inherit this parser.

### Apparatus (footnote/bibliography bank), Draft/Manuscript document I/O, docprops sidecar, and the export writers (RTF hand-rolled / textutil / QPrinter) — as they stand in app/main.cpp; plus the compare engine hunks in allcore::textdiff that a tracked-changes exporter would consume.

| API | Purpose | Anchor |
|---|---|---|
| `DraftPane::loadNotesBank / dataFile` | Lazily loads the OFFICIAL bank into notesBank_ (BankNote{source,int note,lemma,text}) from data/extracted/mixed_nuts_notes.json + mixed_nuts_notes_older.json and bibBank_ (BibEntry{id,source,text,acipRefs}) from mixed_nuts_bibliography.json; then loadCandidates(). dataFile() resolves <appDir>/../../../../../data/<name> or cwd/data/<name>. | app/main.cpp:22918-22961 |
| `DraftPane::searchNotesBank` | Substring search (lemma\|text, case-insensitive) over notesBank_ (cap 10), bibBank_ (text\|acipRefs, cap 8) and candBank_ (cap 5); renders HTML into report_ with anchors note:<ix> / bib:<ix> / cand:<ix>. Candidates are fenced amber with '⚠ PENDING — not GMR-approved'. | app/main.cpp:22963-23031 |
| `DraftPane report_ anchor dispatch` | The only path that inserts bank material into the draft. Inserts PLAIN TEXT bracket markers at the cursor: "[NOTE: <lemma>: <text> — reused from <source>, n.<n>]" (23095-23102), "[BIBLIOGRAPHY: <text> — entry <id> as published in <source>]" (23494-23501), "[NOTE — PENDING, not GMR-approved: <lemma>: <text>]" (23503-23510). No numbering, no footnote object, no back-reference to the bank id beyond the literal text. | app/main.cpp:21972-21981 |
| `DraftPane::proposeNote / proposeNoteText` | Takes the draft selection (or the Manuscript's selection via g_mssProposeNote), splits 'lemma: text' at the first colon (fallback lemma = first 40 chars), appends {lemma,text,proposed(ISO),status:'pending GMR approval'} to data/candidate_notes.json via saveOrWarn, reloads candBank_. No in-app approve path by design. | app/main.cpp:23044-23093 |
| `DraftPane::composeBibDialog / autoFillBib` | Modal form → allcore::composeBibliographyEntry (STD-007) or composeSanskritBibEntry (DCC Sanskrit template); live preview; 'Pair-hyphenate author' (allcore::hgmTechnicalSpelling); 'Auto-fill from catalog' reads data/extracted/catalog_works.json + catalog_titles.json (ACIP → EWTS via acipToEwts). Buttons: 'Insert into draft' → plain marker "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): <entry>]"; 'Save as candidate' → data/candidate_bib.json {entry,acip_no,proposed,status}. | app/main.cpp:23229-23492 |
| `DraftPane::detectQuotes` | allcore::detectQuotations(spine_, source, isAcip, 7) — attested ≥7-syllable corpus match; recommends bank footnotes whose lemma occurs in the matched segment's published English (note:<ix>) and bib entries whose acipRefs contain the course id (bib:<ix>). Uses the same insert markers. | app/main.cpp:23104-23171 |
| `DraftPane::exportRtf` | Hand-rolled RTF writer: *asterisk* runs → {\i …}; escapes \ { }; non-ASCII → \uN?; \n → \par; Times New Roman 12; unclosed asterisk closed and WARNED. Bracket markers export as literal text. Verdict via saveOrWarn (WP-6). | app/main.cpp:23173-23227 |
| `DraftPane file ops` | Draft is a QPlainTextEdit (draft_) saved as UTF-8 plain text (draftPath_); dirty = docprops::digest(text) != savedDigest_; save applies editops::applyHouseStyleSpacing when the pref is on; every save calls docprops::noteSave (sidecar revision/editing time). Print = QTextDocument::print(QPrinter). | app/main.cpp:22044-22140 |
| `ManuscriptPane core I/O` | Manuscript is a QTextEdit (editor_, acceptRichText) persisted as Qt HTML via editor_->toHtml() (save, 33529) / setHtml (openFile, 33514), 'Manuscript (*.html)' filter (33662). Autosave every minute when dirty_ && !path_.isEmpty() (33500-33503). exportRtf saves first, then QProcess 'textutil -convert rtf <path_> -output <fn>' (33671-33684) — macOS-only, exit code is the only verdict. setManuscriptText uses setPlainText (33720) so Draft → Manuscript hand-off drops nothing but also carries markers as plain text. | app/main.cpp:33211-33720 |
| `ManuscriptPane rich-text helpers` | All edits go through QTextCursor on editor_. insertHyperlink uses c.insertHtml("<a href=…>…</a>") — anchors round-trip through toHtml/setHtml (selftest 33800 relies on it). House styles (Body/Heading/Section/Verse/Colophon) are QTextBlockFormat+QTextCharFormat merges registered in styles_ (33268-33367). | app/main.cpp:33562-33600 |
| `docprops namespace` | Word-style File menu + Properties window shared by Overlay/Draft/Manuscript. Because .txt/.html cannot carry properties, they live in a JSON SIDECAR <dataRoot>/library/properties/<basename>.json (revision, editingSeconds, lastSavedBy (g_userName or $USER), lastSavedAt, bookmarks, Summary/Custom fields). Pure functions are selftested; showDialog returns early under g_harnessRun. | app/main.cpp:3925-4269 |
| `Cross-pane hooks (ladder seams)` | Manuscript's PUBLISH row delegates to DraftPane (bib dialog inserts into the DRAFT, not the manuscript: 38579-38582, 33390-33397). Insert menu: 'Footnote…' → draftPane->proposeFootnote(); 'Footnote from the Bank…' → draftPane->focusApparatusSearch() (40147-40148). Official bank for the Apparatus pane / global search: g_appNotes / g_appBib loaded at 38504-38561 from data/extracted/apparatus_notes.json (+apparatus_anchors.json, apparatus_bibliography.json) with fallback to the mixed_nuts_* files. | app/main.cpp:3461-3467, 38493-38582, 40147-40148 |
| `allcore::textdiff (compare engine)` | Pure C++ line diff (Myers), in-line syllable/word/char spans (byte offsets), diff3 merge, and report writers. Hunks cover both inputs completely in order (Equal/Insert/Delete/Change) — the natural input for w:ins/w:del generation. ComparePane holds aLines_/bLines_ as QStringList (compare_pane.inc:298-299) and res_ (Result); report(kind) at compare_pane.inc:370-379 dispatches html/patch/md/csv/txt; saveReport at 380-388. | core/include/allcore/textdiff.h:36-74, 85-107; app/compare_pane.inc:135-140, 358-388 |
| `allcore::translationPrepToRtf` | The only core-side document writer: RTF with Palatino Linotype 12, \qj, \titlepg + \footer \chpgn page numbers, [n] note markers with an appended notes list (tibexport.cpp:541-546). Used by the Overlay 'Save translation prep' path (main.cpp:14916-14930). No .docx writer exists anywhere in app/ or core/ (CLOSER #49 marks a native OOXML writer as backlog). | core/src/tibexport.cpp:470-550; core/include/allcore/tibexport.h:130-138 |

**Reuse for:** Insert Citation (field-coded) — data source — Reuse DraftPane::loadNotesBank/bibBank_ (BibEntry.id + source + text + acipRefs) and the g_appBib global loaded at main.cpp:38548-38561; do NOT add a third loader. The stable key already present is (source, id); acip_refs gives the work identity for quotation-driven citation (detectQuotes at 23104-23171 already matches course id against acipRefs). Official entries insert VERBATIM (rule 1: match, never compose); candidate_bib entries must carry the PENDING label the existing insertCandidate/insertBibEntry markers use.; Insert Citation — in-document representation — Manuscript is QTextDocument HTML; the one construct proven to round-trip through toHtml/setHtml is the anchor (insertHyperlink, 33572-33577; selftest 33800). A field code can therefore be an <a href="cite:<source-key>/<id>">[S1]</a> span (inferred design; QTextFormat::UserProperty does NOT survive the HTML save — inferred from Qt behaviour, not verified in this repo). For the plain-text Draft the existing bracket-marker convention ([BIBLIOGRAPHY: … — entry S1 as published in …]) is the only carrier; a parsable short form would need to be defined and its grammar put in allcore so both panes and the exporters share one parser.; Regenerating bibliography — Pure core function (new core/src/citefield.cpp + core/include/allcore/citefield.h, smoke in core/tests, registered in core/CMakeLists.txt like textdiff_smoke at :148-150): scan document text/HTML for cite fields → ordered distinct keys → render a 'Bibliography' block by looking up the verbatim published text from apparatus_bibliography.json rows (id+source), sectioned by the bank's own 'section' field (Sanskrit vs Tibetan works, as the published volumes do). Unresolvable keys are FLAGGED in the block, never dropped (rule 3). On the Qt side, regenerate by replacing a delimited block (e.g. a bookmark anchor pair) via QTextCursor, and keep docprops::noteSave semantics unchanged.; Footnotes with real numbering (docx w:footnoteReference) — Today footnotes are inline bracket text with no numbering; the closest existing model is TranslationPrep.notes + [n] markers (tibexport.h:151-160, rendered in translationPrepToRtf at tibexport.cpp:541-546). A docx footnote export needs: a footnote object list (text, source citation, tier) + a reference marker in the run stream. Extend the marker grammar from the existing [NOTE: …] insertions rather than inventing a second one, and keep 'reused from <source>, n.<n>' as the mandatory attribution (STD-008).; Native .docx writer (prerequisite for both features) — No OOXML writer exists; the two RTF writers (23173, tibexport.cpp:470) and textutil are the only formatted exports. CLOSER.md #49 already scopes it: zip container + [Content_Types].xml, _rels/.rels, word/document.xml, word/styles.xml, sectPr (+ word/footnotes.xml, word/_rels for footnotes). zlib is linked to allcore only in the USE_SYSTEM_SQLITE=ON branch (core/CMakeLists.txt:79); STORED zip entries need only CRC32 (zlib's crc32) so a Deflate dependency can be avoided. Put the writer in allcore (no Qt) with a smoke test that unzips the output and validates document.xml well-formedness; convert QTextDocument → an intermediate paragraph/run model in a new app/*.inc, mirroring how ManuscriptPane already reads block/char formats (33562-33600).; Tracked-changes .docx (w:ins/w:del) — Consume allcore::textdiff::Result directly: hunks are contiguous and cover both texts (textdiff.h:36-51); Equal → plain runs, Delete → <w:del><w:r><w:delText>, Insert → <w:ins><w:r><w:t>, Change → del then ins; refine Change hunks with diffInline(a,b) Span byte offsets (textdiff.h:66-68) for syllable-level w:ins/w:del inside a line — the same call the pane already makes at compare_pane.inc:567 and 611. Author/date for w:ins come from g_userName / docprops::noteSave's lastSavedBy logic (main.cpp:3965-3978). Expose as a new report kind in ComparePane::report/saveReport (compare_pane.inc:370-388) so the Save-report dialog gains 'Word, tracked changes (*.docx)'. Hunks with unimportant=true or rawNormalised=false must be surfaced (e.g. a w:comment or a leading paragraph), never silently merged (rule 3).

**Risks:** Footnotes/citations have NO object model today: they are literal bracket text in a QPlainTextEdit (Draft) or plain runs in QTextEdit HTML (Manuscript). Any 'Insert Citation' that introduces field codes must also define how the existing [NOTE: …]/[BIBLIOGRAPHY: …] markers in already-saved drafts are treated (parse them, or leave them as legacy text) — otherwise two incompatible conventions coexist.; Two divergent RTF writers (DraftPane::exportRtf at main.cpp:23173 — Times New Roman, *italics* only; allcore::translationPrepToRtf at tibexport.cpp:470 — Palatino, justification, page numbers) plus a third path (textutil from HTML) — a docx writer would be a fourth formatted-export path unless the Manuscript/Draft exports are unified behind one document model.; Manuscript RTF export depends on macOS /usr/bin/textutil (main.cpp:33679) and reports only the exit code; it is not cross-platform (CLAUDE.md targets Windows/Linux for input centers). Library .docx import likewise (27557-27569).; textutil DROPS the RTF \chpgn page-number field (CLOSER #49 measured caveat); Word's own handling is still unverified — any 'export to Word' promise must be tested against Word, not textutil.; The bank JSONs are located by a relative walk from applicationDirPath (dataFile(), main.cpp:22953-22958) with a cwd fallback; installed builds without the repo layout silently get empty banks and the search reports 'apparatus banks not found'. Field-coded citations that resolve against the bank at regenerate time inherit this fragility.; The Apparatus pane and DraftPane load DIFFERENT files (apparatus_notes.json unified store vs mixed_nuts_notes*.json) — main.cpp:38504-38509 vs 22926-22927 — so note indices and counts differ between surfaces; a citation key must be (source,id)/(source,note), never a vector index like today's note:<ix> anchors.

### Find-in-files / file-browsing surfaces: FilesPane (dual-pane file browser), GoferPane (the "Search" tab = the app's Find in Files), OverlayPane::openFile (the one door into a document), editops::FindDialog + the Find menu in main() — mapped for a future "Replace in Files with mandatory preview"

| API | Purpose | Anchor |
|---|---|---|
| `FilesPane::FilesPane` | Dual-pane browser (QFileSystemModel + FilesDirProxy folders-first proxy). openText is the callback used for .txt/.act/.inc/.acip/.md; main() wires it to overlay->openFile + g_raisePane. | app/main.cpp:23822 (ctor), app/main.cpp:38604 (instantiation) |
| `FilesPane::selectedPath` | Current-index path of pane ix (0 left, 1 right) via proxy_->mapToSource; empty when no current index. | app/main.cpp:24251 |
| `FilesPane::selectedPathsIn` | All selectedRows(0) of pane ix as absolute paths; falls back to selectedPath(ix) when the selection is empty. Public. This is the natural 'files to operate on' source for any batch op. | app/main.cpp:24279 |
| `FilesPane::openPath` | Dispatch by extension: text exts -> openText_; png/jpg/tif -> g_openScanInInput; zip/tar/tgz -> browseArchive; else QDesktopServices::openUrl. | app/main.cpp:24257 |
| `FilesPane::addToStack` | Public: add a path to the Drop Stack shelf (persisted in QSettings files/stack). | app/main.cpp:24011 |
| `FilesPane context menu (per view)` | Actions: Add to Drop Stack · Open · Compare with the other side's selection (g_compareFolders/g_compareFiles) · Compare the two selected (when exactly 2) · Reveal in Finder · Open in Terminal here · Move to Trash · Finder tags (disabled display row via mdls). NO find/search/replace item exists here today. | app/main.cpp:24075-24160 |
| `FilesPane workspaces` | Workspace = the set of tab paths per pane, saved under QSettings files/workspaces (names) and files/ws_<name>_<ix> (paths). Private; the Workspaces ribbon button (wsB_/wsMenu_) is the only surface. main() Project menu 'Files Workspaces…' just raises the pane. | app/main.cpp:25368 (rebuildWsMenu), 25359 (applyTabs), 25439 (saveTabs), 40463 (menu) |
| `FilesPane::eventFilter` | Tracks active_ pane on FocusIn; opt-in Commander keys F3/F4 open, F5 copy, F6 move, F7 new folder, F8 trash (confirms multi). | app/main.cpp:25459 |
| `FilesPane preview-then-apply dialogs (patterns to copy)` | Two existing 'mandatory preview' dialogs: batchRename shows old -> new per file (pure computeRenames) with a single Apply and never overwrites; syncFolders builds a plan (pure computeSyncPlan) listed as rows, Apply copies. Neither has per-row tick/untick and neither writes through saveOrWarn (uses QFile::rename / QFile::copy). | app/main.cpp:25156 (batchRename), 25131 (computeRenames), 24969 (syncFolders), 24418 (computeSyncPlan) |
| `GoferPane::searchFor` | Find-in-Files entry (2026-09-08): seeds fields_[0] with q and calls find(). Does not clear the other 7 term boxes (runQuery does). | app/main.cpp:18213 |
| `GoferPane::runQuery` | Hunt palette hook: clears all 8 fields, sets fields_[0], combiner OR, find(). Bound to g_goferQuery in main(). | app/main.cpp:18408, app/main.cpp:38620 |
| `GoferPane::find (private)` | The actual find-in-files. Builds a Gofer query from up to 8 terms (buildQuery: OR / NEAR/1000000 for 'AND (same file)' / NEAR/n), then per checked source row: [0] corpus via allcore::goferSearch + spine_.corpusCount; [1] apparatus (g_appNotes/g_appBib substring); [2] Spotlight via mdfind; rows >= 3 = user folders: if <dir>/.index.db exists -> allcore::LibraryIndex::search(q, 400, &cut, &stats, pump) else allcore::goferSearchFiles(dir, q, 401, &scan). Hits roll up per file (count, first line, snippet) into HTML with 'goferopen:<enc path>\|<line>' anchors; caps disclosed. Re-entrancy guard finding_, Stop via stopped_ + pump. Results are HTML in results_ (QTextBrowser) only — there is NO structured hit list exposed to other panes. | app/main.cpp:19008-19439 (find), 18962 (buildQuery), 19216-19275 (index vs scan branch), 19354-19372 (goferopen anchors) |
| `GoferPane results anchorClicked` | goferopen:<path>\|<line> -> g_openAtLine(path, line); lookup:<term> -> g_lookupQuery; file:// -> QDesktopServices. | app/main.cpp:18370-18393 |
| `GoferPane::goferFoldKeep (public static, pure)` | Strict post-filter for the fold combo (0 ignore space+caps, 1 ignore space only, 2 exact). Pinned in selfTest. | app/main.cpp:18987 |

**Reuse for:** Replace in Files — scope / file set — Take the file list from FilesPane::selectedPathsIn(active pane) (public, app/main.cpp:24279) when invoked from the Files pane context menu, or from GoferPane's checked user-folder rows (dirs_ rows >= kFirstDirRow, app/main.cpp:19205-19210) when invoked from the Find menu. Walk folders with the same eligibility as goferSearchFiles/LibraryIndex (.txt .acip .md .act .inc .ace, <= 10 MB — core/src/gofer.cpp:91-93, core/src/libindex.cpp:43-44) plus Notepad++-style include/exclude globs via QDir::match as FolderComparePage::scan does (app/compare_pane.inc:1093-1103).; Replace in Files — matching semantics — Do NOT reuse the Gofer grammar for replace: it is term/OR/NEAR retrieval with no capture groups or whole-word (core/src/gofer_ast.h). Reuse editops::FindOpts + patternFor (app/main.cpp:4318-4331) to build the QRegularExpression, expandReplacement for $N groups (4435) and shapeCase for preserve-case (4426); apply them to each file's text in memory (QString::replace via globalMatch, back-to-front as replaceAll does at 4473-4490). Optionally pre-filter candidate files with LibraryIndex::search / goferSearchFiles (quoted literal needle) to avoid reading a whole Kangyur, but treat that as a candidate set only — the index matches whole tokens, so the regex pass over the actual file text is the truth.; Replace in Files — reading and writing files faithfully — Read with cmp::readText (app/compare_pane.inc:20 — keeps raw bytes, detects EOL and binary) or, for encoding parity with the Overlay, enc::decode/enc::encode (app/main.cpp:4966/4993, refuse on unmappable — rule 3). Write back through saveOrWarn(parent, path, bytes, "Replace in Files") (app/main.cpp:2952) and gate every success count on its return, exactly as OverlayPane::writeDocumentTo does (8303-8325) and ComparePane::saveSide does (compare_pane.inc:358-366, joins with the side's original EOL).; Replace in Files — mandatory per-file diff preview with tick/untick — For each file compute old lines vs proposed lines with allcore::textdiff::diffLines (core/include/allcore/textdiff.h:62) and render hunks; reuse cmp::colFor/kindName/buildRows (compare_pane.inc:43-82) or allcore::textdiff::unifiedDiff/sideBySideHtml for text. A QTreeWidget with checkable file rows and checkable hunk children is the missing piece (no existing dialog has per-row ticks: batchRename 25156 and syncFolders 24969 have a single Apply; FolderComparePage::confirmOps 1133 is list+OK). Double-click a row -> g_compareTexts(name+' (before)', oldText, name+' (after)', newText) to inspect in the Compare pane (compare_pane.inc:9, main.cpp:38616). Provide a harnessAutoConfirm_-style flag so selfTest can drive Apply (compare_pane.inc:1024, 1133).; Replace in Files — undo set — Before the first write, allcore::backupFile(path, backupDir, stamp, keep) for every file about to change (core/include/allcore/backup.h:16), one shared stamp per Apply (app convention: yyyy-MM-dd-HHmmss under AppDataLocation/backups, main.cpp:38721-38745); record the set (stamp + paths) so an 'Undo this replace' action can allcore::restoreBackup each (backup.h:32) via restoreStoreWithSafety (main.cpp:3022) which backs up the current state first. Note backupFile flattens to '<basename>.<stamp>.bak' — two files with the same basename in different folders would collide (see risks). TODO.md:3124 ties this to the still-unbuilt 'Versions per document' store; nothing named Versions exists in code yet (grep empty).; Replace in Files — refreshing open documents — There is no QFileSystemWatcher in main.cpp. After Apply, if OverlayPane::documentPath() (app/main.cpp:8329) equals a rewritten path and !isDirty() (8498), call revertDocument() (8481) or openFileAtLine (15269) to reload; if it IS dirty, surface the conflict in the result window rather than overwrite (rule 3). ComparePane::reload() (compare_pane.inc:295) covers the Compare pane similarly.

**Risks:** Two different match semantics already coexist and are documented as such: LibraryIndex::search matches whole FTS tokens, goferSearchFiles matches raw case-insensitive substrings (core/include/allcore/libindex.h:6-9). A Replace-in-Files preview seeded from Gofer hits would disagree with a regex replace pass unless the regex pass is the sole truth; the design must not present Gofer hit counts as 'will be replaced' counts (house rule 3).; The Gofer grammar (gofer_ast.h) has no regex, whole-word, capture groups or replacement syntax; editops::FindOpts/patternFor is QRegularExpression-based. Reusing FindDialog toggles for an in-files replace means a separate regex scan, not GoferPane::find — do not bolt replace onto GoferPane::find, whose results are HTML only (results_->setHtml, app/main.cpp:19439) with no structured hit model.; goferSearchFiles reads with std::getline and strips CR (core/src/gofer.cpp:113-118) — it loses encoding and EOL information; it is a search primitive, not a read-for-rewrite primitive. Use cmp::readText or enc::decode for any file that will be written back, and preserve EOL as saveSide (compare_pane.inc:362-363) and writeDocumentTo (main.cpp:8309-8310) do.; allcore::backupFile names backups '<basename>.<stamp>.bak' in a flat backupDir (core/include/allcore/backup.h:16-22): two files with the same basename in different folders (common in ACIP collections, e.g. many 'KL0001MA.ACT'-style names across volumes) would overwrite each other's backup under one stamp. An undo set needs a per-Apply subdirectory or a path-derived name.; No QFileSystemWatcher exists (grep empty). Overlay's savedDigest_/isDirty (main.cpp:8498) compares the box to the last opened/saved content, not to disk; a Replace-in-Files write to the currently open, dirty document would be silently overwritten by the next Overlay Save. The Apply step must check documentPath()/isDirty() first.; OverlayPane::saveDocument asks before overwriting files under /library/ (main.cpp:8282-8290) — 'canonical texts are not casually rewritten'. A Replace in Files over library/ must carry the same (or stronger) guard; FilesPane's default left tab IS root + '/library' (main.cpp:24185-24190), so the default scope points at the canonical collection.

### Text-analysis / statistics precursors: AnalysisPane (LLM passage analysis), docprops::textStatistics (Word-style Properties ▸ Statistics), allcore textdna/verse, the Gofer NEAR grammar + evaluators, the Search pane (GoferPane) and Draft-pane concordance, and the Translator's Survey — i.e. everything that currently counts, ranks, or windows text, mapped for a future Statistics study view (frequency, type/token, hapax, sentence length by shad, particle density, Trends, Collocates, Phrases).

| API | Purpose | Anchor |
|---|---|---|
| `AnalysisPane` | The Research-group 'Analysis' pane: pastes one ACIP passage (+ optional draft), runs allcore::runPrePass (dictionary anchors + corpus concordance), optionally adds LibraryIndex citations (up to 4 phrase anchors x 3 hits), streams a Claude /v1/messages report (SSE), then runs allcore::validateReport and auto-saves report+QC as markdown. It is an AI-report pane, NOT a statistics pane; its only 'report types' are the model's markdown and the QcFinding list. | app/main.cpp:3666 (class), :3733 run(), :3743 runPrePass, :3748-3780 LibraryIndex citations, :3787-3800 request, :3814 onData, :3843 onDone, :3868 validateReport, :3874-3893 auto-save; constructed at app/main.cpp:38485 |
| `allcore::runPrePass / AnalysisPrePass` | Engine stage 1 for Analysis: tokenizeAcip, greedy longest-match spine.lookup() per position (TermAnchor carries hgm_gloss verbatim + tier), then corpusSearch(quoted anchor, "", 2) per anchor until corpus_limit (6) hits. Small, capped, prompt-oriented — not a counting pass. | core/include/allcore/analysis.h:21-62; core/src/analysis.cpp:55-103 (spine.lookup :67, corpusSearch :97) |
| `allcore::validateReport / QcFinding` | Stage-3 machine QC over the AI report (reconstruction-a/b, acip-coverage, hgm-anchor, deviation-mark, acip-integrity, parse). Only 'report type' the Analysis pane has besides raw markdown. | core/include/allcore/qc.h:21-35 |
| `docprops::textStatistics` | Word-style document statistics. Always: characters, charactersWithSpaces, lines, paragraphs. If tibetan: folios = count of /@\d{2,3}[AaBb]\b/, syllables = [A-Za-z']+ runs after stripping @markers and {…}/[…] apparatus, shads = count of ',' in that body, words = caller-supplied tokens, dictionarySpans, entriesResolved. Else words = \S+ runs. ACIP-only regexes; pure; pinned in selftests. | app/main.cpp:3980-4012 (regexes :3983-3988); countMatches :3938; callers :4161 (Statistics tab), :7952 and :22282 (selftests) |
| `docprops::Input / showDialog / propertiesInput` | The Properties window (General/Summary/Statistics/Content/Custom). Overlay supplies tokens=doc_.tokens.size(), spans=doc_.spans.size(), entries=doc_.entries.size(); Draft and Manuscript pass tibetan=false. Tools ▸ Word Count… opens tab 2 (Statistics) of whichever pane is active. Returns early under g_harnessRun (modal). | app/main.cpp:4079-4088 (Input), :4093-4257 (dialog; Statistics tab :4151-4179), Overlay :8646-8660, Draft :22127-22135, Manuscript :33649-33657, Word Count menu :40301-40305 |
| `allcore::textDna / DnaReport` | Per-source-line structure strip: syllables per line (segments split at , ; / then tokenizeDocument), class vs the dominant meter from analyzeVerse, and a ZHES/CES/SHES + speech-verb quote mark. Pure measurement; smoke-tested. This is the existing per-line length series a 'sentence length by shad' histogram could be derived from. | core/include/allcore/textdna.h:14-36; core/src/textdna.cpp:46-100; core/tests/textdna_smoke.cpp; core/CMakeLists.txt:72,392-394 |
| `allcore::analyzeVerse / VerseReport` | Splits at newline/CR and ',' ';' '/' (ACIP shad marks), tokenizes each unit, records syllables per unit, computes the mode of syllable counts (std::map<int,int> counts) and declares verse when mode>=5 covers >=70% of units (min 4 units). The `counts` map IS a shad-delimited unit-length distribution, but it is local and not exposed. | core/include/allcore/verse.h:16-31; core/src/verse.cpp:9-55 (split :13-20, counts :38-45) |
| `Gofer grammar (gofer.h / gofer_ast.h)` | Proximity search over corpus segments where NEAR/N means within N LINES (seq) of the same course (default 3). Lexer accepts NEAR/n or NEAR-n, coalesces adjacent bare terms into a phrase, caps 200 paren depth and 5000 terms. Corpus evaluator: TERM -> spine.corpusSearch('"term"', course, 2000) windows; OR = union; NEAR = same-course join with seq gap <= n. Unit is lines, not syllables — a syllable-window Collocates view cannot reuse evalNode as-is. | core/include/allcore/gofer.h:6-31; core/src/gofer_ast.h:20-79 (lex), :92-150 (Parser); core/src/gofer.cpp:20-62 (evalNode); battery core/tests/gofer_smoke.cpp |
| `allcore::goferSearchFiles / GoferScan` | Same grammar over a folder of ACIP text files (.txt/.acip/.md/.act/.inc/.ace, <=10 MB): case-insensitive substring per line, NEAR/N within N file lines, never across files. Bounded (file_cap, kWindowCap 200000 = LibraryIndex::kScanCap) and reports what it skipped (rule 3). | core/include/allcore/gofer.h:37-72; core/src/gofer.cpp:66-120 |
| `allcore::LibraryIndex::search` | Prebuilt FTS5 index over the library (files/lines/lines_fts; text + affix/paba/lemma-folded text_norm). Speaks the Gofer grammar; per-node scan bounded by kScanCap with disclosure. The only fast path for term counts ACROSS a collection (Trends). | core/include/allcore/libindex.h:100-123; schema core/src/libindex.cpp:77-93 |
| `allcore::Spine corpus API` | Read-only spine access. corpusCount is the uncapped total (returns -1 when FTS5 rejects the query, e.g. NEAR/N grammar) — the mandated companion to any capped corpusSearch page. corpusWindow(course, 0, 1<<30) is how the Survey streams a whole course. No frequency/token table exists in the spine: tables are meta, entries, entry_variants, entries_fts, english_fts, corpus_segments(id, course, seq, acip, wylie, english, raw), corpus_fts(wylie, english, acip, wylie_norm), pron_index, reverse_index. | core/include/allcore/spine.h:40-47 (CorpusSegment), :70 tierCensus, :96-115 corpus methods, :135 allAcipHeadwords; tools/build_spine.py:80-150 (schema; corpus_segments :127-141) |
| `GoferPane (Search pane)` | Eight term boxes + combiner compile to Gofer grammar: OR -> terms joined ' OR '; 'AND (same file)' -> ' NEAR/1000000 '; NEAR -> ' NEAR/n ' (n<=0 => 3). find() runs, per checked scope row: corpus (goferSearch limit 60 + corpusCount + corpusCountLine, renders 20), published apparatus (g_appNotes/g_appBib substring), Spotlight (mdfind), and each user folder (LibraryIndex::search limit 400 with pump, else goferSearchFiles 401 with GoferScan), then rolls file hits up PER FILE (struct FileRoll {count, firstLine, snippet}) sorted by count desc, disclosing caps and skipped files. | app/main.cpp:18209 (class), :18213 searchFor, :18962-18975 buildQuery, :18987 goferFoldKeep, :19008 find, :19035-19077 corpus branch, :19079-19128 apparatus, :19157 mdfind, :19224-19275 index/unindexed branches, :19282-19310 FileRoll rollup, :19422 goferSkipNotice; row labels :18904-18909; tab added :38624 |
| `DraftPane::showConcordance` | KWIC-style concordance card for one term: corpusSearch('"wylie"', "", 200), corpusCount for the true total, std::map<std::string,int> byCourse over the 200 FETCHED (explicitly labelled 'NOT all N' when total > fetched), shows 8 segments [course:seq] wylie/english, then tm84000Html + linkOutHtml. Reached from anchor links in the Draft's source-clause view; pinned by Draft selftests (uncapped total, cap disclosed). | app/main.cpp:22616-22684 (byCourse :22633-22634; kFetch/kShow :22626-22627); link dispatch :22023; selftest pins :22164-22181; class DraftPane :21530 |
| `translatorSurveyMarkdown / showTranslatorSurvey` | The closest existing thing to a per-text statistics report: buildOverlay over the file (static HeadwordIndex), per-token innermost-span tier attribution (covered/curated/glossary/PROVISIONAL/reference-only), std::map<std::string,int> unknown = frequency table of UNCOVERED token forms (Top 15 sorted desc, with remainder line — DATA-6), inverted 7-gram verbatim quotation scan over every corpus course (skips TITLK/TITLT/TITLS/TITLR/SUBJ/AUTH), extractStructure (bam po / le'u / 30-syllable shloka estimate), extractOutline node count, analyzeVerse form, and a difficulty ESTIMATE with formula shown. Rendered as markdown in a QDialog with Save as Markdown; also the CLI --survey <file>. | app/main.cpp:37829-38072 (unknown map :37866; tier attribution :37862-37886; 7-gram scan :37887-37970; Top unknown forms :38008-38034; structure :38044-38057; difficulty :38059-38070); dialog :38074-38108; CLI :41757; selftest :41996-42013; hook g_surveyFile :38426-38428 |

**Reuse for:** Frequency tables (syllable and word/span), type/token ratio, hapax list — Tokenize with allcore::tokenizeDocument (lattice.h:45) for syllables and buildOverlay(spine, HeadwordIndex, text) (lattice.h:81) for dictionary spans; count into a std::map exactly as translatorSurveyMarkdown does for `unknown` (app/main.cpp:37866-37878) but over ALL tokens, and offer the searchFoldWylie lemma fold (searchnorm.h:51) as a 'by lemma' switch. Type/token = map.size()/tokens; hapax = entries with count 1. Render as a ranked table with the remainder-line idiom from :38028-38033 (never a capped list without its cut disclosed) under ux::sourceBadge(Epistemic::Machine) (ux_tokens.h:70). Put the engine in a new core/src/textstats.cpp + core/include/allcore/textstats.h with a textstats_smoke registered like textdna (core/CMakeLists.txt:72, :392-394); keep it Qt-free.; Sentence-length distribution by shad — The unit split already exists twice: analyzeVerse splits at \n \r , ; / and records syllables per unit, with a local std::map<int,int> counts histogram (core/src/verse.cpp:13-45); textDna gives per-source-line syllables and classes (textdna.cpp:46-100). Expose the histogram (unit lengths, mean/median, verse meter) from the new engine rather than duplicating the split; docprops::textStatistics already reports the raw shad count (app/main.cpp:3999). For a clause-level series use splitClauses (reader.h:50).; Particle density — Walk tokens; count classifyParticle(tok) != nullptr and splitFusedEnding(tok) hits (particles.h:24, :35), group by ParticleInfo::group/function; density = particles / tokens. Report 'freestanding vs fused' separately — the header's rule that closed syllables are never split (particles.h:8-10) must hold, so use only the engine's verdicts.; Trends of a term across folios (within one text) — No per-token folio walk exists. Build one by scanning the raw text for @\d{2,3}[AaBb]\b (the regex at app/main.cpp:3983; acipFolioRange at catalog_list.h:51 gives the range) and attributing each token to the folio in force — the same 'folio in force where the paragraph starts' notion tibexport.h:143-149 uses. Then count term hits per folio (exact ACIP token or span match via buildOverlay). Label as counts per folio, not rates, unless folio length is also shown.; Trends of a term across a collection (ordered by author/date) — Reuse GoferPane's per-file rollup (struct FileRoll count/firstLine/snippet, app/main.cpp:19282-19310) over LibraryIndex::search (libindex.h:118) — but note search is capped (400 in the pane; kScanCap per node) so a per-file COUNT needs either a new LibraryIndex method that runs an FTS5 count grouped by file_id (schema libindex.cpp:77-93) or disclosure via corpusCountLine/goferSkipNotice (textreport.h:86-112). Author/date ordering is not in the index; the Library's people layer reads data/extracted/catalog_works.json (app/main.cpp:27837-27840) and authorsearch.h matches names — ordering by those is inferred to be feasible but is new work. For the HGM corpus, the analogous axis is by course: Spine::corpusCount(query, course) per corpusCourses() (spine.h:106-114) gives an UNCAPPED per-course distribution — unlike showConcordance's byCourse over the fetched 200 (app/main.cpp:22633-22634).; Collocates (terms within N syllables of a headword) — The Gofer NEAR grammar is line-based (gofer.h:6-12; evalNode gap on seq, gofer.cpp:44-58), so it answers 'within N lines', not 'within N syllables'. Reuse gofer_ast.h's lexer/parser for query syntax if desired, but implement a token-window counter in the new engine: for each occurrence of the headword (token or span), count tokens/spans within ±N syllables that do not cross barrier_after (lattice.h:29), rank by count; offer 'compare to corpus' using Spine::corpusCount for the pair vs each term. Present as MACHINE; no association-statistic labels beyond raw counts unless the formula is shown, as the Survey does for its difficulty ESTIMATE (app/main.cpp:38059-38070).

**Risks:** Nothing under the names frequency/tfidf/hapax/collocate exists in app/ or core/ (grep hits are comments only: botok.h:235, terminology.h:32, whitney.h:40, main.cpp:6111, :43418). Every 'table' listed above is a local std::map inside one function (Survey `unknown` :37866; showConcordance `byCourse` :22633; GoferPane `byFile` :19289; verse.cpp `counts` :38). A Statistics view must add a core engine, not scrape these.; Cap-as-total is the recurring house defect (SQA DATA-1/DATA-3/DATA-6, PERF-1): corpusSearch, goferSearch, LibraryIndex::search and goferSearchFiles are all capped; only Spine::corpusCount is uncapped, and it returns -1 for NEAR/N queries (textreport.h:34-40). showConcordance's by-course distribution is computed over the fetched 200 and says so (app/main.cpp:22653-22660); a Trends view must use corpusCount per course, and any file-level count must either be exact (new grouped-count query) or disclosed with corpusCountLine/goferSkipNotice.; Observed label/value mismatch to review before reusing: the Statistics tab row 'Words (segmenter)' (app/main.cpp:4173) is fed in.tokens = doc_.tokens.size() (:8650), and lattice.h:27 documents OverlayDoc::tokens as 'normalized ACIP syllables', while the 'Syllables' row (:3998) is a regex count over the raw body. Whether 'words' was meant to be spans (doc_.spans.size(), already shown as 'Dictionary spans') is a design question for Adam, not something to silently rename.; ACIP-only measurement: textStatistics's sylRe [A-Za-z']+ and shad=',' (app/main.cpp:3985, :3999), analyzeVerse's split set , ; / (verse.cpp:15), and tokenizeDocument all assume ACIP; Unicode Tibetan (tsheg U+0F0B, shad U+0F0D) would count 0 syllables. The research doc asks for 'script-agnostic' tables (docs/TEXT_ANALYSIS_SUITE_RESEARCH.md:160) — route Unicode through normalizeTibetanUnicode/unicode_wylie (or botok chunking, botok.h) rather than extending the regexes ad hoc.; The Gofer NEAR unit is lines (corpus seq / file lines), not syllables (gofer.h:6-12, gofer.cpp:44-58; libindex.h:8-10 notes index vs live-scan token semantics differ). Collocates 'within N syllables' is a new evaluator; do not label a line-window result as a syllable window.; Performance: the Survey streams every corpus course with corpusWindow(course, 0, 1<<30) (app/main.cpp:37935) and runs on the GUI thread with a QProgressDialog; LibraryIndex scans need the pump/Stop pattern (app/main.cpp:19235-19240, PERF-4). A collection-wide Trends must be bounded and cancellable, and must say what it did not read (rule 3).

### Compare & Merge suite: allcore::textdiff engine (core, no Qt) + ComparePane / FolderComparePage / ThreeWayPage (app/compare_pane.inc, included into main.cpp's anonymous namespace at main.cpp:23818), Tools ▸ Compare menu, Files-pane hooks, Preferences ▸ Compare defaults, and the --compare CLI. Batches 1–3 shipped 2026-09-08 (commits 594e926, 5e633db, c9dc37c); batch 4 (Apply Patch…, Normalize…, changed-folios report, Versions, Replace in Files, tracked-changes .docx) is filed but NOT built (TODO.md:3122-3127; docs/TEXT_ANALYSIS_SUITE_RESEARCH.md §3/§6/§13).

| API | Purpose | Anchor |
|---|---|---|
| `allcore::textdiff::Options` | All ignore rules; mapped 1:1 onto the RULES ribbon checkboxes (rules_ vector, compare_pane.inc:714-729), Preferences ▸ Compare (main.cpp:36786-36801) and session JSON (cmp::optionsJson/optionsFrom, compare_pane.inc:84-101). | core/include/allcore/textdiff.h:19-31 |
| `allcore::textdiff::Hunk / Result / Kind` | Line-diff output; hunks cover both inputs completely in order (textdiff_smoke coversAll). unimportant = minor under rules; rawNormalised=false flags lines script-agnostic could not convert (house rule 3). | core/include/allcore/textdiff.h:33-52 |
| `splitLines / joinLines` | CRLF/CR/LF splitting with no empty tail; the line model every other call takes. | core/include/allcore/textdiff.h:54-55; core/src/textdiff.cpp:20-39 |
| `normalizeLine` | Public comparison KEY under the options: substitutions → scriptAgnostic via canonical converters (unicodeToWylie warns → raw, ok=false; all-uppercase → acipToEwts) → strip [..]/{..} → strip @NNNA folio → collapse → drop Tibetan punct → whitespace → lowercase. Lossy by design; the 'rules-based' folder criterion and Normalize… previews can call it, but it is not a text transformer. | core/include/allcore/textdiff.h:58; core/src/textdiff.cpp:84-128 |
| `matchesIgnoreRegex` | Line filter (std::regex ECMAScript); invalid patterns are swallowed by catch(...) (filed). | core/src/textdiff.cpp:129-134 |
| `diffLines` | Myers O(ND) linear-space line diff written from the 1986 paper (not a Python port); Delete+Insert adjacent → Change; blank/regex rules → minor; moved-block pairing by key sequence. | core/include/allcore/textdiff.h:60; core/src/textdiff.cpp:136-308 |
| `diffInline / Mode / Span / looksTibetan` | In-line diff at Tibetan-syllable / word / char granularity (byte offsets; cmp::u16 converts to UTF-16 for QTextCursor). Used by render(), showDetails(), sideBySideHtml <mark>, and apparatus differingTokens. | core/include/allcore/textdiff.h:64-67; core/src/textdiff.cpp:310-386 |
| `merge3 / renderMerge / Chunk / MergeResult` | diff3 three-way merge; unresolved conflicts render with <<<<<<< ======= >>>>>>> markers. ThreeWayPage mutates Chunk::resolution then re-renders. | core/include/allcore/textdiff.h:70-81; core/src/textdiff.cpp:387-471 |
| `unifiedDiff` | Unified patch WRITER only ('--- A\n+++ B\n', '@@ -a,n +b,m @@', ' '/'-'/'+' lines; includes minor hunks because 'a patch is exact'). There is NO unified-diff parser or hunk applier anywhere in allcore — Apply Patch must add one. | core/include/allcore/textdiff.h:84-86; core/src/textdiff.cpp:474-507 |
| `sideBySideHtml` | HTML side-by-side report with <mark> on differing syllables. | core/include/allcore/textdiff.h:87-89 |
| `apparatus / ApparatusEntry / apparatusMarkdown / apparatusCsv` | Apparatus criticus cited by folio marker + line-within-folio (citeFor walks back in A for regex @([0-9]{1,4}[AaBb]?)). The cite's folio prefix is the hook for a 'changed folios only' report. | core/include/allcore/textdiff.h:90-99; core/src/textdiff.cpp:569-633 |
| `summary` | One-line honest count: important / minor / moved / 'compared raw (could not be converted)'. Status line and CLI use it. | core/src/textdiff.cpp:634-644 |
| `g_compareFiles / g_compareFolders / g_compareTexts` | App-wide hooks (set at main.cpp:38615-38616 and installComparePages) that any pane/menu uses to open a comparison and raise the pane; g_compareTexts is the ready-made 'preview in the Compare pane' entry point for Apply Patch / Normalize. | app/compare_pane.inc:7-9; app/main.cpp:38613-38617 |
| `cmp::readText / Loaded / firstDifferingByte / linesOf / toStd / fromStd / u16` | File loading (UTF-8 only; binary = NUL in first 8192 bytes; EOL detected CRLF/CR/LF) and Qt↔std bridging used by the pane, the CLI, and the folder criterion. | app/compare_pane.inc:14-34 |

**Reuse for:** Apply Patch… (parse unified diff → match hunks exact / offset / fuzz → preview → apply, never forced) — Nothing parses or applies patches today: allcore has only the unifiedDiff WRITER (textdiff.cpp:474-507). Add a new core TU (e.g. core/include/allcore/textpatch.h + core/src/textpatch.cpp, or extend textdiff) with parseUnified(text) → hunks {aStart,aLen,bStart,bLen, prefixed lines} matching the writer's exact format ('--- A\n+++ B\n' :478, '@@ -a,n +b,m @@' :495, ' '/'-'/'+' :497-503; note the writer emits minor hunks too), and applyHunks(lines, hunks, maxFuzz) returning per-hunk {applied, offset, fuzzUsed, rejected} — exact at the stated line, then scan ± offset for the full context, then fuzz 1–2 context lines (research doc §13, TEXT_ANALYSIS_SUITE_RESEARCH.md:198-204). Register textpatch_smoke exactly like textdiff_smoke (core/CMakeLists.txt:148-150, CHECK macro pattern textdiff_smoke.cpp:9-21) and round-trip: unifiedDiff(a,b,diffLines(a,b,{})) → parse → apply(a) == b; shifted input (offset); altered context (fuzz); altered '-' line (rejected, text unchanged). UI: read the patch with cmp::readText (compare_pane.inc:20-29); preview = g_compareTexts(name, original, name + ' (patched)', result) (main.cpp:38616) or ComparePane::compareTexts (:237-241) — the Compare pane IS the preview, and Save Right / Save Right As… (:358-368) is the apply step; list rejected hunks in the status line / a dialog, never auto-write. To keep it undoable inside an open comparison, implement as an operation on bLines_ via pushUndo()+recompute(true) like copyHunk (:338-353). Entry points: Tools ▸ Compare menu (main.cpp:40344), Sessions ▾ menu (compare_pane.inc:652) and a --apply-patch CLI in the --compare pattern (main.cpp:41738) — NOT a new big ribbon button (ratchet). The vendored diff_match_patch (patch_fromText/patch_apply with Match_Threshold/Match_Distance, diff_match_patch.h:138-149, 565) is a character-level fuzzy applier already linked, but it reads its own patch text format (inferred), throws bare const char* (main.cpp:2740-2760), and works on QString — usable as a fallback for 'fuzz' matching of a single hunk's text, not as the GNU-unified parser.; Normalize… with preview (collapse spacing, unify shad spelling per house convention, strip apparatus, fix line endings) — normalizeLine (textdiff.h:58; textdiff.cpp:95-128) already encodes every rule as a per-line transformation and reports non-convertible lines via normalisedOk — but it is a lossy comparison KEY (drops shads entirely, lowercases, strips folio markers), so do NOT present its output as normalized text. Add a separate transformer (e.g. normalizeText(lines, NormalizeOptions) → {lines, std::vector<int> refusedLines}) in core that reuses the same building blocks: codepoints/isTibetanPunctCp/collapseWs are file-static in textdiff.cpp (:42-59, :75) and must be exposed or moved to a shared header; script conversion must go through the canonical ports only — acipToEwts (engines.h:21) and unicodeToWylie (unicode_wylie.h:22-27; warns>0 ⇒ leave raw and flag), never a new transliteration (house rule 2/3). House-convention switches already live in Preferences ▸ House Style (LOG 22:30) — read them rather than inventing a second rule set; store Normalize settings in the cmp::optionsJson shape (compare_pane.inc:84-101) or a sibling. Preview = compareTexts(name, before, name + ' (normalized)', after) (:237-241) then Save Left/Right — 'show me what would change is exactly what a diff is for' (research §6, :95-103). Source text for the current document: Tools ▸ Compare's frontDoc lambda (main.cpp:40349-40353) yields Manuscript/Draft/Overlay text+path; editops::selectedText / insertPlain (main.cpp:4272+) for a selection. Battery: extend textdiff_smoke's Tibetan-rules block (textdiff_smoke.cpp:68-80) with transformer cases including 'refused line stays byte-identical and is listed'.; 'Changed folios only' report — apparatus(a,b,r) (textdiff.cpp:595-617) already cites every important difference as '@012B.3' via citeFor (:569-581, regex @([0-9]{1,4}[AaBb]?), fallback 'line N'). A changed-folios report = group ApparatusEntry by cite prefix before '.' → per folio: counts by Kind, first/last line, optionally both sides' full folio text (folio boundaries = lines where citeFor's regex matches). Add changedFolios(a,b,r) + changedFoliosMarkdown/Csv beside apparatus in textdiff.h, with CHECK cases in textdiff_smoke's reports block (:119-133 pattern, fixture already has @001A). UI: add kind 'folios' to ComparePane::report (:370-379) and a filter entry in saveReport (:381-382) — but routing is by QFileInfo::suffix() (:384) and a second Markdown kind collides with the apparatus .md; use the dialog's selectedNameFilter or a distinct suffix; add a 'Copy Changed Folios' small button in REPORT (:738-740) only if the width budget allows, else put it under Sessions ▾ / Save Report…; add the suffix to the --compare CLI routing (main.cpp:41750). Folder-level 'changed files only' already exists: FolderComparePage Show ▸ Differences (visibleRows :991-1000) + report (:1008-1024). Research doc verdict: 'BUILD (cheap: filter the apparatus by folio)' (TEXT_ANALYSIS_SUITE_RESEARCH.md:66-67).; Current test pattern (core + app) to copy — Core: core/tests/<name>_smoke.cpp — static int failures; #define CHECK(cond,msg) prints '  [PASS] msg' / '  [FAIL] msg'; setvbuf(stdout,nullptr,_IONBF,0); return failures ? 1 : 0 (textdiff_smoke.cpp:9-21, 135-136); registered with add_executable + target_link_libraries(PRIVATE allcore) + add_test (core/CMakeLists.txt:148-150). No fixture path ⇒ it runs in the clean-checkout gate `ctest -LE fixture`. Live evidence: build/core/textdiff_smoke (built 2026-09-08 19:42:21, newer than textdiff.cpp 19:42:17) prints 'textdiff_smoke: 0 failure(s)', exit 0. App: each pane/page exposes int selfTest(QStringList& log) with a check lambda logging '  [PASS|FAIL] Compare: …' / 'Compare/Folders: …' / 'Compare/Three-way: …' (compare_pane.inc:416-419, 1027-1029, 1248-1250), driven from --selftest (main.cpp:41798-41800; LOG says 39 Compare checks) under ctest app_selftest (QT_QPA_PLATFORM=offscreen, TIMEOUT 300, LABELS fixture, SKIP_REGULAR_EXPRESSION 'cannot open spine db'; app/CMakeLists.txt:90-123). Selftests never open dialogs: g_harnessRun stubs safeGet*/askName and guards editLineFilters/editSubstitutions (:738, :749); FolderComparePage::confirmOps returns harnessAutoConfirm_ under the harness (:1127) so a selftest opts in to copies explicitly and the gauntlet random walk (app/CMakeLists.txt:93-96) can never copy files. Temp fixtures go under QDir::temp()/all_<x>_<pid> and are removed with removeRecursively (:459-479, 1030-1031, 1063). Runner: tools/verify.sh (build then ctest; --quick = app_selftest only; never tests a stale binary).; Ribbon-width ratchet constraint for any new control — app_selftest measures every widget named 'ribbonBar' by minimumSizeHint().width() and FAILS when the widest exceeds kRibbonRatchet = 2572 px (main.cpp:43675-43704). RibbonProxy::minimumSizeHint() returns sizeHint() (5664) so labels never elide; overflow scrolls in the band; an overflow menu was tried twice and removed (5804-5820). The ratchet is app-wide (only the single widest ribbon counts), so Compare's ribbon only trips it if it becomes the widest — but CLOSER #36 says '#36 closes when the widest ribbon fits 1,180' and every pane is expected to lower, not raise, the number. Compare's ribbon is already 7 groups built in the compact idiom ('small buttons in columns; the fit ratchet caps every pane's ribbon width', compare_pane.inc:628-629): QPushButton padding 1px 6px, 2–3 stacked per RibbonGroup::addColumn (main.cpp:5747), big icon proxies only for the three entry actions. Therefore new batch-4 actions should land in the Sessions ▾ menu (:652-680), the Save Report… filter list (:381), Tools ▸ Compare (main.cpp:40344), or the CLI — not as new big buttons. Note the ratchet's [info] line lists QToolButton texts only (43681-43684), so Compare's QPushButtons do not appear in that label but DO count toward the measured width.; Adding a fourth page (e.g. Patch Preview / Normalize) — Follow installComparePages (compare_pane.inc:1327-1335): construct with the ComparePane* owner, owner->addPage(w), set a g_* hook that calls owner->showPage(n) and g_raisePane(owner); return the pointer in ComparePages so main.cpp's --selftest block can add `fails += cmpPages.<page>->selfTest(log)` (main.cpp:41798-41800). The page switcher buttons are hard-coded at :148 (three pageBtn calls) — a new page needs a fourth pageBtn there. Use owner->statusLabel() for messages and owner->options() for rules, as FolderComparePage does (:1090, :1163).

**Risks:** Two independent folio-marker regexes: normalizeLine strips '@[0-9]{1,4}[AaBb]?(?![A-Za-z0-9])' (textdiff.cpp:112) while citeFor cites '@([0-9]{1,4}[AaBb]?)' (:570). A changed-folios report or Normalize that adds a third will drift; unify into one shared definition first.; normalizeLine is a lossy comparison key (drops all shads/tsheg→space, lowercases, strips markers). Presenting it as 'Normalize…' output would destroy text; the transformer must be a separate function that only applies the house-convention rules the user picked.; Invalid std::regex in Line Filters / Substitutions is swallowed by catch(...) (textdiff.cpp:98, :131) — the user gets no signal (filed in TODO.md:3114-3115 and the audit's open items). Normalize/Apply Patch must surface parse failures, not inherit this.; toEwts (textdiff.cpp:84-94) treats any line with uppercase and no lowercase as ACIP and calls acipToEwts, which returns std::string with no warn channel (engines.h:21) — ACIP lines can never be flagged 'not normalised'; only Unicode lines can (unicodeToWylie.warns). Inferred: a malformed ACIP line is silently compared as whatever acipToEwts returns.; cmp::readText is UTF-8 only (compare_pane.inc:27); the enc namespace (main.cpp:4945) exists but is not wired — a Latin-1/MacRoman patch or file would mis-decode. Filed as 'encoding choice for compared files'.; saveReport routes by QFileInfo::suffix() (compare_pane.inc:384): a second Markdown report kind (changed folios) cannot be distinguished from the apparatus .md without switching to selectedNameFilter or a distinct suffix.

### Library / Catalog / Scans record layer — how the app currently describes texts, editions, people and per-text sidecars (core: catalog_*.h, dossier.h, comments.h, proposals.h, libindex.h, reader.h, stardict.h; UI: LibraryPane, CatalogTree/CatalogPane, ScansPane in app/main.cpp)

| API | Purpose | Anchor |
|---|---|---|
| `allcore::AcipFileInfo / decodeAcipFilename` | THE text-identity record. Every pane derives a text's collection/edition string, catalog number, verification status and language from the filename at display time; there is no stored text record. | core/include/allcore/tibexport.h:36-53 |
| `allcore::bdrcScanUrl` | Deterministic, offline KD/KL/TD -> bdr:MW22084_n / MW26071_n / MW23703_n scans URL; '' when no mapping. The app-side wrapper bdrcScanUrlChecked (main.cpp:5832-5900) routes Lhasa (KL) through kl_bdrc_concordance.json and Sungbum through sungbum_scan_links.json, flagging 'other-edition' tiers via g_sungbumOtherEdition. | core/include/allcore/tibexport.h:62-71 |
| `allcore::BibliographyFields / composeBibliographyEntry / SanskritBibFields / composeSanskritBibEntry` | The house (STD-007 / DCC style) citation formatter — pure assembly, fields used verbatim. This is the only place 'edition' appears as a typed field. Reuse as the References pane's citation preview target. | core/include/allcore/tibexport.h:73-110 |
| `allcore::PersonRef / PersonHit / matchPeople` | The PERSON record and its search (Exact/Contains/Spacing/Phonetic/PhoneticNear tiers). One BDRC P-id = one person; spellings are aliases; homonyms never merged. allcore takes no JSON dependency — the app loads data/extracted/author_index.json into these (LibraryPane::buildAuthorIndex, main.cpp:26945). | core/include/allcore/authorsearch.h:52-138 |
| `allcore::Subjects / WorkSubject` | Catalog-assigned subject headings per work key (R6 + Sungbum) — REFERENCE facts, explicitly NOT a register layer and never used to reorder HGM glosses. The closest existing thing to 'tags' on texts. | core/include/allcore/subjects.h:37-62 |
| `allcore::normalizeCatalogKey / baseCatalogKey / extractAcipCitations / collectLibraryNumbers / auditPresence` | Catalog-number identity normalization (S464 == S00464; 'S5002-1' keeps its sub-number; baseCatalogKey strips it) and the bibliography-presence audit. Use these — not the LibraryPane regex — as the canonical record key. | core/include/allcore/catalog_audit.h:26-83 |
| `allcore::qcDuplicateTitles / qcTitleTranslationMismatch` | The existing duplicates finder: groups files by normalized Tibetan title (filename field else title page) and compares colophons; verdict strings 'same colophon - true duplicates' \| 'different colophons - distinct works sharing a title' \| 'no colophon evidence - needs a human read'. Operates on a directory tree of files, not on records. | core/include/allcore/catalog_qc.h:26-57 |
| `allcore::extractAcipTitle / TitleBank / suggestIdentity / findColophonCandidates / suggestVolumeSplits` | Identity proposal lane for uncataloged files: title-page extraction, ranked evidence-carrying candidates (measured 80.8% top-1), colophon author-evidence spans. SUGGESTS only; never writes the catalog. | core/include/allcore/catalog_id.h:38-150 |
| `allcore::CatalogRegister / RegisterEntry / parseChangeStamp / composeChangeStamp` | Read-only import of the registrar's CSV/TSV (delimiter and columns auto-detected by header name — the pattern a BibTeX/RIS importer should mirror) and the '<base> - updated YYYY-MM-DD INI' folder stamp. | core/include/allcore/catalog_register.h:20-62 |
| `allcore::ActionLedger / CatalogAction` | Staged-approval pipeline over the shared Dropbox root: <root>/AWAITING APPROVAL/ + ACTIONS.tsv ledger; approve moves onto shelf, reject to REJECTED/. Model for any team-shared, human-ruled write. | core/include/allcore/catalog_actions.h:28-92 |
| `allcore::composeCatalogFilename / ComposedName / rejoinMetaName` | The filename grammar NUMBER_TIBETAN TITLE_ENGLISH TITLE_AUTHOR (DATES).TXT and the '+' / 'NUMBER META.TXT' companion convention (1,457 pairs in the library) — the de-facto per-text metadata sidecar the References pane must read. | core/include/allcore/catalog_name.h:34-60 |
| `allcore::generateAsciiCatalog` | Field-coded ASCII LIST (S/F/D/T/E/A/V/P lines per record) of what a tree holds — the existing 'export records' format; an RIS-like plain-text export already exists in spirit. | core/include/allcore/catalog_list.h:33-52 |
| `allcore::Worksheet / worksheetSchema / serializeWorksheet / parseWorksheet / worksheetCsvRow` | The team's live 52-column cataloging schema; sidecar '<file>.worksheet.tsv' (Field\tValue per line, unknown fields survive). The richest per-text metadata record the app already round-trips. | core/include/allcore/worksheet.h:20-33 |
| `allcore::DossierStore / Dossier` | Per-text reading position + stamps in library/dossiers.tsv (LOCAL desk, full-rewrite). Closest existing 'read state'. | core/include/allcore/dossier.h:19-49 |

**Reuse for:** Records (texts) — Do not invent a stored text record. A record = work key + AcipFileInfo (decodeAcipFilename) + the JSON banks. Use allcore::normalizeCatalogKey/baseCatalogKey (catalog_audit.h:43-49) as the canonical key rather than LibraryPane's ad-hoc regex '^([A-Za-z]+)0*(\d+)' (main.cpp:27055, 28066, 27958) which silently drops '-sub' numbers. Lift LibraryPane::englishTitle/loadPersons/buildAuthorIndex (27009, 27834, 26945) into one shared loader (a new .inc or a small app-side 'CatalogBanks' struct) so Library, Catalog and References read the same maps; keep the 'set the loaded flag AFTER a successful load' discipline documented at 27010-27016 and 27838-27842.; Records (editions) — There is no Edition type. Edition today = AcipFileInfo.collection string ('Kangyur (Derge edition)', 'Kangyur (Lhasa edition)', 'Sungbum Collection'), the SanskritBibFields.edition/collection fields (tibexport.h:100-104), and the concordance banks (kl_bdrc_concordance, thl_dege_concordance, sungbum_scan_links with its 'other-edition' tier). An Edition record should be a thin wrapper over those, exposing the BDRC MW id from bdrcScanUrl/bdrcScanUrlChecked (5832) and carrying the 'other-edition' honesty flag (g_sungbumOtherEdition, 5883-5886).; Records (people) — Reuse allcore::PersonRef/PersonHit/matchPeople (authorsearch.h) and the author_index.json loader in LibraryPane::buildAuthorIndex; person card HTML already exists (personHtml 27880; Lookup PERSON card per docs/PEOPLE_LAYER_DESIGN.md); ToL links via tolLinkForPid/tolBiographyUrl/tolSearchUrl (main.cpp:5340-5372). Route 'open person' through g_openAuthorByPid (364) instead of duplicating the works table.; Records (places) — Nothing exists — no struct, no bank, no UI (grep of core/include and main.cpp found no Place model). This is NEW, not an extension; scope it separately and source it (BDRC G-ids are not harvested today; PEOPLE_LAYER_DESIGN.md covers persons only).; Attachments — Derive, don't store. Every existing per-text artefact is a basename-keyed sidecar: library/properties/<base>.json (3944), library/glossaries/<base>.tsv (17377), library/links/<base>.tsv (29245), <file>.worksheet.tsv (34909), 'NUMBER META.TXT' companions (catalog_name.h), library/scan_cache/<MW>/ pages + library/bdrc_links.json (11610), comments.tsv rows by basename (comments.h). A record's attachment list can be computed from these paths; only genuinely new attachments (PDFs, external notes) need a new store — keep them as files in a sidecar folder plus a TSV manifest, matching the house plain-text doctrine.; Collections / smart collections / tags — Nothing record-level exists to extend. Closest patterns: FilesPane favorites in QSettings 'files/favs' (25315) — fine for a LOCAL list of record keys; sess::remember for persisting a filter set (25957-25966) — a smart collection = a saved (collection/status/language/name) filter plus optionally a Gofer query run through allcore::LibraryIndex::search (libindex.h:103) with `truncated` disclosed. Catalog subjects (allcore::Subjects, r6subjects_) are REFERENCE tags with an author — display them as such, never as user tags. If tags must be shared, follow the proposals/comments TSV-in-Dropbox pattern (append-only, tsvEscape, conflicted-copy union), not a second SQLite.

**Risks:** Charter conflict: 'the app never writes the official catalog; numbers are registrar-issued; identity suggestions route through ProposalKind::CatalogIdentity' (catalog_id.h:7-9, proposals.h:40-44, CatalogPane banner 34148-34160). A References pane with editable records must stay a VIEW over files + release banks; user-authored fields need their own provenance-tagged sidecar and must never be written into data/extracted/*.json (release-owned, rebuilt by tools/*.py).; Storage-model mismatch (inferred): docs/TEXT_ANALYSIS_SUITE_RESEARCH.md §17 proposes library/references.db (SQLite) 'shared through the team updates folder', but app/updatesDir today only carries DMGs (main.cpp:40815-40860) and every shared human-judgment store is plain TSV designed for Dropbox conflicted-copy union (comments.h:5-10, proposals.h:105-110). A SQLite file in a Dropbox folder breaks that discipline; decide TSV-in-shared-folder vs local SQLite before batch 6a.; Two work-key normalizations coexist: LibraryPane's regex (prefix + unpadded digits, drops '-sub') and allcore::normalizeCatalogKey (keeps '-sub'; baseCatalogKey strips). A duplicates finder or record index must pick one or it will merge S5002-1 with S5002-2 (catalog_audit.h:39-49).; People honesty rules are load-bearing: one pid = one person, spellings are aliases, homonyms never merged, phonetic tiers must look weaker (authorsearch.h:9-44, 93-115); person data is REFERENCE, ToL essays are link-outs only (PEOPLE_LAYER_DESIGN.md). A records UI that lets users 'merge duplicates' across persons would violate this.; Editions: Sungbum scan links carry an 'other-edition' tier (sungbum_scan_links.json; g_sungbumOtherEdition main.cpp:5883-5886) and KL != H numbering (bdrcScanUrlChecked comment 5825-5831) — any Edition record must keep these honest tags; a BDRC 'refresh' must not overwrite our edition with theirs silently.; Places are entirely absent (no struct, bank, or UI) — this is new construction, and BDRC place (G-id) harvesting has no tool yet; scope and license (data/licenses per PEOPLE_LAYER_DESIGN.md item 5b) before promising it.


---

# Batch 4 — Versions, Replace in Files, Apply Patch, Normalize, tracked changes, changed folios

*Synthesized the final batch-4 specification from the honesty-and-provenance-first design (winner on combined judge total, 86 vs 85 and 83), grafting the translator vocabulary and app-layer qCompress version store from design 1 and the gate-hole fix, path-hash version keying and zip verification kit from design 3. All 37 mustFix items from both judges are addressed, including the two neither design caught alone: the THIRD folio regex at app/main.cpp:3999 (verified: @\d{2,3}[AaBb]\b, narrower than both core regexes) and cmp::readText's inability to report a UTF-8 decode failure (verified at app/compare_pane.inc:20-29). Every cited line anchor was re-grepped against HEAD: DraftPane::dataRoot_ declared at 23770 and never assigned in the ctor at 21686; ratchet 2572 at 43708; constitution R3 census reads main.cpp only with baseline 154; compare_pane.inc has zero modal sites so the census widening lands green. Seven features (F0 substrate/gates, F1 Versions, F2 changed folios, F3 Normalize, F4 Replace in Files, F5 Apply Patch, F6 tracked-changes .docx), ~100 hours, zero ribbon controls, zero new QMessageBox sites.*

# Batch 4 — Versions, Replace in Files, Apply Patch, Normalize, tracked-changes .docx, changed folios

**Status:** specification, 2026-09-09. Supersedes the batch-4 lines in `docs/COMPARE_SUITE_PLAN.md`.
**Research verdicts implemented:** `docs/TEXT_ANALYSIS_SUITE_RESEARCH.md` §3(a), §3(d), §5, §6, §7, §12, §13.
**House format:** `docs/release_audit/ComparePane.md` (purpose → user-visible functions → honesty and provenance → open items).

## 0. What governs this batch

The design that won is **honesty-and-provenance-first**, grafted with the translator vocabulary of the
task-first design and the engineering-risk discipline of the risk-first design. Four rules decide every
contested detail:

1. **The preview IS the apply.** The same function, on the same text, produces the diff the translator
   looks at and the bytes that land. Every file is re-hashed at apply and refused if it changed since the
   preview; every undo is refused if the file changed since the run. A preview that could diverge from
   the write is not a preview, it is a picture.
2. **One undo primitive.** Every write by Replace in Files, Apply Patch, Normalize and Restore banks a
   `pre-*` Version *first*; if the bank fails the write does not happen. "Undo" always means "restore
   the version". Versions is therefore built first.
3. **Refusals are designed in, not bolted on.** A line the rules cannot classify is left byte-identical
   and *listed* with its line number and reason. A file that is not valid UTF-8 is skipped and named. A
   version whose SHA-1 does not match its record is shown as damaged and excluded from Compare and
   Restore. Nothing is approximated (rule 3).
4. **No composition, no gloss traffic.** Nothing in this batch reads, writes or moves `hgm_gloss`.
   Machine work counts, aligns, diffs and rewrites the translator's own bytes.

**Fit ratchet:** this batch adds **zero ribbon controls**. `kRibbonRatchet = 2572` at
`app/main.cpp:43708` is untouched; every new action lives in a menu, the Compare pane's Save Report…
filter list, the Sessions ▾ menu, a window of its own, or the CLI. The closing `[info]` line must report
the ratchet unchanged.

**Modal census:** `tools/constitution_check.py` reads `app/main.cpp` only (verified: `main_cpp` at the
R3 block, baseline `154`). Four new `.inc` files with confirmation flows would be invisible to it.
F0 widens the census to `app/main.cpp` + `app/*.inc` **in the same commit that creates the first .inc**,
and every confirmation in this batch is a two-step in-window button (the `FolderComparePage::confirmOps`
idiom at `app/compare_pane.inc:1137`), so the baseline does not move at all. Verified:
`app/compare_pane.inc` has **zero** `QMessageBox`/`QInputDialog` sites today, so the widening lands green.

**Line anchors:** every reference below was re-grepped against HEAD on 2026-09-09. Where the source
designs cited numbers that had drifted, the corrected anchor is given.

---

## F0 — Substrate and gates

### Purpose and user story

*Not a research verdict — the prerequisite the other six features stand on.* As the maintainer, I want the
Draft to store sidecars like the Overlay and Manuscript, one definition of a folio marker instead of
three, one definition of "eligible text file" instead of two, a hunk-selection primitive in the engine
rather than app-side splicing, a report-filter routing that can tell two Markdown kinds apart, and a
modal census that can see the windows this batch adds — before any feature code lands.

### UI

**Placement:** none. Fixes land in `DraftPane`, `allcore::textdiff`, `docprops::textStatistics`,
`safeGetSaveFileName`, the Tools ▸ Compare `frontDoc` lambda, and `tools/constitution_check.py`.

**Controls:** none added.

**Honesty labels:** the Draft's Properties window stops reporting "no data root" once `dataRoot_` is
assigned (the message was true; the cause was the missing assignment). Wherever a folio count is shown
beside a folio-change count, both now come from one rule.

**Fit-ratchet note:** no ribbon change.

### Data

No new store. Three shared definitions are unified:

| Thing | Today | After F0 |
|---|---|---|
| Folio marker | three regexes: `core/src/textdiff.cpp:106` `@[0-9]{1,4}[AaBb]?(?![A-Za-z0-9])` (in `normalizeLine`), `core/src/textdiff.cpp:569` `@([0-9]{1,4}[AaBb]?)` (in `citeFor`, **no lookahead**), and `app/main.cpp:3999` `@\d{2,3}[AaBb]\b` (in `docprops::textStatistics`, **requires a letter suffix and 2–3 digits**) | one exported matcher `allcore::textdiff::folioMarkerEnd`; `textStatistics` calls a Qt-side wrapper built from the same pattern string, so the Folios column of the Versions table and the "N of M folios changed" headline can never be two different numbers for one volume |
| Eligible text file | duplicated at `core/src/gofer.cpp:88-93` and `core/src/libindex.cpp:38-46` (`.txt .acip .md .act .inc .ace`, ≤10 MB) | `allcore::eligibleTextFile` in `core/include/allcore/filewalk.h`; both call sites switched, Replace in Files routed through it, so "eligible" means the same thing in Search and in the formatter |
| Partial hunk application | none (each caller would splice by hand) | `allcore::textdiff::applySelected` |

`safeGetSaveFileName` (`app/main.cpp:3451`) gains an optional trailing
`QString* selectedFilter = nullptr` out-parameter; the harness stub path (`g_saveDialogStub`) leaves it
untouched, so existing selftests are unaffected.

**Provenance fields:** n/a.

### Engine

**Module:** new `core/include/allcore/filewalk.h` + `core/src/filewalk.cpp`; additions to
`core/include/allcore/textdiff.h` + `core/src/textdiff.cpp`; app-side edits in `app/main.cpp`.

```cpp
namespace allcore {
bool eligibleTextFile(const std::filesystem::path& p, std::uintmax_t size,
                      std::uintmax_t maxBytes = 10u * 1024 * 1024);
struct WalkStats { int scanned=0, ineligibleExt=0, tooLarge=0, excluded=0, unreadable=0, capped=0; };
// Globs are ';'-separated fnmatch-style patterns over the path relative to root;
// a '!' prefix negates. Excludes win over includes. Files past fileCap are COUNTED
// in stats.capped, never silently dropped (the PERF-1 lesson from gofer.cpp:92-96).
std::vector<std::string> listEligibleFiles(const std::string& root,
        const std::string& includeGlobs, const std::string& excludeGlobs,
        bool recurse, int fileCap, WalkStats* stats,
        const std::function<bool()>& keepGoing = {});
}

namespace allcore::textdiff {
// -1 when the line carries no marker; otherwise the byte offset just past it,
// with the folio id (uppercased, e.g. "012B") written to folioOut.
// Pattern: @([0-9]{1,4}[AaBb]?)(?![A-Za-z0-9])
int folioMarkerEnd(const std::string& line, std::string* folioOut);
// Hunks cover both inputs in order (the coversAll invariant, textdiff.h:46):
// Equal → copy a[aBeg,aEnd); selected non-Equal → copy b[bBeg,bEnd);
// unselected non-Equal → copy a[aBeg,aEnd).
std::vector<std::string> applySelected(const std::vector<std::string>& a,
                                       const std::vector<std::string>& b,
                                       const Result& r,
                                       const std::vector<int>& selectedHunks);
}
```

App-side, three one-line fixes:

- `DraftPane` constructor (`app/main.cpp:21686-21688`) initialises `dataRoot_(root)` alongside
  `root_(root)`. **Verified:** `dataRoot_` is declared at `app/main.cpp:23770` and read at 22069, 22132
  and 22143, but never assigned — every Draft sidecar write is a silent no-op today, and a version hook
  beside `noteSave` would keep nothing for drafts while the UI reported success.
- The Tools ▸ Compare `frontDoc` lambda (`app/main.cpp:40360-40364`) and "Compare with Saved Version"
  (`:40371`) flatten the Manuscript's **saved HTML** through `QTextDocument::setHtml` → `toPlainText`
  before comparing, so both sides are plain text (today raw HTML on disk is compared against plain
  editor text and every Manuscript comparison is noise).
- `tools/constitution_check.py`: the R3 census text becomes `app/main.cpp` concatenated with every
  `app/*.inc` (sorted glob). R2 stays `main.cpp`-only (the harness flag list lives there).

**Algorithm and published source:** none new — this is promotion and unification of code already in the
tree. Glob matching is fnmatch-style (`*`, `?`, `[…]`), implemented in-house.

**Reuses:** `gofer.cpp:88-93` / `libindex.cpp:38-46` eligibility rule (both switched to the shared
helper); `FolderComparePage::scan`'s `QDir::match` glob idiom (`app/compare_pane.inc:1093-1103`) as the
semantics precedent; `ManuscriptPane::setDataRoot` as the wiring precedent for Draft.

### Acceptance tests

**CTest smoke — `filewalk_smoke` (new):**
1. Temp tree `{a.txt, b.ACT, c.bin, sub/d.inc, big.txt (11 MB)}`;
   `listEligibleFiles(root, "*.txt;*.act;*.inc", "sub/*", recurse, cap 100)` returns exactly
   `[a.txt, b.ACT]` (extension match is case-insensitive) with
   `stats {ineligibleExt:1, tooLarge:1, excluded:1, capped:0}`.
2. Same tree with `cap=1` → 1 path returned, `stats.capped == 1` (counted, not dropped).
3. `"!*.bak"` in the include field excludes `x.bak` even when `*.txt;*.bak` is also listed
   (excludes win).
4. `eligibleTextFile("x.ACE", 5)` true; `("x.pdf", 5)` false; `("x.txt", 11<<20)` false.

**CTest smoke — `textdiff_smoke` (additions):**
5. `folioMarkerEnd("@012B KA", &f) > 0` with `f == "012B"`; `("@012BX KA", …) == -1`;
   `("@1A", …)` matches with `f == "1A"`; `("@0012", …)` matches with `f == "0012"`;
   `("@x", …) == -1`.
6. Regression: `normalizeLine("@12b KA", {ignoreFolioMarkers})` still yields `"KA"`, and `citeFor` on a
   line holding `@12b` still yields `"@12B.1"` — the two paths now share one definition and agree.
7. Regression: the existing apparatus fixture still cites `"@001A.4"`.
8. `applySelected(a={x,y,z}, b={x,Y,z,w}, r, {})` == `a`; `({indexOfChangeHunk})` == `{x,Y,z}`;
   `({indexOfInsertHunk})` == `{x,y,z,w}`; every non-Equal selected == `b`.

**app `--selftest`:**
9. Draft block: after `saveDraft` to a temp path under a temp data root,
   `docprops::sidecarPath(dataRoot_, draftPath_)` is non-empty and the sidecar's `revision == 1`
   (pins the `dataRoot_` fix; the existing Draft checks that pass today because *nothing is written*
   are updated in the same commit).
10. Draft block: `renameDraft` carries `library/properties/<base>.json` to the new base name.
11. Menus block: with a Manuscript saved as HTML containing `<b>KA</b>`, Compare with Saved Version
    yields `result().differences() == 0` (both sides plain text).
12. `safeGetSaveFileName` with a non-null `selectedFilter` under the harness leaves it unchanged and
    returns the stub's name (no existing selftest changes behaviour).

**Gate tests:**
13. `python3 tools/constitution_check.py .` exits 0 with the widened census and baseline `154`
    unchanged (proves `compare_pane.inc` adds no modal sites).
14. Deliberate regression: insert `QMessageBox::warning(this, "x", "y");` into `app/compare_pane.inc`
    → the check **must fail** with the R3 growth message; remove it → passes again.
15. `gofer_smoke` and `libindex_smoke` stay green after both call sites switch to
    `eligibleTextFile` (the regression gate for that move).

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| Wiring `DraftPane::dataRoot_` activates sidecar writes that never happened; Draft selftests that pass because nothing is written will change behaviour | Fix and selftests land in one commit; test 9 pins the new truth |
| Unifying the folio regex changes `citeFor` for tokens like `@001AB` (previously matched `@001A`) | Test 5 pins the new behaviour; the change is called out in the ComparePane audit row |
| `textStatistics`'s regex was *narrower* (2–3 digits + mandatory letter): folio counts in Properties will rise for texts using `@1A` or bare `@012` | Stated in the Versions window footer and the audit row; it is a correction, not a regression |
| Switching `gofer.cpp`/`libindex.cpp` to a shared rule could change retrieval results | Tests 15; the rule is transcribed verbatim before it is shared |

### Effort

**7 hours.**

### Dependencies

None. Everything else in the batch depends on F0.

---

## F1 — Versions

### Purpose and user story

**Research verdict:** §5 ADAPT/BUILD — on every Save keep a compressed copy under
`library/versions/<doc>/<timestamp>`, size-capped and pruned; File ▸ Versions… lists them with the
Properties statistics; Compare with Version opens the Compare pane; Restore copies back with a confirm;
blame = "which version first introduced this line", computed by walking versions with the engine.
Branching SKIP. **§12 BUILD:** change tracking for texts *is* Versions + Compare — "Accept/Reject" is
Copy Left/Right between the current text and a version, and "who" comes from the Team name.

> **§12 mapping, stated in the shipped UI** (Versions window footer, and the Compare status line when a
> version is loaded): *"There is no Track Changes toggle for texts. To accept or reject a change, compare
> a version with what you are editing and use MERGE ▸ Copy Left / Copy Right, then Save."* Without this
> sentence translators hunt for a toggle that does not exist and must not exist
> (`docs/MENU_PARITY_PLAN.md:113` defers Track Changes past 1.0).

**User story.** As a translator saving my source text, my English draft or my manuscript many times a
day, the tool keeps every distinct saved state with who saved it and when, so that I can list the
history, compare any version with what I am editing (or two versions with each other), name the one I
sent to Geshe Michael so it is never pruned, hand yesterday's draft to a colleague without touching the
current file, ask which version first introduced the line under my caret, and restore an earlier copy —
knowing the restore itself banks the current text first, so it can never lose anything.

### UI

**Placement.**
- **File ▸ Versions…** (⌥⌘V), inserted after "Revert File" (`app/main.cpp:39458`), routed through the
  same `active()` dispatch as Save: Manuscript > Draft > Overlay; a status message when no document is
  in front.
- **Tools ▸ Compare ▸ Compare with Version…**, inserted after "Compare with Saved Version"
  (`app/main.cpp:40371`) — opens the window with the list focused and the Compare buttons enabled.
- **Properties window ▸ Statistics tab** (`app/main.cpp:4178`): a row `Versions kept: 14 (oldest 2 Sep
  2026)` and a **Versions…** button (`docprops::Input` gains `std::function<void()> openVersions`).
- **Preferences ▸ Versions** (new page beside House Style, the `PreferencesDialog` page pattern).
- The window is `class VersionsWindow : QDialog` in **`app/versions_pane.inc`**, included from
  `app/main.cpp` after `#include "compare_pane.inc"` (`app/main.cpp:23829`). Modeless
  (`Qt::Window`, `show()`, `WA_DeleteOnClose`); returns early under `g_harnessRun`; never opened under
  `g_sweepActive`.

**Controls.**

| Control | What it does |
|---|---|
| Version table (newest first) | Columns: **When · Who · Kind · Name · Rev · Size · Folios · Syllables · Shads · Words · Lines**. Kind ∈ Saved / Autosaved / Milestone / Before restore / Before replace / Before patch / Before normalize. Statistics are read from the version's own record (captured at save time by `docprops::textStatistics`), never recomputed on the GUI thread. Single- and two-row selection. |
| **Compare with Current** (default; 1 row) | `g_compareTexts(file + " (version <when>, <who>)", versionText, file + " (editing)", currentText)`. Overlay versions are decoded with `enc::decode(bytes, meta.encoding, &bad)`; `bad > 0` refuses. Manuscript versions and the Manuscript editor text are both flattened to plain text. |
| **Compare Two Selected** (2 rows) | Older on the left, newer on the right, same route. |
| **Restore…** (1 row) | Disabled while the editor has unsaved edits, with the inline reason *"Save or revert your unsaved edits first — Restore replaces the file on disk."* Two-step button: the first click relabels itself *"Confirm: restore the version of 8 Sep 2026 14:02 by Adam — the file as it is now is kept as a version first"* for 8 s; the second click runs it. Under `g_harnessRun` a `harnessAutoConfirm_` flag (default **false**) decides. **No QMessageBox.** |
| **Save Version As…** | Writes the decoded bytes of the selected version to a chosen path through `saveOrWarn` — hand yesterday's draft to a colleague without touching the current file. |
| **Name this Version…** | Sets `label` and `pinned = true`; pinned rows carry a pin glyph and are **never pruned**. A row's context menu offers **Unpin**. Renaming to empty clears the label and keeps the pin. |
| **Line Origins…** | Asks for a line number (default: the caret line of the front document), runs `versions::lineOrigins` over the decoded versions with a cancellable progress pump, and reports *"Line 214 first appears in the version of 7 Sep 2026 14:02 (Adam, version 12 of 30) and is unchanged since"* or *"…has changed since the newest version (unsaved edits)"*. Badged MACHINE via `ux::sourceBadge(Epistemic::Machine)`. A second tab lists origins for every current line. |
| **Send Changes to Word…** | Builds the tracked-changes `.docx` of (this version → current text). **Not created at all until F6 has passed its Word verification** — no disabled placeholder, no dead control. |
| Filter combo: All · Saves only · Milestones · Batches | **Batches** groups `pre-replace` / `pre-patch` / `pre-normalize` rows by their changeset id; each batch row carries a two-step **Undo this batch…** button — this is Replace in Files' undo, reachable from here as well as from the Find menu. |
| Checkbox **Show autosaves** (default off) | Reveals the single rolling autosave row. |
| Checkbox **Show versions of a file with this name in another folder (N)** | Appears only when such rows exist; off by default; those rows carry the other path in a tooltip and an *elsewhere* marker. |
| **Open Versions Folder** | `QDesktopServices::openUrl` on `library/versions/<docKey>/`. |
| Notice labels (read-only) | *"N older versions were removed to stay within 64 MB / 200 versions (Preferences ▸ Versions)"*; *"Unchanged since the last version — nothing new was kept"*; *"Autosaves keep one rolling entry until you press Save"*; *"1 record has no readable provenance and is not offered as a version (orphan)"*. |
| Preferences ▸ Versions | **Keep a version on every save** (default on) · **Maximum versions per document** (10–5000, default 200) · **Space per document (MB)** (5–2000, default 64) · **Also keep a rolling autosave version** (default on). Keys `versions/enabled`, `versions/capCount`, `versions/capMB`, `versions/autosaveSlot`. |

**Keyboard:** ⌥⌘V opens the window. Enter on a selected row = Compare with Current. No other bindings.

**Honesty labels.**
- **Who** shows the Team name (`g_userName`, `app/main.cpp:3209`) or `"<login> (no Team name set —
  Community ▸ Team)"`, and the record stores `whoSource` ∈ `team-name | login` so the display is
  reproducible from the data.
- **Kind** is always shown; autosave rows are visually lighter and the notice explains the one-slot
  policy, so the list is never read as a minute-by-minute history.
- Encoding and line ending are in each row's tooltip. A version whose bytes do not decode in the
  recorded encoding refuses Compare and Restore with *"N byte(s) of this version cannot be read as
  Windows-1252. Nothing was guessed — use Save Version As… to keep the raw bytes."*
- A version whose SHA-1 does not match its record is shown as **damaged — cannot be restored** and is
  excluded from Compare and Restore. A `.ver` file with no `.json` is an **orphan**, counted in the
  notice and never offered as a version. *A restore is a write; a version whose provenance cannot be
  read must not be restorable.*
- Restore result: *"Restored the version of 7 Sep 2026 14:02; the text that was on disk was kept as
  version 20260909-101502-337 (before restore)."*
- Line Origins states its basis: *"exact line match, walked through N kept versions with the diff engine;
  pruned versions and edits made in other programs are not seen."*
- Footer: *"Versions exist only for saves made in this app. Edits made in other programs, and saves from
  before this feature existed, left no version."* and the §12 sentence quoted above.
- Manuscript note: *"Manuscript versions are compared as text; formatting is not compared."*

**Fit-ratchet note:** no ribbon controls.

### Data

**Location.** `<dataRoot>/library/versions/<docKey>/` — beside `library/properties/<base>.json` and
`library/glossaries/<base>.tsv`, so the histories travel with the texts and `renameFileTo` can carry
them.

**docKey (the collision fix).**

```
docKey(absPath, dataRoot) =
    absPath is under dataRoot ?  completeBaseName(absPath)
                              :  completeBaseName(absPath) + "~" + first8hex(sha1(absPath))
```

Inside the data root, base-name keying keeps parity with the properties and glossary sidecars, so
`docprops::renameFileTo` carries the history with one extra pair. **Outside** the data root — exactly the
population Replace in Files rewrites, where two `KL0001.ACT` files from different volumes are normal —
the path hash makes collision impossible, so an undo set can never be assembled from another file's
history.

**Per version, two files.**

- `<stamp>.ver` — the version bytes. `codec = qz` means Qt's `qCompress` format (4-byte big-endian
  uncompressed length + a zlib stream, RFC 1950/1951); `codec = raw` means the bytes verbatim. The core
  store never inspects blob bytes, so compression lives in the app layer where Qt already provides it
  and `allcore` gains no zlib dependency (`core/CMakeLists.txt:77-88` links `z` only under
  `USE_SYSTEM_SQLITE=ON`). **This honours §5's "compressed copy" without a deviation.**
- `<stamp>.json` — the record:

```json
{ "schema": "all-version/1",
  "stamp": "20260908-214512-337",          // yyyyMMdd-HHmmss-zzz, +"-N" on collision; sorts chronologically
  "path": "/Users/…/library/kangyur/KL0032.ACT",   // absolute path at save time
  "kind": "document|draft|manuscript|file",
  "reason": "save|autosave|milestone|pre-restore|pre-replace|pre-patch|pre-normalize",
  "changeset": "rif-20260908-214512",      // "" unless part of a batch operation
  "revision": 12,                          // the sidecar revision AFTER this save
  "bytes": 48213,                          // uncompressed
  "codec": "qz",
  "sha1": "a99993e3…",                     // SHA-1 of the DECOMPRESSED bytes as written to disk
  "encoding": "UTF-8|UTF-8 with BOM|UTF-16 LE|Windows-1252|MacRoman|Latin-1",
  "lineEnding": "LF|CRLF|CR|HTML",
  "bom": false,
  "label": "sent to GMR", "pinned": true,
  "savedBy": "Adam", "savedBySource": "team-name|login",
  "savedAt": "2026-09-08T21:45:12",
  "app": "DiamondCutterTranslationTool 0.x",
  "statistics": { "folios": 12, "syllables": 4021, "shads": 390, "words": 0,
                  "lines": 812, "characters": 23910 },
  "statisticsNote": "" }
```

`statistics` is `null` with a filled `statisticsNote` when it could not be computed — never a silent zero.

**Folder record** `_meta.json`: `{"schema":"all-versions-folder/1","pruned":7,"capVersions":200,"capBytes":67108864}`.

**Changeset records** (written by F4, read by the Batches filter and by Find ▸ Undo Last Replace in
Files…): `<dataRoot>/library/versions/_changesets/<id>.json` — see F4.

**No index file.** The listing is a directory scan, so there is nothing to go stale.

**Preferences:** `QSettings("ALL","TranslationTool")` — `versions/enabled`, `versions/capCount`,
`versions/capMB`, `versions/autosaveSlot`.

**Provenance fields:** `path`, `kind`, `reason`, `changeset`, `revision`, `sha1`, `bytes`, `codec`,
`encoding`, `lineEnding`, `bom`, `savedBy`, `savedBySource`, `savedAt`, `app`, `label`, `pinned`,
`statistics` / `statisticsNote`, and `_meta.json.pruned`.

### Engine

**Module:** `core/include/allcore/versions.h` + `core/src/versions.cpp` (Qt-free, `std::filesystem`,
deterministic — the caller supplies stamps and clock strings, as `backup.h` does). Blame lives in
`allcore::textdiff`. Smoke: `core/tests/versions_smoke.cpp`, registered in `core/CMakeLists.txt` after
`backup_smoke` (`:452-454`). App glue: `docprops::noteVersion` and `VersionsWindow` in
`app/versions_pane.inc`.

```cpp
namespace allcore::versions {

struct Meta {
    std::string stamp, path, kind, reason, changeset, encoding, lineEnding,
                codec, sha1, savedBy, savedBySource, savedAt, app, label,
                statisticsJson, statisticsNote;
    int  revision = 0;
    long long bytes = 0;
    bool pinned = false, bom = false;
    bool metaOk = true;              // false = unreadable record, still listed
    std::string problem;             // why it is unusable
};
struct Entry { Meta meta; std::string verPath, jsonPath; };
struct Limits { int maxVersions = 200; long long maxBytes = 64LL << 20; };
enum class PutResult { Stored, Deduplicated, Failed };
struct PutOutcome { PutResult result; std::string stamp; int pruned = 0; };

std::string docKeyFor(const std::string& absPath, const std::string& dataRoot);
std::string versionsDirFor(const std::string& dataRoot, const std::string& docKey);

// blobBytes are OPAQUE to core (the app compresses). meta.sha1 must be the hash
// of the DECODED bytes: dedupe and integrity are both defined on what lands on disk.
PutOutcome put(const std::string& dir, const std::string& blobBytes, Meta meta,
               const Limits& lim);

// Ascending by stamp. Records that cannot be parsed come back with metaOk=false
// and a problem string (never dropped). A .ver with no .json is counted in
// *orphans and is NOT returned as an entry.
std::vector<Entry> list(const std::string& dir, const std::string& onlyPath = "",
                        int* otherPathCount = nullptr, int* orphans = nullptr);

bool readBlob(const std::string& dir, const Entry& e, std::string& out);  // false on missing/unreadable
int  prune(const std::string& dir, const Limits& lim);   // returns removed count
bool renameKey(const std::string& versionsRoot, const std::string& oldKey,
               const std::string& newKey);              // false if the target exists
std::vector<Entry> byChangeset(const std::string& versionsRoot, const std::string& id);
std::string sha1Hex(const std::string& bytes);          // RFC 3174, in-house, ~60 lines

std::string serializeMeta(const Meta&);
bool parseMeta(const std::string& json, Meta&);          // flat JSON, no library
}

namespace allcore::textdiff {
// Per current line: the index into versionsOldestFirst of the version that first
// introduced it and carried it unchanged since; versionsOldestFirst.size() means
// "introduced after the newest version" (unsaved edits).
std::vector<int> lineOrigins(const std::vector<std::vector<std::string>>& versionsOldestFirst,
                             const std::vector<std::string>& current,
                             const Options& o = {});
}
```

App side (`app/versions_pane.inc`, `namespace docprops` extension — defined before `OverlayPane`):

```cpp
inline QString noteVersion(QWidget* parent, const QString& dataRoot, const QString& docPath,
                           const QByteArray& savedBytes, const QString& kind,
                           const QString& reason, const QString& changeset,
                           const QString& encoding, const QString& eol, bool bom,
                           const QJsonObject& statistics);
// qCompress(savedBytes) → codec "qz"; sha1 via QCryptographicHash over savedBytes;
// savedBy/savedBySource from g_userName; revision read back from the sidecar AFTER
// noteSave has bumped it; put() then prune() with the QSettings caps.
// Returns the notice for the pane's hint line ("" when nothing to say).
// No-op when versions/enabled is false or dataRoot is empty.

static bool restoreVersionWithSafety(QWidget*, const QString& dataRoot, const QString& docPath,
                                     const QString& stamp, QString* pendingEncodingOut,
                                     QString* error);
```

**Algorithm.**

*Store.* Blob first, record second, both through `temp + rename`, so a crash never leaves a record whose
blob is missing (the reverse — a blob with no record — is the orphan case, counted and refused).
Dedupe by SHA-1 against the newest non-autosave entry: an identical re-save mints nothing and the
revision counter and version count are seen to differ honestly.

*Autosave.* The Manuscript autosaves once a minute while dirty (`app/main.cpp:33508-33513`, the timer
calls `save()` itself, so `save()` gains a `bool autosave = false` parameter the timer passes). A
**single rolling autosave slot** per document: each autosave replaces the previous autosave record and
blob; any deliberate save (⌘S, Save As, or a `pre-*` bank) removes the slot, because the permanent
version now captures that state. One row, not sixty an hour, and the notice says so.

*Prune.* Never the newest entry; never a pinned one; otherwise oldest-first — autosave, then `pre-*`,
then `save` — until both caps hold. The MB cap counts **uncompressed** bytes so the user's "MB" matches
what Properties shows.

*Restore.* The `restoreStoreWithSafety` rule (`app/main.cpp:3041`) applied to documents: bank the
current **on-disk** bytes as `reason = pre-restore` → verify the chosen version's SHA-1 → decompress →
`saveOrWarn` to the document path → reload the pane through its revert path (`revertDocument` /
`revertDraft` / `revertManuscript`). **The Overlay's revert re-runs `openFile` with `pendingEncoding_`,
so the restore sets `pendingEncoding_` from the version's recorded encoding first** — otherwise a
MacRoman version silently reloads as UTF-8 and the restore changes the text.

*Line origins (blame).* The successive-diff attribution used by version-control blame tools, computed
with `textdiff::diffLines`: every line of `V0` is attributed to 0; for `k = 1..n-1`, diff
`V[k-1] → V[k]` and carry attributions through Equal hunks by position while assigning `k` to
Insert/Change lines; finally diff `V[n-1] → current` and mark new lines as "after the newest version".
Exact diff only (no ignore rules), so the answer is defensible line for line. `O(Σ diffs)`; the window
pumps a cancellable progress and reports how many versions were walked if stopped early.

*SHA-1:* RFC 3174 / FIPS 180-4, implemented in `versions.cpp` so core needs no Qt, drilled against the
published test vectors.

**Reuses.** `docprops::noteSave` (`app/main.cpp:3982`) and the three save sites it already occupies —
Overlay `writeDocumentTo` (`:8334`, payload = the encoded `bytes`, `docEncoding_`, `docLineEnding_`),
Draft `saveDraft` (`:22069`, `toPlainText().toUtf8()`, UTF-8, LF), Manuscript `save` (`:33548`,
`toHtml().toUtf8()`, UTF-8, `HTML`); `docprops::digest` / `sidecarPath` / `textStatistics` /
`load`; `docprops::renameFileTo` (`:4066`) extended so a `(dir, ext)` pair whose ext is `"/"` renames the
**directory** `<dir>/<oldBase>` → `<dir>/<newBase>` (callers at `:8629-8630`, `:22132`, and the
Manuscript's rename add `{dataRoot_ + "/library/versions", "/"}`); `enc::decode` (`:4981`) and
`enc::encode` (`:5007`); `saveOrWarn` (`:2969`) as the only write predicate; `g_compareTexts`
(`app/compare_pane.inc:9`, wired at `app/main.cpp:38627`) and the "Compare with Saved Version" template
(`:40371`); `revertDocument` / `revertDraft` / `revertManuscript`; `harnessAutoConfirm_`
(`app/compare_pane.inc:1033,1137`); `ux::sourceBadge` (`app/ux_tokens.h:70`); the `PreferencesDialog`
page pattern; `allcore::backup.h` is **unchanged** — it keeps guarding the three shared stores;
document Versions are what "Compare with Backup" meant for translator texts
(`docs/COMPARE_SUITE_PLAN.md`).

### Acceptance tests

**CTest smoke — `versions_smoke` (new, fixture-free):**
1. `list` on a fresh temp dir → empty, `orphans == 0`, no error.
2. `put(dir, "A\n", {stamp 0001, path P, reason save, sha1 = sha1Hex("A\n")}, lim)` → `Stored`;
   `list` has 1 entry; `readBlob` returns exactly `"A\n"`; no `*.tmp` remains.
3. `put` the same sha1 at stamp 0002 → `Deduplicated`; still 1 entry, 1 blob.
4. `put("B\n", 0003)` → `list` == `[0001, 0003]` ascending; `readBlob(0003) == "B\n"`.
5. Autosave slot: `put(reason=autosave, x1)` then `put(reason=autosave, x2)` → exactly one autosave
   entry (x2) and x1's blob is gone; then `put(reason=save, b2)` → no autosave entry remains, 2 total.
6. `Limits{maxVersions=3}` after five puts → 3 remain, the oldest two gone, `_meta.json.pruned == 2`,
   the removed blobs deleted.
7. Pinned and newest are never pruned: with `[pinned C, newest D]` and `maxVersions = 1`, `prune`
   removes nothing and returns 0.
8. `maxBytes` small enough for two → oldest pruned until under the cap, counting **uncompressed**
   `bytes`.
9. `list(dir, onlyPath="/x/doc.act", &other)` excludes an entry recorded with `/y/doc.act` and sets
   `other == 1`.
10. `sha1Hex("abc") == "a9993e364706816aba3e25717850c26c9cd0d89d"`;
    `sha1Hex("") == "da39a3ee5e6b4b0d3255bfef95601890afd80709"` (FIPS 180-4 vectors).
11. `docKeyFor("/root/library/x/KL0001.ACT", "/root") == "KL0001"`;
    `docKeyFor("/Volumes/USB/KL0001.ACT", "/root")` starts `"KL0001~"` with 8 hex chars; two different
    outside paths with the same base name give **different** keys.
12. Orphan: a `0006.ver` with no `0006.json` → `list` returns it as no entry and sets `orphans == 1`.
13. Damaged: an entry whose `.ver` bytes were tampered → `readBlob` returns **false** (SHA-1 mismatch).
14. Malformed record: `0007.json` containing `not json` → the entry is returned with `metaOk == false`
    and a non-empty `problem`; the other entries still load.
15. `label` containing a tab and a newline round-trips through `serializeMeta` / `parseMeta`
    byte-identically.
16. `renameKey("doc","doc2")` → `list("doc")` empty, `list("doc2")` complete; onto an existing key it
    returns false and changes nothing.
17. `byChangeset("rif-1")` returns exactly the entries whose record carries that id, across docKeys.

**CTest smoke — `textdiff_smoke` (addition):**
18. `lineOrigins({{a,b},{a,b,c},{a,X,c}}, {a,X,c,new})` == `{0, 2, 1, 3}` (3 == "after the newest").
19. `lineOrigins({}, {a})` == `{0}`; `lineOrigins({{a}}, {})` == `{}` (no crash on empty sides).

**app `--selftest`:**
20. Overlay: save a temp document twice with different text → two entries, `revision` 1 and 2 matching
    the sidecar; a third identical save → still two, and the hint carries *"Unchanged since the last
    version"*.
21. Overlay: each entry's decompressed bytes equal the file that was on disk at that moment
    (`QFile` read == blob), and `encoding` / `lineEnding` equal `docEncoding_` / `docLineEnding_`.
22. Draft: `saveDraft` mints exactly one entry with `encoding "UTF-8"`, `lineEnding "LF"`, non-empty
    `savedBy` (this is the second pin on F0's `dataRoot_` fix).
23. Manuscript: two consecutive autosave-flagged saves → exactly one autosave entry; an explicit
    `save()` → zero autosave entries and one `save` entry.
24. Restore with `harnessAutoConfirm_ = true` on the Overlay: the file bytes equal the chosen version's,
    a new `pre-restore` entry holds the bytes that were on disk, `input_->toPlainText()` matches, and
    `isDirty() == false`.
25. Restore of a version recorded as Windows-1252: `pendingEncoding_` is set from the record before the
    reload, and the reloaded text equals the version's decoded text.
26. Restore refuses (writes nothing, returns false) when `harnessAutoConfirm_` is false.
27. Compare with Version: `ComparePane::result().differences() == 1` on the fixture; the left name
    contains `"(version"` and the right `"(editing)"`; for a Manuscript version the left lines contain
    no `<html` text.
28. Compare with Version on a Windows-1252 version with an undecodable byte → returns false, no compare
    opens, the message names the encoding and the bad-byte count.
29. `renameDocumentTo` carries `library/versions/<old>/` to `<new>/`; the old directory is gone.
30. With `versions/enabled = false` a save mints nothing and the hint carries no version notice;
    re-enabled afterwards.
31. Every Versions action under `g_harnessRun` opens no dialog (a static exec counter stays 0).
32. `tools/constitution_check.py` still reports baseline `154` (the window uses two-step buttons).

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| The Manuscript autosaves once a minute while dirty | Single rolling autosave slot; hidden by default; pruned first; the notice states the policy |
| Two documents with the same base name inside the data root share one history and one cap | Parity with the properties/glossary sidecars (so Rename carries them); each record holds its `path`; the *elsewhere* filter and marker make it visible; outside the data root the path-hash key removes the problem entirely |
| `qCompress` is a Qt container (length prefix + zlib), not readable by a bare `gunzip` | The app is the consumer; the `codec` column leaves room for `raw`; the core store never inspects blob bytes, so the battery drills it with raw blobs |
| Disk growth on large ACIP volumes | 200 versions / 64 MB per document by default, pruned oldest-first with a visible notice; compression typically 3–5× on ACIP |
| Line Origins over 200 versions of a 20k-line file runs 200 Myers diffs on the GUI thread | Bounded by a progress pump with Stop; the result states how many versions were walked if stopped early |
| `revision` must equal the version number for explicit saves | `noteSave` runs first, `noteVersion` reads the sidecar back — call order is pinned by test 20 |
| A restore over unsaved edits would lose them | Restore is disabled while the pane is dirty, with the reason shown |
| Manuscript versions are HTML; formatting-only changes look identical | Stated in the window and in the audit row |

### Effort

**20 hours.**

### Dependencies

F0 (Draft `dataRoot_`; Manuscript plain-text compare; `renameFileTo` directory pairs).

---

## F2 — Changed-folios report

### Purpose and user story

**Research verdict:** §3(d) BUILD — "changed pages only" becomes "changed folios only"; cheap, because
it is a grouping of the apparatus by folio.

**User story.** As a proofreader returning a correction pass to an input centre, I want a report that
says which folios of the volume changed and how many changes each carries — *"7 of 143 folios changed"* —
so the centre re-checks seven folios instead of reading four hundred individual readings.

### UI

**Placement.** Compare pane ▸ **REPORT ▸ Save Report…** (`app/compare_pane.inc:388`) gains two filters;
the Sessions ▾ menu gains **Copy Changed Folios**; the status line gains a fragment; the CLI
`--compare A B out.folios.md|out.folios.csv` routes by the double suffix (`app/main.cpp:41760`).
No ribbon growth.

**Controls.**
- Save Report… filters **"Changed folios, Markdown (*.folios.md)"** and
  **"Changed folios, CSV (*.folios.csv)"**. Routing uses the dialog's **selected filter** first
  (F0's `selectedFilter` out-parameter), falling back to `completeSuffix()` containing `folios` — so a
  user who types `report.md` with the folios filter selected gets folios, and the apparatus `.md` is
  never confused with it.
- Sessions ▾ ▸ **Copy Changed Folios** (Markdown to the clipboard), disabled with the tooltip
  *"The left text has no @NNNA folio markers"* when there are none.
- Status line fragment after every recompute: `· changed folios: 7 of 143`, omitted entirely when the
  left text carries no markers (never `0 of 0`).
- The existing VIEW ▸ **Minor = differences** checkbox decides whether minor hunks count; the report
  header says which way it was set.

**Honesty labels.**
- Header: *"Folios are cited from the left text's `@NNNA` markers; N of M folios changed; lines before
  the first marker are grouped as "(before first folio marker)"; a difference that spans a marker is
  counted in the folio where it starts; minor differences under the rules in force are
  [included/excluded]; K line(s) were compared raw (could not be converted)."*
- The header restates the rules in force and both file names, so the grouping is reproducible.
- A folio with only minor differences is marked **minor only (under the rules in force)** — a folio list
  must never imply a substantive change the rules classified as minor.
- With no markers in the left text the report says *"No folio markers in the left text — use the
  apparatus report"* instead of a one-row table.
- Folio numbering comes from the **left** text only; if the right side is a renumbered edition the cites
  refer to the left, and the header says so.

**Fit-ratchet note:** no ribbon controls.

### Data

**Markdown:** `# Changed folios: <A> against <B>` · the header sentence · a table
`| Folio | Lines (A) | Changed | Only in B | Only in A | Minor | Raw |` with one row per changed folio ·
then a `## @012B` section per folio carrying its apparatus bullets · then `Unchanged folios: N`.

**CSV:** header
`folio,a_first_line,a_last_line,b_first_line,b_last_line,changes,inserts,deletes,minor,unnormalised`
with the RFC-style double-quote escaping already used by `apparatusCsv`.

**Location:** a user-chosen report path via `saveOrWarn`, the clipboard, or the CLI output file. Nothing
is persisted by the app.

**Provenance fields:** left and right names, the `Options` in force, the generated-at timestamp, `N` and
`M`, per-folio `unnormalised`, and the pseudo-folio label when used.

### Engine

**Module:** additions to `core/include/allcore/textdiff.h` + `core/src/textdiff.cpp` (same translation
unit as `apparatus`); tests in `core/tests/textdiff_smoke.cpp` (the reports block).

```cpp
namespace allcore::textdiff {
struct FolioChange {
    std::string folio;              // "@012B", "(before first folio marker)", "(no folio markers)"
    int aFirstLine = 0, aLastLine = 0, bFirstLine = 0, bLastLine = 0;   // 1-based
    int changes = 0, inserts = 0, deletes = 0, minor = 0, unnormalised = 0;
    std::vector<ApparatusEntry> entries;
    bool minorOnly() const { return changes + inserts + deletes == 0 && minor > 0; }
};
struct FolioReport {
    std::vector<FolioChange> changed;
    int totalFolios = 0, unchangedFolios = 0;
    bool hasMarkers = false, includeMinor = false;
};
FolioReport changedFolios(const std::vector<std::string>& a, const std::vector<std::string>& b,
                          const Result& r, bool includeMinor);
std::string changedFoliosMarkdown(const std::string& aName, const std::string& bName,
                                  const Options& o, const Result& r, const FolioReport& f);
std::string changedFoliosCsv(const FolioReport& f);
}
```

**Algorithm.** One pass over the left text with `folioMarkerEnd` (F0) builds folio spans
`[line with marker, next marker)`; the last marker on a line wins, exactly as `citeFor` does
(`core/src/textdiff.cpp:569-581`); lines before the first marker form the pseudo-folio. Each non-Equal
hunk (skipping `unimportant` ones unless `includeMinor`) is attributed to the span containing `aBeg`;
Insert hunks use `max(0, aBeg - 1)` as `apparatus()` does (`core/src/textdiff.cpp:600`); inserts past the
end go to the last span. Counters accumulate by `Kind`, `unimportant` and `rawNormalised == false`. The
readings themselves are the existing `apparatus()` entries grouped by span. `totalFolios` counts real
markers; `unchanged = totalFolios − |changed with a real marker|`. **No new diff work.**

**Reuses:** `apparatus` / `ApparatusEntry` / `citeFor`; `apparatusCsv`'s quoting lambda
(`core/src/textdiff.cpp:627`); `summary(r)`; `ComparePane::report` / `saveReport`
(`app/compare_pane.inc:377-395`); the `--compare` CLI block (`app/main.cpp:41751-41766`); the pane's
`minorAsDiff_` flag and status rendering; F0's `folioMarkerEnd`.

### Acceptance tests

**CTest smoke — `textdiff_smoke` (additions):**
1. `a = ["@001A","ka","kha","@001B","ga","nga","@002A","ca","cha"]`, `b` = `a` with `kha`→`khA` and
   `cha`→`chA` → `changed` == `[{@001A, changes 1}, {@002A, changes 1}]`, `totalFolios 3`,
   `unchangedFolios 1`; the Markdown contains `@001A`, `@002A`, `1 of 3`… wait, `2 of 3 folios changed`,
   `Unchanged folios: 1`, and **not** `@001B`.
2. `b = "x\n" + a` → one entry with folio `"(before first folio marker)"`, `inserts 1`;
   `totalFolios` still 3.
3. `b = a + "\nja"` → the insert is attributed to `@002A`.
4. With `Options.ignoreWhitespaceChange` and `b` differing only by double spaces inside `@001B`:
   `includeMinor = false` → `changed` empty, `unchangedFolios 3`; `includeMinor = true` →
   `[{@001B, minor 1}]` with `minorOnly() == true`.
5. `scriptAgnostic` with an unconvertible line inside `@001B` → that folio's `unnormalised == 1` and the
   Markdown header contains `compared raw`.
6. CSV first line is exactly
   `folio,a_first_line,a_last_line,b_first_line,b_last_line,changes,inserts,deletes,minor,unnormalised`;
   the `@001A` row begins `"@001A",1,3,`.
7. A left text with no markers → `hasMarkers == false` and the Markdown contains
   `No folio markers in the left text`.
8. A hunk spanning a marker boundary is counted once, in the folio where it starts, and the header
   sentence carrying that rule is present.

**app `--selftest`:**
9. After `compareTexts` on the pane's fixture, `statusLabel()->text()` contains
   `changed folios: 2 of 3`.
10. `report("folios-md")` is non-empty and contains `## @001A`; `report("folios-csv")` has the header
    plus 2 data rows.
11. `saveReport` through the harness stub with a `*.folios.csv` name writes the CSV kind; with the
    folios filter **selected** and a bare `report.md` name it still writes the folios Markdown, not the
    apparatus (the `selectedFilter` path).
12. Sessions ▾ ▸ Copy Changed Folios puts Markdown on the clipboard and is disabled for a marker-less
    left text.

**CLI:**
13. `--compare A.act B.act out.folios.csv` exits 1 and writes a file whose first line is the CSV header.

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| Suffix routing cannot separate two Markdown kinds | The dialog's selected filter takes precedence (F0); both paths are pinned by test 11 |
| A difference that spans a folio boundary under-counts the next folio | Attributed to where it starts; the rule is stated in the header and pinned by test 8 |
| Right-side folio numbering may differ | Cites are from the left by design (the apparatus precedent); the header says so |

### Effort

**5 hours.**

### Dependencies

F0 (`folioMarkerEnd`, `selectedFilter`).

---

## F3 — Normalize… with preview

### Purpose and user story

**Research verdict:** §6 BUILD (small) — a Normalize… action on a selection or document offering the
compare rules as *transformations* (collapse spacing, unify shad spelling per house convention, strip
apparatus, fix line endings) with a preview diff in the Compare pane before applying.

**User story.** As an input-centre editor or translator receiving a raw file with double spaces, tabs,
trailing blanks, mixed line endings, inconsistent double-shad spelling and stray `[notes]`, I want to
choose which clean-ups to apply, see exactly which lines would change as a diff, see which lines the
rules refused to touch and why, and only then apply — knowing the original is kept as a version.

### UI

**Placement.** **Format ▸ Normalize Text…**, inserted after "Apply House Style Spacing"
(`app/main.cpp:40266`), acting on the focused plain-text editor (Overlay Document box or Draft;
selection when there is one). Compare pane Sessions ▾ ▸ **Normalize Left…** / **Normalize Right…**
(operates on `aLines_` / `bLines_` through `pushUndo()` + `recompute()`, so Undo Merge reverts it and
Save Left/Right writes it — this is how a file on disk gets normalized). Files pane right-click ▸
**Normalize…** after "Open" (`app/main.cpp` FilesPane context menu) for a file target. The batch form
lives in F4 (Replace in Files, Normalize mode). **The Manuscript is refused** with the status
*"The Manuscript is rich text; Normalize works on plain text — use the Overlay document, the Draft, or a
file in Compare. Format ▸ Apply House Style Spacing handles the Manuscript's spacing."* Dialog:
`class NormalizeDialog : QDialog` in **`app/normalize_pane.inc`**, modeless, early return under
`g_harnessRun`; its target is described by `std::function` hooks so it includes cleanly regardless of
pane definition order.

**Controls.**

| Control | What it does |
|---|---|
| Target label (read-only) | `Document box — KL0032.ACT, 4,212 lines` / `Selection — 38 whole lines` / `File — /…/KL0032.ACT (CRLF, UTF-8)` |
| Scope radio | **Whole document** · **Selection** (enabled only when a selection exists; a selection ending mid-line is **extended to whole lines**, and the label says so, because every rule is line-based) |
| ☐ Collapse runs of spaces and tabs to one space | |
| ☐ Remove trailing spaces | |
| ☐ Reduce runs of blank lines to at most **[1]** | spin 0–5 |
| ☐ Write the double shad as **[ , , ▾ ]** | Combo: `,,` · `, ,` for ACIP; `།།` · `༎` for Tibetan Unicode. **No default** — see honesty labels |
| ☐ Space before a shad: **[ keep ▾ ]** | keep · none (`GA,` / `ཀ།`) · one (`GA ,` / `ཀ །`) |
| ☐ One space after a shad | |
| ☐ Remove `[ ]` and `{ }` apparatus spans | label adds **"removes text — the original is kept as a version"**, in red |
| ☐ Remove folio markers | default off, red warning *"folio citations in reports depend on these markers"* |
| Line endings **[ keep ▾ ]** | keep · LF · CRLF · CR |
| ☐ Ensure the file ends with a line break | |
| ☐ Apply house-style spacing to English lines | reuses `editops::applyHouseStyleSpacing`; disabled with a tooltip when no House Style switch is on |
| **Preview in Compare** | Runs `textnorm::normalizeText` and calls `g_compareTexts(target + " (current)", before, target + " (normalized)", after)`. Result label: *"312 line(s) would change · 4 line(s) left alone (listed) · line endings CRLF → LF"* |
| **List untouched lines** | Expands a list of line numbers with reasons; clicking one jumps the editor to that line |
| **Apply** | Enabled only after a preview computed **for exactly these settings and this text**; any change to a switch or to the editor disables it again. Editor/selection targets: one `QTextCursor` edit block, so a single ⌘Z reverses it, and nothing is written to disk until Save (which mints a Version). File targets: `noteVersion(reason = pre-normalize)` **must return Stored** → `saveOrWarn` with the chosen EOL, the file's own encoding and its BOM → reload the Overlay if it holds that file and is clean |
| **Save as preset…** / preset combo | Presets live in `<dataRoot>/data/house_style/normalize_presets.json` (release-owned) plus the user's own in `QSettings normalize/presets`. **The combo is hidden entirely when there are no presets** — it appears the moment the publishing sheet arrives or the user saves one. No empty control on first run. |
| **Close** | Switches remembered via `sess::remember` under `normalize/*` |

**Honesty labels.**
- Banner: *"Every rule is a mechanical transformation, described beside it. The GMR / input-centre
  formatting sheet has not been supplied; when it is, its rulings become presets here. Nothing is
  guessed: a line the rules cannot classify is left byte-identical and listed."*
- **Line classification is stated:** shad rules run only on lines the classifier calls **ACIP**
  (upper-case letters, no lower-case) or **Tibetan Unicode**; English/Wylie lines are never touched by
  shad rules (*"a comma is a comma there"*) and are counted as *not ACIP/Unicode*; **mixed-script**
  lines (upper- and lower-case together, e.g. an ACIP line with an English comment) are left
  **byte-identical** and listed as *refused: mixed script*.
- **The double-shad and space-before-shad forms have no default.** Both controls ship unchecked with
  the label *"awaiting the house ruling (Preferences ▸ House Style)"*. Shipping a wrong default would
  silently re-spell every shad in a volume.
- **Normalize works strictly per line and never joins or splits lines.** An ACIP syllable that continues
  onto the next line is unaffected by "remove trailing spaces", and the preview shows every trailing-space
  removal as a real change, so a later re-join cannot silently fuse two syllables. Stated in the banner.
- Lossy switches (apparatus, folio markers) say *"removes text"* and every removal appears in the
  preview as a deletion.
- The result label reports counts computed **from the diff the user is looking at** — "312 line(s) would
  change" is counted, never estimated — and states line-ending changes as `CRLF → LF`.
- File applies say *"the original is kept as version `<stamp>`"*.
- **Tibetan Unicode canonical/stacking normalization is not offered.** See "Deliberately omitted".

**Keyboard:** none new (the Format menu item takes the menu's own accelerator).

**Fit-ratchet note:** no ribbon controls.

### Data

No new store. Options as compact JSON in `QSettings("ALL","TranslationTool")` under
`normalize/last` and `normalize/presets`:

```json
{ "collapseSpaces": true, "trimTrailing": true, "maxBlankLines": -1,
  "doubleShad": "" , "spaceBeforeShad": "keep", "spaceAfterShad": false,
  "stripApparatus": false, "stripFolioMarkers": false,
  "eol": "keep", "ensureFinalNewline": false, "houseStyleSpacing": false }
```

Release presets: `<dataRoot>/data/house_style/normalize_presets.json` —
`{"schema":"all-normalize-presets/1","presets":[{"name":"…","source":"GMR formatting sheet §n","options":{…}}]}`.
The `source` field is the provenance of the ruling and is shown in the combo's tooltip.

File applies write a `pre-normalize` Version (F1 schema) whose `changeset` is empty for a single file and
set when the run came from F4.

**Provenance fields:** `preset.source`; the `pre-normalize` version's `savedBy` / `stamp` / `sha1`; the
per-line refusal list surfaced in the dialog.

### Engine

**Module:** `core/include/allcore/textnorm.h` + `core/src/textnorm.cpp` (Qt-free), smoke
`core/tests/textnorm_smoke.cpp` registered in `core/CMakeLists.txt`. The codepoint walker and
whitespace helpers now file-static in `core/src/textdiff.cpp:42-59,75` are promoted to an internal
`core/src/text_util.h` shared by both translation units.

```cpp
namespace allcore::textnorm {

enum class Eol { Keep, LF, CRLF, CR };
enum class ShadSpace { Keep, None, One };
enum class LineClass { Acip, TibetanUnicode, Other, Mixed };

struct Options {
    bool collapseSpaces = false, trimTrailing = false;
    int  maxBlankLines = -1;                 // -1 = keep
    std::string doubleShad;                  // "" = leave; ",," | ", ," | "།།" | "༎"
    ShadSpace spaceBeforeShad = ShadSpace::Keep;
    bool spaceAfterShad = false;
    bool stripApparatus = false, stripFolioMarkers = false, ensureFinalNewline = false;
    Eol eol = Eol::Keep;
};
struct Refusal { int line; std::string reason; };   // 0-based line
struct Result {
    std::vector<std::string> lines;
    std::string eol;                          // the output EOL ("" unchanged rules)
    int changedLines = 0, shadRulesSkipped = 0;
    bool finalNewlineAdded = false;
    std::vector<Refusal> refused;             // left byte-identical
    std::vector<std::string> notes;
};

LineClass classifyLine(const std::string& line);
bool bracketsBalanced(const std::string& line, char open, char close);
std::string detectEol(const std::string& text);
std::string normalizeLineText(const std::string& line, const Options& o,
                              LineClass* cls, std::string* refuseReason);
Result normalizeText(const std::vector<std::string>& lines,
                     const std::string& detectedEol, const Options& o);
}
```

**Algorithm.** Per line, in a fixed, documented, idempotent order:

1. **Classify** (the same judgement `textdiff` already makes at `core/src/textdiff.cpp:84-94`): any
   codepoint in U+0F00–U+0FFF → `TibetanUnicode`; upper-case letters and no lower-case → `Acip`; both
   cases → `Mixed`; otherwise `Other`.
2. **Strip apparatus** — `[…]` and `{…}` spans, **only if both bracket kinds are balanced on the line**.
   An apparatus note wrapped across two lines is *not* matched by the span pattern and would otherwise
   pass through unnoticed; here the line is left byte-identical and refused with
   `"unbalanced [ ]"` / `"unbalanced { }"`.
3. **Strip folio markers** via `textdiff::folioMarkerEnd`; a marker glued to text (`@012BX`) refuses the
   line with `"marker glued to text"`.
4. **Shad rules** — `Acip` and `TibetanUnicode` lines only; `Mixed` lines are refused with
   `"mixed script"`; `Other` lines increment `shadRulesSkipped`. Implemented as a **codepoint walk**, not
   a regex: `std::regex` (which is what Qt-free `allcore` has) **has no lookbehind**, so a
   "space before a shad" rule written as `(?<=\S),` could not compile. The walk treats the double shad
   as one atomic unit (`,,`, `, ,`, `,` + spaces + `,`; `།།`, `༎`), rewrites it to the chosen target,
   applies the before/after spacing to shad codepoints only — ACIP `,`, U+0F0D `།`, U+0F0E `༎`,
   U+0F11 `༑`, U+0F14 `༔` — and **never touches the tsheg U+0F0B `་`**, which is a letter-level
   separator owned by the converters.
5. **Collapse spaces** — runs of `[ \t]` → one space. The tsheg is not whitespace and is untouched.
6. **Trim trailing whitespace.**

Then, document-level: blank-line runs reduced to `maxBlankLines`; the output EOL chosen; the final
newline ensured. `changedLines` counts lines whose output differs from their input. Every rule is a fixed
point of itself, so `normalizeText(normalizeText(x))` == `normalizeText(x)` (pinned as a test). Nothing
converts scripts, lowercases anything, or removes a single shad.

**Reuses:** `textdiff::normalizeLine`'s building blocks (the apparatus pattern, `collapseWs`,
`isTibetanPunctCp`, the codepoint walker) — **promoted, not copied**, and note that `normalizeLine`'s
*output* is a lossy comparison key and is never used here; `textdiff::folioMarkerEnd` (F0);
`textdiff::looksTibetan`; `splitLines` / `joinLines`; `editops::HouseStyle::fromSettings` and
`applyHouseStyleSpacing` (`app/main.cpp:4649+`) for the English pass, applied on the `QTextDocument`
side **after** the core pass (the order is fixed and stated, because collapse-then-house-style and
house-style-then-collapse differ for the two-spaces-after-sentence rule);
`editops::selectedText` / the `QTextCursor` edit-block idiom from `editops::replaceAll`;
`g_compareTexts` as the preview; `ComparePane::pushUndo` / `recompute` / `saveSide` for the pane variant;
`cmp::readText` for a file target's EOL/binary detection **plus `enc::decode(..., "UTF-8", &bad)` for the
strict check** (see F4's encoding rule); `docprops::noteVersion` for the `pre-normalize` bank;
`saveOrWarn` for the write.

### Acceptance tests

**CTest smoke — `textnorm_smoke` (new, fixture-free):**
1. `"A  B\t C  "` with `{collapseSpaces, trimTrailing}` → `"A B C"`, `changedLines 1`.
2. `"BLA MA ,LA"` with `spaceBeforeShad = None` → `"BLA MA,LA"`; with `One` → `"BLA MA ,LA"`;
   `spaceAfterShad` on `"BLA MA,LA"` → `"BLA MA, LA"`.
3. `"KUN , , BDE"` with `doubleShad = ",,"` → `"KUN,, BDE"`; with `", ,"` → `"KUN, , BDE"`;
   with `""` → unchanged (the double shad is one atomic unit).
4. Single shad is never merged: `"KA, KHA"` with `doubleShad = ",,"` → unchanged.
5. English: `"Yes, no, maybe"` with every shad rule on → unchanged, `changedLines 0`, `refused` empty,
   `shadRulesSkipped 1`.
6. Mixed: `"SEMS ,, sems"` with shad rules on → byte-identical, `refused == {{0, "mixed script"}}`.
7. Unicode: `"ཀ།ཁ"` with `spaceAfterShad` → `"ཀ། ཁ"`; `"ཀ ། ཁ"` with `spaceBeforeShad = None` →
   `"ཀ། ཁ"`; `"ཀ།།ཁ"` with `doubleShad = "༎"` → `"ཀ༎ཁ"`.
8. **Tsheg protection:** `"བཀྲ་ཤིས་  ཀ"` with `collapseSpaces` collapses only the ASCII run; every
   U+0F0B survives byte-identically.
9. Apparatus: `"[12] BLA {MA} LA"` with `stripApparatus` + `collapseSpaces` → `"BLA LA"`;
   `"[12 BLA"` → **unchanged**, `refused == {{0, "unbalanced [ ]"}}`; `"BLA }"` → refused
   `"unbalanced { }"`.
10. Folio markers: `"@001A KA"` with `stripFolioMarkers` → `"KA"`; `"@001AX KA"` → refused
    `"marker glued to text"`.
11. `"a\r\nb\r\n"` with `eol = LF` → lines `{a,b}`, `eol == "\n"`; `Keep` → `"\r\n"`; `"a\rb"` `Keep`
    → `"\r"`.
12. `"a\n\n\n\nb"` with `maxBlankLines = 1` → `{a, "", b}`; `ensureFinalNewline` sets
    `finalNewlineAdded` when the input lacked one.
13. **Idempotence:** for every fixture above, `normalizeText(normalizeText(x, o).lines, o).lines ==
    normalizeText(x, o).lines`.
14. All options off → lines byte-identical, `changedLines == 0`.
15. Refused lines are byte-identical in the output and carry the correct 0-based line number.
16. **No line count changes** except by the blank-line rule: for fixtures 1–12 with `maxBlankLines = -1`,
    `result.lines.size() == input.size()` (Normalize never joins or splits a line).

**app `--selftest`:**
17. With the Overlay holding `"BLA  MA ,LA\n"`, the pure driver with `{collapseSpaces,
    spaceBeforeShad=None, spaceAfterShad}` and `preview = true` returns `"BLA MA, LA\n"` **without**
    changing the editor, and the Compare pane shows names ending `(current)` / `(normalized)` with
    `result().differences() == 1`.
18. `apply = true` changes the editor in **one** edit block; a single `document()->undo()` restores the
    original; `isDirty()` is true; nothing on disk changed.
19. File target on a temp CRLF file with a UTF-8 BOM and `eol = LF` → the written bytes start with
    `EF BB BF`, contain no `\r`, end with `\n`, and a `pre-normalize` version exists whose bytes equal
    the original.
20. File target on a binary temp file → refused with `"binary"`; on a Latin-1 file with invalid UTF-8 →
    refused with *"not UTF-8 — open it in the Overlay and choose the encoding first"*; nothing written.
21. Manuscript in front → the Format item refuses with the stated message and changes nothing.
22. Compare pane: Normalize Right pushes exactly one undo snapshot; `undo()` restores `bLines_`.
23. **Apply is disabled** until a preview exists for exactly the current settings; changing any switch
    disables it again (a random gauntlet click can never write).
24. The preset combo is **absent** when no preset file and no user preset exist.

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| The house double-shad and shad-spacing forms are unknown | Both controls ship unchecked with "awaiting the house ruling"; open question 1; no preset ships until the sheet arrives |
| Apparatus stripping is lossy | Red label, mandatory preview, ⌘Z for editor targets, `pre-normalize` version for file targets |
| Mixed-script classification is a case heuristic; dirty input-centre files with a stray lower-case letter produce many refusals | The count and the list are shown, and the refused lines are byte-identical — the file is never half-normalized without saying so; the list is grouped by reason so a hundred "mixed script" lines read as one finding |
| Promoting helpers out of `textdiff.cpp` touches a battery-covered TU | `textdiff_smoke` must stay green; the move is mechanical and lands in its own commit |
| A selection ending mid-line | Extended to whole lines; stated in the target label; pinned by test 16's line-count invariant |

### Effort

**11 hours.**

### Dependencies

F0 (`folioMarkerEnd`, shared text helpers); F1 (`pre-normalize` bank for file targets — the editor
variant works without it, and says so when the document has never been saved).

---

## F4 — Replace in Files… with mandatory preview

### Purpose and user story

**Research verdict:** §7 BUILD — Replace in Files with a **mandatory preview**: every proposed change
listed per file as a diff (the Compare engine's hunks), tick/untick, then apply through `saveOrWarn`;
a result window with counts; an undo file set (the versions from §5); wildcard include/exclude filters
as in Notepad++. *"This is the input-centre formatter's core (roadmap item A)."*

**User story.** As a proofreader of an input-centre batch, I want to fix one systematic error across a
whole folder of ACIP files — see every occurrence with its line before and after, untick the ones that
are right as they are, apply only to the files I ticked, get an honest count of what changed and what was
skipped and why, and undo the whole operation later if the fix was wrong.

### UI

**Placement.**
- **Find ▸ Replace in Files…** (⇧⌥⌘F), immediately after "Find in Files…" (`app/main.cpp:40099`),
  seeded with `FindDialog::needle()` and its replacement (a `replacement()` getter is added beside
  `setReplacement`).
- **Find ▸ Undo Last Replace in Files…**, immediately after it — a *named* entry point, not only a
  button inside the window that ran the batch.
- **Files pane right-click ▸ Replace in these files…** after "Open", with the scope seeded from
  `FilesPane::selectedPathsIn(ix)` (`app/main.cpp:24290`), folders expanded recursively under the
  eligibility filter.
- **Search pane ▸ Search Results tab**: a **Replace in these folders…** button under the results
  (inside the inner tab, not the ribbon), **enabled only when the query is one plain term** — `fields_[0]`
  set, `fields_[1..7]` empty, no NEAR — seeding the needle from that term and the scope from the checked
  user-folder rows. Otherwise disabled with the tooltip *"Replace needs one plain term; OR / NEAR queries
  cannot be replaced."*
- Window: `class ReplaceFilesWindow : QDialog` in **`app/replace_files.inc`**, modeless, included after
  `compare_pane.inc` so it sees `cmp::`, `textdiff`, `saveOrWarn`, `editops`, `docprops`; returns early
  under `g_harnessRun`; all planning, apply and undo functions are static and selftest-driven.

**Controls.**

| Control | What it does |
|---|---|
| Mode tabs **Find and replace** \| **Normalize (rules)** | The second embeds F3's rule panel — batch normalization is what roadmap item A actually needs |
| **Find** / **Replace with** editable combos | Histories shared with the Find bar (`find/history`, `find/replaceHistory`) |
| ☐ Regular expression · ☐ Match case · ☐ Whole word · ☐ Preserve case | `editops::FindOpts` fields; Preserve case disabled when the needle has no letters |
| **In** | Folder list with **Add folder…** / **Remove**, or a read-only *"N selected files"* label when seeded from the Files pane; ☑ Subfolders; ☐ Hidden |
| **Include files** | default `*.txt;*.act;*.inc;*.ace;*.acip;*.md` |
| **Exclude** | globs, `!` prefix negates (Notepad++ semantics); default `*META.TXT;.index.db*` |
| **File cap** spin | default 4000; files past the cap are **counted**, never silently dropped |
| ☐ Allow changes to texts under `/library/` | **default off**; library files are listed with a **library** badge, unticked and disabled until this is on — the same question `OverlayPane::saveDocument` asks per save |
| **Preview changes** | Bounded scan through `allcore::listEligibleFiles` (F0) with a progress label *"Read 312 of 1,204 files…"* and a **Stop** button (`processEvents` pump with a re-entrancy guard) |
| Preview tree (checkable) | File rows `<relative path> — 14 occurrences · CRLF · BOM · library` (checked by default) with **occurrence children** `line 57 · @012B.3 · KHYB PA ▸ KHYAB PA` (checked by default, the old and new whole lines in a tooltip). Ticking a file toggles its children. Children are created **lazily on expand**. Double-click a file → `g_compareTexts(name + " (before)", before, name + " (after, ticked occurrences)", after)`. Right-click ▸ **Open in Overlay at this line** via `g_openAtLine` |
| **Tick all** / **Untick all** / counts label | `38 files, 233 of 241 occurrences ticked` |
| Footer counts (read-only) | `1,204 files read · 38 with matches · 233 of 241 occurrences ticked · 5 skipped (2 binary, 3 not UTF-8) · 0 over 10 MB · 0 past the file cap · stopped early: no` |
| ☐ Update the folder's search index afterwards | Enabled only when `<folder>/.index.db` exists |
| **Apply to ticked files** | Enabled only after a preview **for exactly these inputs**; any change invalidates it. Two-step confirmation naming *"17 files will be rewritten (2 of them canonical library texts under /library/)"*; under the harness a `harnessAutoConfirm_` flag (default false) decides |
| Result panel | `Changed 17 files (233 occurrences) · 0 write failures · 3 skipped: 1 open with unsaved edits, 1 changed on disk since the preview, 1 not confirmed` with clickable file rows (`goferopen:` anchors → `g_openAtLine`) and the changeset id |
| **Undo this run…** | Two-step; restores each recorded file from its `pre-replace` version (each restore banks a `pre-restore` first); result *"Restored 16 files; 1 skipped (changed since this run); 0 failures"* |
| **Close** | Scope, globs and switches remembered via `sess::remember` under `replacefiles/*` |

**Honesty labels.**
- **Counts never come from the Gofer index.** The index matches whole tokens (`libindex.h`); this window
  counts matches in the bytes it read itself, and says so: *"counted in the files' text, not in the
  search index."* A user who saw 12 hits in Search may see 15 occurrences here.
- **Every skip is named, never silently omitted:** unreadable · binary · **not UTF-8** *("open it in the
  Overlay with its encoding and save it as UTF-8 first")* · over 10 MB · excluded by glob · past the file
  cap · **open in the app with unsaved edits** · **changed on disk since the preview**.
- **Script-mismatch notice.** When the needle's dominant script (ACIP / Wylie / Tibetan Unicode /
  English) differs from the dominant script of the scanned files, the footer carries:
  *"The search text is Tibetan Unicode; the files scanned are ACIP. A count of 0 here means the two
  scripts do not match, not that the text is clean. Convert the search text (Format ▸ Tibetan: To ACIP)
  or use the Convert pane."* **A script-mismatched zero must never read as a clean bill of health.**
- Banner: *"Nothing is written until you press Apply to ticked files."*
- **The preview is the apply:** the same function on the same text produces both; each file is re-hashed
  at apply and refused if it changed since the preview.
- Write verdicts come only from `saveOrWarn`; a failed file is listed as `NOT WRITTEN — <reason>` and is
  never counted as changed.
- After apply: *"The search index for `<folder>` is out of date until Library ▸ Update search index
  runs"* unless the checkbox ran it, in which case the indexer's own result line is shown.
- The undo panel names the version stamps restored and states that each restore itself kept a
  `pre-restore` version.

**Keyboard:** ⇧⌥⌘F opens the window.

**Fit-ratchet note:** no ribbon controls.

### Data

**Changeset record** — the durable audit object for the run:
`<dataRoot>/library/versions/_changesets/<id>.json`

```json
{ "schema": "all-changeset/1",
  "id": "rif-20260908-214512",
  "kind": "replace-in-files|normalize-in-files",
  "ranBy": "Adam", "ranBySource": "team-name|login",
  "ranAt": "2026-09-08T21:45:12",
  "spec": { "mode": "regex", "find": "KHYB", "replace": "KHYAB",
            "regex": false, "caseSensitive": true, "wholeWord": false,
            "preserveCase": false },
  "scope": { "folders": ["/…/batch7"], "include": ["*.act"], "exclude": ["*META.TXT"],
             "recurse": true, "hidden": false, "fileCap": 4000 },
  "files": [ { "path": "/…/KL0032.ACT", "docKey": "KL0032",
               "preStamp": "20260908-214512-001", "preSha1": "…", "postSha1": "…",
               "occurrences": 14, "unticked": 1, "eol": "CRLF", "bom": false,
               "status": "changed" } ],
  "summary": { "filesRead": 1204, "withMatches": 38, "changed": 17,
               "skipped": 3, "writeFailed": 0, "occurrences": 233 },
  "undone": null }
```

`status` ∈ `changed | skipped:<reason> | write-failed`. `undone` becomes
`{"at":…, "by":…, "restored":16, "skipped":1}` after an undo.

Per rewritten file: a `pre-replace` Version (F1 schema) carrying this `changeset` id. `QSettings`:
`replacefiles/include`, `/exclude`, `/subfolders`, `/hidden`, `/lastFolder`, `/allowLibrary`,
`/updateIndex`, `/lastChangeset`; `find/history` and `find/replaceHistory` reused.

**Provenance fields:** `id`, `ranBy`, `ranBySource`, `ranAt`, `spec`, `scope`, per-file `preStamp`,
`preSha1`, `postSha1`, `eol`, `bom`, `status`, the summary counts, and `undone`.

### Engine

**Module:** app-layer planner in `app/replace_files.inc`, `namespace replf` — pure, UI-free functions
drilled by `app --selftest`. **The regex engine must be `QRegularExpression`**, both because it is what
the single-editor Find bar uses (so results agree exactly) and because it is UTF-16 aware:
`std::regex` over UTF-8 bytes treats `.` as one byte and would mangle Tibetan Unicode. Scope comes from
`allcore::listEligibleFiles` (F0); the preview diff and partial application from
`allcore::textdiff::diffLines` / `applySelected`; the undo set from `allcore::versions`, which gains the
changeset reader/writer (tested in `versions_smoke`).

```cpp
namespace replf {

struct Occ {                       // one match — the tick unit a proofreader reasons about
    int start = 0, len = 0;        // UTF-16 offsets into the file text
    int line = 0;                  // 1-based
    QString cite;                  // "@012B.3" or "line 57"
    QString oldLine, newLine, with;
    bool ticked = true;
};
struct FilePlan {
    QString path, docKey, eol, skipReason;
    bool endsWithEol = false, bom = false, library = false;
    QByteArray preSha1;
    QStringList before, after;
    QVector<Occ> occs;
    allcore::textdiff::Result diff;   // before → after, for the drill-down view
};
struct Plan {
    QVector<FilePlan> files;
    allcore::WalkStats walk;
    int skippedBinary = 0, skippedNotUtf8 = 0, skippedUnreadable = 0;
    bool stoppedEarly = false;
    QString scriptNotice;             // "" unless the needle's script differs
};

FileRead readForRewrite(const QString& path, qint64 maxBytes);
//   raw read; binary = NUL in the first 8 KB; BOM detected and stripped;
//   STRICT UTF-8: enc::decode(bytes, "UTF-8", &bad) with bad > 0 → not UTF-8.
QVector<Occ> planOccurrences(const QString& text, const QString& needle, const QString& with,
                             const editops::FindOpts& o, bool* regexInvalid);
QString applyTicked(const FilePlan& f);          // back-to-front over ticked occurrences only
QByteArray bytesForWrite(const QString& text, const QString& eol, bool bom, bool endsWithEol);
Plan buildPlan(const QStringList& roots, const ReplaceSpec& spec, /*…globs, cap…*/,
               const std::function<bool(int,int)>& pump);
ApplyReport applyPlan(QWidget*, const QString& dataRoot, Plan&, const ReplaceSpec&,
                      bool allowLibrary, bool confirmed,
                      const std::function<QString(const QString&)>& openDirtyReason);
ApplyReport undoChangeset(QWidget*, const QString& dataRoot, const QString& id, bool confirmed);
}
```

**Algorithm.**

*Scope.* `allcore::listEligibleFiles` (F0) — the **same** eligibility rule as the Gofer scan and the
library index, with include/exclude globs, an optional recursive walk, and a file cap whose overflow is
**counted** in `WalkStats.capped`.

*Plan.* Per file: raw read; binary refused; **strict UTF-8** via `enc::decode(bytes, "UTF-8", &bad)` —
`bad > 0` refuses the file and names it. (`cmp::readText`, `app/compare_pane.inc:20-29`, does a bare
`QString::fromUtf8` and **cannot report a decode failure**; using it on a rewrite path would decode a
Windows-1252 input-centre file to U+FFFD and re-encode it — silent, irreversible corruption of exactly
the batch this feature exists to fix. `cmp::readText` stays the *display* reader; `readForRewrite` is the
rewrite reader.) EOL and BOM detected. Then **one `QRegularExpression` pass over the whole text** with
`editops::patternFor`, collecting matches with line numbers (count of `\n` up to the offset) and folio
cites (walk back to the last marker with F0's pattern). Replacements are built with
`editops::expandReplacement` (`$N`, `\n`, `\t`) and `editops::shapeCase` when Preserve case — the same
functions the Find bar's Replace All uses, so the two agree. Zero-length matches advance one character
(no infinite loop). `after` = the whole-text replacement re-split into lines; `diff = diffLines(before,
after)` gives the file-level drill-down view; `preSha1` is recorded.

*Apply — two phases.*
Phase 1 verify: re-read and re-hash each file; a mismatch against `preSha1` skips it as
**changed on disk since the preview**; a file that is the Overlay/Draft/Manuscript document **with
unsaved edits** is skipped by name; library files are skipped unless allowed; the confirmation must have
been given.
Phase 2 write: `docprops::noteVersion(reason = pre-replace, changeset = id)` — **if the bank does not
return `Stored`, that file is abandoned with `"could not bank the undo copy"` and counted as not
written**; then `bytesForWrite` re-joins with the file's own EOL, re-prepends its BOM and restores its
trailing line break; then `saveOrWarn`. A clean open document holding that file is reloaded through its
revert path.

*Undo.* For each recorded file whose **current SHA-1 still equals `postSha1`**, bank the current bytes as
`pre-restore` (same changeset) and restore the `pre-replace` version. A file edited since the run is
**skipped with the reason**, never clobbered. The changeset's `undone` block is written.

*Occurrence vs hunk.* The tick unit shown to the user is the **occurrence** — one row per match, which is
what a proofreader reasons about — but the underlying partial application goes through
`textdiff::applySelected` where a hunk model is needed (Normalize mode, and the file-level Compare
drill-down), so partial application stays a core-tested primitive.

*Normalize mode.* `textnorm::normalizeText` (F3) replaces the regex pass; occurrences become
changed-line rows; everything else is identical.

**Multi-line patterns are refused**, explicitly: *"Patterns that span lines cannot be shown as
tickable occurrences. Use Find ▸ Replace… in one document for those."* Half-supporting them under a
tick model would be worse than refusing.

**Reuses:** `editops::FindOpts` / `patternFor` / `expandReplacement` / `shapeCase`
(`app/main.cpp:4343-4472`); `FindDialog::needle` / `setNeedle` / `setReplacement` plus a new
`replacement()`; `allcore::listEligibleFiles` (F0); `allcore::textdiff::diffLines` / `diffInline` /
`applySelected`; `g_compareTexts`; `ComparePane::saveSide`'s EOL rejoin rule
(`app/compare_pane.inc:370-371`); `FolderComparePage::confirmOps` + `harnessAutoConfirm_`
(`app/compare_pane.inc:1033,1137`); `FilesPane::selectedPathsIn` (`app/main.cpp:24290`); the Gofer
pane's checked folder rows and `fields_`; `anchorEnc` + `goferopen:` anchors + `g_openAtLine`;
`goferSkipNotice` wording (`app/textreport.h`); `OverlayPane::saveDocument`'s library-guard wording
(`app/main.cpp:8282-8290`); `OverlayPane::documentPath` / `isDirty` / `revertDocument`,
`DraftPane::draftPath` / `draftDirty`, the Manuscript equivalents; `docprops::noteVersion` and
`allcore::versions` for the undo set; `LibraryIndex::update` + `indexResultHtml` for the optional
re-index; `saveOrWarn` as the only write predicate.

### Acceptance tests

**CTest smoke — `versions_smoke` (changeset additions):**
1. `writeChangeSet` / `readChangeSet` round-trip a record with two files byte-identically; a needle
   containing a tab and a quote survives.
2. `listChangeSets` returns ids newest-first; `readChangeSet` of a missing id returns false.

**app `--selftest` — pure planner:**
3. `planOccurrences("KHYB PA, KHYB", "KHYB", "KHYAB", literal)` → 2 occurrences on line 1 with
   `newLine == "KHYAB PA, KHYAB"`; `applyTicked` with both ticked → that string; with `occs[0].ticked =
   false` → `"KHYB PA, KHYAB"`.
4. Regex `"(K)HYB"` → `"$1HYAB"` gives the same result; an invalid regex `"("` sets `regexInvalid` and
   yields zero occurrences (no crash, no preview rows).
5. Preserve case: needle `khyb`, replacement `khyab`, text `"Khyb khyb KHYB"` → `"Khyab khyab KHYAB"`.
6. Whole word: `"SEMS"` in `"SEMS SEMSPA"` → 1 occurrence.
7. Zero-length regex `"^"` on `"a\nb"` → 2 occurrences; `applyTicked` with `"X"` → `"Xa\nXb"`
   (terminates).
8. Cite: an occurrence on the third line after `@012B` has `cite == "@012B.3"`.
9. `bytesForWrite` preserves CRLF, a UTF-8 BOM and the trailing line break; a file that lacked a
   trailing break still lacks one.
10. Multi-line needle with regex on → refused with the stated reason, zero occurrences.

**app `--selftest` — window, headless:**
11. Temp folder: `f1.txt` (CRLF, BOM, `"KHYB\r\nKHYB\r\n"`), `f2.act` (UTF-8, `"KHYB PA"`), `f3.txt`
    (binary NUL), `f4.txt` (Windows-1252 byte 0xE9 forming invalid UTF-8), `sub/f5.txt` (`"KHYB"`).
    Preview with Subfolders on → `filesRead 5`, `withMatches 3`, `skipped 2 (1 binary, 1 not UTF-8)`,
    `occurrences 4`.
12. Untick `f5`; Apply with `harnessAutoConfirm_` → `f1.txt` == BOM + `"KHYAB\r\nKHYAB\r\n"`,
    `f2.act` == `"KHYAB PA"`, `f5` byte-identical to the original; each rewritten file has exactly one
    `pre-replace` version whose bytes equal the original; `_changesets/` holds one record with two
    `changed` rows and one `unticked` row.
13. **Undo:** `undoChangeset(id)` → `f1.txt` and `f2.act` byte-identical to their originals, each with a
    new `pre-restore` version; the record's `undone` block is filled.
14. **Changed-since-preview:** modify `f2.act` on disk between Preview and Apply → it is skipped with
    `"changed on disk since the preview"`, **is not written**, and is listed by name.
15. **Changed-since-run:** modify `f1.txt` after the apply, then Undo → `f1.txt` is skipped with
    `"changed since this run"` and left alone; `f2.act` is still restored.
16. **Open and dirty:** with `f1.txt` open in the Overlay and dirty, Apply lists it as
    *"open with unsaved edits"*, leaves the file and the editor untouched; with it open and clean,
    the file is rewritten and `overlay->documentText()` reflects it.
17. **Library guard:** a scope under `<tmp>/library/` is planned with `library == true`; Apply with
    `allowLibrary = false` writes nothing and counts it skipped; the confirmation text contains
    *"canonical library texts"*; with the switch on it proceeds.
18. **Bank failure:** with the versions directory made unwritable, Apply abandons that file with
    *"could not bank the undo copy"* and **does not write it**.
19. **File cap:** a scope of 4,001 tiny files with cap 4,000 reports `stopped early: yes`,
    `walk.capped == 1`, and reads exactly 4,000.
20. **Script notice:** a Tibetan-Unicode needle over an ACIP folder sets `scriptNotice` non-empty and
    the footer shows it beside the zero count.
21. Search-tab button is disabled when `fields_[1]` is non-empty and enabled when only `fields_[0]`
    holds a plain term.
22. **Normalize mode** with `{collapseSpaces, spaceAfterShad}` on `"BLA  MA ,LA"` → one changed line;
    Apply writes `"BLA MA, LA"` and mints a `pre-normalize` version.
23. Apply is disabled until a preview exists for exactly the current inputs (a gauntlet click is inert),
    and every confirmation returns false without `harnessAutoConfirm_`.
24. `tools/constitution_check.py` baseline `154` unchanged.

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| Non-UTF-8 input-centre files (MacRoman / Windows-1252) | Skipped and **named** with the conversion path (rule 3); never transcoded on a guess. Wiring `enc::decode` per file is filed with the Compare pane's existing encoding open item |
| A replacement that inserts or removes line breaks changes line counts | The diff shows it as Insert/Delete hunks; the occurrence rows still carry the original line numbers, and the drill-down Compare is the truthful view |
| Large scopes on the GUI thread | 4,000-file cap and 10 MB per file, a real Stop button, lazy occurrence children; the cap is disclosed |
| A 4,000-file preview tree is heavy | Occurrence children created on expand; file rows only until then |
| `.index.db` goes stale | Disclosed with a link to Library ▸ Update search index; never re-indexed silently on the GUI thread |
| Bulk rewriting canonical `/library/` texts is a curatorial act | Default-off switch, badge, separate confirmation line; open question 4 asks whether it should be refused outright |
| Two same-named files in different folders | F1's `docKey` path-hash for out-of-root files; the changeset's `(path, preStamp)` pairs keep undo exact regardless |
| Regex semantics must not diverge from the Find bar | `QRegularExpression` and the same `editops` helpers; never `std::regex` |

### Effort

**22 hours.**

### Dependencies

F0 (`listEligibleFiles`, `applySelected`); F1 (`pre-replace` banks, restore rule, changeset store);
F3 (the `textnorm` engine for Normalize mode).

---

## F5 — Apply Patch…

### Purpose and user story

**Research verdict:** §13 BUILD — read a unified diff, match hunks against the current text (exact, then
with offset, then fuzz 1–2 lines of context), preview in the Compare pane, apply; failures listed, never
forced.

**User story.** As a translator whose corrector sends a unified diff for a long ACIP file instead of the
whole file, I want to load the patch against my copy, see which hunks fit exactly, which fit at a shifted
line, which needed loosened context, which are **already applied**, and which do not fit at all — then
look at the result as a diff, save what fits, keep the rejected hunks in a `.rej` file for the corrector,
and know my previous text is a version I can restore.

### UI

**Placement.** **Tools ▸ Compare ▸ Apply Patch…**, after "Open Conflict File…"
(`app/main.cpp:40385`); Compare pane **Sessions ▾ ▸ Apply Patch to Left… / to Right…**; CLI
`--apply-patch <target> <patch> [--out <file>] [--dry-run] [--fuzz N] [--reverse]`, beside `--compare`
(`app/main.cpp:41751`). The Compare pane **is** the preview: left = the target as read, right = the
patched proposal. Dialog: `class ApplyPatchDialog : QDialog` in **`app/apply_patch.inc`**, modeless,
early return under `g_harnessRun`.

**Controls.**

| Control | What it does |
|---|---|
| **Target** path + Browse… | Pre-filled from the front document (`frontDoc`, `app/main.cpp:40360`) when it has a path |
| **Patch** path + Browse… | Filter `Patches (*.patch *.diff *.txt)`; also **Patch from clipboard** |
| **File in patch** combo | Appears only for a multi-file patch; preselects the entry whose basename matches the target; when none matches it starts **empty** and Preview stays disabled — `pickFileForTarget` returns −1 rather than guessing |
| **Fuzz** spin 0–2 (default 2) | *"how many context lines at each end may be ignored when a hunk does not fit"* |
| ☐ Reverse (unapply) | |
| ☐ Loose whitespace in context | default on — input-centre files differ in spacing; `-` lines are **never** relaxed |
| **Preview** | Parses, applies in memory, fills the hunks table and opens `g_compareTexts(target + " (current)", current, target + " (patched — 7 of 8 hunks)", patched)` |
| Hunks table | **Include** (checkbox) · **#** · **@@ header** · **Placement** · **A-lines** · **B-lines**. Placement is one of: `applied exactly at line 120` · `applied with offset +3, at line 123` · `applied with fuzz 1 (leading context ignored), at line 124` · `already applied` · `REJECTED — line 122 does not match "…"`. Double-click scrolls the Compare pane to that hunk (`goToHunk`) |
| **Re-preview with ticked hunks** | Re-runs with `onlyHunks` |
| **Save rejected hunks as .rej…** | Writes `<target>.rej` in unified form (the rejected hunks **verbatim**, with their original `@@` headers) through `saveOrWarn` |
| **Apply to file** | Enabled when ≥1 hunk applied and the target is not open with unsaved edits (otherwise disabled with the reason). Two-step confirm → `noteVersion(reason = pre-patch)` **must return Stored** → `saveOrWarn` in the target's own EOL, encoding and BOM → reload an open, clean pane → result *"Applied 7 hunks (1 with fuzz); 1 rejected (KL0032.ACT.rej written); the previous text is kept as version `<stamp>`"* |
| Compare-side apply | For Sessions ▾ targets the patched text is pushed with `pushUndo()`, so **Undo Merge** returns to the unpatched text, and Save Left/Right is the write |

**Honesty labels.**
- Every hunk's placement is named; **exact**, **offset**, **fuzz** and **already applied** are four
  distinct labels, never folded into "applied".
- **"Already applied"** is a first-class verdict (the `+` side matches at the position): reported,
  skipped, and **never counted as a failure** — re-running a corrector's patch is a normal accident and
  must not produce a `.rej` full of hunks that are in fact present.
- The right-hand name carries `(patched — k of n hunks)`, so a partial result can never be mistaken for
  a complete one; a fuzzy placement can never read as exact.
- Header: *"The patched text is a proposal computed from `<patch>` against `<target>` as read at
  `<time>`; nothing is written until you save."*
- Rejected hunks are shown in full, exported verbatim, and **never forced**.
- Parse problems list the line number and text (*"line 41: expected @@ header"*); a patch that does not
  parse produces no preview.
- *"Every hunk in the patch is applied as written; the compare rules do not filter a patch."*
- When **100 %** of hunks reject: *"No hunk matched. The patch may have been made from a different
  edition, script or encoding of this text — compare the two files instead."*
- A Manuscript target is refused: *"Patches apply to plain-text documents; the Manuscript stores rich
  text."*

**Fit-ratchet note:** no ribbon controls.

### Data

Input: GNU unified diff text. Outputs: the patched target (same EOL, encoding and BOM), an optional
`<target>.rej` whose first line is `# rejected by Diamond Cutter Apply Patch <ISO time> from <patch>`
followed by `--- old` / `+++ new` and the rejected hunks verbatim, a `pre-patch` Version, and — when run
from the Files pane over several targets — a changeset record in the F4 shape with `kind: "apply-patch"`.
`QSettings`: `patch/fuzz`, `patch/looseWs`.

**Provenance fields:** the `pre-patch` version (`savedBy`, `stamp`, `sha1`); the patch file name and its
SHA-1 in the Compare status header; per-hunk placement, offset and fuzz; the `.rej` header naming the
patch and the target it was tried against.

### Engine

**Module:** `core/include/allcore/textpatch.h` + `core/src/textpatch.cpp` (Qt-free), smoke
`core/tests/textpatch_smoke.cpp` registered in `core/CMakeLists.txt` (the `textdiff_smoke` pattern at
`:148-150`).

```cpp
namespace allcore::textpatch {

struct PatchLine { char tag; std::string text; };          // ' ', '-', '+'
struct Hunk {
    int aStart = 0, aLen = 0, bStart = 0, bLen = 0;        // 1-based, as in the header
    std::string header;
    std::vector<PatchLine> lines;
    bool noNewlineA = false, noNewlineB = false;
};
struct PatchFile {
    std::string aName, bName;
    std::vector<Hunk> hunks;
    std::vector<std::string> warnings;                     // tolerated preamble lines
};
struct ParseResult { std::vector<PatchFile> files; std::vector<std::string> errors; };
ParseResult parseUnified(const std::string& patchText);

enum class Placement { Exact, Offset, Fuzz, AlreadyApplied, Rejected };
struct Outcome {
    int hunkIx = 0; Placement how = Placement::Rejected;
    int atLine = -1, offset = 0, fuzz = 0;                 // atLine 0-based in the target
    std::string reason;
};
struct ApplyOptions {
    int maxFuzz = 2; bool reverse = false; bool looseWhitespace = true;
    std::vector<int> onlyHunks;                            // empty = all
};
struct ApplyResult {
    std::vector<std::string> lines;
    std::vector<Outcome> outcomes;
    int exact = 0, offsetApplied = 0, fuzzApplied = 0, alreadyApplied = 0, rejected = 0;
    int applied() const { return exact + offsetApplied + fuzzApplied; }
};
ApplyResult applyPatch(const std::vector<std::string>& target, const PatchFile& p,
                       const ApplyOptions& o);
std::string rejectedAsUnified(const PatchFile& p, const ApplyResult& r);
PatchFile  reversePatch(const PatchFile& p);
int pickFileForTarget(const ParseResult& pr, const std::string& targetBasename);  // -1 = no unambiguous match
}
```

**Algorithm — the documented behaviour of `patch(1)`** (Larry Wall; the GNU `patch` manual,
*"Applying Imperfect Patches"*, and the GNU Diffutils manual's *"Detailed Description of Unified
Format"*). No product code is consulted.

*Parser.* `--- name[\t timestamp]`, `+++ name[\t timestamp]`, `@@ -a[,n] +b[,m] @@[ section]`, body lines
tagged `' '`, `'-'`, `'+'`, and `\ No newline at end of file`. Tolerated as **warnings**, not errors:
`diff --git`, `index …`, `similarity …`, `Index:`, `====`, blank separators, CRLF patch text. A missing
count means 1 (`@@ -5 +5 @@`). A hunk whose body length disagrees with its header is an **error** for that
hunk — never guessed.

*Applier.* Hunks in order, carrying a running line delta from previously applied hunks. The "before"
image is the hunk's `' '` and `'-'` lines. Try the expected position (`aStart − 1 + delta`); if it does
not match, search **alternately outward** (−1, +1, −2, +2, …) over the whole file; if still unmatched and
fuzz is allowed, drop `f = 1..maxFuzz` **leading and trailing context lines** and repeat the search — a
hunk left with **no context at all** after fuzzing is **rejected** rather than matched anywhere, and
**`-` lines are never relaxed under fuzz**. If instead the hunk's `'+'` side (context + `+` lines)
matches at the position, the outcome is **`AlreadyApplied`** and nothing is changed. On a match, the
before image is replaced by the `' '` and `'+'` lines and the delta grows by `bLen − bLen`… by
`(bLen − aLen)`. A rejected hunk changes nothing and leaves the delta unchanged. `looseWhitespace`
compares lines after collapsing runs of spaces and tabs and trimming (patch's `-l`), for context lines
only. `reverse` swaps `-` and `+` (patch's `-R`). Every comparison strips a trailing `\r`, so a CRLF
target and an LF patch interoperate; the caller re-joins with the **target's** EOL.

**Reuses:** `textdiff::unifiedDiff` (`core/src/textdiff.cpp:474-507`) as the in-house writer whose exact
output the parser must round-trip; `textdiff::splitLines` / `joinLines`; `g_compareTexts` and
`ComparePane::goToHunk` / `pushUndo` / `recompute` / `saveSide`; `readForRewrite` (F4) for the target's
strict-UTF-8 read, or the Overlay's `docEncoding_` when the target is the open document;
`frontDoc` (`app/main.cpp:40360`); `docprops::noteVersion` for the `pre-patch` bank; `saveOrWarn`;
the `--compare` CLI block as the pattern for `--apply-patch`.
**`app/thirdparty` `diff_match_patch` is deliberately not used:** it reads its own patch format, throws
bare `const char*`, and is `QString`-bound.

### Acceptance tests

**CTest smoke — `textpatch_smoke` (new, fixture-free):**
1. **Round trip:** `a = L01..L20`; `b` = `a` with `L05 → "L05x"`, `L12` deleted, `"NEW"` inserted after
   `L17`; `parseUnified(unifiedDiff("a","b",a,b,diffLines(a,b,{})))` → 1 file, names `a`/`b`, no errors;
   `applyPatch(a, …)` → `lines == b`, every outcome `Exact`, `rejected == 0`.
2. **Offset:** target = `{P1,P2,P3} + a` → every outcome `Offset` with `offset == 3`; result ==
   `{P1,P2,P3} + b`.
3. **Fuzz:** target = `a` with one *leading context* line of the first hunk altered → that hunk is
   `Fuzz` with `fuzz == 1` and line 5 becomes `"L05x"`; with `maxFuzz = 0` the same hunk is `Rejected`
   and line 5 stays `"L05"`.
4. **Fuzz never relaxes a `-` line:** target = `a` with `L12` (a deleted line) changed → that hunk is
   `Rejected` at every fuzz level, with a reason quoting the target line; the other hunks apply and the
   altered `L12` survives.
5. **No context left:** a hunk with one context line at each end and `maxFuzz = 2` → `Rejected`, not
   matched at an arbitrary position.
6. **Already applied:** `applyPatch(b, p)` → every outcome `AlreadyApplied`, `lines == b`,
   `rejected == 0` (nothing double-applied, no `.rej`).
7. **Reverse:** `applyPatch(b, p, {reverse = true})` → `lines == a`.
8. **GNU tolerance:** a patch carrying `diff --git a/x b/x`, `index 1a2b..3c4d 100644`,
   `--- a/x\t2026-09-08 10:00:00`, a hunk header with section text, CRLF line endings and
   `\ No newline at end of file` → parses with `errors` empty, `warnings` non-empty, `noNewlineB` true,
   names `x`.
9. `@@ -5 +5 @@` → `aLen == 1 && bLen == 1`.
10. `@@ garbage` → `errors == {"line 3: expected @@ header …"}` and no hunks; a header whose counts
    disagree with the body yields a per-hunk error.
11. **Multi-file:** two `---`/`+++` sections → `files.size() == 2`; `pickFileForTarget(pr, "b") == 1`;
    `pickFileForTarget(pr, "zzz") == -1`.
12. **onlyHunks:** `{0}` applies only the first hunk; the others are `Rejected` with reason
    `"not selected"` and are **excluded from `rejectedAsUnified`**.
13. **Loose whitespace:** a context line with two trailing spaces → `Exact` with
    `looseWhitespace = true`; `Fuzz` or `Rejected` with false.
14. `rejectedAsUnified` contains only the rejected hunks with their original `@@` headers, byte-for-byte.
15. Multi-hunk cumulative delta: a first hunk inserting 2 lines leaves the second landing `Exact` at its
    adjusted position.

**app `--selftest`:**
16. Headless with a temp **CRLF + BOM** target and an LF patch: preview reports 3 applied, 1 rejected;
    Apply with `harnessAutoConfirm_` → the target's bytes still start with the BOM, contain only
    `\r\n`, `<target>.rej` exists with exactly one `@@` header, and a `pre-patch` version holds the
    previous bytes.
17. With the target open and dirty in the Overlay, **Apply to file is disabled** and the reason mentions
    *"unsaved edits"*.
18. A patch where every hunk rejects → the status carries the *"compare the two files instead"* hint.
19. A Manuscript target → refused with the stated message; nothing written.
20. Compare-side apply: `pushUndo` snapshot exists and `undo()` restores the unpatched right side.
21. A failed `pre-patch` bank abandons the write (nothing on disk changes).

**CLI:**
22. `--apply-patch t.act p.diff --dry-run` prints the per-hunk outcome list and writes nothing;
    exit **0** when all hunks applied or were already applied, **1** when any rejected, **2** when
    unreadable.

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| `patch(1)` corner cases (overlapping hunks after offsets, empty context) | The smoke pins the documented behaviour; ambiguous placements are rejected rather than chosen |
| Repetitive liturgical text (identical refrains) can accept a fuzzy placement far from the intended spot | `-` lines still match verbatim; the placement is labelled with its offset and fuzz; the preview diff is where a human catches it, and **nothing auto-writes** — Save is manual |
| The in-house writer never emits `\ No newline at end of file` and drops a trailing empty line | Parser flag plus the target-EOL re-join with the trailing break restored, as `saveSide` does |
| Offset search is `O(hunks × lines)` worst case | Bounded by the 10 MB text rule; one pass per preview; no threading needed |
| A `.rej` beside a library text adds a file the census will list | The result names the file so the user can remove it |

### Effort

**15 hours.**

### Dependencies

F0; F1 (`pre-patch` bank); F4's `readForRewrite` (strict UTF-8) — or, if F5 lands first, that helper
moves to F0.

---

## F6 — Tracked-changes .docx export

### Purpose and user story

**Research verdict:** §3(a) BUILD, batch 4 — write the differences between two drafts as a `.docx` with
Word revision marks so Geshe Michael can review in Word.

> **The research doc's premise is false and this specification corrects it.** §3(a) says *"the Manuscript
> already exports .docx; add `w:ins`/`w:del` runs from the hunks."* **Verified at HEAD:** there is no
> OOXML writer anywhere in `app/` or `core/`; the Manuscript exports **RTF** through
> `/usr/bin/textutil` (macOS-only), and `docs/CLOSER.md` #49 scopes a native OOXML writer as backlog.
> The writer is therefore **built here**, and the correction is recorded in the ComparePane audit row.

**User story.** As a translator sending draft 4 to Geshe Michael, I want a Word document in which
everything I changed since draft 3 appears as real Word revision marks — insertions underlined,
deletions struck through — so he can accept or reject each change in Word itself, with a first paragraph
that says exactly what the file compares, under what rules, who ran it, and what the marks are and are
not.

### UI

**Placement and the release gate.**
- **CLI first:** `--compare A B out.docx` (`app/main.cpp:41751`) and the Versions window's
  *Send Changes to Word…* are the writer's first consumers.
- **The Save Report… filter `Word, tracked changes (*.docx)` is NOT added until the Word verification is
  recorded** in `docs/release_audit/ComparePane.md` (Adam opens a sample in Microsoft Word, the version
  is noted, Accept All yields text B, Reject All yields text A). Until then there is **no disabled
  placeholder** in the dialog — a control that cannot be used is a dead control. The interim path is the
  CLI, which is where the sample is produced anyway.
- **Versions ▸ Send Changes to Word…** is likewise **not created** until the gate passes.
- After the gate: the filter is added to `ComparePane::saveReport`
  (`app/compare_pane.inc:388`), `report()` gains a `QByteArray reportBytes(kind)` twin for binary kinds,
  and **Tools ▸ Compare ▸ Send Comparison to Word…** appears for menu discoverability.

**Controls (post-gate).** The Save Report… filter entry; the Versions button; the Tools menu item; a
status line after saving: *"Word file saved: `<path>` — 42 insertions, 17 deletions marked, 3 minor
differences included. Open it in Word ▸ Review to accept or reject."*

**Honesty labels.**
- **First paragraph of the document, plain and untracked:**
  *"Revision marks computed by the Diamond Cutter Translation Tool from A = `<name>` and B = `<name>` on
  `<date>`. Rules in force: `<list, or "none">`. `<summary(r)>`. Minor differences under those rules are
  included as revisions (N of them) — a redline is exact. K line(s) were compared raw (could not be
  converted). Where lines did not pair one to one, whole lines are marked. Moved blocks appear as a
  deletion and an insertion. Both drafts are compared as plain text; formatting was not compared. Export
  run by `<name>`. These marks are machine-computed; they are not an editor's edits."*
- **Author string:** `"<Team name> via Diamond Cutter compare"` (or `"<login> via …"`). Word shows the
  author on every balloon; a bare person's name would present machine hunks as that person's edits.
  `docProps/core.xml` `dc:creator` carries the same string.
- **Minor-difference policy is one policy, not three:** minor differences under the rules in force are
  **always included** as revisions, and their count is printed in the note. The pane's
  "Minor = differences" checkbox does **not** filter the export, and the note says so — a Word reader
  must never receive a redline that quietly omits differences.
- The document contains only the two texts' lines and the note. **No gloss or dictionary material is
  ever written** (rule 1 untouched).
- Tibetan is exported as it stands in the sources; nothing is converted.

**Fit-ratchet note:** no ribbon controls.

### Data

**OPC/OOXML package** (ECMA-376 Part 1 / ISO-IEC 29500, WordprocessingML), a ZIP with **STORED** entries
only:

| Part | Contents |
|---|---|
| `[Content_Types].xml` | Defaults `rels`, `xml`; Overrides for `/word/document.xml`, `/word/styles.xml`, `/word/settings.xml`, `/docProps/core.xml`, `/docProps/app.xml` |
| `_rels/.rels` | officeDocument → `word/document.xml`; core-properties → `docProps/core.xml`; extended → `docProps/app.xml` |
| `word/_rels/document.xml.rels` | styles, settings |
| `word/document.xml` | `w:body`: one `w:p` per line, then `w:sectPr` (Letter page, 1-inch margins) |
| `word/styles.xml` | `Normal` (Times New Roman 12) and a `Tibetan` character style |
| `word/settings.xml` | `<w:trackRevisions/>` so further edits in Word are tracked too |
| `docProps/core.xml` | `dc:creator`, `dcterms:created`, `dc:description` = the provenance line |
| `docProps/app.xml` | `Application` = Diamond Cutter Translation Tool |

Runs: plain `w:r`/`w:t`;
`<w:ins w:id w:author w:date><w:r><w:t xml:space="preserve">…</w:t></w:r></w:ins>`;
`<w:del …><w:r><w:delText xml:space="preserve">…</w:delText></w:r></w:del>`. Inserted and deleted
**paragraph marks** via `<w:pPr><w:rPr><w:ins/>|<w:del/></w:rPr></w:pPr>` (ECMA-376 §17.13.5.15/.18), so
Accept All merges paragraphs correctly.

**Tibetan runs:** any run containing U+0F00–U+0FFF gets
`<w:rPr><w:rFonts w:cs="Noto Serif Tibetan"/><w:cs/></w:rPr>` — the complex-script slot is what Word
actually consults for Tibetan shaping. ASCII (ACIP/Wylie) runs get no font override. The font name is
stated in the provenance paragraph.

ZIP per PKWARE APPNOTE 6.3: local file headers (`0x04034b50`), central directory (`0x02014b50`), EOCD
(`0x06054b50`); method **0** (stored), CRC-32 (reflected polynomial `0xEDB88320`, ISO 3309 / ITU-T V.42),
no data descriptors, no ZIP64. DOS timestamps are fixed so output is **deterministic** and the smoke can
assert byte-identical results on two calls.

**Location:** a user-chosen path via `saveOrWarn`, or the CLI's third argument. Nothing persisted.

**Provenance fields:** the note paragraph (A/B names, date, rules, counts of marked minor and
compared-raw lines, moved-block statement, runner, font); `w:author` and `w:date` on every revision;
`dc:creator`; `dcterms:created`; `dc:description`.

### Engine

**Module:** two headers, so the container is reusable —
`core/include/allcore/zipstore.h` + `core/src/zipstore.cpp` (CRC-32 and the STORED-zip writer/reader;
**no zlib**, which matters because `allcore` links `z` only under `USE_SYSTEM_SQLITE=ON`,
`core/CMakeLists.txt:77-88`), and `core/include/allcore/docx_redline.h` + `core/src/docx_redline.cpp`
(the WordprocessingML writer consuming `textdiff::Result`). Smokes:
`core/tests/zipstore_smoke.cpp` and `core/tests/docx_redline_smoke.cpp`.
`zipListStored` is deliberately public: it is the first brick of the §3(b) `.docx` *reader*, not a
throwaway.

```cpp
namespace allcore::zipstore {
std::uint32_t crc32(const std::string& bytes);
struct Part { std::string name, bytes; };
std::string writeStored(const std::vector<Part>& parts);   // deterministic DOS timestamps
struct EntryView { std::string name; std::size_t dataOffset; std::uint32_t size, crc; };
std::vector<EntryView> listStored(const std::string& archive);   // parses the central directory
}

namespace allcore::docx {
struct RedlineOptions {
    std::string author, dateIso, aName, bName, provenance, title;
    std::string tibetanFont = "Noto Serif Tibetan";
    bool inlineRefine = true;
};
struct RedlineStats {
    int insertions = 0, deletions = 0, minorMarked = 0,
        comparedRaw = 0, moved = 0, unrepresentable = 0;
};
std::string xmlEscape(const std::string& s);
std::string documentXml(const std::vector<std::string>& a, const std::vector<std::string>& b,
                        const textdiff::Result& r, const RedlineOptions& o, RedlineStats* st);
std::string stylesXml(const RedlineOptions&); std::string settingsXml();
std::string coreXml(const RedlineOptions&);   std::string appXml();
std::string contentTypesXml();                std::string relsXml();
std::string documentRelsXml();
std::string buildRedlineDocx(const std::vector<std::string>& a, const std::vector<std::string>& b,
                             const textdiff::Result& r, const RedlineOptions& o,
                             RedlineStats* st);
}
```

**Algorithm.** The hunks cover both inputs in order (`textdiff.h:46`), which is exactly the run stream
needed.

- **Equal** → one plain paragraph per left line.
- **Delete** → one paragraph per left line, a single `w:del` run, paragraph mark deleted.
- **Insert** → one paragraph per right line, a `w:ins` run, paragraph mark inserted.
- **Change with equal line counts** → per line pair, `textdiff::diffInline(a, b, Mode::Auto)` —
  **syllables for Tibetan, words for English** — equal spans as plain runs, unequal spans as
  `w:del(left piece)` then `w:ins(right piece)` inside one paragraph, so Word renders struck-through then
  underlined text in place.
- **Change with unequal counts** → all left lines as deleted paragraphs, then all right lines as
  inserted paragraphs (the `apparatus()` block policy, `core/src/textdiff.cpp:604-608`); no inline
  refinement.
- **`unimportant` hunks** are marked like any other and counted in `minorMarked`.
- **`movedPeer`** hunks are exported as deletion + insertion (Word's `w:moveFrom`/`w:moveTo` omitted) and
  counted.
- `rawNormalised == false` lines are counted for the note.
- `w:id` runs sequentially from 1; `w:date` is the ISO date with `Z`.
- XML escaping of `&`, `<`, `>`, `"`; control characters other than tab are replaced by U+FFFD and
  **counted** in `unrepresentable`, which the note reports — never dropped silently.

**Reuses:** `textdiff::Result` / `Hunk` / `Kind` / `diffInline` / `summary`; `cmp::optionsJson` and the
RULES checkbox labels for the rules string; `ComparePane::report` / `saveReport` routing;
`g_userName` and `docprops::noteSave`'s who-rule; the `--compare` CLI; the Versions decode path.
`tibexport`'s `translationPrepToRtf` is the precedent for a core document writer but is **not** shared
code and is left untouched.

### Acceptance tests

**CTest smoke — `zipstore_smoke` (new):**
1. `crc32("123456789") == 0xCBF43926` (the published check value); `crc32("") == 0`.
2. `writeStored({{"a.txt","AB"},{"b/c.xml","<x/>"}})` starts with `PK\x03\x04`; `listStored` returns
   2 entries whose names, sizes and CRCs match, whose local-header offsets point at `PK\x03\x04`
   records, and whose method fields are 0; the EOCD entry count is 2.
3. Determinism: two calls on the same input produce **byte-identical** archives.
4. `tools/test_zipstore.sh` (beside `tools/test_shortwrite.sh`) runs `unzip -t` on a written archive and
   expects *"No errors detected"*; when `unzip` is absent it prints an honest `[SKIP]` and exits 0.

**CTest smoke — `docx_redline_smoke` (new):**
5. With `a = {"@001A","BLA MA LA","SEMS CAN THAMS CAD"}`,
   `b = {"@001A","BLA MA LA","SEMS CAN KUN","BDE BA"}` and author `"Adam via Diamond Cutter compare"`,
   date `"2026-09-08T12:00:00Z"`: `documentXml` contains
   `<w:delText xml:space="preserve">THAMS CAD</w:delText>` inside a `<w:del` carrying that author and
   date, `<w:t xml:space="preserve">KUN</w:t>` inside a `<w:ins`, and a paragraph for `BDE BA` whose
   `w:pPr/w:rPr` contains `<w:ins `; `"BLA MA LA"` appears once in a plain `w:t` and never inside
   `w:ins`/`w:del`; `stats.insertions == 2`, `deletions == 1`.
6. Every `<w:ins ` and `<w:del ` carries `w:author` and `w:date`; all `w:id` values are unique and start
   at 1.
7. Tag balance: a stack check over all `<w:…>` open/close tags in `documentXml` passes;
   `count("<w:p ") + count("<w:p>") == count("</w:p>")`.
8. `xmlEscape("a<b&c>\"d\"")` == `"a&lt;b&amp;c&gt;&quot;d&quot;"`; a line holding `&` round-trips
   escaped; a U+0001 becomes U+FFFD and `stats.unrepresentable == 1`, which the note reports.
9. Unequal Change (2 left lines vs 3 right lines) → 2 deleted paragraphs then 3 inserted paragraphs,
   with no inline spans.
10. A minor hunk (`Options.ignoreWhitespaceChange`, spacing-only difference) is **marked** like any
    other and `stats.minorMarked == 1`; the note paragraph contains
    `"minor difference"` with that count.
11. `r.unnormalised == 2` → the note contains `"compared raw"`; a `movedPeer` pair → `stats.moved == 1`
    and the note's moved-block sentence is present.
12. **Tibetan run:** a line containing `བཀྲ` produces a run whose `w:rPr` holds
    `w:cs="Noto Serif Tibetan"` and `<w:cs/>`; an ACIP line's run has no `rFonts` override.
13. The **first** `<w:p>` is the provenance paragraph and contains the A and B names, the author, and
    the phrase `machine-computed`.
14. `contentTypesXml` declares Overrides for `/word/document.xml` and `/word/styles.xml`; `relsXml`
    targets `word/document.xml`; `settingsXml` contains `<w:trackRevisions/>`; `coreXml`'s `dc:creator`
    equals the author.
15. `buildRedlineDocx` output's `listStored` names are exactly the eight parts listed above.
16. macOS only, `[SKIP]` elsewhere: write the archive to a temp `.docx`; `textutil -convert txt` exits 0
    and its output contains `BLA MA LA` and `BDE BA`.

**app `--selftest` (post-gate):**
17. After `compareTexts` on the fixture, `reportBytes("docx")` starts with `"PK"` and is ≥ 2 KB;
    `saveReport` through the harness stub with a `.docx` name writes those bytes and the status contains
    `insertions`.

**CLI:**
18. `--compare A.act B.act out.docx` exits 1 and `out.docx` starts with `PK`.

**Manual release gate (recorded in `docs/release_audit/ComparePane.md` before the filter ships):**
19. Adam opens the sample in Microsoft Word (version noted): Review shows the insertions and deletions
    attributed to `"<name> via Diamond Cutter compare"`; **Accept All** yields text B exactly;
    **Reject All** yields text A exactly.

### Risks and mitigations

| Risk | Mitigation |
|---|---|
| Word is stricter than any test here; a namespace or element-order slip shows as "unreadable content" | Structural checks in the smoke (order, balance, `xml:space`), `unzip -t` for the container, `textutil` on macOS as a second reader — and **the filter does not ship until Adam's Word check is recorded** (test 19) |
| `Noto Serif Tibetan` may be absent on the reviewer's machine | Word substitutes; the `w:cs` slot makes substitution work for complex script; the note names the font |
| STORED archives are ≈ 1:1 with the text | Acceptable for review documents; noted in the audit row; adding deflate would need a raw zlib stream, **not** Qt's `qCompress` container — the two must never be conflated |
| Moved blocks lose Word's move semantics | Exported as deletion + insertion; declared in the note; `w:moveFrom`/`w:moveTo` filed as a refinement |
| Manuscript sides are the plain-text projection | The note says "compared as plain text; formatting was not compared" |
| A fourth formatted-export path now exists beside two RTF writers and `textutil` | The writer lives in `allcore` so the Manuscript/prep export can adopt it later (`docs/CLOSER.md` #49); open question 6 asks whether to do that in this batch |

### Effort

**16 hours.**

### Dependencies

`allcore::textdiff` (exists). F1 only for the *Send Changes to Word…* button, which ships after the gate.

---

## Sequencing

| # | Step | Hours | Gate before moving on |
|---|---|---|---|
| 0 | **F0 substrate and gates.** Widen the R3 census to `app/main.cpp` + `app/*.inc` with the deliberate-regression check; fix `DraftPane::dataRoot_`; unify the three folio definitions; promote `eligibleTextFile` into `allcore/filewalk.h` and switch `gofer.cpp` / `libindex.cpp`; add `textdiff::applySelected`; add the `selectedFilter` out-parameter; fix the Manuscript Compare-with-Saved-Version mismatch; extend `renameFileTo` with directory pairs. | 7 | `tools/verify.sh` green; baseline **154** unchanged; `gofer_smoke`, `libindex_smoke`, `textdiff_smoke`, new `filewalk_smoke` green |
| 1 | **F1 Versions.** Core `versions.h/.cpp` + `versions_smoke` and `lineOrigins` first, green before any UI. Then `docprops::noteVersion` at the three save sites (`:8334`, `:22069`, `:33548`), the Manuscript `save(bool autosave)` flag, rename carry at the three callers, the Preferences page, `app/versions_pane.inc` with File ▸ Versions…, Tools ▸ Compare ▸ Compare with Version…, the Properties row and button. | 20 | 32 selftest checks; every downstream feature's undo now exists |
| 2 | **F2 Changed folios.** The cheapest visible win and the first consumer of the unified regex; it also lands the report-filter routing F6 will reuse. | 5 | `textdiff_smoke` additions + 5 selftest checks |
| 3 | **F3 Normalize.** `textnorm.h/.cpp` + `textnorm_smoke` (incl. idempotence and tsheg protection), then `app/normalize_pane.inc`, the Format item, the Files context item, the Compare Sessions ▾ variants. | 11 | 16 smoke + 8 selftest checks; the shad controls ship with no default |
| 4 | **F4 Replace in Files.** The largest dialog, built after the preview/confirm/undo idioms have settled, and before Apply Patch because it is roadmap item A. Includes the changeset store, Normalize mode, the Find-menu Undo entry, the Files context item, the Search-tab button. | 22 | 24 selftest checks incl. changed-since-preview, changed-since-run, bank-failure and library-guard drills; a gauntlet run proving Apply is inert without a preview |
| 5 | **F5 Apply Patch.** `textpatch.h/.cpp` + `textpatch_smoke` (round trip, offset, fuzz, reject, already-applied, GNU tolerance, reverse), then the dialog, the Sessions ▾ entries, the `.rej` export, the `--apply-patch` CLI. | 15 | 15 smoke + 7 selftest checks |
| 6 | **F6 Tracked-changes .docx.** `zipstore` + `zipstore_smoke` (+ the `unzip -t` script), then `docx_redline` + its smoke, then the CLI `.docx` route. **Hand Adam a sample; record the Word verification in `docs/release_audit/ComparePane.md`; only then add the Save Report… filter, the Tools item and the Versions button.** | 16 | 16 smoke checks, then the manual Word gate |
| 7 | **Close-out.** Selftest counts added to the registration block (`app/main.cpp:41809+`); the ribbon `[info]` line verified unchanged (`kRibbonRatchet = 2572`, `app/main.cpp:43708`); heuristic + accessibility pass on the four new windows **before Adam sees them**; audit rows (`docs/release_audit/ComparePane.md` extended, new `VersionsPane.md` and `ReplaceInFiles.md`); §7 "Mark / Bookmark all matches" filed in `TODO.md`; `docs/digests/LOG.md` lines; TODO batch-4 boxes; digest draft; commit. | 4 | `tools/verify.sh`, `ctest -LE fixture`, `app --selftest`, constitution check, gauntlet |

**Total ≈ 100 hours.**

---

## Deliberately omitted

| Item | Why |
|---|---|
| **Branching a document** (Git-style branches) | §5 SKIP: a translator does not branch a text; dossiers already give one desk per text. Pinned milestone versions cover *"the draft I sent to Geshe Michael"*. |
| **`.docx` / `.pdf` text-extraction compare** | §3(b) LATER — a reader is needed. `zipstore::listStored` from F6 is the first brick; the reader itself is not built here. |
| **Scan (image) compare with a blend/flicker slider** | §3(c) LATER; the Scans pane owns the viewer. |
| **Three-way folder merge; archive-contents compare** | §4 LATER (no workflow; compare needs extraction). |
| **Mirror-with-delete folder sync; FTP** | §4 SKIP: nothing in this app deletes; there is no FTP. |
| **Live Track Changes inside the Manuscript editor** | `docs/MENU_PARITY_PLAN.md:113` defers it past 1.0; §12 explicitly routes change tracking for texts through Versions + Compare, and the shipped UI now says so in words. |
| **Word "Combine" (multi-author merge into one tracked document)** | The Three-Way Merge page already covers base + two editors with explicit conflicts; a tracked Combine would present machine merges as edits. |
| **`w:moveFrom` / `w:moveTo` for moved blocks** | Pairing bookkeeping for little review value; moved blocks export as deletion + insertion and the note says so. Filed as a refinement. |
| **Table compare keyed by a column** (glossary / release-to-release) | §14 and `docs/COMPARE_SUITE_PLAN.md` place it in batch 5. |
| **"Mark" / Bookmark all matches in the Find bar** | §7 rates it **BUILD**, but it is a Find-bar / NumberedEdit gutter change, not one of this cluster's six features. **Filed explicitly in `TODO.md` with the §7 citation** so the verdict is not quietly lost. |
| **Replace in all open documents** | Only three editors exist and each already has Replace All in one undo step; the multi-file case is Replace in Files. Cut under the steelman rule. |
| **Multi-line (cross-line) regex in Replace in Files** | The mandatory preview is a tickable-occurrence model; a pattern spanning lines cannot be represented in it. Refused explicitly rather than half-supported. |
| **Normalize for the Manuscript (rich text)** | A plain-text normalization would destroy formatting; refused with a message. Its spacing conventions are already served by Format ▸ Apply House Style Spacing. |
| **Script conversion inside Normalize** | The Convert pane and Format ▸ Tibetan: To … own conversion through the canonical converters; a second path would violate rule 2. |
| **Tibetan-Unicode canonical / stacking normalization inside Normalize** | `allcore::normalizeTibetanUnicode` is a **search** normaliser (`core/include/allcore/searchnorm.h`) whose lenient map respells `པདྨ → པད་མ`; repurposing it as a display-text transformation could silently respell a volume. Even the strict map's lossless-ness for display text has not been verified line by line. Filed as its own item, verification first. |
| **Encoding-aware (non-UTF-8) rewriting in Replace in Files and Normalize** | Rule 3: non-UTF-8 files are skipped and **named**, with the Overlay's explicit Reopen/Save with Encoding as the honest conversion path. Filed with the Compare pane's existing encoding open item. |
| **Automatic search-index refresh after a batch** | `LibraryIndex::update` runs on the GUI thread with a progress dialog; running it silently after every batch would hide cost. The result panel discloses staleness and links to the existing button; the opt-in checkbox is there for those who want it. |
| **A per-line blame gutter in the editors** | Line Origins as a table delivers §5's question without a new editor decoration; a gutter can follow once the algorithm has been used in practice. |
| **Compare with Backup for the three shared stores** | Those stay on `allcore/backup.h`'s launch-time rolling backups and the Help-menu restore. Document Versions are what "Compare with Backup" meant for translator texts. |
| **A deflate implementation inside `allcore`** | `allcore` links `z` only under `USE_SYSTEM_SQLITE=ON`. Version blobs are compressed in the app layer with `qCompress` (a `codec` column keeps the core store codec-agnostic and battery-testable); the `.docx` container is STORED and needs only CRC-32. The two must not be conflated: Word needs raw deflate streams, not Qt's container. |

---

## Open questions for Adam

1. **The house shad conventions.** Normalize needs the publishing team's ruling: is the double shad
   written `,,` or `, ,` in ACIP (and `།།` or `༎` in Unicode)? Does a shad follow its syllable with no
   space (`GA,` / `ཀ།`) or one space (`GA ,` / `ཀ །`)? Until you or the team rule, **both controls ship
   unchecked** with the label *"awaiting the house ruling"*, and no preset ships. Are there other
   input-centre conventions Normalize should carry (line breaks at folio markers, for instance)?
2. **The Microsoft Word verification.** The `.docx` writer is proven against the OOXML and ZIP
   specifications and, on macOS, by `textutil` — neither is Word. Would you open the sample in Microsoft
   Word (and tell me the version)? Review should show the marks attributed to
   *"`<name>` via Diamond Cutter compare"*, Accept All should give text B, Reject All text A. **The
   Save Report… filter, the Tools item and the Versions button do not ship until that check is recorded
   in the audit row.**
3. **Where document versions live when the data root is a shared Dropbox folder.** This design puts them
   under `<dataRoot>/library/versions/` (as §5 proposes), so histories travel with the texts and every
   team member sees them — at the cost of sync traffic. The alternative is each user's private
   Application Support folder: no sync, no shared histories, lost on a machine change. It is a one-line
   switch either way; the defaults (200 versions / 64 MB per document, oldest pruned, compressed) are
   yours to adjust once the location is settled.
4. **Replace in Files over canonical texts under `/library/`.** As designed they are listed with a badge,
   unticked, and gated behind a default-off switch plus a confirmation that names how many are involved.
   Is that enough, or should bulk rewriting of `/library/` be **refused outright**, so only intake and
   input-centre folders can be rewritten in bulk?
5. **The tracked-changes author string.** Proposed: *"`<Team name>` via Diamond Cutter compare"* on every
   mark, plus the untracked provenance paragraph. The alternative is a fixed non-personal author
   *"Diamond Cutter compare"* with the runner named only in the paragraph. Word shows the author on every
   balloon, so this is what Geshe Michael reads on every change.
6. **Should the new OOXML writer also give the Manuscript a plain `.docx` export?** (`docs/CLOSER.md`
   #49; the research doc assumed it already existed.) Roughly 4 hours on top of F6, and it would retire
   the macOS-only `textutil` RTF path for Word delivery. In or out of this batch?
7. **Manuscript autosave versions.** Proposed: a single rolling autosave slot per document — each
   autosave replaces the previous one, any deliberate save clears it, the row is hidden by default. That
   keeps one row instead of sixty an hour but means intermediate states within a session are not kept.
   Confirm, or would you rather autosaves minted no version at all?

## Open questions for Adam (this batch)

- The house shad conventions: is the double shad written ',,' or ', ,' in ACIP (and '།།' or '༎' in Unicode), and does a shad follow its syllable with no space ('GA,' / 'ཀ།') or one space ('GA ,' / 'ཀ །')? Until the publishing team rules, both Normalize controls ship unchecked with the label 'awaiting the house ruling' and no preset ships. Are there other input-centre conventions Normalize should carry?
- The Microsoft Word verification: the .docx writer is proven against the OOXML and ZIP specifications and, on macOS, by textutil — neither is Word. Would you open the sample in Word (and tell me the version)? Review should show the marks attributed to '<name> via Diamond Cutter compare', Accept All should give text B, Reject All text A. The Save Report… filter, the Tools item and the Versions button do not ship until that check is recorded in the audit row.
- Where document versions live when the data root is a shared Dropbox folder: under <dataRoot>/library/versions/ as §5 proposes (histories travel with the texts, every team member sees them, sync traffic) or in each user's private Application Support folder (no sync, no shared histories, lost on a machine change)? One-line switch; the defaults (200 versions / 64 MB per document, compressed, oldest pruned) are yours to adjust once the location is settled.
- Replace in Files over canonical texts under /library/: as designed they are listed with a badge, unticked, and gated behind a default-off switch plus a confirmation naming how many are involved. Is that enough, or should bulk rewriting of /library/ be refused outright so only intake and input-centre folders can be rewritten in bulk?
- The tracked-changes author string: proposed '<Team name> via Diamond Cutter compare' on every mark plus the untracked provenance paragraph, versus a fixed non-personal author 'Diamond Cutter compare' with the runner named only in the paragraph. Word shows the author on every balloon, so this is what Geshe Michael reads on every change.
- Should the new OOXML writer also give the Manuscript a plain .docx export (CLOSER #49; the research doc assumed it already existed)? Roughly 4 hours on top of F6, and it would retire the macOS-only textutil RTF path for Word delivery. In or out of this batch?
- Manuscript autosave versions: proposed is a single rolling autosave slot per document — each autosave replaces the previous one, any deliberate save clears it, the row is hidden by default. One row instead of sixty an hour, but intermediate states within a session are not kept. Confirm, or should autosaves mint no version at all?

## Files this batch creates or touches

| File | Kind | Contents |
|---|---|---|
| `core/include/allcore/filewalk.h` | core-header | allcore::eligibleTextFile + WalkStats + listEligibleFiles — the one shared 'eligible text file' rule (promoted from gofer.cpp:88-93 and libindex.cpp:38-46) with include/exclude globs and capped/skipped files COUNTED, never dropped. |
| `core/src/filewalk.cpp` | core-src | Recursive walk with skip_permission_denied, in-house fnmatch-style globbing (* ? [..] and '!' negation), excludes beating includes, file cap counted into WalkStats.capped. |
| `core/tests/filewalk_smoke.cpp` | core-test | Four checks: extension/size/exclude filtering with exact WalkStats, cap counted not dropped, '!' negation, eligibleTextFile boundaries. |
| `core/include/allcore/versions.h` | core-header | allcore::versions Meta/Entry/Limits/PutOutcome, docKeyFor (base name in-root, base+path-SHA1 outside), put/list/readBlob/prune/renameKey/byChangeset/sha1Hex, flat-JSON serializeMeta/parseMeta, and the changeset reader/writer. |
| `core/src/versions.cpp` | core-src | Blob-then-record temp+rename store, SHA-1 dedupe against the newest non-autosave entry, rolling autosave slot, prune (never newest, never pinned, autosave then pre-* then save), orphan and malformed-record reporting, RFC 3174 SHA-1. |
| `core/tests/versions_smoke.cpp` | core-test | Seventeen checks: store/dedupe/autosave-slot/prune caps/pinned-and-newest-never/path filter/FIPS SHA-1 vectors/docKey collision/orphan/damaged blob/malformed record/tab-and-newline label round-trip/changeset round-trip. |
| `core/include/allcore/textnorm.h` | core-header | allcore::textnorm Options/Refusal/Result, LineClass{Acip,TibetanUnicode,Other,Mixed}, classifyLine, bracketsBalanced, detectEol, normalizeLineText, normalizeText. |
| `core/src/textnorm.cpp` | core-src | Fixed idempotent rule order (classify → apparatus-if-balanced → folio markers → shad rules → collapse → trim), shad handling as a codepoint walk (std::regex has no lookbehind) over ',' ',,' U+0F0D U+0F0E U+0F11 U+0F14, tsheg U+0F0B protected, Mixed lines refused byte-identically, never joins or splits a line. |
| `core/tests/textnorm_smoke.cpp` | core-test | Sixteen checks incl. atomic double shad, English commas untouched, mixed-script refusal, unbalanced-bracket refusal, tsheg protection, EOL modes, idempotence, and the line-count invariant. |
| `core/include/allcore/textpatch.h` | core-header | allcore::textpatch PatchLine/Hunk/PatchFile/ParseResult/Placement{Exact,Offset,Fuzz,AlreadyApplied,Rejected}/Outcome/ApplyOptions/ApplyResult, parseUnified, applyPatch, rejectedAsUnified, reversePatch, pickFileForTarget. |
| `core/src/textpatch.cpp` | core-src | GNU unified-format parser (tolerates diff --git/index/timestamps/CRLF/no-newline marker as warnings; count mismatch is an error) and the patch(1) placement rule: exact → alternating outward offset search → fuzz 1..N dropping leading/trailing context only, '-' lines never relaxed, no-context-left rejected, '+'-side match reported as AlreadyApplied. |
| `core/tests/textpatch_smoke.cpp` | core-test | Fifteen checks: round trip against unifiedDiff, offset, fuzz, fuzz-never-relaxes-a-minus-line, no-context-left rejection, already-applied, reverse, GNU tolerance, malformed headers, multi-file pickFileForTarget returning -1, onlyHunks, loose whitespace, verbatim .rej, cumulative delta. |
| `core/include/allcore/zipstore.h` | core-header | allcore::zipstore crc32, Part, writeStored (STORED/method 0, deterministic DOS timestamps, no zlib) and listStored parsing the central directory — also the first brick of a future .docx reader. |
| `core/src/zipstore.cpp` | core-src | Table-driven CRC-32 (0xEDB88320) plus PKWARE APPNOTE 6.3 local headers, central directory and EOCD; no data descriptors, no ZIP64. |
| `core/tests/zipstore_smoke.cpp` | core-test | CRC-32 published check value 0xCBF43926, two-entry archive parsed back by listStored with matching names/sizes/CRCs/offsets/method 0, and byte-identical determinism on two calls. |
| `core/include/allcore/docx_redline.h` | core-header | allcore::docx RedlineOptions/RedlineStats, xmlEscape, documentXml/stylesXml/settingsXml/coreXml/appXml/contentTypesXml/relsXml/documentRelsXml, buildRedlineDocx over a textdiff::Result. |
| `core/src/docx_redline.cpp` | core-src | Hunks → WordprocessingML: Equal plain, Delete/Insert whole paragraphs with deleted/inserted paragraph marks, 1:1 Change refined by diffInline into del+ins runs, unequal Change as whole-line blocks; minor hunks always marked and counted; Tibetan runs get w:rFonts w:cs + <w:cs/>; settings.xml carries <w:trackRevisions/>; control chars → U+FFFD and counted. |
| `core/tests/docx_redline_smoke.cpp` | core-test | Twelve checks: del/ins run contents with author and date, unique w:id, tag balance, XML escaping and unrepresentable count, unequal-Change block policy, minor marked and counted, Tibetan w:cs run, provenance first paragraph, part names, and the macOS-only textutil read with an honest [SKIP]. |
| `app/versions_pane.inc` | app-inc | docprops::noteVersion (qCompress → codec 'qz', SHA-1, who/whoSource, revision read back after noteSave), restoreVersionWithSafety (bank pre-restore → verify SHA-1 → saveOrWarn → set pendingEncoding_ → revert-path reload), and VersionsWindow with the table, Compare/Compare Two/Restore two-step/Save Version As…/Name this Version…/Line Origins/Batches filter and the §12 Accept-Reject sentence. |
| `app/normalize_pane.inc` | app-inc | NormalizeDialog: target hooks for editor/selection/file/Compare side, the rule panel with no default for the shad forms, Preview in Compare via g_compareTexts, the refused-lines list, and Apply as one QTextCursor edit block (editor) or pre-normalize bank + saveOrWarn preserving EOL/BOM (file). |
| `app/replace_files.inc` | app-inc | namespace replf: readForRewrite (strict UTF-8 via enc::decode bad-count, BOM, EOL), planOccurrences (QRegularExpression + editops helpers), applyTicked, bytesForWrite, buildPlan through allcore::listEligibleFiles, two-phase applyPlan (re-hash, open-dirty and library refusals, bank-must-succeed, saveOrWarn) and undoChangeset; plus ReplaceFilesWindow with the occurrence tree, script-mismatch notice and result panel. |
| `app/apply_patch.inc` | app-inc | ApplyPatchDialog: target/patch pickers, multi-file combo that refuses to guess, fuzz and reverse controls, the hunks table naming exact/offset/fuzz/already-applied/rejected, Compare-pane preview, .rej export, and Apply to file via a pre-patch bank. |
| `docs/ANALYSIS_SUITE_PLAN.md` | docs | This specification: the seven features with UI, data, engine, acceptance tests, risks, effort and dependencies, plus sequencing, deliberately-omitted and open questions. |
| `docs/release_audit/VersionsPane.md` | docs | Release audit for Versions in the ComparePane.md house format: purpose and how it is reached, the user-visible function table with code anchors and coverage, honesty and provenance, open items. |
| `docs/release_audit/ReplaceInFiles.md` | docs | Release audit for Replace in Files, Normalize and Apply Patch: the preview-is-the-apply rule, every skip reason, the changeset/undo model, and the open items (non-UTF-8 rewriting, index refresh, library policy). |
| `tools/test_zipstore.sh` | core-test | Writes a zipstore archive and runs `unzip -t` on it, expecting 'No errors detected'; prints an honest [SKIP] and exits 0 when unzip is absent. |
| `tools/constitution_check.py` | wiring | R3 census input becomes app/main.cpp concatenated with every app/*.inc (sorted glob); R2 stays main.cpp-only; baseline stays 154. |
| `core/src/textdiff.cpp` | wiring | Add folioMarkerEnd as the single folio matcher (replacing the regexes at :106 and :569), applySelected, changedFolios/changedFoliosMarkdown/changedFoliosCsv, and lineOrigins. |
| `core/include/allcore/textdiff.h` | wiring | Declare folioMarkerEnd, applySelected, FolioChange/FolioReport + the three changed-folios writers, and lineOrigins. |
| `core/src/gofer.cpp` | wiring | Replace the inline extension/size test at :88-93 with a call to allcore::eligibleTextFile (gofer_smoke is the regression gate). |
| `core/src/libindex.cpp` | wiring | Replace the local eligible() at :38-46 with a call to allcore::eligibleTextFile (libindex_smoke is the regression gate). |
| `core/CMakeLists.txt` | wiring | Six add_executable/target_link_libraries/add_test blocks in the textdiff_smoke pattern (:148-150) for filewalk_smoke, versions_smoke, textnorm_smoke, textpatch_smoke, zipstore_smoke, docx_redline_smoke — all fixture-free so `ctest -LE fixture` covers them. |
| `app/main.cpp:21686` | wiring | DraftPane constructor initialiser list gains dataRoot_(root) beside root_(root) — the fix that makes Draft sidecars and versions real. |
| `app/main.cpp:3999` | wiring | docprops::textStatistics folioRe becomes the shared pattern @([0-9]{1,4}[AaBb]?)(?![A-Za-z0-9]) so the Folios column and the changed-folios headline cannot disagree. |
| `app/main.cpp:3451` | wiring | safeGetSaveFileName gains a trailing `QString* selectedFilter = nullptr` out-parameter; the g_saveDialogStub path leaves it untouched. |
| `app/main.cpp:4066` | wiring | docprops::renameFileTo: a (dir, ext) pair whose ext is "/" renames the DIRECTORY <dir>/<oldBase> → <dir>/<newBase>, so version histories follow a rename. |
| `app/main.cpp:8334` | wiring | Overlay writeDocumentTo: docprops::noteVersion(..., encoded `bytes`, kind 'document', reason 'save', docEncoding_, docLineEnding_, bom, statistics) immediately after noteSave; the returned notice goes to hint_. |
| `app/main.cpp:22069` | wiring | Draft saveDraft: noteVersion(..., toPlainText().toUtf8(), kind 'draft', reason 'save', "UTF-8", "LF") after noteSave; notice to termLive_. |
| `app/main.cpp:33548` | wiring | Manuscript save(bool autosave = false): noteVersion(..., toHtml().toUtf8(), kind 'manuscript', reason autosave ? 'autosave' : 'save', "UTF-8", "HTML") after noteSave; the 60 s timer at :33511 passes autosave = true. |
| `app/main.cpp:8629` | wiring | Overlay rename sidecar list gains {dataRoot_ + "/library/versions", "/"}; the Draft (:22132) and Manuscript rename callers gain the same pair. |
| `app/main.cpp:23829` | wiring | After #include "compare_pane.inc": #include "versions_pane.inc", "normalize_pane.inc", "replace_files.inc", "apply_patch.inc" — in that order, so each sees cmp::, textdiff, saveOrWarn, editops and docprops. |
| `app/main.cpp:39458` | wiring | File menu: 'Versions…' (⌥⌘V) after 'Revert File', routed through the same active() dispatch as Save. |
| `app/main.cpp:40371` | wiring | Tools ▸ Compare: 'Compare with Version…' after 'Compare with Saved Version'; 'Apply Patch…' after 'Open Conflict File…'; and (post-Word-gate only) 'Send Comparison to Word…'. |
| `app/main.cpp:40360` | wiring | frontDoc lambda and Compare with Saved Version flatten the Manuscript's saved HTML through QTextDocument::setHtml → toPlainText, so both sides are plain text. |
| `app/main.cpp:40099` | wiring | Find menu: 'Replace in Files…' (⇧⌥⌘F) after 'Find in Files…', seeded from FindDialog::needle()/replacement(); then 'Undo Last Replace in Files…' reading the newest changeset. |
| `app/main.cpp:40266` | wiring | Format menu: 'Normalize Text…' after 'Apply House Style Spacing', refusing the Manuscript with the stated message. |
| `app/main.cpp:24290` | wiring | FilesPane context menu after 'Open': 'Replace in these files…' and 'Normalize…', both seeded from selectedPathsIn(ix). |
| `app/main.cpp:4907` | wiring | FindDialog gains a `QString replacement() const` getter beside setReplacement, so the Find menu can seed Replace in Files. |
| `app/main.cpp:36786` | wiring | PreferencesDialog gains a 'Versions' page: keep-on-save, max versions, MB cap, rolling-autosave switch (QSettings versions/*). |
| `app/main.cpp:4178` | wiring | Properties Statistics tab gains a 'Versions kept: N (oldest …)' row and a 'Versions…' button; docprops::Input gains std::function<void()> openVersions. |
| `app/compare_pane.inc:377` | wiring | ComparePane::report gains kinds 'folios-md'/'folios-csv'; a QByteArray reportBytes(kind) twin is added for the binary 'docx' kind (wired post-gate); saveReport routes on the dialog's selectedFilter first, then the .folios. double suffix. |
| `app/compare_pane.inc:660` | wiring | Sessions ▾ menu gains 'Copy Changed Folios', 'Normalize Left…'/'Normalize Right…', and 'Apply Patch to Left…'/'to Right…'; the status line gains the '· changed folios: N of M' fragment. |
| `app/main.cpp:41751` | wiring | CLI: --compare gains .folios.md/.folios.csv/.docx suffix routing, and a new --apply-patch <target> <patch> [--out f] [--dry-run] [--fuzz N] [--reverse] block with exit codes 0 applied / 1 rejected / 2 unreadable. |
| `app/main.cpp:41809` | wiring | Selftest registration: fails += versionsWindow->selfTest(log) + normalizeDlg.selfTest(log) + replaceFiles.selfTest(log) + applyPatch.selfTest(log), beside comparePane's. |

---

# Batch 5 — Repeated Passages, similarity, Statistics study, Entities, Table compare

*Final Batch 5 specification, built on the design both judges picked (engineering-risk-first: one new Study pane with four pages, Table Compare as a page inside the existing Compare pane, main.cpp gets wiring only, Study renders no hgm_gloss so the G2 census cannot grow). Grafted from the other two designs: the .passages.db signature cache, translator-voiced controls, Entities footnote/index deliverables and the compile-time no-Place guarantee, the Provenance engine struct, containment beside Jaccard, standardised TTR, the extracted unitSyllableCounts split, the EXTERNAL tier and pinned provenance columns. All 30 must-fix items are answered and mapped in a ledger (§12). Two repo facts were verified before writing: constitution_check.py's R3 and G2 censuses read app/main.cpp only (so app/*.inc is outside both gates — the plan extends them with a mutation check), and the quotation.cpp tokenizer keeps editorial brackets inside a syllable (CHO[S] → cho[s), which is the batch's largest silent wrong-result path — resolved by one named apparatus policy in allcore/textspan with three modes, refusal-not-truncation for intra-syllable marks, and battery cases for [ ] { } ⟨ ⟩ ( ). Eight units, 180 h, sequenced so Passages precedes Statistics (no placeholder shingler) and so a stop after any unit leaves an honest surface.*

# Analysis suite — Batch 5 plan (Repeated Passages · Similarity · Statistics · Entities · Table Compare)

**Status:** specification, not yet built. **Research verdicts:** `docs/TEXT_ANALYSIS_SUITE_RESEARCH.md` §8 (near-duplicates), §9 (similarity; import TRACER, do not rewrite it; paraphrase LATER), §10 (statistics; Voyant-style Trends/Collocates/Phrases; no word cloud, no readability), §11 (entities, distinctive terms, co-occurrence; no summaries, no sentiment, no chat), §14 (structured-data table compare), §18 (batch sequencing). **House audit format:** `docs/release_audit/ComparePane.md`. **Compare-suite matrix:** `docs/COMPARE_SUITE_PLAN.md`.

How this document was produced: three designs were written for this cluster and scored by two independent judges. Both judges chose the same winner (the engineering-risk-first design; combined totals 89 / 85 / 80). This specification takes that design as its spine, grafts the ideas both judges recommended from the other two — the library-wide signature cache, the translator's vocabulary on every control, the Entities footnote/index deliverables, the `Provenance` struct, containment beside Jaccard, standardised TTR, the shared unit split, the EXTERNAL tier, pinned provenance columns — and resolves every must-fix item. §12 is the ledger that maps each must-fix to the section that answers it.

---

## 0. Decisions that shape the batch

These are settled here so that no feature re-opens them.

**D1 — One new pane, one new page.** All four study surfaces are pages of a single new **Study** pane in the Research group, in its own `app/study_pane.inc`. Table Compare is **page 3 of the existing Compare pane**, in `app/table_compare.inc`, with page-local control rows (the `FolderComparePage` idiom) so the Compare ribbon does not grow at all. `app/main.cpp` receives wiring only.

**D2 — Study renders no `hgm_gloss`.** Meanings are reached by handing a term to the Lookup pane through the existing `g_lookupQuery` hook (`app/main.cpp:3671`). Coverage columns ("in my glossary", "in the dictionary") are booleans plus a tier label, never gloss text. Consequence: the G2 binding-gloss census cannot grow from this batch, and the project's worst historical defect class is structurally out of scope. Where a catalogue's own published English title is shown (Entities), it is shown verbatim under a REFERENCE badge and is not `hgm_gloss`.

**D3 — Estimators never produce findings.** MinHash/LSH are candidate generators; every passage reported has been re-verified syllable by syllable in both texts. TF-IDF prints its formula and refuses to score when the document frequency is unknown. Table records are matched by key as multisets; a duplicated key is AMBIGUOUS, never paired by position.

**D4 — One tokenizer, one script routing, one folio walk, one apparatus policy.** All four live in the new `allcore/textspan`, promoted from code that already exists, and their proof is that the existing `quotation_smoke` and `textdiff_smoke` batteries stay green. No engine in this batch accepts raw text; they accept a `std::vector<Syllable>` produced once at the pane boundary, so a script conversion or an apparatus decision cannot be made twice with two answers.

**D5 — `Provenance` is an engine struct, not a caption.** Every engine returns one; every saved report and every cache row carries it. Rule 4 becomes mechanical.

**D6 — Refusing is shipped behaviour.** No key column chosen → a refusal that lists the available column names, never column 0. A score cell that is not a finite number → the row is refused and line-numbered, never coerced to 0. A candidate that cannot be read → "not compared — <reason>", never 0.0 similarity. A syllable whose editorial bracket falls inside it → counted as refused and listed, never truncated silently.

**D7 — No new keyboard accelerators.** The Find/Compare/Navigate families already own the obvious keys. Study is reached from the Research tab row, from **Tools ▸ Study ▸ …** (four items, no accelerators), from the Library pane's Maintenance menu, and from two CLI flags. If Adam wants an accelerator later it is one line.

**Effort total: 180 hours** across eight units. This is realistically three sub-batches (see §9); the sequencing names the shipping order so that a stop after any unit leaves a coherent, honest surface.

---

## 1. F0 — `allcore/textspan` + `allcore/provenance` (shared substrate)

### Purpose and user story
> As a translator I click a repeated passage on one page and a term on another, and both tell me the same folio and line the Overlay shows me — because there is one folio walk, one syllable tokenizer, one script routing and one apparatus policy in the whole program, not five.

The repository today contains **four** folio walks (`core/src/catalog_id.cpp` `folioBefore`, `core/src/textdiff.cpp:568` `citeFor`, `core/src/catalog_list.cpp` `acipFolioRange`, `app/main.cpp` `OverlayPane::resolveFolioAt`) and **one file-private wylie syllable tokenizer** (`core/src/quotation.cpp:12-43`, in an anonymous namespace). Adding a fifth of each is the maintenance hazard three subsystem maps already name. This module promotes rather than reimplements.

### UI
None. This is an engine module with no control of its own. Its only visible effect is that citations produced by Draft ▸ Detect quotations, Compare ▸ apparatus criticus and every Batch 5 surface use one identical `@012B.3` form. **Fit ratchet: no effect** (no widget).

Honesty behaviour it enforces on its callers:
- `Converted::unconvertedLines` lists every line `unicodeToWylie` warned on or that would not convert. Those lines are never shingled, never counted and never cited as converted; every caller must disclose the count (rule 3).
- `citeAt` falls back to `line 57` when no folio marker precedes the offset. It never invents a folio.
- The apparatus policy is a named, disclosed choice, never a silent one (below).

### The apparatus policy (new, and the batch's largest correctness fix)
The promoted tokenizer strips non-alphanumeric characters only at syllable **edges**, so an editorially marked reading survives with the mark inside it: `CHO[S]` tokenizes as `cho[s`. That form becomes its own frequency type, a false hapax, and a shingle that can never match the same passage in an unmarked witness — silently lowering every similarity number between a marked and an unmarked edition. One policy, chosen once, applied by every study surface:

`enum class Apparatus { StripSpan, KeepInner, AsWritten }`

- **`StripSpan` (default)** removes whole `[…]`, `{…}` and `⟨…⟩` spans **before** splitting, using the same regex `textdiff::normalizeLine` already applies for `ignoreApparatus` (`core/include/allcore/textdiff.h:24`) — so Study and Compare cannot disagree about what an apparatus span is. A mark whose span boundary falls **inside** a syllable (`CHO[S]`) is not truncated: that syllable is **refused**, counted in `Provenance.units_refused`, and listed by line. `(…)` is retained as text and counted, because in ACIP round brackets are not the house apparatus convention; the count is disclosed so the choice is visible.
- **`KeepInner`** removes only the delimiters and keeps the letters, which is what a translator wants when comparing a witness carrying supplied letters against an unmarked one.
- **`AsWritten`** does nothing, for counting a diplomatic transcription as it stands.

The chosen mode is a control on the Study pane, is recorded in `Provenance.apparatus`, and is printed in every saved report header.

### Data
No persisted data. Types only.

```cpp
struct Syllable { std::string text; size_t begByte, endByte; int line; bool barrierAfter; };
struct Converted { std::string wylie; int unconvertedLines = 0; std::vector<int> unconvertedLineNumbers; };
struct FolioLoc { std::string folio; int lineInFolio = 0; int linesInFolio = 0; };
struct Provenance {                 // allcore/provenance.h
    std::string engine;             // "passages/1", "textstats/1", …
    std::string built_utc;
    std::string source, source_sha256;
    std::string script_in;          // acip | wylie | unicode | mixed
    std::string norm;               // "verbatim" | "search-fold/<generation>"
    std::string apparatus;          // "strip-span" | "keep-inner" | "as-written"
    long long units_read = 0;       // syllables actually counted
    long long units_refused = 0;
    int apparatus_marks_stripped = 0;
    std::vector<std::string> refusals;   // line-numbered, human-readable
};
std::string provenanceBlock(const Provenance&);   // the report header text
```
`Provenance` travels inside every engine result in this batch and is stamped into every saved report and every cache row.

### Engine
**Module** `core/include/allcore/textspan.h` + `core/src/textspan.cpp`; `core/include/allcore/provenance.h` + `core/src/provenance.cpp`.
**Rewires (same commits):** `core/src/quotation.cpp` calls the promoted `syllablesWylie`; `core/src/textdiff.cpp`'s file-static `toEwts` (83-93) and `citeFor` (568-580) delegate; `core/src/catalog_list.cpp` `acipFolioRange` delegates (public name unchanged).

```cpp
std::vector<std::string> syllablesWylie(const std::string& wylie);
std::vector<Syllable>    syllableSpans(const std::string& wylie, Apparatus = Apparatus::StripSpan,
                                       Provenance* out = nullptr);
std::string  toEwtsLine(const std::string& line, bool* ok = nullptr);
Converted    toEwtsDocument(const std::string& text);
FolioLoc     folioAt(const std::string& text, size_t byteOffset);          // strict: @NNN[AaBb] required
std::string  citeAt(const std::string& text, size_t byteOffset);          // lenient regex, textdiff-compatible
std::pair<std::string,std::string> folioRange(const std::string& text);
int countFolioMarkers(const std::string& text);   // @\d{2,3}[AaBb]\b — docprops' own regex
int countShads(const std::string& text);          // ',' in the apparatus-stripped body
```

**Algorithm.** No new algorithm; this is de-duplication with a battery. `syllablesWylie` is the code of `core/src/quotation.cpp:12-43` **moved, not retyped** (split on whitespace and `/ | , ;`; strip non-alphanumeric edges except the apostrophe; keep `' + . ~` inside; lowercase A–Z; drop `@`-prefixed markers and all-digit tokens). `syllableSpans` is the same loop carrying byte offsets, 1-based line and a barrier flag set when the flush was caused by `, ; / |` or a line break. `toEwtsLine` is `textdiff.cpp`'s rule moved out (Tibetan codepoints → `unicodeToWylie`, `ok=false` when `warns>0`; all-uppercase-with-no-lowercase → `acipToEwts`; otherwise unchanged) — canonical converters only, per rule 2.

**Two folio functions, deliberately.** `citeAt` keeps `citeFor`'s lenient regex `@([0-9]{1,4}[AaBb]?)` so `textdiff`'s apparatus output does not change; `folioAt` uses the strict side-bearing form that the other three implementations require, and is what new code uses. The divergence is documented in the header **and pinned by a cross-check test** (T9 below) rather than left to drift.

**Reuses:** `allcore::acipToEwts`, `allcore::unicodeToWylie`, `textdiff::Options::ignoreApparatus`'s regex, `docprops::textStatistics`'s folio/shad regexes (`app/main.cpp:3995-4022`) replicated exactly so Properties and Study cannot disagree.

### Acceptance tests
**CTest `textspan_smoke`** (fixture-free, so it runs under `ctest -LE fixture`):
1. `syllablesWylie("@012a SANGS RGYAS , chos 123 pa'i") == {"sangs","rgyas","chos","pa'i"}` — marker dropped, pure digits dropped, split at `,`, case folded, apostrophe kept.
2. For every syllable of `syllableSpans(t)`, `t.substr(beg, end-beg)` lowercased and edge-stripped equals `.text`; offsets strictly increasing.
3. `syllableSpans("a b , c d\ne")` sets `barrierAfter` on the syllable before `,` and before `\n` and nowhere else; lines are 1,1,1,1,2.
4. Apparatus `StripSpan`: `"chos [kyi] nyid"` → `{"chos","nyid"}`, `apparatus_marks_stripped == 1`, `units_refused == 0`.
5. Apparatus `StripSpan`, intra-syllable mark: `"cho[s] nyid"` → `{"nyid"}`, `units_refused == 1`, `refusals[0]` names line 1 and the raw form. **Nothing is truncated to `cho`.**
6. Apparatus `KeepInner` on the same input → `{"chos","nyid"}`, `units_refused == 0`.
7. Apparatus `AsWritten` on the same input → the bracketed form survives as its own token and `apparatus_marks_stripped == 0`.
8. `{…}` word-initial, medial and final, and `⟨…⟩`, behave as (4)/(5); `(…)` is **retained** under every mode and counted separately.
9. **Cross-check:** for a fixture of ten well-formed markers, `citeAt(text, off)` and `folioAt(text, off)`-rendered cites are byte-identical at every offset; for the single malformed case `@012` (no A/B side) the test asserts `citeAt` returns `@012.N` and `folioAt` returns `line N`, and names that as the one deliberate difference.
10. `toEwtsDocument` over a three-line text whose middle line will not convert → `unconvertedLines == 1`, `unconvertedLineNumbers == {2}`.
11. `citeAt("@012A\nx\ny\n", offset of 'y') == "@012A.3"`; a text with no marker → `line 3`.
12. `folioRange("@001A … @014B …") == {"001A","014B"}`; `countFolioMarkers`/`countShads` on the ACIP fixture equal the values `docprops::textStatistics` produces for the same string.
13. **Regression, and the point of the module:** existing `quotation_smoke` and `textdiff_smoke` (43 checks) pass **unchanged** after the promotion. A red battery means the promotion was not faithful.

**App `--selftest`** (in `StudyPane::selfTest`, since the module has no UI): 14. `folioAt`/`citeAt` are exercised through a **Tibetan-Unicode** fixture from the Overlay path, asserting the byte↔UTF-16 conversion in the wrapper is right (a Tibetan codepoint is 3 UTF-8 bytes; an unconverted wrapper cites the wrong line).

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A promotion that is not byte-identical silently changes `detectQuotations` recall | Move the function, do not retype it; `quotation_smoke` unchanged in the same commit is the gate |
| `citeFor` lenient vs `folioAt` strict can print two cites for one byte | Both kept, both named, divergence pinned by test T9 and stated in the header |
| Byte vs UTF-16 offsets at the Overlay boundary | Explicit conversion in the wrapper; selftest T14 on a Unicode fixture |
| `syllablesWylie` takes wylie, not ACIP or Unicode; a caller that forgets gets silently wrong counts | Engines take `std::vector<Syllable>`, never raw text — the conversion happens once at the pane boundary and its refusal list travels with it |
| `catalog_list_smoke` runs against the gitignored `library/`, so the `acipFolioRange` rewire is uncovered on a clean checkout | `textspan_smoke` ships its own `folioRange` fixture strings and is fixture-free |
| Apparatus refusals could be numerous on a heavily marked witness | The count is on screen with a one-click switch to `KeepInner`; no run is silently smaller |

**Effort: 16 h. Depends on: nothing.**

---

## 2. F1 — Study pane shell, wiring, and the constitution gate fix

### Purpose and user story
> As a translator with a text open, I want one place called Study where I can measure it, find what repeats in it, see which other texts share material with it, and list the people, texts and dates it names — without leaving the app or learning four surfaces.

### The gate fix (must land before any new `.inc` surface)
Verified in this repository: `tools/constitution_check.py` builds its census dictionary from `core/src`, `core/include/allcore`, `core/tests` and **`app/main.cpp` only**. Both the **R3 modal census** and the **G2 `hgm_gloss` census** read `app/main.cpp`. Every dialog and every gloss inside `app/compare_pane.inc` — and inside the two new `.inc` files — is invisible to both gates today. This batch **extends both censuses to `app/*.inc`**, reviews the existing `.inc` sites once, installs the new baselines (`tools/constitution_baseline_inc.txt`, `tools/constitution_hgm_baseline_inc.txt`) in the same commit, and verifies with a mutation check: inject an unguarded `QMessageBox` into an `.inc`, confirm the gate fires, revert, record the result in the audit row. This moves a gate Adam owns, so it is flagged in §11.

### UI
**Placement.** New pane **Study** in the Research group: `mkGroup("Research", {"Search","Lookup","Sanskrit","Convert","Analysis","Study","Compare"})` at `app/main.cpp:39208`; `tabs.addTab(studyPane, "Study")` beside the Compare pane's `addTab`; `paneIcon` reuses the existing icon kind `"count"` so no icon code is added. `app/study_pane.inc` is included after `compare_pane.inc` so it can use `cmp::readText`, `textdiff`, `saveOrWarn`, `docprops`, `RibbonBar`, `sess::remember` and `app/ux_tokens.h`. **Not** a page inside AnalysisPane: that pane is API-key-gated, and deterministic counting must not sit behind a key gate.

**Page switcher** (QToolButton row, the `ComparePane` idiom at `app/compare_pane.inc:218`): **Statistics · Passages · Similarity · Entities**.

**Ribbon — three compact groups only.**

| Group | Control | What it does |
|---|---|---|
| STUDY | **Text…** | File picker → `openPath`; reads with `cmp::readText` (UTF-8, EOL detected, binary refused) |
| STUDY | **Use the front document** | Takes name/text/path from the same `frontDoc` lambda Tools ▸ Compare uses; carries the Overlay's chosen encoding |
| STUDY | **Refresh** | Re-runs the current page against the current text |
| SCOPE | *(read-only label)* | "Studying: `<name>` — 4,812 syllables · 612 lines · folios 1a–24b · read as UTF-8" or "…the document open in the Overlay (encoding `<name>`)" |
| SCOPE | **Editorial marks:** `Strip the marked span ▾` | The F0 apparatus policy: Strip the marked span (default) · Keep the letters inside the marks · Leave the text as it stands. Re-runs the current page and restamps the provenance |
| REPORT | **Save report…** | Per-page Markdown/CSV through `saveOrWarn`, routed by `selectedNameFilter` (not by suffix — the apparatus report already owns `.md`) |
| REPORT | **Copy** | The current page's report to the clipboard |

**Foot of the pane:** the shared status line carrying the MACHINE badge and the honest counts.

**Pane banner (always visible):** *"Study measures, counts and matches. It never composes English and never proposes a reading. For what a term means, send it to the Lookup pane."*
**Whenever the header's refusal counts are non-zero:** *"N line(s) could not be converted and are not counted — lines 17, 92, 118."* and *"M syllable(s) carry an editorial mark inside them and are not counted — switch Editorial marks to 'Keep the letters inside the marks' to include them."*

**Entry points elsewhere (no ribbon growth anywhere):** Tools ▸ Study ▸ *Statistics for This Text… / Repeated Passages in This Text… / Compare This Text with Others… / Entities in This Text…*; the Library pane's existing Maintenance menu gains *Statistics for the selected text…* and *Repeated passages in the selected text…*; the Overlay's REVIEW context menu gains *Where else does this passage appear?* (raises Passages with the selection as the query).

**Keyboard:** none new (D7). **Fit ratchet:** three compact groups, no big-icon proxies; design target ≤ 700 px for the Study ribbon, and the app-wide worst must stay ≤ `kRibbonRatchet = 2572` (`app/main.cpp:43708`). Every wide table scrolls inside its own viewport so the pane's `minimumSizeHint().width()` stays ≤ 800.

### Data
Session memory only, through `sess::remember` (no-ops under `g_harnessRun`): `sess/study/page`, `/apparatus`, `/fold`, `/topN`, `/k`, `/minRun`, `/floor`, `/scope`, `study/lastText`. No new directory under `data/` is created by the shell. Report defaults: `<textBaseName>-statistics.md`, `-passages.csv`, `-similarity.md`, `-entities.md`, beside the text, user-chosen.

Every saved report opens with `provenanceBlock(prov)`: engine + version, built UTC, source path and sha256, script in, norm, apparatus mode, units read, units refused, refusals, plus app version and git commit (the strings `buildDiagnosticReport` already uses).

### Engine
`app/study_pane.inc` — `StudyPane` plus four page classes, each with its own `selfTest`. No core module of its own.

```cpp
class StudyPane : public QWidget {
  StudyPane(const QString& root, allcore::Spine& spine, QWidget* parent = nullptr);
  void setText(const QString& name, const QString& text, const QString& path);
  bool openPath(const QString& path);
  void showPage(int);
  int  selfTest(QStringList& log);
};
static std::function<void(const QString&,const QString&,const QString&)> g_studyText;
static std::function<std::vector<allcore::PersonRef>()> g_personBank;
static std::function<QMap<QString,QPair<QString,QString>>()> g_titleBank;
```
CLI: `--stats <file>` and `--passages <file|dir>` print the page's Markdown report to stdout, exit 0/1/2 mirroring `--survey`; both flags are added to the single `early.contains(...)` harness list so R2 stays satisfied and no dialog can open under them.

**Reuses:** `compare_pane.inc:218` page hosting; `cmp::readText`; `frontDoc`; `saveOrWarn` (WP-1); `safeGet*FileName` (harness-stubbed); `RibbonBar`; `ux::sourceBadge` (`app/ux_tokens.h`); `app/textreport.h` `corpusCountLine` / `goferSkipNotice`; hooks `g_raisePane` (39251), `g_openAtLine` (38610), `g_lookupQuery` (3671), `g_compareTexts` (38627), `g_goferQuery` (38631), `g_openAuthorByPid` (25888).

### Acceptance tests
**CTest:** none (no engine). The gate work is verified by the constitution target itself:
1. `constitution` gate passes with the extended `app/*.inc` censuses and the reviewed baselines committed in the same change.
2. Mutation check (run once by hand, recorded in the audit row): an unguarded `QMessageBox` added to an `.inc` **fails** the gate; reverted.
3. R2 still holds: `early.contains("--pasteprobe")` appears exactly once in `app/main.cpp` after `--stats` / `--passages` are added to the list.

**App `--selftest` (`StudyPane::selfTest`):**
4. `setText("fixture", <ACIP fixture>, "")` then `showPage(0..3)`: each page renders without throwing and its status line is non-empty (4 checks).
5. No page opens a modal under `g_harnessRun`: each of **Save report…**, **Text…**, **Import…** is called directly and returns `false` with a status message rather than blocking (the `harnessAutoConfirm_` pattern of `FolderComparePage::confirmOps`).
6. Switching **Editorial marks** re-runs the page and changes the header's refused count on the bracket fixture.
7. Fit gate: Study pane `minimumSizeHint().width() ≤ 800`.
8. Ratchet gate: the app-wide worst ribbon width is logged and `≤ 2572`; the Study ribbon alone is logged for the audit row.
9. CLI: `--stats <fixture.act>` prints a report containing `engine: textstats/1`, `Syllables` and `lines not converted`, exits 0; a missing file exits 2 with one line on stderr.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Two more `.inc` files lengthen an already 45k-line translation unit | Pages are thin (widgets + report assembly); every loop lives in `allcore`, compiled once and tested by CTest |
| The gauntlet random walk clicks every enabled control and could start a long scan | Every Study control is read-only or writes only through a user-chosen dialog; long runs refuse to start under `g_sweepActive` and are Stop-able through the GoferPane pump idiom |
| `cmp::readText` is UTF-8 only; a Latin-1 ACIP file studied by path mis-decodes | The scope label states "read as UTF-8"; the front-document route (which carries the Overlay's encoding) is what the Tools menu uses |
| The baseline bump moves a gate Adam owns | Flagged in §11 and in the audit row, never silently |

**Effort: 14 h (4 gate + 10 shell). Depends on: F0.**

---

## 3. F2 — Repeated Passages (shingles → MinHash → LSH → verify)

### Purpose and user story
> Three moments in a day. (a) Reading a commentary I meet a verse and need to know whether it is quoted from somewhere and how to cite it in a footnote. (b) Proofreading, I want the passages this file repeats against itself: a refrain is fine, but a 40-syllable run repeated verbatim two folios apart is usually a dittography or a double-keyed block. (c) Starting a project I want to know how much of this commentary is verbatim Tsongkapa before I promise a delivery date.

### UI
**Placement.** Study pane ▸ page 1 **Passages**: a control row, a `QTreeWidget` of results, the shared status line. Also reached from the Overlay context menu on a selection ("Where else does this passage appear?"), from the Library Maintenance menu, and from the Statistics page's **Repeating phrases** rows ("See every occurrence").

| Control | Label as shown | What it does |
|---|---|---|
| Scope | **Repeats inside this text** (default) · **Between this text and…** *(Another file… / This folder… / The HGM course corpus)* | Chooses the comparison. The corpus option routes to `allcore::detectQuotations`, not to shingling |
| Folder glob | **Only files matching:** `*.act` | Include/exclude glob for folder scope; the eligibility rule is the search engines' own (`.txt .acip .md .act .inc .ace`, ≤ 10 MB) |
| k | **Passage length to match on: [8] syllables** | The shingle length (3–16). Label states "passages shorter than this are never found" |
| floor | **Shortest run to report: [8] syllables** | The reporting floor (never below k). Default rises to **12** for texts `analyzeVerse` calls verse (8 is one line of a 7-syllable meter plus a particle) and the header says so |
| match | **Match: Exactly as written** (default) · **Also matching spelling variants** | The second applies `searchFoldWylie`; every run found only that way is labelled per row |
| similarity floor | **Report pairs above: [0.30]** | Candidate filter. **Disabled and greyed with a tooltip** in the within-text scope, where it has no meaning |
| Run | **Find repeats** · **Stop** | Pump/Stop idiom; Stop reports "stopped after N of M" |
| Sort | **Group by pair · Longest first · In text order** | Sorts the result list |
| Report | **Save report…** (Markdown / CSV) · **Copy** | Through `saveOrWarn` |

**Result rows.** Level 1 = pair, showing shared runs, longest run, **exact** overlap and, in its own column, the **estimate**. Level 2 = each run: `@012B.3 ↔ @047A.1 · 23 syllables`, the run verbatim in wylie (and Tibetan when the source was Unicode), and per-row buttons **Go to first** · **Go to second** (`g_openAtLine`) · **Show in Compare** (`g_compareTexts` with the two runs ±3 lines of context) · **Copy citation**.

**Honesty labels.**
- Header: *"MACHINE — candidates were found by MinHash/LSH; every run listed below was then re-checked syllable by syllable in both texts. The estimate column is only how the pair was found, never why it is listed."*
- Per row when found only after folding: *"same after search normalisation (affix · da-drag · ba/pa · verb stem) — NOT verbatim"*, with `ux::Epistemic::Machine`. Verbatim rows carry no such mark, and the two modes are never merged into one table.
- Above the floor control: *"Runs shorter than the floor are not reported. Short agreements are ordinary Tibetan phrasing, not evidence of a shared source."*
- *"Shared wording only. This does not detect paraphrase or allusion, and a text that says the same thing in other words will not appear here."*
- Verse annotation from `textDna`: a run inside verse is marked *"in verse — may be a refrain"*; a run following a `zhes`/`ces` announcement is marked *"announced quotation"*.
- Skips: *"Read 412 of 480 files; 68 skipped (3 unreadable, 12 binary, 53 shorter than the passage length). A repeat that exists only in the unread files will not appear here."*
- Caps: *"Stopped after 5,000 candidate pairs — the report is partial and these are the pairs found first, not the strongest."* / *"Showing the 500 strongest pairs of 1,204 found."*
- Stop: *"PARTIAL — stopped after 5 of 480 files"*, in the header **and** in the saved report.
- Corpus scope: results carry the EVIDENCE badge, cite `[COURSE:seq]`, show the published English verbatim under the HGM badge, and say *"through COURSE:seq"* because a cross-segment merge leaves `seq` holding the range end.

**Fit ratchet:** page-local control row (two lines of small widgets), no ribbon group added; results table scrolls horizontally in its own viewport.

### Data
No persistent store in this feature (the signature cache is F3). Report CSV header `a_file,a_cite,b_file,b_cite,syllables,match,text` where `match ∈ {verbatim, folded}`; Markdown opens with `provenanceBlock` plus scope, k, floor, fold, files read/skipped, unconverted lines, caps fired. Reports go to a user-chosen path, default `<textBaseName>-passages.csv`.

Provenance fields: `engine="passages/1"`, k, hashes, bands, seed, fold generation (verb bank loaded or not), apparatus mode, per-file `sha256`, files read/skipped with reasons, unconverted line list, `verbatim` per run.

### Engine
`core/include/allcore/passages.h` + `core/src/passages.cpp`. CTest `passages_smoke`, fixture-free.

```cpp
struct Doc { std::string id, label, raw; std::vector<Syllable> syl; Provenance prov; };
struct Options { int k=8, hashes=128, bands=32, minRun=8, maxCandidatePairs=5000,
                 maxHitsPerPair=200, maxPairs=500;
                 double minJaccard=0.30, stopShingleDocFraction=0.5;
                 bool fold=false; unsigned long long seed=0x9E3779B97F4A7C15ULL; };
struct Repeat { std::string aDoc,bDoc; int aBegSyl,aEndSyl,bBegSyl,bEndSyl,syllables;
                std::string text,aCite,bCite; bool foldedMatch; };
struct PairScore { std::string aDoc,bDoc; double jaccardEstimate, jaccardExact;
                   double containmentA, containmentB; int sharedShingles, longestRun; };
struct Report { Provenance prov; std::vector<Repeat> repeats; std::vector<PairScore> pairs;
                int docs, shingles, candidatePairs, skippedShort, skippedUnreadable;
                bool truncated; std::string capNote; };
struct Signature { std::string id; std::vector<std::uint64_t> h; int shingles; };

std::vector<std::uint64_t> shingleHashes(const std::vector<Syllable>&, const Options&);
Signature   minhash(const Doc&, const Options&);
double      estimateJaccard(const Signature&, const Signature&);
std::vector<std::pair<int,int>> lshCandidates(const std::vector<Signature>&, const Options&);
Report      repeatsWithin (const Doc&, const Options&);
Report      repeatsAcross (const std::vector<Doc>&, const Options&,
                           const std::function<bool(int,int)>& pump = {});
Report      sharedPassages(const Doc& a, const Doc& b, const Options&);   // exact, no LSH
std::vector<std::string> shingleForms(const Doc&, int n);   // Statistics' Phrases uses this
std::uint64_t passageGeneration(const Options&);            // stamped into F3's cache
```

**Algorithm — four published steps, none from any product's code.**
1. **Shingling.** A document is the set of its contiguous *k*-syllable sequences — Broder, *On the resemblance and containment of documents*, SEQUENCES 1997; resemblance is the Jaccard coefficient of the shingle sets and containment is `|S(A)∩S(B)| / |S(A)|`. Shingles are k **syllables** from F0, never characters or words, and never cross a barrier.
2. **Hashing.** 64-bit FNV-1a of the joined syllables (Fowler/Noll/Vo, published specification — and the repository's only existing hash idiom, file-private in `tree_diff.cpp`), then `h_i(x) = splitmix64(a_i·x + b_i)` with `a_i` odd, from the Carter–Wegman universal family (*Universal classes of hash functions*, JCSS 18(2), 1979) with the SplitMix finalizer (Steele, Lea, Flood, OOPSLA 2014). Constants are compile-time and seeded, so signatures are reproducible across runs and machines; `passageGeneration` carries a version integer that **must** be bumped if the family ever changes.
3. **MinHash.** 128 min-wise independent permutations (Broder 1997; Broder, Charikar, Frieze, Mitzenmacher, *Min-wise independent permutations*, STOC 1998). The fraction of agreeing minima estimates resemblance with standard error ≈ 1/√128 ≈ 0.088.
4. **LSH banding.** 32 bands × 4 rows; a pair is a candidate when any band's four minima agree — Indyk & Motwani, STOC 1998; Leskovec, Rajaraman & Ullman, *Mining of Massive Datasets*, ch. 3. The S-curve threshold ≈ (1/b)^(1/r) ≈ 0.42 is printed beside the floor the user chose, so the user knows what the sweep could miss.

**The honesty step.** LSH output is a candidate list only. Every candidate pair is confirmed exactly: index B's k-gram hashes by position, walk A's k-grams, and on a hit compare the actual syllables and extend greedily left and right while syllables are equal — seed-and-extend (Altschul, Gish, Miller, Myers & Lipman, *Basic local alignment search tool*, J. Mol. Biol. 215, 1990), which is exactly the greedy extension `detectQuotations` already performs. Maximal non-overlapping runs ≥ `minRun` are kept; `jaccardExact` and both containments are recomputed on the real shingle sets. `repeatsWithin` is the same procedure with A == B, discarding self-overlap. `sharedPassages` skips LSH entirely, so a two-text study is exact by construction. Shingles occurring in more than `stopShingleDocFraction` of the documents are dropped as stop-shingles and their count is disclosed. Folded mode applies `searchFoldWylie` per distinct syllable (memoised — `stripAffixedParticlesWylie` converts through `wylieToUnicode` repeatedly) and marks every resulting run `foldedMatch = true`.

**Reuses:** F0 entirely (tokens, offsets, barriers, cites, script routing, apparatus); `allcore::searchFoldWylie` (`searchnorm.h:52`); `allcore::detectQuotations` (`quotation.h`) unchanged for the corpus lane; `allcore::textDna` (`textdna.h:37`) for the verse/quotation annotation; `g_compareTexts` for "Show in Compare"; `g_openAtLine`; the GoferPane pump/Stop idiom and `goferSkipNotice` wording.

### Acceptance tests
**CTest `passages_smoke`** (fixture-free):
1. A 10-syllable document with k=8 yields 3 shingles; k=10 yields 1; k=11 yields 0 and the doc appears in `skippedShort` **with a reason**, not silently.
2. **Pinned hash:** `shingleHashes` for a fixed input asserts a literal expected value, so the hash family cannot change silently and invalidate F3's cache; `passageGeneration` differs when k, hashes, bands, fold or the version integer differ.
3. `minhash` twice with the same seed → byte-identical signatures; a different seed → at least one differing value.
4. Two identical documents → `estimateJaccard == 1.0` and `jaccardExact == 1.0`; two disjoint documents → 0.0, 0.0, and `lshCandidates` returns no pair.
5. **Estimator accuracy over five seeds:** two documents sharing exactly 50 of 100 shingles each (exact Jaccard 1/3) → `|estimate − 0.3333| ≤ 0.10` for every one of five seeds, so the bound is not a lucky draw.
6. `repeatsWithin` on `"a b c d e f g h x x a b c d e f g h"` with k=8, minRun=8 → exactly one `Repeat`, `syllables == 8`, `aBegSyl == 0`, `bBegSyl == 10`, `foldedMatch == false`; with `minRun = 9` → zero (the floor is honoured, not rounded).
7. One extra shared syllable → `syllables == 9`, proving the extension is maximal rather than fixed at k.
8. **Containment vs resemblance:** a 100-syllable document fully inside a 1,000-syllable one → `containmentA ≥ 0.95` while `jaccardExact ≈ 0.1`.
9. Fold off, a pair differing only by `pa` vs `pa'i` → no repeat; fold on → one repeat with `foldedMatch == true` and `prov.norm` beginning `search-fold/`.
10. Cites: a fixture with `@012B` before the second copy → `bCite` begins `@012B.`; a fixture without markers → cites begin `line `.
11. `maxCandidatePairs = 1` over three mutually similar documents → `truncated == true`, `capNote` non-empty, and the reported pair is still exactly verified.
12. A document whose middle line will not convert → `prov.units_refused` reflects it and **no shingle contains a syllable from that line**.
13. `sharedPassages(a,b)` returns the same repeats as `repeatsAcross({a,b})` for the same options (one code path, pinned).
14. LSH recall: 40 synthetic documents with three planted pairs at exact Jaccard ≥ 0.60 → all three planted pairs are candidates.

**App `--selftest` (`PassagesPage::selfTest`):**
15. Within-text scope on a planted 12-syllable repeat → one top-level row, one child, whose copy-citation string matches `<file>, @001A.2 ↔ @001B.1`.
16. Results header contains the literal `re-checked` and the word `estimate`.
17. "Also matching spelling variants" → every affected row's HTML contains `NOT verbatim`.
18. The similarity floor spin is **disabled** in the within-text scope and its tooltip explains why.
19. Folder scope with one unreadable file → the summary contains `skipped` and the reason; Stop before completion → header and report contain `PARTIAL`.
20. Folder scope refuses to start under `g_sweepActive`; `Save report…` returns false with a status line under `g_harnessRun`; no modal opens; the temp fixture directory is removed at exit.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Degenerate LSH buckets (a formula shared by every text) turn verification into O(n²) | Stop-shingle removal above a document fraction, candidate-pair cap with `capNote`, per-pair hit cap, Stop pump |
| Presenting an estimate as a finding | Structural: `repeatsAcross` never emits a pair without at least one verified run; `jaccardEstimate` is a separate, separately labelled field |
| Folded mode is lossy and search-layer only | `foldedMatch` per run, per-row label, verbatim default, the two modes never merged in one table |
| Library-wide runs are minutes without a cache | F3 lands in this batch; until it does, folder scope states its cost on the control and is Stop-able |
| `detectQuotations` issues one FTS query per position with `LIMIT 20` — unsuited to a whole text | The corpus lane caps the passage length it sends and says so; long texts route through the shingle engine against a corpus `Doc` set instead |
| A merged `QuotationMatch::seq` holds the range **end** | The corpus lane writes *"through COURSE:seq"*, never implying a start |
| A repeat is not a quotation, and direction is not measurable | The report cites both sides and never asserts direction or dependence; the duplicate-work question is handed to `qcDuplicateTitles`, whose verdicts distinguish "same colophon" from "distinct works sharing a title" |
| Memory on a large library | `repeatsAcross` holds signatures (128×8 bytes per doc ≈ 9 MB for 8,988 files) and re-reads only the two files of a candidate pair for verification |

**Effort: 28 h. Depends on: F0, F1.**

---

## 4. F3 — Library-wide passage index (`.passages.db`)

### Purpose and user story
> When I ask "where else in my library does this passage appear", I expect an answer while I am still holding the thought — and I expect the app to tell me plainly when the index is out of date or was never built, rather than quietly answering from a stale one.

Without this, §8's "across the library" scope recomputes ~9,000 files per run and is a control nobody presses. Grafted wholesale from the design both judges named for it.

### UI
No pane of its own. Three controls:
- **Library pane CARE ribbon** (beside *Update search index*): **Update passage index…** — a `QProgressDialog` with **Stop**, exactly like `updateIndex`; reports added / updated / removed / unchanged / write failures, and *"stopped, N of M done"* — never "up to date" after a cancel.
- **Passages page status line**, above the folder scope: *"Passage index: 8,988 texts, built 2026-09-09 14:02 · 41 texts changed since — [update]"* or *"No passage index for this folder — the study will read every file (about 3 minutes). [Build the index]"*.
- **Preferences ▸ Compare/Study:** *Use the passage index when it exists* (default on). Off forces a full read and says so.

**Honesty labels.** *"Stopped: 412 of 8,988 texts indexed. The rest were not read; a repeat that appears only in them will not be found until the index is finished."* · *"The study settings changed (passage length 8 → 12), so the whole index is being rebuilt. A cancelled rebuild banks nothing."* · *"Index built under: verbatim matching, 8-syllable passages, editorial spans stripped. Results below use those settings."* · *"N texts could not be converted and carry no signature — [list]."*

**Fit ratchet:** one button added to an existing Library ribbon group; no new group.

### Data
SQLite beside the existing library index.

```
PRAGMA user_version    = 1;
PRAGMA application_id  = low 32 bits of allcore::passageGeneration(options);
docs (id INTEGER PRIMARY KEY, path TEXT UNIQUE, mtime INTEGER, size INTEGER,
      syllables INTEGER, shingles INTEGER, not_converted INTEGER, folded INTEGER)
sigs (doc_id INTEGER, ix INTEGER, h INTEGER, PRIMARY KEY(doc_id, ix))
bands(band INTEGER, key INTEGER, doc_id INTEGER);  CREATE INDEX bands_ix ON bands(band, key);
meta (key TEXT PRIMARY KEY, value TEXT)   -- built_utc, option set, engine version
```
**Location:** `<dataRoot>/library/.passages.db` (+ `-wal`/`-shm`), a sibling of the existing `.index.db`, gitignored on the same footing. ~1 KB of signature per text; ~9 MB for the current 8,988-file library. **Signatures only — never text**, so the cache carries no HGM content and no gated material. (Whether it may be written into the shared OFFICIAL library tree is §11 Q3; the fallback is `<dataRoot>/library/studies/passages/cache/<hash-of-abs-path>.db`.)

**Provenance fields:** `application_id` = passage generation; `docs.mtime` + `docs.size` (the incremental key); `docs.not_converted`; `meta.built_utc` and the full option set.

### Engine
`core/include/allcore/passage_index.h` + `core/src/passage_index.cpp` (sqlite3 forward-declared, as `libindex.h` does). CTest `passage_index_smoke` on a temp directory, fixture-free.

```cpp
class PassageIndex {
 public:
  explicit PassageIndex(const std::string& db_path);
  struct UpdateStats { int added=0, updated=0, removed=0, unchanged=0,
                       write_failures=0, not_converted=0;
                       bool canceled=false, rebuilt=false; };
  UpdateStats update(const std::string& root, const passages::Options&,
                     const std::function<bool(int done,int total,const std::string& rel)>& progress = {});
  std::vector<std::string> candidatesFor(const std::vector<std::uint64_t>& signature, int limit) const;
  long long docCount() const; std::string builtUtc() const; std::uint64_t generation() const;
};
```

**Algorithm.** Incremental by `(path, mtime, size)` — the identical contract `LibraryIndex` documents. A file whose stamp is unchanged is skipped; a file that fails to write all its rows is **not** stamped, so the next pass retries it and the count is disclosed. On open, if `PRAGMA application_id` differs from `passageGeneration(options)` the index is dropped and rebuilt, and a cancelled rebuild deliberately withholds the new stamp so an interrupted rebuild cannot look complete. `candidatesFor` hashes the query signature into the same bands and returns every document sharing a band — **candidates only**; the caller still verifies with F2's exact seed-and-extend. No result is ever returned from the cache alone.

**Reuses:** F2's `shingleHashes` / `minhash` / `passageGeneration`; `allcore::LibraryIndex` as the structural model (same eligibility rules, same incremental key, same generation stamp, same cancel semantics, same write-failure disclosure); `LibraryPane::updateIndex`'s progress pattern and `indexResultHtml`'s wording; `core/tests/libindex_cancel_smoke.cpp` as the cancel-battery template.

### Acceptance tests
**CTest `passage_index_smoke`:**
1. Build over a temp dir of 5 texts → `docCount()==5`, `added==5`; a second update with nothing changed → `unchanged==5`, `added==0`.
2. Touch one file (new mtime and size) → `updated==1`, the rest unchanged.
3. Delete one file and update → `removed==1` and its band rows are gone (`candidatesFor` never returns it).
4. Change `Options.k` and reopen → `generation()` differs, `update()` sets `rebuilt==true`, every file re-indexed.
5. A progress callback returning false after 2 files → `canceled==true`, `docCount()==2`, and `generation()` is **not** stamped (the following update redoes everything).
6. `candidatesFor(signature of an indexed doc, 10)` contains that document; the signature of an unrelated document does not.
7. A file of only unconvertible lines → `not_converted` incremented, no `sigs` rows, never offered as a candidate.
8. A simulated write failure mid-file leaves that file unstamped and `write_failures == 1`.

**App `--selftest`:**
9. `LibraryPane`: **Update passage index…** returns immediately under `g_harnessRun` and opens no progress dialog.
10. `StudyPane/Passages`: with no `.passages.db` present the status line contains `No passage index for this folder`; with a stale one, it contains `changed since`.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A second SQLite file doubles the "is my index fresh" question | Both refreshes sit in the same Library CARE group; the Passages page states freshness where it is used |
| Changed hash constants silently invalidate every signature | The pinned-hash assertion in `passages_smoke` plus a version integer inside `passageGeneration` |
| A user who repeatedly stops a rebuild never gets an index | The message says so plainly (the libindex PERF-6 lesson) |
| The cache records every library path | It stays inside `<dataRoot>/library` and is not copied into the shared team updates folder without a decision (§11 Q3) |
| Disk growth | ~9 MB now, linear in library size; stated in Preferences ▸ File Locations |

**Effort: 12 h. Depends on: F2.**

---

## 5. F4 — Similarity studies (ranked candidate sources; TRACER export/import)

### Purpose and user story
> As a philologist holding a commentary, I want the texts on my shelf **ranked** by how much material they actually share with it — verified shared syllables, containment both ways, resemblance — so I can see that this commentary is 40% verbatim from that root text, then open the shared passages and read them against each other.

§9 asks for "ranked candidate sources with the shared passages highlighted". This is a real surface, not a by-product of pairwise repeats.

### UI
**Placement.** Study pane ▸ page 2 **Similarity**. It reuses the Passages page's result widget for the shared-passage list (one widget class, two hosts). Two sections: **Ranked candidates** and **Imported reuse sets**.

| Control | Label | What it does |
|---|---|---|
| Query | **This text** / **Choose…** | The query document |
| Bank | **Against:** *A folder… · The files selected in the Files pane · The HGM course corpus* | The candidate set; folder mode takes the glob and the eligibility rule |
| Order | **Order by:** *Verified shared syllables (default) · Share of my text · Share of the other text · Resemblance* | **Sorts only.** No number changes when you switch, and a selftest asserts it |
| Floor | **Shortest run to report: [8] syllables** · **Match: Exactly as written / Also matching spelling variants** | As F2 |
| Run | **Rank** · **Stop** | Pump/Stop |
| Export | **Export texts for TRACER…** | Writes the selected documents as UTF-8 wylie plus a manifest (path, sha1, syllables, exported-as) |
| Import | **Import reuse pairs…** | Reads a delimited export through the F7 table reader and opens a column-mapping dialog (doc A, span A, doc B, span B, score, method) plus a **required** free-text **Tool name** field |
| Report | **Save study…** (JSON) · **Save report…** (Markdown / CSV) | Through `saveOrWarn` |

**Ranked table columns:** Candidate · **verified shared syllables** · share of MY text · share of THEIR text · resemblance (k=8 shingles) · longest shared run · first cite. Row click fills the shared-passage list; **Open side by side** hands the pair to `g_compareTexts`, so `textdiff` is the highlighter and no second one is written.

**Honesty labels.**
- Column definitions printed **under the table, not in a tooltip**: *"verified shared syllables = the sum of the passages listed below, each re-checked in both texts. share of MY text = that sum ÷ the syllables of the query text. resemblance = |A∩B| ÷ |A∪B| over 8-syllable shingles (Broder 1997)."* No percentage is rendered without its denominator beside it.
- *"This ranks OVERLAP. It does not say which text borrowed from which — that is your judgment, not a measurement."*
- *"Verbatim and near-verbatim overlap only. This does not detect paraphrase or allusion; a low score is not evidence of independence."*
- A candidate that could not be read: *"not compared — binary file"*, never 0.0 similarity.
- Imported rows: *"EXTERNAL — imported from `<tool>` on `<date>` from `<file>` (sha256 …). Not computed here and not verified here."* They live in **their own table**, are never sorted with verified rows, and never enter our totals.
- Import refusal: *"No column named score was found. Nothing was imported."* / *"Row 14: the score cell is 'n/a'. The row was refused."* / *"`<doc>` has changed since it was exported (sha1 differs). Its rows were not joined."*
- Corpus candidates carry the EVIDENCE badge and the spine's own `release_version` and `n_corpus_segments` from `Spine::metaValue`, so the reference collection's identity is on screen.

**Fit ratchet:** page-local control row; no ribbon group.

### Data
- **Similarity study JSON** (`<dataRoot>/library/studies/similarity/<slug>.json`): `provenance`, `settings{k,minRun,norm,apparatus}`, `query{id,label,path,sha256,syllables,refused}`, `rows[]` (as the table, plus `not_compared_reason`), `spans[]`.
- **TRACER export**: `<dataRoot>/library/tracer/<studyName>-manifest.tsv` — `# doc_id\tpath\tsha1\tsyllables\texported_as` — with the exported texts beside it as `<studyName>/<doc_id>.txt` (UTF-8 wylie, one line per source line).
- **Imported set**: `<dataRoot>/library/studies/imported/<name>.json` — `meta{tier:"external", tool, imported_utc, source_file, source_sha256, column_mapping, rows_read, rows_refused}`, `refusals[]`, `pairs[]` with TRACER's own column names and values preserved verbatim.

Keyed by the **study name the user types**, not by a document base name, so `docprops::renameFileTo`'s sidecar-carrying does not apply and nothing orphans on a rename. Stated here so nobody later "fixes" it into the sidecar convention.

### Engine
Additions to `core/include/allcore/passages.h` + `core/src/passages.cpp` (covered by `passages_smoke`); the import/export is app-side in `app/study_pane.inc` using `allcore::table::readDelimited` from F7 — one delimited reader in the whole app.

```cpp
struct SimilarityRow { std::string docId, label; int verifiedSyllables, sharedShingles, longestRun;
                       double jaccard, containmentQuery, containmentDoc;
                       std::string longestCiteA, longestCiteB, notComparedReason; };
std::vector<SimilarityRow> rankAgainst(const Doc& query, const std::vector<Doc>& bank,
                                       const Options&, const std::function<bool(int,int)>& pump = {});
```

**Algorithm.** Resemblance **and** containment, both from Broder 1997 — containment `C(A,B) = |A∩B| / |A|` is asymmetric and is what actually answers "how much of this commentary is in that root text", which Jaccard cannot. LSH shortlists when the bank exceeds 32 documents; below that, direct pairwise intersection is cheaper and exact. **The ranking key is not an estimator:** rows are ordered by `verifiedSyllables`, the sum of re-verified runs. `longestRun` and its cites come from the same verified list. TRACER is not reimplemented in any form: its pipeline is another team's published system and this app imports its output as data.

**Reuses:** F2's shingles/MinHash/LSH/verification; `textdiff::diffLines`/`diffInline` via `g_compareTexts`; `Spine::corpusCourses` + `corpusWindow` for corpus candidates (bounded, cancellable, read count disclosed, following the Translator's Survey precedent); `FilesPane::selectedPathsIn`; F7's `readDelimited`; `saveOrWarn`.

### Acceptance tests
**CTest `passages_smoke` (similarity block):**
1. `rankAgainst(query = A, bank = {A, A-with-half-replaced, disjoint})` → ordered by `verifiedSyllables` descending, resemblances 1.0 / ≈0.33 / 0.0.
2. Containment is not Jaccard: query A, candidate = A + 40 further syllables → `containmentQuery == 1.0` while `jaccard < 0.6`.
3. **Self-consistency:** for every row, `verifiedSyllables` equals the sum of `a_syl_len` over the spans listed for that pair — no headline number that is not derivable from listed evidence.
4. Order-independence: shuffling the bank leaves the ranked ids and every score identical.
5. A candidate that cannot be read → `notComparedReason` non-empty and the row is not scored 0.0 silently.
6. `rankAgainst` with a 33-document bank (LSH path) and the same bank truncated to 32 (exact path) produce identical `verifiedSyllables` for the shared documents.

**App `--selftest` (`SimilarityPage::selfTest`):**
7. A three-file temp folder ranks the containing file first; switching **Order by** changes only the order — a named row's resemblance value is byte-identical before and after.
8. The column-definition text under the table contains the literal `÷ the syllables of the query text`.
9. **No directional wording:** a string scan of the rendered HTML finds none of `quotes`, `copied from`, `source of`. This check stays in the battery permanently.
10. Export writes a manifest whose row count equals the number of selected documents and whose sha1 values match `docprops::digest` of each file.
11. Import of a 3-row TSV with headers `fileA,posA,fileB,posB,score` yields 3 rows in the IMPORTED section, each carrying `EXTERNAL` and the typed tool name; the same TSV with the score column removed imports **0 rows** and sets a status containing `score`.
12. A 4th row whose score is `n/a` → `rows_read == 4`, `rows_refused == 1`, `pairs.size() == 3`, and the refusal names the line and the cell.
13. An imported row whose doc sha1 no longer matches the manifest renders *"the text has changed since it was exported"* and is not joined to a path.
14. **Isolation:** after an import, the ranked table's rows and counts are unchanged.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| "Similarity" invites a plagiarism reading | Philological wording throughout; the no-directional-wording scan is a permanent battery check |
| TRACER's exact output format is unknown to us | We ship the **export + manifest** so a real output file can exist, and a **generic column-mapping** importer that refuses rather than guesses. A named TRACER profile waits on Adam's sample (§11 Q2). The lane is never labelled "TRACER profile" until then |
| Imported claims could be read as ours | Separate table, EXTERNAL tier on every row, never sorted together, isolation test T14 |
| Corpus-wide ranking streams whole courses on the GUI thread | Pump/Stop, bounded reads, disclosed read count |
| Jaccard is length-sensitive; a short query against a long candidate scores low at full containment | Containment is shown for exactly this reason, and the definitions are visible without hovering |
| Exported text is converted wylie, not source ACIP | The manifest records both; positions map back through syllable offsets, and a position outside the recorded length is reported, not approximated |

**Effort: 20 h. Depends on: F2, F7 (reader), F1.**

---

## 6. F5 — Statistics study view

### Purpose and user story
> Before I start translating I want to know what this text will cost me: which terms recur enough that I must settle them first, which forms my glossary and the dictionary do not cover, and where the hard single-occurrence readings are. While proofreading an input-centre file I want the lines whose length is wrong for this text, because that is where a shad went missing. And when I am deciding a rendering I want the words that keep company with my term in **this** text.

### UI
**Placement.** Study pane ▸ page 0 **Statistics**. A control row, then a `QTabWidget` with nine tabs: **Summary · Words & syllables · Once only · Sentence lengths · Particles · Trends · Nearby words · Repeating phrases · Distinctive terms** — each one table a translator can explain.

**Control row (page-local).** **Count by:** *Syllable / Dictionary span* · **Merge spellings the search layer treats as one** (checkbox, off) · **Show top [100]** spin (10–1000, drives every capped table and its remainder line) · **Recount**.

| Tab | Contents and every control |
|---|---|
| **Summary** | Read-only table: syllables, types, hapax, type/token **and** standardised TTR, folios, lines, shad-delimited units (count, mean, median, min, max), particle counts, verse/prose verdict and meter from `analyzeVerse`, lines not converted, syllables refused for editorial marks. Clicking a row copies it |
| **Words & syllables** | Sortable table: Form · Tibetan · Count · % of tokens · **In my glossary** · **In the dictionary** (tier label only) · First cite (click → Overlay at that folio/line). Filter box; **Only forms the dictionary does not cover** (checkbox); **Copy as glossary starter (TSV)**. Double-click → `g_lookupQuery`; right-click → *Find in this text* (`g_goferQuery`) and *Concordance in the corpus* (the Draft's existing concordance) |
| **Once only** | The hapax table, same columns, plus **Send the top 8 to the Search pane** (an OR query through `GoferPane::searchFor`) |
| **Sentence lengths** | Bar rows (text bars of ■, no chart library) per syllable count; **Show units longer than [N] syllables** spin, defaulted to *meter + 3* for verse and 30 for prose; an outlier list of clickable rows — *"line 214 · 41 syllables — check for a missing shad"* — wired to `g_openAtLine`, with each line's `textDna` class beside it |
| **Particles** | Table: Function · Particle · Freestanding · Fused · Total; each row's **show occurrences** runs the Search pane over this text |
| **Trends** | **Term (ACIP or wylie):** line edit · **Across:** *Folios of this text · Files in this folder · Courses of the HGM corpus* · **Order files by:** *Name · Author · Recorded date* · **Go**. Table: bucket · hits · syllables (or segments) in the bucket · **rate per 1,000** · text bar. Rows click through to the Overlay or the Library |
| **Nearby words** *(Collocates)* | **Term:** · **Within [5] syllables** · **Do not cross shads** (on) · **Go**. Table: Form · Together · Its total here · Windows · **Log-likelihood** |
| **Repeating phrases** | **Length [3]–[8] syllables** · **At least [2] times** · **Go**. Table: Phrase · Syllables · Times · First cite; per-row **See every occurrence** opens the Passages page pre-loaded |
| **Distinctive terms** | **Against:** *The HGM course corpus · This folder* · **Go**. Table: Term · Here · In the background (df) · tf·idf |

**Honesty labels.**
- Every table: `ux::sourceBadge(ux::Epistemic::Machine)` and *"MACHINE — these are counts of the text as written. Nothing here is a reading."*
- Unit/fold strip, always visible: *"Counting SYLLABLES, surface forms, editorial spans stripped."* or *"Counting DICTIONARY SPANS (the overlay's longest-match words — a reference layer), spellings merged by the search layer (affix · da-drag · ba/pa · verb stem). That fold is lossy and belongs to searching, not to reading."* Plus, when the verb bank did not load: *"the verb-lemma bank was not read, so folding covered affixes and ba/pa only."*
- Type/token: *"types ÷ tokens = 0.31 over 8,431 tokens · standardised over 1,000-token windows = 0.38 — the raw ratio always falls as a text gets longer, so compare two texts on the standardised figure."* The formula is printed beside the number.
- Every capped table: *"Showing the top 100 of 3,412 forms; the 3,312 not shown all occur once."* — printed **even when the remainder is zero**.
- Page level, once: *"4 line(s) could not be converted and are left out of EVERY count on this page — lines 17, 92, 118, 240."* and the same for syllables refused for editorial marks.
- Nearby words: *"Log-likelihood G² after Dunning (1993); the 2×2 formula is printed below the table. A high score means the two forms occur together more often than chance — it is a frequency fact, not a claim about meaning."*
- Distinctive terms: *"score = (count here ÷ tokens here) × ln(N ÷ (1 + df)); df counted over the HGM course corpus, release `<release_version>`, N = `<n_corpus_segments>` segments, sha256 `<source_master_sha256>`; measured for the 200 most frequent bigrams in this text. These are **distinctive terms**, not keywords — frequency facts, not the text's topics. The text's own topics are its sa bcad outline."* A term whose df comes back **−1** (the corpus could not count it) is listed as *"df unknown — not scored"* with **no number**.
- Trends: the denominator is a column, so a rate is never shown without its base; empty buckets are emitted (an empty folio is a fact); a "no date recorded" bucket is listed **last and never hidden**; *"Counts across a folder come from the folder's search index, which stores search-folded text: `pa`, `pa'i` and `par` are counted together. Switch to 'exact (reads every file)' for verbatim counts."*; *"Trends counts one term. The Search pane's NEAR grammar is evaluated across per-node scans and cannot be grouped per file, so this box takes a term, not a query."*; *"N files in this folder are not indexed and were not counted."*
- *"No readability score is offered: there is no validated readability formula for Classical Tibetan and this tool will not invent one."*
- No `hgm_gloss` text anywhere (D2): the dictionary column shows coverage and tier, and the gloss is one click away in Lookup.

**Fit ratchet:** page-local control row plus a `QTabWidget`; no ribbon group added. All nine tables scroll horizontally in their own viewports.

### Data
Nothing persisted by default — every figure is recomputed from the file, so a stale cache can never be shown as current. Saved report: Markdown opening with `provenanceBlock`, then one section per tab; or CSV of the visible table (`form,count,percent` / `phrase,syllables,count,first_cite` / `form,total,left,right,loglikelihood` / `bucket,hits,size,rate_per_1000`), RFC-4180 quoted in the `apparatusCsv` style. Optional dated snapshot at `<dataRoot>/library/studies/statistics/<base>.json`, stamped "machine snapshot — not an authority".

Reads: the text; `<dataRoot>/library/glossaries/<completeBaseName>.tsv` via `GlossaryStore` for the coverage column; the spine (`build/hgm_spine_v27_2.db`) for dictionary coverage, corpus counts and df; `<folder>/.index.db` for Trends-by-file; `data/extracted/verb_lemmas.tsv`; `data/extracted/catalog_works.json` for the Trends date ordering (Reference tier).

Provenance: `engine="textstats/1"`, source sha256, script in, unit, fold + fold generation, apparatus mode, units read/refused with the line list, the df source and N for distinctive terms, Trends index-vs-exact mode, caps applied.

### Engine
`core/include/allcore/textstats.h` + `core/src/textstats.cpp`; one addition to `core/include/allcore/libindex.h` + `core/src/libindex.cpp`. CTest `textstats_smoke` (fixture-free); the libindex addition rides the existing `libindex_smoke` fixtures.

```cpp
enum class StatUnit { Syllable, DictionarySpan };
enum class StatFold { Surface, SearchFolded };
struct Options { StatUnit unit=StatUnit::Syllable; StatFold fold=StatFold::Surface;
                 int topN=100, collocateWindow=5, phraseMin=3, phraseMax=8, phraseMinCount=2;
                 bool respectBarriers=true; };
struct Count      { std::string form; int count; int firstSyllable; };
struct LengthBin  { int syllables, units; };
struct ParticleRow{ std::string wylie; const char* group; const char* function; int freestanding, fused; };
struct Basic { long long tokens; int types, hapax; double typeToken, sttr1000;
               int units; double unitMean; int unitMedian, unitMin, unitMax;
               std::map<int,int> unitHistogram; std::vector<int> outlierUnits;
               int particleTokens, fusedEndings; int folios, lines, shads;
               bool isVerse; int meter; };
struct Report { Provenance prov; Basic basic;
                std::vector<Count> frequency; int freqRemainder;
                std::vector<Count> hapaxList;  int hapaxRemainder;
                std::vector<LengthBin> unitLengths; std::vector<ParticleRow> particles; };

Report analyze(const std::vector<Syllable>&, const std::string& rawText, const Options&);
std::vector<int> unitSyllableCounts(const std::string& acip);   // EXTRACTED from verse.cpp

struct Collocate { std::string form; int count, left, right, windows, occurrencesInText;
                   double logLikelihood; };
std::vector<Collocate> collocates(const std::vector<Syllable>&, const std::string& term,
                                  const Options&, int* barrierTruncations);
struct TrendPoint { std::string bucket; int hits; long long size; };
std::vector<TrendPoint> trendByFolio(const std::vector<Syllable>&, const std::string& rawText,
                                     const std::string& term);
struct PhraseRow { std::string text; int n, count; std::vector<int> firstSyllable; };
std::vector<PhraseRow> phrases(const std::vector<Syllable>&, const Options&);
struct Distinctive { std::string form; int tf; long df; double tfidf; bool scored; std::string whyUnscored; };
std::vector<Distinctive> distinctiveBigrams(const std::vector<Syllable>&,
                                            const std::function<long(const std::string&)>& dfLookup,
                                            long backgroundDocs, int candidates, int topN);
// libindex addition — single term, exact-or-folded, uncapped, grouped in SQL:
std::vector<std::pair<std::string,int>> LibraryIndex::countTermByFile(
        const std::string& term, bool fold, int maxFiles = 5000, bool* truncated = nullptr) const;
```

**Algorithm — counting, with published definitions and nothing invented.**
- **Frequency / types / hapax:** exact multiset over F0's syllables (optionally through `searchFoldWylie`, memoised per distinct syllable). Hapax = types with count 1.
- **Type/token:** `types ÷ tokens`, reported with the token count **and** a standardised TTR averaged over consecutive fixed 1,000-token windows (the standardised-TTR convention of corpus linguistics; raw TTR is length-dependent — Herdan 1960 — which is why comparing two texts on it alone is a false claim).
- **Unit lengths:** `unitSyllableCounts` is **extracted from `core/src/verse.cpp:10-38`** (split at `\n \r , ; /`, then tokenize) and `analyzeVerse` is rewired to call it, so the histogram and the meter verdict provably come from one split. Mean, **lower** median and the integer histogram are computed directly; an outlier is a unit longer than the caller's threshold, defaulted from `analyzeVerse`'s dominant meter.
- **Particles:** only the verdicts of `allcore::classifyParticle` (`particles.h:26`) and `allcore::splitFusedEnding` (`particles.h:37`), grouped by `ParticleInfo::group`/`function`. Freestanding and fused are reported **separately**, never as one "density" number.
- **Phrases:** repeating n-grams for each n in range, computed with F2's `shingleForms` — **not a second shingler** — barrier-aware (an n-gram never crosses a shad), keeping only **closed** phrases (a shorter phrase wholly inside a longer one with the same count is dropped; the closed-frequent-sequence rule, Yan, Han & Afshar, CloSpan, 2003).
- **Collocates:** symmetric window scan stopping at `barrierAfter` when asked; `count`, `windows` and `occurrencesInText` are reported so any measure can be recomputed by the reader; the association measure is the log-likelihood ratio **G²** of Dunning, *Accurate methods for the statistics of surprise and coincidence*, Computational Linguistics 19(1), 1993, in the 2×2 contingency form, with the formula printed under the table.
- **Trends:** hits and bucket size per folio (from F0's folio walk), per file (via `countTermByFile`) or per course (`Spine::corpusCount`), so the UI shows a **rate** over unequal buckets. `countTermByFile` is one SQL statement over `lines_fts` joined to `files`, grouped by file — deliberately **single-term**, because the Gofer NEAR grammar is evaluated in C++ across per-node scans and cannot be grouped in SQL. The index stores search-folded text, so the folded mode folds the query term and says so; an exact mode reads the files with the pump.
- **Distinctive terms:** TF-IDF over syllable bigrams — inverse document frequency after Spärck Jones, *A statistical interpretation of term specificity and its application in retrieval*, Journal of Documentation 28(1), 1972, in the smoothed form `tf × ln(N/(1+df))` (weighting per Salton & Buckley, IP&M 1988). `df` comes from the caller; the app passes `Spine::corpusCount` (uncapped) for the 200 most frequent bigrams and prints that cap. `df < 0` → `scored=false`, `whyUnscored="the corpus could not count this term"`.
- **No readability index is computed and none is defined.**

**Reuses:** F0; `analyzeVerse` sharing the extracted split; `textDna`; `tokenizeDocument`/`buildOverlay` + `HeadwordIndex` (`lattice.h:45,49,79`) for the DictionarySpan unit and dictionary coverage — reusing the Overlay's index rather than building a second; `classifyParticle`/`splitFusedEnding`; `searchFoldWylie`; `GlossaryStore` (`glossary.h:18`); `Spine::corpusCount`/`corpusCourses`/`metaValue` (`spine.h:105,113,63`); `LibraryIndex`; F2's `shingleForms`; `app/textreport.h` wording; `docprops::textStatistics` shown on Summary **labelled as the raw-file figures**; KWIC is **not** rebuilt — rows hand off to `DraftPane::showConcordance` and `GoferPane::searchFor`.

### Acceptance tests
**CTest `textstats_smoke`** (fixture-free; fixture `F` = `"@012A\nSANGS RGYAS DANG BYANG CHUB SEMS DPA' THAMS CAD LA PHYAG 'TSAL LO,,\nCHOS THAMS CAD NI STONG PA NYID DO,,\n"` after `toEwtsDocument`):
1. `analyze` over `{chos,chos,chos,nyid,nyid,rgyal}` → tokens 6, types 3, hapax 1, `hapaxList[0].form == "rgyal"`, `typeToken == 0.5`.
2. On `F`: `basic.units == 2`, `unitHistogram == {8:1, 13:1}`, `unitMin == 8`, `unitMax == 13`, `unitMean == 10.5`, median 8 (lower median pinned), `folios == 1`, `isVerse == false`.
3. **One split:** for a four-line 7-syllable verse fixture, `unitLengths == {{7,4}}` and `(isVerse, meter)` equals exactly what `allcore::analyzeVerse` returns for the same string — both are called in the test.
4. `outlierUnits` on a 7-syllable-meter document containing one 41-syllable unit → exactly that unit's index.
5. **Delegation pinned:** `basic.particleTokens` equals the count the test recomputes by calling `classifyParticle` over the same token list; `fusedEndings` likewise via `splitFusedEnding`. A private particle table would fail.
6. `topN = 5` over 19 types → `frequency.size() == 5` and `freqRemainder == 14`; ties order by count desc then form asc.
7. Fold off: `pa` and `pa'i` are two types. Fold on: one type, count 2, `prov.norm` begins `search-fold/`.
8. **STTR is the comparable figure:** a 1,000-token text and a 10,000-token text drawn from the same 200-form vocabulary → `|ttr(X) − ttr(Y)| > 0.10` while `|sttr1000(X) − sttr1000(Y)| < 0.05`.
9. `phrases` on `"a b c d a b c e a b c"` with `phraseMin=3, phraseMinCount=2` → contains `{"a b c", 3}`, and `"a b"` is **not** separately listed at count 3 (closed-phrase rule); with `phraseMinCount=4` the result is empty.
10. An n-gram that would span a barrier is never produced.
11. `collocates` on `"x a y z a w"`, term `a`, window 1 → `{x:left 1, y:right 1, z:left 1, w:right 1}`; with `"x a , y"` the window stops at the barrier and `barrierTruncations == 1`; with `respectBarriers=false` the neighbour past the shad **is** counted.
12. `collocates` reports `occurrencesInText`: a form occurring 5 times overall and 3 times in windows → `count==3`, `occurrencesInText==5`.
13. **Log-likelihood:** for the pinned table a=10, b=90, c=100, d=9800, `logLikelihood` equals the value computed by hand from Dunning (1993) to within 1e-6.
14. `trendByFolio("@001A\nchos\n@001B\nchos chos\n", "chos")` → `{{"001A",1,…},{"001B",2,…}}` in folio order, **including a zero bucket** for a folio with no hits.
15. Trend buckets of sizes 100/200/300 with 2/2/2 hits → hits equal, sizes differ, so the caller can show 20.0 / 10.0 / 6.7 per 1,000.
16. `distinctiveBigrams` where `"a b"` has tf 3 and df 1000 and `"c d"` has tf 2 and df 2, N = 10000 → `"c d"` ranks first, and every scored value matches `tf/tokens × ln(N/(1+df))` recomputed in the test to 1e-9.
17. A `dfLookup` returning **−1** for one bigram → that row has `scored == false`, no score, and a non-empty `whyUnscored`.
18. `topN = 2` over 10 distinct forms → `frequency.size()==2` and `freqRemainder == 8`.
19. **`libindex_smoke` addition:** an index over three files where `chos` occurs 2, 0 and 5 times → `countTermByFile("chos", fold=false)` returns the two non-zero files with 2 and 5, `truncated == false`; `maxFiles = 1` → `truncated == true`.
20. **`libindex_smoke` fold mismatch:** a term whose folded and unfolded counts differ (`pa` vs `pa'i` present) → `fold=true` and `fold=false` return **different** counts, and the folded call's total matches what the index actually stores. This is the pin against reporting an index count as exact.

**App `--selftest` (`StatisticsPage::selfTest`):**
21. The Summary tab's syllables / folios / shads equal `docprops::textStatistics(F, true, …)` for the same fixture — Properties and Study can never diverge — **and** the tab labels which figure came from the raw-file path and which from the converted path.
22. Header on a source with 2 unconvertible lines contains `2 line(s) could not be converted`; on the bracket fixture it contains the refused-syllable sentence.
23. Merge-spellings on → every visible table caption contains the word `lossy`.
24. Every capped table's remainder line is present, including when the remainder is 0.
25. Trends with an empty term reports "type a term" rather than running; Trends-by-folder renders the folded-index sentence and the unindexed-file count; "Order files by ▸ Recorded date" renders a `no date recorded` bucket last, and with the catalogue bank absent it says the ordering is unavailable rather than silently falling back to name order.
26. The Distinctive terms tab's HTML contains the literal formula text and the spine's `release_version`; a stubbed df of −1 renders `df unknown — not scored`.
27. With no document open, **Use the front document** is disabled and its tooltip reads "no document is open".
28. `Save report…` returns false with a status line under `g_harnessRun`; no modal opens; the Words table's dictionary column contains no gloss text (string scan for tier labels only — the D2 guarantee).

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| ACIP-only measurement in the neighbours (`docprops` `[A-Za-z']+`, `analyzeVerse`'s split, `tokenizeDocument`) — Unicode would count zero | Everything routes through F0 first; Summary labels which figure came from which path and the header states the source script |
| Properties ▸ Statistics labels the syllable count "Words (segmenter)" | Study says **Syllables** and **Dictionary spans** and has **no** row called "Words"; the Properties row is raised as §11 Q7 and filed as an audit row, never silently renamed |
| Dictionary-span counting needs a `HeadwordIndex` build | Computed lazily on first switch, with a progress label and Stop; the Overlay's existing index is reused; Summary never blocks on it |
| Distinctive terms issue up to 200 uncapped corpus queries | Candidate cap stated on screen, busy cursor, Stop pump; a grouped FTS count is the first profiling target |
| Folded counting calls `stripAffixedParticlesWylie` per token | Memoised per distinct syllable; the memo is pinned by a call-count assertion |
| TTR, particle counts and "distinctive" invite over-reading | Every figure prints its formula and caveat; none is offered as a difficulty score. The Survey's ESTIMATE remains the only composite score |
| A cap read as a total (the recurring house defect) | The remainder line is a battery check, not a label |

**Effort: 34 h. Depends on: F0, F1, F2 (`shingleForms`).**

---

## 7. F6 — Entities in this text (people, works, dates; relationships)

### Purpose and user story
> At footnote time and again at index time I must list every person and every work this text names, with folio references, spelled the way the catalogue spells them. Today I do that with a pencil. I also need it while proofreading: a name the matcher cannot find is often a name the input centre mistyped.

### UI
**Placement.** Study pane ▸ page 3 **Entities**: a `QTreeWidget` grouped by kind over a co-occurrence table in a splitter. Also from the Library Maintenance menu, the Overlay REVIEW menu, and a link on the Apparatus pane ("Names and works in the open text →"), because footnote work starts there.

| Control | Label | What it does |
|---|---|---|
| Run | **Find entities** · **Stop** | Scans on demand, not on every keystroke |
| Filters | **People · Texts cited · Dates · Proper-noun hints · Together on a folio** | Checkboxes; unchecking hides a section and updates the counts line |
| Hints | **Include proper-noun hints from the SOAS lexicon (reference)** | Default **off**; adds an untyped MACHINE-tier list of `n.prop` tokens that matched no bank |
| Row actions | **Person card →** (`g_openAuthorByPid`) · **Open the cited text** · **Where else is this named** (`g_goferQuery`) · every folio cite links into the Overlay | |
| Places | **Look up "`<selected form>`" on THL Places** | Opens the browser on a user-initiated link; enabled only when a form is selected |
| **Deliverables** | **Save as footnote list (Markdown)** · **Copy as index (TSV)** | The two artefacts a published volume needs |
| Report | **Report a name the index does not know…** | Opens the existing proposal channel with kind `CatalogIdentity` pre-filled (evidence = the surface form and its cites) |

**Honesty labels.**
- People and works: `ux::Epistemic::Reference` + *"catalogue identity"* + the matcher's own evidence string (*"exact alias in the person index (P64)"*, *"alias with an affixed particle stripped"*).
- Homonyms: *"Two people are recorded under this name — P1583 and P4321. The app does not choose between them; you do."* Every candidate listed, **none preselected**, key empty until a human picks.
- *"Names are matched by spelling only. The phonetic matchers used in the Library search are not used here, because at the scale of a whole text they invent people."*
- Places, always rendered even when empty: *"No place gazetteer is installed and none of our data licences one. This tool therefore claims no place. Below are proper nouns the SOAS classical lexicon tags `n.prop` — REFERENCE hints, not places, with no person/place distinction — each with a link you can follow to THL Places yourself."*
- Works: *"The published English title is shown as recorded in the catalogue — REFERENCE. The app does not translate a title."* (no "translate this title" affordance appears).
- Dates: *"Candidate years: 1386, 1446, 1506, … An element–animal pair repeats every sixty years, so every year in range is listed; a rab byung number in the text would narrow it. The Tibetan year begins at Losar, so it straddles two western years."* plus *"Day-level conversion is not supported for historical colophons: the Kālacakra karaṇa port covers 2009 onward and the Phugpa port is proven for 2015–2035."*
- Missing bank: *"The person index could not be read at `<path>`. This page cannot list names. This is NOT 'no names found.'"* — the lazy-load flag is set **only after a successful load**.
- Bank census in the header: *"Person index: 462 people, 596 aliases (36 people carry no recorded name and cannot be matched). Title bank: 1,842 titles. N alias(es) and M title(s) were too short to search without false matches and were not used."*
- Co-occurrence: *"These two are named on the same folio. That is a fact about the page — and the folio is an artefact of the printed edition — not a claim about a relationship."*

**Fit ratchet:** page-local control row; splitter tables scroll in their own viewports.

### Data
Reads only; writes only what the translator asks for.
- **Footnote list (Markdown):** one section per kind; each entry `Surface form — Display name (dates) [pid] · @012B.3, @034A.1` with the evidence string.
- **Index (TSV):** `# kind\tsurface\tkey\tdisplay\ttier\tevidence\tcites` then one row per hit, tabs escaped the house way.
- Optional dated snapshot at `<dataRoot>/library/studies/entities/<base>.json`, stamped "machine snapshot — not an authority".

**Reads:** `data/extracted/author_index.json`, `acip_person_links.json`, `persons_bdrc.json`, `tol_links.json`, `catalog_works.json`, `catalog_titles.json`, `work_subjects.tsv`, `data/soas_pos/classical-lexicon.txt` — all through the app's **existing** loaders (`LibraryPane::buildAuthorIndex`, exposed via `g_personBank` / `g_titleBank`; `PosLexicon`). Nothing under `data/extracted` is written.

**Provenance:** per hit `tier` + `evidence` + source bank; per bank name, absolute path, sha256, rows loaded, rows unusable; `matching_tiers_used = "orthographic only (exact · contains · spacing) — phonetic tiers are excluded from bulk text scanning"`; `alternatives[]` and `candidate_years[]` never collapsed; per-bank loaded flags so "not found" can be told from "not looked for".

### Engine
`core/include/allcore/entities.h` + `core/src/entities.cpp`; plus a two-line export of the element/animal name tables out of `core/src/tibcal.cpp`'s anonymous namespace into `core/include/allcore/tibcal.h`. CTest `entities_smoke`, shipping **its own** fixture banks (no dependence on the gitignored `data/extracted` or `library/`).

```cpp
enum class Kind { Person, Work, Date, ProperNoun };   // deliberately NO Place member
enum class Tier { Reference, Machine };
struct Occurrence { int begSyl, endSyl; std::string cite, folio; int line; };
struct Entity { Kind kind; Tier tier; std::string surface, key, display, evidence;
                std::vector<Occurrence> where; std::vector<std::string> alternatives;
                std::vector<int> years; bool occurrencesTruncated = false; };
struct PersonBank { struct P { std::string pid, display; std::vector<std::string> aliases; };
                    std::vector<P> people; };
struct TitleBank  { struct T { std::string key, tibetan, display; }; std::vector<T> titles; };
struct Options { int minAliasSyllables = 2, minTitleSyllables = 3;
                 bool allowAffixOnLastSyllable = true; int maxOccurrencesPerEntity = 200; };
struct Report { Provenance prov; std::vector<Entity> entities;
                bool peopleBankMissing, titleBankMissing; int bankPeople, bankTitles, refusedPatterns;
                std::vector<std::tuple<std::string,std::string,std::string,int>> coOccurrence; };
Report scan(const std::vector<Syllable>&, const std::string& rawText,
            const PersonBank&, const TitleBank&, const Options&);
struct DateHit { int begSyl, endSyl; std::string element, animal; int rabjung; std::vector<int> years; };
std::vector<DateHit> scanYearPhrases(const std::vector<Syllable>&);
const std::vector<std::string>& elementNamesWylie();   // exported from tibcal.cpp, not retyped
const std::vector<std::string>& animalNamesWylie();
```

**Algorithm.** **Aho–Corasick** multi-pattern matching (Aho & Corasick, *Efficient string matching: an aid to bibliographic search*, CACM 18(6), 1975) over a **syllable** alphabet: every alias and every banked title is canonicalised once into a syllable sequence and the goto/failure/output automaton is built once; the text is scanned in one linear pass, reporting the **longest** match at each position. Because the alphabet is syllables, the substring false positives a naive contains-match produces (`pa` inside `pad ma`) are structurally impossible. This is the correct direction: every existing matcher is query→bank, and calling `matchPeople` per token would mean 596 aliases × `pronounce()` per token.

Canonicalisation is identical on both sides — `allcore::authorCanonical` (`authorsearch.h:80`) for aliases, `allcore::normalizeAcipTitle` for titles, then F0's tokenizer — the same guarantee that makes `detectQuotations` exact. **Exactly one grammar-backed tolerance:** the final syllable of a pattern also matches a text syllable whose `splitFusedEnding` base equals it (so `rje tsong kha pa` matches `… TZONG KHA PA'I …`), decided by the particle engine, never by a private suffix list, and labelled *"affixed particle stripped"*.

**No phonetic matching:** the Phonetic and PhoneticNear tiers are documented guesses; a bulk scan is exactly where guesses become a list, so those tiers are not reachable from this API at all. **Dates:** adjacent element+animal syllable pairs from the exported tibcal tables, optionally preceded by `rab byung <n>`, handed to `allcore::yearsFor` (`tibcal.h:42`) — the scanner performs **no** calendar arithmetic of its own. **Places:** none — the enum has no `Place` value, so the API cannot make the claim. **Proper nouns (optional):** `PosLexicon::tags`, reported when the tag set contains `n.prop`, labelled an untyped hint. **Cites** come from F0. **Co-occurrence:** hits bucketed by folio; every unordered pair sharing a bucket emitted with its count — plain counting, rendered as a **table**, not a graph, because a graph implies edge weights we have not defined.

**Reuses:** `PersonRef` / `authorCanonical` / the Exact–Contains–Spacing tier vocabulary and evidence strings verbatim, so Study and the Library search speak one language; `normalizeAcipTitle` / `extractAcipTitle` / `findColophonCandidates` — the colophon's self-naming is its own section at tier Evidence; `normalizeCatalogKey` / `baseCatalogKey` as the canonical work key (not an ad-hoc regex, which would merge `S5002-1` and `S5002-2`); `yearsFor` + the exported tables; `PosLexicon`; `Subjects::forWork` / `isVinaya` for the genre header (with "No subject recorded" a real answer — subjects cover 1,906 of 5,607 keys); the app's existing bank loaders, `personHtml`, tier skins, `g_openAuthorByPid`, the THL Places URL builder.

### Acceptance tests
**CTest `entities_smoke`** (own fixtures):
1. Aliases `{"tsong kha pa","rje tsong kha pa"}` against `"… RJE TZONG KHA PA GSUNG"` → exactly one Person hit covering four syllables, tier exact, evidence naming the **four**-syllable alias.
2. The same against `"… RJE TZONG KHA PA'I GSUNG"` → one hit with evidence containing `affix`; with `allowAffixOnLastSyllable=false` → zero; against `"… TZONG KHA PAN …"` → **no** hit.
3. **Syllable-boundary safety:** pattern `{"pa"}` against `"PAD MA"` → no hit; against `"BDAG PA"` → one hit.
4. Two people sharing an alias → **one** Entity with `alternatives.size()==2`, `key` empty, evidence naming both pids; `entities.size()==1`.
5. A person with empty aliases contributes nothing (the 36-of-462 case); a one-syllable alias is not indexed and increments `refusedPatterns`.
6. Title bank `{("S702","shes rab kyi pha rol tu phyin pa","The Perfection of Wisdom")}` present in the text → one Work hit whose `display` is **byte-identical** to the bank's English; a text containing only `lam rim` → 0 Work hits.
7. Nested patterns: bank has `rgyal tshab` and `rgyal tshab rje`; text has the longer → the longer matches and the shorter produces no second overlapping hit at the same start.
8. `scanYearPhrases("ME STAG")` → one hit whose `years` equals `allcore::yearsFor("me","stag")` **recomputed in the test** and whose size > 1; `"RAB BYUNG 7 ME STAG"` → exactly one year.
9. Every hit's cite equals `allcore::citeAt(text, hit.begByte)` recomputed in the test — one folio walk, no second opinion.
10. Two entities on the same folio → `coOccurrence` contains that pair exactly once; different folios → no pair.
11. **Structural honesty:** a compile-time check that `Kind` has no `Place` member, and an assertion that no emitted `display` contains the word "place".
12. Empty banks → `peopleBankMissing == true`, `titleBankMissing == true`, `entities` empty.
13. `maxOccurrencesPerEntity = 2` with 5 occurrences → `where.size()==2` and `occurrencesTruncated == true`.

**App `--selftest` (`EntitiesPage::selfTest`):**
14. With the real banks absent, the page renders the literal `This is NOT "no names found."` and **not** a bare "no names found"; with a stub bank via `g_personBank`, one row appears with a REFERENCE badge and its evidence string.
15. A homonym row's HTML contains `The app does not choose between them; you do.` and no candidate is preselected.
16. The Places section renders even with zero hints and its HTML contains `claims no place`; the proper-noun checkbox is **off** by default and its rows carry the MACHINE badge.
17. The day-level-unsupported sentence appears whenever a Date hit is rendered.
18. No control, object name or rendered string on the page mentions `phonetic`.
19. **Copy as index (TSV)** produces a body whose first line begins `# kind\tsurface\tkey` and whose row count equals the visible hit count; **Save as footnote list** writes through `saveOrWarn` and its header lists every bank with sha256 and row count.
20. `Save…` returns false with a status line under `g_harnessRun`; no modal opens.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Banks are gitignored and absent on ordinary installs — an empty list reading as "names none" is the highest-consequence honesty risk here | Distinct message per bank; the lazy-load flag set only after a successful load; pinned by selftest T14 |
| OCR-grade alias spellings (`GR AGS`) are simply missed | The banner says the list is of names **the index knows**, not names in the text, and prints the bank census |
| Homonym density in Tibetan names | All candidates listed, none resolved; a future "pick the most likely" convenience must be refused |
| Places are a visible hole a user will ask about | The wording makes the reason the feature; the enum makes the refusal structural |
| `n.prop` distinguishes neither person nor place | Rendered under "Proper nouns (reference)" with the places sentence above it |
| Automaton rebuild cost as the title bank grows | Built once per bank load and cached, not per scan |
| Short titles over-match | `minTitleSyllables = 3`, stated on screen |
| Exporting the tibcal tables widens allcore's surface | `const` and documented as "the same names `yearsFor` accepts"; additions purely additive so `phugpa`/`tibcal_day` smokes are untouched |

**Effort: 26 h. Depends on: F0, F1.**

---

## 8. F7 — Table Compare (Compare pane, page 3) + `--table-compare`

### Purpose and user story
> Two moments. (a) A new dictionary release arrives and I need to know which of the terms I have already used changed — not a 63 MB text diff, a list of headwords with the old and new values side by side. (b) My per-text glossary and the team's have drifted; I want the rows where we disagree, and I want the app to refuse to guess when the same headword appears twice.

### UI
**Placement.** A fourth page in the existing Compare pane: one more `pageBtn` beside *Text Compare · Folder Compare · Three-Way Merge*, the page class in `app/table_compare.inc`, constructed by `installTableComparePage(comparePane)` beside `installComparePages` (`app/compare_pane.inc:1340`) so `main.cpp`'s selftest block can sum its `selfTest` exactly as it sums `cmpPages.folders` and `cmpPages.three` (`app/main.cpp:41810-41811`). **Page-local control rows** — the Compare ribbon gains **nothing**, so the ratchet is untouched. Also from Tools ▸ Compare ▸ *Table Compare…*, from the Overlay's Glossary Workbench (*Compare with another glossary…*), and after Library ▸ Import Data Release (*Compare with the release I have…*).

| Control | Label | What it does |
|---|---|---|
| Sides | **Left / Right** + **Browse…** | Filters `*.tsv *.csv *.txt *.json` |
| Preset | **Preset:** | Fills format, implied column names, key, list columns, pinned provenance columns and the tier banner. Entries: *Per-text glossary (TSV, key wylie)* · *Release dictionary TSV (key wylie)* · *Alignment evidence JSON (key headword+eng)* · *Alignment links JSON (key course+page+id)* · *Reverse index JSON* · *Proposals TSV (key id)* · *Build manifest JSON (key path)* · *Folder snapshot JSON (key path)* · *Registrar CSV (key = detected number column via `normalizeCatalogKey`)* · *Worksheet CSV (key "ACIP Number")* · *Generic* |
| Format | **Format:** *Detect · Comma-separated · Tab-separated* | Detection: a tab in the header wins, else comma |
| Key | **Key column:** + **second key column:** | Composite keys supported; changing either recomputes and re-runs |
| Compare set | **Compare columns…** | Checkable list of shared columns. **Provenance columns carry a lock glyph and cannot be unticked** |
| Lists | **Treat as lists (split on "; "):** | A list cell is diffed as a multiset |
| Rules | *(the pane's own RULES ribbon)* | Cell equality is `textdiff::normalizeLine` under the Compare pane's existing options — **no second rule set**; a label states which rules are on |
| Run | **Compare** · **Swap sides** | |
| Filter | **Show ▸** *All · Changed · Left only · Right only · **Ambiguous keys** · Identical* | |
| Result | Tree: Status · Key · fields changed → per-field rows (Column · Left · Right) with in-cell marks from `diffInline` | **Open in Lookup** on a wylie key; **Compare these two rows as text** → `g_compareTexts` |
| Report | **Save report…** (CSV / Markdown / HTML, routed by `selectedNameFilter`) · **Copy summary** · **Save session… / Open session…** | Sessions land in the existing `library/compare_sessions/` with `mode:"table"` |
| CLI | `--table-compare A B <key> [report.csv]` | Prints the summary; exit 0 identical, 1 different, 2 unreadable. **Without a key it exits 2 and prints the available column names** |

**Honesty labels.**
- Header, both sides: path, byte size, sha256, mtime, and the file's own identity where it has one (release master `meta.version`/`meta.date`; the spine's `release_version` and `source_master_sha256`; the alignment file's `meta.tier`). A comparison whose sides cannot both be identified says so.
- Summary: *"105,634 rows left · 105,701 right · 67 only on the right · 0 only on the left · 412 changed · 56 keys ambiguous. Columns only on the left: `tibetan_source`."* Three separate numbers — rows **read**, rows **compared**, rows **refused** — with the refusal lines listed on demand.
- *"Rows whose key occurs more than once are listed as AMBIGUOUS KEY and are never paired by position — pairing them would be a guess. Choose a second key column, or resolve them in the source."*
- Release-TSV banner (mandatory whenever either side is the TSV): *"The release TSV does not carry `tibetan_source`, `wylie_variants` or the corpus-occurrence detail, and `hgm_evidence` is reduced to a tier string. This comparison is lossy — compare the JSON master for a provenance-faithful answer."*
- Alignment banner, carried **verbatim from the file's own meta**: *"TENTATIVE (machine-matched, unreviewed)"* plus *"This compares an evidence layer. Nothing here is a change to an HGM gloss."*
- *"Columns present on only one side are listed above; their cells are never reported as changed-to-empty."*
- *"N row(s) had a cell count that does not match the header. They were refused and are listed with their line numbers — nothing was padded silently."*
- *"Column names were not read from the file; they come from the preset."*
- *"The spine's `entries.id` is a per-release ordinal and is not offered as a key."* / *"`COURSE:seq` is positional and is renumbered by any corpus insertion; use a content key."*
- *"Nothing is written. This page compares; it does not merge."*

**Fit ratchet:** zero ribbon growth; the result tree scrolls horizontally.

### Data
Reads existing files in place; writes reports and sessions only.
- Report CSV: `status,key,column,left,right`, RFC-4180 quoted in the `apparatusCsv` style. Markdown: `provenanceBlock` + both identities + key + rule set + the ambiguous-key list, then a section per status.
- Session JSON in `<dataRoot>/library/compare_sessions/<safe>.json`: the existing `{mode,left,right,options,show}` shape plus `mode:"table"`, `preset`, `key[]`, `compareColumns[]`, `listColumns[]`.
- Inputs: `<dataRoot>/library/glossaries/<base>.tsv`, `data/hgm_dictionary_v27_2.tsv`, `data/alignment/*.json`, `dist/stage/BUILD_MANIFEST.json`, `*.snapshot.json`, `<proposalsDir>/proposals.tsv`, any registrar CSV/TSV.

**Provenance:** `engine="tablediff/1"`; both sides' path, size, sha256, mtime and file identity; preset name and whether auto-detected or overridden; key columns and case sensitivity; rows read / compared / refused with the refusal list; duplicate-key group counts per side; pinned provenance columns present or absent per side.

### Engine
`core/include/allcore/table.h` + `core/src/table.cpp` (the reader — also used by F4's import lane) and `core/include/allcore/tablediff.h` + `core/src/tablediff.cpp` (the differ). CTest `table_smoke` and `tablediff_smoke`, both fixture-free. JSON is adapted **app-side** in `app/table_compare.inc`, so `allcore` gains no JSON parser.

```cpp
enum class Format { Auto, Csv, Tsv };
struct Row   { std::vector<std::string> cells; int sourceLine; };
struct Table { std::vector<std::string> columns; std::vector<Row> rows;
               std::vector<std::string> comments; char delimiter='\t';
               bool headerFromFile=false; int refusedRows=0;
               std::vector<std::string> refusals; };
Table readDelimited(const std::string& text, Format = Format::Auto,
                    const std::vector<std::string>& impliedColumns = {});
std::string writeCsv(const Table&);

enum class RowStatus { Same, Changed, LeftOnly, RightOnly, AmbiguousKey };
struct FieldDiff { std::string column, left, right;
                   std::vector<std::string> addedItems, removedItems; };
struct RowDiff   { RowStatus status; std::string key; int leftRow=-1, rightRow=-1;
                   int leftDupes=1, rightDupes=1; std::vector<FieldDiff> fields; };
struct DiffOptions { std::vector<std::string> keyColumns, compareColumns,
                                              listColumns, pinnedColumns;
                     textdiff::Options cellRules; bool keyCaseSensitive = true;
                     std::string listSeparator = "; "; };
struct DiffResult { Provenance leftProv, rightProv; std::vector<RowDiff> rows;
                    std::vector<std::string> columns, leftOnlyColumns, rightOnlyColumns;
                    int same=0, changed=0, leftOnly=0, rightOnly=0, ambiguous=0;
                    int leftRows=0, rightRows=0, leftDupGroups=0, rightDupGroups=0;
                    std::vector<std::string> refusals; };
DiffResult  diffTables(const Table& a, const Table& b, const DiffOptions&);
std::string diffCsv(const DiffResult&);
std::string diffMarkdown(const DiffResult&, const std::string& aName, const std::string& bName);
const std::vector<TableProfile>& tableProfiles();
const TableProfile* detectProfile(const std::string& path, const Table& sampled);
```

**Algorithm.** A **relational full outer join by key** — the hash join with anti-join wings, the record-level form of the file-level classification `FolderComparePage::compareWithSnapshot` already performs. Build a multimap key → row indices per side; take the key union; classify. **Multiplicity is checked first:** a key occurring more than once on either side becomes one `AmbiguousKey` row with both group sizes and **no** field diff — pairing it would be a guess, and the release master has 56 duplicated `wylie` values in 105,634 entries. Column sets are intersected by **name** (union order: left order then right-only appended) and the drift reported; a column present on one side never produces a "changed to empty". Cell equality is delegated to `textdiff::normalizeLine` under the caller's options, so the pane's existing RULES ribbon drives it; in-cell marks come from `textdiff::diffInline`. A declared list column is split and compared as a **multiset**, so a reordered gloss list is Same and an added gloss is Changed with the item named. `pinnedColumns` are unioned into `compareColumns` unconditionally.

`readDelimited` **lifts** the quoted-field splitter now trapped in `core/src/catalog_register.cpp:19-51`'s anonymous namespace (`CatalogRegister` is rewired to call it; `register_smoke` is the regression proof): header-driven delimiter detection, RFC-4180 double-quote handling, trailing `\r` stripped, `#` lines kept as comments, and a row whose cell count does not match the header **refused** with its line number and raw text — never padded or truncated. `impliedColumns` supplies names for the headerless `#`-commented house TSVs, so no data row is eaten as a header. `AbbrTable::load`'s quote-blind comma split is **not** inherited; the abbreviations preset is withheld until that file's format is settled.

**Reuses:** `catalog_register`'s `splitRow`; `textdiff::normalizeLine` / `Options` / `diffInline` / `Span`; `FolderComparePage`'s key-union classification, Show ▸ combo, table widget and `report(kind)` shape; `tree_diff`'s status vocabulary; `GlossaryStore`; `normalizeCatalogKey`; `Spine::metaValue`; `ComparePane::addPage`/`showPage`/`statusLabel`/`options` and its session/recent idiom; `worksheet.cpp`'s cell quoting.

### Acceptance tests
**CTest `table_smoke`:**
1. `readDelimited("a,b,c\n1,\"b,with,commas\",\"he said \"\"hi\"\"\"\n")` → one row, cells `{"1","b,with,commas","he said \"hi\""}`.
2. A header containing both a tab and a comma → tab wins, and the choice is recorded.
3. CRLF input → the last cell has no trailing `\r` and the cell count matches the header.
4. A `#`-commented two-column TSV with `impliedColumns {"wylie","gloss"}` → `headerFromFile == false`, columns are the implied pair, comments preserved.
5. A row with fewer cells than the header → `refusedRows == 1`, the row is **not** in `rows`, and `refusals[0]` carries the line number **and** the raw text.
6. `writeCsv` round-trips a cell containing a comma, a double quote and a newline.

**CTest `tablediff_smoke`:**
7. Left `{(a,1),(b,2)}`, right `{(a,1),(b,3),(c,4)}` keyed on column 0 → `same==1`, `changed==1` with one `FieldDiff{column 1,"2","3"}`, `rightOnly==1`, `leftOnly==0`.
8. Left has key `k` twice with different values, right once → one `RowDiff`, `AmbiguousKey`, `leftDupes==2`, `rightDupes==1`, `fields.empty()`, `ambiguous==1`, `leftDupGroups==1`, and **no** row of A paired with B's.
9. **Composite key rescue:** the same fixture keyed on `{col0,col1}` → two paired records, `ambiguous==0`.
10. A column present only on the left → in `leftOnlyColumns`, and **no** `FieldDiff` for any row.
11. List columns: `"a; b"` vs `"b; a"` → Same; `"a; b"` vs `"a; b; c"` → Changed with `addedItems == {"c"}`; with `listColumns` empty the first pair is Changed.
12. `cellRules.ignoreCase` makes `"Chos"` vs `"chos"` Same; without it, Changed.
13. Key case sensitivity: `"paN chen"` vs `"pan chen"` are two records when `keyCaseSensitive==true`, one AmbiguousKey group when false.
14. **Pinned provenance:** with `compareColumns={gloss}` on tables that also carry `hgm_evidence`, a difference in `hgm_evidence` still appears as a `FieldDiff`.
15. **Refusal over guess:** `diffTables` with empty `keyColumns` → `rows.empty()` and `refusals[0] == "no key column chosen"` **plus the available column names**; never a fallback to column 0.
16. `diffCsv`'s first line is exactly `status,key,column,left,right` and a value containing a comma is quoted.

**App `--selftest` (`TableComparePage::selfTest`):**
17. Two temp TSVs → visible counts match the engine's, including one `Changed` and one `AMBIGUOUS KEY` row, and the summary contains `never paired`.
18. The glossary preset selects the key and names the implied columns; the release preset sets the key to `wylie` and shows the lossy banner; an alignment preset renders `TENTATIVE (machine-matched, unreviewed)`.
19. The alignment-evidence adapter over a two-headword stub JSON produces a Table with the expected columns and 2 rows.
20. A provenance column's picker item is **disabled**.
21. `Show ▸ Changed only` leaves exactly the changed rows visible; a session round-trips (preset, key, show mode).
22. **No file is written by a compare:** both inputs' mtimes are unchanged afterwards.
23. `Save report…` returns false with a status line under `g_harnessRun`; no modal opens; the temp directory is removed.
24. CLI `--table-compare A.tsv B.tsv wylie report.csv` exits 0 / 1 / 2 as specified; **without a key it exits 2 and stdout lists the column names**.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| `wylie` is not unique in the release master (56 duplicates) | `AmbiguousKey`, pinned in the battery; the composite key is the documented escape. A "pair duplicates by position" convenience must be refused |
| `entries.id` is a per-release ordinal; `COURSE:seq` is positional | Neither offered as a key; a warning when an id-like column is chosen across differing release identities |
| The TSV hides provenance the JSON master carries | The lossy banner is mandatory — a rule-4 requirement |
| `data/alignment/*` is gated in `PAYLOAD_MANIFEST.txt` | The file's own TENTATIVE banner carried verbatim; shipped-build access is §11 Q5 |
| Two 105k-row tables in memory | A per-side row cap (default 250,000, above the master's 105,634 so the intended use is not on the refusal boundary) with an honest message; the Qt-free engine can be drilled on the real files under a `fixture`-labelled CTest |
| Headerless house TSVs | `impliedColumns` per preset; Auto-detect says when column names were not read from the file |
| `AbbrTable`'s quote-blind comma split | Not inherited; the abbreviations preset withheld and the defect reported |
| The Save dialog routes by suffix and the apparatus owns `.md` | Table reports route by `selectedNameFilter` |

**Effort: 30 h. Depends on: F0 (only for `Provenance`); independent of the Study pane.**

---

## 9. Sequencing

1. **F0 — `textspan` + `provenance` + the three rewires, alone.** Proof: the **existing** `quotation_smoke` and `textdiff_smoke` stay green, plus the new `textspan_smoke` including the apparatus cases and the cite cross-check. Its own commit, so no apparatus-cite change hides inside a feature diff.
2. **Gate work, before any new surface exists.** Extend R3 and G2 to `app/*.inc`, review the existing `.inc` sites once, install both baselines in the same commit, run the mutation check, record it, flag the bump to Adam.
3. **F7 — Table Compare, in parallel from here.** It depends on nothing in this cluster but `Provenance`, lands inside a pane and an audit that already exist, closes the oldest filed item in `docs/COMPARE_SUITE_PLAN.md`, and lifts `splitRow` under the existing `register_smoke`. It gives Adam a visible Batch 5 result while the study engines are still being battery-proved.
4. **F1 — Study pane shell**: four empty pages, ribbon, status line, hooks, menus, CLI flags, selfTest skeleton. Proves wiring, fit and ratchet before any algorithm is written.
5. **F2 — Repeated Passages.** Engine and `passages_smoke` first, then the page. **This precedes Statistics** because Statistics' Phrases tab calls `passages::shingleForms`; no placeholder n-gram counter is ever written.
6. **F3 — Passage index**, immediately after F2, before anyone gets used to the slow folder mode. Acceptance: the status line stops saying "about 3 minutes".
7. **F5 — Statistics.** Its first commit extracts `unitSyllableCounts` from `verse.cpp` and rewires `analyzeVerse`, so the histogram and the meter verdict share one split before any histogram renders.
8. **F4 — Similarity.** Additions to `passages.cpp` plus a page reusing the Passages result widget, the Compare pane and F7's reader. TRACER export first; the importer last, and only as the generic column-mapper until Adam supplies a sample.
9. **F6 — Entities, last.** Most external banks, and the only feature needing new accessors on an existing pane. Its first commit is the shared bank loader lifted out of `LibraryPane`, proven by the existing Library selftests still passing.

**Every unit ships the house way:** a fixture-free core smoke registered in `core/CMakeLists.txt`; the pane/page `selfTest` summed at `app/main.cpp:41796-41818`; the ribbon ratchet and the 800 px fit gate re-measured; an audit row (`docs/release_audit/StudyPane.md`, added rows in `ComparePane.md`); a `docs/digests/LOG.md` line; a TODO tick; and for the three new surfaces the **design-critique + accessibility pass before Adam sees them**, with findings filed to `docs/FINDINGS.md` with dispositions. Press only when Adam asks.

Sub-batch boundaries: **5a** = steps 1–4 ≈ 60 h; **5b** = steps 5–7 ≈ 74 h; **5c** = steps 8–9 ≈ 46 h.

---

## 10. Deliberately omitted

| Omitted | Why |
|---|---|
| **Word cloud (Voyant Cirrus)** | §10 SKIP — decoration. A cloud encodes frequency as area, which no reader can compare, and it cannot be sorted, exported or cited. The frequency table does the same job and every row links to a folio |
| **Readability formulas for Tibetan** | §10 SKIP — no validated formula exists and inventing one would be a guess presented as a measurement. The Survey's difficulty ESTIMATE, which prints its formula, remains the only composite score |
| **Sentiment analysis** | §11 SKIP — meaningless for these texts |
| **Machine summaries and "chat with the text"** | §11 SKIP and rule 1 — a summary is composed English. The Analysis pane remains the one AI surface, key-gated and labelled; Study is deterministic and shares no code with it |
| **Paraphrase/allusion detection; any reimplementation of TRACER's scoring** | §9 LATER, with the explicit instruction not to build TRACER's scoring from memory. F2 says on screen that it finds shared wording only; F4 imports TRACER's own output instead |
| **Topic modelling (LDA and relatives)** | §11 — the text's topics are its sa bcad outline, which `allcore/outline.h` already extracts. A topic model would put machine-composed labels beside Tibetan text |
| **Voyant Bubblelines, TermsBerry, Correlations, Links network** | Not in §10's BUILD list, and each fails the steelman: Bubblelines is Trends without the numbers; Correlations across a handful of documents is statistically hollow; Links is a word cloud with undefined edge weights. Relationships ships as a co-occurrence **table** with an explicit unit |
| **A third KWIC / concordance panel** | HAVE, not SKIP: the Overlay is the reader and the concordance exists twice already. Every Study row links out to those |
| **Charts and plots** | Consistent with the word-cloud ruling: distributions are tables plus text bars of ■. No painting code, no axis to mislabel, copyable, survives the accessibility pass |
| **Word-level (Botok) shingles and word-level similarity** | §8 specifies syllable shingles. Word segmentation is a reference/provisional layer here, so a word-based score would be labelled provisional while catching nothing a syllable shingle misses |
| **A places gazetteer, and any hit labelled "place"** | No place data ships and `docs/TIBETAN_TOOLS_SURVEY.md` records the THL kmaps bundling licence as unclear. F6 offers a user-initiated link-out; the enum has no `Place` member |
| **Phonetic name matching in a bulk text scan** | The Phonetic/PhoneticNear tiers are documented guesses. Reviewable against a typed query; turned loose on a running text they manufacture false people |
| **Association measures we cannot print** | Only Dunning's G² ships, with its 2×2 formula on screen and the sentence that it is a frequency fact. MI, t-score and the rest are not offered |
| **Cross-release joins on `entries.id`; corpus joins on `COURSE:seq`** | Both are per-release/positional ordinals; offering them would produce confident wrong answers |
| **Editing or merging inside Table Compare** | The Compare pane compares and never writes until Save, and release data is release-owned — this repo consumes releases, it does not fork them. Dictionary changes go through the proposals channel |
| **A general JSON tree differ in allcore** | §14 asks for record/field comparison keyed by a column, a table operation. Thin app-side adapters convert three known shapes; a general JSON differ needs its own honesty story about arrays and nulls |
| **Automatic re-run of a study when the document changes** | There is no `QFileSystemWatcher` in the app, and a silently refreshing study would make a saved report's numbers untraceable. Studies are run by a button and stamped with their options |
| **Image compare of scans; `.docx` extraction compare** | Filed LATER in `docs/COMPARE_SUITE_PLAN.md`; not in this cluster, and `.docx` extraction depends on macOS `textutil`, contradicting the Windows/Linux target for the input centres |

---

## 11. Open questions for Adam

1. **Pane shape.** Statistics, Passages, Similarity and Entities are four pages of one new **Study** pane in Research (one tab, one ribbon, one ratchet cost, the switcher idiom translators know from Compare), taking Research to seven tabs. Keep it there, or split Analysis + Study into their own group under your regrouping freedom?
2. **TRACER.** We ship the export and the manifest so a real output file can exist. Do you want the import lane at all, and can you send one real TRACER output file? Until you do, the importer is a generic column-mapper that imports zero rows when a required column is unmapped, and it is never labelled a TRACER profile.
3. **Passage index location.** May the app write `<dataRoot>/library/.passages.db` beside the existing `.index.db` — **signatures only, no text**, ~1 KB per text (~9 MB for the current 8,988-file library) — and rebuild it whenever the study settings change? For the OFFICIAL library that means writing into the shared Dropbox tree; the alternative is `<dataRoot>/library/studies/passages/cache/<hash-of-abs-path>.db`.
4. **Duplicate keys in the dictionary release.** The release TSV has 56 `wylie` values on two rows each (and `(wylie, tibetan)` still collides 10 times). The default marks those AMBIGUOUS and never pairs them. Should the release preset instead default to a composite `wylie + tibetan` key — fewer ambiguous rows, but 10 remain and the pairing then rests on an assumption?
5. **Table Compare and gated data.** Everything under `data/alignment` is gated in `docs/distribution/PAYLOAD_MANIFEST.txt` (HGM course English verbatim). May the Table Compare page open those files in a shipped/market build, or should the alignment presets be team-build only?
6. **Distinctive-terms background.** The HGM corpus (42,199 aligned segments) is the only collection we own with reliable document frequencies — "distinctive against Geshe Michael's usage". For a Sungbum text, is that the comparison you want, should it be the translator's own installed library (which needs a document-frequency index the library index does not expose today), or both as two columns?
7. **Properties ▸ Statistics label.** That tab's row "Words (segmenter)" is fed the syllable-token count, while a separate "Syllables" row counts the raw body with a regex. Study says **Syllables** and **Dictionary spans** and has no row called "Words". Relabel the Properties row (to "Syllables (segmenter)"), re-feed it from `doc_.spans.size()`, or leave it and note it in the audit? It is a visible number in a shipped dialog, so it is your ruling.
8. **The constitution baselines.** The R3 modal census and the G2 `hgm_gloss` census read `app/main.cpp` only, so every dialog and gloss in `compare_pane.inc` — and in the two new `.inc` files — is outside them today. This plan extends both to `app/*.inc`, which requires a one-time reviewed baseline for the sites that already exist. Confirm that ruling, since it moves a gate you own.
9. **The repeated-passage floor.** The default is 8 syllables, raised automatically to 12 for texts the verse analyser calls verse (8 is one line of a 7-syllable meter plus a particle). Keep the automatic raise, or hold 8 everywhere with the noise visible and the control in the translator's hand?
10. **Editorial marks.** The default apparatus policy strips whole `[…]`, `{…}`, `⟨…⟩` spans (the same rule the Compare pane's "Ignore [ ] { } apparatus" already applies) and **refuses** a syllable whose mark falls inside it rather than truncating `CHO[S]` to `cho`; `(…)` is retained and counted. Is that the right default for your witnesses, and is "Keep the letters inside the marks" the right second mode?
11. **Sidecar keying.** The "In my glossary" column reads `<dataRoot>/library/glossaries/<base>.tsv`, keyed by base name only — two texts of the same name in different folders share one glossary today. Should the Study pane follow that keying for parity, or should the keying question be raised as its own fix first? (The TRACER study store is deliberately keyed by the study name you type, so a document rename cannot orphan it.)

---

## 12. Must-fix ledger

| # | Must-fix (from the two judges) | Answered in |
|---|---|---|
| 1 | Statistics-before-Passages inversion; no placeholder n-gram counter | §9 step 5; §6 engine (`phrases` calls `passages::shingleForms`) |
| 2 | §9's ranked candidate-source table must be a real surface | §5 — ranked table, verified shared syllables, containment both ways, longest run + cite, "Open side by side" |
| 3 | Library-wide passages must not be a control nobody presses | §4 lands in this batch; §3 folder scope states its cost, is Stop-able, discloses read/skipped |
| 4 | Do not build the TRACER import against a guessed format | §5 — export + manifest ship; generic column-mapper; refuses unmapped score, non-finite score, sha1 mismatch; EXTERNAL tier; §11 Q2 |
| 5 | Extend `constitution_check.py` R3 + G2 to `app/*.inc` with baselines and a mutation check | §2 gate fix; tests 1–3; §11 Q8 |
| 6 | Promote the tokenizer by moving; keep/pin lenient-vs-strict cite | §1 algorithm; tests T9, T13 |
| 7 | Remainder lines always; scans disclose read/skipped; Stop says PARTIAL | §6 labels + test 24; §3 labels + tests 19–20; §4 labels |
| 8 | MinHash/LSH candidates only; estimate in its own column; hash constants pinned + generation bumped | §3 honesty step, labels, tests 2, 4–8, 11 |
| 9 | Table Compare: AMBIGUOUS keys, no `entries.id`/`seq` keys, lossy TSV banner, alignment TENTATIVE banner | §8 labels + tests 8, 9, 15 |
| 10 | Entities: no places, missing bank ≠ empty, homonyms unresolved, no phonetic tiers, candidate dates with the day-level refusal | §7 labels + tests 1–5, 8, 11–18 |
| 11 | Honour every §10/§11 SKIP; no statistic without its printed formula | §10; §6 labels (TTR, G², TF-IDF) |
| 12 | Trends across a folder is single-term; say so; disclose unindexed files | §6 Trends control, labels, engine note, test 25 |
| 13 | Do not repeat the Properties mislabel; raise it, don't rename it | §6 risks; §11 Q7; test 21 |
| 14 | One new pane; page-local rows in Compare; ratchet re-measured | §0 D1; §2 tests 7–8; §8 placement |
| 15 | Reuse rather than duplicate (`splitRow`, no AbbrTable parser, no third KWIC, corpus stays with `detectQuotations`) | §8 engine; §6 reuses; §3 reuses |
| 16 | Fixture-free smoke per core module; selfTests summed; harness-safe | Every test section; §9 closing paragraph; §13 |
| 17 | **Apparatus brackets** — one policy, applied uniformly, disclosed, pinned for `[ ] { } ( )` | §1 apparatus policy; tests T4–T8; §2 control; `Provenance.apparatus`; §11 Q10 |
| 18 | Byte vs UTF-16 offsets at the Overlay boundary | §1 risks + test T14 |
| 19 | One script routing at the pane boundary; do not mix `docprops` figures with converted ones | §0 D4; §6 risks + test 21 |
| 20 | Trends-by-file exactness against the folded index | §6 engine + labels; `countTermByFile(term, fold, …)`; test 20 |
| 21 | Distinctive terms: formula, spine-sourced background, cap, `df == −1` refusal, never "keywords" | §6 labels + tests 16–17, 26 |
| 22 | `Provenance` as an engine struct in every report | §0 D5; §1; every data section |
| 23 | Containment beside Jaccard, definitions printed, no directional wording | §5 columns, labels, tests 2, 8, 9 |
| 24 | Standardised TTR beside the raw ratio | §6 labels + test 8 |
| 25 | `unitSyllableCounts` extracted from `verse.cpp`, `analyzeVerse` rewired | §6 engine + test 3; §9 step 7 |
| 26 | EXTERNAL tier isolation for imported sets | §5 labels + tests 11–14 |
| 27 | Pinned provenance columns; composite-key rescue; refusal with column names | §8 tests 9, 14, 15, 20, 24 |
| 28 | Trends "across a collection ordered by author/date" with a "no date recorded" bucket last | §6 Trends control + labels + test 25 |
| 29 | Entities footnote list and index TSV deliverables | §7 controls + test 19 |
| 30 | Translator vocabulary on every control | §3 and §6 control tables |

---

## 13. Gate checklist per landing

- [ ] New core module registered in `core/CMakeLists.txt` **without** a fixture path, so it runs under `ctest -LE fixture`.
- [ ] Pane/page `selfTest` summed in the block at `app/main.cpp:41796-41818`.
- [ ] Every file dialog and every long run returns immediately under `g_harnessRun`; long runs refuse to start under `g_sweepActive`.
- [ ] `constitution` gate green with the `app/*.inc` censuses; baselines bumped only with a reviewed diff in the same commit.
- [ ] Ribbon width re-measured against `kRibbonRatchet = 2572`; pane `minimumSizeHint().width() ≤ 800`.
- [ ] Audit row written (`docs/release_audit/StudyPane.md`; rows added to `ComparePane.md`) in the house format.
- [ ] `docs/FINDINGS.md` entries with dispositions from the design-critique and accessibility passes, **before** Adam sees the surface.
- [ ] `docs/digests/LOG.md` line; `TODO.md` tick.

## Open questions for Adam (this batch)

- Pane shape: Statistics, Passages, Similarity and Entities are four pages of ONE new Study pane in Research (one tab, one ribbon, one ratchet cost), taking Research to seven tabs. Keep it there, or split Analysis + Study into their own group under your regrouping freedom?
- TRACER: we ship the export and the sha1 manifest so a real output file can exist. Do you want the import lane at all, and can you send one real TRACER output file? Until then the importer is a generic column-mapper that imports zero rows when a required column is unmapped, and it is never labelled a TRACER profile.
- Passage index location: may the app write <dataRoot>/library/.passages.db beside the existing .index.db — signatures only, no text, ~1 KB per text (~9 MB for the current 8,988-file library) — and rebuild it whenever the study settings change? For the OFFICIAL library that means writing into the shared Dropbox tree; the fallback is <dataRoot>/library/studies/passages/cache/<hash-of-abs-path>.db.
- Duplicate keys in the dictionary release: the TSV has 56 wylie values on two rows each (and (wylie, tibetan) still collides 10 times). Default is AMBIGUOUS and never paired. Should the release preset instead default to a composite wylie+tibetan key — fewer ambiguous rows, but 10 remain and the pairing then rests on an assumption?
- Table Compare and gated data: everything under data/alignment is gated in docs/distribution/PAYLOAD_MANIFEST.txt (HGM course English verbatim). May the Table Compare page open those files in a shipped/market build, or should the alignment presets be team-build only?
- Distinctive-terms background: the HGM corpus (42,199 aligned segments) is the only collection we own with reliable document frequencies — 'distinctive against Geshe Michael's usage'. For a Sungbum text is that the comparison you want, should it be the translator's own installed library (which needs a document-frequency index the library index does not expose today), or both as two columns?
- Properties ▸ Statistics label: that tab's 'Words (segmenter)' row is fed the syllable-token count while a separate 'Syllables' row counts the raw body with a regex. Study says 'Syllables' and 'Dictionary spans' and has no row called 'Words'. Relabel the Properties row, re-feed it from doc_.spans.size(), or leave it and note it in the audit? It is a visible number in a shipped dialog, so it is your ruling.
- The constitution baselines: R3 (modals) and G2 (hgm_gloss) read app/main.cpp only, so every dialog and gloss in compare_pane.inc — and in the two new .inc files — is outside both gates today. This plan extends both to app/*.inc, which needs a one-time reviewed baseline for the sites that already exist. Confirm that ruling, since it moves a gate you own.
- The repeated-passage floor: default 8 syllables, raised automatically to 12 for texts the verse analyser calls verse (8 is one line of a 7-syllable meter plus a particle). Keep the automatic raise, or hold 8 everywhere with the noise visible and the control in the translator's hand?
- Editorial marks: the default apparatus policy strips whole [ … ], { … }, ⟨ … ⟩ spans (the same rule the Compare pane's 'Ignore [ ] { } apparatus' already applies) and REFUSES a syllable whose mark falls inside it rather than truncating CHO[S] to 'cho'; ( … ) is retained and counted. Is that the right default for your witnesses, and is 'Keep the letters inside the marks' the right second mode?
- Sidecar keying: the 'In my glossary' column reads <dataRoot>/library/glossaries/<base>.tsv, keyed by base name only, so two texts of the same name in different folders share one glossary. Should Study follow that keying for parity, or should the keying question be raised as its own fix first? (The TRACER study store is deliberately keyed by the study name you type, so a rename cannot orphan it.)

## Files this batch creates or touches

| File | Kind | Contents |
|---|---|---|
| `docs/ANALYSIS_SUITE_PLAN.md` | docs | This specification (the harness blocked the subagent from writing it; create it from the markdown returned here). |
| `docs/release_audit/StudyPane.md` | docs | Release audit for the new Study pane in the ComparePane.md house format: purpose/how reached, a control-by-control table with code anchors and coverage, honesty-and-provenance section, open items. |
| `core/include/allcore/provenance.h` | core-header | struct Provenance (engine, built_utc, source, source_sha256, script_in, norm, apparatus, units_read, units_refused, apparatus_marks_stripped, refusals) + provenanceBlock(). |
| `core/src/provenance.cpp` | core-src | provenanceBlock() report-header rendering and the JSON string helpers. |
| `core/include/allcore/textspan.h` | core-header | Syllable/Converted/FolioLoc/Apparatus types; syllablesWylie, syllableSpans, toEwtsLine, toEwtsDocument, folioAt, citeAt, folioRange, countFolioMarkers, countShads. |
| `core/src/textspan.cpp` | core-src | The promoted quotation tokenizer, the promoted textdiff script routing, the one folio walk, and the three-mode apparatus policy. |
| `core/tests/textspan_smoke.cpp` | core-test | 14 checks: tokenizer, offsets/barriers, the four apparatus cases, the citeAt/folioAt cross-check, conversion refusals, folio range, docprops parity. |
| `core/include/allcore/passages.h` | core-header | Doc/Options/Repeat/PairScore/Report/Signature; shingleHashes, minhash, estimateJaccard, lshCandidates, repeatsWithin, repeatsAcross, sharedPassages, shingleForms, passageGeneration, rankAgainst/SimilarityRow. |
| `core/src/passages.cpp` | core-src | Broder shingling, Carter–Wegman/SplitMix MinHash, LSH banding, seed-and-extend verification, containment, stop-shingles, and the similarity ranking. |
| `core/tests/passages_smoke.cpp` | core-test | 20 checks including the pinned hash value, five-seed estimator bound, floor honoured, containment vs Jaccard, fold labelling, cites, caps, LSH recall, and the similarity block. |
| `core/include/allcore/passage_index.h` | core-header | class PassageIndex with UpdateStats, update(), candidatesFor(), docCount/builtUtc/generation (sqlite3 forward-declared, libindex.h style). |
| `core/src/passage_index.cpp` | core-src | The .passages.db schema, incremental (path,mtime,size) update, application_id generation stamp, cancel-banks-nothing rebuild, band-bucket candidate lookup. |
| `core/tests/passage_index_smoke.cpp` | core-test | 8 checks over a temp directory: added/unchanged/updated/removed, generation rebuild, cancel withholds the stamp, candidate lookup, unconvertible file, write failure. |
| `core/include/allcore/textstats.h` | core-header | StatUnit/StatFold/Options/Count/LengthBin/ParticleRow/Basic/Report; analyze, unitSyllableCounts, collocates, trendByFolio, phrases, distinctiveBigrams. |
| `core/src/textstats.cpp` | core-src | Frequency/hapax/TTR+STTR, the shared shad split, particle delegation, closed repeating phrases via shingleForms, Dunning G² collocates, folio trends, Spärck Jones tf-idf with the df-unknown refusal. |
| `core/tests/textstats_smoke.cpp` | core-test | 18 checks including the one-split pin against analyzeVerse, particle delegation recomputed in the test, STTR comparability, closed phrases, barrier windows, the hand-computed G² value, zero trend buckets, df == -1 refusal, remainder counts. |
| `core/include/allcore/entities.h` | core-header | Kind (no Place member)/Tier/Occurrence/Entity/PersonBank/TitleBank/Options/Report; scan(), scanYearPhrases(). |
| `core/src/entities.cpp` | core-src | Aho–Corasick over a syllable alphabet with longest-match, the single splitFusedEnding tolerance, orthographic tiers only, date pairs delegated to yearsFor, folio co-occurrence. |
| `core/tests/entities_smoke.cpp` | core-test | 13 checks with its own inline banks: longest alias, affix tolerance, syllable-boundary safety, unresolved homonyms, title floor, nesting, yearsFor delegation, cite parity, no-Place compile check, missing-bank flags, truncation. |
| `core/include/allcore/table.h` | core-header | Format/Row/Table/TableProfile; readDelimited(), writeCsv(), tableProfiles(), detectProfile() — the app's one delimited reader. |
| `core/src/table.cpp` | core-src | The lifted RFC-4180 splitRow, header-driven delimiter detection, '#' comments, implied column names, and refuse-with-line-number for ragged rows. |
| `core/tests/table_smoke.cpp` | core-test | 6 checks: quoted commas, tab-wins detection, CRLF, implied columns on a headerless TSV, refused ragged row with raw text, CSV round-trip. |
| `core/include/allcore/tablediff.h` | core-header | RowStatus/FieldDiff/RowDiff/DiffOptions/DiffResult; diffTables(), diffCsv(), diffMarkdown(). |
| `core/src/tablediff.cpp` | core-src | Key-union outer join with multiplicity checked first (AmbiguousKey never paired), name-aligned columns, delegated cell equality, multiset list columns, pinned provenance columns. |
| `core/tests/tablediff_smoke.cpp` | core-test | 10 checks: statuses, ambiguous key, composite-key rescue, one-sided column never 'changed to empty', list multisets, delegated rules, key case, pinned provenance, refusal with column names, CSV header. |
| `app/study_pane.inc` | app-inc | class StudyPane (three-group ribbon, apparatus control, shared status line) plus StatisticsPage, PassagesPage, SimilarityPage and EntitiesPage, each with its own selfTest and TRACER export/import in the similarity page. |
| `app/table_compare.inc` | app-inc | class TableComparePage with page-local control rows, the preset table, the JSON→allcore::Table adapters, the result tree and reports, plus selfTest and installTableComparePage(). |
| `app/main.cpp` | wiring | #include "study_pane.inc" immediately after the compare_pane.inc include. |
| `app/main.cpp` | wiring | mkGroup("Research", {…}) at line 39208 gains "Study" between "Analysis" and "Compare". |
| `app/main.cpp` | wiring | tabs.addTab(studyPane, "Study") beside the Compare pane's addTab, and a paneIcon entry mapping "Study" to the existing icon kind "count". |
| `app/main.cpp` | wiring | Hook assignments beside g_compareTexts (38627): g_studyText, g_personBank and g_titleBank. |
| `app/main.cpp` | wiring | Two lines in the selftest block at 41796-41818: fails += studyPane->selfTest(log); fails += cmpPages.table->selfTest(log); |
| `app/main.cpp` | wiring | Tools ▸ Study submenu (four items, no accelerators) and two entries added to the Library pane's existing Maintenance menu. |
| `app/main.cpp` | wiring | CLI: --stats, --passages and --table-compare handled beside --compare/--survey, with the three flags added to the single early.contains(...) harness list (R2). |
| `app/compare_pane.inc` | wiring | Three-line addPage(QWidget*, const QString& label) overload plus installTableComparePage() returning the page into ComparePages. |
| `core/CMakeLists.txt` | wiring | Seven fixture-free add_executable/target_link_libraries/add_test blocks (textspan, passages, passage_index, textstats, entities, table, tablediff) in the textdiff_smoke idiom at lines 148-150. |
| `core/src/quotation.cpp` | wiring | Delete the anonymous-namespace syllables() (12-43) and call allcore::syllablesWylie; quotation_smoke must pass unchanged. |
| `core/src/textdiff.cpp` | wiring | File-static toEwts (83-93) and citeFor (568-580) delegate to allcore::toEwtsLine and allcore::citeAt; textdiff_smoke must pass unchanged. |
| `core/src/catalog_list.cpp` | wiring | acipFolioRange reimplemented as a call to allcore::folioRange; the public signature is unchanged. |
| `core/src/catalog_register.cpp` | wiring | Delete the anonymous-namespace splitRow (19-51) and call allcore::table::readDelimited's shared splitter; register_smoke is the regression proof. |
| `core/src/verse.cpp` | wiring | Extract the shad/newline unit split (10-38) into allcore::textstats::unitSyllableCounts and have analyzeVerse call it. |
| `core/include/allcore/tibcal.h` | wiring | Additive exports elementNamesWylie() and animalNamesWylie() for the tables now private in tibcal.cpp. |
| `core/include/allcore/libindex.h` | wiring | Declare countTermByFile(term, fold, maxFiles, truncated) with a comment that the index stores search-folded text, so an unfolded count is not exact. |
| `core/src/libindex.cpp` | wiring | Implement countTermByFile as one grouped SQL statement over lines_fts joined to files, honouring maxFiles and setting truncated. |
| `tools/constitution_check.py` | wiring | Extend the R3 modal census (~line 86) and the G2 hgm_gloss census (~line 124) to read app/*.inc alongside app/main.cpp, with separate baseline files. |
| `tools/constitution_baseline_inc.txt` | wiring | Reviewed one-time R3 baseline for the modal sites already present in app/*.inc, installed in the same commit as the census change. |
| `tools/constitution_hgm_baseline_inc.txt` | wiring | Reviewed one-time G2 baseline for hgm_gloss sites already present in app/*.inc, installed in the same commit as the census change. |

---

# Batch 6 — References pane, Insert Citation, duplicates, BDRC refresh, BibTeX/RIS

*Final Batch 6 specification, built from the translator-first design (both judges' winner, 43/43 and 43) with the engineering-hygiene and provenance grafts the judges named, and with every mustFix from both judges addressed.

Key rulings baked in:
- **One implementation of the house style.** Built-in styles CALL `composeBibliographyEntry`/`composeSanskritBibEntry`; `parseCitationStyle` refuses any style that sets both `builtin` and a template. Verified in source why this matters: the writer applies `endashRanges` to dates AND folios, emits two spaces after the dates period, and switches the `"(ACIP …)"` separator — no flat template can reproduce it. Pinned byte-identically by `citation_smoke` test 1.
- **The ACIP-orthography problem is solved explicitly** (judge B's sharpest mustFix). Verified `catalog_works.json` S702 author is uppercase ACIP `RGYAL BA DGE 'DUN GRUB`. The picker offers three honest routes — type the published spelling (default), pair-hyphenate as a human-pressed MACHINE-badged editable button, or render the ACIP visibly labelled — and `hgmTechnicalSpelling` is never reachable from `renderCitation` (asserted by test).
- **Prerequisite hygiene commit H1–H5**, all verified in source today: `ApparatusBib` (main.cpp:946) genuinely drops `acip_refs` while DraftPane's loader (22956) reads it; `tools/constitution_check.py:29` genuinely reads only `app/main.cpp`, so `.inc` modal sites are unpoliced (compare_pane.inc has 0, baseline holds at 154); export the private `overlap()` from `catalog_id.cpp:24-47` as `titleOverlap` so the duplicates finder and `suggestIdentity` cannot disagree; unify the bank loaders; file the LibraryPane key-regex convergence.
- **Witness folding before duplicate scoring** — files sharing a catalogue key fold into one record with N witnesses, so the normal Sera Mey vs Ganden case never reaches the finder.
- Unrecognised files keyed by **absolute-path hash**, not basename. `normalizeCatalogKey` everywhere. Per-field history + counted divergence. `banks_generation` stamp. "Compared raw" for non-ACIP titles. Brute-force equivalence test proving the blocking loses nothing. Merge touches no file (test compares directory listing AND SHA-1s). BDRC: user-initiated only, request-count assertion under harness, no invented by-id endpoint, default keep-ours, refuse-to-blank, every fetch logged. Legacy `[NOTE:]`/`[BIBLIOGRAPHY:]` markers left byte-untouched and counted.
- **§17's last unanswered verdict is closed:** F10 ships "this text quotes…" on the existing `quotation.h`; the reverse direction is in deliberatelyOmitted with the §8/Batch-5 dependency named and said on screen.
- Storage deviates from §17's shared `library/references.db` (local disposable index + shared judgment TSVs) with the reason written down and raised as open question 1.

10 features, 93.5 h including hygiene. Sequencing is translator-first: the citation picker ships second, before any pane UI.*

# Batch 6 — References, Insert Citation, duplicates, BDRC refresh, BibTeX/RIS

**Status:** plan, not built. Adam, 2026-09-09.
**Source verdicts:** `docs/TEXT_ANALYSIS_SUITE_RESEARCH.md` §17 (EndNote/Zotero mapping table) and §18 ("Batch 6: References pane + Insert Citation (§17)").
**Audit row on completion:** `docs/release_audit/ReferencesPane.md`, in the format of `docs/release_audit/ComparePane.md`.

This plan starts from the translator-first design (the picker before the pane; the shelf as a *view*, never a second catalogue) and grafts the engineering-hygiene and provenance work the reviewers asked for. Where the three candidate designs disagreed, the ruling and its reason are written down here rather than left in code.

---

## 0. The four commitments this batch is built on

1. **The shelf is a VIEW over the catalogue, never a second store of texts.** No text record exists in this app today: identity is decoded from the ACIP filename by `allcore::decodeAcipFilename` and enriched at display time from the release-owned banks in `data/extracted/`. Batch 6 does not change that. Catalogue-derived rows are **copy-on-write** — nothing is stored until a human touches a record — so a data release flows straight through, and `references.db` is a local, disposable index that can be deleted at the cost of one rebuild.
2. **The app never writes the official catalogue and never touches a text file.** "Merge duplicates" records the translator's judgment on their own shelf. The single route to the catalogue is `ProposalKind::CatalogIdentity` (`core/include/allcore/proposals.h:40`), which already exists and is already ruled on by an approver.
3. **The house bibliography format has exactly ONE implementation.** Built-in styles call `allcore::composeBibliographyEntry` / `composeSanskritBibEntry` (`core/include/allcore/tibexport.h:73-124`). JSON templates serve Chicago/MLA/user/secondary styles only. Verified why this matters: the shipped writer (`core/src/tibexport.cpp:172-194`) applies `endashRanges` to **both** dates and folios, emits **two spaces** after the dates period, and switches between `" (ACIP …)"` and `" (tibtitle, ACIP …)"` when the Tibetan title is absent. No flat template grammar can reproduce that, so any style-file re-expression of STD-007 would be the second implementation CLAUDE.md rule 2 forbids.
4. **Citations MATCH published wording; they never compose it.** Where `data/extracted/apparatus_bibliography.json` (verified: 705 entries, each carrying `acip_refs`) already holds a published entry for the work being cited, that entry's text is emitted **verbatim** with "as published in \<volume\>". Only where no published entry exists does the tool compose from typed/catalogue fields, and it says so on the line.

---

## 0.1 Prerequisite hygiene commit (2.5 h, behaviour-neutral, independently revertible)

Land this **before** any References code. Each item was verified in the source today.

| # | Change | Anchor | Why |
|---|---|---|---|
| H1 | Add `acip_refs` to `struct ApparatusBib` and fill it in the global loader; delete DraftPane's second apparatus loader and point it at `g_appBib` | `app/main.cpp:946` (`struct ApparatusBib { QString source, section, id, text; }` — **drops `acip_refs`**); `app/main.cpp:22956` (the only place `acip_refs` is read today) | The published-entry lane joins on the ACIP number. Two loaders reading one file with different fields is how the counts on screen come to disagree. |
| H2 | Export `allcore::titleOverlap(norm_a, norm_b, int* shared)` from `catalog_id.h`, implemented by the existing private `overlap()`/`syllables()` | `core/src/catalog_id.cpp:24-47` | One scorer. `suggestIdentity` and the duplicates finder can then never disagree about what two titles share. No fifth tokenizer. |
| H3 | Extend `tools/constitution_check.py`'s R3 modal census to `app/*.inc` | `tools/constitution_check.py:29` reads **only** `app/main.cpp` | Verified loophole: every modal site in a `.inc` is outside the census today. `app/compare_pane.inc` has **zero** `QMessageBox`/`QInputDialog` sites, so the baseline stays **154**. Ship this in the same commit as `references_pane.inc`, or Batch 6 opens an unpoliced hole. |
| H4 | Unify the catalogue-bank loaders into one shared app-side loader, with the "set the loaded flag only AFTER a bank actually arrived" rule written **once** | `LibraryPane::englishTitle` (27009), `loadPersons` (27834), `buildAuthorIndex` (26945), plus CatalogPane's duplicate loads; the lazy-load bug is annotated three times in `main.cpp` (27010-27016, 27838-27842, 26418-26419) | Without it, References, Library and Catalog quote different work counts on adjacent screens. |
| H5 | File the follow-up (do **not** do it here): converge `LibraryPane`'s ad-hoc work-key regex (`app/main.cpp:27055`) onto `allcore::normalizeCatalogKey` | `core/include/allcore/catalog_audit.h:38-49` — the header's own comment warns that `baseCatalogKey` loses the `-sub` suffix and that `S05002-1`/`S05002-2` are **different works** | References uses `normalizeCatalogKey` everywhere. Until Library does too, the two panes can disagree about S5002-1 on one screen. Filed as a TODO with this anchor. |

---

## F1 — Shelf engine: the record model over the catalogue (`allcore::refrecord`)

**§17 verdict:** ADAPT (row 1 — "a References view over the catalogue: every text/edition/article/person/place is a record with attachments"). Built as a rebuildable view, not a new store.

### Purpose and user story
As a translator I want to see my library as **texts**, not files: one row per work, its number, its Tibetan and published English titles, its author with dates, which edition, how many folios, which files on disk are witnesses to it, and what is attached. I want to see at a glance which of my files the catalogue does not recognise — instead of discovering it three months in.

### UI
No surface of its own. Consumed by F2, F5, F7, F8, F10. Rebuild is triggered from F2's **SHELF** ribbon group and from Library ▸ CARE ▸ "Rebuild the References shelf…", so a translator who lives in the Library pane never has to learn a new place.

**Honesty labels (engine-supplied strings, pinned by test):**
- Rebuild banner: `"1,412 records from 8,988 files · 213 files whose names the catalogue grammar does not recognise (listed below) · 4 files could not be read (listed)."` A cap or a stop never reads as a total: a cancelled rebuild says `"stopped — 2,140 of 8,988 files read; the shelf shows what was read."`
- Missing bank: `"data/extracted/catalog_works.json could not be read — records show filename-derived fields only."` **Never** "no records". (The pattern at `app/main.cpp:26789-26799`.)
- Every field renders with its source verbatim and the matching `ux::Epistemic` badge (`app/ux_tokens.h:69`): REFERENCE for the catalogue banks (named: `catalog_works.json` / `catalog_titles.json` / `acip_person_links.json`), EVIDENCE for the file's own title page or colophon, MACHINE for a suggested identity, and plain "typed by \<name\>" for anything a person entered. **Nothing in this pane is ever badged Binding** — `hgm_gloss` does not appear here at all.
- Standing line: *"The shelf is rebuilt from your library and the catalogue banks. It never changes a text, a filename, or the catalogue."*

Fit-ratchet note: engine only; no ribbon contribution.

### Data
SQLite, `<dataRoot>/library/references.db`, `PRAGMA user_version=1` — the same **derived, local, disposable** idiom as `library/.index.db`.

```
meta(key TEXT PRIMARY KEY, value TEXT)
  -- schema, built_utc, library_root, banks_generation, n_files,
  -- n_unrecognized, n_unreadable, canceled
records(key TEXT PRIMARY KEY, kind, number, collection, edition,
        tib_title, eng_title, author, author_dates, person_pid, folios,
        language, status, colophon, title_norm, title_is_acip INTEGER,
        recognized INTEGER, sources TEXT)
record_files(key TEXT, path TEXT, bytes INTEGER, mtime INTEGER, sha1 TEXT)
CREATE INDEX record_files_key ON record_files(key);
records_fts USING fts5(key UNINDEXED, tib_title, eng_title, author, number,
                       tokenize="unicode61 tokenchars '")
```

**Record key rule (fixes the collision the reviewers caught):**
- recognised text → `allcore::normalizeCatalogKey(number)` (S464 == S00464; `-1` sub-numbers preserved). Never the LibraryPane regex.
- unrecognised file → `FILE:<first 10 hex of sha1 of the ABSOLUTE path>`. **Not** the basename: two `KL0001MA.ACT` files in different folders are two records.
- imported secondary literature → `IMP:<first 8 hex of sha1(raw entry text)>` (F9).

**Location:** `<dataRoot>/library/references.db` (+ `-wal`/`-shm`). Local and rebuildable: gitignored, **not** in the launch backup list, **not** shared. Only the judgment TSVs (F3, F4, F7, F8, F9) are shared and backed up.

**Provenance fields:**
- `records.sources` — one `field=source|tier` line per populated field; source strings verbatim (`filename`, `catalog_works.json`, `catalog_titles.json`, `acip_person_links.json`, `title page (the file's own)`, `colophon (the file's own)`, `bdr:MW22084_184`, `typed`, `bibtex-import:<file>`).
- `records.recognized` — 0 when `decodeAcipFilename` refused the name. The pane shows these as a **named residue**, never as absent.
- `records.title_is_acip` — 0 when the title arrived as Unicode Tibetan or mixed script. F7 refuses to fold such a title through `normalizeAcipTitle` and marks the pair **"compared raw"** (the `textdiff` rawNormalised idiom).
- `meta.banks_generation` — sha1 over (size, mtime) of every bank read. A mismatch drops and rebuilds the index rather than letting the pane silently disagree with the Library card (`core/src/libindex.cpp:176-188` precedent).
- `meta.canceled` — a stopped rebuild is disclosed, never presented as complete.

### Engine
`core/include/allcore/refrecord.h` + `core/src/refrecord.cpp`; battery `core/tests/refrecord_smoke.cpp`, registered in `core/CMakeLists.txt` beside `textdiff_smoke` (`core/CMakeLists.txt:148-150`) with **no fixture path** — the smoke builds its own temp tree, so it runs in the clean-checkout `ctest -LE fixture` gate.

```cpp
struct RefField { std::string value, source, tier; };  // reference|evidence|machine|typed
struct RefFieldHistory { std::vector<RefField> prior; bool divergent = false; };
struct RefRecord {
    std::string key, kind;                     // "text"|"person"|"secondary"
    RefField number, collection, edition, tibetan_title, english_title,
             author, author_dates, person_pid, folios, language, status;
    std::vector<std::string> files, attachments;
    std::string colophon, title_norm;
    bool recognized = false, title_is_acip = true;
    std::map<std::string, RefFieldHistory> history;   // per-field chain
};
struct WorkBankEntry { std::string tib, eng, author, author_eng, dates, pid; };
using WorkBank = std::map<std::string, WorkBankEntry>;   // key already normalised

RefRecord recordForFile(const std::string& path, const WorkBank& bank,
                        const std::string& head_bytes);   // first 8 KB only

struct ShelfStats { int files=0, records=0, unrecognized=0, unreadable=0;
                    bool canceled=false;
                    std::vector<std::string> unreadable_paths, unrecognized_paths; };
ShelfStats buildShelf(const std::string& library_root, const WorkBank& bank,
                      std::vector<RefRecord>& out,
                      const std::function<bool(int done,int total,const std::string&)>& progress = {});
std::string shelfSummary(const ShelfStats&);   // the honest count line, one place

class RefShelfIndex {
public:
    explicit RefShelfIndex(const std::string& db_path);
    [[nodiscard]] bool rebuild(const std::vector<RefRecord>&, const std::string& built_utc,
                               const std::string& banks_generation, const ShelfStats&);
    std::vector<RefRecord> all() const;
    std::vector<RefRecord> query(const ShelfQuery&) const;
    std::optional<RefRecord> get(const std::string& key) const;
    std::string metaValue(const std::string& key) const;
};

struct ShelfQuery { std::string text, kind, edition, collection_id, tag_id, read_state;
                    bool only_with_files=false, only_unrecognized=false; int limit=0; };
std::string serializeShelfQuery(const ShelfQuery&);
bool parseShelfQuery(const std::string&, ShelfQuery*, std::string* err);
std::string explainShelfQuery(const ShelfQuery&);   // ONE English sentence

struct FieldDelta { std::string field, ours, theirs, ours_source, theirs_source;
                    bool differs=false, ours_empty=false; };
std::vector<FieldDelta> compareRecords(const RefRecord& ours, const RefRecord& theirs);
enum class MergeMode { All, EmptyOnly, Chosen };
RefRecord applyMerge(const RefRecord& ours, const RefRecord& theirs, MergeMode,
                     const std::set<std::string>& chosen,
                     std::vector<std::string>* changed_out,
                     std::vector<std::string>* refusals_out);  // "refused to blank <field>"
std::string recordAsFieldText(const RefRecord&);   // sorted "field: value" for g_compareTexts
```

**Algorithm — no new inference.** Identity is *decoded*, not guessed: `decodeAcipFilename` gives collection/number/status/part/sub-number/language; `normalizeCatalogKey` gives the padding-insensitive key; the `WorkBank` supplies Tibetan and **published English** titles, author and dates; `acip_person_links` supplies the BDRC pid. The file's own title page (`extractAcipTitle`) and colophon (`findColophonCandidates`, first span) are stored as EVIDENCE and used only by F7. Folio range comes from `acipFolioRange` (`core/include/allcore/catalog_list.h:51`). Eligibility filter and tree walk follow `generateAsciiCatalog`'s shape (`.txt/.act/.inc/.ace`, META skipped).

**Witness folding (the Tibetan-fit rule that stops the duplicates finder crying wolf):** files sharing a normalised catalogue key become **witnesses of one record**. Two prints of one work — Sera Mey and Ganden — are the normal case in this library, not a duplicate. They fold in F1 and therefore never reach F7.

Nothing is inferred where a bank is silent: an absent English title is absent and says so. The rebuild is a single `BEGIN`/`COMMIT` so a crash leaves the previous shelf intact; progress and cancellation follow `LibraryIndex::update`'s contract verbatim (`core/include/allcore/libindex.h:22-133`), including the rule that a stop is honoured only **between** files.

**Reuses:** `decodeAcipFilename`/`AcipFileInfo` (tibexport.h:36-53) · `normalizeCatalogKey`/`baseCatalogKey` (catalog_audit.h:43-49) · `acipFolioRange`, `generateAsciiCatalog`'s walk (catalog_list.h:47-51) · `extractAcipTitle`/`normalizeAcipTitle`/`findColophonCandidates` (catalog_id.h:38-150) · `LibraryIndex`'s SQLite + FTS5 + cancellable-progress idiom · the H4 shared bank loader.

### Acceptance tests

**CTest — `refrecord_smoke`**
1. `recordForFile("S00184_BYANG CHUB LAM RIM CHEN MO_The Great Book on the Steps of the Path_TZONG KHA PA (1357-1419).TXT", bank{S00184:{…}}, "")` → `key=="S00184"`, `recognized==true`, `number.source=="filename"`, `english_title.value==bank eng`, `english_title.source=="catalog_works.json"`, `author_dates.value=="1357-1419"`.
2. `recordForFile("my notes on emptiness.txt", {}, "")` → `recognized==false`, `key` begins `"FILE:"`, `number.value==""` and `number.source==""` (no invented number).
3. **Path-keyed, not basename-keyed:** two files both named `KL0001MA.ACT` in different folders produce two **different** `FILE:` keys.
4. `buildShelf` over a temp tree of 4 files where two share number S00184 → `files==4`, `records==3`, the S00184 record has `files.size()==2` (witness folding), `unrecognized==1` and `unrecognized_paths` lists that file.
5. Progress callback returns false after the 2nd file → `canceled==true`, `files==2`; `shelfSummary()` contains `"stopped"` and `"2 of 4"` and **not** a bare total.
6. A file made unreadable (chmod 000) → `unreadable==1`, its path in `unreadable_paths`, and it is **not** counted as a record.
7. `RefShelfIndex::rebuild` then `all()` returns every field including per-field source strings byte-identical; reopening the db returns the same `metaValue("built_utc")`; `query({text:"lam rim"})` finds it; `query({only_unrecognized:true})` returns exactly the `FILE:` record.
8. `banks_generation` mismatch on open → the index is dropped and `recordCount()==0` until a rebuild (a stale shelf never silently disagrees with Library).
9. `serializeShelfQuery` → `parseShelfQuery` round-trips a query containing a tab and a `|`; `parseShelfQuery("kind=nonsense")` returns false with `err` naming the offending key; `explainShelfQuery({text:"lam rim", read_state:"unread"})` == `"text contains \"lam rim\" and you have not read it"`.
10. `compareRecords(ours{author_dates:""}, theirs{author_dates:"1878-1941"})` → one delta, `differs`, `ours_empty`. `applyMerge(EmptyOnly)` sets `author_dates` and leaves a differing `english_title` untouched, `changed_out=={"author_dates"}`; `applyMerge(All)` also overwrites and each changed field's source becomes the theirs source; `applyMerge(Chosen,{})` changes nothing.
11. **Refuse to blank:** theirs has an empty value where ours has one; `applyMerge(All)` keeps ours and returns `refusals_out=={"refused to blank english_title"}`.
12. `recordAsFieldText` produces sorted field order on both sides, so diffing two identical records yields **0** hunks (no spurious reordering noise in the Compare pane).

**app `--selftest` (ReferencesPane)**
13. Rebuilding over a 5-file temp library yields 4 records and the banner contains `"1 file whose name the catalogue grammar does not recognise"`.
14. With the banks deliberately unreadable, the banner contains `"could not be read"` and `"filename-derived fields only"`; the record count is still non-zero; **no** empty-state message.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Two work-key normalisations coexist (`LibraryPane` regex vs `normalizeCatalogKey`) — S5002-1 and S5002-2 could merge on one screen and split on another | References uses `normalizeCatalogKey` only; H5 files the Library convergence with the anchor; test 4 pins that a `-1` key survives |
| Banks are gitignored and absent on ordinary installs (`app/main.cpp:35289-35296`) | A missing bank reads as "could not be read", never "no records" (test 14); H4 writes the "set loaded only after arrival" rule once |
| 8,988 files × an 8 KB head read is a multi-second walk | Never on launch. The pane opens on the last built shelf and offers Rebuild, exactly as the Library index does; `QProgressDialog` with Stop; a stop is disclosed |
| Sidecars (properties, glossaries, links, worksheets) are keyed by `completeBaseName` (`app/main.cpp:3947`), so two same-named files share them | Records are path-keyed, but attachments inherit the filesystem's sharing. They are labelled **"sidecar for this base name"**, never "this file's" |

**Effort:** 14 h. **Depends on:** H1–H4.

---

## F2 — References pane (Research group): shelf table + record card

**§17 verdict:** ADAPT — "a References pane in the Research group; left, collections/smart collections/tags; centre, the record table; right, the record with citation preview in the chosen style."

### Purpose and user story
As a translator I want one place that answers *"what is on my shelf, what do I know about it, and what do I cite it as"* — so that starting a new commentary begins with five minutes of looking rather than an afternoon of grepping filenames.

### UI
**Placement.** New tab **"References"** in the Research group: `mkGroup("Research", {"Search","Lookup","Sanskrit","Convert","Analysis","Compare","References"})` (`app/main.cpp:39208`); `paneIcon` entry `{"References","quote"}` (~39080); `tabs.addTab(referencesPane, "References")` beside `comparePane` (38629). Implemented as `app/references_pane.inc`, included **after** `compare_pane.inc` (`app/main.cpp:23829`) so it can use `cmp::readText`, `g_compareTexts`, `saveOrWarn`, `docprops`, `RibbonBar`, `editops`.

**Body.** `QStackedWidget` of **3 pages** (0 Records · 1 Duplicates · 2 Import & Refresh), the `ComparePane::showPage` idiom, so the ribbon never has to carry every surface at once. Records page = `QSplitter [ shelf tree | record table | record card ]`.

**Controls — every one implemented in this batch; nothing greyed with a promise.**

| Group | Control | What it does |
|---|---|---|
| SHELF | **Rebuild shelf…** | `QProgressDialog` with Stop (the `LibraryPane::updateIndex` pattern, 28370-28420); banner reports records / unrecognised / unreadable / cancelled |
| SHELF | **Shelf ▾** | "What was skipped…" (lists unrecognised + unreadable paths, each opening the file) · "Shelf info…" (built_utc, counts, banks_generation) · "Delete and rebuild" |
| FIND | search box (`sess::remember` key `references/search`) | matches number, Tibetan title, English title, author via `records_fts` |
| FIND | Kind ▾ (All texts / Uncatalogued files / Secondary literature) · Edition ▾ (filled from the shelf's own distinct editions) | filters |
| FIND | **Save as smart collection…** | F3 |
| MARK | **Tags ▾** · **Read ▾** (Unread/Reading/Read) · **Add to collection ▾** | act on the table selection (multi-select) |
| CITE | Style ▾ · **Copy citation** · **Insert into Draft** | F5 |
| MORE ▾ | Duplicates… · Refresh from BDRC… · Import BibTeX/RIS… · Export ▸ (BibTeX / RIS / CSV of the visible rows) · Citation Styles… · Save report… | low-frequency actions live here, not on the band |

**Left tree.** All records (N) · Unrecognised files (N) · On my desk (dossiers) · Recently opened (`Progress::recentKeys("openfile")`, `progress.h:73`) · Read state ▸ · Collections ▸ · Smart collections ▸ · Tags ▸ · **Catalogued subjects ▸ (read-only, REFERENCE)**.

**Centre table.** `● tags | ◐ read | Number | Tibetan title | English title (published) | Author | Edition | Folios | Files | Attach`. Sortable, stable. Double-click opens the text in the Overlay (`g_openAtLine`, at the dossier's remembered line when there is one). Context menu: Open · Show in Library · Show in Files · Compare with… (`g_compareFiles`, two rows selected) · Copy citation · Add to collection ▸ · Tag ▸ · Mark read ▸ · **Quotes / quoted by…** (F10).

**Right card.** Identity block with a **source badge per field** and a `[history]` link per field (F1's per-field chain: who said what, when, and what it replaced); attachments list (glossary, properties, worksheet, links, comments count, cached scan folder — each an anchor that opens it); "Their works" via `g_openAuthorByPid`; catalogued subjects via `Subjects::forWork`; citation preview + Copy + Insert into Draft; **Refresh from BDRC…**; read-state buttons; tag chips.

**Status line.** `"Showing 214 of 1,412 records · shelf built 2026-09-09 18:40 · 213 files not recognised"` — shown / total / skipped, always, per the `corpusCountLine` discipline (`app/textreport.h:86-112`).

**Honesty labels.**
- Pane banner: *"The shelf is a view over your library and the catalogue. It never renames, moves or deletes a text, and it never writes the catalogue. Your collections, tags, read marks and notes are yours and are stored separately."*
- Column tooltip on "English title (published)": *"the catalogue's published English title. The app never translates a title."*
- Catalogued subjects: REFERENCE badge + *"catalogued subject heading (R6 / Sungbum) — a catalogue fact, not your tag"*, and not editable here.
- Empty state before any rebuild: *"No shelf yet. Rebuild reads your library and the catalogue banks; it changes nothing."* — not a spinner, not a blank table.

**Keyboard.** ⌘F focuses the search box; F5 rebuilds the shelf (matching Compare's Rescan); ⌥⌘C opens Insert Citation (F5, verified free).

**Fit-ratchet note.** A **pane-local** ratchet of **1180 px** (the app's launch width, `docs/DEMO_TUESDAY.md`) is asserted by the pane's own selftest — *not* just the app-wide `kRibbonRatchet = 2572` (`app/main.cpp:43708`), which is far too loose to protect this pane. Held by: small stacked `QPushButton`s 2–3 per `addColumn`, no big icon proxies, and the MORE ▾ menu over the 3-page stack. Pane height ≤ 800 px.

### Data
No store of its own. Reads F1's `references.db` and F3/F4/F7/F8/F9's TSVs. UI state in `QSettings 'ALL/TranslationTool'` via `sess::remember`: `references/search`, `/kind`, `/edition`, `/style`, `/leftSel`, `/sortColumn`. No-op under `g_harnessRun`.

### Engine
`app/references_pane.inc` — `class ReferencesPane : public QWidget`, with `int selfTest(QStringList&)` registered in `main()`'s `--selftest` sum (`app/main.cpp:41796-41802`). No new core module.

```cpp
class ReferencesPane : public QWidget {
public:
    ReferencesPane(const QString& dataRoot, allcore::Spine& spine,
                   allcore::Progress* progress,
                   std::function<void(const QString&, int)> openAtLine);
    void rebuildShelf(); void showRecord(const QString& key); void focusSearch();
    QString citationPreview(const QString& key) const;
    int selfTest(QStringList& log);
};
static std::function<void(const QString& key)> g_showInReferences;
static std::function<QString(const QString& filePath)> g_recordKeyForFile;
```

Presentation only. Rows come from `RefShelfIndex::query(ShelfQuery)`; left-tree counts are one aggregate query per node; smart-collection nodes run their stored query **at selection time** (they refill; they are never a frozen list). The card is composed HTML using `ux::sourceBadge`, mirroring `LibraryPane::showInfo` so the two cards read as one family.

**Reuses:** LibraryPane's filter-row + splitter + info-card layout (25957-26075) and its anchor dispatcher · `RibbonBar`/`RibbonGroup::addColumn` (5654-5822, `compare_pane.inc:630-736`) · `g_raisePane`, `g_openAtLine`, `g_openAuthorByPid`, `g_compareFiles`, `g_compareTexts` · `DossierStore` (dossier.h:28) · `Subjects::forWork` (subjects.h:45) · existing sidecar paths for the attachment list (nothing is copied).

### Acceptance tests (app `--selftest`)
1. Constructing the pane over a temp dataRoot with 5 library files and a stub bank, then `rebuildShelf()`, fills the table with 4 rows and the status line contains `"Showing 4 of 4 records"`.
2. Setting the search box to a title fragment reduces the table to 1 row; clearing restores 4; the "Showing" number matches the row count in **every** case.
3. Selecting a row renders a card whose HTML contains the REFERENCE badge and the literal `"catalog_works.json"` for the English title; with a bank that has none, the card contains the gap wording and **no** English title text.
4. **No dead controls:** with an **empty** selection every MARK/CITE/CHECK action is `isEnabled()==false` and its tooltip contains "select a record first"; with a **single** selection the per-record actions enable and "Compare with…" stays disabled; with a **multi** selection "Compare with…" enables only at exactly 2 rows. Asserted for all three selection states.
5. Ribbon `minimumSizeHint().width() <= 1180` (pane-local ratchet) **and** the app-wide check at 43675-43704 does not raise `kRibbonRatchet`.
6. Pane `minimumSizeHint().height() <= 800`.
7. Under `g_harnessRun`, "Rebuild shelf…" runs without opening a modal and the gauntlet random walk can trigger neither a network call nor a file write from this pane.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| The Research group reaches 7 tabs | The pane is reachable from Library and Files, so nobody has to hunt for it. Moving it to Read is a one-line `mkGroup` change if Adam prefers (open question 3) |
| The References ribbon becomes the app's widest | Pane-local 1180 px ratchet as an acceptance test; MORE ▾ over a 3-page stack; no big icon proxies |
| `main.cpp` is ~45k lines in one TU; a new `.inc` adds compile time | The pane is thin (table + card + ribbon); all logic lives in the core modules and is tested there |
| A table over 1,412 records is fine; over a 100k-file install it is not | `query()` takes a limit and the status line discloses it ("showing the first 2,000 — narrow the search") |

**Effort:** 16 h. **Depends on:** F1.

---

## F3 — Collections, smart collections and colour tags (`allcore::refshelf`)

**§17 verdict:** BUILD (row 2 — "user collections …, smart collections = saved Gofer/catalogue queries that refill, colour tags").

### Purpose and user story
As a translator working on three projects at once I want a shelf of my own — "Lam rim commentaries", "Pramāṇa — Tuesday group", "To be proofread" — and a shelf that maintains itself ("everything by Tsongkhapa I have not read"), so I stop rebuilding the same list by hand every month.

### UI
Left column of F2. Creation from FIND ▸ **Save as smart collection…** and MARK ▸ **Add to collection ▾ ▸ New collection…**; management from the left tree's context menu (Rename, Change colour, Edit query…, Show what this query finds now, Remove).

- **Add to collection ▾** — existing collections + "New collection…" (name + colour from the closed 7-colour palette); acts on the selection.
- **Save as smart collection…** — a dialog showing the query **in English** (`explainShelfQuery`: *"text contains 'lam rim' and it is a text and you have not read it"*), a live "matches N records now" button, name and colour.
- **Tags ▾** — checkable menu of existing tags + "New tag…"; a record may carry several; the `●` column shows up to 3 dots and "+N". **Every chip renders the tag NAME beside its colour** — colour is never the only channel (WCAG 1.4.1).
- Tag chips on the card — click removes the tag, written as a tombstone, not a deletion.

**Honesty labels.**
- Smart-collection node: *"A smart collection is a saved query. It shows what the query finds now, not a fixed list — if a text is renamed or a bank changes, the membership changes with it."* The English rendering sits beneath the name, always, and "N records now" is recomputed **on show**, never cached.
- Colour palette: *"Colours are yours and mean whatever you decide. They carry no provenance and are never exported into the catalogue or the dictionary."*
- Separator between the translator's tags and **Catalogued subjects** (REFERENCE, read-only).
- Orphans: *"3 records in this collection no longer have a file on disk (kept, not removed)."*

### Data
TSV, `tsvEscape`'d (`proposals.h:126`), a `#` header line naming the columns, conflicted-copy siblings absorbed on load and **counted** (`comments.h:41` idiom):

```
collections.tsv   # id  name  kind  color  query  by  at  state
                  # kind: manual|smart; query = serializeShelfQuery; state: live|removed
membership.tsv    # collection_id  record_key  state(in|out)  by  at     (append-only, last-wins)
tags.tsv          # id  name  color  by  at  state
record_tags.tsv   # record_key  tag_id  state  by  at                    (append-only, last-wins)
```

Colour vocabulary is **closed and validated** against the `ux_tokens.h` semantic palette (gold, act, warn, machine, error, doc, people). An unknown colour is refused with the offending value named — never silently defaulted.

**Location.** `<dataRoot>/library/references/` by default (private shelf). A Preferences ▸ File Locations entry "References shelf (shared)" can point the same files at the team proposals folder beside `proposals.tsv`/`comments.tsv`; when it does, the banner says "shared shelf — \<path\>" and the store reports conflicted-copy siblings. **In the same commit that creates them**, all shared files join the launch backup list (`app/main.cpp:38741`, `allcore::backupFile` keep 14) and Help ▸ "Restore a Shared Store from Backup…" (40966).

**Provenance fields.** `by` (`g_userName`, else `$USER`) · `at` (ISO 8601, **caller-supplied**, so batteries are deterministic) · `state` (tombstones instead of deletion, so a Dropbox union never resurrects or loses a judgment) · `RefShelfStore::conflictSiblings()` surfaced in the pane ("2 conflicted copies were merged by id; no row was discarded").

### Engine
`core/include/allcore/refshelf.h` + `core/src/refshelf.cpp`; battery `core/tests/refshelf_smoke.cpp` (temp dir, deterministic stamps), registered beside `proposals_smoke`.

```cpp
struct Collection { std::string id,name,kind,color,query,by,at,state; };
struct Tag { std::string id,name,color,by,at,state; };
class RefShelfStore {
public:
    explicit RefShelfStore(const std::string& dir);
    bool load(); [[nodiscard]] bool save(); size_t conflictSiblings() const;
    std::string createCollection(name,kind,color,query,by,at,std::string* err);
    bool renameCollection(...); bool setCollectionColor(id,color,std::string* err);
    bool setCollectionQuery(...); bool removeCollection(...);
    bool addToCollection(cid,key,by,at); bool removeFromCollection(cid,key,by,at);
    std::vector<std::string> membersOf(cid) const;
    std::vector<std::string> collectionsOf(key) const;
    std::string createTag(name,color,by,at,std::string* err);
    bool tag(key,tag_id,by,at); bool untag(key,tag_id,by,at);
    std::vector<std::string> tagsOf(key) const;
    const std::vector<Collection>& collections() const;
    const std::vector<Tag>& tags() const;
};
bool isRefColor(const std::string& color);
```

**Algorithm.** No inference. Append-only rows with last-wins resolution by (key tuple, `at`), ties broken by row id so the fold is deterministic on every machine — the same rule `ProposalStore` uses, and the same conflicted-copy absorption `CommentStore` performs (sibling files matching the base name are unioned by row identity; divergent rows are counted and reported, never merged by guess). Manual collections resolve to the record_keys whose newest row is `state=in`. **Smart collections hold a serialized `ShelfQuery` and are evaluated by `RefShelfIndex::query` at display time** — the refill is simply "run the query again", so there is no stale membership to reconcile.

**Reuses:** `tsvEscape`/`tsvUnescape` and `ProposalStore`'s folder-backed store shape · `CommentStore`'s conflicted-copy union and `conflictSiblings()` disclosure · `[[nodiscard]] save()` per `DossierStore`/`ActionLedger` (the SQA FAIL-2 lesson) · F1's query serialization and `explainShelfQuery` · `ux_tokens.h` palette · `backupFile` + Help ▸ Restore.

### Acceptance tests

**CTest — `refshelf_smoke`**
1. `createCollection("Lam rim commentaries","manual","gold","","ADA","2026-09-09T10:00:00")` returns a non-empty id; `save()`; a fresh store over the same dir loads it with every field byte-identical.
2. `addToCollection(id,"S00184")`, `addToCollection(id,"TD04210")`, `removeFromCollection(id,"TD04210")` → `membersOf(id)=={"S00184"}` and the file still holds **3** rows (append-only; tombstone, not deletion).
3. A sibling `membership (Adam's conflicted copy 2026-09-09).tsv` containing a row absent from the main file → `load()` unions it, `conflictSiblings()==1`, no row dropped.
4. Two rows for the same (collection_id, record_key) with different `at` → the later wins regardless of file order; identical `at` breaks by id, and the fold is identical across three shuffled input orders.
5. `createTag("urgent","neon",…)` returns `""` with `err=="unknown colour: neon"`; `createTag("urgent","warn",…)` succeeds.
6. A smart collection's stored query survives save/load and parses back to an equal struct.
7. Compaction: a file exceeding 5,000 rows is rewritten to winning rows plus a `# compacted <at>` line — and `backupFile` is called **first**; the test asserts the backup exists.

**app `--selftest`**
8. "New collection…" under `g_harnessRun` uses `docprops::askName`'s harness stub (returns empty) and creates nothing — no modal, no partial row.
9. A smart collection with query `text="lam rim"` shows 1 row; after a rebuild that adds a second matching record it shows 2 **without any user action** (it refilled), and its "N records now" line updated on show.
10. The smart-collection node's caption contains the English rendering from `explainShelfQuery`.
11. Every tag chip's accessible text contains the tag name (colour is not the only channel).

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A shared shelf makes colour tags social objects (my red is your urgent) | Tag names carry the meaning; colour is decoration and the label says so |
| Append-only files grow | Compaction at 5,000 rows, backup taken first (test 7) |
| A renamed file changes its `FILE:` key and orphans manual membership | Orphans are **shown, not deleted** ("3 records … no longer have a file on disk"), so the translator can re-point them |
| 7 colours is at the edge of distinguishable | Do not add more; the accessibility pass checks contrast in both themes |

**Effort:** 12 h. **Depends on:** F1, F2.

---

## F4 — Read state and "On my desk"

**§17 verdict:** ADAPT (row 3 — "'read' state per text per user; annotations stay comments (provenance)").

### Purpose and user story
As a translator with thousands of files I want to know which texts I have actually read, which I am in the middle of, and where I stopped — so the shelf tells me the truth about my own work rather than making me remember it.

### UI
The `◐` column in F2's table; the **Read ▾** ribbon menu (Unread · Reading · Read, multi-select aware); read-state buttons on the card (**Mark as reading** / **Mark as read** / **Clear** — the third is what makes the other two safe); the left tree's Unread/Reading/Read nodes and **On my desk**. Also a one-line read-state row on the Library pane's info card (`app/main.cpp:28039`) so the state is visible where texts are browsed today.

- "On my desk" lists records whose file has a dossier; the row shows "stopped at line 412" and double-click opens the text **at that line** via `g_openAtLine`.
- Card link: "Open the text at line 412" when a dossier exists; "Start a dossier for this text" when it does not.

**Honesty labels.**
- *"Read marks are yours (\<name\>). They are not a team judgment and nobody else's shelf changes."*
- `◐` tooltip: *"marked read by \<name\> on 2026-09-09 — the app never infers that you read something because you opened it."*
- "Recently opened" is a **separately named** node fed by `Progress::recentKeys("openfile")`, never conflated with read state.
- "On my desk" caption: *"the reading position your dossier remembers — the desk, not a claim about the text."*

### Data
`readstate.tsv  # record_key  user  state(unread|reading|read)  at` — append-only, last-wins by (record_key, user, at). A **Clear writes an explicit `unread` row**, so a cleared mark is a fact rather than an absence.

**Location:** `<dataRoot>/library/references/readstate.tsv` — **local by default even when the other shelf TSVs are shared**, because a read mark is one person's bookkeeping, not a team decision. When the shared-shelf preference is on, the `user` column keeps the shelves separate. Joins the launch backup list.

**Provenance:** `user` (`g_userName` else `$USER`) · `at` (ISO 8601, caller-supplied) · **no derived read state whatsoever**.

### Engine
Same module as F3 — one store, one save path.

```cpp
enum class ReadState { Unread, Reading, Read };
bool RefShelfStore::setReadState(key, user, ReadState, at);
ReadState RefShelfStore::readState(key, user) const;          // Unread when no row
struct ReadStamp { std::string user, at; };
std::optional<ReadStamp> RefShelfStore::readStamp(key, user) const;
std::vector<std::string> RefShelfStore::recordsInState(ReadState, user) const;
```

Last-wins append-only, identical to F3's resolution. "On my desk" is a **display join** between the shelf's `record_files` paths and `DossierStore::all()`'s textPath — computed live, stored nowhere.

**Reuses:** `DossierStore` (dossier.h:19-49) · `Progress::recentKeys` (progress.h:73) · F3's file discipline.

### Acceptance tests
**CTest — `refshelf_smoke`**
1. `setReadState("S00184","ADA",Reading,…)` → `readState("S00184","ADA")==Reading` and `readState("S00184","JB")==Unread` (states do not leak between users).
2. Read, then Clear → `readState==Unread` and the file holds **3** rows (Clear is recorded, not erased).
3. `recordsInState(Read,"ADA")` over 3 records with 1 read returns exactly that key.

**app `--selftest`**
4. With a dossier at line 412 for the S00184 file, "On my desk" lists 1 record, the card contains the literal `"line 412"`, and double-click calls `g_openAtLine(path, 412)` — asserted through a hook that records the last call.
5. **Opening a text through the table does NOT change its read state** (assert `readState` unchanged before and after).
6. Marking a record read writes only `readstate.tsv`; `collections.tsv` and `membership.tsv` are byte-identical before and after.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A shared readstate exposes who has read what | Local by default; the shared switch is explicit and the banner names it |
| `FILE:` keys change if a file is renamed, orphaning the mark | Orphans are shown, not deleted (same as F3) |

**Effort:** 5 h. **Depends on:** F3.

---

## F5 — Insert Citation (`allcore::citation`) with JSON citation styles

**§17 verdict:** BUILD (Cite While You Write) + ADAPT (output styles: "a handful of citation styles, editable JSON; CSL import LATER").

### Purpose and user story
As a translator writing a footnote I am at folio 12b line 3 of the Great Book on the Steps of the Path. I press **Cite this folio**, confirm the text and locator, and a correctly formed house-style citation lands in my draft — the **same wording as the published volumes**, not my ninth slightly different retyping of it.

### UI
**Three entry points, one dialog.**
1. Insert menu ▸ **"Citation…"** (`app/main.cpp:40159`, beside "Footnote from the Bank…"), routed to whichever of Draft/Manuscript is active by the existing `active()`/`frontDoc` lambdas. **⌥⌘C** (verified free).
2. Draft PUBLISH ribbon and Manuscript PUBLISH row: **Insert citation…**
3. Overlay text context menu (`app/main.cpp:8815-8830`, where the "folio %1 · line %2/%3" header already sits): **"Cite this folio"**, prefilled from `OverlayPane::resolveFolioAt` (`app/main.cpp:11985`; its selftest pins are at 6930-6948). **No fifth folio regex is written.**

Also References ▸ CITE ▸ Insert into Draft / Copy citation.

**Citation picker dialog (a `QDialog` inside the `.inc`; zero `QMessageBox`/`QInputDialog`, per the R3 census extended by H3):**
- Search box over the shelf with a results list; the record open in the Overlay, if any, is preselected and labelled "the text you are reading". Each row shows its **kind**, so a monograph can never be cited in the belief that it is a canonical work.
- Locator row: `[f.]` + folio + line, prefilled from `resolveFolioAt`; a "use the whole text" checkbox clears it.
- **Source choice (radio, top option first when it exists):**
  (a) *"Use the published entry, verbatim — \<source volume\>, entry \<id\>"* — offered whenever the record's ACIP number appears in a published entry's `acip_refs`;
  (b) *"Compose from the catalogue fields in the \<style\> template."*
- **Orthography row (the ACIP problem, solved explicitly).** When a composed citation would take its author or Tibetan title from the catalogue bank, the values are **uppercase ACIP** — verified: `catalog_works.json` S702 author is `RGYAL BA DGE 'DUN GRUB`. The dialog therefore offers three, and only three, honest routes, and refuses to guess between them:
  - **Type the published spelling** (a text field, remembered on the record as `typed`) — the default the dialog puts the caret in;
  - **Pair-hyphenate (machine — check it)** — a button the human presses, which runs `allcore::hgmTechnicalSpelling` and drops the result into the *editable* field badged MACHINE. It is **never** applied automatically and **never** called from inside `renderCitation`. The header itself says why: house technical spelling *"is entered, not derived — lexical hyphenation is not guessable"* (`tibexport.h:76-77`);
  - **Use the catalogue's ACIP, labelled** — renders `⟨ACIP: RGYAL BA DGE 'DUN GRUB — not the published spelling⟩` so the entry cannot be mistaken for published typography.
- Style combo: ALL house style (STD-007) · ALL house style — Sanskrit canonical (DCC) · Secondary literature (template) · any user style in `library/references/styles/`. Remembered in `references/style`.
- Live preview, selectable and copyable. **Insert** · **Copy** · **Cancel**; Insert is disabled with an explanatory line until a record is chosen.
- Preferences ▸ Citations: default style, "short markers in the Draft" switch, Reveal the styles folder, Reload styles with each file's parse result listed.

**Honesty labels.**
- Option (a): EVIDENCE badge + *"as published in \<source volume\>, entry \<id\> — used verbatim."*
- Option (b): *"Composed from the catalogue's own fields in the \<style\> template. The English title is the catalogue's published rendering; the app never translates a title."*
- No published English title → the citation carries the visible gap `⟨English title not published — type it⟩`, `complete=false`, and the dialog says *"this citation has a gap you must fill."* **Never** a machine translation, **never** a silent omission, **never** the Tibetan title promoted into the English slot.
- Style parse failure: *"styles/mla.json line 7: unknown key 'authorFormat' — this style was not loaded"*, listed in Preferences; the style simply does not appear in the combo.

**Fit-ratchet note:** two buttons on the Draft/Manuscript PUBLISH rows; no new ribbon group in either pane.

### Data
**Style files, JSON, one per style:**
```json
{ "id": "all-house", "name": "ALL house style (STD-007)", "version": 1,
  "builtin": "std007",
  "note_template": "", "bibliography_template": "", "short_template": "",
  "secondary_template": "",
  "locator_form": ", at {locator}",
  "sort": ["author","english_title"],
  "sections": ["Sanskrit","Tibetan","Secondary"],
  "en_dash_ranges": true }
```
`builtin` is `"std007"` | `"dcc_sanskrit"` | `""` (template-driven). **A style whose `builtin` is set MUST have empty templates**, enforced by `parseCitationStyle` — this is the mechanical guarantee that STD-007 has one implementation.

**In-document carriers — one grammar, two carriers, both scannable:**
- Draft (plain text): `[CITE S00184 f.12b.3 | <rendered text>]`
- Manuscript (Qt HTML): `<a href="cite:S00184|12b.3|all-house">rendered text</a>` — the anchor is the only construct proven to survive `toHtml`/`setHtml` (`ManuscriptPane::insertHyperlink`, `app/main.cpp:33583`; its round-trip selftest at 33810).

**Location.** Shipped: `data/citation_styles/all-house.json`, `all-house-sanskrit.json`, `secondary-basic.json`. User: `<dataRoot>/library/references/styles/*.json`. No store for the citations themselves — they live in the documents.

**Provenance.** A published entry's rendered text is byte-identical to the bank's own `text`, and the marker names the source volume and entry id · composed citations name the style id in the carrier and in the bibliography block header · style files carry `id`/`name`/`version` and the app reports which file each came from.

### Engine
`core/include/allcore/citation.h` + `core/src/citation.cpp`; battery `core/tests/citation_smoke.cpp`, self-contained fixtures.

```cpp
struct CitationStyle { std::string id,name,builtin,note_template,bibliography_template,
                       short_template,secondary_template,locator_form;
                       std::vector<std::string> sort_keys, sections; bool en_dash_ranges=true; };
bool parseCitationStyle(const std::string& json_text, CitationStyle*, std::string* err);
// strict FLAT subset: one object; string/bool/number values and arrays of strings; no
// nesting. An unknown key is an ERROR naming the key and the line. A style with a
// non-empty `builtin` and a non-empty template is an ERROR.

struct CiteField { std::string key, locator, style_id; };
std::string composeCiteHref(const CiteField&);      // "" when locator carries | ? & or a newline
bool parseCiteHref(const std::string&, CiteField*);
std::string composeCiteMarker(const CiteField&, const std::string& rendered);
bool parseCiteMarker(const std::string&, CiteField*, std::string* rendered);

struct CiteScan { std::vector<CiteField> fields; std::vector<std::string> malformed;
                  int legacy_markers = 0; };
CiteScan scanCiteFields(const std::string& text_or_html);

struct CitationOut { std::string text; bool complete=false; std::vector<std::string> gaps; };
CitationOut renderCitation(const CitationStyle&, const RefRecord&, const std::string& locator);
CitationOut renderPublished(const std::string& published_text, const std::string& source,
                            const std::string& entry_id, const std::string& locator);
```

**Algorithm — two paths, both assembly, never composition.**

*(a) Built-in house styles call the existing, battery-proven writers.* `builtin=="std007"` → `composeBibliographyEntry(BibliographyFields filled verbatim from the record)`; `builtin=="dcc_sanskrit"` → `composeSanskritBibEntry`. A locator is applied by the **one** defined transformation: strip the entry's single trailing `.`, append `locator_form` with `{locator}` substituted, restore the `.`. `hgmTechnicalSpelling` is **not** on this path.

*(b) Template styles* use a conditional-field template — the optional-group convention documented in Oren Patashnik, *BibTEXing* (1988): literal text plus `{…}` groups plus `<field>` placeholders. A group is emitted **iff every placeholder inside it is non-empty** (drop-the-whole-group), so an absent date leaves no orphan comma. One level of nesting. An **unknown field name is a style parse error**, not an empty expansion. `en_dash_ranges` applies the same `endashRanges` the house writer uses.

Field vocabulary is closed: epithets, author, author_dates, english_title, tibetan_title, number, folios, edition, collection, locator, publisher, place, year, journal, volume, issue, pages.

*Gaps:* a required field that is empty renders `⟨…⟩`, is listed in `CitationOut::gaps`, and sets `complete=false`.

`scanCiteFields` walks both carriers in one pass, **tolerates the legacy `[BIBLIOGRAPHY: …]` and `[NOTE: …]` markers already sitting in shipped drafts** (`app/main.cpp:23494-23501`) — counted as `legacy_markers`, left byte-untouched — and reports a malformed `[CITE …]` rather than skipping it or half-parsing it.

**Reuses:** `composeBibliographyEntry`/`composeSanskritBibEntry`/`BibliographyFields`/`SanskritBibFields` (tibexport.h:73-124) — *the house format itself* · `g_appBib` with `acip_refs` from H1 — the published-entry lane · `resolveFolioAt` (11985) and the existing folio header · DraftPane's insert-at-cursor idiom · `ManuscriptPane::insertHyperlink` (33583) · the strict-subset JSON parsing precedent already in core (`spine.cpp`'s `jsonStringArray`) — no new dependency.

### Acceptance tests

**CTest — `citation_smoke`**
1. **The house format has exactly one implementation.** `renderCitation(house, record{author:"Pabongka Rinpoche", author_dates:"1878-1941", english_title:"Liberation in Our Hands", tibetan_title:"rNam-grol Lag-bcangs", number:"S00184", folios:"1a-11a"}, "")` returns **byte-identical** output to `composeBibliographyEntry` on the same fields — including the **two spaces** after the dates period and the **en dashes** `endashRanges` produces in *both* the dates and the folios.
2. The same call with `locator "f. 12b.3"` and `locator_form ", at {locator}"` inserts `", at f. 12b.3"` before the final period.
3. `tibetan_title` empty, `acip_number` present → the output contains `" (ACIP S00184)"` — the separator switch the writer performs — proving the built-in path is not a template.
4. `author_dates` empty → `"Pabongka Rinpoche.  Liberation in Our Hands …"` — no orphan comma.
5. `english_title` empty → `complete==false`, `gaps=={"english_title"}`, text contains `⟨English title not published — type it⟩`, and the Tibetan title is **not** in the English slot.
6. **`hgmTechnicalSpelling` is never reached from `renderCitation`** — asserted by a link-level test that a record carrying raw wylie in `author` renders that wylie verbatim, unchanged.
7. `parseCitationStyle` of `"authorFormat": "x"` → false with `err` containing `unknown key 'authorFormat'`; a style with `builtin:"std007"` **and** a non-empty `bibliography_template` → false with `err` naming the conflict; a valid fixture parses with `sort_keys.size()==2`.
8. Template style `"{<author>, }{<year>. }<title>."` renders `"Dreyfus, 1997. Recognizing Reality."` and, with year empty, `"Dreyfus, Recognizing Reality."` (group dropped whole).
9. `composeCiteHref({"S00184","12b.3","all-house"}) == "cite:S00184|12b.3|all-house"` and round-trips; a locator containing `|`, `?`, `&` or a newline is **refused** (returns `""`) rather than producing an unparseable field.
10. `scanCiteFields` over a draft with 2 `[CITE …]`, 1 truncated `"[CITE S00184 f.1a"` and 1 legacy `[BIBLIOGRAPHY: …]` → `fields==2`, `malformed==1`, `legacy_markers==1`, and the legacy marker's bytes are unchanged in the input.
11. `renderPublished(text,"A Song of My Spiritual Life…","S1","")` returns the published text **byte-for-byte**, `complete==true`.

**app `--selftest`**
12. Insert ▸ Citation… with the Draft active and a fixture record inserts a marker; `draftText()` then satisfies `scanCiteFields` with exactly 1 field whose key is the fixture key.
13. Overlay ▸ Cite this folio with the caret after `@012B` and two newlines prefills locator `"12b.3"` (asserted through the dialog's harness accessor; the dialog itself returns early under `g_harnessRun`).
14. Insert a citation in the Manuscript, save to a temp `.html`, reopen via `openFile` → `parseCiteHref` on the anchor yields the same key and locator (the anchor round-trip the existing selftest at 33810 already proves for plain anchors).
15. A record whose number appears in `apparatus_bibliography.json`'s `acip_refs` makes the published-entry radio the **preselected** option and the preview equals the bank text.
16. A record whose author came from `catalog_works.json` shows the ACIP-orthography row with the "Type the published spelling" field focused, and Insert with the ACIP left as-is produces the labelled `⟨ACIP: …⟩` form, never a bare uppercase author inside a house entry.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Two conventions in one draft (legacy `[NOTE:]`/`[BIBLIOGRAPHY:]`) | Designed in: counted as legacy, never rewritten; F6 reports them. Migration only as a separate, previewed action if Adam asks (open question 6) |
| `QTextFormat::UserProperty` does not survive `toHtml`, so the Manuscript field must be an anchor; editing the anchor's visible text desynchronises it | F6's regeneration re-renders every field's visible text and **reports** the ones it changed — drift corrected and disclosed, never hidden |
| The Draft marker is long and visible in prose | Deliberate and honest (it matches the existing bank markers). Preferences switch "short markers in the Draft" writes `[CITE S00184 f.12b.3]` alone, with the rendered text living only in the bibliography block |
| A style that parses but is nonsense (empty templates) yields empty citations | `parseCitationStyle` requires a non-empty `note_template` **or** a `builtin`, else `err` |
| Uppercase ACIP author/title leaking into a published-looking entry | Three explicit routes, the machine one human-pressed and editable, the ACIP one visibly labelled (test 16) |

**Effort:** 18 h. **Depends on:** F1, H1.

---

## F6 — Regenerating bibliography block

**§17 verdict:** BUILD — "a bibliography section regenerates from the fields".

### Purpose and user story
As a translator finishing a chapter I want the bibliography at the end to be exactly the works I cited, in the house order, sectioned like the published volumes — regenerated in one click, and telling me loudly about any citation whose text is no longer on my shelf.

### UI
Draft PUBLISH ribbon and Manuscript PUBLISH row: **Bibliography ▾** — Insert bibliography here · **Update bibliography (⌥⌘B)** · Go to bibliography · Remove bibliography. Mirrored in the Insert menu beside Citation….

- **Insert bibliography here** — disabled with the reason shown when a block already exists ("this document already has a bibliography — use Update").
- **Update bibliography** — rescans, re-renders, replaces the block contents in **one undo step** (`beginEditBlock`/`endEditBlock` in the Manuscript; a single splice in the Draft).
- **Remove bibliography** — deletes the block only, with a confirm; citations in the body are untouched.
- Report line, in the status bar and at the head of the block: *"12 entries from 17 citations · 3 published entries used verbatim · 1 citation could not be resolved (listed)."*

**Honesty labels.**
- Block header: *"Regenerated from the citations in this document, \<style name\>, \<ISO date\>. Entries marked 'as published' are reproduced verbatim from the volume named."*
- Unresolved keys appear **IN** the block: `⟨unresolved citation: S99999 — no record with that number on the shelf; the citation is still in the text at line 214⟩`. Never dropped.
- Mixed styles reported, not silently unified: *"2 citations use a different style (chicago-notes); they were rendered in their own style."*
- Drift: *"3 citation fields whose visible text had been edited were re-rendered; their fields were unchanged."*
- Legacy: *"3 legacy [BIBLIOGRAPHY: …] markers found in this draft. They are plain text, not citation fields, and are not in the regenerated list."*

### Data
Delimiters, both scannable and both surviving their carrier:
- Draft: `[BIBLIOGRAPHY-START style=all-house generated=2026-09-09T18:40]` … `[BIBLIOGRAPHY-END]`
- Manuscript: `<a name="all-bib-start"></a>` … `<a name="all-bib-end"></a>` (named anchors round-trip)

No store: the bibliography is a rendering of the document's own fields plus the shelf. **Provenance:** style id and generation timestamp in the header; per-entry "as published in \<source\>, entry \<id\>"; unresolved keys listed with the line number of the citation that referenced them.

### Engine
`core/include/allcore/citation.h` (same module as F5).

```cpp
struct BibEntryOut { std::string key, section, text; bool resolved=false;
                     std::string why_unresolved; bool published=false;
                     std::string published_source, published_id; };
struct BibliographyOut { std::vector<BibEntryOut> entries; int citations=0, resolved=0,
                         published=0; std::vector<std::string> other_styles; };
BibliographyOut renderBibliography(
    const CitationStyle&, const std::vector<CiteField>&,
    const std::function<const RefRecord*(const std::string&)>& lookup,
    const std::function<bool(const std::string& key, std::string* text,
                             std::string* source, std::string* id)>& published_lookup);
std::string bibliographyBlockText(const BibliographyOut&, const CitationStyle&,
                                  const std::string& generated_iso);
struct BlockSpan { int begin=-1, end=-1; };
BlockSpan findBibliographyBlock(const std::string& text_or_html);
```

**Algorithm — deterministic assembly.** Scan once (`scanCiteFields`); deduplicate by record key preserving first-appearance order for the report; **follow `merged_into` when a key resolves to a record that was merged** (F7), so a citation already saved in a shipped Manuscript keeps resolving; prefer the published entry when `published_lookup` finds one for that number (rule 1: match, never compose); render the rest with the style's template or built-in writer; group into the style's sections (published entry's own section when there is one, else "Tibetan" for records with an ACIP number, "Sanskrit" when a Sanskrit title is present, "Secondary" for `kind==secondary`); sort within each section by the style's sort keys with a **stable, locale-free** comparison on the raw field strings. Unresolved keys are emitted as entries with `resolved=false` so the caller cannot drop them by accident. Block replacement is a pure span computation so the app-side edit is a single splice.

**Reuses:** F5's scan/render and the published lane · `g_appBib` and its `acip_refs` join (H1) · `ManuscriptPane`'s cursor helpers (33562-33600) and DraftPane's plain-text splice · `saveOrWarn` on save (WP-1).

### Acceptance tests

**CTest — `citation_smoke`**
1. `renderBibliography(house, {S00184, TD03971, S99999}, …)` where TD03971 has a published entry → 3 entries; the TD03971 entry is `published==true` with text byte-identical to the bank; the S99999 entry has `resolved==false` and `why_unresolved=="no record with that number on the shelf"`; `citations==3, resolved==2, published==1`.
2. Two `CiteField`s with the same key and different locators produce **one** bibliography entry (locators belong to the notes).
3. Entries group into sections and sort by author then english_title; the same input in a different order produces **byte-identical** output (stable, locale-free).
4. `bibliographyBlockText` → `findBibliographyBlock` covers exactly the generated block; re-running over a document that already contains the block produces the same block — **idempotent**, no nesting, no duplication, and the bytes **outside** the delimiters are byte-identical.
5. Two style ids in one document → `other_styles=={"chicago-notes"}` and every entry still rendered.
6. **Merged-record resolution:** a `cite:` key whose record was merged into another resolves through `merged_into` and renders the surviving record's entry.

**app `--selftest`**
7. Draft with 2 citations → Insert bibliography → `draftText()` contains both entries and the header; Update again → byte-identical text and exactly one `[BIBLIOGRAPHY-START]`.
8. Manuscript with 1 citation → Insert bibliography → save → reopen (`setHtml`/`toHtml` round trip) → `findBibliographyBlock` still finds the block and the citation anchor still parses.
9. Deleting a citation from the body and updating removes its entry; the report says "1 entry removed".
10. A draft containing a legacy `[BIBLIOGRAPHY: …]` marker: after Update, that marker's bytes are unchanged and the report line contains "1 legacy" and "not citation fields".

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A translator hand-edits inside the block and loses it on Update | The header says so in words; Update is never automatic; the pre-update text is one editor-undo away (single edit block) |
| Foreign HTML could contain an anchor named `all-bib-start` | `findBibliographyBlock` requires **both** anchors in order and refuses (span -1) otherwise, with the status line saying why |
| Sorting wylie author strings by byte order is not Tibetan alphabetical order | The style declares its sort keys and the header names the order used. A true Tibetan collation is out of scope and is **not claimed** |

**Effort:** 8 h. **Depends on:** F5.

---

## F7 — Duplicates finder with side-by-side keep/merge

**§17 verdict:** BUILD (Find Duplicates — "duplicate/near-duplicate records … side-by-side keep/merge with provenance").

### Purpose and user story
As a translator about to start a three-month commentary I want to know, **before I begin**, whether this text is already on my shelf under a different filename — and if two records are really one work with two witnesses, I want to say so once and never be asked again.

### UI
References pane **page 1 (Duplicates)** — a page in the stack, not a modal, so the translator can keep working. Also linked from Catalog ▸ QC (which has the file-level `qcDuplicateTitles` lane) as "…and the record-level duplicates are in References".

- Scope row: Whole shelf / This collection / Selected records · title-similarity slider (0.60–1.00, default 0.75, the number shown) · **Find duplicates**.
- Group list (left): `"3 records · title 0.94 · colophon 1.00 · same colophon — duplicate records"`.
- Side-by-side (right): two records at a time, field by field, each value with its source badge; differing fields highlighted with `allcore::textdiff::diffInline` (`textdiff.h:66`) so a one-syllable title difference is visible.
- Per-record: **Open the text** · **Compare the files…** (`g_compareFiles` — the real "which is the better witness" tool) · **Show in Library**.
- Group: **These are one work** (writes a same-work link; the shelf then shows one record with N witnesses) · **These are distinct** (durable, symmetric ruling; the pair never re-flags) · **Propose a catalogue correction…** (`ProposalStore::propose(CatalogIdentity, …)` prefilled with the evidence string) · **Leave for now**.
- **Save report…** — Markdown or CSV of the groups with their evidence.

**Honesty labels.**
- Page banner: *"A flag is a question, not a verdict."* (verbatim from `catalog_qc.h`'s own doctrine.)
- Verdict vocabulary **shared with** `qcDuplicateTitles` (`catalog_qc.h:26-57`) so the two surfaces speak one language: "identical files (same bytes)" · "same title, same colophon — duplicate records" · "same title, different colophons — distinct works sharing a title" · "near-identical titles — no colophon evidence, needs a human read".
- Evidence line on every group: *"title 0.94 (shared syllables over the longer title) · colophon 1.00 · 2 files · same ACIP number: no · threshold in force: 0.75."*
- **"compared raw"** on any pair whose title is not ACIP (`title_is_acip==false`): *"this title is not ACIP; it was compared raw, not normalised."*
- Above the group actions: *"This pane never renames, moves or deletes a text. 'These are one work' records YOUR judgment on your shelf. Changing the catalogue goes through a proposal."*
- Records with no comparable title are **reported**, never implied unique: *"41 records had no title to compare (person, place or secondary literature)."*
- Slider caption: *"Lowering the threshold finds more candidates and more false ones."*

**Fit-ratchet note:** the Duplicates page contributes no ribbon groups; it is reached from MORE ▾ and the page switcher.

### Data
`record_links.tsv  # id  key_a  key_b  relation(same-work|distinct)  evidence  by  at  state`
`key_a`/`key_b` sorted so a pair has one identity; **`evidence` freezes the similarity numbers and the threshold as they were on screen when the human ruled**, so a later re-run at a different threshold cannot silently reinterpret an old judgment. `state`: live|removed (a reversed ruling is a tombstone row). Append-only, last-wins; `tsvEscape`'d; conflicted-copy union; `[[nodiscard]] save`; launch backup + Help ▸ Restore, in the same commit.

**Location:** `<dataRoot>/library/references/record_links.tsv`, shared when the shared-shelf preference is on (this is exactly the kind of judgment a team benefits from sharing).

### Engine
`core/include/allcore/refdup.h` + `core/src/refdup.cpp`; battery `core/tests/refdup_smoke.cpp` with **self-contained fixture records** (no dependency on the gitignored `library/`), registered in `core/CMakeLists.txt` with no fixture label.

```cpp
struct DupMember { std::string key, file, title_norm, colophon, sha1; bool title_is_acip=true; };
struct DupGroup { std::vector<DupMember> members; double title_sim=0.0, colophon_sim=-1.0;
                  bool same_bytes=false, compared_raw=false;
                  std::string verdict, evidence; };
struct DupOptions { double title_floor=0.75; int colophon_k=5; int max_groups=200;
                    std::set<std::pair<std::string,std::string>> ignore_pairs; };
struct DupScan { int records=0, no_title=0; long long candidates=0, comparisons=0;
                 bool cut=false; double threshold=0.0; };
std::vector<DupGroup> findDuplicateRecords(const std::vector<RefRecord>&,
                                           const DupOptions& = {}, DupScan* = nullptr);
std::string dupSummary(const DupScan&);   // honest header line, one place
std::string dupReportMarkdown(const std::vector<DupGroup>&);
std::string dupReportCsv(const std::vector<DupGroup>&);
```

**Algorithm — published, self-contained, and deliberately smaller than §8's machinery.**
1. **Fold witnesses first.** F1 already folds files sharing a catalogue key into one record, so the Sera Mey / Ganden pair never reaches the finder. The finder sees only different-number and unrecognised cases.
2. **Normalise** every title with `allcore::normalizeAcipTitle` — *only when `title_is_acip`*. A Unicode-Tibetan or mixed-script title is routed through the canonical converters first; where conversion fails it is scored **raw** and the group carries `compared_raw`.
3. **Blocking:** bucket records by shared title syllables — the classic record-linkage blocking step (Newcombe et al. 1959; Fellegi & Sunter, *A Theory for Record Linkage*, JASA 1969) that keeps the comparison from being all-pairs, with the comparison count capped and the cap **disclosed**.
4. **Score with the exported `allcore::titleOverlap`** (H2) — shared distinct syllables over the **longer** syllable set, the identical function `suggestIdentity` uses, so the two can never disagree. Colophons are compared with 5-syllable shingles and the Jaccard index (Jaccard 1912; the resemblance formulation of Broder, *On the Resemblance and Containment of Documents*, SEQUENCES 1997).
5. **Group** pairs at or above `title_floor` with a disjoint-set forest (Tarjan, JACM 22(2), 1975); the group's evidence reports the pairwise **minimum** similarity, which is the honest summary of a transitive group.
6. **Verdict by evidence class, never by score alone**: identical bytes → "identical files"; colophons present and ≥0.90 → "duplicate records"; colophons present and <0.50 → "distinct works sharing a title"; a missing colophon on either side → "no colophon evidence — needs a human read".
7. `ignore_pairs` (the "distinct" rulings) are removed **before** grouping so a settled question never returns.

**MinHash/LSH is deliberately not used** — see *Deliberately omitted*.

**Reuses:** `titleOverlap` (H2, exported not copied) · `normalizeAcipTitle` · `qcDuplicateTitles`' verdict vocabulary and colophon-aware doctrine · `findColophonCandidates` (colophons already stored on the record by F1) · `textdiff::diffInline` for field highlighting · `g_compareFiles`/ComparePane for the file-level comparison (**no second diff view**) · `ProposalStore::propose(CatalogIdentity, …)`.

**The engine has no filesystem write path at all.**

### Acceptance tests

**CTest — `refdup_smoke`**
1. Two records with identical `title_norm` and identical colophon → 1 group, `title_sim==1.0`, `colophon_sim==1.0`, verdict "same title, same colophon — duplicate records".
2. Identical titles, colophons sharing no shingles → "same title, different colophons — distinct works sharing a title", `colophon_sim==0.0`.
3. Identical titles, both colophons empty → `colophon_sim==-1.0`, "near-identical titles — no colophon evidence, needs a human read".
4. Titles of 10 syllables differing in 1 → `titleOverlap ≥ 0.75` and the pair groups; differing in 5 → below floor, no group.
5. A~B and B~C but A≁C → exactly 1 group of 3, and the evidence contains the pairwise **minimum**, not the maximum.
6. `ignore_pairs` with (A,C) still groups A,B,C via B; adding (A,B) and (B,C) yields 0 groups. Suppression is **symmetric**: passing (B,A) suppresses the same pair.
7. **Blocking loses nothing (the test that earns the claim):** 200 synthetic records scored by the blocked finder **and** by a brute-force all-pairs scorer written in the test → the two pair **sets** are identical.
8. 2,000 synthetic records with 200 planted duplicates → all 200 groups found, `scan.cut` false, and the call completes under a generous 2 s ceiling.
9. With `max_groups` reduced below the true count → `scan.cut==true` and `dupSummary()` contains the comparison count and the word "cap"; never a bare pair count.
10. Records with empty titles → counted in `scan.no_title` and `dupSummary()` says "N records had no title to compare"; they appear in no group.
11. A record with `title_is_acip==false` whose conversion fails → its group carries `compared_raw==true` and the evidence says "compared raw".
12. `dupReportCsv` has header `group,verdict,title_sim,colophon_sim,threshold,key,file` and quotes cells containing commas (the `apparatusCsv` escaping style).

**app `--selftest`**
13. Find duplicates over a 4-record fixture lists 1 group; **These are distinct** writes a `record_links` row and re-running lists 0 groups; the ruling survives a store reload and its evidence string contains the threshold that was in force.
14. **These are one work** makes the shelf show one row with "2 files" for that work, and **no file on disk was renamed, moved or deleted** — asserted by comparing the full directory listing **and every file's SHA-1** before and after.
15. Under `g_harnessRun` the merge confirmation returns `harnessAutoConfirm_` (false unless the selftest sets it), so a gauntlet random click can never apply a ruling (`compare_pane.inc:1126` pattern).

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| "Merge" could be read as "delete a text" | Structural: `refdup` has no filesystem write path; the only catalogue route is a proposal; the confirm says in words that both records and both files remain (test 14) |
| Colophon extraction is heuristic; an undetected colophon leaves a pair looking near-duplicate forever | The verdict says exactly that, and the group can be ruled distinct once |
| A low threshold on a large shelf produces a list nobody reads | Default 0.75, `max_groups` 200 with the cut disclosed, blocking cheap enough to retry |
| Two witnesses of one work are the normal case here | F1 folds them before the finder ever sees them |

**Effort:** 12 h. **Depends on:** F1, F3, H2.

---

## F8 — Refresh from BDRC, shown ours-vs-theirs

**§17 verdict:** ADAPT (Find Reference Updates — "fetch the record, show ours vs theirs side by side (Compare pane), update all / empty / chosen fields; nothing overwritten silently").

### Purpose and user story
As a translator writing a bibliography entry I find the catalogue has the author but no dates. BDRC has them. I want to pull the two facts I need, see exactly what would change **before** it changes, and have the record remember that those two fields came from BDRC — without anything else on my shelf being touched.

### UI
References pane **page 2 (Import & Refresh)**, right half; also on the record card as **Refresh from BDRC…**, enabled only when exactly one record is selected.

- Network notice + **Fetch** — the request is never made until this is pressed.
- **Resolution.** For KD/KL/TD numbers the mapping is deterministic (`allcore::bdrcScanUrl`, tibexport.h:62-71) and the resolved `bdr:MW…` id is shown, with the app's existing `bdrcScanUrlChecked` concordance tiers carried through. Otherwise the **shipped BLMP title-search dialog** runs (`app/main.cpp:11639-11730`, whose query is at 11672), with the title converted ACIP → EWTS through `allcore::acipToEwts` — the transliteration boundary rule from `docs/BDRC_LINKING_DESIGN.md`. Nothing is auto-selected. **No by-id endpoint is invented**: any by-id template stays disabled until it is verified against the live service.
- Ours-vs-theirs table: field | ours (+ source badge) | theirs (+ `bdr:MW…`) | a checkbox per differing field; unchanged fields collapsed behind "12 fields agree — show".
- Mode radio: **Update empty fields only (default)** · Update the fields I ticked · Update all differing fields. **Every row defaults to keep-ours**, so pressing Apply without touching anything changes nothing.
- **Open in Compare pane** — builds two `recordAsFieldText` renderings and calls `g_compareTexts("<key> — ours", …, "<key> — BDRC MW…", …)`, so the shipped Compare pane does the side-by-side exactly as §17 intends. No second diff view.
- **Apply** · **Cancel**; Apply disabled until at least one field would change, with the reason shown.
- Afterwards: each taken field renders with a REFERENCE badge naming `bdr:MW…` and a **Revert this field to the catalogue** link.
- **Refresh log…** — what was fetched, when, by whom, and what changed.

**Honesty labels.**
- Before the fetch: *"This sends this text's catalogue number (or its title, converted to Wylie) to purl.bdrc.io. Nothing else leaves your machine, and nothing is fetched until you press Fetch."*
- On results: *"BDRC's record is reference material. Nothing has changed yet."*
- After Apply: *"3 fields now come from bdr:MW22084_184 (marked on the record). 9 fields were left as the catalogue has them. The catalogue itself was not changed."*
- Failure, in the existing honest family (`bdrcErrorLine`, 11679): *"BDRC could not be reached — \<reason\>. Nothing was changed."* / *"BDRC replied, but not with JSON — the service may be degraded."*
- Title-search resolution, verbatim from the shipped scans dialog: *"No catalogue mapping exists for this text, so the record must be found by title and confirmed by you — the match is never asserted automatically."*
- Other-edition caveat carried through (`g_sungbumOtherEdition`, `app/main.cpp:5883`): *"this identifier points to another edition of the work."*
- Unmapped predicates are counted and shown: *"BDRC returned 14 fields; 6 were understood."*

### Data
```
overrides.tsv    # record_key  field  value  source  by  at  state(live|reverted)
refresh_log.tsv  # at  by  record_key  endpoint  resolved_id  mode  changed_fields  outcome
                 # outcome: applied|no-change|network-error|refused
```
`source` e.g. `bdr:MW22084_184`. **Overrides are applied on READ**, on top of the bank values, so a rebuild of `references.db` neither loses them nor bakes them in.

**Location:** `<dataRoot>/library/references/`. **Never** `data/extracted/*.json` — those are release-owned and rebuilt by `tools/*.py`.

**Provenance:** exact `bdr:` identifier · `by`/`at` · `state` (a revert is a row, so a field's history is legible) · **every fetch logged, including the ones that changed nothing and the ones that failed**.

### Engine
Pure merge in `allcore` (F1's `compareRecords`/`applyMerge`/`recordAsFieldText`); the JSON fetch and the shallow binding-to-field pick stay **app-side** in `app/references_pane.inc` (`QNetworkAccessManager` + `QJsonDocument`), following the `authorsearch` precedent that allcore takes no JSON or network dependency. Override storage in `refshelf.h`:

```cpp
struct Override { std::string record_key, field, value, source, by, at, state; };
bool RefShelfStore::setOverride(key, field, value, source, by, at);
bool RefShelfStore::revertOverride(key, field, by, at);
std::vector<Override> RefShelfStore::overridesFor(key) const;
void applyOverrides(RefRecord&, const std::vector<Override>&);  // sets value AND source/tier
// app side:
static std::map<QString,QString> bdrcBindingToFields(const QJsonObject& binding);
void ReferencesPane::fetchBdrc(const QString& key);   // user-initiated only; no-op under harness
```

**Algorithm.** Deterministic resolution first, human confirmation second. The BLMP reply is a flat `results.bindings` array, so the app-side extraction is a shallow field pick — not a JSON-LD graph walk — feeding a small **explicit mapping table** (author label, dates, titles, edition). An unmapped predicate is counted and shown, never coerced. `compareRecords` produces the deltas; `applyMerge` produces the new record, the changed list, and any **"refused to blank \<field\>"** warnings; the app writes one override row per changed field.

**Reuses:** `bdrcScanUrl` + `bdrcScanUrlChecked` with its concordance tiers (5832-5900) · the shipped BLMP dialog, its EWTS conversion, its error wording and its `jsonObjectOrFlag` guard (11639-11730) · the **existing** `library/bdrc_links.json` user-confirmed link store (11610) — **no second link store** · `acipToEwts` · `g_compareTexts` · F1's pure merge · F3's file discipline.

### Acceptance tests
**CTest** — covered by `refrecord_smoke` tests 10–12 (EmptyOnly / All / Chosen; refuse-to-blank; stable field text) and:

**`refshelf_smoke`**
1. `setOverride("S00184","author_dates","1878-1941","bdr:MW22084_184","ADA",at)` → `applyOverrides` sets the value **and** `author_dates.source=="bdr:MW22084_184"` with tier `reference`; `revertOverride` writes a row and `applyOverrides` then leaves the bank value untouched, with **2** rows in the file.

**app `--selftest`**
2. Under `g_harnessRun`, `fetchBdrc()` returns immediately, issues **zero** network requests (asserted by a counting stub installed on the pane's `QNetworkAccessManager`, comparing the request count before and after) and writes nothing.
3. With a stubbed "theirs" record injected, Apply in **EmptyOnly** mode writes exactly 1 override row; the card shows the `bdr:` source badge; **Revert** restores the catalogue value and leaves 2 rows in `overrides.tsv`.
4. Apply with **nothing ticked** in Chosen mode writes zero rows and the log records `outcome=no-change`.
5. A rebuild of `references.db` after an override still shows the overridden value (applied on read, not baked in).
6. **Open in Compare pane** calls `g_compareTexts` with names `"<key> — ours"` and `"<key> — BDRC MW…"` (recording stub).
7. A record with **no** identifier: the action is disabled, its tooltip contains "has no BDRC identifier", and no request is attempted.
8. A stubbed non-JSON reply produces the existing degraded-service sentence, not an empty list, and `refresh_log.tsv` gains a `network-error` row.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| Silent network use would violate the offline-core rule and `docs/PRIVACY.md` | User-initiated only, one record at a time, one explicit notice before the request, a Stop, a log of every fetch, and a hard no-op under the harness (test 2). No background job, no startup check, no telemetry |
| BDRC's record is another catalogue's opinion and could contradict the ALL catalogue the team's numbers depend on | Overrides are local to the shelf, never written to `data/extracted`, the field visibly says where it came from, and the default mode only fills gaps |
| Title-search resolution can attach the wrong work | No auto-selection; the existing "confirmed by you" wording; the resolved id in the log so a wrong link is traceable and revertible |
| KL numbering is not H numbering; Sungbum links can be other-edition | The tier warning travels with the row and into the evidence string |
| JSON-LD/binding shape drift | Unmapped predicates are counted and shown, so a shape change reads as a smaller harvest, not as wrong data |
| Licensing of fetched BDRC values stored in a shared team folder | Open question 8 — a factual check before 6b ships, not a design decision |

**Effort:** 10 h. **Depends on:** F1, F2, F3.

---

## F9 — BibTeX / RIS import and export (`allcore::bibimport`)

**§17 verdict:** ADAPT (Import filters — "import BibTeX/RIS/Zotero RDF for secondary literature"). Zotero RDF deliberately omitted.

### Purpose and user story
As a translator writing an introduction I cite Dreyfus, Cabezón and Ruegg alongside the canonical texts. Those references live in a colleague's Zotero, which exports BibTeX. I want to import them once, cite them from the same picker as my Tibetan texts, and have them appear in the same regenerated bibliography — and I want to hand a colleague my list back in their format.

### UI
References pane **page 2 (Import & Refresh)**, left half; also Library ▸ CARE ▸ "Import references (BibTeX/RIS)…" so it sits beside the import lanes translators already know.

- **Import BibTeX/RIS…** — file picker (`safeGetOpenFileName`, `*.bib *.bibtex *.ris *.txt`), then a **PREVIEW** dialog: every entry that parsed (type, author, title, year) with a checkbox each, plus a separate section listing every entry **REFUSED** with its line number and reason. Nothing is written until **Import checked**.
- An entry whose raw text hashes to an existing record shows "already imported (unchanged)" with its checkbox off.
- **Save the problem list…** (plain text, via `saveOrWarn`) so a malformed `.bib` can be fixed at its source.
- **Export ▾** — BibTeX / RIS / CSV of the visible rows or the selection, through `saveOrWarn`.
- Imported records appear under a **Secondary literature** kind node, are taggable and collectable, and are citable through F5 with the style's `secondary_template`.

**Honesty labels.**
- Preview header: *"12 entries read · 2 refused (listed below) · 1 already on the shelf · 3 tags kept as extra."* Never a bare success count.
- Refusals are specific: *"line 42: @book{dreyfus1997 — unbalanced braces, entry not imported"*, *"line 87: RIS entry ends at end of file without ER — not imported"*, *"line 55: undefined macro 'suny' — not imported"*.
- Record badge: REFERENCE + *"imported from \<file\> (BibTeX) on 2026-09-09 — secondary literature."*
- Standing line: *"Imported references are secondary literature. They are never merged into the dictionary, the corpus or the catalogue."* The card says: *"This is not a catalogue text. It has no ACIP number and no folio range."*
- *"Values are preserved as written; the app does not re-case or re-punctuate them."*
- Encoding: the detected encoding is named in the preview header (decoded through the existing `enc` namespace, `app/main.cpp:4966`, with refusal on unmappable bytes) — never an assumed UTF-8.

### Data
```
imported.tsv  # key  type  author  title  journal  book_title  publisher  place  year
              # volume  issue  pages  editor  translator  edition  doi  url  isbn  note
              # source_format  source_file  by  at  raw  state
```
`key = "IMP:" + first 8 hex of sha1(raw entry text)` — stable across re-imports, so a second import of the same file changes nothing. `raw` is the source entry **verbatim**, `tsvEscape`'d, so an import is always re-derivable and auditable. **Unmapped BibTeX fields and RIS tags are preserved individually** as `extra:<TAG>` columns appended to `note` with their original tag name and counted — not folded into one blob.

**Location:** `<dataRoot>/library/references/imported.tsv`; joins the launch backup list. Records materialise into `references.db` on rebuild with `kind="secondary"`. Imported source files are kept verbatim at `<dataRoot>/library/references_imports/<iso>-<name>` so the provenance claim can be re-checked years later.

### Engine
`core/include/allcore/bibimport.h` + `core/src/bibimport.cpp`; battery `core/tests/bibimport_smoke.cpp` with **inline fixture strings** (no external file), registered in `core/CMakeLists.txt`.

```cpp
struct ImportedRef { std::string type, author, title, journal, book_title, publisher, place,
                     year, volume, issue, pages, editor, translator, edition, doi, url, isbn,
                     note, raw, source_format, cite_key;
                     std::map<std::string,std::string> extra; };
struct ImportReport { std::vector<ImportedRef> records; int entries_seen=0, unmapped_tags=0;
                      std::vector<std::string> refused; };  // each carries line number + reason
ImportReport parseBibtex(const std::string& text);
ImportReport parseRis(const std::string& text);
ImportReport importReferences(const std::string& text);   // sniffs; refuses honestly when neither
std::string writeBibtex(const std::vector<ImportedRef>&);
std::string writeRis(const std::vector<ImportedRef>&);
std::string importedKey(const ImportedRef&);
std::string importSummary(const ImportReport&, const std::string& file);
```

**Algorithm — two parsers written from the published specifications, refusing rather than guessing.**

*BibTeX*, per Oren Patashnik, *BibTEXing* (1988) and the format's documented grammar: entries begin at `@`; `@comment`/`@preamble` are skipped; `@string{name = value}` **defines a macro and is resolved**; an entry is `@type{citekey, field = value, …}` where a value is a brace-balanced `{…}`, a `"…"` string, a number, or a macro name, with `#` concatenation; field names fold to lower case; the **outermost** brace pair is stripped and inner braces (case protection) are preserved verbatim. Any entry with unbalanced braces, a missing citekey, or an **undefined** macro is REFUSED with its starting line and reason; parsing resumes at the next line-initial `@` so one bad entry does not swallow the file.

*RIS*, per the published RIS tag specification: each record starts `TY  - <type>` and ends `ER  - `; every line is a two-letter tag, two spaces, a hyphen, a space, then the value; a non-matching line continues the previous field; repeated `AU`/`A1` accumulate, joined with `; `. A record without `ER` at end of input is REFUSED. Type codes map through an explicit table (JOUR→article, BOOK→book, CHAP→chapter, THES→thesis, ELEC→web); an **unknown code is kept verbatim and counted**, never coerced to `book`.

The writers are the inverse mappings, emitting only non-empty fields.

**Reuses:** `tsvEscape`/`tsvUnescape` and F3's store discipline · `CatalogRegister::loadText`'s tolerant-reader **posture** (header-driven; refuses rather than guesses) as the model, not the code · `LibraryPane::importPaths`' result-reporting flow shape (27534) · `worksheetCsvRow`/`apparatusCsv` quoting for CSV export · F5's `secondary_template`.

### Acceptance tests

**CTest — `bibimport_smoke`**
1. `@book{dreyfus1997, author = {Dreyfus, Georges B. J.}, title = {Recognizing Reality}, publisher = {SUNY Press}, year = 1997}` → 1 record, `type=="book"`, `cite_key=="dreyfus1997"`, `year=="1997"`, refusals empty.
2. `title = {The {Two} Truths}` → `title=="The {Two} Truths"` (outermost stripped, case-protection preserved verbatim).
3. An entry with an unbalanced brace → records empty for it, `refused[0]` contains both `"line 3"` and `"unbalanced"`; a **valid** entry after it is still imported (recovery at the next `@`).
4. `@string{suny = {SUNY Press}}` then `publisher = suny` → `publisher=="SUNY Press"`; `publisher = unknownmacro` → that entry refused with `"undefined macro 'unknownmacro'"`.
5. RIS `TY - JOUR / AU ×2 / TI / PY / ER` → 1 record, `type=="article"`, authors joined with `"; "`, `year=="1996"`.
6. An RIS record with no `ER` → 0 records, `refused[0]` contains `"without ER"`.
7. An RIS type code `XYZ` → `type=="XYZ"` verbatim and counted, not mapped.
8. An unmapped BibTeX field `annote` and an unmapped RIS tag `XX` → each preserved in `extra` under its **own** original tag name, `unmapped_tags==2`, and `importSummary` names the count.
9. `writeBibtex(parseBibtex(fixture).records)` re-parsed gives an identical field map per record; `writeRis` likewise; neither writer emits an empty field.
10. `importedKey` is stable across calls; two entries differing by one character give different keys.
11. `importReferences` on a plain paragraph → 0 records, and the refusal reason contains **both** "not BibTeX" and "not RIS".

**app `--selftest`**
12. Importing a 3-entry fixture with 1 malformed entry writes exactly **2** rows to `imported.tsv`; the preview lists 1 refusal with its line number; re-importing the same file adds **0** rows.
13. Cancelling the preview writes nothing: `imported.tsv` byte-identical and `library/references_imports/` not created.
14. An imported record is citable — F5's picker finds it, and `renderCitation` with `secondary_template` produces `"Dreyfus, Georges B. J. 1997. Recognizing Reality. SUNY Press."`
15. Imported records appear under the Secondary literature node with `kind=="secondary"`, **no** ACIP number field, the card sentence "This is not a catalogue text", and they are **excluded from F7's catalogue verdicts** unless the user opts in.
16. Export ▾ ▸ BibTeX… writes through `saveOrWarn` and the success message is gated on its return value (WP-1).

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| BibTeX in the wild is messier than its specification (LaTeX accents, math mode, crossref) | Values are preserved verbatim and never "cleaned"; the label says so; anything outside the declared subset is a refusal with a line number, so a messy file yields fewer records and a longer problem list — never a plausible-looking wrong record |
| Secondary literature could drift toward looking like catalogue records | `kind=secondary`, a distinct badge naming the import file, no ACIP field, a separate node, and exclusion from catalogue duplicate verdicts (test 15) |
| Two colleagues importing the same `.bib` into a shared shelf | The key is a hash of the raw entry, so they converge; whitespace-different rows are caught by F7 as a title match |
| Imported keys colliding with catalogue work keys (e.g. `S1`) | The `IMP:` prefix namespaces them, and `parseCiteHref` carries the prefix, so a `cite:` field can never resolve to the wrong record |
| Export leaking gated material | The exporter's field list is **closed and reviewed**: titles and numbers only; never corpus English, never `hgm_gloss` (PAYLOAD_MANIFEST discipline) |

**Effort:** 10 h. **Depends on:** F1, F3, F5.

---

## F10 — "This text quotes…" facet on the record card

**§17 verdict:** ADAPT (Web of Science citing/related — "'texts that quote this passage' and 'texts this text quotes' from `quotation.h` + §8"). **Half of this verdict ships now; the other half is filed with its dependency named** — see *Deliberately omitted*.

### Purpose and user story
As a translator choosing a commentary I want to see, on the record, which canonical works this text demonstrably quotes — attested by the corpus, not guessed — so I know what I will need on the desk beside it.

### UI
Record card facet **"Quotes (attested)"**, collapsed by default with a **Find quotations** button (it is a spine scan, so it is never run on card open). Also the table context-menu item "Quotes / quoted by…". Results list each match: the corpus segment's course and sequence, the attested wylie span, its syllable count, and the published English of the segment — each row opening the source in the Overlay.

**Honesty labels.**
- *"Attested: this passage matches a corpus segment over N consecutive syllables. Below the threshold nothing is reported — a near miss is not a quotation."* (`quotation.h`'s own doctrine, rule 3.)
- EVIDENCE badge on every row, naming the corpus segment.
- The reverse direction is **absent and said to be absent**: *"'Texts that quote this one' needs the collection-wide passage index (§8, Batch 5). It is not built, so it is not shown."*
- Count line: "scanned 214 of 214 folios · 7 attested quotations · minimum span 7 syllables".

**Fit-ratchet note:** no ribbon contribution; a card facet and one context-menu item.

### Data
No store. The facet is computed live from the record's file and the spine.

### Engine
No new module. `allcore::detectQuotations(spine, passage, passage_is_acip, min_syllables=7)` (`core/include/allcore/quotation.h:28`), called with the record's file text and `passage_is_acip` set from `decodeAcipFilename`. Results rendered by the pane.

### Acceptance tests
**app `--selftest`**
1. With a fixture record whose text contains a corpus segment's wylie verbatim over ≥7 syllables, the facet lists exactly 1 match whose `matched_wylie` equals the attested span, and the row carries the EVIDENCE badge.
2. A text with only a 4-syllable overlap lists **0** matches and the count line says "0 attested quotations", not "no data".
3. The facet does **not** run on card open (assert the spine query count is unchanged until Find quotations is pressed).
4. The card contains the literal sentence naming §8/Batch 5 as the reason the reverse direction is missing.

### Risks and mitigations
| Risk | Mitigation |
|---|---|
| A full-text spine scan on a long text is slow | Button-triggered, pumped, with the folio count disclosed; never on card open (test 3) |
| Users may read "quotes" as "cites" | The label says "attested: matches a corpus segment over N syllables", and the threshold is on screen |

**Effort:** 6 h. **Depends on:** F1, F2.

---

## Sequencing

Each step ends the same way (the definition of done is in §"Gates" below).

| # | Step | Why here |
|---|---|---|
| 0 | **Hygiene commit H1–H5** (2.5 h) | `acip_refs`, exported `titleOverlap`, the `.inc` modal census, the unified bank loader, the LibraryPane follow-up filed. Behaviour-neutral, independently revertible, and F5/F7 depend on two of them |
| 1 | **F1** — `allcore::refrecord` + `refrecord_smoke` | Nothing else can start; it is also the only piece with real algorithmic risk (key normalisation, the 8,988-file walk). Green `ctest` before any UI exists |
| 2 | **F5** — `allcore::citation` + the picker, wired to Insert menu / Draft / Manuscript / Overlay "Cite this folio" | **Translator-first**: the picker is a dialog over the record engine and needs no pane, so the most expensive daily task (retyping STD-007 entries) ends two features in. *This reorders §17's 6a grouping deliberately; the deviation and its reason are recorded in the audit row* |
| 3 | **F6** — regenerating bibliography | Same core module and battery as F5; a citation you cannot roll up is half the job |
| 4 | **F2** — the References pane shell (3-page stack, table, card, ribbon, rebuild) | The shelf becomes browsable and the picker gains a home. The pane-local ratchet assertion lands with the first ribbon |
| 5 | **F3** — collections, smart collections, colour tags (`allcore::refshelf`) | Introduces the shared-judgment TSV store that F4, F7, F8 and F9 all write into, with its backup and Restore rows in the same commit |
| 6 | **F4** — read state and "On my desk" | Cheap; turns the shelf from a list into a work surface; rides F3's store with no new file discipline |
| — | **GATE: 6a → 6b** | design-critique + nielsen-heuristics + accessibility passes on the new surfaces, findings into `docs/FINDINGS.md` with dispositions; audit row started; `ctest -LE fixture` green |
| 7 | **F9** — BibTeX/RIS import and export | Before F7 and F8: it needs no network, it produces offline fixtures, and the bibliography (F6) can absorb secondary literature by the time introductions are written |
| 8 | **F7** — duplicates finder | Needs F3's `record_links` for the durable "distinct" ruling and F9's population so one duplicate engine serves both |
| 9 | **F8** — refresh from BDRC | Last on purpose: the only network feature, the only one that can be wrong because someone else's data is wrong, and it benefits from the ours-vs-theirs plumbing being the last thing added to a settled record model |
| 10 | **F10** — "This text quotes…" facet | Small, independent, and closes §17's last unanswered verdict |
| — | **CLOSE** | audit row `docs/release_audit/ReferencesPane.md`, TODO ticks, `docs/digests/LOG.md` lines, digest draft; press only when Adam asks |

### Gates — the definition of done for every step
1. `ctest` green, **including the new smokes under the clean-checkout `ctest -LE fixture` gate** — no new battery may depend on the gitignored `library/` or `data/extracted/`.
2. `app --selftest` green, with the pane's `selfTest` added to `main()`'s sum (`app/main.cpp:41796-41802`).
3. `tools/constitution_check.py` green **with the `.inc` extension from H3**, baseline still 154.
4. The pane-local ribbon ratchet (≤ 1180 px) **and** the app-wide `kRibbonRatchet` check.
5. The 800 px height fit.
6. Every write through `saveOrWarn` with the success message gated on its return value (WP-1); every new store's `save()` is `[[nodiscard]]`; shared TSVs use `tsvEscape`, absorb conflicted-copy siblings and **report** the count; compaction takes a backup first.
7. Every shared store joins the launch backup list (`app/main.cpp:38741`, keep 14) and Help ▸ Restore (40966) **in the commit that creates it**, not later.
8. Audit row in `docs/release_audit/ReferencesPane.md`, a `docs/digests/LOG.md` line, a TODO tick.
9. New surfaces get the design-critique + accessibility pass **before Adam sees them**.

**Total effort:** 2.5 h hygiene + 91 h features = **93.5 h**.

---

## Deliberately omitted

| Item | Why |
|---|---|
| **Manuscript Matcher / journal matcher** | §17 rates it SKIP: we have no journals to match. The named honest analogue — "which text does this passage come from" — is §9 and belongs to Batch 5 |
| **AI Research Assistant** (chat, key takeaways, summaries, translation) | §17 rates it SKIP under rule 1: no composed English may be presented as a reading. Revisit only as a separately labelled `Epistemic::Ai` surface if Adam asks |
| **"Texts that quote this passage" (the reverse direction of §17's Web-of-Science row)** | Needs the collection-wide passage index of §8, which is **Batch 5 and does not exist**. F10 ships the forward direction on the existing `quotation.h`; the reverse is filed with that dependency named, and the card says so on screen rather than leaving a silent gap |
| **CSL proper and its 9,000+ styles** | §17 rates CSL import LATER: "not CSL itself unless we vendor a processor". Vendoring citeproc is a licensing and maintenance decision of its own. We ship built-in house styles plus a documented template grammar; a CSL bridge can be added later without changing the carrier or the store |
| **Zotero RDF import** | §17 names it beside BibTeX/RIS, but RDF/XML needs an XML parser `allcore` deliberately does not have, for no new reach — every tool in the chain, Zotero included, exports BibTeX and RIS. Filed as LATER |
| **MinHash / LSH for the duplicates finder (§8's machinery)** | §17 suggests reusing §8's shingles, but §8 is Batch 5 and this cluster must not depend on unbuilt code. More importantly, titles and colophons are short and blocking keeps the candidate set small, so exact scoring is cheaper **and exactly reproducible** — and `refdup_smoke` test 7 proves the blocking loses nothing. If §8 ships, `refdup` can delegate its colophon shingling with no interface change |
| **A second scorer or tokenizer for titles** | H2 exports the existing `overlap()`; a fresh Jaccard implementation would let the duplicates finder and `suggestIdentity` disagree |
| **Library sharing with up to 1,000 people, cross-device sync, web access** | §17 maps these onto the team folder, which is what F3's shared-shelf switch uses. A sync service is a product, not a feature, and it would contradict the offline-core rule. No accounts, no server, no telemetry |
| **A Cite While You Write plug-in for Microsoft Word** | Our word processor is the Manuscript pane, where Insert Citation is native. A Word add-in is a separate deliverable with its own toolchain and signing story |
| **PDF annotation inside the References pane** | §17 row 3 rules that annotations stay team comments, for provenance. A second annotation store would fork the record of who said what |
| **Copying attachment files into a References store** | Every per-text artefact already lives somewhere (properties, glossaries, links, worksheets, scan cache, comments). Copying them would create a second source of truth and break Rename. Attachments are derived links computed at display time |
| **Records for places** | §17 lists places in the record vocabulary, but there is no place matcher, no gazetteer, and `docs/TIBETAN_TOOLS_SURVEY.md:133-137` records the THL Places bundling licence as unclear. Batch 6 ships Text / Person / Edition / Secondary literature. A place layer without a source would break rule 3 (open question 5) |
| **Editing catalogue fields directly on a record** | The catalogue charter: the app never writes the official catalogue and numbers are registrar-issued. Every correction routes through `ProposalKind::CatalogIdentity` |
| **Smart collections with full-text (Gofer) clauses** | Reduced on purpose in 6a: a smart collection must be explainable from the fields the table shows, and a capped index-dependent clause is not. Filed; if added later it must carry the cap disclosure verbatim from `app/textreport.h:86-112` |
| **`library/references.db` as the shared store** | §17 proposes it; this plan deviates. See open question 1 and §"Honesty and provenance" below |

---

## Honesty and provenance (the audit-row section, drafted now)

- The shelf is a **view**. It never renames, moves or deletes a text and never writes the catalogue. The duplicates engine has no filesystem write path at all.
- Every field on a record names its source verbatim and carries its `ux::Epistemic` badge. Nothing here is ever badged **Binding** — `hgm_gloss` does not appear in this pane.
- The house bibliography format has exactly **one** implementation (`composeBibliographyEntry` / `composeSanskritBibEntry`), enforced by `parseCitationStyle` refusing a style that sets both `builtin` and a template, and pinned byte-identically by `citation_smoke` test 1.
- Published bibliography entries are reproduced **verbatim** and labelled with their volume. Composed entries say they are composed. The two are never visually interchangeable.
- The app never translates a title, never derives a technical spelling into a citation, and never promotes a Tibetan title into an English slot. A missing published English title renders a visible gap.
- Refusals are first-class: unparsable citation markers, unresolved keys, refused BibTeX entries, refused style keys, refused blanking, and un-normalisable titles are all **named with their reason and their line**, never dropped.
- Count lines always distinguish shown / total / skipped; a cap or a cancelled run is never presented as a total; a missing bank is never presented as "no records".
- Read state is per person, local by default, and **never inferred** from opening a file.
- **Storage deviation, stated on the record:** §17 says `library/references.db` (SQLite, shared through the team updates folder). This plan splits it — a **local, rebuildable SQLite index** plus **shared judgment TSVs** — because a SQLite file in a synced folder cannot be conflicted-copy-unioned the way `proposals.tsv` and `comments.tsv` are, cannot be backed up by `allcore::backupFile`, and cannot be put back through Help ▸ Restore. The deviation is Adam's to rule (open question 1) and is written into the audit row either way.

---

## Open questions for Adam

*(also returned separately)*

---

## Appendix — files and wiring

See `moduleSplit`: five new `allcore` modules with their batteries, one `app/*.inc`, three shipped style descriptors, one BDRC fixture, two docs, and fourteen wiring points.

## Open questions for Adam (this batch)

- STORAGE — the one architectural decision in this batch. §17 says `library/references.db` (SQLite, shared through the team updates folder). This plan splits it: a LOCAL, rebuildable SQLite index (like `library/.index.db`) plus SHARED judgment TSVs (like `proposals.tsv` and `comments.tsv`), because a SQLite file in a Dropbox folder cannot be conflicted-copy-unioned, cannot be backed up by `allcore::backupFile`, and cannot be put back through Help ▸ Restore — the three properties every other shared store in this app depends on. Do you want my split, or the doc's single shared .db (in which case it is local-only and never in the team folder)?
- SHARED OR PRIVATE SHELF BY DEFAULT? The judgment TSVs (collections, tags, same-work rulings, BDRC overrides, imports) default to the LOCAL `<dataRoot>/library/references/`, with a Preferences switch to point them at the team proposals folder. Read marks stay per person and local in either case. Should the TEAM shelf be the default instead?
- WHICH CITATION STYLES SHIP? The two house styles cost nothing — they are already code (STD-007 `composeBibliographyEntry`, DCC `composeSanskritBibEntry`). Chicago and MLA would be OUR READING of published manuals, real work and real maintenance, and would have to ship labelled 'our reading of the published manual — not an authorised style'. Do translators ever publish in Chicago or MLA, or does batch 6 ship the two house styles plus one template style for secondary literature and wait? Related: does the ALL style sheet exist as a document? TODO already asks you for it. Without it, everything is anchored on the shipped writer and its equality test rather than on a sheet.
- CONFIRM THE PUBLISHED-VERBATIM RULE before F5/F6 are built. When a citation resolves to an entry in `apparatus_bibliography.json` (705 entries with `acip_refs`), the chosen style is IGNORED and the published text is emitted verbatim with 'as published in <volume>'. That is the right reading of rule 1 — MATCH the wording, never compose a new one — but it produces a visibly mixed bibliography when a document cites both published and composed entries. Your call, recorded.
- DOES THE SHELF SHOW UNCATALOGUED FILES? The library holds ~8,988 files; the catalogue banks cover 1,846 works (verified). My design shows uncatalogued files as records keyed `FILE:<path hash>`, flagged and gathered under 'Unrecognised files', because a translator's own materials are part of their shelf and the residue is worth seeing. Do you want them in the shelf, or should References show only catalogue-recognised texts and leave the rest to the Library pane?
- IS 'THESE ARE ONE WORK' A LEGITIMATE DESK-LEVEL JUDGMENT? The duplicates finder writes a local (or team) same-work link and offers a separate button to raise a `ProposalKind::CatalogIdentity` proposal. That means a translator can decide, for their own shelf, that two records are one work without an approver ruling. Acceptable, or must every such judgment go through the proposal channel so the shelf never disagrees with the catalogue?
- MAY A BDRC-SOURCED FIELD BE OFFERED TO THE CATALOGUE? Fields taken from BDRC stay in a local `overrides.tsv` and never touch `data/extracted`. Should the Apply dialog also carry a 'propose this to the catalogue' button (routing the value plus its `bdr:` source through `ProposalKind::CatalogIdentity`), or is BDRC material strictly desk-level reference that a human must retype into a proposal deliberately?
- LEGACY MARKERS — shipped drafts already contain `[NOTE: …]` and `[BIBLIOGRAPHY: …]` from the existing insert buttons (app/main.cpp:23494). My design leaves them byte-untouched, counts them, and reports them ('not citation fields'), because rewriting a translator's prose without being asked is not something the tool should do. Do you want an explicit, PREVIEWED 'convert legacy markers to citation fields' action in 6b, or should they stay as they are?
- DRAFT CITATION CLUTTER — the Manuscript can hide a citation field inside an anchor, but the Draft is plain text, so a field there reads `[CITE S00184 f.12b.3 | Pabongka Rinpoche, 1878–1941.  Liberation in Our Hands …]`. Acceptable in a working draft (with the Preferences switch for short markers), or should citation fields be Manuscript-only and the Draft keep plain bracket markers?
- BDRC DATA LICENCE — F8 can write fetched BDRC field values into a store that may live in the shared team folder. That is a redistribution question, not a design one. Should someone confirm BDRC's terms before 6b ships, and is that you or John?
- PLACES — §17's record vocabulary names places, but there is no place matcher, no gazetteer, and the THL Places bundling licence is recorded as unclear (docs/TIBETAN_TOOLS_SURVEY.md:133-137). Batch 6 ships Text / Person / Edition / Secondary literature only. Do you want a place source found and licensed first, or are places out of scope until someone needs them?
- PANE PLACEMENT — References makes the Research group 7 tabs (Search, Lookup, Sanskrit, Convert, Analysis, Compare, References). §17 proposes Research and your regrouping freedom allows it, but moving it to Read is a one-line `mkGroup` change. Keep it in Research?

## Files this batch creates or touches

| File | Kind | Contents |
|---|---|---|
| `core/include/allcore/refrecord.h` | core-header | RefField/RefFieldHistory/RefRecord/WorkBank/ShelfStats/ShelfQuery/FieldDelta/MergeMode; recordForFile, buildShelf, shelfSummary, RefShelfIndex, serializeShelfQuery/parseShelfQuery/explainShelfQuery, compareRecords, applyMerge, recordAsFieldText — the shelf projection over decodeAcipFilename + the catalogue banks, with per-field provenance and history. |
| `core/src/refrecord.cpp` | core-src | Layered projection (filename → bank → overrides), witness folding by normalizeCatalogKey, absolute-path-hash FILE: keys, single-transaction SQLite/FTS5 rebuild with banks_generation stamping and LibraryIndex-style cancellable progress; pure merge with refuse-to-blank. |
| `core/tests/refrecord_smoke.cpp` | core-test | 14 checks: filename/bank field sources, unrecognised keys are path-hashed not basename-keyed, witness folding, cancellation reports 2-of-4, unreadable counted separately, index round trip, banks_generation drop-and-rebuild, query serialize/parse/explain, EmptyOnly/All/Chosen merges, refuse-to-blank, stable field text (0 hunks). |
| `core/include/allcore/refshelf.h` | core-header | Collection/Tag/ReadState/ReadStamp/Override; RefShelfStore (append-only, conflicted-copy union, [[nodiscard]] save) with collections, membership, tags, read state, overrides, applyOverrides, isRefColor. |
| `core/src/refshelf.cpp` | core-src | tsvEscape'd append-only TSVs with last-wins-by-(key,at) folds, id tiebreak, Dropbox conflicted-copy sibling union with conflictSiblings() disclosure, tombstones instead of deletion, and compaction at 5,000 rows that takes a backupFile first. |
| `core/tests/refshelf_smoke.cpp` | core-test | 11 checks: collection round trip, append-only membership (3 rows for 2 members), sibling union, deterministic fold across shuffled orders, closed colour palette refusal, smart-query persistence, compaction-takes-a-backup, read state isolated per user, Clear writes a row, recordsInState, override sets value AND source then reverts. |
| `core/include/allcore/citation.h` | core-header | CitationStyle/CiteField/CiteScan/CitationOut/BibEntryOut/BibliographyOut/BlockSpan; parseCitationStyle (flat subset; builtin and template are mutually exclusive), composeCiteHref/Marker + parsers, scanCiteFields, renderCitation, renderPublished, renderBibliography, bibliographyBlockText, findBibliographyBlock. |
| `core/src/citation.cpp` | core-src | builtin std007/dcc_sanskrit dispatch straight to composeBibliographyEntry/composeSanskritBibEntry (never a template); Patashnik-style drop-whole-group templates for other styles; locator splice; visible gap markers; legacy-marker-tolerant carrier scan; deterministic sectioned, locale-free bibliography with merged_into following and unresolved keys emitted. |
| `core/tests/citation_smoke.cpp` | core-test | 17 checks: house render byte-identical to composeBibliographyEntry incl. two spaces and en dashes on dates AND folios, the (ACIP …) separator switch, no orphan comma, visible gap with complete=false, hgmTechnicalSpelling never reached, style key/builtin-conflict refusals, template group drop, locator with \| ? & refused, legacy markers counted and untouched, published verbatim, bibliography counts/dedup/stability/idempotence/merged_into. |
| `core/include/allcore/refdup.h` | core-header | DupMember/DupGroup/DupOptions/DupScan; findDuplicateRecords, dupSummary, dupReportMarkdown, dupReportCsv — record-level duplicates over folded witnesses, scored with the exported allcore::titleOverlap. No filesystem write path. |
| `core/src/refdup.cpp` | core-src | normalizeAcipTitle only when title_is_acip (else compared raw and flagged); syllable blocking with a disclosed comparison cap; titleOverlap for titles and 5-syllable Jaccard for colophons; disjoint-set grouping reporting the pairwise minimum; verdicts by evidence class in the qcDuplicateTitles vocabulary; symmetric ignore_pairs applied before grouping. |
| `core/tests/refdup_smoke.cpp` | core-test | 12 checks incl. the brute-force equivalence test (200 synthetic records: blocked pair set == all-pairs pair set), verdict split by colophon evidence, transitive group reports the minimum, symmetric suppression, 2,000-record performance, cap disclosed, no-title records counted, compared-raw flag, CSV header and quoting. |
| `core/include/allcore/bibimport.h` | core-header | ImportedRef (with per-tag extra map)/ImportReport; parseBibtex, parseRis, importReferences (format sniffing), writeBibtex, writeRis, importedKey, importSummary. |
| `core/src/bibimport.cpp` | core-src | BibTeX per Patashnik 1988 — brace-depth values, @string macros resolved, undefined macro/unbalanced brace/missing citekey REFUSED with line and reason, resync at the next line-initial @; RIS per the published tag spec — TY…ER, continuations, repeated AU, unknown type codes kept verbatim; unmapped fields/tags preserved individually as extra:<TAG> and counted; inverse writers emitting only non-empty fields. |
| `core/tests/bibimport_smoke.cpp` | core-test | 11 checks with inline fixtures: basic BibTeX and RIS mapping, case-protection braces preserved, unbalanced brace refused by line with recovery, macro resolved and undefined macro refused, missing ER refused, unknown RIS type kept, per-tag extra preserved and counted, write→parse fixed point, stable importedKey, neither-format refusal naming both. |
| `app/references_pane.inc` | app-inc | class ReferencesPane (QStackedWidget: Records \| Duplicates \| Import & Refresh) with the shelf tree, record table, provenance card with per-field [history], SHELF/FIND/MARK/CITE ribbon groups plus MORE ▾, the Insert Citation QDialog, the ours-vs-theirs BDRC dialog, the BibTeX/RIS preview dialog, the F10 quotations facet, g_showInReferences/g_recordKeyForFile/g_insertCitation hooks, and int selfTest(QStringList&) — all dialogs QDialogs inside the .inc, zero QMessageBox/QInputDialog. |
| `data/citation_styles/all-house.json` | docs | Style descriptor: {"id":"all-house","builtin":"std007","locator_form":", at {locator}","sort":["author","english_title"],"sections":["Sanskrit","Tibetan","Secondary"]} with EMPTY templates — the house format stays in composeBibliographyEntry. |
| `data/citation_styles/all-house-sanskrit.json` | docs | Style descriptor with builtin "dcc_sanskrit" and empty templates, dispatching to composeSanskritBibEntry. |
| `data/citation_styles/secondary-basic.json` | docs | The one shipped TEMPLATE style, for imported secondary literature only, exercising the {…}/<field> grammar so the template path has a real tested consumer that never touches the house format. |
| `data/refs/fixtures/bdrc_binding_MW22084.json` | docs | One checked-in BLMP results.bindings row (~2 KB) so the F8 ours-vs-theirs table and its mapping are testable offline with zero network calls. |
| `docs/ANALYSIS_SUITE_PLAN.md` | docs | This specification: the four commitments, the H1–H5 hygiene commit, F1–F10 with UI/data/engine/tests/risks/effort, sequencing with gates, deliberately-omitted with reasons, and the open questions. |
| `docs/release_audit/ReferencesPane.md` | docs | Release audit row in the ComparePane.md format — purpose and how it is reached, the user-visible function table with code anchors and automated coverage, the honesty and provenance section (including the §17 storage deviation and its reason), and open items filed in TODO. |
| `app/main.cpp` | wiring | Line ~946: add `QStringList acip_refs;` to `struct ApparatusBib` (H1). |
| `app/main.cpp` | wiring | Line ~38560: fill `acip_refs` in the global apparatus loader and delete DraftPane's second loader at 22956, pointing it at g_appBib (H1). |
| `app/main.cpp` | wiring | Line 23829: `#include "references_pane.inc"` immediately after `#include "compare_pane.inc"`. |
| `app/main.cpp` | wiring | Line ~38629: `auto* referencesPane = new ReferencesPane(root, spine, &progress, openAtLine); tabs.addTab(referencesPane, "References");` beside the comparePane block. |
| `app/main.cpp` | wiring | Line 39208: `mkGroup("Research", {"Search","Lookup","Sanskrit","Convert","Analysis","Compare","References"});` |
| `app/main.cpp` | wiring | Line ~39080: add `{"References", "quote"}` to the paneIcon map. |
| `app/main.cpp` | wiring | Line ~41800: `fails += referencesPane->selfTest(log);` in main()'s --selftest sum. |
| `app/main.cpp` | wiring | Line ~38741: add the shared shelf TSVs (collections, membership, tags, record_tags, readstate, record_links, overrides, refresh_log, imported) to the launch backupFile block, keep 14. |
| `app/main.cpp` | wiring | Line ~40966: add the same shared shelf TSVs to Help ▸ "Restore a Shared Store from Backup…". |
| `app/main.cpp` | wiring | Line ~40159: Insert menu gains "Citation…" (⌥⌘C) and "Bibliography ▾" (Insert here / Update ⌥⌘B / Go to / Remove), beside "Footnote from the Bank…". |
| `app/main.cpp` | wiring | Line ~8815: Overlay text context menu gains "Cite this folio…", prefilled from resolveFolioAt (11985), under the existing folio identity header. |
| `app/main.cpp` | wiring | Draft PUBLISH ribbon (~21931) and Manuscript PUBLISH row (~33372): [Insert citation…] and [Bibliography ▾]. |
| `app/main.cpp` | wiring | Line ~28039: Library pane info card gains an "Open in References" anchor and a one-line read-state row, routed through g_showInReferences. |
| `app/main.cpp` | wiring | Files pane context menu gains "Show in References"; declare `g_showInReferences` / `g_recordKeyForFile` / `g_insertCitation` before the .inc includes. |
| `core/include/allcore/catalog_id.h` | wiring | Export `double titleOverlap(const std::string& norm_a, const std::string& norm_b, int* shared_syllables = nullptr);` implemented by the existing private overlap()/syllables() at core/src/catalog_id.cpp:24-47 (H2). |
| `tools/constitution_check.py` | wiring | Line ~29: extend the R3 modal census from app/main.cpp alone to app/main.cpp plus every app/*.inc; baseline stays 154 (compare_pane.inc has zero sites) (H3). |
| `core/CMakeLists.txt` | wiring | Register refrecord_smoke, refshelf_smoke, citation_smoke, refdup_smoke and bibimport_smoke with add_executable / target_link_libraries(PRIVATE allcore) / add_test — no fixture paths and no LABELS "fixture", so all five run in the clean-checkout `ctest -LE fixture` gate. |
