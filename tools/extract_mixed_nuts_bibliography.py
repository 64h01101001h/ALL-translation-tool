#!/usr/bin/env python3
"""extract_mixed_nuts_bibliography.py — build the shared bibliography bank.

Companion to extract_mixed_nuts_notes.py (STD-008 reuse program). Extracts
every bibliography entry from the RELEASED volumes staged in the data
project's Input/GMR_volumes_2026-08/. Only published, GMR-approved volumes
feed this bank — that is Adam's ruling of 2026-08-06: apparatus enters the
official MAIN documents only once published and 100% approved.

Entry format in the volumes: a "Bibliography of works ..." heading, then
alternating ID paragraphs (S1 / B12 / E2 ...) and entry paragraphs. Entries
carry ACIP catalog numbers (e.g. ACIP TD04158, ACIP S05392L) which we also
extract — they key the entry to our file decoder and BDRC scan links.

Output: data/extracted/mixed_nuts_bibliography.json
  [{source, section, id, text, acip_refs}]

Usage: python3 tools/extract_mixed_nuts_bibliography.py
"""
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Input/GMR_volumes_2026-08")
OUT = os.path.join(HERE, "data", "extracted", "mixed_nuts_bibliography.json")

VOLS = [
    ("A Song of My Spiritual Life (tr. Chang w/ GMR, 2025)",
     "SPIRITUAL SONG 2 1 25 (AN bibliography edits completed 2 4 25) final 8 13 25.docx"),
    ("The Other Side of Emptiness (tr. Chang w/ GMR, 2026)",
     "OTHER SIDE 2 25 26.docx"),
    ("The Sixty Verses of Nagarjuna (tr. Lashaw w/ GMR, 2023)",
     "60 VERSES 11 27 23 (GMR & AN final edit 5 12 25).docx"),
]

ID_RE = re.compile(r"^([SBEC])(\d{1,3})$")
ACIP_RE = re.compile(r"ACIP\s+([A-Z]{1,2}\d{4,5}[A-Z0-9-]*)")
SECTION_RE = re.compile(r"^Bibliography of (works|material)", re.IGNORECASE)


def main():
    entries = []
    for source, fname in VOLS:
        txt = subprocess.run(
            ["pandoc", "-t", "plain", "--wrap=none",
             os.path.join(SRC, fname)],
            capture_output=True, text=True, check=True).stdout
        pars = [re.sub(r"\s+", " ", p).strip()
                for p in re.split(r"\n\s*\n", txt)]
        section = None
        pending_id = None
        seen = set()
        for p in pars:
            if not p:
                continue
            if SECTION_RE.match(p):
                # heading may flow over two paragraphs ("originally
                # written in Sanskrit"); keep first line as anchor
                section = p.rstrip(" @")
                pending_id = None
                continue
            if section is None:
                continue
            m = ID_RE.match(p)
            if m:
                pending_id = p
                continue
            if pending_id and len(p) > 40:
                # heading continuation lines refine the section label
                key = (pending_id, p[:60])
                if key not in seen:
                    seen.add(key)
                    entries.append({
                        "source": source,
                        "section": section,
                        "id": pending_id,
                        "text": p,
                        "acip_refs": sorted(set(ACIP_RE.findall(p))),
                    })
                pending_id = None
                continue
            if len(p) < 60 and ("written in" in p or "in English" in p):
                section = section.split(" originally")[0] + " " + p.rstrip(" @")
                continue
            # explicit post-bibliography headings end extraction;
            # stray annotation paragraphs (e.g. B10's catalog-number
            # note) do NOT — they are simply skipped
            if p.startswith(("The Diamond Cutter Classics Series",
                             "List of Titles", "Dedication",
                             "About the ", "Endnotes")):
                section = None
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        json.dump(entries, f, ensure_ascii=False, indent=1)
    per = {}
    for e in entries:
        per[e["source"]] = per.get(e["source"], 0) + 1
    print(json.dumps(per, indent=1))
    acip = sum(1 for e in entries if e["acip_refs"])
    print("total entries:", len(entries), f"({acip} with ACIP refs) ->", OUT)


if __name__ == "__main__":
    main()
