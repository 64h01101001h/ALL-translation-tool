# GMR cataloging sessions — the master at the bench

*Adam's directive (2026-08-19): "learn and grok what you can from
these videos to help build out the catalog workflow." This document
accumulates one section per session as Adam supplies the links. It is
the companion to DCC_CATALOG_AGENT_DEMO.md (the Lon & Tom AI demo) —
that doc is what an outside engineer built; THIS doc is how the
authority himself catalogs, in his own words, watched at work.*

*Transcripts + metadata: `data/research/gmr_cataloging/` (yt-dlp
auto-captions; speaker names and Tibetan terms are approximate — the
captions garble both).*

---

## Session 1 — "ENG Session 1 - Cataloging DCC with GMR", Dec 22, 2025
youtube SBceNNuHtRc · 58:14 · Diamond Cutter Classics channel

The founding session of the cataloging class. GMR frames the whole
enterprise, shows the database live, walks one real case (Rendawa's
sungbum), and assigns the first team job.

### The stakes, in his numbers (0:00–12:00)

- The database is **38 years** of input (HP grant via David Packard
  Jr., data entry centers in Asia, second- and third-generation
  typists), **80 million words**, **~20,000 books input** — but only
  **~10,000 cataloged**. The other ~10,000 sit in an uncataloged
  folder he has not touched in ~20 years.
- **Five-pass entry standard**: two typists type independently, two
  comparison passes, a fifth check — "it's as accurate as the wood."
  This is the provenance bar everything else is judged against.
- He personally cataloged **every book that entered, 1987–1999**, and
  **no book has entered the catalog since 1999** (three-year retreat,
  then no time): "I can either die and that'll just disappear… the
  books that didn't make it into the Kangyur disappeared in Tibet."
