#!/usr/bin/env python3
"""gen_kck_fixtures.py — oracle fixtures from Henning's original KCK.

Runs the unmodified KCK binary (third_party/kck_henning/build_macos.sh)
over a sweep of Tibetan dates in the Modern Karana epoch (2009) and
captures, per date: zla-dag (true month), gza-dag (true weekday), 
nyi-dag (true sun), lunar-day daybreak value, spyi zhag, Julian day,
and the western date. These become the diff battery for the future
allcore day-level port — every intermediate provable, per rule 2.

Output: data/kck_fixtures.json
Usage:  third_party/kck_henning/build_macos.sh && python3 tools/gen_kck_fixtures.py
"""
import json
import os
import re
import subprocess

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
KCK = "/tmp/kck_oracle_build/kck"
OUT = os.path.join(HERE, "third_party", "kck_henning", "kck_fixtures.json")


def run_date(tshes, month, year, steps=0):
    # epoch 3 (Modern Karana), menu 3 (cycle), date, then `t` presses
    stdin = f"3\n3\n{tshes} {month} {year}\n" + "t" * steps + "e9\n"
    p = subprocess.run([KCK], input=stdin, text=True,
                       capture_output=True, timeout=60)
    return p.stdout


BLOCK = re.compile(
    r"The ZLA-DAG is: (?P<zla>[^,]+), Month = (?P<month>\d+), "
    r"Year = (?P<year>\d+).*?"
    r"DATE is: (?P<tshes>\d+) / \d+ / \d+.*?"
    r"The gza-dag is: (?P<gza>[^\n]+).*?"
    r"The nyi-dag is: (?P<nyi>[^-]+)-.*?"
    r"Daybreak: (?P<daybreak>[^.]+)\..*?"
    r"SPYI ZHAG = (?P<spyi>\d+)\. Julian day = (?P<jd>\d+)\. "
    r"Western date = (?P<west>[^\n]+)",
    re.S)


def main():
    fixtures = []
    sweeps = [
        (1, 1, 2010, 40),    # forty consecutive lunar days from 1/1/2010
        (1, 1, 2012, 10),
        (15, 4, 2015, 5),    # saga zla-ba full moon area
        (1, 1, 2020, 10),
        (30, 12, 2024, 5),
        (3, 8, 2011, 0),     # the NOTES example
    ]
    for tshes, month, year, steps in sweeps:
        out = run_date(tshes, month, year, steps)
        for m in BLOCK.finditer(out):
            fixtures.append({k: m.group(k).strip() for k in
                             ("zla", "month", "year", "tshes", "gza",
                              "nyi", "daybreak", "spyi", "jd", "west")})
    # dedupe on (year, month, tshes, jd)
    seen = set()
    uniq = []
    for f in fixtures:
        k = (f["year"], f["month"], f["tshes"], f["jd"])
        if k in seen:
            continue
        seen.add(k)
        uniq.append(f)
    json.dump(uniq, open(OUT, "w"), indent=1)
    print(f"{len(uniq)} fixtures -> {OUT}")


if __name__ == "__main__":
    main()
