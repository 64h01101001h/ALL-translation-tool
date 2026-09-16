#!/usr/bin/env python3
"""build_punchlist.py — re-emit the mechanical half of the release punchlist.

docs/RELEASE_PUNCHLIST.md was built on 2026-09-11 by reading every unchecked
item in the three source lists and classifying each by what stands in its way.
Good work, and the counts were worth trusting that day. Then 139 commits landed
before anyone looked at it again, and by 2026-09-16 the page described a tree
that no longer existed — an item it listed as release-blocking and open
(TODO.md:3057) had been marked DONE in the source.

It drifted because it could not be refreshed: there was no generator, so the
only way to update it was to commission the whole reading again. That is the
BUILD-7 shape from docs/FIXTURES.md wearing different clothes — a number nobody
can re-derive is a number nobody can check.

WHAT THIS DOES AND DOES NOT DO.

It does the mechanical half: find every open item in the sources, carry its
CURRENT file and line, and key it by a hash of its own text so the key survives
the file being re-ordered around it.

It does NOT invent classifications. Which section an item belongs in, whether it
is release-blocking, how big it is, and who it is waiting on are readings, and
they live in docs/punchlist_classification.json keyed by item. An item with no
entry there is emitted under "Not yet classified" rather than guessed at or
dropped — so a new item surfaces instead of vanishing, which is the failure
this whole exercise is about.

THE OLD PAGE'S CLASSIFICATIONS COULD NOT BE MIGRATED, and the reason is worth
recording. Its rows are the surveyor's SUMMARIES, not source text — "Red-flag
licence list" appears nowhere in TODO.md — and each row's `file:line` points at
whatever line supported the summary, not at the item's first line. Of 191 rows,
23 still resolve to something that looks like an open item. Matching the rest
programmatically would be guesswork dressed as data, so the classifications are
rebuilt by the re-survey instead, and written here as it goes.

Usage:  python3 tools/build_punchlist.py            # report, write nothing
        python3 tools/build_punchlist.py --emit     # rewrite the page
"""
import hashlib
import io
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCES = ["TODO.md", "docs/BACKLOG.md", "docs/OPEN_TASKS_2026-09-10.md"]
CLASS = os.path.join(ROOT, "docs", "punchlist_classification.json")
PAGE = os.path.join(ROOT, "docs", "RELEASE_PUNCHLIST.md")

SECTIONS = [
    ("blocking-mine", "Release-blocking, and mine to do"),
    ("blocking-adam", "Release-blocking, and Adam's to do"),
    ("code", "Code, unblocked, not release-blocking"),
    ("other", "Everything else unblocked (docs, data, process)"),
    ("adam", "Waiting on Adam"),
    ("outside", "Waiting on someone outside"),
]


def key_of(text):
    """Identity is the item's own words, so re-ordering a file does not
    orphan its classification and editing an item asks for a fresh reading."""
    return hashlib.sha256(" ".join(text.split()).encode("utf-8")).hexdigest()[:16]


def first_sentence(text, cap=150):
    t = " ".join(text.split())
    t = re.sub(r"\*\*|__|`", "", t)
    m = re.search(r"(?<=[.!?])\s", t)
    if m and m.start() < cap:
        t = t[:m.start() + 1]
    return t[:cap].rstrip() + ("…" if len(t) > cap else "")


def items():
    """Every OPEN item in the three sources, with where it lives now."""
    out = []

    # TODO.md — "- [ ] item", body continues until the next checkbox or heading
    p = os.path.join(ROOT, "TODO.md")
    lines = io.open(p, encoding="utf-8").read().split("\n")
    for i, line in enumerate(lines):
        if not re.match(r"\s*[-*] \[ \]", line):
            continue
        body = [re.sub(r"^\s*[-*] \[ \]\s*", "", line)]
        for nxt in lines[i + 1:]:
            if re.match(r"\s*[-*] \[[ xX]\]", nxt) or nxt.startswith("#"):
                break
            body.append(nxt)
        out.append(("TODO.md", i + 1, "\n".join(body).strip()))

    # BACKLOG.md — each "## " heading is one back-burner topic; "###" are its
    # own subsections and are part of it, not separate items.
    p = os.path.join(ROOT, "docs/BACKLOG.md")
    lines = io.open(p, encoding="utf-8").read().split("\n")
    starts = [i for i, l in enumerate(lines) if l.startswith("## ")]
    for n, i in enumerate(starts):
        end = starts[n + 1] if n + 1 < len(starts) else len(lines)
        out.append(("docs/BACKLOG.md", i + 1,
                    "\n".join(lines[i:end]).strip()))

    # OPEN_TASKS — "## N — title" sections, and their "- **a. …**" sub-items
    p = os.path.join(ROOT, "docs/OPEN_TASKS_2026-09-10.md")
    lines = io.open(p, encoding="utf-8").read().split("\n")
    for i, line in enumerate(lines):
        if re.match(r"##\s+\d+\s+—", line):
            body = [line]
            for nxt in lines[i + 1:]:
                if nxt.startswith("## "):
                    break
                body.append(nxt)
            txt = "\n".join(body).strip()
            if "DONE" not in txt.split("\n")[0].upper():
                out.append(("docs/OPEN_TASKS_2026-09-10.md", i + 1, txt))
    return out


