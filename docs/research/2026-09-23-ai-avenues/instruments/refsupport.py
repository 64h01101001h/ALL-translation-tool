# READ-ONLY: does lexical support from reference dictionaries (hgm_gloss,
# Hopkins, 84000) separate the Claude-labelled audit defects from SOUND calls?
# Labels are Claude's (audit_verdicts_20260904.json names Claude models), so
# this measures agreement with Claude, NOT accuracy.
import json, sqlite3, re, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/audit_verdicts_20260904.json'))
db=sqlite3.connect('file:'+R+'build/hgm_spine_v27_2.db?mode=ro',uri=True)
g=json.load(open(R+'data/84000/g84000.json'))['entries']
STOP=set('the a an of to and or in on for with by as is are be was were his her its their our your my this that these those it i you he she we they not no from at into'.split())
def toks(s): return {w for w in re.findall(r"[a-z]+", (s or '').lower()) if w not in STOP and len(w)>2}
def stem(w): return w[:5]
def refs(h):
    out=set()
    cands=[h, re.sub(r"(\s?(kyi|gyi|gi|'i|yi|kyis|gyis|gis|'is|yis|la|las|nas|na|du|tu|su|ru|r|s|pa|ba|po|bo|ma))$","",h).strip()]
    for c in set(cands):
        for hg,hop in db.execute("select hgm_gloss,hopkins_english from entries where wylie=?",(c,)):
            try: out|= {stem(w) for x in json.loads(hg or '[]') for w in toks(x)}
            except Exception: out|={stem(w) for w in toks(hg)}
            out|={stem(w) for w in toks(hop)}
        e=g.get(c) if isinstance(g,dict) else None
        if e: out|={stem(w) for w in toks(json.dumps(e))}
    return out
tab=collections.Counter()
for x in d['items']:
    r=refs(x['headword'].strip()); e={stem(w) for w in toks(x['eng'])}
    sup='no-eng-content' if not e else ('supported' if e & r else ('no-ref-entry' if not r else 'unsupported'))
    tab[(x['final'],sup)]+=1
for k,v in sorted(tab.items()): print(k,v)
