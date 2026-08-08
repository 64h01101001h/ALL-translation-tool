#!/usr/bin/env python3
"""thl_wylie_oracle.py — second independent diff-oracle for wylie→Unicode.

THL's wylie.js (Roger Espel Llima's EWTS converter — the JS port of THL
Wylie.java, the REFERENCE implementation the whole EWTS ecosystem
descends from, including pyewts via buda-base's ewts-converter) is run
over every dictionary entry that carries BOTH wylie and source-attested
Tibetan, exactly like tools/pyewts_oracle.py. Same classes, same
philosophy: VALIDATION HARNESS only — this code never runs in the app;
our canonical engines stay authoritative, and divergences are
classified evidence, not verdicts.

Lineage note: pyewts and wylie.js share an ancestor (Espel's
algorithm), so this is an independent CODEBASE, not an independent
algorithm family — agreement between the two oracles and our engine is
still three implementations arriving at the same letters.

Usage: python3 tools/thl_wylie_oracle.py [spine.db] [wylie.js path]
Writes: data/extracted/thl_wylie_oracle_report.json
"""
import collections
import json
import os
import re
import sqlite3
import subprocess
import sys
import unicodedata

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    HERE, "build", "hgm_spine_v27_2.db")
WYLIE_JS = sys.argv[2] if len(sys.argv) > 2 else os.path.expanduser(
    "~/ACIP Software Developmeent/tibetan/wylie.js")
OUT = os.path.join(HERE, "data", "extracted",
                   "thl_wylie_oracle_report.json")


def main():
    db = sqlite3.connect(DB)
    rows = db.execute(
        "SELECT wylie, tibetan FROM entries "
        "WHERE wylie != '' AND tibetan != ''").fetchall()

    proc = subprocess.Popen(
        ["node", os.path.join(HERE, "tools", "thl_wylie_runner.js"),
         WYLIE_JS],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
    payload = "".join(json.dumps(w) + "\n" for w, _ in rows)
    out, _ = proc.communicate(payload)
    results = [json.loads(x) for x in out.splitlines()]
    assert len(results) == len(rows), (len(results), len(rows))

    agree = 0
    diffs = []
    warn_kinds = collections.Counter()
    iast = 0
    fmt_conv = 0
    defects = []
    flagged = 0
    errors = 0
    for (wylie, tib), res in zip(rows, results):
        if res is None or "err" in res:
            errors += 1
            continue
        got = res["u"].rstrip("་།").strip()
        for w in res["w"]:
            warn_kinds[re.sub(r"\".*?\"|'.*?'", "…", w)] += 1
        segs = [x.rstrip("་").strip() for x in re.split("[།༎]", tib)
                if x.strip()]
        # NFC first: the master stores precomposed DHA/DDHA/etc.
        # (U+0F52 …), THL emits the decomposed base+0FB7 sequences —
        # canonically EQUIVALENT, not a letter difference
        norm = lambda t: unicodedata.normalize(
            "NFC", re.sub(r"[\s་]+", "་", t).strip("་"))
        if got in segs or norm(got) in [norm(x) for x in segs]:
            agree += 1
        elif re.search(r"[āīūṛṝḷḹṃḥṅñṭḍṇśṣ]", wylie):
            iast += 1               # IAST-flavored wylie: capability gap
        elif "⟨" in tib:
            flagged += 1            # our honest ⟨wylie⟩ refusal flags
        elif ("..." in wylie or "…" in wylie or "/" in wylie
              or "," in wylie):
            fmt_conv += 1           # rendering conventions, not letters
        else:
            n_ours = [norm(x) for x in segs]
            n_thl = norm(got)
            if any(x and (n_thl.startswith(x) or x.startswith(n_thl))
                   for x in n_ours):
                defects.append(
                    {"wylie": wylie, "ours": tib, "thl": got})
            else:
                diffs.append({"wylie": wylie, "ours": tib, "thl": got})
    report = {
        "oracle": "THL wylie.js (Espel) — independent codebase, "
                  "shared EWTS ancestry with pyewts",
        "pairs": len(rows),
        "agree": agree,
        "agree_pct": round(100.0 * agree / max(1, len(rows)), 3),
        "iast_flavored_wylie": iast,
        "format_conventions": fmt_conv,
        "flagged_input_entries": flagged,
        "runner_errors": errors,
        "source_defect_candidates": len(defects),
        "letter_level_divergence": len(diffs),
        "top_warning_kinds": dict(warn_kinds.most_common(10)),
        "diff_samples": diffs[:60],
    }
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    json.dump(defects, open(OUT.replace("report", "defect_candidates"),
                            "w"), ensure_ascii=False, indent=1)
    json.dump(report, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(json.dumps({k: v for k, v in report.items()
                      if k != "diff_samples"}, ensure_ascii=False,
                     indent=1))
    print("full report ->", OUT)


if __name__ == "__main__":
    main()
