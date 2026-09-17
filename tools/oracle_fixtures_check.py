#!/usr/bin/env python3
"""oracle_fixtures_check.py — the banked oracle dumps must still be what
their generators produce.

WHY THIS EXISTS. The batteries that prove Rule 2 — that each C++ port agrees
with its canonical Python engine — do not run the Python. They diff the port
against a DUMP of the Python's output, banked under build/. That makes the
dump a load-bearing part of the proof, and a dump has a failure mode the
engines do not: it can go stale in total silence.

It did. Commit 5c3d77f8 fixed a real defect in BOTH engines — the EWTS prefix
disambiguator, where `g-y` was refused and 641 master entries kept a literal
hyphen in their Tibetan. Both files were changed in parity, the fix was right,
and build/forward_reference.tsv was left holding the OLD, broken values. From
that commit forward the forward battery reported 34,988/35,211 (99.367%)
against a 100% floor: red, on a stale artifact, for a fix that was correct.

The dump could not be checked without being destroyed — all three generators
wrote to one hardcoded path — so "regenerate and look" meant losing the thing
you were comparing against. They take --out now, and this runs them.

WHAT A FAILURE HERE MEANS. Not that the engine is wrong. It means the banked
dump and the generator disagree, and SOMEONE HAS TO SAY WHICH IS RIGHT:

  - an intended engine fix, dump not refreshed  -> regenerate, and read every
    changed row before banking it. A fixture is only worth what you can say
    about where it came from.
  - an unintended engine change                 -> the engine is the bug.
  - a changed generator                         -> the sample moved. These
    dumps pin RATCHETS; a wrong generator relocates the floor to a basis
    nobody chose, with no engine touched and the suite green throughout.

Usage:  python3 tools/oracle_fixtures_check.py
"""
import io
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# (generator, banked fixture, what the battery that consumes it proves)
# (generator, argv template, banked fixture(s), what the battery proves).
# {out} is the temp path; {db} the spine. The first three take --out; the
# rest predate that and take positional paths, which is just as good.
#
# This covered 3 of the 10 build/ artifacts ctest reads by absolute path until
# 2026-09-16 — a gate whose own coverage nobody had measured. The one still
# missing is build/towylie_reference.tsv, which docs/FIXTURES.md records
# honestly as having "no generator in this repo (emitted by the pyewts oracle
# run)"; pyewts is not installed here and is not ours to install. That is the
# standing BUILD-7 item, named rather than quietly skipped.
FIXTURES = [
    ("tools/build_forward_reference.py", ["--out", "{out}"],
     ["build/forward_reference.tsv"],
     "forward_battery — C++ wylie->unicode vs engines/ewts_unicode.py"),
    ("tools/build_pron_reference.py", ["--out", "{out}"],
     ["build/pron_reference.tsv"],
     "pron parity — C++ phonetics vs Handoff/tools/pron_engine.py"),
    ("tools/build_sanskrit_reference.py", ["--out", "{out}"],
     ["build/sanskrit_reference.tsv"],
     "sanskrit parity — C++ vs Handoff/tools/sanskrit_convert.py"),
    ("tools/build_botok_reference.py", ["{db}", "{out}"],
     ["build/botok_syls_reference.tsv"],
     "botok_smoke — C++ syllable components vs the botok data"),
    ("tools/build_botok_tok_reference.py", ["{out}", "{out2}"],
     ["build/botok_tok_scenarios.txt", "build/botok_tok_corpus.tsv"],
     "botok_tok_smoke — C++ tokenizer vs the Python prototype"),
    ("tools/weirdness_rank.py", ["{db}", "{out}"],
     ["build/weird_top.tsv"],
     "weird_battery — the weirdness ranking the C++ must reproduce"),
]


def main():
    fails = 0
    unchecked = []
    for gen, argv, bankeds, proves in FIXTURES:
        gp = os.path.join(ROOT, gen)
        bps = [os.path.join(ROOT, b) for b in bankeds]
        if not all(os.path.exists(b) for b in bps):
            print("  [SKIP] %s — not built yet" % bankeds[0])
            continue
        if not os.path.exists(gp):
            print("  [FAIL] %s — no generator at %s. A dump nobody can "
                  "rebuild is a dump nobody can check." % (bankeds[0], gen))
            fails += 1
            continue
        with tempfile.TemporaryDirectory() as td:
            outs = [os.path.join(td, os.path.basename(b)) for b in bankeds]
            subs = {"out": outs[0],
                    "out2": outs[1] if len(outs) > 1 else outs[0],
                    "db": os.path.join(ROOT, "build", "hgm_spine_v27_2.db")}
            cmd = [sys.executable, gp] + [a.format(**subs) for a in argv]
            r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
            if r.returncode != 0:
                # A third-party oracle package that is not installed here is an
                # ENVIRONMENT absence, not a stale dump. It must still be named:
                # a gate that says nothing about what it could not check is a
                # tick meaning "not examined". botok and pyewts are both in this
                # class and neither is ours to install.
                miss = re.search(r"No module named '([^']+)'", r.stderr or "")
                if miss:
                    print("  [UNCHECKED] %-26s needs the '%s' package, which is "
                          "not installed here" % (bankeds[0], miss.group(1)))
                    unchecked.append((bankeds[0], miss.group(1)))
                    continue
                print("  [FAIL] %s — generator exited %d\n    %s"
                      % (bankeds[0], r.returncode, r.stderr.strip()[:300]))
                fails += 1
                continue
            for out, bp, banked in zip(outs, bps, bankeds):
                if not os.path.exists(out):
                    print("  [FAIL] %s — the generator wrote nothing there. If "
                          "it writes to a fixed path it would have overwritten "
                          "the very file under comparison." % banked)
                    fails += 1
                    continue
                got = io.open(out, encoding="utf-8", errors="replace").read().split("\n")
                want = io.open(bp, encoding="utf-8", errors="replace").read().split("\n")
                if got == want:
                    print("  [PASS] %-32s %d lines reproduce exactly"
                          % (os.path.basename(banked), len(want)))
                    continue
                fails += 1
                diff = [i for i in range(max(len(got), len(want)))
                        if (got[i] if i < len(got) else None)
                        != (want[i] if i < len(want) else None)]
                print("  [FAIL] %s — %d line(s) differ from what %s produces.\n"
                      "         It proves: %s\n"
                      "         Decide which is right before regenerating."
                      % (banked, len(diff), gen, proves))
                for i in diff[:5]:
                    print("           line %d\n             banked: %s\n"
                          "             now:    %s"
                          % (i + 1, (want[i] if i < len(want) else "<missing>")[:90],
                             (got[i] if i < len(got) else "<missing>")[:90]))
                if len(diff) > 5:
                    print("           ... and %d more" % (len(diff) - 5))
    if unchecked:
        print("  %d fixture(s) could NOT be checked, and that is not a pass:"
              % len(unchecked))
        for b, m in unchecked:
            print("      %-34s missing package: %s" % (b, m))
    print("  build/towylie_reference.tsv has no generator at all "
          "(docs/FIXTURES.md, BUILD-7) — it comes from a pyewts run.")
    print("oracle fixtures: %s (%d failure%s)"
          % ("ALL PASS" if not fails else "FAIL", fails,
             "" if fails == 1 else "s"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
