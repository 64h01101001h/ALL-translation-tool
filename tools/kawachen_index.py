#!/usr/bin/env python3
"""Rename the harvested Kawachen audio descriptively, and index it by sound.

    python3 tools/kawachen_index.py --rename          # dry run, prints the plan
    python3 tools/kawachen_index.py --rename --apply  # do it
    python3 tools/kawachen_index.py --rebuild         # (re)build by-sound/
    python3 tools/kawachen_index.py --check           # verify; nonzero on drift

Two views of the same 3,002 recordings:

  syllable_mp3/49_2_3--klud-glud-blud-rlud-slud.mp3     the file
  by-sound/klud--49_2_3.mp3 -> ../syllable_mp3/49_2_3--…  one link per spelling

The files sort in Tibetan alphabet order, because the site's consonant index
leads the name.  The links sort alphabetically by sound, so `ls by-sound/kla*`
works without opening a CSV.  A merged class is visible in either view: five
files ending `--49_2_3.mp3` in by-sound/ are plainly one recording.

Nothing here deletes audio.  Renames are validated in full before the first
file moves, and a name clash is fatal rather than silently resolved.
"""
import argparse
import csv
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from kawachen_labels import (descriptive_name, site_key_of,      # noqa: E402
                             slug, UNKNOWN, SEP)

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT_DIR, "data", "audio", "kawachen")
MANIFEST = os.path.join(OUT, "manifest.csv")
BY_SOUND = os.path.join(OUT, "by-sound")
DIRS = ("syllable_mp3", "word_mp3")


SKIP = 77          # CTest reads this as "not applicable here", not a failure


def load_manifest(skip_if_absent=False):
    """The manifest, or a clean skip when this checkout simply has no audio.

    The recordings are a gitignored local asset, so a fresh clone has none.
    That is not a failure, and the gate must not report it as one.
    """
    if not os.path.exists(MANIFEST):
        if skip_if_absent:
            print("no Kawachen audio in this checkout — nothing to check")
            sys.exit(SKIP)
        sys.exit("no manifest at %s — run tools/kawachen_harvest.py first" % MANIFEST)
    return list(csv.DictReader(io.open(MANIFEST, encoding="utf-8")))


def plan(rows):
    """(dir, current name, wanted name) for every file, plus what is missing."""
    moves, missing = [], []
    for r in rows:
        d, key = r["dir"], site_key_of(r["file"])
        if key is None:
            missing.append((d, r["file"], "unparseable name"))
            continue
        want = descriptive_name(key, r["wylie"])
        here = None
        for cand in (want, key + ".mp3", r["file"]):
            if os.path.exists(os.path.join(OUT, d, cand)):
                here = cand
                break
        if here is None:
            missing.append((d, r["file"], "not on disk"))
            continue
        if here != want:
            moves.append((d, here, want))
    return moves, missing


def validate(rows):
    """Refuse to touch anything unless every wanted name is safe and unique."""
    seen, fold, bad = {}, {}, []
    for r in rows:
        key = site_key_of(r["file"])
        if key is None:
            continue
        want = descriptive_name(key, r["wylie"])
        if want.count(SEP) < 1:
            bad.append("%s: no %r boundary" % (want, SEP))
        if "/" in want or want.startswith(".") or want.startswith("-"):
            bad.append("%s: unsafe name" % want)
        if len(want.encode("utf-8")) > 255:
            bad.append("%s: over 255 bytes" % want)
        k = (r["dir"], want)
        if k in seen:
            bad.append("collision: %s and %s both want %s"
                       % (seen[k], r["file"], want))
        seen[k] = r["file"]
        kf = (r["dir"], want.lower())
        if kf in fold and fold[kf] != r["file"]:
            bad.append("case-insensitive collision: %s vs %s"
                       % (fold[kf], r["file"]))
        fold[kf] = r["file"]
    return bad


