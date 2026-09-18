#!/usr/bin/env python3
"""The three distribution manual parts are a SPLIT of the canonical manual.

data/help/USER_MANUAL.md is canonical -- app/main.cpp loadChapters() reads it
into the Help window. docs/distribution/manual_part{1,2,3}_*.md exist so the
manual can be handed out in three readable pieces, and they are exactly its
three `# PART` sections. Nothing derives them, so nothing kept them.

WHAT WAS FOUND WHEN THIS TOOL WAS WRITTEN (2026-09-18). The three parts had
not been touched since 22 August and the manual had moved under them: 31 diff
hunks in part one alone. They were still advertising a smaller app -- no
keyboard reading controls, no THL pronunciation mode, no 84000 glossary, no
Das/Jaschke page links, "reference dictionaries (LC/TD/THL)" where the app now
carries LC/TD/THL/OT/IW -- and part three told the reader to open "Input ->
Scan", a pane that has been called Input -> OCR for weeks. 1,996 lines of
outward-facing documentation describing a version of the tool that no longer
exists, with no check that could have said so.

The press already gates the OTHER copy of the manual, because those two
drifted 58 lines in a day and somebody noticed (package_macos.sh step 0b).
The parts are the same shape with nobody watching. They are generated now, and
test_generated_docs_in_sync.py compares them to a fresh generation.

Usage: gen_manual_parts.py [--out-dir DIR] [--check]
"""
import argparse
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CANON = os.path.join(ROOT, "data", "help", "USER_MANUAL.md")
PARTS = ["manual_part1_read_translate.md",
         "manual_part2_research_learn.md",
         "manual_part3_input_community_app.md"]


def sections():
    """The manual's `# PART` sections, verbatim, in order."""
    text = io.open(CANON, encoding="utf-8").read()
    lines = text.split("\n")
    heads = [i for i, l in enumerate(lines) if l.startswith("# PART ")]
    if len(heads) != len(PARTS):
        sys.exit("REFUSED: %s has %d `# PART` headings, expected %d — the "
                 "split changed and the file names have not. Fix PARTS in "
                 "this tool in the same commit."
                 % (os.path.relpath(CANON, ROOT), len(heads), len(PARTS)))
    bounds = heads + [len(lines)]
    out = []
    for k in range(len(PARTS)):
        body = lines[bounds[k]:bounds[k + 1]]
        while body and not body[-1].strip():
            body.pop()
        out.append("\n".join(body) + "\n")
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out-dir", default=os.path.join(ROOT, "docs",
                                                      "distribution"))
    ap.add_argument("--check", action="store_true",
                    help="exit 1 if a committed part differs; write nothing")
    a = ap.parse_args()
    bad = 0
    for name, body in zip(PARTS, sections()):
        p = os.path.join(a.out_dir, name)
        old = io.open(p, encoding="utf-8").read() if os.path.exists(p) else None
        if a.check:
            if old != body:
                bad += 1
                print("DRIFTED docs/distribution/%s" % name)
            continue
        io.open(p, "w", encoding="utf-8").write(body)
        print("%-40s %6d bytes%s" % (name, len(body),
                                     "" if old == body else "  (updated)"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
