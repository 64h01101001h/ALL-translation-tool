#!/usr/bin/env python3
"""The builder must harvest EVERY span the pages contain.

This is the defect that motivated the suite. build_alignment_layer.py matched
spans with

    <span class="u" data-d="..." data-l="...">

requiring the class attribute to be exactly "u". But the generator writes the
analyst's grammar label into that same attribute — "u noun", "u verb-nominal",
"u compound member, noun" — so every LABELLED span was invisible to the
builder and its analysis was discarded without a word. The pages rendered
those labels correctly the whole time, so nothing looked wrong anywhere.

Measured when found on 2026-08-28: 1,260 spans across 141 of 320 pages, 2.7%
of everything analysed, and 77-80% of the three newest C03 pages, whose
analysts happened to label heavily. Two of those pages had already been
registered and reported as landed.

The class of bug is "a parser silently accepts less than it is given", and no
gate could see it, because every OTHER gate reads the layer — the artifact
that had already lost the data. This one compares the layer against the PAGES,
which is the only place the loss is visible.

The invariant: for every page, the number of spans the builder's own pattern
matches must equal the number of alignment spans actually present. Not
approximately. A parser that drops 2.7% of its input silently is not a parser.

WHAT THIS GATE DOES NOT CATCH, stated so nobody trusts it further than it
goes. It imports the builder's REAL pattern rather than a copy, so narrowing
that pattern trips it immediately - reverting to the historical class="u"
reports 1,247 spans dropped across 130 pages. But it compares that pattern
against a second one written here, and if BOTH were wrong in the same way -
if the generator started emitting a shape neither anticipated - the counts
would agree and the gate would pass. It protects against the builder drifting
away from the pages, not against the pages drifting away from both.
"""
import glob, io, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import build_alignment_layer as B   # noqa: E402  -- the REAL pattern, not a copy

# Deliberately permissive: any span carrying data-d and data-l is an alignment
# unit, whatever its class list says. If the builder's own SPAN pattern matches
# fewer of these than exist, it is dropping analysis.
PRESENT = re.compile(r'<span class="u[^"]*" data-d="\d" data-l="[^"]+"\s*>')

# SPAN was the one that broke, but it is not the only pattern the builder
# parses pages with, and the other two carry the identical fragility: NOTE and
# TREE also pin the class attribute exactly. They are clean today - 921 notes
# and 11 trees, none dropped, verified rather than assumed - and they are
# checked here so that stays true. The bug class is "a parser silently accepts
# less than it is given", and closing it for one pattern while leaving two
# open would be closing it for none.
LOOSE = {
    'note': re.compile(r'<div class="note[^"]*"[^>]*>'),
    'tree': re.compile(r'<div class="tree[^"]*"[^>]*>'),
}

def main():
    dirs = ['pages', 'pages_c01', 'pages_c03']
    seen = present = npages = apparatus = 0
    other = {'note': 0, 'tree': 0}
    bad = []
    for d in dirs:
        for f in sorted(glob.glob(os.path.join(ROOT, 'data/alignment', d, '*.html'))):
            s = io.open(f, encoding='utf-8').read()
            # APPARATUS spans - typo queue, supplied-sentence flags,
            # editorial notes - are excluded ON PURPOSE and counted out loud.
            # They are not alignment pairs and must never reach the dictionary.
            ap = len(B.APPARATUS.findall(s))
            a, b = len(B.SPAN.findall(s)), len(PRESENT.findall(s)) - ap
            seen += a
            present += b
            apparatus += ap
            npages += 1
            if a != b:
                bad.append((os.path.basename(f), 'span', a, b, b - a))
            for name, loose in LOOSE.items():
                pat = B.NOTE if name == 'note' else B.TREE
                x, y = len(pat.findall(s)), len(loose.findall(s))
                other[name] += x
                if x != y:
                    bad.append((os.path.basename(f), name, x, y, y - x))
    if bad:
        bad.sort(key=lambda x: -abs(x[4]))
        print('FAIL the builder does not see everything the pages contain: '
              '%d page/pattern pairs differ' % len(bad))
        for f, what, x, y, d_ in bad[:12]:
            print('      %-16s %-5s builder sees %4d of %4d  (drops %d)'
                  % (f, what, x, y, d_))
        return 1
    print('builder sees everything: %d spans, %d notes, %d trees across %d '
          'pages, none dropped; %d apparatus spans excluded on purpose (typo '
          'queue, supplied-sentence flags, editorial notes - never dictionary '
          'content)' % (seen, other['note'], other['tree'], npages, apparatus))
    return 0

if __name__ == '__main__':
    sys.exit(main())
