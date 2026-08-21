#!/usr/bin/env python3
"""Meter census across the installed collections (Adam 2026-08-14:
"understanding the number of syllables per line in meter of tengyur
texts and other places it is highly used").

Mirrors the app's meterUnits()/meterBlocks() logic exactly
(app/main.cpp): shad-delimited units (',' ';' '\n' in ACIP), tokens
are letter/'/+ runs, [bracket] spans skipped; verse blocks = mode of
an 8-unit window, odd meters >=5 (best>=3) or even meters >=6
(best>=4, the stricter song-meter rule), extended while deviations
(|d|<=2) stay under 25%, kept at >=4 lines.

Output: data/extracted/meter_census.json
"""
import json, os, re, sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIB = os.path.join(ROOT, "library")
OUT = os.path.join(ROOT, "data", "extracted", "meter_census.json")

TOK = re.compile(r"[A-Za-z'+]+")


def units_of(text):
    text = re.sub(r"\[[^\]]*\]", " ", text)
    out = []
    for unit in re.split(r"[,;\n]", text):
        n = len(TOK.findall(unit))
        if n:
            out.append(n)
    return out


def blocks_of(units):
    blocks = []
    n = len(units)
    i = 0
    while i < n:
        freq = Counter(units[i:i + 8])
        meter, best = 0, 0
        for m, c in sorted(freq.items()):
            odd_ok = m >= 5 and m % 2 == 1 and c > best and c >= 3
            even_ok = m >= 6 and m % 2 == 0 and c > best and c >= 4
            if odd_ok or even_ok:
                meter, best = m, c
        if not meter:
            i += 1
            continue
        first = last = i
        off = length = 0
        j = i
        while j < n:
            if units[j] == meter:
                last = j
                length += 1
            elif abs(units[j] - meter) <= 2 and off * 4 < length:
                last = j
                off += 1
                length += 1
            else:
                break
            j += 1
        if length >= 4:
            blocks.append((meter, first, last))
        i = last + 1
    return blocks


def main():
    per_coll = {}
    meter_texts = defaultdict(Counter)   # meter -> {file: lines}
    for coll in ("kangyur", "tengyur", "sungbum"):
        base = os.path.join(LIB, coll)
        if not os.path.isdir(base):
            continue
        tot_units = 0
        verse_lines = Counter()
        nfiles = 0
        for dirpath, _dirs, files in os.walk(base):
            for fn in files:
                if not fn.lower().endswith((".txt", ".act", ".inc")):
                    continue
                nfiles += 1
                try:
                    with open(os.path.join(dirpath, fn),
                              encoding="utf-8",
                              errors="replace") as f:
                        text = f.read()
                except OSError:
                    continue
                u = units_of(text)
                tot_units += len(u)
                for meter, first, last in blocks_of(u):
                    lines = last - first + 1
                    verse_lines[meter] += lines
                    meter_texts[meter][fn.split("_")[0]] += lines
        per_coll[coll] = {
            "files": nfiles,
            "units": tot_units,
            "verse_lines_by_meter": dict(
                sorted(verse_lines.items())),
        }
        print(f"{coll}: {nfiles} files, {tot_units} units, "
              f"verse {sum(verse_lines.values())}", flush=True)

    total_verse = sum(
        sum(c["verse_lines_by_meter"].values())
        for c in per_coll.values())
    meters = {}
    for coll in per_coll.values():
        for m, c in coll["verse_lines_by_meter"].items():
            meters[m] = meters.get(m, 0) + c
    summary = {
        m: {
            "verse_lines": c,
            "share_pct": round(100.0 * c / total_verse, 2)
            if total_verse else 0,
            "top_texts": meter_texts[m].most_common(5),
        }
        for m, c in sorted(meters.items())
    }
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        json.dump(
            {
                "note": "meter census over the installed "
                        "collections; logic mirrors the app's "
                        "meterUnits/meterBlocks (odd>=5 best>=3, "
                        "even>=6 best>=4 song rule, len>=4)",
                "collections": per_coll,
                "meters": summary,
                "total_verse_lines": total_verse,
            },
            f, indent=1)
    print(f"wrote {OUT}: {total_verse} verse lines, "
          f"meters {sorted(meters)}")


if __name__ == "__main__":
    sys.exit(main())
