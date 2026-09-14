#!/usr/bin/env python3
"""Full-byte evidence bindings, physical extents, and release-row checks."""
import datetime, difflib, gzip, hashlib, json, os, runpy
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[3]
WT=ROOT/'campaign-worktree'
assert __debug__ and os.environ.get('PYTHONDONTWRITEBYTECODE')=='1'
def pin(b):return {'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
def write(name,obj):
    p=HERE/name;assert not p.exists(),p
    p.write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
manifest=json.loads((HERE/'original-to-copy-manifest.json').read_text())
spine=manifest[-1]
spine['aliases']=[str(WT/'build/hgm_spine_v27_2.db'),str((WT/'build/hgm_spine_v27_2.db').resolve())]
spine['original']=str(WT/'build/hgm_spine_v27_2.db')
f=WT/'tools/test_no_supplied_span_head.py';data=f.read_bytes();copy=HERE/'sources/21-test_no_supplied_span_head.py';copy.write_bytes(data)
if not any(m['original']==str(f) for m in manifest):
    manifest.append({'original':str(f),'resolved_original':str(f.resolve()),'aliases':[str(f)],'copy':str(copy.relative_to(HERE)),**pin(data)})
checks=[]
for m in manifest:
    original=Path(m['original']).read_bytes();assert pin(original)=={k:m[k] for k in ['bytes','sha256']},m['original']
    if 'copy' in m:assert (HERE/m['copy']).read_bytes()==original
    checks.append({'original':m['original'],'resolved_original':str(Path(m['original']).resolve()),**pin(original),'copy_equal':True if 'copy' in m else None})
(HERE/'original-to-copy-manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
write('original-source-stability.json',{'checked_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'all_full_bytes_match':True,'checks':checks,'limit':'The large SQLite is retained whole at the resolved original, with complete query results copied; this is not a copy of the entire database.'})
base=Path('/Users/adamderickandrade/Desktop/HGM DICTIONARY - TRANSLATION APP')
mac=Path("/Users/adamderickandrade/ACIP Software Developmeent/Ven. Phil's Development Folder/Mac_OSX")
refs=[base,Path('/Users/adamderickandrade/ACIP Software Developmeent/Gofer Developmetn Files')]+[mac/n for n in ['ACIPHypercontext Tool','LokeshChandraDictionary','CreateLokeshChandraDIctionary','FixLokeshChandra']]+[Path('/Users/adamderickandrade/ACIP Software Developmeent/tibetan-spellchecker'),Path('/Users/adamderickandrade/Tibetan Translation Tool')]
access=[{'path':str(p),'is_directory':p.is_dir(),'readable':os.access(p,os.R_OK)} for p in refs];assert all(x['is_directory'] and x['readable'] for x in access);write('required-reference-access.json',access)
master=json.loads(gzip.decompress((WT/'data/hgm_dictionary_v27_2.json.gz').read_bytes()))
corpus=json.loads(gzip.decompress((WT/'data/full_parallel_corpus_v32.json.gz').read_bytes()))
entry_ids=set();corpus_ids=set()
for f in (HERE/'queries').glob('*.json'):
    d=json.loads(f.read_text())
    if not isinstance(d,dict) or 'rows' not in d:continue
    for row in d['rows']:
        if not isinstance(row,dict) or 'raw' not in row:continue
        raw=json.loads(row['raw'])
        if 'hgm_gloss' in row:
            assert raw==master['unified_entries'][row['id']-1],(f,row['id'])
            assert json.loads(row['hgm_gloss'] or '[]')==(raw.get('hgm_gloss') or [])
            entry_ids.add(row['id'])
        else:
            assert raw==corpus[row['id']-1],(f,row['id'])
            assert all(row[k]==raw[k] for k in ['course','acip','wylie','english'])
            corpus_ids.add(row['id'])
write('release-row-binding.json',{'master_pin':pin((WT/'data/hgm_dictionary_v27_2.json.gz').read_bytes()),'corpus_pin':pin((WT/'data/full_parallel_corpus_v32.json.gz').read_bytes()),'master_total_entries':len(master['unified_entries']),'corpus_total_rows':len(corpus),'all_copied_material_raw_rows_equal_original_release_entries':True,'unique_dictionary_entry_ids':sorted(entry_ids),'unique_corpus_row_ids':sorted(corpus_ids),'authority':'HGM gloss and original source English, never comparative Hopkins/LC English. No release or hgm_gloss changes.'})
targets=json.loads((HERE/'queries/target-rows.json').read_text())['rows']
physical=[];engine=runpy.run_path(str(WT/'engines/hgm_tools.py'),run_name='source_converter_only')
for course,windows in [('C05',[(283,2771,2771,2772,2776),(284,2778,2778,2779,2783),(285,2785,2785,2786,2796)]),('C15',[(283,4161,4161,4162,4166),(284,4168,4169,4170,4174),(285,4176,4176,4177,4186)])]:
    f=base/'To DO/Formal Courses/ACI Readings In Roman ASCII'/f'{course}ReadingASCII.txt';raw=f.read_bytes();lines=raw.splitlines(keepends=True)
    for seq,tlo,thi,elo,ehi in windows:
        tb=b''.join(lines[tlo-1:thi]);eb=b''.join(lines[elo-1:ehi]);acip=' '.join(tb.decode().split());eng=' '.join(eb.decode().split());wyl=engine['acip_to_ewts'](acip)
        target=next(x for x in targets if x['seq']==seq)
        q=json.loads((HERE/'queries'/f'parallel-{seq}.json').read_text())['rows'];cp=next(r for r in q if r['course']==course)
        assert all(x==cp[k] for x,k in [(acip,'acip'),(wyl,'wylie'),(eng,'english')]),(course,seq,eng,cp['english'])
        physical.append({'original':str(f),'source_pin':pin(raw),'target_seq':seq,'parallel_course':course,'parallel_seq':cp['seq'],'acip_lines':[tlo,thi],'english_lines':[elo,ehi],'acip_byte_range':[sum(map(len,lines[:tlo-1])),sum(map(len,lines[:thi]))],'english_byte_range':[sum(map(len,lines[:elo-1])),sum(map(len,lines[:ehi]))],'original_acip_lines':tb.decode(),'original_english_lines':eb.decode(),'whitespace_joined_acip':acip,'converted_wylie':wyl,'whitespace_joined_english':eng,'three_fields_equal_named_spine_row':True,'same_fields_as_C05_target':{k:cp[k]==target[k] for k in ['acip','wylie','english']},'english_difference':list(difflib.ndiff([target['english']],[cp['english']])) if target['english']!=cp['english'] else []})
f=base/'To DO/Formal Courses/ACI Readings In Roman ASCII/C16ReadingASCII.txt';norm=b' '.join(f.read_bytes().split())
write('physical-source-bindings.json',{'rows':physical,'full_argument_read':{'source':'sources/18-C05ReadingASCII.txt','lines_inclusive':[2338,3151],'description':'Complete Reading Six opening attribution and Stream argument, through its explicit conclusion. Includes the three substances, the positions disputed, the limbs analogy and following rebuttals.'},'related_comparative_extent':{'source':'sources/20-C16ReadingASCII.txt','lines_inclusive':[9307,9353],'description':'Later concluding remarks support spatial blocking terminology; not a complete witness for targets 283–285.','target_full_acip_found':{str(t['seq']):t['acip'].encode('ascii') in norm for t in targets}},'method':'Original CRLF bytes retained in full copies. Line extents retain original strings and byte offsets; only whitespace joined for comparison. Canonical acip_to_ewts applied read-only.','lineage_limit':'C15 supports compared text fields; punctuation differs at 283/428. No claim of independent publication or ingestion lineage. P7:31 is only a truncated corpus parallel and cannot attest its missing final word.'})
print(json.dumps({'original_files':len(manifest),'unique_dictionary_rows':len(entry_ids),'unique_corpus_rows':len(corpus_ids),'physical_target_blocks':len(physical),'all_original_bindings':'PASS'}))
