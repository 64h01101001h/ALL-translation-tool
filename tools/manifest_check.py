#!/usr/bin/env python3
"""manifest_check.py — the gate that opens the payload.

Written 2026-08-23 for SQA BUILD-4 and BUILD-5.

WHAT WENT WRONG. The constitution's L2 rule existed to stop
unmanifested payload reaching a DMG. It worked by mapping each staged
directory name to a keyword and passing if the keyword appeared
ANYWHERE in OPEN_SOURCE_NOTICES.md:

    credit = { ... "extracted": "84000", ... }

So 102 MB and 3,219 files under data/extracted were credited forever
because the unrelated string "84000" occurs in the notices. Inside
that directory sat 1,926 raw THL catalog HTML records (1,118 Dege and
808 Lhasa) - while the notices' own THL line said the THL payload is
"NOT redistributed".
The rule also only saw the press's `for d in ...; do` loop, so every
path staged by an explicit `cp` - including the 80 MB reference.db
whose own builder header says "The DB never ships" - was invisible to
it.

WHAT THIS DOES INSTEAD. docs/distribution/PAYLOAD_MANIFEST.txt names
every path that may be in a DMG, with its licence and the notices
anchor that must back it. This tool

  * drives the press's staging list FROM the manifest, so a data
    directory cannot be staged without being named (--list-data-dirs);
  * prunes the staged data/extracted tree to the manifest, printing
    every path it removes and its size (--prune-extracted);
  * walks the FINISHED staged tree and fails on any file or directory
    the manifest does not name, on any team-only payload found in a
    market press, and on any `drop` row that survived the prune
    (--press);
  * verifies the README the press just wrote actually carries the
    stamp for the mode it was pressed in (--readme);
  * checks statically that every manifest anchor is really in the
    notices, that the press still calls all four of the above, and
    that every file in the repo's data/extracted is classified either
    as shipping or as dropped (--static). The constitution's L2 rule
    is now this check.

Nothing here guesses. An unclassified path is a hard failure with the
path and its size printed, never a silent pass and never a silent
delete.

Usage:
  manifest_check.py --static          <repo_root>
  manifest_check.py --list-data-dirs  <repo_root> <team|market>
  manifest_check.py --prune-extracted <repo_root> <stage_dir> <team|market>
  manifest_check.py --readme          <repo_root> <readme> <team|market>
  manifest_check.py --press           <repo_root> <stage_dir> <team|market>
  manifest_check.py --selftest        <repo_root>
"""
import fnmatch
import os
import shutil
import sys

MANIFEST = "docs/distribution/PAYLOAD_MANIFEST.txt"
NOTICES = "docs/distribution/OPEN_SOURCE_NOTICES.md"
DATA_DIR_PREFIX = "Diamond Cutter Tool Data/data/"
EXTRACTED = "Diamond Cutter Tool Data/data/extracted"
MODES = ("both", "team", "market", "drop")
STATUSES = ("ok", "itemise", "gated")
# Finder droppings. Not payload, and not worth failing a press over —
# but they are reported, never silently swallowed.
IGNORED_NAMES = (".DS_Store", ".localized", "Icon\r")


class Row(object):
    __slots__ = ("mode", "status", "path", "licence", "anchor",
                 "provenance", "lineno", "is_dir")

    def __init__(self, fields, lineno):
        (self.mode, self.status, self.path, self.licence,
         self.anchor, self.provenance) = fields
        self.lineno = lineno
        self.is_dir = self.path.endswith("/")

    @property
    def key(self):
        """The path with any trailing slash removed."""
        return self.path[:-1] if self.is_dir else self.path


