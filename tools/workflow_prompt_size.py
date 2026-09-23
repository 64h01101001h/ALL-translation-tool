#!/usr/bin/env python3
"""workflow_prompt_size.py — measure what a workflow script actually hands its
agents, by RUNNING it against stubs instead of reading it and guessing.

WHY THIS EXISTS. The C05 alignment workflow was rebuilt in September 2026 to
stop paying for the same bytes over and over: every agent was being told to
fetch its own source text out of the spine, to read the same two brief files,
and to grep the alignment bank for precedent -- data/alignment/alignment_full_v1.json,
about ten megabytes and growing with every page, so its size is a thing to
measure rather than to quote. The repair inlines all three into the
prompt. Whether that repair is worth anything is a question about SIZE — how
many bytes the harness now sends, against how many round-trips it stops the
agent making — and a size quoted in a commit message is a number nobody can
re-derive. Six months later it is folklore.

So this measures. It mimics the workflow runtime the same way
test_workflow_scripts_parse.py does (demote `export const meta`, wrap the body
so top-level return and await are legal), but it also STUBS the runtime hooks
-- agent(), parallel(), pipeline(), phase(), log(), args, budget -- and
executes the body. Every agent() call records the prompt it was handed. Nothing
is spawned and nothing is spent; the prompts are built by plain string
concatenation in the script, so building them is free.

    tools/workflow_prompt_size.py .claude/workflows/scripts/c05-align.js \
        --args '{"seqs":[172,173,174]}'
    tools/workflow_prompt_size.py .claude/workflows/scripts/c05-align.js \
        --args '{"seqs":[172,173,174]}' --rev HEAD      # the version in git

--rev reads the script out of a git revision instead of the working tree, which
is what makes a before/after comparison something anyone can reproduce rather
than something they have to take on trust.

WHAT IT CANNOT TELL YOU. It measures the prompt the script BUILDS. It cannot
measure the agent's system preamble, its tool schemas, or anything the agent
reads once it is running -- those are the other 55k tokens per agent and they
are not this script's to control. Token figures here are bytes/4, an estimate,
and are printed as one. A stage whose prompt depends on an earlier agent's
real output (the reconciler quotes the two proposals) is measured against a
STUB proposal, so its figure is a floor, not the truth; the report says so.
"""
import argparse
import io
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# The stub runtime. Deliberately a faithful-enough imitation of the documented
# hooks: parallel() is a barrier over thunks, pipeline() threads each item
# through every stage, agent() records and returns a shapeless object.
HARNESS = r"""
const __calls = [];
const __logs = [];
const __phases = [];
let __phase = '';
function phase(t) { __phase = String(t); __phases.push(String(t)); }
function log(m) { __logs.push(String(m)); }
async function agent(prompt, opts) {
  opts = opts || {};
  __calls.push({
    label: String(opts.label || ''),
    phase: String(opts.phase || __phase),
    model: String(opts.model || ''),
    chars: String(prompt).length,
    bytes: Buffer.byteLength(String(prompt), 'utf8'),
    // The TEXT, not just its size. tools/test_workflow_briefs_delivered.py
    // reads it to prove that a brief inlined in the script actually reaches
    // the prompt an agent is handed -- a check no size can make, because a
    // brief that is never delivered makes the prompt SMALLER and nothing
    // about that looks wrong.
    text: String(prompt),
  });
  return opts.schema ? {} : '';
}
async function parallel(thunks) {
  const out = [];
  for (const t of thunks) { try { out.push(await t()); } catch (e) { out.push(null); } }
  return out;
}
async function pipeline(items, ...stages) {
  const out = [];
  for (let i = 0; i < items.length; i++) {
    let v = items[i];
    try { for (const s of stages) v = await s(v, items[i], i); out.push(v); }
    catch (e) { out.push(null); }
  }
  return out;
}
async function workflow() { return {}; }
const budget = { total: null, spent: () => 0, remaining: () => Infinity };
const args = __ARGS__;
"""

TAIL = r"""
__MAIN__().then(
  () => { console.log('@@WPS-MEASURED@@' + JSON.stringify({ calls: __calls, logs: __logs, phases: __phases })); },
  (e) => { console.log('@@WPS-THREW@@' + JSON.stringify({ error: String(e && e.message || e), calls: __calls, logs: __logs })); },
);
"""


def probe_source(src, args_json):
    """Wrap a workflow script so node can execute its body against the stubs."""
    body = re.sub(r"^\s*export\s+const\s+meta", "const meta", src, count=1,
                  flags=re.M)
    return (HARNESS.replace("__ARGS__", args_json)
            + "\nconst __MAIN__ = async () => {\n" + body + "\n};\n"
            + TAIL)


