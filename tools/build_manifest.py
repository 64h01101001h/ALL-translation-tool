#!/usr/bin/env python3
"""build_manifest.py — write down what this release was actually built from.

SQA 2026-08-22, BUILD-6: there is no lockfile, no CI and no record in the
DMG of which Homebrew bottles macdeployqt copied in. A shipped OpenSSL CVE
could not be answered with "which version did release X carry?", and two
presses on two machines were not guaranteed to produce the same artifact.

This walks a staged .app, and writes BUILD_MANIFEST.json (machine) and
BUILD_MANIFEST.txt (human) beside it:

  * the toolchain — macOS product+build, SDK, clang, cmake, python, Qt, git
    commit and whether the tree was dirty
  * the architecture and macOS floor actually baked into the executable
    (lipo -archs, LC_BUILD_VERSION minos/sdk) — BUILD-1's "make a change
    visible"
  * every Mach-O in the bundle: sha256, size, and the Homebrew formula,
    formula version and SPDX licence it came from

House rule 2 (never guess): a binary that cannot be mapped to a formula is
recorded as "UNMAPPED" and counted in the summary — never dropped, never
given a plausible-looking licence. A probe that fails records
"NOT RECORDED: <reason>" rather than an empty string. --strict turns any
UNMAPPED component into a non-zero exit, which is the gate BUILD-2 wants
once the licence work lands.

Usage:
    build_manifest.py --bundle <path/to/App.app> --out <dir>
                      [--source <repo root>] [--brew-prefix /opt/homebrew]
                      [--payload <data dir>] [--mode team|market] [--strict]
"""
import argparse
import datetime
import hashlib
import json
import os
import re
import subprocess
import sys

UNMAPPED = "UNMAPPED"


def probe(cmd, pattern=None, cwd=None):
    """Run a version probe. Returns the value, or 'NOT RECORDED: <why>'."""
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd,
                              timeout=30)
    except (OSError, subprocess.SubprocessError) as exc:
        return "NOT RECORDED: %s (%s)" % (" ".join(cmd), exc)
    if proc.returncode != 0:
        return "NOT RECORDED: %s exited %d" % (" ".join(cmd), proc.returncode)
    text = (proc.stdout or proc.stderr or "").strip()
    if pattern:
        m = re.search(pattern, text)
        if not m:
            return "NOT RECORDED: %s matched no version in its output" % (
                " ".join(cmd))
        return m.group(1)
    first = text.splitlines()[0].strip() if text else ""
    return first or "NOT RECORDED: %s printed nothing" % " ".join(cmd)


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def is_macho(path):
    try:
        with open(path, "rb") as f:
            magic = f.read(4)
    except OSError:
        return False
    return magic in (b"\xcf\xfa\xed\xfe", b"\xce\xfa\xed\xfe",
                     b"\xfe\xed\xfa\xcf", b"\xfe\xed\xfa\xce",
                     b"\xca\xfe\xba\xbe", b"\xbe\xba\xfe\xca")


def parse_license(formula_rb):
    """The SPDX expression from a Homebrew formula, or an honest marker."""
    try:
        with open(formula_rb, encoding="utf-8", errors="replace") as f:
            text = f.read()
    except OSError as exc:
        return "NOT RECORDED: %s (%s)" % (formula_rb, exc)
    m = re.search(r'^\s*license\s+"([^"]+)"', text, re.M)
    if m:
        return m.group(1)
    m = re.search(r'^\s*license\s+:(\w+)', text, re.M)
    if m:
        return m.group(1)
    m = re.search(r'^\s*license\s+(all_of|any_of):\s*\[(.*?)\]', text,
                  re.M | re.S)
    if m:
        parts = re.findall(r'"([^"]+)"', m.group(2))
        joiner = " AND " if m.group(1) == "all_of" else " OR "
        if parts:
            return "(" + joiner.join(parts) + ")"
    return "NOT RECORDED: no license stanza parsed in %s" % formula_rb


