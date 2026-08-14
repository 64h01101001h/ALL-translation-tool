#!/usr/bin/env python3
"""index_ekangyur.py — Toh-number index over the Esukhia
derge-kangyur e-text (Public Domain per its README §License;
cloned 2026-08-13 to editions/derge-kangyur-esukhia, kept out of
git and out of the press — 336 MB reference corpus).

The e-text carries {DNNNN} markers at each Tohoku text's start and
[NNa.L] folio.line markers throughout — so Toh → (volume file,
byte offset, folio) is a single scan.

  output  data/extracted/ekangyur_index.json
"""
import json, os, re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
import sys
TENGYUR = "--tengyur" in sys.argv
REPO = ("derge-tengyur-esukhia" if TENGYUR
        else "derge-kangyur-esukhia")
SRC = os.path.join(ROOT, "editions", REPO, "text")
OUT = os.path.join(ROOT, "data", "extracted",
                   "etengyur_index.json" if TENGYUR
                   else "ekangyur_index.json")

DMARK = re.compile(r"\{D(\d+)\}")
FOLIO = re.compile(r"\[(\d+[ab])(?:\.\d+)?\]")


def main():
    index = {}
    files = sorted(os.listdir(SRC))
    for fn in files:
        if not fn.endswith(".txt"):
            continue
        text = open(os.path.join(SRC, fn),
                    encoding="utf-8").read()
        for m in DMARK.finditer(text):
            toh = m.group(1)
            if toh in index:
                continue   # first occurrence = the text's start
            fm = None
            for f in FOLIO.finditer(text[:m.start()]):
                fm = f.group(1)
            index[toh] = {"file": fn, "offset": m.start(),
                          "folio": fm or ""}
    json.dump({
        "meta": {
            "source": f"Esukhia {REPO} (Public Domain per "
                      "README §License), cloned 2026-08-13",
            "volumes": len(files),
            "texts": len(index),
        },
        "toh": index,
    }, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"{len(index)} Toh texts indexed across {len(files)} "
          f"volumes -> {OUT}")


if __name__ == "__main__":
    main()
