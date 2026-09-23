import json,os,glob,collections,statistics
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows'); SUB=os.path.join(B,'subagents','workflows')
comp=[]
for f in sorted(glob.glob(os.path.join(WF,'wf_*.json'))):
    d=json.load(open(f))
    if d.get('workflowName')=='c05-align' and d.get('status')=='completed': comp.append(os.path.basename(f)[:-5])

specs={}   # (seq,angle) -> spans set
toolcensus=collections.Counter(); bashcensus=collections.Counter(); nagents=0
for w in comp:
    for f in glob.glob(os.path.join(SUB,w,'agent-*.jsonl')):
        nagents+=1; last=None; angle=None; seq=None
        for line in open(f,errors='replace'):
            try:o=json.loads(line)
            except:continue
            m=o.get('message') or {}
            c=m.get('content')
            if isinstance(c,list):
                for b in c:
                    if not isinstance(b,dict):continue
                    if b.get('type')=='tool_use':
                        nm=b.get('name'); toolcensus[nm]+=1
                        if nm=='Bash':
                            cmd=(b.get('input') or {}).get('command','')
                            if 'gen_alignment_page' in cmd: bashcensus['generator']+=1
                            elif 'hgm_spine' in cmd or 'sqlite3' in cmd: bashcensus['spine']+=1
                            elif 'alignment_full_v1' in cmd: bashcensus['precedent_full_bank']+=1
                            elif 'alignment_evidence_v1' in cmd: bashcensus['evidence_index']+=1
                            elif 'botok' in cmd: bashcensus['botok']+=1
                            else: bashcensus['other']+=1
                        if nm=='StructuredOutput':
                            last=b.get('input')
        if last and isinstance(last,dict):
            seq=last.get('seq'); angle=last.get('angle')
            sp=set()
            for s in (last.get('spans') or []):
                if isinstance(s,dict): sp.add((s.get('tib'),s.get('eng'),s.get('d')))
            if seq is not None and angle: specs[(seq,str(angle))]=sp
print('agents scanned:',nagents,'specs recovered:',len(specs))
angles=collections.Counter(a for (_,a) in specs)
print('angles:',dict(angles))
seqs=sorted({s for (s,a) in specs})
J=[];both=[];inA=[];inB=[];neither=[];drops=[];disp=[];dispkept=[]
tot_un=0;tot_drop=0;segs_with_drop=0;tot_disp=0;tot_dispkept=0
for s in seqs:
    A=specs.get((s,'tibetan-first')); Bx=specs.get((s,'english-first')); R=specs.get((s,'reconciled'))
    if A is None or Bx is None or R is None: continue
    u=A|Bx
    J.append(len(A&Bx)/len(u) if u else 0)
    if R:
        both.append(len(R&A&Bx)/len(R)); inA.append(len(R&A)/len(R)); inB.append(len(R&Bx)/len(R))
        neither.append(len(R-A-Bx)/len(R))
    un=A&Bx; dr=un-R
    tot_un+=len(un); tot_drop+=len(dr)
    if dr: segs_with_drop+=1
    dd=(A^Bx); tot_disp+=len(dd); tot_dispkept+=len(dd&R)
print()
print('segments with all three: %d'%len(J))
print('Jaccard(tib-first, eng-first): mean %.3f median %.3f'%(statistics.mean(J),statistics.median(J)))
print('reconciled spans already in BOTH: mean %.1f%% median %.1f%%'%(100*statistics.mean(both),100*statistics.median(both)))
print('in tibetan-first %.1f%% | in english-first %.1f%%'%(100*statistics.mean(inA),100*statistics.mean(inB)))
print('reconciler-ORIGINATED: mean %.2f%% median %.2f%%'%(100*statistics.mean(neither),100*statistics.median(neither)))
print('unanimous spans %d, reconciler DROPPED %d (%.2f%%), affecting %d of %d segments'%(tot_un,tot_drop,100*tot_drop/tot_un,segs_with_drop,len(J)))
print('disputed spans %d (%.1f/seg), reconciler KEPT %.1f%%'%(tot_disp,tot_disp/len(J),100*tot_dispkept/tot_disp))
print()
print('TOOL CENSUS:',dict(toolcensus))
print('BASH SPLIT:',dict(bashcensus),' total bash',sum(bashcensus.values()))
n=189
print('per segment: tool calls %.1f  generator %.1f  spine %.1f  precedent %.1f  evidence %.2f  botok %.2f'%(sum(toolcensus.values())/n,bashcensus['generator']/n,bashcensus['spine']/n,bashcensus['precedent_full_bank']/n,bashcensus['evidence_index']/n,bashcensus['botok']/n))
