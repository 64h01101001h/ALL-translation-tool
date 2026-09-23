#!/usr/bin/env python3
"""Read-only analysis of alignment-agent transcripts (local files only)."""
import json, glob, os, re, sys, statistics as st
from collections import Counter, defaultdict

BASE = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
files = glob.glob(BASE + '*/subagents/workflows/*/agent-*.jsonl')

PRICE = dict(inp=5.0, cw5=6.25, cw1h=10.0, cr=0.5, out=25.0)  # Opus 5 list, $/M

def classify_bash(cmd):
    c = cmd
    if 'gen_alignment_page' in c: return 'bash:generator'
    if 'verify-inlined' in c: return 'bash:verify'
    if 'sqlite3' in c: return 'bash:sqlite'
    if 'alignment_full_v1' in c or 'precedent' in c: return 'bash:bank-query'
    if re.search(r'\bgrep\b|\brg\b', c): return 'bash:grep'
    if re.search(r'python3? -c|python3? - <<|<<', c): return 'bash:python-inline'
    if re.search(r'\bcat\b|\bhead\b|\bsed -n\b|\btail\b', c): return 'bash:cat'
    if re.search(r'\bmkdir\b|\bls\b|\bmv\b|\bcp\b|\brm\b', c): return 'bash:fs'
    return 'bash:other'

rows = []
for f in files:
    try:
        lines = open(f).read().splitlines()
    except Exception:
        continue
    if not lines: continue
    try:
        first = json.loads(lines[0])
    except Exception:
        continue
    p = first.get('message', {}).get('content')
    if not isinstance(p, str): continue
    if 'YOUR ANGLE IS TIBETAN-FIRST' in p: role = 'propose-T'
    elif 'YOUR ANGLE IS ENGLISH-FIRST' in p: role = 'propose-E'
    elif 'reconcile the two independent proposals' in p: role = 'reconcile'
    else: continue
    version = 'inlined' if 'HARNESS SUPPLEMENT' in p else 'fetch-yourself'
    course = None
    m = re.search(r'for (C\d\d) segment (\d+)', p)
    if m: course = m.group(1)
    reqs = {}
    order = []
    tools = Counter()
    tool_input_chars = 0
    text_chars = 0
    models = Counter()
    for l in lines:
        try: d = json.loads(l)
        except Exception: continue
        if d.get('type') != 'assistant': continue
        msg = d.get('message', {})
        rid = d.get('requestId') or msg.get('id')
        u = msg.get('usage') or {}
        if rid not in reqs:
            order.append(rid)
            reqs[rid] = u
        else:
            # keep the entry with the largest output_tokens (final)
            if (u.get('output_tokens') or 0) >= (reqs[rid].get('output_tokens') or 0):
                reqs[rid] = u
        if msg.get('model'): models[msg['model']] += 1
        for b in msg.get('content') or []:
            if b.get('type') == 'tool_use':
                name = b.get('name')
                inp = b.get('input') or {}
                tool_input_chars += len(json.dumps(inp, ensure_ascii=False))
                if name == 'Bash':
                    tools[classify_bash(inp.get('command', ''))] += 1
                else:
                    tools[name] += 1
            elif b.get('type') == 'text':
                text_chars += len(b.get('text') or '')
    if not order: continue
    us = [reqs[r] for r in order]
    def g(u, k): return u.get(k) or 0
    cw5 = sum((u.get('cache_creation') or {}).get('ephemeral_5m_input_tokens', 0) or 0 for u in us)
    cw1 = sum((u.get('cache_creation') or {}).get('ephemeral_1h_input_tokens', 0) or 0 for u in us)
    cw = sum(g(u, 'cache_creation_input_tokens') for u in us)
    if cw5 + cw1 == 0: cw5 = cw
    cr = sum(g(u, 'cache_read_input_tokens') for u in us)
    inp = sum(g(u, 'input_tokens') for u in us)
    out = sum(g(u, 'output_tokens') for u in us)
    think = sum(((u.get('output_tokens_details') or {}).get('thinking_tokens') or 0) for u in us)
    cost = (inp*PRICE['inp'] + cw5*PRICE['cw5'] + cw1*PRICE['cw1h'] + cr*PRICE['cr'] + out*PRICE['out'])/1e6
    u0 = us[0]
    ctx = [g(u,'input_tokens')+g(u,'cache_creation_input_tokens')+g(u,'cache_read_input_tokens') for u in us]
    rows.append(dict(f=f, role=role, version=version, course=course, n=len(us), cw=cw, cw1=cw1, cr=cr, inp=inp,
                     out=out, think=think, cost=cost, first_cw=g(u0,'cache_creation_input_tokens'),
                     first_cr=g(u0,'cache_read_input_tokens'), ctx0=ctx[0], ctxN=ctx[-1], ctxmax=max(ctx),
                     tools=tools, tic=tool_input_chars, txt=text_chars, models=models, plen=len(p),
                     mtime=os.path.getmtime(f)))

def summ(sel, label):
    if not sel: return
    def med(k): return st.median([r[k] for r in sel])
    def mean(k): return st.mean([r[k] for r in sel])
    tc = Counter()
    for r in sel: tc.update(r['tools'])
    n = len(sel)
    print(f"\n== {label}: {n} agents")
    for k in ['n','ctx0','ctxN','first_cw','first_cr','cw','cw1','cr','out','think','cost','plen']:
        print(f"   {k:9s} mean {mean(k):12,.1f}  median {med(k):12,.1f}")
    tot_cost = sum(r['cost'] for r in sel)
    parts = dict(cr=sum(r['cr'] for r in sel)*PRICE['cr']/1e6,
                 cw=sum((r['cw']-r['cw1'])*PRICE['cw5']+r['cw1']*PRICE['cw1h'] for r in sel)/1e6,
                 out_think=sum(r['think'] for r in sel)*PRICE['out']/1e6,
                 out_visible=sum(r['out']-r['think'] for r in sel)*PRICE['out']/1e6)
    print('   $ share:', {k: f"{v/tot_cost*100:.1f}%" for k,v in parts.items()}, f"total ${tot_cost:,.2f}")
    print('   tool calls per agent:', {k: round(v/n,2) for k,v in tc.most_common(14)})
    print('   tool-input chars/agent', round(mean('tic')), ' text chars/agent', round(mean('txt')))
    mc = Counter()
    for r in sel: mc.update(r['models'])
    print('   models:', dict(mc.most_common(4)))

for ver in ['fetch-yourself', 'inlined']:
    for role in ['propose-T', 'propose-E', 'reconcile']:
        summ([r for r in rows if r['version']==ver and r['role']==role], f"{ver} / {role}")
    sel = [r for r in rows if r['version']==ver]
    if sel:
        # per segment: approx = 3 agents
        print(f"\n## {ver}: per-segment approx cost (sum of means over 3 roles):",
              round(sum(st.mean([r['cost'] for r in sel if r['role']==ro]) for ro in ['propose-T','propose-E','reconcile'] if any(r['role']==ro for r in sel)),2),
              " requests/segment:", round(sum(st.mean([r['n'] for r in sel if r['role']==ro]) for ro in ['propose-T','propose-E','reconcile'] if any(r['role']==ro for r in sel)),1))
json.dump([{k:(dict(v) if isinstance(v,Counter) else v) for k,v in r.items()} for r in rows],
          open(os.path.join(os.path.dirname(__file__),'rows.json'),'w'))
