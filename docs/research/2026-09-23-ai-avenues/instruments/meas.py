import json,os,glob,statistics,collections
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows')
runs=[]
status_c=collections.Counter(); name_c=collections.Counter()
for f in glob.glob(os.path.join(WF,'wf_*.json')):
    d=json.load(open(f))
    name_c[d.get('workflowName')]+=1
    if d.get('workflowName')!='c05-align': continue
    status_c[d.get('status')]+=1
    runs.append((os.path.basename(f)[:-5],d))
print('workflowName census:',dict(name_c))
print('c05-align status census:',dict(status_c))
print('total c05 runs:',len(runs))
