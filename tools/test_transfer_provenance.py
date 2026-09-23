#!/usr/bin/env python3
"""A transferred reading must be verbatim, and must say it was transferred.

WHAT THIS IS FOR. tools/transfer_duplicate_segments.py carries a landed reading
across to a segment whose Tibetan and Geshe Michael's English are byte-identical
to it, instead of paying an analyst pass to re-derive the same answer. Two
things can go wrong with that, and only two:

  1. the carried span is not actually the target's own text -- a transfer that
     is not verbatim is a fabrication wearing a citation;
  2. the carried span looks, in the bank, exactly like a span an analyst read.
     Then the evidence layer counts one reading as several, and a reader has no
     way to tell an attestation from an echo of one.

This gate holds both, in four parts.

PART A -- DECLARED TRANSFERS. Every segment stamped as transferred is checked
end to end: the stamp parses, the segment is registered on the page it sits in
(so coverage still counts it), the two spine rows are byte-identical in BOTH
columns, every span is verbatim in the TARGET row, the span set is the source's
span set unchanged, the page says in plain English that no analyst read it, and
every span of that segment in the bank carries its `xfer` provenance naming the
source. Conversely, no span may claim transfer provenance that no page stamps.

PART B -- UNDECLARED TRANSFERS, which is where the real finding is. 91 landed
segments already carry an aligned block that is identical, once the labels and
the whole heading are rewritten, to another landed segment's -- over spine rows
that are byte-identical in both columns. Eighty-three of them are the
liturgical opening (segments 1-21) that starts C02, C03, C04 and C05 as it
starts C01 -- all twenty-one in C03, C04 and C05, and twenty of C02's
twenty-one, because C02:6 was read onto different markup and so is a separate
reading of an identical row. C04:1-21 and C05:1-21 are documented as hand
copies of C03 in a comment in build_alignment_layer.py; the rest are documented
nowhere. The other eight are the Lama supplication repeated four times inside
C03 (97, 99, 101, 102, 103, 105, 106, 107 against 87, 89 and 98).

Whether a given one was copied or two readings landed on the same markup is not
something this gate can tell, and it does not need to: either way the bank holds
one reading several times over and says nothing.

HOW FAR THEY ACTUALLY REACH IS MEASURED AND PRINTED, NOT ASSERTED. The first
version of this text said the evidence layer "counts it up to five times as
though several segments attested it". Measured, that is not true of this set:
all of its links sit at depths 1, 2 and 7, and the evidence layer harvests
depth 5, so not one evidence rendering cites one of them. They do reach the
published dictionary, which harvests every depth. Both numbers come out of the
gate on every run -- bank links and their depths, evidence renderings,
published rows, and how many head renderings would move if the echoes were
discounted -- so nobody has to trust a sentence written on the day it was true.

The set is frozen at exactly what was measured on 2026-09-22 and listed by
name, because a count would hide which segments it meant. A new one fails. So
does the set SHRINKING without the baseline being re-frozen: slack in a ratchet
is how a gate stops measuring.

PART D -- THE PUBLISHED DICTIONARY, which is the artifact a person actually
reads. Parts A and B stop at the bank. docs/geshe_michael_roach_dictionary.html
is built by a second code path (tools/build_dictionary_view.py) that groups the
bank's links itself, and until 2026-09-22 it never read `xfer`: a carried
reading appeared there as an ordinary citation beside the one it was carried
from, and -- because renderings are ordered by how many segments cite them --
an echo could outrank a reading. One transferred segment was measured moving
the head rendering of 13 headwords. So the mark has to survive into the view,
and this part is what stops it being lost again.

It is an INDEPENDENT RECOUNT, deliberately, and not a regeneration check. A
regeneration check compares the generator with itself and can never catch a
broken generator -- the precedent-index gate was falsified exactly that way the
same day. This part recomputes FROM THE BANK which segments arrived by
transfer, and requires the page to say so on every citation of one of them, to
say it of nothing else, to order its renderings by the independent count, and
to disclose the total it actually carries.

PART C -- --selftest, and why it is not optional. Part A examines zero segments
today, because nothing has been transferred yet, and a check that examines
nothing passes for free. So the selftest builds a real transferred page in a
scratch directory from the first duplicate pair that proves, then breaks it five
ways -- corrupts a transferred span, strips the provenance off the bank, removes
the page's stamp, alters a span's grammar label without touching a character of
its text, and names a source whose text differs -- and requires this gate's own
checker to fail and name the fault each time. It is the part that
goes red if the checking is hollowed out. It then does the same for Part D: it
puts a real transfer into a scratch copy of the bank, runs the REAL dictionary
builder over it, and requires the published page to come out marked -- and the
recount to fail the moment the marks are rubbed off it.

Usage:
    test_transfer_provenance.py [--selftest] [--verbose]
"""
import io
import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import alignment_page_dirs as D            # noqa: E402  (discovery, not a list)
import build_alignment_layer as B          # noqa: E402  (COURSES, spans, spine)
import transfer_duplicate_segments as T    # noqa: E402  (the page reader)
# the citation convention and the payload parser are IMPORTED from the tools
# that own them, never restated here: a second hand-written copy of a
# convention is how the checker and the thing it checks quietly diverge.
import build_dictionary_view as V          # noqa: E402  (XFER_MARK, is_xfer)
import test_view_matches_layer as VM       # noqa: E402  (payload, rows)

