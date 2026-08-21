#!/usr/bin/env python3
"""G2 (gauntlet): rank the whole dictionary by WEIRDNESS and emit the
top slice — the tail that stride sampling is structurally blind to.

Weirdness signals (each normalized, summed):
- headword length extremes (very long AND very short)
- stacked-glyph depth (runs of subjoined U+0F90-0FBC)
- codepoints outside the core Tibetan block in the tibetan field
- mixed ASCII inside the tibetan field (markers, flags)
- variant count · flag count
- empty-field patterns (has gloss but no pronunciation, acip but no
  tibetan, etc.)
- control/format characters anywhere

Output: build/weird_top.tsv  (id \t wylie \t score \t reasons)
"""
import sqlite3
import sys
import unicodedata

db = sqlite3.connect(sys.argv[1] if len(sys.argv) > 1
                     else "build/hgm_spine_v27_2.db")
rows = db.execute(
    "SELECT id, wylie, tibetan, acip, pronunciation, hgm_gloss, "
    "flags FROM entries").fetchall()

scored = []
for eid, wylie, tib, acip, pron, gloss, flags in rows:
    w = wylie or ""
    t = tib or ""
    score = 0.0
    reasons = []
    L = len(w)
    if L > 60:
        score += min((L - 60) / 20.0, 5)
        reasons.append(f"len{L}")
    if L <= 2:
        score += 2
        reasons.append("tiny")
    run = mx = 0
    for ch in t:
        if 0x0F90 <= ord(ch) <= 0x0FBC:
            run += 1
            mx = max(mx, run)
        else:
            run = 0
    if mx >= 3:
        score += mx
        reasons.append(f"stack{mx}")
    odd = sum(1 for ch in t
              if ch and not (0x0F00 <= ord(ch) <= 0x0FDA) and
              ord(ch) > 0x7F)
    if odd:
        score += min(odd, 6)
        reasons.append(f"odd{odd}")
    ascii_in_tib = sum(1 for ch in t if ord(ch) < 0x7F and
                       not ch.isspace())
    if ascii_in_tib:
        score += min(ascii_in_tib / 2.0, 4)
        reasons.append(f"mixed{ascii_in_tib}")
    ctrl = sum(1 for ch in (w + t)
               if unicodedata.category(ch) in ("Cc", "Cf"))
    if ctrl:
        score += 5 + ctrl
        reasons.append(f"ctrl{ctrl}")
    nflags = len((flags or "").split("|")) if flags else 0
    if nflags >= 2:
        score += nflags
        reasons.append(f"flags{nflags}")
    if (acip or "") and not t:
        score += 3
        reasons.append("acip-no-tib")
    if (gloss or "") and not (pron or ""):
        score += 1
        reasons.append("gloss-no-pron")
    if score > 0:
        scored.append((score, eid, w, ";".join(reasons)))

scored.sort(reverse=True)
top = scored[:1000]
with open("build/weird_top.tsv", "w") as f:
    for score, eid, w, why in top:
        if "\t" in w or "\n" in w:
            continue
        f.write(f"{eid}\t{w}\t{score:.1f}\t{why}\n")
print(f"scored {len(scored)} nonzero of {len(rows)}; "
      f"top slice written ({len(top)} rows)")
print("weirdest 5:")
for score, eid, w, why in top[:5]:
    print(f"  {score:5.1f}  {w[:60]!r}  [{why}]")
