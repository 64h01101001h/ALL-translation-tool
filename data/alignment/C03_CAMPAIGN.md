# ✅ WORK ORDER 2026-08-28 — BACKFILL DONE, C03 RESUMES AT 115

The coverage backfill Adam ordered is COMPLETE. C01 closed at 496/496
and C02 at 68/68, both `"complete": True` and both gate-proved; the
gate was liveness-tested on each. C03 resumes at segment 115 with 506
open, and is the only course still carrying a coverage gap.

Original work order, kept for the record:

# ⛔ WORK ORDER 2026-08-28 — BACKFILL BEFORE NEW C03 SEGMENTS

Adam's ruling: this scan is intended as the MAIN Geshe Michael dictionary,
and the coverage gaps are the errors to fix first. C03:115+ WAITS.

Order of work:
1. **C01:1–22** — the liturgical opening, never scanned. 21/22 are
   byte-identical to C03:1–22 (already done, pages c3p1–c3p7); the sole
   difference is segment 22, the per-course syllabus marker. Build as
   pages c1L1..c1L8 in pages_c01/, registering into the C01 course entry.
2. **C01:39, 43, 143** — three marker segments (English part-titles in the
   wylie field), skipped. Marker protocol, as 259/324/427/446/483.
3. **C02:1–22** — same liturgy again; 20/22 identical to C03:1–22.
4. Then set `"complete": True` for C01 and C02 in the builder and let the
   COVERAGE GATE prove it. If it refuses, the backfill is not done.
5. Only then resume C03 at segment 115.

Standing, per Adam 2026-08-28: GMR's English stays VERBATIM (typos flagged,
never corrected); nothing is binding without human review; every batch
records its producing model.

# C03 CAMPAIGN — ACI Course 3 (Applied Meditation), full-depth alignment
Started 2026-08-28, immediately after C01 closed. Standing mandate:
C03..C18 in order, three segments per batch, the C01 pipeline
(generate-both-sides from the spine, letter-exact QA, ledger,
app_selftest, commit, push backup) unchanged.

## Course shape (spine scan, 620 segments)
Liturgical opening 1–115: verse lines whose english field carries
GMR-CONVENTION PHONETICS, alternating with full-stanza English
translations (~89 phonetic segments in 66 runs across the course).
Prose from ~116 on, with scattered phonetic one-liners (138, 169,
192, 194–197, 218, 222, 230, 240, 242–245, 248, 259–264, 287, 316,
539). Classification is heuristic; every batch reads its own
segments and rules locally.

## PHONETICS PROTOCOL (new for C03)
When a segment's english field is phonetics, not translation:
1. The right-hand label reads GESHE MICHAEL'S PHONETICS (GMR
   CONVENTION, VERBATIM) — never the English-translation label.
2. Correspondences link at Wilson depth 7 (Syllable/compound
   member), NEVER depth 5. The builder harvests only d==5 into the
   evidence-pair bank, so phonetic sound-spelling pairs are excluded
   from the successor-dictionary evidence BY CONSTRUCTION while
   remaining fully linked on the page and banked in the full bank.
3. The AI NOTE on every phonetics block states the pairing is
   sound-to-spelling, not translation.
4. Blended segments (mantra + translation + mantra, e.g. seg 6):
   phonetic stretches at d=7, translated stretches at d=5, each
   under its own clause with honest labels.

## ✓ RULING CARRIED OUT (Adam, 2026-08-28) — ACIP is now the key
The equivalents are keyed on **ACIP**, the source of record. Delivered
2026-08-28: `acip_span()` in the builder (offset-based, self-proving —
a recovery is returned only if it round-trips), CTest suite
`acip_recovery` (87 suites green), `acip` on every pair record, a
top-level `acip_index` (ACIP→wylie) as the join key, `tib_acip` on
every full-bank link. Coverage 4,038/4,039; the one miss is a Sanskrit
long-vowel apostrophe that returns None rather than guessing.
Remaining from item A: ACIP on the page display (cosmetic — the data
is correctly keyed regardless). Original ruling: Verified: the spine's `acip` column is the source of
record; `acip_to_ewts(acip)` == stored `wylie` on 8,961/8,961 segments of
C01–C18, so nothing shipped is wrong — it is keyed in the wrong
transliteration. Fix is additive (emit both forms; ACIP as join key). A
full **Fable re-audit of C01–C18** is also mandated (item B). Every batch
entry from here records its producing model; everything through C03 batch
32 was Fable.

## Progress
c3p1–c3p53 (1–159) done; 160+ open; 461 segs remain.

## Queues (wylie / english / PHONETICS), C03
- WYLIE: ACIP anusvara capitals dM/kM/ND + ni'a oddity (seg 6).
- PHONETICS #1: sangs rgyas = "sangye" (4, 8, 11) vs "sanggye" (18)
  — spelling variation inside the phonetics layer; kept verbatim.
- PHONETICS #2: chos kyi = "chukyi" (82) vs "chu-kyi" (57) — hyphen.
- PHONETICS #3: kun 'byung = "kuynjung" (91) — unexpected y glide.
- PHONETICS #4: snying po = "nyingpo" (86) vs snying po'i =
  "nyinpoy" (108) — the -oy genitive is regular, the lost g is not.
- PHONETICS #5: the genitive is hyphenated "chang-gi" (116) against
  unhyphenated gungyi (55), lamgyi (115), tsokyi (8).
- ENGLISH: consecutive mid-sentence truncations at 119 ("described
  i") and 120 ("a good environment") — corpus artifacts, nulled with
  reasons, never completed. **120's loss is REPAIRED BY THE SOURCE
  at 121**: the Jewel of the Sutras verse lists all five qualities,
  so grogs bzang and yo byad ldan are attested in GMR's own English
  one segment later. Nothing supplied.
- ENGLISH #1: "commentray" ×2 (seg 22 syllabus, for "commentary").
- ENGLISH #2: seg 22 TRUNCATES mid-word ("Je Tsongkapa's Sour" —
  cuts "Source of All My Good") — corpus truncation artifact.
- ENGLISH #9: "your are quick to regret them" (125) — "your" for
  "you". Kept verbatim; page carries a dedicated check confirming the
  string survives byte-exact.
- ENGLISH #10: "ones" for "one's" — "position ones body" (134, 135)
  and "Ones eyes" (137, capitalised). THREE occurrences make it a
  habit rather than a slip. Kept verbatim; page checks confirm.
- ENGLISH #11: "cross- legged" (144) — line-break hyphen artifact,
  same family as "even- mindedness" and "self- nature".

### Batch C03-1 (c3p1, C03:1–3) — 2026-08-28
- PHONETICS PROTOCOL debut: right label = GESHE MICHAEL'S PHONETICS
  (GMR CONVENTION, VERBATIM); all links at Wilson depth 7. VERIFIED
  BY CONSTRUCTION: evidence layer unchanged (3,845/7,167) while the
  full bank grew +17 links — phonetic pairs cannot enter the
  successor dictionary.
- 1: maNDla (ACIP Sanskrit capitals in the wylie) ↔ "mandel".
- 2: spos kyis ↔ "pukyi" (both final -s silent); byugs ↔ "juk";
  bkram ↔ "tram".
- 3: nyi zlas ↔ "nyinde" (zla spoken "da", nasal migrates to nyi);
  brgyan ↔ "gyen"; bzhi ↔ "shi".
- BUILDER FIX (this batch): the C03 course registration was first
  spliced into the wrong dict (main()'s full literal — syntactically
  valid, semantically dead); caught because the full bank failed to
  grow, refixed into COURSES properly. Lesson: verify registration
  by the bank delta, not by ast.parse alone.

### Batch C03-2 (c3p2, C03:4–6) — 2026-08-28
- 4: dbul bar ↔ "ulwar" (d- silent, b→w); dmigs ↔ "mik"; sangs
  rgyas ↔ "sangye".
- 5: spyod par ↔ "chupar" (spy-→ch); 'gro ↔ "dro"; zhing in lines
  3+4 (shingdu/shingla).
- 6: FIRST BLENDED SEGMENT — mantra d=7 to BOTH phonetic
  occurrences; the English stanza is an UNPAIRED clause (s6trans):
  its Tibetan source is segs 2–5 (cross-segment consolidation), a
  new page-topology precedent for the liturgical section. Right
  label variant: GESHE MICHAEL'S PHONETICS + ENGLISH (VERBATIM).
  WYLIE QUEUE: ACIP anusvara capitals dM/kM/ND; ni'a ta ya mi ↔
  niryatayami ('a-chung, linked as unit); ra maND la kM ↔ ratna
  mandalakam (unit). Verse capitalization flattened, kept verbatim.
- Evidence layer still 3,845/7,167 (phonetics excluded by
  construction); full bank +20 links.

### Batch C03-3 (c3p3, C03:7–9) — 2026-08-28
- 7: title skyabs 'gro sems bskyed ↔ "kyabdro semkye" (bskyed →
  "kye": bsk- → k, final -d drops).
- 8: chos → "chu" — GMR's ö-as-u convention (2nd attestation, cf.
  spos → "pu"); tshogs → "tso"; mchog rnams → "choknam"; la survives
  unchanged.
- 9: byang chub → "jangchub" (by- → j consistent); bdag ni →
  "dakni"; mchis → "chi".
- Evidence layer 3,845/7,167 unchanged; full bank +18.

