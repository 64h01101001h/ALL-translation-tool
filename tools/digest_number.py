#!/usr/bin/env python3
"""The digest's running number, derived from docs/digests/NUMBERS.tsv.

Usage:
    tools/digest_number.py 2026-09-12            -> prints the number
    tools/digest_number.py 2026-09-12 --assign   -> assigns it if new, then prints

The number is a running count, so it cannot be recovered from the day's work
the way every other figure in a digest can. It goes out in a subject line, and
a running count kept in someone's head is wrong eventually. So it is kept in a
file, assigned once, and never silently changed afterwards.

Two refusals, both deliberate:
  * a date already in the ledger keeps its number, always — those subject
    lines are in other people's mailboxes and renumbering them would make the
    series disagree with itself;
  * a date EARLIER than the last one in the ledger is refused rather than
    inserted, because inserting it would shift every number after it.
"""
import sys
from pathlib import Path

LEDGER = Path(__file__).resolve().parent.parent / "docs/digests/NUMBERS.tsv"


def rows():
    out = []
    for line in LEDGER.read_text(encoding="utf-8").splitlines():
        if not line.strip() or line.lstrip().startswith("#"):
            continue
        parts = line.split("\t")
        out.append((int(parts[0]), parts[1].strip(),
                    parts[2].strip() if len(parts) > 2 else ""))
    return sorted(out)


def number_for(date, assign=False):
    table = rows()
    for n, d, _ in table:
        if d == date:
            return n
    if not assign:
        raise SystemExit(
            f"{date} is not in the ledger and --assign was not given.\n"
            f"Run: tools/digest_number.py {date} --assign")
    if table and date < table[-1][1]:
        raise SystemExit(
            f"refusing to assign {date}: it is earlier than the last digest "
            f"({table[-1][1]}, #{table[-1][0]}). Inserting it would shift "
            f"every number after it, and those have already been sent.")
    n = (table[-1][0] + 1) if table else 1
    with LEDGER.open("a", encoding="utf-8") as f:
        f.write(f"{n}\t{date}\tdrafted\n")
    return n


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise SystemExit("usage: digest_number.py YYYY-MM-DD [--assign]")
    print(number_for(sys.argv[1], "--assign" in sys.argv))
