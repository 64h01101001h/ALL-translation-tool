#!/usr/bin/env python3
"""Pins for build_alignment_layer's text extraction.

The builder reads the generated pages and banks what it finds as dictionary
headwords. If its tag-stripping alters the text, it banks Tibetan strings
that do not exist -- silently, because every other gate checks the PAGE
against the spine, and the page is fine. The corruption happens after.

That is exactly what occurred on 2026-08-28: strip_tags replaced each tag
with a space, so a depth-7 member splitting `dbu ma'i` into `dbu ma` + `'i`
banked the headword as "dbu ma 'i". It was caught only because the ACIP
round-trip could not convert it -- an accident, not a gate.
"""
import os, sys, re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402

fails = []


def check(name, got, want):
    if got != want:
        fails.append("%s\n     got  %r\n     want %r" % (name, got, want))
    else:
        print("  ok   %s" % name)


print("build_alignment_layer text extraction:")

# 1. A nested span that splits a word MUST NOT insert a space. This is the
#    2026-08-28 defect. MUTATION: restore `" "` as the tag replacement ->
#    this pin fails.
check("a nested span splitting a word leaves no gap",
      B.strip_tags('dbu ma<span class="u" data-d="7" data-l="s1y1">\'i</span>'),
      "dbu ma'i")

check("a nested member inside a compound leaves no gap",
      B.strip_tags('<span class="u" data-d="5" data-l="s1w1">'
                   'de lta bu<span class="u" data-d="7" data-l="s1y1">s'
                   '</span></span>'),
      "de lta bus")

# 2. Punctuation adjacent to a span must not gain a space before it. This
#    one was corrupting a real headword ("the Sakya , Geluk ,") before the
#    fix, on a page nobody had touched in weeks.
check("punctuation after a span keeps no space",
      B.strip_tags('<span class="u" data-d="5" data-l="s1w1">Sakya</span>, '
                   '<span class="u" data-d="5" data-l="s1w2">Geluk</span>,'),
      "Sakya, Geluk,")

# 3. Real spaces BETWEEN sibling spans survive -- they are in the source
#    text, not supplied by the markup. If tag-stripping ate them, every
#    multi-word headword would run together.
check("a real space between siblings survives",
      B.strip_tags('<span class="u" data-d="5" data-l="s1w1">sems</span> '
                   '<span class="u" data-d="5" data-l="s1w2">gnas</span>'),
      "sems gnas")

# 4. Entities are unescaped, so an apostrophe in the Tibetan comes back as
#    itself rather than as &#x27;.
check("entities decode",
      B.strip_tags("<span>&#x27;gro don &amp; &quot;x&quot;</span>"),
      "'gro don & \"x\"")

if fails:
    print("\nFAILED:")
    for f in fails:
        print("  - " + f)
    sys.exit(1)
print("build_alignment_layer: extraction is faithful")