### Batch C03-4 (c3p4, C03:10–12) — 2026-08-28
- 10: bdag gis ↔ "dakki" — case particle ASSIMILATES AND DOUBLES
  (first junction-doubling); bgyis pa'i ↔ "gyipay" (genitive
  diphthong -ay); bsod → "su" (ö-as-u #3); sbyin → "jin".
- 11: phan phyir ↔ "penchir" — a-umlaut-as-e debut + phy-→ch (joins
  spy-→ch); 'grub par ↔ "druppar" (junction p-doubling). English
  refuge stanza = UNPAIRED clause s11trans, source segs 8–11 —
  2nd cross-segment consolidation (seg-6 topology confirmed as the
  liturgical pattern).
- 12: bsngo ba ↔ "ngowa" (bsng-→ng; intervocalic b→w).
- Evidence layer 3,845/7,167 unchanged; full bank +17.

### Batch C03-5 (c3p5, C03:13–15) — 2026-08-28
- 13: dge ba ↔ "gewa", skye bo ↔ "kyewo" (intervocalic b→w both);
  'di yis ↔ "diyi".
- 14: FIRST HYPHENATED PHONETIC — tshogs rdzogs ↔ "tsok-dzok"
  (GMR hyphenates a closed-syllable junction; two d=7 units);
  rdz-→dz.
- 15: las byung ↔ "lejung" (a-umlaut-as-e #2); ba'i ↔ "way"
  (b→w + -ay). Lines 14/15 share "sunam yeshe" — the two
  collections as verse anaphora.
- Evidence layer 3,845/7,167 unchanged; full bank +19.

### Batch C03-6 (c3p6, C03:16–18) — 2026-08-28
- 16: 'thob par ↔ "topar" ('th-→t); sku gnyis ↔ "kunyi". English
  dedication stanza = UNPAIRED s16trans (source 13–16, 3rd
  consolidation — topology settled).
- 17: mchod pa ↔ "chupa" → THREE-WAY HOMOPHONE COLLISION: chos (8),
  spyod (5), mchod (17) all spoken "chu"; only the wylie
  disambiguates. Ö-as-u #4.
- 18: ston pa ↔ "tonpa"; bla med ↔ "lame". PHONETICS QUEUE #1:
  sanggye (18) vs sangye (4/8/11).
- Evidence layer 3,845/7,167 unchanged; full bank +15.

### Batch C03-7 (c3p7, C03:19–21) — 2026-08-28
- 19: skyob pa ↔ "kyoppa" — junction doubling #3 (dakki/druppar/
  kyoppa); dam chos ↔ "damchu".
- 20: dge 'dun ↔ "gendun" — 'a-chung NASAL BRIDGE (ge+dün→gen-dun,
  family of nyinde); ü-as-u.
- 21: dkon mchog gsum ↔ "konchok sum"; skyabs gnas ↔ "kyabne".
  English "A Buddhist Grace" stanza = UNPAIRED s21trans (source
  18–21, 4th consolidation). The stanza's "I offer this / Higher
  than any other" ×4 = English anaphora mirroring Tibetan bla med
  anaphora.
- Evidence layer 3,845/7,167 unchanged; full bank +19.

### Batch C03-8 (c3p8, C03:22–24) — 2026-08-28
- 22: C03 MARKER #1 — wylie = "the asian classics institute";
  English = the whole Course III syllabus (apparatus, unpaired
  s22app). ENGLISH QUEUE #1 "commentray" ×2; #2 mid-word truncation
  ("Sour"); double-hyphen folios.
- 23: FIRST C03 EVIDENCE PAIRS — lam rim → "the Steps of the Path";
  chen mo → "the Great Book" (crossed); las → "From" (fronted).
- 24: Tsongkapa's outline: log rtog dgag pa → "refute certain
  mistaken ideas"; thun → "meditation periods"; sbyor ba'i chos
  drug → "Six Preliminaries" — CROSS-COURSE CONSISTENCY with
  C01:485; dang po ×2 rendered by role ("the first" / "This starts
  with").
- Evidence layer FIRST C03 GROWTH: 3,845→3,855 headwords,
  7,167→7,181 pairs; phonetics still excluded.

### Batch C03-9 (c3p9, C03:25–27) — 2026-08-28
- 25: gser gling pa'i rnam thar → "the descriptions of how Serlingpa
  practiced during his life" (CROSS-COURSE: C01:468 Serlingpa
  epithet vs the original master here). e1 full reversal. "altar" +
  enumeration frames SUPPLIED. 1a/2a/3a wylie prefixes + English
  [1][2][3] = corpus list-markers, verbatim.
- 26: g-yo med par → "without using any dishonest means" — WYLIE
  QUEUE: g-y token family crosses courses (g-yon/g-yas → g-yo).
  e2 full reversal (w6-w5-w4).
- 27: nyan sa → "the Levels of Listeners"; "[by Master Asanga]" =
  TRANSLATOR'S BRACKET SUPPLY. phan tshun 'chag → '"press them upon
  each other'; khri'am khri'u → "a platform or smaller seat"; ngam
  ("or") rendered AS the parenthesis; spyod lam crosses clauses
  (t-c3→e4); rgyud dang 'dres → "sink your mind deep into".
- Evidence layer: 3,890 headwords / 7,225 pairs (+35/+44).

### Batch C03-10 (c3p10, C03:28–30) — 2026-08-28
- 28: zhing → "the garden" (3rd render: Paradise/fields/garden);
  rang sangs rgyas → "self-made Buddhas"; bka' sdod → "protectors
  of the Dharma"; Tibetan final clause FRONTED to e1; bsams te
  crosses t-c2→e2.
