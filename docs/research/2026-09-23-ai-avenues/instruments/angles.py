import json,glob,os,statistics
from collections import defaultdict,Counter
BASE=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
ids={json.load(open(f))['runId'] for f in glob.glob(BASE+'/workflows/wf_*.json')
     if json.load(open(f)).get('workflowName')=='c05-align' and json.load(open(f)).get('status')=='completed'}
specs=defaultdict(dict)   # seq -> angle -> spanset
for runId in ids:
    for jf in glob.glob(os.path.join(BASE,'subagents','workflows',runId)+'/agent-*.jsonl'):
        last=None
        for line in open(jf):
            try: o=json.loads(line)
            except: continue
            if o.get('type')!='assistant': continue
            for b in ((o.get('message') or {}).get('content') or []):
                if b.get('type')=='tool_use' and b.get('name')=='StructuredOutput':
                    last=b.get('input') or {}
        if not last: continue
        seq=last.get('seq'); ang=last.get('angle')
        if seq is None or not ang: continue
        sp={(s.get('tib','').strip(), (s.get('eng') or '').strip(), s.get('d'))
            for s in (last.get('spans') or []) if s.get('tib')}
        specs[seq][ang]=sp
full=[s for s,v in specs.items() if len(v)>=3 and 'reconciled' in v]
print('segments with both angles + reconciled:',len(full))
def J(a,b): return len(a&b)/len(a|b) if (a|b) else 1.0
jab=[];ra=[];rb=[];orig=[];unan_kept=[];unan_drop=[];dropcnt=0;segs_with_drop=0
for s in full:
    v=specs[s]; keys=[k for k in v if k!='reconciled']
    if len(keys)<2: continue
    A,B,Rc=v[keys[0]],v[keys[1]],v['reconciled']
    jab.append(J(A,B)); ra.append(len(Rc&A)/len(Rc) if Rc else 0); rb.append(len(Rc&B)/len(Rc) if Rc else 0)
    U=A&B
    orig.append(len(Rc-(A|B))/len(Rc) if Rc else 0)
    kept=len(U&Rc); dr=len(U-Rc)
    unan_kept.append(kept); unan_drop.append(dr)
    if dr: segs_with_drop+=1; dropcnt+=dr
    # what ONE angle alone would have missed relative to reconciled
print('angle A vs angle B  Jaccard: mean %.3f median %.3f'%(statistics.mean(jab),statistics.median(jab)))
print('share of RECONCILED spans present in angle A: %.1f%%'%(100*statistics.mean(ra)))
print('share of RECONCILED spans present in angle B: %.1f%%'%(100*statistics.mean(rb)))
print('share of RECONCILED spans in NEITHER angle (reconciler-originated): %.1f%%'%(100*statistics.mean(orig)))
tu=sum(unan_kept)+sum(unan_drop)
print('UNANIMOUS spans (in both angles): %d | kept %d | DROPPED by reconciler %d (%.2f%%)'%(tu,sum(unan_kept),dropcnt,100*dropcnt/tu if tu else 0))
print('segments where the reconciler dropped >=1 unanimous span: %d/%d (1 in %.1f)'%(segs_with_drop,len(jab),len(jab)/segs_with_drop if segs_with_drop else 0))
# the load-bearing number: what a SINGLE angle would lose
loss=[1-x for x in ra]
print('\nIF ONLY ANGLE A RAN: it would miss %.1f%% of the reconciled spans on average (median %.1f%%)'%(100*statistics.mean(loss),100*statistics.median(loss)))