FULL = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
EVID = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")
VIEW = os.path.join(ROOT, "docs", "geshe_michael_roach_dictionary.html")
BASELINE = os.path.join(ROOT, "tools", "transfer_provenance_baseline.txt")


# ------------------------------------------------------------------ the world

def read_pages():
    """(course, page id, path, doc) for every alignment page that exists.

    Discovered from the directories, not from a list. Six gates here analysed
    none of Course 5 for weeks because each carried its own written-down set
    (tools/alignment_page_dirs.py); the same mistake here would make a whole
    course of transfers unexaminable while the gate reported a clean run.
    """
    by_path = {}
    for course, cfg in sorted(B.COURSES.items()):
        for pg in cfg["pages"]:
            by_path[os.path.abspath(os.path.join(cfg["dir"], pg + ".html"))] = \
                (course, pg)
    out = []
    for p in D.page_files():
        course, pg = by_path.get(os.path.abspath(p), (None, None))
        out.append((course, pg, p, io.open(p, encoding="utf-8").read()))
    return out


def registered_index():
    out = {}
    for course, cfg in sorted(B.COURSES.items()):
        for pg, seqs in cfg["pages"].items():
            for q in seqs:
                out[(course, q)] = pg
    return out


def blocks_of(doc):
    body = T.page_body(doc)
    return T.segment_blocks(body) if body else {}


def span_diff(tgt, src):
    """What actually differs between a transferred span list and its source's.

    The first version of this message reported the two LENGTHS, which are equal
    in the case that matters: altering a span's grammar label, or its text,
    leaves the count alone, and the gate would print "31 span(s) here against
    31 at the source" and leave the reader to find it. Rule 3 asks a refusal to
    name what it refused.
    """
    ti = [s["id"] for s in tgt]
    si = [s["id"] for s in src]
    only_t = [i for i in ti if i not in set(si)]
    only_s = [i for i in si if i not in set(ti)]
    if only_t or only_s:
        parts = []
        if only_t:
            parts.append("%d span(s) here that the source does not have (%s)"
                         % (len(only_t), ", ".join(only_t[:6])))
        if only_s:
            parts.append("%d span(s) at the source that are missing here (%s)"
                         % (len(only_s), ", ".join(only_s[:6])))
        return "; ".join(parts)
    if ti != si:
        for a, b in zip(ti, si):
            if a != b:
                return ("the same spans in a different order: %s where the "
                        "source has %s" % (a, b))
    tm = {s["id"]: s for s in tgt}
    sm = {s["id"]: s for s in src}
    for i in ti:
        a, b = tm[i], sm[i]
        for k in sorted(set(a) | set(b)):
            if a.get(k) != b.get(k):
                return ("span %s differs in %s: %r here, %r at the source"
                        % (i, k, a.get(k), b.get(k)))
    return ("the span lists compare unequal but no field differs; the "
            "comparison itself is wrong and must be looked at")


# ------------------------------------------------- PART A: declared transfers

