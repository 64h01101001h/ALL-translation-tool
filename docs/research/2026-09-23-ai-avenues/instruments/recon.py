import json, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
rec=[(a['wf'],a['f'],a['seq']) for a in A if a['role']=='reconcile']
def spans(o):
    return {(s.get('tib'), s.get('eng'), s.get('d')) for s in (o.get('spans') or []) if isinstance(s,dict)}
res=[]
for wf,fn,seq in rec:
    path=os.path.join(D,wf,fn)
    prompt=None; final=None
    lines=open(path).read().splitlines()
    for l in lines:
        try: r=json.loads(l)
        except: continue
        mg=r.get('message') or {}
        c=mg.get('content')
        txt=c if isinstance(c,str) else (' '.join(x.get('text','') for x in c if isinstance(x,dict)) if isinstance(c,list) else '')
        if prompt is None and r.get('type')=='user' and 'PROPOSAL A (' in txt: prompt=txt
        if r.get('type')=='assistant':
            for b in (mg.get('content') or []):
                if isinstance(b,dict) and b.get('type')=='tool_use' and b.get('name')=='StructuredOutput':
                    final=b.get('input')
    if not prompt or not final: continue
    # parse proposals
    try:
        ia=prompt.index('PROPOSAL A ('); ib=prompt.index('PROPOSAL B (')
        a_txt=prompt[prompt.index('\n',ia)+1:ib]
        rest=prompt[prompt.index('\n',ib)+1:]
        iend=rest.find('\n  Read ')
        b_txt=rest[:iend if iend>0 else len(rest)]
        def unind(t):
            ls=[x[2:] if x.startswith('  ') else x for x in t.splitlines()]
            return '\n'.join(ls)
        A1=json.loads(unind(a_txt).strip()); B1=json.loads(unind(b_txt).strip())
    except Exception as e:
        continue
    sa=spans(A1); sb=spans(B1); sf=spans(final)
    res.append((seq,sa,sb,sf))
print('reconcile agents with both proposals + final parsed:',len(res))
tot_f=sum(len(f) for _,_,_,f in res)
both=sum(len(a&b) for _,a,b,_ in res)
unan_kept=sum(len(a&b&f) for _,a,b,f in res)
unan_drop=sum(len((a&b)-f) for _,a,b,f in res)
from_a=sum(len((a-b)&f) for _,a,b,f in res); from_b=sum(len((b-a)&f) for _,a,b,f in res)
neither=sum(len(f-(a|b)) for _,a,b,f in res)
jac=[len(a&b)/len(a|b) for _,a,b,_ in res if (a|b)]
segs_with_drop=sum(1 for _,a,b,f in res if (a&b)-f)
print(f'final spans {tot_f}; unanimous (in both proposals) {both}')
print(f'  unanimous KEPT {unan_kept}; unanimous DROPPED {unan_drop} = {100*unan_drop/both:.2f}% of unanimous')
print(f'  segments with >=1 unanimous drop: {segs_with_drop}/{len(res)} = {100*segs_with_drop/len(res):.1f}%')
print(f'  final from A only {from_a}, from B only {from_b}, from NEITHER {neither} = {100*neither/tot_f:.1f}% of final')
print(f'  in-both share of final {100*unan_kept/tot_f:.1f}%')
import statistics
print(f'  analyst Jaccard mean {statistics.mean(jac):.3f} median {statistics.median(jac):.3f}')
covA=sum(len(f&a) for _,a,b,f in res)/tot_f; covB=sum(len(f&b) for _,a,b,f in res)/tot_f
print(f'  final spans contained in A alone {100*covA:.1f}% ; in B alone {100*covB:.1f}%')
json.dump([[s,sorted(map(list,a)),sorted(map(list,b)),sorted(map(list,f))] for s,a,b,f in res[:0]],open('/dev/null','w'))
# sample drops
n=0
for seq,a,b,f in res:
    for d in sorted((a&b)-f):
        if n<12: print('   DROP seq',seq,d); n+=1
