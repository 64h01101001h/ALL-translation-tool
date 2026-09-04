#!/usr/bin/env python3
"""No depth-7 member may repeat its parent's whole text.

A member is meant to mark a TIGHTER boundary inside a compound: `lam` and
`gtso` inside `lam gtso`. One whose text IS the parent's entire text draws
a box around a box. It passes containment and it passes uniqueness, so the
generator accepted it for the whole campaign -- and it banks the same
(tibetan, english) pair twice, once at depth 5 and once at depth 7, which
is noise in the dictionary view.

Found 2026-08-28 by a reconciler that wrote its own validator and applied
a check the generator did not have. Eleven had shipped across five pages.
"""
import re, io, os, glob, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAT = re.compile(r'<span class="u[^"]*" data-d="5" data-l="(s\d+\w+)">'
                 r'<span class="u[^"]*" data-d="7" data-l="(s\d+\w+)">'
                 r'([^<]*)</span></span>')


def main():
    bad, n_pages = [], 0
    for d in ("pages_c01", "pages", "pages_c03", "pages_c04"):
        for f in sorted(glob.glob(os.path.join(ROOT, "data", "alignment",
                                               d, "*.html"))):
            n_pages += 1
            for m in PAT.finditer(io.open(f, encoding="utf-8").read()):
                bad.append((os.path.basename(f), m.group(1), m.group(2),
                            m.group(3)[:40]))
    if bad:
        print("DEGENERATE MEMBERS (a depth-7 span repeating its parent): %d"
              % len(bad))
        for b in bad:
            print("  %-16s %-10s wraps %-10s %r" % b)
        print("\nA member must mark a tighter boundary than its parent.")
        print("Drop the member and keep the depth-5 span.")
        sys.exit(1)
    print("no degenerate members across %d pages" % n_pages)


if __name__ == "__main__":
    main()
