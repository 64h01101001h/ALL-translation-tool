#!/usr/bin/env python3
"""Repair-round census of the alignment campaign's workflow agents (read-only).

Reads Claude Code transcripts of workflow agents and answers: how often did a
propose/reconcile agent actually RUN the generator, how often was it REFUSED,
how often did the harness reject the StructuredOutput for schema mismatch, and
how many tool calls merely READ the generator's source.

Role comes from the journal's result.angle (tibetan-first/english-first =
propose, anything else = reconcile). Usage is de-duplicated per message id,
taking the max of each meter (transcripts log one line per content block).
Nothing is written anywhere; output goes to stdout.
"""
import collections, glob, json, os, re, statistics

BASE = os.path.expanduser(
    "~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool")
EXEC = re.compile(r"python3?\s+\S*gen_alignment_page\.py")
REFUSED = re.compile(r"REFUSED: s?\d")


def text_of(c):
    if isinstance(c, str):
        return c
    if isinstance(c, list):
        return "\n".join(x.get("text", "") for x in c if isinstance(x, dict))
    return ""


role = {}
for j in glob.glob(BASE + "/*/subagents/workflows/wf_*/journal.jsonl"):
    for line in open(j):
        try:
            o = json.loads(line)
        except ValueError:
            continue
        r = o.get("result")
        if o.get("type") == "result" and isinstance(r, dict) and r.get("angle"):
            role[o["agentId"]] = ("propose" if r["angle"] in
                                  ("tibetan-first", "english-first") else "reconcile")

C = collections.defaultdict(collections.Counter)
n = collections.Counter()
days, models = set(), collections.Counter()
for f in glob.glob(BASE + "/*/subagents/workflows/wf_*/agent-*.jsonl"):
    r = role.get(os.path.basename(f)[6:-6])
    if not r:
        continue
    n[r] += 1
    pend, usage, first = {}, {}, None
    for line in open(f):
        try:
            o = json.loads(line)
        except ValueError:
            continue
        if o.get("timestamp"):
            days.add(o["timestamp"][:10])
        m = o.get("message", {})
        if o.get("type") == "assistant" and m.get("usage"):
            models[m.get("model")] += 1
            u, prev = m["usage"], usage.get(m.get("id"), {})
            usage[m.get("id")] = {k: max(prev.get(k, 0), u.get(k) or 0) for k in
                                  ("cache_read_input_tokens",
                                   "cache_creation_input_tokens",
                                   "output_tokens", "input_tokens")}
        c = m.get("content")
        if not isinstance(c, list):
            continue
        for b in c:
            if not isinstance(b, dict):
                continue
            if b.get("type") == "tool_use":
                C[r]["tool_calls"] += 1
                inp = b.get("input") if isinstance(b.get("input"), dict) else {}
                cmd = inp.get("command", "") or ""
                if b.get("name") == "StructuredOutput":
                    pend[b["id"]] = "so"; C[r]["structured_output_calls"] += 1
                elif EXEC.search(cmd):
                    pend[b["id"]] = "gen"; C[r]["generator_runs"] += 1
                elif "gen_alignment_page" in json.dumps(inp):
                    C[r]["generator_source_reads"] += 1
            elif b.get("type") == "tool_result":
                k, t = pend.get(b.get("tool_use_id")), text_of(b.get("content"))
                if k == "so" and "does not match required schema" in t:
                    C[r]["schema_rejections"] += 1
                if k == "gen":
                    ref = bool(REFUSED.search(t)) and "+ msg" not in t
                    C[r]["generator_refusals"] += ref
                    if first is None:
                        first = ref
    C[r]["first_run_refused"] += bool(first)
    for v in usage.values():
        for k, x in v.items():
            C[r][k] += x
    C[r]["requests"] += len(usage)

print("transcript days:", sorted(days), " models:", dict(models))
for r in sorted(n):
    print("\n%s agents: %d" % (r, n[r]))
    for k, v in sorted(C[r].items()):
        print("  %-30s total %10d   per agent %10.2f" % (k, v, v / n[r]))
