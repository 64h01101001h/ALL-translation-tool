#!/usr/bin/env python3
"""build_transcriber_lexicon.py — the keyterm list for the GMR
recordings re-transcription program (Adam, 2026-08-12; survey:
docs/research/TRANSCRIBER_SURVEY.md).

Every commercial transcriber caps custom vocabulary near 1,000
terms, and vocabulary must be supplied in the form the words are
SPOKEN (phonetics — "sönam", "kamdir"), not Wylie. This tool emits:

  data/teaching/transcriber_lexicon.tsv
      rank · phonetic (GMR convention) · wylie · hgm_gloss
      — the full mapping table (post-pass: phonetics → canonical
        Wylie after transcription)
  data/teaching/transcriber_keyterms.txt
      the top-N phonetic forms alone, one per line, ready to paste
      into a keyterms/word-boost field

Ranking = how often the term's exact wylie occurs across the
aligned corpus (the best available proxy for how often Geshe
Michael says it in teachings). HGM-glossed entries only — these
are the terms whose recognition matters. Deterministic; no
guessing: entries without a stored pronunciation are skipped, not
synthesized here.

Usage: build_transcriber_lexicon.py [--top N]   (default 1000)
"""
import json, os, re, sqlite3, sys
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUTDIR = os.path.join(ROOT, "data", "teaching")

def main():
    top = 1000
    if "--top" in sys.argv:
        top = int(sys.argv[sys.argv.index("--top") + 1])
    db = sqlite3.connect(DB)

    # corpus wylie word-sequence counts (up to 4-syllable windows)
    seq = Counter()
    for (w,) in db.execute("SELECT wylie FROM corpus_segments"):
        toks = [t for t in re.split(r"[^a-zA-Z'+.]+", w or "")
                if t]
        for n in (1, 2, 3, 4):
            for i in range(len(toks) - n + 1):
                seq[" ".join(toks[i:i + n])] += 1

    rows = db.execute(
        "SELECT wylie, raw FROM entries").fetchall()
    picks = []
    seen_pron = set()
    for wylie, raw in rows:
        d = json.loads(raw)
        if not d.get("hgm_gloss"):
            continue
        pron = (d.get("pronunciation") or "").strip()
        if not pron:
            continue   # never synthesized here
        freq = seq.get(wylie.strip().lower(), 0)
        if freq == 0:
            continue
        # keyterm slots are scarce (services cap near 1,000): spend
        # them on real vocabulary, not grammar particles — keep
        # multi-syllable terms, and single syllables only when the
        # spoken form is long enough to be distinctive
        syls = len(wylie.split())
        if syls < 2 and len(re.sub(r"[^a-zA-Zäöü]", "",
                                   pron)) < 5:
            continue
        key = pron.lower()
        if key in seen_pron:
            continue   # one keyterm slot per spoken form
        seen_pron.add(key)
        gloss = d.get("hgm_gloss") or ""
        if isinstance(gloss, list):
            gloss = "; ".join(map(str, gloss))
        picks.append((freq, pron, wylie, str(gloss)[:120]))
    picks.sort(key=lambda x: (-x[0], x[2]))
    picks = picks[:top]

    os.makedirs(OUTDIR, exist_ok=True)
    tsv = os.path.join(OUTDIR, "transcriber_lexicon.tsv")
    with open(tsv, "w") as f:
        f.write("rank\tcorpus_freq\tphonetic_gmr\twylie\t"
                "hgm_gloss\n")
        for i, (freq, pron, wylie, gloss) in enumerate(picks, 1):
            f.write(f"{i}\t{freq}\t{pron}\t{wylie}\t{gloss}\n")
    txt = os.path.join(OUTDIR, "transcriber_keyterms.txt")
    with open(txt, "w") as f:
        for _, pron, _, _ in picks:
            f.write(pron + "\n")
    print(f"{len(picks)} keyterms -> {tsv} + {txt}")
    for row in picks[:10]:
        print("  ", row)

if __name__ == "__main__":
    main()
