#!/usr/bin/env python3
"""build_manifest_check.py — prove the build manifest records, and never
invents, what shipped.

SQA 2026-08-22, BUILD-6. tools/build_manifest.py is the answer to "which
OpenSSL did release X carry?", so it has to be provable without Homebrew,
without a network and without a real 400 MB bundle. This builds a fake
Homebrew prefix and a fake .app in a temp dir and asserts on the manifest
it produces.

The claim that matters is house rule 2: a binary the manifest cannot map
to a formula is recorded as UNMAPPED and counted — never dropped, never
given a plausible licence.

Usage:
    build_manifest_check.py <source_dir> <scratch_dir>
"""
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

failures = 0

# a real Mach-O that is on every macOS and belongs to no Homebrew formula
SEED_BINARY = "/bin/echo"


def check(cond, msg):
    global failures
    if cond:
        print("  [PASS] %s" % msg)
    else:
        print("  [FAIL] %s" % msg)
        failures += 1


def build_fake_world(tmp):
    """A Homebrew prefix with one formula, and a bundle with three
    Mach-O objects: ours, a mapped dependency, and a stranger."""
    keg = os.path.join(tmp, "brew", "Cellar", "fakelib", "1.2.3")
    os.makedirs(os.path.join(keg, "lib"))
    os.makedirs(os.path.join(keg, ".brew"))
    with open(os.path.join(keg, ".brew", "fakelib.rb"), "w") as f:
        f.write('class Fakelib < Formula\n'
                '  desc "fixture"\n'
                '  license "MIT"\n'
                'end\n')
    shutil.copy(SEED_BINARY, os.path.join(keg, "lib", "libfake.dylib"))

    app = os.path.join(tmp, "Fake.app")
    os.makedirs(os.path.join(app, "Contents", "MacOS"))
    os.makedirs(os.path.join(app, "Contents", "Frameworks"))
    os.makedirs(os.path.join(app, "Contents", "Resources"))
    shutil.copy(SEED_BINARY, os.path.join(app, "Contents", "MacOS", "Fake"))
    shutil.copy(SEED_BINARY,
                os.path.join(app, "Contents", "Frameworks", "libfake.dylib"))
    shutil.copy(SEED_BINARY,
                os.path.join(app, "Contents", "Frameworks",
                             "libstranger.dylib"))
    with open(os.path.join(app, "Contents", "Resources", "notes.txt"),
              "w") as f:
        f.write("not a Mach-O object\n")

    src = os.path.join(tmp, "src")
    os.makedirs(src)
    with open(os.path.join(src, "VERSION"), "w") as f:
        f.write("9.9.9-fixture\n")
    return app, src


