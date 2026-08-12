#!/usr/bin/env python3
"""build_citation_web.py — the Citation Web (Adam's wow round #10).

Which works quote which? For every aligned corpus segment of seven
syllables or more, find the SAME Tibetan line appearing in OTHER
courses (exact FTS phrase over the corpus — attested reuse only,
no fuzzy matching, rule 3). Edges are course pairs; each carries
its shared-passage count and capped examples with both courses'
published English.

Regenerate per corpus release. Output:
  data/extracted/citation_web.json
    { "meta": {...},
      "edges": [ {"a": course, "b": course, "count": n,
                  "examples": [{"wylie":…, "english_a":…,
                                "english_b":…, "seq_a":…, "seq_b":…}]} ] }
"""
import json, os, re, sqlite3, sys, time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUT = os.path.join(ROOT, "data", "extracted", "citation_web.json")

# catalog/subject waves are metadata, not translated passages
SKIP = {"TITLK", "TITLT", "TITLS", "TITLR", "SUBJ", "AUTH"}

def main():
    db = sqlite3.connect(DB)
    segs = db.execute(
        "SELECT id, course, seq, wylie, english FROM corpus_segments "
        "WHERE wylie IS NOT NULL AND wylie != ''").fetchall()
    print(len(segs), "segments")
    edges = {}   # (a,b) sorted -> {"count", "examples", seen set}
    n_checked = 0
    t0 = time.time()
    for sid, course, seq, wylie, english in segs:
        if course in SKIP:
            continue
        toks = [t for t in re.split(r"\s+", wylie.strip()) if t]
        if len(toks) < 7:
            continue
        n_checked += 1
        # exact phrase over the corpus FTS; quotes escape operators
        q = '"' + wylie.strip().replace('"', ' ') + '"'
        try:
            hits = db.execute(
                "SELECT s.id, s.course, s.seq, s.english "
                "FROM corpus_fts f JOIN corpus_segments s "
                "ON s.id = f.rowid WHERE corpus_fts MATCH ? "
                "LIMIT 20", (q,)).fetchall()
        except sqlite3.OperationalError:
            continue   # exotic FTS syntax in the line: skip, honest
        for hid, hcourse, hseq, henglish in hits:
            if hid == sid or hcourse == course or hcourse in SKIP:
                continue
            key = tuple(sorted((course, hcourse)))
            e = edges.setdefault(
                key, {"count": 0, "examples": [], "seen": set()})
            pair = (min(sid, hid), max(sid, hid))
            if pair in e["seen"]:
                continue
            e["seen"].add(pair)
            e["count"] += 1
            if len(e["examples"]) < 5:
                a_first = key[0] == course
                e["examples"].append({
                    "wylie": wylie.strip()[:200],
                    "english_a": (english if a_first else henglish
                                  or "")[:220],
                    "english_b": (henglish if a_first else english
                                  or "")[:220],
                    "seq_a": seq if a_first else hseq,
                    "seq_b": hseq if a_first else seq,
                })
        if n_checked % 2000 == 0:
            print(f"  {n_checked} checked, {len(edges)} edges, "
                  f"{time.time()-t0:.0f}s", flush=True)
    out = {
        "meta": {
            "generated": time.strftime("%Y-%m-%d"),
            "method": "exact full-segment FTS reuse across courses, "
                      ">=7 syllables; attested only, no fuzzy "
                      "matching; example cap 5/edge",
            "segments_checked": n_checked,
        },
        "edges": sorted(
            ({"a": k[0], "b": k[1], "count": v["count"],
              "examples": v["examples"]}
             for k, v in edges.items()),
            key=lambda e: -e["count"]),
    }
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    json.dump(out, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"DONE: {len(out['edges'])} edges from {n_checked} "
          f"checked segments -> {OUT}")

if __name__ == "__main__":
    main()
