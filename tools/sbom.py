#!/usr/bin/env python3
"""Emit a CycloneDX software bill of materials for a staged bundle.

Written 2026-08-24 for the P1 gap "No SBOM". BUILD-2 produced the hard
part already: tools/bundle_licenses.py walks the staged bundle, maps
every Mach-O object to the Homebrew formula that produced it, and
writes Contents/Resources/licenses/BUNDLED_COMPONENTS.tsv with a
version and an SPDX id per project. This turns that into the
machine-readable artifact institutions ask for.

TWO LAYERS, because for THIS product the second one is the exposure.
A conventional SBOM lists software. The licensing finding that actually
cost this project a day (BUILD-5) was DATA - 1,926 THL catalogue
records shipping inside a DMG whose notices said they were not
redistributed. So the data layers from docs/distribution/
PAYLOAD_MANIFEST.txt are emitted as components too, with their licence
and their manifest status. An SBOM for this app that covered only the
dylibs would omit the half that has already gone wrong.

HONEST LIMITS, stated in the document itself rather than implied:
  * SPDX ids for software come from Homebrew's local metadata, which
    is the packager's reading of upstream, not a legal audit.
  * Data-layer rows marked `gated` or `itemise` in the payload manifest
    carry terms that are OPEN, not settled. They are emitted with that
    status visible rather than quietly given a licence id.
  * A component this cannot map is a failure, not a blank row -
    bundle_licenses.py already fails the press on one.

Usage:
  tools/sbom.py <staged.app> [--out FILE]
  tools/sbom.py --selftest
"""
import argparse
import io
import json
import os
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def read_components_tsv(path):
    """component, version, spdx, license_texts, objects"""
    out = []
    for line in io.open(path, encoding="utf-8"):
        line = line.rstrip("\n")
        if not line or line.startswith("#"):
            continue
        parts = line.split("\t")
        if len(parts) < 5:
            continue
        name, version, spdx, texts, objects = parts[:5]
        out.append({"name": name, "version": version, "spdx": spdx,
                    "texts": texts, "objects": objects})
    return out


def read_payload_manifest(path):
    """mode | status | path | licence | anchor | provenance"""
    out = []
    for line in io.open(path, encoding="utf-8"):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = [p.strip() for p in line.split("|")]
        if len(parts) < 6:
            continue
        mode, status, p, licence, anchor, prov = parts[:6]
        if status == "drop":          # deliberately not shipped
            continue
        if p.endswith("/") and licence.startswith("itemised"):
            continue                  # the container row, not a payload
        out.append({"mode": mode, "status": status, "path": p,
                    "licence": licence, "anchor": anchor})
    return out


def git_head():
    try:
        return subprocess.run(["git", "-C", ROOT, "rev-parse", "HEAD"],
                              capture_output=True, text=True).stdout.strip() \
            or "UNRESOLVED"
    except Exception:
        # say so in the document rather than shipping a blank field
        return "UNRESOLVED"


def build_sbom(software, data, app_version, head, stamp):
    comps = []
    for c in software:
        comp = {
            "type": "library",
            "name": c["name"],
            "version": c["version"],
            "purl": "pkg:brew/%s@%s" % (c["name"], c["version"]),
            "properties": [
                {"name": "all:machO_objects", "value": str(c["objects"])},
                {"name": "all:license_texts_shipped", "value": c["texts"]},
            ],
        }
        spdx = (c["spdx"] or "").strip()
        if spdx and spdx.upper() not in ("", "NOASSERTION", "UNKNOWN"):
            comp["licenses"] = [{"license": {"id": spdx}}]
        else:
            # never invent an id; say it is unresolved
            comp["licenses"] = [{"license": {"name": "UNRESOLVED"}}]
        comps.append(comp)

    for d in data:
        settled = d["status"] == "ok"
        comp = {
            "type": "data",
            "name": d["path"],
            "properties": [
                {"name": "all:payload_status", "value": d["status"]},
                {"name": "all:press_mode", "value": d["mode"]},
                {"name": "all:notices_anchor", "value": d["anchor"]},
            ],
        }
        comp["licenses"] = [{"license": {"name": d["licence"]}}] if settled \
            else [{"license": {"name": "TERMS OPEN (%s) - %s"
                               % (d["status"], d["licence"])}}]
        comps.append(comp)

    return {
        "bomFormat": "CycloneDX",
        "specVersion": "1.5",
        "version": 1,
        "metadata": {
            "timestamp": stamp,
            "component": {
                "type": "application",
                "name": "Diamond Cutter Translation Tool",
                "version": app_version,
            },
            "tools": [{"name": "tools/sbom.py",
                       "vendor": "Asian Legacy Library"}],
            "properties": [
                {"name": "all:git_commit", "value": head},
                {"name": "all:caveat",
                 "value": "SPDX ids for software come from Homebrew local "
                          "metadata - the packager's reading of upstream, "
                          "not a legal audit. Data rows whose payload "
                          "status is not 'ok' carry terms that are OPEN, "
                          "not settled, and say so instead of being given "
                          "a licence id."},
            ],
        },
        "components": comps,
    }


