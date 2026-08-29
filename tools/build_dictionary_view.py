#!/usr/bin/env python3
"""Rebuild the browsable dictionary view from the current alignment layer.

Adam asked to see the new dictionary as it grows batch to batch, with a
slider for the depth layers. This regenerates docs/michael_dictionary.html
from whatever the layer currently holds, so republishing after a batch
shows the batch.

    python3 tools/build_dictionary_view.py
    # then: Artifact(file_path="docs/michael_dictionary.html", url=<same URL>)

Publishing to the SAME url keeps one link that updates in place. Passing a
new path, or omitting the url from a different conversation, creates a
second artifact instead.

The page carries the PROVISIONAL banner from the layer's own meta block,
so the tier label can never drift from what the layer actually claims.
"""
import json, io, os, re, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "engines"))
import pron_engine as _P   # noqa: E402


def _n(s):
    return re.sub(r"[^a-z]", "", (s or "").lower())


_PHC = {}


def is_phonetic(tib, eng):
    """True when the English is a TRANSCRIPTION of the Tibetan rather than a
    translation of it.

    The liturgy pages align Geshe Michael's phonetic transcription lines
    against the Tibetan they sound out -- `byugs shing` against "jukshing".
    That is real, valuable data and it is exactly what the phonetics
    protocol intends, but it is NOT dictionary content: shown as an entry
    it reads as though `bsod nams` means "sunam", when it means merit.

    Adam caught this in the published view, 2026-08-28. The evidence layer
    the app reads was never affected -- it harvests d=5 only, where the
    0.59% that match this test are legitimate proper names and titles
    (`dge bshes` -> "Geshe", `aa ti sha` -> "Atisha"). The defect was in
    the view, which harvests every depth, and d=7 is 46% phonetic.

    Tested with the project's own pron_engine rather than a heuristic.
    """
    if tib not in _PHC:
        try:
            _PHC[tib] = _n(_P.pronounce(tib))
        except Exception:
            _PHC[tib] = ""
    ph, ne = _PHC[tib], _n(eng)
    if not ph or not ne:
        return False
    return ph == ne or (len(ne) > 3 and (ph.startswith(ne) or ne.startswith(ph)))


FULL = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
EVID = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")
SHELL = os.path.join(ROOT, "tools", "dictionary_view_shell.html")
OUT = os.path.join(ROOT, "docs", "michael_dictionary.html")


def main():
    links = json.load(io.open(FULL, encoding="utf-8"))["links"]
    ev = json.load(io.open(EVID, encoding="utf-8"))
    meta, acip = ev["meta"], ev.get("acip_index", {})

    byd = collections.defaultdict(
        lambda: collections.defaultdict(lambda: collections.defaultdict(list)))
    phon = collections.defaultdict(lambda: collections.defaultdict(list))
    skipped = n_phon = 0
    for l in links:
        d, t = l.get("d"), (l.get("tib") or "").strip()
        e, seg, co = (l.get("eng") or "").strip(), l.get("seg"), l.get("course")
        # a link with no English exponent is a null morpheme: correct on the
        # page, meaningless in a dictionary view.
        if not d or not t or not e or seg is None or not co:
            skipped += 1
            continue
        # A proper name is not a phonetic pair even though it matches the
        # test: `tsong kha pa` -> "Tsongkapa" IS the English. Names are
        # capitalised in GMR's text and phonetic lines are not, which
        # separates them without a hand-kept list.
        if is_phonetic(t, e) and not e[:1].isupper():
            phon[t][e].append("%s:%s" % (co, seg))
            n_phon += 1
            continue
        byd[d][t][e].append("%s:%s" % (co, seg))

    depths = {}
    for d in sorted(byd):
        ents = []
        for t, rend in byd[d].items():
            rl = sorted(([e, sorted(set(rs))] for e, rs in rend.items()),
                        key=lambda x: -len(x[1]))
            ents.append([t, acip.get(t) or "", rl])
        ents.sort(key=lambda x: (-len(x[2]), x[0].lower()))
        depths[d] = ents
        sys.stderr.write("  d=%d  %5d headwords  %6d renderings\n"
                         % (d, len(ents), sum(len(x[2]) for x in ents)))

    pents = []
    for t, rend in phon.items():
        rl = sorted(([e, sorted(set(rs))] for e, rs in rend.items()),
                    key=lambda x: -len(x[1]))
        pents.append([t, acip.get(t) or "", rl])
    pents.sort(key=lambda x: (-len(x[2]), x[0].lower()))
    sys.stderr.write("  phonetics %5d headwords  %6d transcriptions "
                     "(held apart, not dictionary content)\n"
                     % (len(pents), sum(len(x[2]) for x in pents)))

    payload = {"meta": {"date": meta.get("date"), "tier": meta.get("tier"),
                        "corpus": meta.get("source_corpus"),
                        "rule": meta.get("rule")},
               "depths": depths, "phonetics": pents}
    shell = io.open(SHELL, encoding="utf-8").read()
    data = json.dumps(payload, ensure_ascii=False, separators=(",", ":"))
    html = shell.replace("/*PAYLOAD*/", "const PAYLOAD = " + data + ";", 1)
    io.open(OUT, "w", encoding="utf-8").write(html)
    sys.stderr.write("\n  skipped %d links with no English exponent\n" % skipped)
    sys.stderr.write("  wrote %s (%.2f MB)\n" % (OUT, len(html) / 1048576.0))


if __name__ == "__main__":
    main()
