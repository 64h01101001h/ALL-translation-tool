import datetime,difflib,hashlib,json,os,re,sqlite3,subprocess,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent;B=P.parent;W=B.parent.parent/'campaign-worktree';S=B/'semantic-source-preparation'
def dump(name,v):
 p=P/name;p.parent.mkdir(parents=True,exist_ok=True);assert not p.exists(),p;p.write_text(json.dumps(v,ensure_ascii=False,indent=2)+'\n')
def pin(p):
 b=p.read_bytes();return {'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
c=sqlite3.connect((W/'build/hgm_spine_v27_2.db').resolve().as_uri()+'?mode=ro',uri=True);c.row_factory=sqlite3.Row;c.execute('PRAGMA query_only=ON')
sql='SELECT * FROM corpus_segments WHERE (course=? AND seq BETWEEN ? AND ?) OR (course=? AND seq BETWEEN ? AND ?) ORDER BY course,seq';params=['C05',277,291,'C15',428,430];rows=[dict(r) for r in c.execute(sql,params)];lookup={(r['course'],r['seq']):r for r in rows}
dump('errata-source-query.json',{'sql':sql,'parameters':params,'count':len(rows),'rows':rows,'uri':(W/'build/hgm_spine_v27_2.db').resolve().as_uri()+'?mode=ro','initialization_sql':'PRAGMA query_only=ON'})
physical=[];manifest=json.loads((P/'original-copy-manifest.json').read_bytes())
for course in ['C05','C15']:
 src=Path('/Users/adamderickandrade/Desktop/HGM DICTIONARY - TRANSLATION APP/To DO/Formal Courses/ACI Readings In Roman ASCII')/(course+'ReadingASCII.txt');raw=src.read_bytes();v=pin(src);obj=P/'evidence-objects'/v['sha256']
 if not obj.exists():obj.write_bytes(raw)
 assert obj.read_bytes()==raw
 manifest.append({'original_path':str(src),'copy_path':str(obj),'content_addressed_copy':str(obj),**v})
 text=raw.decode('latin-1')
 for seq in ([283,284,285] if course=='C05' else [428,429,430]):
  row=lookup[(course,seq)]
  for field in ['acip','english']:
   expr=r'\s+'.join(re.escape(t) for t in row[field].split());m=list(re.finditer(expr,text));assert len(m)==1
   physical.append({'course':course,'seq':seq,'field':field,'original':str(src),'original_pin':v,'regex':expr,'count':1,'byte_range':list(m[0].span()),'full_physical_text':m[0].group(),'equals_source_after_whitespace_normalization':' '.join(m[0].group().split())==' '.join(row[field].split())})
dump('physical-reopen.json',physical)
dump('additional-original-copy-manifest.json',manifest[-2:])
register_path=W/'docs/errata_register.json';register=json.loads(register_path.read_bytes());assert pin(register_path)==pin(P/'governing/errata_register.json')
ids=['E-044','E-071','E-076','E-107','E-111','E-120','E-121'];classes=[e for e in register if e['item_id'] in ids];targets=[e for e in register if any(('C05:'+str(s)) in str(e.get('segment','')) for s in [283,284,285])]
dump('errata-register-check.json',{'register':{'path':str(register_path),**pin(register_path)},'entry_count':len(register),'class_ids_examined':ids,'full_class_entries':classes,'target_segment_matches':targets})
assert not targets
comparisons=[]
for seq in [283,284,285]:
 a=lookup[('C05',seq)];b=lookup[('C15',seq+145)]
 assert a['acip']==b['acip'] and a['wylie']==b['wylie']
 ops=[{'operation':op,'c05_range':[i,j],'c15_range':[k,l],'c05_text':a['english'][i:j],'c15_text':b['english'][k:l]} for op,i,j,k,l in difflib.SequenceMatcher(a=a['english'],b=b['english'],autojunk=False).get_opcodes() if op!='equal']
 if seq==283:
  assert len(ops)==2
  assert all(x['c05_text']==', ' and x['c15_text']=='--' for x in ops)
 else:assert not ops and a['english']==b['english']
 comparisons.append({'seq':seq,'witness':'C15:'+str(seq+145),'acip_equal':True,'wylie_equal':True,'english_equal':not ops,'non_equal_operations':ops,'independent_publication_proven':False})
dump('parallel-comparisons.json',comparisons)
own_reasons={
 'C05-283-SCREEN-01':'The source explicitly counts five, and the immediately preceding four limbs plus head explains those five. Replacing five with four would create a numeral error.',
 'C05-283-SCREEN-02':'Commas can delimit this embedded when-clause just as double hyphens can. The current complete sentence is grammatical; the digital punctuation variant supplies no error proof.',
 'C05-284-SCREEN-01':'The three appearances are three kinds; pus and blood together name one kind in this argument. The other two are the remaining kinds. The English explanatory numeral is defensible while remaining outside sogs.',
 'C05-285-SCREEN-01':'ma yin explicitly negates the object classification and par thal raises its consequence as an objection. The negative question preserves this scope; an affirmative assertion would change it.'}
refutations=[]
for seq in [283,284,285]:
 for claim in json.loads((P/'reviewed-final'/str(seq)/'errata-analysis.json').read_bytes()):
  a=lookup[('C05',seq)];w=lookup[('C15',seq+145)];found=claim['found'];counts={'exact_found_occurrences':a['english'].count(found),'exact_C15_counterpart_rows':sum(r['course']=='C15' and r['seq']==seq+145 for r in rows),'target_source_rows':sum(r['course']=='C05' and 283<=r['seq']<=285 for r in rows),'english_punctuation_replacements_to_C15':len(comparisons[seq-283]['non_equal_operations'])}
  assert counts['exact_found_occurrences']==1
  for k,v in claim['five_grounds']['e']['counts_rechecked'].items():
   aliases={'found_occurrences':'exact_found_occurrences','full_English_replacements':'english_punctuation_replacements_to_C15'};assert counts[aliases.get(k,k)]==v
  refutations.append({'seq':seq,'candidate_id':claim['candidate_id'],'found':found,'expected':claim['expected'],'refuted':True,'retained_erratum':False,'reason':own_reasons[claim['candidate_id']],'five_grounds':{'a':{'checked':True,'found_in_english':True,'all_three_full_source_fields':a},'b':{'checked':True,'full_digital_witness':w,'comparison':comparisons[seq-283],'result':'No independently published witness established. Digital equality or punctuation variation is not used as the decisive refutation.'},'c':{'checked':True,'full_register_check':str(P/'errata-register-check.json'),'result':'No target duplicate; no converter, running-head, truncation, footnote or row-boundary class establishes this proposed error.'},'d':{'checked':True,'result':own_reasons[claim['candidate_id']]},'e':{'checked':True,'counts_rechecked':counts,'all_original_numerical_claims_equal':True}},'suggested_severity':None,'suggested_confidence':None})
dump('errata-refutations.json',refutations);dump('bank-ready-errata.json',[])
original_runs=[]
for angle in ['tibetan','english']:
 for seq in [283,284,285]:
  d=P/'original-angles'/angle/str(seq);call=P/'original-canonical'/angle/str(seq);call.mkdir(parents=True);stdin=(d/'spec.json').read_bytes();argv=[sys.executable,'-B',str(P/'canonical-ro.py')];start=datetime.datetime.now(datetime.timezone.utc).isoformat();r=subprocess.run(argv,cwd=W,env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0'),input=stdin,capture_output=True)
  for name,raw in [('stdin.bin',stdin),('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',(str(r.returncode)+'\n').encode())]:(call/name).write_bytes(raw)
  dump(str(call.relative_to(P))+'/execution.json',{'argv':argv,'cwd':str(W),'environment':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'},'started_at':start,'finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'exit':r.returncode})
  assert r.returncode==0 and not r.stderr and r.stdout==(d/'body.html').read_bytes()
  original_runs.append({'angle':angle,'seq':seq,'exit':0,'stderr_bytes':0,'body_match':True,'span_count':len(json.loads(stdin)['segments'][0]['spans'])})
dump('original-canonical-summary.json',original_runs)
assert sum(x['span_count'] for x in original_runs)==145
print(json.dumps({'physical_fields_reopened':len(physical),'refuted_screenings':len(refutations),'registered_target_duplicates':len(targets),'original_canonical_runs':len(original_runs),'original_spans':145},indent=2))
