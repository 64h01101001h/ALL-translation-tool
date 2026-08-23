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

## 2026-08-22 · Genre register (#31): the missing piece is data, not code

Measured before designing. Adam's ask was that subject/genre govern how
words are read — subject-aware gloss ordering, register warnings,
suggestions tuned to genre, with Vinaya especially in mind because it
is the oldest language in the monastic curriculum.

**What exists, as fact:**

| | |
|---|---|
| library work keys | 5,607 |
| with an R6 subject label | 1,406 (25.1%) |
| with a Sungbum subject label | 1,554 (27.7%) |
| with **either** | **1,888 (33.7%)** |
| distinct R6 subject labels | 243, bilingual (`Middle-Way_Philosophy--DBU_MA`) |
| labelled `Vowed_Morality--'DUL_BA` (Vinaya) | 15 works, **12 present locally** (KD1–3, TD41xx) |

**What does NOT exist, and this is the finding:**

The binding layer carries almost no register annotation. Of 12,004
HGM-glossed entries, **32 carry any parenthetical at all**, and only
about **five** are register or genre marks:

- `bsod nams` → "goodness (prayer register)"
- `bden par grub pa` → "solid existence (poetic register)"
- `dgongs pa` → "thought (honorific)"
- `mdzod` → "treasure house of knowledge (abhidharmakosha)" ×2

`bsod nams` — the example CLAUDE.md itself cites for register — is
essentially alone. There is no register layer to surface.

**Why that forbids the feature as first imagined.** Reordering glosses
by genre would mean deciding that HGM means X in Vinaya and Y in
Pramana. He has not said that for all but a handful of terms.
Deriving it from course statistics is inference, and rule 1 permits
machine work to MATCH his English from evidence but never to COMPOSE
it. A subject-aware gloss ranking would be composition wearing the
binding tier's clothes — the exact failure the whole provenance
architecture exists to prevent.

I tested the inference anyway, on `bsod nams` across its 539 corpus
segments in 58 courses: "merit" 323 (concentrated in Sunlight, 77),
"goodness" 50 (spread thinly across TCS04Initiation, ILL, C03, C18),
"good karma" 40 (concentrated in GIE, 12). Real variation, but not a
clean genre split — and even a clean one would still be the machine
deciding what he meant.

There is also **no course → subject mapping**: the corpus is tagged by
ACI course, the catalog by R6/Sungbum subject, and nothing joins them.

**What the tool can honestly do, and what it cannot:**

- CAN state a text's catalogued subject — that is a fact someone
  recorded, with 33.7% coverage and the other 66% said plainly.
- CAN mark Vinaya prominently, per Adam's ruling that it reads
  differently.
- CAN show the corpus concordance GROUPED by course, so a translator
  sees the real distribution and draws their own conclusion. That is
  displaying evidence, not ruling on it.
- CANNOT reorder or re-rank binding glosses by inferred register.
- CANNOT warn "this word reads differently here" unless HGM said so.

**The real fix is authorship, not engineering.** A register layer has
to be WRITTEN by someone entitled to write it — Adam, or Geshe Michael
through the approval channel. The tool can host it, carry it through
the tiers, and surface it per genre the moment it exists. Until then
the honest feature is the three CANs above, and the gap should be
visible rather than papered over with a plausible-looking ranking.

## 2026-08-22 · #32 answers #31: the register rules are in the classes, not the dictionary

Mining 2,800 caption files of Geshe Michael's own recorded classes for
translation-method passages returned **407 candidates** — and **29 of
them are context-dependent reading rules**, which is exactly what the
#31 measurement found absent from the dictionary that same day.

Examples, verbatim from the captions (which mishear — the recording is
the authority):

- *"it doesn't mean wisdom here … don't translate it as wisdom, when
  you see it connected to [X] it always means corrupt intelligence"* —
  a register rule with its trigger stated.
- *"the four higher truths … mistranslated as noble truths … the word
  means Arya"* — a direct correction of a standard rendering.
- *"when you see [mdo dang sngags] you always have to translate it as
  open and secret teachings, you shouldn't translate it as [sutra]"* —
  the caption writes "citrus".
- Xuanzang's five classes of word that should not be translated at
  all, taught as translation theory.

**Why this matters for #31.** That item concluded the register layer
must be AUTHORED because the dictionary records ~5 register marks in
12,004 entries. That conclusion stands — the tool still may not compose
a reading. But the seed material for the authoring now exists and is
citable to the second, so the ask changes from "write a register layer
from scratch" to "rule on 29 passages where you already said it out
loud." That is a far smaller thing to ask.

**Two caption defects were fixed to get here**, and both would have made
the output useless:

1. YouTube rolling captions repeat the previous line plus a few words.
   The existing term indexer collapses only exactly-equal neighbours,
   which is right for counting term hits but leaves passages reading
   *"difficult to when you these are difficult to when you these are"*.
   Reading a passage back needs the overlap removed: keep the longest
   suffix of what is already accumulated that prefixes the new cue and
   append only the remainder.
2. Consecutive cues each trip the same pattern, so one sentence became
   three passages at t=2094/2096/2096. Collapsed to one passage per
   video per 45-second bucket.

Together these took a 200-file sample from 218 unreadable hits to 68
readable ones — fewer, and worth reading.

## 2026-08-22 · External-URL sweep (prompted by the Treasury of Lives 404)

Adam's dead biography link raised the obvious question: what ELSE points
somewhere that no longer exists? Swept all 33 URL literals in
app/main.cpp and probed every static one.

**Clean:** 84000, asianlegacylibrary.org, library.bdrc.io (all four
forms), purl.bdrc.io, old.thlib.org, online.adarshah.org,
places.kmaps.virginia.edu, lotsawahouse.org, www2.hf.uio.no,
buda-base.github.io/tibetan-ocr-app. The `https://x/...` literals are
selftest fixtures, not links.

**One real change in the world — BDRC IIIF now requires auth.**
`https://iiifpres.bdrc.io/collection/wio:<work>` returns **401** for
every work tried (W22084, W1KG5772), on 2.0 and 3.0 paths alike, from a
client BDRC otherwise serves happily (purl.bdrc.io answers us 200). The
route itself is still correct — wrong shapes return 404, this returns
401 — so BDRC has moved IIIF presentation behind their Auth0 layer
(buda-base/bdrc-auth-lib), not retired it.

Consequence: "Follow along in scans" cannot fetch an outline for
anonymous users. It FAILS HONESTLY — `followScans()` checks
`rep->error()` and shows "BDRC unreachable: <reason> — open in the
browser" with a working library.bdrc.io fallback — so this is a gated
feature, not a dead affordance, and no code change is warranted on
honesty grounds.

Disposition: **folded into the BDRC letter** (OVERSIGHT A4 / F2) rather
than patched. The letter already asks for written confirmation on the
OCR models; an API-access question belongs in the same envelope instead
of a second approach. Recorded here so the 401 is not rediscovered as a
bug later.
