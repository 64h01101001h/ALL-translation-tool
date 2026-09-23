import json, glob, os, collections, statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
print('meta files:', len(metas))
m=json.load(open(metas[0]))
print(json.dumps(m)[:1200])
