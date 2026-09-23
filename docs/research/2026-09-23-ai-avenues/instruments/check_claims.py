import sys, os, pickle, collections, json
sys.path.insert(0,"/Users/adamderickandrade/ALL-translation-tool/tools")
import span_id_compiler as S
blockinfo, ws_blocks = pickle.load(open("recount.pkl","rb"))
res = S.battery()
# map my keys -> compiler keys
def ckey(k):
    d,page,idx = k
    info = blockinfo[k]
    return None
# compiler keys are d/page:seq(#n); build a lookup (d,page) -> list of keys in order
bypage = collections.defaultdict(list)
for key in res["blocks"]:
    dp, seq = key.rsplit(":",1)
    bypage[dp].append(key)
# order within page: the compiler iterates chunks in order; dict preserves insertion order
wscls = collections.Counter()
for (d,page,idx) in ws_blocks:
    key = bypage["%s/%s"%(d,page)][idx-1]
    wscls[(res["blocks"][key]["class"], res["blocks"][key]["kind"])] += 1
print("WHITESPACE_EDGE blocks by compiler class:", dict(wscls))
# per directory / kind
per = collections.defaultdict(lambda: [0,0])
for key,b in res["blocks"].items():
    d = key.split("/")[0]
    per[(d,b["kind"])][0]+=1; per[(d,b["kind"])][1]+= b["class"]=="OK"
for k in sorted(per): print("  %-12s %-20s blocks %4d round-trip %4d" % (k[0],k[1],per[k][0],per[k][1]))
base = S.load_baseline()
print("baseline segments_ok", base["segments_ok"], "frozen", len(base["segments"]))
print("frozen by dir", collections.Counter(k.split("/")[0] for k in base["segments"]))
c3 = collections.Counter(b["class"] for k,b in res["blocks"].items() if k.startswith("pages_c03/") and b["kind"]=="GENERATOR" and b["class"]!="OK")
print("C03 generator-labelled failures by class", dict(c3))
nen = [(k,b["kind"]) for k,b in res["blocks"].items() if b["class"]=="NO_ENGLISH_NO_MARKER"]
print("NO_ENGLISH_NO_MARKER", nen)
emp = [k for k,b in res["blocks"].items()]
pickle.dump(res, open("res.pkl","wb"))