def main():
    found = items()
    cls = {}
    if os.path.exists(CLASS):
        cls = json.load(io.open(CLASS, encoding="utf-8"))

    rows = []
    for src, line, text in found:
        k = key_of(text)
        rows.append({"key": k, "source": src, "line": line,
                     "summary": first_sentence(text),
                     "cls": cls.get(k)})
    classified = [r for r in rows if r["cls"]]
    unclassified = [r for r in rows if not r["cls"]]
    orphans = [k for k in cls if k not in {r["key"] for r in rows}]

    print("punchlist: %d open item(s) across %d source(s)"
          % (len(rows), len(SOURCES)))
    for s in SOURCES:
        print("    %-34s %d" % (s, sum(1 for r in rows if r["source"] == s)))
    print("  classified: %d    not yet classified: %d" %
          (len(classified), len(unclassified)))
    if orphans:
        print("  %d classification(s) no longer match any open item — the item "
              "was closed or its wording changed:" % len(orphans))
        for k in orphans[:5]:
            print("      %s  %s" % (k, str(cls[k].get("summary", ""))[:70]))

    if "--emit" not in sys.argv:
        print("\n  (report only — pass --emit to rewrite the page)")
        return 0

    buf = []
    buf.append("# The release punchlist\n")
    buf.append("*Generated by `python3 tools/build_punchlist.py --emit`. The "
               "open items and their source lines are extracted; the "
               "classification of each is a reading, kept in "
               "`docs/punchlist_classification.json` and keyed by the item's "
               "own words.*\n")
    buf.append("> **Adam, 2026-09-16: come back to this and make sure every "
               "loose end is tied up.** Standing instruction, not a dated "
               "task.\n")
    buf.append("The reading that produced the previous version of this page is "
               "kept at `docs/RELEASE_PUNCHLIST_2026-09-11.md`. Its "
               "classifications are the input to the re-survey that fills in "
               "the section below; they are not carried across "
               "automatically, because its rows are summaries whose source "
               "pointers have drifted and matching them by guess would be "
               "guesswork dressed as data.\n")
    buf.append("**%d open items.** %d classified, %d awaiting a reading.\n"
               % (len(rows), len(classified), len(unclassified)))
    for sec_key, title in SECTIONS:
        got = [r for r in classified if r["cls"].get("section") == sec_key]
        if not got:
            continue
        buf.append("\n## %s\n" % title)
        buf.append("| | item | effort | source |")
        buf.append("|---|---|---|---|")
        for n, r in enumerate(got, 1):
            flag = "🚢 " if r["cls"].get("blocking") else ""
            buf.append("| %d | %s%s | %s | `%s:%d` |"
                       % (n, flag, r["cls"].get("summary") or r["summary"],
                          r["cls"].get("effort", ""), r["source"], r["line"]))
    if unclassified:
        buf.append("\n## Not yet classified\n")
        buf.append("*Extracted from the sources and not yet read. They are "
                   "here rather than absent: an item that vanishes because "
                   "nobody classified it is the failure this page exists to "
                   "prevent.*\n")
        buf.append("| | item | source |")
        buf.append("|---|---|---|")
        for n, r in enumerate(unclassified, 1):
            buf.append("| %d | %s | `%s:%d` |"
                       % (n, r["summary"], r["source"], r["line"]))
    io.open(PAGE, "w", encoding="utf-8").write("\n".join(buf) + "\n")
    print("\n  wrote %s" % os.path.relpath(PAGE, ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())
