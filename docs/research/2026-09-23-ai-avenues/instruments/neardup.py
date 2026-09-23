#!/usr/bin/env python3
"""Read-only: how many of the mandated, still-unread, non-exact-duplicate
segments have a NEAR-duplicate (lexical, both sides) that is already landed
or is another remaining segment? Lexical shingles only -- no model."""
import collections, json, re, sqlite3

ROOT = "/Users/adamderickandrade/ALL-translation-tool"
L = json.load(open(ROOT + "/data/alignment/alignment_full_v1.json"))["links"]
landed = {(l["course"], l["seg"]) for l in L if l["seg"]}
db = sqlite3.connect(ROOT + "/build/hgm_spine_v27_2.db")
rows = db.execute("select course, seq, coalesce(wylie,''), coalesce(english,'')"
                  " from corpus_segments").fetchall()
MAND = {"C%02d" % i for i in range(1, 19)} - {"C13"}
seg = {(c, s): (w.strip(), e.strip()) for c, s, w, e in rows
       if c in MAND or (c, s) in landed}
SYL = re.compile(r"[A-Za-z0-9'+.\-~^?]+")
WRD = re.compile(r"[a-z0-9']+")


def sh(tokens, n=3):
    if len(tokens) < n:
        return {" ".join(tokens)} if tokens else set()
    return {" ".join(tokens[i:i + n]) for i in range(len(tokens) - n + 1)}


T = {k: sh(SYL.findall(w)) for k, (w, e) in seg.items()}
E = {k: sh(WRD.findall(e.lower())) for k, (w, e) in seg.items()}

# exact duplicates: first occurrence (landed first, then course/seq order) wins
first = {}
remaining, exactdup = [], 0
order = sorted(seg, key=lambda k: (k not in landed, k))
for k in order:
    key = seg[k]
    if key in first:
        if k not in landed and k[0] in MAND:
            exactdup += 1
        continue
    first[key] = k
    if k not in landed and k[0] in MAND:
        remaining.append(k)
print("mandate segments", sum(1 for k in seg if k[0] in MAND))
print("landed", len(landed), "remaining unique", len(remaining),
      "exact dups skipped", exactdup)

reps = set(first.values())
df = collections.Counter()
for k in reps:
    for g in T[k]:
        df[g] += 1
inv = collections.defaultdict(list)
for k in reps:
    for g in T[k]:
        if df[g] <= 40:
            inv[g].append(k)


def jac(a, b):
    return len(a & b) / len(a | b) if a and b else 0.0


rem = set(remaining)
best = {}
for k in remaining:
    cands = collections.Counter()
    for g in T[k]:
        if df[g] <= 40:
            for o in inv[g]:
                if o != k:
                    cands[o] += 1
    bt = (0, 0, None)
    for o, _ in cands.most_common(50):
        jt, je = jac(T[k], T[o]), jac(E[k], E[o])
        m = min(jt, je)
        if m > min(bt[0], bt[1]):
            bt = (jt, je, o)
    best[k] = bt

for thr in (0.9, 0.8, 0.7, 0.5):
    tl = sum(1 for k, (jt, je, o) in best.items()
             if o and min(jt, je) >= thr and o in landed)
    tr = sum(1 for k, (jt, je, o) in best.items()
             if o and min(jt, je) >= thr)
    print("near-dup both sides >= %.1f : %4d of %d remaining (%.1f%%); "
          "partner already landed: %d" % (thr, tr, len(remaining),
                                          100.0 * tr / len(remaining), tl))
bycourse = collections.Counter(k[0] for k, (jt, je, o) in best.items()
                               if o and min(jt, je) >= 0.8)
print("by course (>=0.8):", dict(sorted(bycourse.items())))
# tibetan-only near dup (english differs) -- liturgy retranslated
tonly = sum(1 for k, (jt, je, o) in best.items() if o and jt >= 0.8 and je < 0.5)
print("tibetan >=0.8 but english <0.5 (same text, different rendering):", tonly)
ex = [(k, v) for k, v in best.items() if v[2] and 0.8 <= min(v[0], v[1]) < 1][:5]
for k, (jt, je, o) in ex:
    print(k, o, round(jt, 2), round(je, 2))
    print("   ", seg[k][0][:110]); print("   ", seg[o][0][:110])
    print("   ", seg[k][1][:110]); print("   ", seg[o][1][:110])
