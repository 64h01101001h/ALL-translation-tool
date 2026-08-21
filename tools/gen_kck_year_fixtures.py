#!/usr/bin/env python3
"""gen_kck_year_fixtures.py — TSV fixtures from the original's printed
year files (third_party/kck_henning/kc_YYYY.txt) for the month-view
battery: every 'Lunar Month' header and every day's western date.

Usage: python3 tools/gen_kck_year_fixtures.py
"""
import os
import re

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
D = os.path.join(HERE, "third_party", "kck_henning")
OUT = os.path.join(D, "kck_year_fixtures.tsv")

MON = {m: i + 1 for i, m in enumerate(
    "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec".split())}

rows = []
for fn in sorted(os.listdir(D)):
    if not re.match(r"kc_\d{4}\.txt$", fn):
        continue
    year = int(fn[3:7])
    mix = -1
    for line in open(os.path.join(D, fn), encoding="utf-8",
                     errors="replace"):
        m = re.match(
            r"Lunar Month: (\d+)( \(Intercalary\))? - "
            r"(\w+)-(male|female)-(\w+)", line)
        if m:
            mix += 1
            rows.append(["MONTH", year, mix, m.group(1),
                         1 if m.group(2) else 0, m.group(3),
                         m.group(4), m.group(5)])
            continue
        d = re.match(r"(\d+): \w+\. .*?; (\d+) (\w+) (\d+)\s*$", line)
        if d and mix >= 0:
            rows.append(["DAY", year, mix, d.group(1), d.group(2),
                         MON[d.group(3)], d.group(4)])
with open(OUT, "w") as f:
    f.write("# MONTH year idx display intercal elem gender animal | "
            "DAY year monthidx tshes d m y\n")
    for r in rows:
        f.write("\t".join(str(x) for x in r) + "\n")
months = sum(1 for r in rows if r[0] == "MONTH")
days = sum(1 for r in rows if r[0] == "DAY")
print(f"{months} months, {days} days -> {OUT}")
