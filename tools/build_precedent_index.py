#!/usr/bin/env python3
"""build_precedent_index.py — the one question every analyst asks, answered once.

THE DEFECT THIS WAS WRITTEN FOR. An analyst aligning a segment constantly
needs to know how a Tibetan string has been rendered before: has `kyi` ever
carried English here, or is it always left unrendered; did Geshe Michael write
"quietude" or "stillness" for `zhi gnas`; is `sogs` ever "involvement" (it is
not). Until now the only way to ask was to grep or re-parse
data/alignment/alignment_full_v1.json — 10.9 MB, 50,370 link records — ONE
QUESTION AT A TIME. The campaign audit counted 1,866 such calls, 15% of every
tool call the agents made, and each returns raw records the agent then counts
by hand. A hand count is a number nobody can re-derive, which is this
project's characteristic defect, and several reconciler reports in the git log
open by re-deriving totals that were already true.

So: fold the bank once, from the bank alone, into a form small enough to paste
the relevant PART of into a prompt. For any Tibetan string the index says what
English it has been given, how often, at what depths, how often it was left
with NO English exponent at all — the null count, which the reconciliation
rules turn on — and where every one of those links sits, so no count in here
is a number anybody has to take on trust.

EVERY COUNT IS A LIST. There is no stored count anywhere in a record: a
rendering carries the citations of the links that made it, and the count is
their number. That is not tidiness. A count and a list can disagree, and when
they do it is the count that gets quoted; keeping only the list means the
cheapest thing to print is also the thing that can be checked.

TWO COUNTS, AND ONLY ONE OF THEM IS EVIDENCE. Some segments are TRANSFERRED:
their spine rows are byte-identical to another segment's, so the reading was
copied across and nobody read it. build_alignment_layer.py stamps every such
span, and a citation for one is marked with a trailing x. It is a real link
and it is counted as one, but it attests nothing on its own, so the extract
prints x11/8r — eleven links, eight of them independent readings — and ranks
renderings by the readings. Without that, a rendering echoed across forty
copies would head the list over one that was read twelve times, and the head
of the list is the first thing anybody reads.

WHAT THIS IS NOT. docs/geshe_michael_roach_dictionary.html is built from the
same bank and looks similar. It is a 2 MB browsable page for a reader, and it
TRIMS glued headings off the English before showing it. This index trims
nothing: every string is the bank's own bytes.

PHONETICS, AND A RULE THIS TOOL DOES NOT USE. Some segments — the chanted
prayers at the head of the courses — pair the Tibetan with Geshe Michael's
PRONUNCIATION rather than with a translation, and a transcription offered as a
rendering would be a falsehood even though it is verbatim. The one existing
rule for spotting those links is alignment_invariants.is_pronunciation_link,
which calls a link phonetic when its id looks like `s359y43`. Measured against
this bank that rule matches 2,262 links across 428 segments in five courses,
1,701 of them in C03 alone — where the page author used `y` ids for ordinary
depth-7 members. C03:359 is a prose translation ("He must occupy himself more
with the Dharma...") and its y-links include `dgos` -> "must", `med` -> "less"
and `byed pa` -> "occupy". Those are not transcriptions.

So this tool classifies nothing. Every rendering is shown as what the bank
holds, with the segment it came from, and the reader decides. A classifier
invented here would be a guess (rule 3) dressed as a fact, and the mechanical
guard that actually matters is elsewhere and unaffected: gen_alignment_page.py
refuses any span it cannot find verbatim in the segment being aligned, so a
transcription cannot travel into a prose segment however it is labelled. (The
same measurement is a real hole in alignment_invariants, which skips every one
of those 2,262 links in its English-containment checks — and 1,981 of them
carry English. That is that gate's to fix; it is reported, not patched here.)

  usage:
    build_precedent_index.py                     rebuild the committed index
    build_precedent_index.py --out F             write somewhere else (the gate)
    build_precedent_index.py --extract C05:361   the extract for one segment
    build_precedent_index.py --extract C05:361 --json
    build_precedent_index.py --measure C05       extract sizes over a course
"""
import argparse
import collections
import hashlib
import io
import json
import os
import re
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
# Occurrence is defined by the GENERATOR's rule, not by a second rule that
# happens to agree with it today. find_word() is what decides whether a span
# can be wrapped at all, so a precedent offered for a string the generator
# would not match in this segment is a precedent for nothing.
from gen_alignment_page import find_word            # noqa: E402

