#!/usr/bin/env python3
"""The constitution checker (Fidelity engine, track C).

Every rule below was paid for with a real bug in this repo. A lesson
in a ledger is a hope; a lesson in a gate is a guarantee. Run as a
ctest suite and inside the press.

Ruled exceptions: annotate the offending line with
  // constitution: ruled(<rule>) <why>
and the checker skips it — visibly, never silently.

Usage: constitution_check.py <repo_root>
"""
import re
import sys
import os

fails = []
notes = []


def read(p):
    with open(p, encoding="utf-8", errors="replace") as f:
        return f.read()


def main():
    root = sys.argv[1]
    main_cpp = read(os.path.join(root, "app/main.cpp"))
    press = read(os.path.join(root, "tools/package_macos.sh"))

    core_srcs = {}
    for d in ("core/src", "core/include/allcore", "core/tests"):
        full = os.path.join(root, d)
        for n in os.listdir(full):
            if n.endswith((".cpp", ".h")):
                core_srcs[d + "/" + n] = read(os.path.join(full, n))

    everything = dict(core_srcs)
    everything["app/main.cpp"] = main_cpp

    # R1 — regex literals must not carry collapsed escapes.
    # Incident: the 84000 entity-link regex ("\(", "\s", "\)" in a
    # plain string) turned literal parens into capture groups and
    # barred 's' instead of whitespace. Raw strings only.
    regex_call = re.compile(
        r'QRegularExpression\s*\(\s*\n?\s*"((?:[^"\\]|\\.)*)"',
        re.S)
    bad_escape = re.compile(r'(?<!\\)\\([swbdSWBD()\[\]{}.+?*|])')
    for name, src in everything.items():
        for m in regex_call.finditer(src):
            lit = m.group(1)
            line = src[:m.start()].count("\n") + 1
            ctx = src.splitlines()[line - 1] if line <= len(
                src.splitlines()) else ""
            if "constitution: ruled(R1)" in src[
                    max(0, m.start() - 200):m.end() + 200]:
                notes.append(f"R1 ruled exception at {name}:{line}")
                continue
            b = bad_escape.search(lit)
            if b:
                fails.append(
                    f"R1 {name}:{line}: plain-string regex literal "
                    f"carries collapsed escape '\\{b.group(1)}' — "
                    f"use a raw string R\"(...)\" (incident: the "
                    f"broken 84000 entity link)")

    # R2 — the harness flag list is defined exactly once.
    # Incident: findDataRoot re-parsed its own list; the two lists
    # diverged within one commit pair (--survey hang).
    n = main_cpp.count('early.contains("--pasteprobe")')
    if n != 1:
        fails.append(
            f"R2 app/main.cpp: harness flag list marker "
            f"'--pasteprobe' appears {n}x (must be exactly 1 — one "
            f"list, everything tests g_harnessRun)")

    # R3 — modal-dialog census. Incident: warnOpenFail hung the
    # scriptable --survey mode. New QMessageBox call sites must be
    # reviewed for harness reachability; bump the baseline WITH the
    # review, in this file, in the same commit.
    R3_BASELINE = None  # set at install time below
    msgbox = len(re.findall(r"QMessageBox::(warning|information|"
                            r"question|critical)\s*\(", main_cpp))
    baseline_path = os.path.join(root,
                                 "tools/constitution_baseline.txt")
    if os.path.exists(baseline_path):
        R3_BASELINE = int(read(baseline_path).strip())
    if R3_BASELINE is None:
        with open(baseline_path, "w") as f:
            f.write(str(msgbox) + "\n")
        notes.append(f"R3 baseline installed: {msgbox} modal sites")
    elif msgbox > R3_BASELINE:
        fails.append(
            f"R3 app/main.cpp: modal call sites grew "
            f"{R3_BASELINE} -> {msgbox}. Review each new site for "
            f"harness reachability (g_harnessRun guard), then bump "
            f"tools/constitution_baseline.txt in the same commit "
            f"(incident: the --survey modal hang)")
    elif msgbox < R3_BASELINE:
        with open(baseline_path, "w") as f:
            f.write(str(msgbox) + "\n")
        notes.append(f"R3 baseline lowered to {msgbox}")

    # R4 — the pre-W5 failing inks may not return as text colors.
    # Incident: captions at 2.78:1 shipped for weeks.
    for bad in ("color:#9C948A", "color:#8A8A8A", "color:#999;",
                "color:#999'", "color:#B26B00"):
        if bad in main_cpp:
            line = main_cpp[:main_cpp.index(bad)].count("\n") + 1
            fails.append(
                f"R4 app/main.cpp:{line}: forbidden ink '{bad}' "
                f"(failed WCAG AA; use the ux:: tokens — W5)")

    # R5 — no truncate-rewrite of shared-store TSVs outside the
    # merge-save. Incident: last-writer-wins ate proposals (S1-01).
    prop = core_srcs.get("core/src/proposals.cpp", "")
    truncs = prop.count("std::ios::trunc")
    if truncs != 1:
        fails.append(
            f"R5 core/src/proposals.cpp: expected exactly 1 "
            f"ios::trunc (the merge-save), found {truncs}")
    if "parseProposalsFile(dir_ + \"/proposals.tsv\", disk)" not in prop:
        fails.append(
            "R5 core/src/proposals.cpp: the merge-before-save "
            "guard is missing (S1-01 regression)")
    for name, src in everything.items():
        if name.endswith("proposals.cpp") or "/tests/" in name:
            continue
        if re.search(r'proposals\.tsv.*trunc|trunc.*proposals\.tsv',
                     src):
            fails.append(f"R5 {name}: writes proposals.tsv with "
                         f"trunc outside the store")

    # R6 — battery drills clean up after themselves.
    # Incident: the conflicted-copy drill leaked state; the battery
    # failed only on the SECOND run.
    smoke = core_srcs.get("core/tests/proposals_smoke.cpp", "")
    if "conflicted copy" in smoke and \
            smoke.count("conflicted copy") < 3:
        fails.append(
            "R6 proposals_smoke.cpp: the conflict drill must remove "
            "its conflicted-copy file at every entry point")

    # L2 — no unmanifested payload: every runtime data dir the press
    # stages must be credited in OPEN_SOURCE_NOTICES.md. Incident:
    # reference.db and CC BY-NC models were shipping by habit.
    notices = read(os.path.join(
        root, "docs/distribution/OPEN_SOURCE_NOTICES.md")).lower()
    staged = re.search(r"for d in ([a-z0-9_ \\\n]+); do", press)
    credit = {
        "fonts": "ofl", "honorifics": "hgm", "pron_colloquial": "hgm",
        "abbreviations": "abbreviation", "extracted": "84000",
        "idioms": "hgm", "botok": "botok", "spellcheck": "spellchecker",
        "soas_pos": "soas", "whitney": "whitney",
        "candidate_alignments": "hgm", "84000": "84000",
        "help": "hgm",
    }
    if staged:
        for d in staged.group(1).split():
            if d == "\\":
                continue
            key = credit.get(d)
            if key is None:
                fails.append(
                    f"L2 press stages data dir '{d}' with no entry in "
                    f"the constitution's credit map - add the dir AND "
                    f"its OPEN_SOURCE_NOTICES.md line")
            elif key not in ("hgm",) and key not in notices:
                fails.append(
                    f"L2 staged dir '{d}' expects '{key}' credited in "
                    f"OPEN_SOURCE_NOTICES.md - not found")
    else:
        fails.append("L2 could not locate the press's staged-dir list")

    # C2 — the press keeps its gates. A press that silently lost a
    # gate is a failed press.
    for gate, label in (("ctest", "battery gate"),
                        ("shot_diff.py", "visual-regression gate"),
                        ("constitution_check.py", "constitution gate")):
        if gate not in press:
            fails.append(f"C2 tools/package_macos.sh: {label} "
                         f"missing from the press")

    for x in notes:
        print("  note:", x)
    if fails:
        print(f"constitution: {len(fails)} violation(s)")
        for x in fails:
            print("  FAIL", x)
        return 1
    print("constitution: all rules hold")
    return 0


if __name__ == "__main__":
    sys.exit(main())
