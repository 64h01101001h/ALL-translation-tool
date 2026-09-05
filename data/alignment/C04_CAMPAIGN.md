# C04 CAMPAIGN — ACI Course 4 (The Proof of Future Lives), full-depth alignment

Opened 2026-09-04, after the C03-stratified correctness audit
(`docs/AUDIT_2026-09-04_C03_STRATIFIED.md`, 19/100 confirmed defects) and
its two preconditions: the rule-7 hard check in RECONCILE_BRIEF and the
gate `no_supplied_span_head` (zero-or-allowlisted for this directory).
Briefs are course-parametrised from today; the task names the course.

Standing rules (Adam): GMR's English stays VERBATIM, typos flagged never
corrected; nothing binding without human review; every batch records its
producing model (propose: Claude Opus 5 · reconcile/refute: Claude Fable
5.1); completeness is proved by the builder's COVERAGE GATE, never asserted
in prose; a reconciler's deliverable is a spec FILE that exited 0.

## Course shape (spine scan, 198 segments)
- **1–21: the liturgical opening, BYTE-IDENTICAL to C03:1–21** in both the
  wylie and the english columns (mandala offering, refuge and the Wish,
  dedication, the Buddhist grace; english = GMR phonetics, with the English
  stanza fused onto rows 6, 11, 16, 21). Not re-proposed: c4p1–c4p7 are
  the landed c3p1–c3p7 copied verbatim with course labels changed and a
  CLONED PAGE banner. A regeneration-based clone was tried first and
  refused by the generator: the C03 phonetics pages carry d=7 links with
  no d=5 parent (the phonetics protocol), a shape the generator's later
  d=7-parent rule no longer accepts, so those pages cannot be regenerated
  by anyone. Identity proof: every sentence's plain text was checked
  against the C04 rows at copy time (21/21), and the layer gates re-derive
  every span against the C04 rows on each run. Citations become C04:n;
  the pairs are the same pairs.
- **22 and 28: corpus markers** (wylie = an English running head; english =
  the Course IV syllabus / the English introduction to Jewel of the True
  Thought, both cut at the 1,500-char cap, E-107). Hand-built marker
  blocks per the E-071 protocol, unpaired clause, no dictionary pairs.
- **23–198: prose**, Buddhist logic. Gyaltsab Je's *Light on the Path to
  Freedom* (rgyal tshab dar ma rin chen), Geshe Yeshe Wangchuk's *Jewel of
  the True Thought* (ser smad spom ra dge bshes ye shes dbang phyug), Kedrup
  Tenpa Dargye's *Tik chen*; debate format — `chos can` (subject), `thal`
  (it follows), `phyir` (because), `ma grub na` (if not established), `skyon
  med de` (no fault); GMR renders the syllogism frame with a fixed English
  register ("It follows that…", "because…", "Suppose you say…"). Row
  headings like "Reading Two Subject: …" are fused onto the English of rows
  27, 29, 37, 49, 62, 78, 86, 93, 117, 133, 168 (the following-heading
  class, registered — do not file). Median wylie length 180 chars, max 560.
- Verse citations from Dharmakirti's *Commentary on Valid Perception* in
  rows 137–145, 182, 186, 188, 194, 197 (double-shad lineation `, ,`).

## Gates
The seventeen standing gates plus `no_supplied_span_head`, which for this
directory refuses any d=5/d=7 English span beginning with
the/a/an/and/or/his/our/your/I/you unless `data/alignment/span_head_allow.json`
lists it under `pages_c04` with the Tibetan word that licenses it. The
reconciler names the licensor or trims the span; there is no third option.

## Progress
**COMPLETE 2026-09-04.** c4p1–c4p66 (1–198) landed; coverage gate proved 198/198 with `"complete": True`; 17 gates green; `pages_c04` finished at 0 function-word-initial d5/d7 spans.

## Queues (wylie / english), C04
(opened empty)

### Batches C04-8 … C04-11 (C04:22–33) — 2026-09-04 — propose: Opus 5 · reconcile/refute: Fable 5.1

