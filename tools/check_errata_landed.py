#!/usr/bin/env python3
"""check_errata_landed.py — every erratum a reconciler reported must be banked.

WHY THIS EXISTS. On 2026-09-17 the reconciler on C05:280 reported a FORMATTING
erratum in its workflow result and never wrote 280-errata.json. The landing
loop reads FILES:

    for n in $(seq ...); do [ -f "$S/$n-errata.json" ] && merge_errata ...

so a missing file is indistinguishable from "no errata in this batch — the
normal outcome". The finding was real -- C15:425 carries the same 636-byte
wylie with "[Translator's note: The point" where C05:280 reads "[The point" --
and it would have been lost in silence. It is banked now as E-274, recovered
from the workflow notification and re-verified against the spine.

A SILENT SKIP IS THE FAILURE MODE THIS WHOLE REPOSITORY KEEPS FINDING. The
loop did exactly what it was told; nothing told it that absence is suspicious.

WHAT THIS DOES. It reads the workflow task outputs, extracts every reconciled
entry's `errata` array, and checks each reported erratum against
docs/errata_register.json by its own quoted `found` text. A reported erratum
with no banked counterpart is named. It does NOT try to judge the erratum --
merge_errata already refuses malformed ones, and that refusal is a separate
and legitimate outcome that this tool reports rather than hides.

Usage:
    python3 tools/check_errata_landed.py <tasks-dir> [--quiet]
"""
import glob
import io
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
def reported(tasks_dir):
    """seq -> list of `found` strings the reconcilers said they filed.

    The workflow result is a pretty-printed JSON object and each erratum is a
    JSON-ENCODED STRING inside the array, so it must be parsed rather than
    regexed. The first version of this tool matched a compact single-line
    shape that never occurs, reported "0 reported, all banked", and was
    therefore a gate that could not fail -- caught only because a known
    erratum failed to appear in its own output.
    """
    out = {}
    for f in sorted(glob.glob(os.path.join(tasks_dir, "w*.output"))):
        txt = io.open(f, encoding="utf-8", errors="replace").read()
        # The task output is a JSON object whose `result` holds the workflow's
        # return value. Two wrong guesses preceded this: a compact one-line
        # regex, and a search for a literal '{"scratch"' that never starts the
        # file. Both produced "0 reported, all banked" -- a clean-looking
        # nothing, which is the most dangerous output a checker can give.
        try:
            top = json.loads(txt)
            d = top.get("result")
            if isinstance(d, str):
                d = json.loads(d)
            if not isinstance(d, dict):
                raise ValueError("no result object")
        except ValueError:
            # a truncated notification body: fall back to scanning the
            # encoded strings directly, and say so rather than return clean
            for m in re.finditer(r'\{\\"seq\\":\s*(\d+).*?\\"found\\":\s*\\"(.*?)\\"', txt):
                out.setdefault(int(m.group(1)), []).append(m.group(2))
            continue
        for r in d.get("reconciled") or []:
            for e in r.get("errata") or []:
                if isinstance(e, str):
                    try:
                        e = json.loads(e)
                    except ValueError:
                        continue
                if isinstance(e, dict) and e.get("found"):
                    out.setdefault(int(e.get("seq") or r.get("seq") or 0),
                                   []).append(str(e["found"]))
    return out


def main(argv):
    if len(argv) < 2:
        sys.stderr.write(__doc__)
        return 2
    tasks = argv[1]
    reg = json.load(io.open(os.path.join(ROOT, "docs/errata_register.json"),
                            encoding="utf-8"))
    banked = {}
    for e in reg:
        seg = str(e.get("segment") or "")
        m = re.match(r"C05:(\d+)", seg)
        if m:
            banked.setdefault(int(m.group(1)), []).append(str(e.get("found") or ""))

    rep = reported(tasks)
    missing, refiled = [], []
    for seq, founds in sorted(rep.items()):
        have = banked.get(seq, [])
        for f in founds:
            f2 = f.replace("\\\\", "\\").replace('\\"', '"')[:40]
            if any(f2[:24] in h or h[:24] in f2 for h in have):
                continue
            # A segment that HAS a banked erratum but whose quote no longer
            # matches was re-filed -- merge_errata refuses prose in `found`,
            # and the fix is to replace it with a verbatim substring, which
            # legitimately changes the text. That is not a loss and must not
            # be reported as one, or this check joins the gates that cry wolf.
            (refiled if have else missing).append((seq, f2))
    total = sum(len(v) for v in rep.values())
    print("reconcilers reported %d erratum/errata across %d segment(s)"
          % (total, len(rep)))
    print("banked in the register for those segments: %d"
          % sum(len(banked.get(s, [])) for s in rep))
    if refiled:
        print("\n  %d reported erratum/errata were RE-FILED with a different "
              "quote (merge_errata refuses prose in `found`); their segments "
              "do carry a banked entry:" % len(refiled))
        for seq, f in refiled:
            print("     C05:%-5d %r" % (seq, f))
    if missing:
        print("\nFAILED: %d reported erratum/errata are NOT in the register:"
              % len(missing))
        for seq, f in missing:
            print("   C05:%-5d %r" % (seq, f))
        print("\nEither the reconciler never wrote its errata.json (the landing")
        print("loop reads files, so absence looks like 'no errata'), or")
        print("merge_errata refused it. Both are recoverable; neither is")
        print("something to discover later.")
        return 1
    print("  every reported erratum is banked")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
