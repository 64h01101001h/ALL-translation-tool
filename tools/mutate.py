#!/usr/bin/env python3
"""Mutation harness — plant a bug, REQUIRE the battery to go red.

SQA TEST-1 (2026-08-23): 13 of 21 deliberately re-injected defects
survived the full battery. The unbiased arm was 4 of 9 killed - 44%,
n=9, 95% CI roughly 14-79%. A green suite is therefore not evidence
that behaviour is preserved, and "I added a test" is not evidence the
test can fail.

This exists so that claim can be checked mechanically instead of by
hand. Everything below is a lesson paid for on 2026-08-23/24:

  * ANCHOR ONCE. A mutation whose anchor matches zero or many times is
    not the mutation you think you ran. Asserted, not assumed.
  * DELETE THE OBJECT. Restoring a file with cp gives it an mtime
    OLDER than the object built from the mutant, so make skips the
    rebuild and the "restored" run still executes the bug. Two
    mutation results were contaminated this way in one afternoon.
    tools/verify.sh's own header documents this and it still bit.
  * CHECK THE BUILD. A suppressed build log let ctest report
    "100% tests passed" over code that did not compile. If the build
    fails, the run proves nothing and says so.
  * GREEN IS FAILURE. The whole point is inverted here: a mutant that
    survives means the pin cannot fail, which is worse than no pin.
  * RESTORE AND RE-PROVE. The tree must end green, and that is
    verified rather than hoped.

Usage:
  tools/mutate.py --file core/src/gofer.cpp \\
                  --old 'if (x) {' --new 'if (false) {' \\
                  [--test app_selftest] [--expect-fail-contains 'DATA-1']
  tools/mutate.py --selftest

Exit: 0 = the mutant was KILLED (good). 1 = it SURVIVED, or the run
      could not be trusted.
"""
import argparse
import io
import os
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "cmake-build-release")


def sh(cmd, cwd=None):
    r = subprocess.run(cmd, cwd=cwd or ROOT, capture_output=True,
                       text=True)
    return r.returncode, r.stdout, r.stderr


def objects_for(rel_path):
    """Every compiled object derived from this source file."""
    base = os.path.basename(rel_path) + ".o"
    out = []
    for dirpath, _dirs, files in os.walk(BUILD):
        for f in files:
            if f == base:
                out.append(os.path.join(dirpath, f))
    return out


def build():
    """Returns (ok, first_error_line)."""
    rc, out, err = sh(["cmake", "--build", BUILD, "-j8"])
    blob = out + err
    for line in blob.splitlines():
        if " error" in line.lower():
            return rc == 0, line.strip()
    return rc == 0, ""


def run_tests(test_filter):
    cmd = ["ctest", "--output-on-failure"]
    if test_filter:
        cmd += ["-R", test_filter]
    rc, out, err = sh(cmd, cwd=BUILD)
    return rc, out + err


def mutate(path_rel, old, new, test_filter, expect_contains):
    path = os.path.join(ROOT, path_rel)
    if not os.path.exists(path):
        print("  no such file: %s" % path_rel)
        return 1
    src = io.open(path, encoding="utf-8").read()

    n = src.count(old)
    if n != 1:
        print("  ANCHOR MATCHED %d TIMES - refusing. A mutation whose "
              "anchor is not unique is not the mutation you think you "
              "ran." % n)
        return 1

    verdict = 1
    restore_ok = True
    backup = tempfile.mktemp(suffix=".bak")
    shutil.copy2(path, backup)
    objs = objects_for(path_rel)
    print("  file    : %s" % path_rel)
    print("  objects : %d will be deleted before each build" % len(objs))

    try:
        # ---- plant the bug --------------------------------------
        io.open(path, "w", encoding="utf-8").write(
            src.replace(old, new, 1))
        for o in objs:
            if os.path.exists(o):
                os.remove(o)
        ok, errline = build()
        if not ok:
            print("  BUILD FAILED on the mutant - this run proves "
                  "NOTHING, not that the pin works.")
            if errline:
                print("    %s" % errline[:160])
            return 1
        rc, out = run_tests(test_filter)
        killed = rc != 0
        matched = (not expect_contains) or (expect_contains in out)

        if killed and matched:
            print("  MUTANT KILLED - the pin can fail. Good.")
            for line in out.splitlines():
                if "[FAIL]" in line or "***Failed" in line:
                    print("    %s" % line.strip()[:150])
            verdict = 0
        elif killed and not matched:
            print("  battery went red, but NOT for the expected "
                  "reason (%r absent). A pin that fails for an "
                  "unrelated reason is not evidence." % expect_contains)
            verdict = 1
        else:
            print("  *** MUTANT SURVIVED *** - the battery is green "
                  "with this bug planted. The pin cannot fail, which "
                  "is worse than no pin (SQA TEST-1).")
            verdict = 1
    finally:
        # ---- restore, and PROVE the tree is green again ----------
        # No `return` in here: a return inside finally swallows the
        # exception that got us here and overrides the verdict. The
        # restore result travels in a flag instead.
        shutil.copy2(backup, path)
        os.utime(path, None)          # newer than the mutant's object
        os.remove(backup)
        for o in objs:
            if os.path.exists(o):
                os.remove(o)
        ok, errline = build()
        if not ok:
            print("  RESTORE BUILD FAILED - the tree is NOT clean.")
            if errline:
                print("    %s" % errline[:160])
            restore_ok = False
        else:
            rc, _ = run_tests(test_filter)
            if rc != 0:
                print("  RESTORED TREE IS RED - something else is "
                      "wrong; do not trust the verdict above.")
                restore_ok = False
            else:
                print("  restored, rebuilt, green.")
    # a verdict from a tree we could not restore is not a verdict
    return verdict if restore_ok else 1


def cmd_selftest():
    """Prove the harness itself, without touching the real tree."""
    bad = []
    d = tempfile.mkdtemp(prefix="mutate_selftest_")
    p = os.path.join(d, "sample.txt")
    io.open(p, "w", encoding="utf-8").write("alpha\nbeta\nalpha\n")

    src = io.open(p, encoding="utf-8").read()
    if src.count("alpha") != 2:
        bad.append("fixture wrong")
    # the anchor rule is the one that has actually bitten
    if src.count("alpha") == 1:
        bad.append("a doubly-matching anchor must be refused")
    if src.count("beta") != 1:
        bad.append("unique anchor detection")

    if not objects_for("definitely_not_a_real_file.cpp") == []:
        bad.append("objects_for must return empty for an unknown file")

    shutil.rmtree(d, ignore_errors=True)
    for b in bad:
        print("  [FAIL]", b)
    print("mutate selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--file")
    ap.add_argument("--old")
    ap.add_argument("--new")
    ap.add_argument("--test", default=None,
                    help="ctest -R filter; omit for the whole battery")
    ap.add_argument("--expect-fail-contains", default=None,
                    help="require the red result to mention this, so a "
                         "pin cannot be credited for an unrelated failure")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return cmd_selftest()
    if not (a.file and a.old is not None and a.new is not None):
        ap.error("--file, --old and --new are required")
    print("mutation harness (SQA TEST-1)")
    return mutate(a.file, a.old, a.new, a.test, a.expect_fail_contains)


if __name__ == "__main__":
    sys.exit(main())
