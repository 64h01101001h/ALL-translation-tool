# Errata Register — Asian Legacy Library Core Documents
**Compiled 2026-08-28 · Working draft for the Director and in-house editors**
*Machine-readable sidecar: `docs/errata_register.json` — same entries, sortable and filterable.*

---

## 1. What this is

This register lists defects found in the ALL core documents — the Tibetan (ACIP/Wylie) source text and Geshe Michael Roach's facing English — for Course 1 and the opening of Course 3. It was produced as a **by-product of a machine alignment campaign**: the campaign's job was to align Tibetan and English segment by segment, and the notes below are the anomalies its operators recorded in passing. Every entry has since been **checked individually against the corpus database** (42,199 aligned segments, spine `hgm_spine_v27_2.db`) — the quoted strings were retrieved from the spine, the proposed corrections were tested against parallel passages, and the campaign's own segment references were re-derived rather than trusted. Fifteen of the campaign's segment hints were wrong and have been corrected here; four of its proposed corrections were wrong and are flagged as such.

Fifty-six records were reviewed. Four are duplicates of other records and have been merged, leaving **52 distinct findings**: **32 document errata** (2 HIGH, 24 rows at MEDIUM, 6 at LOW), **8 data-pipeline artefacts**, **8 observations checked and closed as not errata**, **3 campaign status lines carrying no claim**, and **1 record that could not be verified at all**. Because several passages are ingested twice (the same text appears in C01 and again in C16, C07 or C17), the list of loci an editor must touch runs about two dozen entries longer than the list of errata.

One internal coherence check is worth recording: the campaign's own running typo numbering across these notes maps one-to-one onto the loci actually found in the spine, which is mild independent evidence that no note in this batch was invented.

---

## 2. What we are NOT claiming

- **Nothing has been changed.** No document, ACIP file, English text, or database row has been edited on the strength of this register. It is a list of findings, not a record of corrections.
- **Geshe Michael's English is preserved byte-for-byte**, by standing project rule. Every English "expected" reading below is a proposal for a human editor, not something the project has adopted. The same applies to the Tibetan: the campaign held every flagged reading verbatim.
- **Confidence is stated per entry and means what it says.** CONFIRMED means the defect was reproduced in the spine and the correction is supported by named witnesses. PROBABLE and UNCERTAIN entries need a human decision and must not be actioned automatically.
- **A confirmed defect is not always a confirmed repair.** Five entries (C01:122, C01:388, C01:481, C03:48, C03:53) establish that something is wrong without establishing what the source actually reads. These are listed again in §6.
- **This covers only what has been scanned, AND only what was written down.** C01 in full, C02, and C03 through segment 295 as of compiling. But the register is built from anomalies the campaign *happened to note in prose* — it is not a systematic proofread. **An unflagged segment is not a clean segment; it is an unexamined one.** Courses C04-C18 have not been read at this depth, though duplicate passages inside them are named where a C01/C03 fix reaches them.
- **Not every anomaly is an error.** Eight records are registered specifically as *not* errata so a later pass does not re-raise them.

---

## 3. HIGH severity

Defects that would mislead a reader who has no other cue in front of them.

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:127** (E-008; E-009 is the same finding) | `'ga' zhig pha ma rigs rus sogs kyi zhen khog blos **mthongs** pas bon la sogs pa'i log chos kyis bslus pa ni` | `zhen khog blos **ma thongs** pas` | `blos mthongs` occurs once in 42,199 segments. `blos thongs` occurs 9 times (C01:156/158/159/160/188, C16:62/64/65/87), `blos ma thongs` twice (C01:170, C16:71). C01:170 fixes the negative idiom: `chos brgyad blos ma thongs na` = "if a person fails to stand free of the eight worldly thoughts." The English at 127 reads "out of a mistaken loyalty to their family traditions stick stubbornly to whatever beliefs their parents happened to have held" — they have *not* given up, so the negation is required. Same error shape as C01:193: `ma X` collapsed to `mX`. | CONFIRMED |
| **C03:39** (E-049; E-050 is the same finding) | `sil snyan rnams dang **byung pa** gdugs mchog dang` | `sil snyan rnams dang **byug pa** gdugs mchog dang` | C03:38, the glossary segment immediately preceding, spells it correctly and defines it: `byug pa ni spos zhim po'i lde gu'o`. The facing English reads "Cymbals and ointments"; C03:40 glosses "'Ointments' are thick potions with a delicious smell of incense." `byung pa gdugs` occurs once corpus-wide; `byug pa gdugs` occurs nowhere, so the corpus holds no correct copy of this line. The error is in the ACIP input, not the conversion. | CONFIRMED |

**Two warnings attached to these rows.**

1. At C01:127 the campaign log's proposed correction — restoring `blos thongs pas` ("has given up") — **inverts the sentence** and must not be applied. The required reading is `blos ma thongs pas`. Flagging this is the main value of the entry.
2. Both errors are invisible to tooling. `mthongs` is a real dictionary headword (skylight/opening, cf. `rgya mthongs`) and `byung pa` is an ordinary word ("arose, occurred"). Neither will trip a spellchecker; only context catches them. C03:39 is graded HIGH rather than MEDIUM (one reviewer graded it MEDIUM) because it is the root verse of Shantideva's offering section, a text students recite from memory, and a reader consulting the Tibetan alone is taught the wrong word with nothing to signal it.

---

## 4. MEDIUM severity

Errors that are real and worth an editor's time, but where the facing English, a parallel line, or the surrounding argument prevents an attentive reader from being actually misled.