def load(root):
    """Read the manifest. Returns (rows, errors)."""
    path = os.path.join(root, MANIFEST)
    rows, errors = [], []
    if not os.path.exists(path):
        return rows, ["manifest missing: " + MANIFEST]
    with open(path, encoding="utf-8") as fh:
        for lineno, line in enumerate(fh, 1):
            line = line.rstrip("\n")
            if not line.strip() or line.lstrip().startswith("#"):
                continue
            fields = line.split("|")
            if len(fields) != 6:
                errors.append(
                    "%s:%d: expected 6 '|'-separated fields, found %d"
                    % (MANIFEST, lineno, len(fields)))
                continue
            row = Row([f.strip() for f in fields], lineno)
            if row.mode not in MODES:
                errors.append("%s:%d: unknown mode %r (expected %s)"
                              % (MANIFEST, lineno, row.mode,
                                 "|".join(MODES)))
            if row.status not in STATUSES:
                errors.append("%s:%d: unknown status %r (expected %s)"
                              % (MANIFEST, lineno, row.status,
                                 "|".join(STATUSES)))
            rows.append(row)
    seen = {}
    for row in rows:
        if row.key in seen:
            errors.append("%s:%d: duplicate path %r (first at line %d)"
                          % (MANIFEST, row.lineno, row.key, seen[row.key]))
        seen[row.key] = row.lineno
    return rows, errors


def ships_in(row, mode):
    """Is this row allowed to be in a <mode> DMG?"""
    if row.mode == "drop":
        return False
    if row.mode == "both":
        return True
    return row.mode == mode


def covers_subtree(row, rows):
    """A directory row covers its whole subtree ONLY while no more
    specific row names something inside it. The most specific row wins
    — that is what lets data/extracted/ be itemised file by file while
    data/fonts/ is credited as a whole tree."""
    if not row.is_dir:
        return False
    prefix = row.key + "/"
    return not any(r is not row and r.key.startswith(prefix) for r in rows)


def match(rel, rows, isdir):
    """The most specific row matching this staged path, or None."""
    best = None
    for row in rows:
        if row.is_dir:
            if fnmatch.fnmatch(rel, row.key) and isdir:
                if covers_subtree(row, rows):
                    best = row if best is None or \
                        len(row.key) > len(best.key) else best
        elif fnmatch.fnmatch(rel, row.path):
            best = row if best is None or \
                len(row.path) > len(best.path) else best
    return best


def has_descendant(rel, rows):
    prefix = rel + "/"
    return any(r.key.startswith(prefix) for r in rows)


def size_of(path):
    if os.path.islink(path):
        return 0
    if os.path.isdir(path):
        total = 0
        for dirpath, _dirs, files in os.walk(path):
            for name in files:
                fp = os.path.join(dirpath, name)
                if not os.path.islink(fp):
                    total += os.path.getsize(fp)
        return total
    return os.path.getsize(path)


