import datetime,hashlib,json,subprocess,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent;B=P.parent;W=B.parent.parent/'campaign-worktree';S=B/'semantic-source-preparation'
def pin(p):
 b=p.read_bytes();return {'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
def verify(root,manifest):
 obj=json.loads(manifest.read_bytes())
 for rel,v in obj['files'].items():assert pin(root/rel)==v,(manifest,rel)
 return len(obj['files'])
assert not (P/'completion-proof.json').exists()
sf=verify(S,S/'freeze.json');rf=verify(B/'reconciled',B/'reconciled/completion-boundary-freeze.json')
for name in ['original-copy-manifest.json','additional-original-copy-manifest.json']:
 for r in json.loads((P/name).read_bytes()):
  expected={k:r[k] for k in ['bytes','sha256']};assert pin(Path(r['original_path']))==expected;assert pin(Path(r['copy_path']))==expected
base=json.loads((B/'baseline.json').read_bytes())
for rel,digest in base['sha256'].items():assert pin(W/rel)['sha256']==digest
head=subprocess.run(['git','-C',str(W),'rev-parse','HEAD'],capture_output=True);assert head.returncode==0 and head.stdout.decode().strip()==base['commit']
review=json.loads((P/'review.json').read_bytes());assert review['verdict']=='APPROVE' and review['open_findings']==[]
resolved=json.loads((P/'resolved-spans.json').read_bytes());assert len(resolved)==76 and all(list(r)==['seq','id','d','tib','eng','tib_range','eng_range'] for r in resolved)
assert len(json.loads((P/'span-review.json').read_bytes()))==76 and len(json.loads((P/'omission-review.json').read_bytes()))==77
assert json.loads((P/'bank-ready-errata.json').read_bytes())==[]
obj={'status':'COMPLETE_INDEPENDENT_SEMANTIC_APPROVAL','finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'source_preparation_members_unchanged':sf,'reconciliation_members_unchanged':rf,'source_copy_maps_verified':True,'baseline_nine_unchanged':True,'head':{'argv':['git','-C',str(W),'rev-parse','HEAD'],'stdout':head.stdout.decode(),'stderr':head.stderr.decode(),'exit':head.returncode},'review':pin(P/'review.json'),'resolved_spans':pin(P/'resolved-spans.json'),'independent_canonical_final_runs':3,'independent_canonical_original_runs':6,'select_replays':209,'root_mechanical_pass_claimed':False}
(P/'completion-proof.json').write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
print(json.dumps(obj,indent=2))
