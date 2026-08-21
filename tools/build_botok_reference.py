#!/usr/bin/env python3
"""Dump the Python botok oracle's syllable chunking over every distinct
Tibetan-unicode string in the spine, for the C++ port's diff-battery.

    python3 tools/build_botok_reference.py build/hgm_spine_v27_2.db \
        build/botok_syls_reference.tsv

Needs the botok package (pip3 install --user botok — same harness-only role
as tools/pyewts_oracle.py; the C++ port in allcore stays the shipped code).
Output TSV: tibetan_text <TAB> syl1|syl2|... (cleaned syllables from
TokChunks.get_syls; empty second field = no syllable chunks found).
Strings containing tab/newline/pipe are skipped and counted (none expected).
"""
import sqlite3
import sys

from botok import TokChunks


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__)
        return 2
    db, out_path = sys.argv[1], sys.argv[2]
    con = sqlite3.connect(db)
    rows = con.execute(
        "SELECT DISTINCT tibetan FROM entries "
        "WHERE tibetan IS NOT NULL AND tibetan != '' ORDER BY tibetan"
    ).fetchall()
    skipped = 0
    n = 0
    with open(out_path, "w", encoding="utf-8") as f:
        for (tib,) in rows:
            if "\t" in tib or "\n" in tib or "|" in tib:
                skipped += 1
                continue
            syls = TokChunks(tib).get_syls()
            f.write(f"{tib}\t{'|'.join(syls)}\n")
            n += 1
    print(f"wrote {n} oracle rows to {out_path} ({skipped} skipped)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
