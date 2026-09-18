#!/usr/bin/env python3
"""Ratchet gate: word-level English spans must not begin with a supplied
function word (the/a/an/and/or/his/our/your/I/you) unless the licensing
Tibetan word is named.

Rule 7 of docs/ALIGNMENT_LAYER_SPEC.md says an article or hinge GMR supplied
stays UNWRAPPED. The 2026-09-04 C03-stratified audit
(docs/AUDIT_2026-09-04_C03_STRATIFIED.md) confirmed 19 defects in 100 pairs;
eight were exactly this: a one-word article or hinge wrapped at d=5. The
same scan across the shipped layer found the practice is not rare:

    pages_c01  1,640 of 9,168 d=5 English spans (17.9%) begin this way
    pages         49 of   680                  ( 7.2%)
    pages_c03    577 of 8,192                  ( 7.0%)  + 36 at d=7

Some of those ARE licensed (a demonstrative `de`, a possessive `rang gi`,
a genitive), so a hard zero would be wrong and a repair of 2,300 spans is
not this gate's job. This gate is a RATCHET:

  * for the three shipped directories the count may not rise above the
    frozen baseline below (repairs lower it; the gate is re-frozen by hand
    when they land, in the same commit, with the new number in the ledger);
  * for any other pages directory (C04 onward) the count must be zero,
    except spans listed in data/alignment/span_head_allow.json with the
    Tibetan word that licenses the function word - the reconciler names it.

Counts d=5 and d=7 only. Depth 1-4 spans are sentences/clauses/phrases and
legitimately begin with articles.
"""
import re, io, os, glob, sys, json

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALIGN = os.path.join(ROOT, "data", "alignment")
ALLOW = os.path.join(ALIGN, "span_head_allow.json")
# Ratchets, TIGHTENED 2026-09-17 to the measurement after the sogs class
# licensor landed: pages 50->48, c01 1643->1631, c03 603->578. Those 39
# spans were sogs all along and had been counted against the ratchet.
# Leaving the old numbers would have left 39 spans of dead room below the
# gate -- the exact shape found twice elsewhere today (a battery floor set
# under its own measured value, a ceiling twenty-five times above it).
# A ratchet is only a ratchet if it sits ON the measurement.
BASELINE = {"pages_c01": 1631, "pages": 47, "pages_c03": 578}   # frozen 2026-09-04; c03 613->603 after the audit repairs the same day
# 2026-09-18: "pages" was 48 here and MEASURED 47 -- one unit of silent
# regression room, set by me in 1990ee5b, the commit whose whole subject
# was this gate being blind. data/alignment/pages/ has not changed since,
# and a worktree at that commit measures 47 too, so the slack was there
# from the moment it was written. A ratchet frozen above its measurement
# is the defect the audit found in forward_battery (floor 99.95% over a
# measured 100.000%) and in no_phonetics_in_layer (2% ceiling over 0.08%).
# The SLACK CHECK below exists so the next one says so instead of waiting
# to be noticed.
# CLASS LICENSOR, added 2026-09-17. 20 of the 24 pages_c05 entries in
# span_head_allow.json said the identical thing: `sogs` / `la sogs pa` owns
# its closing gesture -- "and the rest", "and so on", "and the like", "or the
# like", "or other". Twenty near-identical entries are not twenty rulings;
# they are one ruling, re-typed, and re-typing a ruling is how rubber-stamping
# starts. The Tibetan word IS the licensor here and always will be: the "and"
# or "or" is not supplied English at all, it is what the etc.-particle means.
#
# This is safe because it is CHECKED, not assumed. Each span id appears twice
# on a page, once per column, so the gate looks up the TIBETAN side of the
# very same id and licenses the span only when that side is the sogs family.
# A span that merely begins with "and" gets no free pass.
SOGS = re.compile(r'^(?:la\s+)?sogs(?:\s+pa)?$')

# SECOND CLASS LICENSOR, 2026-09-17. Widening FN to catch bare function-word
# spans surfaced 73 of them, and READING them is what made the fix usable:
# most are a pronoun rendering a pronoun, or the disjunctive particle
# rendering "or". `bdag`/`bdag gis`/`ngas`/`nga` -> "I" (20 spans),
# `khyod`/`khyed`/`rang nyid`/`rang` -> "you" (12), `'am` -> "or" (4),
# `kho bos`/`rang gi` -> "our". That is the word's own meaning, not English
# the translator supplied -- the same argument already accepted per-span for
# `bdag cag gi` -> "our" at c5p61.
#
# DELIBERATELY NARROW. It licenses only pronouns and the disjunctive, and
# only when the English is the matching pronoun or "or". It does NOT license
# "the", "a" or "his", because that is exactly where the real defects sit:
# `lam` -> "the", `lugs` -> "the", `tshe` -> "the", `ston pa` -> "his".
# Licensing by class must never be licensing by convenience.
PRONOUN = re.compile(
    r"^(?:bdag|nga|khyod|khyed|rang|kho bo|bdag cag|'o skol|rang re|rang nyid)"
    r"(?:\s+(?:cag|nyid|re|phyogs))?(?:\s*(?:'i|s|gi|gis|gyi|gyis|kyi|kyis))?$")
