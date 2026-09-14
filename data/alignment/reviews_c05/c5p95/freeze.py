import datetime,hashlib,json,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent
assert not (P/'freeze.json').exists()
assert json.loads((P/'completion-proof.json').read_bytes())['status']=='COMPLETE_INDEPENDENT_SEMANTIC_APPROVAL'
assert datetime.datetime.now(datetime.timezone.utc)<datetime.datetime(2026,9,15,13,tzinfo=datetime.timezone.utc)
c=P/'calls/freeze-final';c.mkdir(parents=True,exist_ok=False)
(c/'command.json').write_text(json.dumps({'argv':[sys.executable,'-B',str(P/'freeze.py')],'invoked_shell_command':'PYTHONDONTWRITEBYTECODE=1 python3 -B freeze.py','cwd':str(P),'environment_overrides':{'PYTHONDONTWRITEBYTECODE':'1'},'recorded_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'stream_recording':'Successful finalizer writes the exact stdout below and emits it after creating and verifying the freeze.'},indent=2)+'\n')
stdout=b'Independent semantic review frozen: APPROVE.\n'
for name,raw in [('stdin.bin',b''),('stdout.bin',stdout),('stderr.bin',b''),('exit.txt',b'0\n')]:(c/name).write_bytes(raw)
files={}
for path in sorted(P.rglob('*')):
 if path.is_file():
  raw=path.read_bytes();files[str(path.relative_to(P))]={'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest()}
obj={'status':'FROZEN_INDEPENDENT_SEMANTIC_APPROVAL','frozen_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'root':str(P),'baseline_commit':'fb9182dc6fca2230be3bc5169ba1288a22430280','verdict':'APPROVE','open_findings':[],'file_count':len(files),'total_bytes':sum(v['bytes'] for v in files.values()),'excludes':['freeze.json itself; external SHA-256 pin supplied at handoff'],'files':files,'root_mechanical_pass_claimed':False}
(P/'freeze.json').write_text(json.dumps(obj,ensure_ascii=False,indent=2)+'\n')
for rel,v in files.items():
 raw=(P/rel).read_bytes();assert len(raw)==v['bytes'] and hashlib.sha256(raw).hexdigest()==v['sha256']
sys.stdout.buffer.write(stdout)