def check_declared(pages, registered, rows, links, evidence, problems):
    """Every stamped segment, end to end. Returns the number examined.

    The two caches are not an optimisation detail worth hiding: at 399
    transferred segments the uncached form re-scans all 578 pages and re-parses
    the source page once PER STAMP, and a gate slow enough to be skipped is a
    gate that is not run.
    """
    stamped = {}
    by_page = {(c, p): d for c, p, _path, d in pages}
    parsed = {}

    def blocks_cached(key, doc_):
        if key not in parsed:
            parsed[key] = blocks_of(doc_)
        return parsed[key]

    for course, pg, path, doc in pages:
        for m in T.MARK_RE.finditer(doc):
            tc, tq = m.group(1), int(m.group(2))
            sc, sq = m.group(3), int(m.group(4))
            spage, nspans = m.group(5), int(m.group(6))
            where = "%s:%d (%s)" % (tc, tq, os.path.relpath(path, ROOT))
            stamped[(tc, tq)] = (sc, sq)

            if course is None or tc != course:
                problems.append(
                    "%s is stamped as transferred but the page it sits in is "
                    "not registered for course %s, so the coverage gate can "
                    "never count it" % (where, tc))
                continue
            if registered.get((tc, tq)) != pg:
                problems.append(
                    "%s is stamped as transferred but segment %d is not "
                    "registered on page %s; a page that is not registered is "
                    "not landed, and the hole stays open" % (where, tq, pg))
                continue
            if (tc, tq) not in rows:
                problems.append("%s names a target the spine does not have"
                                % where)
                continue
            if (sc, sq) not in rows:
                problems.append("%s names source %s:%d, which the spine does "
                                "not have" % (where, sc, sq))
                continue
            if rows[(tc, tq)] != rows[(sc, sq)]:
                which = []
                if rows[(tc, tq)][0] != rows[(sc, sq)][0]:
                    which.append("the Tibetan")
                if rows[(tc, tq)][1] != rows[(sc, sq)][1]:
                    which.append("Geshe Michael's English")
                problems.append(
                    "%s claims to be a transfer of %s:%d, but %s differs "
                    "between the two rows. A transfer is only honest between "
                    "byte-identical rows" % (where, sc, sq, " and ".join(which)))
                continue

            block = blocks_cached((course, pg), doc).get(tq)
            if block is None:
                problems.append("%s is stamped but the page carries no "
                                "aligned block for it" % where)
                continue

            wyl, eng = rows[(tc, tq)]
            tp, ts = T.parse_side(block[2])
            # the no-exponent markers are the machine's own annotation, not
            # Geshe Michael's English; they are stripped before the verbatim
            # question is asked, exactly as build_alignment_layer.py does.
            ep, es = T.parse_side(
                re.sub(r'<span class="nul">.*?</span>', "", block[3], flags=re.S))
            if tp is None or ep is None:
                problems.append("%s: the span markup does not balance" % where)
                continue
            bad = 0
            for _d, lab, _c, lo, hi in ts:
                if tp[lo:hi] and tp[lo:hi] not in wyl:
                    problems.append(
                        "%s: the transferred Tibetan span %s (%r) is NOT "
                        "PRESENT in the target's own wylie. A transferred span "
                        "must be the target's own text, not the source's"
                        % (where, lab, tp[lo:hi]))
                    bad += 1
            for _d, lab, _c, lo, hi in es:
                frag = ep[lo:hi]
                if frag and frag not in eng:
                    problems.append(
                        "%s: the transferred English span %s (%r) is NOT "
                        "PRESENT in Geshe Michael's English for the target "
                        "segment" % (where, lab, frag))
                    bad += 1
            if bad:
                continue

            tgt_spec, why = T.reconstruct(block)
            if why:
                problems.append("%s: the transferred block cannot be read "
                                "back: %s" % (where, why))
                continue
            src_pg = registered.get((sc, sq))
            src_doc = by_page.get((sc, src_pg))
            if src_doc is None:
                problems.append("%s: the source page %s/%s is not landed any "
                                "more, so the transfer cannot be checked"
                                % (where, sc, src_pg))
                continue
            src_block = blocks_cached((sc, src_pg), src_doc).get(sq)
            if src_block is None:
                problems.append("%s: the source page no longer carries segment "
                                "%d" % (where, sq))
                continue
            src_spec, why = T.reconstruct(src_block)
            if why:
                problems.append("%s: the source block cannot be read back: %s"
                                % (where, why))
                continue
            if tgt_spec["spans"] != src_spec["spans"]:
                problems.append(
                    "%s: the transferred span set is not the source's. A "
                    "transfer may not add, drop or alter a single span: %s"
                    % (where, span_diff(tgt_spec["spans"],
                                        src_spec["spans"])))
                continue
            if nspans != len(tgt_spec["spans"]):
                problems.append("%s: the stamp declares %d span(s) and the "
                                "page carries %d" % (where, nspans,
                                                     len(tgt_spec["spans"])))
            note = block[4] or ""
            if "TRANSFERRED" not in note or "%s:%d" % (sc, sq) not in note:
                problems.append(
                    "%s: the page does not TELL A READER it was transferred. "
                    "The AI NOTE must open by saying no analyst read this "
                    "segment and naming %s:%d" % (where, sc, sq))
            if "/" not in spage:
                problems.append("%s: the stamp's source_page is not of the "
                                "form COURSE/page" % where)

    # the bank must carry the same story, per span
    seen = {}
    for L in links:
        key = (L["course"], L["seg"])
        if key in stamped:
            want = "%s:%d" % stamped[key]
            seen[key] = seen.get(key, 0) + 1
            if L.get("xfer") != want:
                problems.append(
                    "the bank's span %s/%s %s carries xfer=%r but its page "
                    "stamps it as transferred from %s. A transferred span that "
                    "does not say so in the bank is indistinguishable from one "
                    "an analyst read"
                    % (L["course"], L["page"], L["id"], L.get("xfer"), want))
        elif L.get("xfer"):
            problems.append(
                "the bank's span %s/%s %s claims xfer=%r, but no page stamps "
                "%s:%s as transferred. Provenance that nothing can be checked "
                "against is not provenance"
                % (L["course"], L["page"], L["id"], L["xfer"],
                   L["course"], L["seg"]))
    for key in stamped:
        if key not in seen:
            problems.append(
                "%s:%d is stamped as transferred but contributes no span to "
                "the bank; either the page is empty or the builder is not "
                "reading it" % key)

    # the evidence layer must name transferred citations
    for tn, evs in (evidence or {}).get("pairs", {}).items():
        for ev in evs:
            xr = [r for r in ev["refs"]
                  if tuple([r.split(":")[0], int(r.split(":")[1])]) in
                  {(c, q) for c, q in stamped}]
            if xr and sorted(ev.get("xrefs") or []) != sorted(xr):
                problems.append(
                    "the evidence pair %r -> %r cites transferred segment(s) "
                    "%s but does not list them in xrefs, so its count reads as "
                    "independent attestation" % (tn, ev["eng"], ", ".join(xr)))
    return len(stamped)


