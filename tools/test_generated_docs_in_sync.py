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
import filecmp, importlib.util, io, os, shutil, subprocess, sys, tempfile

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

def check_punchlist():
    """The punchlist page must match a fresh generation from the three source
    lists. This is the whole point of the generator: the previous page was a
    one-off reading that drifted for five days and 139 commits until an item it
    called release-blocking had been marked DONE in its own source. Editing a
    source list without regenerating now fails here instead of quietly aging."""
    out = os.path.join(ROOT, 'docs/RELEASE_PUNCHLIST.md')
    gen = os.path.join(ROOT, 'tools/build_punchlist.py')
    spec = importlib.util.spec_from_file_location('build_punchlist', gen)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    before = io.open(out, encoding='utf-8').read()
    tmp = tempfile.mkdtemp()
    try:
        real, mod.PAGE = mod.PAGE, os.path.join(tmp, 'regen.md')
        real_argv, sys.argv = sys.argv, ['build_punchlist.py', '--emit']
        buf, real_out = io.StringIO(), sys.stdout
        sys.stdout = buf
        try:
            mod.main()
        finally:
            sys.stdout = real_out
            sys.argv = real_argv
            mod.PAGE = real
        after = io.open(os.path.join(tmp, 'regen.md'), encoding='utf-8').read()
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    if before == after:
        return 'OK', '%d bytes, byte-identical to a fresh generation' % len(before)
    b, a = before.splitlines(), after.splitlines()
    for i, (x, y) in enumerate(zip(b, a)):
        if x != y:
            return 'DRIFTED', ('first difference at line %d\n     committed: %s'
                               '\n     regenerated: %s' % (i + 1, x[:110], y[:110]))
    return 'DRIFTED', ('identical for %d lines, then the files differ in length '
                       '(committed %d lines, regenerated %d)'
                       % (min(len(b), len(a)), len(b), len(a)))


def check_rebuild_order():
    """The handoff telling the data project which master entries to rebuild.
    Its whole argument is that the figures can be re-derived, so a drifted
    copy is worse than none — it would send someone to rewrite 627 entries on
    a count that no longer holds. Skips where the master is absent: data/ is
    gitignored, so a fresh clone has no dictionary and that is not a failure."""
    out = os.path.join(ROOT, 'docs', 'MASTER_REBUILD_ORDER.md')
    master = os.path.join(ROOT, 'data', 'hgm_dictionary_v27_2.json.gz')
    if not os.path.exists(master):
        return 'SKIPPED', 'no master present (data/ is gitignored)'
    if not os.path.exists(out):
        return 'MISSING', 'docs/MASTER_REBUILD_ORDER.md does not exist'
    before = io.open(out, encoding='utf-8').read()
    tmp = out + '.regen'
    r = subprocess.run([sys.executable,
                        os.path.join(ROOT, 'tools',
                                     'build_master_rebuild_order.py'),
                        '--out', tmp],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0:
        return 'FAILED', 'generator exited %d: %s' % (r.returncode,
                                                      r.stderr.strip()[:200])
    after = io.open(tmp, encoding='utf-8').read()
    os.remove(tmp)
    if before == after:
        return 'OK', '%d bytes, byte-identical to a fresh generation' % len(before)
    b, a2 = before.split('\n'), after.split('\n')
    for i, (x, y) in enumerate(zip(b, a2)):
        if x != y:
            return 'DRIFTED', ('first difference at line %d\n     committed: %s'
                               '\n     regenerated: %s' % (i + 1, x[:110], y[:110]))
    return 'DRIFTED', ('committed %d lines, regenerated %d' % (len(b), len(a2)))

def check_manual_parts():
    """The three distribution manual parts are the canonical manual's `# PART`
    sections. Nothing derived them, so nothing kept them: when the generator
    was written they had been frozen since 22 August while the manual moved,
    still advertising a smaller app and telling the reader to open a pane
    ("Input -> Scan") that had been renamed weeks earlier. The press already
    gates the OTHER copy of the manual because those two drifted 58 lines in a
    day; this closes the same hole one file over."""
    gen = os.path.join(ROOT, 'tools', 'gen_manual_parts.py')
    r = subprocess.run([sys.executable, gen, '--check'],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode == 0:
        import importlib.util as _iu
        spec = _iu.spec_from_file_location('_gmp', gen)
        mod = _iu.module_from_spec(spec)
        spec.loader.exec_module(mod)
        n = sum(len(io.open(os.path.join(ROOT, 'docs/distribution', f),
                            encoding='utf-8').read().split('\n'))
                for f in mod.PARTS)
        return 'OK', ('3 parts, %d lines, byte-identical to a fresh split of '
                      'data/help/USER_MANUAL.md' % n)
    out = (r.stdout + r.stderr).strip()
    if 'REFUSED' in out:
        return 'FAILED', out[:300]
    return 'DRIFTED', (out[:300] + '\n     regenerate: python3 '
                       'tools/gen_manual_parts.py')


CHECKS = [('docs/ERRATA_REGISTER.md', check_errata_register),
          ('docs/RELEASE_PUNCHLIST.md', check_punchlist),
          ('docs/MASTER_REBUILD_ORDER.md', check_rebuild_order),
          ('docs/distribution/manual_part*.md', check_manual_parts)]

def main():
    # "in sync" is a CLAIM. A check that could not run has not made it, and
    # saying so in the same words as a check that did is how a tick comes to
    # mean "not examined". SKIPPED prints as itself, is counted, and is named
    # again in the summary; only a real failure is a failure.
    bad = 0
    skipped = 0
    for name, fn in CHECKS:
        status, detail = fn()
        if status == 'OK':
            print('in sync: %s — %s' % (name, detail))
        elif status == 'SKIPPED':
            skipped += 1
            print('SKIPPED (NOT verified): %s — %s' % (name, detail))
        else:
            bad += 1
            print('FAIL %s: %s\n     %s' % (name, status, detail))
    if skipped:
        print('%d of %d generated document(s) could NOT be checked'
              % (skipped, len(CHECKS)))
    if bad:
        return 1
    # every check skipped means the run proved nothing — ctest's skip code, so
    # it reads as skipped rather than as a pass.
    if skipped == len(CHECKS):
        return 77
    return 0

if __name__ == '__main__':
    sys.exit(main())
