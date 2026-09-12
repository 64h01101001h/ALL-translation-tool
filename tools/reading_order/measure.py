import json,io,collections,subprocess,sys
SCR="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad"
d=json.load(io.open('data/alignment/alignment_full_v1.json'))
seg=collections.defaultdict(list)
for L in d['links']: seg[(L['course'],L['page'],L['seg'])].append(L)
def kids(p,pool):
    pt=p['tib'] or ''; out=[];pos=0
    for c in sorted(pool,key=lambda x:x['id']):
        ct=c['tib'] or ''
        if not ct: continue
        i=pt.find(ct,pos)
        if i<0: i=pt.find(ct)
        if i<0: continue
        out.append((i,c)); pos=max(pos,i)
    out.sort(key=lambda x:x[0]); return [c for _,c in out]
cases=[]
for key,ls in seg.items():
    byd=collections.defaultdict(list)
    for L in ls: byd[L['d']].append(L)
    for dep in sorted(byd):
        for p in byd[dep]:
            ch=[c for c in kids(p,byd.get(dep+1,[])) if c.get('eng')]
            if len(ch)<2: continue
            pe=p.get('eng') or ''
            pos=[pe.find(c['eng']) for c in ch]
            if min(pos)<0 or len(set(pos))<len(pos): continue
            cases.append((p,ch,pos,dep))
lines=[]
for i,(p,ch,pos,dep) in enumerate(cases):
    lines.append(f"P{i}\t{p['tib']}")
    for j,c in enumerate(ch): lines.append(f"C{i}_{j}\t{c['tib']}")
r=subprocess.run([f"{SCR}/orderprobe","build/hgm_spine_v27_2.db"],
                 input="\n".join(lines),capture_output=True,text=True)
out={}
for ln in r.stdout.splitlines():
    f=ln.split("\t"); out[f[0]]=f[1:]
def parse(f):
    o=[]
    for x in f:
        if "|" not in x or ":" not in x.split("|",1)[0]: continue
        head,txt=x.split("|",1); ci,order=head[1:].split(":")
        o.append((int(ci),int(order),txt))
    return o
eng=idt=tot=0; bydep=collections.defaultdict(lambda:[0,0,0]); unmap=0
for i,(p,ch,pos,dep) in enumerate(cases):
    pf=out.get(f"P{i}")
    if not pf: unmap+=1; continue
    pc=parse(pf)
    if not pc: unmap+=1; continue
    joined=" ".join(t for _,_,t in pc); starts=[];acc=0
    for _,_,t in pc: starts.append(acc); acc+=len(t)+1
    pr=[];ok=True
    for j,c in enumerate(ch):
        cf=out.get(f"C{i}_{j}")
        if not cf: ok=False;break
        cc=parse(cf)
        if not cc: ok=False;break
        ca=" ".join(t for _,_,t in cc); k=joined.find(ca)
        if k<0: ok=False;break
        ix=max(x for x in range(len(pc)) if starts[x]<=k)
        ci,order,_=pc[ix]
        # predicted global order: clause first (written order), then the
        # engine's ruling inside the clause; order 0 attaches rightward
        pr.append(((ci, order if order>0 else 99), j))
    if not ok or len(pr)<2: unmap+=1; continue
    pred=[j for _,j in sorted(pr)]
    obs=[j for _,j in sorted(zip(pos,range(len(ch))))]
    ident=list(range(len(ch)))
    tot+=1; eng+=(pred==obs); idt+=(ident==obs)
    bydep[dep][0]+=1; bydep[dep][1]+=(pred==obs); bydep[dep][2]+=(ident==obs)
print(f"candidate spans: {len(cases)}   comparable: {tot}   unmappable: {unmap}")
print(f"\n  engine (clause order + ruling inside) : {eng:4}  {100*eng/max(1,tot):.0f}%")
print(f"  plain left to right                  : {idt:4}  {100*idt/max(1,tot):.0f}%\n")
LVL={1:'segment->clauses',2:'clause->phrases',3:'phrase->sub',4:'sub->words',5:'word->particles',6:'particle->syl'}
print(f"{'level':<20}{'spans':>7}{'engine':>9}{'left-to-right':>15}")
for dep in sorted(bydep):
    n,e,ii=bydep[dep]
    if n<8: continue
    print(f"  {LVL.get(dep,dep):<18}{n:>7}{100*e/n:>8.0f}%{100*ii/n:>14.0f}%")
