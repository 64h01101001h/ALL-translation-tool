#!/usr/bin/env python3
"""Widen Treasury of Lives biography coverage for catalog authors.

Why this exists
---------------
Our person records come from BDRC, whose ``owl:sameAs`` carries a
Treasury of Lives link for only a minority of people — 43 of our 350
catalog author pids. Wikidata curates the same identifier
independently (property P4138), and knows far more of them.

What it does NOT do: invent, fuzzy-match, or resolve disagreements.
A pid gets a TOL id only when a named source states it. Where BDRC
and Wikidata disagree, the BDRC value stands (it is the record tied
to that pid) and the disagreement is written to ``conflicts`` for a
human to settle — never silently merged (inviolable rule 3).

Both identifier sets are facts about identity, not creative work;
Wikidata publishes under CC0. Credited in
docs/distribution/OPEN_SOURCE_NOTICES.md.

Usage:  python3 tools/widen_tol_links.py [--offline]
Writes: data/extracted/tol_links.json
"""
import json
import os
import re
import sys
import urllib.parse
import urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PERSONS = os.path.join(ROOT, "data/extracted/persons_bdrc.json")
OUT = os.path.join(ROOT, "data/extracted/tol_links.json")
CACHE = "/tmp/wd_tol_map.json"
SPARQL = "https://query.wikidata.org/sparql"
QUERY = "SELECT ?bdrc ?tol WHERE { ?item wdt:P2477 ?bdrc . " \
        "?item wdt:P4138 ?tol . }"
UA = "ALL-translation-tool/1.0 (Tibetan catalog research)"


def wikidata_pairs(offline):
    if offline and os.path.exists(CACHE):
        return json.load(open(CACHE))
    url = SPARQL + "?" + urllib.parse.urlencode({"query": QUERY})
    req = urllib.request.Request(
        url, headers={"Accept": "application/sparql-results+json",
                      "User-Agent": UA})
    with urllib.request.urlopen(req, timeout=120) as r:
        d = json.load(r)
    m = {b["bdrc"]["value"]: b["tol"]["value"]
         for b in d["results"]["bindings"]}
    json.dump(m, open(CACHE, "w"))
    return m


def main():
    offline = "--offline" in sys.argv
    wd = wikidata_pairs(offline)
    authors = json.load(open(PERSONS))["authors"]

    banked, pids = {}, set()
    for name, v in authors.items():
        for c in v.get("candidates", []):
            pid = c.get("pid")
            if not pid:
                continue
            pids.add(pid)
            m = re.search(r"TOLP(\d+)", c.get("tol") or "")
            if m:
                banked[pid] = (m.group(1), name)

    links, conflicts = {}, []
    for pid, (tol, name) in banked.items():
        links[pid] = {"tol": tol, "source": "bdrc-sameas"}
        if pid in wd and wd[pid] != tol:
            conflicts.append({"pid": pid, "author": name,
                              "bdrc_sameas": tol,
                              "wikidata_p4138": wd[pid],
                              "applied": "bdrc-sameas",
                              "note": "unresolved — a human must "
                                      "decide; possible homonym"})
    added = 0
    for pid in pids:
        if pid in links or pid not in wd:
            continue
        links[pid] = {"tol": wd[pid], "source": "wikidata-P4138"}
        added += 1

    out = {
        "_meta": {
            "what": "BDRC person id -> Treasury of Lives biography "
                    "id, for building readable biography URLs",
            "url_pattern": "https://www.treasuryoflives.org/"
                           "biographies/view/{tol}",
            "pattern_authority": "Wikidata P4138 formatter URL "
                                 "(P1630)",
            "sources": {
                "bdrc-sameas": "BDRC owl:sameAs, already banked in "
                               "persons_bdrc.json",
                "wikidata-P4138": "Wikidata 'Treasury of Lives ID' "
                                  "joined on P2477 BDRC Resource ID "
                                  "(CC0)"},
            "catalog_author_pids": len(pids),
            "linked": len(links),
            "from_bdrc": len(banked),
            "from_wikidata": added,
            "conflicts": len(conflicts),
        },
        "links": dict(sorted(links.items())),
        "conflicts": conflicts,
    }
    with open(OUT, "w") as f:
        json.dump(out, f, indent=1, sort_keys=False)
        f.write("\n")
    print("catalog author pids : %d" % len(pids))
    print("  from BDRC sameAs  : %d" % len(banked))
    print("  added by Wikidata : %d" % added)
    print("  linked total      : %d  (%.0f%%)"
          % (len(links), 100.0 * len(links) / max(1, len(pids))))
    print("  conflicts flagged : %d (BDRC value kept, logged)"
          % len(conflicts))
    print("wrote %s" % os.path.relpath(OUT, ROOT))


if __name__ == "__main__":
    main()
