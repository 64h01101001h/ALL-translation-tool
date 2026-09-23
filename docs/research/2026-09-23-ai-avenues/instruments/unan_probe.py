"""Does a corpus-only IBM1 aligner (no bank priors) separate the 41 unanimous spans the
reconciler DROPPED from the 4,741 it KEPT? Read-only; local; scratchpad."""
import json, glob, os, re, sqlite3, collections, time
import numpy as np
exec(open('ibm1_v2.py').read().split("results = {}")[0])   # corpus load, vocab, gold, em(), posts()
BASE = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
ids = set()
for f in glob.glob(BASE + '/workflows/wf_*.json'):
    j = json.load(open(f))
    if j.get('workflowName') == 'c05-align' and j.get('status') == 'completed':
        ids.add(j['runId'])
specs = collections.defaultdict(dict)
for runId in ids:
    for jf in glob.glob(os.path.join(BASE, 'subagents', 'workflows', runId) + '/agent-*.jsonl'):
        last = None
        for line in open(jf):
            try: o = json.loads(line)
            except Exception: continue
            if o.get('type') != 'assistant': continue
            for b in ((o.get('message') or {}).get('content') or []):
                if b.get('type') == 'tool_use' and b.get('name') == 'StructuredOutput':
                    last = b.get('input') or {}
        if not last: continue
        seq = last.get('seq'); ang = last.get('angle')
        if seq is None or not ang: continue
        specs[seq][ang] = {(s.get('tib', '').strip(), (s.get('eng') or '').strip(), s.get('d')) for s in (last.get('spans') or []) if s.get('tib')}
T_ef = em('e|f', base); T_fe = em('f|e', base)
print('trained arm A (corpus only)', f'{time.time()-t0:.1f}s', flush=True)
rows = {'kept': [], 'dropped': []}
unloc = collections.Counter()
for seq, v in specs.items():
    if 'reconciled' not in v or len(v) < 3: continue
    ks = [k for k in v if k != 'reconciled']
    A, B, R = v[ks[0]], v[ks[1]], v['reconciled']
    key = ('C05', int(seq))
    if key not in seg_tok: continue
    ft, et = seg_tok[key]
    Pef, Pfe = posts(T_ef, T_fe, ft, et)
    a_e = Pef.argmax(0)
    for (tib, eng, d) in A & B:
        if d != 5 or not eng: continue
        cls = 'kept' if (tib, eng, d) in R else 'dropped'
        tt, ee = tib_tokens(tib), eng_tokens(eng)
        to = find_seq(ft, tt); eo = find_seq(et, ee)
        if not tt or not ee or not to or len(eo) != 1:
            unloc[cls] += 1; continue
        eset = set(range(eo[0], eo[0] + len(ee)))
        content = [j for j in eset if et[j] not in STOP] or list(eset)
        best = max(sum(a_e[j] in set(range(o, o + len(tt))) for j in content) / len(content) for o in to)
        rows[cls].append((best, seq, tib, eng))
for c in rows:
    s = np.asarray([r[0] for r in rows[c]])
    print(c, 'd5 non-null located', len(s), 'unlocated', unloc[c], 'mean support', round(float(s.mean()), 3) if len(s) else None,
          'zero-support share', round(float((s == 0).mean()), 3) if len(s) else None)
pos = [r[0] for r in rows['dropped']]; neg = [r[0] for r in rows['kept']]
auc = sum((1 - p > 1 - q) + 0.5 * (p == q) for p in pos for q in neg) / max(len(pos) * len(neg), 1)
print('AUC (low support -> dropped)', round(auc, 3))
for r in sorted(rows['dropped']): print('  dropped', r)
