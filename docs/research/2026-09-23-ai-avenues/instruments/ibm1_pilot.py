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
pairs = [([fid(x) for x in ft], [eid(x) for x in et]) for ft, et in train.values()]
Vf, Ve = len(fv), len(ev)
print('vocab tib', Vf, 'eng', Ve, flush=True)


def build(direction):
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


def em(direction, iters=6):
    uniq, cid, src_of, starts, sizes = build(direction)
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

T_ef = em('e|f')
T_fe = em('f|e')


def post_matrix(ft, et, lam=0.0):
    """P_ef[i,j] = P(English j aligned to Tibetan i) from e|f model; P_fe from f|e.
    Returns combined scores (m x n) and the two directional posteriors."""
    f = [fv.get(x) for x in ft]
    e = [ev.get(x) for x in et]
    m, n = len(f), len(e)
    A = np.zeros((m + 1, n))
    for j, ej in enumerate(e):
        for i, fi in enumerate([0] + f):
            if ej is None or fi is None:
                A[i, j] = 1e-9
            else:
                A[i, j] = T_ef.get(fi * Ve + ej, 1e-9)
    B = np.zeros((m, n + 1))
    for i, fi in enumerate(f):
        for j, ej in enumerate([0] + e):
            if ej is None or fi is None:
                B[i, j] = 1e-9
            else:
                B[i, j] = T_fe.get(ej * Vf + fi, 1e-9)
    if lam:
        ii = (np.arange(m) + 0.5) / m
        jj = (np.arange(n) + 0.5) / n
        D = np.exp(-lam * np.abs(ii[:, None] - jj[None, :]))
        A[1:, :] *= D
        B[:, 1:] *= D
    A = A / A.sum(0, keepdims=True)
    B = B / B.sum(1, keepdims=True)
    Pef = A[1:, :]            # m x n, col-normalised incl NULL
    Pfe = B[:, 1:]            # m x n, row-normalised incl NULL
    return Pef, Pfe

# ---------------- the bank ----------------
bank = json.load(open(f'{REPO}/data/alignment/alignment_full_v1.json'))['links']
by_seg = collections.defaultdict(list)
for l in bank:
    by_seg[(l['course'], l['seg'])].append(l)

def natkey(l):
    m = re.match(r's\d+([a-z]+)(\d+)', l['id'])
    return (m.group(1), int(m.group(2))) if m else ('z', 0)

def find_seq(hay, needle, start=0):
    out = []
    for k in range(start, len(hay) - len(needle) + 1):
        if hay[k:k + len(needle)] == needle:
            out.append(k)
    return out

STOP = set('the a an of to and in is that this it be as for with which by are or on from at your you we our they their its his her he she i my me was were been has have had not no so if but then than these those there what when where who whom whose all any some one'.split())

gold_links = []   # (segkey, tset, eset, link)
located = skipped_t = skipped_e = 0
for key, links in by_seg.items():
    if key not in seg_tok:
        continue
    ft, et = seg_tok[key]
    d5 = sorted([l for l in links if l['d'] == 5 and l['eng']], key=natkey)
    cur = 0
    for l in d5:
        tt = tib_tokens(l['tib'])
        ee = eng_tokens(l['eng'])
        if not tt or not ee:
            continue
        occ = find_seq(ft, tt, cur)
        if not occ:
            occ_all = find_seq(ft, tt, 0)
            if len(occ_all) == 1:
                occ = occ_all
            else:
                skipped_t += 1
                continue
        ti = occ[0]
        cur = ti + len(tt)
        eocc = find_seq(et, ee, 0)
        if len(eocc) != 1:
            skipped_e += 1
            continue
        ej = eocc[0]
        gold_links.append((key, set(range(ti, ti + len(tt))), set(range(ej, ej + len(ee))), l))
        located += 1
print('d5 links located', located, 'skipped tib', skipped_t, 'skipped eng-ambiguous', skipped_e, flush=True)

