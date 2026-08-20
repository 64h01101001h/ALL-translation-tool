#!/usr/bin/env python3
"""Route 4 of the apparatus Tibetan-anchor program (task 9b):
resolve footnote lemmas through HGM's OWN reverse index.

A lemma anchors here only when it is EXACTLY one of an entry's own
hgm_glosses (fold: lowercase, punctuation out). That makes every
candidate HGM's own English equivalence — rule 1 is satisfied by
construction (we MATCH his English, never compose it).

Tier discipline (the 0.5-floor lesson — candidates spend a
translator's attention): curated/glossary candidates are kept
always; auto-aligned (PROVISIONAL) candidates are kept only when no
better tier answered, and at most three.

Reads  data/extracted/apparatus_notes.json (the 2,234-note bank)
       data/hgm_reverse_index_v27_2.json
       data/hgm_dictionary_v27_2.tsv       (tier via status/gloss)
       data/extracted/apparatus_anchors.json (existing rows kept —
       route 3's body-quoted anchors are never touched)
Writes data/extracted/apparatus_anchors.json (merged, method-tagged)
"""
import csv, json, re, sys, datetime

ROOT = "."

def fold(s):
    s = s.lower()
    s = re.sub(r"[^a-z' ]+", " ", s)
    return re.sub(r"\s+", " ", s).strip()

def main():
    csv.field_size_limit(10**7)
    glosses = {}
    with open(f"{ROOT}/data/hgm_dictionary_v27_2.tsv") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            w, g = row["wylie"], row["hgm_glosses"]
            if not w or not g:
                continue
            d = glosses.setdefault(w, {})
            for part in g.split(";"):
                p = fold(part)
                if p:
                    d.setdefault(p, part.strip())
    rev = json.load(open(f"{ROOT}/data/hgm_reverse_index_v27_2.json"))
    notes = json.load(
        open(f"{ROOT}/data/extracted/apparatus_notes.json"))
    bank = json.load(
        open(f"{ROOT}/data/extracted/apparatus_anchors.json"))
    anchored = {(r["lemma"], r["source"]) for r in bank["rows"]}

    TIER_RANK = {"curated": 0, "glossary": 1, "auto-aligned": 2}
    added, notes_added = 0, 0
    for x in notes:
        key = (x["lemma"], x["source"])
        if key in anchored:
            continue
        f = fold(x["lemma"])
        if not f or f not in rev:
            continue
        cands = []
        for e in rev[f]:
            w = e["wylie"]
            gd = glosses.get(w, {})
            if f not in gd:
                continue
            cands.append(
                {"wylie": w, "tier": e.get("tier", ""),
                 "gloss": gd[f]})
        if not cands:
            continue
        cands.sort(key=lambda c: (TIER_RANK.get(c["tier"], 3),
                                  c["wylie"]))
        best = [c for c in cands
                if TIER_RANK.get(c["tier"], 3) <= 1]
        if not best:
            best = cands[:3]   # provisional-only: cap the spend
        row = {"lemma": x["lemma"], "source": x["source"],
               "num": x.get("num", 0), "method": "reverse-index",
               "candidates": []}
        for c in best:
            prov = ("" if c["tier"] != "auto-aligned"
                    else " — PROVISIONAL tier")
            row["candidates"].append({
                "wylie": c["wylie"],
                "as_written": c["wylie"],
                "evidence":
                    "HGM's own English equivalent for this entry "
                    f"is exactly this lemma (“{c['gloss']}”, "
                    f"{c['tier']} tier{prov}) — hgm_reverse_index "
                    "v27.2.",
                "technical_spelling": False})
        bank["rows"].append(row)
        anchored.add(key)
        notes_added += 1
        added += len(row["candidates"])

    bank["meta"]["route4"] = (
        f"reverse-index lemma resolution added "
        f"{datetime.date.today().isoformat()}: a lemma anchors only "
        "when it is EXACTLY one of an entry's own hgm_glosses; "
        "curated/glossary preferred, auto-aligned kept only alone "
        "and capped at 3, always labeled PROVISIONAL.")
    json.dump(bank, open(
        f"{ROOT}/data/extracted/apparatus_anchors.json", "w"),
        ensure_ascii=False, indent=1)
    print(f"route 4: +{notes_added} notes anchored "
          f"({added} candidates); bank now {len(bank['rows'])} rows")

if __name__ == "__main__":
    sys.exit(main())
