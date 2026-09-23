#!/usr/bin/env python3
"""sync_workflow_briefs.py — a brief inlined into a workflow prompt is a COPY,
and a copy that nothing compares is a copy that will be wrong.

WHY THIS EXISTS. The C05 alignment harness used to tell each of its three
agents per segment to open docs/alignment_briefs/PROPOSE_BRIEF.md and
RECONCILE_BRIEF.md and read them in full. A transport audit dated 2026-09-20
counted 566 Read calls across 567 agents fetching the same two files — a
figure from session transcripts that are NOT in this repository, so it is
quoted here as the reason for the change and cannot be re-derived from this
tree. (What this tree can re-derive is what the prompts now carry:
tools/workflow_prompt_size.py.) The repair is to inline the
briefs in the prompt — but the briefs are the AUTHORITY for how a span is
judged, and the moment the inlined copy and the file disagree, agents are being
governed by a rule set nobody can see and nobody edited on purpose. A rule
quietly lost that way is a silent quality regression: every page after it is
judged by a shorter brief and every page looks fine.

So the copy is generated, never typed, and this compares it byte-for-byte
against the file on every build. `--write` regenerates; `--check` (the default,
and what ctest runs) refuses a tree where they have drifted apart and names the
line where they part company.

    tools/sync_workflow_briefs.py            # check, exit 1 on drift
    tools/sync_workflow_briefs.py --write    # re-inline from the files

DISCOVERY, NOT A LIST. Six gates in this repository were blind to a whole
course for weeks because each carried its own hardcoded directory list. So:
the scripts come from the same glob test_workflow_scripts_parse.py uses (one
source of truth for "where workflow scripts live"), and the briefs come from
the markers found IN each script. Inline a third brief, or add a second
workflow script, and this gate covers it with no edit here.

DISCOVERY NEEDS A FLOOR, AND THE FLOOR CANNOT LIVE INSIDE THE THING IT
MEASURES. Discovery alone was proved hollow on 2026-09-22. Coverage was
whatever `// >>> INLINED` markers happened to survive, and nothing said how
many there should be, so: weaken rule 10 inside the PROPOSE literal AND delete
that block's two marker comments, and this printed

    [PASS] ... <- docs/alignment_briefs/RECONCILE_BRIEF.md (10150 bytes)
    workflow briefs: 1 script(s), 1 inlined brief(s), 0 stale

and exited 0. Every propose agent would then be governed by a rule 10 nobody
edited on purpose. Renaming the script out of the glob was worse still: "no
workflow scripts tracked — nothing to sync", exit 0. A check that examines
nothing is not a check.

So the census is FROZEN AT ITS MEASUREMENT in tools/workflow_briefs_baseline.txt
-- a file outside every script it describes -- and any drift from it fails:
a pair that vanishes (markers deleted, script renamed, brief removed) and a
pair that appears un-frozen both stop the build. `--freeze` rewrites the
baseline, deliberately a separate, explicit act from `--write`: if regenerating
the copies also re-froze the census, deleting a brief and running the sanctioned
repair would launder the deletion.

THE OTHER HALF IS A DIFFERENT GATE. This proves the copy is FAITHFUL. It
cannot prove the copy is DELIVERED -- leave both literals byte-perfect, remove
`PROPOSE_BRIEF,` from proposeCommon(), and every check here still passes while
the agent is handed "=== BRIEF ... ===" followed immediately by "=== END BRIEF
===". tools/test_workflow_briefs_delivered.py renders the prompts and counts.

THE MARKER SHAPE, which --write emits and --check reads:

    // >>> INLINED docs/alignment_briefs/PROPOSE_BRIEF.md
    const PROPOSE_BRIEF = "...";
    // <<< INLINED docs/alignment_briefs/PROPOSE_BRIEF.md

The literal is written with json.dumps, so it is pure ASCII with every
non-ASCII character escaped, it is a valid JavaScript string literal, and it
round-trips through json.loads exactly. That exactness is the point: the
comparison below is byte-for-byte on the decoded text, not a resemblance.
"""
import argparse
import glob
import io
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))

try:
    # One source of truth for where workflow scripts live.
    from test_workflow_scripts_parse import PATTERNS
except Exception:                                    # pragma: no cover
    PATTERNS = [".claude/workflows/scripts/*.js"]

OPEN = re.compile(r"^(?P<indent>[ \t]*)// >>> INLINED (?P<path>\S+)[ \t]*$",
                  re.M)

BASELINE = os.path.join(ROOT, "tools", "workflow_briefs_baseline.txt")

