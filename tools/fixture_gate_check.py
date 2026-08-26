#!/usr/bin/env python3
"""fixture_gate_check.py — the battery must say what it did not run.

SQA 2026-08-22, BUILD-7: 37 of 73 suites need data .gitignore excludes,
so "72/72 passed" was only true on one laptop. cmake/AllFixtureTests.cmake
turns an absent fixture into a ctest SKIP that names the missing path.
This suite holds three claims about that machinery:

  A. every untracked fixture path the battery references is documented
     in docs/FIXTURES.md (so a skip message is followed by a page that
     says how to get the data back);
  B. in THIS build tree, every registered suite that reads an untracked
     fixture carries the ctest label "fixture" (so `ctest -LE fixture`
     is a real clean-checkout gate), and every suite whose fixture is
     absent is a skip stub with SKIP_REGULAR_EXPRESSION set;
  C. --selftest: a throwaway CMake project, configured against the real
     cmake/AllFixtureTests.cmake, proves an absent fixture becomes
     ***Skipped and a present one still runs. This is the part that
     goes red if the wrapper is removed.

Usage:
    fixture_gate_check.py <source_dir> <binary_dir> <cmake> <ctest>
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

UNTRACKED_ROOTS = ("build", "library", "data/extracted", "data/das",
                   "data/teaching")

failures = 0


def check(cond, msg):
    global failures
    if cond:
        print("  [PASS] %s" % msg)
    else:
        print("  [FAIL] %s" % msg)
        failures += 1


def add_test_blocks(text):
    """Yield the source text of each add_test(...) call, parens balanced."""
    i = 0
    while True:
        j = text.find("add_test(", i)
        if j < 0:
            return
        depth = 0
        k = j + len("add_test")
        while k < len(text):
            if text[k] == "(":
                depth += 1
            elif text[k] == ")":
                depth -= 1
                if depth == 0:
                    break
            k += 1
        yield text[j:k + 1]
        i = k + 1


def under_untracked(rel):
    return any(rel == r or rel.startswith(r + "/") for r in UNTRACKED_ROOTS)


def derive(src):
    """suite -> set(untracked relative paths it reads), from the CMakeLists."""
    need = {}
    for name in ("core/CMakeLists.txt", "app/CMakeLists.txt",
                 "ocr/CMakeLists.txt"):
        path = os.path.join(src, name)
        if not os.path.exists(path):
            continue
        with open(path, encoding="utf-8") as f:
            text = f.read()
        for block in add_test_blocks(text):
            m = re.search(r"NAME\s+(\S+)", block)
            if not m:
                continue
            suite = m.group(1)
            need.setdefault(suite, set())
            for tok in re.split(r"\s+", block[block.find("(") + 1:-1].strip()):
                if not tok.startswith("${CMAKE_SOURCE_DIR}"):
                    continue
                rel = tok[len("${CMAKE_SOURCE_DIR}"):].lstrip("/")
                if rel and under_untracked(rel):
                    need[suite].add(rel)
    return need


def read_ctest_properties(binary_dir):
    """suite -> (labels set, has SKIP_REGULAR_EXPRESSION) from the build."""
    props = {}
    for root, _dirs, files in os.walk(binary_dir):
        if "CTestTestfile.cmake" not in files:
            continue
        with open(os.path.join(root, "CTestTestfile.cmake"),
                  encoding="utf-8", errors="replace") as f:
            for line in f:
                m = re.match(r'set_tests_properties\("([^"]+)"\s+PROPERTIES',
                             line)
                if not m:
                    continue
                suite = m.group(1)
                labels = set()
                lm = re.search(r'LABELS\s+"([^"]*)"', line)
                if lm:
                    labels = {x for x in lm.group(1).split(";") if x}
                skip = "SKIP_REGULAR_EXPRESSION" in line
                old_labels, old_skip = props.get(suite, (set(), False))
                props[suite] = (old_labels | labels, old_skip or skip)
    return props


def selftest(src, cmake, ctest):
    """Prove the wrapper against a throwaway project. Hermetic."""
    module = os.path.join(src, "cmake", "AllFixtureTests.cmake")
    check(os.path.exists(module),
          "cmake/AllFixtureTests.cmake is present to be proven")
    if not os.path.exists(module):
        return
    tmp = tempfile.mkdtemp(prefix="all_fixture_gate_")
    try:
        os.makedirs(os.path.join(tmp, "src", "build"))
        with open(os.path.join(tmp, "src", "build", "present.db"), "w") as f:
            f.write("x")
        with open(os.path.join(tmp, "src", "CMakeLists.txt"), "w") as f:
            f.write(
                'cmake_minimum_required(VERSION 3.24)\n'
                'project(fixture_gate_selftest LANGUAGES NONE)\n'
                'enable_testing()\n'
                'list(APPEND CMAKE_MODULE_PATH "%s")\n'
                'include(AllFixtureTests)\n'
                'add_test(NAME present_plain COMMAND ${CMAKE_COMMAND} -E '
                'echo plain-ran)\n'
                'add_test(NAME present_fixture COMMAND ${CMAKE_COMMAND} -E '
                'cat ${CMAKE_SOURCE_DIR}/build/present.db)\n'
                'add_test(NAME absent_fixture COMMAND ${CMAKE_COMMAND} -E '
                'cat ${CMAKE_SOURCE_DIR}/build/absent.db)\n'
                'all_report_fixture_status()\n'
                % os.path.join(src, "cmake").replace("\\", "/"))
        cfg = subprocess.run([cmake, "-S", os.path.join(tmp, "src"),
                              "-B", os.path.join(tmp, "b")],
                             capture_output=True, text=True)
        check(cfg.returncode == 0,
              "the wrapper configures a project cleanly")
        check("1 will SKIP" in cfg.stdout,
              "configure reports the skip count out loud "
              "(saw: %s)" % next((ln.strip() for ln in cfg.stdout.splitlines()
                                  if "ALL battery" in ln), "<nothing>"))
        run = subprocess.run([ctest, "-V"], cwd=os.path.join(tmp, "b"),
                             capture_output=True, text=True)
        out = run.stdout + run.stderr
        check(re.search(r"absent_fixture \.+\*\*\*Skipped", out) is not None,
              "an absent fixture is reported by ctest as Skipped, never as "
              "a pass")
        check("[SKIP] absent_fixture" in out and "build/absent.db" in out,
              "the skip line names the exact missing path")
        check("docs/FIXTURES.md" in out,
              "the skip line points at the page that says how to get it")
        check(re.search(r"present_plain \.+ +Passed", out) is not None
              and re.search(r"present_fixture \.+ +Passed", out) is not None,
              "the two suites whose fixtures ARE present still run")
        listed = subprocess.run([ctest, "-N", "-LE", "fixture"],
                                cwd=os.path.join(tmp, "b"),
                                capture_output=True, text=True).stdout
        check("Total Tests: 1" in listed,
              "'ctest -LE fixture' leaves exactly the fixture-free suite")
        listed = subprocess.run([ctest, "-N", "-L", "fixture"],
                                cwd=os.path.join(tmp, "b"),
                                capture_output=True, text=True).stdout
        check("Total Tests: 2" in listed,
              "'ctest -L fixture' selects both fixture-reading suites, "
              "present and absent alike")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main(argv):
    if len(argv) < 5:
        print("usage: fixture_gate_check.py <source_dir> <binary_dir> "
              "<cmake> <ctest>")
        return 2
    # absolute: the selftest writes a CMAKE_MODULE_PATH into a project in
    # a temp dir, where a relative path would resolve to nothing
    src = os.path.abspath(argv[1])
    binary_dir = os.path.abspath(argv[2])
    cmake, ctest = argv[3], argv[4]

    need = derive(src)
    fixture_suites = {s for s, paths in need.items() if paths}
    all_paths = sorted({p for paths in need.values() for p in paths})
    print("  derived: %d registered suites · %d need an untracked fixture · "
          "%d distinct fixture paths"
          % (len(need), len(fixture_suites), len(all_paths)))
    check(len(need) > 0, "the CMakeLists parse yielded registered suites")
    check(len(fixture_suites) > 0,
          "the CMakeLists parse yielded fixture-dependent suites (a zero "
          "here would make every check below vacuous)")

    # A. documented
    doc_path = os.path.join(src, "docs", "FIXTURES.md")
    doc = ""
    if os.path.exists(doc_path):
        with open(doc_path, encoding="utf-8") as f:
            doc = f.read()
    check(bool(doc), "docs/FIXTURES.md exists")
    undocumented = [p for p in all_paths if p not in doc]
    check(not undocumented,
          "every untracked fixture path the battery reads is documented in "
          "docs/FIXTURES.md (%d checked%s)"
          % (len(all_paths),
             "" if not undocumented else "; missing: %s" % undocumented))

    # B. this build tree
    props = read_ctest_properties(binary_dir)
    check(len(props) > 0,
          "the build tree's CTestTestfile.cmake files were readable")
    unlabelled, unstubbed = [], []
    absent_here = 0
    for suite in sorted(fixture_suites):
        if suite not in props:
            # not registered in this build (e.g. Qt absent) — not a claim
            continue
        labels, has_skip = props[suite]
        if "fixture" not in labels:
            unlabelled.append(suite)
        missing = [p for p in need[suite]
                   if not os.path.exists(os.path.join(src, p))]
        if missing:
            absent_here += 1
            if not has_skip:
                unstubbed.append(suite)
    check(not unlabelled,
          "every fixture-reading suite registered here carries the 'fixture' "
          "label%s" % ("" if not unlabelled else ": %s" % unlabelled))
    print("  this checkout: %d fixture-reading suite(s) have an absent path"
          % absent_here)
    check(not unstubbed,
          "every suite with an absent fixture is a skip stub, not a live "
          "command%s" % ("" if not unstubbed else ": %s" % unstubbed))

    # C. the machinery itself
    selftest(src, cmake, ctest)

    print("fixture_gate: %d failure(s)" % failures)
    print("FAILURES" if failures else "fixture_gate OK")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
