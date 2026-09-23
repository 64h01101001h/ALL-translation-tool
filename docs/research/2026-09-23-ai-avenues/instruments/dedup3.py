import sqlite3, collections, json, re
R='/Users/adamderickandrade/ALL-translation-tool/'
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db')
rows=list(con.execute("select course,seq,wylie,english from corpus_segments where course glob 'C[0-9][0-9]'"))
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
read=set((l['course'],l['seg']) for l in L if l.get('seg') is not None)
def norm(s): return re.sub(r'\s+',' ',(s or '').strip().lower())
seen=set()
for c,s,w,e in rows:
    if (c,s) in read: seen.add((norm(w),norm(e)))
stat=collections.defaultdict(lambda:[0,0,0])   # course -> [total, dup, unique]
for c,s,w,e in sorted(rows, key=lambda r:(r[0],r[1])):
    if (c,s) in read or c=='C13': continue
    k=(norm(w),norm(e))
    stat[c][0]+=1
    if k in seen: stat[c][1]+=1
    else: stat[c][2]+=1; seen.add(k)
print(f"{'course':7}{'to do':>7}{'dup':>7}{'unique':>8}  {'dup share':>9}")
T=D=U=0
for c in sorted(stat):
    t,d,u=stat[c]; T+=t; D+=d; U+=u
    print(f'{c:7}{t:>7}{d:>7}{u:>8}  {100*d/t:>8.1f}%')
print(f"{'TOTAL':7}{T:>7}{D:>7}{U:>8}  {100*D/T:>8.1f}%")
print(f'\nfree saving from copying duplicate pages: {D} segments = {D*322727/1e9:.2f}B tokens ({100*D/T:.1f}% of the remaining mandate)')
