exec(open('cov.py').read().split("def incidence_stats")[0])
multi={f:m for f,m in fam_members.items() if len(m)>1}
print(len(multi)); 
for f,m in list(multi.items())[:5]: print(m, f[:60])
# which banked segments lack d5 links
allsegs=set((l['course'],l['seg']) for l in L if l['seg'] is not None)
nod5=allsegs-set(segs)
import collections
print(len(nod5), collections.Counter(c for c,s in nod5))
print(sorted(s for c,s in nod5 if c=='C03')[:40])
# family keys over ALL banked segments (not just those with d5)
allfam=collections.defaultdict(list)
for k in allsegs: allfam[fam_key(*k)].append(k)
print('families over all banked segs', len(allfam), 'segs', len(allsegs))
# segments 1-21 of C03: d5 link counts
for s in range(1,22):
    print(s, len(seg_h.get(('C03',s),())), len(seg_h.get(('C01',s),())), rows[('C03',s)][1][:50])
