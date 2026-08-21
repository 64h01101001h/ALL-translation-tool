#!/usr/bin/env python3
"""harvest_bdrc_outline.py — recursively walk a BDRC IIIF outline
(wio: collection tree) and record every node's id, label, parent,
and depth. The Sungbum pilot: MW22109 (Tsongkhapa's 27-volume
gsung 'bum) — per-text titles live at the leaves of a nested tree
(volume → section → text), so a flat parts probe finds nothing.

  usage:  harvest_bdrc_outline.py [MW-id]        (default MW22109)
  output: data/extracted/<mwid>_outline.json

Polite pacing (1.2s per request); nodes are fetched once.
"""
import json, os, sys, time, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MW = sys.argv[1] if len(sys.argv) > 1 else "MW22109"
OUT = os.path.join(ROOT, "data", "extracted",
                   MW.lower() + "_outline.json")
BASE = "https://iiifpres.bdrc.io/collection/wio:bdr:"


def lab(x):
    l = x.get("label")
    if isinstance(l, list):
        l = l[0].get("@value") if l else ""
    elif isinstance(l, dict):
        l = l.get("@value")
    return l or ""


def fetch(cid):
    try:
        with urllib.request.urlopen(BASE + cid, timeout=30) as r:
            return json.load(r)
    except Exception as e:
        print("  fetch failed", cid, e)
        return None


def main():
    nodes = []
    seen = set()
    queue = [(MW, None, 0)]
    while queue:
        cid, parent, depth = queue.pop(0)
        if cid in seen:
            continue
        seen.add(cid)
        d = fetch(cid)
        time.sleep(1.2)
        if not d:
            continue
        for c in d.get("collections", []):
            sub = c.get("@id", "").split(":")[-1]
            nodes.append({"id": sub, "label": lab(c),
                          "parent": cid, "depth": depth + 1,
                          "kind": "collection"})
            queue.append((sub, cid, depth + 1))
        for m in d.get("manifests", []):
            nodes.append({"id": m.get("@id", ""),
                          "label": lab(m), "parent": cid,
                          "depth": depth + 1, "kind": "manifest"})
        if len(seen) % 20 == 0:
            print(f"  {len(seen)} nodes fetched, "
                  f"{len(queue)} queued")
    json.dump({"meta": {"root": MW, "nodes": len(nodes),
                        "harvested": "2026-08-13"},
               "nodes": nodes}, open(OUT, "w"),
              ensure_ascii=False, indent=1)
    print(f"{len(nodes)} outline nodes -> {OUT}")


if __name__ == "__main__":
    main()