### 4a. Tibetan source text

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:86**, **C16:34** | `ma math bslab pa gsum gyis thugs rgyud dul ba` (ACIP: `MA MATH`) | `ma mtha' bslab pa gsum gyis …` (ACIP: `MA MTHA'`) | `math` is not a possible Tibetan syllable (no -th suffix), so it cannot be a variant. English opens "At the very least, your lama must absolutely be a person who has controlled his mind…"; "at the very least" is `ma mtha'` elsewhere in this corpus (TCS03:504, TCS04Initiation:569/731/856). `ma mtha'` occurs 38 times corpus-wide; `ma math` only here and in its duplicate. Minimal explanation is a keying transposition MTHA' → MATH with loss of the apostrophe. Error is in the ACIP source, not introduced by conversion. | CONFIRMED |
| **C01:97** (a) | `dge bshes sne'u zur pa'i slob ma la 'chi rkyen **byung bsogs** ltar` | `byung ba sogs` | English renders "…who met with an untimely death, among others"; "among others" = `sogs`, which requires the nominaliser `ba`. `byung ba sogs` occurs 9 times corpus-wide; `byung bsogs` only here. Present in the ACIP field. Single locus. | CONFIRMED |
| **C01:97** (b) | `mi nus pa'i tshul tsam gyis **che phyi mar** dmyal ba mnar med du skye bar gsungs` | `tshe phyi mar` | English: "the result which ripens onto a person in his future lives"; `tshe phyi ma` = next/future life. `tshe phyi mar` attested 3 times; `che phyi mar` only here. Present in the ACIP field. Single locus. This is the more dangerous of the two at this segment: `che` and `tshe` are both real words, so the line scans as Tibetan and is silently wrong — only the facing English discloses it. | CONFIRMED |
| **C01:113** | `**lem rim** 'di lha bla ma byang chub 'od kyis jo bo la…` | `lam rim 'di …` | English begins "Now about the expression 'Steps of the Path'" — `lam rim` is exactly that, and the segment goes on to explain how the expression arose. `lam rim` occurs 209 times, `lam rim 'di` 12 times; `lem rim` exactly once, here. Present in the ACIP. Single locus. Consequence: the one segment that *defines* the term is the one segment that cannot be found by searching for it. | CONFIRMED |
| **C01:122**, **C16:48** | `rang gis nyams su len bya'i **chos kyi thugs** de yang ma nor ba…` | `chos kyi khungs` *(recommended — see §6)* | The clause requires a source-noun: English reads "whatever dharma we choose to practice should have its ultimate origin in something infallible," and the sentence's own simile is `chu 'go'i phugs gangs la thug dgos pa bzhin du`. `thugs` (honorific "mind") cannot carry "ultimate origin." `chos kyi khungs` is corroborated twice in the same argument: C01:123 `chos khungs med la…` ("no authentic origin") and C01:128 `bon chos kyi khungs phyin ci log tu gzigs te` ("Bon had errant beginnings"). | CONFIRMED (defect); restoration recommended, not settled |
| **C01:159**, **C16:65** | `zhes sogs phyag dang kha ton **dgom** pas thams cad la de ltar gsungs pa na` | `sgom pas` | `dgom` has zero headwords in the 105,634-entry master dictionary; `sgom` and `sgom pa` are both present, and `sgom pas` occurs in 20 segments. English: "…making prostrations, and reciting prayers, and meditating, and everything else." Same typo in the duplicated Drom Tonpa passage at C16:65, so it is inherited from the shared source file. | CONFIRMED |
| **C01:193**, **C16:89** | `nga skyid pa gnyen gyis **mtshor** zhing , ,sdug pa dgra yis ma tshor bar,` | `gnyen gyis ma tshor zhing` | Five parallel lines in the same verse keep the negation intact (`dgra yis ma tshor bar`, `grogs kyis ma tshor zhing`, `sring mos ma tshor bar`, `mi yis ma tshor zhing`). English of line 1 is negative: "No way my loved ones know I'm glad." The seven-syllable meter requires the extra syllable. `gnyen gyis ma tshor` occurs nowhere else. **Caution:** `mtshor` alone is legitimate elsewhere (`mtsho` + terminative, e.g. `rgya mtshor`) and is a dictionary headword — do not fix by global search-and-replace. | CONFIRMED |
| **C01:216**, **C16:103** | `de bzhin ngan 'gro **thams do**, ,dge las bde 'gro thams cad dang ,` | `de bzhin ngan 'gro thams cad do` | `thams do` occurs twice in 42,199 segments — here and its duplicate — and nowhere else. Line 3 of the same verse has `thams cad` intact. Meter is decisive: lines 1, 3, 4 scan as 7 syllables; `thams do` scans as 6 and `thams cad do` restores 7. English renders "all" in both lines. **The log's proposed correction (`thams cad`) drops the terminative and leaves the line a syllable short.** The mechanism is a dropped syllable, not a substitution. | CONFIRMED |
| **C01:244**, **C16:120** | `tsha bas gdungs tshe bsil ba dang , **grangs bas** gdungs tshe dro ba dang…` | `grang bas gdungs tshe` | Three witnesses: the mirror clause `tsha bas gdungs` requires the noun for cold as its parallel member; the English reads "When you're in a place that's very cold, then something warm seems like pleasure"; the dictionary distinguishes `grang ba` (cold) from `grangs` (number, counting). `grang bas` returns zero corpus hits, so the correct form is nowhere attested here. Substitutes one real word for another, so not self-evidently a non-word — a student keying off the Tibetan could mis-derive a gloss. | CONFIRMED |
| **C01:284**, **C07:38**, **C16:142** | `de **thabs cad** rang gi dgra gnyen gnyis ka lan grangs mang po byas pa'ang mnyam na` | `de thams cad` | English: "they are equal in that every one has acted as both my enemy and my friend, many many times." The same segment already uses `thams cad` correctly in its closing clause (`mkha' khyab kyi sems can thams cad la`). `thabs cad` is not a Tibetan phrase. **Knock-on effect:** the malformed string has already propagated into the dictionary as a glossary-tier headword `thabs cad` with an empty gloss and `corpus_courses [C01, C07, C16]`; the text fix should be paired with retiring that scaffold entry. | CONFIRMED |
| **C01:425** | `zhes sbyar na **go bad bde** bar` | `zhes sbyar na go bde bar` | `go bad` is a hapax in the whole corpus. The same sentence appears verbatim twice more — C06:159 and C17:81 — with identical preceding Wylie and identical English ("It helps your understanding of this point if you take the same pattern and read it as I am emptiness, Emptiness me"); both read `go bde bar`. `go bde ba` is well attested (TCS05:394, TCS05:443, TCS18Commentary:178/225). **The log's guess `go ba bde bar` is wrong** — that form appears nowhere in the corpus. A two-witness correction, safe to apply. | CONFIRMED |
| **C01:455** | `rgyud sde chen po'i dkyil 'khor **rgyal ma chog** dang khungs 'dra ba yin` | `dkyil 'khor rgyal mchog` | `rgyal ma chog` is a hapax. `dkyil 'khor rgyal mchog` is attested eight times (TCS06:71/139/160, TCS10:648/655, TCS11:98, TCS13:917, TCS17:679) as a fixed technical term, matching this segment's English about "the holy practices in the great secret traditions." Mis-segmentation, not mis-hearing: `mchog` split into `ma chog`. Consequence: search failure, and `ma chog` reads as a negation ("not allowed"), inverting the sense for anyone parsing syllable by syllable. *(Note: the campaign filed this under a heading for 456; the string is at 455.)* | CONFIRMED |
| **C01:472** | `skyugs pa'i dam chos **rin cen** srid 'dir lham` (ACIP: `RIN CEN`) | `rin chen` (ACIP: `RIN CHEN`) | `rin cen` occurs in 2 of 42,199 segments; `rin chen` in 294. The other instance, SSL:192 `rin cen sems mchog`, has a direct parallel at C01:316 spelled `rin chen sems mchog` — same phrase, correct, same course. English here: "The jewels of the holy dharma." The ACIP carries the same dropped H, so this is input-side keying, not conversion. | CONFIRMED |
| **C01:481** | `ban gzugs chos med nad pa 'dan grva su dha'i **badzrsa**,` | `badzras` (ergative -s) *(see §6)* | `badzrsa` is a hapax; `badzra` appears in 350 segments. The English names him "Suddhi Vadzra, a monk from the monastery of Den," and endnote 202 confirms the identification (the Tibetan editor, Lobsang Dorje). The name is the agent of the sentence-final `bsgrigs pa` (compiled), requiring the ergative -s on a vowel-final name; OSE:290 gives the exact parallel in another colophon: `dge slong su dharme shva ras`. Read as a transposition BADZRAS → BADZRSA, **not** as the log's "stray s" — deleting the s would leave the colophon's agent without its ergative marker. | CONFIRMED (defect); repair needs the woodblock |
| **C03:35** | `rdul gcig steng na rdul **snyeed** sangs rgyas rnams` | `rdul snyed` | `snyeed` occurs exactly once in 42,199 segments, and is in the ACIP input as well as the Wylie, so it is keying, not conversion loss. `rdul snyed` is attested twice in this same course (C03:33, C03:50) and again at C16:303, TCS03:277, MB2:17, ILL:298, ILL:4079. `snyeed` is not a legal Tibetan syllable. | CONFIRMED |
| **C03:36** | `de dag **bsngags ba** zhes pa'i tshigs bcad gcig ste` | `bsngags pa` | Self-proving inside the segment: `bsngags ba` at the head, then the correct `bsngags pa` nineteen words later in the same line. After a -gs suffix the particle is `pa`, not `ba`. Present in the ACIP input. Breaks exact-string search for the verse incipit. | CONFIRMED |
| **C03:53** | `de nas dmigs pa gsal bar byas nas **mNaDl** phul te` (ACIP `MnAdL`) | `maNDla` (ACIP `MAndLA`), the spelling used at C03:1 *(see §6)* | C03:1 records the same word as `MAndLA` / `maNDla` with English "mandel." C03:53 transposes the vowel A onto the retroflex and drops the final A, leaving a form that is not well-formed Wylie (bare `m` root with no vowel; a `Dl` stack with no vowel). C03:53's English confirms the word: "make an offering of a mandala." These are the only two occurrences in the corpus, so there is no third witness. | CONFIRMED (discrepancy and malformedness); which spelling the printed source carries is not established |
| **C03:157**, **C03:179**, **EM:843** | `sems 'jog pa, rgyun du 'jog pa, **glan de 'jog pa**, nye bar 'jog pa…` | `glan te 'jog pa` | Two witnesses inside the same course read `glan te 'jog pa`: C03:201 (parallel enumeration of the nine states) and C03:205, which explains the term through the patching simile `gos lta bu'i gzhi la lhan pa brgyag pa bzhin` ("The place where you put a normal patch is a piece of cloth"). Grammar agrees: after final -n the continuative particle is `te`. Corpus-wide, a syllable in -n is followed by `te` 1,913 times. **The log's proposed `slan te 'jog pa` is wrong** — the root here is `glan` (patch), not `slan` (again); only the particle is at fault, and "correcting" to `slan te` would introduce a second error. | CONFIRMED |
| **C03:158** | `brtson 'grus kyi stobs kyis bdun pa dang brgyad **pa'i grub**` | `brgyad pa 'grub` | All four other clauses in the same sentence read the verb `'grub` with no genitive (`sems gnas dang po 'grub`, `gnyis pa 'grub`, `gsum pa dang bzhi pa 'grub`, `lnga pa dang drug pa 'grub`), and it closes `dgu pa 'grub po`. English: "The seventh and eighth you bring about through the power of effort, and the ninth with the power of complete habituation." The 'a-chung has migrated one position left across the space. | CONFIRMED |

