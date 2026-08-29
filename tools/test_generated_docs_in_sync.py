#!/usr/bin/env python3
"""A generated document must match the data it was generated from.

docs/ERRATA_REGISTER.md is the human-readable face of docs/errata_register.json,
and the two have already drifted once: the prose claimed 52 findings while the
sidecar held 78, because the prose was hand-maintained after the generator ran.
That drift is what turned the register into a document nobody could quote
safely, and it was found by accident rather than by a check.

The generator is deterministic, so the check is simply: regenerate into a
temporary file and compare. If they differ, someone edited the prose by hand or
forgot to re-run the generator after changing the sidecar, and the committed
document is making claims its own data does not support.

This is cheap, exact, and needs no knowledge of the content — which is the
whole reason it is worth having.
"""
import filecmp, importlib.util, io, os, shutil, sys, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def check_errata_register():
    """Regenerate ERRATA_REGISTER.md into a temp dir and compare."""
    out = os.path.join(ROOT, 'docs', 'ERRATA_REGISTER.md')
    if not os.path.exists(out):
        return 'MISSING', 'docs/ERRATA_REGISTER.md does not exist'
    spec = importlib.util.spec_from_file_location(
        '_ber', os.path.join(ROOT, 'tools', 'build_errata_register.py'))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    before = io.open(out, encoding='utf-8').read()
    tmp = tempfile.mkdtemp()
    try:
        # the generator writes to a module-level OUT; point it at the temp copy
        # and put it back afterwards, so a failing run cannot rewrite the
        # committed file and make itself pass.
        real, mod.OUT = mod.OUT, os.path.join(tmp, 'regen.md')
        err = io.StringIO()
        real_err, sys.stderr = sys.stderr, err
        try:
            mod.main()
        finally:
            sys.stderr = real_err
            mod.OUT = real
        after = io.open(os.path.join(tmp, 'regen.md'), encoding='utf-8').read()
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    if before == after:
        return 'OK', '%d bytes, byte-identical to a fresh generation' % len(before)
    # say WHERE it drifted, not merely that it did
    b, a = before.splitlines(), after.splitlines()
    for i, (x, y) in enumerate(zip(b, a)):
        if x != y:
            return 'DRIFTED', ('first difference at line %d\n     committed: %s'
                               '\n     regenerated: %s' % (i + 1, x[:110], y[:110]))
    return 'DRIFTED', ('identical for %d lines, then the files differ in length '
                       '(committed %d lines, regenerated %d)'
                       % (min(len(b), len(a)), len(b), len(a)))

CHECKS = [('docs/ERRATA_REGISTER.md', check_errata_register)]

def main():
    bad = 0
    for name, fn in CHECKS:
        status, detail = fn()
        if status == 'OK':
            print('in sync: %s — %s' % (name, detail))
        else:
            bad += 1
            print('FAIL %s: %s\n     %s' % (name, status, detail))
    return 1 if bad else 0

if __name__ == '__main__':
    sys.exit(main())