- 29: LONGEST FULL MIRROR of the campaign — e3 = w18→w13, SIX units
  reversed. yan lag bdun → "Seven Ingredients"; rkyen trio
  (mthun/'gal/lhan-cig-byed) → conducive/oppose/supporting; nyer
  len → "principal causes"; med na inverted → "You also need".
- 30: ka/ ↔ "[a]" letter-numeral pairing; two 3-unit mirrors; ji
  snyed su dag → the King of All Prayers incipit; NEW TOPOLOGY
  "TIBETAN-AHEAD": wylie's ten-directions explanation + Yeshe De
  commentary have no English in this segment (unpaired s30rest) —
  English arrives in following segments; segment ends mid-sentence
  "[The full verse reads:".
- Evidence layer: 3,917 headwords / 7,266 pairs (+27/+41).

### Batch C03-11 (c3p11, C03:31–33) — 2026-08-28
- THE CITATION RELAY named: segment N cites a verse and truncates
  ("[The full verse reads:"); segment N+1 carries the verse's
  English + the delayed explanation English for N's unpaired
  remainder. Two relays completed: 30→31, 32→33.
- 31: THE VERSE READ BACKWARD (lines 4-3-2-1, the C01 praise-verse
  rotation family). 'jig rten → "planets"; mi yi seng ge → "Lions
  of living kind"; dang bas → "and purely". s31exp1-3 unpaired
  (source = s30rest).
- 32: four-unit mirror (w9-w8-w7-w6) + three-unit mirror in one
  short segment; second truncation.
- 33: rotation 1-2-4-3; TWO bow-verbs (btud/phyag 'tshal) MERGE
  into one "I bow myself"; two rab merge into one "Perfectly";
  zhing 4th appearance ("all the worlds").
- Evidence layer: 3,935 headwords / 7,300 pairs — 7,300 crossed.

### Batch C03-12 (c3p12, C03:34–36) — 2026-08-28
- Relays #3 (34→35, mind) and #4 (36→37, speech) — the
  CITATION-FORMULA MIRROR confirmed systematic: [quote] zhes pa'i
  tshigs bcad gcig ↔ "a single verse, the one with the words
  [quote]" — four-unit mirror, instances 3 and 4.
- 35: WYLIE QUEUE: snyeed (double-e, for snyed); ",de ltar" missing
  comma-space. chos kyi dbyings (dharmadhatu) → "Every corner of
  space"; sras → "sons and daughters"; mos → "I see" fronted.
- 36: WYLIE QUEUE: bsngags ba (ba for pa; same segment later spells
  bsngags pa). s36rest holds the multiplied heads/tongues + Yeshe De
  #3 + "rgya mtsho ni mang po'i tshig" (ocean = many).
- Evidence layer: 3,949 headwords / 7,321 pairs.

### Batch C03-13 (c3p13, C03:37–39) — 2026-08-28
- 37: SEGMENT BOUNDARY SPLITS "tshig go" — the "go" particle of
  36's last sentence opens 37 (corpus artifact, nulled). Verse
  rotation 4-3-1-2; bde bar gshegs pa → "Who have Gone to Bliss";
  dbyangs kyi yan lag → "the seed of harmony". Delayed English for
  s36rest (heads/tongues, sea=multitude).
- 38: kha/ ↔ "[b]" — letter-numeral SYSTEM confirmed (ka/kha).
  bla na yod pa'i mchod pa → "the offering which does have
  something higher" (vs bla na med pa to come). Citation mirror #5,
  with gnyis → "the two" (verses). s38rest = the offering glossary,
  English to come.
- 39: rotation 4-1-2-3 (same as C01 verse 10). WYLIE QUEUE: byung
  pa (typo for byug pa — 38's glossary spells it right);
  'phreng/phreng 'a-chung variation across adjacent segments; dang
  chain → commas.
- Evidence layer: 3,967 headwords / 7,350 pairs.

### Batch C03-14 (c3p14, C03:40–42) — 2026-08-28
- 40: rotation 4-1-2-3 consistent with 39 (offering verses share
  it). ri rab → "Meru" HERE vs "The Great Mountain" (seg 6) —
  Tibetan-stable/English-varies within one course. Glossary =
  delayed English for s38rest. ENGLISH QUEUE #3: "imbues" (for
  imbued); #4: truncation "spread itself or sprinkle". stong gsum
  → "a thousand thousand planets".
- 41: bla na med pa completes the yod/med technical pair; citation
  mirror #6; s41tail = bkod pa gloss whose English fell in 40's
  truncated stretch.
- 42: mos → "I see" (2nd, consistent with 35); L3 three-unit
  mirror; relay #6 closes with the emanated-offerings explanation +
  the last-two-feet instruction (tshig rkang pa tha ma gnyis).
- Evidence layer: 3,977 headwords / 7,371 pairs.

### Batch C03-15 (c3p15, C03:43–45) — 2026-08-28
- 43: ga/ ↔ "[c]" (letter-numeral #3); citation mirror #7; 'dod
  chags cited alone, English citation expands to "like and dislike".
- 44: FULL-REVERSE rotation 4-3-2-1 (course's 2nd, with 31). TWO
  bdag gis both → "I" at crossed positions. lus → "In action"
  (body by function); gti mug → "dark ignorance"; so sor → "one by
  one". Relay #7 closes (do/bid/rejoice triad; 'phro 'chad do →
  "prevents any future wrong from starting at all").
- 45: nga/ ↔ "[d]" (system 4 deep); FIVE-unit citation mirror
  (quote splits crosswise). s45rest holds the pauper-treasure
  simile (dbul pos gter rnyed pa ltar).
- Evidence layer: 3,984 headwords / 7,391 pairs.

### Batch C03-16 (c3p16, C03:46–48) — 2026-08-28
- 46: relay #8 closes. slob/mi slob → "paths of learning/done
  learning"; the pauper simile lands as "like a beggar who has
  stumbled across a mine of gold" (gter → gold mine). ENGLISH QUEUE
  #5: STRAY DUPLICATE "[d]" mid-segment.
- 47: ca/ ↔ "[e]" (letter-numeral #5); the ingredient TITLE itself
  is a four-unit mirror; crosswise citation quote again.
- 48: GMR'S FIRST-PERSON BRACKET — "[…and I have followed him.]" —
  the translator adjudicating root-text readings via Yeshe De.
  sgron ma → "Lamps"; 'jig rten → "the worlds" (3rd render); mgon
  po → "Saviors"; bskul crosses L4→L3. WYLIE QUEUE: "dag,byang"
  missing comma-space.
- MILESTONE: 4,000 HEADWORDS CROSSED — 4,000 / 7,421 pairs.

### Batch C03-17 (c3p17, C03:49–51) — 2026-08-28
- 49: cha/ ↔ "[f]"; mya ngan 'da' ston → "pretend to pass into
  nirvana" (ston "show" → "pretend" — the docetic display).
- 50: rotation 4-1-2-3; opening three-unit mirror; TWO "Stay"s in
  English vs ONE bzhugs (first = supplied anticipation, unwrapped);
  "at my breast" SUPPLIED; press/palms crossed; w10-w9-w8 mirror
  tail.
- 51: ja/ ↔ "[g]" — LETTER-NUMERAL SEQUENCE COMPLETE (ka..ja =
  a..g, one per ingredient). Citation keeps the trailing particle
  ('"prostrations and..."'). s51rest = the bsag/sbyang/spel/mi-zad
  taxonomy of all seven ingredients; English ahead.
- Evidence layer: 4,012 headwords / 7,441 pairs.

### Batch C03-18 (c3p18, C03:52–54) — 2026-08-28
- 52: relay #11 closes, rotation 4-3-1-2; two bdag gis → two crossed
  "I"s (44 pattern). Taxonomy lands (gather/clean/multiply/
  inexhaustible). GMR first person: "as I have taught you to here".
  ENGLISH QUEUE #6: truncates mid-word "disappea".
- 53: ENGLISH QUEUE #7 "Them make" (for Then); #8 "self- nature".
  WYLIE QUEUE: mNaDl vs maNDla (seg 1) — two spellings of the
  mandala abbreviation. byin gyis brlab → "bless" crosses clauses;
  bshes gnyen → "Spiritual Guide" (C01: "spiritual teacher");
  phyi/nang swap; embedded section header (s53title) = next
  segment's Tibetan.
- 54: PHONETICS resumes: rnal 'byor → "nelnjor" (nasal bridge #3);
  lha → "hla"; the Ganden Lhagyama opens (C01 fn 194 quoted its
  final verses).
- Evidence layer: 4,039 headwords / 7,481 pairs.

### Batch C03-19 (c3p19, C03:55–57) — 2026-08-28
- 55: thugs ka ↔ "tukka" (junction doubling #4); mgon ↔ "gun"
  (ö-as-u #5); nas ↔ "ne" (a-umlaut-as-e); hla-gyay (hyphen #2).
- 56: chu 'dzin ↔ "chunzin" — NASAL BRIDGE #4 (nyinde/gendun/
  nelnjor/chunzin); zho gsar ("fresh curd") ↔ "shosar".
- 57: Tsongkapa's name line: blo bzang grags ↔ "losang drak"
  (bz-→s, gr-→dr); chu-kyi and kun-kyen hyphenated — three hyphens
  on one page; the hyphenation practice concentrates in this
  prayer.
- Evidence layer unchanged (pure phonetics batch): 4,039 / 7,481.

### Batch C03-20 (c3p20, C03:58–60) — 2026-08-28
- 58: STANZA-LEVEL REVERSAL — the English stanza renders lines
  57-58-56-55 (the 4-3-2-1 rotation on whole lines of a
  consolidated stanza). IMAGE SWAP: zho gsar ("fresh curd") →
  "mountains of pure white snow" — poetic substitution, flagged.
  nendir = nasal bridge #5; sras → "se", bcas → "che".
- 59: penday = nasal bridge #6; khri → "tri"; dun-gyi (hyphen #4).
- 60: can → "chen" (palatalized + umlaut in one syllable);
  dzumkar — the white smile.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-21 (c3p21, C03:61–63) — 2026-08-28
- 61: depay (umlaut + -ay); sunam consistent; shingchok.
- 62: NUMBER SHIFT: bskal brgyar ("a hundred eons") → "For a
  million years" (C01:474 family); "backs of lions" SUPPLIED from
  iconography; stanza rotates 59-60-62-61; gyechir (phy-→ch holds).
- 63: TWO ö-as-u in one line — khyon → "kyun" (#6), blo gros →
  "lodru" (#7); jelway.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-22 (c3p22, C03:64–66) — 2026-08-28
- 64: kelsang/lekshe (umlauts); gyengyur; naway.
- 65: lham mer → "hlammer" — lh→hl + CROSS-COURSE token (C01:472
  queued "lham"); pelgyi (case -s drops).
- 66: umlaut cascade (tongtu #8, dunden #9, drenpe, chaktsel).
  FIRST IN-ORDER STANZA (63-64-65-66, the thugs/gsung/sku triple).
  lham mer → TRIPLE anaphora ("Shines forth/with brightness/with a
  glory"); mthong thos dran pas → "Just to see/hear/think of You".
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-23 (c3p23, C03:67–69) — 2026-08-28
- 67: yid 'ong → "yi-ong" — 'a-chung juncture HYPHENATED (vs the
  nasal-bridge treatment): both strategies now attested. mchod yon
  → "chuyun" (chu member + ö-as-u #10).
- 68: bdug spos → "dukpu" (spos→pu, c3p1 consistency); nangsel.
- 69: NEW CLUSTER RULE spr-→tr ×2 (yitrul, chutrin); dngos → "ngu"
  (ö-as-u #11); the dngos-bshams/yid-sprul pair (real vs emanated)
  from 41's prose, now in verse.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-24 (c3p24, C03:70–72) — 2026-08-28
- 70: stanza (source 67–70) rotates (69 leads); zhing mchog
  paraphrased "There is no higher With whom I may make merit";
  kyela (khy-→ky).
- 71: dus nas → "dune" — s-drop + umlaut FUSION into one word.
- 72: lus ngag → "lu-ngak" — HYPHEN AS DISAMBIGUATION (prevents
  "lungak" misread); ci → "chi" (c-palatalization).
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-25 (c3p25, C03:73–75) — 2026-08-28
- 73: kyepar (khy-→ky + umlaut); chok (phy-→ch holds).
- 74: ö-as-u reaches the CASE PARTICLE (drag pos → "drakpu" #13;
  gyupa #12). Stanza rotates 74-72-71-73. so sor → "One by one" —
  CROSS-TEXT consistency with seg 44 (Great Book verse vs Lhagyama).
- 75: dundir (nasal bridge #7); mangtu (thos→tu, 66 consistency);
  tsun (#14). Rejoicing verse opens.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-26 (c3p26, C03:76–78) — 2026-08-28
- 76: delnjor (bridge on dal 'byor itself); dunyu (DOUBLE ö-as-u
  #15/#16); pangpe.
- 77: kyukyi (the pronoun takes ö-as-u #17); gunpo consistent.
- 78: 2nd in-order stanza (75-78). chos brgyad → "The eight worldly
  thoughts" (C01 fn 172 exact); dal 'byor → "Your leisure and
  fortune" — ENGLISH RENDER #5, the first LITERAL one; bsam pa thag
  pas → "happy deep inside" (thag pa depth idiom).
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-27 (c3p27, C03:79–81) — 2026-08-28
- 79: THE PRONOUN PAIR DISTINGUISHED — khyod → "kyu" (77) vs khyed
  → "kye" (79): the umlaut system encodes the vowel contrast.
- 80: NEW DIPHTHONGS — sku'i → "kuy" (-u'i→uy), brtse'i → "tsey"
  (-e'i→ey); 'khrigs → "trik" ('khr-→tr); chukuy extends the chu
  collision to the Dharma-body.
- 81: duljay; jitar (lt-→t); dzinma (the earth-holder).
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-28 (c3p28, C03:82–84) — 2026-08-28
- 82: dbab → "ap" (db- VOCALIZES, dbul family). PHONETICS QUEUE #2:
  chukyi (82) vs chu-kyi (57) — hyphen inconsistency, same word.
  Third in-order stanza (79-82); ji ltar 'tshams pa'i → "As we are
  ready"; zab rgyas → "Deep and wide".
- 83: dakki consistent; ji-nye (disambiguation hyphen); 'dis → "di"
  — case -s erodes in speech, phonetics identical to 'di.
- 84: gangpen (phan→pen consistent); tendang fusion.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-29 (c3p29, C03:85–87) — 2026-08-28
- 85: kyepar/jetsun/losang drakpa — all consistent.
- 86: 4th in-order stanza (83-86). bstan pa → "the Word" (Kadampa
  calque echo); gsal byed → "carry forth The Light"; ring du →
  "for many years to come".
- 87: dpal ldan → "pelnden" — NEW JUNCTION PHENOMENON: ld- onset
  surfaces as nd after liquid coda (cousin of the nasal bridges).
  The root-lama verse opens.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-30 (c3p30, C03:88–90) — 2026-08-28
- 88: bdag gi AND bdag gis both → "dakki" — case erosion doubly
  attested; chiwor.
- 89: chenpoy — THE DIPHTHONG SYSTEM COMPLETES (ay/uy/ey/oy);
  gone (sg-→g + umlaut); jesung.
- 90: tukkyi/ngudrup consistent. Stanza finds: rjes bzung → "Take
  me after You"; dpal ldan → "Shining and precious"; dngos grub
  (siddhi) → "Grant that I attain".
- Evidence layer unchanged: 4,039 / 7,481. 30 pages / 90 segments
  of C03 done.

### Batch C03-31 (c3p31, C03:91–93) — 2026-08-28
- 91: THIRD LITURGICAL TOPOLOGY — line-by-line English drip (own
  couplet per segment). rdo rje 'chang → "the One who Holds the
  Diamond" (C01 crown calque cross-course). PHONETICS QUEUE #3:
  kuynjung (unexpected y; expected kunjung). tubwang (db-→w).
- 92: THE MIGTSEMA NAMESAKE (dmigs med brtse ba → mikme tseway);
  spyan ras gzigs → chenre sik.
- 93: 'jam dpal dbyangs → jampel yang — DOUBLE TREATMENT of db-:
  dbang→wang (w) but dbyangs→yang (y glide wins); dri med → drime
  (C01 3-render word, now its sound).
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-32 (c3p32, C03:94–96) — 2026-08-28
- 94: gsang ba'i bdag (Vajrapani); dupung fusion; jomdze.
- 95: gangchen; tsuk-gyen (hyphen); kepay.
- 96: THE MIGTSEMA STANZA (92–96, in order, sixfold "I pray").
  CROWN-CALQUE HARVEST: spyan ras gzigs → "Loving Eyes"; 'jam dpal
  dbyangs → "Soft Glorious Voice" (full 3-syllable calque; C01's
  "Gentle Voice" + dpal); gsang ba'i bdag → "the Keeper of the
  Secret"; gangs can → "the Land of Snow"; gtsug rgyan → "Crowning
  jewel"; dmigs med → "With no one it loves". Final line's triple
  frame kept verbatim.
- Evidence layer unchanged: 4,039 / 7,481.

### Batch C03-33 (c3p33, C03:97–99) — 2026-08-28 — model: Opus
- FIRST BATCH UNDER THE ACIP KEYING and the first to record its model.
- 97: verbatim repeat of 87. ACIP source reads RTZA BA'I → derived
  wylie rtsa ba'i — the TZ/ts distinction live on the page; the
  headword now keys as RTZA BA.
- 98: THE ONE CHANGED WORD — 88 had spyi bor ("at the crown of my
  head"), 97–99 has snying khar ("at my heart"). The second
  recitation moves the visualization from crown to heart; everything
  else repeats verbatim. nying-kar (hyphen, sny-→ny) vs 88's chiwor.
- 99: verbatim repeat of 89; phonetics identical (kadrin/chenpoy/
  gone/jesung) — the convention reproduces exactly across recitations,
  which is evidence it is systematic rather than ad hoc.
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-34 (c3p34, C03:100–102) — 2026-08-28 — model: Opus
- 100: THE CHANGE TRACKS IN BOTH LANGUAGES. The stanza (source
  97–100) is word-for-word identical to segment 90's except for one
  phrase: 90 "Atop my head", 100 "In my heart". One word changed in
  the Tibetan (spyi bor → snying khar), one phrase changed in the
  English, nothing else moved. Clean evidence of GMR's translation
  consistency at stanza scale — the same source rendered the same
  way twice with the single difference faithfully carried. Highest
  grade of successor-dictionary evidence.
- 101: third recitation of the opening line (87/97/101), phonetics
  identical each time. ACIP RTZA BA'I → wylie rtsa ba'i.
- 102: snying khar again — the verse cycles crown (88) → heart (98)
  → heart (102).
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-35 (c3p35, C03:103–105) — 2026-08-28 — model: Opus
- 104: THE VARIATION MOVES FROM THE PLACE TO THE REQUEST. The stanza
  (source 101–104) again matches 90's and 100's word for word,
  keeps "In my heart" from 100, and changes only the final request:
  90/100 asked "Your body, speech and mind" (sku gsung thugs), 104
  asks "Both common and highest goals" (mchog dang thun mong). The
  liturgy varies ONE slot per recitation — first the place, then the
  boon — and the English varies with it and nowhere else. Together
  with batch 34 this is a three-point demonstration of GMR's
  stanza-scale consistency.
- ORDER REVERSED: Tibetan mchog ("highest") dang thun mong
  ("common") → English "Both common and highest".
- dngos grub (siddhi) → "goals" here vs "Grant that I attain" at 90
  — second render.
- CROSS-COURSE: thun mong = "common" here, but "as a whole" at
  C01:486 (thun mong du sbyor tshul) — two registers of one word.
- 103/105: verbatim repeats (89/99/103; 87/97/101/105), phonetics
  identical each time.
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-36 (c3p36, C03:106–108) — 2026-08-28 — model: Opus
- 108: THE FOURTH-LINE SLOT, THIRD FILLING. Across four recitations
  the closing line runs: sku gsung thugs kyi dngos grub rtsol du
  gsol (90, 100) → "Grant that I attain Your body, speech and mind";
  mchog dang thun mong … (104) → "… Both common and highest goals";
  byang chub snying po'i bar du brtan par bzhugs (108) → "Stay here
  never moving Until the Buddha's essence". This filling changes the
  VERB as well as the object — rtsol du gsol (grant) becomes brtan
  par bzhugs (stay) — and the English turns with it while the eight
  lines above stay identical. Four-point consistency demonstration.
- snying po → "essence", consistent with 86 (bstan pa'i snying po →
  "The Essence of the Word"). brtan par → "never moving".
- PHONETICS QUEUE #4 opened: nyingpo (86) vs nyinpoy (108).
- 106/107: verbatim repeats (98/102/106; 89/99/103/107).
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-37 (c3p37, C03:109–111) — 2026-08-28 — model: Opus
- THE ERGATIVE IS AUDIBLE — the batch's principal finding. The
  agentive/ergative -s does not vanish in GMR's phonetics; it
  fronts the preceding vowel:
    tsong kha pa  → "tsongkapa" (95, caseless)
    tsong kha pas → "tsongkape" (109, ergative)
    rgyal ba      → "gyalwa"    (109, caseless)
    rgyal bas     → "gyalwe"    (111, ergative)
  Two matched minimal pairs, both on the same page. This is
  GRAMMAR carried by the sound system, not merely sound — and it
  bears directly on the pronunciation engine's treatment of case.
  Note the asymmetry: a stem-final -s drops (tshe rabs → "tserab")
  while the case -s survives as the umlaut.
- 110: bshes gnyen → "she-nyen" (disambiguation hyphen; the same
  word rendered "a spiritual teacher" at C01:485 and "your
  Spiritual Guide" at C03:53). theg mchog → "tekchok"; dngos su →
  "ngusu" (69/91 consistent); mthus → "tu".
- 111: bsngags pa'i → "ngakpay" (bsng-→ng, cf. ngowa at 12); lam
  bzang → "lamsang" (bz-→s, losang family); de nyid → "de-nyi";
  las → "le" (15 consistent).
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-38 (c3p38, C03:112–114) — 2026-08-28 — model: Opus
- 112: theg mchog → "the Greater Way", and C01:486 rendered theg
  chen "the greater way" too — TWO Tibetan compounds (mchog
  "supreme" / chen "great") collapsing to ONE English phrase:
  Tibetan-varies/English-stabilizes, now attested across courses.
  bshes gnyen → "my spiritual guide", holding against C03:53 "your
  Spiritual Guide" and C01:485 "a spiritual teacher" — one word,
  one concept, three determiners. dngos su mdzad pa → "serve
  himself as".
- 113: skye ba → "kyewa" vs segment 13's skye bo → "kyewo" — the
  ba/bo pair stays distinct in the phonetics.
- 114: 'bral med → "drelme"; longs spyod → "longchu". PHONETICS
  QUEUE #2 EXTENDED: chos kyi is "chu-kyi" here and at 57, plain
  "chukyi" at 82 — two hyphenated against one plain.
- SIDE INVESTIGATION (not a batch deliverable): probed
  pron_engine.py against the C03 liturgical phonetics after batch
  37's ergative finding. The engine DOES reproduce the ergative
  umlaut correctly. Seven other recurring rule classes differ.
  Written up honestly — including two discarded, unsound
  measurements — in docs/PRON_ENGINE_VS_C03_LITURGY.md. NO ENGINE
  CHANGE MADE; needs Adam's ruling on whether prayer transcription
  and card phonetics are meant to share a convention.
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-39 (c3p39, C03:115–117) — 2026-08-28 — model: Opus
- FIRST BATCH AFTER THE BACKFILL. C01 and C02 are closed and locked;
  C03 is now the only course carrying a coverage gap.
- 115: yon tan ↔ "yunten"; sa dang lam, the levels-and-paths pair.
- 116: THE GANDEN LHAGYAMA ENDS. rdo rje 'chang → "the One who Holds
  the Diamond", the same rendering as C03:91 — the crown calque holds
  across both occurrences in the prayer. go 'phang → "the place where
  I Become myself"; myur thob → "reach then quickly".
  PHONETICS QUEUE #5 opened: chang-gi hyphenated against gungyi /
  lamgyi / tsokyi unhyphenated.
  The segment carries three things: the phonetic line, the closing
  stanza (s116trans, source 113–116), and the Reading Three header
  plus apparatus (s116app, no Tibetan source).
- 117: MARKER, unusual in that its wylie is REAL TIBETAN (zhi gnas
  kyi tshogs) rather than an English title fragment. zhi gnas
  (shamatha) → "Quietude", consistent with 195's "Achieving quietude
  [shamata]".
  SENSE SPLIT BANKED: tshogs → "Conditions" here against
  "collections" (merit and wisdom) at segments 14 and 15. One common
  word, two genuine senses, both GMR's — a review-pass item and a
  good example of why the scan is an attestation layer.
- Evidence layer: 4,039 headwords / 7,481 pairs; ACIP 4,038/4,039.

### Batch C03-40 (c3p40, C03:118–120) — 2026-08-28 — model: Opus
- 118: THE CITATION FORMULA REPEATS. Segment 23 gave "…by Je
  Tsongkapa:"; 118 drops the attribution. Same source, attribution
  stated once then assumed. chen mo/lam rim crossed as at 23.
- 119: lhag mthong → "special sight" HERE vs "(vipashyana, or
  insight)" at 287 — two English renders of zhi gnas's pair-mate,
  banked. zhi gnas holds as "quietude"; tshogs holds as
  "conditions", confirming last batch's sense split in running prose
  rather than on a header alone.
- 120: each quality introduced as a quoted term (rnyed sla ba →
  '"easy to find'; gnas bzang ba → 'a "good place'; sa bzang ba →
  'a "good environment'). Comma-inside-quote trap throughout.
- TWO CONSECUTIVE TRUNCATIONS (119, 120): Tibetan tails with no
  English. Nulled with reasons, never completed.

#### PROCESS FIX THIS BATCH
The generator caught a real error — two English spans in the wrong
cursor order at s119 — and refused to write the page. Correct
behaviour. But the batch script had already run its sed-register, so
the builder was left pointing at a page that was never written and
died on a FileNotFoundError traceback.

Added a MISSING-PAGE GUARD to build_alignment_layer.py: a page
registered in COURSES whose file does not exist now REFUSES the build
with a plain message naming the course, the page and the path, and
saying what to do about it. Liveness-proved by registering a
nonexistent page.

Standing correction to the batch order: register AFTER the page is
written, not before. Until every batch follows that, the guard is
what turns a half-finished run into a legible error instead of a
stack trace.
- Evidence layer: 4,057 headwords / 7,503 pairs.

### Batch C03-41 (c3p41, C03:121–123) — 2026-08-28 — model: Opus
- First batch under the corrected order: the page is written and its
  existence CHECKED before the registration runs.
- 121: THE TRUNCATION IS REPAIRED BY THE SOURCE ITSELF. 120's English
  stopped at 'a "good environment'; this verse citation lists all
  five qualities, so the two lost ones (grogs bzang → "A place where
  good friends stay"; yo byad ldan → "With all the practitioner
  needs, with ease") arrive attested rather than supplied.
  mdo sde'i rgyan → "the Jewel of the Sutras" (rgyan "ornament" →
  "Jewel"); blo ldan → "intelligent people"; zhes gsungs so → "says",
  fronted to the citation frame.
- 122: 'dod pa chung ba → "you keep your wants few"; chos gos →
  "robes"; the 'am particle survives as English "or" rather than
  being absorbed — rare.
- 123: chog shes pa appears TWICE in the Tibetan and twice in the
  English — "you are easily satisfied", then "you always feel like
  you have enough". One compound, two renderings inside a single
  segment, both GMR's: the term stated then unpacked. The clearest
  kind of attestation evidence, and exactly what a definition list
  would have to collapse into one gloss.
- Evidence layer: 4,070 headwords / 7,524 pairs.

### Batch C03-42 (c3p42, C03:124–126) — 2026-08-28 — model: Opus
- 124: nyo tshong ("buying and selling") → "business"; skar ma brtsi
  ba ("star-reckoning") → "astrology"; khyim pa / rab byung →
  "laymen" / "monks".
- 125: ENGLISH QUEUE #9 — "your are quick to regret them". Kept
  verbatim, with a page-level check proving the string survives.
  so thar (pratimoksha) → "vows of individual freedom" — GMR
  calques this one but KEEPS the Sanskrit in "bodhisattva vows"
  alongside it, in the same sentence. Two policies, one clause.
  rang bzhin dang bcas pa / kha na ma tho ba → "naturally wrong to
  do" / "those that the Buddha has prohibited" — the
  natural-vs-proscribed distinction rendered by explanation.
- 126: rnam rtog → "THE STUPID THOUGHTS" — a strong, unusual render
  for a word normally given as conceptual/discursive thought.
  Exactly the contextual force a definition list would lose.
  nyes dmigs → "the problems", holding against C01:488 — cross-course.
  yid du 'ong → "pretty things". Truncates at "how quickly it
  disappe"; Tibetan tail nulled with its reason.
- Evidence layer: 4,095 headwords / 7,558 pairs.

### Batch C03-43 (c3p43, C03:127–129) — 2026-08-28 — model: Opus
- 127: sgom rim bar pa → "the Stages of Meditation"; nyan thos kyi sa
  → "the Levels of Listeners", holding exactly against C03:27 a
  hundred segments earlier. Truncates at "you realiz".
- 128: tshogs → "the gathering" — A THIRD ENGLISH SENSE in one
  course: "collections" (14–15), "conditions" (117, 119), "gathering"
  (128, twice). All three GMR's, all three right in place, none able
  to serve alone as the definition. Directly illustrates Adam's
  preserve-every-rendering ruling.
- 129: THREE INDIAN TITLES, THREE CALQUES, NO SANSKRIT — "the Jewel
  of the Sutras" (121), "the Levels of Listeners" (27, 127), "the
  Commentary of the True Thought" (129). A consistent naming policy.
  Verse inversion: Tibetan puts myur du 'grub bo LAST, English puts
  "If you want to reach quietude quickly" FIRST.
- GENERATOR CAUGHT a bad split marker (",zhes gsungs" — the wylie has
  ", , zhes gsungs so"). The corrected batch order held: the page was
  never written, so the `test -f` chain short-circuited and NO
  registration ran. No broken state this time — the process fix from
  batch 40 proved itself.
- RETRIEVAL FINDING banked in docs/PRESERVE_ALL_RENDERINGS.md: 86.2%
  of headword keys are multi-word spans, so renderings scatter.
  chos holds 36 under its bare key but appears in 51 other keys
  holding 115 more — 151 total, of which a bare lookup surfaces 36.
  Data complete; retrieval path missing.
- Evidence layer: 4,125 headwords / 7,599 pairs.

### Batch C03-44 (c3p44, C03:130–132) — 2026-08-28 — model: Opus
- 130: lam sgron → "A Lamp for the Path" (Atisha) — a FOURTH Indian
  title calqued, never transliterated, with the Jewel of the Sutras,
  the Levels of Listeners and the Commentary of the True Thought.
  The naming policy is now well attested across four titles.
  Inside the verse zhi gnas yan lag → "the parts of QUIET", not
  "quietude". ting 'dzin (contracted) → "this deep concentration".
- 131: **GMR PRINTS BOTH RENDERINGS HIMSELF**: 'quietude ("quiet")'.
  Having used "quiet" in the verse he just quoted and "quietude" in
  his own prose, he reconciles them on the spot by printing one and
  parenthesising the other. The author doing explicitly what this
  layer does structurally — holding two renderings together rather
  than choosing between them. The single best piece of evidence yet
  for Adam's preserve-every-rendering ruling: it is GMR's own
  practice, not an artifact of machine matching.
  yan lag gam tshogs → 'the various "parts" or conditions' — tshogs
  in its "conditions" sense a third time. zhi lhag (contraction of
  zhi gnas + lhag mthong) → "quietude and special sight", named by
  abbreviation in Tibetan, spelled out in English. nyan sa
  (contracted) → "the Levels of Listeners", third holding.
- 132: sbyor ba / dngos gzhi → "the preparation" / "the actual
  meditation". de la brten nas → "once one has assembled all the
  proper circumstances" — the pronoun unpacked into its referent.
- Evidence layer: 4,138 headwords / 7,621 pairs.

### Batch C03-45 (c3p45, C03:133–135) — 2026-08-28 — model: Opus
- 133: sbyor ba'i chos drug → "the six preliminary PRACTICES" — a
  SECOND rendering against "the six preliminaries" at C01:485 and
  C03:24. Same compound, two English forms, both GMR's, preserved
  side by side.
  thun mong ba'i → "that are shared" — A THIRD SENSE for thun mong,
  joining "common" (C03:104) and "as a whole" (C01:486).
  skyes bu chung 'bring → "people of lesser and medium capacities" —
  the scopes named by capacity rather than by "scope".
- 134: ENGLISH QUEUE #10 opened — "position ones body", "ones" for
  "one's", recurring at 135. Kept verbatim.
  REGISTER SHIFT noted: the translation moves to impersonal "one
  should" from the "you" of the preceding pages, tracking the
  Tibetan's own shift into prescriptive outline.
- 135: chos brgyad → "eight different features" HERE, against "the
  eight worldly thoughts" ('jig rten chos brgyad) at C03:76. Two
  wholly different senses of the same compound, separated only by
  context — worldly concerns there, posture features here. Both now
  attested with citations.
  stan → "meditation cushion"; the Bhavanakrama volumes numbered in
  English where the Tibetan names them by position.
- Evidence layer: 4,154 headwords / 7,644 pairs.

### Batch C03-46 (c3p46, C03:136–138) — 2026-08-28 — model: Opus
- 136: A NAMING-POLICY EXCEPTION. rnam par snang mdzad → "Vairochana",
  kept in SANSKRIT where C01 calqued every other holy name ('jam
  dbyangs → "Gentle Voice", byams pa → "Loving One", tshangs pa →
  "Pure One", thub pa → "the Able", rdo rje 'chang → "the One who
  Holds the Diamond"). So: Indian TEXT titles always calqued (four
  attested), Buddha names usually calqued — but not this one.
  rje btsun → "the holy being named", the phrase that introduces an
  untranslated name.
- 137: ENGLISH QUEUE #10 third occurrence ("Ones eyes"). gtad
  ("fixed, directed") → "loosely focussed" — the English softens a
  firm verb, with "around the plane of" supplied to match.
- 138: **THE REGISTER OSCILLATES INSIDE ONE LIST.** Items [1] and [2]
  are impersonal ("one should", "Ones eyes"); item [3] switches to
  "your body", "your spine", "your thoughts". Three consecutive
  numbered items, two grammatical persons, and the Tibetan gives no
  such signal — lus ni is exactly as impersonal as mig ni. This is
  the translator's own variation and precisely what a register
  warning should surface rather than smooth away.
  dran pa (mindfulness) → "your thoughts" — a notably plain render
  of a technical term.
- Evidence layer: 4,171 headwords / 7,663 pairs.

### Batch C03-47 (c3p47, C03:139–141) — 2026-08-28 — model: Opus
- 139: three-unit mirror (Tibetan w1-w2-w3 → English w1-w3-w2).
- 140: ORDER REVERSED ON A PAIRED NEGATION. Tibetan mi mtho ("not
  high") then mi dma' ("not low"); English "neither tilted DOWN nor
  UP". The pair is faithfully rendered and its members swapped —
  the kind of quiet inversion a word-order check would flag and a
  letter-exact check never sees. Worth remembering when the Fable
  re-audit designs its checks.
- 141: **THE REGISTER ALTERNATION HAS A RHYTHM.** Six posture items
  so far: [1] "one should", [2] "Ones eyes", [3] "your body",
  [4] "The shoulders", [5] "The head", [6] "your teeth and your
  lips" — impersonal, impersonal, PERSONAL, impersonal, impersonal,
  PERSONAL. Two and one, twice over. The Tibetan marks nothing of
  the kind: every item is a bare topic particle (rkang pa ni, mig
  ni, lus ni, phrag pa ni, mgo ni, so dang mchu ni).
  RECORDED AS AN OBSERVATION, NOT A RULE. Items [7] and [8] next
  batch will test whether the pattern holds or whether six items
  simply fell that way.
  **→ DISCONFIRMED at batch 48. See that entry. Item [7] mixes both
  persons in one sentence; there is no rhythm, only drift. This note
  stands as written, corrected below, per the correction-of-record
  protocol.**
  rang lugs su tha mal par → "in their normal, natural position" —
  two adverbs to two adjectives, crossed w4-w1-w3-w2.
- Evidence layer: 4,186 headwords / 7,678 pairs.

### Batch C03-48 (c3p48, C03:142–144) — 2026-08-28 — model: Opus
#### THE BATCH-47 RHYTHM IS DISCONFIRMED
Batch 47 saw a two-and-one register alternation across posture items
[1]–[6] and predicted [7] would be impersonal. It is NEITHER:

    [7] "The tongue should be kept close to the palate,
         behind YOUR upper teeth."

— an impersonal subject and a second-person possessive in one
sentence. So the register does not alternate on a pattern; it
DRIFTS, unmarked, sometimes within a single clause.

That is the more useful result. A rhythm would imply deliberate
design and something a translator could rely on. Drift means the
person GMR happens to use carries no information, and the register
surface should say exactly that rather than inviting inference. The
six-item pattern was one I read into thin data; it is recorded as
disconfirmed rather than quietly dropped.

Item [8] IS personal throughout, which the discarded rhythm also
predicted — but a prediction that fails on [7] and succeeds on [8]
has predicted nothing.

- 143: phar rgyu tshur rgyu → "when it is coming and going" — the
  phar/tshur DEIXIS pair from C01:377 in its plainest use, of
  breath. dal bu dal bus (reduplicated) → "very slowly", the
  doubling rendered as intensification.
- 144: ENGLISH QUEUE #11 "cross- legged". nyan sar → "In the Levels
  of Listeners", a FOURTH holding — and note it is the CONTRACTED
  form rendered identically to the full nyan thos kyi sa.
  khri'am khri'u → "a platform or seat" against C01:27's "a platform
  or smaller seat": the diminutive marked there, unmarked here.
- Evidence layer: 4,204 headwords / 7,700 pairs.

### Batch C03-49 (c3p49, C03:145–147) — 2026-08-28 — model: Opus
- 145: shin tu sbyangs pa → "practiced ease" (twice). CROSS-REF: the
  same term in its shin sbyang contraction is "Physical / Mental
  meditative pleasure" at C03:196–197. TWO English treatments of
  prasrabdhi in one course — "practiced ease" in prose, "meditative
  pleasure" in the chart headings. Both attested.
  myur ba myur bar (reduplicated) → "very quickly", the same
  intensifying doubling as dal bu dal bus at 143.
- 146: de'i lus ("that one's body") folds into plain "you" — the
  Tibetan's third-person impersonal becoming second person, the
  batch-48 drift running in the other direction.
- 147: **THE NEGATED FORM IS STABLE WHERE THE POSITIVE VARIES.**
  thun mong ma yin pa → "is unique" here; C01:494 gave the same
  negated compound "is truly unique". Two courses, one sense.
  But positive thun mong has three: "common" (C03:104), "as a whole"
  (C01:486), "that are shared" (C03:133). The negation is settled
  and the positive is not — a genuine asymmetry, and one a
  single-gloss entry could not express.
  mu stegs → "non-Buddhists"; phas kyi rgol ba ("opponents") →
  "those whom you may be debating", a technical term rendered as a
  situation.
- Evidence layer: 4,216 headwords / 7,716 pairs.

### Batch C03-50 (c3p50, C03:148–150) — 2026-08-28 — model: Opus
### FIFTIETH PAGE OF C03
- 148: dang ba (clear faith) → "deep feelings of faith" — the same
  word is "and purely" in the C01 prostration verse (C01:31, dang
  bas). An adverb of manner there, a noun phrase of emotion here.
  The Tibetan conditional (mthong na, "if others see") generalises
  to "Whenever".
- 149: **nyan thos TWO WAYS IN ONE COURSE.** "the disciples" in
  running prose here, but capital-L "Listeners" inside the title
  "the Levels of Listeners" (nyan thos kyi sa — four holdings). The
  title form is frozen, the prose form is not. A clean example of
  genre governing rendering rather than meaning.
  gsungs so → "states the text" — the supplied subject naming a
  SOURCE, where C01 usually supplied a person ("our Lama", "Lord
  Tsongkapa").
  ENGLISH QUEUE #11 "cross- legged" recurs (2nd, with 144).
- 150: rmugs pa dang gnyid → "mental fogginess and sleepiness";
  C03:27 gave the same pair REVERSED as "sleepiness and fogginess",
  with "mental" added here. A SECOND paired-term inversion, after
  segment 140's mi mtho / mi dma'. Two instances make this a
  recurring shape rather than a one-off — and one the letter-exact
  battery is structurally blind to. Worth a word-order check in the
  re-audit design.
- Evidence layer: 4,231 headwords / 7,735 pairs.

### Batch C03-51 (c3p51, C03:151–153) — 2026-08-28 — model: Opus
- 152: **A FOURTH TREATMENT OF zhi gnas.** The layer now holds FIVE
  distinct written forms for this one term, all GMR's:
    "quietude"                     (117, 119, 129, …)
    'quietude ("quiet")'           (131 — GMR reconciling his own two)
    "quietude (shamata)"           (152 — Sanskrit, round brackets)
    "quietude [shamata]"           (195 — square brackets)
    "quiet"                        (130, inside the verse compound)
  Exactly what the preserve-all ruling protects: a single-gloss entry
  would discard four of the five.
  theg pa gsum → "all three ways"; rnal 'byor pa → "any deep
  practitioner" (the yogi by seriousness, not by role).
- 153: **A NAMING INCONSISTENCY ACROSS COURSES.** byams pa's
  honorific here is "Lord MAITREYA" — Sanskrit — where C01 calqued
  the same being "the Loving One". With Vairochana (C03:136) that is
  TWO holy names kept in Sanskrit in C03 against calques in C01. The
  policy is NOT uniform across courses; the dictionary must record
  both without implying either is the correct form.
  dbus mtha' → "Distinguishing the Middle and the Extremes" — a
  FIFTH Indian title calqued. So: titles always calqued, holy names
  sometimes.
  The five obstacles are each rendered by EFFECT not by term: le lo
  → "not feeling like meditating"; gdams ngag brjed pa ("forgetting
  the instruction") → "losing the object"; bying rgod → "dullness
  and agitation"; 'du mi byed pa / 'du byed pa → "failing to take
  action" / "taking action when there is no need".
  Truncates at "The"; the four antidotes — including shin sbyangs in
  a THIRD context — have no English here.
- Evidence layer: 4,255 headwords / 7,764 pairs.

### Batch C03-52 (c3p52, C03:154–156) — 2026-08-28 — model: Opus
- 154: dran pa → "to bring the mind back to the object" — a THIRD
  rendering, after "your thoughts" (138) and beside 156's "bringing
  The mind back to your object". Three treatments of mindfulness in
  one course, from bare noun to full instruction.
  btang snyoms (equanimity) → "to leave well enough alone" — a
  technical term as an English idiom; strong review-pass candidate.
  shes bzhin → "watchfulness", holding against 169.
- 155: sems gnas dgu → "nine mental states" (200 consistent); stobs
  drug → "six different powers"; yid byed bzhi → "four modes of
  focus". gsungs pas fronted to "It is further stated", the same
  supplied-source move as 149's "states the text".
- 156: dbu ma snying po → "The Essence of the Middle Way" — a SIXTH
  calqued Indian title; snying po → "Essence" holds with 86 and 108.
  THE VERSE IS REORDERED: Tibetan closes with rim dbang bya ("master
  by stages"), English OPENS with "Master the elephant of the mind
  Step by step" — the instruction fronted so the image can follow.
  The three implements map cleanly with their genitives: ka ba →
  "the stake" (object), thag pa → "a strong rope" (mindfulness),
  lcags kyu → "the iron hook" (wisdom).
  Truncates at "and th".
- GENERATOR CAUGHT a clause-split error at s155 ("grouped into four
  modes" sits before the boundary, not after). No page written, no
  registration ran — the batch-40 process fix held for the second
  time under a real failure.
- Evidence layer: 4,278 headwords / 7,792 pairs.

### Batch C03-53 (c3p53, C03:157–159) — 2026-08-28 — model: Opus
- 157: A CLEAN NINE-TO-NINE LIST, in order, no crossings — rare here.
  The three 'jog pa states are distinguished by what happens to the
  object: rgyun du → "with brief continuity", glan de → "with
  patches where you lose the object", nye bar → "tightly". zhi bar /
  rnam par zhi bar → "quieting" / "completely quieting", the rnam
  par prefix carried by one adverb.
  States 8 and 9 match the chart headings at 192 and 194 word for
  word — prose and chart agree.
  WYLIE QUEUE: glan de 'jog pa (normally slan te 'jog pa).
- 158: dran pa reaches a FOURTH rendering. Across this course:
  "your thoughts" (138), "to bring the mind back to the object"
  (154), "bringing The mind back to your object" (156), "bringing
  your mind back to the object" (158). GMR treats the term as an
  instruction, never as a noun.
  WYLIE QUEUE: brgyad pa'i grub — stray genitive where the parallel
  clauses read pa 'grub.
- 159: A CLEAN FOUR-TO-FOUR MAPPING on the modes of focus, all four
  distinguished by relation to interruption.
  chad = "interrupted" here, against the annihilation extreme at
  C01:417 ("thinking things have stopped"). Two unrelated senses of
  one syllable — a concrete reason to key on spans, not syllables.
- Evidence layer: 4,304 headwords / 7,820 pairs.

### Batches C03-54 and C03-55 (c3p54, c3p55, C03:160–165) — 2026-08-28 — model: Opus
First batch produced through the committed generator
(`tools/gen_alignment_page.py`) rather than an inline heredoc, and through a
two-proposer workflow: each segment analysed independently Tibetan-first and
English-first, then reconciled with a standing instruction to prefer the
TIGHTER span wherever the two disagreed.

**The generator refused all six segments on the first attempt, and every
refusal was correct.** It exposed three real bugs in itself and two authoring
rules worth keeping.

Bugs in the tool, all now fixed and pinned:
- **It could not do crossings at all** — one forward-only cursor shared by
  both sides, so `lo stong` → "a thousand years" refused. The tool was
  unusable on real input and its six mutation-verified pins all used
  non-crossing spans.
- **No depth-7 nesting.** The member `gnas` of `sems gnas` matched the
  unrelated `zhi gnas` 130 characters later and the cursor rejected
  everything after it.
- **Two `sort()` calls compared span dicts** on a tie and raised TypeError —
  a crash, not a refusal, which is the worse failure because a crash after
  registration leaves the builder pointing at a page that was never written.

Authoring rules the refusals taught, now applied mechanically:
- **A morpheme bound inside the word above it is depth 7, not depth 6.**
  A bare `'i`, `pa`, `la`, `s` matches all over the wylie; as a flat span it
  attaches wherever it first hits. Twelve were re-depthed in this batch.
- **An English link to a single function word that occurs more than once is
  not a link.** Nobody can say which "to" or "of" it means. Fifteen were
  nulled rather than guessed at.

**And a defect in the BUILDER that only this batch could have exposed:**
`strip_tags` replaced each tag with a space, so a depth-7 member splitting
`dbu ma'i` banked the headword as `dbu ma 'i` — a Tibetan string that does
not exist. Every existing gate passed: they check the PAGE against the spine,
and the page was fine; the corruption happened downstream. It surfaced only
because the ACIP round-trip could not convert it, which was luck, not a gate.
Fixed, pinned (`tools/test_alignment_builder.py`), and the fix repaired one
pre-existing defect too — `sa dge rnying` had been banked as "the Sakya ,
Geluk , and Nyingma" with spurious spaces before the commas.

Content notes:
- **160**: `mthar` ("final result") is the seventh Tibetan token but opens
  the English, so the crossing runs backwards over five spans. `shin sbyangs`
  takes "meditative pleasure" rather than the usual "pliancy" — worth
  cross-checking against the other C03 occurrences before treating that as
  this course's settled rendering. `sems` in `sems gnas` is unrendered: GMR
  writes "these nine states", not "nine mental states".
- **162**: the sustained battle metaphor — `ral gri rnon po` "razor-sharp
  sword", `g-yul ngo` "field of battle", `rgyal thabs chen po` "the great
  victory". `zung du 'brel ba` → "married together", which is why the numeral
  gate misfired on `zung` elsewhere: it genuinely means "a pair".
- **163–165**: the closing verses. 164 and 165 are the colophon and the
  printing dedication — Trijang Trulku naming his patron, and the estate of
  the deceased Asong funding the woodblocks. Verse, so the spans are shorter
  and the line-crossing convention of the C03 verse pages applies.

- Evidence layer: 4,359 headwords / 7,910 pairs. C03 at 165/620.

### Batches C03-56/57/58 (C03:166–174) — 2026-08-28 — model: Opus
The printer's colophon plus the first captions of the elephant-path chart —
the diagram where the mind is an elephant led up a winding road by a monk.

**A defect class found here reaches back across the whole project.** The
generator's word-boundary rule refused `par shing` → "block", because "block"
also sits inside "blockprint" earlier in the same sentence. Sweeping all 248
pages for the same shape turned up 16 English spans cut out of the middle of
a word. Fourteen are a deliberate convention and are correct:

> **The negative-affix convention.** The Tibetan negation `mi`/`ma` owns the
> English negative, so the positive stem is spanned on its own: `n't` inside
> "doesn't", "necessary" inside "unnecessary", "changing" inside
> "unchanging", "not" inside "cannot". Used on 14 spans across C01.

**Two were real damage, and both had been banked into the layer:**
- `c3p43` (C03:129) banked `gong ma gong ma'i rgyu` as **"are caus"** — a
  truncation of "are causes". Not English. It had been in the layer since
  that batch shipped.
- `c3p54` (C03:161) banked `rtse gcig` as **"one-pointed"**, cut out of
  "one-pointedly".

Neither was catchable before. Every existing gate proves the page byte-exact
against the spine, and both pages *were* byte-exact — all the characters were
right and only the span boundary was wrong. Now gated by
`tools/test_no_broken_words.py`, which scans all 11,227 English spans on
every build, with the fourteen legitimate cases listed by name so that
allowing one is a deliberate act.

The generator gained a `"subword": true` opt-in for the same reason: refuse
the accident, allow the declared intent.

Content notes:
- **166** is the printer's colophon: blocks carved in the Fire-Sheep year
  (1967) in Sikkim, stored at the Tarndu Ling Temple of Baksa Monastery. GMR
  **inverts** the two monastery words relative to their dictionary senses —
  `chos sgar` (literally "dharma camp") becomes "Monastery" and `dgon`
  (literally "monastery") becomes "Temple". The spans follow his English,
  not the lexicon. The wylie carries a damaged head token `shogces pa'i`
  (illegal syllabification, `shog` and `ces` run together) and `19a67`, where
  a folio marker has collided with the numeral 1967. Both left whole and
  unparsed rather than guessed at. **WYLIE QUEUE: both.**
- **167** reproduces the prose wording of 157 verbatim — "setting the mind on
  the object". The caption series and the prose treatment of the nine states
  agree, which is worth knowing: the chart is not a separate tradition.
- **171–172**: the elephant is the mind and the monkey is distraction; black
  colour is dullness on one and agitation on the other. `kha dog nag pa` →
  "black color" has its members CROSSING inside the compound — `nag pa`
  (black) precedes `kha dog` (color) in English but follows it in Tibetan.
  That crossing is what exposed the generator's member-ordering bug.
- **174**: the wylie is the agentive `'dis`, not a bare `'di`; the ergative
  -s is not separable here.

- Evidence layer: 4,380 headwords / 7,954 pairs. C03 at 174/620.

### Batches C03-59 … C03-63 (C03:175–189) — 2026-08-28 — model: Opus
Five pages, the middle run of elephant-path captions. Fifteen segments, and
**ten generated on the first attempt** — against zero on the batch before
last, because the rules the earlier refusals taught are now written into the
proposer prompt rather than discovered by failing.

The five refusals were all one shape: a genitive `'i` marked as a member of a
parent that stops short of it (`rgod pa` + `'i` where the wylie is `rgod pa'i`).
Ten unnestable null members dropped, two with real content promoted.

**A recurring authoring trap, now seen twice:** the agentive `'dis` proposed
as `'di` + a separable `-s`. It is not separable, and the word-boundary rule
catches it (a flat `'di` would have to sit inside `'dis`). Segments 174 and
178 both hit it. **Worth adding to the proposer brief.**

One reconciler independently derived the folio-marker pattern from the
two-digit cases — "176 `1a0/` = 10, 177 `1a1/` = 11 … so `9a/` is marker 9" —
which is exactly the rule the whole-spine sweep confirmed on 115 of 119
occurrences the same hour. Two independent derivations agreeing is worth more
than either alone. See `docs/upstream/FOLIO_MARKER_COLLISIONS.md`.

Content notes:
- **175** is the second of the nine states, and the caption is the prose list
  item at C03:157 **verbatim** — differing only in the capital at the head of
  a caption. The chart captions are drawn from the prose list, not paraphrased
  off it. The same holds at 179 (one comma added). Worth carrying in the app
  as a live cross-link: caption ⇄ list item.
- **`'jog pa` takes four renderings across the nine states**: `sems 'jog pa`
  "setting", `rgyun du` / `glan de 'jog pa` "keeping", `nye bar 'jog pa`
  "maintaining", `mnyam par 'jog pa` "reaching deep meditation". A translator
  must not fix one equivalent from any single caption. This is precisely the
  case Adam's preserve-all-renderings ruling exists for.
- **175 leaves "the mind on the object" and "brief" unwrapped** — neither has
  a Tibetan exponent in a three-word caption. GMR carries the object phrase
  over from the prose list, and "brief" is his gloss distinguishing state 2.
  Letting `'jog pa` own them would be over-capture exactly.
- **187** carries GMR's `[17]` where the sequence and the Tibetan marker both
  say 21. His text stands verbatim; the discrepancy is recorded upstream.

- Evidence layer: 4,400 headwords / 8,006 pairs. C03 at 189/620.
  Broken-words gate: 11,328 English spans across 253 pages, clean.

### Batches C03-64 … C03-67 (C03:190–201) — 2026-08-28 — model: Opus
Four pages closing the elephant-path captions and opening the prose section
that re-treats the nine states at length.

**Twelve of twelve segments generated on the first attempt** — the first
fully clean batch of the campaign. Nothing dropped, nothing promoted, no
repairs needed. The progression across four batches is the point: 0/6, then
5/9, then 10/15, now 12/12, as each batch's refusals were written into the
proposer brief rather than remembered.

**A reconciler contradicted a verified finding, and was wrong.** On C03:191
it judged the folio marker `2a5/` intact, reasoning from its clean-looking
neighbours `2a4/` and `2a6/`. Sound reasoning, wrong conclusion: the run
`1a0/ 1a1/ … 1a9/ 2a0/ 2a1/` is caption numbers 10–19 and 20–29 with a folio
letter spliced in — folios would read `1a, 1b, 2a, 2b` — and Geshe Michael's
own bracketed numbers confirm the repair on 115 of the 119 occurrences found
across the whole spine. **The correction is recorded in the page note rather
than silently applied**, because a future reader will make the same
three-neighbour inference and should find the answer there.

Worth naming as a general lesson: an agent reasoning from local evidence
reached a confident, plausible, wrong conclusion against a finding proved on
115 independent witnesses. Local plausibility does not outrank a measurement.

**The terminology divergence is now three-way, and confirmed against the
spine.** `sems 'jog pa` is:
- "setting the mind on the object" — the list at C03:157 and caption 167
- "placing the mind on the object" — the prose at C03:201, 202

Two bands of Geshe Michael's own English for the same state, in the same
course, differing by one verb. Exactly what Adam's preserve-all-renderings
ruling protects, and a case a single-equivalent dictionary would have to
destroy.

- Evidence layer: 4,419 headwords / 8,067 pairs. C03 at 201/620.
  Broken-words gate: 11,420 English spans across 257 pages, clean.

### Batches C03-68 … C03-71 (C03:202–209) — 2026-08-28 — model: Opus
The prose treatment of mental states one through six. Long segments (300–800
characters of wylie, 33–90 spans each) against the short captions before them.

**Three generator defects surfaced, two of them corrupting output rather than
refusing** — the worse kind, and both were found only because the prose is
denser than anything the tool had seen.

1. **`with_members` sorted members by their GLOBAL first occurrence.** C03:207
   has the parent "fourth state" with members "fourth" and "state" — and
   "state" also sits in "fifth state" 90 characters earlier, so the members
   sorted backwards and the nesting cursor refused the page. Members now sort
   by position *inside their parent*.

2. **An ambiguous member could silently land on the wrong occurrence.** C03:186
   has the parent `'phror` with the terminative member `r`; the search took the
   FIRST `r` and rendered `'ph<r>or`, cutting the syllable in two **on a page
   that had already shipped**. Members are exempt from the word-boundary rule
   by design — a member *is* a sub-part — so nothing else could catch it.
   Now guarded: a member occurring more than once inside its parent refuses.
   `c3p62` regenerated.

3. **Equal counts were being treated as ambiguity.** C03:209 has `bying rgod`
   six times in the Tibetan and "dullness and agitation" six times in the
   English. The blunt rule nulled all six. **Equal counts in order are a
   determination, not a guess** — the i-th span takes the i-th occurrence —
   and unequal counts still refuse. That change alone recovered 7 of 8
   `bying rgod` links in this batch.

**The broken-words gate earned its keep on its first live run.** It flagged
`un` inside "unable" on c3p71. That is legitimate — `mi nus` → "unable" with
the member `mi` → "un" — but it exposed that ALLOWED_SUBWORD held only the
*stem* half of the negative-affix convention ("necessary" out of
"unnecessary") and not the *negative* half. Both are the same convention seen
from opposite ends; the list now says so.

- Evidence layer: 4,466 headwords / 8,214 pairs. C03 at 209/620.
  Broken-words gate: 11,686 English spans across 261 pages, clean.
  24 pins on the generator, 12 mutation-verified.

### Batches C03-72 … C03-75 (C03:210–217) — 2026-08-28 — model: Opus
States eight and nine in prose, the long impressions passage (102 spans on a
single segment — the campaign's densest), and the opening of the Lam-Rim
outline.

**Two findings in Geshe Michael's own text, both preserved verbatim:**
- **C03:210 reads "the eight state of meditation"** where "eighth" is meant.
  His slip, and it stands. Recorded so a reader meets the note rather than
  the puzzle.
- **The Tibetan name of state 8 is not in the segment at all.** Grepping the
  wylie for `rtse gcig`, and for `gcig`, returns nothing; the wylie opens
  with the bare ordinal `brgyad pa'i skabs su ni`, "on the occasion of the
  eighth". Geshe Michael supplies the quoted label from the list band for the
  reader's benefit. So no span claims it and the whole quoted phrase stays
  unwrapped — which is the correct treatment and worth stating, because a
  span claiming it would look perfectly reasonable.

**A false positive in one of my own guards.** The bound-morpheme diagnostic
tested containment: a flat span whose Tibetan sits inside the depth-5 span
above it was refused as a mis-depthed morpheme. C03:212 has `gnyis pa`
("second") followed by `gnyis` ("two") as a genuinely separate word, and the
test refused it. **In Wylie every standalone word starts a syllable, and
syllables are space-separated — so position, not containment, is the signal.**
The check now looks for a syllable boundary. Regression-checked across all
six batches; pinned, mutation-verified.

**The negative-affix convention has a third form.** The broken-words gate
flagged `less` inside "effortlessly" — `'bad rtsol med par`, where the
negation `med` owns the English suffix. Three forms have now surfaced, one
per batch: the contraction `n't`, the prefix `un`, the suffix `less`. The
allowed list is grown deliberately, one confirmed case at a time, rather than
guessed at up front — which is why each addition carries its citation.

- Evidence layer: 4,505 headwords / 8,308 pairs. C03 at 217/620.
  Broken words: 11,871 English spans clean. Split syllables: 15,023 Tibetan
  spans clean. Both across 265 pages.

### Batches C03-76 … C03-79 (C03:218–237) — 2026-08-28 — model: Opus
Twenty Lam-Rim outline headings across four pages. **Twenty of twenty clean on
the first attempt** — the second fully first-pass batch, and the largest.

**The reconciliation on C03:218 is the strongest of the campaign**, and worth
reading as a model. It checked the heading's exact wylie across the whole
spine, **corrected two claims its own proposals had made**, and turned up a
register split neither proposer saw:

- Both proposals said the phrase occurs three times, all in Course 3. It
  occurs **four** times — C03:218, C03:350, C16:415, P1:37 — and the latter
  three carry byte-identical English. So the terse heading rendering is a
  single witness standing against three identical expansive ones, not a
  course-local split.
- Proposal A claimed `bskyed pa` → "Developing" is consistent across sibling
  headings 226, 241, 247, 273. It holds for the first three and **fails at
  273** ("How to Develop"). Restated at the strength the evidence supports.

**A REGISTER SPLIT ON `gus pa`, verified independently and quantified.**
This is the class CLAUDE.md names as requiring a warning and forbidding
auto-resolution (its example is `bsod nams`: "goodness" in prayers, "merit" in
prose). Measured across the whole spine:

    segments whose wylie contains `gus pa`        184
      English contains "reverence"                 34
      English contains "respect"                   93
        ...but 25 of those ALSO contain another
        respect-word (bkur sti, 'dud pa, zhe sa),
        so at most                                 69   are attributable
      BOTH "reverence" and "respect"                1
      neither                                      56

**CORRECTED, and the correction matters.** The figure was first stated as
34 / 93, which counts the *word* "respect" appearing in a segment whose wylie
contains `gus pa` — not the word *rendering* `gus pa`. In 25 of those segments
another respect-word is present and may be what "respect" answers to. The
defensible attribution is **at most 69**, not 93.

The asymmetry survives the correction — 34 against ≤69, with **one segment in
184 using both words** — so a register effect is still the best reading:
"reverence" in the devotional passages (C03:218, 297, 349, 374), "respect" in
vow-keeping and conduct (C03:53, 530, 531). But it is now a *reading*, not a
measurement.

**The honest limit: segment co-occurrence cannot attribute a rendering to a
key.** Only span-level evidence can, and the alignment layer currently holds
exactly one `gus pa` span (C03:218 → "Reverence"). Confirming this split
properly needs the corpus scanned far enough to supply span-level witnesses on
both sides. Until then it is a strong candidate, flagged for human review, and
must not be stated as established.

*(Two counting traps found here, both worth keeping: `respect` had to be
filtered for "respectively" and "with respect to" — 102 unfiltered against 93
— and then for competing Tibetan respect-words — 93 against 69.)*

- Evidence layer: 4,513 headwords / 8,361 pairs. C03 at 237/620.
  Broken words: 11,967 English spans clean. Split syllables: 15,141 Tibetan
  spans clean. Both across 269 pages.
