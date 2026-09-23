#!/usr/bin/env python3
"""Read-only label-free diagnostics over the alignment bank.
Reads data/alignment/*.json and build/hgm_spine_v27_2.db; writes nothing in the repo."""
import json, sqlite3, re, collections, math, sys, random
ROOT = '/Users/adamderickandrade/ALL-translation-tool'
bank = json.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/bank_20260904.json'))
L = [l for l in bank['links'] if l['seg'] is not None]
db = sqlite3.connect(f'file:{ROOT}/build/hgm_spine_v27_2.db?mode=ro', uri=True)
rows = db.execute('select course, seq, wylie, english from corpus_segments').fetchall()
seg = {(c, s): (w or '', e or '') for c, s, w, e in rows}
print('spine segments', len(seg))

# ---------- A. duplicate pairs inside the bank ----------
banked = collections.defaultdict(list)
for l in L:
    banked[(l['course'], l['seg'])].append(l)
print('banked segments', len(banked))
bytext = collections.defaultdict(list)
for k in banked:
    bytext[seg[k]].append(k)
groups = [g for g in bytext.values() if len(g) > 1]
pairs = []
for g in groups:
    g = sorted(g)
    for i in range(len(g)):
        for j in range(i + 1, len(g)):
            pairs.append((g[i], g[j]))
def sig(k, dmin=5):
    return collections.Counter((l['d'], l['tib'], l['eng']) for l in banked[k] if l['d'] >= dmin)
agree = 0; jac = []
bycp = collections.Counter(); bycp_ag = collections.Counter()
dis = []
for a, b in pairs:
    sa, sb = sig(a), sig(b)
    inter = sum((sa & sb).values()); uni = sum((sa | sb).values())
    j = inter / uni if uni else 1.0
    jac.append(j)
    cp = (a[0], b[0]); bycp[cp] += 1
    if sa == sb:
        agree += 1; bycp_ag[cp] += 1
    else:
        dis.append((a, b, j, len(sa), len(sb)))
print(f'A. banked duplicate pairs: {len(pairs)} in {len(groups)} groups; identical d>=5 span multisets: {agree}')
print('   by course pair (pairs, identical):', {k: (bycp[k], bycp_ag[k]) for k in sorted(bycp)})
nonempty = [p for p in pairs if sum(sig(p[0]).values()) > 0]
print('   pairs with >=1 d>=5 span:', len(nonempty))
for d in dis[:10]:
    print('   disagree', d)
