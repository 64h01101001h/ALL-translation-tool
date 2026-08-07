#!/usr/bin/env python3
"""pyewts_oracle.py — independent diff-oracle for our wylie→Unicode engine.

pyewts (OpenPecha; the Python port of buda-base's ewts-converter, the
ecosystem-standard EWTS implementation) is run over every dictionary
entry that carries BOTH wylie and source-attested Tibetan. For each
pair we compare three ways:

  ours   = the spine's stored tibetan (validated 98.88% variant-aware
           against the master's ground truth — engines_battery)
  oracle = pyewts.toUnicode(wylie)

Classes reported:
  AGREE        — oracle output equals the stored tibetan exactly
  ORACLE_DIFF  — oracle differs from stored tibetan (candidate EWTS
                 edge case OR source variant; sampled for review)

This is a VALIDATION HARNESS only (survey quick-win #2): pyewts never
runs in the app; our canonical engines stay authoritative. Divergences
are classified evidence, not verdicts — the stored tibetan itself is
source-attested and can legitimately differ from strict EWTS.

Usage: python3 tools/pyewts_oracle.py [spine.db]
Writes: data/extracted/pyewts_oracle_report.json
"""
import collections
import json
import os
import re
import sqlite3
import sys

import pyewts

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    HERE, "build", "hgm_spine_v27_2.db")
OUT = os.path.join(HERE, "data", "extracted",
                   "pyewts_oracle_report.json")


def main():
    conv = pyewts.pyewts()
    db = sqlite3.connect(DB)
    rows = db.execute(
        "SELECT wylie, tibetan FROM entries "
        "WHERE wylie != '' AND tibetan != ''").fetchall()
    agree = 0
    diffs = []
    warn_kinds = collections.Counter()
    iast = 0
    fmt_conv = 0
    defects = []
    flagged = 0
    for wylie, tib in rows:
        # variant-aware, like our own battery: the stored tibetan may
        # carry expansions after shads — any segment counting as the
        # headword's form is agreement
        segs = [x.rstrip("་").strip() for x in re.split("[།༎]", tib)
                if x.strip()]
        warns = []
        got = conv.toUnicode(wylie, warns).rstrip("་།").strip()
        for w in warns:
            warn_kinds[re.sub(r"\".*?\"|'.*?'", "…", w)] += 1
        norm = lambda t: re.sub(r"[\s་]+", "་", t).strip("་")
        if got in segs or norm(got) in [norm(x) for x in segs]:
            agree += 1
        elif re.search(r"[āīūṛṝḷḹṃḥṅñṭḍṇśṣ]", wylie):
            # IAST-flavored wylie: our engine converts it, pyewts
            # passes diacritics through raw — a capability gap on
            # their side, counted separately
            iast += 1
        elif "\u27e8" in tib:
            # the master marks syllables the canonical engine REFUSED
            # to convert (⟨wylie⟩ flags — rule 3); pyewts converts or
            # passes them through blindly (even literal hyphens inside
            # Tibetan output). Philosophy difference, counted as its
            # own class — our flags are the honest behavior.
            flagged += 1
        elif "..." in wylie or "\u2026" in wylie or "/" in wylie:
            # ellipsis / slash rendering conventions differ (we emit
            # shad-pause, pyewts emits run-of-tshegs) — formatting,
            # not letters
            fmt_conv += 1
        elif "," in wylie:
            # comma renders as shad in the master (ACIP heritage),
            # literally in pyewts — convention, not letters
            fmt_conv += 1
        else:
            n_ours = [norm(x) for x in segs]
            n_py = norm(got)
            if any(x and (n_py.startswith(x) or x.startswith(n_py))
                   for x in n_ours):
                # letter-true prefix relation: the stored tibetan is
                # truncated or padded vs its own wylie — an
                # independent confirmation of a master source defect
                defects.append(
                    {"wylie": wylie, "ours": tib, "pyewts": got})
            else:
                diffs.append({"wylie": wylie, "ours": tib,
                              "pyewts": got})
    report = {
        "pairs": len(rows),
        "agree": agree,
        "agree_pct": round(100.0 * agree / max(1, len(rows)), 3),
        "iast_flavored_wylie": iast,
        "format_conventions": fmt_conv,
        "flagged_input_entries": flagged,
        "source_defect_candidates": len(defects),
        "letter_level_divergence": len(diffs),
        "top_warning_kinds":
            dict(warn_kinds.most_common(10)),
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
