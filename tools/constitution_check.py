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
    # QInputDialog counts too: it is exactly as blocking as a
    # QMessageBox and was entirely outside this census until
    # 2026-08-22 (15 sites, unwatched). A global dialog reaper now
    # protects every headless mode, but the census still tracks growth
    # — a mechanism plus a count, not one or the other.
    msgbox = len(re.findall(r"QMessageBox::(warning|information|"
                            r"question|critical)\s*\(", main_cpp))
    msgbox += len(re.findall(r"QInputDialog::(getText|getItem|getInt|"
                             r"getDouble|getMultiLineText)\s*\(",
                             main_cpp))
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

    # G1 — the green vocabulary is FROZEN (SQA DATA-12). The audit
    # found seven saturated greens across 43 sites giving the binding
    # colour four different meanings; 2026-08-26 consolidated them to
    # exactly three, each with ONE meaning:
    #   #1E6B4E  kAct    - binding authority (HGM's English, approvals,
    #                      confirmed rulings)
    #   #2E7D32  reserved- the second binding-family green, kept out of
    #                      machine/reference use by the ux pins
    #   #3B7A3B          - the terminology-check "matched" bullet
    # Pale wash tints (all channels > 0xC8) are backgrounds, not inks.
    # A NEW saturated green is a new meaning: name it here with a
    # ruling or use a token that exists.
    import re as _re
    G1_ALLOWED = {"1E6B4E", "2E7D32", "3B7A3B"}
    for m in _re.finditer(r"#([0-9A-Fa-f]{6})", main_cpp):
        hx = m.group(1).upper()
        r, g, b = (int(hx[i:i+2], 16) for i in (0, 2, 4))
        if g > r and g >= b and g < 0xC8 and hx not in G1_ALLOWED:
            line = main_cpp[:m.start()].count("\n") + 1
            fails.append(
                f"G1 app/main.cpp:{line}: unvocabularied green #{hx} "
                f"- the green vocabulary is frozen at three named "
                f"meanings (DATA-12); use a token or add a ruling "
                f"here")

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

    # L2 — no unmanifested payload. Incident: reference.db and CC
    # BY-NC models were shipping by habit.
    #
    # The first version of this rule mapped each staged directory to a
    # keyword and passed if the keyword occurred ANYWHERE in
    # OPEN_SOURCE_NOTICES.md — so 102 MB and 3,219 files under
    # data/extracted were credited forever by the unrelated string
    # "84000", among them 1,926 raw THL catalogue records the notices
    # themselves called "NOT redistributed". It also only read the
    # press's `for d in ...` loop, so everything staged by an explicit
    # `cp`, including the 80 MB reference.db, was invisible to it
    # (SQA BUILD-4, BUILD-5). A gate that never opens the payload is
    # not a gate.
    #
    # The rule is now a manifest. docs/distribution/PAYLOAD_MANIFEST.txt
    # carries one row per path that may be in a DMG, with its licence
    # and the notices anchor that backs it; the press stages FROM it,
    # prunes to it and is gated on it. What this checker can verify
    # without a press, tools/manifest_check.py --static does:
    #   * every anchor a row claims really occurs in the notices;
    #   * the press still calls all four manifest entry points;
    #   * nothing sits in data/extracted unclassified — which is the
    #     check that would have caught the THL trees the day they
    #     landed, no press needed.
    import subprocess
    try:
        r = subprocess.run(
            [sys.executable,
             os.path.join(root, "tools/manifest_check.py"),
             "--static", root], capture_output=True, text=True)
        found = 0
        for line in r.stdout.splitlines():
            t = line.strip()
            if t.startswith("FAIL "):
                fails.append("L2 " + t[5:])
                found += 1
            elif t.startswith("note: "):
                notes.append("L2 " + t[6:])
        if r.returncode != 0 and found == 0:
            fails.append(
                "L2 manifest_check.py --static exited %d with no "
                "parsable finding: %s"
                % (r.returncode, r.stderr.strip() or "(no stderr)"))
    except Exception as e:
        fails.append("L2 manifest_check.py could not run: %s" % e)

    # C2 — the press keeps its gates. A press that silently lost a
    # gate is a failed press.
    for gate, label in (("ctest", "battery gate"),
                        ("shot_diff.py", "visual-regression gate"),
                        ("constitution_check.py", "constitution gate")):
        if gate not in press:
            fails.append(f"C2 tools/package_macos.sh: {label} "
                         f"missing from the press")

    # L3 — the human-gated view must not drift from the backlog.
    # Incident: ten items needing Adam, including every one filed that
    # day, existed in CLOSER.md and appeared nowhere on the page he
    # reads. An item only he can move, that he cannot see, is lost.
    try:
        import subprocess
        r = subprocess.run(
            [sys.executable, os.path.join(root, "tools/reconcile_lists.py"),
             root], capture_output=True, text=True)
        f3, n3 = shell_verdict("L3", "reconcile_lists.py",
                               r.returncode, r.stdout, r.stderr)
        fails.extend(f3)
        notes.extend(n3)
    except Exception as e:
        # STATIC-3: this was notes.append, and the notes print sat
        # FIFTEEN LINES ABOVE this block - so the message could never
        # reach the terminal even when it was produced.
        fails.append("L3 reconcile could not run: %s" % e)

    for x in notes:
        print("  note:", x)
    if fails:
        print(f"constitution: {len(fails)} violation(s)")
        for x in fails:
            print("  FAIL", x)
        return 1
    print("constitution: all rules hold")
    return 0


