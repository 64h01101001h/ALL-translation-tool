#!/usr/bin/env python3
"""build_misspelling_layer.py — extract the Illuminator's own misspelling
judgements, stratified by how safely each can be acted on.

WHY THE ILLUMINATOR. Tony Duff names it as the companion to his grammar
volumes (Standard Tibetan Grammar Vol II p.213: "the Illuminator Encyclopaedic
Tibetan-English Dictionary contains extensive definitions of all terms found in
Tibetan grammar texts"). Its misspelling notes are PKTC's judgement, not ours,
which is what makes them usable — this project may MATCH evidence, never
compose it.

WHY IT IS STRATIFIED, which is the entire point. A naive "does the definition
mention a mis-spelling" scan is badly wrong and says so loudly if you look:
it returns 2,138 corpus hits on `rtsa`, a 1,754-character entry with many
senses where the note is one minor sense, and it returns BOTH `rig`->`rigs`
AND `rigs`->`rig`, which cannot both be true. `rig` reads "1) From the verb
རིག་པ་ 2) Common mis-spelling of རིགས་" — an ordinary word that also carries a
misspelling sense. Flagging those would tell a translator that `dka' ba`
("difficult") and `khru` ("cubit") are errors.

  TIER A  the whole entry is an unhedged misspelling note and nothing else.
          Safe to surface as a correction, attributed to the Illuminator.
  TIER B  sole sense but hedged ("Sometimes seen as a mis-spelling of…").
          Surface as a possibility. Never as a fact.
  TIER C  the headword is ALSO a legitimate word. NEVER flag blindly.

Usage:
  python3 tools/build_misspelling_layer.py --archive <path-to-illuminator.sqlite>
                                           [--out data/misspellings.json]
Skips cleanly when the archive is absent — it is Adam's, outside this repo.
"""
import io
import json
import os
import re
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_OUT = os.path.join(ROOT, "data", "misspellings.json")

NOTE = re.compile(
    r"(?:mis-spelling|mistaken spelling|incorrect spelling|corruption) of\s+([ༀ-࿿]+)")
HEDGE = re.compile(r"\b(sometimes|occasionally|may be|can be|in some)\b", re.I)
# a second sense, in either numbering style the dictionary uses
SENSES = re.compile(r"(\b[IVX]{1,4}\.\s)|(\b[2-9]\)\s)")


def main():
    if "--archive" not in sys.argv:
        print("  usage: --archive <illuminator.sqlite> [--out PATH]")
        return 2
    arch = sys.argv[sys.argv.index("--archive") + 1]
    out = (sys.argv[sys.argv.index("--out") + 1]
           if "--out" in sys.argv else DEFAULT_OUT)
    if not os.path.exists(arch):
        print("  [SKIP] no Illuminator archive at %s — it lives outside this "
              "repo and a clone will not have it." % arch)
        return 0

    c = sqlite3.connect(arch)
    t2w = {}
    for tib, tr in c.execute(
            "select tibetan,transliteration from records where tibetan is not null"):
        if tib and tr and tib not in t2w:
            t2w[tib] = tr

    tiers = {"A": [], "B": [], "C": []}
    scanned = 0
    for tr, tib, d in c.execute(
            """select transliteration,tibetan,definition_text from records
               where definition_text like '%mis-spelling%'
                  or definition_text like '%mistaken spelling%'
                  or definition_text like '%incorrect spelling%'
                  or definition_text like '%corruption of%'"""):
        scanned += 1
        d = d or ""
        body = re.sub(r"^\s*»[^\n]*\n", "", d, count=1).strip()
        body = re.sub(r"^\s*»\s*", "", body)
        m = NOTE.search(body)
        if not m:
            continue
        tgt = m.group(1).rstrip("་།")
        right = t2w.get(tgt) or t2w.get(tgt + "་")
        if not right or not tr or right == tr:
            continue
        if SENSES.search(body) or len(body) > 170:
            tier = "C"
        elif HEDGE.search(body):
            tier = "B"
        else:
            tier = "A"
        tiers[tier].append({"wrong": tr, "right": right,
                            "illuminator_says": " ".join(body.split())[:200]})

    for t in tiers:
        tiers[t].sort(key=lambda r: r["wrong"])
    doc = {
        "source": "Illuminator Encyclopaedic Tibetan-English Dictionary (2016), "
                  "supplied by Adam 2026-09-16",
        "generated_by": "tools/build_misspelling_layer.py",
        "records_flagging_a_spelling_problem": scanned,
        "tier_meaning": {
            "A": "whole entry is an unhedged misspelling note; safe to surface "
                 "as a correction, attributed to the Illuminator",
            "B": "sole sense but hedged; surface as a possibility, never a fact",
            "C": "headword is ALSO a legitimate word; never flag blindly",
        },
        "counts": {t: len(tiers[t]) for t in "ABC"},
        "tiers": tiers,
    }
    os.makedirs(os.path.dirname(out), exist_ok=True)
    io.open(out, "w", encoding="utf-8").write(
        json.dumps(doc, ensure_ascii=False, indent=1) + "\n")
    print("  wrote %s" % os.path.relpath(out, ROOT))
    print("  scanned %d flagged records -> A=%d  B=%d  C=%d"
          % (scanned, len(tiers["A"]), len(tiers["B"]), len(tiers["C"])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