# ----------------------------------------------- PART B: undeclared transfers

HEADING = re.compile(r'<h2>Segment \d+ .*?</h2>', re.S)


def label_normalised(block, seq):
    """A segment's aligned columns with its own labels and heading blanked.

    Two segments whose normalised blocks are equal carry the SAME reading. That
    is what a transfer is, whether or not anyone declared one.

    THE WHOLE HEADING GOES, not just the "Segment N" part of it. Until
    2026-09-22 this blanked '<h2>Segment N ' and left the analyst-written title
    inside the comparison key, so an identical reading under a different
    caption was invisible here: C03:87 and C03:97 are 837 bytes each and equal
    once the title goes, over byte-identical spine rows, and their titles are
    'the precious root Lama' and 'the precious root Lama, again'. Seven landed
    segments were hidden that way, and worse, any future undeclared copy could
    leave this gate by having its title edited by three words -- the
    silent-staleness shape this repository has been bitten by repeatedly. The
    title is the analyst's caption for the reading; it is not the reading.
    """
    part = T.pair_part(block[5])
    part = part.replace('data-l="s%d' % seq, 'data-l="sX')
    return HEADING.sub('<h2>Segment X</h2>', part, count=1)


def check_undeclared(pages, registered, rows, problems, verbose=False):
    groups = {}
    stamped = set()
    for course, pg, path, doc in pages:
        if course is None:
            continue
        for m in T.MARK_RE.finditer(doc):
            stamped.add((m.group(1), int(m.group(2))))
        for seq, block in blocks_of(doc).items():
            if registered.get((course, seq)) != pg or (course, seq) not in rows:
                continue
            groups.setdefault(label_normalised(block, seq), []).append(
                (course, seq))
    found = set()
    for members in groups.values():
        if len(members) < 2:
            continue
        members = sorted(members)
        if len({rows[k] for k in members}) != 1:
            continue          # same markup, different text: not a transfer
        for k in members[1:]:
            if k not in stamped:
                found.add("%s:%d<-%s:%d" % (k[0], k[1],
                                            members[0][0], members[0][1]))
    frozen = set()
    if os.path.exists(BASELINE):
        for line in io.open(BASELINE, encoding="utf-8"):
            line = line.split("#")[0].strip()
            if line:
                frozen.add(line)
    new = sorted(found - frozen)
    gone = sorted(frozen - found)
    if new:
        problems.append(
            "%d landed segment(s) carry another segment's reading, verbatim "
            "and with byte-identical spine rows, and do not say so: %s. Stamp "
            "them, or add them to %s with a reason."
            % (len(new), ", ".join(new[:12]) + (" ..." if len(new) > 12 else ""),
               os.path.relpath(BASELINE, ROOT)))
    if gone:
        problems.append(
            "%d baselined undeclared transfer(s) no longer measure that way: "
            "%s. A ratchet with slack in it has stopped measuring; re-freeze "
            "%s at what is there now."
            % (len(gone), ", ".join(gone[:12]),
               os.path.relpath(BASELINE, ROOT)))
    if verbose:
        for x in sorted(found):
            print("    %s" % x)
    return found, len(frozen)


def undeclared_exposure(found, links, evidence):
    """How far the undeclared echoes actually reach, RE-DERIVED every run.

    The first version of this gate asserted in its own docstring that the
    evidence layer "counts it up to five times as though several segments
    attested it". Measured, that is not true of this set: all 547 links in the
    91 segments sit at depth 1, 2 and 7, and the evidence layer harvests depth
    5 only, so today not one evidence rendering cites one of them. The claim
    was plausible, unmeasured, and banked in prose where it would have been
    quoted. So the number is printed instead of written down, and it moves on
    its own when the data does.
    """
    targets = {f.split("<-")[0] for f in found}
    n_links = 0
    depths = set()
    for L in links:
        if "%s:%s" % (L["course"], L["seg"]) in targets:
            n_links += 1
            if L.get("d"):
                depths.add(L["d"])
    n_ev = 0
    for _tn, evs in (evidence or {}).get("pairs", {}).items():
        for ev in evs:
            if any(r in targets for r in ev["refs"]):
                n_ev += 1
    return n_links, sorted(depths), n_ev


# --------------------------------------------- PART D: the published dictionary

def transfers_in_bank(links, problems):
    """-> {"COURSE:seq": "SOURCE:seq"}, recounted from the bank itself.

    A segment is transferred or it is not; a bank that stamps only some of a
    segment's spans would publish the same segment twice in one citation list,
    once marked and once not, and a reader would count it as two attestations.
    """
    seen = {}
    for L in links:
        seen.setdefault("%s:%s" % (L["course"], L["seg"]), set()).add(
            L.get("xfer"))
    out = {}
    for ref, vals in sorted(seen.items()):
        marked = {v for v in vals if v}
        if not marked:
            continue
        if len(vals) > 1:
            problems.append(
                "the bank stamps only some of %s's spans as transferred (%s). "
                "The published dictionary cites a SEGMENT, so a half-stamped "
                "segment is published twice in one citation list, once marked "
                "and once not, and reads as two attestations"
                % (ref, ", ".join(sorted(str(v) for v in vals))))
        out[ref] = sorted(marked)[0]
    return out


