#!/usr/bin/env python3
"""ingest_acip_catalog.py — Élie Roux's (BDRC) ACIP_Catalog.csv →
per-text authoritative author links.

Provenance: emailed by Élie Roux (BDRC) 2026-08-13 in reply to
Adam's outreach — "the link between the authors in the ACIP catalog
and the BDRC catalog … we've worked on with Joel some time ago";
column K = ACIP text id, column L = BDRC id (persons bdr:P…, topic
rows bdr:T…). Banked at data/extracted/acip_catalog_bdrc.csv.

Output: data/extracted/acip_person_links.json
  { "S05524": {"pid": "P80", "author": "...", "title_wylie": "...",
               "title_en": "...", "acip_raw": "S05524E"}, ... }

Keys are the BARE work core (letter prefix + digits, suffixes
stripped) so the app can match decodeAcipFilename output. Where two
raw ids collapse to one core with DIFFERENT persons, the core is
dropped to a conflicts list — never guessed.
"""
import csv, json, os, re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "data", "extracted",
                   "acip_catalog_bdrc.csv")
OUT = os.path.join(ROOT, "data", "extracted",
                   "acip_person_links.json")

CORE = re.compile(r"^([A-Z]{1,2})0*(\d+)")


def main():
    rows = list(csv.reader(open(SRC, encoding="utf-8-sig",
                                errors="replace")))[1:]
    links, conflicts, topics = {}, {}, 0
    for r in rows:
        if len(r) < 12:
            continue
        aid, bid = r[10].strip(), r[11].strip()
        if not aid or not bid:
            continue
        if bid.startswith("bdr:T"):
            topics += 1
            continue
        if not bid.startswith("bdr:P"):
            continue
        m = CORE.match(aid)
        if not m:
            continue
        core = m.group(1) + m.group(2)
        pid = bid.split(":", 1)[1]
        rec = {"pid": pid, "author": r[9].strip(),
               "title_wylie": r[7].strip(),
               "title_en": r[8].strip(), "acip_raw": aid}
        if core in links and links[core]["pid"] != pid:
            conflicts.setdefault(core, [links[core]]).append(rec)
            continue
        links[core] = rec
    for c in conflicts:
        links.pop(c, None)
    json.dump({
        "meta": {
            "source": "ACIP_Catalog.csv from Élie Roux (BDRC), "
                      "2026-08-13 — the ACIP↔BDRC author linkage "
                      "worked on with Joel; authoritative per-text "
                      "links (name matching not involved)",
            "texts": len(links),
            "conflicts": len(conflicts),
            "topic_rows_skipped": topics,
        },
        "links": links,
        "conflicts": conflicts,
    }, open(OUT, "w"), indent=1, ensure_ascii=False)
    print(f"{len(links)} per-text author links · "
          f"{len(conflicts)} conflicting cores dropped · "
          f"{topics} topic rows -> {OUT}")


if __name__ == "__main__":
    main()
