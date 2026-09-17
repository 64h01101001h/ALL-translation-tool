#!/usr/bin/env python3
"""test_workflow_scripts_parse.py — a workflow script that will not parse is
a workflow that does not exist.

2026-09-17. The C05 alignment workflow lived only inside one session's
scratch directory and hardcoded that session's path, so it could not run
anywhere else. Landing it in the repository was the fix — except the copy that
landed had a multi-line replacement pasted into a single-quoted JavaScript
string, so it could not parse. It was committed, pushed, and only discovered
because the next batch was actually launched from it:

    Script parse error: Unterminated string constant (145:4)

That is the BUILD-7 shape again (docs/FIXTURES.md): a fix that is written but
not landed is a fix that does not exist, and a fix that is landed but broken
is worse, because the repository now asserts that it works.

Nothing here checked. `.claude/` carries the skills and is tracked, but no
gate had ever read a file in it. This one does: every tracked workflow script
must parse.

SKIPS (77) rather than fails when node is unavailable, since node is not a
build dependency of this project and a missing interpreter is not a defect in
the script.

AND THE CHECK ITSELF HAD TO BE PROVED. The first version ran `node --check`
on the .js file directly and PASSED a file with a deliberate unterminated
string appended -- a gate that cannot fail. The reason is specific and worth
recording: workflow scripts begin with `export const meta`, and on a .js file
node sees module syntax it cannot treat as CommonJS, falls back to detection,
and returns success WITHOUT PARSING. Measured:

    node --check esm_bad.js  -> exit 0    (a syntax error, reported clean)
    node --check esm_bad.mjs -> exit 1    (correctly rejected)

Checking as plain ESM was wrong in the other direction: it rejected the real
script over its top-level `return`, which the WORKFLOW RUNTIME allows because
it wraps the body in an async function. A workflow script is neither ordinary
CommonJS nor ordinary ESM, so a check against either is answering a question
nobody asked.

What this does instead is mimic the runtime: demote `export const meta` to a
plain declaration, wrap the remainder in an async arrow so top-level `return`
and `await` are legal exactly where the runtime makes them legal, and parse
that. It is a parse check only -- necessary, never sufficient. Run the
workflow once before trusting it.

Mutation-tested both ways round: it fails on a script with an unterminated
string and passes the real one.
"""
import glob
import io
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PATTERNS = [".claude/workflows/scripts/*.js"]


def main():
    node = shutil.which("node")
    scripts = []
    for pat in PATTERNS:
        scripts.extend(sorted(glob.glob(os.path.join(ROOT, pat))))
    if not scripts:
        print("workflow scripts: none tracked — nothing to check")
        return 0
    if not node:
        print("workflow scripts: SKIPPED — node is not installed, and it is "
              "not a build dependency of this project. %d script(s) unchecked:"
              % len(scripts))
        for s in scripts:
            print("    %s" % os.path.relpath(s, ROOT))
        return 77

    bad = 0
    for s in scripts:
        rel = os.path.relpath(s, ROOT)
        # .mjs, not .js -- see the module note at the top of this file. On a
        # .js file node returns success without parsing when it sees `export`.
        src = io.open(s, encoding="utf-8").read()
        # `export const meta` -> `const meta`, so the file is not module-only;
        # then wrap, so top-level return/await parse where the runtime allows
        # them. .mjs because node --check silently skips a .js file that still
        # contains module syntax.
        body = re.sub(r"^\s*export\s+const\s+meta", "const meta", src, count=1,
                      flags=re.M)
        with tempfile.TemporaryDirectory() as td:
            probe = os.path.join(td, "probe.mjs")
            io.open(probe, "w", encoding="utf-8").write(
                "(async () => {\n" + body + "\n})();\n")
            r = subprocess.run([node, "--check", probe],
                               capture_output=True, text=True)
        if r.returncode == 0:
            print("  [PASS] %s" % rel)
        else:
            bad += 1
            first = (r.stderr or "").strip().split("\n")
            print("  [FAIL] %s" % rel)
            for line in first[:4]:
                print("         %s" % line)
    print("workflow scripts: %d checked, %d that will not parse"
          % (len(scripts), bad))
    if bad:
        print("\nA workflow that cannot parse cannot run. Fix it, and run it "
              "once before committing — parsing is necessary, not sufficient.")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