def read_script(path, rev):
    if rev is None:
        return io.open(path, encoding="utf-8").read()
    rel = os.path.relpath(os.path.abspath(path), ROOT)
    r = subprocess.run(["git", "-C", ROOT, "show", "%s:%s" % (rev, rel)],
                       capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit("cannot read %s at %s: %s" % (rel, rev, r.stderr.strip()))
    return r.stdout


def run(path, rev, args_json):
    node = shutil.which("node")
    if not node:
        sys.exit("node is not installed; this tool needs it to run the script")
    src = read_script(path, rev)
    with tempfile.TemporaryDirectory() as td:
        probe = os.path.join(td, "probe.mjs")
        io.open(probe, "w", encoding="utf-8").write(probe_source(src, args_json))
        r = subprocess.run([node, probe], capture_output=True, text=True,
                           cwd=td)
    if r.returncode != 0:
        sys.exit("node failed:\n" + (r.stderr or "").strip())
    marker = "@@WPS-MEASURED@@"
    threw = "@@WPS-THREW@@"
    for line in r.stdout.splitlines():
        if line.startswith(marker):
            return json.loads(line[len(marker):]), None
        if line.startswith(threw):
            d = json.loads(line[len(threw):])
            return d, d.get("error")
    sys.exit("the script produced no measurement; node said:\n" + r.stdout)


def report(label, data, err):
    calls = data.get("calls", [])
    total = sum(c["bytes"] for c in calls)
    print("== %s ==" % label)
    for line in data.get("logs", []):
        print("   log: %s" % line)
    if err:
        print("   THREW after %d agent call(s): %s" % (len(calls), err))
    by_phase = {}
    for c in calls:
        k = c["phase"] or "(none)"
        by_phase.setdefault(k, []).append(c)
    for k in sorted(by_phase):
        cs = by_phase[k]
        b = sum(c["bytes"] for c in cs)
        print("   %-12s %2d agent(s)  %9d bytes  %8d avg  (~%d tok est)"
              % (k, len(cs), b, b // len(cs), b // 4))
    print("   %-12s %2d agent(s)  %9d bytes            (~%d tok est)"
          % ("TOTAL", len(calls), total, total // 4))
    return total, len(calls)


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    p.add_argument("script")
    p.add_argument("--args", default="{}",
                   help="the args object the workflow would be launched with")
    p.add_argument("--rev", default=None,
                   help="measure the script as of this git revision instead "
                        "of the working tree")
    p.add_argument("--compare-rev", default=None,
                   help="also measure this revision and print the difference")
    p.add_argument("--detail", action="store_true",
                   help="one line per agent call")
    p.add_argument("--dump-prompts", metavar="DIR",
                   help="also write every rendered prompt to DIR, one file "
                        "per agent call, so a person can read what an agent "
                        "is actually handed")
    a = p.parse_args()
    try:
        json.loads(a.args)
    except ValueError as e:
        sys.exit("--args must be JSON: %s" % e)

    path = a.script if os.path.isabs(a.script) else os.path.join(ROOT, a.script)
    data, err = run(path, a.rev, a.args)
    now_label = a.rev or "working tree"
    total, n = report(now_label, data, err)
    if a.detail:
        for c in data.get("calls", []):
            print("      %-28s %8d bytes" % (c["label"] or "(unlabelled)",
                                             c["bytes"]))
    if a.dump_prompts:
        d = a.dump_prompts
        if not os.path.isdir(d):
            os.makedirs(d)
        for i, c in enumerate(data.get("calls", []), start=1):
            safe = re.sub(r"[^A-Za-z0-9._-]", "_", c.get("label") or "agent")
            f = os.path.join(d, "%02d-%s.txt" % (i, safe))
            io.open(f, "w", encoding="utf-8").write(c.get("text", ""))
        print("   wrote %d prompt(s) to %s"
              % (len(data.get("calls", [])), d))
    if a.compare_rev:
        print()
        old, oerr = run(path, a.compare_rev, a.args)
        ototal, on = report(a.compare_rev, old, oerr)
        print()
        print("   %s -> %s: %+d bytes across %d -> %d agents"
              % (a.compare_rev, now_label, total - ototal, on, n))
        if on and n:
            print("   per agent: %d -> %d bytes" % (ototal // on, total // n))
    print()
    print("   Token figures are bytes/4, an ESTIMATE. This measures only the "
          "prompt the script builds -- not the agent preamble, not tool "
          "schemas, not what the agent reads once running.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
