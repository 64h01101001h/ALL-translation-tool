#!/usr/bin/env python3
"""extract_das_outline.py — Das (1902) syllable-onset -> PDF page map
(task #61), from Adam's own bookmarked PDF (HyperContexting
Materials). 822 bookmarks: wylie onset + Tibetan script + page.
Self-owned route — no third-party index needed. Output:
data/extracted/das_pages.json {onset_wylie, tibetan, pdf_page}[]."""
import json, os, re, sys
import pypdf

PDF = ("/Users/adamderickandrade/Asian Classics Dropbox/adam andrade/"
       "HyperContexting Materials/Dictionaries_n_Glossaries_n_Grammar/"
       "Tibetan-English-Dictionary_Chandra Das-bookmarks.pdf")
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "data/extracted/das_pages.json")

r = pypdf.PdfReader(PDF)
rows = []
def walk(o):
    for it in o:
        if isinstance(it, list):
            walk(it)
        else:
            t = (it.title or "").strip()
            m = re.match(r"^([a-zA-Z'’\-]+)[\s·]*([ༀ-࿿]*)", t)
            if not m: continue
            try:
                pg = r.get_destination_page_number(it)
            except Exception:
                continue
            rows.append({"onset": m.group(1).lower().replace("’", "'"),
                         "tibetan": m.group(2),
                         "pdf_page": pg + 1})
walk(r.outline)
rows = [x for x in rows if x["onset"] not in
        ("alphabetical", "grammatical")]
json.dump({"meta": {"source": os.path.basename(PDF),
                    "pages": len(r.pages),
                    "note": "syllable-onset -> 1-based PDF page; "
                            "Das 1902 is public domain; the "
                            "bookmarks are the team's own"},
           "sections": rows}, open(OUT, "w"), ensure_ascii=False,
          indent=0)
print(len(rows), "sections ->", OUT)
