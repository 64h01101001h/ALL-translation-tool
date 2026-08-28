#!/usr/bin/env python3
"""Merge structured errata from an alignment batch into the register sidecar.

The first register was built by grepping prose out of campaign logs, and it
cost a verification pass in which FIFTEEN segment references and FOUR
proposed corrections turned out to be wrong. Batches now emit errata as
structured records instead, so nothing has to be recovered from narrative
again.

This merges those records into docs/errata_register.json, assigns stable
ids, and REFUSES to add an entry whose quoted text is not actually in the
spine at the cited segment. That last check is the point: it makes the
misquotation class impossible rather than merely discouraged.

usage: merge_errata.py <batch.json> [--course C03] [--apply]
       without --apply it reports what it would do and changes nothing.
"""
import json, io, os, sys, sqlite3, argparse

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402

SIDECAR = os.path.join(ROOT, "docs", "errata_register.json")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("batch")
    ap.add_argument("--course", default="C03")
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()

    batch = json.load(io.open(a.batch, encoding="utf-8"))
    incoming = batch if isinstance(batch, list) else batch.get("errata", [])
    if not incoming:
        print("no errata in this batch — the normal outcome")
        return

    reg = json.load(io.open(SIDECAR, encoding="utf-8"))
    have = {(e.get("segment"), e.get("found", "")[:60]) for e in reg}
    nextid = 1 + max((int(e["item_id"].split("-")[1])
                      for e in reg if e.get("item_id", "").startswith("E-")),
                     default=0)

    db = sqlite3.connect(B.SPINE)
    c = db.cursor()
    added, refused, dup = [], [], 0
    for e in incoming:
        seq = e.get("seq")
        seg = "%s:%s" % (a.course, seq)
        c.execute("select wylie, english from corpus_segments "
                  "where course=? and seq=?", (a.course, int(seq)))
        row = c.fetchone()
        if not row:
            refused.append((seg, "segment not in the spine"))
            continue
        found = (e.get("found") or "").strip()
        # THE CHECK THAT MATTERS: the quoted text must actually be there.
        if found and found not in row[0] and found not in row[1]:
            refused.append((seg, "quoted text %r is not in the spine at this "
                                 "segment — misquoted or mis-cited" % found[:48]))
            continue
        if (seg, found[:60]) in have:
            dup += 1
            continue
        rec = {"item_id": "E-%03d" % nextid, "course": a.course,
               "segment": seg, "kind": e.get("kind"), "found": found,
               "expected": e.get("expected", ""), "evidence": e.get("evidence", ""),
               "severity": e.get("severity"), "confidence": e.get("confidence"),
               "note": "filed from the alignment batch, spine-verified on merge"}
        added.append(rec)
        nextid += 1

    print("incoming %d | would add %d | duplicate %d | REFUSED %d"
          % (len(incoming), len(added), dup, len(refused)))
    for seg, why in refused:
        print("  REFUSED %-10s %s" % (seg, why))
    for r in added:
        print("  add %-8s %-10s %-8s %-9s %s"
              % (r["item_id"], r["segment"], r["severity"], r["confidence"],
                 r["found"][:44]))
    if a.apply and added:
        reg.extend(added)
        json.dump(reg, io.open(SIDECAR, "w", encoding="utf-8"),
                  ensure_ascii=False, indent=1)
        print("\nwrote %d new entries to %s" % (len(added), SIDECAR))
    elif added:
        print("\n(dry run — pass --apply to write)")


if __name__ == "__main__":
    main()
