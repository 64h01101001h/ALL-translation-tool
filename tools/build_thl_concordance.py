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


def norm(w):
    w = w.lower()
    w = re.sub(r"[^a-z'+ ]+", " ", w)
    return " ".join(w.split())


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
        # JSON-LD: find any prefLabel valued in bo-x-ewts
        def walk(x):
            if isinstance(x, dict):
                for k, v in x.items():
                    if k.endswith("prefLabel"):
                        vals = v if isinstance(v, list) else [v]
                        for it in vals:
                            if isinstance(it, dict) and \
                               it.get("@language") == "bo-x-ewts":
                                yield it.get("@value", "")
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
    mapping, unmatched, ambiguous = {}, [], []
    for d, title in sorted(thl.items()):
        # verified offset grows: Toh candidate = d-14 … d+2
        hits = []
        for t in range(max(1, d - 14), d + 3):
            for lab in toh.get(t, []):
                if lab == title:
                    hits.append(t)
                    break
        hits = sorted(set(hits))
        if len(hits) == 1:
            mapping[str(d)] = hits[0]
        elif len(hits) > 1:
            ambiguous.append({"thl": d, "candidates": hits})
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
        "ambiguous": ambiguous,
        "unmatched": unmatched,
    }, open(OUT, "w"), indent=1)
    print(f"matched {len(mapping)} · ambiguous {len(ambiguous)} · "
          f"unmatched {len(unmatched)} -> {OUT}")


if __name__ == "__main__":
    main()
