#!/usr/bin/env python3
"""Query OSV.dev for known vulnerabilities in the bundled components.

Written 2026-08-24 for the P1 gap "No dependency vulnerability
scanning". The SBOM made it possible: tools/bundle_licenses.py already
maps every Mach-O object in the bundle to an upstream project and
version, so there is finally a list to ask about.

Why this app deserves the check despite being a translation tool: it
parses untrusted input in C++ - OCR images, imported texts, .dic/.ndx
payloads, scanned pecha pages - through freetype, libpng, OpenCV,
libxml2 and friends. That is the classic memory-safety surface, and
MEM-1 (a ~10 KB heap overread whose bytes were appended to OCR output
as recognised Tibetan) is proof it is not hypothetical here.

TWO HONESTY RULES, both learned the hard way this week.

  1. A FAILED QUERY IS NEVER "CLEAN". If the network is down or OSV
     errors, this exits non-zero and says so. A scanner that reports
     "0 vulnerabilities" because it could not ask is the same family
     of defect as a store reporting a successful write it never made.

  2. THE MATCH IS BY NAME, AND SAYS SO. OSV has no Homebrew ecosystem -
     measured 2026-08-24: an ecosystem-scoped query for openssl 3.0.0
     returns 85 results, a name-only query returns 667. Name-only is
     the only shape that answers for upstream C libraries, and it
     OVER-reports: it sweeps in advisories filed against other
     distributions' packaging of the same project. Output is therefore
     labelled CANDIDATES REQUIRING TRIAGE, never "vulnerabilities
     found". Reporting a number as if it were a verdict would be its
     own kind of lie.

Usage:
  tools/vuln_check.py <staged.app>      # or --tsv <BUNDLED_COMPONENTS.tsv>
  tools/vuln_check.py --selftest
"""
import argparse
import io
import json
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OSV = "https://api.osv.dev/v1/querybatch"


def read_components(tsv_path):
    out = []
    for line in io.open(tsv_path, encoding="utf-8"):
        line = line.rstrip("\n")
        if not line or line.startswith("#"):
            continue
        p = line.split("\t")
        if len(p) >= 2 and p[0] and p[1]:
            out.append((p[0], p[1]))
    return out


def query_osv(components, timeout=90, endpoint=OSV):
    """Returns (results, error). error is a string, or None on success.

    A non-None error means the question was not answered. The caller
    must not present that as a clean result.
    """
    queries = [{"package": {"name": n}, "version": v}
               for n, v in components]
    body = json.dumps({"queries": queries})
    try:
        r = subprocess.run(
            ["curl", "-s", "--fail", "--max-time", str(timeout),
             "-X", "POST", endpoint, "-H",
             "Content-Type: application/json",
             "-d", body],
            capture_output=True, text=True)
    except Exception as e:
        return None, "could not run curl: %s" % e
    if r.returncode != 0:
        return None, ("OSV query failed (curl exit %d). %s"
                      % (r.returncode, r.stderr.strip()[:200] or "no stderr"))
    try:
        d = json.loads(r.stdout)
    except Exception as e:
        return None, "OSV returned unparsable JSON: %s" % e
    res = d.get("results")
    if res is None or len(res) != len(components):
        return None, ("OSV returned %s result(s) for %d query(ies) - "
                      "the answer does not line up with the question"
                      % ("no" if res is None else len(res), len(components)))
    return res, None


def report(components, results):
    rows = []
    for (name, version), r in zip(components, results):
        vulns = (r or {}).get("vulns") or []
        if vulns:
            rows.append((len(vulns), name, version,
                         [v.get("id", "?") for v in vulns[:4]]))
    rows.sort(reverse=True)
    total = sum(r[0] for r in rows)
    print("OSV: %d CANDIDATE advisor(ies) across %d of %d component(s)"
          % (total, len(rows), len(components)))
    print("  These are CANDIDATES REQUIRING TRIAGE, not confirmed")
    print("  vulnerabilities. OSV has no Homebrew ecosystem, so the match")
    print("  is by NAME and sweeps in advisories filed against other")
    print("  distributions' packaging of the same upstream project.")
    print()
    for n, name, version, ids in rows:
        print("  %4d  %-22s %-14s %s%s"
              % (n, name[:22], version[:14], ", ".join(ids[:3]),
                 " ..." if n > 3 else ""))
    if not rows:
        print("  no candidates returned for any component")
    return total


