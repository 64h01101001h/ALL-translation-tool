"""Pilot v2 (scratchpad, read-only on repo, nothing leaves the machine).
IBM Model 1 both directions + mild diagonal at decode time. Cross-fitted:
the bank's segments are split into 5 folds by segment; supervision (banked d=5
pairs, and optionally curated/glossary hgm_gloss pairs) comes only from the
other folds; scoring is on the held-out fold. Arms:
  A  corpus only
  B  corpus + banked d=5 pairs from other folds (as mini-sentences, x3)
  C  B + curated/glossary hgm_gloss (headword, gloss) mini-sentences
"""
import json, re, sqlite3, time, collections, random
import numpy as np

REPO = '/Users/adamderickandrade/ALL-translation-tool'
OUT = '/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/ibm1_v2_out.json'
t0 = time.time()
db = sqlite3.connect(f'file:{REPO}/build/hgm_spine_v27_2.db?mode=ro', uri=True)
rows = db.execute('select course, seq, wylie, english from corpus_segments').fetchall()
TIB_RE = re.compile(r"[^\s,/|;_:!\[\]\(\)\{\}\*#@0-9]+")
ENG_RE = re.compile(r"[A-Za-z][A-Za-z'\-]*|[0-9]+")
tib_tokens = lambda w: [m.group(0) for m in TIB_RE.finditer(w or '')]
eng_tokens = lambda e: [m.group(0).lower().strip("'") for m in ENG_RE.finditer(e or '')]

seg_tok, train = {}, {}
for c, s, w, e in rows:
    ft, et = tib_tokens(w), eng_tokens(e)
    seg_tok[(c, s)] = (ft, et)
    if ft and et and len(ft) * len(et) <= 60000:
        train[(w, e)] = (ft, et)

gloss_pairs = []
for w, g, tier in db.execute("select wylie, hgm_gloss, tier from entries where hgm_gloss is not null and hgm_gloss<>'' and tier in ('curated','glossary')"):
    try:
        gl = json.loads(g)
    except Exception:
        gl = [g]
    for x in gl if isinstance(gl, list) else [gl]:
        ft, et = tib_tokens(w), eng_tokens(str(x))
        if ft and et and len(et) <= 8:
            gloss_pairs.append((ft, et))
print('gloss pairs (curated+glossary)', len(gloss_pairs), flush=True)

fv, ev = {'<NULL>': 0}, {'<NULL>': 0}
fid = lambda x: fv.setdefault(x, len(fv))
eid = lambda x: ev.setdefault(x, len(ev))
enc = lambda ft, et: ([fid(x) for x in ft], [eid(x) for x in et])
base = [enc(ft, et) for ft, et in train.values()]
gloss_enc = [enc(ft, et) for ft, et in gloss_pairs]

bank = json.load(open(f'{REPO}/data/alignment/alignment_full_v1.json'))['links']
by_seg = collections.defaultdict(list)
for l in bank:
    by_seg[(l['course'], l['seg'])].append(l)
for l in bank:
    if l['d'] == 5 and l['eng']:
        enc(tib_tokens(l['tib']), eng_tokens(l['eng']))
for ft, et in seg_tok.values():
    enc(ft, et)
Vf, Ve = len(fv), len(ev)

def natkey(l):
    m = re.match(r's\d+([a-z]+)(\d+)', l['id'])
    return (m.group(1), int(m.group(2))) if m else ('z', 0)

def find_seq(hay, needle, start=0):
    return [k for k in range(start, len(hay) - len(needle) + 1) if hay[k:k + len(needle)] == needle]

STOP = set('the a an of to and in is that this it be as for with which by are or on from at your you we our they their its his her he she i my me was were been has have had not no so if but then than these those there what when where who whom whose all any some one'.split())

gold = []
for key, links in by_seg.items():
    if key not in seg_tok:
        continue
    ft, et = seg_tok[key]
    cur = 0
    for l in sorted([l for l in links if l['d'] == 5 and l['eng']], key=natkey):
        tt, ee = tib_tokens(l['tib']), eng_tokens(l['eng'])
        if not tt or not ee:
            continue
        occ = find_seq(ft, tt, cur) or (lambda a: a if len(a) == 1 else [])(find_seq(ft, tt, 0))
        if not occ:
            continue
        ti = occ[0]; cur = ti + len(tt)
        eocc = find_seq(et, ee, 0)
        if len(eocc) != 1:
            continue
        gold.append((key, ti, len(tt), eocc[0], len(ee), l))
segkeys = sorted({g[0] for g in gold})
random.Random(20260923).shuffle(segkeys)
fold_of = {k: i % 5 for i, k in enumerate(segkeys)}
print('gold located', len(gold), 'segments', len(segkeys), f'{time.time()-t0:.1f}s', flush=True)

def em(direction, pairs, iters=6):
    V = Ve if direction == 'e|f' else Vf
    keys, sizes = [], []
    for f, e in pairs:
        src = np.asarray([0] + (f if direction == 'e|f' else e), dtype=np.int64)
        tgt = np.asarray(e if direction == 'e|f' else f, dtype=np.int64)
        keys.append((src[None, :] * V + tgt[:, None]).ravel())
        sizes.extend([len(src)] * len(tgt))
    keys = np.concatenate(keys)
    uniq, cid = np.unique(keys, return_inverse=True)
    src_of = uniq // V
    sizes = np.asarray(sizes); starts = np.concatenate([[0], np.cumsum(sizes)[:-1]])
    grp = np.repeat(np.arange(len(starts)), sizes)
    t = np.ones(len(uniq))
    for _ in range(iters):
        val = t[cid]
        post = val / np.add.reduceat(val, starts)[grp]
        cnt = np.bincount(cid, weights=post, minlength=len(uniq))
        t = cnt / np.bincount(src_of, weights=cnt)[src_of]
    return dict(zip(uniq.tolist(), t.tolist()))

