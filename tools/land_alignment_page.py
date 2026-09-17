#!/usr/bin/env python3
"""land_alignment_page.py — turn reconciled per-segment specs into a landed page.

WHY THIS IS A TOOL. tools/wrap_alignment_page.py was promoted out of a
scratchpad in August for exactly this reason: the page-production pipeline
could not be reproduced from a clean checkout, because the generator was
tracked and its wrapper was not. The same was true one step further out. From
the reconciler's spec files to a landed page there were six manual steps —
merge the specs, run the generator, pipe through the wrapper with a hand-typed
title, register the page in build_alignment_layer.py, add any span-head
licensor, rerun the builder — and doing them by hand is how a page number gets
mistyped or a registration forgotten.

WHAT IT REFUSES TO DO. It will not invent a title, and it will not register a
page whose segments are not contiguous or do not match the specs it was given.
It runs the generator and stops on a non-zero exit, because a refusal is the
gate doing its job and the fix belongs in the spec.

Usage:
  land_alignment_page.py --page c5p57 --seqs 169,170,171 \
      --specs <dir> --title "<short description>" [--prev c5p56] [--dry-run]

The title is the human half — a few words naming what the page is about. The
subtitle is composed from the reconcilers' own per-segment titles.
"""
import argparse
import io
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def die(msg):
    sys.exit("REFUSED: " + msg)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--page", required=True)          # c5p57
    ap.add_argument("--seqs", required=True)          # 169,170,171
    ap.add_argument("--specs", required=True)         # dir of <seq>-reconciled.json
    ap.add_argument("--title", required=True)
    ap.add_argument("--prev", default="-")
    ap.add_argument("--next", default="-")
    ap.add_argument("--course", default="C05")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()

    seqs = [int(x) for x in a.seqs.split(",")]
    if seqs != list(range(seqs[0], seqs[0] + len(seqs))):
        die("segments are not contiguous: %s" % seqs)
    m = re.fullmatch(r"c(\d)p(\d+)", a.page)
    if not m:
        die("page id must look like c5p57, got %r" % a.page)
    course_no = m.group(1)

    segs = []
    for q in seqs:
        p = os.path.join(a.specs, "%d-reconciled.json" % q)
        if not os.path.exists(p):
            die("no reconciled spec for segment %d at %s" % (q, p))
        d = json.load(io.open(p, encoding="utf-8"))
        for s in d.get("segments", []):
            if s.get("seq") != q:
                die("spec %s carries seq %s, expected %d" % (p, s.get("seq"), q))
            segs.append(s)
    if len(segs) != len(seqs):
        die("expected %d segments, assembled %d" % (len(seqs), len(segs)))

    spec = {"course": a.course, "segments": segs}
    outdir = os.path.join(ROOT, "data", "alignment", "pages_c0%s" % course_no)
    page_path = os.path.join(outdir, a.page + ".html")

    sub = " &middot; ".join((s.get("title") or "").strip() for s in segs)
    sub += " &mdash; segments %d&ndash;%d" % (seqs[0], seqs[-1])
    title = "C%s&middot;P%s &mdash; %s (%s:%d&ndash;%d)" % (
        course_no, m.group(2), a.title, a.course, seqs[0], seqs[-1])

    print("  page     %s" % a.page)
    print("  segments %s (%d spans)" % (seqs, sum(len(s.get("spans") or []) for s in segs)))
    print("  title    %s" % title)

    # flag anything the span-head gate will refuse, before it does
    BAD = {"the", "a", "an", "and", "or", "his", "our", "your", "i", "you"}
    for s in segs:
        for sp in s.get("spans") or []:
            e = (sp.get("eng") or "").strip()
            if sp.get("d") in (5, 7) and e and e.split()[0].lower().strip('".,') in BAD:
                print("  LICENSOR NEEDED: %s/s%dw?? id=%s d=%s tib=%r eng=%r"
                      % (a.page, s["seq"], sp.get("id"), sp.get("d"), sp.get("tib"), e))
                print("     -> add \"%s/s%d%s\" to data/alignment/span_head_allow.json"
                      % (a.page, s["seq"], sp.get("id")))

    if a.dry_run:
        print("  (dry run — nothing written)")
        return 0

    gen = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "gen_alignment_page.py")],
                         input=json.dumps(spec, ensure_ascii=False),
                         capture_output=True, text=True, cwd=ROOT)
    if gen.returncode != 0:
        die("generator exited %d — fix the spec, not the rule:\n%s"
            % (gen.returncode, gen.stderr.strip()[:600]))
    wrap = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "wrap_alignment_page.py"),
         page_path, title, sub, a.prev, a.next, course_no],
        input=gen.stdout, capture_output=True, text=True, cwd=ROOT)
    if wrap.returncode != 0:
        die("wrapper exited %d:\n%s" % (wrap.returncode, wrap.stderr.strip()[:400]))
    print("  wrote %s (%d bytes)" % (os.path.relpath(page_path, ROOT),
                                     os.path.getsize(page_path)))

    # register the page in the builder, next to the previous one
    bp = os.path.join(ROOT, "tools", "build_alignment_layer.py")
    b = io.open(bp, encoding="utf-8").read()
    if '"%s"' % a.page in b:
        print("  already registered in build_alignment_layer.py")
    else:
        prev = a.prev if a.prev != "-" else None
        anchor = None
        if prev:
            anchor = re.search(r'("%s":\s*\[[^\]]*\])' % re.escape(prev), b)
        if not anchor:
            die("cannot find %r in build_alignment_layer.py to register after"
                % (prev or "(no --prev given)"))
        b = b.replace(anchor.group(1),
                      '%s, "%s": %s' % (anchor.group(1), a.page, json.dumps(seqs)), 1)
        io.open(bp, "w", encoding="utf-8").write(b)
        print("  registered %s -> %s" % (a.page, seqs))
    print("  now run: python3 tools/build_alignment_layer.py")
    return 0


if __name__ == "__main__":
    sys.exit(main())