def index_cellar(prefix):
    """basename -> (formula, version, licence). Built from the Cellar itself,
    so no `brew` invocation and no network."""
    index = {}
    cellar = os.path.join(prefix, "Cellar")
    if not os.path.isdir(cellar):
        return index, "NOT RECORDED: no Cellar at %s" % cellar
    licences = {}
    for formula in sorted(os.listdir(cellar)):
        fdir = os.path.join(cellar, formula)
        if not os.path.isdir(fdir):
            continue
        for version in sorted(os.listdir(fdir)):
            vdir = os.path.join(fdir, version)
            if not os.path.isdir(vdir):
                continue
            key = (formula, version)
            if key not in licences:
                licences[key] = parse_license(
                    os.path.join(vdir, ".brew", "%s.rb" % formula))
            # the whole keg, not just lib/: Qt's plugins (libqcocoa and
            # friends, which macdeployqt copies into Contents/PlugIns)
            # live under share/qt/plugins
            for root, _dirs, files in os.walk(vdir):
                in_framework = ".framework/" in (root + "/")
                for name in files:
                    # symlinks ARE indexed: macdeployqt copies the
                    # major-version alias (libbrotlicommon.1.dylib), which
                    # in the Cellar is a link to the full-version file.
                    # Both names belong to the same formula.
                    ext = os.path.splitext(name)[1]
                    if ext not in (".dylib", ".so") and not in_framework:
                        continue
                    # first writer wins; a later formula shadowing a
                    # basename is recorded as a collision, not silently
                    # overwritten
                    prev = index.get(name)
                    if prev is None:
                        index[name] = [formula, version, licences[key]]
                    elif prev[0] != formula:
                        prev[0] = "%s|%s" % (prev[0], formula)
    return index, None


