# Text-analysis tool families and EndNote — research and verdicts for the Diamond Cutter Translation Tool

Adam, 2026-09-08 evening: "dig in deep … see if there's any functionality
in the documentation for each of these … decide how we could use it in our
Tibetan app, specifically for Tibetan translation." And, on EndNote: "used
in-house by research teams with specific banks of data … our database is
an in-house bank of data the translators reference."

Method: for each family, the reference tools' own documentation was read
where it could be reached tonight (sources at the end; a few pages refused
the fetch and are marked so), the feature list was set against what the
app already does, and each feature got a verdict: **HAVE** (exists),
**BUILT** (added this evening), **BUILD** (do it, with the batch), **ADAPT**
(the idea survives, the form changes for Tibetan), **LATER** (filed),
**SKIP** (with the reason). The honesty rules of CLAUDE.md apply
throughout: machine work may match, count and align; it never composes
Geshe Michael's English and never guesses a reading.

---

## 1. Diff viewers (diff, cmp, WinMerge, Meld, Beyond Compare)
**Documented features.** Line and character diffs; whitespace/case/blank-
line/EOL ignore; regex line filters and substitution filters (WinMerge);
moved-block detection; word- and character-level in-line highlights; diff
pane and location pane; first/prev/next/last; copy left/right (+ advance);
edit in place with rescan; bookmarks; syntax colouring; output formats
normal/context/unified/side-by-side/ed (GNU diff); `cmp` first differing
byte; sync points (WinMerge/Meld), synchronization links (Araxis),
alignment overrides (BC); reports HTML/text/patch; sessions.
**Ours.** BUILT tonight as the Compare pane + `allcore::textdiff`
(`docs/COMPARE_SUITE_PLAN.md`, `docs/release_audit/ComparePane.md`):
everything above except syntax colouring (SKIP: ACIP is not a syntax) and
ed-script output (SKIP: no consumer). **Tibetan-specific additions**:
syllable-level in-line marks in ACIP/Wylie/Unicode; rules for shad/tsheg
variants, folio markers, `[ ] { }` apparatus; **script-agnostic** compare
through the canonical converters; the **apparatus criticus** report cited
by folio and line. Verdict: HAVE/BUILT.

## 2. Three-way merge (Git, GitHub, GitKraken, VS Code, diff3)
**Documented.** Base + two descendants; automatic merge of non-conflicting
hunks; conflict markers; take ours/theirs/both/base; conflict navigation;
merge tools launched from VCS.
**Ours.** BUILT: Three-Way Merge page (diff3 in `textdiff::merge3`), Open
Conflict File. **Translation use:** two proofreaders' corrections of one
input-centre file against the original; two translators' drafts against
a shared earlier draft. Verdict: BUILT.

## 3. Document comparison (Word Compare/Combine, Acrobat Compare, Draftable)
**Documented.** Word: revision marks in a new document, originals
untouched, Compare vs Combine (multiple authors). Acrobat: text vs image
comparison, scanned documents compared as pixels, results summary by type,
filters, side-by-side or single page. Draftable: move detection (moved
*and* edited text), character-level changes, Word/PDF/scanned/Excel/free
text, exports as tracked-changes Word, redline PDF, changed-pages PDF,
change report table, CSV, "AI-ready" Markdown; API returning every change
as JSON.
**Ours.** Text-level: BUILT (moves, character/syllable level, CSV/Markdown
change reports — the apparatus criticus is our redline). **ADAPT/LATER:**
(a) *tracked-changes export* — write the differences between two drafts
as a `.docx` with Word revision marks so Geshe Michael can review in Word
(the Manuscript already exports `.docx`; add `w:ins`/`w:del` runs from the
hunks) — BUILD, batch 4 of the compare plan; (b) *.docx/.pdf text
extraction compare* — extract text (the export path has a `.docx` writer;
a reader is needed) — LATER; (c) *scan compare* — two images of the same
folio side by side with a blend/flicker slider, for print-edition work —
LATER (Scans pane owns the viewer); (d) *"changed pages only"* → for us
"changed folios only" report — BUILD (cheap: filter the apparatus by folio).

