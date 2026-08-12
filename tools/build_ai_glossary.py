#!/usr/bin/env python3
"""build_ai_glossary.py — the ALL Working Glossary (AI-provisional
tier; Adam's proposal, 2026-08-12; deep-entry form 2026-08-12).

THE WALL (inviolable): AI never touches Geshe Michael's English.
- Candidates are ONLY entries with NO hgm_gloss.
- Prompts receive ONLY reference comparanda (Lokesh Chandra
  Sanskrit, Hopkins English) — hgm_evidence and all corpus English
  are deliberately withheld.
- Output lives in its OWN file and tier ("ai-draft"), rendered only
  under an unmistakable banner, suppressed wherever HGM speaks.
- INSUFFICIENT is a permitted answer (rule 3).

THE FORM (Adam, from the Illuminator's example — structure only,
never Duff's text): part of speech, numbered senses with context
prose, Sanskrit, and see-also links. Guardrails on the form:
- POS is DETERMINISTIC only: the SOAS hand-tagged lexicon
  (unambiguous tags) or the dictionary's own tense evidence
  ("verb"). The model never invents a part of speech.
- see_also may be chosen ONLY from a deterministically computed
  candidate list (same first syllable / shared Sanskrit token,
  all real headwords) — every link resolves, none hallucinated.

Usage: build_ai_glossary.py [--limit N]   (default pilot: 50)
Requires ANTHROPIC_API_KEY (Adam's key, standing authorization).
"""
import json, os, re, sqlite3, sys, time, urllib.request
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
SOAS = os.path.join(ROOT, "data", "soas_pos", "classical-lexicon.txt")
OUTDIR = os.path.join(ROOT, "data", "ai_glossary")
OUT = os.path.join(OUTDIR, "ai_glossary.json")
MODEL = "claude-opus-5"   # the app's own analysis model
BATCH = 5

SYSTEM = """You are a careful Tibetan-lexicography assistant for the \
Asian Legacy Library, drafting WORKING dictionary entries. For each \
numbered headword you receive: reference comparanda (a Sanskrit \
equivalent from Lokesh Chandra and/or an English gloss from the \
Hopkins dictionary) and a numbered RELATED list of real dictionary \
headwords. Rules:
(1) Use ONLY the material given — no outside knowledge of how any \
particular translator renders the term.
(2) Write 1-3 numbered SENSES; each sense has a concise "gloss" (a \
few words, dictionary style) and a "context" of 1-3 sentences \
explaining the sense as far as the comparanda genuinely support — \
never further.
(3) Choose up to 4 "see_also" entries ONLY from the RELATED list, by \
their numbers, and only when genuinely related.
(4) If the comparanda are too thin, answer {"n": <n>, \
"insufficient": true}.
(5) Output strict JSON: an array, one object per headword: \
{"n": <n>, "senses": [{"gloss": "...", "context": "..."}], \
"see_also": [<related numbers>]} — nothing else."""

def api(user_text):
    key = os.environ.get("ANTHROPIC_API_KEY")
    if not key:
        sys.exit("ANTHROPIC_API_KEY not set")
    req = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps({
            "model": MODEL,
            "max_tokens": 3000,
            "system": SYSTEM,
            "messages": [{"role": "user", "content": user_text}],
        }).encode(),
        headers={
            "x-api-key": key,
            "anthropic-version": "2023-06-01",
            "content-type": "application/json",
        })
    with urllib.request.urlopen(req, timeout=180) as r:
        d = json.load(r)
    return "".join(b.get("text", "") for b in d.get("content", []))