BANK = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUT = os.path.join(ROOT, "data", "alignment", "precedent_index_v1.json")

# A run of letters/digits/Tibetan — what find_word treats as a word for
# boundary purposes. Used here only to pick a lookup key and to walk a
# segment's words; the match itself is always find_word's.
WORDRUN = re.compile(r"[A-Za-z0-9ༀ-࿿]+")

# One link as a citation: where it sits, at what depth, whether it was
# flagged a Wilson case particle, and whether it arrived by TRANSFER rather
# than by somebody reading the segment. The depth is \d+ rather than \d
# because a citation the expression cannot read used to come back None and
# crash on .group(); depths run 1-7 in this bank, so that was theoretical,
# but the house rule asks for a refusal that names the token, not a crash.
CITE = re.compile(r"^(?P<course>[^:]+):(?P<seg>\d+|\?)"
                  r"/(?P<d>\d+)(?P<case>c?)(?P<xfer>x?)$")

SCHEMA = (
    "index[tib] is one banked Tibetan string. eng is its renderings, "
    "strongest first — strongest meaning MOST INDEPENDENTLY READ, not most "
    "cited: [english, [citations]]. nul is the citations of links "
    "where that Tibetan carried NO English exponent. acip is its ACIP (the "
    "source of record; unambiguous across the whole bank, 0 conflicts). "
    "A CITATION is COURSE:seg/depth, with a trailing c when the link is "
    "flagged a case particle and a trailing x when the span was TRANSFERRED "
    "rather than read: C03:160/5, C01:23/6c, C07:85/5x. A transferred span "
    "sits in a segment byte-identical to the one it was copied from and is "
    "not a second attestation of anything; meta.xfer_links counts the marks. "
    "COURSE:? is a link the "
    "bank records with no segment number. There are no stored counts: a "
    "count is the length of a citation list, so nothing inside a record can "
    "disagree with anything else in it. bank_headwords minus headwords are "
    "Tibetan strings appearing only in links with no English and nothing "
    "else to record — they have no record rather than an empty one."
)

CAUTION = (
    "evidence, not permission. Geshe Michael's English is MATCHED from the "
    "segment being aligned, never copied in from another: "
    "gen_alignment_page.py refuses any span it cannot find verbatim in that "
    "segment's own spine text. Some segments pair Tibetan with Geshe "
    "Michael's pronunciation instead of a translation; nothing here "
    "classifies those, and the citation on every rendering is what lets you "
    "tell. A citation marked x is a TRANSFERRED span, copied from a "
    "byte-identical segment and never read by anybody, so it attests "
    "nothing on its own; a count printed x11/8r is 11 links of which 8 are "
    "independent readings, and the order of the renderings follows the "
    "readings, not the links."
)


def parse_cite(c):
    """One citation token read back, or a refusal that names it.

    depths_of used to do CITE.match(c).group("d") with a single-digit depth
    in the expression, so a citation this tool could not read came back None
    and died as AttributeError on the next attribute — the crash rule 3 asks
    us not to take. Refusing by name costs one branch.
    """
    m = CITE.match(c)
    if m is None:
        sys.exit("REFUSED: %r is not a citation. A citation is "
                 "COURSE:seg/depth — seg a number or ? — with an optional "
                 "trailing c for a case particle and x for a transferred "
                 "span." % (c,))
    return m


