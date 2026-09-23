import json,os,glob,collections
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
SUB=os.path.join(B,'subagents','workflows')
def cost(wfid):
    byid={}
    for f in glob.glob(os.path.join(SUB,wfid,'agent-*.jsonl')):
        for line in open(f,errors='replace'):
            try:o=json.loads(line)
            except:continue
            if o.get('type')!='assistant':continue
            m=o.get('message') or {};u=m.get('usage')
            if not u:continue
            e=byid.setdefault(m.get('id'),dict(i=u.get('input_tokens') or 0,cw=u.get('cache_creation_input_tokens') or 0,cr=u.get('cache_read_input_tokens') or 0,o=0))
            e['o']=max(e['o'],u.get('output_tokens') or 0)
    t=collections.Counter()
    for v in byid.values():
        t['i']+=v['i'];t['cw']+=v['cw'];t['cr']+=v['cr'];t['o']+=v['o']
    d=(t['i']*5+t['cw']*6.25+t['cr']*0.5+t['o']*25)/1e6
    return t,d
tot=0
for w,lab in [('wf_16aeea90-db0','KILLED seqs361-369'),('wf_93755fb2-96f','p56'),('wf_d26676a2-f7f','p57')]:
    t,d=cost(w); tot+=d
    print('%-18s %-20s in=%d cw=%d cr=%d out=%d  COST=$%.2f'%(w,lab,t['i'],t['cw'],t['cr'],t['o'],d))
print('EXCLUDED TOTAL $%.2f'%tot)
print('doc measured cohort $1264.73 over 189 segments = $6.6917/seg')
print('all-in c05 spend $%.2f over 189 LANDED segments = $%.4f/seg'%(1264.73+tot,(1264.73+tot)/189))
print('inflation vs doc: %.1f%%'%(((1264.73+tot)/1264.73-1)*100))
