# THL Language Reference & Tools — Research Dossier

**Scope:** The Tibetan & Himalayan Library's (THL, thlib.org, University of Virginia) language reference materials and tools: the EWTS transliteration spec, the Simplified Phonetics spec, fonts, input/Unicode guidance, dictionaries/translation tool, and learning materials.
**Researched:** 2026-08-12. Method: live fetches of the new thlib.org (WordPress rebuild) and the still-running legacy server **old.thlib.org**, plus Wayback Machine recovery of the retired essay system (`essay_reader.php`) and THL Toolbox wiki.
**Ground rule observed:** nothing below is filled in from general knowledge; every claim traces to a fetched page. Gaps are flagged as GAP.

**Site-state summary (important for future retrieval):**
- `https://thlib.org` / `https://www.thlib.org` — NEW WordPress site (2025–26 rebuild). Old paths (`/tools/`, `/reference/transliteration/`, `#!essay=` URLs) are **404** there.
- `https://old.thlib.org` — the complete legacy site is still up (PHP, essay reader, converters, stacks chart). Intermittent 503s; retry works.
- `https://ttt.thlib.org` — the Tibetan-to-English Translation Tool, alive (v4.0.1, compiled 1-July-2026).
- The new site's tool links point into a SPA: `https://thlib.org/terms/#/terms/transliterate`, `.../terms/phonetics`, `.../translate/tibetan`, and the EWTS text at `/terms/#/texts/67579` (same text id as the Mandala mirror `https://texts.mandala.library.virginia.edu/text/thl-extended-wylie-transliteration-scheme`).

---

## 1. THL Extended Wylie Transliteration Scheme (EWTS) — CAPTURED COMPLETE

**Canonical source captured:** the "Whole Document" render of the essay, Wayback capture 2020-03-01 of
`http://www.thlib.org/reference/transliteration/?_escaped_fragment_=essay=/thl/ewts/all/`
(this is the crawler-static form of the old `#!essay=/thl/ewts` URL). Also live today at `https://old.thlib.org/reference/transliteration/#!essay=/thl/ewts` (JS) and as a book on the new site (`/terms/#/texts/67579`).

**Document metadata (as published):** Version 2.0 · Status: Working Draft · Date: 2004-01-07 · Contributors: David Chandler, David Chapman, Robert Chilton, Tony Duff, Chris Fynn, Nathaniel Grove (co-director), David Germano (co-director), Steve Hartwell, Peter Hauer, Andrew West. Extends Turrell Wylie, "A Standard System of Tibetan Transcription," *HJAS* 22 (1959) 261-67.

**Completeness status: COMPLETE.** All prose sections (Overview, Tibetan Transliteration of Sanskrit, Punctuation), all 16 rules, all 7 charts, and — crucially — the essay's per-character appendix (246 character entries with Unicode codepoint, Unicode name, EWTS equivalent, and usage notes) were captured. The chart tables in the essay body render the Tibetan column as JPG images (lost in text capture), but every character's identity is fully recoverable from the appendix (the `alt`/`title` attributes carried the actual Unicode character and codepoint, which we extracted). The one true loss: the ~33 **private-use-area glyphs (U+F021–U+F042)** are images only (archived at `http://texts.thlib.org/essays/thl/ewts/UF0xx.jpg` via Wayback) — their visual forms are not in this capture, though Tony Duff's descriptive notes for several are (see table).

### 1.1 Design decisions recorded in the essay

- Keeps Wylie's consonant/vowel values unchanged; rejects the PRC modification of `v`/`x` for a-chung/a-chen (reasons given: apostrophe isn't a vowel, grammatical status controversial, and `v`/`x` destroy phonetic approximation).
- Three gaps in original Wylie that EWTS fills: (1) Tibetanized Sanskrit characters without diacritics, (2) unambiguous representation of unusual stacks, (3) Tibetan punctuation coverage.
- Retroflex Sanskrit letters (Tibetan reversed dentals) = **capitalized dental**: `T Th D Dh N Sh` (chart form: `T`, `Th`, `D`, `D+h`, `N`, `Sh`). Anusvara = `M`, visarga = `H` (capitals). Consequence: capitals are *not* available for proper names in strict transliteration (allowed only informally when embedded in English).
- Non-standard stacks joined with `+` (e.g. `d+ha`, `g+ha`, `b+ha`; tantra = `tan+t+ra`, citta = `tsit+ta`, maṇḍala = `maN+Dala`). Within one tsheg bar, an implicit "a" is inserted after each cluster lacking an explicit vowel.
- `.` (period) prevents stacking: `g.yon` (left) vs `gyon`; extendable to any stack-prevention need.
- Escape sequences: `\uXXXX` (4-hex) / `\UXXXXXXXX` (8-hex) for any Unicode char; PUA F021–F0FF assigned to non-Unicode Tibetan marks.

### 1.2 The sixteen rules (content, faithfully condensed; examples are the essay's own)

1. Transliterate left→right in a syllable, top→bottom in stacks; vowel comes after the final consonant of the root stack. (Ex: → `bsgribs`.)
2. No explicit vowel mark ⇒ implicit vowel `a` after the final consonant of root/stack. (Ex: → `mkhan`.)
3. Period displays two consonants horizontally that would otherwise stack. (`gyon` vs `g.yon`.)
4. `+` is **required** between consonants of a non-standard Tibetan stack (see §1.5 stacks list). (Ex: `sat+t+wa`.)
5. `+` between transliterations of multiple vowel signs above/below one stack; vowels transliterated **bottom-to-top** even against logical phrase order. (Ex: `bru+e`; the rdo-rje abbreviation = `rdo+e`.)
6. For a standard stack, `+`-form and plain form are equivalent (`rta` = `r+ta`; plain preferred).
7. Multi-syllable Sanskrit inside one tsheg bar: insert implicit `a` after each vowel-less cluster except under virama (srog med); before final anusvara/visarga the `a` precedes `M`/`H`. (Ex: `sarba mang+galaM`.)
8. Any character may be written `\u`+4-hex (or `\U`+8-hex for surrogates); full-width codes, no dropped leading zeros; PUA values follow the same rule. (`ka` ≡ `ཀ`.)
9. Run of non-Tibetan text: enclose in `[ ]` (UTF-8), nestable; `\u`/`\U` escapes usable inside. (Ex: `khong [New York] la phebs song /`.)
10. Single non-Tibetan character inside Tibetan: prefix with backslash (`de la \3 yod/`). Exception: letter `u`/`U` cannot be backslash-inserted (collides with `\u`) — use `[u]`/`[U]`.
11. A-chen: word-initial without vowel sign = `a`; otherwise transliterate by its vowel sign; mid-stack = `+a`; mid-tsheg-bar = `.a`. (Ex: `a khu`, `ug pa`, `aM`.)
12. Capitals for Sanskrit-based characters: long vowels `A I U -I`; anusvara `M`; visarga `H`; retroflexes `T Th D D+h N Sh`. (`mA` = mā, `duH` = duḥ, `phaT` = phaṭ.)
13. Capital `R` = full-form ra as TOP letter of a non-standard stack (≡ U+0F6A). (`R+na`, `R+Ya` vs `R+ya`.)
14. Full-form ra in the standard stacks rnya/rla/rwa is written lower-case `r`.
15. Capitals `W`, `Y`, `R` = full form of wa/ya/ra in any position except top-most.
16. Lower-case `r`, `y`, `w` in non-standard stacks = superscribed ra (ra mgo), subscribed ra (ra btags), subscribed ya (ya btags), subscribed wa (wa zur).

### 1.3 The seven charts