def cite(link):
    """One link as a citation token.

    The bank calls the row "seg"; the spine column is "seq". Same number, two
    spellings, and the first draft of this tool read the bank with the spine's
    name and raised KeyError on all 50,370 links.

    Three links carry seg: null — pages that registered a span without saying
    which corpus row it came from, one of them a real rendering (`ldog` ->
    "stop"). They are cited COURSE:? rather than dropped or given a plausible
    number, and counted in meta as uncited_links so the quantity is visible
    rather than silent. It should go to nothing when those pages are repaired.

    PROVENANCE IS SACRED, AND IT TRAVELS HERE TOO. build_alignment_layer.py
    stamps link["xfer"] on every span of a TRANSFERRED segment — one whose
    spine rows are byte-identical to another's, so its reading was copied
    rather than read — "so nothing downstream can read it as an independent
    reading". This is downstream. Without the mark a transferred rendering
    cites exactly like a read one, and since the extract shows the
    most-attested renderings first, an inflated count is the first thing an
    analyst sees. The trailing x uses the convention the c flag already
    established; meta.xfer_links counts the marks and the gate recounts them
    from the bank, so the mark cannot go missing quietly.
    """
    return "%s:%s/%d%s%s" % (link["course"],
                             "?" if link["seg"] is None else link["seg"],
                             link["d"],
                             "c" if link.get("case") else "",
                             "x" if link.get("xfer") else "")


def build(links):
    """Fold the bank's links into one record per Tibetan string.

    Everything here is counting, and nothing here judges. Strings are not
    trimmed, corrected, normalised or classified — what goes in comes out,
    because the whole value of the index is that a reconciler can act on it
    without re-deriving it from the bank.
    """
    eng = collections.defaultdict(lambda: collections.defaultdict(list))
    nul = collections.defaultdict(list)
    acip = {}
    # course/seg order decides which citation comes first, so sort once here
    # rather than trusting the bank's file order.
    for l in sorted(links, key=lambda x: (x["course"], x["seg"] is None,
                                          x["seg"] or 0, str(x["id"]))):
        t = l["tib"]
        if l.get("tib_acip") and t not in acip:
            acip[t] = l["tib_acip"]
        if l["eng"] is None:
            nul[t].append(cite(l))
        else:
            eng[t][l["eng"]].append(cite(l))

    out = {}
    for t in sorted(set(eng) | set(nul)):
        o = {}
        if t in acip:
            o["acip"] = acip[t]
        if t in eng:
            # STRONGEST FIRST, AND STRENGTH IS READINGS. A transferred link
            # is a copy of a reading, not a second one, so ranking on the
            # raw link count would put a rendering that was echoed forty
            # times above one that was read twelve. Ties break on the link
            # total and then on the English, so in a bank with no transfers
            # this is byte-for-byte the ordering it was.
            o["eng"] = [[e, c] for e, c in
                        sorted(eng[t].items(),
                               key=lambda kv: (-n_read(kv[1]), -len(kv[1]),
                                               kv[0]))]
        if t in nul:
            o["nul"] = nul[t]
        out[t] = o
    return out


def record_links(rec):
    """Every citation in one record — the record's link count, derived."""
    return ([c for _e, cs in rec.get("eng", ()) for c in cs]
            + list(rec.get("nul", ())))


def n_read(cites):
    """How many of these citations are INDEPENDENT READINGS.

    A transferred citation is a real link and is counted as one, but the
    segment it sits in is byte-identical to the segment it was copied from
    and nobody read it. Two numbers, both true, and only one of them is
    evidence of how often Geshe Michael was seen to render the string that
    way — so both get printed, never just the larger.
    """
    return sum(1 for c in cites if not parse_cite(c).group("xfer"))


def n_xfer(cites):
    return len(cites) - n_read(cites)


def depths_of(cites):
    # by number, not by string: "10" sorts before "2" lexically, and the
    # whole point of widening the depth was that a two-digit depth should
    # behave rather than surprise.
    return sorted({parse_cite(c).group("d") for c in cites}, key=int)


