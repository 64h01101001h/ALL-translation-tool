#!/usr/bin/env python3
"""Freeze each complete own segment tree. No files are added after its freeze."""
import datetime, hashlib, json, os
from pathlib import Path
HERE=Path(__file__).resolve().parent
E=HERE.parent
assert __debug__ and os.environ.get('PYTHONDONTWRITEBYTECODE')=='1'
def pin(p):
    data=p.read_bytes();return {'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()}
for seq in [283,284,285]:
    D=E/str(seq);F=D/'freeze.json';assert not F.exists(),F
    proof=json.loads((D/'mechanical-proof.json').read_text())
    assert pin(D/'body.html')==proof['body'] and pin(D/'spec.json')==proof['spec']
    assert (D/'body.html').read_bytes()==(D/'generator-attempt01/stdout.bin').read_bytes()
    assert (D/'generator-attempt01/stderr.bin').read_bytes()==b''
    assert (D/'generator-attempt01/exit.txt').read_text()=='0\n'
    spec=json.loads((D/'spec.json').read_text())['segments'][0]
    intended=json.loads((D/'intended-spans.json').read_text())
    assert [r['id'] for r in intended]==[s['id'] for s in spec['spans']]
    records=[]
    for p in sorted(D.rglob('*')):
        if p.is_file():
            assert not p.is_symlink(),p
            records.append({'path':str(p.relative_to(D)),**pin(p)})
    frozen={'producer':'Codex (GPT-6), independent English-first original proposer','historical_agent_name':'c05_280_282_english','current_batch':'C05-283-285','seq':seq,'status':'PROVISIONAL_UNREVIEWED','baseline_closed_head':'fb9182dc6fca2230be3bc5169ba1288a22430280','frozen_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'schema':'files is a LIST; each path is relative to this segment root; bytes and SHA-256 cover full file; freeze.json excludes itself','files':records,'file_count':len(records),'total_bytes':sum(r['bytes'] for r in records),'shared_evidence_root':'../283','intended_spans_schema':'LIST in exact spec order; zero-based half-open Unicode code-point ranges in unchanged original spine text','boundary':'All own source copies, query results, original source mappings, failed/superseded preparations, specs, intended ranges, reports, bodies and actual canonical streams are frozen. Mechanical checks are not semantic approval.'}
    F.write_text(json.dumps(frozen,ensure_ascii=False,indent=2)+'\n')
    for r in records:assert pin(D/r['path'])=={k:r[k] for k in ['bytes','sha256']}
    assert sorted(str(p.relative_to(D)) for p in D.rglob('*') if p.is_file() and p!=F)==sorted(r['path'] for r in records)
    print(json.dumps({'seq':seq,'freeze':str(F),'freeze_pin':pin(F),'file_count':len(records),'total_bytes':frozen['total_bytes'],'spans':proof['spec_spans'],'d5':proof['depths']['5'],'nulls':proof['nulls'],'generator_exit':proof['generator_exit'],'errata':proof['errata']}))