### 4b. English text

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:197** | "What we mean by 'leisure' is to be free of the eight different ways a person can lack opportunity, and to the opportunity to attempt some spiritual practice." | "…and **to have** the opportunity to attempt some spiritual practice." | Ungrammatical as it stands — the second conjunct has no verb. The Tibetan being rendered is `dal ba ni mi khom pa brgyad dang bral zhing chos byed pa la khom pa`, i.e. "free of the eight lacks of opportunity **and having** the opportunity to practise dharma," so a verb is required to carry `khom pa`. "Opportunity to attempt" occurs only here in the corpus, so no parallel preserves the intended wording. The register asserts only that a verb is missing; the exact restoration ("to have the opportunity" vs "to have the leisure") is the editor's call. | CONFIRMED |
| **C01:323** | "By the merit of giving and other **goods** deeds I do" | "…and other good deeds I do" | Endnote 141 (Buddha-Dharma-Sangha prayer) in Notes to Reading Eight, in the four-line refuge-and-bodhichitta verse. The same good/goods slip recurs as "goods things" at C01:29, C01:89, C01:250 and C16:125, which argues for an author-or-typesetting error rather than an ingest slip. **Highest practical value in the LOW/MEDIUM band:** the text is a refuge prayer intended for recitation and reprinting, so the error propagates into practice materials. | CONFIRMED |
| **C01:369**, **C16:195** | "if every object is nothing more **that** what we label it" | "nothing more **than** what we label it" | Two hits corpus-wide, both carrying the same passage (the gold/brass, pillar/pitcher objection). "That" for "than" is the only grammatical construal. Appearing identically in both course printings indicates it is in the underlying English, not a single ingest slip. Graded MEDIUM because it sits in the hinge sentence of the labelling/emptiness argument, where a student is already working hard. | CONFIRMED |
| **C01:388**, **C16:203** | "when **for your this** so-vivid 'me' that stands out there on his own without relying on the two of mind and body ceases to be" | *Undetermined — see §6* | Both segments read "for your this" identically, so the defect is in the shared source text. The clause has no reading as it stands. The Tibetan (`lus sems gnyis la ma brten par nga kho rang gi ngos nas grub pa ling ling po de med par…`) supports only "when this so-vivid me … ceases to be"; whether "for your" is a corruption of "for you" or stray matter is not recoverable. **Do not auto-correct.** *(The same note flagged "mind and body" as reversed against `lus sems`; that is ordinary English idiom and is not filed as an erratum.)* | CONFIRMED (defect); correction open |
| **C01:438**, **C16:216** | "What he meant by the expression '**Look for ahead**' was that we should set our sights on becoming a Buddha." | "'Look **far** ahead'" | C01:437, the segment directly above, quotes Geshe Dolpa's three words correctly: "'Look far ahead,' 'Think very big,' 'Keep a pace.'" — so the gloss misquotes the line it is glossing, one segment later. The Tibetan `mig rgyang bsring` (extend the gaze far) is decisive. The identical pair recurs at C16:215/216. Self-correcting for an attentive reader, but anyone quoting the gloss alone propagates a corrupted form of a well-known three-word mnemonic. *(E-034 was a duplicate record of this finding and has been merged — do not bill an editor twice.)* | CONFIRMED |