def posts(T_ef, T_fe, ft, et, lam=2.0):
    f = [fv[x] for x in ft]; e = [ev[x] for x in et]
    m, n = len(f), len(e)
    A = np.array([[T_ef.get(fi * Ve + ej, 1e-9) for ej in e] for fi in [0] + f])
    B = np.array([[T_fe.get(ej * Vf + fi, 1e-9) for ej in [0] + e] for fi in f])
    D = np.exp(-lam * np.abs(((np.arange(m) + .5) / m)[:, None] - ((np.arange(n) + .5) / n)[None, :]))
    A[1:] *= D; B[:, 1:] *= D
    A /= A.sum(0, keepdims=True); B /= B.sum(1, keepdims=True)
    return A[1:], B[:, 1:]

results = {}
for arm in ('A', 'B', 'C'):
    agg = collections.Counter(); conf = collections.defaultdict(lambda: [0, 0]); supports = []
    folds = [0] if arm == 'A' else range(5)
    for k in folds:
        pairs = list(base)
        if arm in ('B', 'C'):
            for key, ti, tl, ej, el, l in gold:
                if fold_of[key] != k:
                    p = enc(tib_tokens(l['tib']), eng_tokens(l['eng']))
                    pairs.extend([p] * 3)
        if arm == 'C':
            pairs.extend(gloss_enc)
        T_ef = em('e|f', pairs); T_fe = em('f|e', pairs)
        test = [g for g in gold if (arm == 'A' or fold_of[g[0]] == k)]
        cache = {}
        for key, ti, tl, ej, el, l in test:
            if key not in cache:
                ft, et = seg_tok[key]
                Pef, Pfe = posts(T_ef, T_fe, ft, et)
                a_e = Pef.argmax(0); a_f = Pfe.argmax(1)
                inter = {(int(i), j): float(Pef[i, j] * Pfe[i, j]) for j, i in enumerate(a_e) if a_f[i] == j}
                cache[key] = (Pef, a_e, inter, et)
            Pef, a_e, inter, et = cache[key]
            tset = set(range(ti, ti + tl)); eset = set(range(ej, ej + el))
            content = [j for j in eset if et[j] not in STOP] or list(eset)
            supports.append(sum(a_e[j] in tset for j in content) / len(content))
            agg['links'] += 1
            agg['recall_any'] += any((i, j) in inter for i in tset for j in eset)
            # candidate English span proposed for this Tibetan span: words whose best tib is inside, content only
            cand = sorted(j for j in range(len(et)) if a_e[j] in tset and et[j] not in STOP)
            if cand:
                hull = set(range(cand[0], cand[-1] + 1))
                agg['cand_any'] += 1
                agg['cand_exact'] += hull == eset
                agg['cand_overlap'] += bool(hull & eset)
                agg['cand_inside'] += hull <= eset
        for key, (Pef, a_e, inter, et) in cache.items():
            cov = {}
            for g in test:
                if g[0] == key:
                    for i in range(g[1], g[1] + g[2]):
                        cov[i] = set(range(g[3], g[3] + g[4]))
            for (i, j), s in inter.items():
                if i in cov and et[j] not in STOP:
                    b = min(int(s * 10), 9)
                    conf[b][0] += 1; conf[b][1] += j in cov[i]
        print(' arm', arm, 'fold', k, f'{time.time()-t0:.1f}s', flush=True)
    s = np.asarray(supports)
    r = dict(links=agg['links'], mean_support=round(float(s.mean()), 3), zero_support=round(float((s == 0).mean()), 3),
             full_support=round(float((s == 1).mean()), 3),
             recall_any_intersection=round(agg['recall_any'] / agg['links'], 3),
             candidate_for_span=round(agg['cand_any'] / agg['links'], 3),
             candidate_exact_match=round(agg['cand_exact'] / agg['links'], 3),
             candidate_overlaps=round(agg['cand_overlap'] / agg['links'], 3),
             candidate_inside_bank_span=round(agg['cand_inside'] / agg['links'], 3),
             precision_by_confidence={f'{b/10:.1f}-{(b+1)/10:.1f}': [n, round(c / n, 3) if n else None] for b, (n, c) in sorted(conf.items())})
    tot_n = sum(n for n, c in conf.values()); tot_c = sum(c for n, c in conf.values())
    r['intersection_precision_all'] = round(tot_c / tot_n, 3); r['intersection_links'] = tot_n
    hi = [(n, c) for b, (n, c) in conf.items() if b >= 5]
    r['precision_conf_ge_0.5'] = [sum(n for n, c in hi), round(sum(c for n, c in hi) / max(sum(n for n, c in hi), 1), 3)]
    results[arm] = r
    print(arm, json.dumps(r), flush=True)
json.dump(results, open(OUT, 'w'), indent=1)
print('done', f'{time.time()-t0:.1f}s')
