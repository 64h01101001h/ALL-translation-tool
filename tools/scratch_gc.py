#!/usr/bin/env python3
"""Report — and optionally reclaim — stale Claude working scratch.

Why this exists (2026-08-24). The data volume fell from 14 GiB free to
327 MiB in a single working day. None of it was the project: it was
~32 GB under /private/tmp/claude-501 left by a 15-agent assessment and
six remediation tracks, each of which copied the source tree and built
it. Nothing has ever cleaned that up.

It matters more than housekeeping. FAIL-1's whole family of defects are
ENOSPC bugs - a store reporting success over a write that never landed.
Running this project low on disk puts the app in the exact regime its
worst data-integrity bugs live in, so a full volume does not merely
stop work, it makes the product look healthy while it is not.

DEFAULT IS REPORT-ONLY. Deleting someone's working files because a
number looked large is its own kind of damage, so --reclaim is
explicit, it never touches the session that is running now, and it
never touches anything modified inside the idle window.

Usage:
  tools/scratch_gc.py                      # report
  tools/scratch_gc.py --reclaim            # delete stale session dirs
  tools/scratch_gc.py --reclaim --hours 6  # widen/narrow "stale"
  tools/scratch_gc.py --selftest
"""
import argparse
import os
import shutil
import sys
import tempfile
import time

ROOT = "/private/tmp/claude-501"


def dir_size(path):
    total = 0
    for dirpath, _dirs, files in os.walk(path):
        for f in files:
            fp = os.path.join(dirpath, f)
            try:
                if not os.path.islink(fp):
                    total += os.path.getsize(fp)
            except OSError:
                pass
    return total


def newest_mtime(path):
    newest = 0.0
    for dirpath, _dirs, files in os.walk(path):
        for f in files:
            fp = os.path.join(dirpath, f)
            try:
                newest = max(newest, os.path.getmtime(fp))
            except OSError:
                pass
    return newest


def human(n):
    for unit in ("B", "KB", "MB", "GB"):
        if n < 1024 or unit == "GB":
            return "%.1f %s" % (n, unit)
        n /= 1024.0


def sessions(root):
    """Every session scratch dir under root, deepest project level."""
    out = []
    if not os.path.isdir(root):
        return out
    for project in sorted(os.listdir(root)):
        pdir = os.path.join(root, project)
        if not os.path.isdir(pdir):
            continue
        for sess in sorted(os.listdir(pdir)):
            sdir = os.path.join(pdir, sess)
            if os.path.isdir(sdir):
                out.append(sdir)
    return out


def report(root, hours, reclaim, current_session):
    now = time.time()
    cutoff = now - hours * 3600
    rows = []
    for sdir in sessions(root):
        size = dir_size(sdir)
        if size == 0:
            continue
        mt = newest_mtime(sdir)
        idle_h = (now - mt) / 3600.0 if mt else 999.0
        is_current = current_session and current_session in sdir
        rows.append((size, idle_h, sdir, is_current))
    rows.sort(reverse=True)

    total = sum(r[0] for r in rows)
    print("claude scratch under %s: %s across %d session(s)"
          % (root, human(total), len(rows)))
    freed = 0
    for size, idle_h, sdir, is_current in rows:
        tag = "  [CURRENT SESSION - never reclaimed]" if is_current else ""
        stale = (not is_current) and idle_h >= hours
        mark = "STALE" if stale else "     "
        print("  %s %9s  idle %5.1fh  %s%s"
              % (mark, human(size), idle_h, os.path.basename(sdir)[:20], tag))
        if reclaim and stale:
            try:
                shutil.rmtree(sdir)
                freed += size
                print("           reclaimed")
            except OSError as e:
                print("           could NOT reclaim: %s" % e)
    if reclaim:
        print("reclaimed %s" % human(freed))
    elif any((not r[3]) and r[1] >= hours for r in rows):
        print("run with --reclaim to delete the STALE rows "
              "(the current session is never touched)")
    return 0


def cmd_selftest():
    """Prove the two rules that make --reclaim safe to exist."""
    bad = []
    root = tempfile.mkdtemp(prefix="scratch_gc_selftest_")
    proj = os.path.join(root, "proj")
    fresh = os.path.join(proj, "fresh-session")
    stale = os.path.join(proj, "stale-session")
    for d in (fresh, stale):
        os.makedirs(d)
        with open(os.path.join(d, "f.bin"), "w") as fh:
            fh.write("x" * 1024)
    # make one of them old
    old = time.time() - 48 * 3600
    os.utime(os.path.join(stale, "f.bin"), (old, old))

    found = sessions(root)
    if len(found) != 2:
        bad.append("sessions() must find both session dirs")

    # the current session is never stale, however old it looks
    report(root, hours=1, reclaim=True, current_session="stale-session")
    if not os.path.isdir(stale):
        bad.append("the CURRENT session was reclaimed - it must never be, "
                   "however idle it looks")
    if not os.path.isdir(fresh):
        bad.append("a session inside the idle window was reclaimed")

    # and with no current-session protection, the stale one goes
    report(root, hours=1, reclaim=True, current_session=None)
    if os.path.isdir(stale):
        bad.append("a genuinely stale session was NOT reclaimed")
    if not os.path.isdir(fresh):
        bad.append("a fresh session was reclaimed - the idle window is "
                   "the whole safety mechanism")

    shutil.rmtree(root, ignore_errors=True)
    for b in bad:
        print("  [FAIL]", b)
    print("scratch_gc selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=ROOT)
    ap.add_argument("--hours", type=float, default=12.0,
                    help="a session idle this long is stale (default 12)")
    ap.add_argument("--reclaim", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return cmd_selftest()
    return report(a.root, a.hours, a.reclaim,
                  os.environ.get("CLAUDE_CODE_SESSION_ID"))


if __name__ == "__main__":
    sys.exit(main())