---

## 5. LOW severity

Orthographic slips. Meaning is unaffected and no reader is misled.

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:87**, **C16:35** | "represents greater merit **that** any other good qualities that the student might possess" | "greater merit **than** any other good qualities" | Verbatim in the English field of both segments. "That" for "than" after a comparative is unambiguous. | CONFIRMED |
| **C01:89** | "Towards any of the **goods** things in the present or future lives" | "any of the good things" | Verbatim in the Tsongkapa verse on relying on the spiritual guide. Part of the good/goods family below. | CONFIRMED |
| **C01:236** | "and suffer as **you** body is sliced apart or ripped to pieces" | "as **your** body is sliced apart" | Facing Wylie `lus bcad pa dang dral ba sogs kyi sdug bsngal` (`lus` = body). A spine-wide search for "as you body" returns exactly one hit, so this is an isolated slip, not a house convention. | CONFIRMED |
| **C01:250**, **C01:29**, **C16:125** | "not even A moment's wish for the **goods** things of cyclic life" | "the good things of cyclic life" | The Three Principal Paths renunciation quatrain, quoted more than once in the course. Corroborated by C01:232, which uses the correct form: "a total disgust even for the supposed good things of this revolving life." Together with C01:89 and C01:323 this makes **five loci in one good/goods sweep**. *(The campaign hint of 249 was wrong — that segment is the prose lead-in. It also cited C01:238 as a witness; 238 reads "wonderful things" and is not one.)* | CONFIRMED |
| **C01:253** (×2), **C01:262**, **C01:267**, **C16:131**, **C16:224**, **TITLS:2982** | "paths for **practicioners** of lesser scope"; "even practicioners of lower paths"; "Practicioners who have not yet developed…" | "practitioners" | Standard English spelling is "practitioners." The spine contains no instance of the correct spelling adjacent to these, so this is a house-wide consistent misspelling rather than a local slip — a single global pass, not seven separate edits. *(Recorded in the source batch as two entries, E-018 and E-019, merged here.)* | CONFIRMED |
| **C01:434** | "People like you **an** I can start a practice in the morning" | "People like you **and** I…" | Dropped "d"; the sentence admits no other reading. No parallel segment exists, and none is needed. *(Separately: even corrected, "People like you and I" is a case slip for "you and me." That is a style question for the editor and is not filed as an erratum.)* | CONFIRMED |

