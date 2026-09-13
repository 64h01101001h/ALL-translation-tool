"""attest_mine.py — v0.24 attestation recount + practice-wave miners."""
import json, re
from collections import Counter, defaultdict

STOP = set('the of to and a in is that it for on with as be this you your my we all who '
           'from or by at are was were which what when how not no an any there their his '
           'her its our they them then than so if but do does did has have had will would '
           'can could should may might must one two other some such very more most into '
           'up out about over after before between through where why also just only even '
           'own same s t don now he she him me us these those been being am were'.split())

def wtoks(w):
    # keep unicode letters (paṇ ≠ pa); strip punctuation only
    return [t for t in re.sub(r"[^\w'\.\-\+]", ' ', (w or '').lower(), flags=re.UNICODE).replace('_',' ').split() if t]

def etoks(e):
    return [t for t in re.sub(r"[^a-z' ]", ' ', (e or '').lower()).split() if t]

def contiguous(hay, needle):
    n = len(needle)
    if n == 0 or n > len(hay): return False
    for i in range(len(hay) - n + 1):
        if hay[i:i + n] == needle: return True
    return False

print('loading...')
corpus = json.load(open('full_parallel_corpus.json'))
d = json.load(open('hgm_dictionary_v23_1.json'))
U = d['unified_entries']

seg_wtoks = [wtoks(s.get('wylie')) for s in corpus]
seg_course = [s.get('course', '?') for s in corpus]
index = defaultdict(set)
for i, toks in enumerate(seg_wtoks):
    for t in set(toks): index[t].add(i)

# English n-gram document frequency (for miner df filter)
print('building english df...')
edf = Counter()
seg_egrams = []
for s in corpus:
    toks = etoks(s.get('english'))
    grams = set()
    for n in (1, 2, 3, 4):
        for i in range(len(toks) - n + 1):
            g = tuple(toks[i:i + n])
            if g[0] in STOP or g[-1] in STOP: continue
            grams.add(g)
    seg_egrams.append(grams)
    for g in grams: edf[g] += 1
NSEG = len(corpus)
DFMAX = max(3, NSEG // 100)

print('attestation recount...')
n_attested = 0; ctx_added = 0; newly_evidenced = []
for e in U:
    toks = wtoks(e.get('wylie'))
    if not toks: continue
    cand = None
    for t in toks:
        s = index.get(t)
        if s is None: cand = set(); break
        cand = s if cand is None else (cand & s)
        if not cand: break
    hits = sorted(i for i in (cand or ()) if contiguous(seg_wtoks[i], toks))
    if not hits:
        continue
    courses = sorted({seg_course[i] for i in hits})
    e['corpus_occurrences'] = {'courses': courses, 'n_segments': len(hits)}
    n_attested += 1
    p_hits = [i for i in hits if seg_course[i].startswith('P') and seg_course[i][1:].isdigit()]
    if e.get('status') == 'awaiting-hgm-equivalent':
        ctxs = e.get('attestation_contexts') or []
        if len(ctxs) < 2 and p_hits:
            have = {(c.get('course'), (c.get('hgm_english') or '')[:60]) for c in ctxs}
            for i in p_hits:
                if len(ctxs) >= 2: break
                s = corpus[i]
                k = (s['course'], s['english'][:60])
                if k in have: continue
                ctxs.append({'course': s['course'],
                             'tibetan_wylie': s['wylie'][:220],
                             'hgm_english': s['english'][:260]})
                have.add(k); ctx_added += 1
            e['attestation_contexts'] = ctxs
        if p_hits:
            newly_evidenced.append((e, hits))

print('entries attested in corpus:', n_attested, '| contexts added:', ctx_added,
      '| newly P-evidenced awaiting:', len(newly_evidenced))

print('mining (intersection)...')
applied = 0; candidates = 0
for e, hits in newly_evidenced:
    if len(hits) < 2: continue
    votes = Counter()
    for i in hits:
        for g in seg_egrams[i]: votes[g] += 1
    best = None
    for g, sup in votes.items():
        if edf[g] > DFMAX: continue
        if len(g) == 1:
            if sup < 3 or edf[g] > 50: continue
        else:
            if sup < 2: continue
        score = (sup, len(g))
        if best is None or score > best[0]: best = (score, g, sup)
    if best is None: continue
    (_, g, sup) = best
    gloss = ' '.join(g)
    ratio = sup / max(1, len(hits))
    strong = (sup >= 3 and len(g) >= 2 and ratio >= 0.25
              and (len(wtoks(e['wylie'])) >= 2 or len(hits) <= 30))
    if len(g) == 1 and sup >= 3 or len(g) > 1 and sup >= 2:
        if strong:
            e['hgm_gloss'] = [gloss]
            e['hgm_evidence'] = {'tier': 'auto-aligned', 'method': 'intersection',
                                 'confidence': 'medium', 'support': sup,
                                 'of_segments': len(hits), 'source_wave': 'practice-v24'}
            e['status'] = 'hgm-auto (mined)'
            applied += 1
        else:
            e['auto_candidate'] = gloss
            e['auto_candidate_evidence'] = {'method': 'intersection', 'support': sup,
                                            'of_segments': len(hits),
                                            'source_wave': 'practice-v24'}
            candidates += 1
print('miner: applied glosses', applied, '| auto-candidates', candidates)

print('mining (quote-pattern on P segments)...')
QP = re.compile(r"(?:what we call|known as|which we call|called)\s+[\"']?([A-Za-z][A-Za-z' \-]{2,40}?)[\"']?[,;.\)]")
qp_applied = 0
pnew = [s for s in corpus if s.get('wylie_method') == 'glyph-decode']
for s in pnew:
    m = QP.search(s.get('english') or '')
    if not m: continue
    stoks = set(wtoks(s['wylie']))
    aw = [e for e, h in newly_evidenced
          if e.get('status') == 'awaiting-hgm-equivalent'
          and set(wtoks(e['wylie'])) <= stoks
          and any(i for i in h if corpus[i] is s)]
    if len(aw) == 1:
        e = aw[0]
        e['auto_candidate'] = m.group(1).strip()
        e['auto_candidate_evidence'] = {'method': 'quote-pattern',
                                        'source_wave': 'practice-v24'}
        qp_applied += 1
print('quote-pattern candidates:', qp_applied)

json.dump(d, open('hgm_dictionary_v24_work.json', 'w'), ensure_ascii=False)
print('saved work dictionary')
