#!/usr/bin/env python3
"""Rebuild the browsable dictionary view from the current alignment layer.

Adam asked to see the new dictionary as it grows batch to batch, with a
slider for the depth layers. This regenerates docs/michael_dictionary.html
from whatever the layer currently holds, so republishing after a batch
shows the batch.

    python3 tools/build_dictionary_view.py
    # then: Artifact(file_path="docs/michael_dictionary.html", url=<same URL>)

Publishing to the SAME url keeps one link that updates in place. Passing a
new path, or omitting the url from a different conversation, creates a
second artifact instead.

The page carries the PROVISIONAL banner from the layer's own meta block,
so the tier label can never drift from what the layer actually claims.
"""
import json, io, os, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FULL = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
EVID = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")
SHELL = os.path.join(ROOT, "tools", "dictionary_view_shell.html")
OUT = os.path.join(ROOT, "docs", "michael_dictionary.html")


def main():
    links = json.load(io.open(FULL, encoding="utf-8"))["links"]
    ev = json.load(io.open(EVID, encoding="utf-8"))
    meta, acip = ev["meta"], ev.get("acip_index", {})

    byd = collections.defaultdict(
        lambda: collections.defaultdict(lambda: collections.defaultdict(list)))
    skipped = 0
    for l in links:
        d, t = l.get("d"), (l.get("tib") or "").strip()
        e, seg, co = (l.get("eng") or "").strip(), l.get("seg"), l.get("course")
        # a link with no English exponent is a null morpheme: correct on the
        # page, meaningless in a dictionary view.
        if not d or not t or not e or seg is None or not co:
            skipped += 1
            continue
        byd[d][t][e].append("%s:%s" % (co, seg))

    depths = {}
    for d in sorted(byd):
        ents = []
        for t, rend in byd[d].items():
            rl = sorted(([e, sorted(set(rs))] for e, rs in rend.items()),
                        key=lambda x: -len(x[1]))
            ents.append([t, acip.get(t) or "", rl])
        ents.sort(key=lambda x: (-len(x[2]), x[0].lower()))
        depths[d] = ents
        sys.stderr.write("  d=%d  %5d headwords  %6d renderings\n"
                         % (d, len(ents), sum(len(x[2]) for x in ents)))

    payload = {"meta": {"date": meta.get("date"), "tier": meta.get("tier"),
                        "corpus": meta.get("source_corpus"),
                        "rule": meta.get("rule")},
               "depths": depths}
    shell = io.open(SHELL, encoding="utf-8").read()
    data = json.dumps(payload, ensure_ascii=False, separators=(",", ":"))
    html = shell.replace("/*PAYLOAD*/", "const PAYLOAD = " + data + ";", 1)
    io.open(OUT, "w", encoding="utf-8").write(html)
    sys.stderr.write("\n  skipped %d links with no English exponent\n" % skipped)
    sys.stderr.write("  wrote %s (%.2f MB)\n" % (OUT, len(html) / 1048576.0))


if __name__ == "__main__":
    main()
