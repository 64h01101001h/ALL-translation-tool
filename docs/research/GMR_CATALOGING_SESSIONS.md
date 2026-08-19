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

## Session 2 — Jan 12, 2026 (gYv8bbfB3N0, 1:00:13)

The stakes restated (eight Diamond Way course tracks all depend on a
healthy database; ~15,000 books cataloged personally, "many done
waiting in the bus line at the Port Authority"), then **Jamie Plant's
Knowledge Base demo** and the **bibliography-gap run**:

- The Knowledge Base: AI transcripts of 4–5,000 classes, chunked
  (~2-min), levels (transcript → AI summary w/ tags → Bo's hand-marked
  Tibetan-outline lines), everything time-linked to audio/video and to
  the source-text folio. Editor flow: search → edit → submit →
  admin review → save. 30,000-entry glossary auto-built from GMR's
  paragraph-aligned translations; separate **title glossary** for
  title-translation consistency; bidirectional English↔Tibetan search
  over the ACIP database; BDRC search embedded in-app; scans synced
  folio-by-folio to input (Kangyur 100% coverage, Sungbum 40% — a
  missing-scan list to be published).
- Jamie's catalog-tool sketch, blessed by GMR: recognize the title,
  show how it was translated before, **cataloger assigns the shelf and
  writes a short reason why** — the system learns from the reasons;
  "not that the tool would do the cataloging automatically… make the
  cataloger's job more informed. It's still the choice of the
  cataloger."
- The bibliography-gap AI run (GMR's job #1 executed): sources = the
  newest bibliography + a catalog of the current database (**5,606
  cataloged texts**); results TRIAGED INTO BASKETS: (a) cited works
  already input but sitting in uncataloged data — start here; (b)
  never input — with WHERE to find each (BDRC links, Mongolia,
  scans/etexts); (c) works hiding inside UNSPLIT volumes, with
  attempted machine cuts. "First we confirm whether the AI's findings
  were correct… then we put them in baskets."
- The Narthang Tengyur split: ~3,000 texts in 210 volumes where a text
  ends and the next begins ON THE SAME LINE with no marker; Oscar a
  quarter through manually; Jamie's machine pass (line-start/line-end
  per text per volume) to be shared — "focus the manual labor on
  verifying what the AI found."

## Session 3 — Jan 19, 2026 (2uwH3zbj71c, 1:06:41)

Database anatomy, future-proofing doctrine, and the ASCII catalog:

- Structure: Kangyur 12 top categories + native catalog as the 13th;
  Tengyur ~16 (tantra subdivided by the four levels); Sungbum (the
  Tibetan commentaries tree); Reference (catalogs, dictionaries,
  official/historic documents). Oversized catalogs live outside the
  searched tree because they "blow up search programs" — St.
  Petersburg institute catalog alone is **141,000 entries** (plus
  8,000 at the university across the river). Aisha Scuffil is building
  the parallel **Sanskrit database** (nearly done after years).
- **Future-proofing doctrine** (the Jean Smith / Ask Sam story: his
  database program died and took the data): ACIP's job is "a very
  primitive, very low-level, but unchangeable record"; fancy tools
  last 10–20 years and are rebuilt on top. Whole DB downloadable free,
  runs offline ("desert island"), direct download without Dropbox
  ordered for Feb 14. Backup policy discussed: keep EVERY old copy
  (an ever-growing pile of drives — a corrupted master must never
  overwrite history), air-gapped machines on several continents, the
  threat model being a silent mass rewrite of internet-connected data.
- **The divergence problem**: GMR's copy and Nick's Dropbox master
  have drifted for years. Task to Jamie: AI-compare the two trees,
  list the differences; Nick confirms subject categories for anything
  not in GMR's copy ("we didn't subject-categorize anything that
  wasn't approved by you").
- **THE ASCII MASTER CATALOG** (the session's core directive): the
  old ACIP catalog file (last touched ~2012) listed every cataloged
  book in field-coded ASCII. The St. Petersburg catalog shows the
  format at its richest: `S:` our number · `N:` the holder's own
  catalog number(s) · `T:` title · author · format (xylograph) ·
  page count · **physical measurements of page AND text block** ("an
  irrefutable proof you got the right book") · **the colophon**
  ("extremely valuable"). GMR: "We have the database, we have the
  folders, but we don't have a separate LIST of what's in the
  folders… I want this team to go back to having an ASCII catalog."
  Where St. Petersburg already holds the same copy, copy their entry.
- Catalog numbers: deliberately NON-sequential (decided in month one,
  1987 — "we keep finding new books every day"), deliberately short
  (vs BDRC's "unwieldy, unusable" 10–30 char IDs). 8-character
  filename stems on old folders are DOS-era relics, un-renamed.
- Input-center map (bonus task): 2 Tibetan centers in the south, 2
  Sanskrit centers (Varanasi, Kathmandu), Mongolia (National Library
  staff shifted from cataloging to input; a small West Mongolia
  monastery cataloging). Year-end 10% wage bonus ordered for all,
  timed to Losar.

## Session 4 — Jan 26, 2026 (Su3f_wIcXkM, 1:03:30)

**The definitive procedural specimen: one text cataloged end-to-end
live** (Rendawa's sungbum, vol. 1, first biography):

1. **Works-list first**: get the definitive list of the author's
   works from the best native catalog (here "Shedza" — a modern
   Tibetan catalog of the lineage's important books), searched via
   Gofer. Skip entries where the person is vow-master / recipient /
   requester — you want authored works. The parenthesized numbers =
   position in volume + page count; a `lo rgyus`/biography row marks
   the end of the works list. Volume numbers are the HOLDING
   INSTITUTE's shelf numbers.
2. **Provenance travels**: Gofer's Command-X copies the hit WITH its
   file path — the extract lands in a working doc labeled "catalog of
   Rendawa's works from Shedza."
3. **Witness comparison**: 4-vol vs 10-vol sungbum; work from each
   volume's kar chag (TOC) start to finish, "get the names right and
   get the names tied to their proper files."
4. **Author is NEVER assumed** — not from the sungbum ("common error:
   biographies in a sungbum's first volume are ABOUT the author, not
   BY him"), not from TBRC ("I don't accept other people's work… if
   we misidentify the author, we may relegate this biography to
   obscurity forever. It is our duty to find the author."). Method:
   find the text's END (search the NEXT work's title from the kar
   chag; fall back to page numbers when hard returns break the title
   search), then read the colophon backwards. Here the author
   self-identifies with a Sanskrit pen-name (Ratna—; flagged: also a
   deity name, so searches will be noisy).
5. **Dates**: BDRC is trusted for dates (Jean Smith's dating
   tradition: "a bad guess by Jean Smith is better than an excellent
   guess by Geshe Michael"); when absent: n.d. / BDRC's new "dates
   undetermined"; house style `????-????` variants; "fl. 1405" when
   only a flourishing date is known.
6. **Number issuance**: check the **Tohoku catalog FIRST** (covers
   Kangyur, Tengyur, and several thousand Tibetan works) — if Tohoku
   has it, USE the Tohoku number; check spelling variants before
   concluding absence ("it was one of the first books we ever input
   and the monks made some mistakes"). Only then does Nick issue an
   ACIP number from his spreadsheet (S25239 here; sequential within
   the spreadsheet's tail).
7. **The mother copy is sacred**: copy the input to a "for cutting" /
   "chop" folder before any surgery. Cut markers: `XXX` at a work's
   start, `$` at boundaries ("an odd character you wouldn't normally
   see").
8. **File the cut text**: plain ASCII, extension **capital .TXT**
   ("our standard"); filename `NUMBER_TIBETAN TITLE_ENGLISH
   TITLE_AUTHOR`; BZHUGS SO (and sutra "nāma") never part of the
   title. Filed by **findability**: "the purpose of the database is
   to help people — put it where most people would look" (Rendawa,
   not his monk's name; the cheese-grater rule). Folder =
   collection/genre/person-with-dates ("Rendawa (1349-1412)").
9. **THE CHANGE LOG**: after adding to the database, rename the
   collection folder with today's date + your initials — "so we know
   you changed it on this day, and to blame you if something went
   wrong. That is our policy."
10. Western-style page numbers in an input (no folio A/B) mean: typed
    book → single-pass → suspect; "a faithful reproduction of a
    corrupt version" is still the input standard.

## Session 5 — Feb 2, 2026 (Ivu7w828vJI, 1:06:54)

Second live text (the second biography in Rendawa vol. 1):

- "**If it's typed, it's suspect. If the suspect typer gives you a
  free table of contents — beware.**" This TOC's page numbers and
  names are wrong in both directions; composing a TOC is authorship,
  and these authors are unqualified.
- Boundary caution at closing formulas: a line before "Mangalam
  bhavantu" may belong to the NEXT text; a past helper who cut
  hundreds of titles made this mistake so often GMR discarded all of
  it. ("Depends on the mood of the typist, which is why I don't like
  typists.")
- Colophon reading practice: place-of-composition ≠ author (the
  cave-where-Milarepa-set-his-feet line names the PLACE); the author
  turned up mid-file (unusual), verified against BDRC AND GMR's own
  prior research before acceptance.
- Title translation practice: descriptive title + poetic title —
  **English leads with the poetic title** (found at the END of the
  Tibetan); "the wondrous" over-literalism debated live.
- Filename punctuation: **no colon** (illegal); ONE hyphen, no
  spaces, where the Tibetan had a colon-like break; double-space
  typos hunted in names.
- **The filename length limit struck live** (the OS refused the
  name): the fix deferred to "a metafile" — see session of May 5.
- Numbers S25240, S25241 issued; Nick's spreadsheet is the register.

## Session 8 — Mar 2, 2026 (E_oY_qsQUiw, 1:03:52)

Nick drives, GMR supervises ("you and Ben do the work and I yell at
you — better for the future"):

- ACIP transliteration as **provenance watermark**: "by glancing at
  the data you can see that it's our data… it identifies the data as
  having gone through John Brady's five levels of correction."
- Reading a modern reprint: skip the publisher's front matter; page
  numbers without A/B = western-style pagination; folio front/back
  letter marks in carvings explained; "multiply Tibetan pecha pages
  by ~8 for English-equivalent length."
- **The cut-backwards method** (for when the TOC breaks down): jump
  to the END of the volume and work backwards. Justification: "once
  the book is split up, it never needs to go back together again —
  all the Kalachakras go to the Kalachakra shelf… they will never
  meet again."
- **Remove-as-you-extract**: each extracted work is deleted from the
  working copy, so the residue shrinks and you should run out of
  pages exactly at the volume's end — a built-in completeness check.
- **Do not split what the author didn't split**: the TOC's last four
  "titles" were the four parts of ONE empowerment text (preliminary /
  main / wrap-up structure); the give-away was impossible page
  numbers (the preliminary "starting" after the main). GMR's teacher:
  "You don't mess with it. You're not good enough to do that. They
  don't need you to do the chapters 600 years later."
- End matter (publisher credits, government publishing info, "here
  end the collected works" lines) is cut away — added by publishers,
  not part of the work.
- Commentary-on-a-commentary chains: find the root text via the
  Tengyur's "in the language of India…" heads; take the Tohoku number
  (Toh 1785 here); then VERIFY Tohoku's author claim against the
  text's own colophon ("it's possible a human being made a mistake —
  the guys in 1937"); read past the TRANSLATOR credit at the very end
  (translator ≠ author) to the "written by master…" line.
- Numbers S25246–47; the register must be updated same-day so numbers
  never double-issue. A catalog number's real purpose: "mostly to
  help find the text in an emergency — you just search the title."
- Lowercase-typed lines are input errors ("somebody typed in
  lowercase letters illegally") — search still works; fix later.

## Untitled continuation sessions (all "Mixed Nuts Translation…" uploads)

**Mar 17 (wkLnUX3Qhc8)** — Rendawa vol. 2 turns out to be ONE text
continuing (whole-volume single title; Ben pre-checks vol. 3 to prove
it doesn't continue further). Title practice: word-choice consistency
is a public act — "people are going to emulate your work… you're a
model" (bshad = explanation vs bshad-sgrub distinctions; "a vast sea
of explanations" for rgya-mtsho used augmentatively). "Someone's
going to teach an AI to do this — let's give him some [data]."
Shared chopped-texts folder discussion begins.

**Mar 24 (bq8SfTu558c)** — NO CAPTIONS available on this upload; not
yet analyzable by transcript. (Candidate for a whisper pass if Adam
wants it mined.)

**Apr 28 (DDEyyjyya08)** — Boundary/pagination rules: a short cut
text with no page number in its body does NOT get one inserted
("misleading — it's not the first thing on the page"); instead
**folio start/end are recorded in the master spreadsheet** (the
number register grows folio columns). **Keep the uncut volume too**:
GMR files the whole uncut volume alongside the cuts "in case someone
wants to look up where it was" (his own Muchen Q&A precedent: cut
into works by topic AND kept whole; "letters should also be one bag").
Each cut work gets a FULL catalog number — never parent-number+part
("we treat each work as if it's worth its own attention"). Ben's
VS-prefix (volume-level number) idea: "sounds dangerous, let's talk
later." A dubious TOC that breaks a name mid-word (Lokeshvara):
leave the Tibetan alone, clarify the English.

**May 5 (paWZztYor74)** — **The METAFILE convention, fully
specified**: when a filename hits the OS limit (~255), fill the name
to the max and STOP EVEN MID-WORD (no trailing spaces — a maxed-out
name is machine-detectable), append `+`, and create a companion
`_META` file (capital) in the same folder carrying the full title,
dates, and the rest; "sometime in the future somebody will run an AI
program that will rejoin them when the architecture allows it."
Typography as provenance: a parenthesized title in the input means
SMALL PRINT in the source — likely an outside insertion, and if the
source was carved, the suspect TOC-maker probably didn't invent it.
Mixed Nuts rule: a tantra system named in English always carries the
Sanskrit in parentheses (no diacritics in filenames). Honorific
titles ("my precious teacher") avoided as presumptuous — "our
teacher" as the neutral out.

**May 12 (GEFKe9pJB9o)** — Nick proposes streamlining (pre-cut +
spreadsheet, skip the public cutting); GMR keeps the live cutting
"because people need to learn that too" — the class IS the training
pipeline. Genre filing: gdams-ngag/"Advices to…" lane. Page number
sitting exactly at a cut boundary "goes with what comes after it,
normally." Dropbox discipline: close files before handover so no
conflicted copies; Nick backfills the day's cuts into the register
spreadsheet during class.

**Jun 16 (Edj39XoNVW0)** — Misspelling vs variant protocol: a wrong
letter in a filename is corrected outright, but FIRST check whether
the form recurs in the text and the scan (it appeared twice in the
scan → maybe the source's own spelling; semantic sanity check — "you
don't get painters who are local officials normally"); the TEXT is
never silently corrected. Personal names in titles disambiguated
with "of PLACE". The initials-on-folder policy repeated verbatim at
close ("when you update the folder name, put your initials at the
end").

**Aug 4 (MBJXM1YD5pk)** — Cataloging jumps queues by IMPORTANCE
("we've always done this: go for the most important book and get way
behind on the normal cataloging"): a pre-Tsongkhapa Kagyu Vinaya
classic (2 huge volumes, monks/nuns) whose woodblock FOOTNOTES are
carved half-size with tiny dot-markers that wear off the block —
"footnotes floating around the book"; input attempted 2–3 times,
still unsatisfactory. NEW CLEANUP LANE: someone inserted a diagonal
slash between every line of Jamyang Shepa's Abhidharma commentary —
breaks cross-line search; strip it corpus-wide. Volume suffixes in
filenames discussed (old style: capital letter; E-file era). "Take
the title from the TEXT, not from the TBRC cataloging guy — probably
the same, but don't trust it." Reader-request lane: a student asks
for a missing ritual text → email GMR → he locates a witness (own
copy or TBRC) → input.

**Aug 11 (JLGDWuJuokM)** — Corrections policy for PUBLISHED
translations: re-typeset every ~10 years; in between, **errata
pages** maintained by Anatol (who also keeps reader-found
corrections "in a sacred place he never loses" and feeds them to
authors' next editions). Numbers now in the S253xx range with the E
language suffix spoken aloud as part of the name. Priority detours
again (Dunhuang Kamalashila witnesses of the Diamond Cutter
commentary — wait until all three versions are in hand, then do them
together). NOTE: GMR announces Lioran Katz (AI-for-governments,
Thailand) will demo translation tools "similar to what **Adam's**
been working on" — this project is on the class's radar; both tools
are built on GMR's own translations, "the only ones I would want to
use."

---

## The house rules, consolidated (the build spec)

Every rule below was stated or enacted on camera; session in
parentheses. This is the specification our Catalog workflow should
implement or support.

**Identity & authorship**
- Works-list from the best native catalog first; provenance travels
  with every extract (S4).
- Title from the TEXT, never from a TOC or TBRC (S5, Aug 4). TOCs of
  modern typed editions are compositions by unqualified authors (S5).
- Author from the colophon; never assumed from the collection, never
  copied from BDRC without verification; translator ≠ author; place
  ≠ author (S4, S5, S8).
- Dates from BDRC (Jean Smith tradition); n.d. / fl. / ????-???? when
  unknown (S4, S5).
- Sanskrit heads of late witnesses are ornamental and unreliable (S1).

**Numbering**
- Tohoku number if Tohoku has the work (check spelling variants);
  else the registrar issues an ACIP number from the register
  spreadsheet; numbers non-sequential by design, short by design;
  the register gains folio start/end columns per cut work; update
  same-day (S4, S8, Apr 28).
- Every cut work gets a FULL number — never parent+part (Apr 28).
- Number ≠ input ≠ cataloged: three independent states (S1, S2).

**Splitting volumes ("chopping")**
- Never touch the mother copy: work in a chop copy (S4, S5).
- Markers: XXX at starts, $ at boundaries (S4).
- Find ends by searching the next work's title; fall back to page
  numbers; cut-backwards from the volume end when the TOC fails (S8).
- Remove-as-you-extract: the residue shrinking to nothing is the
  completeness proof (S8).
- Do not split what the author didn't split (empowerment parts,
  chapters) (S8); do keep the uncut volume filed as well (Apr 28);
  letters = one bag per collection (Apr 28).
- Boundary details: closing formulas may belong to the next text
  (S5); a page number at the cut goes with what follows (May 12);
  no fabricated page numbers at cut heads (Apr 28).

**Filenames**
- `NUMBER_TIBETAN TITLE_ENGLISH TITLE_AUTHOR` + capital `.TXT`;
  plain ASCII files (S4).
- BZHUGS SO / nāma dropped from titles (S5); poetic title leads the
  English (S5); tantra systems get "(Sanskrit)" (May 5); no
  diacritics; no colons; single hyphen, no spaces, for internal
  breaks (S5); "of PLACE" for ambiguous personal names (Jun 16).
- At the OS length limit: fill to the max, stop mid-word, append
  `+`, spill the remainder into a companion `_META` file (capital)
  in the same folder (May 5; the library's `* META.TXT` files are
  this convention).
- Volume suffix conventions exist (old capital-letter style; E-file
  language suffixes) (Aug 4, Aug 11).

**Filing (the shelves)**
- Findability rule: "put it where most people would look" — famous
  name over formal name; as much of the name as you have for common
  names (S4).
- Folder = collection / genre / person (anglicized, with dates)
  (S4); genre lanes incl. namtar (biographies), gdams-ngag (advices)
  (S4, May 12).
- A book on the wrong shelf is lost forever; subject shelves are the
  whole point (S1, S3).

**Provenance & fidelity**
- Witness quality: carving > vetted manuscript > modern single-pass
  typeset; typed = suspect; free TOC from a suspect typer = doubly
  suspect (S1, S5).
- Page-number style, parenthesized (small-print) titles, western
  pagination, lowercase runs — all read as provenance signals (S8,
  May 5, Aug 4).
- Input reproduces the witness faithfully, errors included;
  corrections happen downstream, attributed; the TEXT is never
  silently corrected even when a filename is (S1, Jun 16).
- Change log: renamed collection folder carries date + initials of
  the last changer (S4, Jun 16). Published translations: errata
  pages between decennial re-typesets (Aug 11).

**Process & people**
- The registrar (Nick) is a single human; the master register is a
  spreadsheet; conflicted-copy hygiene on shared storage (S4,
  May 12).
- Trust is earned by supervised volume ("a few hundred… I did
  10,000") — the live class IS the training pipeline; streamlining
  that removes the teaching is rejected (S1, May 12).
- Queue jumps by importance are normal and deliberate (Aug 4, Aug
  11).
- Machine assistance is welcomed for: tree diffs, bibliography gaps,
  volume splitting, scan↔input sync, missing-scan lists — always
  followed by human verification of the machine's findings, in
  baskets (S2, S3).
- Future-proofing: ASCII master data, an ASCII master catalog
  (field-coded, St. Petersburg-style: number / holder's number /
  title / author / format / pages / physical dims / colophon), keep
  every old backup copy, offline-capable (S3).

## Sessions pending

All 13 links Adam supplied 2026-08-19 are analyzed above except
Mar 24 (bq8SfTu558c — no captions on the upload; whisper pass
possible on request). Future sessions: add a section above and fold
new rules into the consolidated spec. Cross-reference:
DCC_CATALOG_AGENT_DEMO.md (the Lon & Tom demo — GMR's verdict there:
"it finally caught up… now it's usable").
