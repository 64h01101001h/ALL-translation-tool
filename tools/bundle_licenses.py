#!/usr/bin/env python3
"""bundle_licenses.py — name every binary in the bundle, and ship its
licence text with it.

Written 2026-08-23 for SQA BUILD-2.

WHAT WENT WRONG. OPEN_SOURCE_NOTICES.md named six software components.
The staged bundle carried 152 Mach-O objects from ~33 distinct upstream
projects, of which three were named — roughly 9% coverage. Shipped and
unrecorded: libgfortran / libquadmath / libgcc_s (GPL-3.0-or-later with
the GCC Runtime Library Exception, pulled in transitively through
OpenBLAS), glib / gobject / gio (LGPL-2.1-or-later), OpenSSL, OpenCV,
TBB, RE2, protobuf, ICU, freetype, and the Qt Pdf/Qml/Quick frameworks.
The DMG contained six licence files, all data-layer; there was no LGPL
text at all, which LGPL-3.0 §4 requires be supplied with a
distribution.

WHAT THIS DOES. It walks the staged bundle, maps every Mach-O object to
the Homebrew formula that produced it (by filename, against the local
Cellar — no network), reads that formula's version and SPDX licence id
from Homebrew's own local metadata, copies the licence texts out of the
keg into Contents/Resources/licenses/<formula>/, and writes
Contents/Resources/licenses/BUNDLED_COMPONENTS.tsv.

It FAILS the press on any Mach-O object it cannot map, because an
unmappable binary is exactly the case that produced the finding. Where
a keg carries no licence text (Homebrew's qt and glib kegs do not), the
component's row says LICENSE_TEXT_MISSING and the summary prints it —
flagged and surfaced, never quietly absent. Supply the text yourself by
dropping it in docs/licenses/texts/<SPDX-id>.txt and it will be picked
up and shipped; --require-texts turns a missing text into a failure,
which is what a market press uses.

Usage:
  bundle_licenses.py <app.app> <repo_root> [--require-texts] [--out DIR]

--out writes the licences tree somewhere other than the bundle, so the
tool can be exercised against a read-only bundle.
"""
import json
import os
import re
import shutil
import subprocess
import sys

CELLAR = "/opt/homebrew/Cellar"
LICENCE_NAMES = re.compile(
    r"^(LICENSE|LICENCE|COPYING|NOTICE|COPYRIGHT)"
    r"([._-].*)?$|^.*(LGPL|GPL|BSD|MIT|APACHE)[^/]*\.txt$",
    re.I)
# Objects the bundle owns. Everything else must map to a formula.
OURS = ("DiamondCutterTranslationTool",)


def machos(app):
    """Every Mach-O object in the bundle, as (relative path, basename)."""
    out = []
    for dirpath, _dirs, files in os.walk(app):
        for name in files:
            full = os.path.join(dirpath, name)
            if os.path.islink(full):
                continue
            try:
                head = open(full, "rb").read(4)
            except OSError:
                continue
            # Mach-O 64 little/big endian + universal
            if head not in (b"\xcf\xfa\xed\xfe", b"\xfe\xed\xfa\xcf",
                            b"\xca\xfe\xba\xbe", b"\xbe\xba\xfe\xca"):
                continue
            out.append((os.path.relpath(full, app), name))
    return sorted(out)


def cellar_index():
    """basename -> set of (formula, version), built from the local
    Cellar only. No network, no `brew` subprocess."""
    idx = {}
    if not os.path.isdir(CELLAR):
        return idx
    for formula in os.listdir(CELLAR):
        fdir = os.path.join(CELLAR, formula)
        if not os.path.isdir(fdir):
            continue
        for version in os.listdir(fdir):
            vdir = os.path.join(fdir, version)
            if not os.path.isdir(vdir):
                continue
            for dirpath, dirs, files in os.walk(vdir):
                for name in files + dirs:
                    if name.endswith(".dylib") or name.endswith(".so"):
                        idx.setdefault(name, set()).add((formula, version))
                    elif name.endswith(".framework"):
                        # the framework's binary carries the bare name
                        idx.setdefault(name[:-10], set()).add(
                            (formula, version))
    return idx


