import json, sqlite3, collections, math, re, random
REPO='/Users/adamderickandrade/ALL-translation-tool'
con=sqlite3.connect(REPO+'/build/hgm_spine_v27_2.db')
print([r[1] for r in con.execute('pragma table_info(entries)')])