def main(argv):
    if len(argv) < 3:
        print("usage: build_manifest_check.py <source_dir> <scratch_dir>")
        return 2
    source = os.path.abspath(argv[1])
    scratch = os.path.abspath(argv[2])
    os.makedirs(scratch, exist_ok=True)

    tool = os.path.join(source, "tools", "build_manifest.py")
    check(os.path.exists(tool), "tools/build_manifest.py is present")
    if not os.path.exists(tool):
        print("FAILURES")
        return 1
    check(os.path.exists(SEED_BINARY),
          "the Mach-O seed %s exists to build a fixture from" % SEED_BINARY)

    tmp = tempfile.mkdtemp(prefix="all_build_manifest_", dir=scratch)
    try:
        app, src = build_fake_world(tmp)
        out = os.path.join(tmp, "out")
        proc = subprocess.run(
            [sys.executable, tool, "--bundle", app, "--out", out,
             "--source", src, "--brew-prefix", os.path.join(tmp, "brew"),
             "--mode", "market"],
            capture_output=True, text=True)
        check(proc.returncode == 0,
              "the manifest generator succeeds on a well-formed bundle "
              "(exit %d)" % proc.returncode)
        json_path = os.path.join(out, "BUILD_MANIFEST.json")
        txt_path = os.path.join(out, "BUILD_MANIFEST.txt")
        check(os.path.exists(json_path) and os.path.exists(txt_path),
              "both BUILD_MANIFEST.json and BUILD_MANIFEST.txt are written")
        if not os.path.exists(json_path):
            print("FAILURES")
            return 1
        with open(json_path, encoding="utf-8") as f:
            man = json.load(f)

        # --- the toolchain is recorded, and absences are declared ---------
        expected_keys = {"macos_product", "macos_build", "sdk", "clang",
                         "cmake", "python", "homebrew_prefix"}
        missing_keys = expected_keys - set(man.get("toolchain", {}))
        check(not missing_keys,
              "every toolchain field is present%s"
              % ("" if not missing_keys else ": missing %s" % missing_keys))
        blank = [k for k, v in man.get("toolchain", {}).items()
                 if not str(v).strip()]
        check(not blank,
              "no toolchain field is silently blank — a failed probe says "
              "'NOT RECORDED: <reason>'%s"
              % ("" if not blank else ": %s" % blank))
        check(man["toolchain"]["macos_product"].count(".") >= 1
              and not man["toolchain"]["macos_product"].startswith(
                  "NOT RECORDED"),
              "the macOS version the press ran on is recorded (%s)"
              % man["toolchain"]["macos_product"])
        check(man["product"]["version"] == "9.9.9-fixture",
              "the product version comes from the VERSION file")
        check(man["product"]["press_mode"] == "market",
              "the press mode is recorded (team vs market ship different "
              "payloads)")

        # --- BUILD-1: the architecture and floor are in the record --------
        arch = man.get("architecture", {})
        check(any(a in str(arch.get("archs", ""))
                  for a in ("arm64", "x86_64")),
              "the executable's architecture is recorded (%s)"
              % arch.get("archs"))
        check(re.match(r"^\d+\.\d+", str(arch.get("minos", ""))) is not None,
              "the macOS floor baked into the executable is recorded (minos "
              "%s)" % arch.get("minos"))

        # --- components: mapped, unmapped, and nothing invented -----------
        comps = {c["path"]: c for c in man["components"]}
        check(len(comps) == 3,
              "exactly the three Mach-O objects are listed, and the .txt "
              "file is not (%d listed)" % len(comps))
        check("Contents/Resources/notes.txt" not in comps,
              "a non-Mach-O file is not passed off as a bundled binary")

        fake = comps.get("Contents/Frameworks/libfake.dylib", {})
        check(fake.get("formula") == "fakelib"
              and fake.get("formula_version") == "1.2.3"
              and fake.get("license") == "MIT",
              "a bundled dylib is mapped to its formula, version and SPDX "
              "licence (%s %s %s)"
              % (fake.get("formula"), fake.get("formula_version"),
                 fake.get("license")))

        stranger = comps.get("Contents/Frameworks/libstranger.dylib", {})
        check(stranger.get("license") == "UNMAPPED",
              "a binary no formula owns is recorded as UNMAPPED, not given "
              "a plausible licence (got %r)" % stranger.get("license"))
        check("Contents/Frameworks/libstranger.dylib"
              in man["summary"]["unmapped_paths"],
              "the unmapped binary is named in the summary, not just "
              "buried in the component list")
        check(man["summary"]["unmapped"] == 1
              and man["summary"]["mapped"] + man["summary"]["unmapped"]
              == man["summary"]["components"],
              "the summary counts add up (mapped %s + unmapped %s = %s)"
              % (man["summary"]["mapped"], man["summary"]["unmapped"],
                 man["summary"]["components"]))

        ours = comps.get("Contents/MacOS/Fake", {})
        check(ours.get("formula") == "(built by this project)",
              "the product's own binary is labelled as ours, not left to "
              "look like an unmapped third party")

        bad_hash = [p for p, c in comps.items()
                    if not re.fullmatch(r"[0-9a-f]{64}", c.get("sha256", ""))
                    or c.get("size", 0) <= 0]
        check(not bad_hash,
              "every component carries a sha256 and a real size%s"
              % ("" if not bad_hash else ": %s" % bad_hash))

        with open(txt_path, encoding="utf-8") as f:
            txt = f.read()
        check("UNMAPPED" in txt and "libstranger.dylib" in txt,
              "the human-readable manifest surfaces the unmapped binary "
              "too — a reader of the DMG sees it")

        # --- --strict is a real gate --------------------------------------
        strict = subprocess.run(
            [sys.executable, tool, "--bundle", app, "--out", out,
             "--source", src, "--brew-prefix", os.path.join(tmp, "brew"),
             "--strict"],
            capture_output=True, text=True)
        check(strict.returncode != 0,
              "--strict fails the press when a shipped binary has no "
              "recorded licence (exit %d)" % strict.returncode)

        # --- the press actually uses it -----------------------------------
        press = os.path.join(source, "tools", "package_macos.sh")
        press_text = ""
        if os.path.exists(press):
            with open(press, encoding="utf-8") as f:
                press_text = f.read()
        check("build_manifest.py" in press_text,
              "the press runs tools/build_manifest.py — the manifest is not "
              "dead code")
        check("BUILD_MANIFEST" in press_text,
              "the press stages BUILD_MANIFEST into the distribution")

        # --- the declared dependencies exist ------------------------------
        brewfile = os.path.join(source, "Brewfile")
        brew_text = ""
        if os.path.exists(brewfile):
            with open(brewfile, encoding="utf-8") as f:
                brew_text = f.read()
        check(bool(brew_text), "a Brewfile declares the build dependencies")
        declared = set(re.findall(r'^\s*brew\s+"([^"]+)"', brew_text, re.M))
        needed = {"cmake", "qt", "onnxruntime", "opencv"}
        check(needed <= declared,
              "the Brewfile declares the formulae the CMake files look for%s"
              % ("" if needed <= declared
                 else ": missing %s" % sorted(needed - declared)))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)

    print("build_manifest: %d failure(s)" % failures)
    print("FAILURES" if failures else "build_manifest OK")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
