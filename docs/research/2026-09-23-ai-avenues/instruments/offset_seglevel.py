"""Column-offset probe (scratchpad; read-only on repo; nothing leaves the machine).
PRE-REGISTERED before running (2026-09-23):
  score(w, e) = 0.5*(mean over English content tokens of max_i t(e|f_i)
                   + mean over Tibetan syllables of max_j t(f|e_j)), IBM1 both ways.
  A segment s is 'lag-1 flagged' if score(w_s, e_{s-1}) > score(w_s, e_s)
  (and likewise lag+1). A RUN is >= 6 flagged of 8 consecutive segments in one course.
  SUCCESS iff (a) a lag-1 run overlaps C13:63-82 and (b) <= 10 other runs exist spine-wide.
  Otherwise the probe joins the falsified list."""
import re, sqlite3, time, collections
import numpy as np
REPO = '/Users/adamderickandrade/ALL-translation-tool'
t0 = time.time()
db = sqlite3.connect(f'file:{REPO}/build/hgm_spine_v27_2.db?mode=ro', uri=True)
rows = db.execute('select course, seq, wylie, english from corpus_segments order by course, seq').fetchall()
TIB_RE = re.compile(r"[^\s,/|;_:!\[\]\(\)\{\}\*#@0-9]+")
ENG_RE = re.compile(r"[A-Za-z][A-Za-z'\-]*|[0-9]+")
STOP = set('the a an of to and in is that this it be as for with which by are or on from at your you we our they their its his her he she i my me was were been has have had not no so if but then than these those there what when where who whom whose all any some one'.split())
tt = lambda w: [m.group(0) for m in TIB_RE.finditer(w or '')]
et_ = lambda e: [m.group(0).lower().strip("'") for m in ENG_RE.finditer(e or '')]
fv, ev = {'<NULL>': 0}, {'<NULL>': 0}
segs = []
train = {}
for c, s, w, e in rows:
    f = [fv.setdefault(x, len(fv)) for x in tt(w)]
    g = [ev.setdefault(x, len(ev)) for x in et_(e)]
    segs.append((c, s, f, g, [x not in STOP for x in et_(e)]))
    if f and g and len(f) * len(g) <= 60000:
        train[(w, e)] = (f, g)
pairs = list(train.values())
Vf, Ve = len(fv), len(ev)

def em(direction, iters=6):
    V = Ve if direction == 'e|f' else Vf
    keys, sizes = [], []
    for f, e in pairs:
        src = np.asarray([0] + (f if direction == 'e|f' else e), dtype=np.int64)
        tgt = np.asarray(e if direction == 'e|f' else f, dtype=np.int64)
        keys.append((src[None, :] * V + tgt[:, None]).ravel()); sizes.extend([len(src)] * len(tgt))
    keys = np.concatenate(keys)
    uniq, cid = np.unique(keys, return_inverse=True)
    src_of = uniq // V; sizes = np.asarray(sizes); starts = np.concatenate([[0], np.cumsum(sizes)[:-1]])
    grp = np.repeat(np.arange(len(starts)), sizes); t = np.ones(len(uniq))
    for _ in range(iters):
        val = t[cid]; post = val / np.add.reduceat(val, starts)[grp]
        cnt = np.bincount(cid, weights=post, minlength=len(uniq)); t = cnt / np.bincount(src_of, weights=cnt)[src_of]
    return uniq, t
Tef = em('e|f'); Tfe = em('f|e')

def look(T, keys):
    uniq, t = T
    idx = np.searchsorted(uniq, keys)
    idx = np.minimum(idx, len(uniq) - 1)
    return np.where(uniq[idx] == keys, t[idx], 0.0)
print('trained', f'{time.time()-t0:.1f}s', flush=True)

def score(f, g, cmask):
    if not f or not g:
        return None
    F = np.asarray(f, dtype=np.int64); G = np.asarray(g, dtype=np.int64)
    A = look(Tef, F[:, None] * Ve + G[None, :])      # t(e|f), m x n
    B = look(Tfe, G[None, :] * Vf + F[:, None])      # t(f|e), m x n
    cm = np.asarray(cmask, dtype=bool)
    ce = A.max(0)[cm] if cm.any() else np.zeros(1)
    cf = B.max(1)
    return 0.5 * (float(ce.mean()) + float(cf.mean()))


import re as _re
by_course = collections.defaultdict(list)
for c, s_, f, g, m in segs:
    by_course[c].append((s_, f, g, m))
mand = [c for c in by_course if _re.fullmatch(r'C\d+', c) and 1 <= int(c[1:]) <= 18]
tot = 0; flagged = []
for c in mand:
    L = sorted(by_course[c])
    for k, (s_, f, g, m) in enumerate(L):
        s0 = score(f, g, m)
        if s0 is None: continue
        tot += 1
        best = None
        for lag in (-1, 1):
            j = k + lag
            if 0 <= j < len(L):
                s1 = score(f, L[j][2], L[j][3])
                if s1 is not None and s1 > 2 * s0 and s1 - s0 > 0.05:
                    best = (lag, round(s0, 3), round(s1, 3))
        if best: flagged.append((c, s_) + best)
print('mandate segments scored', tot, 'flagged (neighbour >2x own score and +0.05)', len(flagged))
byc = collections.Counter(c for c, *_ in flagged)
print(sorted(byc.items(), key=lambda x: int(x[0][1:])))
known = [x for x in flagged if x[0] == 'C13' and 63 <= x[1] <= 82]
print('of which in known C13:63-82 run', len(known))
c18 = [x for x in flagged if x[0] == 'C18' and 600 <= x[1] <= 700]
print('C18 600-700 flagged', c18)