---

## 6. Data-pipeline artefacts

These are not editorial errors and should not go to a document editor. They are damage introduced or preserved by digitisation and by the ACIP→Wylie conversion, and they are fixed once, upstream, for the whole corpus.

**Already documented separately:** 119 folio-marker collisions are catalogued in `docs/upstream/FOLIO_MARKER_COLLISIONS.md`. The instances noted at C03:165–166 below look like the same class and should be checked against that document rather than tracked here.

### Class A — hyphen-plus-space in the English (line breaks preserved at scanning)

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:355**, **C03:274** | "in the cause-and- effect interpretation accepted by all the schools" | "cause-and-effect" | A hyphen followed by a space mid-compound is the signature of a hyphenated line break preserved at digitisation. Search for `cause-and- effect` returns exactly these two segments. | CONFIRMED |
| **C01:379** | "the oh-so- vivid, self-standing 'me'" | "oh-so-vivid" | Single hit. "oh-so-deep" and "oh-so-high" appear correctly closed at C03:346, C16:413, P1:35. | CONFIRMED |
| **C01:380**, **C16:199** | "The non- existence of this is what we call 'no-self.'" | "non-existence" | Same class. C01 alone carries at least 16 instances of word-internal "x- y": seq 56 "single- pointed", 72 "all- knowingness", 265 "self- made victors", 310 and 496 "loving- kindness", 489 "even- mindedness", plus 355 and 379 above. | CONFIRMED |
| **C03:144**, **C03:149** | "sitting in a cross- legged position" | "cross-legged" | The closed form occurs 11 times elsewhere, so the spaced form is the anomaly. **C03:149 carries the identical defect and was missed by the campaign note.** "self- nature" at C03:53 and 27 further segments belongs to the same family. | CONFIRMED |

**Recommendation:** one regex sweep for word-internal `[a-z]- [a-z]` closes this whole class corpus-wide. Fixing these one segment at a time is wasted effort.

### Class B — missing space after the shad in the ACIP input

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **C01:475** | `rigs pa'i nyag phran **dag,sbyor** bas log smra'i snying la 'big par mdzod` | `nyag phran dag, sbyor bas` | Single hit corpus-wide. Every other shad in the same segment is spaced (`dpa' po yis, ,mdo rgyud`), so the closed-up comma is an inconsistency, not a convention. | CONFIRMED |
| **C01:477** | `lam bzang bgrod pa'i **mig,mkhas** dang grub pa'i rjes su gtod na mdzes` | `bgrod pa'i mig, mkhas dang` | Single hit; two lines later in the same closing verse-set; other shads in the segment are spaced. English confirms the clause break: "May I win the beauty of eyes … fixed upon the wise and adept ahead." | CONFIRMED |
| **C03:35** (secondary) | `bzhugs pa **dag,de** ltar` | `bzhugs pa dag, de ltar` | Every other clause break in the segment is spaced. A missing space, not a missing comma as the note claimed. | CONFIRMED |
| **C03:48** | `'jig rten sgron ma **dag,byang** chub rim par sangs rgyas ma chags brnyes` | `sgron ma dag, ,byang chub rim par` *(exact repair to be confirmed)* | The same segment punctuates its other three verse-line breaks with `, ,` (`…ma chags brnyes, ,mgon po de dag…`). English confirms a line boundary: "Lamps who light / Each of the worlds… / Those who have taken / Each of the steps." Whether the repair is a space or the `, ,` shad pair is inferred, not established. Neighbours in the same class: C03:163 (`dag,dben`, `shog,g-yeng`), C03:164 (`cig,dpe`, `gi,yi ge`), C03:212 (`dgos;bdun pa`). | CONFIRMED (defect); exact repair open |

Corpus-wide, letter-comma-letter appears in **271 of 42,199 segments (0.6%)**, 16 of them in C01. Cosmetic to a human reader, but it fuses two syllables into one token, so both words are unfindable by search. **Note:** these live in the ACIP e-text itself, so the fix belongs to the input-correction stream rather than to the converter.

### Class C — the ACIP→Wylie converter drops braced Sanskrit stacks

| Citation | What it says | What it should say | Evidence | Confidence |
|---|---|---|---|---|
| **Seq 6 in all eighteen courses** (verified identical rows; recorded as C01:6 and C03:6) | ACIP `,AI Dm GU RU RA{tn} MAnd LA Km NI{ry}'A TA YA MI, ,` → Wylie `ai dM gu ru ra maND la kM ni'a ta ya mi,` | Wylie should carry the stacks the ACIP encodes: `ratna` from `RA{tn}`, `nirya` from `NI{ry}` | The ACIP source is **correct**; `{tn}` and `{ry}` are the ACIP escapes for Sanskrit stacks (cf. C01:344 `BU{ddh} P'A LI TA` = Buddhapalita). The derived Wylie silently drops the braced content. The same segment's own English prints "Idam guru ratna mandalakam niryatayami" correctly, twice, and TCS07:82 (also 334/406/454) carries the undamaged Wylie. | CONFIRMED |

