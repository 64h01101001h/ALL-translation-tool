#!/usr/bin/env python3
"""build_thl_concordance.py — Degé Kangyur: THL catalog number ⇄
Tohoku number concordance.

Background: docs/research/THL/dege_tohoku_verification.md REFUTED
the assumed THL==Toh numbering (a growing offset, +0 → +10, plus
THL-only insertions), so deep-linking by number is unsafe. This
tool builds the real mapping by TITLE MATCH:

  inputs   data/extracted/thl_dege_records/dNNNN.html
             (harvested from old.thlib.org cat_reader.php)
           data/extracted/bdrc_toh_labels/tohN.json
             (BDRC MW22084_NNNN — 84000's RDF asserts these part
              numbers ARE Tohoku numbers)
  output   data/extracted/thl_dege_concordance.json
             {"thl_to_toh": {…}, "unmatched": […], "meta": …}

Method: normalized-wylie equality within a sliding window (the
verified offset never exceeded +10 by D.1100; window is ±0..+14 to
be safe). Only UNIQUE window matches are accepted; everything else
is listed unmatched — never guessed (rule 3).
"""
import html, json, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
THL = os.path.join(ROOT, "data", "extracted", "thl_dege_records")
BDRC = os.path.join(ROOT, "data", "extracted", "bdrc_toh_labels")
OUT = os.path.join(ROOT, "data", "extracted",
                   "thl_dege_concordance.json")

TITLE = re.compile(r'class="wyl bibltitle">([^<]+)<')


PA_BA = {"ba": "pa", "ba'i": "pa'i", "bar": "par", "bo": "po",
         "bas": "pas", "bo'i": "po'i"}


def norm(w):
    """lowercase, strip punctuation, and fold the ba/pa suffix
    alternation token-wise — the same orthographic normalization
    BDRC's own lucene-bo applies (ported in allcore affixnorm)."""
    w = w.lower()
    w = re.sub(r"[^a-z'+ ]+", " ", w)
    toks = [PA_BA.get(t, t) for t in w.split()]
    return " ".join(toks)


def thl_titles():
    out = {}
    for fn in sorted(os.listdir(THL)):
        m = re.match(r"d(\d{4})\.html", fn)
        if not m:
            continue
        t = TITLE.search(open(os.path.join(THL, fn),
                              errors="replace").read())
        if t:
            out[int(m.group(1))] = norm(html.unescape(t.group(1)))
    return out


def bdrc_labels():
    out = {}
    for fn in sorted(os.listdir(BDRC)):
        m = re.match(r"toh(\d+)\.json", fn)
        if not m:
            continue
        try:
            d = json.load(open(os.path.join(BDRC, fn)))
        except Exception:
            continue
        n = int(m.group(1))
        # purl.bdrc.io serves expanded RDF/JSON: keys are full
        # predicate URIs, literals are {"value","lang"}; accept
        # the JSON-LD compact shape too. ALL bo-x-ewts labels in
        # the file count — the title entities carry the variant
        # titles, which raise the match rate honestly.
        def walk(x):
            if isinstance(x, dict):
                for k, v in x.items():
                    if k.endswith("prefLabel") or \
                       k.endswith("altLabel"):
                        vals = v if isinstance(v, list) else [v]
                        for it in vals:
                            if not isinstance(it, dict):
                                continue
                            lang = it.get("lang",
                                          it.get("@language"))
                            if lang == "bo-x-ewts":
                                yield it.get("value",
                                             it.get("@value", ""))
                    else:
                        yield from walk(v)
            elif isinstance(x, list):
                for it in x:
                    yield from walk(it)
        labels = [l for l in walk(d) if l]
        if labels:
            out[n] = [norm(l) for l in labels]
    return out


def main():
    thl = thl_titles()
    toh = bdrc_labels()
    print(f"{len(thl)} THL titles · {len(toh)} BDRC label sets")
    mapping, kind, unmatched, ambiguous = {}, {}, [], []
    for d, title in sorted(thl.items()):
        # verified offset grows: Toh candidate = d-14 … d+2
        window = range(max(1, d - 14), d + 3)
        hits = []
        for t in window:
            if any(lab == title for lab in toh.get(t, [])):
                hits.append(t)
        hits = sorted(set(hits))
        how = "exact"
        if not hits:
            # tier 2: exact after dropping free-standing genitive
            # particles — titles alternate kyi/gyi/gi freely
            # (found via the D.828 inversion: real mate differed
            # only by one "kyi")
            def pfree(s):
                return " ".join(t for t in s.split()
                                if t not in ("kyi", "gyi", "gi"))
            tp = pfree(title)
            for t in window:
                if any(pfree(lab) == tp
                       for lab in toh.get(t, [])):
                    hits.append(t)
            hits = sorted(set(hits))
            how = "particle-fold"
        if not hits:
            # tier 3, labeled: one title contains the other
            # (long-form vs short-form conventions) — unique hit
            # only, and the shorter must be ≥60% of the longer so
            # a generic stub title cannot swallow a long one
            for t in window:
                for lab in toh.get(t, []):
                    lo, hi = sorted((len(lab), len(title)))
                    if lo >= 15 and lo / hi >= 0.6 and \
                       (lab in title or title in lab):
                        hits.append(t)
                        break
            hits = sorted(set(hits))
            how = "contained"
        if len(hits) == 1:
            mapping[str(d)] = hits[0]
            kind[str(d)] = how
        elif len(hits) > 1:
            ambiguous.append({"thl": d, "candidates": hits,
                              "tier": how})
        else:
            unmatched.append(d)
    json.dump({
        "meta": {
            "method": "normalized-wylie exact title equality, "
                      "window d-14..d+2 (per the verified drifting "
                      "offset); unique matches only — ambiguous "
                      "and unmatched listed, never guessed",
            "thl_records": len(thl),
            "bdrc_records": len(toh),
            "matched": len(mapping),
            "ambiguous": len(ambiguous),
            "unmatched": len(unmatched),
            "sources": ["old.thlib.org cat_reader (kt/d)",
                        "purl.bdrc.io MW22084 parts"],
        },
        "thl_to_toh": mapping,
        "match_kind": kind,
        "ambiguous": ambiguous,
        "unmatched": unmatched,
    }, open(OUT, "w"), indent=1)
    print(f"matched {len(mapping)} · ambiguous {len(ambiguous)} · "
          f"unmatched {len(unmatched)} -> {OUT}")


if __name__ == "__main__":
    main()
