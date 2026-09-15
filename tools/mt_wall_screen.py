#!/usr/bin/env python3
"""mt_wall_screen.py — THE WALL, enforced in code for arbitrary
models (Adam's Hugging Face question, 2026-08-13).

Any machine-produced English destined for this project must be
screened against Geshe Michael Roach's published English before it
is kept: model weights (ours or anyone's) may have memorized his
translations, and a silent reproduction would violate the
project's first inviolable rule. This tool makes that violation
detectable: it flags any candidate text sharing a verbatim
N-word run with the aligned corpus.

  --glossary            screen data/ai_glossary/ai_glossary.json
                        (glosses + sense contexts)
  --file F              screen one candidate per line of F
  --n N                 run length in words (default 6 — long
                        enough that shared runs mean copying, not
                        shared vocabulary)
  --report PATH         write the full report (default
                        /tmp/mt_wall_screen_report.txt)

Verdicts: CLEAN or FLAGGED (with the matched run and its source
course, for human judgment — stock formulae do exist; the tool
surfaces evidence, a person rules).
"""
import argparse, json, os, re, sqlite3, sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")


def words(text):
    return re.findall(r"[a-z']+", text.lower())


def build_reference(n):
    """word-n-gram -> one example source course"""
    db = sqlite3.connect(DB)
    grams = {}
    for eng, course in db.execute(
            "SELECT english, course FROM corpus_segments "
            "WHERE english != ''"):
        ws = words(eng)
        for i in range(len(ws) - n + 1):
            g = " ".join(ws[i:i + n])
            if g not in grams:
                grams[g] = course
    return grams


def screen(cands, grams, n):
    """cands: list of (label, text) -> list of findings"""
    findings = []
    for label, text in cands:
        ws = words(text)
        hits = []
        for i in range(len(ws) - n + 1):
            g = " ".join(ws[i:i + n])
            if g in grams:
                hits.append((g, grams[g]))
        if hits:
            # collapse overlapping runs into maximal spans
            seen = set()
            uniq = []
            for g, c in hits:
                if g not in seen:
                    seen.add(g)
                    uniq.append((g, c))
            findings.append((label, text, uniq[:5]))
    return findings


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--glossary", action="store_true")
    ap.add_argument("--file")
    ap.add_argument("--n", type=int, default=6)
    ap.add_argument("--report",
                    default="/tmp/mt_wall_screen_report.txt")
    a = ap.parse_args()

    cands = []
    if a.glossary:
        d = json.load(open(os.path.join(
            ROOT, "data", "ai_glossary", "ai_glossary.json")))
        for w, e in d.get("entries", {}).items():
            parts = [e.get("gloss", "")]
            for sv in e.get("senses", []):
                parts.append(sv.get("gloss", ""))
                parts.append(sv.get("context", ""))
            cands.append((w, " … ".join(p for p in parts if p)))
    elif a.file:
        for i, line in enumerate(open(a.file), 1):
            if line.strip():
                cands.append((f"line {i}", line.strip()))
    else:
        sys.exit("give --glossary or --file")

    print(f"building {a.n}-gram reference from the corpus…",
          flush=True)
    grams = build_reference(a.n)
    print(f"{len(grams):,} reference runs · screening "
          f"{len(cands):,} candidate(s)", flush=True)
    findings = screen(cands, grams, a.n)

    with open(a.report, "w") as f:
        f.write(f"mt_wall_screen: n={a.n}, "
                f"{len(cands)} candidates, "
                f"{len(findings)} FLAGGED\n\n")
        for label, text, hits in findings:
            f.write(f"FLAGGED  {label}\n  text: {text[:200]}\n")
            for g, c in hits:
                f.write(f"  shared run: \"{g}\"  (corpus: {c})\n")
            f.write("\n")
    print(f"{len(findings)} FLAGGED of {len(cands)} "
          f"-> {a.report}")
    for label, _, hits in findings[:10]:
        print(f"  FLAGGED {label}: \"{hits[0][0]}\"")
    return 1 if findings else 0


if __name__ == "__main__":
    sys.exit(main())
