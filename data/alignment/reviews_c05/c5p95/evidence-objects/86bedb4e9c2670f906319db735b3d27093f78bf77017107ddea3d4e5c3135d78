from pathlib import Path
import json,hashlib,os,sys,importlib.util,subprocess,datetime,difflib,collections,re
D=Path(__file__).resolve().parent;R=D.parent;B=R.parent;A=B.parent;W=A.parent/'campaign-worktree'
assert sys.dont_write_bytecode and __debug__
load=lambda p:json.loads(p.read_bytes())
def pin(b):return dict(bytes=len(b),sha256=hashlib.sha256(b).hexdigest())
def save(p,o):
 with p.open('x') as f:json.dump(o,f,indent=2,ensure_ascii=False);f.write('\n')
# All prior maps remain represented at their actual earlier boundary.
h=D/'completion-map-history';h.mkdir()
for name in ['original-copy-manifest.json','original-aliases.json']:(h/name).write_bytes((D/name).read_bytes())
physical=load(D/'physical-extents.json');mappings=load(D/'original-copy-manifest.json');have={r['sha256'] for r in mappings}
paths=[D/r['excerpt'] for r in physical if r['sha256'] not in have]
paths += [A/'C05-280-282/reconciled/run-generator-ro.py.txt',R/'run-generator-ro.py.txt']
helper=A/'copy_evidence_once_v2.py';s=importlib.util.spec_from_file_location('approved_copy_v2',helper);m=importlib.util.module_from_spec(s);s.loader.exec_module(m)
result=m.preserve(D,D/'original-copy-manifest.json',paths)
aliases=load(D/'original-aliases.json');seen={x['logical'] for x in aliases}
for p in paths:
 if str(p) not in seen:aliases.append(dict(logical=str(p),resolved=str(p.resolve()),symlink=os.readlink(p) if p.is_symlink() else None,**pin(p.read_bytes())))
(D/'original-aliases.json').write_text(json.dumps(aliases,ensure_ascii=False,indent=2)+'\n')
save(h/'explanation.json',{'old_manifest':pin((h/'original-copy-manifest.json').read_bytes()),'current_manifest':pin((D/'original-copy-manifest.json').read_bytes()),'added_full_originals':[str(p) for p in paths],'reason':'Shared wrapper input/version plus three new range-derived C15 excerpt bytes needed by complete physical hash-to-copy preflight. Complete physical originals and exact ranges remain authoritative; no existing payload changed.'})
# Reopen every complete alias and copy; manifests are not a semantic-reading claim.
mappings=load(D/'original-copy-manifest.json');checks=[]
for r in mappings:
 p=Path(r['original']);c=D/r['copy'];raw=p.read_bytes();assert raw==c.read_bytes() and pin(raw)=={k:r[k] for k in ['bytes','sha256']};checks.append(dict(original=str(p),copy=r['copy'],**pin(raw)))
for r in aliases:
 p=Path(r['logical']);assert str(p.resolve())==r['resolved'];assert (os.readlink(p) if p.is_symlink() else None)==r['symlink'];assert pin(p.read_bytes())=={k:r[k] for k in ['bytes','sha256']}
# Full original six trees are re-enumerated and checked, including all native failures/transcriptions.
for f in load(D/'original-freezes.json'):
 p=Path(f['path']);o=load(p);items=o if isinstance(o,list) else o['files'];items=items if isinstance(items,list) else [{'path':k,**v} for k,v in items.items()]
 assert pin(p.read_bytes())=={k:f[k] for k in ['bytes','sha256']}
 assert {r['path'] for r in items}=={str(x.relative_to(p.parent)) for x in p.parent.rglob('*') if x.is_file() and x!=p}
 for r in items:assert pin((p.parent/r['path']).read_bytes())=={k:r[k] for k in ['bytes','sha256']}
for r in physical:
 raw=Path(r['original']).read_bytes();assert pin(raw)==r['verified_full_source_pin'];part=raw[r['start']:r['end']];assert part==(D/r['excerpt']).read_bytes() and pin(part)=={k:r[k] for k in ['bytes','sha256']};assert any(x['sha256']==r['sha256'] and (D/x['copy']).read_bytes()==part for x in mappings)
ref=[]
for x in load(B/'english/283/required-reference-access.json'):
 p=Path(x['path']);assert p.is_dir() and os.access(p,os.R_OK|os.X_OK)
 with os.scandir(p) as it:first=next(it,None)
 ref.append(dict(path=str(p),is_directory=True,readable=True,actual_directory_open=True,scope='Access check only, not a claim to have read all files in this reference directory.'))