The essay's charts: 1. Consonants (30, Wylie-identical: k kh g ng / c ch j ny / t th d n / p ph b m / ts tsh dz w / zh z ' y / r l sh s / h a); 2. Vowels (a i u e o, vocalic `r-i l-i -i r-I l-I`, long `A I U`, `ai au`); 3. Numerals (0–9 = Arabic digits; half-numbers only via `༪`–`༳`); 4. Sanskrit letters (aspirates `g+h d+h b+h dz+h`; retroflexes `T Th D D+h N Sh`; compounds `oM k+Sh f v R+ +W +Y +R`; other marks <code>H M ~M ~M` ? &</code>); 5. Punctuation (four groups — intersyllabic, phrase-delimiting, head marks, paired brackets); 6. Other marks/signs in Unicode 4.0 (name markers, cantillation, astrological, symbols, misc, combinations, post-3.0 additions); 7. Characters not in Unicode 4.0 (PUA F021–F042). The definitive per-character values are in the appendix table below — treat §1.4 as the machine-usable form of all seven charts.

Notable single-character assignments called out in the prose: space = tsheg; `*` = non-breaking tsheg; `_` = Tibetan white space; `/` = shad; `;` = tsheg shad; `|` = rin chen spungs shad; `!` = sbrul shad; `:` = gter tsheg (NB the essay discusses and rejects using `:` for visarga, which is `H`); `@` = yig mgo mdun ma; `#` = yig mgo sgab ma (chosen for keyboard adjacency); `=` = bsdus rtags; `<` `>` = gug rtags; `(` `)` = ang khang; `~X`/`X` = honorific/emphasis marks U+0F35/U+0F37; `^` = tsa 'phru; `$`, `%` = caret yig mgo variants U+0F06/U+0F07; `f`/`v` = the U+0F55/U+0F56 + tsa-'phru forms (pha/ba with lenition mark, used for foreign f/v).

### 1.4 Complete character appendix (Unicode ⇄ EWTS), 246 entries — CAPTURED COMPLETE

This is the extracted appendix of the EWTS essay (glyph column from the page's own `alt` text; empty glyph = subjoined form or PUA image without alt). `(blank)` EWTS on 0F0D/0F0E was a converter artifact — corrected against raw HTML: **U+0F0D = `/`, U+0F0E = `//`**.

| Unicode | Glyph | Character name | EWTS | Notes |
|---|---|---|---|---|
| U+0F00 | ༀ | TIBETAN SYLLABLE OM | `oM` |  |
| U+0F01 | ༁ | TIBETAN MARK GTER YIG MGO TRUNCATED A (ter yik go a thung) | `\u0F01` |  |
| U+0F02 | ༂ | TIBETAN MARK GTER YIG MGO -UM RNAM BCAD MA (ter yik go wum nam chey ma) | `\u0F02` |  |
| U+0F03 | ༃ | TIBETAN MARK GTER YIG MGO -UM GTER TSHEG MA (ter yik go wum ter tsek ma) | `\u0F03` |  |
| U+0F04 | ༄ | TIBETAN MARK INITIAL YIG MGO MDUN MA (yik go dun ma) | `@` | honorific; marks beginning of text or start of new folio; cf. mongolian birga 1800 |
| U+0F05 | ༅ | TIBETAN MARK CLOSING YIG MGO SGAB MA (yik go kab ma) | `#` | follows and ligates with initial yig-mgo |
| U+0F06 | ༆ | TIBETAN MARK CARET YIG MGO PHUR SHAD MA (yik go pur shey ma) | `$` |  |
| U+0F07 | ༇ | TIBETAN MARK YIG MGO TSHEG SHAD MA (yik go tsek shey ma) | `%` |  |
| U+0F08 | ༈ | TIBETAN MARK SBRUL SHAD (drul shey) | `!` | separates sections of meaning ≈ topics and sub-topics |
| U+0F09 | ༉ | TIBETAN MARK BSKUR YIG MGO (kur yik go) | `\u0F09` | list enumerator, used in Bhutan |
| U+0F0A | ༊ | TIBETAN MARK BKA- SHOG YIG MGO (ka sho yik go) | `\u0F0A` | petition honorific, used in Bhutan |
| U+0F0B | ་ | TIBETAN MARK INTERSYLLABIC TSHEG (tsek) | (spacebar) | morpheme delimiter; the normal tsheg; provides a break opportunity |
| U+0F0C | ༌ | TIBETAN MARK DELIMITER TSHEG BSTAR (tsek tar) | `*` | non-breaking tsheg; inhibits line breaking; \<noBreak\> 0F0B |
| U+0F0D | ། | TIBETAN MARK SHAD (shey) | `/` | marks end of a section of text (tshig-grub); cf. devanagari danda 0964 |
| U+0F0E | ༎ | TIBETAN MARK NYIS SHAD (nyi shey) | `//` | marks end of a whole topic (don-tshan); cf. devanagari double danda 0965 |
| U+0F0F | ༏ | TIBETAN MARK TSHEG SHAD (tsek shey) | `;` |  |
| U+0F10 | ༐ | TIBETAN MARK NYIS TSHEG SHAD (nyi tsek shey) | `\u0F10` |  |
| U+0F11 | ༑ | TIBETAN MARK RIN CHEN SPUNGS SHAD (rinchen pung shey) | `\|` | shad which follows a tsheg-bar that starts a new line |
| U+0F12 | ༒ | TIBETAN MARK RGYA GRAM SHAD (gya tram shey) | `\u0F12` |  |
| U+0F13 | ༓ | TIBETAN MARK CARET -DZUD RTAGS ME LONG CAN (dzu ta me long chen) | `\u0F13` |  |
| U+0F14 | ༔ | TIBETAN MARK GTER TSHEG (ter tsek) | `:` | comma-like text delimiter; cf. khmer camnuc pii kuuh 17D6 |
| U+0F15 | ༕ | TIBETAN LOGOTYPE SIGN CHAD RTAGS (che ta) | `\u0F15` |  |
| U+0F16 | ༖ | TIBETAN LOGOTYPE SIGN LHAG RTAGS (hlak ta) | `\u0F16` |  |
| U+0F17 | ༗ | TIBETAN ASTROLOGICAL SIGN SGRA GCAN -CHAR RTAGS (trachen char ta) | `\u0F17` |  |
| U+0F18 | ༘ | TIBETAN ASTROLOGICAL SIGN -KHYUD PA (kyu pa) | `\u0F18` | combines with digits |
| U+0F19 | ༙ | TIBETAN ASTROLOGICAL SIGN SDONG TSHUGS (dong tsu) | `\u0F19` | combines with digits |
| U+0F1A | ༚ | TIBETAN SIGN RDEL DKAR GCIG (deka chig) | `\u0F1A` |  |
| U+0F1B | ༛ | TIBETAN SIGN RDEL DKAR GNYIS (deka nyi) | `\u0F1B` |  |
| U+0F1C | ༜ | TIBETAN SIGN RDEL DKAR GSUM (deka sum) | `\u0F1C` |  |
| U+0F1D | ༝ | TIBETAN SIGN RDEL NAG GCIG (dena chig) | `\u0F1D` |  |
| U+0F1E | ༞ | TIBETAN SIGN RDEL NAG GNYIS (dena nyi) | `\u0F1E` |  |
| U+0F1F | ༟ | TIBETAN SIGN RDEL DKAR RDEL NAG (deka dena) | `\u0F1F` |  |
| U+0F20–0F29 | ༠–༩ | TIBETAN DIGIT ZERO…NINE | `0`–`9` |  |
| U+0F2A–0F33 | ༪–༳ | TIBETAN DIGIT HALF ONE…HALF ZERO | `\u0F2A`–`\u0F33` |  |
| U+0F34 | ༴ | TIBETAN MARK BSDUS RTAGS (du ta) | `=` | repetition |
| U+0F35 | ༵ | TIBETAN MARK NGAS BZUNG NYI ZLA (nge zung nyi da) | `~X` | honorific, emphasis; used like underlining |
| U+0F36 | ༶ | TIBETAN MARK CARET -DZUD RTAGS BZHI MIG CAN (dzu ta shi mig chen) | `\u0F36` | marks point of text insertion or annotation |
| U+0F37 | ༷ | TIBETAN MARK NGAS BZUNG SGOR RTAGS (nge zung gor ta) | `X` | emphasis; used like underlining |
| U+0F38 | ༸ | TIBETAN MARK CHE MGO (che go) | `\u0F38` |  |
| U+0F39 | ༹ | TIBETAN MARK TSA -PHRU (tsa tru) | `^` | a lenition mark |
| U+0F3A | ༺ | TIBETAN MARK GUG RTAGS GYON (gug ta yun) | `<` |  |
| U+0F3B | ༻ | TIBETAN MARK GUG RTAGS GYAS (gug ta ye) | `>` | brackets |
| U+0F3C | ༼ | TIBETAN MARK ANG KHANG GYON (ang kang yun) | `(` |  |
| U+0F3D | ༽ | TIBETAN MARK ANG KHANG GYAS (ang kang ye) | `)` | bracketing with a roof over |
| U+0F3E | ༾ | TIBETAN SIGN YAR TSHES (yar tse) | `\u0F3E` | astrological; combines only with digits |
| U+0F3F | ༿ | TIBETAN SIGN MAR TSHES (mar tse) | `\u0F3F` | astrological; combines only with digits |
| U+0F40 | ཀ | TIBETAN LETTER KA | `k` |  |
| U+0F41 | ཁ | TIBETAN LETTER KHA | `kh` |  |
| U+0F42 | ག | TIBETAN LETTER GA | `g` |  |
| U+0F43 | གྷ | TIBETAN LETTER GHA | `g+h` | decomposes 0F42 0FB7 |
| U+0F44 | ང | TIBETAN LETTER NGA | `ng` |  |
| U+0F45 | ཅ | TIBETAN LETTER CA | `c` |  |
| U+0F46 | ཆ | TIBETAN LETTER CHA | `ch` |  |
| U+0F47 | ཇ | TIBETAN LETTER JA | `j` |  |
| U+0F49 | ཉ | TIBETAN LETTER NYA | `ny` |  |
| U+0F4A | ཊ | TIBETAN LETTER TTA | `T` |  |
| U+0F4B | ཋ | TIBETAN LETTER TTHA | `Th` |  |
| U+0F4C | ཌ | TIBETAN LETTER DDA | `D` |  |
| U+0F4D | ཌྷ | TIBETAN LETTER DDHA | `D+h` |  |
| U+0F4E | ཎ | TIBETAN LETTER NNA | `N` |  |
| U+0F4F | ཏ | TIBETAN LETTER TA | `t` |  |
| U+0F50 | ཐ | TIBETAN LETTER THA | `th` |  |
| U+0F51 | ད | TIBETAN LETTER DA | `d` |  |
| U+0F52 | དྷ | TIBETAN LETTER DHA | `d+h` |  |
| U+0F53 | ན | TIBETAN LETTER NA | `n` |  |
| U+0F54 | པ | TIBETAN LETTER PA | `p` |  |
| U+0F55 | ཕ | TIBETAN LETTER PHA | `ph` |  |
| U+0F56 | བ | TIBETAN LETTER BA | `b` |  |
| U+0F57 | བྷ | TIBETAN LETTER BHA | `b+h` |  |
| U+0F58 | མ | TIBETAN LETTER MA | `m` |  |
| U+0F59 | ཙ | TIBETAN LETTER TSA | `ts` |  |
| U+0F5A | ཚ | TIBETAN LETTER TSHA | `tsh` |  |
| U+0F5B | ཛ | TIBETAN LETTER DZA | `dz` |  |
| U+0F5C | ཛྷ | TIBETAN LETTER DZHA | `dz+h` |  |
| U+0F5D | ཝ | TIBETAN LETTER WA | `w` |  |
| U+0F5E | ཞ | TIBETAN LETTER ZHA | `zh` |  |
| U+0F5F | ཟ | TIBETAN LETTER ZA | `z` |  |
| U+0F60 | འ | TIBETAN LETTER -A (a chung) | `'` |  |
| U+0F61 | ཡ | TIBETAN LETTER YA | `y` |  |
| U+0F62 | ར | TIBETAN LETTER RA | `r` |  |
| U+0F63 | ལ | TIBETAN LETTER LA | `l` |  |
| U+0F64 | ཤ | TIBETAN LETTER SHA | `sh` |  |
| U+0F65 | ཥ | TIBETAN LETTER SSA (reversed sha) | `Sh` |  |
| U+0F66 | ས | TIBETAN LETTER SA | `s` |  |
| U+0F67 | ཧ | TIBETAN LETTER HA | `h` |  |
| U+0F68 | ཨ | TIBETAN LETTER A (a chen) | `a` |  |
| U+0F69 | ཀྵ | TIBETAN LETTER KSSA | `k+Sh` |  |
| U+0F6A | ཪ | TIBETAN LETTER FIXED-FORM RA | `R+` | used only in transliteration and transcription |
| U+0F71 | ཱ | TIBETAN VOWEL SIGN AA | `A` |  |
| U+0F72 | ི | TIBETAN VOWEL SIGN I | `i` |  |
| U+0F73 | ཱི | TIBETAN VOWEL SIGN II | `I` | 0F71 0F72 |
| U+0F74 | ུ | TIBETAN VOWEL SIGN U | `u` |  |
| U+0F75 | ཱུ | TIBETAN VOWEL SIGN UU | `U` | 0F71 0F74 |
| U+0F76 | ྲྀ | TIBETAN VOWEL SIGN VOCALIC R | `r-i` |  |
| U+0F77 | ཷ | TIBETAN VOWEL SIGN VOCALIC RR | `r-I` |  |
| U+0F78 | ླྀ | TIBETAN VOWEL SIGN VOCALIC L | `l-i` |  |
| U+0F79 | ཹ | TIBETAN VOWEL SIGN VOCALIC LL | `l-I` |  |
| U+0F7A | ེ | TIBETAN VOWEL SIGN E | `e` |  |
| U+0F7B | ཻ | TIBETAN VOWEL SIGN EE | `ai` |  |
| U+0F7C | ོ | TIBETAN VOWEL SIGN O | `o` |  |
| U+0F7D | ཽ | TIBETAN VOWEL SIGN OO | `au` |  |
| U+0F7E | ཾ | TIBETAN SIGN RJES SU NGA RO (anusvara) | `M` |  |
| U+0F7F | ཿ | TIBETAN SIGN RNAM BCAD (visarga) | `H` |  |
| U+0F80 | ྀ | TIBETAN VOWEL SIGN REVERSED I | `-i` |  |
| U+0F81 | ཱྀ | TIBETAN VOWEL SIGN REVERSED II | `-I` | 0F71 0F80 |
| U+0F82 | ྂ | TIBETAN SIGN NYI ZLA NAA DA | <code>~M`</code> |  |
| U+0F83 | ྃ | TIBETAN SIGN SNA LDAN | `~M` |  |
| U+0F84 | ྄ | TIBETAN MARK HALANTA (virama/srog med) | `?` |  |
| U+0F85 | ྅ | TIBETAN MARK PALUTA | `&` |  |
| U+0F86 | ྆ | TIBETAN SIGN LCI RTAGS | `\u0F86` |  |
| U+0F87 | ྇ | TIBETAN SIGN YANG RTAGS | `\u0F87` |  |
| U+0F88 | ྈ | TIBETAN SIGN LCE TSA CAN | `\u0F88` |  |
| U+0F89 | ྉ | TIBETAN SIGN MCHU CAN | `\u0F89` |  |
| U+0F8A | ྊ | TIBETAN SIGN GRU CAN RGYINGS | `\u0F8A` |  |
| U+0F8B | ྋ | TIBETAN SIGN GRU MED RGYINGS | `\u0F8B` |  |
| U+0F90–0FB8 |  | TIBETAN SUBJOINED LETTER KA…A | same string as the head letter (`k`, `kh`, … `a`) — position in the stack is what marks subjoining; `+` used when non-standard |  |
| U+0FB9 |  | TIBETAN SUBJOINED LETTER KSSA | `k+Sh` |  |
| U+0FBA |  | TIBETAN SUBJOINED LETTER FIXED-FORM WA | `+W` |  |
| U+0FBB |  | TIBETAN SUBJOINED LETTER FIXED-FORM YA | `+Y` |  |
| U+0FBC |  | TIBETAN SUBJOINED LETTER FIXED-FORM RA | `+R` |  |
| U+0FBE | ྾ | TIBETAN KU RU KHA | `\u0FBE` |  |
| U+0FBF | ྿ | TIBETAN KU RU KHA BZHI MIG CAN | `\u0FBF` |  |
| U+0FC0–0FC3 | ࿀–࿃ | TIBETAN CANTILLATION SIGNS (heavy beat, light beat, cang te-u, sbub -chal) | `\u0FC0`–`\u0FC3` |  |
| U+0FC4–0FCC | ࿄–࿌ | TIBETAN SYMBOLS (dril bu, rdo rje, rgya gram, phur pa, nor bu, nyi zla variants…) | `\u0FC4`–`\u0FCC` |  |
| U+0FCE | ࿎ | TIBETAN SIGN RDEL NAG RDEL DKAR | `\u0FCE` |  |
| U+0FCF | ࿏ | TIBETAN SIGN RDEL NAG GSUM | `\u0FCF` |  |
| U+0FD0 | ࿐ | TIBETAN MARK BSKA- SHOG GI MGO RGYAN | `\u0FD0` |  |
| U+0FD1 | ࿑ | TIBETAN MARK MNYAM YIG GI MGO RGYAN | `\u0FD1` |  |
| U+0FD2–0FDA | ࿒–࿚ | Additions since Unicode 3.0 (nyis tsheg; brda rnying yig mgo variants; mchan rtags marks; trailing/leading mchan rtags) | `\u0FD2`–`\u0FDA` |  |
| U+F021–U+F042 | (images only) | PUA assignments for marks NOT in Unicode 4.0 (33 assigned of range F021–F0FF). Notes captured for several: F021 terton's mark of Mingyur Rinpoche; F022 terton's mark of Ratna Lingpa; F023 terton's mark of Dilgo Khyentse Rinpoche; F024/F025 Sambhota system terma marks 1–2; F026 sign of the seven successive Buddhas / seven trustees; F042 "ithi" secrecy sign (all per Tony Duff) | ``–`` | glyph images only (Wayback: `texts.thlib.org/essays/thl/ewts/UF0xx.jpg`) |

*(Rows for sequential runs — digits, half-digits, subjoined letters, cantillation, symbols, 0FD2–0FDA, PUA — are compressed here; the underlying capture has all 246 individual entries with per-character names and notes. Combination-character examples given by the essay: lce tsa can + k = `ྈ+k`, + kh = `ྈ+kh`.)*

### 1.5 Standard Tibetan Stacks list — CAPTURED COMPLETE (both Tibetan and Wylie charts)

Source (live): `https://old.thlib.org/reference/transliteration/tibstacks.php` (also Wayback 2020). This list is normative for EWTS rules 4, 6, 13, 16 ("non-standard stack" = any combination not on this list; excludes loan words and Sanskrit transliteration stacks). Page credits Chris Fynn's cross-indexed "Table of standard Tibetan letter combinations" as a companion reference.

```
rka rga rnga rja rnya rta rda rna rba rma rtsa rdza
lka lga lnga lca lja lta lda lpa lba lha
ska sga snga snya sta sda sna spa sba sma stsa
kwa khwa gwa cwa nywa twa dwa tswa tshwa zhwa zwa rwa shwa swa hwa
kya khya gya pya phya bya mya
kra khra gra tra thra dra pra phra bra mra shra sra hra
kla gla bla zla rla sla
rkya rgya rmya rgwa rtswa
skya sgya spya sbya smya
skra sgra snra spra sbra smra
grwa drwa phywa
```

**Relevance to ALL Tool:** this is the authoritative disambiguation core for any EWTS engine — our C++ `ewts_unicode` port and the Convert pane should treat exactly this set as stackable-without-`+`. Worth diff-testing our engine's implicit stack table against this list. The full charmap (§1.4) is also the right conformance target for a "strict EWTS" mode (escape sequences, punctuation, PUA policy) beyond the everyday subset our corpus uses.

---

## 2. THL Simplified Phonetic Transcription of Standard Tibetan (Germano & Tournadre) — CAPTURED COMPLETE

**Canonical source captured:** all 13 sections of the essay via `essay_reader.php` (Wayback 2020-02-27 for b1–b11, 2024-05-20 for b12; b13 verified live on old.thlib.org). Live base URL today: `https://old.thlib.org/global/php/essay_reader.php?url=/thl/phonetics/s/bN` (N = 1…13); JS UI at `https://old.thlib.org/reference/transliteration/#!essay=/thl/phonetics/`. Cross-checked against the 2003/2007 THDL edition: `http://www.thdl.org/xml/showEssay.php?xml=/collections/langling/THDL_phonetics.xml&m=all` (Wayback 2007-03-15) — substantively identical text.

**Document metadata (as published):** by David Germano and Nicolas Tournadre, December 12, 2003. Copyright © 2003 by David Germano, Nicolas Tournadre, and THL. Version 1.0 · Status: Working Draft · Date: 2010-02-01. Simplified from the more precise "Tournadre Phonetic Transcription."

**Completeness status: COMPLETE for engine-porting purposes.** Sections: 1 About ✓, 2 Background ✓, 3 THL Phonetics (principles) ✓, 4 The General Principle ✓, 5 Special Rules (all 16) ✓, 6 Exceptional Pronunciations ✓, 7 Transliterated Sanskrit ✓, 8 Note about Diacritics ✓, 9 Word Boundaries ✓, 10 Punctuation ✓, 11 Examples (all 33) ✓, 12 List of Exceptions (all 19) ✓, 13 Notes — **an empty stub** (verified live: renders a blank heading; the 2007 THDL full document likewise ends at the List of Exceptions, so nothing is missing).
Quirk to note: section 5's prose says "fifteen rules" but sixteen numbered rules follow — a defect in the source itself, not the capture.

### 2.1 General principle (section 4)

- Root-letter consonants keep their Wylie value except: `c`→**ch**, `th`→**t**, `ph`→**p**, `tsh`→**ts**; a-chung (`'`) as root = its vowel (`a i u e o`).
- Thirty-consonant table (phonetic renderings, in alphabet order): k kh g ng / ch ch j ny / t t d n / p p b m / ts ts dz w / zh z a y / r l sh s / h a. Four vowels: i u e o (implicit a). Approximate values: a as in "hut", i as in "hit"/"reel", e as in "say"/"help", o as in "note".
- **Drop everything silent:** superscripts r l s; prefixes g d b m '; suffixes d ' s; post-suffixes s d. Silent letters may still color the vowel/root (rules below).
- **Pronounced suffixes:** g ng n b m, plus r and l (retained although speech often lengthens the vowel instead).
- Words used in English take English grammar (plural -s, possessive 's) with no effect on the transcription. Examples given: dag pa>dakpa, ring po>ringpo, rin chen>Rinchen, lab>lap, dum bu>dumbu, dmar po>marpo, ril bu>rilbu, sa skya pa>Sakyapa, blo bzang>Lozang (also Lopzang), rnying ma pa>Nyingmapa, dge lugs pa>Gelukpa, gzhis ka rtse>Zhikatsé, mar me>marmé, dge bshes>geshé.

### 2.2 The sixteen Special Rules (section 5) — complete

1. Wylie `c` and `ch` both ⇒ **ch** (avoid hard-c misreading). bcu>chu, gcig pa>chikpa, rin chen>Rinchen, nag chu>Nakchu.
2. `th`⇒**t**, `ph`⇒**p**, `tsh`⇒**ts**. 'phag pa>pakpa, gser thang>Sertang, khang tshan>khangtsen.
3. Vowels **o, u** followed by suffix **d, n, l, s** ⇒ **ö, ü** (suffixes l and n remain pronounced; d and s only umlaut the vowel). ö ≈ German *Götter* / French *feu*; ü ≈ German *Rücke* / French *rue*. bka' brgyud>Kagyü, bsod nams>Sönam, yul>yül, dus tshod>dütsö, bon po>Bönpo.
4. Vowel **a** + suffix **d, n, l, s** ⇒ **e**; in the word's FINAL syllable ⇒ **é**. Vowel **e** as final sound of last syllable ⇒ **é** (elsewhere plain e). Value ≈ German *Käse* / French *aime*. sde dge>Degé, brgyad>gyé, dge rgan>gegen, ral pa can>Relpachen, tshe ring>Tsering, byes>Jé, bstan 'dzin>Tendzin, 'jam dpal dbyangs>Jampelyang.
5. Suffixes **g, b** devoice ⇒ **k, p**. dge legs>Gelek, kha btags>khatak, sngags pa>ngakpa, byang chub>jangchup, thub bstan>Tupten, thabs>tap.
6. **ba/bo as final syllable** ⇒ **wa/wo** (incl. ba'i>wé, bar>war as finals). lha sa ba>Lhasawa, jo bo>Jowo, dpa' bo>pawo, 'gro ba'i>drowé, gsal bar>selwar. Final-syllable only: bar ba>barwa not warwa.
7. **py, phy** ⇒ **ch**; **by** ⇒ **j**. spyan ras gzigs>Chenrezik, phyag>chak, sbyin bdag>jindak.
8. **my** ⇒ **ny**. smyong>nyong, dmyal ba>nyelwa.
9. Pronounced r-subscript clusters are retroflex: **kr, pr, tr, khr, phr, thr** ⇒ **tr**; **gr, br, dr** ⇒ **dr**. sgrol ma>Drölma, grub thob>druptop, sprul sku>trülku, 'bras spungs>Drepung, 'phrin las>trinlé. Unpronounced r-clusters (mr, sr, …) just drop the r: srung ma>sungma, srog rlung>soklung, rdzun smra ba>dzünmawa.
10. l-subscript clusters ⇒ **l**, EXCEPT **zl** ⇒ **d**. klad pa>lepa, glog>lok, zla ba>dawa.
11. l-superscript + h root ⇒ **lh**. lha sa>Lhasa, lho phyogs>lhochok, lhun grub>lhündrup.
12. **db** ⇒ **w**; **dby** ⇒ **y**; **dbr** ⇒ **r**. dbang>wang, dbyar kha>yarkha, dbral>rel.
13. Second syllable begins with a-chung prefix (') ⇒ **nasalization**: insert **n** after first syllable, eliding its suffix (bka' 'gyur>Kangyur, dge 'dun>Gendün, ngos 'dzin>ngöndzin, rig 'dzin>Rindzin, mkha' 'gro>khandro, dkyil 'khor>kyinkhor). If second root is **ph/b** ⇒ insert **m** (chos 'phel>Chömpel, dpal 'bar>Pembar, sku 'bum>Kumbum); but ph/b **with y or r subscript** ⇒ **n** (dpal 'byor>Penjor, rgyu 'bras>gyundré). Documented form-exceptions: skyabs 'gro>kyamdro, rten 'brel>temdrel, lam 'bras>lamdré.
14. Different-sounding vowels joined by a-chung: drop the a-chung. le'u>leu, khyi'u>khyiu.
15. Two same vowels joined by a-chung: drop a-chung, merge vowels. pa'ang>pang, gri'i>dri.
16. Two different vowels joined by a-chung where the second is **i** (genitive particle, always word-final): first vowel transforms as in rules 3–4. 'gro ba'i>drowé, rgyal bu'i>gyelbü, rin po che'i>rinpoché, bdag po'i>dakpö, le'u'i>leü.

### 2.3 Exceptional Pronunciations (section 6) — rule-adjacent phenomena, documented via exception list

1. Superscript l / a-chung ('), and prefix m, can produce nasal **n**/labial **m** at the END of the preceding syllable: dga' ldan>Ganden, 'bras ljongs>Drenjong, me mda'>menda, a mdo>Amdo, rta mgrin>Tamdrin.
2. That labialization/nasalization can elide the preceding suffix: chab mdo>Chamdo, dpal ldan>Penden.
3. Some words with r-subscript on p/ph/b lose the r: 'bri ru>Biru, sbra nag zhol>Banakzhöl.
4. Some words pronounce a second-syllable r-superscript: rdo rje>Dorjé, o rgyan>Orgyen, lha rje>lharjé.

### 2.4 Transliterated Sanskrit (section 7)

Sanskrit-in-Tibetan is rendered with no diacritics, applying all preceding rules: long vowels shortened (sūtra>sutra); retroflexes de-retroflexed even when written with reversed letters (paṇḍita>pendita); ś and ṣ both ⇒ **sh**. Examples: śūnyatā>shunyata, tiṣṭha>tishta, maṇḍala>mendel, vajra>badzra. Applies to Sanskrit letters inside Tibetan words: kaH thog>Katok.

### 2.5 Diacritics policy (section 8)

Only three diacritics exist in the system: **é** (final-syllable), **ö**, **ü**. All are droppable in constrained computing environments (with accepted loss of pronunciation fidelity).

### 2.6 Word boundaries (section 9)

- Mono-, bi-, and trisyllabic words ⇒ one word (lha sa>Lhasa; lha mo skyid>Lhamokyi; but don't merge across word boundaries: bod rang skyong ljongs>"Bö Rangkyong Jong", bu ston rin chen grub>"Butön Rinchendrup", ye shes 'od>"Yeshé Ö").
- Quadrisyllabic ⇒ two words (bsod nams rin chen>"Sönam Rinchen"; gtsang pa khang mtshan>"Tsangpa Khangtsen").
- Personal/place names: capitalize each word.
- Grammatical particles attach to the construed word (usually preceding): chos kyi rnam grangs>"chökyi namdrang", gtan la phab pa>"tenla pappa", 'gyur med>"gyurmé"; preceding-particle example ma byas pa>"majepa".
- Named lexical exception: rgyal rtse>**Gyantsé** (not Gyentsé).
- Interim machine mode documented: syllable-by-syllable with spaces, skipping word-boundary-dependent rules (final ba>wa; final é).

### 2.7 Punctuation (section 10)

For running text: delete all Tibetan punctuation except the shad, which ⇒ **comma**. (Tsheg handling is implicit in syllable/word assembly.)

### 2.8 Examples (section 11) — full list as published

g.yon>yön · rgyal>gyel · phyag>chak · bkra shis>trashi · khros ma>tröma · sprul>trül · mri tam ga>mitamga · srid pa>sipa · pad ma>pema · pan chen>penchen · ral pa can>relpachen · thun>thün · dus gsum>düsum · sbed>bé · ces>ché · btsan dbang>tsenwang · tshong khang>tsongkhang · rdzong>dzong · stabs>tap · thabs>tap · thug pa>tukpa · debs>dep · sib sib>sipsip · lobs pa>loppa · grub>drup · kla col>lachöl · spyan snga ba>chenngawa · byang chub sems dpa'i>jangchup sempé · rin po che'i>rinpoché · gri'i>dri · bon po'i>Bönpö · le'u'i>leü · nga'i deb>ngé dep.
**Internal inconsistency to resolve at port time:** rule 2 says `th`⇒`t` (and the list itself has thabs>tap, thug pa>tukpa consistent with that), yet the published list also prints `thun>thün` — keeping the "th". This contradiction is in the source document itself. Treat the 33 examples as the acceptance battery, decide a policy for the thun case (likely a typo in the essay: rules 2+3 predict "tün"), and document the decision.

### 2.9 List of Exceptions (section 12) — complete, 19 entries (Tibetan alphabetical order)

skyabs 'gro>kyamdro · dga' ldan>Ganden · rgyal rtse>Gyantsé · chab mdo>Chamdo · rta mgrin>Tamdrin · rten 'brel>temdrel · rdo rje>Dorjé · dpal ldan>Penden · bla brang>labrang · 'bras ljongs>Drenjong · 'bri ru>Biru · sbra nag zhol>Banakzhöl · me mda'>menda · lam 'bras>lamdré · lha rje>lharjé · lha bris pa>lhapripa · a mdo>Amdo · u rgyan>Urgyen · o rgyan>Orgyen.
(The 2007 THDL edition lacked me mda' in this list; the current THL edition includes it. Current list is the 19 above.)

**Relevance to ALL Tool:** this is everything needed for a rule-for-rule "THL Phonetics" display mode alongside our GMR pron_engine: 16 rules + 4 exceptional-pronunciation classes + 19-word exception list + 33-example acceptance battery + the interim per-syllable machine mode THL itself used. It also gives us the license posture: © 2003 Germano/Tournadre/THL — the *rules* are facts we may implement; credit the system by name ("THL Simplified Phonetics, Germano & Tournadre, THL") in the UI.

---

## 3. THL's online converters

| Tool | Legacy URL (live) | New URL | Notes |
|---|---|---|---|
| Tibetan-Wylie (transliteration) converter | `https://old.thlib.org/reference/transliteration/wyconverter.php` (200 OK) | `https://thlib.org/terms/#/terms/transliterate` | Unicode⇄EWTS both directions. Created by **Roger Espel Llima**; originally published at `http://www.digitaltibetan.org/cgi-bin/phonetics.pl`. |
| Tibetan-Phonetics converter | `https://old.thlib.org/reference/transliteration/phconverter.php` (200 OK) | `https://thlib.org/terms/#/terms/phonetics` | Wylie or Unicode → phonetics. Word-splitting options: built-in word list / per-syllable / join-char / separator-char (join/sep only for romanized input). Output systems: **THL Simplified Phonemic Transcription, Rigpa English Phonetics, Rigpa Spanish Phonetics, Rigpa German Phonetics**. Also by Roger Espel Llima. |
| CGI backend | `thlib.org/cgi-bin/thl/lbow/phonetics.pl` (Wayback; also on old/dev/beta hosts) | — | The Perl service behind the converters. |

**Relevance:** the phconverter's four-way word-splitting UI is a proven UX pattern for our Phonetics pane; its "built-in word list" mirrors what the essay says about word-boundary automation. The Rigpa multi-language phonetic modes are prior art if ALL ever needs non-English phonetics.

---

## 4. Tibetan fonts THL distributes/documents

Primary source: THL Toolbox "Tibetan Fonts" page (Wayback 2017 of `thlib.org/tools/scripts/wiki/Tibetan+Fonts.html`) + per-font pages.

### 4.1 Tibetan Machine Uni (TMU) — THL's flagship Unicode font
- OpenType, Unicode, dbu can, with full chos-skad Sanskrit stack support; for Tibetan, Dzongkha, Ladakhi. ~4,000 glyphs generating >20,000 combinations (stats block: 3540 glyphs alpha, 5109 version 1; latest noted version 1.901).
- Lineage: based on Tony Duff's Tibetan Machine font; **rights purchased from Duff by the Trace Foundation to free it under the GPL**; OpenType tables + >2,000 glyphs added by Nathaniel Garson (Grove) and Christopher Fynn for THL. (Page also records a public dispute: Duff criticized THL's modifications; THL posted a rebuttal notice.)
- **License, verbatim from the THL page:** "The Tibetan Machine Uni font is licensed under the terms of the GNU General Public License… The Tibetan Machine Uni font is Copyright © 2000, by Tony Duff; portions Copyright © 2004 by the Tibetan and Himalayan Library. This font is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version." (Stats block adds: "Portions Copyright 2007 Tibetan and Himalayan Library (THL)"; License: gnu.org/copyleft/gpl.html.) Distributed from THL and SourceForge (`thdltools`), incl. source files and a VOLT version.
- **Caution for ALL:** GPLv2+ font — fine to bundle as a *font file* alongside a proprietary/other-licensed app (fonts as data), but document it in OPEN_SOURCE_NOTICES and ship the GPL text; do not subset/modify without releasing the modified font under GPL.

### 4.2 Jomolhari / Jomolhari ID (Chris Fynn)
- OpenType TTF, dbu can, Bhutanese-manuscript style; supports Unicode Tibetan AND part A of the Chinese precomposed-Tibetan encoding (GB/T 20524-2006). Version documented: 000.003c (2006); Jomolhari ID a0.003d (2009).
- **License (as stated on the THL page):** Copyright © 2006 Christopher J. Fynn; "License: SIL Open Font License". (Fonts table lists Jomolhari as "Open" and Jomolhari ID as "OFL".)

### 4.3 Fonts table (THL Toolbox "Tibetan Fonts" page, 2017 state — as published)

| Font | Platform | Format | Script | Designer/Developer | License/Availability |
|---|---|---|---|---|---|
| CTRC-Betsu / CTRC-Drutsa / CTRC-Uchen / CTRC-Tsumachu | Windows, Linux | OpenType | Petsuk / Drutsa / Uchen / Tsukmakhyuk | China Tibetology Research Center – Tashi Tsering | Open; free download |
| Drepung | Macintosh | — | dbu can | Xenotype | Closed; commercial |
| Jomolhari | Win, Linux | OpenType | dbu can | Chris Fynn | Open; free (2006) |
| Jomolhari ID | Win, Mac, Linux | OpenType | dbu can | Chris Fynn | **OFL**; free (2009) |
| Kailasa / Kokonor | Macintosh | — | dbu can | Shojiro Nomura, Steve Hartwell | ships with Mac OS X 10.5 |
| OSX Tibetan | Macintosh | AAT | dbu can | Xenotype | Closed; commercial |
| Qomolangma (family) | Mac, Win, Linux | OpenType | multiple scripts | yalasoo | Open; free (2012). New site links the family at `github.com/AI-Bod/Qomolangma-Tibetan-Unicode-Fonts` |
| Sambhota Ededris | Win, Linux | OpenType | dbu can | Nitartha | (unstated) |
| Terton Sogyal Uni | unknown | unknown | — | — | (unstated) |
| **Tibetan Machine Uni** | Win, Linux | OpenType | dbu can | Tony Duff / Than Garson, Chris Fynn | **Open-GPL**; free (2008) |
| UTibetan | Win, Linux | TrueType | dbu can | Pierre Robillard / Grisha Mokhin | (unstated) |
| XenoType TB HorYig / sPungsChen / Tibetan New / Woodblock | Macintosh | — | ornamental / dbu can | Xenotype | Closed; commercial |
| Yagpo | Win, Linux | OpenType | dbu can | — | Open; free |

Legacy (pre-Unicode) fonts are documented separately ("Legacy Tibetan Font Systems", "Tabular Survey of Legacy Tibetan Fonts" wiki pages — includes Tibetan Machine, Tibetan Machine Web/TMW, Sambhota, LTibetan, TibetBT, etc.). Not captured in detail; GAP if ever needed (Wayback has them, URLs in §8 inventory).

**New-site font pointers (live, 2026):** the rebuilt reference page points to Qomolangma (GitHub above), Nitartha Sambhota fonts, "Digital Tibetan" (`digitaltibetan.github.io/DigitalTibetan/`) and its "Tibetan on Digital Devices" page — THL now largely defers font/device guidance to those.

---

## 5. Input/keyboard tools & Unicode guidance

The THL Toolbox wiki (mirrored under `thlib.org/tools/scripts/wiki/…`, live on old.thlib.org, archived on Wayback) is a full handbook of Tibetan computing circa 2008–2018. The **"Tibetan Input Tools" overview page's body is JS-loaded and captured empty ("closed") in every snapshot tried — PARTIAL**; the per-tool pages exist as static wiki pages. Inventory found (page names verbatim from the wiki):

- **Input systems/keyboards:** THL EWTS Unicode keyboard (named as the companion to TMU on Windows), Tise, Tsampa Keyboard, Dzongkha Unicode Keyboard, Keyman, "Legacy Tibetan Input Methods for Windows", "Installing a Tibetan System Keyboard in Windows XP", per-OS pages (Tibetan Input Tools for Windows / Macintosh / Linux / Browsers-or-Cross-Platform), "Tibetan Input System Principles".
- **Editors/tools:** Jskad Editor (THDL's Java Tibetan editor, part of `thdltools`), WylieWord (Word macro system; v1.0 & 2+ docs), UDP ("Converting to Tibetan Unicode with UDP"), ATTU, Babelpad, Wylie2uni, "TB Youtso converter to unicode", Universal Tibetan Font Converter, "Tabular Survey of Converters & Reverters for Tibetan", "Recommended Programs for Specific Conversions-Reversions involving Tibetan Fonts", "APIs For Converting Tibetan Strings".
- **Unicode guidance:** "Encoding Model of the Tibetan Script in the UCS", "Table of Tibetan Characters in the UCS", "Deprecated Tibetan Characters", "Tibetan Character Encoding Proposals", "Documentation of Tibetan Script Glyph by Glyph", "Sorting Tibetan", "MySQL Tibetan Collation", "Tibetan CSS", "Updating Uniscribe for Tibetan in Windows 2000 & XP", "Tibetan Unicode in Windows Vista", "Using Tibetan in 64-bit Windows Systems", "Problems with Tibetan in Windows", "Troubleshooting Problems with Tibetan Unicode", "Unicode Tibetan Use", "Unicode Tibetan for Developers Only", per-app pages (Microsoft Word, OpenOffice Writer/Calc/Impress, Mellel, Nisus Writer Pro, TextEdit, Firefox, IE, LaTeX, Java applications, instant messaging in Tibetan Unicode).
- **Transliteration-scheme documentation set** (Toolbox → "Transliteration & Transcription Schemes"): THL EWTS, THL Simplified Phonetics, **ACIP Transliteration of Tibetan**, Chinese Partial Wylie, LOC Transliteration of Tibetan, Hopkins Essay Phonetics, Ethnic Pinyin of Tibetan, plus Chinese/Hindi/Mongolian/Nepali/Sanskrit-and-Prakrit transliteration pages.
  - The ACIP page itself is thin: one paragraph noting ACIP was developed by the Asian Classics Input Project for its monastery-based input work in India, "loosely based on Wylie, but with considerable differences," and used only within that project. (No conversion chart on that page — our own `hgm_tools.py` acip_to_ewts remains the working reference.)
  - The umbrella essay "Tibetan Transliteration and Transcription" (captured) gives THL's problem statement: 30 consonants + 4 vowel marks, four horizontal slots, vertical stacking, syllable-only punctuation, ten-century-old orthography vs modern divergent dialect pronunciation — and positions EWTS (spelling-preserving) + Simplified Phonetics (pronunciation) as the two recommended systems.

**Relevance:** mostly historical; the durable items for us are the EWTS keyboard concept (a Convert-pane affordance: type EWTS, see Unicode live — already our model), the collation/sorting pages if the Library pane ever needs Tibetan-alphabet sort, and the scheme-comparison pages as documentation fodder.

---

## 6. Dictionaries & the Tibetan-to-English Translation Tool (licensing focus)

### 6.1 The live tool
`https://ttt.thlib.org/` — "The Online Tibetan to English Dictionary and Translation Tool", **version 4.0.1 compiled on 1-July-2026. Copyright © 2000-2025 by Andrés Montano. All rights reserved.** Display options: Tibetan script (Tibetan Machine Uni) or roman; search scopes All/English/Tibetan/Sanskrit/Custom.

**Dictionaries in the live tool (16, exactly as listed in its UI):**
1. Term ids (TID)
2. Jeffrey Hopkins (hopkins)
3. tshig mdzod chen mo (bod rgya)
4. Dan Martin (martin)
5. Rangjung Yeshe (rangjung)
6. Ives Waldo (waldo)
7. Jim Valby (valby)
8. Richard Barron (barron)
9. Thupten Phuntsok (phuntsok)
10. dag yig gsar bsgrigs (dag yig)
11. dung dkar tshig mdzod (dung dkar)
12. mkhas grub ming mdzod (ming mdzod)
13. smon lam tshig mdzod (smon lam)
14. Negi Sanskrit (negi)
15. Lokesh Chandra (lokesh)  ← note: LC is in THL's stack too
16. Yogācāra Sanskrit (yogacara)

### 6.2 Source code and data
- GitHub: **`https://github.com/tibetan-nlp/ttt`** (linked from the live tool itself). Java. Contains the offline dictionary database files **`thl.def`, `thl.dic`, `thl.wrd`** — i.e., the exact format of the files we already hold in `~/Tibetan Translation Tool` (thl.wrd/thl.def) and parse in our Lookup pane. Also contains the TMU font under `/Fonts/TibetanMachineUni`.
- Older home: SourceForge project `thdltools` (Jskad, Translation Tool, TiblEdit, fonts).

### 6.3 Licenses found (verbatim identifications)
- The `ttt` repo's `/license` folder contains: `Readme.txt`, `amp_license.txt` (**AMP Open Community License Version 1.0**), `opl_license.txt` (Open Public License), `thdl_license.txt` (**THDL Open Community License Version 1.0**), `thdl_rationale.html`. Per the Readme: **THDL Open Community License v1.0 covers most code; AMP Open Community License v1.0 covers the package `org.thdl.scanner`** (the segmentation engine!). The THL Toolbox page adds: "The AMP Open Community License is a free software license. The source code for our software is completely open and public… The core of our license is the Open Public License (OPL), which is also used by THL and Enhydra. The OPL is a slightly modified version of the popular Mozilla Public License."
- THDL-era download page: "All tools available from this site are protected by the terms of the THDL Open Community License, Version 1.0."
- Site footer everywhere on the legacy site: "THL Community License © [year] www.thlib.org". THL's copyright-policy wiki page ("Copyrights in THL", captured from Wayback 2011) states the general regime: contributors keep copyright, THL holds a non-exclusive permanent display/use right; THL-staff-created works belong to THL; reuse limitations are set per object.
- **GAP / red flag for our shipping pane:** none of the above states the license of the *dictionary content* per contributing dictionary (Hopkins, Rangjung Yeshe, tshig mdzod chen mo, etc.). The one page that would ("Dictionaries Available" / "Dictionaries Download", `collab.its.virginia.edu/wiki/toolbox/Dictionaries Available.html`) is **unreachable and never archived** (collab.its.virginia.edu is offline; Wayback has the parent pages but not that one). The code licenses (THDL/AMP) do NOT automatically cover the lexical data. Known context from the tool's FAQ (captured): the Rangjung Yeshe dictionary was also sold commercially (Dharma Dictionary 3.0 CD bundled the Translation Tool), implying RY content is *not* freely redistributable in general. **Action for ALL: before any redistribution of thl.wrd/thl.def content beyond in-house use, either (a) inspect per-dictionary headers in the data files, (b) ask THL (thdltools@virginia.edu historically; thlib.org/contact now), or (c) ship only lookups against user-supplied dictionary files.** Our current pane already treats THL data as "extract, per-dictionary licensing review" (CLAUDE.md) — this research confirms that caution is warranted and adds the concrete contact path and repo to check.

### 6.4 THL's newer dictionary properties
- **THL Terms** (`https://thlib.org/terms/` SPA; legacy `dictionary.thlib.org` Rails app archived) — the "comprehensive historical dictionary"; new reference page also lists THL Places (gazetteer, 21k+ entries) and THL Subjects.
- The old "Tibetan Historical Dictionary" project pages (dictionary project overview, editorial manual, "Preparing Dictionary Entries Offline for Importation") survive on the collab-wiki mirrors/Wayback.
- External dictionaries THL now recommends (new reference page, live): Dharma Dictionary (rywiki.tsadra.org), Dzogchen Wiki, Esukhia, Monlam Dictionary, Rigpa Shedra, Steinert Online Tibetan Dictionary, Digital Dictionaries of South Asia (incl. Jäschke-era Nepali/Newari works), Monier-Williams and Edgerton BHS at Cologne (sanskrit-lexicon.uni-koeln.de), Negi via DSAL, Monlam AI (MT/OCR/TTS at monlam.ai).

---

## 7. Tibetan language learning materials

- **THL's own "Tibetan Grammar" reference section was never built**: the nav link `/reference/tibetan-grammar/` 404s on the live legacy server AND in the only Wayback capture (2009). The "Education" section similarly never launched (commented out of the old site's nav; `/education/` 404s). So THL itself published no grammar essays/verb tables/honorific guides under those headings — do not go hunting for them there.
- What THL actually offers learners:
  - The transliteration/phonetics essays themselves (§§1–2) — THL's de facto orthography curriculum.
  - "Teaching EWTS" by Alexandru Anton-Luca — linked as an additional resource from the EWTS essay (hosted in the collab wiki; collab host now dead — GAP, likely recoverable via Wayback if wanted).
  - The Translation Tool as a reading aid (its docs explicitly frame it as help for beginners with word segmentation).
  - The new reference page's curated external list (live, 2026): **Rangjung Yeshe Institute** (Kathmandu), **Esukhia**, **Lotsawa Rinchen Zangpo Translator Program**, **School for International Training (Nepal)**, **Loplao**, **Tibetan Language Institute** — plus Monlam AI text-to-speech for pronunciation.
- Related THL reference assets that support language study: Tibetan Calendars converters, Treasury of Lives biographies, THL Places/Subjects/Terms — catalogued on the new reference page (see sibling dossiers `knowledge_infrastructure.md`, `collections_catalogs.md`).

---

## 8. DEAD / UNREACHABLE list (as of 2026-08-12)

| URL | Status | Recovery route used / available |
|---|---|---|
| `https://www.thlib.org/tools/` | 404 on new site | old.thlib.org + Wayback (used) |
| `https://www.thlib.org/reference/transliteration/` (and all `#!essay=` URLs on www) | 404 on new site | **old.thlib.org serves everything live** (essay_reader.php, converters, tibstacks.php); Wayback `?_escaped_fragment_=` captures (used) |
| `https://old.thlib.org/*` | ALIVE but intermittent 503s | retry with delay works |
| Phonetics essay section 13 "Notes" | exists but empty (live-verified) | not a loss — 2007 full edition confirms essay ends at List of Exceptions |
| EWTS PUA glyphs U+F021–F042 | images only, no alt text | Wayback images `web.archive.org/web/2020*im_/http://texts.thlib.org/essays/thl/ewts/UF0xx.jpg` (not fetched) |
| `collab.its.virginia.edu` (THL Toolbox source wiki) | DNS dead | Wayback has most pages; used for Translation Tool + Dictionaries pages |
| `collab.its.virginia.edu/wiki/toolbox/Dictionaries Available.html` | dead AND never archived | **GAP — the per-dictionary licensing page.** Alternatives: `github.com/tibetan-nlp/ttt` data-file headers; ask THL |
| `https://ttt.thlib.org/` | ALIVE (one 503 observed; retry OK) | — |
| `http://www.thlib.org/reference/tibetan-grammar/` | 404 always (2009→now) | never existed with content |
| `https://old.thlib.org/education/…` | 404 | never launched |
| `texts.mandala.library.virginia.edu` (book reader + text pages) | returns empty body to curl (JS SPA) | content mirrored the EWTS essay; not needed (Wayback capture is complete) |
| `http://www.digitaltibetan.org/cgi-bin/phonetics.pl` (Espel Llima original) | not probed | modern successor: digitaltibetan.github.io |
| THL Toolbox wiki page "Tibetan Input Tools" (overview body) | JS-loaded, empty in every capture | per-tool pages exist individually on Wayback (inventory in §5) |
| `dictionary.thlib.org` | dead (Rails app; asset captures only) | replaced by thlib.org/terms SPA |

---

## 9. Raw capture provenance (for re-verification)

All captures were made 2026-08-12 into the session scratchpad; key sources:
- EWTS whole document: `web.archive.org/web/20200301125415/http://www.thlib.org/reference/transliteration/?_escaped_fragment_=essay=/thl/ewts/all/` (312,930 bytes)
- Phonetics sections: `web.archive.org/web/2020*/http://www.thlib.org/global/php/essay_reader.php?url=/thl/phonetics/s/b{1..11}`; b12 from 2024-05-20 capture; b13 live from old.thlib.org
- THDL 2003/2007 edition: `web.archive.org/web/20070315033720/http://www.thdl.org/xml/showEssay.php?xml=/collections/langling/THDL_phonetics.xml&m=all`
- Stacks: live `https://old.thlib.org/reference/transliteration/tibstacks.php`
- Fonts/TMU/Jomolhari/ACIP/converter/overview wiki pages: Wayback 2010/2017 of `thlib.org/tools/scripts/wiki/*`
- Translation Tool docs: Wayback of `collab.its.virginia.edu/wiki/toolbox/Tibetan Translation Tool.html`; live `ttt.thlib.org`; `github.com/tibetan-nlp/ttt`
- THL copyright policy: Wayback 2011-02-15 of `thlib.org/reference/transliteration/about/wiki/copyrights.html`