PRONOUN_ENG = {"I", "you", "You", "our", "Our", "your", "Your",
               "my", "My", "we", "We"}
DISJUNCT = re.compile(r"^(?:&#x27;|')?am$")


def licensed_by_class(tibs, eng):
    """The Tibetan word itself is the licensor, checked against the span's
    own other column -- never assumed from the English alone."""
    for t in tibs:
        if SOGS.match(t):
            return True
        if DISJUNCT.match(t) and eng.strip() in ("or", "Or"):
            return True
        if PRONOUN.match(t) and eng.strip() in PRONOUN_ENG:
            return True
    return False
ID = re.compile(r'data-l="(s\d+\w+)"[^>]*>([^<]*)')

# The trailing space was a blind spot, found 2026-09-17 by auditing the gate
# against the question "can it fail?". It required a space AFTER the function
# word, so a span whose English is EXACTLY the function word -- `>the</span>`
# -- did not match. That is not an edge case; it is the PUREST form of the
# defect this gate exists to catch, a d=5 span carrying nothing but an article
# the translator supplied. 73 such spans were invisible across the pages.
#
# Reading them is what makes the fix safe rather than noisy: most are correct.
# `bdag`/`bdag gis`/`ngas`/`nga` -> "I" (20), `khyod`/`khyed`/`rang nyid` ->
# "you" (12), `'am` -> "or" (4), `dang bcas pa` -> "and", `gcig` -> "a",
# `rang gi` -> "our". A pronoun rendering a pronoun is the word's own meaning,
# not supplied English. A handful are not: `lam` -> "the", `lugs` -> "the",
# `tshe` -> "the", `ston pa` -> "his". Those are named in
# docs/SPAN_HEAD_BARE_REVIEW.md for a reader; they are in shipped C01/C03
# pages, which the campaign ratchets rather than re-cuts.
#
# (?=[ <]) matches the function word followed by a space OR by the element
# close, which is the whole point.
FN = r'(?:the|The|a|A|an|An|and|And|or|Or|his|His|our|Our|your|Your|I|you|You)(?=[ <])'
PAT = re.compile(r'<span class="u[^"]*" data-d="([57])" data-l="(s\d+\w+)">'
                 r'(?:<span class="u[^"]*" data-d="7" data-l="s\d+\w+">)?' + FN)


def main():
    allow = json.load(io.open(ALLOW, encoding="utf-8")) if os.path.exists(ALLOW) else {}
    fail = False
    dirs = sorted(d for d in os.listdir(ALIGN)
                  if os.path.isdir(os.path.join(ALIGN, d)) and d.startswith("pages"))
    for d in dirs:
        hits = []
        for f in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            page = os.path.basename(f)[:-5]
            html = io.open(f, encoding="utf-8").read()
            # every text this page carries under each span id, both columns
            texts = {}
            for i, t in ID.findall(html):
                texts.setdefault(i, []).append(t.strip())
            for m in PAT.finditer(html):
                key = "%s/%s" % (page, m.group(2))
                if key in allow.get(d, {}):
                    continue
                tibs = texts.get(m.group(2), [])
                if licensed_by_class(tibs, m.group(0).split(">")[-1]):
                    continue          # class licensor: see above
                hits.append((page, m.group(2), m.group(1)))
        if d in BASELINE:
            if len(hits) > BASELINE[d]:
                fail = True
                print("SUPPLIED SPAN HEAD ratchet broken in %s: %d > baseline %d"
                      % (d, len(hits), BASELINE[d]))
            elif len(hits) < BASELINE[d]:
                # A baseline above its measurement is regression room nobody
                # granted. Say it, and say it as a FAILURE: a note would be
                # read as passing, and this one sat unread for a day.
                fail = True
                print("SUPPLIED SPAN HEAD baseline has SLACK in %s: measured "
                      "%d, baseline %d. Tighten BASELINE to %d in a commit — "
                      "%d span(s) could regress in silence."
                      % (d, len(hits), BASELINE[d], len(hits),
                         BASELINE[d] - len(hits)))
            else:
                print("%-10s %4d function-word-initial d5/d7 spans (baseline %d)"
                      % (d, len(hits), BASELINE[d]))
        else:
            if hits:
                fail = True
                print("SUPPLIED SPAN HEAD in %s: %d spans begin with an article or"
                      " hinge and name no licensor:" % (d, len(hits)))
                for h in hits[:40]:
                    print("  %s %s d=%s" % h)
                print("Either trim the span (rule 7) or add it to "
                      "data/alignment/span_head_allow.json under %r with the "
                      "Tibetan word that licenses the function word." % d)
            else:
                print("%-10s    0 function-word-initial d5/d7 spans" % d)
    sys.exit(1 if fail else 0)


if __name__ == "__main__":
    main()