BASELINE_HEADER = """\
# The frozen census of inlined briefs — the floor under tools/sync_workflow_briefs.py.
#
# Coverage is DISCOVERED from the `// >>> INLINED <path>` markers inside each
# workflow script, and discovery with no floor is not a check: deleting a
# block's two marker comments used to delete that brief's coverage while the
# gate still printed success, and renaming the only script out of the glob used
# to print "nothing to sync" and exit 0.
#
# So this file, which lives outside every script it describes, says what must
# be there. One line per (script, brief) pair. A pair that disappears fails;
# a pair that appears without being frozen here fails too, so that adding a
# brief is a deliberate act and forgetting one cannot pass quietly.
#
# Rewritten only by `tools/sync_workflow_briefs.py --freeze`, never by --write:
# if regenerating the copies also re-froze the census, deleting a brief and
# running the sanctioned repair would launder the deletion.
"""


def close_line(path):
    return "// <<< INLINED " + path


def read_baseline():
    """Frozen (script, brief) pairs. Missing file is not silently empty."""
    if not os.path.exists(BASELINE):
        return None
    pairs = []
    for ln in io.open(BASELINE, encoding="utf-8"):
        ln = ln.strip()
        if not ln or ln.startswith("#"):
            continue
        parts = ln.split()
        if len(parts) != 2:
            sys.exit("%s: cannot read line %r — expected "
                     "`<script> <brief>`"
                     % (os.path.relpath(BASELINE, ROOT), ln))
        pairs.append((parts[0], parts[1]))
    return pairs


def write_baseline(pairs):
    with io.open(BASELINE, "w", encoding="utf-8") as f:
        f.write(BASELINE_HEADER)
        for s, b in sorted(pairs):
            f.write("%s %s\n" % (s, b))


def scripts():
    out = []
    for pat in PATTERNS:
        out.extend(sorted(glob.glob(os.path.join(ROOT, pat))))
    return out


def blocks(src, rel):
    """Every marker block in one script: (path, start_idx, end_idx, body)."""
    found = []
    for m in OPEN.finditer(src):
        path = m.group("path")
        end_marker = m.group("indent") + close_line(path)
        tail = src.index("\n", m.end()) + 1
        end = src.find("\n" + end_marker, m.end())
        if end < 0:
            sys.exit("%s: block for %s opens at offset %d and never closes "
                     "with `%s`" % (rel, path, m.start(), end_marker))
        found.append((path, tail, end + 1, src[tail:end + 1]))
    return found


def literal_of(body, rel, path):
    """Pull the JSON string literal out of `const NAME = "...";`."""
    m = re.search(r"=\s*(\"(?:[^\"\\]|\\.)*\")\s*;?\s*$", body.strip(),
                  re.S)
    if not m:
        sys.exit("%s: the inlined block for %s is not a single "
                 "`const NAME = \"...\";` assignment — regenerate it with "
                 "tools/sync_workflow_briefs.py --write" % (rel, path))
    try:
        return json.loads(m.group(1))
    except ValueError as e:
        sys.exit("%s: the inlined literal for %s is not a valid string "
                 "literal (%s)" % (rel, path, e))


def const_name(body):
    m = re.search(r"const\s+([A-Za-z_$][\w$]*)\s*=", body)
    return m.group(1) if m else None


