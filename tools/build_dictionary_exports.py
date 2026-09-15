#!/usr/bin/env python3
"""Write the machine-readable exports of the new dictionary.

Adam, 2026-09-09: "from now on, we need to generate the dictionary and have
it output in a .csv file, with ALL of the available metadata associated with
each entry", and "the .csv should be generated anytime the new dictionary is
updated."

This module is imported by tools/build_dictionary_view.py and runs from the
SAME assembled entries the published page renders, so the CSV and the page
cannot disagree about what the dictionary says. Running it standalone
rebuilds the page too, for the same reason.

Four files, each answering a different question:

  geshe_michael_roach_dictionary.csv          one row per rendering. The deliverable:
                                  everything the layer knows about an entry,
                                  provenance included, so a lone CSV on
                                  someone else's desk still says what tier it
                                  is and where its English came from.
  geshe_michael_roach_dictionary_reverse.csv  English -> Tibetan. "Has Geshe Michael rendered this
                                  word before, and as what?" is the question
                                  an editor actually asks, and the page
                                  cannot answer it.
  geshe_michael_roach_dictionary_by_course.csv  the glossary of one course, ranked by how
                                  often each term occurs in it. What a
                                  student of C01 needs and no one has.
  geshe_michael_roach_dictionary_changes.csv  what is new since the previous export.
                                  Written only when a previous CSV exists.
                                  Nobody re-reads six thousand headwords; a
                                  reviewer reads the delta.

Nothing here composes English. Every rendering is HGM's own corpus text,
machine-MATCHED, and every row carries the tier that says so.
"""
import collections
import csv
import io
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTDIR = os.path.join(ROOT, "docs")
sys.path.insert(0, os.path.join(ROOT, "engines"))

try:
    import pron_engine as _P
except Exception:
    _P = None
try:
    from ewts_unicode import wylie_to_unicode as _U
except Exception:
    _U = None

_PRON, _TIB = {}, {}


def pron(t):
    if t not in _PRON:
        try:
            _PRON[t] = _P.pronounce(t) if _P else ""
        except Exception:
            _PRON[t] = ""
    return _PRON[t]


def tibetan(t):
    if t not in _TIB:
        try:
            u, ok = _U(t) if _U else ("", False)
            _TIB[t] = u if ok else ""
        except Exception:
            _TIB[t] = ""
    return _TIB[t]


def _rows(depths, phonetics, meta, acip_by_wylie=None):
    """One row per (headword, rendering, depth). Phonetic transcriptions are
    included and LABELLED, never mixed in unlabelled: they are real data and
    they are not what the Tibetan means."""
    for kind, groups in (("translation", depths.items()),
                         ("phonetic transcription", [("", phonetics)])):
        for d, ents in groups:
            for tib, acip, rl in ents:
                # The view passes what acip_index.get(wylie) returned, and that
                # index is keyed ACIP -> wylie, so it has always returned
                # nothing: the published page's ACIP column is empty for every
                # entry (found 2026-09-09). Look it up the right way round.
                acip = acip or (acip_by_wylie or {}).get(tib, "")
                for eng, refs in rl:
                    courses = sorted({r.split(":")[0] for r in refs})
                    yield {
                        "wylie": tib,
                        "acip": acip,
                        # GENERATED, and named so. The layer banks wylie and
                        # English; script and phonetics are computed here by
                        # the project's own engines. A reader must never take
                        # a computed column for an attested one — and a few
                        # headwords in the layer are English front matter,
                        # where a computed "Tibetan" is meaningless.
                        "tibetan_generated": tibetan(tib),
                        "pronunciation_generated": pron(tib),
                        "in_acip_index": "yes" if (acip_by_wylie or {}).get(tib) else "no",
                        "english": eng,
                        "kind": kind,
                        "depth": d,
                        "occurrences": len(refs),
                        "courses": " ".join(courses),
                        "n_courses": len(courses),
                        "refs": " ".join(refs),
                        "first_ref": refs[0] if refs else "",
                        "tier": meta.get("tier", ""),
                        "rule": meta.get("rule", ""),
                        "source_corpus": meta.get("source_corpus", ""),
                        "source_pages": meta.get("source_pages", ""),
                        "generator": meta.get("generator", ""),
                        "layer_date": meta.get("date", ""),
                        "generated_columns":
                            "tibetan_generated = generated-ewts-v27-1; "
                            "pronunciation_generated = generated GMR-convention "
                            "phonetics. Both computed from the wylie, not "
                            "attested in the layer.",
                    }


FIELDS = ["wylie", "acip", "tibetan_generated", "pronunciation_generated",
          "in_acip_index", "english", "kind", "depth", "occurrences",
          "courses", "n_courses", "refs", "first_ref", "tier", "rule",
          "generated_columns", "source_corpus", "source_pages", "generator",
          "layer_date"]


def _write(path, fields, rows):
    # utf-8-sig: Excel on Windows reads a plain UTF-8 CSV as Latin-1 and turns
    # every Tibetan and every diacritic into mojibake. The BOM is what makes
    # it open correctly for the people this is being sent to.
    with io.open(path, "w", encoding="utf-8-sig", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fields, quoting=csv.QUOTE_MINIMAL)
        w.writeheader()
        n = 0
        for r in rows:
            w.writerow(r)
            n += 1
    return n