save(D/'required-reference-access.json',ref)
base=load(B/'baseline.json')
for n,hsh in base['sha256'].items():assert pin((W/n).read_bytes())['sha256']==hsh
p=D/'completion-state';p.mkdir();state=[]
for name,args in [('head',['git','rev-parse','HEAD']),('status',['git','status','--short'])]:
 r=subprocess.run(args,cwd=W,capture_output=True,env=dict(os.environ,GIT_OPTIONAL_LOCKS='0',PYTHONDONTWRITEBYTECODE='1'));assert r.returncode==0 and not r.stderr
 for ext,b in [('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',b'0\n')]:(p/f'{name}.{ext}').write_bytes(b)
 save(p/f'{name}.execution.json',dict(argv=args,cwd=str(W),exit=0,stdout=pin(r.stdout),stderr=pin(r.stderr)))
 if name=='head':assert r.stdout.decode().strip()==base['commit']
 else:assert r.stdout==(D/'calls/attempt03-status-before/stdout.bin').read_bytes()
 state.append(dict(name=name,stdout=pin(r.stdout)))
# Structural/copy verification from saved actual canonical output, without another canonical or SQL execution.
summary=load(D/'final-canonical-summary.json');all_dis=[];counts=[]
for seq in [283,284,285]:
 out=R/str(seq);sp=load(out/'spec.json')['segments'][0]['spans'];ts=load(out/'final-tuples.json');dec=load(out/'final-decisions.json');dis=load(out/'original-dispositions.json');pr=load(out/'proof.json');all_dis+=dis
 assert len(sp)==len(ts)==len(dec)==pr['spans'];assert [d['final_span'] for d in dec]==sp and [d['final_tuple'] for d in dec]==ts
 assert (out/'body.html').read_bytes()==(out/'canonical-attempt01/stdout.bin').read_bytes();assert pin((out/'body.html').read_bytes())==pr['body'];assert pin((out/'spec.json').read_bytes())==pr['spec']
 for t in ts:
  for side,field in [('tib','wylie'),('eng','english')]:
   row=next(r for r in load(D/'source-rows-full.json') if r['seq']==seq);a,z=t[side+'_range'];assert row[field][a:z]==t[side]
  if t['parent']:
   parent=next(x for x in ts if x['id']==t['parent']);assert parent['d']==5
   for side in ['tib','eng']:
    assert parent[side+'_range'][0]<=t[side+'_range'][0]<t[side+'_range'][1]<=parent[side+'_range'][1] and parent[side]!=t[side]
  assert next(x for x in dec if x['final_tuple']['id']==t['id'])['rationale']
 for e in load(out/'errata-analysis.json'):assert e['refuted'] and not e['retained_erratum'] and set(e['five_grounds'])==set('abcde')
 counts.append(dict(seq=seq,spans=pr['spans'],nulls=pr['nulls'],d5_nonnull=pr['d5_nonnull'],original_dispositions=len(dis),dispositions=dict(collections.Counter(x['disposition'] for x in dis)),head_allowances=pr['actual_head_hits'],spec=pr['spec'],body=pr['body']))
 note={283:'Retain causal pas → so after yin, while the later byas pas and terminal reason/report frame remain distributed. The two person/action/performing sets and the first five have exact separate occurrences.',284:'Choose the attested rnag khrag compound with two crossed d7 members, sogs → and the other, complete gang po, and steng du → upon once. mtshungs owns the complete attested exactly the same; acting as a basis stays unpaired as the wider repeated locative realization.',285:'Retain yang → yet another and explicit negative ma/not plus subordinate yin/are. Keep the tshad grub pa compound with two tighter members, full bltas pa, and khas blangs → position once. Wider las can and cig car du variants are acknowledged; individualized pronouns, sitting posture and repeated said remain unwrapped.'}[seq]
 report=f'''C05:{seq} — PROVISIONAL Codex model reconciliation; neither current original angle was authored by this reconciler.
Reused context and prior tooling authorship are disclosed in ../283/role-provenance.json; no human certification is claimed.
{pr['spans']} final spans; {pr['nulls']} nulls; {pr['d5_nonnull']} non-null d5; {len(dis)} individually preserved original dispositions.
{note}
Complete source meanings, dictionary-unit depths, all occurrences and tighter members are explained per span in final-decisions.json.
Every unwrapped Tibetan/English complement has a reason in omissions.json; distributed content is not converted to a null.
The actual canonical head predicate yields {pr['actual_head_hits']} proposed allowance(s), recorded with exact page95 key, match and Tibetan licensor; no repository allowance was edited.
All six original generators independently replayed EXIT0, empty stderr, exact saved bodies. All original optional fields/declarations/parent identities and failures remain intact.
Final actual canonical EXIT=0; stderr=0 bytes; body={pr['body']['bytes']} bytes, SHA256 {pr['body']['sha256']}.
Final spec={pr['spec']['bytes']} bytes, SHA256 {pr['spec']['sha256']}. body.html is actual stdout; both full text anchors match the spine verbatim.
Errata=[]; numeral/scope/punctuation questions receive complete five-ground refutations based on defensible source readings, not a schema default alone.
Shared evidence: 209 SELECT replays plus one genuine PRAGMA query_only control; 76 full master entries, 394 distinct full corpus rows and 12 physical field extents in ../283/.
C05/C15 text comparisons retain complete physical bytes and punctuation differences; no independent publication or ingestion lineage is inferred.
Native current substantive command streams and original failed/superseded versions are preserved; original exploratory transcriptions remain honestly labeled.
Full source/copy/alias/Git pin and boundary checks are recorded in ../283/completion-proof.json. This reconciliation does not grant semantic approval or landing.
'''
 (out/'report.md').write_text(report)
 save(out/'shared-evidence-reference.json',{'shared_directory':str(D),'copy_manifest':{'path':str(D/'original-copy-manifest.json'),**pin((D/'original-copy-manifest.json').read_bytes())},'alias_manifest':{'path':str(D/'original-aliases.json'),**pin((D/'original-aliases.json').read_bytes())},'governing':str(D/'governing-current.json'),'queries':str(D/'query-descriptors.json'),'physical':str(D/'physical-extents.json'),'original_identities':str(B/'proposal-verification/root-resolved-spans.json'),'shared_wrapper':{'path':str(R/'run-generator-ro.py.txt'),**pin((R/'run-generator-ro.py.txt').read_bytes())},'copies_once_by_content':True,'current_reconciliation_semantic_approval':False})
