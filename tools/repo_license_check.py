#!/usr/bin/env python3
"""The repo-tree counterpart of manifest_check: every git-tracked file
must fall under exactly one row of docs/REPO_LICENSING.md.

Rules, each one a lesson:
  * unmapped file -> FAIL. Adding material costs a ledger row. This is
    the gate that was missing when three in-copyright books sat public.
  * unknown status word -> FAIL. 'ships' vs 'ok' already burned us once
    in the payload manifest; a ledger only means something if its
    vocabulary is closed.
  * a row that matches NO tracked file -> warning, not failure. A stale
    row is clutter; a missing row is exposure. Different severities.
  * open rows are REPORTED with their count, never hidden. The exit is
    still 0: 'open' is an honest recorded state, not a violation.
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LEDGER = ROOT / "docs" / "REPO_LICENSING.md"
VALID = {"own", "settled", "open"}


def rows():
    out = []
    pat = re.compile(r"^\|\s*([^|\s]+)\s*\|\s*([^|\s]+)\s*\|")
    for ln in LEDGER.read_text(encoding="utf-8").splitlines():
        m = pat.match(ln)
        if not m or set(m.group(1)) <= {"-", ":"} or m.group(1) == "prefix":
            continue   # header and separator rows, any dash count
        out.append((m.group(1), m.group(2)))
    return out


def main():
    table = rows()
    bad = [s for _, s in table if s not in VALID]
    if bad:
        print("FAIL repo-licensing: unknown status word(s): %s" % sorted(set(bad)))
        print("  valid: %s" % sorted(VALID))
        return 1
    files = subprocess.run(
        ["git", "ls-files"], cwd=ROOT, capture_output=True, text=True,
        check=True).stdout.splitlines()
    # longest-prefix wins, so data/soas_pos/ beats a hypothetical data/ row
    table.sort(key=lambda r: -len(r[0]))
    unmapped, hits, openf = [], {p: 0 for p, _ in table}, 0
    for f in files:
        for p, s in table:
            if f == p or f.startswith(p):
                hits[p] += 1
                if s == "open":
                    openf += 1
                break
        else:
            unmapped.append(f)
    if unmapped:
        print("FAIL repo-licensing: %d tracked file(s) match no ledger row:"
              % len(unmapped))
        for f in unmapped[:12]:
            print("   ", f)
        if len(unmapped) > 12:
            print("    ... and %d more" % (len(unmapped) - 12))
        print("  Add a row to docs/REPO_LICENSING.md - with real terms, "
              "not a guess.")
        return 1
    stale = [p for p, n in hits.items() if n == 0]
    for p in stale:
        print("  note: ledger row matches nothing tracked: %s" % p)
    nopen = sum(1 for _, s in table if s == "open")
    print("repo-licensing: %d tracked files, all mapped; %d open row(s) "
          "covering %d file(s) - recorded, not hidden" %
          (len(files), nopen, openf))
    return 0


if __name__ == "__main__":
    sys.exit(main())
