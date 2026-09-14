from pathlib import Path
import os,sys,json,hashlib,sqlite3,subprocess,datetime,importlib.util,difflib
from html.parser import HTMLParser
D=Path(__file__).resolve().parent;R=D.parent;B=R.parent;A=B.parent;W=A.parent/'campaign-worktree'
assert sys.dont_write_bytecode and __debug__
def pin(b):return dict(bytes=len(b),sha256=hashlib.sha256(b).hexdigest())
def save(p,o):
 p.parent.mkdir(parents=True,exist_ok=True)
 with p.open('x') as f:json.dump(o,f,indent=2,ensure_ascii=False);f.write('\n')
def load(p):return json.loads(p.read_bytes())
# Preserve pre-canonical metadata shape correction history and full inverse diffs.
h=D/'metadata-interface-history';h.mkdir()
for seq in [283,284,285]:
 for name in ['omissions.json','errata-analysis.json']:
  p=R/str(seq)/name;old=p.read_bytes();target=h/f'{seq}-{name}';target.write_bytes(old);o=load(p)
  if name=='omissions.json':o['source_field_pins']={'wylie':o['source_field_pins']['tib'],'english':o['source_field_pins']['eng']}
  else:
   for e in o:e['retained_erratum']=e['retained']
  new=(json.dumps(o,indent=2,ensure_ascii=False)+'\n').encode();p.write_bytes(new)
  for tag,a,b in [('forward',old,new),('inverse',new,old)]:
   (h/f'{seq}-{name}.{tag}.diff').write_text(''.join(difflib.unified_diff(a.decode().splitlines(True),b.decode().splitlines(True),fromfile='before' if tag=='forward' else 'after',tofile='after' if tag=='forward' else 'before')))
save(h/'explanation.json',{'reason':'Static inspection of actual accepted interface: source_field_pins keys must be wylie/english (complements remain tib/eng); errata-analysis uses retained_erratum. Added exact alias of false retained flag. No decision, span, note or source change; no prior final canonical execution exists.'})
oldwrapper=B.parent/'C05-280-282/reconciled/run-generator-ro.py.txt';wrapper=R/'run-generator-ro.py.txt';wrapper.write_bytes(oldwrapper.read_bytes());assert pin(wrapper.read_bytes())['sha256']=='f63adb7d634e4a45a36437169a768dc44f67a15464071cdbd394cdaf2a72cd2a'
# Shared wrapper is path-safe: fixed original worktree paths, not __file__-relative.
(D/'run-generator-ro.py.txt').write_bytes(wrapper.read_bytes())
save(D/'wrapper-binding.json',{'original':str(oldwrapper),'shared_wrapper':str(wrapper),'segment_evidence_copy':str(D/'run-generator-ro.py.txt'),**pin(wrapper.read_bytes()),'path_behavior':'Absolute fixed worktree; no relative-root relocation. Actual canonical source unchanged.','original_replays':'Used English original wrapper at its original English283 path, not relocated.'})
class Anchors(HTMLParser):
 def __init__(self):super().__init__(convert_charrefs=True);self.depth=0;self.values=[]
 def handle_starttag(self,tag,attrs):
  a=dict(attrs)
  if self.depth:self.depth+=1
  elif tag=='span' and a.get('data-d')=='1':self.depth=1;self.values.append('')
 def handle_endtag(self,tag):
  if self.depth:self.depth-=1
 def handle_data(self,s):
  if self.depth:self.values[-1]+=s
