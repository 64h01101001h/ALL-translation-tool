import ast, collections, copy, hashlib, io, json, pathlib, re, subprocess, sys, types, unittest
ROOT=pathlib.Path(__file__).resolve().parents[4]/'campaign-worktree'
ART=pathlib.Path(__file__).resolve().parent
BASE='4aa4aa6cce64fe2b5129ad666e8753a2947ea64e'
sys.path.insert(0,str(ROOT/'tools'))
import build_dictionary_view as new
import test_view_matches_layer as gate
import test_dictionary_view_trimming as tests

def old_text(path):return subprocess.check_output(['git','show',BASE+':'+path],cwd=ROOT,text=True)
def module(name,source,path):
 m=types.ModuleType(name);m.__file__=str(ROOT/path);exec(compile(source,str(ROOT/path),'exec'),m.__dict__);return m
oldsrc=old_text('tools/build_dictionary_view.py')
old=module('old_builder',oldsrc,'tools/build_dictionary_view.py')
oldgate=module('old_gate',old_text('tools/test_view_matches_layer.py'),'tools/test_view_matches_layer.py')
# Extract the exact OLD inline pronunciation block by AST; no reimplementation oracle.
tree=ast.parse(oldsrc);main=next(n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='main')
branch=next(n for n in ast.walk(main) if isinstance(n,ast.If) and 'is_phonetic(t, e)' in ast.unparse(n.test))
block=[]
for node in branch.body:
 block.append(node)
 if isinstance(node,ast.Assign) and ast.unparse(node.targets[0])=='e':break
fn=ast.parse('def original_phon(t,e):\n return e').body[0];fn.body=block+[ast.Return(ast.Name('e',ast.Load()))]
exec(compile(ast.fix_missing_locations(ast.Module(body=[fn],type_ignores=[])),'old_phon_ast','exec'),old.__dict__)
full=json.loads((ROOT/'data/alignment/alignment_full_v1.json').read_text()); ev=json.loads((ROOT/'data/alignment/alignment_evidence_v1.json').read_text())
summary={'base':BASE,'hashes':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in json.loads((ART.parent/'reviewed-code-hashes.json').read_text())}}
assert summary['hashes']==json.loads((ART.parent/'reviewed-code-hashes.json').read_text())
summary['bank_hashes']={p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in ['data/alignment/alignment_full_v1.json','data/alignment/alignment_evidence_v1.json']}
assert summary['bank_hashes']==json.loads((ART.parent/'proof-summary.json').read_text())['bank_sha256']
expected_refs={'C03:131','C03:199','C04:37','C04:49','C04:117','C04:133','C04:168','C05:96','C05:118','C05:145','C05:206'}
before=collections.defaultdict(set);after=collections.defaultdict(set);changes=[];phon=0;valid=0
for l in full['links']:
 t=(l.get('tib') or '').strip();e=(l.get('eng') or '').strip();d=l.get('d')
 if not d or not t or not e or l.get('seg') is None or not l.get('course'):continue
 valid+=1;ref=f"{l['course']}:{l['seg']}"
 isphon=old.is_phonetic(t,e) and not e[:1].isupper()
 assert isphon==(new.is_phonetic(t,e) and not e[:1].isupper())
 if isphon:
  phon+=1;d='phon';a=old.original_phon(t,e);b=new.trim_phonetic(t,e);assert a==b
 else:
  d=str(d);a=old.trim_glued(e)[0];b=new.trim_glued(e)[0]
 if a!=b:
  assert ref in expected_refs and d=='1'
  # Independent cut oracle: literal full-heading occurrence and preceding page token.
  heading=e.index(' The Asian Classics Institute Course ')
  cut=e.rfind(' ',0,heading)
  assert e[cut+1:heading].isdigit() and e[cut-1] in '.!?'
  expected=e[:cut];assert b==expected
  changes.append({'ref':ref,'id':l['id'],'depth':d,'old':a,'new':b,'source':e,'cut':cut})
 before[(d,t,a)].add(ref);after[(d,t,b)].add(ref)
assert len(changes)==11 and {x['ref'] for x in changes}==expected_refs
assert phon==947
removed=set(before)-set(after);added=set(after)-set(before)
assert len(removed)==len(added)==11
assert all(before[k]==after[k] for k in set(before)&set(after))

def payload(rows):
 p={'depths':{},'phonetics':[]}
 for (d,t,e),refs in rows.items():
  band=p['phonetics'] if d=='phon' else p['depths'].setdefault(d,[])
  band.append([t,'',[[e,sorted(refs)]]])
 return p