**This is systematic, and one of its instances is more serious than the rest.** The same converter loss produces **`bu pA li ta` for Buddhapalita at C01:344** — there a reader is taught a wrong author name with no correct form anywhere nearby, which is a HIGH-impact consequence of a MEDIUM-severity pipeline defect. The converter also leaks raw braces at C03:351 (`a{u2} Ta'i` from `Sm {bh}A{u2} tA'I`).

**Recommendation:** one converter fix clears all eighteen courses plus C01:344 and C03:351 at once. Do not open eighteen tickets. (Records E-044 and E-046 are the same defect seen from two courses and have been merged.)

---

## 7. Entries we could not verify, or could not fully resolve

### 7a. Could not be verified at all

| Record | Status | What would resolve it |
|---|---|---|
| **E-055** (C03, hinted at 161) | **UNCERTAIN.** The log excerpt is truncated. It opens mid-sentence ("unparsed rather than guessed at") and its flag reads "WYLIE QUEUE: both," where "both" refers back to two items named in a sentence that is not in the excerpt. C03:161 itself was checked and is clean; its Tibetan and English correspond. | Retrieve the two sentences preceding line 1102 of the source campaign log. The item is almost certainly recoverable from them. **Separately and on its own account:** C03:165 reads `tshe 'das aa song gizhbs 'jug byed po` and C03:166 reads `spyi lo 19a67 lor` (a folio marker `a` spliced into the year 1967) and opens `shogces pa'i`, a word split across the C03:165/166 boundary and then run together. Any of these could be the "both" the log meant. They are **not** registered as this item, because linking them would be a guess — but they need an editor's eyes regardless, and should be checked against `docs/upstream/FOLIO_MARKER_COLLISIONS.md`. |
| **E-056, first sentence** | The sentence "His slip, and it stands" belongs to a preceding item not present in the excerpt. It could not be attached to any segment. | Re-read the source campaign log at line 1242. If that sentence names a real defect, it is still unregistered. |
| **E-042** | The record itself is a campaign wrap-up line with no quotable text, and its segment hint (C01:1) is spurious. But it asserts **a 26-item English typo queue for C01 that is not itemised anywhere in this batch.** | Extract those 26 items from the campaign logs and verify each individually. This record is a pointer to them, not a substitute for them. It should not be left in the register as an open erratum. |
| **E-022**, **E-043** | Campaign status/gate lines ("Zero-catch again"; "battery/letter gates green first run"; layer counts; "C03 is next"). Neither quotes core-document text or names a searchable string. The "drafting slip" mentioned in E-022 was in the campaign's own draft page, not in the translation or the ACIP, and was corrected before that page was written. | Nothing. Recommend striking both from the register rather than leaving them open. |

### 7b. Defect confirmed, correction not

These are already listed in §4 but are collected here because each needs a human decision before anything is changed.

| Citation | The open question |
|---|---|
| **C01:122** / C16:48 | `khungs` is the recommended restoration (attested at C01:123 and C01:128 in this exact sense). The only competing candidate is `phugs`, echoing `chu 'go'i phugs` seven words earlier in the same sentence. An editor should tick one box. |
| **C01:388** / C16:203 | Delete "for your," or emend to "for you"? The Tibetan does not decide it. Do not auto-correct. |
| **C01:481** | `BADZRAS` (transposition) vs `BADZRA` (stray s). The distinction matters: deleting the s leaves the colophon's agent without its ergative marker. Confirm against the woodblock before changing. *(The preceding `su dha'i` was not assessed; the campaign did not flag it and we have no independent reading.)* |
| **C03:53** | Which spelling does the printed folio carry? If C03:53 is faithful to a scribal abbreviation in the source, this becomes a non-erratum needing only a search alias, not a correction. Check the original folio rather than normalising blind. |
| **C03:48** | Space, or the `, ,` shad pair used at the other three breaks in the same segment? Confirm against the printed source. |
| **C01:211** | Filed as **PROBABLE not-an-erratum**, no action recommended. "he must believe in the law of actions and consequences at all" renders `de la dang por las 'bras la yid ches dgos`, where `dang por` = "in the first place." The campaign itself filed it as "(not typo)." "At first" is a possible alternative reading of the intent, but nothing establishes that anything other than what is printed was written. Recorded so it is not re-raised. |

### 7c. Checked and closed — not errata

Registered so that a later pass does not re-derive them. **No editorial action on any of these.**

