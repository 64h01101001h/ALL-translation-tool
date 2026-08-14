#!/usr/bin/env python3
"""match_sungbum_outline.py — the Sungbum pilot matcher: ACIP
S-texts (Tsongkhapa, per Élie's authoritative catalog titles) ↔
BDRC MW22109 outline leaves (per-text nodes of the 27-volume
gsung 'bum), by title.

Sides:
  A: data/extracted/acip_person_links.json — rows with pid P64
     carry the catalog's own WYLIE titles (col H of Élie's CSV).
  B: data/extracted/mw22109_outline.json — recursive wio: walk.

Tiers (the Degé recipe): exact → particle-fold → guarded
containment (unique, ≥60% length ratio). No numeric window exists
here (hashed node ids), so uniqueness is required corpus-wide and
ambiguity is listed, never resolved.

Output: data/extracted/sungbum_mw22109_concordance.json
  { "S5392": {"node": "MW22109_XXXX", "label": "...",
              "volume": "...", "tier": "exact"}, ... }
"""
import json, os, re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LINKS = os.path.join(ROOT, "data/extracted/acip_person_links.json")
OUTL = os.path.join(ROOT, "data/extracted/mw22109_outline.json")
OUT = os.path.join(ROOT,
                   "data/extracted/sungbum_mw22109_concordance.json")

PA_BA = {"ba": "pa", "ba'i": "pa'i", "bar": "par", "bo": "po",
         "bas": "pas", "bo'i": "po'i", "du": "tu"}


def norm(w):
    w = w.lower()
    w = re.sub(r"[（(].*?[)）]", " ", w)
    w = re.sub(r"[^a-z'+ ]+", " ", w)
    return " ".join(PA_BA.get(t, t) for t in w.split())


def pfree(s):
    drop = {"kyi", "gyi", "gi", "gyis", "kyis", "zhes", "bya",
            "ba", "pa", "bzhugs", "so"}
    return " ".join(t for t in s.split() if t not in drop)


def main():
    links = json.load(open(LINKS))["links"]
    ts = {k: v for k, v in links.items() if v["pid"] == "P64"
          and k.startswith("S")}
    outline = json.load(open(OUTL))["nodes"]
    # leaves: collection nodes with no COLLECTION children (their
    # children are volume manifests) — the per-text nodes; the
    # outline's furniture rows are excluded
    col_parents = {n["parent"] for n in outline
                   if n["kind"] == "collection"}
    FURNITURE = {"contents", "dkar chag", "preface", ""}
    leaves = [n for n in outline
              if n["kind"] == "collection"
              and n["id"] not in col_parents
              and n["label"].strip().rstrip("/").strip()
                  not in FURNITURE]
    # volume context: walk up to the depth-1 ancestor label
    byid = {n["id"]: n for n in outline}

    def volume_of(n):
        cur = n
        while cur and cur["depth"] > 1:
            cur = byid.get(cur["parent"])
        return cur["label"] if cur else ""

    lab_norm = [(n, norm(n["label"])) for n in leaves]
    mapping, ambiguous, unmatched = {}, [], []
    for s, rec in sorted(ts.items()):
        title = norm(rec["title_wylie"])
        if not title:
            unmatched.append(s)
            continue
        hits, tier = [], "exact"
        hits = [n for n, ln in lab_norm if ln == title]
        if not hits:
            tp = pfree(title)
            hits = [n for n, ln in lab_norm
                    if tp and pfree(ln) == tp]
            tier = "particle-fold"
        if not hits:
            for n, ln in lab_norm:
                if not ln:
                    continue
                lo, hi = sorted((len(ln), len(title)))
                if lo >= 15 and lo / hi >= 0.6 and \
                   (ln in title or title in ln):
                    hits.append(n)
            tier = "contained"
        ids = sorted({n["id"] for n in hits})
        if len(ids) == 1:
            n = hits[0]
            mapping[s] = {"node": n["id"], "label": n["label"],
                          "volume": volume_of(n), "tier": tier}
        elif len(ids) > 1:
            ambiguous.append({"s": s, "candidates": ids[:6],
                              "tier": tier})
        else:
            unmatched.append(s)
    json.dump({
        "meta": {
            "method": "title match: Élie-catalog wylie titles "
                      "(P64) vs MW22109 outline leaf labels; "
                      "tiers exact/particle-fold/guarded-"
                      "containment; uniqueness required "
                      "corpus-wide (hashed ids, no numeric "
                      "window); ambiguous/unmatched listed, "
                      "never guessed",
            "s_texts": len(ts), "outline_leaves": len(leaves),
            "matched": len(mapping),
            "ambiguous": len(ambiguous),
            "unmatched": len(unmatched),
        },
        "s_to_node": mapping,
        "ambiguous": ambiguous,
        "unmatched": unmatched,
    }, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"{len(ts)} S-texts · {len(leaves)} outline leaves · "
          f"matched {len(mapping)} · ambiguous {len(ambiguous)} · "
          f"unmatched {len(unmatched)} -> {OUT}")
    for s in ("S5392", "S5275"):
        if s in mapping:
            print(" ", s, "→", mapping[s]["label"][:60],
                  f"[{mapping[s]['tier']}]")


if __name__ == "__main__":
    main()