def do_rename(apply_it):
    rows = load_manifest()
    bad = validate(rows)
    if bad:
        print("REFUSING TO RENAME — %d problem(s):" % len(bad))
        for b in bad[:20]:
            print("   ", b)
        return 1
    moves, missing = plan(rows)
    print("manifest rows      %d" % len(rows))
    print("already correct    %d" % (len(rows) - len(moves) - len(missing)))
    print("to rename          %d" % len(moves))
    print("missing on disk    %d" % len(missing))
    for d, a, b in moves[:8]:
        print("    %s/%s  ->  %s" % (d, a, b))
    if len(moves) > 8:
        print("    ... and %d more" % (len(moves) - 8))
    for d, f, why in missing[:5]:
        print("    MISSING %s/%s (%s)" % (d, f, why))
    if not apply_it:
        print("\ndry run — nothing changed. Re-run with --apply.")
        return 0
    done = 0
    for d, a, b in moves:
        src, dst = os.path.join(OUT, d, a), os.path.join(OUT, d, b)
        if os.path.exists(dst):
            print("   skip, target exists: %s" % b)
            continue
        os.rename(src, dst)
        done += 1
    print("\nrenamed %d files" % done)
    rewrite_manifest_names(rows)
    return 0


def rewrite_manifest_names(rows):
    """Point the manifest's `file` column at the new names, atomically."""
    for r in rows:
        key = site_key_of(r["file"])
        if key:
            r["file"] = descriptive_name(key, r["wylie"])
    cols = list(rows[0].keys())
    tmp = MANIFEST + ".tmp"
    with io.open(tmp, "w", encoding="utf-8", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols)
        w.writeheader()
        w.writerows(rows)
    os.replace(tmp, MANIFEST)
    print("manifest updated (%d rows)" % len(rows))


def links_for(rows):
    """(link name, relative target) for every identified spelling."""
    out = []
    for r in rows:
        if r["identified"] != "True":
            continue                      # an unknown gets no link, ever
        key = site_key_of(r["file"])
        if key is None:
            continue
        fname = descriptive_name(key, r["wylie"])
        for part in r["wylie"].split("/"):
            part = part.strip().replace(" ", "_")
            if not part:
                continue
            out.append(("%s%s%s.mp3" % (part, SEP, key),
                        os.path.join("..", r["dir"], fname)))
    return out


def do_rebuild():
    rows = load_manifest()
    links = links_for(rows)
    names = [n for n, _ in links]
    if len(set(names)) != len(names):
        dup = {n for n in names if names.count(n) > 1}
        print("REFUSING — duplicate link names:", sorted(dup)[:5])
        return 1
    if len({n.lower() for n in names}) != len(names):
        print("REFUSING — link names collide when case is folded")
        return 1
    tmp = BY_SOUND + ".tmp"
    shutil.rmtree(tmp, ignore_errors=True)
    os.makedirs(tmp)
    made = 0
    for name, target in links:
        os.symlink(target, os.path.join(tmp, name))
        made += 1
    broken = [n for n, _ in links
              if not os.path.exists(os.path.realpath(os.path.join(tmp, n)))]
    if broken:
        print("REFUSING — %d links point at nothing, e.g. %s"
              % (len(broken), broken[:3]))
        shutil.rmtree(tmp, ignore_errors=True)
        return 1
    shutil.rmtree(BY_SOUND, ignore_errors=True)
    os.rename(tmp, BY_SOUND)
    print("by-sound/: %d links over %d recordings, none broken"
          % (made, len({t for _, t in links})))
    return 0


def do_check():
    rows = load_manifest(skip_if_absent=True)
    moves, missing = plan(rows)
    links = links_for(rows)
    on_disk = set(os.listdir(BY_SOUND)) if os.path.isdir(BY_SOUND) else set()
    want = {n for n, _ in links}
    bad_links = sorted(want - on_disk)
    extra = sorted(on_disk - want)
    ok = not (moves or missing or bad_links or extra)
    print("names correct      %s" % ("yes" if not moves else "NO — %d wrong" % len(moves)))
    print("all files present  %s" % ("yes" if not missing else "NO — %d missing" % len(missing)))
    print("by-sound complete  %s" % ("yes" if not bad_links else "NO — %d absent" % len(bad_links)))
    print("by-sound clean     %s" % ("yes" if not extra else "NO — %d stray" % len(extra)))
    for n in (bad_links[:3] + extra[:3]):
        print("    %s" % n)
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rename", action="store_true")
    ap.add_argument("--apply", action="store_true", help="with --rename, actually move files")
    ap.add_argument("--rebuild", action="store_true")
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args()
    if a.rename:
        return do_rename(a.apply)
    if a.rebuild:
        return do_rebuild()
    if a.check:
        return do_check()
    ap.print_help()
    return 0


if __name__ == "__main__":
    sys.exit(main())
