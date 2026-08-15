#!/usr/bin/env python3
"""extract_older_apparatus.py — mine the OLDER compiled footnote
collections (2018-2021) into a second STD-008 bank.

Source: ~/Desktop/HGM DICTIONARY - TRANSLATION APP/
        bibliographies and footnotes/Older Bibliographies and Footnotes/

Format differs from the recent GMR volumes: notes are UN-numbered
"Lemma phrase: note body" paragraphs grouped under multi-line book
title headings. Book headings are detected as runs of short lines
(no "Lemma:" shape); the run's text joined = the source title.

Output: data/extracted/mixed_nuts_notes_older.json — same row shape
as the main bank ({source, note, lemma, text}) with note numbers
assigned per-source in reading order and the compilation file +
book title carried in `source` for provenance. Rows whose
(lemma, first-120-chars) already exist in the MAIN bank are dropped
as duplicates (counted). Nothing is paraphrased or synthesized —
verbatim extraction only, per STD-008 (reuse performed work).
"""
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/"
    "bibliographies and footnotes/Older Bibliographies and Footnotes")
OUT = os.path.join(HERE, "data", "extracted",
                   "mixed_nuts_notes_older.json")
MAIN = os.path.join(HERE, "data", "extracted", "mixed_nuts_notes.json")

# only the footnote compilations; the bibliography compilations have
# their own shapes and ride in a later pass if wanted
FOOTNOTE_FILES = [
    "Compiled Translation Footnotes_2018-05.docx",
    "Compiled Translation Footnotes_2019-02.docx",
]

NOTE_RE = re.compile(r"^(.{2,80}?):\s+(.{10,})$", re.S)
ROMAN_RE = re.compile(r"^[ivxlcdm]{1,8}\s+", re.I)


def main() -> int:
    main_seen = set()
    try:
        for x in json.load(open(MAIN)):
            main_seen.add((x["lemma"].lower(), x["text"][:120]))
    except FileNotFoundError:
        pass

    notes = []
    dropped_dupes = 0
    for fname in FOOTNOTE_FILES:
        path = os.path.join(SRC, fname)
        if not os.path.exists(path):
            print("MISSING:", fname)
            continue
        txt = subprocess.run(
            ["pandoc", "-t", "plain", "--wrap=none", path],
            capture_output=True, text=True, check=True).stdout
        txt = txt.replace("‘", "'").replace("’", "'")
        title_run = []
        # the 2018 compilation opens with roman-marked notes and no
        # book heading at all — label honestly, never infer a title
        cur_title = f"(untitled opening section, {fname.split('.')[0]})"
        n_in_src = 0
        year = re.search(r"_(\d{4})-", fname).group(1)
        # roman note-markers sometimes glue two notes into one
        # paragraph in the plain export — split at the markers
        txt = re.sub(r"\s(?=[ivxlcdm]{1,6}\s+[A-Z][^:\n]{2,60}:\s)",
                     "\n\n", txt)
        for par in re.split(r"\n\s*\n", txt):
            p = re.sub(r"\s+", " ", par).strip()
            if not p:
                continue
            # page-number roman numerals glue onto the following
            # word in the plain export — strip the artifact
            p = ROMAN_RE.sub("", p)
            m = NOTE_RE.match(p)
            # heading heuristic: a short colon-less line that LOOKS
            # like a title — starts uppercase, letters/quotes only,
            # never verse-comma endings, no citation fragments
            if (len(p) < 60 and ":" not in p and
                    re.match(r'^["\u201c]?[A-Z][A-Za-z\'\u2019 \-&,]*[a-z"\u201d]$', p) and
                    not p.endswith((",", ";")) and
                    "%" not in p and ")" not in p):
                title_run.append(p.strip('"\u201c\u201d'))
                continue
            # anything else short and colon-less (verse lines,
            # spilled citation tails) breaks a pending title run
            if len(p) < 60 and ":" not in p:
                title_run = []
                continue
            # a colon-ended short line continues a pending title
            # ("Deathless Nectar / For Helping Others:")
            if (title_run and len(p) < 60 and p.endswith(":") and
                    not NOTE_RE.match(p)):
                title_run.append(p.rstrip(":").strip('"\u201c\u201d'))
                continue
            if title_run:
                # a SINGLE short line is a quoted root-text verse
                # marker, not a book heading — notes stay with the
                # current book; only multi-line runs open a source
                if len(title_run) >= 2:
                    cur_title = " — ".join(
                        t for t in title_run if t) or cur_title
                    n_in_src = 0
                title_run = []
            if not m:
                continue
            lemma, body = m.group(1).strip(), m.group(2).strip()
            # refuse pseudo-lemmas that are clearly sentence text
            if lemma.count(" ") > 9 or lemma.endswith((".", "!", "?")):
                continue
            key = (lemma.lower(), body[:120])
            if key in main_seen:
                dropped_dupes += 1
                continue
            main_seen.add(key)
            n_in_src += 1
            notes.append({
                "source": f"{cur_title} (compiled {year})",
                "note": n_in_src,
                "lemma": lemma,
                "text": body,
            })
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        json.dump(notes, f, ensure_ascii=False, indent=1)
    per = {}
    for x in notes:
        per[x["source"]] = per.get(x["source"], 0) + 1
    for k in sorted(per, key=per.get, reverse=True)[:12]:
        print(f"{per[k]:5d}  {k}")
    print(f"sources: {len(per)} · notes: {len(notes)} · "
          f"duplicates vs main bank dropped: {dropped_dupes}")
    print("->", OUT)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
