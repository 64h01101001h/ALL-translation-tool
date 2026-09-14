from pathlib import Path
import os,sys,json,hashlib,sqlite3,importlib.util,subprocess,gzip,datetime
D=Path(__file__).resolve().parent;R=D.parent;B=R.parent;A=B.parent;ROOT=A.parent;W=ROOT/'campaign-worktree';I=A/'integration';BASE='fb9182dc6fca2230be3bc5169ba1288a22430280'
assert sys.dont_write_bytecode and __debug__
def pin(b):return dict(bytes=len(b),sha256=hashlib.sha256(b).hexdigest())
def save(n,o):
 p=D/n;p.parent.mkdir(parents=True,exist_ok=True)
 with p.open('x') as f:json.dump(o,f,ensure_ascii=False,indent=2);f.write('\n')
def load(p):return json.loads(p.read_bytes())
def call(argv,name,stdin=b''):
 t=datetime.datetime.now(datetime.timezone.utc).isoformat();name='attempt03-'+name;p=D/'calls'/name;p.mkdir(parents=True)
 r=subprocess.run(argv,input=stdin,capture_output=True,cwd=W,env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0',GIT_OPTIONAL_LOCKS='0'))
 for n,b in [('stdin.bin',stdin),('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',f'{r.returncode}\n'.encode())]:(p/n).write_bytes(b)
 save('calls/'+name+'/execution.json',dict(argv=argv,cwd=str(W),started_at=t,finished_at=datetime.datetime.now(datetime.timezone.utc).isoformat(),exit=r.returncode,stdin=pin(stdin),stdout=pin(r.stdout),stderr=pin(r.stderr)))
 assert r.returncode==0 and not r.stderr,(name,r.returncode,r.stderr);return r.stdout
assert call(['git','rev-parse','HEAD'],'head-before').decode().strip()==BASE
status=call(['git','status','--short'],'status-before');baseline=load(B/'baseline.json');assert baseline['commit']==BASE
for n,h in baseline['sha256'].items():assert pin((W/n).read_bytes())['sha256']==h
paths=[];freeze_records=[]
for angle in ['tibetan','english']:
 for seq in [283,284,285]:
  p=B/angle/str(seq);f=p/('freeze-manifest.json' if angle=='tibetan' else 'freeze.json');o=load(f);members=o if isinstance(o,list) else o['files'];rows=members if isinstance(members,list) else [{'path':k,**v} for k,v in members.items()]
  assert {x['path'] for x in rows}=={str(x.relative_to(p)) for x in p.rglob('*') if x.is_file() and x!=f}
  for x in rows:assert pin((p/x['path']).read_bytes())=={k:x[k] for k in ['bytes','sha256']}
  freeze_records.append(dict(angle=angle,seq=seq,path=str(f),files=len(rows),**pin(f.read_bytes())));paths+=sorted(x for x in p.rglob('*') if x.is_file())
roots=[B/'reconciliation-source-preparation',B/'proposal-verification',B/'original-verification-caller',I/'through285-root-proposal-comparison']
for p in roots:
 if p.exists():paths+=sorted(x for x in p.rglob('*') if x.is_file())
for n in ['reconciliation-brief.md','root-review-targets.md','source.json','context.json','baseline.json','tibetan-brief.md','english-brief.md']:
 paths.append(B/n)
paths += [I/'through285-dispatch-record.json',A/'copy_evidence_once_v2.py',A/'copy_evidence_once_v2-verification.json',A/'proposal-protocol.md']
# Every full original behind original author/source-preparation copy records is reopened.
map_sources=[B/'tibetan/283/evidence/original-to-copy-map.json',B/'english/283/original-to-copy-manifest.json',B/'reconciliation-source-preparation/original-copy-manifest.json']
source_map_checks=[]
for mf in map_sources:
 for r in load(mf):
  p=Path(r['original']);raw=p.read_bytes();assert pin(raw)=={k:r[k] for k in ['bytes','sha256']},str(p)
  cp=mf.parent/r['copy'] if 'copy' in r else None
  if cp is not None:assert cp.read_bytes()==raw,str(cp)
  paths += [p] if cp is None else [p,cp];source_map_checks.append({'descriptor':str(mf),'original':str(p),'copy':str(cp) if cp else None,**pin(raw),'full_bytes_equal':True if cp else None,'original_disposition':r})
# Physical originals named in the complete English descriptors, including C15.
phys=load(B/'english/283/physical-source-bindings.json');physical=[]
for r in phys['rows']:
 p=Path(r['original']);raw=p.read_bytes();assert pin(raw)==r['source_pin'];paths.append(p)
 for field in ['acip','english']:
  lo,hi=r[field+'_byte_range'];q=raw[lo:hi];assert q==r['original_'+field+'_lines'].encode()
  dest=f"physical/{r['parallel_course']}-{r['parallel_seq']}-{field}.txt";out=D/dest;out.parent.mkdir(exist_ok=True)
  if out.exists():assert out.read_bytes()==q
  else:out.write_bytes(q)
  physical.append(dict(seq=r['target_seq'],course=r['parallel_course'],parallel_seq=r['parallel_seq'],field=field,original=str(p),verified_full_source_pin=pin(raw),start=lo,end=hi,excerpt=dest,**pin(q),descriptor=str(B/'english/283/physical-source-bindings.json'),source_field=r['whitespace_joined_'+field]))
save('physical-extents.json',physical)
# Approved content-addressed full copying; logical aliases retained independently.
helper=A/'copy_evidence_once_v2.py';assert pin(helper.read_bytes())['sha256']=='b8eabf4b53c29174d7f67c2f723007d429edc1e153ee3e5b32ff0a6e6fa147d1'
s=importlib.util.spec_from_file_location('copyv2',helper);m=importlib.util.module_from_spec(s);s.loader.exec_module(m)
paths=list(dict.fromkeys(paths));result=m.preserve(D,D/'original-copy-manifest.json',paths)
save('original-aliases.json',[dict(logical=str(p),resolved=str(p.resolve()),symlink=os.readlink(p) if p.is_symlink() else None,**pin(p.read_bytes())) for p in paths])
save('original-source-pair-checks.json',source_map_checks);save('original-freezes.json',freeze_records)
# Preserve actual governing Git objects, not merely live aliases.
gov=load(B/'reconciliation-source-preparation/governing.json');runs=[]
for i,r in enumerate(gov['actual_runs']):
 raw=call(r['argv'],f'governing-{i:02d}');assert pin(raw)=={k:r[k] for k in ['bytes','sha256']};p='governing/'+Path(r['copy']).name;(D/p).parent.mkdir(exist_ok=True);(D/p).write_bytes(raw);runs.append({**r,'copy':p,'execution':f'calls/governing-{i:02d}/execution.json'})
save('governing.json',dict(commit=BASE,actual_runs=runs))
dbp=W/'build/hgm_spine_v27_2.db';dbpin=pin(dbp.read_bytes());db=sqlite3.connect(dbp.resolve().as_uri()+'?mode=ro',uri=True);db.row_factory=sqlite3.Row;db.execute('PRAGMA query_only=ON');assert db.execute('PRAGMA query_only').fetchone()[0]==1;db.execute('BEGIN')
# Genuine descriptors are replayed without altering their count/parameter schema.
qinputs=[]
for p in sorted((B/'english/283/queries').glob('*.json')):
 o=load(p)
 if 'sql' in o and 'rows' in o:qinputs.append((p,[],o))
for p in [B/'tibetan/283/evidence/sqlite-query-results.json',B/'tibetan/283/evidence/supplement-lexical-queries.json']:
 for i,q in enumerate(load(p)['queries']):qinputs.append((p,['queries',i],q))
for base in [B/'reconciliation-source-preparation/queries',I/'through285-root-proposal-comparison/queries']:
 for p in sorted(base.glob('*.json')):
  o=load(p)
  if 'sql' in o and 'rows' in o:qinputs.append((p,[],o))
corpus=load(W/'data/full_parallel_corpus_v32.json.gz') if False else json.loads(gzip.decompress((W/'data/full_parallel_corpus_v32.json.gz').read_bytes()))
master=json.loads(gzip.decompress((W/'data/hgm_dictionary_v27_2.json.gz').read_bytes()))
if isinstance(master,dict):master=master['entries']
bycourse={}
for row in corpus:bycourse.setdefault(row['course'],[]).append(row)
queries=[];entries={};corpusrows={}
for p,sel,q in qinputs:
 params=q.get('parameters',q.get('params'));assert params is not None,(p,q.keys())
 rows=[dict(r) for r in db.execute(q['sql'],params)];assert rows==q['rows'],str(p)
 countkeys=[k for k in ['count','row_count'] if k in q]
 for k in countkeys:assert q[k]==len(rows)
 index=len(queries);name=f'queries/{index:03d}.json';save(name,{'sql':q['sql'],'parameters':params,'count':len(rows),'rows':rows})
 queries.append({'path':name,'original_descriptor':str(p),'selector':sel,'parameter_key':'parameters' if 'parameters' in q else 'params','count_keys_present':countkeys,'original_descriptor_pin':pin(p.read_bytes()),'count':len(rows)})
 for row in rows:
  if 'raw' not in row:continue
  obj=json.loads(row['raw'])
  if 'course' in row:
   assert obj==bycourse[row['course']][row['seq']-1];corpusrows[row['id']]=row
  elif 'hgm_gloss' in row:
   assert obj==master[row['id']-1];entries[row['id']]=row
# Exact complete current context reopens; independent extra lookup needed for final boundaries.
extra=[('SELECT * FROM corpus_segments WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq',['C05',277,291])]
extra += [('SELECT e.* FROM entries e WHERE e.wylie=? OR EXISTS(SELECT 1 FROM entry_variants v WHERE v.entry_id=e.id AND v.wylie=?) ORDER BY e.id',[s,s]) for s in ['khas blangs','khas blangs pa','steng du','gang po','mtshungs','sogs','las can','tshad','tshad grub pa','bltas pa']]
for sql,params in extra:
 rows=[dict(r) for r in db.execute(sql,params)];name=f'queries/{len(queries):03d}.json';save(name,dict(sql=sql,parameters=params,count=len(rows),rows=rows));queries.append(dict(path=name,original_descriptor=str(D/name),selector=[],parameter_key='parameters',count_keys_present=['count'],count=len(rows)))
 for row in rows:
  if 'hgm_gloss' in row:assert json.loads(row['raw'])==master[row['id']-1];entries[row['id']]=row
  elif 'course' in row:assert json.loads(row['raw'])==bycourse[row['course']][row['seq']-1];corpusrows[row['id']]=row
context=[dict(r) for r in db.execute('SELECT * FROM corpus_segments WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq',['C05',277,291])];source=[r for r in context if 283<=r['seq']<=285]
assert [{k:r[k] for k in ['id','course','seq','acip','wylie','english']} for r in source]==load(B/'source.json')
save('source-rows-full.json',source);save('context-rows-full.json',context);save('query-descriptors.json',queries);save('master-full-objects.json',[dict(id=k,full_row=v,master_object=json.loads(v['raw'])) for k,v in sorted(entries.items())]);save('corpus-full-objects.json',[dict(id=k,full_row=v,archive_object=json.loads(v['raw'])) for k,v in sorted(corpusrows.items())]);db.rollback();db.close();assert pin(dbp.read_bytes())==dbpin
# Six canonical original replays using the original English wrapper at its original location.
wrapper=B/'english/283/run_canonical_readonly.py';canonical=[]
for angle in ['tibetan','english']:
 for seq in [283,284,285]:
  p=B/angle/str(seq);raw=call([sys.executable,'-B',str(wrapper)],f'original-{angle}-{seq}',(p/'spec.json').read_bytes());assert raw==(p/'body.html').read_bytes();canonical.append(dict(angle=angle,seq=seq,exit=0,empty_stderr=True,exact_saved_body=True,body=pin(raw),spec=pin((p/'spec.json').read_bytes()),execution=f'calls/original-{angle}-{seq}/execution.json'))
save('original-canonical-replays.json',canonical)
assert call(['git','rev-parse','HEAD'],'head-after').decode().strip()==BASE;assert call(['git','status','--short'],'status-after')==status
for n,h in baseline['sha256'].items():assert pin((W/n).read_bytes())['sha256']==h
save('evidence-proof.json',dict(status='PASS_REOPEN_COPY_QUERY_AND_ORIGINAL_CANONICAL',baseline=BASE,copies=result,queries=len(queries),distinct_dictionary_entries=len(entries),distinct_corpus_rows=len(corpusrows),physical_extents=len(physical),originals=145,original_frozen_members=sum(r['files'] for r in freeze_records),canonical_replays=canonical,database=dict(logical=str(dbp),resolved=str(dbp.resolve()),uri=dbp.resolve().as_uri()+'?mode=ro',query_only=True,**dbpin),source_unchanged=True))
print(json.dumps({'status':'PASS','copies':result,'queries':len(queries),'entries':len(entries),'corpus':len(corpusrows),'physical':len(physical),'canonical':len(canonical)}))