P=payload(after);O=payload(before)
c,b,both=gate.classify(P,full,ev);assert not b and not both and c['exact']==21406 and c['trimmed (trim_glued)']==41
oldc,_,_=oldgate.classify(O,full,ev);assert oldc['trimmed (trim_glued)']==41 and oldc['trimmed (trim_glued)']>oldgate.CEILING['trimmed (trim_glued)']
wrongc,wrong,both=gate.classify(O,full,ev);assert wrongc['INVENTED-BY-THE-VIEW']==11 and not both
# Independently recover all 40 prior shortened rows from original committed HTML/bank.
baseP=gate.payload(old_text('docs/geshe_michael_roach_dictionary.html'))
basefull=json.loads(old_text('data/alignment/alignment_full_v1.json'));baseev=json.loads(old_text('data/alignment/alignment_evidence_v1.json'))
banked=collections.defaultdict(set)
for l in basefull['links']:
 if l.get('tib') and l.get('eng'):banked[l['tib']].add(l['eng'])
for t,ss in baseev['pairs'].items():
 for s in ss:
  if s.get('eng'):banked[t].add(s['eng'])
prior=[]
for d,band in list(baseP['depths'].items())+[('phon',baseP['phonetics'])]:
 for t,e,refs in gate.rows(band):
  if e not in banked[t]:
   assert (d,t,e) in before
   prior.append((d,t,e))
assert len(prior)==40 and sum(d=='phon' for d,t,e in prior)==18
assert all(k in after and before[k]==after[k] for k in prior if k[0]=='phon')
assert sum(k not in after for k in prior)==10
# Fresh literal suite and correlated implementation mutations; no files changed.
def suite():
 out=io.StringIO();r=unittest.TextTestRunner(stream=out).run(unittest.defaultTestLoader.loadTestsFromTestCase(tests.HeadingTrimmingTests));return r,out.getvalue()
r,out=suite();assert r.wasSuccessful() and r.testsRun==10
(ART/'green.txt').write_text(out)
mutations=[]
orig=new.NUMBERED_COURSE_HEAD;new.NUMBERED_COURSE_HEAD=re.compile(r'(?!)')
r,out=suite();new.NUMBERED_COURSE_HEAD=orig;assert not r.wasSuccessful();mutations.append({'name':'shared numbered recognizer removed','failures':len(r.failures),'errors':len(r.errors),'output':out})
orig=new.trim_phonetic;origgate=gate.trim_phonetic
new.trim_phonetic=gate.trim_phonetic=lambda t,e:e.split()[0]
r,out=suite();new.trim_phonetic=orig;gate.trim_phonetic=origgate;assert not r.wasSuccessful();mutations.append({'name':'shared phonetic function arbitrary first word','failures':len(r.failures),'errors':len(r.errors),'output':out})
# Direct actual-payload corruptions including every citation, depth and both band directions.
target=next(x for x in changes if x['ref']=='C05:206')
key=next(k for k in after if k[0]=='1' and k[2]==target['new'] and 'C05:206' in after[k])
for name,output,refs,band in [('empty','',None,'1'),('arbitrary','Because',None,'1'),('shorter',target['new'][:-1],None,'1'),('extended',target['new']+' 73',None,'1'),('invented','An invented output',None,'1'),('wrong reference',target['new'],{'C05:999'},'1'),('extra wrong reference',target['new'],{'C05:206','C05:999'},'1'),('no refs',target['new'],set(),'1'),('wrong depth',target['new'],None,'2'),('translation in phonetics',target['new'],None,'phon')]:
 rows=dict(after);references=rows.pop(key);rows[(band,key[1],output)]=references if refs is None else refs
 counts,bad,_=gate.classify(payload(rows),full,ev);assert counts['INVENTED-BY-THE-VIEW']==1,(name,counts)
 mutations.append({'name':name,'rejected':True})
# Existing exact-pair acceptance and pairwise overlap contract are deliberately retained.
x={'depths':{'5':[['log lta','',[['wrong views',[]]]]]},'phonetics':[['log lta','',[['wrong views',[]]]]]}
counts,_,_=gate.classify(x,{'links':[{'tib':'log lta','eng':'wrong views'}]},{'pairs':{}});assert counts['exact']==2 and counts['phonetics-leaked-into-depth-5']==1
summary.update({'valid_links':valid,'changed_links':len(changes),'changed_display_rows':len(added),'unchanged_display_keys':len(set(before)&set(after)),'all_unchanged_keys_keep_references':True,'phonetic_links_unchanged':phon,'prior_shortened_rows':len(prior),'prior_phonetic_rows_unchanged':18,'prior_heading_rows_changed':10,'prior_other_heading_rows_unchanged':12,'new_heading_row':'C05:206','classification':c,'old_rule_current_payload_counts':oldc,'fixed_gate_rejects_old_values':wrongc,'literal_tests':r.testsRun,'mutation_checks':len(mutations),'exact_pair_and_overlap_contract':True})
(ART/'independent-migrations.json').write_text(json.dumps(changes,ensure_ascii=False,indent=2)+'\n')
(ART/'mutations.json').write_text(json.dumps(mutations,indent=2)+'\n')
(ART/'verification.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
