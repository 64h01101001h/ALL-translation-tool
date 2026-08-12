#!/usr/bin/env python3
"""build_ai_glossary.py — the ALL Working Glossary (AI-provisional
tier; Adam's proposal, 2026-08-12).

THE WALL (inviolable): AI never touches Geshe Michael's English.
- Candidates are ONLY entries with NO hgm_gloss.
- The prompt receives ONLY reference comparanda (Lokesh Chandra
  Sanskrit, Hopkins English) — hgm_evidence and all corpus English
  are deliberately withheld, so the model cannot paraphrase Geshe
  Michael into an entry.
- Output is stored in its OWN file, its own tier ("ai-draft"),
  displayed only under an unmistakable banner, and suppressed the
  moment a term gains a real HGM gloss.
- The model must answer INSUFFICIENT when comparanda are too thin
  (rule 3: flagged, never guessed).

Usage: build_ai_glossary.py [--limit N]   (default pilot: 50)
Requires ANTHROPIC_API_KEY in the environment (Adam's key, his
standing authorization).
"""
import json, os, re, sqlite3, sys, time, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUTDIR = os.path.join(ROOT, "data", "ai_glossary")
OUT = os.path.join(OUTDIR, "ai_glossary.json")
MODEL = "claude-opus-5"   # the app's own analysis model
BATCH = 10

SYSTEM = """You are a careful Tibetan-lexicography assistant for the \
Asian Legacy Library. For each numbered headword you receive reference \
comparanda: a Sanskrit equivalent (Lokesh Chandra's Tibetan-Sanskrit \
dictionary) and/or an English gloss from the Hopkins dictionary. \
Write ONE concise English working gloss per headword (a few words, \
dictionary style), synthesized ONLY from the given comparanda. Rules: \
(1) Use ONLY the material given — no outside knowledge of how any \
particular translator renders the term. (2) If the comparanda are too \
thin or contradictory to support a gloss, answer exactly INSUFFICIENT. \
(3) Output strict JSON: an array of {"n": <number>, "gloss": \
"<gloss or INSUFFICIENT>"} — nothing else."""

def api(messages):
    key = os.environ.get("ANTHROPIC_API_KEY")
    if not key:
        sys.exit("ANTHROPIC_API_KEY not set")
    req = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps({
            "model": MODEL,
            "max_tokens": 1500,
            "system": SYSTEM,
            "messages": messages,
        }).encode(),
        headers={
            "x-api-key": key,
            "anthropic-version": "2023-06-01",
            "content-type": "application/json",
        })
    with urllib.request.urlopen(req, timeout=120) as r:
        d = json.load(r)
    return "".join(b.get("text", "") for b in d.get("content", []))

def main():
    limit = 50
    if "--limit" in sys.argv:
        limit = int(sys.argv[sys.argv.index("--limit") + 1])
    db = sqlite3.connect(DB)
    rows = db.execute(
        "SELECT wylie, raw FROM entries "
        "WHERE (hgm_gloss IS NULL OR hgm_gloss='') "
        "ORDER BY wylie").fetchall()
    cands = []
    for wylie, raw in rows:
        d = json.loads(raw)
        if d.get("hgm_gloss"):
            continue   # belt and braces: the wall
        skt = (d.get("sanskrit") or "").strip()
        hop = (d.get("hopkins_english") or "").strip()
        if not skt and not hop:
            continue
        cands.append({"wylie": wylie, "sanskrit": skt,
                      "hopkins": hop})
        if len(cands) >= limit:
            break
    print(f"{len(cands)} candidates (of {len(rows)} HGM-less entries)")

    existing = {}
    if os.path.exists(OUT):
        existing = json.load(open(OUT)).get("entries", {})
    made, insuff = 0, 0
    for i in range(0, len(cands), BATCH):
        batch = cands[i:i + BATCH]
        lines = []
        for n, c in enumerate(batch):
            ref = []
            if c["sanskrit"]:
                ref.append("Sanskrit (Lokesh Chandra): " +
                           c["sanskrit"][:120])
            if c["hopkins"]:
                ref.append("Hopkins: " + c["hopkins"][:200])
            lines.append(f'{n}. headword (wylie): {c["wylie"]}\n   ' +
                         "\n   ".join(ref))
        try:
            txt = api([{"role": "user",
                        "content": "\n".join(lines)}])
            m = re.search(r"\[.*\]", txt, re.S)
            arr = json.loads(m.group(0)) if m else []
        except Exception as e:
            print("  !! batch", i, e)
            continue
        for item in arr:
            n = item.get("n")
            g = (item.get("gloss") or "").strip()
            if n is None or not (0 <= n < len(batch)):
                continue
            c = batch[n]
            if g and g != "INSUFFICIENT":
                grounded = []
                if c["sanskrit"]: grounded.append("LC-sanskrit")
                if c["hopkins"]: grounded.append("hopkins")
                existing[c["wylie"]] = {
                    "gloss": g[:200],
                    "grounded_in": grounded,
                    "model": MODEL,
                    "date": time.strftime("%Y-%m-%d"),
                    "status": "ai-draft",
                }
                made += 1
            else:
                insuff += 1
        print(f"  {i+len(batch)}/{len(cands)} · {made} drafted · "
              f"{insuff} insufficient", flush=True)
        time.sleep(1)
    os.makedirs(OUTDIR, exist_ok=True)
    json.dump({
        "meta": {
            "tier": "ai-draft — NEVER Geshe Michael's English",
            "method": "synthesized ONLY from reference comparanda "
                      "(LC Sanskrit, Hopkins); hgm_evidence and all "
                      "corpus English withheld from the prompt; "
                      "INSUFFICIENT allowed; suppressed wherever a "
                      "real HGM gloss exists",
            "model": MODEL,
            "generated": time.strftime("%Y-%m-%d"),
            "count": len(existing),
        },
        "entries": existing,
    }, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"DONE: {made} new drafts, {insuff} insufficient "
          f"-> {OUT} ({len(existing)} total)")

if __name__ == "__main__":
    main()