def brew_meta():
    """formula -> (installed version, SPDX licence). Local metadata."""
    env = dict(os.environ, HOMEBREW_NO_AUTO_UPDATE="1")
    try:
        raw = subprocess.run(
            ["brew", "info", "--json=v2", "--installed"],
            capture_output=True, text=True, env=env, timeout=180)
    except (OSError, subprocess.SubprocessError) as exc:
        sys.stderr.write("   (brew metadata unavailable: %s)\n" % exc)
        return {}
    if raw.returncode != 0:
        sys.stderr.write("   (brew info failed: rc=%d)\n" % raw.returncode)
        return {}
    try:
        doc = json.loads(raw.stdout)
    except ValueError as exc:
        sys.stderr.write("   (brew json unreadable: %s)\n" % exc)
        return {}
    meta = {}
    for f in doc.get("formulae", []):
        inst = f.get("installed") or []
        meta[f["name"]] = (inst[0]["version"] if inst else "",
                           f.get("license") or "")
    return meta


# Sub-formulas whose objects are the same upstream project. Folding
# them keeps the component count honest: 13 Qt frameworks are one Qt.
FOLD = {"qtbase": "qt", "qtdeclarative": "qt", "qtwebengine": "qt",
        "qtsvg": "qt", "qttools": "qt", "qt5": "qt"}


def pick(hits):
    """One formula for an object that several kegs contain."""
    folded = sorted({FOLD.get(f, f) for f, _v in hits})
    if len(folded) == 1:
        # prefer the keg whose own name matches the folded name
        for f, v in sorted(hits):
            if f == folded[0]:
                return f, v
        return sorted(hits)[0][0], sorted(hits)[0][1]
    # genuinely ambiguous: prefer a non-folded (top-level) formula
    for f, v in sorted(hits):
        if f not in FOLD:
            return f, v
    return sorted(hits)[0]


def copy_texts(formula, version, dest, repo, spdx):
    """Copy the keg's licence texts. Returns the list of filenames
    shipped (possibly empty)."""
    shipped = []
    keg = os.path.join(CELLAR, formula, version)
    if os.path.isdir(keg):
        for dirpath, dirs, files in os.walk(keg):
            depth = os.path.relpath(dirpath, keg).count(os.sep)
            if depth >= 3:
                dirs[:] = []
                continue
            for name in files:
                if not LICENCE_NAMES.match(name):
                    continue
                src = os.path.join(dirpath, name)
                if os.path.islink(src) or os.path.getsize(src) > 400000:
                    continue
                os.makedirs(dest, exist_ok=True)
                out = name if name not in shipped else \
                    os.path.relpath(src, keg).replace(os.sep, "_")
                shutil.copyfile(src, os.path.join(dest, out))
                shipped.append(out)
    if shipped:
        return shipped
    # nothing in the keg — take a text the project supplied by hand
    for ident in [i for i in re.split(r"[^A-Za-z0-9.+-]+", spdx) if i]:
        cand = os.path.join(repo, "docs/licenses/texts", ident + ".txt")
        if os.path.isfile(cand):
            os.makedirs(dest, exist_ok=True)
            shutil.copyfile(cand, os.path.join(dest, ident + ".txt"))
            shipped.append(ident + ".txt")
    return shipped


