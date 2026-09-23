#!/usr/bin/env python3
"""Read-only measurement: how often does the EXACT-STRING precedent index
already surface the rendering a segment ended up with, and how much headroom
is left for any fuzzier retriever (lexical variant or dense)?

Causal split: target course T, precedent pool = links from courses listed
before T (C01 < C02 < C03 < C04 < C05). Also a whole-bank leave-one-segment-out
where the pool excludes every segment with byte-identical wylie to the target.

Unit = a d=5 link in the target (the dictionary layer).
"""
import collections, json, re, sqlite3, sys

ROOT = "/Users/adamderickandrade/ALL-translation-tool"
L = json.load(open(ROOT + "/data/alignment/alignment_full_v1.json"))["links"]
db = sqlite3.connect(ROOT + "/build/hgm_spine_v27_2.db")
wy = {(c, s): (w or "") for c, s, w in db.execute(
    "select course, seq, wylie from corpus_segments")}
ORDER = ["C01", "C02", "C03", "C04", "C05"]

PART = {"kyi", "gyi", "gi", "'i", "yi", "kyis", "gyis", "gis", "'is", "yis",
        "la", "su", "ru", "du", "tu", "r", "na", "nas", "las", "pa", "ba",
        "po", "bo", "ma", "mo", "dang", "ni", "yang", "kyang", "'ang", "te",
        "ste", "de", "go", "ngo", "do", "no", "bo", "'o", "ro", "so", "to",
        "cing", "zhing", "shing", "gin", "kyin", "gyin", "yin", "pas", "bas",
        "par", "bar", "pa'i", "ba'i", "po'i", "bo'i", "ma'i", "mo'i"}
SYL = re.compile(r"[A-Za-z0-9'+.\-~^?]+")


def syls(t):
    return SYL.findall(t)


def norm(t):
    s = syls(t)
    while len(s) > 1 and s[-1] in PART:
        s = s[:-1]
    return " ".join(s)


def ntext(e):
    return re.sub(r"\s+", " ", e.strip().strip(",.;:").lower())


def run(label, targets, pool_pred):
    pool = [l for l in L if pool_pred(l)]
    eng = collections.defaultdict(collections.Counter)
    nul = collections.Counter()
    heads = set()
    for l in pool:
        heads.add(l["tib"])
        if l["eng"] is None:
            nul[l["tib"]] += 1
        else:
            eng[l["tib"]][l["eng"]] += 1
    normmap = collections.defaultdict(set)
    for h in heads:
        normmap[norm(h)].add(h)
    eng_any = collections.defaultdict(set)   # normalised english -> tibs
    for l in pool:
        if l["eng"] is not None:
            eng_any[ntext(l["eng"])].add(l["tib"])
    headsyl = collections.defaultdict(set)
    for h in heads:
        for s in set(syls(h)):
            headsyl[s].add(h)

    st = collections.Counter()
    rank_hits = collections.Counter()
    for l in targets:
        st["n"] += 1
        t, e = l["tib"], l["eng"]
        st["null" if e is None else "rendered"] += 1
        if t in heads:
            st["H_exact"] += 1
            if e is None:
                if nul[t]:
                    st["ans_exact"] += 1
            else:
                ranked = [x for x, _ in sorted(eng[t].items(),
                                               key=lambda kv: (-kv[1], kv[0]))]
                if e in ranked:
                    st["ans_exact"] += 1
                    r = ranked.index(e) + 1
                    for k in (1, 3, 6):
                        if r <= k:
                            rank_hits[k] += 1
                else:
                    st["rendered_novel_for_known_head"] += 1
            continue
        n = norm(t)
        if n and n in normmap:
            st["H_norm_only"] += 1
            if e is not None and any(e in eng[h] for h in normmap[n]):
                st["ans_norm_only"] += 1
            continue
        # English-side: same English verbatim elsewhere, with a Tibetan that
        # shares a syllable -- the case only a variant-aware lookup finds
        if e is not None:
            tibs = eng_any.get(ntext(e), set())
            ss = set(syls(t))
            if tibs:
                st["eng_seen_elsewhere"] += 1
                if any(ss & set(syls(x)) for x in tibs):
                    st["eng_seen_with_shared_syllable"] += 1
        ss = syls(t)
        if any(s in headsyl for s in ss):
            st["partial_syllable_overlap"] += 1
        else:
            st["no_lexical_hook"] += 1
    n = st["n"]
    print("\n== %s  (pool %d links, %d headwords)" % (label, len(pool),
                                                   len(heads)))
    for k in ["n", "rendered", "null", "H_exact", "ans_exact",
              "rendered_novel_for_known_head", "H_norm_only", "ans_norm_only",
              "eng_seen_elsewhere", "eng_seen_with_shared_syllable",
              "partial_syllable_overlap", "no_lexical_hook"]:
        print("  %-34s %6d  %5.1f%%" % (k, st[k], 100.0 * st[k] / n))
    ren_ans = sum(rank_hits.values()) and rank_hits
    print("  rank of the used rendering among exact-head renderings "
          "(rendered links whose rendering is attested):")
    tot = st["ans_exact"] - sum(1 for l in targets if l["eng"] is None
                                and l["tib"] in heads and nul[l["tib"]])
    for k in (1, 3, 6):
        print("    recall@%d  %d / %d = %.1f%%" % (k, rank_hits[k], tot,
                                                   100.0 * rank_hits[k] / max(tot, 1)))


# causal: each course against the courses before it
for i, T in enumerate(ORDER[1:], 1):
    prior = set(ORDER[:i])
    tg = [l for l in L if l["course"] == T and l["d"] == 5 and l["seg"]]
    run("CAUSAL %s <- %s" % (T, "+".join(ORDER[:i])), tg,
        lambda l, prior=prior: l["course"] in prior)

# leave-one-segment-out over the whole bank, identical-wylie segments excluded
bywy = collections.defaultdict(set)
for (c, s), w in wy.items():
    bywy[w.strip()].add((c, s))
segs = sorted({(l["course"], l["seg"]) for l in L if l["seg"]})
# sample every 4th segment to keep it quick but whole-bank
import random
random.seed(7)
samp = random.sample(segs, 300)
agg_t = []
for (c, s) in samp:
    pass
# do it per-segment (pool excludes the segment and its exact twins)
tot = collections.Counter()
for (c, s) in samp:
    twins = bywy.get(wy.get((c, s), "").strip(), {(c, s)}) | {(c, s)}
    tg = [l for l in L if l["course"] == c and l["seg"] == s and l["d"] == 5]
    if not tg:
        continue
    # reuse run() quietly by accumulating
    pool = [l for l in L if (l["course"], l["seg"]) not in twins]
    heads = {l["tib"] for l in pool}
    pairs = {(l["tib"], l["eng"]) for l in pool}
    for l in tg:
        tot["n"] += 1
        if l["tib"] in heads:
            tot["H_exact"] += 1
            if (l["tib"], l["eng"]) in pairs:
                tot["ans_exact"] += 1
print("\n== LOSO whole bank, 300 random segments, twins excluded")
for k in ("n", "H_exact", "ans_exact"):
    print("  %-10s %6d  %5.1f%%" % (k, tot[k], 100.0 * tot[k] / tot["n"]))