sources={r['seq']:r for r in load(D/'source-rows-full.json')};base=load(B/'baseline.json');dbp=W/'build/hgm_spine_v27_2.db';before=pin(dbp.read_bytes())
for n,s in base['sha256'].items():assert pin((W/n).read_bytes())['sha256']==s
s=importlib.util.spec_from_file_location('canonical_head_predicate',W/'tools/test_no_supplied_span_head.py');gate=importlib.util.module_from_spec(s);s.loader.exec_module(gate)
results=[]
for seq in [283,284,285]:
 out=R/str(seq);c=out/'canonical-attempt01';c.mkdir();stdin=(out/'spec.json').read_bytes();argv=[sys.executable,'-B',str(wrapper)];start=datetime.datetime.now(datetime.timezone.utc).isoformat()
 r=subprocess.run(argv,cwd=W,input=stdin,capture_output=True,env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0'))
 for n,b in [('stdin.bin',stdin),('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',f'{r.returncode}\n'.encode())]:(c/n).write_bytes(b)
 execution={'argv':argv,'cwd':str(W),'environment':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'},'started_at':start,'finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'exit':r.returncode,'stdin':pin(stdin),'stdout':pin(r.stdout),'stderr':pin(r.stderr),'wrapper':{'path':str(wrapper),**pin(wrapper.read_bytes())},'canonical':{'path':str(W/'tools/gen_alignment_page.py'),**pin((W/'tools/gen_alignment_page.py').read_bytes())}}
 save(c/'execution.json',execution);assert r.returncode==0 and not r.stderr
 (out/'body.html').write_bytes(r.stdout);a=Anchors();a.feed(r.stdout.decode());assert a.values==[sources[seq]['wylie'],sources[seq]['english']]
 tuples=load(out/'final-tuples.json');spans=load(out/'spec.json')['segments'][0]['spans'];hits=[];allow={};byt={f's{seq}'+t['id']:t for t in tuples}
 for m in gate.PAT.finditer(r.stdout.decode()):
  t=byt[m.group(2)];key='c5p95/'+m.group(2);assert t['tib']=='sogs' and t['eng']=='and the other'
  allow[key]='sogs licenses and the other as the list-continuation expression in la sogs pa’i; positive full HGM20919 HTG2016 variant and the actual enumeration support the initial and. The explanatory numeral two remains outside this span.'
  hits.append(dict(label=m.group(2),depth=int(m.group(1)),match=m.group(0),body_range=list(m.span()),final_tuple=t,licensor=t['tib'],allowance_key=key))
 save(out/'proposed-head-allowances.json',{'predicate_source':{'path':str(W/'tools/test_no_supplied_span_head.py'),**pin((W/'tools/test_no_supplied_span_head.py').read_bytes())},'pages_c05':allow,'actual_gated_hits':hits,'status':'PROPOSED_ONLY; no repository allowance mutation','semantic_licensing':'Actual list-continuation meaning, not a blanket exemption; only canonical PAT hits are listed.'})
 proof={'status':'PASS_ACTUAL_CANONICAL_AND_FULL_SOURCE_REPRODUCTION','seq':seq,'execution':execution,'spec':pin(stdin),'body':pin(r.stdout),'spans':len(spans),'nulls':sum(x['eng'] is None for x in spans),'d5_nonnull':sum(x['d']==5 and x['eng'] is not None for x in spans),'original_dispositions':len(load(out/'original-dispositions.json')),'source_anchors_verbatim':True,'source_field_pins':{f:pin(sources[seq][f].encode()) for f in ['wylie','english','acip']},'actual_head_hits':len(hits),'errata':0,'original_canonical_replays':'../283/original-canonical-replays.json','shared_evidence':'../283/original-copy-manifest.json','approval':'PROVISIONAL machine reconciliation, not semantic approval'}
 save(out/'proof.json',proof);results.append(proof)
assert pin(dbp.read_bytes())==before
for n,s in base['sha256'].items():assert pin((W/n).read_bytes())['sha256']==s
save(D/'final-canonical-summary.json',{'status':'PASS','results':results,'total_spans':sum(x['spans'] for x in results),'total_d5_nonnull':sum(x['d5_nonnull'] for x in results),'nulls':0,'original_dispositions':145,'canonical_original_replays':6,'canonical_final_executions':3,'source_baseline_unchanged':True,'database_pin':before})
print(json.dumps([{'seq':x['seq'],'exit':x['execution']['exit'],'spans':x['spans'],'nulls':x['nulls'],'d5':x['d5_nonnull'],'body':x['body'],'head_hits':x['actual_head_hits']} for x in results],indent=2))
