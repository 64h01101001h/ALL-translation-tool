#!/usr/bin/env python3
"""Add PHONETIC author aliases to data/extracted/author_index.json.

Why. The demo audit (2026-08-23) found that "Gyaltsab Je", "Milarepa",
"Nagarjuna" and "Khedrup" all return "No person of that name" — the
four names a visitor is most likely to type. Every alias in the index
is a Wylie-caps variant (RGYAL TSAB DAR MA RIN CHEN, RGYAL TSAB RJE DAR
MA …), so a phonetic query matches nothing.

The missing half was already on disk. The Sungbum author folders are
named

    RGYAL TSAB DAR MA RIN CHEN_GYALTSAB DARMA RINCHEN (1364-1432)
    ^ wylie                    ^ phonetic              ^ dates

so the phonetic reading, the one a human actually types, is sitting
right there after the underscore. This harvests it and files it as an
alias against the person whose Wylie form matches.

Idempotent: run it as often as you like. It only ever ADDS aliases, and
records what it did in _meta so the provenance survives (rule 4).

    python3 tools/add_phonetic_aliases.py [--dry-run]
"""
import io
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INDEX = os.path.join(ROOT, "data/extracted/author_index.json")
SUNGBUM = os.path.join(ROOT, "library/sungbum")

# "WYLIE CAPS_PHONETIC NAME (1364-1432)"  — dates optional
PAT = re.compile(r"^(?P<wylie>[^_]+)_(?P<phon>[^(]+?)\s*(?:\((?P<dates>[^)]*)\))?$")


def norm(s):
    """Fold for matching only: case, punctuation and spacing."""
    s = s.upper()
    s = re.sub(r"[^A-Z0-9' ]+", " ", s)
    return " ".join(s.split())


def harvest():
    """folder-name → (wylie, phonetic, dates), deduped."""
    found = {}
    for dirpath, dirnames, _ in os.walk(SUNGBUM):
        for d in dirnames:
            m = PAT.match(d)
            if not m:
                continue
            wylie = m.group("wylie").strip()
            phon = m.group("phon").strip()
            # the phonetic half must look like a name, not a subject
            # label: subject folders are "BSTOD PA_EULOGIES (STOTRA)"
            # and their second half is a single English word or two
            if not wylie or not phon or len(phon) < 4:
                continue
            if norm(phon) == norm(wylie):
                continue
            found.setdefault(norm(wylie), (wylie, phon, m.group("dates")))
    return found


def main():
    dry = "--dry-run" in sys.argv
    idx = json.load(io.open(INDEX, encoding="utf-8"))
    people = idx["people"]
    harvested = harvest()
    print("author folders yielding a phonetic half: %d" % len(harvested))

    added = 0
    touched = 0
    examples = []
    for pid, rec in people.items():
        keys = {norm(a) for a in (rec.get("aliases") or [])
                if isinstance(a, str)}
        for nm in (rec.get("display"), rec.get("name_recorded")):
            # display/name_recorded are sometimes bool (the index uses
            # them as flags on some records), so type-check rather than
            # truth-check
            if isinstance(nm, str) and nm:
                keys.add(norm(nm))
        hit = None
        for k in keys:
            if k in harvested:
                hit = harvested[k]
                break
        if not hit:
            continue
        wylie, phon, dates = hit
        al = rec.setdefault("aliases", [])
        if any(norm(a) == norm(phon) for a in al):
            continue
        al.append(phon)
        added += 1
        touched += 1
        if len(examples) < 6:
            examples.append("%s  %s  ->  %s" % (pid, wylie[:34], phon))

    print("people gaining a phonetic alias: %d" % touched)
    for e in examples:
        print("   " + e)

    if dry:
        print("(dry run — nothing written)")
        return 0

    idx.setdefault("_meta", {})["phonetic_aliases"] = {
        "added": added,
        "source": "library/sungbum author folder names, the half after "
                  "the underscore",
        "why": "Gyaltsab Je / Milarepa / Nagarjuna / Khedrup all returned "
               "'No person of that name' — every alias was Wylie-caps",
        "tool": "tools/add_phonetic_aliases.py",
        "date": "2026-08-23",
    }
    json.dump(idx, io.open(INDEX, "w", encoding="utf-8"),
              ensure_ascii=False, indent=1)
    print("wrote %s" % INDEX)
    return 0


if __name__ == "__main__":
    sys.exit(main())
