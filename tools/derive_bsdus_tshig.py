#!/usr/bin/env python3
"""Derive the bsdus-tshig (syllabic contraction) register from the HGM
dictionary itself — attestation only, never invention (Adam's request,
2026-08-07: a register of the abbreviations Wilson's OM dot denotes).

A pair (short, long) enters the register only when ALL of:
  1. BOTH forms are dictionary entries WITH HGM glosses;
  2. the short's syllables appear in the long, in order, first syllable
     identical (Wilson's contraction pattern — rnam shes <- rnam par
     shes pa);
  3. every dropped syllable is grammatically droppable (case particles,
     par/bar, nominal endings pa/ba/po/mo/ma) — the "clean" class — OR
     the drop set includes trailing content syllables, in which case the
     pair is admitted only on EXACT gloss-alternative equality and
     flagged "end-truncated" (dge bshes <- dge ba'i bshes gnyen);
  4. the glosses agree (shared alternative, or one contains the other,
     >= 5 chars).
Corpus counts for both forms ride along as evidence. Everything is
DERIVED tier: the register document is for human review; the app labels
any use of it as such.

    python3 tools/derive_bsdus_tshig.py

Reads  build/hgm_spine_v27_2.db
Writes data/extracted/bsdus_tshig.tsv  (app input; regenerate per release)
       docs/BSDUS_TSHIG_REGISTER.md    (the human review register)
"""
import json
import re
import sqlite3
import sys

REPO = __file__.rsplit("/", 2)[0]

DROPPABLE = {
    # case/semi particles and connectives Wilson's contractions drop
    "par", "bar", "kyi", "gyi", "gi", "yi", "'i", "kyis", "gyis", "gis",
    "yis", "tu", "du", "su", "ru", "la", "na", "nas", "las", "cing",
    "zhing", "shing", "ste", "te", "dang",
    # nominal endings
    "pa", "ba", "po", "bo", "mo", "ma", "pa'i", "ba'i", "po'i", "mo'i",
}


def norm_glosses(raw):
    """hgm_gloss holds a JSON-ish list; normalize each alternative."""
    if not raw:
        return []
    try:
        alts = json.loads(raw)
        if not isinstance(alts, list):
            alts = [str(alts)]
    except Exception:
        alts = [raw]
    out = []
    for a in alts:
        a = re.sub(r"\([^)]*\)", " ", str(a).lower())
        a = re.sub(r"[^a-z' ]+", " ", a)
        a = " ".join(a.split())
        if a:
            out.append(a)
    return out


def gloss_match(a_alts, b_alts):
    """(kind, evidence) — 'exact' shared alternative, 'overlap' containment."""
    for a in a_alts:
        for b in b_alts:
            if a == b:
                return "exact", a
    for a in a_alts:
        for b in b_alts:
            if len(a) >= 5 and len(b) >= 5 and (a in b or b in a):
                return "overlap", a if a in b else b
    return None, None


