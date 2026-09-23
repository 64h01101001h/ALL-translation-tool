import json, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
idx=[(a['wf'],a['f'],a['role']) for a in A if a['role']]
mods=collections.Counter(); fts=0; like=0; sqlcmds=0
for wf,fn,role in idx:
    for l in open(os.path.join(D,wf,fn)):
        try: r=json.loads(l)
        except: continue
        mg=r.get('message') or {}
        if r.get('type')=='assistant' and mg.get('model'): mods[(role,mg['model'])]+=1
        if r.get('type')=='assistant':
            for b in (mg.get('content') or []):
                if isinstance(b,dict) and b.get('type')=='tool_use' and b.get('name')=='Bash':
                    c=(b.get('input') or {}).get('command') or ''
                    if 'sqlite3' in c:
                        sqlcmds+=1
                        if re.search(r'corpus_fts|english_fts|entries_fts',c): fts+=1
                        if re.search(r'(?i)\blike\b',c): like+=1
print('model rows by role:'); 
for k,v in mods.most_common(): print('  ',k,v)
print(f'\nsqlite3 Bash commands {sqlcmds}; naming an FTS table {fts} ({100*fts/sqlcmds:.1f}%); using LIKE {like} ({100*like/sqlcmds:.1f}%)')