def bank_meta(path, links, index):
    """Meta carries no build date, deliberately.

    A timestamp would make the committed index differ from a fresh generation
    every day, which would either break the in-sync gate or force the gate to
    ignore the field — and a gate that ignores a field is how drift hides. The
    bank's own digest is the thing worth recording: it changes exactly when
    the index has to be rebuilt.
    """
    h = hashlib.sha256(io.open(path, "rb").read()).hexdigest()
    courses = collections.Counter(l["course"] for l in links)
    return {
        "what": ("for any Tibetan string in the alignment bank: what English "
                 "it has been given, how often, at what depth, and how often "
                 "it was left with no English exponent"),
        "built_from": os.path.relpath(path, ROOT),
        "bank_sha256": h,
        "bank_links": len(links),
        "bank_headwords": len({l["tib"] for l in links}),
        "headwords": len(index),
        "indexed_links": sum(len(record_links(v)) for v in index.values()),
        "renderings": sum(len(v.get("eng", ())) for v in index.values()),
        "null_links": sum(len(v.get("nul", ())) for v in index.values()),
        # links the bank cannot place on a corpus row. Surfaced, not dropped:
        # the rendering is real and its provenance is short.
        "uncited_links": sum(1 for l in links if l["seg"] is None),
        # links that arrived by TRANSFER and carry an x on their citation.
        # Here so that the mark has a total the gate can recount from the
        # bank: a fold that quietly stopped marking transfers would
        # otherwise regenerate byte-identically and pass everything.
        "xfer_links": sum(1 for l in links if l.get("xfer")),
        # headwords no segment lookup can ever match — see locatable()
        "unlocatable_headwords": sum(1 for t in index if not locatable(t)),
        # discovered from the bank, never declared: a course that starts
        # landing pages appears here by itself.
        "courses": {c: courses[c] for c in sorted(courses)},
        "caution": CAUTION,
        "schema": SCHEMA,
    }


