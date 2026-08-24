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
    a = ap.parse_args()
    if a.selftest:
        return cmd_selftest()

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
