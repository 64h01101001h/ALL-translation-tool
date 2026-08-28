#!/usr/bin/env python3
"""No English span may be cut out of the middle of a word, undeclared.

Geshe Michael's English is banked verbatim into what will become the
dictionary. A span that stops mid-word banks a fragment: `gong ma gong
ma'i rgyu` was recorded as "are caus" for weeks, and `rtse gcig` as
"one-pointed" out of "one-pointedly". Neither is English, and no gate
noticed -- the page still proved byte-exact against the spine, because
the CHARACTERS were all correct. Only the span boundary was wrong.

There IS a legitimate sub-word convention, used on 14 spans in C01: the
Tibetan negation maps to the "n't" of "doesn't" and the "un" of
"unnecessary", so the positive stem is spanned on its own. Those are
listed below by name. Anything else is an accident.

Run over every page in the project on every build.
"""
import re, io, os, glob, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LET = re.compile(r"[A-Za-z]")
SPAN = re.compile(r'<span class="u[^"]*" data-d="\d" data-l="(s\d+\w+)"[^>]*>'
                  r'([^<]*)</span>')

# The negative-affix convention: the Tibetan negation owns the English
# negative, so the positive stem is spanned separately and sits inside a
# larger word on purpose. Keyed by the span text, which is what makes it
# recognisable; adding to this list is a deliberate act.
# The list has two halves. The STEM half is spanned when the Tibetan
# negation owns the English negative affix: "necessary" out of
# "unnecessary", "changing" out of "unchanging". The NEGATIVE half is the
# affix itself, spanned when the Tibetan negation is a compound member:
# C03:209 has `mi nus` -> "unable" with the member `mi` -> "un".
# Both are the same convention seen from opposite ends.
ALLOWED_SUBWORD = {
    # stem half — the positive word, negation lives elsewhere
    "n't", "necessary", "sufficient", "changing", "not",
    # negative half — the affix itself, owned by mi / ma
    "un",
}


def eng_blocks(page):
    out = []
    for chunk in page.split('<div class="eng">')[1:]:
        for stop in ('<div class="pair">', '<div class="note"',
                     '<div class="legend"', '<h2'):
            i = chunk.find(stop)
            if i >= 0:
                chunk = chunk[:i]
        out.append(chunk)
    return out


def main():
    bad = []
    n_pages = n_spans = 0
    for d in ("pages_c01", "pages", "pages_c03"):
        for f in sorted(glob.glob(os.path.join(ROOT, "data", "alignment",
                                               d, "*.html"))):
            n_pages += 1
            page = io.open(f, encoding="utf-8").read()
            for chunk in eng_blocks(page):
                for m in SPAN.finditer(chunk):
                    t = m.group(2)
                    if not t:
                        continue
                    n_spans += 1
                    if t in ALLOWED_SUBWORD:
                        continue
                    nxt = chunk[m.end():m.end() + 1]
                    prv = chunk[m.start() - 1:m.start()]
                    if LET.match(t[-1]) and nxt and LET.match(nxt):
                        bad.append((os.path.basename(f), m.group(1), t,
                                    "runs on into %r" % chunk[m.end():m.end() + 6]))
                    elif LET.match(t[0]) and prv and LET.match(prv):
                        bad.append((os.path.basename(f), m.group(1), t,
                                    "prefixed by %r" % prv))
    if bad:
        print("BROKEN WORDS in English spans: %d" % len(bad))
        for b in bad:
            print("  %-16s %-10s %-30r %s" % b)
        print("\nIf one of these is the deliberate negative-affix convention,")
        print("add its span text to ALLOWED_SUBWORD with a reason. Otherwise")
        print("fix the span boundary on the page.")
        sys.exit(1)
    print("no broken words: %d English spans across %d pages" % (n_spans, n_pages))


if __name__ == "__main__":
    main()