def check_view(view_path, links, problems, undeclared=()):
    """INDEPENDENT RECOUNT of the published dictionary against the bank.

    Returns (citations marked, transferred segments the bank holds, published
    rows citing an UNDECLARED echo, headwords whose head rendering that
    changes). The last two are measured, not gated: the undeclared 91 are
    Adam's ruling to make, and the gate's job is to keep saying how much they
    reach rather than to decide it.
    """
    if not os.path.exists(view_path):
        problems.append("the published dictionary %s does not exist, so "
                        "nothing can be said about what it shows a reader"
                        % os.path.relpath(view_path, ROOT))
        return 0, 0, 0, 0
    echoes = {f.split("<-")[0] for f in undeclared}
    xfer_of = transfers_in_bank(links, problems)
    # a transferred segment only reaches the page if it contributes a link
    # with both sides; that is the same filter build_dictionary_view applies.
    publishable = {"%s:%s" % (L["course"], L["seg"]) for L in links
                   if L.get("xfer") and L.get("tib") and L.get("eng")}

    P = VM.payload(io.open(view_path, encoding="utf-8").read())
    marked = 0
    echo_rows = echo_moves = 0
    cited = set()
    bands = list(P["depths"].items()) + [("phon", P["phonetics"])]
    for band_name, band in bands:
        for hw, _acip, rl in band:
            # how far the UNDECLARED echoes of Part B reach into this page.
            # Measured and reported, never gated: whether those 91 should be
            # stamped, left alone, or discounted is Adam's ruling.
            counted, discounted = [], []
            for _e, refs in rl:
                k = sum(1 for r in refs if r in echoes)
                echo_rows += 1 if k else 0
                counted.append(len(refs))
                discounted.append(len(refs) - k)
            if len(rl) > 1 and (
                    counted.index(max(counted))
                    != discounted.index(max(discounted))):
                echo_moves += 1
            for eng, refs in rl:
                for r in refs:
                    base, _sep, src = r.partition(V.XFER_MARK)
                    where = "%s (band %s) -> %r" % (hw[:40], band_name, eng[:50])
                    if base in xfer_of:
                        cited.add(base)
                        if not src:
                            problems.append(
                                "the published dictionary cites %s under %s as "
                                "an ordinary citation. The bank says that "
                                "reading was CARRIED from %s and nobody read "
                                "%s, so the page shows an echo of a reading as "
                                "an independent attestation of it"
                                % (base, where, xfer_of[base], base))
                        elif src != xfer_of[base]:
                            problems.append(
                                "the published dictionary says %s was carried "
                                "from %s; the bank says %s"
                                % (base, src, xfer_of[base]))
                        else:
                            marked += 1
                    elif src:
                        problems.append(
                            "the published dictionary marks %s under %s as "
                            "carried from %s, and the bank carries no transfer "
                            "for that segment. Provenance that nothing can be "
                            "checked against is not provenance"
                            % (base, where, src))
            # ORDER. Renderings are shown most-attested first, so the count
            # they are ranked on must be the independent one; otherwise an
            # echo outranks a reading and the first thing a reader sees is
            # the wrong one.
            ind = [V.n_read(refs) for _e, refs in rl]
            if ind != sorted(ind, reverse=True):
                problems.append(
                    "the published dictionary ranks the renderings of %s "
                    "(band %s) by a count that includes carried readings: "
                    "independent counts run %s down the entry, which is not "
                    "in order. An echo must never outrank a reading"
                    % (hw[:40], band_name, ind))

    stale = sorted(publishable - cited)
    if stale:
        problems.append(
            "the bank holds %d transferred segment(s) the published "
            "dictionary does not cite at all (%s). The page predates the "
            "transfer, so it cannot be vouched for either way; rebuild it "
            "with python3 tools/build_dictionary_view.py"
            % (len(stale), ", ".join(stale[:8])))

    declared = P.get("meta", {}).get("n_xfer") or 0
    if declared != marked:
        problems.append(
            "the published dictionary discloses %d carried citation(s) and "
            "carries %d. The disclosure a reader is shown must be the number "
            "that is actually there" % (declared, marked))
    return marked, len(xfer_of), echo_rows, echo_moves


# ------------------------------------------------------------- PART C: selftest

def build_fixture(tmp):
    """Write one real transferred page into `tmp`, and return what it is.

    The pair is DISCOVERED -- the first duplicate whose source page can rebuild
    itself through the generator -- so the fixture follows the campaign instead
    of freezing one segment that may be re-landed tomorrow.
    """
    courses = T.mandate_courses()
    rows = T.spine_rows(courses)
    landed = T.landed_index()
    pairs, _groups = T.duplicate_pairs(courses, rows, landed)
    for tgt, src in sorted((t, s) for t, s in pairs if s in landed):
        pg, pdir = landed[src]
        doc = io.open(os.path.join(pdir, pg + ".html"), encoding="utf-8").read()
        block = blocks_of(doc).get(src[1])
        if block is None:
            continue
        body, nspans, why = T.prove(src, tgt, block, rows, pg)
        if why:
            continue
        return {"target": tgt, "source": src, "source_page": pg,
                "spans": nspans, "body": body, "source_doc": doc,
                "rows": rows}
    return None


