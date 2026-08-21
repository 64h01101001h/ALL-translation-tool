#!/usr/bin/env python3
"""harvest_sanskrit_clusters.py — the running list Adam asked for
(2026-08-13): every distinct Sanskrit-flavored syllable attested in
the library's ACIP keying, each tested through the canonical chain
(acip_to_ewts → wylie_to_unicode), classified ok / FLAGGED.

Sanskrit signals in ACIP keying (per the official Sanskrit Input
Code standard + observed input-center practice):
  - apostrophe long vowels ('A 'I 'U) and syllable-initial A'A
  - lowercase retroflex/palatal letters inside caps text
    (t d n sh = ṭ ḍ ṇ ṣ)
  - # (anusvara candrabindu class), % (visarga class) marks
  - characteristic clusters: DZ, TZ+aspirate, BH DH GH KSH, YH, etc.
  - explicit wa-zur clusters with V

Output:
  data/extracted/sanskrit_cluster_census.tsv
    columns: count · syllable(ACIP) · ewts · unicode · status ·
             example file
  summary printed; FLAGGED rows are the exceptions list.
"""
import os, re, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIBS = [os.path.join(ROOT, "library"),
        "/Applications/ALL Translation Tool/ALL Tool Data/library"]
HANDOFF = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools")
sys.path.insert(0, HANDOFF)
from hgm_tools import acip_to_ewts          # canonical oracle
from ewts_unicode import wylie_to_unicode   # canonical oracle

OUT = os.path.join(ROOT, "data", "extracted",
                   "sanskrit_cluster_census.tsv")

SKT = re.compile(
    r"(?:'[AIU]|A'A|[#%]|[a-z]|DZ|TZ|BH|DH(?![AE]NG)|GH|KSH|SHR|V)"
)
TOK = re.compile(r"[A-Za-z'#%+:~-]+")
SKIP_DIRS = ("ocr_out", "input_work", "scans", "ocr_models",
             "glossaries")


def main():
    root = next((p for p in LIBS if os.path.isdir(p)), None)
    if not root:
        sys.exit("no library found")
    seen = collections.Counter()
    example = {}
    nfiles = 0
    for dirp, dirs, files in os.walk(root):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for fn in files:
            if not fn.lower().endswith((".txt", ".act", ".inc")):
                continue
            p = os.path.join(dirp, fn)
            try:
                text = open(p, encoding="utf-8",
                            errors="replace").read()
            except Exception:
                continue
            nfiles += 1
            for tok in TOK.findall(text):
                if len(tok) < 2 or tok.startswith("@"):
                    continue
                # must carry a Sanskrit signal AND letters
                if not SKT.search(tok):
                    continue
                if not re.search(r"[A-Za-z]", tok):
                    continue
                seen[tok] += 1
                example.setdefault(tok, os.path.relpath(p, root))
    print(f"{nfiles} files · {len(seen)} distinct "
          f"Sanskrit-flavored syllables")
    rows = []
    flagged = 0
    for tok, n in seen.most_common():
        try:
            ew = acip_to_ewts(tok)
            r = wylie_to_unicode(ew)
            uni = r[0] if isinstance(r, tuple) else r
        except Exception as e:
            ew, uni = "ERROR", str(e)[:60]
        status = "ok"
        if "⟨" in uni or "ERROR" in ew:
            status = "FLAGGED"
            flagged += 1
        rows.append((n, tok, ew, uni, status, example[tok]))
    with open(OUT, "w", encoding="utf-8") as f:
        f.write("count\tacip\tewts\tunicode\tstatus\texample_file\n")
        for r in rows:
            f.write("\t".join(str(x) for x in r) + "\n")
    total_occ = sum(seen.values())
    flag_occ = sum(n for n, t, e, u, s, x in rows if s == "FLAGGED")
    print(f"{flagged}/{len(rows)} distinct syllables FLAGGED "
          f"({flag_occ}/{total_occ} occurrences) -> {OUT}")
    print("top flagged:")
    shown = 0
    for n, tok, ew, uni, s, x in rows:
        if s != "FLAGGED":
            continue
        print(f"  {n:6d}  {tok:20} -> {ew:24} {uni[:40]}")
        shown += 1
        if shown >= 25:
            break


if __name__ == "__main__":
    main()
