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
