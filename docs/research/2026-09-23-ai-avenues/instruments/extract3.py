import json, os, glob, re, collections, statistics as st
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
rows=[]
for wf in sorted(os.listdir(D)):
    p=os.path.join(D,wf)
    if not os.path.isdir(p): continue
    for f in sorted(glob.glob(p+'/agent-*.jsonl')):
        lines=[]
        for l in open(f,errors='replace'):
            l=l.strip()
            if not l: continue
            try: lines.append(json.loads(l))
            except: pass
        if not lines: continue
        head=json.dumps(lines[0])
        if not ('PROPOSE_BRIEF' in head or 'RECONCILE_BRIEF' in head or 'full-depth alignment' in head): continue
        mseq=re.search(r'segment (\d+)', head); seq=mseq.group(1) if mseq else None
        byid={}; order=[]
        for o in lines:
            if o.get('type')!='assistant': continue
            mm=o.get('message',{}); mid=mm.get('id'); u=mm.get('usage',{}) or {}
            if mid not in byid:
                byid[mid]=dict(o=0, arg=0, txt=0, nthink=0, ntool=0, names=[])
                order.append(mid)
            e=byid[mid]; e['o']=max(e['o'], u.get('output_tokens',0) or 0)
            for b in mm.get('content',[]) or []:
                if not isinstance(b,dict): continue
                t=b.get('type')
                if t=='tool_use':
                    e['arg']=max(e['arg'], e['arg'])
                    a=len(json.dumps(b.get('input',{}) or {}))
                    if b.get('id') not in e['names']:
                        e['names'].append(b.get('id')); e['arg']+=a; e['ntool']+=1
                elif t=='thinking': e['nthink']=1
                elif t=='text': e['txt']=max(e['txt'], len(b.get('text','')))
        for mid in order:
            e=byid[mid]
            rows.append(dict(wf=wf, seq=seq, out=e['o'], argtok=e['arg']/3.6, txttok=e['txt']/3.6, think=e['nthink'], ntool=e['ntool']))
nseg=len({(r['wf'],r['seq']) for r in rows if r['seq']})
to=sum(r['out'] for r in rows); ta=sum(r['argtok'] for r in rows); tt=sum(r['txttok'] for r in rows)
print('calls %d segments %d' % (len(rows), nseg))
print('output tokens total %d  (%.0f/segment)' % (to, to/nseg))
print('  tool-argument tokens ~%.0f (%.1f%%)  = %.0f/segment' % (ta, 100*ta/to, ta/nseg))
print('  visible text tokens  ~%.0f (%.1f%%)  = %.0f/segment' % (tt, 100*tt/to, tt/nseg))
print('  RESIDUAL (extended thinking) ~%.0f (%.1f%%) = %.0f/segment' % (to-ta-tt, 100*(to-ta-tt)/to, (to-ta-tt)/nseg))
print('calls containing a thinking block: %d/%d (%.0f%%)' % (sum(r['think'] for r in rows), len(rows), 100*sum(r['think'] for r in rows)/len(rows)))