def walk_stage(stage, rows, mode, complete=False):
    """Inspect the real staged tree. Returns (fails, notes)."""
    fails, notes, itemised = [], [], []

    def visit(rel):
        here = os.path.join(stage, rel) if rel else stage
        for name in sorted(os.listdir(here)):
            if name in IGNORED_NAMES:
                notes.append("ignored filesystem dropping: %s"
                             % (rel + "/" + name if rel else name))
                continue
            child = rel + "/" + name if rel else name
            full = os.path.join(here, name)
            isdir = os.path.isdir(full) and not os.path.islink(full)
            row = match(child, rows, isdir)
            if row is not None:
                if row.mode == "drop":
                    fails.append(
                        "UNPRUNED: %s (%d bytes) is manifested `drop` "
                        "- it must not be in a DMG. %s"
                        % (child, size_of(full), row.licence))
                elif not ships_in(row, mode):
                    fails.append(
                        "WRONG MODE: %s (%d bytes) is manifested "
                        "`%s`, this is a %s press. %s"
                        % (child, size_of(full), row.mode, mode,
                           row.licence))
                elif mode == "market" and row.status in ("itemise",
                                                         "gated"):
                    fails.append(
                        "UNRESOLVED TERMS: %s is manifested status "
                        "`%s` - %s (%s)"
                        % (child, row.status, row.licence,
                           row.provenance))
                elif row.status == "gated":
                    notes.append("GATED %s - %s"
                                 % (child, row.provenance))
                elif row.status == "itemise":
                    itemised.append(child)
                continue
            if isdir and has_descendant(child, rows):
                visit(child)
                continue
            fails.append(
                "UNMANIFESTED PAYLOAD: %s (%d bytes) is staged and no "
                "row of %s names it. Add a row (mode/status/licence/"
                "notices anchor) or stop staging it."
                % (child, size_of(full), MANIFEST))

    visit("")
    if complete:
        # BUILD-20, the other direction: absence. A walk of what exists
        # can never notice what should exist and does not - a cp that
        # failed mid-press leaves a manifested payload silently missing
        # from the image. Every row that ships in this mode must
        # resolve to something staged. Keys may carry fnmatch
        # wildcards (hgm_spine_*.db), so match accordingly.
        for row in rows:
            if not ships_in(row, mode):
                continue
            if "OPTIONAL" in row.licence:
                # the press stages these behind its own [[ -f ]] guard;
                # absence is a configuration of the build, not a failed
                # copy. The marker lives in the licence field so the row
                # itself explains why the gate lets it be absent.
                continue
            key = row.key.rstrip("/")
            full = os.path.join(stage, key)
            if "*" in key or "?" in key:
                d = os.path.dirname(full)
                pat = os.path.basename(key)
                found = os.path.isdir(d) and any(
                    fnmatch.fnmatch(n, pat) for n in os.listdir(d))
            else:
                found = os.path.exists(full)
            if not found:
                fails.append(
                    "MISSING PAYLOAD: %s is manifested to ship in a "
                    "%s press and nothing staged matches it - a copy "
                    "step failed or the payload was never built. (%s)"
                    % (row.key, mode, row.licence))
    if itemised:
        notes.append(
            "%d staged path(s) ship under SHIP.md's blanket "
            "\"data/extracted: SHIPS-WITH-ATTRIBUTION, pending the "
            "per-file itemization\" and have no per-file terms "
            "recorded yet: %s" % (len(itemised),
                                  ", ".join(os.path.basename(p)
                                            for p in sorted(itemised))))
    return fails, notes


def check_readme(text, mode):
    """The README that rides in the DMG must say what the DMG is.

    BUILD-5: docs/SHIP.md claimed market mode "stamps the README
    accordingly"; the heredoc that writes it had no mode conditional at
    all, so a 424 MB team image carrying build/reference.db - the
    unlicensed reference-dictionary compilations the interface labels
    "(local only)" - shipped with eight lines of install instructions
    and no word about redistribution. This reads the file the press
    actually wrote, not the script that wrote it."""
    fails = []
    low = text.lower()
    if "open_source_notices.md" not in low:
        fails.append("README.txt does not point at OPEN_SOURCE_NOTICES.md")
    if mode == "team":
        if "not for redistribution" not in low:
            fails.append(
                "team README.txt carries no non-redistribution notice - "
                "this image holds payload the notices declare not "
                "redistributable")
        if "reference.db" not in low:
            fails.append(
                "team README.txt does not name build/reference.db, the "
                "payload the notice is about")
    elif mode == "market":
        if "team copy" in low:
            fails.append(
                "market README.txt carries the team-only stamp")
        if "reference.db" not in low:
            fails.append(
                "market README.txt does not say that reference.db is "
                "omitted")
    else:
        fails.append("unknown press mode %r" % mode)
    return fails


