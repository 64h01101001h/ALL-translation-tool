from pathlib import Path
import subprocess,os,sys,json,hashlib,datetime
assert sys.dont_write_bytecode
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=False)
argv=sys.argv[2:];start=datetime.datetime.now(datetime.timezone.utc).isoformat()
r=subprocess.run(argv,capture_output=True,env=dict(os.environ,PYTHONDONTWRITEBYTECODE='1',PYTHONOPTIMIZE='0',GIT_OPTIONAL_LOCKS='0'))
def pin(b):return {'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
for n,b in [('stdin.bin',b''),('stdout.bin',r.stdout),('stderr.bin',r.stderr),('exit.txt',f'{r.returncode}\n'.encode())]:(out/n).write_bytes(b)
(out/'execution.json').write_text(json.dumps({'argv':argv,'cwd':os.getcwd(),'environment':{'PYTHONDONTWRITEBYTECODE':'1','PYTHONOPTIMIZE':'0','GIT_OPTIONAL_LOCKS':'0'},'started_at':start,'finished_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),'exit':r.returncode,'stdin':pin(b''),'stdout':pin(r.stdout),'stderr':pin(r.stderr)},indent=2)+'\n')
print(r.stdout.decode(errors='replace'),end='');print(r.stderr.decode(errors='replace'),end='',file=sys.stderr);sys.exit(r.returncode)