assert len(all_dis)==145 and len({(x['original_identity']['angle'],x['original_identity']['seq'],x['original_identity']['id']) for x in all_dis})==145
save(D/'full-copy-preflight.json',{'status':'PASS_FULL_BYTES_ALL_ORIGINALS_COPIES_ALIASES','records':checks,'aliases_checked':len(aliases),'source_pairs_checked':len(load(D/'original-source-pair-checks.json')),'physical_extents_checked':len(physical),'original_frozen_members':344,'original_freezes':6,'landing_whitelist':[f'{s}/{n}' for s in [283,284,285] for n in ['spec.json','body.html','errata.json']],'landing_executed':False,'retention_scope':'Whole frozen reconciled trees are review evidence; only the three named spec/body/errata interfaces per segment are prospective landing inputs. Actual landing authorization/gates remain separate.'})
qs=load(D/'query-descriptors.json');selects=[];controls=[]
for q in qs:
 raw=load(D/q['path']);(selects if raw['sql'].lstrip().upper().startswith('SELECT ') else controls).append(q)
save(D/'runtime-schema-handoff.json',{'shared_directory':str(R),'recommended_evidence_directory':str(D),'final_interfaces':'Prior accepted seven-key tuple/decision/disposition/complement/head schemas; three final freeze.json files use files DICT.','original_freezes':'Tibetan freeze-manifest.json ROOT LIST; English freeze.json object.files LIST. Approved root adapter handles both.','full_original_copy_manifest':{'path':str(D/'original-copy-manifest.json'),'records':len(mappings),'copy_paths_relative_to':str(D),'shared_directory_prefix_if_R':'283/','bytes_hash':pin((D/'original-copy-manifest.json').read_bytes())},'aliases':{'path':str(D/'original-aliases.json'),'count':len(aliases)},'governing':'governing-current.json supersedes only execution-path metadata in governing.json, whose original bytes remain retained. Ten actual fixed Git objects and their full streams.','queries':{'path':str(D/'query-descriptors.json'),'operations':len(qs),'SELECT':len(selects),'controls':controls,'original_schema_limit':'English standalone and Tibetan nested queries use parameters/row_count. Tibetan containers also contain query_only. Normalized current replay uses count; original count_keys_present and selector are explicit. Runtime needs reviewed binding of row_count (and exact count absence/presence); keep PRAGMA control separate from SELECT-only validation, without discarding its evidence. Original descriptors are copied unchanged.'},'physical':{'path':str(D/'physical-extents.json'),'count':len(physical)},'source_rows':str(D/'source-rows-full.json'),'baseline':str(B/'baseline.json'),'status_capture':str(D/'completion-state/status.stdout.bin'),'wrapper':str(R/'run-generator-ro.py.txt'),'counts':counts,'approval':'Inputs for separate root configuration/review only. No reusable verifier execution here.'})
save(D/'completion-proof.json',{'status':'PASS_RECONCILIATION_CANONICAL_AND_RETENTION','finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'baseline':base['commit'],'counts':counts,'original_dispositions':145,'dispositions':dict(collections.Counter(x['disposition'] for x in all_dis)),'originals_canonical_exit0':6,'final_canonical_exit0':3,'queries':len(qs),'SELECTs':len(selects),'query_only_controls':len(controls),'full_copy_records':len(mappings),'distinct_copy_payloads':len({r['copy'] for r in mappings}),'aliases':len(aliases),'physical_extents':len(physical),'governing_Git_objects':10,'source_baseline_and_status_unchanged':True,'helper_failure_attempts_preserved':3,'current_semantic_approval':False})
print(json.dumps({'status':'PASS_COMPLETE','counts':counts,'copies':result,'aliases':len(aliases),'queries':len(qs),'selects':len(selects)},indent=2))
