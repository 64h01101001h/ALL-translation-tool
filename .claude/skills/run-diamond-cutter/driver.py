#!/usr/bin/env python3
"""driver.py — drive the Diamond Cutter Translation Tool without a mouse.

This is a Qt 6 desktop app with no web surface, so there is no
chromium-cli to borrow. What it DOES have is a set of headless modes
built into the binary itself, plus a static core library you can link
a throwaway probe against. This wraps both.

Run from the repo root:

    .claude/skills/run-diamond-cutter/driver.py <command>

Commands (every one of these was run to build this file):

    build            configure + build Release
    verify           build + 73 ctest suites + the constitution gate
    selftest         the app's own ~290 rendered-output pins
    shots [DIR]      22 pane screenshots (PNG) via the offscreen platform
    open FILE        open a real document in the Overlay; prints
                     timings and the segmentation hint
    sweep            click every control in every pane, crash-free check
    probe [FILE]     compile a C++ snippet against allcore and run it
                     (reads stdin when FILE is omitted) — the direct
                     invocation path; see SKILL.md
    paths            print what the driver resolved, and exit

Why a driver at all: `open build/…/DiamondCutter…app` puts a window on
Adam's screen and blocks. None of that is reachable from an agent
session. Everything here runs headless and prints.
"""
import os
import shlex
import subprocess
import sys
import tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
BUILD = os.environ.get("ALL_BUILD_DIR", os.path.join(ROOT, "cmake-build-release"))
# The build product is a .app BUNDLE, not a bare executable. Reaching
# for cmake-build-release/app/DiamondCutterTranslationTool gets you
# "no such file or directory"; the binary is two levels in.
BIN = os.path.join(BUILD, "app", "DiamondCutterTranslationTool.app",
                   "Contents", "MacOS", "DiamondCutterTranslationTool")
INSTALLED = ("/Applications/Diamond Cutter Translation Tool/"
             "Diamond Cutter Translation Tool.app/Contents/MacOS/"
             "DiamondCutterTranslationTool")


def sh(cmd, **kw):
    """Run a command, streaming output, and return its exit status.

    Never pipes through grep: a pipeline's status is the LAST command's,
    which is how this project twice reported '72/72 tests passed' over a
    build that had failed. See tools/verify.sh.
    """
    print("+ " + (cmd if isinstance(cmd, str) else " ".join(map(shlex.quote, cmd))),
          flush=True)
    return subprocess.call(cmd, shell=isinstance(cmd, str), cwd=ROOT, **kw)


def qt_env():
    """Offscreen platform: the app is a GUI, the agent has no display."""
    e = dict(os.environ)
    e["QT_QPA_PLATFORM"] = "offscreen"
    return e


def need_bin():
    if not os.path.exists(BIN):
        print("no built binary at:\n  %s\nrun:  driver.py build" % BIN)
        sys.exit(2)
    return BIN


def cmd_paths(argv):
    print("ROOT     %s" % ROOT)
    print("BUILD    %s" % BUILD)
    print("BIN      %s  %s" % (BIN, "(built)" if os.path.exists(BIN) else "(MISSING)"))
    print("INSTALLED %s  %s" % (INSTALLED,
                                "(present)" if os.path.exists(INSTALLED) else "(absent)"))
    return 0


def cmd_build(argv):
    rc = sh(["cmake", "-S", ROOT, "-B", BUILD])
    if rc:
        return rc
    return sh(["cmake", "--build", BUILD, "-j", os.environ.get("ALL_JOBS", "8")])


def cmd_verify(argv):
    # tools/verify.sh exists precisely because build-then-test chained by
    # hand let a stale binary answer for a failed build.
    return sh(["bash", os.path.join(ROOT, "tools", "verify.sh")] + argv)


def cmd_selftest(argv):
    return sh([need_bin(), "--selftest"], env=qt_env())


def cmd_shots(argv):
    out = os.path.abspath(argv[0]) if argv else os.path.join(
        tempfile.gettempdir(), "dctt-shots")
    os.makedirs(out, exist_ok=True)
    rc = sh([need_bin(), "--screenshots", out], env=qt_env())
    if rc == 0:
        pngs = sorted(f for f in os.listdir(out) if f.endswith(".png"))
        print("\n%d screenshot(s) in %s" % (len(pngs), out))
        for p in pngs:
            print("  " + p)
    return rc


def cmd_open(argv):
    if not argv:
        print("usage: driver.py open <file>")
        return 2
    # NOTE: --openprobe is NOT in the binary's g_harnessRun list, so the
    # dialog reaper is not armed for it. It is fine for opening a plain
    # text, but do not point it at a flow that raises a modal.
    return sh([need_bin(), "--openprobe", os.path.abspath(argv[0])], env=qt_env())


def cmd_sweep(argv):
    return sh([need_bin(), "--sweep"] + argv, env=qt_env())


PROBE_PRELUDE = """// auto-prepended by driver.py probe
#include <cstdio>
#include <chrono>
#include <string>
#include <vector>
"""


def cmd_probe(argv):
    """Compile a snippet against allcore and run it.

    This is the path most changes actually need. `allcore` holds the
    spine (SQLite/FTS5), the ported engines and the library index, and
    a 20-line probe answers questions the full GUI cannot be asked —
    'how long does this query take', 'what does this lookup return'.
    Today it measured a search at 3,310 ms / 40.7 MB that had been
    346,116 ms / 18.0 GB.
    """
    src = open(argv[0]).read() if argv else sys.stdin.read()
    if "int main" not in src:
        print("snippet needs an int main(); see SKILL.md for the shape")
        return 2
    lib = os.path.join(BUILD, "core", "liballcore.a")
    if not os.path.exists(lib):
        print("no liballcore.a — run: driver.py build")
        return 2
    with tempfile.TemporaryDirectory() as td:
        cpp = os.path.join(td, "probe.cpp")
        exe = os.path.join(td, "probe")
        with open(cpp, "w") as f:
            f.write(PROBE_PRELUDE + src)
        rc = sh(["clang++", "-std=c++20", "-O2",
                 "-I", os.path.join(ROOT, "core", "include"),
                 cpp, lib, "-lsqlite3", "-o", exe])
        if rc:
            return rc
        return sh([exe] + argv[1:])


COMMANDS = {
    "paths": cmd_paths, "build": cmd_build, "verify": cmd_verify,
    "selftest": cmd_selftest, "shots": cmd_shots, "open": cmd_open,
    "sweep": cmd_sweep, "probe": cmd_probe,
}

if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] not in COMMANDS:
        print(__doc__)
        sys.exit(0 if len(sys.argv) < 2 else 2)
    sys.exit(COMMANDS[sys.argv[1]](sys.argv[2:]))
