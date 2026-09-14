#!/usr/bin/env python3
"""Independent English-first source queries; all outputs stay in this own angle."""
import hashlib, json, os, re, sqlite3
from pathlib import Path

assert __debug__ and os.environ.get('PYTHONDONTWRITEBYTECODE') == '1'
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[3]
WT = ROOT / 'campaign-worktree'
BATCH = HERE.parents[1]
SPINE_ALIAS = WT / 'build/hgm_spine_v27_2.db'
SPINE = SPINE_ALIAS.resolve()
db = sqlite3.connect(SPINE.as_uri() + '?mode=ro', uri=True)
db.execute('PRAGMA query_only=ON')
db.row_factory = sqlite3.Row
assert db.execute('PRAGMA query_only').fetchone()[0] == 1
OUT = HERE / 'queries'
OUT.mkdir(exist_ok=True)
def save(name, value):
    p = OUT / (name + '.json')
    assert not p.exists(), p
    p.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n')
def query(name, sql, params=()):
    rows = [dict(row) for row in db.execute(sql, params)]
    save(name, {'sql':sql, 'parameters':params, 'row_count':len(rows), 'rows':rows})
    return rows

query('schema', "SELECT type,name,sql FROM sqlite_master WHERE name IN ('entries','entry_variants','corpus_segments','meta') ORDER BY name")
query('meta', 'SELECT * FROM meta ORDER BY key')
target=query('target-rows', 'SELECT * FROM corpus_segments WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq', ('C05',283,285))
context=query('argument-277-291', 'SELECT * FROM corpus_segments WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq', ('C05',277,291))
snapshots=[]
for name, original in [('source.json',target),('context.json',context)]:
    supplied=json.loads((BATCH/name).read_text())
    if isinstance(supplied,dict):
        supplied=supplied.get('segments',supplied.get('rows'))
    assert isinstance(supplied,list)
    fields=['id','course','seq','acip','wylie','english']
    comparisons=[{'seq':r['seq'],'fields':{f:r[f]==next(s for s in supplied if s['seq']==r['seq'])[f] for f in fields}} for r in original]
    assert len(supplied)==len(original) and all(all(c['fields'].values()) for c in comparisons)
    snapshots.append({'snapshot':name,'row_count':len(original),'comparisons':comparisons})
save('snapshot-verification',snapshots)
for row in target:
    query('parallel-'+str(row['seq']), 'SELECT * FROM corpus_segments WHERE acip=? OR wylie=? OR english=? ORDER BY course,seq', (row['acip'],row['wylie'],row['english']))

terms=['byas pa','byas','kho','na re','skyon','med','yan lag','lnga','skyes bu','cha shas','yin','bya ba',"'jog",'dgos',"phyir",'zer','snga ma','mtshungs','rnag','khrag','sogs','sogs pa','dngos po','gsum','brlan','gsher ba','phor pa','gang','gang po','snang ba','steng','steng du','snang tshul',"mi 'dra ba",'kha cig','phan tshun','go sa',"'gog pa",'las','las can',"'gro ba",'chu','cig car','bltas pa','tshad','tshad ma','grub pa','tshad grub','tshad grub pa','srid','srid pa','khas blangs','khas blangs pa','yang','zhing','na','can']
lex=[]
for i,term in enumerate(terms):
    rows=query('lexical-%02d'%i, 'SELECT * FROM entries WHERE wylie=? OR id IN (SELECT entry_id FROM entry_variants WHERE wylie=?) ORDER BY id', (term,term))
    lex.append({'term':term,'file':'lexical-%02d.json'%i,'rows':len(rows),'hgm':[{'id':r['id'],'wylie':r['wylie'],'hgm_gloss':r['hgm_gloss'],'tier':r['tier']} for r in rows]})
save('lexical-index',lex)
families=[('limbs-person', '%yan lag%skyes bu%'),('parts', '%cha shas%'),('wet-flowing','%brlan%gsher%'),('mutual-spatial','%phan tshun%go sa%'),('valid-established','%tshad grub%'),('simultaneous-look','%cig car%blta%'),('case-same','%snga ma%mtshungs%')]
for name,pattern in families:
    query('corpus-'+name,'SELECT * FROM corpus_segments WHERE wylie LIKE ? ORDER BY course,seq',(pattern,))
save('source-boundary',{'role':'Codex independent English-first original proposer, provisional and unreviewed; historical agent name does not identify current batch or role.','spine_alias':str(SPINE_ALIAS),'resolved_spine':str(SPINE),'spine_bytes':SPINE.stat().st_size,'spine_sha256':hashlib.sha256(SPINE.read_bytes()).hexdigest(),'query_only':1,'sqlite_uri_mode':'ro','python_assertions':__debug__,'six_snapshot_fields':True,'baseline_commit':json.loads((BATCH/'baseline.json').read_text())['commit'],'scope':'Read only batch brief, source/context/baseline and own angle; no other angle, root judgments or reconciled sources read.'})
print(json.dumps({'target_rows':len(target),'context_rows':len(context),'lexical_queries':len(lex),'corpus_families':len(families),'snapshot_six_fields':'PASS'},sort_keys=True))
