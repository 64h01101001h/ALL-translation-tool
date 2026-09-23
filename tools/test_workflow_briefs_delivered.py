#!/usr/bin/env python3
"""test_workflow_briefs_delivered.py — a brief that is faithfully inlined and
never handed to anybody is the same thing as no brief at all.

WHY THIS EXISTS, AND WHAT IT IS NOT. tools/sync_workflow_briefs.py compares
each `// >>> INLINED` literal in a workflow script against the file it came
from, byte for byte, and it is a real gate: weaken a rule in the copy and it
names the file, the brief, the line and both sides of the divergence. But it
proves only that the COPY IS FAITHFUL. On 2026-09-22 a verifier left both
literals byte-perfect, removed `PROPOSE_BRIEF,` from proposeCommon() in
.claude/workflows/scripts/c05-align.js, and every check in the tree stayed
green while the propose prompt fell from 16,489 to 8,548 bytes per agent and
the agent was handed

    === BRIEF: docs/alignment_briefs/PROPOSE_BRIEF.md (verbatim, the authority) ===
    === END BRIEF ===

with the ten rules gone and a supplement telling it not to open the file. That
is the whole claim of the transport rewrite, and nothing checked it.

HOW IT CHECKS — AN INDEPENDENT RECOUNT, NOT A REGENERATION. The repeated fault
in this repository is a check that compares a generator with itself: it cannot
catch a broken generator, because both sides move together. So this does not
read the script's literals at all. It reads THE BRIEF FILES, renders the
prompts the script actually builds, and counts. The two sides come from
different places: the expected text from docs/alignment_briefs/, the delivered
text from executing the script.

Rendering reuses tools/workflow_prompt_size.py, which mimics the workflow
runtime (demote `export const meta`, wrap the body so top-level return and
await are legal) and stubs agent()/parallel()/phase()/log()/args. Nothing is
spawned and nothing is spent; the prompts are plain string concatenation.

NO RATCHET ABOVE THE MEASUREMENT. The count is frozen exactly where it was
measured, in tools/workflow_delivery_baseline.txt, and both directions fail:
one prompt fewer carrying the brief is a delivery regression, one more is a
change nobody froze. The args each script is rendered against are frozen in
the same file, because a count means nothing without the launch that produced
it.

    tools/test_workflow_briefs_delivered.py            # check
    tools/test_workflow_briefs_delivered.py --freeze   # re-measure and freeze

SKIPS (77) when node is not installed: node is not a build dependency of this
project, and a missing interpreter is not a defect in the script. The skip is
this gate's alone — the byte-for-byte comparison in sync_workflow_briefs.py
needs no interpreter and never skips, so a drifted copy still fails on a
machine with no node.
"""
import argparse
import datetime
import io
import os
import shutil
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import workflow_prompt_size as W            # noqa: E402  (path set above)
import sync_workflow_briefs as S            # noqa: E402

BASELINE = os.path.join(ROOT, "tools", "workflow_delivery_baseline.txt")

HEADER = """\
# What each workflow script actually HANDS its agents — frozen at the
# measurement, on the date below.
#
# tools/sync_workflow_briefs.py proves the inlined copy of a brief is the file.
# It cannot prove the copy reaches a prompt: removing `PROPOSE_BRIEF,` from
# proposeCommon() left every literal byte-perfect and every gate green while
# the propose agents were handed an empty brief block. This file is the other
# half — the args each script is rendered against, and how many of the prompts
# that rendering produces carry each brief IN FULL.
#
# Both directions fail. One prompt fewer is a delivery regression; one more is
# a change nobody froze. Re-freeze deliberately, with
# `tools/test_workflow_briefs_delivered.py --freeze`, and say why in the
# commit.
#
#   args     <script>  <the args JSON, one line>
#   agents   <script>  <how many agent() calls that rendering makes>
#   delivers <script>  <brief>  <prompts carrying it in full>
"""


