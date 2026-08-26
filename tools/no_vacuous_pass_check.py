#!/usr/bin/env python3
"""no_vacuous_pass_check.py — a suite handed no data must FAIL, not pass.

SQA 2026-08-22, BUILD-7. Run against a path that does not exist, three
suites used to exit 0:

    catalog_id_smoke     "[SKIP] library battery (no library at ...)"
    catalog_qc_smoke     "library: 0 mismatch flag(s)" then
                         "[PASS] the installed library's own mismatch rate stays low"
    shelf_suggest_smoke  "held-out: 0 queries · top-1 0 (nan%)" then
                         "[PASS] ... (measured 31%)"   <- a string literal

House rule 4: nothing prints a success string that was not produced by
the code path that verified it. This suite is the mechanical form of
that rule for the batteries themselves.

Usage:
    no_vacuous_pass_check.py <scratch_dir> <binary> [<binary> ...]

Each binary is invoked with an absent library root (argument shape is
looked up by basename) and must:

  1. exit non-zero                     — an empty measure is not a pass
  2. not die on a signal               — a clean failure, not a crash
  3. print no line carrying both "[PASS]" and "measured"
                                       — no fabricated measurement
  4. not print its own success string  — "<name> OK"
"""
import os
import subprocess
import sys

ABSENT = "/nonexistent/all-fixture-that-is-not-there"

# basename -> argv shape, given (absent_root, scratch_dir)
SHAPES = {
    "catalog_id_smoke": lambda absent, scratch: [absent],
    "catalog_qc_smoke": lambda absent, scratch: [scratch, absent],
    "shelf_suggest_smoke": lambda absent, scratch: [absent],
}


def main(argv):
    if len(argv) < 3:
        print("usage: no_vacuous_pass_check.py <scratch_dir> <binary>...")
        return 2
    scratch = argv[1]
    binaries = argv[2:]
    os.makedirs(scratch, exist_ok=True)
    if os.path.exists(ABSENT):
        print("  [FAIL] the 'absent' probe path %s exists — this check "
              "cannot mean anything" % ABSENT)
        return 1

    failures = 0
    examined = 0
    for path in binaries:
        name = os.path.basename(path)
        shape = SHAPES.get(name)
        if shape is None:
            print("  [FAIL] %s: no argument shape recorded for this suite — "
                  "add one rather than skipping it" % name)
            failures += 1
            continue
        if not os.path.exists(path):
            print("  [FAIL] %s: binary not built at %s" % (name, path))
            failures += 1
            continue
        cmd = [path] + shape(ABSENT, os.path.join(scratch, name + "_vacuous"))
        proc = subprocess.run(cmd, capture_output=True, text=True)
        out = (proc.stdout or "") + (proc.stderr or "")
        examined += 1

        bad = []
        if proc.returncode == 0:
            bad.append("exited 0 over an absent fixture (a vacuous pass)")
        if proc.returncode < 0:
            bad.append("died on signal %d — a crash, not a reported failure"
                       % -proc.returncode)
        elif proc.returncode >= 128:
            bad.append("exited %d — a crash, not a reported failure"
                       % proc.returncode)
        for line in out.splitlines():
            if "[PASS]" in line and "measured" in line:
                bad.append("printed a measurement on a PASS line over no "
                           "data: %s" % line.strip())
        if ("%s OK" % name) in out:
            bad.append("printed its success string '%s OK'" % name)

        if bad:
            failures += len(bad)
            for b in bad:
                print("  [FAIL] %s: %s" % (name, b))
        else:
            print("  [PASS] %s: exit %d, no success string, no fabricated "
                  "measurement" % (name, proc.returncode))

    if examined == 0:
        print("  [FAIL] no suite was examined — this check would be the "
              "vacuous pass it exists to forbid")
        failures += 1

    print("no_vacuous_pass: %d suite(s) examined, %d violation(s)"
          % (examined, failures))
    print("FAILURES" if failures else "no_vacuous_pass OK")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
