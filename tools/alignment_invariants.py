#!/usr/bin/env python3
"""Invariants over the banked alignment layer — the checks that need no judgement.

WHY THIS EXISTS. On 2026-09-15 the Codex alignment campaign reported four
inherited C01 mappings as wrong. Two turned out to be real and two did not,
and finding that out took a person reading whole segments. That is the right
way to settle a question of MEANING. But one of the two real ones —
`mi` -> "trans" and `'da' bar` -> "gressing", one English word split across two
links — is not a question of meaning at all. "gressing" appears in that
segment's English only inside "transgressing", never at a word boundary, and
no reading is required to know that.

So this tool holds the line between the two. Every check here is EXACT: it
compares the banked layer against the spine and against the layer's own spec,
and it asks only questions a string comparison can answer. Nothing in here
judges whether a rendering is good.

Two detectors were tried first and rejected, recorded so they are not tried
again:

  "English shorter than N letters"     391 hits, almost all correct
                                       ('de la -> "Now", 'dris pa -> "met").
                                       Short is not the fault.

  "English not in /usr/share/dict"     2,259 hits at 10.9%, roughly nine in
                                       ten false: the system word list has no
                                       inflected forms, so "teachings",
                                       "imparted", "Buddhas" and "notes" all
                                       look like non-words.

The word-boundary check below finds the same class those were groping at,
exactly, and with no word list.

usage: alignment_invariants.py [--bank F] [--spine F] [--samples N] [--strict]
       --strict exits non-zero if any EXACT check has a finding.
"""
import argparse
import io
import json
import os
import re
import sqlite3
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BANK = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")

# Links whose English is one of these alone are reported ADVISORY, not exact:
# an article can be a legitimate exponent in some constructions.
def is_pronunciation_link(r):
    """y-links are d=7 phonetic renderings (sa gzhi -> "sashi"), not English.
    Every English-containment check must skip them or it reports the whole
    chanted-prayer layer as invented English."""
    return bool(re.match(r"^s\d+y\d+$", str(r.get("id") or "")))


def eng_parts(e):
    """A discontinuous span is banked with an ellipsis: 'Idam ... Idam'.
    Each part must be checked on its own."""
    return [p.strip() for p in str(e).split("\u2026") if p.strip()]


BARE_FUNCTION = {
    "a", "an", "the", "of", "to", "in", "on", "at", "by", "for", "and", "or",
    "is", "are", "was", "were", "be",
}


