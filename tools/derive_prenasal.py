#!/usr/bin/env python3
"""derive_prenasal.py — the b(s)-coda prenasal class (HGM's kamdir
rule), derived with SYLLABLE ALIGNMENT and the word-internal condition.

Supersedes the two ad-hoc passes of 2026-08-08, whose site-count guard
could transform a cluster at the wrong juncture (yang dag 'das has one
wylie juncture and one g·d cluster — but they are different places).

THE FOUR PRENASAL SYSTEMS (register README): the canonical engine
already nasalizes word-internal junctures for vowel-final, -d, -g(s),
-s, -l, -r codas (dakndzin, lundzin, kurndep, telngyur). The ONE class
it leaves plain is the -b(s) coda, which per HGM (via Adam,
2026-08-08: SKABS 'DIR = "kamdir") becomes m — keeping its own labial
place regardless of the following consonant. This tool derives exactly
that class:

  1. only entries whose stored pronunciation EQUALS the canonical
     engine's output (custom/curated prons are skipped and counted —
     never transformed blind);
  2. each juncture located by PREFIX PRONUNCIATION: the engine pron of
     the wylie up to and including the b(s) syllable anchors the site
     (allowing the b/p voicing alternation at the prefix edge);
  3. the word-internal condition: a space at the juncture means the
     sandhi does not apply (the master and engine agree on this
     conditioning) — those junctures are skipped as regular;
  4. the b/p at the site becomes m.

Also writes the mechanical defect report: rows in the OTHER coda
classes whose stored pron differs from the canonical engine at all —
candidates for the data project (the engine nasalizes; a stored plain
form is either curation or a defect).

Usage:  python3 tools/derive_prenasal.py
Reads   build/hgm_spine_v27_2.db + the canonical pron_engine
Rewrites the prenasal-derived rows of
        data/pron_colloquial/colloquial_pron.tsv
Writes  data/extracted/prenasal_defect_candidates.tsv
"""
import os
import re
import sqlite3
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ENGINE_DIR = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools")
sys.path.insert(0, ENGINE_DIR)
from pron_engine import pronounce  # noqa: E402

A = "'"
TSV = os.path.join(HERE, "data", "pron_colloquial", "colloquial_pron.tsv")
DEFECTS = os.path.join(HERE, "data", "extracted",
                       "prenasal_defect_candidates.tsv")


def find_junctures(syls):
    """indices k where syls[k] ends b/bs and syls[k+1] is 'C (C=d/g/b/j)."""
    out = []
    for k in range(len(syls) - 1):
        s1, s2 = syls[k], syls[k + 1]
        if (s1.endswith("b") or s1.endswith("bs")) and \
           len(s2) >= 2 and s2[0] == A and s2[1] in "dgbj":
            out.append(k)
    return out


def main() -> int:
    con = sqlite3.connect(os.path.join(HERE, "build", "hgm_spine_v27_2.db"))
    rows = con.execute(
        "SELECT wylie, pronunciation FROM entries "
        "WHERE pronunciation IS NOT NULL AND pronunciation != ''"
    ).fetchall()

    derived = []
    n_custom = n_unstable = n_boundary = n_site = 0
    seen = set()
    for wylie, stored in rows:
        syls = wylie.split(" ")
        juncs = find_junctures(syls)
        if not juncs:
            continue
        try:
            engine = pronounce(wylie)
        except Exception:
            continue
        if engine != stored:
            n_custom += 1          # curated pron — never transformed blind
            continue
        sites = []
        ok = True
        for k in juncs:
            pre = " ".join(syls[:k + 1])
            try:
                pp = pronounce(pre)
            except Exception:
                ok = False
                break
            # allow the b/p voicing alternation at the prefix edge
            if not (pp and pp[-1] in "bp" and
                    stored.startswith(pp[:-1]) and
                    len(pp) <= len(stored) and
                    stored[len(pp) - 1] in "bp"):
                n_unstable += 1
                ok = False
                break
            site = len(pp) - 1
            nxt = stored[site + 1] if site + 1 < len(stored) else ""
            if nxt == " " or nxt == "":
                n_boundary += 1    # word-boundary: sandhi does not apply
                ok = False
                break
            sites.append(site)
        if not ok or not sites:
            continue
        out = list(stored)
        for s in sites:
            out[s] = "m"
        d = "".join(out)
        if d == stored or (d, wylie) in seen:
            n_site += 1
            continue
        seen.add((d, wylie))
        derived.append((d, wylie, stored))

    # rewrite the TSV: keep header + community rows, replace ALL
    # prenasal-derived rows with the fresh aligned set
    kept = []
    for line in open(TSV, encoding="utf-8"):
        line = line.rstrip("\n")
        if line.startswith("#") or not line.endswith("prenasal-derived"):
            if line:
                kept.append(line)
    with open(TSV, "w", encoding="utf-8") as f:
        for line in kept:
            f.write(line + "\n")
        for d, w, p in derived:
            f.write(f"{d}\t{w}\t{p}\tprenasal-derived\n")

    print(f"b(s)-class derived (aligned, word-internal): {len(derived)}")
    print(f"skipped — custom pron: {n_custom} · unstable prefix: "
          f"{n_unstable} · word-boundary (regular): {n_boundary} · "
          f"no-op: {n_site}")

    # mechanical defect report: other-class rows where stored != engine
    os.makedirs(os.path.dirname(DEFECTS), exist_ok=True)
    rx_other = re.compile(
        r"(\S*(?:[aeiou]|[^bn\s]g|[aeiou]gs|[aeiou][slr]|[aeiou]d)) "
        r"(" + A + r"[dgbj]\S*)")
    n_letter = n_fmt = 0
    strip = lambda s: s.replace("-", "").replace(" ", "")
    with open(DEFECTS, "w", encoding="utf-8") as f:
        f.write("# stored pronunciation differs from the canonical "
                "engine at a prenasal juncture — data-project review\n")
        f.write("# class: letter-diff = real letters differ · "
                "formatting-only = hyphen/space layout only\n")
        f.write("# wylie\tstored\tengine\tclass\n")
        for wylie, stored in rows:
            if not rx_other.search(wylie):
                continue
            try:
                engine = pronounce(wylie)
            except Exception:
                continue
            if engine != stored:
                cls = ("formatting-only"
                       if strip(engine) == strip(stored)
                       else "letter-diff")
                if cls == "letter-diff":
                    n_letter += 1
                else:
                    n_fmt += 1
                f.write(f"{wylie}\t{stored}\t{engine}\t{cls}\n")
    print(f"defect candidates: {n_letter} letter-diff · "
          f"{n_fmt} formatting-only")
    print(f"-> {DEFECTS}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
