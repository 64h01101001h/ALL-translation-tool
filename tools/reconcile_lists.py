#!/usr/bin/env python3
"""Keep the backlog and its human-gated view from drifting apart.

Adam asked (2026-08-22) whether the two lists should be merged. They
should not: docs/CLOSER.md is the single source of truth and carries
every piece of open work, while docs/OVERSIGHT.md is a VIEW of the
subset only he can move, sorted by what KIND of act each needs so a
spare ten minutes goes to the highest-leverage thing. Merging would
bury 26 actionable rows among 43 engineering ones and destroy that.

Generating the view was the other option and is worse: the oversight
page carries cost estimates, what each item unblocks, and why nobody
else can do it. Those are editorial judgments, not derivable facts, and
a generator would flatten them.

So instead of merging or generating: RECONCILE. This fails loudly when
the two disagree, which is the only failure mode that actually loses
work. Wired into the constitution so it runs on every press.

Checks:
  1. every human-gated backlog row (WAITING / OPEN(Adam…)) appears in
     the oversight view
  2. every oversight row cites a backlog number that exists
  3. no oversight row in an OPEN section cites a CLOSED backlog item

Usage: python3 tools/reconcile_lists.py [root]
"""
import os
import re
import sys


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    closer = open(os.path.join(root, "docs/CLOSER.md"),
                  encoding="utf-8").read()
    over = open(os.path.join(root, "docs/OVERSIGHT.md"),
                encoding="utf-8").read()

    # backlog: number -> state
    rows = {}
    for m in re.finditer(r"^\|\s*(\d+)\s*\|(.*)\|([^|]*)\|\s*$",
                         closer, re.M):
        rows[int(m.group(1))] = m.group(3).strip()

    gated = {n: st for n, st in rows.items()
             if ("WAITING(" in st or "OPEN(Adam" in st
                 or "DEFERRED(" in st or st.startswith("| LIVING")
                 or "LIVING" in st)}
    closed = {n for n, st in rows.items() if st.startswith("**CLOSED")
              or st.startswith("CLOSED")}

    # the view: which backlog numbers does it cite, and where
    cited = set()
    closed_section = over.find("## CLOSED")
    open_cited, closed_cited = set(), set()
    for m in re.finditer(r"[Bb]acklog #(\d+)|(?<!\w)#(\d+)\b", over):
        n = int(m.group(1) or m.group(2))
        cited.add(n)
        # A citation that SAYS the item is closed is a reference, not a
        # claim that it is pending — "backlog #32 is CLOSED on the
        # auto-caption files; her transcripts would extend it" is
        # exactly right and must not be reported as drift. Look at the
        # words immediately around the number.
        window = over[max(0, m.start() - 40):m.end() + 40]
        if "CLOSED" in window or "closed" in window:
            continue
        (closed_cited if closed_section >= 0 and m.start() > closed_section
         else open_cited).add(n)

    problems = []
    for n in sorted(gated):
        if n not in cited:
            problems.append(
                "backlog #%d is human-gated (%s) but appears nowhere in "
                "docs/OVERSIGHT.md — it can only be moved by Adam and he "
                "cannot see it" % (n, gated[n][:56]))
    for n in sorted(open_cited):
        if n not in rows:
            problems.append(
                "docs/OVERSIGHT.md cites backlog #%d, which does not "
                "exist" % n)
        elif n in closed:
            problems.append(
                "docs/OVERSIGHT.md still lists backlog #%d as needing "
                "Adam, but the backlog has it CLOSED" % n)

    print("backlog rows            : %d" % len(rows))
    print("  human-gated           : %d" % len(gated))
    print("oversight cites         : %d backlog item(s)" % len(cited))
    if problems:
        print("\nDRIFT (%d):" % len(problems))
        for p in problems:
            print("  - " + p)
        return 1
    print("\nlists reconcile: nothing human-gated is invisible")
    return 0


if __name__ == "__main__":
    sys.exit(main())
