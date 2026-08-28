#!/usr/bin/env python3
"""No depth-7 member may be cut out of the middle of a Tibetan syllable.

A member span is exempt from the word-boundary rule by design -- a member
IS a sub-part of its parent -- so nothing else catches it when the member
lands on the wrong occurrence inside its parent.

C03:186 has the parent `'phror` with the terminative member `r`. The
search took the FIRST r and rendered `'ph<r>or`, splitting a syllable in
two on a page that had already shipped. It was found only because a
denser batch exposed the generator bug behind it.

This gate walks the rendered Tibetan of every page and reports any span
whose closing tag falls between two letters of the same syllable, unless
what follows is a legitimate Wylie suffix the campaign splits on purpose.
"""
import re, io, os, glob, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LET = re.compile(r"[A-Za-z]")
SPAN = re.compile(r'<span class="u[^"]*" data-d="(\d)" data-l="(s\d+\w+)"[^>]*>'
                  r'([^<]*)</span>')

# Wylie morphology the campaign splits deliberately: a syllable may be cut
# before one of these and that is correct, not damage.
#   s      the agentive / ergative and the past-stem -s
#   'i     the genitive after a vowel
#   i      the genitive after a syllable already closed with an apostrophe
#   'o     the assertive, 'am / 'ang the disjunctive and inclusive
#   r      the terminative after a vowel
#   ng     nasal finals split from their stem
SUFFIX = {"s", "'i", "i", "'o", "'am", "'ang", "r", "ng", "'u"}


def main():
    bad = []
    n_pages = n_spans = 0
    for d in ("pages_c01", "pages", "pages_c03"):
        for f in sorted(glob.glob(os.path.join(ROOT, "data", "alignment",
                                               d, "*.html"))):
            n_pages += 1
            page = io.open(f, encoding="utf-8").read()
            for chunk in page.split('<div class="tib">')[1:]:
                i = chunk.find('<div class="eng">')
                if i >= 0:
                    chunk = chunk[:i]
                for m in SPAN.finditer(chunk):
                    t = m.group(3)
                    if not t:
                        continue
                    n_spans += 1
                    nxt = chunk[m.end():m.end() + 8]
                    if not (nxt and LET.match(nxt[0])):
                        continue
                    run = re.match(r"[A-Za-z']+", nxt)
                    tail = run.group(0) if run else ""
                    if tail in SUFFIX:
                        continue
                    bad.append((os.path.basename(f), m.group(2), t[-24:], tail))
    if bad:
        print("SPLIT SYLLABLES in Tibetan spans: %d" % len(bad))
        for b in bad:
            print("  %-16s %-10s %-26r followed by %r" % b)
        print("\nA span may only stop mid-syllable before a real Wylie suffix.")
        print("If this is one, add it to SUFFIX with a reason. Otherwise the")
        print("member landed on the wrong occurrence -- drop it or widen the")
        print("parent, and regenerate the page.")
        sys.exit(1)
    print("no split syllables: %d Tibetan spans across %d pages"
          % (n_spans, n_pages))


if __name__ == "__main__":
    main()