def write_exports(depths, phonetics, meta, acip_index=None):
    # acip_index is ACIP -> wylie; invert it once for the lookup
    by_wylie = {v: k for k, v in (acip_index or {}).items()}
    rows = list(_rows(depths, phonetics, meta, by_wylie))

    main_path = os.path.join(OUTDIR, "geshe_michael_roach_dictionary.csv")
    prev = {}
    if os.path.exists(main_path):
        with io.open(main_path, encoding="utf-8-sig", newline="") as f:
            for r in csv.DictReader(f):
                prev[(r.get("wylie"), r.get("english"), r.get("depth"))] = r

    n_main = _write(main_path, FIELDS, rows)

    # English -> Tibetan
    rev = collections.defaultdict(list)
    for r in rows:
        if r["kind"] == "translation":
            rev[r["english"]].append(r)
    rev_rows = []
    for eng in sorted(rev, key=lambda e: (-sum(x["occurrences"] for x in rev[e]),
                                          e.lower())):
        for r in sorted(rev[eng], key=lambda x: -x["occurrences"]):
            rev_rows.append({"english": eng, "wylie": r["wylie"],
                             "acip": r["acip"], "tibetan_generated": r["tibetan_generated"],
                             "pronunciation_generated": r["pronunciation_generated"],
                             "depth": r["depth"],
                             "occurrences": r["occurrences"],
                             "courses": r["courses"], "refs": r["refs"],
                             "tier": r["tier"]})
    n_rev = _write(os.path.join(OUTDIR, "geshe_michael_roach_dictionary_reverse.csv"),
                   ["english", "wylie", "acip", "tibetan_generated",
                    "pronunciation_generated", "depth", "occurrences",
                    "courses", "refs", "tier"],
                   rev_rows)

    # per course, ranked by occurrences in THAT course
    per = collections.defaultdict(lambda: collections.defaultdict(int))
    info = {}
    for r in rows:
        if r["kind"] != "translation":
            continue
        for ref in r["refs"].split():
            co = ref.split(":")[0]
            per[co][(r["wylie"], r["english"])] += 1
            info[(r["wylie"], r["english"])] = r
    course_rows = []
    for co in sorted(per):
        for (t, e), n in sorted(per[co].items(), key=lambda kv: (-kv[1],
                                                                kv[0][0].lower())):
            r = info[(t, e)]
            course_rows.append({"course": co, "wylie": t, "acip": r["acip"],
                                "tibetan_generated": r["tibetan_generated"],
                                "pronunciation_generated": r["pronunciation_generated"],
                                "english": e, "occurrences_in_course": n,
                                "depth": r["depth"], "tier": r["tier"]})
    n_course = _write(os.path.join(OUTDIR, "geshe_michael_roach_dictionary_by_course.csv"),
                      ["course", "wylie", "acip", "tibetan_generated",
                       "pronunciation_generated", "english",
                       "occurrences_in_course", "depth", "tier"],
                      course_rows)

    # what changed since the previous export
    n_chg = None
    if prev:
        seen_words = {k[0] for k in prev}
        chg = []
        for r in rows:
            key = (r["wylie"], r["english"], str(r["depth"]))
            if key in prev:
                continue
            chg.append({"change": "new headword" if r["wylie"] not in seen_words
                        else "new rendering",
                        "wylie": r["wylie"], "acip": r["acip"],
                        "tibetan_generated": r["tibetan_generated"], "english": r["english"],
                        "kind": r["kind"], "depth": r["depth"],
                        "occurrences": r["occurrences"],
                        "courses": r["courses"], "refs": r["refs"]})
        cur = {(r["wylie"], r["english"], str(r["depth"])) for r in rows}
        for k, r in prev.items():
            if k not in cur:
                chg.append({"change": "withdrawn", "wylie": r.get("wylie", ""),
                            "acip": r.get("acip", ""),
                            "tibetan_generated": r.get("tibetan_generated", ""),
                            "english": r.get("english", ""),
                            "kind": r.get("kind", ""), "depth": r.get("depth", ""),
                            "occurrences": r.get("occurrences", ""),
                            "courses": r.get("courses", ""),
                            "refs": r.get("refs", "")})
        n_chg = _write(os.path.join(OUTDIR, "geshe_michael_roach_dictionary_changes.csv"),
                       ["change", "wylie", "acip", "tibetan_generated", "english", "kind",
                        "depth", "occurrences", "courses", "refs"], chg)

    sys.stderr.write("  csv  %6d rows  docs/geshe_michael_roach_dictionary.csv\n" % n_main)
    sys.stderr.write("  csv  %6d rows  docs/geshe_michael_roach_dictionary_reverse.csv\n" % n_rev)
    sys.stderr.write("  csv  %6d rows  docs/geshe_michael_roach_dictionary_by_course.csv\n" % n_course)
    if n_chg is not None:
        sys.stderr.write("  csv  %6d rows  docs/geshe_michael_roach_dictionary_changes.csv "
                         "(vs the previous export)\n" % n_chg)
    else:
        sys.stderr.write("  csv         no previous export, so no change report "
                         "this time\n")
    return n_main


if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import build_dictionary_view
    build_dictionary_view.main()