- Corpus scope: Kangyur 990 works (all input by ~2005), Tengyur ~3,700
  Indian commentaries (all input — "the only surviving books in
  Tibetan translation from India, we covered all of them"), Tibetan
  commentaries est. 300,000 of which maybe 60,000 are high quality.

### The architecture he defends (6:00–9:00)

- **The database IS the file system.** "We don't use a database
  structure… we put the books into the file system of every computer…
  and we built a search program that can go into the operating system
  and read it." Unchanged in 37 years; searches 80M words in seconds.
  (That search program is Gofer — the grammar our Search pane ports.)
- **Subject categories are shelves.** Kangyur ~10 top categories,
  Tengyur ~14 (plus a "secret catalog" of ~100 they used), and their
  own creation: **thousands of subject categories over all of
  Buddhist literature** — ~167 tantras, Vajrayogini alone split into
  ~20 subject shelves. "No one's ever done that in history." BDRC by
  contrast "has no subject cataloging — the books are just in a big
  pile."
- **A book on the wrong shelf is lost forever.** The Columbia
  University story: a 100-volume Narthang Tengyur found in a
  janitor's closet. "If you put it in the wrong place… no one can
  ever find it."

### The gate (8:46–10:20, 16:40–17:55)

- "Garbage in, garbage out. I'm fanatical about the control of the
  data… Every book has to pass through my eyes." Only a small percent
  of Tibetan literature is high quality; the mission is to find and
  preserve exactly that.
- The same gate applies verbatim to the AI plans: "You cannot have a
  great AI if it doesn't have good data… If the data is a lie and the
  AI massages it, you get double lie." — "We need a defined data set
  and we have to guard the border of that data with our life."
- Sectarian accusations answered: "I have no idea who's Gelukpa,
  who's not. If the stuff's good, I put it in the database."

### The teaching method (11:15–13:15, 47:10–47:40)

- He will not teach cataloging as a course — he tried for years and
  it failed. Instead: **he catalogs, stream-of-consciousness, on
  screen; the class watches and gets thrown lookup tasks.** "Look
  this up. Find this for me. That way you can learn to catalog."
- Trust is earned by volume: "You've got to do a few hundred to do it
  responsibly. I did 10,000. When I trust you guys, I'll let you do
  it, but I will not trust you now."

### The live case: Rendawa's sungbum (21:45–43:00)

The best workflow specimen in the session — a real uncataloged
collection worked in front of the class:

1. **Why this text**: he just translated a commentary by Rendawa
   (Tsongkhapa's teacher, "the first scholar in Tibet to understand
   Middle Way according to Nagarjuna and Chandrakirti"), wrote his
   biography, and judged it a gap: "If you ask me what's the first
   books to put in after your 25-year vacation — Rendawa is a good
   choice." *Identity and importance judged first, files second.*
2. **Witness census**: two sungbums exist — a 4-volume and a
   10-volume — "and I have a feeling they are not the same works."
3. **Witness quality read from the page image**: seeing a clean
   computer font he says "I get scared… it's too nice. It's not a
   woodblock — somebody in the last 10 years typed it in. They do
   not do multiple entry." Then the proof of corruption: **the
   table of contents titles are made up** — "they misspelled all of
   them… they are not the correct titles of the books." Verdict:
   "a corrupt manuscript" — but "there is nothing else. There is no
   carving," so it is used, eyes open.
4. **The input standard for a bad witness**: ACIP's typing of the bad
   scan is "a faithful reproduction of a corrupt version — seriously,
   we didn't add new mistakes." *Fidelity to the witness, never
   silent correction; corrections happen downstream, attributed.*
5. **Woodblock supremacy**: a carving took ~3 years and was monitored
   by geshes — "generally they're very accurate and that's what we
   want." Scans of carvings are BDRC's great contribution ("you
   cannot search a scan… somebody has to input it" — and Tibetan OCR
   is, in his estimate, still far from the bar).
6. **The witness hunt is delegated, by name**: check BDRC for a
   better Rendawa witness; contact **Carola Roloff** (Buddhist nun,
   university professor in Germany, "a Rendawa expert — she will
   know if there's any carving"); try the **three Mongolian
   libraries** newly under agreement; try **Sakya organizations**
   (Sakya Trizin's seat, Washington state). *Cataloging includes
   provenance fieldwork, assigned to people, tracked.*
7. **Sanskrit ornamentation warning**: the 1700s title-page Sanskrit
   is "completely wrong… by that time Sanskrit was only used for
   ornamentation and no one could read it." *The RGYA GAR SKAD DU
   line of late witnesses is evidence, not gospel — relevant to our
   identity lane, which carries it as evidence only.*

### Catalog numbers and the registrar (41:53–42:05)

"If you want to put a book in the database, you have to go to Uncle
Nick and he gives you a catalog number." — Number issuance is a
single human registrar's act. Books cited in Mixed Nuts
bibliographies already received numbers this way **without being in
the database yet**: numbered ≠ cataloged ≠ input.

### THE FIRST JOB he assigned (42:08–45:40, 56:17–57:25)

> "Take all the bibliographies of all the Mixed Nuts books we have
> finished — about 15 to 20 — and confirm that every one is in the
> database, because if a book made it into a bibliography it was
> important enough that it sure as hell better be in the database.
> And they're not. I'd guess it's going to be a hundred books, and
> they're all important."

He demonstrates the check live: search the catalog number (S464) with
the search tool; first book found, second book **not in the current
database**. "What if it's not? — We put it in."

**WE RAN THIS EXACT AUDIT against our installed library the same day
(2026-08-19), from the extracted bibliography banks
(mixed_nuts_bibliography.json + apparatus_bibliography.json, 596
ACIP-number citations):**

- 349 distinct ACIP catalog numbers cited across the published
  bibliographies;
- **298 present in the installed library, 51 missing** (list banked
  at `data/research/gmr_cataloging/bibliography_audit_missing.json`),
  incl. three KL Kangyur texts cited by the Sixty Verses volume and
  a cluster of S-numbers cited by King of Dharma.
- Caveats: our library is the app's installed subset, not ACIP's
  master tree — "missing here" means "missing from what this app
  has," a lower bound on what GMR's own audit would find. R-prefix
  numbers (Russian/St. Petersburg holdings?) may live outside the
  collections we ship.

### The ACIP-vs-Wylie aside (51:30–56:10)

The economics of the input code: TZ vs TSH etc. — the H "adds 30% to
your workload" across 80M words ("we have spent $15 million; you
increase the typing by a third, you'll pay $20 million"), and the
systems interconvert trivially ("search and replace… ten seconds").
"He mistook transcription for pronunciation." — context for why rule
sets like ours must treat ACIP as the native register and conversion
as mechanical.

### The team (19:45–21:30)

Mixed Nuts count-off roster as of this session, alphabetical by first
name: 1 Adam · 2 Ben Kramer · 3 Dr. Blue · 4 Gibson · 5 Nick · 6 Si ·
7 Stanley · 8 Sukang(?) · 9 Tim · 10–12 (caption-garbled). Named
roles: **Nick** = catalog-number registrar ("Uncle Nick") and witness
supplier; **Anatol(iy)** = corrections/latest-version keeper of
finished translations; **Jamie Plant** = Knowledge Base director (AI
tooling, with Bo); **John Brady** = scan-acquisition ("goes and finds
books and takes pictures of them").

### The AI mandate given to Jamie (13:20–17:55)

Take everything ACI translated since ~1991 — "probably 50,000 pages
of paragraph-by-paragraph Tibetan-English… I built it for this day" —
clean it to be AI-grade data (old fonts etc.), so AI can hypertext
paragraph pairs into a lookup dictionary and eventually translation.
*This is precisely the corpus lineage our spine already consumes; his
"clean blocks of Tibetan and English" is our full_parallel_corpus.*

---

## What this changes about our Catalog workflow (TODO 9g)

Adoptables extracted 2026-08-19; the numbers refer to the 9g roadmap.

1. **NEW LANE — Bibliography audit (GMR's job #1, automatable
   today).** We hold the bibliographies AND the library; the app can
   render the 51-missing report live, per source volume, with the
   citation text as evidence. This belongs beside the intake census:
   intake answers "what have we got?", the audit answers "what are we
   MISSING that the published record says we must have?" → queued as
   9g(2a), ahead of the Lon-demo QC lanes.
2. **Worksheet fields earn their place (9g step 3).** The live case
   used: witness census (how many versions exist, are they the same
   works), witness quality (carving > vetted manuscript > modern
   single-pass typeset), corruption evidence (TOC titles fabricated),
   input-fidelity note ("faithful reproduction of a corrupt
   version"), provenance leads (who to ask, by name, with status).
   These become worksheet fields.
3. **Number ≠ catalog ≠ input — three states, not one.** A work can
   hold an issued number and exist nowhere. The worksheet must carry
   the states separately: number issued (by whom) / input exists /
   cataloged to a shelf.
4. **Registrar discipline (9g step 4).** Catalog numbers are issued
   by one human (Nick). The app NEVER mints numbers — routing exports
   "needs a number" as a request, never an assignment.
5. **Subject shelving is the end game.** GMR's catalog is subject
   shelves (thousands), and the destination tree in our dual-browser
   pane is literally his mental model ("the database is the file
   system", shelves are folders). The step-5 handoff should
   eventually offer shelf suggestions from r6_work_subjects /
   sungbum_subjects — labeled suggestions, human-placed, because "a
   book on the wrong shelf is missing forever."
6. **Trust gradient in the UI.** He grants cataloging authority only
   after "a few hundred" supervised reps. Our approval routing
   already matches (charter: machine suggests, humans approve); the
   worksheet should record WHO proposed and WHO approved, so the
   record of reps exists.
7. **Sanskrit head caution, confirmed from the source.** Late
   witnesses' ornamental Sanskrit is often garbage; our identity lane
   correctly carries it as evidence-only and must never key a match
   on it alone.

## Sessions pending

Adam will supply further links; add a section per session above and
extend the adoptables list. Cross-reference: DCC_CATALOG_AGENT_DEMO.md
(the Lon & Tom demo — GMR's verdict there: "it finally caught up…
now it's usable").
