#!/usr/bin/env python3
"""alignment_page_dirs.py — the one list of page directories.

WHY THIS EXISTS. Every gate over the alignment pages carried its own hardcoded
list of directories, and every one of those lists was written before Course 5
had a directory. Measured 2026-09-17, after 66 C05 pages had been landed over
several weeks:

    build_alignment_layer.py        c01 c03 c04 c05      <- the builder knew
    test_builder_sees_every_span    c01 c03 c04          <- blind to C05
    test_no_broken_words            c01 c03 c04          <- blind to C05
    test_no_degenerate_members      c01 c03 c04          <- blind to C05
    test_no_split_syllables         c01 c03 c04          <- blind to C05
    detect_overcapture              c01 c03 c04          <- blind to C05
    detect_ordinal_displacement     c01 c03 c04          <- blind to C05

Six gates saw none of Course 5, and said so in a way that reads as complete:
"across 456 pages" — which is exactly pages + c01 + c03 + c04, with the 66 C05
pages absent. The number was right about what it counted and silent about what
it did not. NOT RUN IS NOT PASSED.

The builder knew because someone remembered to add C05 to it. The gates did
not, because nothing makes forgetting fail. So: discover the directories, do
not declare them. A new course drops in a directory and every gate picks it up
without anyone remembering anything.
"""
import glob
import io
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALIGN = os.path.join(ROOT, "data", "alignment")


def page_dirs(root=None):
    """Every alignment page directory, as bare names, sorted.

    'pages' is the C02 directory for historical reasons; the rest are
    pages_c<NN>. Sorted so 'pages' leads and the courses follow in order.
    """
    base = os.path.join(root, "data", "alignment") if root else ALIGN
    out = []
    for p in sorted(glob.glob(os.path.join(base, "pages*"))):
        if os.path.isdir(p) and glob.glob(os.path.join(p, "*.html")):
            out.append(os.path.basename(p))
    return out


def page_files(root=None):
    """Every page html file under every page directory."""
    base = os.path.join(root, "data", "alignment") if root else ALIGN
    files = []
    for d in page_dirs(root):
        files.extend(sorted(glob.glob(os.path.join(base, d, "*.html"))))
    return files


# THE GATE ASKS THE OUTCOME, NOT THE SOURCE. The first version scanned the
# tools for a hardcoded "pages_cNN" literal and immediately cried wolf on four
# lines that were all legitimate -- a frozen ratchet baseline keyed by
# directory, and two places deriving a course code FROM a discovered directory
# name. A gate that fires on correct code gets switched off, which is the
# lesson from the contrast gate (docs, 2026-09-15).
#
# So this runs the page gates and checks what they actually covered. Each one
# reports "across N pages" and N must equal the number of pages that exist. A
# gate that quietly looks at four directories out of five cannot report the
# right total, whatever its source looks like.
COVERAGE_GATES = [
    "test_builder_sees_every_span.py",
    "test_no_broken_words.py",
    "test_no_degenerate_members.py",
    "test_no_split_syllables.py",
]


def gate():
    import re as _re
    import subprocess
    want = len(page_files())
    ds = page_dirs()
    print("alignment page directories discovered: %d (%d pages)"
          % (len(ds), want))
    for d in ds:
        print("   %-14s %4d" % (d, len(glob.glob(os.path.join(ALIGN, d, "*.html")))))
    bad = []
    print()
    for g in COVERAGE_GATES:
        r = subprocess.run([_sysexe(), os.path.join(ROOT, "tools", g)],
                           capture_output=True, text=True)
        m = _re.search(r"across (\d+) pages", r.stdout)
        got = int(m.group(1)) if m else None
        ok = got == want
        print("   %-34s covered %s of %d  %s"
              % (g, got if got is not None else "?", want,
                 "" if ok else "  <-- BLIND"))
        if not ok:
            bad.append((g, got))
    if bad:
        print("\nFAILED: %d gate(s) do not cover every page." % len(bad))
        print("This is how six gates analysed NONE of Course 5 for weeks while")
        print("reporting 'across 456 pages' -- exactly pages + c01 + c03 + c04,")
        print("with the 66 C05 pages absent. Use page_dirs() to iterate.")
        return 1
    print("\n  every page gate covers every page directory")
    return 0


def _sysexe():
    import sys
    return sys.executable


if __name__ == "__main__":
    import sys as _s
    if "--gate" in _s.argv:
        _s.exit(gate())
    ds = page_dirs()
    print("alignment page directories discovered: %d" % len(ds))
    for d in ds:
        n = len(glob.glob(os.path.join(ALIGN, d, "*.html")))
        print("   %-14s %4d pages" % (d, n))
    print("   %-14s %4d pages total" % ("", len(page_files())))
