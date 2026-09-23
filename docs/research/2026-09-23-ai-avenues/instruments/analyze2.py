#!/usr/bin/env python3
"""Generator refusal reasons, wall-clock, tool-result sizes (read-only)."""
import json, os, re, statistics as st
from collections import Counter
from datetime import datetime

rows = json.load(open(os.path.join(os.path.dirname(__file__), 'rows.json')))
ref = Counter(); gen_calls = 0; gen_exit0 = 0; first_try_ok = Counter(); agents_by_role = Counter()
walls = {}; result_chars = Counter(); result_counts = Counter()
refused_lines_per_call = []
for r in rows:
    f = r['f']; role = r['role']
    agents_by_role[role] += 1
    lines = [json.loads(l) for l in open(f) if l.strip()]
    ts = [l.get('timestamp') for l in lines if l.get('timestamp')]
    if ts:
        t0 = datetime.fromisoformat(ts[0].replace('Z', '+00:00')); t1 = datetime.fromisoformat(ts[-1].replace('Z', '+00:00'))
        walls.setdefault(role, []).append((t1 - t0).total_seconds())
    pending = {}
    gen_seq = []
    for d in lines:
        msg = d.get('message') or {}
        c = msg.get('content')
        if not isinstance(c, list): continue
        for b in c:
            if b.get('type') == 'tool_use':
                name = b.get('name'); inp = b.get('input') or {}
                cmd = inp.get('command', '') if name == 'Bash' else ''
                kind = 'gen' if 'gen_alignment_page' in cmd else (name if name != 'Bash' else 'bash')
                pending[b.get('id')] = kind
            elif b.get('type') == 'tool_result':
                kind = pending.get(b.get('tool_use_id'), '?')
                content = b.get('content')
                text = content if isinstance(content, str) else ' '.join((x.get('text') or '') for x in (content or []) if isinstance(x, dict))
                result_chars[(role, kind)] += len(text); result_counts[(role, kind)] += 1
                if kind == 'gen':
                    gen_calls += 1
                    ok = 'EXIT=0' in text
                    gen_seq.append(ok)
                    if ok: gen_exit0 += 1
                    rl = [ln for ln in text.splitlines() if 'REFUSED' in ln]
                    refused_lines_per_call.append(len(rl))
                    for ln in rl:
                        l = ln.lower()
                        if 'not present' in l or 'not found' in l or 'not a substring' in l or 'not verbatim' in l: k = 'not-verbatim/not-present'
                        elif 'order' in l or 'cursor' in l or 'after' in l: k = 'order/cursor'
                        elif 'ambiguous' in l or 'occurs' in l or 'count' in l: k = 'ambiguous-english'
                        elif 'member' in l or 'parent' in l or 'd=7' in l or 'nest' in l: k = 'nesting/member'
                        elif 'broken' in l or 'word boundary' in l or 'mid-word' in l or 'subword' in l: k = 'broken-word'
                        elif 'eng_order' in l: k = 'eng_order'
                        elif 'depth' in l or 'd=' in l: k = 'depth'
                        else: k = 'other: ' + re.sub(r'[\"\'].*?[\"\']', '..', ln.strip())[:90]
                        ref[k] += 1
    if gen_seq:
        first_try_ok[(role, gen_seq[0])] += 1

print('generator calls', gen_calls, 'exit0', gen_exit0)
print('first generator run EXIT=0 by role:', dict(first_try_ok))
print('REFUSED line categories:')
for k, v in ref.most_common(30): print(f'  {v:6d}  {k}')
print('\nwall-clock seconds per agent (median / p90):')
for role, w in walls.items():
    w = sorted(w); print(f'  {role:10s} n={len(w)} median {st.median(w):.0f}s p90 {w[int(len(w)*0.9)]:.0f}s')
print('\ntool-result chars per agent by (role, kind):')
for (role, kind), v in sorted(result_chars.items(), key=lambda x: -x[1])[:24]:
    print(f'  {role:10s} {kind:18s} {v/agents_by_role[role]:9.0f} chars/agent  ({result_counts[(role,kind)]/agents_by_role[role]:.2f} calls/agent)')
