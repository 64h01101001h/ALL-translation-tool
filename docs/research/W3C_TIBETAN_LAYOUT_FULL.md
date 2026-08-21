# W3C / r12a Tibetan layout — full-document pass (2026-08-14)

*The digest (via DigitalTibetan) is superseded by this direct
pass. Finding: w3.org/TR/tlreq is a stub; the substance lives in
Richard Ishida's Tibetan Orthography Notes
(r12a.github.io/scripts/tibt/bo.html), which W3C's gap analysis
treats as the source.*

## Our eight digest rules: all CONFIRMED, two refined

The bare-ka/ga shad-drop carries an exception — **ka/ga with a
subjoined consonant keeps its shad** (གྲུ།) — which our lint's
adjacency implementation already honored (selftest-pinned now).
Justification is richer than letter-spacing (see N1/N2).

## Newly implemented in the Typography check (this press)

- **Tsa-'phru placement** (U+0F39 immediately after its
  consonant, never after subjoins/vowels).
- **Stack order**: subjoined a-chung U+0F71 precedes vowel signs.
- **Yig-mgo grammar**: ༄ opens a text/folio/headline only —
  mid-line occurrences flag.

## Banked for the pecha renderer (v4 lane, TODO'd)

- **N1 tsheg-padding justification** (the dominant traditional
  method): line ending in tsheg justifies by repeating tshegs to
  the margin; letter-spacing is the secondary method.
- **N2 double-shad-gap justification**: ། ། lines justify by
  stretching the inter-shad gap (rendered at 1/4–1/3 space width
  normally), never by tsheg padding.
- **N3 rin chen spungs shad**: a lone wrapped final syllable's །
  becomes ༑ (double shad: first only, ༑ ། — recompute per
  reflow).
- **N4/N6 break hygiene**: no break after the swallowed-shad
  space (must be NBSP); a break taken after ། discards the
  following space.
- **N5 sbrul shad ༈**: ~three NBSPs each side, unbreakable unit,
  never line-initial.
- Also noted: breaks additionally allowed after ༴ U+0F34 and
  ྾ U+0FBE; bracketed ༼num༽ groups kept unbreakable; U+0F35
  emphasis centering; half-digit forms (value − ½).