def cmd_selftest():
    bad = []
    d = tempfile.mkdtemp(prefix="sbom_selftest_")
    tsv = os.path.join(d, "c.tsv")
    io.open(tsv, "w", encoding="utf-8").write(
        "# header comment\n"
        "abseil\t20260107.1\tApache-2.0\tLICENSE\t78\n"
        "mystery\t1.0\t\t\t2\n")
    man = os.path.join(d, "m.txt")
    io.open(man, "w", encoding="utf-8").write(
        "# comment\n"
        "both | ok | Data/x.tsv | CC-BY-4.0 | 84000 glossary | note\n"
        "both | gated | Data/soas | terms unlocated | SOAS POS | note\n"
        "both | drop | Data/thl_records | unreviewed | THL | note\n")

    sw = read_components_tsv(tsv)
    da = read_payload_manifest(man)
    if len(sw) != 2:
        bad.append("software rows parsed")
    if len(da) != 2:
        bad.append("a `drop` row must NOT appear in the SBOM - it is "
                   "deliberately not shipped")

    doc = build_sbom(sw, da, "1.0.0-rc.1", "deadbeef", "2026-08-24T00:00:00Z")
    blob = json.dumps(doc)
    if doc["bomFormat"] != "CycloneDX" or doc["specVersion"] != "1.5":
        bad.append("CycloneDX envelope")
    if "thl_records" in blob:
        bad.append("a dropped payload leaked into the SBOM")

    mystery = [c for c in doc["components"] if c["name"] == "mystery"][0]
    if mystery["licenses"][0]["license"].get("name") != "UNRESOLVED":
        bad.append("a component with no SPDX id must say UNRESOLVED, "
                   "never be given a plausible one")
    soas = [c for c in doc["components"] if c["name"] == "Data/soas"][0]
    if "TERMS OPEN" not in soas["licenses"][0]["license"]["name"]:
        bad.append("a payload row whose status is not 'ok' must say its "
                   "terms are OPEN rather than read as licensed")

    import shutil
    shutil.rmtree(d, ignore_errors=True)
    for b in bad:
        print("  [FAIL]", b)
    print("sbom selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("app", nargs="?")
    ap.add_argument("--out", default=None)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return cmd_selftest()
    if not a.app:
        ap.error("need a staged .app (or --selftest)")

    tsv = os.path.join(a.app, "Contents", "Resources", "licenses",
                       "BUNDLED_COMPONENTS.tsv")
    if not os.path.exists(tsv):
        print("no BUNDLED_COMPONENTS.tsv in the bundle - run the press "
              "first; tools/bundle_licenses.py writes it.")
        return 1
    man = os.path.join(ROOT, "docs", "distribution", "PAYLOAD_MANIFEST.txt")
    software = read_components_tsv(tsv)
    data = read_payload_manifest(man) if os.path.exists(man) else []
    version = io.open(os.path.join(ROOT, "VERSION"),
                      encoding="utf-8").read().strip().split("\n")[0]
    import datetime
    stamp = datetime.datetime.now(datetime.timezone.utc).strftime(
        "%Y-%m-%dT%H:%M:%SZ")
    doc = build_sbom(software, data, version, git_head(), stamp)
    out = a.out or os.path.join(a.app, "Contents", "Resources", "licenses",
                                "sbom.cdx.json")
    io.open(out, "w", encoding="utf-8").write(
        json.dumps(doc, indent=2, ensure_ascii=False) + "\n")
    print("wrote %s" % out)
    print("  %d software component(s), %d data layer(s)"
          % (len(software), len(data)))
    unres = sum(1 for c in doc["components"]
                if c["licenses"][0]["license"].get("name") == "UNRESOLVED")
    openterms = sum(1 for c in doc["components"]
                    if "TERMS OPEN" in str(c["licenses"][0]["license"]))
    print("  %d unresolved licence(s), %d with OPEN terms" % (unres, openterms))
    return 0


if __name__ == "__main__":
    sys.exit(main())