def cmd_static(root):
    rows, fails = load(root)
    notes = []
    notices_path = os.path.join(root, NOTICES)
    notices = ""
    if os.path.exists(notices_path):
        with open(notices_path, encoding="utf-8") as fh:
            notices = fh.read().lower()
    else:
        fails.append("notices missing: " + NOTICES)

    # S1 — every anchor a row claims must really be in the notices.
    for row in rows:
        if row.anchor == "-":
            continue
        if row.anchor.lower() not in notices:
            fails.append(
                "%s:%d: %s claims notices anchor %r, which does not "
                "occur in %s" % (MANIFEST, row.lineno, row.key,
                                 row.anchor, NOTICES))

    # S2 — the press must still call all four manifest entry points.
    press_path = os.path.join(root, "tools/package_macos.sh")
    press = ""
    if os.path.exists(press_path):
        with open(press_path, encoding="utf-8") as fh:
            press = fh.read()
    else:
        fails.append("press missing: tools/package_macos.sh")
    if press and "manifest_check.py" not in press:
        fails.append("tools/package_macos.sh: the press does not call "
                     "manifest_check.py at all - the payload gate is "
                     "gone")
    for flag, why in (("--list-data-dirs",
                       "the staging list must be driven FROM the "
                       "manifest"),
                      ("--prune-extracted",
                       "the drop rows must actually be removed"),
                      ("--readme",
                       "the README stamp must be verified in the file "
                       "that ships"),
                      ("--press",
                       "the finished stage must be inspected")):
        if flag not in press:
            fails.append("tools/package_macos.sh: no %s call - %s"
                         % (flag, why))

    # S3 — nothing may sit in the repo's data/extracted unclassified.
    # This is the rule that would have caught the THL record trees on
    # the day they were added, with no press needed.
    ext = os.path.join(root, "data/extracted")
    if os.path.isdir(ext):
        for name in sorted(os.listdir(ext)):
            if name in IGNORED_NAMES:
                continue
            rel = EXTRACTED + "/" + name
            row = match(rel, rows, os.path.isdir(os.path.join(ext, name)))
            if row is None:
                fails.append(
                    "UNCLASSIFIED: data/extracted/%s (%d bytes) is in "
                    "the tree the press copies wholesale and %s does "
                    "not classify it. Add a row: `both` to ship it, "
                    "`drop` to keep it out."
                    % (name, size_of(os.path.join(ext, name)), MANIFEST))
    else:
        notes.append("data/extracted absent - S3 not exercised")

    stage = os.path.join(root, "dist/stage")
    if os.path.isdir(stage):
        stale, _ = walk_stage(stage, rows, "team")
        if stale:
            notes.append(
                "dist/stage is a previous press's output and does not "
                "match the manifest (%d finding(s)); step 3 deletes it "
                "and the --press gate judges the fresh one" % len(stale))
    return fails, notes


def cmd_list_data_dirs(root, mode):
    rows, errors = load(root)
    if errors:
        for e in errors:
            sys.stderr.write("manifest: " + e + "\n")
        return None
    out = []
    for row in rows:
        if not row.is_dir or not row.key.startswith(DATA_DIR_PREFIX):
            continue
        name = row.key[len(DATA_DIR_PREFIX):]
        if "/" in name or not ships_in(row, mode):
            continue
        out.append(name)
    return out


def cmd_prune_extracted(root, stage, mode):
    """Remove the manifest's `drop` rows from the staged extracted
    tree. Prints every removal with its size. Deliberately narrow:
    it only ever removes paths a row explicitly marks `drop`, and only
    under data/extracted. Anything else unmanifested is failed by
    --press, never deleted behind the operator's back."""
    rows, errors = load(root)
    if errors:
        for e in errors:
            sys.stderr.write("manifest: " + e + "\n")
        return 1, 0
    removed = 0
    freed = 0
    for row in rows:
        if row.mode != "drop":
            continue
        if not row.key.startswith(EXTRACTED + "/"):
            continue
        full = os.path.join(stage, row.key)
        if not os.path.exists(full):
            continue
        n = size_of(full)
        if os.path.isdir(full) and not os.path.islink(full):
            shutil.rmtree(full)
        else:
            os.remove(full)
        removed += 1
        freed += n
        print("   pruned %s (%s) - %s"
              % (row.key[len(EXTRACTED) + 1:], human(n), row.licence))
    return 0, freed


def human(n):
    for unit in ("B", "KB", "MB", "GB"):
        if n < 1024 or unit == "GB":
            return "%.1f %s" % (n, unit)
        n /= 1024.0


