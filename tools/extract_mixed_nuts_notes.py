#!/usr/bin/env python3
"""extract_mixed_nuts_notes.py — build the shared footnote bank (STD-008).

Geshe Michael's directive (reported by Adam, 2026-08-06): footnotes created
in any released Mixed Nuts translation can and should be REUSED by
translators. This extracts every endnote from the released volumes staged
in the data project's Input/GMR_volumes_2026-08/ into a structured bank the
Draft workspace can search and insert from — always with its source
citation, so reuse stays attributed.

Output: data/extracted/mixed_nuts_notes.json
  [{source, note, lemma, text}]  — lemma = the note's headword phrase
                                   (the part before the first colon)

Usage: python3 tools/extract_mixed_nuts_notes.py
"""
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Input/GMR_volumes_2026-08")
OUT = os.path.join(HERE, "data", "extracted", "mixed_nuts_notes.json")

VOLS = [
    ("A Song of My Spiritual Life (tr. Chang w/ GMR, 2025)",
     "SPIRITUAL SONG 2 1 25 (AN bibliography edits completed 2 4 25) final 8 13 25.docx"),
    ("The Other Side of Emptiness (tr. Chang w/ GMR, 2026)",
     "OTHER SIDE 2 25 26.docx"),
    ("The Sixty Verses of Nagarjuna (tr. Lashaw w/ GMR, 2023)",
     "60 VERSES 11 27 23 (GMR & AN final edit 5 12 25).docx"),
]


def main():
    notes = []
    for source, fname in VOLS:
        txt = subprocess.run(
            ["pandoc", "-t", "plain", "--wrap=none",
             os.path.join(SRC, fname)],
            capture_output=True, text=True, check=True).stdout
        txt = txt.replace("‘", "'").replace("’", "'")
        seen = set()
        for par in re.split(r"\n\s*\n", txt):
            p = re.sub(r"\s+", " ", par).strip()
            m = re.match(r"^\[(\d+)\]\s+([^:]{2,80}):\s+(.{20,})$", p)
            if not m:
                continue
            n, lemma, body = int(m.group(1)), m.group(2).strip(), m.group(3).strip()
            key = (n, lemma)
            if key in seen:
                continue
            seen.add(key)
            notes.append({"source": source, "note": n, "lemma": lemma,
                          "text": body})
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        json.dump(notes, f, ensure_ascii=False, indent=1)
    per = {}
    for x in notes:
        per[x["source"]] = per.get(x["source"], 0) + 1
    print(json.dumps(per, indent=1))
    print("total notes:", len(notes), "->", OUT)


if __name__ == "__main__":
    main()