def synth_links(body, course, seq, page, xfer):
    """The links the builder would derive from one transferred block."""
    doc = '<div class="pair">' + body.split('<div class="pair">', 1)[1]
    out = []
    for d, lab, _txt in B.spans_of(doc, "tib"):
        rec = {"course": course, "page": page, "seg": seq,
               "id": lab.split("|")[0], "d": d, "tib": _txt}
        if xfer:
            rec["xfer"] = xfer
        out.append(rec)
    return out


def selftest_view(fx, verbose=False):
    """Publish a real transfer through the REAL dictionary builder.

    Part D examines nothing today, because nothing has been transferred. So
    this puts a transfer into a SCRATCH COPY of the bank -- the fixture's own
    discovered pair, with the source segment's links carried across under the
    target's name and stamped, which is exactly what landing the page would
    write -- runs tools/build_dictionary_view.py over it into a scratch
    directory, and then breaks the result three ways:

      the marks are rubbed off the page (the defect this part exists for);
      two renderings of one headword are swapped, so an echo outranks a
        reading;
      the page's disclosure is edited to understate what it carries.

    It also refuses to pass if the fixture never exercises the ordering at
    all, because an ordering check over an entry whose renderings are all
    equally attested proves nothing.
    """
    tgt, src = fx["target"], fx["source"]
    tref, sref = "%s:%d" % tgt, "%s:%d" % src
    bank = json.load(io.open(FULL, encoding="utf-8"))
    page = "c%dx1" % int(tgt[0][1:])
    twin = []
    for L in bank["links"]:
        if "%s:%s" % (L["course"], L["seg"]) == sref:
            t = dict(L)
            t["course"], t["seg"], t["page"] = tgt[0], tgt[1], page
            t["xfer"] = sref
            twin.append(t)
    if not twin:
        print("  [FAIL] the bank holds no links for %s, so the published "
              "dictionary cannot be exercised" % sref)
        return 1
    bank["links"].extend(twin)

    tmp = tempfile.mkdtemp(prefix="xfer-view-")
    full = os.path.join(tmp, "alignment_full_v1.json")
    out = os.path.join(tmp, "view.html")
    with io.open(full, "w", encoding="utf-8") as f:
        json.dump(bank, f, ensure_ascii=False)
    env = dict(os.environ, ALL_VIEW_FULL=full, ALL_VIEW_OUT=out,
               ALL_VIEW_DOCS=tmp)
    r = subprocess.run([sys.executable,
                        os.path.join(ROOT, "tools", "build_dictionary_view.py")],
                       capture_output=True, text=True, cwd=ROOT, env=env)
    failures = 0
    if r.returncode != 0 or not os.path.exists(out):
        print("  [FAIL] the dictionary builder refused the transferred bank: "
              "%s" % r.stderr.strip()[-300:])
        for f in os.listdir(tmp):
            os.unlink(os.path.join(tmp, f))
        os.rmdir(tmp)
        return 1
    doc = io.open(out, encoding="utf-8").read()

    def run(doc_):
        p = os.path.join(tmp, "probe.html")
        io.open(p, "w", encoding="utf-8").write(doc_)
        probs = []
        n, _segs, _er, _em = check_view(p, bank["links"], probs)
        return n, probs

    n, probs = run(doc)
    if n and not probs:
        print("  [PASS] the published dictionary marks all %d carried "
              "citation(s) of %s, naming %s" % (n, tref, sref))
    else:
        print("  [FAIL] the published transfer did not check out: %d marked, "
              "%s" % (n, probs[:2]))
        failures += 1

    # MUTATION 6 -- rub the marks off the published page. This is exactly the
    # state the view was in before 2026-09-22.
    rubbed = doc.replace(V.XFER_MARK + sref, "")
    _n, probs = run(rubbed)
    if any("as an ordinary citation" in p for p in probs):
        print("  [PASS] rubbing the marks off the published dictionary fails "
              "the gate, and it says the page shows an echo of a reading as "
              "an independent attestation of it")
        if verbose:
            print("         -> %s" % probs[0])
    else:
        print("  [FAIL] an unmarked published dictionary passed: %s" % probs[:2])
        failures += 1

    # MUTATION 7 -- let an echo outrank a reading, with every mark intact.
    P = VM.payload(doc)
    swapped = None
    n_would_move = 0
    for band in list(P["depths"].values()) + [P["phonetics"]]:
        for hw, _a, rl in band:
            ind = [V.n_read(refs) for _e, refs in rl]
            tot = [len(refs) for _e, refs in rl]
            if tot != sorted(tot, reverse=True):
                n_would_move += 1        # ranking by the total would differ
            if swapped is None:
                for i in range(len(rl) - 1):
                    if ind[i] > ind[i + 1]:
                        swapped = (hw, i)
                        break
    if not n_would_move:
        print("  [FAIL] this fixture never makes a carried citation outrank a "
              "read one, so the ordering check is not exercised")
        failures += 1
    else:
        print("  [PASS] %d headword(s) would be ranked differently by the "
              "total than by the independent count, so the ordering rule is "
              "load-bearing on this fixture" % n_would_move)
    if swapped is None:
        print("  [FAIL] no entry with unequal independent counts to reorder")
        failures += 1
    else:
        hw, i = swapped
        mutant = json.loads(json.dumps(P))
        for band in list(mutant["depths"].values()) + [mutant["phonetics"]]:
            for e in band:
                if e[0] == hw:
                    e[2][i], e[2][i + 1] = e[2][i + 1], e[2][i]
        _n, probs = run(doc.replace(
            "const PAYLOAD = " + json.dumps(P, ensure_ascii=False,
                                            separators=(",", ":")),
            "const PAYLOAD = " + json.dumps(mutant, ensure_ascii=False,
                                            separators=(",", ":")), 1))
        if any("An echo must never outrank a reading" in p for p in probs):
            print("  [PASS] reordering an entry so a carried citation outranks "
                  "a read one fails the gate")
            if verbose:
                print("         -> %s" % probs[0])
        else:
            print("  [FAIL] a reordered entry passed: %s" % probs[:2])
            failures += 1

    # MUTATION 8 -- the page understates its own disclosure.
    lied = doc.replace('"n_xfer":%d' % n, '"n_xfer":1', 1)
    if lied == doc:
        print("  [FAIL] the page carries no n_xfer disclosure to falsify")
        failures += 1
    else:
        _n, probs = run(lied)
        if any("must be the number that is actually there" in p for p in probs):
            print("  [PASS] understating the disclosure fails the gate")
        else:
            print("  [FAIL] a false disclosure passed: %s" % probs[:2])
            failures += 1

    for f in os.listdir(tmp):
        os.unlink(os.path.join(tmp, f))
    os.rmdir(tmp)
    return failures