res = {}
for lam in (0.0, 2.0):
    agree = tot = 0
    rec_any = 0
    inter_in = inter_tot = 0
    link_support = []
    seg_cache = {}
    for key, tset, eset, l in gold_links:
        if key not in seg_cache:
            ft, et = seg_tok[key]
            Pef, Pfe = post_matrix(ft, et, lam)
            # intersection links: argmax both ways
            a_e = Pef.argmax(0)                  # best tib for each eng word
            nullish_e = Pef.max(0) < 0.2
            a_f = Pfe.argmax(1)
            inter = set()
            for j, i in enumerate(a_e):
                if a_f[i] == j:
                    inter.add((int(i), j))
            seg_cache[key] = (Pef, Pfe, a_e, inter, et)
        Pef, Pfe, a_e, inter, et = seg_cache[key]
        content = [j for j in eset if et[j] not in STOP] or list(eset)
        inside = sum(1 for j in content if a_e[j] in tset)
        link_support.append(inside / len(content))
        rec_any += any((i, j) in inter for i in tset for j in eset)
    # precision of intersection links against d5 blocks, restricted to tib tokens covered by a located d5 span
    for key in seg_cache:
        blocks = [(t, e) for k2, t, e, _ in gold_links if k2 == key]
        cov = {}
        for t, e in blocks:
            for i in t:
                cov[i] = e
        et = seg_cache[key][4]
        for (i, j) in seg_cache[key][3]:
            if i in cov and et[j] not in STOP:
                inter_tot += 1
                inter_in += j in cov[i]
    ls = np.asarray(link_support)
    res[lam] = dict(links=len(ls), mean_support=float(ls.mean()),
                    full_support=float((ls == 1).mean()), zero_support=float((ls == 0).mean()),
                    recall_any_intersection=rec_any / len(ls),
                    intersection_precision=inter_in / max(inter_tot, 1), intersection_links=inter_tot)
    print('lambda', lam, json.dumps(res[lam]), f'{time.time()-t0:.1f}s', flush=True)

# ---------- audit items: does low support go with the (Claude-labelled) defects? ----------
aud = json.load(open(f'{REPO}/data/alignment/audit_verdicts_20260904.json'))['items']
idx = {}
for key, tset, eset, l in gold_links:
    idx.setdefault((f"{key[0]}:{key[1]}", l['tib'], l['eng']), (key, tset, eset))
scored = []
for it in aud:
    k = (it['ref'], it['headword'], it['eng'])
    if k not in idx:
        continue
    key, tset, eset = idx[k]
    ft, et = seg_tok[key]
    Pef, Pfe = post_matrix(ft, et, 0.0)
    a_e = Pef.argmax(0)
    content = [j for j in eset if et[j] not in STOP] or list(eset)
    sup = sum(1 for j in content if a_e[j] in tset) / len(content)
    scored.append((sup, bool(it.get('stands')), it['ref'], it['headword'], it['eng']))
print('audit items matched', len(scored), 'of', len(aud))
pos = [s for s, d, *_ in scored if d]
neg = [s for s, d, *_ in scored if not d]
# AUC of (1 - support) as a defect score
auc_num = sum((1 - p > 1 - q) + 0.5 * (p == q) for p in pos for q in neg)
auc = auc_num / max(len(pos) * len(neg), 1)
print('defective n', len(pos), 'mean support', np.mean(pos) if pos else None,
      '| sound n', len(neg), 'mean support', np.mean(neg) if neg else None, '| AUC', round(auc, 3))
for thr in (0.0, 0.34, 0.5, 0.99):
    flag = [d for s, d, *_ in scored if s <= thr]
    print(f' flag if support<={thr}: fires {len(flag)}, of which Claude-defective {sum(flag)}')
json.dump({'res': {str(k): v for k, v in res.items()}, 'audit_scored': scored},
          open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/ibm1_pilot_out.json', 'w'), ensure_ascii=False, indent=1)
print('done', f'{time.time()-t0:.1f}s')