def main(argv):
    if len(argv) < 3:
        sys.stderr.write(__doc__)
        return 2
    app, repo = argv[1], argv[2]
    require_texts = "--require-texts" in argv[3:]
    licdir_override = None
    if "--out" in argv[3:]:
        licdir_override = argv[argv.index("--out") + 1]
    if not os.path.isdir(app):
        sys.stderr.write("no bundle at %s\n" % app)
        return 2

    objects = machos(app)
    idx = cellar_index()
    meta = brew_meta()

    components = {}          # folded name -> dict
    unmapped = []
    for rel, name in objects:
        if name in OURS:
            continue
        hits = idx.get(name)
        if not hits:
            unmapped.append(rel)
            continue
        formula, version = pick(hits)
        folded = FOLD.get(formula, formula)
        v, spdx = meta.get(formula, ("", ""))
        if not v:
            v = version
        if not spdx:
            v2, s2 = meta.get(folded, ("", ""))
            spdx = s2
            v = v or v2
        comp = components.setdefault(folded, {
            "formula": folded, "keg": formula, "version": v,
            "spdx": spdx, "objects": []})
        comp["objects"].append(rel)
        if not comp["spdx"] and spdx:
            comp["spdx"] = spdx

    if unmapped:
        print("BUNDLED COMPONENTS: %d Mach-O object(s) map to no "
              "Homebrew formula and therefore have no recorded licence:"
              % len(unmapped))
        for rel in sorted(unmapped):
            print("  UNMAPPED " + rel)
        print("Add the formula to the Cellar index, or add the object's "
              "name to OURS in tools/bundle_licenses.py if this project "
              "built it.")
        return 1

    licdir = licdir_override or os.path.join(
        app, "Contents/Resources/licenses")
    shutil.rmtree(licdir, ignore_errors=True)
    os.makedirs(licdir, exist_ok=True)

    missing = []
    rows = []
    for name in sorted(components):
        comp = components[name]
        texts = copy_texts(comp["keg"], comp["version"] or
                           keg_version(comp["keg"]),
                           os.path.join(licdir, name), repo,
                           comp["spdx"])
        if not texts:
            missing.append((name, comp["spdx"]))
        rows.append((name, comp["version"], comp["spdx"] or "UNRECORDED",
                     ";".join(texts) if texts else "LICENSE_TEXT_MISSING",
                     str(len(comp["objects"]))))

    with open(os.path.join(licdir, "BUNDLED_COMPONENTS.tsv"), "w",
              encoding="utf-8") as fh:
        fh.write("# Generated by tools/bundle_licenses.py from the "
                 "staged bundle. One row per upstream project.\n")
        fh.write("# component\tversion\tspdx\tlicense_texts\tobjects\n")
        for row in rows:
            fh.write("\t".join(row) + "\n")

    print("   %d Mach-O objects -> %d upstream projects; licence texts "
          "in Contents/Resources/licenses/" % (len(objects), len(rows)))
    unrecorded = [r[0] for r in rows if r[2] == "UNRECORDED"]
    if unrecorded:
        print("   NO SPDX ID RECORDED for: " + ", ".join(unrecorded))
    if missing:
        print("   LICENCE TEXT MISSING for %d component(s) — the keg "
              "carries none:" % len(missing))
        for name, spdx in missing:
            print("     %-22s %s   (drop the text at "
                  "docs/licenses/texts/<spdx>.txt to ship it)"
                  % (name, spdx or "no SPDX id"))
    # Strong copyleft with no alternative and no linking exception is
    # the question a distribution must answer deliberately, so it is
    # named on every press instead of sitting in a table nobody reads.
    copyleft = [r[0] for r in rows if flags_copyleft(r[2])]
    if copyleft:
        print("   REVIEW — strong copyleft with no stated alternative "
              "and no linking exception: " + ", ".join(copyleft))
        for row in rows:
            if row[0] in copyleft:
                print("     %-22s %s" % (row[0], row[2]))
    if (missing or unrecorded) and require_texts:
        print("   --require-texts: refusing to press with licence texts "
              "missing.")
        return 1
    return 0


def flags_copyleft(spdx):
    """True when the expression names a GPL without an `OR`
    alternative and without a `WITH ... exception`. Deliberately
    advisory: it points a human at the row, it does not judge it."""
    if "GPL" not in spdx.upper():
        return False
    up = spdx.upper()
    if "LGPL" in up and "GPL-" not in up.replace("LGPL-", ""):
        return False
    if " OR " in up or "WITH " in up:
        return False
    return True


def keg_version(formula):
    d = os.path.join(CELLAR, formula)
    if os.path.isdir(d):
        vs = sorted(os.listdir(d))
        if vs:
            return vs[-1]
    return ""


if __name__ == "__main__":
    sys.exit(main(sys.argv))
