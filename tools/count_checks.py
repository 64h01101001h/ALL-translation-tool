#!/usr/bin/env python3
"""count_checks.py - how many automatic checks are there, and did they run?

WHY THIS EXISTS. Digest #9 told leadership "124 automatic checks, all passing".
Nothing in the repository produces 124. A fresh `ctest -N` gives 121 in
cmake-build-release and 119 in cmake-build, and the sixteen Python gates in
tools/test_*.py are already inside those totals - CMake registers them with the
`test_` prefix stripped, so grepping for the filename finds nothing and invites
you to add them a second time. 121 + 16 = 137 is the wrong answer arrived at
honestly, which is the worst kind.

That is the shape this repository keeps producing: a figure that was true when
someone counted, with nothing that re-counts it, quoted afterwards by everyone
including the person who wrote it down.

THE SECOND THING IT REPORTS, which matters more than the total. Suite
registration is CONDITIONAL - many add_test() calls sit inside if() blocks that
test for a spine database, a model file, or a fixture. A build directory
missing one of those silently registers fewer suites, and a suite that was
never registered cannot fail. That is why the two build directories disagree by
two. NOT RUN IS NOT PASSED, so this names the difference rather than averaging
over it.

THE ROOT CAUSE, found 2026-09-17. cmake-build-release was configured on
2026-09-15 22:49; core/CMakeLists.txt was last edited 2026-09-17 11:13. ctest
reads the test list written at CONFIGURE time, so every suite added in those
two days was invisible - `ctest -N` reported 121 and said nothing about the
three it had never heard of. The three were oracle_fixtures_in_sync,
oracle_never_raises and press_relaunch_trap: the gates written that same week
in response to three defects that had shipped silently. The checks written to
catch silent failure failed silently.

So --gate refuses on a build directory older than the CMake files that
describe it, and on any suite declared but registered nowhere. Both are
"not run is not passed" wearing a build-system hat.

Usage:
    python3 tools/count_checks.py                 # registered counts + the gap
    python3 tools/count_checks.py --log <file>    # also judge a ctest run
    python3 tools/count_checks.py --gate          # non-zero if stale or orphaned
    python3 tools/count_checks.py --gate --build cmake-build-release
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILDS = ["cmake-build-release", "cmake-build"]


def registered(build):
    """The suite names ctest would actually run in this build directory."""
    d = os.path.join(ROOT, build)
    if not os.path.isdir(d):
        return None
    try:
        out = subprocess.run(["ctest", "--test-dir", d, "-N"],
                             capture_output=True, text=True, timeout=300).stdout
    except (OSError, subprocess.SubprocessError):
        return None
    return set(re.findall(r"^\s*Test\s+#\d+:\s*(\S+)", out, re.M))


def declared():
    """Every add_test(NAME x) in the CMake files, conditional or not."""
    names = set()
    for base, _, files in os.walk(ROOT):
        if any(p in base for p in (".git", "cmake-build", "worktrees")):
            continue
        for f in files:
            if f != "CMakeLists.txt":
                continue
            with open(os.path.join(base, f), encoding="utf-8",
                      errors="replace") as fh:
                names |= set(re.findall(r"add_test\(\s*NAME\s+(\S+)", fh.read()))
    return names


def main(argv):
    decl = declared()
    print("suites declared by add_test() anywhere in CMake: %d" % len(decl))

    seen = {}
    for b in BUILDS:
        r = registered(b)
        if r is None:
            print("  %-22s (not configured)" % b)
            continue
        seen[b] = r
        print("  %-22s registers %d" % (b, len(r)))

    if len(seen) > 1:
        a, b = list(seen)[0], list(seen)[1]
        only_a, only_b = seen[a] - seen[b], seen[b] - seen[a]
        if only_a or only_b:
            print("\nthe build directories DISAGREE - a suite that is not "
                  "registered cannot fail, so this gap is silent:")
            for n in sorted(only_a):
                print("    only in %s: %s" % (a, n))
            for n in sorted(only_b):
                print("    only in %s: %s" % (b, n))

    never = decl - set().union(*seen.values()) if seen else decl
    if never:
        print("\ndeclared but registered NOWHERE (%d) - gated off in every "
              "build directory on this machine:" % len(never))
        for n in sorted(never):
            print("    %s" % n)

    best = max(seen, key=lambda k: len(seen[k])) if seen else None
    if best:
        print("\nTHE NUMBER TO QUOTE: %d checks (%s), and only after a run "
              "that passed." % (len(seen[best]), best))

    if "--gate" in argv:
        bad = 0
        # Only files git TRACKS. A walk of the tree also finds the generated
        # CMake files under build/ and cmake-build*/, which are rewritten by
        # every build and would make this gate cry stale forever - and a gate
        # that cries wolf gets switched off.
        try:
            tracked = subprocess.run(
                ["git", "-C", ROOT, "ls-files", "*CMakeLists.txt", "*.cmake"],
                capture_output=True, text=True, timeout=60).stdout.split()
        except (OSError, subprocess.SubprocessError):
            tracked = []
        cmake_files = [os.path.join(ROOT, f) for f in tracked
                       if os.path.exists(os.path.join(ROOT, f))]
        newest = max(cmake_files, key=os.path.getmtime) if cmake_files else None
        # Refuse only on the build directory ctest is running in - CMake
        # passes it as --build. A developer's other, unused build directory
        # being stale is true but not their problem right now, and a gate
        # that cries wolf gets switched off.
        want = None
        if "--build" in argv:
            want = os.path.basename(os.path.normpath(argv[argv.index("--build") + 1]))
        for b in seen:
            if want and b != want:
                continue
            # Compare against the file ctest ACTUALLY READS. CTestTestfile
            # .cmake is rewritten by every regenerate; CMakeCache.txt is NOT
            # - cmake leaves the cache alone when no cache entry changed. So
            # editing a COMMENT in core/CMakeLists.txt and then running the
            # very command this gate prints left the cache older than the
            # source, and the gate went on crying STALE at a build directory
            # that was perfectly current. The only way to silence it was to
            # touch the cache, which teaches the one reflex a staleness gate
            # must never teach. The cache is kept as the fallback for a
            # directory that has no test list at all.
            stamp = os.path.join(ROOT, b, "CTestTestfile.cmake")
            if not os.path.exists(stamp):
                stamp = os.path.join(ROOT, b, "CMakeCache.txt")
            if newest and os.path.exists(stamp) and \
                    os.path.getmtime(stamp) < os.path.getmtime(newest):
                print("\nSTALE: %s was configured before %s was last edited."
                      % (b, os.path.relpath(newest, ROOT)))
                print("  ctest reads the test list written at CONFIGURE time, "
                      "so any suite added since is INVISIBLE - it is not "
                      "failing, it does not exist. Reconfigure:")
                print("      cmake -S %s -B %s" % (ROOT, os.path.join(ROOT, b)))
                bad = 1
        if never:
            print("\nORPHANED: %d suite(s) are declared and registered nowhere "
                  "- they have never run." % len(never))
            bad = 1
        if not bad:
            print("\ngate: every build directory is current and every declared "
                  "suite is registered somewhere")
        return bad

    log = None
    if "--log" in argv:
        log = argv[argv.index("--log") + 1]
    if not log:
        return 0
    if not os.path.exists(log):
        print("\n--log %s does not exist - nothing was judged" % log)
        return 1
    text = open(log, encoding="utf-8", errors="replace").read()
    # ctest prints "100% tests passed out of 125" when nothing failed and
    # "98% tests passed, 3 tests failed out of 121" when something did. The
    # first version of this matched only the SECOND form, so it read a clean
    # run as "the run did not finish" - a checker that cries wolf on success,
    # which is precisely how a checker stops being consulted.
    m = re.search(r"(\d+)% tests passed, (\d+) tests failed out of (\d+)", text)
    if m:
        pct, failed, total = m.group(1), int(m.group(2)), int(m.group(3))
    else:
        m = re.search(r"(\d+)% tests passed out of (\d+)", text)
        if not m:
            print("\n%s holds no ctest summary - the run did not finish, and "
                  "an unfinished run is not a pass" % log)
            return 1
        pct, failed, total = m.group(1), 0, int(m.group(2))
    print("\nthe run in %s: %s of %s passed, %d failed"
          % (os.path.basename(log), total - failed, total, failed))
    if best and total != len(seen[best]):
        print("  NOTE: it ran %d suites but %s registers %d - the run and the "
              "count are not describing the same thing"
              % (total, best, len(seen[best])))
    if failed:
        for n in re.findall(r"^\s*\d+\s+-\s+(\S+)\s+\(Failed\)", text, re.M):
            print("    FAILED: %s" % n)
        print("  do not quote a passing number")
        return 1
    print("  safe to quote: %d checks, all passing" % total)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