def architecture(executable):
    arch = probe(["lipo", "-archs", executable])
    minos, sdk, platform = ("NOT RECORDED: otool -l produced no "
                            "LC_BUILD_VERSION",) * 3
    try:
        out = subprocess.run(["otool", "-l", executable], capture_output=True,
                             text=True, timeout=60).stdout
    except (OSError, subprocess.SubprocessError) as exc:
        return {"archs": arch, "minos": "NOT RECORDED: %s" % exc,
                "sdk": "NOT RECORDED: %s" % exc,
                "platform": "NOT RECORDED: %s" % exc}
    block = re.search(r"LC_BUILD_VERSION(.*?)(?=cmd |\Z)", out, re.S)
    if block:
        body = block.group(1)
        for key, field in (("minos", "minos"), ("sdk", "sdk"),
                           ("platform", "platform")):
            m = re.search(r"\b%s\s+(\S+)" % field, body)
            if m:
                if key == "minos":
                    minos = m.group(1)
                elif key == "sdk":
                    sdk = m.group(1)
                else:
                    platform = m.group(1)
    return {"archs": arch, "minos": minos, "sdk": sdk, "platform": platform}


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--bundle", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--source", default=None)
    ap.add_argument("--brew-prefix", default="/opt/homebrew")
    ap.add_argument("--payload", default=None)
    ap.add_argument("--mode", default="team")
    ap.add_argument("--strict", action="store_true")
    args = ap.parse_args(argv)

    bundle = os.path.abspath(args.bundle)
    if not os.path.isdir(bundle):
        print("build_manifest: no bundle at %s" % bundle, file=sys.stderr)
        return 2
    source = os.path.abspath(args.source) if args.source else None

    macos_dir = os.path.join(bundle, "Contents", "MacOS")
    executable = None
    if os.path.isdir(macos_dir):
        for name in sorted(os.listdir(macos_dir)):
            path = os.path.join(macos_dir, name)
            if os.path.isfile(path) and is_macho(path):
                executable = path
                break

    version = "NOT RECORDED: no VERSION file"
    if source and os.path.exists(os.path.join(source, "VERSION")):
        with open(os.path.join(source, "VERSION")) as f:
            version = f.readline().strip()

    git_commit = git_dirty = "NOT RECORDED: no source tree given"
    if source:
        git_commit = probe(["git", "-C", source, "rev-parse", "HEAD"])
        status = probe(["git", "-C", source, "status", "--porcelain"])
        if status.startswith("NOT RECORDED"):
            git_dirty = status
        else:
            git_dirty = "clean" if status == "" else "DIRTY"

    manifest = {
        "schema": "all-build-manifest/1",
        "generated_utc": datetime.datetime.now(
            datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "product": {
            "name": os.path.basename(bundle),
            "version": version,
            "press_mode": args.mode,
            "git_commit": git_commit,
            "git_tree": git_dirty,
        },
        "toolchain": {
            "macos_product": probe(["sw_vers", "-productVersion"]),
            "macos_build": probe(["sw_vers", "-buildVersion"]),
            "sdk": probe(["xcrun", "--sdk", "macosx", "--show-sdk-version"]),
            "clang": probe(["clang++", "--version"]),
            "cmake": probe(["cmake", "--version"],
                           r"cmake version (\S+)"),
            "python": probe([sys.executable, "--version"],
                            r"Python (\S+)"),
            "homebrew_prefix": args.brew_prefix,
            "homebrew": probe(["git", "-C",
                               os.path.join(args.brew_prefix, "Library",
                                            "Homebrew"), "rev-parse",
                               "--short", "HEAD"]),
        },
        "architecture": (architecture(executable) if executable else
                         {"archs": "NOT RECORDED: no Mach-O executable "
                                   "found in the bundle"}),
        "components": [],
        "payload": [],
    }

    index, cellar_note = index_cellar(args.brew_prefix)
    if cellar_note:
        manifest["toolchain"]["homebrew_cellar"] = cellar_note

    unmapped = []
    for root, _dirs, files in os.walk(bundle):
        for name in sorted(files):
            path = os.path.join(root, name)
            if os.path.islink(path) or not os.path.isfile(path):
                continue
            if not is_macho(path):
                continue
            rel = os.path.relpath(path, bundle)
            mapped = index.get(name)
            if path == executable:
                # the product's own binary: not a dependency, and this
                # project ships no LICENSE file (SQA BUILD-2), so say so
                formula, formula_version = "(built by this project)", version
                licence = ("NOT RECORDED: this project ships no LICENSE "
                           "file (SQA BUILD-2)")
            elif mapped:
                formula, formula_version, licence = mapped
            else:
                formula, formula_version, licence = (UNMAPPED, UNMAPPED,
                                                     UNMAPPED)
                unmapped.append(rel)
            manifest["components"].append({
                "path": rel,
                "size": os.path.getsize(path),
                "sha256": sha256(path),
                "formula": formula,
                "formula_version": formula_version,
                "license": licence,
            })

    if args.payload and os.path.isdir(args.payload):
        build_dir = os.path.join(args.payload, "build")
        if os.path.isdir(build_dir):
            for name in sorted(os.listdir(build_dir)):
                path = os.path.join(build_dir, name)
                if os.path.isfile(path) and not os.path.islink(path):
                    manifest["payload"].append({
                        "path": os.path.join("build", name),
                        "size": os.path.getsize(path),
                        "sha256": sha256(path),
                    })

    manifest["summary"] = {
        "components": len(manifest["components"]),
        "mapped": len(manifest["components"]) - len(unmapped),
        "unmapped": len(unmapped),
        "unmapped_paths": unmapped,
        "payload_files_hashed": len(manifest["payload"]),
    }

    os.makedirs(args.out, exist_ok=True)
    json_path = os.path.join(args.out, "BUILD_MANIFEST.json")
    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(manifest, f, indent=2, sort_keys=False)
        f.write("\n")

    lines = []
    lines.append("Diamond Cutter Translation Tool — build manifest")
    lines.append("=" * 62)
    lines.append("")
    lines.append("This file records what this copy was built from, so a "
                 "question about")
    lines.append("a dependency version or a licence has an answer that does "
                 "not depend")
    lines.append("on anyone's memory. Generated by tools/build_manifest.py.")
    lines.append("")
    for section in ("product", "toolchain", "architecture"):
        lines.append("[%s]" % section)
        for key, value in manifest[section].items():
            lines.append("  %-20s %s" % (key, value))
        lines.append("")
    lines.append("[bundled binaries]  %d Mach-O objects, %d mapped to a "
                 "Homebrew formula, %d UNMAPPED"
                 % (manifest["summary"]["components"],
                    manifest["summary"]["mapped"],
                    manifest["summary"]["unmapped"]))
    lines.append("")
    lines.append("  %-46s %-22s %s" % ("formula", "version", "licence"))
    seen = {}
    for comp in manifest["components"]:
        seen.setdefault((comp["formula"], comp["formula_version"],
                         comp["license"]), 0)
        seen[(comp["formula"], comp["formula_version"],
              comp["license"])] += 1
    for (formula, fver, licence), count in sorted(seen.items()):
        lines.append("  %-46s %-22s %s  (%d file%s)"
                     % (formula, fver, licence, count,
                        "" if count == 1 else "s"))
    if unmapped:
        lines.append("")
        lines.append("  UNMAPPED — no Homebrew formula owns these; their "
                     "licence is UNRECORDED:")
        for rel in unmapped:
            lines.append("    %s" % rel)
    if manifest["payload"]:
        lines.append("")
        lines.append("[data payload]")
        for item in manifest["payload"]:
            lines.append("  %-46s %14d  %s"
                         % (item["path"], item["size"], item["sha256"]))
    lines.append("")
    txt_path = os.path.join(args.out, "BUILD_MANIFEST.txt")
    with open(txt_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

    print("build_manifest: %d Mach-O objects, %d mapped, %d UNMAPPED -> %s"
          % (manifest["summary"]["components"], manifest["summary"]["mapped"],
             manifest["summary"]["unmapped"], json_path))
    if unmapped:
        print("build_manifest: UNMAPPED (licence unrecorded): %s"
              % ", ".join(unmapped[:8])
              + (" …" if len(unmapped) > 8 else ""))
    if args.strict and unmapped:
        print("build_manifest: --strict and %d component(s) unmapped"
              % len(unmapped))
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