def report(title, fails, notes, ok_line="every staged path is "
                                        "manifested"):
    for note in notes:
        print("   note: " + note)
    if fails:
        print("%s: %d finding(s)" % (title, len(fails)))
        for f in fails:
            print("  FAIL " + f)
        return 1
    print("%s: %s" % (title, ok_line))
    return 0


CHECKS = 11


def cmd_selftest(root):
    """Prove the gate can fail. Builds synthetic staged trees in a
    temp dir and asserts the walk's verdict on each."""
    import tempfile
    rows, errors = load(root)
    if errors:
        for e in errors:
            print("  FAIL manifest: " + e)
        return 1
    bad = []

    def stage_with(tmp, rel, content=b"x"):
        full = os.path.join(tmp, rel)
        os.makedirs(os.path.dirname(full), exist_ok=True)
        with open(full, "wb") as fh:
            fh.write(content)

    def check(label, builder, mode, want_substr, complete=False):
        tmp = tempfile.mkdtemp(prefix="manifest_selftest_")
        try:
            builder(tmp)
            fails, _notes = walk_stage(tmp, rows, mode, complete=complete)
            joined = "\n".join(fails)
            if want_substr is None:
                if fails:
                    bad.append("%s: expected a clean walk, got: %s"
                               % (label, joined))
            elif want_substr not in joined:
                bad.append("%s: expected %r in the findings, got: %s"
                           % (label, want_substr, joined or "(none)"))
        finally:
            shutil.rmtree(tmp, ignore_errors=True)

    # 0. BUILD-20: the gate can only see what is PRESENT. A cp that
    #    fails mid-press leaves a manifested payload missing from the
    #    stage, and a walk of what exists never notices absence. With
    #    completeness on, a one-file stage must report the rows that
    #    should have shipped and did not.
    check("manifested payload missing from the stage",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/extracted/tol_links.json"),
          "team", "MISSING PAYLOAD", complete=True)
    # 1. a manifested file passes
    check("manifested file",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/extracted/tol_links.json"),
          "team", None)
    # 2. an unmanifested file inside a credited directory FAILS.
    #    This is BUILD-4 exactly: the old rule passed the whole of
    #    data/extracted on the keyword "84000".
    check("unmanifested file in a credited dir",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/extracted/scraped.html"),
          "team", "UNMANIFESTED PAYLOAD")
    # 3. a `drop` tree that survived the prune FAILS. This is BUILD-5:
    #    THL records in a DMG whose notices say they are not
    #    redistributed.
    check("unpruned drop payload",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/extracted/"
                 "thl_dege_records/d0001.html"),
          "team", "UNPRUNED")
    # 4. team-only payload in a market press FAILS.
    check("team payload in a market press",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/build/reference.db"),
          "market", "WRONG MODE")
    # 5. the same payload is fine in a team press.
    check("team payload in a team press",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/build/reference.db"),
          "team", None)
    # 6. unresolved terms block a market press but not a team one.
    # Fixture note: this used soas_pos until 2026-08-26, when that row
    # was verified CC BY 4.0 against the Zenodo record and moved to ok.
    # The teachings index is the remaining genuinely-gated payload
    # (SHIP.md item 3, Adam's consent line). If THAT closes too, this
    # fixture must move again - the test wants any still-gated row.
    check("gated payload in a market press",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/teaching/"
                 "teaching_moments_card.json"),
          "market", "UNRESOLVED TERMS")
    check("gated payload in a team press",
          lambda t: stage_with(
              t, "Diamond Cutter Tool Data/data/teaching/"
                 "teaching_moments_card.json"),
          "team", None)
    # 7. a wholly unmanifested top-level directory FAILS.
    check("unmanifested top-level dir",
          lambda t: stage_with(t, "Secret Payload/thing.bin"),
          "team", "UNMANIFESTED PAYLOAD")

    # 8. the README the press writes must say what the image is. The
    #    literal pre-2026-08-23 README is the negative case.
    old_readme = (
        "Diamond Cutter Translation Tool - release 1.0.0-rc.1\n\n"
        "1. Drag BOTH the app and \"Diamond Cutter Tool Data\" to the "
        "same place.\n"
        "2. First launch: RIGHT-CLICK the app and choose Open.\n\n"
        "Everything runs offline except the Analysis pane's AI report "
        "and the woodblock viewer's BDRC page images.\n")
    if not check_readme(old_readme, "team"):
        bad.append("README stamp: the pre-fix 8-line README passed the "
                   "team check - the check cannot fail")
    stamped = old_readme + (
        "\nTEAM COPY - NOT FOR REDISTRIBUTION\n"
        "This image holds build/reference.db, whose licensing review is "
        "open.\nFull terms: OPEN_SOURCE_NOTICES.md beside this file.\n")
    if check_readme(stamped, "team"):
        bad.append("README stamp: a correctly stamped team README was "
                   "rejected: %s" % check_readme(stamped, "team"))
    if not check_readme(stamped, "market"):
        bad.append("README stamp: a team-stamped README passed as a "
                   "market README")

    # 9. the manifest itself must be internally consistent and must
    #    match the repo it describes.
    fails, notes = cmd_static(root)
    for f in fails:
        bad.append("static: " + f)

    if bad:
        print("manifest selftest: %d failure(s)" % len(bad))
        for b in bad:
            print("  FAIL " + b)
        return 1
    print("manifest selftest: %d checks passed - the gate fails on "
          "every kind of unmanifested payload, on team payload in a "
          "market press and on unresolved terms, the README stamp "
          "rejects the pre-fix README, and the manifest matches this "
          "repo" % CHECKS)
    return 0