def read_baseline():
    if not os.path.exists(BASELINE):
        return None
    args, agents, delivers = {}, {}, {}
    for ln in io.open(BASELINE, encoding="utf-8"):
        ln = ln.rstrip("\n")
        if not ln.strip() or ln.lstrip().startswith("#"):
            continue
        parts = ln.split(None, 2)
        if len(parts) < 3:
            sys.exit("%s: cannot read line %r"
                     % (os.path.relpath(BASELINE, ROOT), ln))
        kind, script, rest = parts[0], parts[1], parts[2]
        if kind == "args":
            args[script] = rest.strip()
        elif kind == "agents":
            agents[script] = int(rest.strip())
        elif kind == "delivers":
            bits = rest.split()
            if len(bits) != 2:
                sys.exit("%s: cannot read line %r"
                         % (os.path.relpath(BASELINE, ROOT), ln))
            delivers[(script, bits[0])] = int(bits[1])
        else:
            sys.exit("%s: unknown record %r" % (
                os.path.relpath(BASELINE, ROOT), kind))
    return args, agents, delivers


def write_baseline(rows):
    with io.open(BASELINE, "w", encoding="utf-8") as f:
        f.write(HEADER)
        f.write("#\n# Measured %s by "
                "tools/test_workflow_briefs_delivered.py --freeze.\n"
                % datetime.date.today().isoformat())
        for script in sorted(rows):
            r = rows[script]
            f.write("\nargs     %s  %s\n" % (script, r["args"]))
            f.write("agents   %s  %d\n" % (script, r["agents"]))
            for brief in sorted(r["delivers"]):
                f.write("delivers %s  %s  %d\n"
                        % (script, brief, r["delivers"][brief]))


def render(script_rel, args_json):
    """Every prompt the script builds, as text."""
    path = os.path.join(ROOT, script_rel)
    data, err = W.run(path, None, args_json)
    return data.get("calls", []), err


def briefs_of(script_rel):
    """Which briefs this script inlines — discovered from the markers, and
    floored by sync_workflow_briefs.py's own frozen census, which is what
    stops a deleted marker from shrinking this gate too."""
    src = io.open(os.path.join(ROOT, script_rel), encoding="utf-8").read()
    return [b[0] for b in S.blocks(src, script_rel)]


