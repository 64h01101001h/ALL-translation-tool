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
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# (generator, banked fixture, what the battery that consumes it proves)
FIXTURES = [
    ("tools/build_forward_reference.py", "build/forward_reference.tsv",
     "forward_battery — C++ wylie->unicode vs engines/ewts_unicode.py"),
    ("tools/build_pron_reference.py", "build/pron_reference.tsv",
     "pron parity — C++ phonetics vs Handoff/tools/pron_engine.py"),
    ("tools/build_sanskrit_reference.py", "build/sanskrit_reference.tsv",
     "sanskrit parity — C++ vs Handoff/tools/sanskrit_convert.py"),
]


def main():
    fails = 0
    for gen, banked, proves in FIXTURES:
        gp, bp = os.path.join(ROOT, gen), os.path.join(ROOT, banked)
        if not os.path.exists(bp):
            print("  [SKIP] %s — not built yet" % banked)
            continue
        if not os.path.exists(gp):
            print("  [FAIL] %s — no generator at %s. A dump nobody can "
                  "rebuild is a dump nobody can check." % (banked, gen))
            fails += 1
            continue
        with tempfile.TemporaryDirectory() as td:
            out = os.path.join(td, os.path.basename(banked))
            r = subprocess.run([sys.executable, gp, "--out", out],
                               capture_output=True, text=True, cwd=ROOT)
            if r.returncode != 0:
                print("  [FAIL] %s — generator exited %d\n    %s"
                      % (banked, r.returncode, r.stderr.strip()[:300]))
                fails += 1
                continue
            got = io.open(out, encoding="utf-8").read().split("\n")
            want = io.open(bp, encoding="utf-8").read().split("\n")
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
    print("oracle fixtures: %s (%d failure%s)"
          % ("ALL PASS" if not fails else "FAIL", fails,
             "" if fails == 1 else "s"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
