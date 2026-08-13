#!/usr/bin/env python3
"""build_kl_bdrc_concordance.py — ACIP KL number → BDRC MW26071 part
number (Lhasa Kangyur scans), by title match.

WHY THIS EXISTS (the 2026-08-13 root cause): BDRC's MW26071 parts
follow the Lhasa (H) catalog numbering, and KL ≠ H (verified live:
KL16, the Diamond Cutter, is MW26071_0018 — part 0016 is the
Perfection of Wisdom in 500 Lines). The app's follow-along must
therefore route KL files through THIS concordance, never through
numeric equality.

Our side: every KL file carries its ACIP title in the filename,
converted to wylie via the canonical oracle (Handoff
hgm_tools.acip_to_ewts). BDRC side: part titles harvested from the
IIIF collection labels (data/extracted/mw26071_part_titles.tsv),
already wylie.

Match tiers (the Degé recipe): exact → genitive-particle-fold →
guarded containment (unique hit, ≥60% length ratio). Window ±30.
Unmatched and ambiguous are listed, never guessed.

  output  data/extracted/kl_bdrc_concordance.json
"""
import json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB = os.path.join(ROOT, "library", "kangyur")
LIB_ALT = "/Applications/ALL Translation Tool/ALL Tool Data/library/kangyur"
TSV = os.path.join(ROOT, "data", "extracted",
                   "mw26071_part_titles.tsv")
OUT = os.path.join(ROOT, "data", "extracted",
                   "kl_bdrc_concordance.json")
HANDOFF = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools")

sys.path.insert(0, HANDOFF)
try:
    from hgm_tools import acip_to_ewts   # the canonical oracle
except ImportError:
    sys.exit("canonical engine not reachable at " + HANDOFF)

KLFILE = re.compile(r"KL(\d{5})[A-Z0-9]*_([^_]+)_?")

PA_BA = {"ba": "pa", "ba'i": "pa'i", "bar": "par", "bo": "po",
         "bas": "pas", "bo'i": "po'i", "du": "tu"}


def norm(w):
    w = w.lower()
    w = re.sub(r"[^a-z'+ ]+", " ", w)
    return " ".join(PA_BA.get(t, t) for t in w.split())


def pfree(s):
    return " ".join(t for t in s.split()
                    if t not in ("kyi", "gyi", "gi", "'phags", "pa"))


def kl_titles():
    root = LIB if os.path.isdir(LIB) else LIB_ALT
    out = {}
    for dirp, _, files in os.walk(root):
        for fn in files:
            m = KLFILE.match(fn)
            if not m:
                continue
            n = int(m.group(1))
            if n in out:
                continue
            try:
                out[n] = norm(acip_to_ewts(m.group(2)))
            except Exception:
                continue
    return out


def part_titles():
    out = {}
    for line in open(TSV, encoding="utf-8"):
        if "\t" not in line:
            continue
        n, t = line.rstrip("\n").split("\t", 1)
        out[int(n)] = norm(t)
    return out


