#!/usr/bin/env python3
"""Transport only: run the original generator, restricting its SQLite open to ro."""
import hashlib, os, runpy, sqlite3, sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[3]
WT=ROOT/'campaign-worktree'
GEN=WT/'tools/gen_alignment_page.py'
SPINE=(WT/'build/hgm_spine_v27_2.db').resolve()
assert __debug__ and os.environ.get('PYTHONDONTWRITEBYTECODE')=='1'
assert hashlib.sha256(GEN.read_bytes()).hexdigest()=='7c1c22345e18949e1aa34bb99115c32903527643b5a2c171e1a88f06d2a4cb8e'
original_connect=sqlite3.connect
def readonly_connect(database,*args,**kwargs):
    assert Path(database).resolve()==SPINE, database
    assert not args and not kwargs, (args,kwargs)
    con=original_connect(SPINE.as_uri()+'?mode=ro',uri=True)
    con.execute('PRAGMA query_only=ON')
    assert con.execute('PRAGMA query_only').fetchone()[0]==1
    return con
sqlite3.connect=readonly_connect
sys.dont_write_bytecode=True
sys.argv=[str(GEN)]
runpy.run_path(str(GEN),run_name='__main__')
