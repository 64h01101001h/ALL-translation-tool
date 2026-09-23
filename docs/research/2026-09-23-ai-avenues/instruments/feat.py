import json,re,sqlite3,collections,math
R='/Users/adamderickandrade/ALL-translation-tool/'
V=json.load(open(R+'data/alignment/audit_verdicts_20260904.json'))
items=V['items']
freq=json.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/freq2.json'))
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('[]','null')")
g=collections.defaultdict(set)
for w,gg in cur.fetchall():
    try: arr=json.loads(gg)
    except Exception: arr=[]
    for a in arr:
        if isinstance(a,str): g[w].add(a.strip().lower())
FN=re.compile(r'^(the|a|an|and|or|his|our|your|I|you)\b', re.I)
def label(it):
    # confirmed = first_read DEFECTIVE confirmed, or second_reader CHALLENGE confirmed
    fr=it.get('first_read') or {}; sr=it.get('second_reader') or {}
    sk=it.get('skeptics') or it.get('skeptic') or None
    return it
# determine the confirmed flag per item
keys=set()
for it in items: keys|=set(it.keys())
print('item keys:', sorted(keys))
def confirmed(it):
    for k in ['confirmed','final','verdict_final','outcome']:
        if k in it: return it[k]
    return None
print('example confirmed field:', {k:items[0].get(k) for k in ['confirmed','final','verdict_final','outcome'] if k in items[0]})
