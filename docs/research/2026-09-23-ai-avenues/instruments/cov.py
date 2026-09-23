#!/usr/bin/env python3
"""Read-only coverage analysis over the alignment bank. Scratchpad only.
Units: passage families (segments grouped by normalised Tibetan).
Species: (a) d=5 Tibetan headword strings, (b) (tib, eng) pairs.
"""
import json, sqlite3, collections, re, math, random, sys

ROOT = "/Users/adamderickandrade/ALL-translation-tool"
d = json.load(open(ROOT + "/data/alignment/alignment_full_v1.json"))
L = d["links"]
db = sqlite3.connect(ROOT + "/build/hgm_spine_v27_2.db")
rows = {(c, s): (w or "", e or "") for c, s, w, e in
        db.execute("select course,seq,wylie,english from corpus_segments")}

FN = {"the", "a", "an", "and", "or", "his", "our", "your"}
def norm_tib(t):
    t = re.sub(r"[,/|;:!\.་-༔]", " ", t)
    return " ".join(t.split())
def norm_eng(e, trim=True):
    e = e.lower()
    e = re.sub(r"[\"'‘’“”,;:\.\!\?\(\)\[\]]", " ", e)
    w = e.split()
    if trim:
        while w and w[0] in FN:
            w = w[1:]
    return " ".join(w)

def fam_key(course, seg):
    w, e = rows.get((course, seg), ("", ""))
    return norm_tib(w)

TRIM = "--notrim" not in sys.argv
links = [l for l in L if l["d"] == 5 and l["eng"] is not None and l["seg"] is not None]
# species per segment
seg_h = collections.defaultdict(set); seg_p = collections.defaultdict(set)
for l in links:
    k = (l["course"], l["seg"])
    t = norm_tib(l["tib"])
    if not t: continue
    e = norm_eng(l["eng"], TRIM)
    seg_h[k].add(t)
    if e: seg_p[k].add((t, e))
segs = sorted(seg_h, key=lambda k: (k[0], k[1]))
# families: first-seen order
fam_of = {}
fam_members = collections.defaultdict(list)
for k in segs:
    f = fam_key(*k)
    fam_of[k] = f
    fam_members[f].append(k)
print("banked segments with d5 links:", len(segs), " families:", len(fam_members))
per_course_fams = collections.Counter()
for f, m in fam_members.items():
    per_course_fams[min(m)[0]] += 1
print("families by first course:", dict(sorted(per_course_fams.items())))
# family incidence sets
fam_h = {f: set().union(*(seg_h[k] for k in m)) for f, m in fam_members.items()}
fam_p = {f: set().union(*(seg_p[k] for k in m)) for f, m in fam_members.items()}

def incidence_stats(unit_sets):
    T = len(unit_sets)
    inc = collections.Counter()
    for s in unit_sets:
        for x in s: inc[x] += 1
    Q = collections.Counter(inc.values())
    U = sum(inc.values())
    S = len(inc)
    Q1, Q2 = Q[1], Q[2]
    if Q2 > 0:
        Q0 = (T - 1) / T * Q1 * Q1 / (2 * Q2)
    else:
        Q0 = (T - 1) / T * Q1 * (Q1 - 1) / 2
    C = 1 - (Q1 / U) * ((T - 1) * Q1 / ((T - 1) * Q1 + 2 * Q2)) if U else 0
    return dict(T=T, U=U, S=S, Q1=Q1, Q2=Q2, Q0=Q0, chao2=S + Q0, C=C, inc=inc, Q=Q)

def chao_extrap(st, tstar):
    """Chao et al. 2014 incidence extrapolation: expected NEW species in tstar more units."""
    T, Q1, Q0 = st["T"], st["Q1"], st["Q0"]
    if Q0 <= 0: return 0.0
    return Q0 * (1 - (1 - Q1 / (T * Q0 + Q1)) ** tstar)

