import sqlite3,json,collections
R='/Users/adamderickandrade/ALL-translation-tool'
db=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
C=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s)"%(','.join('?'*18)),C).fetchall()
print('C01-C18 segments:',len(rows))
bank=json.load(open(R+'/data/alignment/alignment_full_v1.json'))
links=bank['links'] if isinstance(bank,dict) and 'links' in bank else bank
if isinstance(bank,dict):
    print('bank top keys:',list(bank.keys())[:10])
links=bank.get('links') if isinstance(bank,dict) else bank
landed=set()
for L in links:
    if L.get('seg') is not None: landed.add((L.get('course'),L.get('seg')))
print('landed (course,seg) pairs:',len(landed))
nullseg=sum(1 for L in links if L.get('seg') is None)
print('links with seg=null:',nullseg,'total links:',len(links))

C13=[r for r in rows if r[0]=='C13']
rem=[r for r in rows if r[0]!='C13' and (r[0],r[1]) not in landed]
print('C13:',len(C13),' remaining after C13+landed:',len(rem))
readtext={(r[2],r[3]) for r in rows if (r[0],r[1]) in landed}
seen=set(); uniq=[]; dup_read=0; dup_self=0
for r in rem:
    k=(r[2],r[3])
    if k in readtext: dup_read+=1; continue
    if k in seen: dup_self+=1; continue
    seen.add(k); uniq.append(r)
print('dup of already-read:',dup_read,'dup of each other:',dup_self,'UNIQUE REMAINING:',len(uniq))
wy=sum(len(r[2] or '') for r in uniq); en=sum(len(r[3] or '') for r in uniq)
print('UNIQUE REMAINING chars: wylie %d english %d total %d'%(wy,en,wy+en))
print('  mean per segment: wylie %.0f english %.0f total %.0f'%(wy/len(uniq),en/len(uniq),(wy+en)/len(uniq)))
# whole spine
allr=db.execute("select wylie,english from corpus_segments").fetchall()
awy=sum(len(a or '') for a,b in allr); aen=sum(len(b or '') for a,b in allr)
print('WHOLE SPINE chars: wylie %d english %d total %d over %d segments'%(awy,aen,awy+aen,len(allr)))
# landed segments char count (work already done)
ldone=[r for r in rows if (r[0],r[1]) in landed]
lwy=sum(len(r[2] or '') for r in ldone); len_=sum(len(r[3] or '') for r in ldone)
print('LANDED chars: %d over %d segments (mean %.0f)'%(lwy+len_,len(ldone),(lwy+len_)/len(ldone)))
# per-course unique
pc=collections.Counter(r[0] for r in uniq); rc=collections.Counter(r[0] for r in rem)
print('per course remaining->unique:', {k:(rc[k],pc[k]) for k in sorted(rc)})
json.dump([[r[0],r[1]] for r in uniq],open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/uniq.json','w'))