def selftest(verbose=False):
    def show(probs):
        if verbose:
            for x in probs[:2]:
                print("         -> %s" % x)
    print("  building a transferred page from the campaign's own data...")
    fx = build_fixture(None)
    if not fx:
        print("  [FAIL] no duplicate pair proves transferable; the selftest "
              "cannot mean anything")
        return 1
    tgt, src = fx["target"], fx["source"]
    print("  fixture: %s:%d <- %s:%d (%d spans)"
          % (tgt[0], tgt[1], src[0], src[1], fx["spans"]))

    tmp = tempfile.mkdtemp(prefix="xfer-selftest-")
    page = "c%dx1" % int(tgt[0][1:])
    path = os.path.join(tmp, page + ".html")
    # through the REAL wrapper, so the fixture is a page of the shape the
    # campaign actually lands, not one shaped to suit the checker
    w = subprocess.run(
        [sys.executable, os.path.join(ROOT, "tools", "wrap_alignment_page.py"),
         path, "selftest", "selftest", "-", "-", tgt[0][1:].lstrip("0")],
        input=fx["body"] + "\n", capture_output=True, text=True, cwd=ROOT)
    if w.returncode != 0:
        print("  [FAIL] the wrapper refused the fixture: %s" % w.stderr[:200])
        return 1
    doc = io.open(path, encoding="utf-8").read()

    src_pg = fx["source_page"]
    registered = {tgt: page, src: src_pg}
    xfer = "%s:%d" % src
    links = synth_links(fx["body"], tgt[0], tgt[1], page, xfer)

    def run(doc_, links_):
        pages = [(tgt[0], page, path, doc_),
                 (src[0], src_pg, "source", fx["source_doc"])]
        probs = []
        n = check_declared(pages, registered, fx["rows"], links_, None, probs)
        return n, probs

    failures = 0

    n, probs = run(doc, links)
    if n == 1 and not probs:
        print("  [PASS] the clean transferred page passes, and is examined")
    else:
        print("  [FAIL] the clean page did not pass: %s" % probs[:2])
        failures += 1

    # MUTATION 1 -- corrupt a transferred span.
    m = re.search(r'(<span class="u[^"]*" data-d="5" data-l="s%d[^"]*">)'
                  r'([^<]+)(</span>)' % tgt[1], doc)
    if not m:
        print("  [FAIL] no depth-5 span to corrupt in the fixture")
        failures += 1
    else:
        broken = doc[:m.start(2)] + m.group(2) + "XQZ" + doc[m.end(2):]
        _n, probs = run(broken, links)
        if any("NOT PRESENT" in p for p in probs):
            print("  [PASS] corrupting a transferred span fails the gate, and "
                  "it says the span is not present in the target's own text")
            show(probs)
        else:
            print("  [FAIL] a corrupted transferred span passed: %s" % probs)
            failures += 1

    # MUTATION 2 -- strip the provenance field off the banked spans.
    stripped = [{k: v for k, v in L.items() if k != "xfer"} for L in links]
    _n, probs = run(doc, stripped)
    if any("does not say so in the bank" in p for p in probs):
        print("  [PASS] stripping xfer off the bank fails the gate, and it "
              "says a transferred span that does not say so is "
              "indistinguishable from one an analyst read")
        show(probs)
    else:
        print("  [FAIL] a bank with no provenance passed: %s" % probs)
        failures += 1

    # MUTATION 3 -- remove the stamp while the bank still claims transfer.
    unstamped = re.sub(r'<!-- TRANSFER v1 .*?-->\n', "", doc)
    _n, probs = run(unstamped, links)
    if any("no page stamps" in p for p in probs):
        print("  [PASS] removing the page stamp fails the gate, and it says "
              "the bank's claim has nothing to check against")
        show(probs)
    else:
        print("  [FAIL] an unstamped page with a claiming bank passed: %s"
              % probs)
        failures += 1

    # MUTATION 4 -- alter a span WITHOUT touching a character of its text, by
    # giving it a grammar label the source does not have. The span counts stay
    # equal, so this is the case where a message that reports the two counts
    # says nothing at all.
    m = re.search(r'<span class="u" data-d="5" data-l="s%d[^"]*">' % tgt[1], doc)
    if not m:
        print("  [FAIL] no unlabelled depth-5 span to relabel in the fixture")
        failures += 1
    else:
        relabelled = (doc[:m.start()]
                      + m.group(0).replace('class="u"', 'class="u noun"')
                      + doc[m.end():])
        _n, probs = run(relabelled, links)
        if any("differs in cls" in p for p in probs):
            print("  [PASS] altering a span's grammar label fails the gate, "
                  "and it names the span and the field rather than two equal "
                  "counts")
            show(probs)
        else:
            print("  [FAIL] a relabelled span passed, or was reported without "
                  "naming the difference: %s" % probs[:2])
            failures += 1

    # MUTATION 5 -- say the reading came from a segment it does not match.
    other = None
    for (c, q) in fx["rows"]:
        if (c, q) != src and fx["rows"][(c, q)] != fx["rows"][src]:
            other = (c, q)
            break
    lied = doc.replace('source="%s:%d"' % src, 'source="%s:%d"' % other)
    reg2 = dict(registered)
    reg2[other] = src_pg
    pages = [(tgt[0], page, path, lied),
             (src[0], src_pg, "source", fx["source_doc"])]
    probs = []
    check_declared(pages, reg2, fx["rows"], links, None, probs)
    if any("byte-identical rows" in p for p in probs):
        print("  [PASS] naming a source whose text differs fails the gate")
        show(probs)
    else:
        print("  [FAIL] a transfer from a non-identical row passed: %s" % probs)
        failures += 1

    for f in os.listdir(tmp):
        os.unlink(os.path.join(tmp, f))
    os.rmdir(tmp)

    print("  publishing that transfer through the real dictionary builder...")
    failures += selftest_view(fx, verbose)
    return failures


