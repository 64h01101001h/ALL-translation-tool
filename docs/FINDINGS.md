# FINDINGS — the inbox
*(S6: one file where every field finding lands, and the promise that
every entry gets a disposition in a ledger. File from the app —
Help → Troubleshooting → File a Finding — or by hand below. Campaign
items use their TEST_CAMPAIGN.md IDs.)*

Disposition stamps: FIXED(commit) · RULED(where) · DEFERRED(stamp) ·
NEEDS-INFO(question back to the filer).

## Standing: Adam's outstanding verdicts (never silently expire)
- The sems can card probe (BCAS report follow-up) — awaiting verdict.
- Card format letter A/B/C — B shipped in slice form; formal verdict open.
- GUI layout 1/2/3 — the band ≈ layout 1 shipped; formal verdict open.
- Campaign findings by ID — campaign run pending on the RC (#20).

## Inbox
<!-- newest first; the app's File a Finding appends a template here -->

### 2026-08-21 · design-wing heuristic pass · Team comments dialog
(first combined act of the installed skills: design-critique/Nielsen
lenses on the newest surface, findings → engineering flow → verify)
- **H1/H9 violation:** with no identity set, the Add button LOOKS
  active but silently no-ops (the caption hints, the button lies).
  DISPOSITION: FIXED — Add disabled with explanatory tooltip until
  an identity exists.
- **H5/wording:** the dialog never states that comments are permanent
  (append-only); only the band tooltip says so. DISPOSITION: FIXED —
  the permanence line now lives in the dialog itself.
- Append-only/no-delete: RULED (matches the rulings-record ethos;
  permanence now stated where it applies).

### 2026-08-21 15:00 · FIELD CRASH (Adam) · card layers dark → toggle → SIGABRT
Crash report: uncaught C++ exception from allcore::Spine::corpusSearch
(__cxa_throw → terminate), 15:00:33 — exactly while press step 6c
rsync'd the data folder UNDER a live instance Adam had reopened
mid-press. The yanked sqlite file explains BOTH symptoms: layers went
dark (queries failing) and the corpus toggle then died on the throw.
DISPOSITION: **FIXED, twice over, same hour** —
1. All 15 public Spine query methods now degrade to empty on any
   internal error (never abort; stderr notes the suppression); the
   exact field event is fossilized as spine_resilience_smoke
   (suite 70: healthy copy answers → file yanked+garbled under the
   open handle → five hammer rounds → alive and empty).
2. The press installer refuses a live target: any running instance is
   politely quit (osascript, 20s grace, then stopped) BEFORE rsync
   touches /Applications; step 8's relaunch brings the new build back.
Blindspot class: *the installer was only ever tested with the app
closed — the human habit of reopening mid-press was never modeled.*

## 2026-08-22 · "The user should never need to leave the app" (Adam's principle)

Adam's ruling: a translator should never have to leave the application
to find a scan, install a collection, convert a script, or research an
author. Audited every one of the 13 `QDesktopServices::openUrl` call
sites against it.

**The line drawn.** "Never leave" means never leave to GET WORK DONE.
It does not mean rendering other people's sites inside ours — pulling
Treasury of Lives or 84000 content into our own UI would take their
scholarship without their attribution, and would be the one line in
the licensing manifest we could not defend. The test applied to each
site: *does leaving break the work, or is leaving the correct
outcome?*

| Exit point | Verdict |
|---|---|
| L11640 PDF fallback when QPdfView is unavailable | correct — local file to the system viewer |
| L13507 `if (u.isLocalFile())` | correct — local file |
| L18312 "Reveal in Finder" | correct — literally what was asked for |
| L18481 unhandled file type | correct — texts, images, and archives are all intercepted in-app above it; only e.g. .docx falls through |
| L31249 "Open the updates folder?" | correct — local folder, user consented in a dialog |
| L31572 findings file · L31681 bug-report mailto | correct — hand-off to the tools that own those jobs |
| L2134 · L5693 · L16523 · L19965 · L28676 · L29244 — the `startsWith("http")` link-out tiers | correct — these reach BDRC work/person pages, 84000, Treasury of Lives, Lotsawa House. Reference-site reading is the category where leaving IS the right outcome. |

**Verified in-app, not exits:** scans render through IIIF fetched by
the app (`iiifpres.bdrc.io` collection manifests) and `openPath()`
routes png/jpg/jpeg/tif/tiff to the scan viewer, texts to the Overlay,
archives to the in-app browser. Converters are our own engine ports.

**Two genuine flow-breaks found and fixed** (commits 519aac0, edb6d9e,
fd29861, 73ce8bc):

1. **Collections.** The downloader existed and worked, but the visible
   button opened a LOCAL file picker while the real one hid in a
   Maintenance menu under "check for updates". Worse, the three
   official URLs were only a fallback, and a refused HEAD request left
   a row with no download button at all. Now the three are the window's
   spine, always installable, direct links shown.
2. **Author research.** Treasury of Lives links pointed at the RDF API
   endpoint, not a biography — the link never worked. Coverage was 43
   of 350 author pids. Now mapped via Wikidata's P4138 formatter and
   widened to 193, with third-party matches labeled as such.

**Left open:** backlog #33 — searching the Library BY author (ACIP or
Wylie, plus pronunciation-approximate). Today an author's works are
reachable only by landing on one of their texts first.

## 2026-08-22 · Author search would have missed 80% of authors who have texts

Measured before wiring the UI, not after. The matcher built in 7d13801
searches the author names in `persons_bdrc.json` (183). The mapping
from an author to their TEXTS lives somewhere else entirely, in
`acip_person_links.json` (1,768 per-text links, 209 distinct authors).

The two sets barely overlap:

| set | count |
|---|---|
| link-table authors (these HAVE local texts) | 209 |
| persons_bdrc authors (what the matcher searches) | 183 |
| in both | **41** |
| have texts but the matcher cannot find them | **168** |
| findable but with no texts behind them | **142** |

So wiring the UI to the matcher as built would have meant: four out of
five authors who actually have texts in the library are unfindable,
and four out of five authors you CAN find show you nothing. It would
have passed a demo on Tsongkhapa (who is in both sets) and failed on
almost everyone else.

Coverage of the link table itself is total — every one of the 209
authors has at least one local text, and every linked work is present
(135/135 for Dngul-chu Dharmabhadra, 132/132 for Tsongkhapa). So the
"matched an author but they have no local texts" empty state, which I
had assumed was the common case worth designing for, does not arise
from the link table at all. It arises only from persons_bdrc names.

**Consequence for the design**: the searchable name set must be the
UNION, and each result must say which of the two it came from, because
they promise different things — a link-table name promises texts, a
persons_bdrc name promises a biography.

**Second finding, unlooked for**: the link table carries OCR-grade
spelling variants of ONE person as separate author strings —
"'JAM DBYANGS BZHAD PA NGAG DBANG BRTZON 'GRUS",
"'JAM BBYANGS BZHAD PA'I RDO RJE",
"'JAM BYANGS BZHAD PADO RJE NGAG DBANG BRTZON 'GRUS" are all Jamyang
Zhepa. The Spacing and Phonetic tiers therefore have a second job
nobody specified: clustering the catalog's own spelling variance, not
just the user's. Those variants must be offered together while
remaining separately labeled — the homonym rule cuts the other way
here, and telling the two situations apart is a human's call, not the
machine's.