def norm_version(v):
    """Numeric parts of a version, for a conservative comparison.

    Distro packaging suffixes (-r1, _1, +deb11u2) are dropped. That is
    deliberately LOSSY, and it is why the classifier below never uses
    this to declare something safe on its own.
    """
    import re
    v = v or ""
    # Strip a distro EPOCH ("2:1.6.40-8.el10") before anything else.
    # Without this the epoch becomes the leading component and swamps
    # the comparison: 2:1.6.40 parsed as (2,1,6,40) reads as NEWER than
    # our (1,6,58), so every RHEL/Alma advisory was flagged and the
    # triage said "needs attention" 40 times out of 40 - conservative
    # in form and useless in substance.
    if ":" in v:
        v = v.split(":", 1)[1]
    # DATE-VERSIONED packages (re2 ships "2025-11-05") must be
    # collapsed BEFORE the revision split, or the dashes eat the month
    # and day: "2025-11-05" -> (2025,) while a distro's "20180101-5.1"
    # keeps all eight digits as (20180101,), so a 2018 fix reads as
    # NEWER than our 2025 build. That flagged twelve re2 advisories
    # that are all long fixed. Third version-parsing bug in this
    # classifier, and like the other two it was found by looking at
    # the biggest number rather than trusting it.
    m = re.match(r"^(\d{4})-(\d{2})-(\d{2})$", v)
    if m:
        v = "".join(m.groups())
    # Cut the packaging revision: upstream version is what we can
    # compare. "1.6.40-8.el10_1.1" -> "1.6.40"
    v = re.split(r"[-_+~]", v)[0]
    parts = re.split(r"[^0-9]+", v)
    return tuple(int(x) for x in parts if x != "")[:4]


def classify(our_version, advisory):
    """NEEDS ATTENTION / probably-fixed / undetermined.

    Conservative by construction: anything this cannot decide is
    NEEDS ATTENTION. A triage tool that guesses "safe" is the same
    failure as a scanner that reports zero because it could not ask.
    """
    ours = norm_version(our_version)
    if not ours:
        return "undetermined", "our version does not parse"
    fixed_seen = []
    for aff in advisory.get("affected", []):
        for rng in aff.get("ranges", []):
            for ev in rng.get("events", []):
                if "fixed" in ev:
                    fixed_seen.append(ev["fixed"])
    if not fixed_seen:
        # No published fix means the ADVISORY carries no version data
        # to decide with - typically a distro that never recorded a fix
        # because the package is unsupported there. That is not
        # evidence about us either way, and calling it "needs
        # attention" buries the real findings: jasper 4.2.9 produced
        # forty such rows, all 2016-2017 Ubuntu CVEs, which drowned the
        # two genuine libpng hits when the buckets were merged.
        # UNDETERMINED is the honest bucket - it says the question was
        # not answered rather than answering it wrongly in either
        # direction.
        return "undetermined", ("no fixed version published in this "
                                "advisory - nothing to compare against "
                                "our %s" % our_version)
    # If EVERY published fix is at or below our version, the upstream
    # defect was probably resolved before the build we ship. "Probably"
    # is the honest word: the suffix we dropped (-r1) is often exactly
    # where a distro's patch lives.
    newer = [f for f in fixed_seen if norm_version(f) > ours]
    same = [f for f in fixed_seen if norm_version(f) == ours]
    if newer:
        return "needs-attention", "fixed in %s, we ship %s" % (
            sorted(newer)[0], our_version)
    if same:
        return "needs-attention", (
            "fixed in %s - same upstream numbers as our %s, so the patch "
            "may live in the packaging suffix we cannot compare"
            % (sorted(same)[0], our_version))
    return "probably-fixed", "all published fixes at or below %s" % our_version


def cmd_triage(name, version, limit=40):
    import urllib.parse
    q = json.dumps({"package": {"name": name}, "version": version})
    r = subprocess.run(["curl", "-s", "--fail", "--max-time", "60", "-X",
                        "POST", "https://api.osv.dev/v1/query", "-H",
                        "Content-Type: application/json", "-d", q],
                       capture_output=True, text=True)
    if r.returncode != 0:
        print("query failed - NOT a clean result, nothing was checked")
        return 2
    vulns = json.loads(r.stdout).get("vulns", [])
    print("%s %s: %d candidate(s); triaging the first %d"
          % (name, version, len(vulns), min(limit, len(vulns))))
    buckets = {"needs-attention": [], "probably-fixed": [],
               "undetermined": []}
    for v in vulns[:limit]:
        verdict, why = classify(version, v)
        buckets[verdict].append((v.get("id", "?"), why))
    for k in ("needs-attention", "undetermined", "probably-fixed"):
        print("  %-16s %d" % (k, len(buckets[k])))
    print()
    for vid, why in buckets["needs-attention"][:12]:
        print("  NEEDS ATTENTION  %-28s %s" % (vid[:28], why[:80]))
    return 0


def osv_query_one(name, version, tries=4):
    """One component, with backoff. OSV rate-limits in practice.

    Returns (vulns, error). Measured 2026-08-24: three consecutive
    triage runs failed under rate-limiting, which is exactly why this
    retries AND why a give-up still returns an error rather than an
    empty list.
    """
    import time
    q = json.dumps({"package": {"name": name}, "version": version})
    last = "not attempted"
    for attempt in range(tries):
        r = subprocess.run(["curl", "-s", "--fail", "--max-time", "60",
                            "-X", "POST", "https://api.osv.dev/v1/query",
                            "-H", "Content-Type: application/json",
                            "-d", q], capture_output=True, text=True)
        if r.returncode == 0:
            try:
                return json.loads(r.stdout).get("vulns", []), None
            except Exception as e:
                last = "unparsable JSON: %s" % e
        else:
            last = "curl exit %d" % r.returncode
        time.sleep(2 ** attempt * 3)
    return None, last


