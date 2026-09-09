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
import glob
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
    # F0 (2026-09-09): the R3 modal census covers every .inc the single TU
    # includes, not main.cpp alone — a pane file could otherwise add modal
    # sites the gate never sees. R2 stays main.cpp-only (the flag list lives there).
    inc_files = sorted(glob.glob(os.path.join(root, "app/*.inc")))
    r3_text = main_cpp + "".join(read(p) for p in inc_files)
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
                            r"question|critical)\s*\(", r3_text))
    msgbox += len(re.findall(r"QInputDialog::(getText|getItem|getInt|"
                             r"getDouble|getMultiLineText)\s*\(",
                             r3_text))
    # STATIC-R4: constructor-form dialogs (QMessageBox box(...);
    # ... box.exec()) were structurally invisible to the static-call
    # census - one live uncounted site existed when this landed.
    msgbox += len(re.findall(
        r"\b(?:QMessageBox|QInputDialog)\s+\w+\s*\(", r3_text))
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

    # G2 — the binding gloss census (SQA DATA-3). Nothing mechanical
    # prevented the NEXT surface from printing hgm_gloss untiered - the
    # incident class behind "HGM has:" over a machine gloss, the demo
    # banner, and most of the serious defects this project has had.
    # Every site in app/main.cpp that touches hgm_gloss is counted;
    # growth fails the gate until a human confirms the new site tiers
    # its output (curated/glossary/PROVISIONAL, colour + label) and
    # bumps tools/constitution_hgm_baseline.txt in the same commit.
    hgm_sites = main_cpp.count("hgm_gloss")
    hgm_baseline_path = os.path.join(root,
                                     "tools/constitution_hgm_baseline.txt")
    if os.path.exists(hgm_baseline_path):
        hgm_base = int(read(hgm_baseline_path).strip())
        if hgm_sites > hgm_base:
            fails.append(
                f"G2 app/main.cpp: hgm_gloss consumer sites grew "
                f"{hgm_base} -> {hgm_sites}. Review each new site: the "
                f"binding gloss may only render TIERED (curated/"
                f"glossary/PROVISIONAL, colour and label), then bump "
                f"tools/constitution_hgm_baseline.txt in the same "
                f"commit (DATA-3)")
        elif hgm_sites < hgm_base:
            # GATE-4: the gate used to WRITE the lowered baseline
            # itself - a check run mutating the source tree, and a
            # ratchet moving without a human commit. It now only
            # says so; the tightening is a person's diff.
            notes.append(
                f"G2 baseline has slack: {hgm_sites} sites < baseline "
                f"{hgm_base} - tighten "
                f"tools/constitution_hgm_baseline.txt in a commit")
    else:
        fails.append(
            "G2 baseline file missing "
            "(tools/constitution_hgm_baseline.txt) - the census has "
            "no ratchet to check against; commit the baseline (the "
            "gate no longer installs it itself, GATE-4)")

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

    # G3 — PROVISIONAL never wears error-red (SQA low; the manual
    # promises amber three times). Red says "wrong"; provisional says
    # "machine-matched, verify" - a translator taught that red means
    # broken learns to ignore red, which then costs them on a REAL
    # error. kMachine #B4540A is the tier's colour.
    # SQA re-measurement 2026-08-26 (adversarially confirmed): the
    # first draft matched three exact substrings, all married to the
    # PRE-sweep formatting - after the sweep repainted those sites the
    # patterns matched NOTHING, so the rule blessed every future
    # red-PROVISIONAL that dared to use different quoting. Same class
    # as C2's disarmed-gate lesson: a rule is only as alive as its
    # ability to see its subject. Now: every PROVISIONAL token is
    # inspected against the nearest color declaration BEFORE it, and
    # the rule fails itself if it cannot find its subject at all.
    import re as _re3
    prov_sites = [m.start() for m in _re3.finditer("PROVISIONAL",
                                                   main_cpp)]
    if len(prov_sites) < 3:
        fails.append(
            "G3 liveness: fewer than 3 PROVISIONAL tokens in "
            "app/main.cpp - the tier label this rule polices has "
            "vanished; the rule refuses to pass over a missing "
            "subject")
    # DH-3 extension: a provisional MARK (code guarded by
    # e.provisional()) painted red is the same lie without the word
    # PROVISIONAL nearby - scan those guards' forward context too.
    for m in _re3.finditer(r"provisional\(\)", main_cpp):
        fwd = main_cpp[m.end():m.end() + 320]
        rm = _re3.search(r"color:\s*#[bcdef]0{2}\b", fwd)
        if rm:
            line = main_cpp[:m.start()].count("\n") + 1
            fails.append(
                f"G3 app/main.cpp:{line}: a provisional-guarded mark "
                f"painted error-red ({rm.group(0)}) - the tier's "
                f"colour is kMachine amber (#B4540A)")
    red_ink = _re3.compile(
        r"color:\s*(#[bcdef]0{2,5}\b|#f44|#e53|red\b|crimson)",
        _re3.I)
    for pos in prov_sites:
        ctx = main_cpp[max(0, pos - 120):pos]
        cm = None
        for cm in red_ink.finditer(ctx):
            pass   # keep the LAST color decl before the token
        if cm:
            line = main_cpp[:pos].count("\n") + 1
            fails.append(
                f"G3 app/main.cpp:{line}: PROVISIONAL painted "
                f"error-red ({cm.group(1)}) - the tier's colour is "
                f"kMachine amber (#B4540A); red is reserved for "
                f"genuine failure")

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

    # C2 — the press keeps its gates, ALIVE. The original check was a
    # substring test, and the audit proved it blessable over a press
    # whose gates were all disarmed: comment the line out or append
    # "|| true" and the substring survives while the gate dies
    # (BUILD-10, mutation-proven). A gate is LIVE when at least one
    # uncommented invocation carries no failure-swallower; the press
    # runs under set -euo pipefail, so an unswallowed failure stops it.
    # REL-5: the ritual ABOVE the press decays the same way - a
    # written gate nothing checks (the MEM-5 mode). release.sh's own
    # gates get the same liveness rules as the press's.
    rel_sh = ""
    rel_path = os.path.join(root, "tools", "release.sh")
    if os.path.exists(rel_path):
        rel_sh = open(rel_path, encoding="utf-8").read()
    for gate, label in (("sanitized_battery.sh", "release sanitizer gate"),
                        ("mutate.py --sweep", "release mutation gate"),
                        ("package_macos.sh", "press-before-tag gate")):
        live = 0
        for ln in rel_sh.splitlines():
            t = ln.strip()
            if t.startswith("#") or gate not in ln:
                continue
            if "|| true" in ln or "||true" in ln:
                continue
            live += 1
        if rel_sh and live == 0:
            fails.append(
                f"C2 tools/release.sh: the {label} ('{gate}') has no "
                f"live invocation - deleted or disarmed; the release "
                f"ritual must keep its gates (REL-5, the MEM-5 decay "
                f"mode)")
    for gate, label in (("ctest", "battery gate"),
                        ("shot_diff.py", "visual-regression gate"),
                        ("constitution_check.py", "constitution gate")):
        live = 0
        disarmed = []
        import re as _re2
        # \b keeps ctest_press.log and prose mentions from counting
        # as invocations - the first draft of this hardening was
        # satisfied by a LOG FILENAME, the substring disease one level
        # down.
        inv = _re2.compile(r"\b" + _re2.escape(gate) + r"\b")
        for ln, line in enumerate(press.splitlines(), 1):
            if not inv.search(line):
                continue
            stripped = line.lstrip()
            if stripped.startswith("#"):
                continue
            if _re2.search(_re2.escape(gate) + r"[\w.]*\.log", line):
                continue   # a log path, not an invocation
            if "|| true" in line or "|| :" in line:
                disarmed.append(ln)
                continue
            live += 1
        if live == 0:
            detail = (f" (found only disarmed invocation(s) at line(s) "
                      f"{disarmed})" if disarmed else "")
            fails.append(f"C2 tools/package_macos.sh: {label} is not "
                         f"LIVE in the press{detail} - a disarmed gate "
                         f"blesses what it never judged (BUILD-10)")

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