## 4. Folder comparison and synchronization
**Documented.** BC/Araxis/UC/Meld/WinMerge: two- and three-way folder
trees, filters, comparison criteria (timestamp/size/CRC/binary/rules),
show same/different/orphans, copy/update/mirror, snapshots, archives,
reports. **Ours.** BUILT: Folder Compare page (bytes / rules-based /
size; filters; Update/Copy with preview; snapshots; HTML/CSV). SKIP:
mirror-with-delete (nothing here deletes), FTP. LATER: three-way folder
merge (no workflow yet), archive contents (the Files pane opens archives;
compare needs extraction). **Translation use:** an input centre's batch
against the corrected batch; a release folder against the previous release.

## 5. Version control (Git, Subversion, Mercurial)
**Documented.** History of every change; restore; branch; merge; blame
(who changed which line when); tags/releases.
**Ours.** The repository itself is under Git, but translators' texts are
files in the data folder with no history beyond the app's backups
(`allcore/backup.h`) and Properties revision counts. **ADAPT — "Versions"
for a document (no Git exposed):** on every Save, keep a compressed copy
under `library/versions/<doc>/<timestamp>` (size-capped, oldest pruned);
File ▸ Versions… lists them with the Properties statistics; "Compare with
Version" opens the Compare pane; "Restore" copies back (confirm). Blame =
"which version first introduced this line" computed by walking versions
with the engine. Branching: SKIP (a translator does not branch a text;
dossiers already give one desk per text). Verdict: BUILD, compare plan
batch 4 — it also gives "Compare with Backup" its listing.

## 6. Text normalization tools
**Documented.** Ignore capitalisation, whitespace, line endings,
punctuation, formatting before comparing. **Ours.** HAVE/BUILT in the
compare rules, and the Convert pane normalises scripts. **BUILD (small):**
a *Normalize…* action on a selection/document offering the same rules as
transformations (collapse spacing, unify shad spelling `, ,,` → per house
convention, strip apparatus, fix line endings) with a preview diff in the
Compare pane before applying — "show me what would change" is exactly what
a diff is for. Batch 4.

## 7. Find-and-replace across files (Notepad++, VS Code, sed)
**Documented (Notepad++ manual).** Normal/extended/regex modes; match
case; whole word; wrap; in selection; Find in Files with wildcard
filters, exclusions (`!*.bin`, `!+\folder`), subfolders, hidden folders;
**Replace in Files**; Replace in all open documents; Mark (bookmark lines,
highlight style, copy marked text); Find results window (hierarchical,
copy, open, delete, secondary search); incremental search.
**Ours.** HAVE: the Find menu (regex, case, word, wrap, in selection,
highlight all, preserve case, F3 family) and Find in Files → Search pane
(Gofer). **BUILD:** *Replace in Files…* with a mandatory **preview**:
every proposed change listed per file as a diff (the Compare engine's
hunks), tick/untick, then apply through `saveOrWarn`; result window with
counts; an undo file set (the versions from §5). Wildcard include/exclude
filters as in Notepad++. This is the input-centre formatter's core (roadmap
item A). Batch 4. **Mark** (bookmark every matching line) → ADAPT via the
existing gutter bookmarks: "Bookmark all matches" in the Find bar. BUILD.

