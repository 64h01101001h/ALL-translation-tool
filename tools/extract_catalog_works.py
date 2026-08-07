#!/usr/bin/env python3
"""extract_catalog_works.py — catalog-number → work-details join for the app.

Reads ACIP Catalog.TXT (the Thursday meeting folder; source of the v0.29
title wave) and emits data/extracted/catalog_works.json:
  { "TD3828": {"tib": ..., "eng": ..., "author": ..., "dates": ...}, ... }
Keys are normalized (letters + unpadded number) to match filename decoding.
Feeds the Library English titles and the bibliography composer auto-fill.

Usage: python3 tools/extract_catalog_works.py
"""
import json
import os
import re

MEET = os.path.expanduser(
    "~/Desktop/Thursday Database-Cataloging 6 4 26 Meeting")
HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(HERE, "data", "extracted", "catalog_works.json")


def norm(no):
    m = re.match(r"^([A-Z]+)0*(\d+)", no.upper())
    return m.group(1) + m.group(2) if m else None


def main():
    works = {}
    for line in open(os.path.join(MEET, "ACIP Catalog.TXT"),
                     encoding="utf-8", errors="replace"):
        line = line.strip()
        if not line.startswith("ACIP Catlog Number:"):
            continue
        f = {}
        for p in line.split("\t"):
            if ":" in p:
                k, v = p.split(":", 1)
                f[k.strip()] = v.strip()
        key = norm(f.get("ACIP Catlog Number", ""))
        if not key or key in works:
            continue
        clean = lambda s: "" if s.strip().lower() in ("", "n/a") else s.strip()
        works[key] = {
            "tib": re.sub(r"[\s,]+$", "", f.get("Title Tibetan", "")),
            "eng": clean(f.get("Title English", "")),
            "author": re.sub(r"[\s,]+$", "", f.get("Author Tibetan", "")),
            "author_eng": clean(f.get("Author English", "")),
            "dates": clean(f.get("Dates", "")),
        }
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    json.dump(works, open(OUT, "w"), ensure_ascii=False, indent=0)
    print(f"{len(works)} works -> {OUT}")


if __name__ == "__main__":
    main()
