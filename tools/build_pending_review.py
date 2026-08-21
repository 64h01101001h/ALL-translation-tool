#!/usr/bin/env python3
"""build_pending_review.py — format the pending queue for GMR review.

The two-tier apparatus rule (STD-008 refinement): candidates enter the
official MAIN documents only through publication with Geshe Michael's
approval. This tool closes the workflow loop: it formats everything in
the pending queue into one review sheet Adam can bring to a review
session. Approval itself still happens in the publishing process — this
sheet just makes the queue visible.

Reads:  data/candidate_notes.json   (Draft pane "Propose footnote")
        data/candidate_bib.json     (composer "Save as candidate")
Writes: data/PENDING_REVIEW.md      (+ .docx via pandoc if available)
"""
import json
import os
import subprocess

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(HERE, "data", "candidate_notes.json")
OUT = os.path.join(HERE, "data", "PENDING_REVIEW.md")


def main():
    try:
        cands = json.load(open(SRC))
    except FileNotFoundError:
        cands = []
    try:
        bibs = json.load(open(SRC.replace("candidate_notes",
                                          "candidate_bib")))
    except FileNotFoundError:
        bibs = []
    with open(OUT, "w") as f:
        f.write("# Pending apparatus candidates — for review\n\n")
        f.write("> These translator-proposed footnotes are NOT part of "
                "the official apparatus. They enter the MAIN documents "
                "only once published and 100% approved by Geshe Michael "
                "(STD-008). This sheet exists so the queue can be "
                "brought to a review session.\n\n")
        if not cands and not bibs:
            f.write("*The pending queue is empty.*\n")
        for i, c in enumerate(cands, 1):
            f.write(f"## {i}. {c.get('lemma', '(no lemma)')}\n\n"
                    f"{c.get('text', '')}\n\n"
                    f"*proposed {c.get('proposed', '?')} · status: "
                    f"{c.get('status', 'pending')}*\n\n"
                    "Decision: ☐ approve for a future volume · "
                    "☐ revise · ☐ decline\n\n")
        if bibs:
            f.write("# Pending bibliography entries\n\n")
        for i, b in enumerate(bibs, 1):
            f.write(f"## B-candidate {i}"
                    f"{' · ACIP ' + b['acip_no'] if b.get('acip_no') else ''}\n\n"
                    f"{b.get('entry', '')}\n\n"
                    f"*proposed {b.get('proposed', '?')}*\n\n"
                    "Decision: ☐ approve · ☐ revise · ☐ decline\n\n")
    print(f"wrote {OUT} ({len(cands)} note + {len(bibs)} bibliography "
          "candidates)")
    try:
        subprocess.run(["pandoc", OUT, "-o",
                        OUT.replace(".md", ".docx")], check=True)
        print("wrote", OUT.replace(".md", ".docx"))
    except Exception:
        print("(pandoc unavailable — .md only)")


if __name__ == "__main__":
    main()