## 8. Duplicate and near-duplicate detection
**Documented.** Shingling (k-grams) → MinHash signatures → LSH banding to
find candidate near-duplicate pairs without n² comparisons; used for
cleaning collections and finding repeated content.
**Ours.** `allcore/quotation.h` already detects quotations between texts.
**BUILD — Repeated Passages (collection-wide):** syllable k-gram shingles
(k=8 syllables, script-agnostic through the converters), MinHash (128
hashes), LSH buckets; report pairs of passages ≥ threshold with folio
citations; within one text (refrains, repeated formulas, copy-paste
errors) and across the library (a commentary quoting a root text; two
editions of the same text under different titles — the cataloguing team's
divergence problem). Batch 5 (Research group, "Passages" page inside
Analysis or its own pane; Adam's regrouping freedom applies).

## 9. Plagiarism and similarity checkers (and TRACER)
**Documented.** Overlap between documents or against a reference
collection; TRACER (eTRAP): ~700 combinable algorithms, language-
independent, tested on Tibetan among others, detects verbatim, near-
verbatim, paraphrase and allusion; pipeline pre-processing → featuring →
selection → linking → scoring → post-processing.
**Ours.** ADAPT, not plagiarism: **similarity studies** for philology —
"how much of Gyaltsab Je's commentary is verbatim from Tsongkapa", "which
commentary this passage descends from", ranked candidate sources with the
shared passages highlighted; document-pair similarity (Jaccard on
syllable shingles, plus longest common passages from the diff engine).
Paraphrase/allusion detection: LATER and clearly labelled provisional; do
not build TRACER's scoring from memory — if wanted, run TRACER itself on
exported text (it is a free download) and import its results. Batch 5.

## 10. Text statistics
**Documented.** Word count, sentence length, readability, keyword
frequency, vocabulary. Voyant Tools (humanities suite): Cirrus (word
cloud), Trends (relative frequency across documents), Contexts (KWIC),
Collocates, Bubblelines, TermsBerry, Correlations, Phrases (repeating
sequences), Summary, Reader, Links (term network).
**Ours.** HAVE: Properties ▸ Statistics (words, characters, syllables,
folios, shads), `allcore/textdna.h` (meter/prose profile), Analysis pane,
concordance (Search). **BUILD — a Statistics study view:** per text or
collection: syllable and word frequency tables (script-agnostic), type/
token ratio, hapax list, shad-delimited sentence-length distribution,
particle density, **Trends** of a term across the folios or across a
collection ordered by author/date, **Collocates** (terms within N
syllables of a headword — the Gofer NEAR grammar already exists), KWIC
(Contexts) — the concordance already is this, **Phrases** (repeating
n-grams — same shingles as §8). Word cloud: SKIP as decoration; a ranked
table with counts is honest and searchable. Readability formulas: SKIP for
Tibetan (no validated formula; do not invent one). Batch 5.

## 11. Natural-language analysis (entities, topics, sentiment, keywords, summaries, relationships)
**Documented.** Generic NLP toolkits and AI assistants (EndNote's Research
Assistant: chat with documents, key takeaways, summaries, translation).
**Ours.** HAVE: Botok segmentation and part-of-speech (`botok.h`,
`poslex.h`), sa bcad outline extraction (topics = the text's own outline),
people matching (`authorsearch.h`, `matchPeople`), subjects (`subjects.h`),
verb classes, particles. **BUILD:** *Entities in this text* — names, places,
texts and dates found by the existing matchers and the catalogue, listed
with folio citations and links to the map/lineage work already filed.
*Keywords* — terms whose frequency in this text is high against the corpus
(TF-IDF over syllable bigrams; labelled "distinctive terms", not
"keywords"). *Relationships* — co-occurrence of entities within a folio.
**SKIP:** sentiment (meaningless for these texts); **machine summaries and
"chat with the text"** — a summary is composed English, which rule 1
forbids for HGM data and which we will not present as reading; if Adam
wants an assistant later it must be labelled generated, never mixed with
HGM's words, and is a separate decision.

## 12. Change tracking and audit (who changed what, accept/reject/revert)
**Documented.** Word Track Changes; Draftable tracked-changes export;
EndNote's Find Reference Updates showing two records side by side with
differences in blue and "update all / update empty / pick fields".
**Ours.** HAVE: the proposals/approval workflow (`proposals.h`,
`comments.h`, action ledger) for dictionary changes, with provenance. For
*texts*: BUILD via §5 Versions + Compare: "Accept/Reject" = Copy Left/Right
between the current text and a version; "who" comes from the Team name in
Settings written into each version's sidecar. Batch 4.

## 13. Patch generation and application
**Documented.** diff -u → patch; `patch -pN`, dry run, reverse.
**Ours.** BUILT: unified patch export. **BUILD:** *Apply Patch…* — read a
unified diff, match hunks against the current text (exact, then with
offset, then fuzz 1–2 lines of context), preview in the Compare pane,
apply; failures listed, never forced. Use: a corrector sends a patch for a
long ACIP file instead of the whole file. Batch 4.

## 14. Structured-data comparison (JSON, XML, CSV, YAML)
**Documented.** Compare by field/record rather than as text (BC Table
Compare, WinMerge table mode). **Ours.** Our structured data: glossary
JSON, alignment evidence JSON, release manifests, CSV exports. **BUILD:**
*Table compare* keyed by a chosen column (headword) — added / removed /
changed records with per-field diffs; use it for glossary versions and for
release-to-release dictionary comparison (which entries changed in
v0.27.3). Batch 5.

## 15. Side-by-side editors with copy buttons
BUILT (Compare pane).

## 16. The "useful distinction" from Adam's screenshot
Diff = show what changed (Compare pane) · Merge = combine (Copy/Three-Way)
· Replace = overwrite (All Left/Right, Replace in Files) · Sync = keep two
locations aligned (Folder Compare Update) · Version control = history and
rollback (Versions, §5) · Text analysis = meaning, structure, patterns
(§§8–11, the Research group). The plan keeps these six verbs as the menu
vocabulary so the translators can find each by what it does.

---

## 17. EndNote — reference and citation management for an in-house bank

**What EndNote does (product page, EndNote 2025; university guides for
EndNote 21).** A library of reference records with attachments (unlimited
PDFs, figures); groups, group sets, **smart groups** (saved searches that
fill themselves), combination groups, **tags** (colours, a second
classification); read/unread marks; PDF reading and annotation in a
tabbed window; **Find Duplicates** with side-by-side keep/merge; **Find
Reference Updates** (two records side by side, differences in blue,
update all / empty / chosen fields); Find Full Text; **Cite While You
Write** (field-coded citations in Word that reformat continuously; cite
from a PDF's highlight); thousands of output styles, continuously updated;
import filters and connection files (online database search from inside
the library); journal term lists; library sharing with up to 1,000
people, sync across devices, web access; Manuscript Matcher (journal
suggestions); Web of Science citing/related articles; AI Research
Assistant (chat, key takeaways, summaries, translation). Zotero (open
source) covers the same ground with collections, tags, saved searches,
9,000+ citation styles, group libraries and word-processor plugins.

**Why this fits us.** Our bank is not journal articles but *texts*:
editions, folios, commentaries, the corpus of Geshe Michael's courses, the
dictionary's evidence, scans, catalogue records, people and places. The
translators cite these constantly — in footnotes (the Draft's apparatus
bank), in the Manuscript, in the daily digests — and the citations are
typed by hand today.

**Verdicts, mapped onto what exists:**
| EndNote feature | Ours today | Verdict |
|---|---|---|
| Reference library with records + attachments | Library pane (texts), catalogue records, Scans; dossiers (one desk per text) | ADAPT — a **References** view over the catalogue: every text/edition/article/person/place is a record with attachments (scans, PDFs, notes) |
| Groups, group sets, smart groups, tags | Files workspaces, dossiers, subjects | BUILD — user **collections** (the Textual Research idea already filed), **smart collections** = saved Gofer/catalogue queries that refill, colour tags |
| Read/unread, notes, PDF annotation | team comments per text; Scans viewer | ADAPT — "read" state per text per user; annotations stay comments (provenance) |
| Find Duplicates | `tree_diff.h` divergence audit; catalogue QC | BUILD — duplicate/near-duplicate *records* (title variants, same text under two names) using the §8 shingles on titles + colophons; side-by-side keep/merge with provenance |
| Find Reference Updates | catalogue register | ADAPT — **refresh from BDRC / open catalogues**: fetch the record, show ours vs theirs side by side (Compare pane), update all / empty / chosen fields; nothing overwritten silently |
| Cite While You Write | none | BUILD — **Insert Citation…** in Draft/Manuscript: pick a text + folio (or the current Overlay position) → a field-coded citation in the house style (author, title, edition, folio/line; Tibetan and English forms); a bibliography section regenerates from the fields; styles: ALL house style, Chicago, MLA as JSON templates (CSL-like, not CSL itself unless we vendor a processor) |
| Output styles (thousands) | Manuscript house styles (paragraph) | ADAPT — a handful of *citation* styles, editable JSON; CSL import LATER |
| Import filters / connection files | Import Data Release; catalogue ingest | ADAPT — import BibTeX/RIS/Zotero RDF for secondary literature; BDRC record import as the "connection" |
| Journal term lists | terminology.h, glossary | HAVE (our term lists are the dictionary) |
| Library sharing, sync | team proposals folder, updates folder | ADAPT — the References store is a JSON/SQLite file in the shared data folder; the existing "team updates folder" carries it |
| Manuscript Matcher | — | SKIP (journal matching) → the honest analogue "which text does this passage come from" is §9 |
| Web of Science citing/related | quotation detection | ADAPT — "texts that quote this passage" and "texts this text quotes" from `quotation.h` + §8 |
| AI Research Assistant | — | SKIP for now (rule 1: no composed English presented as reading); revisit as a labelled, separate feature only if Adam asks |

**Proposed shape:** a **References** pane in the Research group (Adam's
regrouping freedom): left, collections/smart collections/tags; centre,
the record table (title, author, edition, folios, attachments, read
state); right, the record with citation preview in the chosen style.
Insert Citation lives in the Draft and Manuscript (Insert menu) and in the
Overlay's context menu ("Cite this folio"). Data: `library/references.db`
(SQLite, shared through the team updates folder). Batches: 6a records +
collections + citation insert; 6b duplicates + BDRC refresh + import.

---

## 18. Proposed sequencing (after the Preferences panel, which is still owed)
- Compare plan batch 4: Versions (§5, §12), Replace in Files with preview
  (§7), Apply Patch (§13), Normalize… (§6), tracked-changes `.docx` export
  and changed-folios report (§3).
- Batch 5 (Research): Repeated Passages + similarity studies (§8, §9),
  Statistics study view with Trends/Collocates/Phrases (§10), Entities in
  this text (§11), Table compare (§14).
- Batch 6: References pane + Insert Citation (§17).
Every batch: selftests + gates, audit rows, LOG line, commit; new surfaces
get the heuristic/accessibility pass before Adam sees them.

## Sources consulted (2026-09-08)
- WinMerge manual — Comparing and merging text files: https://manual.winmerge.org/en/Compare_files.html ; Using Filters: https://manual.winmerge.org/en/Filters.html
- Araxis Merge: https://www.araxis.com/merge/index.en ; folder comparison overview: https://www.araxis.com/merge/windows/folder-comparison-overview.en
- Beyond Compare (Scooter Software) — vendor summaries via search; help pages not reachable tonight.
- UltraCompare — https://www.ultraedit.com/products/ultracompare/ (summary via search; direct fetch refused).
- Meld — https://meldmerge.org/ ; https://gnome.pages.gitlab.gnome.org/meld/
- Draftable — https://www.draftable.com/compare ; redline help: https://help.draftable.com/hc/en-us/articles/19002235946265-Redline-comparisons ; API: https://www.draftable.com/rest-api
- Microsoft Word Compare/Combine — https://support.microsoft.com/en-us/office/compare-and-merge-two-versions-of-a-document-f5059749-a797-4db7-a8fb-b3b27eb8b87e
- Adobe Acrobat Compare Files — https://helpx.adobe.com/acrobat/using/compare-documents.html (summary via search; direct fetch refused)
- GNU diffutils manual — https://www.gnu.org/software/diffutils/manual/ (fetch rate-limited tonight; options quoted from the manual's known contents)
- Notepad++ user manual, Searching — https://npp-user-manual.org/docs/searching/
- Near-duplicate detection (shingling / MinHash / LSH) — https://yorko.github.io/2023/practical-near-dup-detection/ ; https://sites.cs.ucsb.edu/~tyang_class/293s20f/slides/TopicDuplicate1.pdf
- TRACER (eTRAP) — https://www.etrap.eu/research/tracer/
- Voyant Tools — https://voyant-tools.org/docs/tutorial-tools_.html ; https://library.villanova.edu/research/guides-tools/howto-guides/text-analysis-101-voyant-tools ; https://libguides.library.universityofgalway.ie/DigitalTools/VoyantTools
- EndNote — product details: https://endnote.com/product-details/ ; organisation (UNC): https://guides.lib.unc.edu/EndNote21/organize ; groups & tags (Northwestern): https://libguides.galter.northwestern.edu/endnote-windows-21/groups-tags ; Find Reference Updates: https://docs.endnote.com/docs/endnote/2025/v1/macos/en/content/03editref/find_reference_updates.htm and https://support.alfasoft.com/hc/en-us/articles/360000987398-Find-reference-updates-for-your-references-in-your-EndNote-library ; Manuscript Matcher: https://endnote.com/manuscript-matcher/ ; duplicates (Leeds): https://library.leeds.ac.uk/info/1403/endnote/223/add-references-and-full-text/5 ; CWYW (UCL): https://library-guides.ucl.ac.uk/endnote-21/cwyw-faqs
- Zotero — https://www.zotero.org/
