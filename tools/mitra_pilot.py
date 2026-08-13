#!/usr/bin/env python3
"""mitra_pilot.py — the guarded MT pilot harness (built 2026-08-13;
survey: docs/research/HF_TIBETAN_MT.md).

Evaluates a local Buddhist-MT model (default: Dharmamitra's
buddhist-nlp/mitra-qwen35-it, Apache 2.0) on segments where we hold
GROUND TRUTH — the aligned corpus — under the project's wall:

  1. NO AUTO-DOWNLOAD. If the model is not already cached locally,
     this prints the exact fetch command and exits. The ~18 GB pull
     is Adam's call ("go MITRA"), never a side effect.
  2. EVERY output is wall-screened (mt_wall_screen n-gram logic)
     against Geshe Michael's published English before it is even
     written to the report — contaminated outputs are shown ONLY as
     flags, never as text suggestions.
  3. Nothing here merges into any data file. The product is an
     evaluation report: Tibetan | model English | HGM English
     (reference) | wall verdict — for human judgment of whether
     the Chodrak-definition lane is worth pursuing.

Usage:
  mitra_pilot.py [--model ID] [--n 20] [--seed-course COURSE]
"""
import argparse, json, os, re, sqlite3, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
DEFAULT_MODEL = "buddhist-nlp/mitra-qwen35-it"
REPORT = "/tmp/mitra_pilot_report.md"


def words(text):
    return re.findall(r"[a-z']+", text.lower())


def build_reference(n=6):
    db = sqlite3.connect(DB)
    grams = set()
    for (eng,) in db.execute(
            "SELECT english FROM corpus_segments "
            "WHERE english != ''"):
        ws = words(eng)
        for i in range(len(ws) - n + 1):
            grams.add(" ".join(ws[i:i + n]))
    return grams


def wall_hits(text, grams, n=6):
    ws = words(text)
    return [" ".join(ws[i:i + n])
            for i in range(len(ws) - n + 1)
            if " ".join(ws[i:i + n]) in grams]


def model_cached(model_id):
    home = os.path.expanduser("~/.cache/huggingface/hub")
    tag = "models--" + model_id.replace("/", "--")
    return os.path.isdir(os.path.join(home, tag))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--model", default=DEFAULT_MODEL)
    ap.add_argument("--n", type=int, default=20)
    ap.add_argument("--seed-course", default="")
    a = ap.parse_args()

    if not model_cached(a.model):
        print(f"model {a.model} is NOT cached locally.")
        print("This harness never downloads by itself (~18 GB).")
        print("When Adam says go:")
        print(f"  pip3 install --break-system-packages "
              f"'transformers>=4.51' torch accelerate")
        print(f"  hf download {a.model}")
        print("then rerun this script.")
        return 2

    try:
        import torch
        from transformers import (AutoModelForCausalLM,
                                  AutoTokenizer)
    except ImportError as ex:
        sys.exit(f"missing python deps: {ex}")

    db = sqlite3.connect(DB)
    q = ("SELECT wylie, english, course FROM corpus_segments "
         "WHERE english != '' AND length(wylie) BETWEEN 40 AND 200 ")
    if a.seed_course:
        q += "AND course = ? ORDER BY seq LIMIT ?"
        rows = db.execute(q, (a.seed_course, a.n)).fetchall()
    else:
        q += "ORDER BY id LIMIT ?"
        rows = db.execute(q, (a.n,)).fetchall()
    if not rows:
        sys.exit("no corpus segments matched")

    print(f"loading {a.model}…", flush=True)
    tok = AutoTokenizer.from_pretrained(a.model)
    model = AutoModelForCausalLM.from_pretrained(
        a.model, torch_dtype=torch.bfloat16, device_map="auto")

    print("building wall reference…", flush=True)
    grams = build_reference()

    out = ["# MITRA pilot — evaluation only, nothing merges\n",
           f"model: {a.model} · {len(rows)} segments · "
           "wall = 6-gram screen vs the full HGM corpus\n"]
    flagged = 0
    for i, (wylie, hgm, course) in enumerate(rows, 1):
        prompt = (f"Translate the following classical Tibetan "
                  f"(Wylie) into English:\n{wylie}\n")
        ids = tok(prompt, return_tensors="pt").to(model.device)
        gen = model.generate(**ids, max_new_tokens=200,
                             do_sample=False)
        mt = tok.decode(gen[0][ids["input_ids"].shape[1]:],
                        skip_special_tokens=True).strip()
        hits = wall_hits(mt, grams)
        out.append(f"\n## {i}. ({course})\n")
        out.append(f"- Tibetan: `{wylie}`\n")
        if hits:
            flagged += 1
            out.append(f"- model: **WALL-FLAGGED** — {len(hits)} "
                       f"shared run(s), first: “{hits[0]}” — "
                       "output withheld from this report\n")
        else:
            out.append(f"- model (wall-clean): {mt}\n")
        out.append(f"- HGM (reference, the standard): {hgm}\n")
        print(f"  {i}/{len(rows)}"
              f"{' FLAGGED' if hits else ''}", flush=True)

    out.append(f"\n---\n{flagged}/{len(rows)} outputs "
               "wall-flagged (withheld).\n")
    open(REPORT, "w").write("".join(out))
    print(f"report -> {REPORT}")


if __name__ == "__main__":
    main()