def write_index(path, links, bank=BANK):
    index = build(links)
    doc = {"meta": bank_meta(bank, links, index), "index": index}
    with io.open(path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(doc, f, ensure_ascii=False, sort_keys=True,
                  separators=(",", ":"))
        f.write("\n")
    return doc


# ---------------------------------------------------------------- the extract

def locatable(t):
    """Can this headword be found in a segment at all?

    The bank holds one link whose Tibetan is the EMPTY STRING — C03:126,
    id s126tail, a d=5 span with no text and no English. The empty string is
    a substring of every segment, so the first run of the extract offered it
    as precedent for all of them, on every line, cited to C03:126. Anything
    with no letter, digit or Tibetan character in it is in the same position:
    the generator's word rule cannot place it, so it is not a precedent for
    anything. It stays IN the index, because the index records what the bank
    holds; it just cannot be matched to a segment. meta.unlocatable_headwords
    counts them, so the day that empty span is repaired the number says so.
    """
    return bool(WORDRUN.search(t))


def buckets_of(index):
    """tib strings grouped by their first word, so a segment can be matched
    against twelve thousand headwords without testing twelve thousand."""
    b = collections.defaultdict(list)
    for t in index:
        if not locatable(t):
            continue
        b[WORDRUN.search(t).group(0)].append(t)
    for k in b:
        b[k].sort()
    return b


def segment_headwords(index, buckets, wylie):
    """Which banked strings occur in this segment's wylie, in reading order.

    A candidate clears three things: its first word appears in the text, the
    whole string appears as a substring, and find_word — the generator's own
    rule — finds it on a word boundary. The third is not decoration: `par
    shing` is rendered "block", and "block" also sits inside "blockprint"
    twenty characters earlier; the same class of interior hit once attached
    the ACIP of `tshogs` to the span `shog`.
    """
    hits = {}
    seen = set()
    for m in WORDRUN.finditer(wylie):
        w = m.group(0)
        if w in seen:
            continue
        seen.add(w)
        for t in buckets.get(w, ()):
            if t in hits or t not in wylie:
                continue
            # the position find_word gives, not the position a plain substring
            # search gives: they differ exactly when the first occurrence is
            # inside a longer word, and taking the wrong one puts the entry in
            # the wrong place in a list whose whole claim is reading order.
            i = find_word(wylie, t, 0)
            if i >= 0:
                hits[t] = i
    # reading order, a compound ahead of its own parts at the same offset
    return sorted(hits, key=lambda t: (hits[t], -len(t), t))


def segment_text(spine, course, seq):
    db = sqlite3.connect(spine)
    try:
        return db.execute("select wylie, english from corpus_segments "
                          "where course=? and seq=?", (course, seq)).fetchone()
    finally:
        db.close()


def extract(index, buckets, spine, course, seq):
    """The precedent for ONE segment's vocabulary — what a harness inlines.

    THE SEGMENT'S OWN LINKS ARE REMOVED. A segment that has already been
    aligned is in the bank, and leaving its own links in would hand an analyst
    the answer sheet and call it corroboration: the first run of this tool
    offered C05:361 its own whole-sentence rendering of itself, cited to
    C05:361. How many were removed is reported, because a segment that is
    already banked is worth knowing about for its own sake.

    Refuses rather than approximates (rule 3): a segment the spine does not
    have, or one with no wylie, returns an error record and no entries. An
    empty extract is a fact about the bank and is reported as one.
    """
    row = segment_text(spine, course, seq)
    if row is None:
        return {"course": course, "seq": seq,
                "error": "no such segment in the spine"}
    wylie = row[0] or ""
    if not wylie.strip():
        return {"course": course, "seq": seq,
                "error": "segment has no wylie in the spine"}
    here = "%s:%d/" % (course, seq)
    ents, own = [], 0
    for t in segment_headwords(index, buckets, wylie):
        rec = index[t]
        o = {"tib": t}
        if "acip" in rec:
            o["acip"] = rec["acip"]
        eng = []
        for e, cs in rec.get("eng", ()):
            keep = [c for c in cs if not c.startswith(here)]
            own += len(cs) - len(keep)
            if keep:
                eng.append([e, keep])
        nul = [c for c in rec.get("nul", ()) if not c.startswith(here)]
        own += len(rec.get("nul", ())) - len(nul)
        if eng:
            o["eng"] = eng
        if nul:
            o["nul"] = nul
        if eng or nul:
            ents.append(o)
    every = [c for e in ents for c in record_links(e)]
    return {"course": course, "seq": seq, "wylie_chars": len(wylie),
            "headwords": len(ents),
            "links": len(every),
            # beside the link count, never instead of it: the two differ by
            # exactly the transferred copies, and the difference is the
            # thing a reader has to know before weighing a count.
            "read_links": n_read(every),
            "xfer_links": n_xfer(every),
            "own_links_removed": own, "entries": ents}


def median(xs):
    """The MIDDLE of a sorted list, not the upper middle.

    report_sizes used to print xs[n // 2] under the name "median". On an
    even-length list that is the HIGHER of the two middle values: [2, 4] has
    a median of 3 and xs[n // 2] returns 4. Every course in this repository
    has an even or odd count as it happens to have, so the size lines were
    off by a few bytes on about half of them.

    No figure is quoted here on purpose. The extract text has changed since
    the error was found, so any number written down would already be wrong;
    `build_precedent_index.py --measure` re-derives the whole table in a
    second, which is why report_sizes prints and stores nothing.
    """
    n = len(xs)
    return xs[n // 2] if n % 2 else (xs[n // 2 - 1] + xs[n // 2]) // 2


def report_sizes(label, sizes):
    """One line of extract-size measurement. Printed, never stored: a figure
    kept in a document is a figure that goes stale without saying so, and this
    one is cheap enough to re-derive whenever it is quoted."""
    n = len(sizes)
    return ("%-16s %5d segments   extract bytes: min %d  median %d  mean %d  "
            "p90 %d  max %d"
            % (label, n, sizes[0], median(sizes), sum(sizes) // n,
               sizes[min(n - 1, int(n * 0.9))], sizes[-1]))


MAX_RENDERINGS = 6
NAMEW = 26

HEADER = ("PRECEDENT — %s:%d · %d of this segment's Tibetan strings are "
          "aligned elsewhere in the bank (%d links, %d of them transferred "
          "copies rather than independent readings).\n")


def count_bit(n, read):
    """x27, or x27/19r when some of the 27 were copied rather than read.

    The suffix appears only when the two numbers differ, and the difference
    is the only case in which the raw count misleads. A bank with no
    transfers reads exactly as it did.
    """
    return "x%d%s" % (n, "/%dr" % read if read != n else "")


def render(ex):
    """The extract as the plain text a prompt carries."""
    if "error" in ex:
        return "PRECEDENT — %s:%d · UNAVAILABLE: %s\n" % (
            ex["course"], ex["seq"], ex["error"])
    out = [HEADER % (ex["course"], ex["seq"], ex["headwords"], ex["links"],
                     ex["xfer_links"]),
           CAUTION[0].upper() + CAUTION[1:] + "\n"]
    if ex["own_links_removed"]:
        out.append("This segment is ALREADY aligned: %d of its own links are "
                   "left out below, because a segment is not its own "
                   "precedent.\n" % ex["own_links_removed"])
    if not ex["entries"]:
        out.append("  (nothing in this segment has been aligned before)\n")
        return "".join(out)
    for e in ex["entries"]:
        cites = record_links(e)
        bits = []
        if e.get("nul"):
            # THE NULL COUNT, SPLIT BY DEPTH. "unrendered x274" merges links
            # of structurally different kinds -- a depth-5 word left without
            # an exponent and a depth-6 member inside one are not the same
            # observation -- and the reconciliation rules turn on the figure.
            # `de`'s 274 nulls are 82 at d5, 191 at d6 and 1 at d7, and the
            # entry-level depth list a few columns left cannot say so
            # because it covers the renderings too. The JSON already carries
            # the depth on every citation; this is just refusing to add up
            # things that do not add up.
            nd = collections.Counter(parse_cite(c).group("d")
                                     for c in e["nul"])
            bits.append((len(e["nul"]), n_read(e["nul"]), "unrendered",
                         e["nul"][0],
                         " · " + " ".join("d%s %d" % (d, nd[d])
                                          for d in sorted(nd, key=int))))
        for eng, cs in e.get("eng", ()):
            bits.append((len(cs), n_read(cs), '"%s"' % eng, cs[0], ""))
        # ORDERED BY READINGS. Sorting on the link count alone is what let a
        # transferred echo take the head of the list, and the head of the
        # list is the first thing an analyst reads.
        bits.sort(key=lambda b: (-b[1], -b[0], b[2]))
        shown, tail = bits[:MAX_RENDERINGS], ""
        if len(bits) > len(shown):
            # never drop in silence: say how many went and how much weight
            hid = bits[len(shown):]
            tail = " · +%d more (%s links)" % (
                len(hid), count_bit(sum(b[0] for b in hid),
                                    sum(b[1] for b in hid)))
        body = "%-5s %-8s %s%s" % (
            count_bit(len(cites), n_read(cites)),
            ",".join("d" + d for d in depths_of(cites)),
            " · ".join("%s %s (%s%s)"
                       % (b[2], count_bit(b[0], b[1]), b[3].split("/")[0],
                          b[4])
                       for b in shown), tail)
        if len(e["tib"]) > NAMEW:
            # a whole-sentence headword would push every line off the page;
            # give it its own line rather than truncating Geshe Michael's
            # English or the Tibetan beside it
            out.append("  %s\n  %-*s %s\n" % (e["tib"], NAMEW, "", body))
        else:
            out.append("  %-*s %s\n" % (NAMEW, e["tib"], body))
        # read the flag, do not sniff the last character: a case particle on
        # a transferred span cites C07:85/5cx and ends in x, so endswith("c")
        # would have gone quietly to zero for exactly those links the moment
        # the first transfer landed.
        ncase = sum(1 for c in cites if parse_cite(c).group("case"))
        if ncase:
            out.append("  %-*s   flagged a case particle on %d of those\n"
                       % (NAMEW, "", ncase))
    return "".join(out)


def load_index(path=None):
    return json.load(io.open(path or OUT, encoding="utf-8"))["index"]


def precedent_for(course, seq, index=None, spine=None, as_text=True):
    """The public entry point: the precedent extract for one segment."""
    index = load_index() if index is None else index
    ex = extract(index, buckets_of(index), spine or SPINE, course, seq)
    return render(ex) if as_text else ex


# ------------------------------------------------------------------------ cli

def parse_ref(s):
    c, _, q = s.partition(":")
    if not c or not q.isdigit():
        sys.exit("REFUSED: --extract wants COURSE:SEQ, got %r" % s)
    return c, int(q)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", default=OUT)
    ap.add_argument("--bank", default=BANK)
    ap.add_argument("--spine", default=SPINE)
    ap.add_argument("--extract", nargs="+", metavar="COURSE:SEQ")
    ap.add_argument("--json", action="store_true",
                    help="with --extract, emit the record instead of the text")
    ap.add_argument("--measure", nargs="*", metavar="COURSE",
                    help="extract sizes across whole courses, so any figure "
                         "quoted for them can be re-derived. Name none and it "
                         "measures every course the spine has, which is how a "
                         "course added later gets measured without anyone "
                         "remembering to add it here")
    a = ap.parse_args()

    if a.extract or a.measure is not None:
        index = load_index(a.out)
        bk = buckets_of(index)
        if a.extract:
            # An unavailable extract still PRINTS -- the reader has to see
            # which segment and why -- but it also exits nonzero, because a
            # harness that inlines this would otherwise paste an UNAVAILABLE
            # line into a prompt and carry on as though it had precedent.
            bad = 0
            for ref in a.extract:
                c, q = parse_ref(ref)
                ex = extract(index, bk, a.spine, c, q)
                bad += "error" in ex
                if a.json:
                    print(json.dumps(ex, ensure_ascii=False, sort_keys=True))
                else:
                    sys.stdout.write(render(ex))
            return 1 if bad else 0
        db = sqlite3.connect(a.spine)
        courses = a.measure or [r[0] for r in db.execute(
            "select distinct course from corpus_segments order by course")]
        every = []
        for course in courses:
            seqs = [r[0] for r in db.execute(
                "select seq from corpus_segments where course=? order by seq",
                (course,))]
            if not seqs:
                sys.exit("REFUSED: no course %r in %s" % (course, a.spine))
            sizes = sorted(len(render(extract(index, bk, a.spine, course, q))
                               .encode("utf-8")) for q in seqs)
            every.extend(sizes)
            print(report_sizes(course, sizes))
        db.close()
        if len(courses) > 1:
            every.sort()
            print(report_sizes("ALL %d courses" % len(courses), every))
        return 0

    links = json.load(io.open(a.bank, encoding="utf-8"))["links"]
    doc = write_index(a.out, links, a.bank)
    m = doc["meta"]
    sys.stderr.write(
        "%s\n  %d links -> %d of the bank's %d headwords, %d renderings, "
        "%d null links, %d uncited, %d bytes\n"
        % (os.path.relpath(a.out, ROOT), m["bank_links"], m["headwords"],
           m["bank_headwords"], m["renderings"], m["null_links"],
           m["uncited_links"], os.path.getsize(a.out)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
