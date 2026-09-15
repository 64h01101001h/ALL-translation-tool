#!/usr/bin/env python3
"""Build the person-keyed author index the Library's author search needs.

Why this exists
---------------
The catalog's per-text author links (acip_person_links.json, 1,768 of
them, Elie Roux/BDRC) carry an author STRING and a BDRC person id per
work. The strings are OCR-grade: one person appears under as many as
ten spellings ("RJE TZONG KHA PA BLO BZANG GRAGS PA",
"RJE T ZONG KHA PA...", "...BLO BZANG GR AGS PA"). Searching strings
would return Tsongkhapa seven times with his 161 works split across
the variants.

The pid clusters them EXACTLY - it is BDRC's own per-text linkage, not
a name match, so this grouping is evidence rather than inference
(inviolable rule 3). Every link carries one; none are missing.

The index is keyed by pid and carries every spelling as a searchable
alias, so a query matching any variant finds the one person and all
their works.

persons_bdrc.json is joined in for dates and biography candidates, and
tol_links.json for the Treasury of Lives page. A person present in
only one of the banks is kept and SAYS so - the two promise different
things (texts vs a biography).

Usage:  python3 tools/build_author_index.py
Writes: data/extracted/author_index.json
"""
import collections
import json
import os
import re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EX = os.path.join(ROOT, "data", "extracted")
OUT = os.path.join(EX, "author_index.json")
KEYRE = re.compile(r"^([A-Za-z]+)0*(\d+)")


def norm(a):
    """Strip life-dates, collapse whitespace, uppercase."""
    a = re.sub(r"\s*\([^)]*\d[^)]*\)\s*$", "", a or "")
    return re.sub(r"\s+", " ", a).strip().upper()


def library_work_keys():
    have = set()
    lib = os.path.join(ROOT, "library")
    for dp, _, fs in os.walk(lib):
        for f in fs:
            if f.startswith("."):
                continue
            m = KEYRE.match(f)
            if m:
                have.add(m.group(1).upper() + m.group(2))
    return have


def main():
    links = json.load(open(os.path.join(EX, "acip_person_links.json")))["links"]
    try:
        bdrc = json.load(open(os.path.join(EX, "persons_bdrc.json")))["authors"]
    except Exception:
        bdrc = {}
    try:
        tol = json.load(open(os.path.join(EX, "tol_links.json")))["links"]
    except Exception:
        tol = {}
    present = library_work_keys()

    spellings = collections.defaultdict(collections.Counter)
    works = collections.defaultdict(set)
    dates = {}
    for wk, v in links.items():
        pid = v.get("pid")
        if not pid:
            continue
        a = v.get("author") or ""
        n = norm(a)
        if n:
            spellings[pid][n] += 1
        m = re.search(r"\((\d{3,4}\s*-\s*\d{0,4})\)", a)
        if m and pid not in dates:
            dates[pid] = m.group(1).replace(" ", "")
        works[pid].add(wk)

    # pid -> dates/name from the persons bank, matched on candidate pid
    bdrcByPid = {}
    for name, rec in bdrc.items():
        for c in rec.get("candidates", []):
            p = c.get("pid")
            if p:
                bdrcByPid.setdefault(p, {"name": name, "rec": rec})

    out = {}
    unnamed = 0
    # iterate every pid that has WORKS, not only those carrying a name.
    # 38 pids reach us with an empty author field on every one of their
    # links (39 works); keying off the name silently dropped them and
    # their texts. The works are real whether or not the catalog wrote
    # a name down.
    for pid in sorted(works):
        var = spellings.get(pid) or collections.Counter()
        if var:
            # the display form is the MOST FREQUENT spelling: typos are
            # rare, so the mode is the catalog's own consensus. Never
            # invented.
            display = var.most_common(1)[0][0]
        elif bdrcByPid.get(pid):
            display = norm(bdrcByPid[pid]["name"])
        else:
            # no name anywhere. Say so rather than inventing one or
            # dropping the works (inviolable rule 3).
            display = ""
            unnamed += 1
        wks = sorted(works[pid])
        local = sorted(w for w in wks if w.upper() in present)
        srcs = ["acip-person-links"]
        b = bdrcByPid.get(pid)
        if b:
            srcs.append("persons-bdrc")
        entry = {
            "display": display,
            "name_recorded": bool(display),
            "aliases": sorted(var),
            "works": wks,
            "works_local": local,
            "sources": srcs,
        }
        if pid in dates:
            entry["dates"] = dates[pid]
        elif b and b["rec"].get("dates") not in (None, "", "Unknown"):
            entry["dates"] = b["rec"]["dates"]
        if pid in tol:
            entry["tol"] = tol[pid]["tol"]
            entry["tol_source"] = tol[pid]["source"]
        out[pid] = entry

    # persons_bdrc people with NO catalog texts: kept, and marked, because
    # they still answer "who is this" even when they answer no texts.
    bioOnly = 0
    for pid, b in sorted(bdrcByPid.items()):
        if pid in out:
            continue
        e = {"display": norm(b["name"]), "aliases": [norm(b["name"])],
             "works": [], "works_local": [], "sources": ["persons-bdrc"]}
        d = b["rec"].get("dates")
        if d and d != "Unknown":
            e["dates"] = d
        if pid in tol:
            e["tol"] = tol[pid]["tol"]
            e["tol_source"] = tol[pid]["source"]
        out[pid] = e
        bioOnly += 1

    withTexts = sum(1 for e in out.values() if e["works_local"])
    doc = {
        "_meta": {
            "what": "BDRC person id -> every catalog spelling of that "
                    "person, and every work attributed to them",
            "grouping": "by BDRC pid from acip_person_links (per-text "
                        "linkage, not name matching) - exact, not inferred",
            "display_rule": "the most frequent spelling; typos are rare so "
                            "the mode is the catalog's own consensus",
            "people": len(out),
            "with_local_texts": withTexts,
            "biography_only": bioOnly,
            "author_strings_collapsed": sum(len(e["aliases"]) for e in out.values()),
            "no_name_recorded": unnamed,
            "links_read": len(links),
        },
        "people": out,
    }
    with open(OUT, "w") as f:
        json.dump(doc, f, indent=1, sort_keys=False)
        f.write("\n")
    print("people indexed        : %d" % len(out))
    print("  with local texts    : %d" % withTexts)
    print("  biography only      : %d" % bioOnly)
    print("spellings collapsed   : %d" % doc["_meta"]["author_strings_collapsed"])
    print("  no name recorded    : %d (works kept, name left blank)" % unnamed)
    print("wrote %s" % os.path.relpath(OUT, ROOT))


if __name__ == "__main__":
    main()
