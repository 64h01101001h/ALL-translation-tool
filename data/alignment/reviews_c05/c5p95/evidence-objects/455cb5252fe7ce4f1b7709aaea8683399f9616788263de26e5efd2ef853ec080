#!/usr/bin/env python3
"""Capture the unchanged canonical generator and independently inspect its emitted ranges."""
import datetime, hashlib, json, os, re, subprocess, sys
from html.parser import HTMLParser
from pathlib import Path
HERE=Path(__file__).resolve().parent
E=HERE.parent
ROOT=HERE.parents[3]
WT=ROOT/'campaign-worktree'
assert __debug__ and os.environ.get('PYTHONDONTWRITEBYTECODE')=='1'
def digest(data):return {'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()}
def dump(path,obj):
    assert not path.exists(),path
    path.write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
rows={r['seq']:r for r in json.loads((HERE/'queries/target-rows.json').read_text())['rows']}
class Emitted(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True);self.side=None;self.active=[];self.text={'tib':'','eng':''};self.ranges={'tib':{},'eng':{}}
    def handle_starttag(self,tag,attrs):
        a=dict(attrs)
        if tag=='div' and a.get('class') in ('tib','eng'):self.side=a['class']
        if tag=='span' and a.get('data-l'):
            self.active.append((a['data-l'],len(self.text[self.side]),a.get('data-d')))
    def handle_data(self,data):
        if self.active and self.side:self.text[self.side]+=data
    def handle_endtag(self,tag):
        if tag=='span' and self.active:
            ident,start,depth=self.active.pop();self.ranges[self.side][ident]={'range':[start,len(self.text[self.side])],'d':int(depth)}
for seq in [283,284,285]:
    D=E/str(seq);R=D/'generator-attempt01';R.mkdir(exist_ok=False)
    raw=(D/'spec.json').read_bytes();(R/'stdin.bin').write_bytes(raw)
    argv=[sys.executable,'-B',str(HERE/'run_canonical_readonly.py')]
    dump(R/'command.json',{'argv':argv,'cwd':str(WT),'environment_overrides':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'},'stdin_file':'stdin.bin','canonical_source':str(WT/'tools/gen_alignment_page.py'),'canonical_source_pin':digest((WT/'tools/gen_alignment_page.py').read_bytes()),'transport_source':str(HERE/'run_canonical_readonly.py'),'transport_source_pin':digest((HERE/'run_canonical_readonly.py').read_bytes()),'assertions_enabled':True})
    started=datetime.datetime.now(datetime.timezone.utc).isoformat()
    proc=subprocess.run(argv,input=raw,stdout=subprocess.PIPE,stderr=subprocess.PIPE,cwd=WT,env={**os.environ,'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'})
    finished=datetime.datetime.now(datetime.timezone.utc).isoformat()
    (R/'stdout.bin').write_bytes(proc.stdout);(R/'stderr.bin').write_bytes(proc.stderr);(R/'exit.txt').write_text(str(proc.returncode)+'\n')
    dump(R/'execution.json',{'started_utc':started,'finished_utc':finished,'exit':proc.returncode,'stdin':digest(raw),'stdout':digest(proc.stdout),'stderr':digest(proc.stderr)})
    assert proc.returncode==0 and proc.stderr==b'',(seq,proc.returncode,proc.stderr)
    assert not (D/'body.html').exists();(D/'body.html').write_bytes(proc.stdout)
    assert (D/'body.html').read_bytes()==(R/'stdout.bin').read_bytes()
    h=Emitted();h.feed(proc.stdout.decode());h.close()
    assert h.text['tib']==rows[seq]['wylie']
    assert h.text['eng']==rows[seq]['english']
    intended=json.loads((D/'intended-spans.json').read_text())
    for r in intended:
        ident=f's{seq}'+r['id']
        assert h.ranges['tib'][ident]['range']==r['tib_range'],(seq,ident,'tib',h.ranges['tib'][ident],r)
        assert h.ranges['eng'][ident]['range']==r['eng_range'],(seq,ident,'eng',h.ranges['eng'][ident],r)
        assert h.ranges['tib'][ident]['d']==h.ranges['eng'][ident]['d']==r['d']
    assert set(h.ranges['tib'])==set(h.ranges['eng'])=={f's{seq}S'}|{f's{seq}'+r['id'] for r in intended}
    # Exact unchanged canonical supplied-head expression, read from its inert module.
    import runpy
    gate=runpy.run_path(str(WT/'tools/test_no_supplied_span_head.py'),run_name='source_predicate_only')
    hits=[{'match':m.group(0),'groups':m.groups(),'range':[m.start(),m.end()]} for m in gate['PAT'].finditer(proc.stdout.decode())]
    assert not hits,hits
    proof={'role':'Codex independent English-first proposal mechanical check, not semantic approval','seq':seq,'spec_spans':len(intended),'depths':{str(d):sum(r['d']==d for r in intended) for d in range(1,8)},'nulls':0,'errata':len(json.loads((D/'errata.json').read_text())),'generator_exit':proc.returncode,'stderr_bytes':len(proc.stderr),'body':digest(proc.stdout),'spec':digest(raw),'body_equals_complete_stdout':True,'spine_text_reconstructed_exactly':True,'all_intended_ranges_match_emitted_html':True,'emitted_ranges':h.ranges,'actual_supplied_head_matches':hits,'canonical_predicate_source_pin':digest((WT/'tools/test_no_supplied_span_head.py').read_bytes())}
    dump(D/'mechanical-proof.json',proof)
    print(f'{seq} EXIT={proc.returncode} body_bytes={len(proc.stdout)} spans={len(intended)} null=0 sha256={hashlib.sha256(proc.stdout).hexdigest()}')
