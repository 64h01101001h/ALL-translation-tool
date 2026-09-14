from pathlib import Path
import json,sys,hashlib,datetime,difflib
D=Path(__file__).resolve().parent;R=D.parent
assert sys.dont_write_bytecode

def pin(b):return dict(bytes=len(b),sha256=hashlib.sha256(b).hexdigest())
h=D/'source-quotation-metadata-history';h.mkdir()
for seq in [284,285]:
 p=R/str(seq)/'proposed-head-allowances.json';old=p.read_bytes();assert 'pa’i'.encode() in old;new=old.replace('pa’i'.encode(),b"pa'i");(h/f'{seq}-before.json').write_bytes(old);p.write_bytes(new)
 for label,a,b in [('forward',old,new),('inverse',new,old)]:
  (h/f'{seq}-{label}.diff').write_text(''.join(difflib.unified_diff(a.decode().splitlines(True),b.decode().splitlines(True),fromfile='before' if label=='forward' else 'after',tofile='after' if label=='forward' else 'before')))
(h/'explanation.json').write_text(json.dumps({'reason':'Use literal source ASCII apostrophe in la sogs pa\'i licensing explanation; preserve prior typographic apostrophe as metadata history. Actual licensor sogs, source spans/tuples, specs, notes and bodies remain byte-exact. No canonical rerun justified by this explanatory-only correction.'},indent=2)+'\n')
results=[]
for seq in [283,284,285]:
 d=R/str(seq);p=d/'freeze.json';assert not p.exists();files={str(x.relative_to(d)):pin(x.read_bytes()) for x in sorted(d.rglob('*')) if x.is_file()}
 o={'seq':seq,'status':'PROVISIONAL_RECONCILIATION_FROZEN','producer':'Codex model reconciler with reused context; neither current original author','frozen_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'files':files,'file_count':len(files),'total_bytes':sum(r['bytes'] for r in files.values()),'schema':'files DICT relative to segment root; every current file except this freeze.json, including nested failed and superseded history.','source_shared_directory':str(D),'source_manifest_pin':pin((D/'original-copy-manifest.json').read_bytes()),'semantic_approval':False}
 p.write_text(json.dumps(o,indent=2,ensure_ascii=False)+'\n');assert {str(x.relative_to(d)) for x in d.rglob('*') if x.is_file() and x!=p}==set(files)
 for name,hsh in files.items():assert pin((d/name).read_bytes())==hsh
 results.append({'seq':seq,'freeze':str(p),**pin(p.read_bytes()),'file_count':len(files),'total_bytes':o['total_bytes']})
print(json.dumps({'status':'DONE_FROZEN_PROVISIONAL_RECONCILIATION','freezes':results},indent=2))
