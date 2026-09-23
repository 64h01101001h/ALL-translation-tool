"""Scratchpad pilot: IBM Model 1 (both directions) on the spine corpus, scored
against the landed alignment bank. Read-only on the repo. Nothing leaves the
machine. This is NOT eflomal; it is a deliberately weak floor (no HMM/fertility,
no Bayesian priors) to see whether a free statistical aligner has any signal."""
import json, re, sqlite3, sys, time, collections, math
import numpy as np

REPO = '/Users/adamderickandrade/ALL-translation-tool'
t0 = time.time()
db = sqlite3.connect(f'file:{REPO}/build/hgm_spine_v27_2.db?mode=ro', uri=True)
rows = db.execute('select course, seq, wylie, english from corpus_segments').fetchall()

TIB_RE = re.compile(r"[^\s,/|;_:!\[\]\(\)\{\}\*#@0-9]+")
ENG_RE = re.compile(r"[A-Za-z][A-Za-z'\-]*|[0-9]+")

def tib_tokens(w):
    return [m.group(0) for m in TIB_RE.finditer(w or '')]

def eng_tokens(e):
    return [m.group(0).lower().strip("'") for m in ENG_RE.finditer(e or '')]

seg_tok = {}
train = {}
for c, s, w, e in rows:
    ft, et = tib_tokens(w), eng_tokens(e)
    seg_tok[(c, s)] = (ft, et)
    if ft and et and len(ft) * len(et) <= 60000:
        train[(w, e)] = (ft, et)          # exact-duplicate removal
print('segments', len(rows), 'unique training pairs', len(train), f'{time.time()-t0:.1f}s', flush=True)

fv, ev = {'<NULL>': 0}, {'<NULL>': 0}
def fid(x):
    return fv.setdefault(x, len(fv))
def eid(x):
    return ev.setdefault(x, len(ev))
base_pairs = [([fid(x) for x in ft], [eid(x) for x in et]) for ft, et in train.values()]
print('vocab tib', Vf, 'eng', Ve, flush=True)


def build(direction, pairs):
    """direction 'e|f': each English token picks a Tibetan token (or NULL)."""
    keys, grp_sizes = [], []
    for f, e in pairs:
        src = [0] + f if direction == 'e|f' else [0] + e
        tgt = e if direction == 'e|f' else f
        src = np.asarray(src, dtype=np.int64)
        tgt = np.asarray(tgt, dtype=np.int64)
        V = Ve if direction == 'e|f' else Vf
        k = (src[None, :] * V + tgt[:, None]).ravel()   # ordered by (tgt pos, src pos)
        keys.append(k)
        grp_sizes.extend([len(src)] * len(tgt))
    keys = np.concatenate(keys)
    uniq, cid = np.unique(keys, return_inverse=True)
    V = Ve if direction == 'e|f' else Vf
    src_of = (uniq // V).astype(np.int64)
    sizes = np.asarray(grp_sizes, dtype=np.int64)
    starts = np.concatenate([[0], np.cumsum(sizes)[:-1]])
    return uniq, cid.astype(np.int64), src_of, starts, sizes


def em(direction, pairs, iters=6):
    uniq, cid, src_of, starts, sizes = build(direction, pairs)
    print(direction, 'pair slots', len(cid), 'distinct pairs', len(uniq), flush=True)
    t = np.full(len(uniq), 1.0)
    grp_idx = np.repeat(np.arange(len(starts)), sizes)
    for it in range(iters):
        val = t[cid]
        denom = np.add.reduceat(val, starts)
        post = val / denom[grp_idx]
        cnt = np.bincount(cid, weights=post, minlength=len(uniq))
        tot = np.bincount(src_of, weights=cnt)
        t = cnt / tot[src_of]
        print(' iter', it, f'{time.time()-t0:.1f}s', flush=True)
    return dict(zip(uniq.tolist(), t.tolist()))

