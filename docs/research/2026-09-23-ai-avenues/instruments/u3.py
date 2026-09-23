import json, glob, os, collections
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
f=base+'278f36fb-d2cc-471e-85a2-1b1ad6303489.jsonl'
hits=collections.Counter()
for line in open(f, errors='replace'):
    if 'cache_creation_input_tokens' in line and ('"agent"' in line or 'Workflow' in line or 'propose:' in line):
        hits['cand']+=1
        if hits['cand']<=1:
            d=json.loads(line)
            print(json.dumps(d)[:1500])
print(hits)
