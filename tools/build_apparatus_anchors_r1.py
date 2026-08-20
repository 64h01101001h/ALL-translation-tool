#!/usr/bin/env python3
"""Route 1 of the apparatus Tibetan-anchor program (task 9b):
course-constrained PASSAGE evidence.

For footnotes from the three published single-book sources, the
corpus carries the whole book as aligned segments under one course id
(SVN / SSL / OSE). Where a note's lemma appears VERBATIM inside the
published English of its OWN course, the aligned segment is attached
as a PASSAGE candidate — explicitly a passage, never a term
equivalence (the 2026-08-15 measurement showed why: naive containment
returned segments from the wrong work entirely; the course constraint
is what makes this honest, and the passage label is what keeps the
whole-segment warning satisfied).

Reads  data/extracted/apparatus_notes.json
       build/hgm_spine_v27_2.db  (corpus_segments)
       data/extracted/apparatus_anchors.json (existing rows kept)
Writes data/extracted/apparatus_anchors.json (merged)
"""
import json, re, sqlite3, sys, datetime

BOOK_COURSE = {
    "The Sixty Verses of Nagarjuna (tr. Lashaw w/ GMR, 2023)": "SVN",
    "A Song of My Spiritual Life (tr. Chang w/ GMR, 2025)": "SSL",
    "The Other Side of Emptiness (tr. Chang w/ GMR, 2026)": "OSE",
}
MAX_PASSAGES = 3

def fold(s):
    return re.sub(r"\s+", " ",
                  re.sub(r"[^a-z' ]+", " ", s.lower())).strip()

def main():
    db = sqlite3.connect("build/hgm_spine_v27_2.db")
    notes = json.load(open("data/extracted/apparatus_notes.json"))
    bank = json.load(open("data/extracted/apparatus_anchors.json"))
    anchored = {(r["lemma"], r["source"]) for r in bank["rows"]}
    segs = {}
    for src, c in BOOK_COURSE.items():
        segs[c] = db.execute(
            "SELECT seq, english, wylie FROM corpus_segments "
            "WHERE course=? ORDER BY seq", (c,)).fetchall()
    added_notes = added_passages = 0
    for x in notes:
        c = BOOK_COURSE.get(x["source"])
        if not c or (x["lemma"], x["source"]) in anchored:
            continue
        f = fold(x["lemma"])
        if not f or len(f) < 4:
            continue
        pat = re.compile(r"\b" + re.escape(f) + r"\b")
        row = {"lemma": x["lemma"], "source": x["source"],
               "num": x.get("num", 0), "method": "course-passage",
               "candidates": []}
        for seq, eng, wy in segs[c]:
            if not eng or not pat.search(fold(eng)):
                continue
            row["candidates"].append({
                "passage": True,
                "wylie": wy or "",
                "as_written": f"{c}:{seq}",
                "evidence": eng.strip(),
                "technical_spelling": False})
            if len(row["candidates"]) >= MAX_PASSAGES:
                break
        if row["candidates"]:
            bank["rows"].append(row)
            anchored.add((x["lemma"], x["source"]))
            added_notes += 1
            added_passages += len(row["candidates"])
    bank["meta"]["route1"] = (
        f"course-constrained passage evidence added "
        f"{datetime.date.today().isoformat()}: the lemma appears "
        "verbatim in the published English of the note's OWN work "
        "(courses SVN/SSL/OSE); attached as labeled PASSAGES, never "
        f"term equivalences; capped at {MAX_PASSAGES} per note.")
    json.dump(bank,
              open("data/extracted/apparatus_anchors.json", "w"),
              ensure_ascii=False, indent=1)
    print(f"route 1: +{added_notes} notes ({added_passages} "
          f"passages); bank now {len(bank['rows'])} rows")

if __name__ == "__main__":
    sys.exit(main())
