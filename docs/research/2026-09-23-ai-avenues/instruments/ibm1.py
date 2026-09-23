#!/usr/bin/env python3
"""IBM Model 1 (English word <- Tibetan syllable), trained ONLY on the GMR spine,
unique (wylie, english) texts, numpy-vectorised. Read-only; writes to scratchpad.
Then scores banked d=5 links: for each English content word in the banked span,
which Tibetan syllable of the SAME segment has the highest posterior? Is it inside
the banked Tibetan span?"""
import json, re, sqlite3, collections, sys, time
import numpy as np
ROOT = '/Users/adamderickandrade/ALL-translation-tool'
SP = '/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad'
BANK = sys.argv[1] if len(sys.argv) > 1 else f'{ROOT}/data/alignment/alignment_full_v1.json'
STOP = set('the a an of to and or in is that this it be for on as with by his her their our your we you i my are was not which there these those one who what when from at into its all any has have had will can may must do does also such so than then but if no me he she they them us him'.split())
db = sqlite3.connect(f'file:{ROOT}/build/hgm_spine_v27_2.db?mode=ro', uri=True)
rows = db.execute('select course, seq, wylie, english from corpus_segments').fetchall()
seg = {(c, s): (w or '', e or '') for c, s, w, e in rows}
uniq = {}
for k, t in seg.items(): uniq.setdefault(t, k)
texts = list(uniq)
def tsyl(w): return re.sub(r"[,/|;:!?\[\]{}()\"@#*0-9]+", ' ', w.lower()).split()
def ewords(e): return [x for x in re.findall(r"[a-z]+", e.lower()) if x not in STOP]
tv = {}; ev = {}
T = []; E = []
for w, e in texts:
    ts = tsyl(w)[:120]; es = ewords(e)[:120]
    if not ts or not es: T.append(np.zeros(0, np.int64)); E.append(np.zeros(0, np.int64)); continue
    T.append(np.array([tv.setdefault(x, len(tv)) for x in ['<NULL>'] + ts], np.int64))
    E.append(np.array([ev.setdefault(x, len(ev)) for x in es], np.int64))
V = len(ev) + 1
print('texts', len(texts), 'tib vocab', len(tv), 'eng vocab', len(ev))
# build pair-occurrence arrays: for each segment, each english token j, each tib token i
seg_ids = []; ej_ids = []; pair = []
g = 0  # global english-token index
for si, (t, e) in enumerate(zip(T, E)):
    if len(t) == 0: continue
    nt, ne = len(t), len(e)
    pair.append((t[None, :] * V + e[:, None]).ravel())       # ne x nt
    ej_ids.append(np.repeat(np.arange(g, g + ne), nt))
    g += ne
pair = np.concatenate(pair); ej = np.concatenate(ej_ids)
print('pair occurrences', len(pair))
up, inv = np.unique(pair, return_inverse=True)
print('distinct pairs', len(up))
tprob = np.ones(len(up))
tib_of = up // V
for it in range(6):
    t0 = time.time()
    p = tprob[inv]
    z = np.bincount(ej, weights=p, minlength=g)
    post = p / z[ej]
    cnt = np.bincount(inv, weights=post, minlength=len(up))
    tot = np.bincount(tib_of, weights=cnt)
    tprob = cnt / tot[tib_of]
    print('iter', it, 'secs %.1f' % (time.time() - t0))
tdict_idx = {int(x): i for i, x in enumerate(up)}  # may be big; used sparsely below
np.save(f'{SP}/ibm1_up.npy', up); np.save(f'{SP}/ibm1_t.npy', tprob)
json.dump({'tv': tv, 'ev': ev, 'V': V}, open(f'{SP}/ibm1_vocab.json', 'w'))
# ---- score banked d=5 links
L = [l for l in json.load(open(BANK))['links'] if l['seg'] is not None]
d5 = [l for l in L if l['d'] == 5 and l['eng'] and l['tib'].strip() == l['tib']]
def tval(ti, wi):
    k = ti * V + wi
    j = np.searchsorted(up, k)
    return tprob[j] if j < len(up) and up[j] == k else 0.0
res = collections.Counter(); out = []
for l in d5:
    w, e = seg[(l['course'], l['seg'])]
    ts = tsyl(w)[:120]
    span = tsyl(l['tib'])
    ew = [x for x in ewords(l['eng']) if x in ev]
    if not ew or not span or not ts:
        res['unscorable'] += 1; continue
    # positions of span syllables in ts (first occurrence of contiguous match)
    pos = None
    for i in range(len(ts) - len(span) + 1):
        if ts[i:i + len(span)] == span: pos = set(range(i, i + len(span))); break
    if pos is None: res['span_not_found'] += 1; continue
    tids = [tv.get(x) for x in ts]
    inside = 0; best_out = []
    for wd in ew:
        wi = ev[wd]
        sc = [tval(ti, wi) if ti is not None else 0.0 for ti in tids]
        b = int(np.argmax(sc))
        if b in pos: inside += 1
        else: best_out.append((wd, ts[b], round(sc[b], 3), round(max(sc[i] for i in pos), 3)))
    frac = inside / len(ew)
    res['scored'] += 1
    res['all_inside'] += frac == 1.0
    res['none_inside'] += frac == 0.0
    out.append(dict(course=l['course'], seg=l['seg'], tib=l['tib'], eng=l['eng'], frac=frac, out=best_out))
print(dict(res))
print('share of scored d=5 links where IBM-1 puts EVERY English content word on a syllable inside the banked span: %.1f%%' % (100 * res['all_inside'] / res['scored']))
print('share where it puts NONE inside: %.1f%%' % (100 * res['none_inside'] / res['scored']))
json.dump(out, open(BANK.split('/')[-1] + '.ibm1.json' if BANK.startswith(SP) else f'{SP}/current.ibm1.json', 'w'))
