import hashlib,json,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent
obj=json.loads((P/'freeze.json').read_bytes())
assert set(obj['files'])=={str(p.relative_to(P)) for p in P.rglob('*') if p.is_file() and p!=P/'freeze.json'}
for rel,v in obj['files'].items():
 raw=(P/rel).read_bytes();assert len(raw)==v['bytes'] and hashlib.sha256(raw).hexdigest()==v['sha256']
names=['freeze.json','review.md','review.json','resolved-spans.json','errata-refutations.json','bank-ready-errata.json','completion-proof.json']
print(json.dumps({'verdict':'APPROVE','verified_members':len(obj['files']),'member_bytes':obj['total_bytes'],'pins':[{'path':str(P/n),'bytes':len((P/n).read_bytes()),'sha256':hashlib.sha256((P/n).read_bytes()).hexdigest()} for n in names]},indent=2))
