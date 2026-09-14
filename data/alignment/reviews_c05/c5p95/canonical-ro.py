import os,runpy,sqlite3,sys
from pathlib import Path
assert __debug__ and sys.dont_write_bytecode
W=Path(__file__).resolve().parents[3]/'campaign-worktree'
connect=sqlite3.connect
def readonly(database,*args,**kwargs):
 p=Path(database).resolve();assert p==(W/'build/hgm_spine_v27_2.db').resolve()
 kwargs['uri']=True
 c=connect(p.as_uri()+'?mode=ro',*args,**kwargs);c.execute('PRAGMA query_only=ON');assert c.execute('PRAGMA query_only').fetchone()[0]==1
 return c
sqlite3.connect=readonly
os.chdir(W);sys.argv=[str(W/'tools/gen_alignment_page.py')]
runpy.run_path(sys.argv[0],run_name='__main__')