First proposed batch of the course: 10 segments (22 and 28 are hand-built
markers), 20 proposals, 20/20 exited 0 on re-run; 10 reconciled specs,
10/10 exited 0 first run. Pages c4p8–c4p11 wrapped and registered;
coverage 33/198; layer 5,963 headwords / 12,132 pairs; 16 gates green and
`no_supplied_span_head` reports **0** function-word-initial d5/d7 spans in
pages_c04 — the rule-7 hard check held on the first batch under it.

**Course shape confirmed on the ground.** C16:516–522 are byte-identical
parallel witnesses of C04:26, 27, 33, 34, 35, 36 (same file re-ingested,
so not independent). C04:29 is an **UNPAIRED ROW**: its English column is a
page number plus the running head ("13 The Asian Classics Institute Course
IV … Reading 2B: What it Was that Master Dignaga Stated") while the wylie
carries the two inference definitions; both analysts and the reconciler
banked nulls only (69 spans, zero pairs). Six Tibetan-side errata filed
from that row (fused `rjes dpggrags`, `sbyis` for `sbyin`, missing `mi`,
`tsham` for `tsam`, `rtag` for `rtags`) — ACIP column carries the same
readings, so they are source-of-record keyings; sent to the refute pass
before anything is registered.

**Rule 10 rulings.** 23 `ma dmigs pa` → ∅ (both analysts had
"non-existence"; dmigs pa is observe, not exist); 24 `gsung` → "written"
(not "written by"; the "by" is the genitive's hinge); 26 `gnas 'di la`
split flat, `bya'o` → "should"; 27 `sbyong bar` → ∅ (train, not "remain"),
`skyon brjod kyi smra ba` → "speak badly of others" as one idiom; 30
`dgod pa` → ∅ (GMR's frame "What it Was that" has no single word under
it); 31 title compounds kept with d=7 members; 32 `tshad ma` → "correct
person" (the Pramanasiddhi sense, GMR's fixed rendering); 33 `rtsa ba'i`
→ "entire work" over "major". Reconciler 25 named licensors for three
one-word spans (`nga` → "I" ×2, `'am` → "or"); these are whole-span
pairings, not span heads, so the gate does not list them.

**Errata 24.** Both analysts flagged "Light **for** the Path to Freedom"
against "Light **on**" elsewhere; the reconciler dropped it (a title
variant in GMR's own prose, no witness says which is canonical) — noted
here, not filed.

### Batches C04-12 … C04-19 (C04:34–57) — 2026-09-04 — propose: Opus 5 · reconcile/refute: Fable 5.1

24 segments, 48 proposals, 48/48 exited 0 on re-run; 24 reconciled specs,
24/24 exited 0. Pages c4p12–c4p19. Coverage 57/198; layer 6,043 headwords /
12,346 pairs; 16 gates green; `no_supplied_span_head` still 0 in pages_c04.

**Interruption.** Five reconcilers (39, 40, 41, 43, 44) were killed by the
account's Fable rate limit mid-run. 39 and 43 had finished; 40, 41, 44 had
written a spec that re-ran at exit 0 but no errata.json/report.md. They
were RESUMED in their own context once the limit lifted, not re-run and not
moved to another model — the model policy held. Nothing was landed on an
unfinished reconcile.

**First allowlist entry.** 57 `rang re'i` → "our" (span w1): the
first-person possessive licenses the pronoun; recorded in
`data/alignment/span_head_allow.json` under pages_c04 with the reconciler's
reason. All other rule-7 heads on these pages were trimmed.

**Rule 10 rulings of note.** 34 `dang po` → ∅ (means "the first", GMR
resolved the outline reference to "main subject"); 36 the tighter span
each time ("One who turned correct" → "turned correct"); 38 `chos kyi grags
pas` whole → "Dharmakirti" (rule 9, agentive -s), `dgongs` → "Thought" not
"True Thought"; 40 `ngos gzung ba'o` → ∅ (one verb, two "to identify");
42 first `mtshan nyid` → ∅ (four "definition" against two tokens); 47
`dgos pa'i` → ∅ (two "must", first is GMR's frame), the enclitic `'am`
dropped rather than minted as a bound d=6; 48 `gsar du rtogs pa` → "fresh
perception" as a compound; 50/52/53 `thub pa` → "Buddha" / "able Buddha"
with "Victorious" left to GMR; 51 mirrored the landed C04:31 (byte-identical
row) and rejected `ser`/`smad` → "Sera"/"Mey" (transcription, not English);
53 `de` → "it" (the quoted string), `ldan` → ∅; 54 the two `pa'ang` stand
at d=5 (cannot be d=6, bound); 55 `gsung` → ∅; 56 the `ma grub na` /
`gzhi grub par thal` formulas flattened to the word layer; 57 `mtshan nyid
du bzhag` → "define" as one idiom, `phyogs` → "look" rejected though it
passed; 58 `rtogs zin` split (`zin` → ∅), `kho bos` → "our" whole.

**Errata.** 57 `bcas shes` for `bcad shes` (ACIP `BCAS SHES`; independent
witnesses C04:48 and SVN:1202) — to the refute pass. 61 "There are two
reason why" (ENGLISH_TYPO) filed by both analysts, pending reconcile.

**Ruling item (2026-09-04, C04:80).** GMR's English reads "The root text of
Master Chandrakirti says:" where the root text quoted is Dharmakirti's
*Commentary on Valid Perception*; the wylie says only `rtsa bar` ("in the
root text") and names no author, so this is not a Tibetan-checkable erratum
and was not filed. It is an attribution in GMR's own frame — E-120 class,
for GMR's ruling; the English stays byte-exact. Both analysts and the
reconciler noted it.

### Batches C04-20 … C04-35 (C04:58–105) — 2026-09-04 — propose: Opus 5 · reconcile/refute: Fable 5.1

Sixteen pages landed in eight waves of six segments (commits 5500a93 →
f489385); every reconciler spec exited 0 first or second run; 17 gates
green after each wave; `pages_c04` stayed at 0 function-word-initial
d5/d7 spans throughout — the rule-7 hard check plus the allowlist is
holding as designed.

**Allowlist entries added** (each names its licensor, checked by the
`no_supplied_span_head` gate): c4p21/s61w12 `rang gi` → "its proper";
c4p28/s83w18 `khyod kyi` → "your"; c4p30/s89w2 `'o skol gyi` → "our";
c4p33/s99w18 `rang re'i` → "our". Every other rule-7 head was trimmed
(C04:102 both analysts trimmed "Our own" → "own" and the reconciler kept
the agreed trim).

**Errata filed (each skeptic-passed, Fable 5.1).** E-144 C04:57 `bcas
shes` for `bcad shes`; E-145 C04:61 "There are two reason why"; E-146
C04:73 "even it"; E-147 C04:98 "The two phrases terms" (dittography, no
Tibetan source for either noun; C16:575 is the same file re-ingested —
173/198 C04 wylie lines recur in C16 — so no independent witness). Dropped
as not errata: C04:24 "Light for"; C04:75/77 `'ags kyang` (attested);
C04:81 `gnyis gyis` (published reading).

**Conventions settled in this stretch.** `thugs rje chen po` → "holy
great compassion" (the honorific `thugs` is the exponent of GMR's "holy",
the very distinction C04:98 teaches) and `snying rje chen po` → "great
compassion" (98, 100, 107, 108). `sgrub byed` takes the tight verb and
leaves GMR's expansion unwrapped ("helps bring", not "helps bring Him
about"; "allows", not "allows one to reach": 96, 97, 104). `der`/`de` →
"this", never "this state"/"same One" (93, 94). `stobs kyis` → ∅ when GMR's
exponent is discontinuous ("driven … by", 93). `mthar phyin pa'i` kept as
the d=5 unit with the split readings as d=7 members (92). `la sogs pa` →
∅ rather than "or the like" (every trim heads with a supplied function
word, 99). `rdzogs pa'i` → "enlightened" rejected as a rule-10 falsehood
(rdzogs pa is *complete*, 99). The opening `zhes gsungs` of a row closes
the previous row's citation and pairs with nothing (107). A single verb
GMR renders twice by *different* strings may be banked once at its literal
sense (`bshad` → "said", 108), unlike the identical-string cases nulled at
40/47.

### Batches C04-36 … C04-66 (C04:106–198) — 2026-09-04 — propose: Opus 5 · reconcile/refute: Fable 5.1 — COURSE CLOSED

Thirty-one pages in sixteen waves (commits 08112f1 → the closing commit);
every reconciler spec exited 0; 17 gates green after every wave; the
rule-7 hard check held at 0 violations for the whole course. One wave
(132–143) was killed by a Fable/Opus rate limit before any deliverable
existed and was relaunched whole under the same model policy.

**Allowlist entries added:** c4p37/s110w3 `rang re'i` → "our own";
c4p51/s152w9 `sogs` → "or the like"; c4p52/s156w17 `sogs` → "and the
like"; c4p55/s163w7 `sogs` → "and so on" (the etc. particle is its own
licensor; C03 precedent "or anything else"/"or whatever"). `yang` → "And"
at C04:187 sits at d=6 and is outside the gate's d5/d7 scope.

**Errata filed (each skeptic-passed, Fable 5.1):** E-147 C04:98 "The two
phrases terms"; E-148 C04:117 `lus gyi` → `lus kyi`; E-149 C04:117 `med
bar 'dod` → `med par`; E-150 C04:134 `dge bshes dge bshes` → `dge bshes ye
shes` (seven Tibetan witnesses); E-151 C04:143 "which it depends" missing
"on"; E-152 C04:160 `des dbang gis` → `de'i dbang gis` (28 vs 2 corpus
rows); E-153 C04:166 "it is state of mind" missing "a". Dropped: C04:140
"missing las skyes pa" (two complete 7-syllable variant lines, not a
corruption); C04:141 leading `bdag` (registered E-111 row spill); C04:153,
170, 171 ACIP `MTSAMS`/`MTSAN` (TS = tsha keying convention, corpus
596/0 and 1199/0); C04:172 "non- meritorious" (registered hyphen-break
class E-025/028/029/045; refuted as a re-file).

**Conventions settled 106–198.** `thal`/`thal ba'i` → ∅ when GMR's "would
have to"/"is the case"/"It is true that" is his syllogism frame
(127/131/148/157/184/198); `na`/`ltar na` → ∅ for "Suppose you say"
(124/125/131); `chos can` → "consider" (fourteen landed spans, 164/185);
`phyi ma` → "new life"/"future lives"/"another"/"later" on the HGM gloss,
never "states" (149/153/158/195); `bdag` → "self-nature" whole when GMR
hyphenates it (HGM glossary tier: 178/188/190/194), "self" otherwise
(177); `'du byed kyi las` → "immature karma" as the compound only, `'du
byed` alone stays null (170/171); `srid pa` → "ripened karma" (180);
`mtshams sbyor` → "cross the line" on the HGM gloss, members never split
(139/149/165); `rnam smin` ≠ "karma" (158); `res 'ga'` → "variable" banked
as sense-for-sense with a literal note (120); transcribed names and titles
(Kedrup, Geshe, Sera Mey, Tenpa Dargye) stay null, Sanskrit-origin names in
GMR's English are banked (119/134/169/181); a pronoun is never banked as
its referent (`de'i` → "wall", 116; `de dag` → "This", 130); compounds
minted only to escape a count ambiguity are dissolved (123/125/174/185),
real HGM-glossed units are kept (`skye ba snga phyi`, 135; the four
upādāna, 177).

**Ruling items carried forward for Adam:** C04:80 Chandrakirti
attribution (E-120 class); the layer-wide article habit; the negation
subword ruling; audit items 5/15/40/46/77; Phase 3 gate ratification.
