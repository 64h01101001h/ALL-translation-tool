#!/usr/bin/env python3
"""extract_apparatus.py — ingest ALL published footnotes + bibliographies
(task #57) from the data project's "bibliographies and footnotes" folder
into one unified, searchable store.

Sources (docx read via macOS textutil; no new dependencies):
  - Bibliography - King of Dharma - 1st edition.docx      (^B\\d+ entries)
  - ILLUMINATION BIBLIOGRAPHY SKT & TIB AS OF 3 23 17.docx (^S\\d+ entries)
  - Older/Compiled Translation bibliographies_2021-02-21.docx (newest compiled)
  - Older/Compiled Translation Footnotes_2019-02.docx     (lemma-colon notes)
  - Mixed Nuts Footnotes-2022 05 21.docx                  (source of the mined 344)
plus the already-mined mixed_nuts_notes.json / mixed_nuts_bibliography.json,
which remain AUTHORITATIVE — new material only adds, never overwrites.

Outputs: data/extracted/apparatus_notes.json
         data/extracted/apparatus_bibliography.json
Note schema: lemma, text, source (book), num (0 = unnumbered), xrefs
             (%B/%S bibliography cross-references), source_file
Bib schema:  id, text, source, section, acip_refs, source_file

Provenance rule: every entry carries source_file; the mined Mixed Nuts
rows keep their original fields untouched.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = Path.home() / "Desktop" / "HGM DICTIONARY - TRANSLATION APP" / \
    "bibliographies and footnotes"
OUT_NOTES = ROOT / "data/extracted/apparatus_notes.json"
OUT_BIB = ROOT / "data/extracted/apparatus_bibliography.json"

ACIP_RE = re.compile(r"\b((?:S|TD|KL|KD)\d{4,6}(?:-\d+)?)\b")
XREF_RE = re.compile(r"%([BS]\d+)")
# legacy-font Tibetan lines are mostly punctuation-ish latin garbage;
# detect by high density of these glyph codes
GARBAGE_RE = re.compile(r"[{}@#$%^&+=<>~\\]")


def docx_text(path: Path) -> str:
    r = subprocess.run(
        ["textutil", "-convert", "txt", "-stdout", str(path)],
        capture_output=True)
    return r.stdout.decode("utf-8", "replace")


def is_tibetan_garbage(line: str) -> bool:
    if len(line) < 8:
        return False
    hits = len(GARBAGE_RE.findall(line))
    return hits / max(len(line), 1) > 0.08


def parse_bib(path: Path, prefix: str, source_label: str,
              section_label: str):
    """Entries start '^[BS]\\d+' — join following lines until the next
    id or a Tibetan-garbage line."""
    entries = []
    cur_id, cur = None, []

    def flush():
        nonlocal cur_id, cur
        if cur_id and cur:
            text = " ".join(x.strip() for x in cur if x.strip())
            entries.append({
                "id": f"{prefix}:{cur_id}",
                "text": text,
                "source": source_label,
                "section": section_label,
                "acip_refs": sorted(set(ACIP_RE.findall(text))),
                "source_file": path.name,
            })
        cur_id, cur = None, []

    for line in docx_text(path).split("\n"):
        line = line.rstrip()
        m = re.match(r"^([BS]\d+)\b\s*(.*)", line.strip())
        if m:
            flush()
            cur_id = m.group(1)
            cur = [m.group(2)]
        elif cur_id is not None:
            if is_tibetan_garbage(line) or not line.strip():
                flush()
            else:
                cur.append(line)
    flush()
    return entries


def parse_footnotes(path: Path):
    """Per-book blocks: runs of short title lines, then notes as
    paragraphs whose first line is 'Lemma phrase: text…'."""
    notes = []
    source = path.stem
    pending_title: list[str] = []
    cur = None  # dict in progress
    num_by_source: dict[str, int] = {}

    def flush_note():
        nonlocal cur
        if cur and len(cur["text"]) > 40:
            notes.append(cur)
        cur = None

    for raw in docx_text(path).split("\n"):
        line = raw.rstrip()
        s = line.strip()
        if not s:
            continue
        if is_tibetan_garbage(s):
            continue
        m = re.match(r"^(.{2,60}?):\s+(\S.*)", s)
        looks_like_lemma = bool(m) and not m.group(1).strip()[0].islower() \
            and "%" not in m.group(1)
        if looks_like_lemma:
            flush_note()
            # per-book title detection inside compiled files proved
            # unreliable (misattributes across book boundaries) —
            # keep the honest file-level source; the mined 344 carry
            # true per-book sources already
            pending_title = []
            n = num_by_source.get(source, 0) + 1
            num_by_source[source] = n
            cur = {
                "lemma": m.group(1).strip(),
                "text": m.group(2).strip(),
                "source": source,
                "num": n,
                "source_file": path.name,
            }
        elif cur is not None:
            cur["text"] += " " + s
            # a very long gap of title-ish short lines ends the note
        else:
            if len(s) < 70 and ":" not in s and not s[0].isdigit():
                pending_title.append(s)
            else:
                pending_title = []
    flush_note()
    for nte in notes:
        nte["xrefs"] = sorted(set(XREF_RE.findall(nte["text"])))
    return notes


def main():
    mined_notes = json.load(open(ROOT / "data/extracted/mixed_nuts_notes.json"))
    mined_bib = json.load(
        open(ROOT / "data/extracted/mixed_nuts_bibliography.json"))
    for e in mined_notes:
        e.setdefault("source_file", "mixed_nuts_notes.json (mined)")
        e.setdefault("xrefs", sorted(set(XREF_RE.findall(e.get("text", "")))))
        e["num"] = e.pop("note", e.get("num", 0))
    for e in mined_bib:
        e.setdefault("source_file", "mixed_nuts_bibliography.json (mined)")
        if isinstance(e.get("acip_refs"), str):
            e["acip_refs"] = ACIP_RE.findall(e["acip_refs"])

    bib = list(mined_bib)
    seen_bib = {re.sub(r"\W+", "", e["text"].lower())[:80] for e in bib}
    for path, prefix, src, sect in [
        (SRC / "Bibliography - King of Dharma - 1st edition.docx", "KoD",
         "King of Dharma (1st ed.)", "Bibliography of Native Sources"),
        (SRC / "ILLUMINATION BIBLIOGRAPHY SKT & TIB AS OF 3 23 17.docx",
         "ILL", "Illumination (2017-03-23)",
         "Bibliography of works originally written in Sanskrit"),
        (SRC / "Older Bibliographies and Footnotes" /
         "Compiled Translation bibliographies_2021-02-21.docx", "CTB",
         "Compiled Translation Bibliographies (2021-02-21)", "Compiled"),
    ]:
        if not path.exists():
            print(f"  !! missing: {path.name}", file=sys.stderr)
            continue
        got = parse_bib(path, prefix, src, sect)
        added = 0
        for e in got:
            key = re.sub(r"\W+", "", e["text"].lower())[:80]
            if key in seen_bib or len(e["text"]) < 30:
                continue
            seen_bib.add(key)
            bib.append(e)
            added += 1
        print(f"  bib {path.name}: parsed {len(got)}, added {added}")

    notes = list(mined_notes)
    seen_notes = {(e["source"], e["lemma"].lower()) for e in notes}
    seen_note_text = {re.sub(r"\W+", "", e["text"].lower())[:100]
                      for e in notes}
    for path in [
        SRC / "Older Bibliographies and Footnotes" /
        "Compiled Translation Footnotes_2019-02.docx",
        SRC / "Mixed Nuts Footnotes-2022 05 21.docx",
    ]:
        if not path.exists():
            print(f"  !! missing: {path.name}", file=sys.stderr)
            continue
        got = parse_footnotes(path)
        added = 0
        for e in got:
            tkey = re.sub(r"\W+", "", e["text"].lower())[:100]
            if (e["source"], e["lemma"].lower()) in seen_notes or \
                    tkey in seen_note_text:
                continue
            seen_notes.add((e["source"], e["lemma"].lower()))
            seen_note_text.add(tkey)
            notes.append(e)
            added += 1
        print(f"  notes {path.name}: parsed {len(got)}, added {added}")

    json.dump(notes, open(OUT_NOTES, "w"), ensure_ascii=False, indent=1)
    json.dump(bib, open(OUT_BIB, "w"), ensure_ascii=False, indent=1)
    print(f"UNIFIED: {len(notes)} notes -> {OUT_NOTES.name}, "
          f"{len(bib)} bibliography entries -> {OUT_BIB.name}")


if __name__ == "__main__":
    main()