def cmd_triage_all(tsv_path, limit=40):
    comps = read_components(tsv_path)
    print("triaging %d component(s) against OSV" % len(comps))
    print()
    unchecked, rows = [], []
    for name, version in comps:
        vulns, err = osv_query_one(name, version)
        if err is not None:
            unchecked.append((name, version, err))
            continue
        if not vulns:
            continue
        b = {"needs-attention": 0, "undetermined": 0, "probably-fixed": 0}
        ids = []
        for v in vulns[:limit]:
            verdict, _why = classify(version, v)
            b[verdict] += 1
            if verdict == "needs-attention":
                ids.append(v.get("id", "?"))
        rows.append((b["needs-attention"], name, version, b, ids))
    rows.sort(reverse=True)

    print("%-20s %-14s %6s %6s %6s" % ("component", "version", "ATTN",
                                       "undet", "fixed"))
    for attn, name, version, b, ids in rows:
        print("%-20s %-14s %6d %6d %6d  %s"
              % (name[:20], version[:14], attn, b["undetermined"],
                 b["probably-fixed"], ", ".join(ids[:2])))
    total_attn = sum(r[0] for r in rows)
    print()
    print("%d component(s) with something needing attention; %d advisory"
          " row(s) in that bucket" % (sum(1 for r in rows if r[0]), total_attn))
    if unchecked:
        # RULE 1 again: a component we could not ask about is NOT clean.
        print()
        print("NOT CHECKED - these were never answered, and must not be "
              "read as clean:")
        for name, version, err in unchecked:
            print("  %-20s %-14s %s" % (name[:20], version[:14], err))
        return 2
    return 0


def cmd_selftest():
    """Pin the two honesty rules. No network."""
    bad = []
    import tempfile
    d = tempfile.mkdtemp(prefix="vuln_selftest_")
    tsv = os.path.join(d, "c.tsv")
    io.open(tsv, "w", encoding="utf-8").write(
        "# comment\nopenssl\t3.0.0\tApache-2.0\tLICENSE\t3\n"
        "brotli\t1.2.0\tMIT\tLICENSE\t2\n")
    comps = read_components(tsv)
    if comps != [("openssl", "3.0.0"), ("brotli", "1.2.0")]:
        bad.append("component parsing")

    # rule 1: a failed query must never be reported as clean.
    # Forced by an unresolvable endpoint - NOT by timeout=0, which
    # curl reads as "no timeout at all" and which therefore SUCCEEDED
    # when this pin was first written. The pin caught that; it is the
    # reason the endpoint is injectable.
    res, err = query_osv(comps, timeout=5,
                         endpoint="https://osv.invalid.example/v1/querybatch")
    if err is None:
        bad.append("a query to an unreachable endpoint must report an "
                   "ERROR, not an empty clean result")
    if res is not None:
        bad.append("a failed query must return no results at all, so it "
                   "cannot be mistaken for 'nothing found'")

    # rule 2: a mismatched answer is an error, not a partial result
    ok = report(comps, [{}, {}])
    if ok != 0:
        bad.append("empty OSV results must count as zero candidates")

    import shutil
    shutil.rmtree(d, ignore_errors=True)
    for b in bad:
        print("  [FAIL]", b)
    print("vuln_check selftest: %d failure(s)" % len(bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("app", nargs="?")
    ap.add_argument("--tsv", default=None)
    ap.add_argument("--selftest", action="store_true")
    ap.add_argument("--triage", nargs=2,
                    metavar=("NAME", "VERSION"))
    ap.add_argument("--triage-all", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return cmd_selftest()
    if a.triage:
        return cmd_triage(a.triage[0], a.triage[1])
    if a.triage_all:
        t = a.tsv or (os.path.join(a.app, "Contents", "Resources",
                      "licenses", "BUNDLED_COMPONENTS.tsv") if a.app else None)
        if not t or not os.path.exists(t):
            print("need a staged .app or --tsv"); return 1
        return cmd_triage_all(t)

    tsv = a.tsv
    if not tsv and a.app:
        tsv = os.path.join(a.app, "Contents", "Resources", "licenses",
                           "BUNDLED_COMPONENTS.tsv")
    if not tsv or not os.path.exists(tsv):
        print("need a staged .app or --tsv <BUNDLED_COMPONENTS.tsv>; "
              "the press writes it at step 4e")
        return 1

    comps = read_components(tsv)
    print("asking OSV about %d bundled component(s)..." % len(comps))
    results, err = query_osv(comps)
    if err:
        # RULE 1. Loud, and non-zero.
        print("VULNERABILITY SCAN DID NOT RUN: %s" % err)
        print("This is NOT a clean result. Nothing was checked.")
        return 2
    report(comps, results)
    return 0


if __name__ == "__main__":
    sys.exit(main())
