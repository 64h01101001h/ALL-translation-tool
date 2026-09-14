import datetime,hashlib,json,os,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent;B=P.parent;A=B.parent;W=A.parent/'campaign-worktree';S=B/'semantic-source-preparation'
def dump(name,v):
 p=P/name;p.parent.mkdir(parents=True,exist_ok=True);assert not p.exists();p.write_text(json.dumps(v,ensure_ascii=False,indent=2)+'\n')
def pin(p):
 b=p.read_bytes();return {'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
def verify(root,manifest):
 o=json.loads(manifest.read_bytes());
 for name,v in o['files'].items():assert pin(root/name)==v,(manifest,name)
 return len(o['files'])
release=A/'integration/through285-frozen-final-read-and-parallel-semantic-release/release.json'
assert pin(release)=={'bytes':4571,'sha256':'5011025d47d6054bf181769bf963de3c0ebfa9ece6ddb36b36abbbc43d4125ef'}
assert pin(S/'freeze.json')['sha256']=='0b621426c42afd849a42320f1d184307ce219a2ccd69aa9e98d0e9c510efbda9'
counts={'source_preparation':verify(S,S/'freeze.json'),'reconciliation_boundary':verify(B/'reconciled',B/'reconciled/completion-boundary-freeze.json')}
for seq in [283,284,285]:counts[str(seq)]=verify(B/'reconciled'/str(seq),B/'reconciled'/str(seq)/'freeze.json')
manifest=[];objects=P/'evidence-objects';objects.mkdir()
def copy(src,dest):
 b=src.read_bytes();v=pin(src);obj=objects/v['sha256']
 if not obj.exists():obj.write_bytes(b)
 assert obj.read_bytes()==b
 dest.parent.mkdir(parents=True,exist_ok=True)
 if dest!=obj:os.link(obj,dest)
 manifest.append({'original_path':str(src),'copy_path':str(dest),'content_addressed_copy':str(obj),**v})
for src in sorted((B/'reconciled').rglob('*')):
 if src.is_file():copy(src,P/'reviewed-final'/src.relative_to(B/'reconciled'))
for angle in ['tibetan','english']:
 for src in sorted((B/angle).rglob('*')):
  if src.is_file():copy(src,P/'original-angles'/angle/src.relative_to(B/angle))
for src in [release,A/'semantic-review-protocol.md',A/'THROUGHPUT_REFINEMENT.md',B/'root-review-targets.md',A/'integration/through285-root-proposal-comparison/review-questions.md',W/'docs/errata_register.json',W/'tools/gen_alignment_page.py',W/'tools/build_alignment_layer.py',W/'tools/test_no_supplied_span_head.py']:
 copy(src,P/'governing'/src.name)
dump('original-copy-manifest.json',manifest)
dump('source-preparation-reference.json',{'root':str(S),'freeze':pin(S/'freeze.json'),'verified_members':counts['source_preparation'],'original_copy_manifest':str(S/'original-copy-manifest.json'),'queries':str(S/'query-index.json'),'full_source_query':str(S/'queries/02-context.json'),'raw_archive_proof':str(S/'raw-archive-equality.json'),'frozen_evidence_reused_without_mutation':True})
dump('input-verification.json',{'counts':counts,'release':pin(release),'file_copies':len(manifest),'unique_physical_copies':len(list(objects.iterdir())),'copy_layout':'Exact content-addressed private copies with hard links for every original relative path; no links to source originals or frozen preparation.','root_mechanical_status_at_release':'PENDING; not asserted passing','historical_identity':'Fresh Codex semantic reviewer; inherited GPT-6 identity, exact runtime model unavailable. Earlier count/hash-only metadata exposure disclosed in source preparation report.'})
print(json.dumps({'verified_counts':counts,'copied_paths':len(manifest),'unique_contents':len(list(objects.iterdir()))},indent=2))