def shell_verdict(rule, script, returncode, stdout, stderr):
    """What a rule that shelled out is entitled to conclude.

    SQA STATIC-3 (2026-08-24). The contract is that the subprocess must
    be shown to have RUN. A non-zero exit carrying nothing parsable is
    a LOST GATE, not a clean one - and that is precisely how L3 let the
    very file it exists to police be deleted while this checker printed
    "all rules hold" and exited 0.

    L2 had this guard and L3 did not, 28 lines apart in one file. They
    share it now, because two copies of a rule are how the copies
    drift.

    Returns (fails, notes).
    """
    fails, notes = [], []
    found = 0
    for line in stdout.splitlines():
        t = line.strip()
        if t.startswith("- "):
            fails.append("%s %s" % (rule, t[2:]))
            found += 1
        elif t.startswith("FAIL "):
            fails.append("%s %s" % (rule, t[5:]))
            found += 1
        elif t.startswith("note: "):
            notes.append("%s %s" % (rule, t[6:]))
    if returncode != 0 and found == 0:
        fails.append(
            "%s %s exited %d with no parsable finding: %s"
            % (rule, script, returncode,
               stderr.strip() or "(no stderr)"))
    return fails, notes


def cmd_selftest():
    """SQA STATIC-3 (2026-08-24). A rule that shells out must assert
    the subprocess RAN, not merely that it did not complain.

    L2 had that guard; L3 did not, so deleting the very file L3 exists
    to police left the gate printing "all rules hold" with exit 0. The
    asymmetry was provable in one pair of commands, 28 lines apart in
    this file. These pins fix the contract in one place that both rules
    now share.
    """
    bad = []

    # the file is gone / raised on import: exit code, no parsable line
    f, _ = shell_verdict("L3", "reconcile_lists.py", 2, "",
                         "Traceback ... FileNotFoundError")
    if not f:
        bad.append("a subprocess that exited non-zero with NO parsable "
                   "finding must fail the gate - that is exactly how a "
                   "deleted gate used to pass")
    elif "2" not in f[0]:
        bad.append("the failure must name the exit code")

    # it ran and reported real findings: those are the findings
    f, _ = shell_verdict("L3", "reconcile_lists.py", 1,
                         "- item one\n- item two\n", "")
    if len(f) != 2:
        bad.append("parsable findings are reported individually, not "
                   "collapsed into 'it failed'")

    # control: it ran clean. A guard that fails everything is not a
    # guard, it is an outage.
    f, _ = shell_verdict("L3", "reconcile_lists.py", 0, "", "")
    if f:
        bad.append("a clean run must NOT fail the gate")

    for b in bad:
        print("  [FAIL]", b)
    print("constitution selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0


if __name__ == "__main__":
    if "--selftest" in sys.argv:
        sys.exit(cmd_selftest())
    sys.exit(main())