def main():
    kl = kl_titles()
    parts = part_titles()
    print(f"{len(kl)} KL titles (filenames via the oracle) · "
          f"{len(parts)} MW26071 part titles (BDRC IIIF)")
    mapping, kind, unmatched, ambiguous = {}, {}, [], []
    for n, title in sorted(kl.items()):
        window = range(max(1, n - 30), n + 31)
        hits, how = [], "exact"
        for t in window:
            if parts.get(t, "") == title and title:
                hits.append(t)
        if not hits:
            tp = pfree(title)
            for t in window:
                lab = parts.get(t, "")
                if lab and tp and pfree(lab) == tp:
                    hits.append(t)
            how = "particle-fold"
        if not hits:
            for t in window:
                lab = parts.get(t, "")
                if not lab:
                    continue
                lo, hi = sorted((len(lab), len(title)))
                if lo >= 15 and lo / hi >= 0.6 and \
                   (lab in title or title in lab):
                    hits.append(t)
            how = "contained"
        hits = sorted(set(hits))
        if len(hits) == 1:
            mapping[str(n)] = hits[0]
            kind[str(n)] = how
        elif len(hits) > 1:
            ambiguous.append({"kl": n, "candidates": hits,
                              "tier": how})
        else:
            unmatched.append(n)

    # ---- drift-interpolated second pass: for texts the fixed ±30
    # window missed (drift exceeds it at high KL numbers), center a
    # tight window on the offset interpolated between surrounding
    # EXACT-tier anchors; exact/particle tiers only — containment is
    # not allowed to reach this far.
    anchors = sorted((int(k), v) for k, v in mapping.items()
                     if kind[k] == "exact")
    recovered = []
    for n in sorted(unmatched):
        title = kl.get(n, "")
        if not title or not anchors:
            continue
        lo = [a for a in anchors if a[0] <= n]
        hi = [a for a in anchors if a[0] >= n]
        if lo and hi:
            (k0, p0), (k1, p1) = lo[-1], hi[0]
            exp = p0 if k1 == k0 else \
                p0 + (p1 - p0) * (n - k0) / (k1 - k0)
        elif lo:
            exp = lo[-1][1] + (n - lo[-1][0])
        else:
            exp = hi[0][1] + (n - hi[0][0])
        center = int(round(exp))
        window = range(max(1, center - 15), center + 16)
        hits, how = [], "exact-interp"
        for t in window:
            if parts.get(t, "") == title:
                hits.append(t)
        if not hits:
            tp = pfree(title)
            for t in window:
                lab = parts.get(t, "")
                if lab and tp and pfree(lab) == tp:
                    hits.append(t)
            how = "particle-fold-interp"
        hits = sorted(set(hits))
        if len(hits) == 1:
            mapping[str(n)] = hits[0]
            kind[str(n)] = how
            recovered.append(n)
    unmatched = [n for n in unmatched if n not in set(recovered)]
    if recovered:
        print(f"drift-interpolated pass recovered "
              f"{len(recovered)}: {recovered[:10]}…")

    # ---- order-inversion pruning to fixpoint (the Degé sanity
    # check, tier-ranked): both catalogs are in canonical order, so
    # kl_i < kl_j must give part_i <= part_j. On an inversion the
    # LOWER-CONFIDENCE side is the false positive; equal confidence
    # means we cannot tell — both leave the mapping as ambiguous.
    RANK = {"exact": 0, "exact-interp": 1, "particle-fold": 2,
            "particle-fold-interp": 3, "contained": 4}
    dropped_inv = 0
    changed = True
    while changed:
        changed = False
        keys = sorted(int(k) for k in mapping)
        for a, b in zip(keys, keys[1:]):
            if mapping[str(a)] <= mapping[str(b)]:
                continue
            ra, rb = RANK[kind[str(a)]], RANK[kind[str(b)]]
            victims = [a] if ra > rb else [b] if rb > ra else [a, b]
            for v in victims:
                if len(victims) == 2:
                    ambiguous.append(
                        {"kl": v,
                         "candidates": [mapping[str(v)]],
                         "tier": kind[str(v)] + "-inverted"})
                else:
                    unmatched.append(v)
                del mapping[str(v)]
                del kind[str(v)]
                dropped_inv += 1
            changed = True
            break
    if dropped_inv:
        print(f"order-inversion pruning dropped {dropped_inv}")
    unmatched = sorted(set(unmatched))

    json.dump({
        "meta": {
            "method": "title match KL filename (oracle wylie) vs "
                      "BDRC MW26071 IIIF part labels; tiers exact/"
                      "particle-fold/guarded-containment, window "
                      "±30; unmatched/ambiguous listed, never "
                      "guessed. Root cause: MW26071 parts follow H "
                      "numbering, KL≠H (KL16=part 0018, verified "
                      "live 2026-08-13).",
            "kl": len(kl), "parts": len(parts),
            "matched": len(mapping),
            "ambiguous": len(ambiguous),
            "unmatched": len(unmatched),
        },
        "kl_to_part": mapping,
        "match_kind": kind,
        "ambiguous": ambiguous,
        "unmatched": unmatched,
    }, open(OUT, "w"), indent=1)
    print(f"matched {len(mapping)} · ambiguous {len(ambiguous)} · "
          f"unmatched {len(unmatched)} -> {OUT}")


if __name__ == "__main__":
    main()
