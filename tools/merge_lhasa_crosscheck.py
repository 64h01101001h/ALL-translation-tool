#!/usr/bin/env python3
"""merge_lhasa_crosscheck.py — cross-verify the two independent
KL→H routes and merge THL-only matches into the scan-routing
concordance under guards.

Route A: kl_bdrc_concordance.json  (BDRC IIIF part labels)
Route B: kl_lhasa_concordance.json (THL kt/h catalog records)

Both target the same H numbering (MW26071 parts ARE H numbers —
verified live 2026-08-13). Agreement on the overlap is the
verification; disagreements are listed for human review (never
auto-resolved); THL-only matches merge as tier "thl-cross" only if
(a) the part exists in the harvested outline and (b) catalog order
is preserved against the existing mapping.
"""
import json, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
A = os.path.join(ROOT, "data/extracted/kl_bdrc_concordance.json")
B = os.path.join(ROOT, "data/extracted/kl_lhasa_concordance.json")
TSV = os.path.join(ROOT, "data/extracted/mw26071_part_titles.tsv")


def main():
    da = json.load(open(A))
    db = json.load(open(B))
    a = {int(k): v for k, v in da["kl_to_part"].items()}
    b = {int(k): v for k, v in db["kl_to_h"].items()}
    parts = set()
    for line in open(TSV):
        if "\t" in line:
            parts.add(int(line.split("\t")[0]))
    both = sorted(set(a) & set(b))
    agree = [k for k in both if a[k] == b[k]]
    disagree = [(k, a[k], b[k]) for k in both if a[k] != b[k]]
    print(f"overlap {len(both)} · agree {len(agree)} · "
          f"disagree {len(disagree)}")
    if disagree:
        print("DISAGREEMENTS (human review, not auto-resolved):",
              disagree)
    kind = da["match_kind"]
    added = []
    for k, h in sorted((k, v) for k, v in b.items()):
        if k in a or h not in parts:
            continue
        lo = max((v for kk, v in a.items() if kk < k), default=0)
        hi = min((v for kk, v in a.items() if kk > k),
                 default=10**9)
        if lo <= h <= hi:
            a[k] = h
            kind[str(k)] = "thl-cross"
            added.append((k, h))
    da["kl_to_part"] = {str(k): v for k, v in sorted(a.items())}
    da["match_kind"] = kind
    da["meta"]["matched"] = len(a)
    da["meta"]["cross_check"] = (
        f"THL kt/h lane: {len(agree)}/{len(both)} agreement on the "
        f"overlap; {len(disagree)} disagreement(s) listed for "
        f"review; {len(added)} THL-only matches merged as "
        f"thl-cross (part exists + order preserved)")
    da["meta"]["disagreements"] = [
        {"kl": k, "bdrc": x, "thl": y} for k, x, y in disagree]
    da["unmatched"] = [u for u in da["unmatched"] if u not in a]
    da["meta"]["unmatched"] = len(da["unmatched"])
    ks = sorted(a)
    inv = sum(1 for x, y in zip(ks, ks[1:]) if a[x] > a[y])
    assert inv == 0, "order inversion after merge — refusing to save"
    json.dump(da, open(A, "w"), indent=1)
    print(f"merged {len(added)} · total {len(a)} · inversions {inv}"
          f" -> {A}")


if __name__ == "__main__":
    main()