def main() -> int:
    con = sqlite3.connect(f"{REPO}/build/hgm_spine_v27_2.db")
    rows = con.execute(
        "SELECT wylie, hgm_gloss FROM entries "
        "WHERE hgm_gloss IS NOT NULL AND hgm_gloss != '' AND wylie != ''"
    ).fetchall()

    # index glossed entries; collapse duplicate wylies (union the glosses)
    glossed = {}
    for wylie, g in rows:
        alts = norm_glosses(g)
        if not alts:
            continue
        glossed.setdefault(wylie, set()).update(alts)

    by_first = {}
    for w in glossed:
        syls = w.split()
        if 3 <= len(syls) <= 6:
            by_first.setdefault(syls[0], []).append((w, syls))

    def corpus_count(phrase):
        try:
            n = con.execute(
                "SELECT count(*) FROM corpus_fts WHERE corpus_fts MATCH ?",
                ('"' + phrase + '"',)).fetchone()[0]
            return n
        except Exception:
            try:
                return con.execute(
                    "SELECT count(*) FROM corpus_segments WHERE wylie LIKE ?",
                    ("%" + phrase + "%",)).fetchone()[0]
            except Exception:
                return -1

    results = []
    for short, s_alts in glossed.items():
        s_syls = short.split()
        if len(s_syls) != 2 or s_syls[0] == s_syls[1]:
            continue
        s1, s2 = s_syls
        if s2 in DROPPABLE:  # 'X pa' style is no contraction
            continue
        for long, l_syls in by_first.get(s1, []):
            if long == short:
                continue
            try:
                k = l_syls.index(s2, 1)
            except ValueError:
                continue
            mid = l_syls[1:k]
            tail = l_syls[k + 1:]
            # a true syllabic contraction drops something BETWEEN the
            # retained syllables — that omission is what the OM dot marks
            # (Wilson p.569). Tail-only drops are just a word plus its
            # ending/particle (ma yin vs ma yin pa) — excluded.
            if not mid:
                continue
            if not all(s in DROPPABLE for s in mid):
                continue
            tail_clean = all(s in DROPPABLE for s in tail)
            kind, evidence = gloss_match(sorted(s_alts),
                                         sorted(glossed[long]))
            if kind is None:
                continue
            if tail_clean:
                klass = "clean"
            elif kind == "exact" and len(tail) <= 2:
                klass = "end-truncated"
            else:
                continue
            results.append({
                "short": short, "long": long,
                "dropped": " ".join(mid + tail) or "-",
                "class": klass, "gloss_kind": kind,
                "gloss": evidence,
                "short_n": corpus_count(short),
                "long_n": corpus_count(long),
            })

    # rank: clean+exact first, then by combined corpus evidence
    order = {"clean": 0, "end-truncated": 1}
    results.sort(key=lambda r: (order[r["class"]],
                                0 if r["gloss_kind"] == "exact" else 1,
                                -(max(r["short_n"], 0) + max(r["long_n"], 0)),
                                r["short"]))

    tsv = f"{REPO}/data/extracted/bsdus_tshig.tsv"
    with open(tsv, "w", encoding="utf-8") as f:
        for r in results:
            f.write("\t".join([r["short"], r["long"], r["dropped"],
                               r["class"], r["gloss_kind"], r["gloss"],
                               str(r["short_n"]), str(r["long_n"])]) + "\n")

    md = f"{REPO}/docs/BSDUS_TSHIG_REGISTER.md"
    with open(md, "w", encoding="utf-8") as f:
        f.write(
"""# The bsdus-tshig register — syllabic contractions (DERIVED, for review)

The abbreviations Wilson's **OM dot** denotes: a surviving dot inside a
contracted form standing where syllables were omitted (App. 2, p. 569:
རྣམ་ཤེས for རྣམ་པར་ཤེས་པ). Requested by Adam 2026-08-07.

**How this register is derived — attestation only, never invention:**
every pair below has BOTH forms as HGM dictionary entries WITH glosses;
the contraction follows Wilson's drop pattern (particles and nominal
endings omitted — or, in the *end-truncated* class, a trailing content
word, admitted only on exact gloss equality); and the two entries'
glosses agree. Corpus counts are shown as further evidence. Regenerate
per release: `python3 tools/derive_bsdus_tshig.py`.

**Tier: DERIVED (provisional).** This is a review document — nothing
here is asserted as HGM's own judgment; in the app every use is labeled
"derived register".

| short | full form | dropped | class | gloss evidence | corpus (short/long) |
|---|---|---|---|---|---|
""")
        for r in results:
            f.write("| {short} | {long} | {dropped} | {klass} | {gk}: "
                    "“{g}” | {sn} / {ln} |\n".format(
                        short=r["short"], long=r["long"],
                        dropped=r["dropped"], klass=r["class"],
                        gk=r["gloss_kind"], g=r["gloss"],
                        sn=r["short_n"], ln=r["long_n"]))
        f.write(f"\n{len(results)} pairs derived.\n")

    print(f"{len(results)} pairs -> {tsv} + {md}")
    kinds = {}
    for r in results:
        kinds[(r['class'], r['gloss_kind'])] = kinds.get(
            (r['class'], r['gloss_kind']), 0) + 1
    for k, n in sorted(kinds.items()):
        print(" ", k, n)
    return 0


if __name__ == "__main__":
    sys.exit(main())