def chao_cov_extrap(st, tstar):
    T, U, Q1, Q2 = st["T"], st["U"], st["Q1"], st["Q2"]
    r = (T - 1) * Q1 / ((T - 1) * Q1 + 2 * Q2)
    return 1 - (Q1 / U) * r ** (tstar + 1)

def sgt(st, t, kind="bin"):
    """Orlitsky-Suresh-Wu smoothed Good-Toulmin, incidence version (n := T)."""
    n = st["T"]; Q = st["Q"]
    maxi = max(Q) if Q else 0
    if t <= 1:
        return sum(-((-t) ** i) * Q[i] for i in range(1, maxi + 1))
    if kind == "pois":
        r = 1 / (2 * t) * math.log(n * (t + 1) ** 2 / (t - 1))
        def tail(i):  # P(L >= i), L ~ Poisson(r)
            return 1 - sum(math.exp(-r) * r ** j / math.factorial(j) for j in range(i))
    else:
        k = math.ceil(0.5 * math.log(n * t * t / (t - 1), 3))
        q = 2 / (t + 2)
        def tail(i):
            return sum(math.comb(k, j) * q ** j * (1 - q) ** (k - j) for j in range(i, k + 1))
    tot = 0.0
    for i in range(1, maxi + 1):
        if Q[i] == 0: continue
        p = tail(i)
        if p < 1e-15: break
        tot += -((-t) ** i) * p * Q[i]
    return tot

COURSES = ["C01", "C02", "C03", "C04", "C05"]
def course_of_fam(f):
    return min(fam_members[f])[0]

def fams_in(courses):
    return [f for f, m in fam_members.items() if any(k[0] in courses for k in m)]

def backtest(sets, label, mode):
    print("\n== backtest", label, mode)
    out = []
    for i, tgt in enumerate(COURSES):
        if mode == "forward":
            if i == 0: continue
            ref_c = COURSES[:i]
        else:
            ref_c = [c for c in COURSES if c != tgt]
        ref = set(fams_in(ref_c))
        tg = [f for f in fams_in([tgt]) if f not in ref]   # novel families only
        if not ref or not tg: continue
        st = incidence_stats([sets[f] for f in ref])
        seen = set(st["inc"])
        actual = len(set().union(*(sets[f] for f in tg)) - seen)
        t = len(tg) / len(ref)
        p_chao = chao_extrap(st, len(tg))
        p_sgt = sgt(st, t, "bin")
        p_sgtp = sgt(st, t, "pois")
        out.append((tgt, len(ref), len(tg), t, actual, p_chao, p_sgt, p_sgtp))
        print("%s ref=%4d tgt=%4d t=%.2f actual=%5d  chao=%7.0f (%.2fx)  sgt_bin=%7.0f (%.2fx) sgt_pois=%7.0f (%.2fx)  C_ref=%.3f" % (
            tgt, len(ref), len(tg), t, actual, p_chao, p_chao / actual, p_sgt, p_sgt / actual, p_sgtp, p_sgtp / actual, st["C"]))
    return out

for label, sets in (("HEADWORDS", fam_h), ("PAIRS", fam_p)):
    st = incidence_stats(list(sets.values()))
    print("\n##", label, "trim" if TRIM else "notrim")
    print("T=%d U=%d S_obs=%d Q1=%d Q2=%d  Chao2=%.0f  incidence coverage C=%.3f" % (
        st["T"], st["U"], st["S"], st["Q1"], st["Q2"], st["chao2"], st["C"]))
    backtest(sets, label, "forward")
    backtest(sets, label, "loco")
    # mandate forecast: 4,073 unique remaining segments; treat as novel families
    T = st["T"]
    for m in (1000, 2000, 4073):
        t = m / T
        print("forecast +%d families (t=%.2f): chao_extrap=%.0f  sgt_bin=%.0f  sgt_pois=%.0f  C_after=%.3f" % (
            m, t, chao_extrap(st, m), sgt(st, t, "bin"), sgt(st, t, "pois"), chao_cov_extrap(st, m)))
