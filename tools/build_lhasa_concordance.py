#!/usr/bin/env python3
"""build_lhasa_concordance.py — ACIP KL number ⇄ THL Lhasa (H)
number concordance, by title match (the Degé recipe applied to the
Lhasa Kangyur; KL≠H numerically — refuted 2026-08-13 at H.11).

Our side of the match comes from the library itself: every KL file
carries its ACIP Tibetan title in the filename
(KL00011E1_'PHAGS PA SHES RAB ..._English.txt → middle segment),
converted to wylie through the canonical engine (Handoff
hgm_tools.acip_to_ewts — the oracle, never re-implemented).

  inputs   the installed library's kangyur tree (KL filenames)
           data/extracted/thl_lhasa_records/hNNNN.html
  output   data/extracted/kl_lhasa_concordance.json

Match tiers (as the Degé builder): exact → genitive-particle-fold
→ guarded containment (unique hit, ≥60% length ratio). Window is
wide (±30) because the two orderings genuinely diverge. Unmatched
and ambiguous are listed, never guessed.
"""
# NOTE 2026-08-26: the raw THL records were removed from the repo
# (THL's own prose; notices promise numbers-only). The scrape cache
# is banked at /Volumes/Oct2024(8TB)/ALL-ingest-caches/
# thl_records_2026-08-26/ - point THL there to re-derive.
import html, json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB = os.path.join(ROOT, "library", "kangyur")
# both install paths: the tool was renamed 2026-08-22 and an
# already-installed copy still lives under the old one
LIB_ALTS = ["/Applications/Diamond Cutter Translation Tool/Diamond Cutter Tool Data/library/kangyur",
            "/Applications/ALL Translation Tool/ALL Tool Data/library/kangyur"]
LIB_ALT = next((p for p in LIB_ALTS if os.path.isdir(p)), LIB_ALTS[0])
THL = os.path.join(ROOT, "data", "extracted", "thl_lhasa_records")
OUT = os.path.join(ROOT, "data", "extracted",
                   "kl_lhasa_concordance.json")
HANDOFF = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools")

sys.path.insert(0, HANDOFF)
try:
    from hgm_tools import acip_to_ewts   # the canonical oracle
except ImportError:
    sys.exit("canonical engine not reachable at " + HANDOFF)

TITLE = re.compile(r'class="wyl bibltitle">([^<]+)<')
KLFILE = re.compile(r"KL(\d{5})[A-Z0-9]*_([^_]+)_")

PA_BA = {"ba": "pa", "ba'i": "pa'i", "bar": "par", "bo": "po",
         "bas": "pas", "bo'i": "po'i"}


def norm(w):
    w = w.lower()
    w = re.sub(r"[^a-z'+ ]+", " ", w)
    return " ".join(PA_BA.get(t, t) for t in w.split())


def pfree(s):
    return " ".join(t for t in s.split()
                    if t not in ("kyi", "gyi", "gi"))


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
                continue   # first part's title stands for the work
            acip_title = m.group(2)
            try:
                wy = acip_to_ewts(acip_title)
            except Exception:
                continue
            out[n] = norm(wy)
    return out


def thl_titles():
    out = {}
    for fn in sorted(os.listdir(THL)):
        m = re.match(r"h(\d{4})\.html", fn)
        if not m:
            continue
        t = TITLE.search(open(os.path.join(THL, fn),
                              errors="replace").read())
        if t:
            out[int(m.group(1))] = norm(html.unescape(t.group(1)))
    return out


def main():
    kl = kl_titles()
    th = thl_titles()
    print(f"{len(kl)} KL titles (from filenames, via the oracle) · "
          f"{len(th)} THL H titles")
    mapping, kind, unmatched, ambiguous = {}, {}, [], []
    for n, title in sorted(kl.items()):
        window = range(max(1, n - 30), n + 31)
        hits, how = [], "exact"
        for t in window:
            if any(lab == title for lab in [th.get(t, "")] if lab):
                hits.append(t)
        if not hits:
            tp = pfree(title)
            for t in window:
                lab = th.get(t, "")
                if lab and pfree(lab) == tp:
                    hits.append(t)
            how = "particle-fold"
        if not hits:
            for t in window:
                lab = th.get(t, "")
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
            "method": "title match, tiers exact/particle-fold/"
                      "guarded-containment, window ±30; KL titles "
                      "from library filenames via the canonical "
                      "acip_to_ewts; unmatched/ambiguous listed, "
                      "never guessed",
            "kl": len(kl), "thl_h": len(th),
            "matched": len(mapping),
            "ambiguous": len(ambiguous),
            "unmatched": len(unmatched),
        },
        "kl_to_h": mapping,
        "match_kind": kind,
        "ambiguous": ambiguous,
        "unmatched": unmatched,
    }, open(OUT, "w"), indent=1)
    print(f"matched {len(mapping)} · ambiguous {len(ambiguous)} · "
          f"unmatched {len(unmatched)} -> {OUT}")


if __name__ == "__main__":
    main()