# segment length of duplicate pairs (are they trivially short?)
lens = [len(seg[a][0]) for a, b in pairs]
lens.sort()
print('   wylie length of duplicate segments: median', lens[len(lens)//2], 'p90', lens[int(len(lens)*.9)], 'max', lens[-1])
spans_in_dups = sum(sum(sig(a).values()) for a, b in pairs)
print('   d>=5 spans in first member of pairs:', spans_in_dups)

# ---------- B. cross-occurrence consistency of d=5 renderings ----------
# independent reading = distinct segment TEXT (collapse exact duplicates)
d5 = [l for l in L if l['d'] == 5 and l['eng'] is not None and l['tib'] and l['tib'] == l['tib'].strip()]
print('\nB. d=5 non-null links', len(d5))
hw = collections.defaultdict(lambda: collections.defaultdict(set))
for l in d5:
    hw[l['tib']][l['eng'].lower()].add(seg[(l['course'], l['seg'])])
nread = {t: len(set().union(*r.values())) for t, r in hw.items()}
dist = collections.Counter(min(n, 10) for n in nread.values())
print('   headwords', len(hw), 'by #independent readings (10=10+):', sorted(dist.items()))
multi = [t for t in hw if nread[t] >= 3]
print('   headwords with >=3 independent readings:', len(multi), 'covering links:',
      sum(1 for l in d5 if nread[l['tib']] >= 3))
# minority renderings: headword read >=5 times, rendering seen in exactly 1 text,
# and the modal rendering holds >=60% of readings
minority = []
for t, r in hw.items():
    n = nread[t]
    if n < 5: continue
    top_e, top_s = max(r.items(), key=lambda kv: len(kv[1]))
    if len(top_s) / n < 0.6: continue
    for e, s in r.items():
        if len(s) == 1 and e != top_e:
            minority.append((t, e, top_e, n))
print('   minority-rendering candidates (read>=5, modal>=60%, rendering seen once):', len(minority))
random.seed(7)
for m in random.sample(minority, min(12, len(minority))):
    print('     ', m)

# ---------- C. over-capture containment signal ----------
# link English strictly contains (as whole words) a DIFFERENT rendering of the same
# headword that is attested in >=2 other independent readings
def words(s): return re.findall(r"[a-z0-9']+", s.lower())
contain = []
for l in d5:
    t = l['tib']; e = l['eng'].lower(); ew = ' ' + ' '.join(words(e)) + ' '
    for e2, s2 in hw[t].items():
        if e2 == e: continue
        w2 = ' '.join(words(e2))
        if not w2 or len(s2) < 2: continue
        if (' ' + w2 + ' ') in ew and len(w2) < len(ew.strip()):
            contain.append((l['course'], l['seg'], t, l['eng'], e2, len(s2)))
            break
print('\nC. d=5 links whose English strictly contains another rendering of the same headword attested in >=2 readings:', len(contain))
for c in random.sample(contain, min(12, len(contain))):
    print('     ', c)

# ---------- D. corpus association (label-free, 42K segments) ----------
# unique segment texts only (repeated text is not independent evidence)
uniq = {}
for k, (w, e) in seg.items():
    uniq.setdefault((w, e), k)
texts = list(uniq.keys())
print('\nD. unique (wylie, english) texts in spine:', len(texts))
def syl_norm(w):
    w = re.sub(r"[,/|;:!?\[\]{}()\"@#*]+", ' ', w.lower())
    return ' ' + ' '.join(w.split()) + ' '
W = [syl_norm(w) for w, e in texts]
E = [' ' + ' '.join(words(e)) + ' ' for w, e in texts]
syl_post = collections.defaultdict(set); word_post = collections.defaultdict(set)
for i, w in enumerate(W):
    for s in set(w.split()): syl_post[s].add(i)
for i, e in enumerate(E):
    for s in set(e.split()): word_post[s].add(i)
cacheT = {}; cacheE = {}
def segs_with_tib(t):
    if t in cacheT: return cacheT[t]
    tn = syl_norm(t); sy = tn.split()
    if not sy: cacheT[t] = set(); return cacheT[t]
    cand = set.intersection(*[syl_post.get(s, set()) for s in sy])
    r = {i for i in cand if tn in W[i]}
    cacheT[t] = r; return r
def segs_with_eng(e):
    if e in cacheE: return cacheE[e]
    ws = words(e)
    if not ws: cacheE[e] = set(); return cacheE[e]
    en = ' ' + ' '.join(ws) + ' '
    cand = set.intersection(*[word_post.get(s, set()) for s in ws])
    r = {i for i in cand if en in E[i]}
    cacheE[e] = r; return r
idx_of_text = {t: i for i, t in enumerate(texts)}
N = len(texts)
assoc = []
for l in d5:
    own = idx_of_text[seg[(l['course'], l['seg'])]]
    T = segs_with_tib(l['tib']) - {own}
    Ee = segs_with_eng(l['eng']) - {own}
    nt, ne, nte = len(T), len(Ee), len(T & Ee)
    assoc.append((l, nt, ne, nte))
# how many links have ANY out-of-segment corpus support?
has_t = sum(1 for a in assoc if a[1] > 0)
has_te = sum(1 for a in assoc if a[3] > 0)
print(f'   d=5 links whose Tibetan recurs elsewhere in corpus: {has_t}/{len(assoc)} ({has_t/len(assoc):.1%})')
print(f'   ... and co-occurs with the same English elsewhere at least once: {has_te}/{len(assoc)} ({has_te/len(assoc):.1%})')
# unsupported: tib recurs >=10 times elsewhere, eng occurs elsewhere, but never together
uns = [a for a in assoc if a[1] >= 10 and a[2] >= 1 and a[3] == 0]
print(f'   tib recurs >=10x, English occurs elsewhere, never together: {len(uns)}')
uns2 = [a for a in assoc if a[1] >= 10 and a[3] == 0]
print(f'   tib recurs >=10x, never with this English elsewhere: {len(uns2)}')
json.dump([dict(course=a[0]['course'], seg=a[0]['seg'], tib=a[0]['tib'], eng=a[0]['eng'], nt=a[1], ne=a[2], nte=a[3]) for a in assoc],
          open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/assoc_0904.json', 'w'))

# ---------- E. relate signals to the (MACHINE-labelled) audit, as a sanity check only ----------
v = json.load(open(f'{ROOT}/data/alignment/audit_verdicts_20260904.json'))
amap = {}
for a in assoc:
    l = a[0]
    amap.setdefault((l['tib'], l['eng'], f"{l['course']}:{l['seg']}"), a)
cont_keys = {(c[2], c[3], f'{c[0]}:{c[1]}') for c in contain}
minor_keys = {(m[0], m[1]) for m in minority}
res = collections.Counter(); found = 0
for it in v['items']:
    key = (it['headword'], it['eng'], it['ref'])
    lab = 'DEF' if it['final'] == 'CONFIRMED_DEFECT' else 'OK'
    a = amap.get(key)
    if a is None:
        res[(lab, 'unmatched')] += 1; continue
    found += 1
    nt, ne, nte = a[1], a[2], a[3]
    res[(lab, 'contain')] += key in cont_keys
    res[(lab, 'minority')] += (it['headword'], it['eng'].lower()) in minor_keys
    res[(lab, 'unsupported_tib>=10_nte0')] += (nt >= 10 and nte == 0)
    res[(lab, 'nte0')] += (nte == 0)
    res[(lab, 'n')] += 1
print('\nE. audit items matched to d=5 links:', found)
for k in sorted(res): print('  ', k, res[k])