# ------------------------------------------------------------------------ main

def main():
    verbose = "--verbose" in sys.argv
    if "--selftest" in sys.argv:
        print("transfer provenance — selftest")
        f = selftest("--verbose" in sys.argv)
        print("  %s" % ("selftest OK" if not f else "%d FAILURE(S)" % f))
        return 1 if f else 0

    pages = read_pages()
    registered = registered_index()
    courses = T.mandate_courses()
    rows = T.spine_rows(courses)
    links = json.load(io.open(FULL, encoding="utf-8"))["links"] \
        if os.path.exists(FULL) else []
    evidence = json.load(io.open(EVID, encoding="utf-8")) \
        if os.path.exists(EVID) else None

    problems = []
    n_declared = check_declared(pages, registered, rows, links, evidence,
                                problems)
    found, n_frozen = check_undeclared(pages, registered, rows,
                                       problems, verbose)
    n_links, depths, n_ev = undeclared_exposure(found, links, evidence)
    n_marked, n_xfer_segs, n_echo_rows, n_echo_moves = check_view(
        VIEW, links, problems, found)

    print("transfer provenance, across %d pages" % len(pages))
    print("  declared transfers examined     %d" % n_declared)
    print("  undeclared transfers measured   %d (baseline %d)"
          % (len(found), n_frozen))
    print("    they carry %d bank link(s), at depth(s) %s; %d evidence-layer "
          "rendering(s) cite one" % (n_links, ", ".join(str(d) for d in depths),
                                     n_ev))
    print("    %d published dictionary row(s) cite one, and discounting them "
          "would move the head rendering of %d headword(s)"
          % (n_echo_rows, n_echo_moves))
    print("  transferred segments in the bank %d, carried citations marked in "
          "the published dictionary %d" % (n_xfer_segs, n_marked))
    if problems:
        for p in problems[:40]:
            print("  [FAIL] %s" % p)
        if len(problems) > 40:
            print("  ... and %d more" % (len(problems) - 40))
        return 1
    print("  every transferred span is verbatim in its own segment and names "
          "the reading it came from, in the bank and in the published "
          "dictionary alike")
    if n_declared == 0:
        print("  NOTE: nothing has been transferred yet, so Parts A and D "
              "examined nothing. Run --selftest, which builds a transfer, "
              "breaks it five ways, and then publishes it through the real "
              "dictionary builder and breaks that three ways; that is what "
              "makes this gate mean something today.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
