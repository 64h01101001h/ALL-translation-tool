#!/usr/bin/env python3
"""Flatten the catalogued subject labels into one work-keyed table.

What this is, and firmly is not
-------------------------------
It is a record of what the CATALOG says a text is about: R6's bilingual
subject headings (243 of them, "Middle-Way_Philosophy--DBU_MA") and the
Sungbum catalog's Tibetan headings. Someone wrote these down; they are
facts with an author.

It is NOT a register layer. Nothing here says how a word should be READ
in a genre. That would require Geshe Michael to have said so, and
measurement shows he has for about five terms out of 12,004
(docs/FINDINGS.md, 2026-08-22). Ranking glosses by genre would be the
machine deciding what he meant, which rule 1 forbids.

Vinaya is flagged because Adam ruled it reads differently, being the
oldest language in the monastic curriculum. The flag is set ONLY on an
exact match of R6's own "Vowed_Morality--'DUL_BA" heading. It is
deliberately not a substring test: "Works_On_Good-Heart_Teachings_For_
Barbarian_Lands--MTHA'_'KHOB_'DUL_BA'I_BLO_SBYONG" contains 'DUL_BA and
is a lojong text, not Vinaya.

Usage:  python3 tools/build_subject_index.py
Writes: data/extracted/work_subjects.tsv
"""
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EX = os.path.join(ROOT, "data", "extracted")
OUT = os.path.join(EX, "work_subjects.tsv")
KEYRE = re.compile(r"^([A-Za-z]+)0*(\d+)")
VINAYA = "Vowed_Morality--'DUL_BA"


def norm(k):
    m = KEYRE.match(k or "")
    return (m.group(1).upper() + m.group(2)) if m else ""


def split_label(lab):
    """'Middle-Way_Philosophy--DBU_MA' -> ('Middle-Way Philosophy','DBU MA')"""
    if "--" in lab:
        en, bo = lab.split("--", 1)
    else:
        en, bo = lab, ""
    return en.replace("_", " ").strip(), bo.replace("_", " ").strip()


def main():
    rows = []
    r6 = json.load(open(os.path.join(EX, "r6_work_subjects.json")))
    for k, v in r6.items():
        wk = norm(k)
        if not wk:
            continue
        for lab in (v if isinstance(v, list) else [v]):
            en, bo = split_label(lab)
            rows.append((wk, en, bo, lab, "r6",
                         "1" if lab == VINAYA else "0"))
    sb = json.load(open(os.path.join(EX, "sungbum_subjects.json")))
    for k, v in sb.items():
        wk = norm(k)
        if not wk or not v:
            continue
        # Sungbum headings are Tibetan unicode, no English side
        rows.append((wk, "", str(v).replace("\t", " ").strip(),
                     str(v).replace("\t", " ").strip(), "sungbum", "0"))

    seen = set()
    uniq = []
    for r in rows:
        if r in seen:
            continue
        seen.add(r)
        uniq.append(r)
    uniq.sort()
    with open(OUT, "w") as f:
        f.write("# work_key\tenglish\ttibetan\traw_label\tsource\tvinaya\n")
        for r in uniq:
            f.write("\t".join(r) + "\n")

    keys = {r[0] for r in uniq}
    vin = {r[0] for r in uniq if r[5] == "1"}
    print("subject rows        : %d" % len(uniq))
    print("distinct work keys  : %d" % len(keys))
    print("  flagged Vinaya    : %d" % len(vin))
    print("distinct R6 labels  : %d"
          % len({r[3] for r in uniq if r[4] == "r6"}))
    print("wrote %s" % os.path.relpath(OUT, ROOT))


if __name__ == "__main__":
    main()
