import datetime,hashlib,json,os,subprocess,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
P=Path(__file__).resolve().parent
name,*argv=sys.argv[1:]; d=P/'calls'/name;d.mkdir(parents=True,exist_ok=False)
env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0')
start=datetime.datetime.now(datetime.timezone.utc).isoformat()
(d/'command.json').write_text(json.dumps({'argv':argv,'cwd':str(P),'environment_overrides':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0'},'started_at':start},indent=2)+'\n')
r=subprocess.run(argv,cwd=P,env=env,input=b'',capture_output=True)
for n,b in [('stdin.bin',b''),('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',(str(r.returncode)+'\n').encode())]:(d/n).write_bytes(b)
(d/'execution.json').write_text(json.dumps({'exit':r.returncode,'finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'stdout_bytes':len(r.stdout),'stderr_bytes':len(r.stderr)},indent=2)+'\n')
sys.stdout.buffer.write(r.stdout);sys.stderr.buffer.write(r.stderr);sys.exit(r.returncode)
