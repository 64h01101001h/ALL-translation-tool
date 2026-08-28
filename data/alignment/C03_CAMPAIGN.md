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
c3p1–c3p42 (1–126) done; 127+ open; 494 segs remain.

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
