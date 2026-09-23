import json,collections,statistics
ROOT="/Users/adamderickandrade/ALL-translation-tool"
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
by=collections.defaultdict(lambda:{"h":set(),"p":set(),"n":0})
for l in bank:
    if l.get("d")==5 and l.get("eng") and l.get("tib") and l.get("seg") is not None:
        k=(l["course"],l["seg"]); by[k]["h"].add(l["tib"]); by[k]["p"].add((l["tib"],l["eng"])); by[k]["n"]+=1
order=sorted(by)   # course then seq = reading order
seenh=set(); seenp=set(); dec=[]; N=len(order)
rows=[]
for i,k in enumerate(order):
    nh=len(by[k]["h"]-seenh); np=len(by[k]["p"]-seenp)
    seenh|=by[k]["h"]; seenp|=by[k]["p"]
    rows.append((nh,np,by[k]["n"]))
print("segments:",N)
for d in range(10):
    a=int(d*N/10); b=int((d+1)*N/10)
    ch=sum(r[0] for r in rows[a:b])/(b-a); cp=sum(r[1] for r in rows[a:b])/(b-a); cn=sum(r[2] for r in rows[a:b])/(b-a)
    print("  decile %2d  new headwords/seg %5.2f   new pairs/seg %5.2f   occurrences/seg %5.2f"%(d+1,ch,cp,cn))
last=rows[int(0.8*N):]
print("last 20%%: new headwords/seg %.2f  new pairs/seg %.2f  occurrences/seg %.2f"%(
  sum(r[0] for r in last)/len(last),sum(r[1] for r in last)/len(last),sum(r[2] for r in last)/len(last)))
