# ACIP Standards for Simplified Tibetan and Sanskrit Pronunciation
*(banked 2026-08-06 from the organization's own reference: ACIP Release IV
manual, "A Thousand Books of Wisdom", pp. 173–178 — drawn from the ACI New
York document included in the ACIP database at R0060. This is the standards
document the sanskrit/pronunciation work was previously gated on.)*

## Principles
- Simplified pronunciation lets an uninitiated English speaker approximate
  the sound using ONLY normal written English — no diacritics.
- Pronunciation is distinct from transliteration; macrons etc. are
  meaningless to non-specialists and are not used.

## Consonants (ACIP input code → simplified pronunciation)
- K→k (kawa), KH→k (Tsongkapa), G main→g (gawa), G prenasal→ng (renga for
  RE 'GA'), G suffix→k (Gelukpa), GH→gh (Maghada), NG→ng / ambiguous
  syllable-initial → -ng (ka-ngel for DKA' NGAL)
- C→ch (chungse), CH→ch (chu for CHOS), J→j / prenasal→nj (tanjuk for
  MTHA' MJUG), NY→ny / ambiguous → -ny (cha-nyam)
- retroflex t/th/d/dh/n → t/t/d/dh/n (tikchen, kota, damaru, uttarashadha,
  panchen)
- T→t (ten for GTAN), TH→t (tarpa), D main→d (den) / prenasal→nd (Ganden),
  DH→dh (sindhura), N→n (ne for GNAS, len for LAN)
- P→p (pawo), PH→p (pentok), B main→b (bardo) / prenasal→mb (chumbep for
  CHU 'BEBS) / suffix→p (raptu tsawa), BH→bh (Sambhota), M→m (menpa, rimpa)
- TZ→ts (tsukpu for GTZUG PHUD), TS→ts (Tsarchen), DZ→dz (dze for RDZAS) /
  prenasal→ndz (sandzin for GZA' 'DZIN)
- W→w, ZH→sh (shenpa), Z→s (sampa), '→vowel alone (oma for 'O MA); ' as
  suffix in diphthong → subsequent vowel alone except I (yinpao for YIN
  PA'O); ' + genitive 'I → ay (lamay tuk for BLA MA'I THUGS)
- Y→y (yarwa for G-YAR BA), R→r, L→l, SH→sh (shepa), S→s (sipa),
  H→h (hla for LHA), A→vowel alone

## Vowels
- A→a, but before an umlauting suffix → e (rangwang; lenpa for LAN PA)
- I→i (yi for YID) · U→u (lu for LUS) · E→e (leppa for SLEB PA)
- O→o, but before an umlauting suffix → u (gongpa; **sunam for BSOD NAMS**)
- **Umlauting suffixes: D N R L S.**

## Validation against our canonical engine (2026-08-06)
All 60 example pairs from the chart were run through the canonical
`pron_engine` (ACIP → EWTS → pronounce, whitespace/hyphen-normalized):

- **60 total: 53 exact.**
- **Pure-Tibetan examples: 51/52.** The single difference: YIN PA'O — the
  standard gives *yinpao* (diphthong rule), the engine gives *yinpa*.
- The other 6 misses are all Sanskrit-in-Tibetan-letters (MAGHADA, t'IK
  CHEN, AUTTA RA shA dhA, PAn CHEN, SINDHU RA, SAM BHO tA): the engine is a
  Tibetan-phonetics engine and thins Sanskrit aspirates/stacks; the standard
  expects Sanskrit letters to pass through romanized.

Per founding rule 2 the canonical engine is untouched; these are documented
reference findings. The Sanskrit-side simplified style used by
`sanskritLetterGuide`/`iastToPronunciation` (no macrons, c→ch, ṣ→sh) is
CONSISTENT with this standard and is no longer "provisional pending the
standards doc" — the doc is this one.

Reproduce the measurement:
```bash
python3 - <<'EOF'
import sys; sys.path.insert(0,'engines')
from pron_engine import pronounce; from hgm_tools import acip_to_ewts
print(pronounce(acip_to_ewts("BSOD NAMS")))   # sunam
EOF
```
*(full 60-pair list lives in this file's history / the session log; a C++
battery H SHIPPED 2026-08-14: core/tests/pron_standard_smoke.cpp — 32 explicit standard pairs, each pre-validated against the canonical Python engine).*