- **C01:123** — `chos khungs med` is spelled *correctly* here; this record is the supporting witness for the C01:122 correction and should travel with it, not be actioned.
- **C01:295** (and C07:49, C17:110) — `longs spyad ma bra ba` is a legitimate idiom, not a corruption. GIE:960 glosses it explicitly (`btang mi bra ba zhes pa ni, 'jig rten na mi bra zer ba de yin`); AK:142 attests it independently. The English "hesitating to use anything for herself" is exactly the sense. The conjectured `ma phod` form returns zero hits. *(Lexicon note, not a text erratum: the dictionary carries `bra ba` with the auto-aligned gloss `["had"]`, which is meaningless and should be reviewed against GIE:960.)*
- **C01:306** — `lhag pa'i bsam pa` vs the contracted `lhag bsam` at C01:282. Both are correct Tibetan for the same term. Variant, not typo.
- **C01:365** (and C16:193) — `grub grub pa` is emphatic reduplication, not dittography. Attested 9 times across eight independent works (DDA:67/208/253/256, EM:289, GIE:633, ILL:1357, TCS06:75/78, TCS13:699), always in the same idiomatic frame of something established from its own side. **Correcting it to a single `grub` would damage the text.** Withdraw from the queue.
- **C01:484** — the leading backtick is a structural marker in the ACIP e-text, opening a new text or titled section, not stray damage. It opens 111 segments across at least 15 courses (C09 ×23, C13 ×19, C18 ×12, C17 ×9, C08 ×7, C07 ×6). Keep it. **C09 alone will raise this 23 more times if it is not recorded as resolved.**
- **C01:492** — `ae` is the standard Classical Tibetan interrogative particle and functions normally here; `nges nges` is ordinary emphatic reduplication. A review queue of unusual-looking but valid tokens; registering any of it would be a false positive. *(One correction to the log for the record: its cross-reference "cf. 436 ×2" is off — C01:436 contains a single standalone `ae`.)*
- **C03:210** — the Tibetan names the eighth state only as "the eighth" while the English supplies "making the mind single-pointed." This is the translator's consistent practice, not an omission: C03:206–211 all do the same, and the names are restored from the enumeration at C03:157 and C03:201. The supplied name matches that list.
- **C01:472, secondary claims** — `bcu` → "all ten forces" is correct (the `stobs bcu` of a Buddha; the book's own endnote 196 at C01:496 glosses them), and the tail `lham` is not truncated: the verse scans as four clean 9-syllable lines, and `lham` is most likely the metrically clipped form of `lham me` (shining forth), matching "into this world came." Leave `lham` alone.
- **Orthographic variation, flagged for consistency only, not as defects:** `phreng ba` (C03:38) vs `'phreng ba` (C03:39) — both are attested spellings of "garland." `MTSO`/`TSIG` in the C03:36 ACIP are correct ACIP orthography for `mtsho`/`tshig`, not misspellings. `byang chub 'od` rendered "Jangchub Uw" at C01:113 is GMR's own phonetic convention, attested in the published text.

Throughout the batch, a large number of the campaign's notes were **translation-technique observations rather than error reports** — `brten` fronted at C01:86, `rnal 'byor` → "the master meditator," `zung 'brel` rendered as a doubling, `jo bo` → "this holy place," `chos kyi dbyings` → "Every corner of space," `sras` → "sons and daughters," "omniscient enlightenment" vs "all-knowing Lord," "Think very big" shortened in a paraphrase. None of these are filed as errata and none should be forwarded as such.

---

## 8. Recommended actions

### For document editors

1. **Start with the two HIGH entries: C01:127 and C03:39.** Both substitute one real Tibetan word for another, so neither is catchable by tooling, and both sit in text students read on their own.
2. **Do not apply the campaign log's proposed corrections at four places.** They are wrong and would each introduce a new error:
   - C01:127 — `blos thongs pas` inverts the sentence; the correct reading is `blos ma thongs pas`.
   - C01:216 — `thams cad` leaves the line a syllable short; the correct reading is `thams cad do`.
   - C03:157 — `slan te 'jog pa` changes the root; the correct reading is `glan te 'jog pa`.
   - C01:425 — `go ba bde bar` is unattested; the two parallel witnesses read `go bde bar`.
   - C01:481 — deleting the stray "s" strips the ergative; the reading is `badzras`, pending the woodblock.
3. **Run three sweeps rather than editing segment by segment:**
   - *good/goods* — 5 loci: C01:29, C01:89, C01:250, C01:323, C16:125. **C01:323 is in a refuge prayer intended for recitation and reprinting**, so this sweep has the widest downstream reach of anything at LOW/MEDIUM.
   - *practicioners → practitioners* — 7 loci: C01:253 (×2), C01:262, C01:267, C16:131, C16:224, TITLS:2982. Subject to the Director's ruling in the next section.
   - *duplicate loci* — most C01 Tibetan errata recur in C16 (and at C07:38 / C17:110 / C17:81 for three of them), because the same passages are ingested more than once. Every fix must be applied at all named loci **or upstream in the shared source file**, which is the better option.
4. **Take the §7c list as closed.** Those readings were checked and are correct; re-flagging them costs time on every later course, and C01:484's backtick alone will resurface 23 times in C09.

### For data-pipeline maintainers

1. **Fix the ACIP→Wylie converter's handling of braced Sanskrit stacks** (`{tn}`, `{ry}`, `{ddh}`, `{bh}`, `{u2}`). One change clears seq 6 in all eighteen courses, restores Buddhapalita's name at C01:344, and stops the raw-brace leak at C03:351. This is the single highest-leverage item in the register.
2. **Sweep the English for word-internal `[a-z]- [a-z]`** — at least 16 instances in C01 alone, plus C03:144, C03:149, C03:274 and 27 further segments in the "self- nature" family.
3. **Normalise the missing space after the shad** across the 271 affected segments (0.6% of the corpus), after confirming the repair shape at C03:48 against the printed source.
4. **Cross-check C03:165–166 against `docs/upstream/FOLIO_MARKER_COLLISIONS.md`** — `19a67` for 1967 and the `shogces pa'i` split across the segment boundary look like the documented folio-marker class.
5. **Three dictionary-side cleanups follow from text errata and should be scheduled with them:**
   - Retire the glossary-tier scaffold headword `thabs cad` (empty gloss, `corpus_courses [C01, C07, C16]`), which exists only because of the C01:284 typo.
   - Review `bra ba`, currently carrying the auto-aligned gloss `["had"]`, against the explicit gloss at GIE:960.
   - Resolve the three coexisting headwords `glan de 'jog pa` (glossed, and the gloss looks unrelated), `slan te 'jog pa`, and `glan te 'jog pa`.

### Decisions that need the Director

1. **Where corrections live.** Project rule preserves Geshe Michael's English byte-for-byte, and the campaign held every flagged Tibetan reading verbatim. Nothing in this register can be applied until it is settled whether corrections are made in the source ACIP/English files, or carried as a separate errata layer displayed alongside the text. This decision gates every editor action above.
2. **Is "practicioners" a house spelling?** It is consistent across seven loci in three documents, with no correct instance anywhere adjacent. That pattern is what an intentional house form looks like as much as what a propagated typo looks like. Someone with the publication history needs to say which it is.
3. **Who checks the physical sources.** Four open readings (C01:481, C03:53, C03:48, and confirmation of C01:122) need the woodblock or the printed folio. They cannot be closed from the database.
4. **The C01 26-item English typo queue.** The campaign asserts one exists; it is not itemised in the material handed over. Decide whether to have it extracted from the logs and verified before C01 is declared scanned.
5. **Scope and sequence.** C01 is complete at this depth; C03 is read to roughly segment 210. C02 and C04–C18 are unscanned, though duplicate-passage evidence already reaches into C06, C07, C09, C16, C17 and C18. Decide whether to finish C03, proceed in course order, or first run the corpus-wide pipeline sweeps in §8 (which would remove a large share of what any later course scan would otherwise report by hand).
---

## Addendum, same day — seven entries the compilation missed, including the worst one

**The register as first compiled did not contain its own highest-severity
finding.** C03:286 — `zhi gnas` glossed "(shamata, or insight)" — was absent,
along with six others, for a reason worth stating plainly:

> The register was built by extracting anomalies from the **campaign logs**.
> These seven were found *after* those logs were written, or were recorded in
> commit messages and design documents rather than in a batch log. The
> extraction saw only one of the places findings actually live.

That is the capture problem in miniature, and it is exactly what Adam's
standing rule was created to stop. The seven are now filed (E-057 … E-063):

| id | segment | severity | what |
|---|---|---|---|
| E-057 | C03:286 | **HIGH** | `zhi gnas` glossed "(shamata, or insight)"; shamata is quietude |
| E-062 | C03:166 | MEDIUM | `shogces pa'i` — illegal syllabification, `shog` and `ces` run together |
| E-063 | C03:187 | MEDIUM | caption numbered `[17]` where the Tibetan and the sequence both give 21 |
| E-058 | C03:276 | LOW | "Bodhisatttva" (three t's) |
| E-059 | C03:258 | LOW | "The Ten **Menal** Afflictions" |
| E-060 | C03:210 | LOW | "the **eight** state of meditation" |
| E-061 | C03:199 | LOW | `[33}` — curly brace closing a bracket |

### What changed so this cannot recur

1. **Batches now emit errata as structured records**, not prose. The alignment
   spec carries an `errata` array with kind, verbatim quote, proposed reading,
   evidence, severity and confidence. Nothing has to be recovered from
   narrative again.
2. **`tools/merge_errata.py` files them, and refuses what it cannot verify.**
   Before an entry enters the register the tool checks that the quoted text is
   **actually present in the spine at the cited segment**. A misquotation or a
   wrong citation is rejected outright rather than merely discouraged — which
   matters, because misquotation is precisely what cost this register a full
   verification pass (fifteen wrong segment references, four wrong proposed
   corrections).

The guard was tested by feeding it a deliberately fabricated entry alongside a
real one. It accepted the real one and refused the fabrication.

---

## Appendix — the finding that changes the fix strategy

**Nine of these errors exist at more than one locus**, because several passages
are ingested into the corpus twice. The `ma math` spelling at C01:86 stands
identically at C16:34. The `that`/`than` slip at C01:87 recurs at C16:35. The
`goods things` typo appears at **four** loci across **two unrelated verses**
(C01:29, C01:250, C16:125, and the Tsongkapa verse at C01:89). One error
reaches three courses at once (C01:284, C07:38, C16:142).

Two consequences for whoever acts on this:

1. **A per-segment fix will leave duplicates behind.** Correcting C01:86 alone
   leaves the same error live in C16. Every repair needs its full locus list —
   which is why the note field on each entry carries one.
2. **The right fix is usually upstream, in the shared source file**, not in the
   per-course output. If C01 and C16 both draw a passage from one file,
   repairing that file fixes both and cannot drift apart later.

**A recurrence across unrelated texts, like `goods things`, is worth a global
find-and-replace rather than four separate edits** — it is a house typo, not
four coincidences.

---

## Appendix — how to add to this register

The campaign that produced these findings is ongoing (C03 is at 295 of 620,
and C04–C18 follow). New findings arrive continuously.

**The standing capture rule:** a source-document finding goes in this register,
not only in a batch log. The campaign log records what a batch did; the
register records what the organisation needs to fix. They have different
readers, and a finding buried in batch narrative is a finding nobody acts on.

Each entry needs: the citation, what the source says **quoted verbatim from the
spine** (not from a note — several notes in this batch misquoted), the proposed
reading if determinable, the evidence that establishes it, severity, and
confidence. An honest UNVERIFIABLE is worth more than a plausible invention.
