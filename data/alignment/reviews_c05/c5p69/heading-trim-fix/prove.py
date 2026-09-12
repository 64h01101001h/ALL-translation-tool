import sys,json,types,subprocess,pathlib,copy,re,unittest,io,hashlib
ROOT=pathlib.Path('/Users/adamderickandrade/Documents/ChatGPT/Geshe Michael Roach Tib _ Eng Alignment/campaign-worktree')
ART=ROOT.parent/'campaign-artifacts/integration/heading-trim-fix'
sys.path.insert(0,str(ROOT/'tools'))
import build_dictionary_view as new
import test_view_matches_layer as gate
import test_dictionary_view_trimming as tests
old=types.ModuleType('old_builder');old.__file__=new.__file__
exec(compile(subprocess.check_output(['git','show','HEAD:tools/build_dictionary_view.py'],cwd=ROOT,text=True),'old_builder','exec'),old.__dict__)
def old_phon(t,e):
    trimmed=e
    try:
        say=old._P.pronounce(t);cut=len(say)+2
        if len(e)>cut*1.6:
            m=old.re.match(r'^.{%d,%d}?[\.\,]'%(max(0,cut-12),cut+14),e)
            if m:trimmed=m.group(0)
    except Exception:pass
    return trimmed
full=json.load(open(new.FULL));ev=json.load(open(new.EVID));P=gate.payload(pathlib.Path(new.OUT).read_text())
changes=[];phon=[]
for l in full['links']:
    t,e=(l.get('tib') or '').strip(),(l.get('eng') or '').strip()
    if not l.get('d') or not t or not e or l.get('seg') is None or not l.get('course'):continue
    if old.is_phonetic(t,e) and not e[:1].isupper():
        before,after=old_phon(t,e),new.trim_phonetic(t,e)
        phon.append({'source_link':l,'old':before,'new':after,'identical':before==after})
        continue
    before=old.trim_glued(e)[0];after=new.trim_glued(e)[0]
    if before!=after:
        m=new.NUMBERED_COURSE_HEAD.search(e)
        changes.append({'source_link':l,'ref':f"{l['course']}:{l['seg']}",'old':before,'new':after,'range_units':'Unicode code points; zero-based half-open, original source','old_removed_range':[len(before),len(e)],'new_removed_range':[len(after),len(e)],'removed_from_display':before[len(after):],'numbered_heading_match_range':list(m.span()) if m else None,'reason':'Sentence punctuation, page number, full Asian Classics Institute Course Roman-numeral heading, then an existing recognized Reading heading.'})
assert all(x['identical'] for x in phon)
assert len(changes)==11
assert {x['ref'] for x in changes}=={'C03:131','C03:199','C04:37','C04:49','C04:117','C04:133','C04:168','C05:96','C05:118','C05:145','C05:206'}
(ART/'migration.json').write_text(json.dumps(changes,ensure_ascii=False,indent=2)+'\n')
(ART/'phonetic-all-links-proof.json').write_text(json.dumps(phon,ensure_ascii=False,indent=2)+'\n')
inv=json.load(open(ART/'baseline-inventory.json'))
for row in inv:
    row['new_canonical']=[new.trim_phonetic(row['tib'],s) if row['depth']=='phon' else new.trim_glued(s)[0] for s in row['banked_prefix_sources']]
    row['phonetic_identical']=None if row['depth']!='phon' else all(new.trim_phonetic(row['tib'],s)==row['eng'] for s in row['banked_prefix_sources'])
assert all(x['phonetic_identical'] for x in inv if x['depth']=='phon')
(ART/'baseline-inventory-after.json').write_text(json.dumps(inv,ensure_ascii=False,indent=2)+'\n')
Q=copy.deepcopy(P);replaced=[]
for d,band in list(Q['depths'].items())+[('phon',Q['phonetics'])]:
    for t,a,senses in band:
        for sense in senses:
            matching=[x for x in changes if str(x['source_link']['d'])==d and x['source_link']['tib']==t and x['old']==sense[0] and x['ref'] in sense[1]]
            if matching:
                assert len({x['new'] for x in matching})==1
                replaced.append({'d':d,'tib':t,'refs':sense[1],'old':sense[0],'new':matching[0]['new']})
                sense[0]=matching[0]['new']
counts,bad,both=gate.classify(Q,full,ev)
assert not bad and not both
assert len(replaced)==11
assert counts['trimmed (trim_glued)']==41
(ART/'in-memory-classification.json').write_text(json.dumps({'counts':counts,'bad':bad,'both':both,'row_changes':replaced},ensure_ascii=False,indent=2)+'\n')
# Mutate actual in-memory display values, with no file regeneration.
mutations=[]
for name,eng,refs in [('arbitrary-prefix','Because',None),('extended-heading',changes[-1]['new']+' 73',None),('invented','invented English',None),('wrong-reference',changes[-1]['new'],['C05:999'])]:
    R=copy.deepcopy(Q)
    row=next(row for row in R['depths']['1'] if row[0]==changes[-1]['source_link']['tib'])
    sense=next(s for s in row[2] if 'C05:206' in s[1]);sense[0]=eng
    if refs is not None:sense[1]=refs
    c,b,_=gate.classify(R,full,ev)
    assert c['INVENTED-BY-THE-VIEW']==1,(name,c)
    mutations.append({'mutation':name,'counts':c,'bad':b,'rejected':True})
# Mutate the shared implementation in memory, verify independent literals fail.
original_head=new.NUMBERED_COURSE_HEAD
new.NUMBERED_COURSE_HEAD=re.compile(r'(?!)')
out=io.StringIO();r=unittest.TextTestRunner(stream=out).run(unittest.defaultTestLoader.loadTestsFromTestCase(tests.HeadingTrimmingTests))
new.NUMBERED_COURSE_HEAD=original_head
assert not r.wasSuccessful()
mutations.append({'mutation':'remove-numbered-heading-recognition','rejected':True,'output':out.getvalue()})
original_phon=new.trim_phonetic;original_gate_phon=gate.trim_phonetic
new.trim_phonetic=lambda t,e:e.split()[0]
gate.trim_phonetic=new.trim_phonetic
out=io.StringIO();r=unittest.TextTestRunner(stream=out).run(unittest.defaultTestLoader.loadTestsFromTestCase(tests.HeadingTrimmingTests))
new.trim_phonetic=original_phon;gate.trim_phonetic=original_gate_phon
assert not r.wasSuccessful()
mutations.append({'mutation':'arbitrary-phonetic-first-word','rejected':True,'output':out.getvalue()})
(ART/'mutations.json').write_text(json.dumps(mutations,ensure_ascii=False,indent=2)+'\n')
summary={'changed_links':len(changes),'changed_display_rows':len(replaced),'changed_refs':[x['ref'] for x in changes],'phonetic_links':len(phon),'all_phonetic_outputs_identical':True,'prior_phonetic_shortened_rows':18,'classification':counts,'mutations_rejected':len(mutations),'bank_sha256':{str(pathlib.Path(p).relative_to(ROOT)):hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest() for p in [new.FULL,new.EVID]}}
(ART/'proof-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
print(json.dumps(summary,indent=2))
