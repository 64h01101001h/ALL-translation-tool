#!/usr/bin/env python3
"""build_whitney.py — bank William Dwight Whitney's two preeminent works
as a reference layer (Adam's request, 2026-08-08).

Sources (data/whitney/src/, from github.com/gasyoun/WhitneyRoots,
Apache-2.0; underlying works public domain — Whitney d. 1894):
  Whitney-numbered-2026.md        the digitization's declared REFERENCE
                                  master: 938 roots of "The Roots,
                                  Verb-Forms and Primary Derivatives of
                                  the Sanskrit Language" (1885) with
                                  homonym numbers and meanings
  Whitney_Grammar_Citations.md    every root's section citations into
                                  the "Sanskrit Grammar" (1879; Harvard
                                  1950 repr.), with generic/specific(✦)/
                                  exception(⚠) markers, plus the
                                  digitization's class column and DCS
                                  corpus classes

DATA-QUALITY DECISIONS (verified against the sources 2026-08-08):
- The repo's class-PP table has column-bleed corruption in its PPP
  column (e.g. rows 106-108) — NOT banked; the layer ships without
  principal-part forms until a clean source is verified.
- Class attribution in the citations table is FORM-level: homonyms
  written identically (Whitney's 1 kṛ make / 2 kṝ scatter both appear
  as "kṛ") share one class row. Banked, but the app labels the column
  "as digitized (form-level)" — never as Whitney's own per-homonym
  assignment. The DCS corpus-class column rides along as comparanda.

Writes data/whitney/whitney_roots.tsv:
  id  root  homonym  meaning  classes  grammar_secs  dcs_classes  notes

Usage: python3 tools/build_whitney.py
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(HERE, "data", "whitney", "src")
OUT = os.path.join(HERE, "data", "whitney", "whitney_roots.tsv")


def main() -> int:
    # 1. the numbered reference master: `N. [h ]√root “meaning”`
    master = {}
    rx = re.compile(
        r"^(\d+)\.\s+(?:(\d)\s+)?√?([^\s“(]+)\s*(\(\?\))?\s*"
        r"(?:“([^”]+)”|\(see\s+√?([^)]+)\))")
    with open(os.path.join(SRC, "Whitney-numbered-2026.md"),
              encoding="utf-8") as f:
        for line in f:
            m = rx.match(line.strip())
            if m:
                # Whitney's own annotations kept verbatim: "(?)" marks
                # a doubtful root; "(see √X)" is his cross-reference
                meaning = m.group(5) or ("see √" + m.group(6).strip())
                if m.group(4):
                    meaning += " [doubtful root (?)]"
                master[int(m.group(1))] = {
                    "homonym": m.group(2) or "",
                    "root": m.group(3),
                    "meaning": meaning,
                }
    if len(master) < 900:
        print(f"master parse suspiciously small: {len(master)}",
              file=sys.stderr)
        return 1

    # 2. the Grammar citations table (markdown pipes)
    cites = {}
    with open(os.path.join(SRC, "Whitney_Grammar_Citations.md"),
              encoding="utf-8") as f:
        for line in f:
            if not line.startswith("| "):
                continue
            cols = [c.strip() for c in line.strip().strip("|").split("|")]
            if len(cols) < 6 or not cols[0].isdigit():
                continue
            # strip markdown emphasis but KEEP the ✦/⚠ markers — they
            # are the generic/specific/exception legend
            secs = re.sub(r"[*_`]", "", cols[3])
            cites[int(cols[0])] = {
                "table_root": re.sub(r"^\d+\s+", "", cols[1]),
                "classes": cols[2],
                "secs": secs,
                "dcs": cols[4],
                "notes": cols[5],
            }

    mismatch = 0
    with open(OUT, "w", encoding="utf-8") as out:
        out.write("# Whitney root layer — see data/whitney/README.md "
                  "for provenance and caveats\n")
        out.write("# id\troot\thomonym\tmeaning\tclasses(form-level)\t"
                  "grammar_secs\tdcs_classes\tnotes\n")
        for i in sorted(master):
            m = master[i]
            c = cites.get(i, {})
            if c and c.get("table_root") != m["root"]:
                mismatch += 1
            row = [str(i), m["root"], m["homonym"], m["meaning"],
                   c.get("classes", ""), c.get("secs", ""),
                   c.get("dcs", ""), c.get("notes", "")]
            out.write("\t".join(x.replace("\t", " ") for x in row) + "\n")

    print(f"{len(master)} roots -> {OUT}")
    print(f"citation rows: {len(cites)}; root-form mismatches vs "
          f"master: {mismatch}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