def first_divergence(a, b):
    """Name the line where two texts part, the way a reader would look."""
    al, bl = a.split("\n"), b.split("\n")
    for i in range(max(len(al), len(bl))):
        x = al[i] if i < len(al) else "<end of inlined copy>"
        y = bl[i] if i < len(bl) else "<end of file>"
        if x != y:
            return i + 1, x, y
    return None, None, None


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    p.add_argument("--write", action="store_true",
                   help="re-inline each block from its file")
    p.add_argument("--freeze", action="store_true",
                   help="rewrite tools/workflow_briefs_baseline.txt from what "
                        "is inlined now — a deliberate act, never a side "
                        "effect of --write")
    a = p.parse_args()

    frozen = read_baseline()
    found = scripts()
    # A census of zero is not a pass. The floor is consulted BEFORE anything
    # is discovered, so an empty glob fails against a non-empty baseline
    # instead of printing "nothing to sync".
    if not found and not a.freeze:
        if frozen:
            print("workflow briefs: NO workflow scripts were found, but "
                  "%s freezes %d (script, brief) pair(s):"
                  % (os.path.relpath(BASELINE, ROOT), len(frozen)))
            for s, b in frozen:
                print("    %s <- %s" % (s, b))
            print("\nThe glob is %s. A script renamed out of it takes its "
                  "briefs' coverage with it and this gate would otherwise "
                  "examine nothing and call that success."
                  % ", ".join(PATTERNS))
            return 1
        print("workflow briefs: no workflow scripts tracked — nothing to sync")
        return 0

    n_blocks = 0
    drifted = []
    rewrote = []
    seen_pairs = []
    for s in found:
        rel = os.path.relpath(s, ROOT)
        src = io.open(s, encoding="utf-8").read()
        bl = blocks(src, rel)
        if not bl:
            print("  [ -- ] %s — no inlined briefs" % rel)
            continue
        out = src
        for path, start, end, body in reversed(bl):
            n_blocks += 1
            seen_pairs.append((rel, path))
            full = os.path.join(ROOT, path)
            if not os.path.exists(full):
                print("  [FAIL] %s inlines %s, which does not exist"
                      % (rel, path))
                drifted.append((rel, path, "the file is gone"))
                continue
            want = io.open(full, encoding="utf-8").read()
            name = const_name(body) or "BRIEF"
            have = literal_of(body, rel, path)
            if have == want:
                print("  [PASS] %s <- %s (%d bytes)"
                      % (rel, path, len(want.encode("utf-8"))))
                continue
            if a.write:
                out = (out[:start] + "const " + name + " = "
                       + json.dumps(want) + ";\n" + out[end:])
                rewrote.append((rel, path))
                print("  [WROTE] %s <- %s (%d bytes)"
                      % (rel, path, len(want.encode("utf-8"))))
            else:
                line, mine, theirs = first_divergence(have, want)
                print("  [FAIL] %s has a STALE copy of %s" % (rel, path))
                print("         they first differ at line %s:" % line)
                print("           inlined: %s" % mine)
                print("           file   : %s" % theirs)
                drifted.append((rel, path, "line %s" % line))
        if a.write and out != src:
            io.open(s, "w", encoding="utf-8").write(out)

    if a.freeze:
        write_baseline(seen_pairs)
        print("workflow briefs: froze %d (script, brief) pair(s) in %s"
              % (len(seen_pairs), os.path.relpath(BASELINE, ROOT)))
        for s, b in sorted(seen_pairs):
            print("    %s <- %s" % (s, b))
        return 0

    print("workflow briefs: %d script(s), %d inlined brief(s), %d stale"
          % (len(found), n_blocks, len(drifted)))

    # THE FLOOR. Compared after the discovery loop, against a file none of the
    # scripts can edit.
    census_bad = False
    if frozen is None:
        print("\n  [FAIL] %s does not exist, so nothing says how many briefs "
              "should be inlined and a deleted marker would pass unseen. "
              "Create it:\n      tools/sync_workflow_briefs.py --freeze"
              % os.path.relpath(BASELINE, ROOT))
        census_bad = True
    else:
        have, want = set(seen_pairs), set(frozen)
        for s, b in sorted(want - have):
            print("  [FAIL] the census has SHRUNK: %s no longer inlines %s"
                  % (s, b))
            census_bad = True
        for s, b in sorted(have - want):
            print("  [FAIL] %s inlines %s, which is not in the frozen census"
                  % (s, b))
            census_bad = True
    if census_bad:
        print("\n  The census is the floor under a check whose coverage is "
              "otherwise discovered from markers INSIDE the file it checks —\n"
              "  delete the two `// >>> INLINED` comments and the brief stops "
              "being checked while this still reports success.\n"
              "  A pair that vanished: restore the block, or if the removal "
              "is intended, re-freeze and say so in the commit.\n"
              "  A pair that appeared: freeze it, so that forgetting the next "
              "one fails.\n"
              "      tools/sync_workflow_briefs.py --freeze\n"
              "  Freezing is also a claim about DELIVERY: re-run "
              "tools/test_workflow_briefs_delivered.py --freeze after it.")

    if drifted and not a.write:
        print("\nThe brief is the authority for how a span is judged. An agent "
              "governed by a stale copy is judged by rules nobody edited on "
              "purpose, and every page after it looks fine. Run\n"
              "    tools/sync_workflow_briefs.py --write\n"
              "and read the diff before you trust it.")
    # --write repairs a DRIFTED copy; it cannot restore a block somebody
    # deleted, so a broken census is a failure even then.
    if (drifted and not a.write) or census_bad:
        return 1
    if a.write and rewrote:
        print("\nRe-inlined %d block(s). Re-run tools/workflow_prompt_size.py "
              "if you quote a prompt size anywhere." % len(rewrote))
    return 0


if __name__ == "__main__":
    sys.exit(main())