def main(argv):
    if len(argv) < 3:
        sys.stderr.write(__doc__)
        return 2
    cmd, root = argv[1], argv[2]
    if cmd == "--static":
        fails, notes = cmd_static(root)
        return report("payload manifest (static)", fails, notes,
                      "manifest, notices and press agree; nothing in "
                      "data/extracted is unclassified")
    if cmd == "--selftest":
        return cmd_selftest(root)
    if cmd == "--list-data-dirs":
        if len(argv) < 4:
            sys.stderr.write("--list-data-dirs needs a mode\n")
            return 2
        dirs = cmd_list_data_dirs(root, argv[3])
        if dirs is None:
            return 1
        print(" ".join(dirs))
        return 0
    if cmd == "--prune-extracted":
        if len(argv) < 5:
            sys.stderr.write("--prune-extracted needs <stage> <mode>\n")
            return 2
        rc, freed = cmd_prune_extracted(root, argv[3], argv[4])
        if rc == 0 and freed:
            print("   %s of unshipped ingest material removed from the "
                  "stage" % human(freed))
        return rc
    if cmd == "--readme":
        if len(argv) < 5:
            sys.stderr.write("--readme needs <readme_path> <mode>\n")
            return 2
        try:
            with open(argv[3], encoding="utf-8") as fh:
                text = fh.read()
        except OSError as exc:
            print("README stamp: 1 finding(s)")
            print("  FAIL cannot read %s: %s" % (argv[3], exc))
            return 1
        return report("README stamp", check_readme(text, argv[4]), [],
                      "the %s README says what this image is" % argv[4])
    if cmd == "--press":
        if len(argv) < 5:
            sys.stderr.write("--press needs <stage> <mode>\n")
            return 2
        stage, mode = argv[3], argv[4]
        rows, fails = load(root)
        if fails:
            return report("payload manifest", fails, [])
        # the press gate judges absence too (BUILD-20); the L2
        # constitution peek at dist/stage above deliberately does not,
        # because a stale stage is a note there, not a finding
        f2, notes = walk_stage(stage, rows, mode, complete=True)
        return report("payload manifest (%s press)" % mode, f2, notes)
    sys.stderr.write("unknown command %r\n" % cmd)
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv))
