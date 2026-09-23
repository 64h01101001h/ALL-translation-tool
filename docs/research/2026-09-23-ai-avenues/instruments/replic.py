import json,sqlite3,collections,re
R='/Users/adamderickandrade/ALL-translation-tool/'
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,wylie,english from corpus_segments")
rows=[(c,s,w or '',e or '') for c,s,w,e in cur.fetchall()]
c18=[r for r in rows if re.fullmatch(r'C(0[1-9]|1[0-8])',r[0])]
print('C01-C18 segments', len(c18))
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
landed={(x['course'],x['seg']) for x in full['links'] if x['seg'] is not None}
print('landed (real)', len(landed))
g=collections.defaultdict(list)
for c,s,w,e in c18:
    g[(w,e)].append((c,s))
# unlanded segments whose text duplicates a LANDED segment  == free blind-replication supply
supply=[]
for k,v in g.items():
    L=[m for m in v if m in landed]; U=[m for m in v if m not in landed]
    if L and U: supply.extend(U)
print('UNLANDED segments whose exact text is already landed (free replication supply):', len(supply))
print('  distinct courses:', collections.Counter(c for c,s in supply).most_common())
# C13 exclusion
print('  of which C13:', sum(1 for c,s in supply if c=='C13'))
# remaining-work denominator re-derivation
c13=[m for m in landed if m[0]=='C13']
rem=[(c,s) for c,s,w,e in c18 if (c,s) not in landed and c!='C13']
print('remaining after landed and C13:', len(rem))
seen={ (w,e) for c,s,w,e in c18 if (c,s) in landed }
txt={(c,s):(w,e) for c,s,w,e in c18}
dup_read=[m for m in rem if txt[m] in seen]
rest=[m for m in rem if txt[m] not in seen]
grp=collections.defaultdict(list)
for m in rest: grp[txt[m]].append(m)
dup_each=sum(len(v)-1 for v in grp.values())
print('duplicates of already-read text:', len(dup_read), ' duplicates of each other:', dup_each,
      ' UNIQUE remaining:', len(rest)-dup_each)
