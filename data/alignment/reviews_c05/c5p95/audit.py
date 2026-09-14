import datetime,gzip,hashlib,importlib.util,json,os,sqlite3,subprocess,sys
from pathlib import Path
from html.parser import HTMLParser
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent;B=P.parent;A=B.parent;W=A.parent/'campaign-worktree';S=B/'semantic-source-preparation';F=P/'reviewed-final'
def dump(name,v):
 p=P/name;p.parent.mkdir(parents=True,exist_ok=True);assert not p.exists(),p;p.write_text(json.dumps(v,ensure_ascii=False,indent=2)+'\n')
def pin(p):
 b=p.read_bytes();return {'path':str(p),'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
baseline=json.loads((B/'baseline.json').read_bytes())
for rel,h in baseline['sha256'].items():assert pin(W/rel)['sha256']==h
dbp=W/'build/hgm_spine_v27_2.db';before=pin(dbp)
c=sqlite3.connect(dbp.resolve().as_uri()+'?mode=ro',uri=True);c.row_factory=sqlite3.Row;c.execute('PRAGMA query_only=ON');assert c.execute('PRAGMA query_only').fetchone()[0]==1
ctxsql='SELECT * FROM corpus_segments WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq';ctxparams=['C05',277,291]
context=[dict(r) for r in c.execute(ctxsql,ctxparams)]
assert context==json.loads((S/'queries/02-context.json').read_bytes())['rows']
dump('source-reopened.json',{'uri':dbp.resolve().as_uri()+'?mode=ro','initialization_sql':'PRAGMA query_only=ON','sql':ctxsql,'parameters':ctxparams,'count':len(context),'rows':context})
source={r['seq']:r for r in context}
replays=[];rawentries={};rawcorpus={};qfiles=sorted((F/'283/queries').glob('*.json'))
assert len(qfiles)==210
for index,path in enumerate(qfiles):
 q=json.loads(path.read_bytes());sql=q['sql'];params=q['parameters'];assert sql.lstrip().upper().startswith(('SELECT','PRAGMA QUERY_ONLY'))
 rows=[dict(r) for r in c.execute(sql,params)];assert rows==q['rows'],path;assert len(rows)==q['count']
 dest='query-replays/'+path.name
 dump(dest,{'source_query':str(B/'reconciled/283/queries'/path.name),'uri':dbp.resolve().as_uri()+'?mode=ro','sql':sql,'parameters':params,'count':len(rows),'rows':rows})
 replays.append({'file':str(P/dest),'sql':sql,'parameters':params,'count':len(rows),'matches_frozen_full_rows':True})
 for r in rows:
  if 'raw' in r:
   if 'seq' in r:rawcorpus[r['id']]=r
   else:rawentries[r['id']]=r
dump('query-replay-index.json',replays)
originals=Path('/Users/adamderickandrade/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff')
mp=originals/'release/hgm_dictionary_v27_2.json.gz';cp=originals/'data/full_parallel_corpus_v32.json.gz'
master=json.loads(gzip.decompress(mp.read_bytes()));corpus=json.loads(gzip.decompress(cp.read_bytes()))
checks=[]
for rs,arr,archive,prefix in [(rawentries,master['unified_entries'],mp,'/unified_entries'),(rawcorpus,corpus,cp,'')]:
 for ident,row in sorted(rs.items()):
  raw=json.loads(row['raw']);assert raw==arr[ident-1]
  checks.append({'id':ident,'archive':str(archive),'json_pointer':prefix+'/'+str(ident-1),'raw_string_sha256':hashlib.sha256(row['raw'].encode()).hexdigest(),'all_fields_equal':True})
dump('archive-reopened-checks.json',{'master':pin(mp),'corpus':pin(cp),'full_master_entries':len(rawentries),'full_corpus_segments':len(rawcorpus),'checks':checks})
dump('selected-lexical-reading.json',[{'id':i,'wylie':r['wylie'],'hgm_gloss':r['hgm_gloss'],'tier':r['tier'],'hgm_source':r['hgm_source'],'status':r['status']} for i,r in sorted(rawentries.items())])
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
imp=importlib.util.spec_from_file_location('actual_canonical',W/'tools/gen_alignment_page.py');g=importlib.util.module_from_spec(imp);imp.loader.exec_module(g)
hi=importlib.util.spec_from_file_location('actual_head_predicate',W/'tools/test_no_supplied_span_head.py');h=importlib.util.module_from_spec(hi);hi.loader.exec_module(h)
resolved=[];results=[];omission_checks=[];head_hits=[]
for seq in [283,284,285]:
 d=F/str(seq);sp=json.loads((d/'spec.json').read_bytes());seg=sp['segments'][0];spans=seg['spans'];stdin=(d/'spec.json').read_bytes();call=P/'canonical'/str(seq);call.mkdir(parents=True)
 argv=[sys.executable,'-B',str(P/'canonical-ro.py')];started=datetime.datetime.now(datetime.timezone.utc).isoformat();run=subprocess.run(argv,cwd=W,env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0'),input=stdin,capture_output=True)
 for n,b in [('stdin.bin',stdin),('stdout.bin',run.stdout),('stderr.bin',run.stderr),('exit.txt',(str(run.returncode)+'\n').encode())]:(call/n).write_bytes(b)
 dump('canonical/'+str(seq)+'/execution.json',{'argv':argv,'cwd':str(W),'environment':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'},'started_at':started,'ended_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'exit':run.returncode,'canonical':pin(W/'tools/gen_alignment_page.py'),'wrapper':pin(P/'canonical-ro.py')})
 assert run.returncode==0 and not run.stderr
 assert run.stdout==(d/'body.html').read_bytes()
 a=Anchors();a.feed(run.stdout.decode());assert a.values==[source[seq]['wylie'],source[seq]['english']]
 tr=g.resolve(source[seq]['wylie'],spans,'tib',seq);byid={x['id']:x for x in spans};er=g.resolve(source[seq]['english'],g.with_members(spans,[byid[i] for i in seg['eng_order']],source[seq]['english']),'eng',seq)
 tuples=[]
 for x in spans:
  t={'seq':seq,'id':x['id'],'d':x['d'],'tib':x['tib'],'eng':x['eng'],'tib_range':list(tr[x['id']]),'eng_range':list(er[x['id']]) if x['eng'] is not None else None};resolved.append(t);tuples.append(t)
 final=json.loads((d/'final-tuples.json').read_bytes());assert [{k:v for k,v in t.items() if k!='seq'} for t in tuples]==[{k:v for k,v in t.items() if k!='parent'} for t in final]
 decisions=json.loads((d/'final-decisions.json').read_bytes());assert len(decisions)==len(spans)
 for x,y in zip(decisions,spans):assert x['final_span']==y
 omissions=json.loads((d/'omissions.json').read_bytes())
 for side,field,rr in [('tib','wylie',tr),('eng','english',er)]:
  text=source[seq][field];covered=set()
  for lo,up in rr.values():covered.update(range(lo,up))
  expected=[];start=None
  for i in range(len(text)+1):
   free=i<len(text) and i not in covered
   if free and start is None:start=i
   if not free and start is not None:expected.append({'start':start,'end':i,'text':text[start:i]});start=None
  assert expected==omissions['full_unwrapped_complements'][side]
 for item in omissions['material']:
  text=source[seq]['wylie' if item['side']=='tib' else 'english'];assert text[item['start']:item['end']]==item['text'];assert item['null_claim'] is False
 omission_checks.append({'seq':seq,'material_count':len(omissions['material']),'all_full_complements_match_resolved_source_ranges':True})
 for m in h.PAT.finditer(run.stdout.decode()):head_hits.append({'seq':seq,'key':'c5p95/'+m.group(2),'matched_text':m.group(0),'body_range':list(m.span())})
 results.append({'seq':seq,'spans':len(spans),'nulls':sum(x['eng'] is None for x in spans),'d5_nonnull':sum(x['d']==5 and x['eng'] is not None for x in spans),'generator_exit':run.returncode,'stderr_bytes':len(run.stderr),'spec':pin(d/'spec.json'),'body':pin(d/'body.html'),'native_stdout_matches_body':True,'full_source_anchors_match':True})
dump('resolved-spans.json',resolved);dump('canonical-results.json',results);dump('omission-range-checks.json',omission_checks);dump('actual-head-hits.json',head_hits)
assert len(resolved)==76 and sum(r['material_count'] for r in omission_checks)==77
assert pin(dbp)==before
for rel,digest in baseline['sha256'].items():assert pin(W/rel)['sha256']==digest
dump('audit-proof.json',{'status':'ACTUAL_CANONICAL_AND_SOURCE_FIDELITY_PASS_ONLY','source_rows':len(context),'select_replays':sum(q['sql'].lstrip().upper().startswith('SELECT') for q in replays),'control_replays':sum(q['sql'].lstrip().upper().startswith('PRAGMA') for q in replays),'full_master_entries':len(rawentries),'full_corpus_segments':len(rawcorpus),'spans':len(resolved),'omissions':sum(x['material_count'] for x in omission_checks),'head_keys':[x['key'] for x in head_hits],'database':before,'baseline_nine_unchanged':True,'source_preparation_unchanged':True,'semantic_approval':False,'root_mechanical_verification':'Separate root work; not asserted by this proof.'})
print(json.dumps({'results':results,'selects':sum(q['sql'].lstrip().upper().startswith('SELECT') for q in replays),'master':len(rawentries),'corpus':len(rawcorpus),'spans':len(resolved),'omissions':sum(x['material_count'] for x in omission_checks),'heads':[x['key'] for x in head_hits]},indent=2))