def measure(script_rel, args_json):
    calls, err = render(script_rel, args_json)
    out = {"args": args_json, "agents": len(calls), "delivers": {}, "err": err,
           "calls": calls}
    for brief in briefs_of(script_rel):
        full = os.path.join(ROOT, brief)
        if not os.path.exists(full):
            out["delivers"][brief] = -1
            continue
        want = io.open(full, encoding="utf-8").read()
        out["delivers"][brief] = sum(1 for c in calls if want in c["text"])
    return out


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    p.add_argument("--freeze", action="store_true",
                   help="re-measure and rewrite the baseline")
    a = p.parse_args()

    if not shutil.which("node"):
        print("workflow briefs delivered: SKIPPED — node is not installed, "
              "and it is not a build dependency of this project. Nothing "
              "proved that the briefs reach the prompts.")
        print("    (tools/sync_workflow_briefs.py still compared the inlined "
              "copies against the files; that check needs no interpreter.)")
        return 77

    base = read_baseline()
    scripts = sorted(os.path.relpath(s, ROOT) for s in S.scripts())

    if a.freeze:
        rows = {}
        for rel in scripts:
            if not briefs_of(rel):
                continue
            args_json = (base[0].get(rel) if base else None) or '{"seqs":[172,173]}'
            m = measure(rel, args_json)
            rows[rel] = m
            print("  [FROZE] %s — %d agent prompt(s) from args %s"
                  % (rel, m["agents"], args_json))
            for b in sorted(m["delivers"]):
                print("           %s in %d prompt(s)" % (b, m["delivers"][b]))
        write_baseline(rows)
        print("workflow briefs delivered: froze %d script(s) in %s"
              % (len(rows), os.path.relpath(BASELINE, ROOT)))
        return 0

    if base is None:
        print("  [FAIL] %s does not exist, so no count says how many prompts "
              "should carry each brief.\n      Create it: "
              "tools/test_workflow_briefs_delivered.py --freeze"
              % os.path.relpath(BASELINE, ROOT))
        return 1
    b_args, b_agents, b_delivers = base

    bad = 0
    # The baseline names the scripts. A script renamed out of the glob is a
    # failure here, not an empty census.
    for rel in sorted(set(list(b_args) + scripts)):
        frozen = rel in b_args
        present = rel in scripts and os.path.exists(os.path.join(ROOT, rel))
        if frozen and not present:
            print("  [FAIL] %s is frozen in the baseline but the workflow "
                  "glob no longer finds it. A script renamed out of %s takes "
                  "its briefs with it."
                  % (rel, ", ".join(S.PATTERNS)))
            bad += 1
            continue
        if not frozen:
            if briefs_of(rel):
                print("  [FAIL] %s inlines a brief and is not in the frozen "
                      "baseline; freeze it so that forgetting the next one "
                      "fails." % rel)
                bad += 1
            continue

        m = measure(rel, b_args[rel])
        if m["err"]:
            print("  [FAIL] %s threw while rendering its prompts: %s"
                  % (rel, m["err"]))
            bad += 1
            continue
        if m["agents"] != b_agents.get(rel):
            print("  [FAIL] %s hands out %d agent prompt(s); the baseline "
                  "froze %d. The delivery counts below are measured against "
                  "a different run than the one frozen."
                  % (rel, m["agents"], b_agents.get(rel)))
            bad += 1
        want_briefs = set(k[1] for k in b_delivers if k[0] == rel)
        for brief in sorted(want_briefs | set(m["delivers"])):
            want = b_delivers.get((rel, brief))
            got = m["delivers"].get(brief)
            if got is None:
                print("  [FAIL] %s no longer inlines %s at all, so nothing "
                      "delivers it (%d prompt(s) carried it when frozen)"
                      % (rel, brief, want))
                bad += 1
                continue
            if got == -1:
                print("  [FAIL] %s inlines %s, which does not exist on disk"
                      % (rel, brief))
                bad += 1
                continue
            if want is None:
                print("  [FAIL] %s delivers %s to %d prompt(s), which is not "
                      "frozen anywhere" % (rel, brief, got))
                bad += 1
                continue
            if got == want:
                print("  [PASS] %s hands %s to %d of %d prompt(s), in full"
                      % (rel, brief, got, m["agents"]))
                continue
            bad += 1
            print("  [FAIL] %s hands %s to %d prompt(s); the baseline froze "
                  "%d" % (rel, brief, got, want))
            # A count alone is a useless diagnostic. Name the prompts, and
            # separate "the header is there and the text is not" -- which is
            # what an undelivered brief looks like from the inside -- from a
            # prompt that never mentioned the brief.
            header = "=== BRIEF: %s" % brief
            for c in m["calls"]:
                text = c["text"]
                full = io.open(os.path.join(ROOT, brief),
                               encoding="utf-8").read()
                if full in text:
                    continue
                label = c["label"] or "(unlabelled)"
                if header in text:
                    print("           %-24s announces the brief and does NOT "
                          "carry it (%d bytes of prompt)"
                          % (label, c["bytes"]))
                else:
                    print("           %-24s does not carry it (%d bytes)"
                          % (label, c["bytes"]))

    if bad:
        print("\nworkflow briefs delivered: %d failure(s)" % bad)
        print("  The brief is the authority for how a span is judged. A copy "
              "that is byte-perfect in the script and absent from the prompt "
              "governs nothing,\n  and it makes the prompt SMALLER, so "
              "nothing about it looks wrong. If the change is intended, "
              "re-measure and re-freeze:\n"
              "      tools/test_workflow_briefs_delivered.py --freeze")
        return 1
    print("workflow briefs delivered: %d script(s), every frozen brief "
          "reaches the prompts it was measured in, in full" % len(b_args))
    return 0


if __name__ == "__main__":
    sys.exit(main())
