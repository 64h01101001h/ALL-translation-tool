#!/usr/bin/env python3
"""Regenerate docs/ERRATA_REGISTER.md from the JSON sidecar.

The register has two faces: a prose document the Director and in-house
editors read, and docs/errata_register.json that tooling files into. Batches
add to the JSON via merge_errata.py, and the prose does NOT follow by itself
-- it drifted from 52 findings to a real 78, and from 2 HIGH to 5 HIGH,
within a few hours of the first version being published. A register whose
headline count is stale is worse than no register, because a reader trusts
the number.

This regenerates the prose from the sidecar, so the two cannot disagree.
The narrative preamble is preserved from a template; only the counts and the
tables are derived.

    python3 tools/build_errata_register.py
"""
import json, io, os, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SIDE = os.path.join(ROOT, "docs", "errata_register.json")
OUT = os.path.join(ROOT, "docs", "ERRATA_REGISTER.md")

SEV = ["HIGH", "MEDIUM", "LOW"]
KIND_LABEL = {
    "TIBETAN_SPELLING": "Tibetan spelling",
    "ENGLISH_TYPO": "English typo",
    "ENGLISH_FACTUAL_ERROR": "English factual error",
    "INGEST_ARTEFACT": "Digitisation artefact",
    "FORMATTING": "Formatting",
}


def esc(s):
    return (s or "").replace("|", "\\|").replace("\n", " ").strip()


def table(rows):
    out = ["| Citation | What it says | Should say | Evidence | Confidence |",
           "|---|---|---|---|---|"]
    for e in rows:
        out.append("| `%s` | %s | %s | %s | %s |" % (
            esc(e.get("segment")), esc(e.get("found"))[:150],
            esc(e.get("expected"))[:110] or "&mdash;",
            esc(e.get("evidence"))[:260], e.get("confidence", "")))
    return "\n".join(out)