def main():
    limit = 50
    if "--limit" in sys.argv:
        limit = int(sys.argv[sys.argv.index("--limit") + 1])

    # deterministic POS: SOAS unambiguous tags
    soas = defaultdict(set)
    if os.path.exists(SOAS):
        for line in open(SOAS, encoding="utf-8", errors="replace"):
            parts = line.rstrip("\n").split("\t")
            if len(parts) >= 2 and parts[0] and parts[1]:
                soas[parts[0].strip()].add(parts[1].strip())

    db = sqlite3.connect(DB)
    rows = db.execute("SELECT wylie, raw FROM entries").fetchall()
    # link pools over the WHOLE dictionary (HGM entries are the best
    # see-also targets); candidate selection is deterministic
    by_first = defaultdict(list)
    by_skt_tok = defaultdict(list)
    all_entries = {}
    for wylie, raw in rows:
        d = json.loads(raw)
        all_entries[wylie] = d
        syl = wylie.split(" ")[0] if wylie else ""
        if syl and len(by_first[syl]) < 40:
            by_first[syl].append(wylie)
        for t in re.split(r"[^a-zA-Zāīūṛṝḷḹṃḥṅñṭḍṇśṣ]+",
                          (d.get("sanskrit") or "").lower()):
            if len(t) >= 4 and len(by_skt_tok[t]) < 40:
                by_skt_tok[t].append(wylie)

    done = set()
    if os.path.exists(OUT):
        done = set(json.load(open(OUT)).get("entries", {}).keys())
    cands = []
    for wylie, raw in rows:
        d = all_entries[wylie]
        if d.get("hgm_gloss"):
            continue
        if wylie in done and "--refresh" not in sys.argv:
            continue   # already drafted; --refresh regenerates
        skt = (d.get("sanskrit") or "").strip()
        hop = (d.get("hopkins_english") or "").strip()
        if not skt and not hop:
            continue
        # deterministic POS
        pos, pos_src = "", ""
        tags = soas.get(wylie, set())
        if len(tags) == 1:
            pos, pos_src = next(iter(tags)), "SOAS lexicon"
        elif (d.get("tenses") or "").strip():
            pos, pos_src = "verb", "tense evidence"
        # deterministic see-also candidate pool
        rel, seen = [], {wylie}
        first = wylie.split(" ")[0]
        pool = list(by_first.get(first, []))
        for t in re.split(r"[^a-zA-Zāīūṛṝḷḹṃḥṅñṭḍṇśṣ]+",
                          skt.lower()):
            if len(t) >= 4:
                pool += by_skt_tok.get(t, [])
        for w in pool:
            if w in seen:
                continue
            seen.add(w)
            rel.append(w)
            if len(rel) >= 8:
                break
        cands.append({"wylie": wylie, "sanskrit": skt,
                      "hopkins": hop, "pos": pos,
                      "pos_src": pos_src, "related": rel})
        if len(cands) >= limit:
            break
    print(f"{len(cands)} candidates", flush=True)

    existing = {}
    if os.path.exists(OUT):
        existing = json.load(open(OUT)).get("entries", {})
    made = insuff = 0
    for i in range(0, len(cands), BATCH):
        batch = cands[i:i + BATCH]
        lines = []
        for n, c in enumerate(batch):
            block = [f"{n}. headword (wylie): {c['wylie']}"]
            if c["sanskrit"]:
                block.append("   Sanskrit (Lokesh Chandra): " +
                             c["sanskrit"][:140])
            if c["hopkins"]:
                block.append("   Hopkins: " + c["hopkins"][:240])
            if c["related"]:
                block.append("   RELATED: " + "; ".join(
                    f"[{j}] {w}" for j, w in enumerate(c["related"])))
            lines.append("\n".join(block))
        try:
            txt = api("\n\n".join(lines))
            m = re.search(r"\[.*\]", txt, re.S)
            arr = json.loads(m.group(0)) if m else []
        except Exception as e:
            print("  !! batch", i, e, flush=True)
            continue
        for item in arr:
            n = item.get("n")
            if n is None or not (0 <= n < len(batch)):
                continue
            c = batch[n]
            if item.get("insufficient"):
                insuff += 1
                continue
            senses = []
            for sv in item.get("senses", [])[:3]:
                g = (sv.get("gloss") or "").strip()[:160]
                ctx = (sv.get("context") or "").strip()[:400]
                if g:
                    senses.append({"gloss": g, "context": ctx})
            if not senses:
                insuff += 1
                continue
            see = []
            for j in item.get("see_also", [])[:4]:
                if isinstance(j, int) and 0 <= j < len(c["related"]):
                    see.append(c["related"][j])
            grounded = []
            if c["sanskrit"]: grounded.append("LC-sanskrit")
            if c["hopkins"]: grounded.append("hopkins")
            existing[c["wylie"]] = {
                "gloss": senses[0]["gloss"],
                "senses": senses,
                "see_also": see,
                "pos": c["pos"], "pos_src": c["pos_src"],
                "sanskrit": c["sanskrit"][:140],
                "grounded_in": grounded,
                "model": MODEL,
                "date": time.strftime("%Y-%m-%d"),
                "status": "ai-draft",
            }
            made += 1
        print(f"  {min(i+BATCH,len(cands))}/{len(cands)} · "
              f"{made} drafted · {insuff} insufficient", flush=True)
        time.sleep(1)
    os.makedirs(OUTDIR, exist_ok=True)
    json.dump({
        "meta": {
            "tier": "ai-draft — NEVER Geshe Michael's English",
            "method": "deep-entry form: senses+context synthesized "
                      "ONLY from LC/Hopkins comparanda (hgm_evidence "
                      "and corpus English withheld); POS "
                      "deterministic only (SOAS / tense evidence); "
                      "see_also restricted to a computed candidate "
                      "list of real headwords; INSUFFICIENT allowed",
            "model": MODEL,
            "generated": time.strftime("%Y-%m-%d"),
            "count": len(existing),
        },
        "entries": existing,
    }, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"DONE: {made} drafted, {insuff} insufficient "
          f"-> {OUT} ({len(existing)} total)")

if __name__ == "__main__":
    main()