def word_occurrences(hay, needle):
    """Offsets where `needle` sits in `hay` on BOTH sides at a word boundary."""
    out = []
    if not needle:
        return out
    start = 0
    while True:
        i = hay.find(needle, start)
        if i < 0:
            return out
        before_ok = i == 0 or not hay[i - 1].isalpha()
        j = i + len(needle)
        after_ok = j >= len(hay) or not hay[j].isalpha()
        if before_ok and after_ok:
            out.append(i)
        start = i + 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bank", default=BANK)
    ap.add_argument("--spine", default=SPINE)
    ap.add_argument("--samples", type=int, default=6)
    ap.add_argument("--strict", action="store_true")
    ap.add_argument("--baseline",
                    default=os.path.join(ROOT, "tools",
                                         "alignment_invariants_baseline.txt"))
    a = ap.parse_args()

    bank = json.load(io.open(a.bank, encoding="utf-8"))
    links = bank.get("links", [])
    db = sqlite3.connect(a.spine)
    seg_text = {}
    for course, seq, eng, acip, wylie in db.execute(
            "select course, seq, english, acip, wylie from corpus_segments"):
        seg_text[(course, seq)] = (eng or "", acip or "", wylie or "")

    by_seg = defaultdict(list)
    for r in links:
        by_seg[(r.get("course"), r.get("seg"))].append(r)

    # findings[name] = (is_exact, description, [sample lines])
    findings = {}

    def record(name, exact, desc, hits):
        findings[name] = (exact, desc, hits)

    # ---- 1. English cut mid-word ------------------------------------------
    # The trans/gressing class. A link's English occurs in the segment's
    # English, but NEVER at a word boundary on both sides — so the layer has
    # claimed a piece of a word as if it were a rendering.
    cut = []
    for key, rows in by_seg.items():
        eng = seg_text.get(key, ("", "", ""))[0]
        if not eng:
            continue
        for r in rows:
            if is_pronunciation_link(r):
                continue
            # A BOUND MORPHEME is supposed to land inside a word: the spec's
            # own design maps mi -> "im" in "impossible" and ma -> "n't" in
            # "isn't". Only a d=5 WORD link cutting a word in half is a fault,
            # which is exactly what 'da' bar -> "gressing" is.
            if (r.get("d") or 0) != 5:
                continue
            e = (r.get("eng") or "").strip()
            if not e or " " in e:
                continue
            if e not in eng:
                continue                      # handled by check 2
            if not word_occurrences(eng, e):
                cut.append("%s:%s %-10s %-22r -> %r" % (
                    key[0], key[1], r.get("id"), (r.get("tib") or "")[:20], e))
    # ADVISORY, and the reason is worth stating. mi -> "im" with srid ->
    # "possible" composes "impossible" and is the spec's intended design; mi ->
    # "trans" with 'da' bar -> "gressing" composes "transgressing" and is a
    # fault, because "trans-" is not a negation. The two are IDENTICAL to a
    # string comparison. What separates them is whether the morpheme's English
    # is a plausible exponent of the Tibetan, and that is a reading. So this
    # check hands a person a short list instead of pretending to rule.
    record("english_cut_mid_word", False,
           "a d5 word's English lands inside a longer English word — the other "
           "half is usually a bound morpheme and usually CORRECT (mi/srid -> "
           "im/possible). Review for the case where the morpheme's English is "
           "not a real exponent (mi/'da' bar -> trans/gressing, E-183)",
           cut)

    # ---- 2. English not in the segment at all -----------------------------
    # Rule 5: no invented English. If the banked rendering is not a substring
    # of what Geshe Michael actually wrote for this segment, it came from
    # somewhere else.
    absent, whitespace = [], []
    for key, rows in by_seg.items():
        eng = seg_text.get(key, ("", "", ""))[0]
        if not eng:
            continue
        for r in rows:
            if is_pronunciation_link(r):
                continue
            e = (r.get("eng") or "").strip()
            if not e:
                continue
            if all(p in eng for p in eng_parts(e)):
                continue
            # Known and already registered: the layer normalises the source's
            # broken hyphenation, so it banks "single-pointed concentration"
            # where the spine reads "single- pointed". A Rule 1 breach, not
            # invented English, and it has its own register entry. Separate it
            # so the serious bucket stays serious.
            # Compare on LETTERS AND DIGITS ONLY. Everything the layer differs
            # by below that line is punctuation and spacing: it banks
            # 'grasping "self"' where the source reads 'grasping "self,"', and
            # 'single-pointed' where the source reads 'single- pointed'. That
            # is a Rule 1 verbatim breach with its own errata entry, not
            # invented English, and mixing the two would bury the serious case.
            def letters(x):
                return re.sub(r"[^0-9a-z]", "", x.lower())
            flat = letters(eng)
            if all(letters(p) and letters(p) in flat for p in eng_parts(e)):
                whitespace.append("%s:%s %-10s %r" % (
                    key[0], key[1], r.get("id"), e[:50]))
                continue
            absent.append("%s:%s %-10s %r" % (
                key[0], key[1], r.get("id"), e[:56]))
    record("english_not_in_segment", True,
           "the banked English is not in the segment's English even ignoring "
           "whitespace — genuinely invented (spec rule 5)", absent)
    record("english_differs_only_in_punctuation", False,
           "banked English matches the segment on letters and digits but not "
           "on punctuation or spacing — the source's broken hyphenation and "
           "quote placement, already in the errata register", whitespace)

    # ---- 3. Tibetan not in the segment ------------------------------------
    miscited = []
    for key, rows in by_seg.items():
        _, acip, wylie = seg_text.get(key, ("", "", ""))
        if not (acip or wylie):
            continue
        for r in rows:
            t = (r.get("tib") or "").strip()
            if not t:
                continue
            def bare(x):
                return re.sub(r"[^a-z']", "", x.lower())
            if all(bare(p) and bare(p) not in bare(wylie) and
                   bare(p) not in bare(acip) for p in eng_parts(t)):
                miscited.append("%s:%s %-10s %r" % (
                    key[0], key[1], r.get("id"), t[:44]))
    record("tibetan_not_in_segment", True,
           "the banked Tibetan span is not in the segment's source — "
           "mis-cited or mis-segmented", miscited)

    # ---- 4. Duplicate ids within a segment --------------------------------
    dupes = []
    for key, rows in by_seg.items():
        seen = set()
        for r in rows:
            i = r.get("id")
            if i in seen:
                dupes.append("%s:%s %s" % (key[0], key[1], i))
            seen.add(i)
    record("duplicate_ids", True, "two links in one segment share an id", dupes)

    # ---- 5. Empty-string English ------------------------------------------
    # The spec distinguishes eng: null (no exponent, affirmative) from a
    # rendering. An empty STRING is neither, and reads as a rendering of
    # nothing.
    empties = []
    for key, rows in by_seg.items():
        for r in rows:
            if r.get("eng") is not None and not str(r["eng"]).strip():
                empties.append("%s:%s %s" % (key[0], key[1], r.get("id")))
    record("empty_string_english", True,
           "English is an empty string rather than null — the spec's way of "
           "saying 'no exponent' is null", empties)

    # ---- 6. Member English outside its parent's ---------------------------
    # A d>=6 member sits under a d=5 word. Its English must lie inside that
    # word's English, or the nesting is claiming something the layer does not
    # support.
    outside = []
    for key, rows in by_seg.items():
        words = [r for r in rows if r.get("d") == 5 and (r.get("eng") or "").strip()]
        for r in rows:
            if (r.get("d") or 0) < 6:
                continue
            e = (r.get("eng") or "").strip()
            t = (r.get("tib") or "").strip()
            if not e or not t:
                continue
            if is_pronunciation_link(r):
                continue
            parents = [w for w in words if t and t in (w.get("tib") or "")]
            if len(parents) == 1 and not any(e in (w.get("eng") or "")
                                             for w in parents):
                outside.append("%s:%s %-10s %r not inside its d5 parent" % (
                    key[0], key[1], r.get("id"), e[:36]))
    # ADVISORY: parentage is inferred from Tibetan containment, which picks the
    # wrong parent whenever a short morpheme occurs in several spans. A real
    # check needs the layer's own parent pointers, which these records do not
    # carry.
    record("member_english_outside_parent", False,
           "a d>=6 member's English is not inside the English of the d5 word "
           "whose Tibetan contains it", outside)

    # ---- 7. ADVISORY: a bare function word for a content span -------------
    # 'ong rgyu -> "a" is this shape. Advisory because an article genuinely is
    # the exponent in some constructions; a person has to look.
    bare = []
    for key, rows in by_seg.items():
        for r in rows:
            e = (r.get("eng") or "").strip().lower().strip('.,;:"“”')
            t = (r.get("tib") or "").strip()
            if e in BARE_FUNCTION and len(t.split()) >= 2:
                bare.append("%s:%s %-10s %-20r -> %r" % (
                    key[0], key[1], r.get("id"), t[:18], e))
    record("bare_function_word_for_phrase", False,
           "a multi-word Tibetan span rendered by a single English function "
           "word ('ong rgyu -> \"a\") — ADVISORY, an article can be right",
           bare)

    # ---- report -----------------------------------------------------------
    print("alignment invariants — %d links across %d segments\n"
          % (len(links), len(by_seg)))
    exact_total = 0
    for name, (exact, desc, hits) in findings.items():
        tag = "EXACT   " if exact else "ADVISORY"
        print("%s %-32s %6d" % (tag, name, len(hits)))
        print("         %s" % desc)
        for h in hits[:a.samples]:
            print("           %s" % h)
        if len(hits) > a.samples:
            print("           ... and %d more" % (len(hits) - a.samples))
        print()
        if exact:
            exact_total += len(hits)

    print("exact-check findings: %d" % exact_total)
    if not a.strict:
        return 0

    # A BASELINE, not a zero. The findings standing today are filed errata
    # (E-187 and its C01:45 twin: an editorial annotation stored in the eng
    # field). Failing on their existence would leave the build permanently red
    # and teach everyone to ignore it. Failing on an INCREASE catches the next
    # one on the day it lands. The number may go down freely -- lower it here
    # when it does, in the same commit, the way DATA-3 handles the G2 census.
    allowed = 0
    try:
        allowed = int(io.open(a.baseline, encoding="utf-8").read().strip())
    except Exception:
        pass
    if exact_total > allowed:
        print("\nFAILED: exact findings %d exceed the banked baseline %d.\n"
              "  Something new is wrong with the layer. Read the EXACT blocks\n"
              "  above, file what is real in the errata register, and bump\n"
              "  %s in the same commit only when the finding is understood."
              % (exact_total, allowed, os.path.relpath(a.baseline, ROOT)))
        return 1
    if exact_total < allowed:
        print("\nThe baseline is now loose: %d findings against a banked %d.\n"
              "  Lower %s to %d so the gate keeps its grip."
              % (exact_total, allowed, os.path.relpath(a.baseline, ROOT),
                 exact_total))
    return 0


if __name__ == "__main__":
    sys.exit(main())