def main():
    reg = json.load(io.open(SIDE, encoding="utf-8"))
    real = [e for e in reg
            if e.get("kind") not in ("NOT_AN_ERRATUM", "UNVERIFIABLE")]
    ingest = [e for e in real if e.get("kind") == "INGEST_ARTEFACT"]
    docerr = [e for e in real if e.get("kind") != "INGEST_ARTEFACT"]
    dismissed = [e for e in reg if e.get("kind") == "NOT_AN_ERRATUM"]
    unver = [e for e in reg if e.get("kind") == "UNVERIFIABLE"]
    sev = collections.Counter(e.get("severity") for e in docerr)
    kinds = collections.Counter(KIND_LABEL.get(e.get("kind"), e.get("kind"))
                                for e in real)

    L = []
    A = L.append
    A("# Errata Register &mdash; Asian Legacy Library Core Documents")
    A("**Working draft for the Director and in-house editors**  ")
    A("*Regenerated from `docs/errata_register.json` by "
      "`tools/build_errata_register.py`. Do not hand-edit the tables &mdash; "
      "they are derived, and an edit here is lost on the next batch.*")
    A("")
    A("---")
    A("")
    A("## 1. What this is")
    A("")
    A("Defects found in the ALL core documents &mdash; the Tibetan "
      "(ACIP/Wylie) source and Geshe Michael Roach's facing English &mdash; "
      "recorded as a by-product of a machine alignment campaign reading the "
      "courses segment by segment. **Every entry has been checked against the "
      "corpus database** (42,199 segments, spine `hgm_spine_v27_2.db`): the "
      "quoted strings were retrieved from the spine, not from a note, and "
      "each citation was re-derived rather than trusted.")
    A("")
    A("**%d entries**: **%d document errata** (%d HIGH, %d MEDIUM, %d LOW), "
      "**%d digitisation artefacts**, %d observations checked and closed as "
      "*not* errata, and %d that could not be verified."
      % (len(reg), len(docerr), sev["HIGH"], sev["MEDIUM"], sev["LOW"],
         len(ingest), len(dismissed), len(unver)))
    A("")
    A("| Kind | n |")
    A("|---|---|")
    for k, n in kinds.most_common():
        A("| %s | %d |" % (k, n))
    A("")
    A("---")
    A("")
    A("## 2. What we are NOT claiming")
    A("")
    A("- **Nothing has been changed.** No document, ACIP file, English text or "
      "database row has been edited on the strength of this register.")
    A("- **Geshe Michael's English is preserved byte-for-byte** by standing "
      "project rule. Every \"should say\" below is a proposal for a human "
      "editor, not something the project has adopted.")
    A("- **Confidence means what it says.** CONFIRMED was reproduced in the "
      "spine with named witnesses. PROBABLE and UNCERTAIN need a human "
      "decision and must not be actioned automatically.")
    A("- **This is not a systematic proofread.** It records anomalies the "
      "campaign happened to notice while aligning. **An unflagged segment is "
      "not a clean segment; it is an unexamined one.**")
    A("- **Coverage is partial.** C01 and C02 in full, C03 in progress, "
      "C04&ndash;C18 unread at this depth &mdash; though duplicate passages "
      "inside them are named where a fix reaches them.")
    A("")
    A("---")
    A("")
    for s in SEV:
        rows = [e for e in docerr if e.get("severity") == s]
        if not rows:
            continue
        A("## %s severity &mdash; %d %s"
          % (s.title(), len(rows), "entry" if len(rows) == 1 else "entries"))
        A("")
        if s == "HIGH":
            A("*Would mislead a reader who has no other cue in front of them.*")
            A("")
        A(table(sorted(rows, key=lambda e: str(e.get("segment")))))
        A("")
    A("---")
    A("")
    A("## Digitisation artefacts &mdash; %d" % len(ingest))
    A("")
    A("*These are fixed **upstream in the data pipeline**, not by editing a "
      "document, and go to a different person. Two are class entries carrying "
      "a measured extent rather than one row per instance.*")
    A("")
    A(table(sorted(ingest, key=lambda e: str(e.get("segment")))))
    A("")
    if dismissed:
        A("---")
        A("")
        A("## Checked and closed as NOT errata &mdash; %d" % len(dismissed))
        A("")
        A("*Recorded so a later pass does not re-raise them.*")
        A("")
        A("| Citation | Observation | Why it stands |")
        A("|---|---|---|")
        for e in sorted(dismissed, key=lambda e: str(e.get("segment"))):
            A("| `%s` | %s | %s |" % (esc(e.get("segment")),
                                      esc(e.get("found"))[:110],
                                      esc(e.get("evidence"))[:220]))
        A("")
    if unver:
        A("---")
        A("")
        A("## Could not be verified &mdash; %d" % len(unver))
        A("")
        for e in unver:
            A("- `%s` &mdash; %s" % (esc(e.get("segment")),
                                     esc(e.get("evidence"))[:300]))
        A("")
    A("---")
    A("")
    A("## Recommended actions, by owner")
    A("")
    A("**Document editors.** The %d document errata above. Start with the %d "
      "HIGH entries. Note that several errors exist at MORE THAN ONE LOCUS "
      "because passages are ingested twice &mdash; each entry's evidence "
      "names them, and a per-segment fix will leave duplicates behind."
      % (len(docerr), sev["HIGH"]))
    A("")
    A("**Data-pipeline maintainers.** The %d digitisation artefacts, plus two "
      "reports filed separately: `docs/upstream/FOLIO_MARKER_COLLISIONS.md` "
      "(119 folio markers spliced into caption numbers) and "
      "`docs/upstream/C13_COLUMN_OFFSET.md` (**C13:63&ndash;82 pairs Tibetan "
      "with the English of a different text &mdash; C13 should not be scanned "
      "until it is fixed**)." % len(ingest))
    A("")
    A("**Director.** The standing policy question: does ALL issue an erratum "
      "sheet, correct future printings, or annotate in place? The scan "
      "preserves everything verbatim either way &mdash; this is an editorial "
      "decision, not a technical one.")
    A("")
    io.open(OUT, "w", encoding="utf-8").write("\n".join(L))
    sys.stderr.write("wrote %s\n  %d entries, %d document errata "
                     "(%d HIGH), %d artefacts\n"
                     % (OUT, len(reg), len(docerr), sev["HIGH"], len(ingest)))


if __name__ == "__main__":
    main()
