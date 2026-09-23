#!/usr/bin/env python3
"""The precedent index must be the bank, folded — not the bank as it was.

data/alignment/precedent_index_v1.json is what tells an analyst that
`zhi gnas` is "quietude" eleven times and that `'i` goes unrendered 467
times out of 492. Those counts are acted on: a reconciler nulls a particle
because the bank says it is nearly always nulled, and takes a boundary
because the bank says the tighter span is the attested one. A stale index
does not look stale. It looks like an answer, and it is the one artifact in
this campaign a reader has no way to sanity-check, because checking it by
hand means re-deriving it from a 10.9 MB file — which is the work the index
exists to remove. An index that has drifted from the bank is therefore worse
than no index at all.

The generator is deterministic and carries no build date, so the check is the
one docs/ERRATA_REGISTER.md gets in test_generated_docs_in_sync.py:
regenerate into a temporary file and compare bytes. Land a page, forget to
rebuild, and this says so.

A second check asks a question regeneration CANNOT ask. Regenerating compares
the generator against itself: if the fold miscounts, both copies miscount
alike and the comparison passes. So the totals in meta — links, headwords,
renderings, null links, uncited links — are recomputed here straight from the
bank, by code that shares nothing with the builder, and compared. That is the
check that fails when the FOLD is wrong rather than when the FILE is old.

AND THAT WAS STILL NOT ENOUGH, WHICH IS THE WHOLE LESSON OF THIS FILE. Both
checks above are blind to the INDEX BODY. Regeneration compares the generator
with itself; the meta totals are counts of LINKS, and not one of them involves
a citation's depth, its case flag or its transfer mark. A reviewer proved it:
rewriting cite() so every citation reports depth 5 falsified 20,292 of the
50,370 citations — 40% of the file's content — left the file at exactly the
same 3,200,419 bytes, and both checks printed success and exited 0. Dropping
the case flag lost all 1,698 case markings for the same clean pass.

So the third check RECOUNTS THE BODY. It parses every citation in the index
with its own expression, tallies them by depth, by case flag, by transfer
mark, by course and by whether the bank could place them on a corpus row, and
holds each tally against a count taken straight from the bank. It then
compares the citations THEMSELVES, one for one, against the links that should
have made them — because a tally still adds up for a citation that is
perfectly well formed and points at the wrong segment. A regeneration check
can never catch a broken generator. Only an independent recount can.

AND THE SAME HOLE HAD A SECOND MOUTH, found by re-verifying the fix. Every
recount above reads CITATIONS, and the ACIP headform on a record is not a
citation — so nothing looked at it. A one-character corruption of the fold's
ACIP falsified 9,021 of the 11,283 headforms and all three checks printed
success. ACIP is the join key for the whole campaign (Adam, 2026-08-28) and
this file's own schema calls it "the source of record". compare_acip now
holds it to the bank, using the schema's own claim — unambiguous, 0 conflicts
— rather than the builder's "first link wins", so it stays a second opinion
and also fails the day that claim stops being true.

A FOURTH CHECK, ON THE TEXT RATHER THAN THE FILE. Everything above judges the
committed JSON. No analyst reads the JSON: render() turns a record into the
plain text every propose and reconcile prompt carries, and that text is where
a count is quoted to somebody. It is where the transfer marking had a live
defect (a case flag tested with endswith("c"), which a transferred citation
ending in x silently defeats). It is checked on a written-out fixture that
carries transfers, not on the live bank, because the bank carries none today
and a check that asserts nothing until the first transfer lands is a check
that can be deleted in the meantime without anything going red.
"""
import collections
import io
import json
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INDEX = os.path.join(ROOT, "data", "alignment", "precedent_index_v1.json")
BANK = os.path.join(ROOT, "data", "alignment", "alignment_full_v1.json")
GEN = os.path.join(ROOT, "tools", "build_precedent_index.py")


def check_in_sync():
    """Regenerate the index into a temp file and compare it byte for byte."""
    if not os.path.exists(BANK):
        return "SKIPPED", "no alignment bank present"
    if not os.path.exists(INDEX):
        return "MISSING", ("data/alignment/precedent_index_v1.json does not "
                           "exist — build it: python3 "
                           "tools/build_precedent_index.py")
    before = io.open(INDEX, "rb").read()
    tmp = tempfile.mkdtemp()
    try:
        # Written somewhere else on purpose: a check that regenerates over the
        # committed file cannot fail, because it destroys the evidence on its
        # way to reading it.
        out = os.path.join(tmp, "regen.json")
        r = subprocess.run([sys.executable, GEN, "--out", out],
                           capture_output=True, text=True, cwd=ROOT)
        if r.returncode != 0:
            return "FAILED", ("generator exited %d: %s"
                              % (r.returncode, r.stderr.strip()[:300]))
        after = io.open(out, "rb").read()
    finally:
        import shutil
        shutil.rmtree(tmp, ignore_errors=True)
    if before == after:
        return "OK", ("%d bytes, byte-identical to a fresh fold of the bank"
                      % len(before))
    # say WHAT drifted, not merely that something did: the first headword
    # whose record differs is the thing somebody needs to look at.
    try:
        a = json.loads(before.decode("utf-8"))
        b = json.loads(after.decode("utf-8"))
    except ValueError:
        return "DRIFTED", ("committed %d bytes, regenerated %d, and the "
                           "committed file is not valid JSON"
                           % (len(before), len(after)))
    # The INDEX first, then meta. meta carries the bank's digest, so a bank
    # that moved by one character differs there too — and "bank_sha256 is a
    # different hash" sends nobody anywhere, while "headword 'zhi gnas'
    # differs" is the thing somebody can act on.
    ai, bi = a["index"], b["index"]
    gone = sorted(set(ai) - set(bi))
    new = sorted(set(bi) - set(ai))
    if gone or new:
        return "DRIFTED", ("%d headwords only in the committed index (%.60r), "
                           "%d only in a fresh fold (%.60r)"
                           % (len(gone), gone[:3], len(new), new[:3]))
    for k in sorted(ai):
        if ai[k] != bi[k]:
            return "DRIFTED", "headword %r: %s" % (k, describe(ai[k], bi[k]))
    for k in sorted(set(a["meta"]) | set(b["meta"])):
        if a["meta"].get(k) != b["meta"].get(k):
            return "DRIFTED", ("meta.%s: committed %.120r, regenerated %.120r"
                               % (k, a["meta"].get(k), b["meta"].get(k)))
    return "DRIFTED", ("the two files differ in bytes but not in content — "
                       "committed %d, regenerated %d" % (len(before),
                                                         len(after)))


def describe(x, y):
    """Say WHICH part of a record moved.

    The first version of this printed the head of each record, and under a
    one-citation edit both sides printed the same 150 characters -- a failure
    message that proved a difference existed and hid it. A gate that cannot
    say what it caught sends the reader back to diffing a 3 MB file by hand.
    """
    for f in sorted(set(x) | set(y)):
        if f != "eng" and x.get(f) != y.get(f):
            return "%s: committed %.100r, regenerated %.100r" % (f, x.get(f),
                                                                 y.get(f))
    xe, ye = x.get("eng", []), y.get("eng", [])
    for i in range(max(len(xe), len(ye))):
        a = xe[i] if i < len(xe) else None
        b = ye[i] if i < len(ye) else None
        if a == b:
            continue
        if a is None or b is None:
            return "rendering %d: committed %.80r, regenerated %.80r" % (i, a,
                                                                         b)
        if a[0] != b[0]:
            return ("rendering %d is %r in the committed index and %r in a "
                    "fresh fold" % (i, a[0], b[0]))
        ax, bx = set(a[1]), set(b[1])
        return ("%r: %d links committed, %d in a fresh fold; only committed "
                "%.80r, only fresh %.80r"
                % (a[0], len(a[1]), len(b[1]), sorted(ax - bx)[:4],
                   sorted(bx - ax)[:4]))
    return "committed %.80r, regenerated %.80r" % (x, y)


# The citation grammar, written out here rather than imported: an
# expression borrowed from the builder would agree with the builder by
# construction, which is the failure this check exists to end.
# COURSE:seg/depth, seg a number or ?, optional c for a case particle,
# optional x for a transferred span.
CITE = re.compile(r"^([^:]+):(\d+|\?)/(\d+)(c?)(x?)$")

# A headword the generator's word rule can find no word in -- no letter, no
# digit, no Tibetan character -- cannot be matched to any segment, so it can
# never be offered as precedent. The bank holds one (the empty Tibetan of
# C03:126, id s126tail). meta.unlocatable_headwords is a NUMBER IN A
# COMMITTED FILE, and until this expression existed nothing recounted it:
# check_in_sync only proves the generator agrees with itself. Written out
# from the rule as the builder's docstring STATES it, not copied from its
# code.
HASWORD = re.compile(r"[A-Za-z0-9\u0f00-\u0fff]")


def body_tally(index):
    """Count every citation in the index BODY, by hand, here.

    Nothing above this reads the body at all. The point is to produce, from
    the committed file alone, the same six quantities the next function
    produces from the bank alone — so that a fold which gets the totals right
    and the details wrong has somewhere to fail.

    A token this cannot parse is COUNTED AND NAMED, never skipped. Skipping
    would let a fold that emitted rubbish tally zero of everything and look
    orderly while doing it.
    """
    t = {"depth": collections.Counter(), "course": collections.Counter(),
         "case": 0, "xfer": 0, "uncited": 0, "total": 0, "unreadable": 0}
    examples = []
    for tib, rec in index.items():
        cites = [c for _e, cs in rec.get("eng", ()) for c in cs]
        cites += list(rec.get("nul", ()))
        for c in cites:
            t["total"] += 1
            m = CITE.match(c)
            if m is None:
                t["unreadable"] += 1
                if len(examples) < 3:
                    examples.append("%r cited %r" % (tib, c))
                continue
            t["course"][m.group(1)] += 1
            t["uncited"] += m.group(2) == "?"
            t["depth"][m.group(3)] += 1
            t["case"] += bool(m.group(4))
            t["xfer"] += bool(m.group(5))
    t["examples"] = examples
    return t


def cite_of(l):
    """What one bank link's citation MUST read, spelled out here.

    Yes, this says the same thing build_precedent_index.cite() says. That is
    the point and not an oversight: the citation format is a contract, and a
    contract asserted in one place is an assumption. A tally can only catch a
    fold that gets the SHAPE wrong — a citation that is perfectly well formed
    and points at the wrong segment leaves every depth, case, course and
    transfer count exactly right. Nothing but the citations themselves,
    compared one for one against the links that made them, can catch that.

    If the format ever changes, change it here SEPARATELY, from the spec,
    rather than pasting the builder's line in. A copy agrees by construction
    and checks nothing.
    """
    return "%s:%s/%d%s%s" % (l["course"],
                             "?" if l["seg"] is None else l["seg"],
                             l["d"],
                             "c" if l.get("case") else "",
                             "x" if l.get("xfer") else "")


def fold_bank(links):
    """The index body, refolded here from the bank: tib -> eng -> citations.

    Order is deliberately NOT reproduced. check_in_sync already pins the
    order to the generator, and duplicating the sort would turn this into a
    copy of the builder instead of a second opinion. Content is compared as
    a multiset; the ordering has its own check, which reads it off the body.
    """
    eng = collections.defaultdict(
        lambda: collections.defaultdict(collections.Counter))
    nul = collections.defaultdict(collections.Counter)
    for l in links:
        c = cite_of(l)
        if l["eng"] is None:
            nul[l["tib"]][c] += 1
        else:
            eng[l["tib"]][l["eng"]][c] += 1
    return eng, nul


def fold_index(index):
    """The same shape, read out of the committed file."""
    eng = collections.defaultdict(
        lambda: collections.defaultdict(collections.Counter))
    nul = collections.defaultdict(collections.Counter)
    dups = []
    for t, rec in index.items():
        seen = set()
        for e, cs in rec.get("eng", ()):
            if e in seen:
                dups.append("%r lists the rendering %r twice" % (t, e))
            seen.add(e)
            eng[t][e].update(cs)
        if rec.get("nul"):
            nul[t].update(rec["nul"])
    return eng, nul, dups


def either_way(got, wanted):
    """What one side holds that the other does not, said both ways."""
    return ("the index body has %r the bank does not, and the bank has %r "
            "the index body does not"
            % (sorted((got - wanted).elements())[:3],
               sorted((wanted - got).elements())[:3]))


def compare_bodies(index, links):
    """Every citation in the body against the link that should have made it.

    Returns the differences, most-specific first, capped — a fold that has
    gone wrong usually goes wrong everywhere at once, and five named
    headwords send somebody to the cause faster than twelve thousand do.
    """
    be, bn = fold_bank(links)
    ie, iN, bad = fold_index(index)
    n = 0
    for t in sorted(set(be) | set(ie) | set(bn) | set(iN)):
        for e in sorted(set(be.get(t, {})) | set(ie.get(t, {}))):
            got = ie.get(t, {}).get(e, collections.Counter())
            wanted = be.get(t, {}).get(e, collections.Counter())
            if got != wanted:
                n += 1
                if len(bad) < 5:
                    bad.append("headword %r -> %r: %s"
                               % (t, e, either_way(got, wanted)))
        got, wanted = (iN.get(t, collections.Counter()),
                       bn.get(t, collections.Counter()))
        if got != wanted:
            n += 1
            if len(bad) < 5:
                bad.append("headword %r, its links with NO English: %s"
                           % (t, either_way(got, wanted)))
    if n > 5:
        bad.append("%d citation list(s) in all disagree with the bank; the "
                   "five above are the first by headword" % n)
    return bad


def compare_acip(index, links):
    """The ACIP headform on every record, against the bank.

    ACIP IS THE JOIN KEY (Adam, 2026-08-28), not a decoration: the schema in
    this very file calls it "the source of record". It is also the one field
    in a record that is NOT a citation, so nothing above this looks at it.
    That gap is the same shape as the one this file was rewritten to close --
    a fold that corrupts per-record metadata regenerates byte-identically and
    every tally still adds up. Measured: a one-character corruption of the
    fold's ACIP falsified 9,021 of the 11,283 headforms and all three checks
    printed success.

    The rule is taken from the SCHEMA'S OWN CLAIM rather than from the
    builder's code, so this stays a second opinion: the schema says the ACIP
    is "unambiguous across the whole bank, 0 conflicts". So for each headword
    the bank's distinct non-empty tib_acip values are collected here; there
    must be exactly one and the record must carry it, or none and the record
    must carry no acip at all. Reproducing the builder's "first link in its
    own sort order wins" would be a copy of the builder and would check
    nothing -- and it would also hide the day the no-conflict claim stops
    being true, which is itself worth failing on: the value would then depend
    on a sort nobody chose for that purpose.
    """
    forms = collections.defaultdict(set)
    for l in links:
        if l.get("tib_acip"):
            forms[l["tib"]].add(l["tib_acip"])
    bad, n, conflicts = [], 0, 0
    for t in sorted(set(index) | set(forms)):
        want = forms.get(t, set())
        got = index.get(t, {}).get("acip")
        if len(want) > 1:
            conflicts += 1
            if len(bad) < 5:
                bad.append("headword %r: the bank carries %d different ACIP "
                           "forms for it (%.60r), so the schema's "
                           "\"unambiguous, 0 conflicts\" is false and the "
                           "index's %r is whichever link the fold saw first"
                           % (t, len(want), sorted(want), got))
            continue
        wanted = sorted(want)[0] if want else None
        if got != wanted:
            n += 1
            if len(bad) < 5:
                bad.append("headword %r: ACIP is %r in the index body and %r "
                           "in the bank" % (t, got, wanted))
    if n > 5 or conflicts > 5:
        bad.append("%d record(s) carry an ACIP the bank does not, and the "
                   "bank holds %d headword(s) with conflicting ACIP forms; "
                   "the five above are the first by headword" % (n, conflicts))
    return bad


def n_read(cites):
    """How many of these citations are independent readings, not copies."""
    return sum(1 for c in cites if not c.endswith("x"))


def seg_rank(c):
    """Where a citation sits, for ordering: (course, segment), ? last.

    Read off the citation text with this file's own expression. A token this
    cannot parse sorts last and is named by check_totals, which is where an
    unreadable citation belongs; this function's job is only the order.
    """
    m = CITE.match(c)
    if m is None:
        return ("\uffff", 1, 0)
    return (m.group(1), 1, 0) if m.group(2) == "?" else (m.group(1), 0,
                                                         int(m.group(2)))


def check_order():
    """Renderings must be strongest-first, and STRONGEST MEANS READINGS.

    This repo's recurring defect is a ranking that sorts on the biggest
    number to hand rather than the one that means something — spans ranked
    by length while ignoring tier, found in five separate places. The
    tempting number here is the citation count, and the citation count
    includes TRANSFERRED copies: a rendering echoed forty times across
    byte-identical segments would head the list over one an analyst read
    twelve times, and the head of the list is the first thing anybody reads.

    Read entirely off the body's own citations, so it holds whatever the
    builder happens to do, and it cannot be satisfied by a stored number.
    """
    if not os.path.exists(INDEX):
        return "MISSING", "no index to check"
    index = json.load(io.open(INDEX, encoding="utf-8"))["index"]
    bad, n = [], 0
    for t in sorted(index):
        # AND THE ORDER INSIDE A LIST, which is not the same question.
        # compare_bodies deliberately compares each list as a MULTISET, so it
        # stays a second opinion rather than a copy of the fold -- which
        # leaves the ORDER of the citations inside one list judged by nothing
        # but regeneration, and regeneration compares the fold with itself.
        # Measured: reversing the fold's link sort left all four checks green
        # while 3,578 of the 22,209 renderings changed which attestation the
        # extract prints beside them. Every one is still a real citation, so
        # nothing false is stated (rule 10 holds) -- but the extract prints
        # exactly ONE of them, and that one is what an analyst opens to see
        # the rendering in context. The property is the builder's own stated
        # rule, restated here: course/segment order decides which citation
        # comes first, with the unplaceable ? last.
        lists = [(e, cs) for e, cs in index[t].get("eng", ())]
        if index[t].get("nul"):
            lists.append((None, index[t]["nul"]))
        for e, cs in lists:
            ranks = [seg_rank(c) for c in cs]
            if ranks != sorted(ranks):
                n += 1
                if len(bad) < 5:
                    i = next(j for j in range(1, len(ranks))
                             if ranks[j] < ranks[j - 1])
                    bad.append("headword %r%s cites %s before %s; citations "
                               "inside one list run in course/segment order, "
                               "and the extract prints the FIRST of them as "
                               "the attestation to look at"
                               % (t, " -> %r" % e if e else " (no English)",
                                  cs[i - 1], cs[i]))
        eng = index[t].get("eng")
        if not eng:
            continue
        keys = [(-n_read(cs), -len(cs), e) for e, cs in eng]
        if keys != sorted(keys):
            n += 1
            if len(bad) < 5:
                first = next(i for i in range(1, len(keys))
                             if keys[i] < keys[i - 1])
                a, b = eng[first - 1], eng[first]
                bad.append(
                    "headword %r lists %r (%d links, %d read) ahead of %r "
                    "(%d links, %d read)"
                    % (t, a[0], len(a[1]), n_read(a[1]),
                       b[0], len(b[1]), n_read(b[1])))
    if bad:
        if n > 5:
            bad.append("%d headwords in all are out of order" % n)
        return "WRONG", "\n     ".join(bad)
    xf = sum(1 for t in index for _e, cs in index[t].get("eng", ())
             for c in cs if c.endswith("x"))
    return "OK", ("%d headwords, renderings strongest-first by independent "
                  "readings (%d transferred citation(s) in the body carry no "
                  "ranking weight)" % (len(index), xf))


def check_totals():
    """Recount the bank here, independently, and hold the index to it.

    Nothing in this function calls the builder or imports it. It is a second
    opinion, deliberately written in a different shape, because the point is
    to disagree with the fold when the fold is wrong.

    Two halves. The first holds META to the bank — how many links, headwords,
    renderings, nulls, uncited and transferred the bank has. The second holds
    the BODY to the bank: the depth on every citation, the case flag, the
    transfer mark, the course and the unplaceable ?. The first half was here
    before and passed a fold that falsified 40% of the citations, because a
    depth appears in no total. The second half is the one that fails on that.
    """
    if not os.path.exists(BANK):
        return "SKIPPED", "no alignment bank present"
    if not os.path.exists(INDEX):
        return "MISSING", "no index to check"
    doc = json.load(io.open(INDEX, encoding="utf-8"))
    meta, index = doc["meta"], doc["index"]
    links = json.load(io.open(BANK, encoding="utf-8"))["links"]
    pairs = collections.Counter()
    heads, nulls, uncited, cased, xfers = set(), 0, 0, 0, 0
    courses = collections.Counter()
    depths = collections.Counter()
    for l in links:
        heads.add(l["tib"])
        courses[l["course"]] += 1
        depths[str(l["d"])] += 1
        if l.get("case"):
            cased += 1
        if l.get("xfer"):
            xfers += 1
        if l["seg"] is None:
            uncited += 1
        if l["eng"] is None:
            nulls += 1
        else:
            pairs[(l["tib"], l["eng"])] += 1
    # headwords is in here twice on purpose. Every Tibetan string with a link
    # gets a record, so the number of records and the number of distinct
    # strings in the bank are the same number seen from two ends; a fold that
    # starts quietly dropping records breaks the identity.
    want = {"bank_links": len(links), "bank_headwords": len(heads),
            "headwords": len(heads), "indexed_links": len(links),
            "renderings": len(pairs), "null_links": nulls,
            "uncited_links": uncited, "xfer_links": xfers,
            # recounted off the BODY's own keys, so the day that empty span
            # is repaired this number has to move with it
            "unlocatable_headwords": sum(1 for t in index
                                         if not HASWORD.search(t)),
            "courses": dict(courses)}
    # every mismatch, not the first: a broken fold usually breaks several
    # totals at once, and fixing them one run at a time is how a morning goes.
    bad = ["%s: index says %r, the bank says %r" % (k, meta.get(k), v)
           for k, v in sorted(want.items()) if meta.get(k) != v]

    b = body_tally(index)
    if b["unreadable"]:
        bad.append("%d of the %d citations in the index body are not "
                   "COURSE:seg/depth[c][x] and could not be counted at all, "
                   "e.g. %s" % (b["unreadable"], b["total"],
                                "; ".join(b["examples"])))
    for d in sorted(set(depths) | set(b["depth"]), key=int):
        if b["depth"][d] != depths[d]:
            bad.append("citations at DEPTH %s: the index body has %d, the "
                       "bank has %d links at that depth"
                       % (d, b["depth"][d], depths[d]))
    for what, got, wanted in (
            ("flagged a CASE particle", b["case"], cased),
            ("marked TRANSFERRED (trailing x)", b["xfer"], xfers),
            ("with NO SEGMENT number (?)", b["uncited"], uncited),
            ("in all", b["total"], len(links))):
        if got != wanted:
            bad.append("citations %s: the index body has %d, the bank has %d"
                       % (what, got, wanted))
    for c in sorted(set(courses) | set(b["course"])):
        if b["course"][c] != courses[c]:
            bad.append("citations in COURSE %s: the index body has %d, the "
                       "bank has %d" % (c, b["course"][c], courses[c]))
    # and finally the citations themselves, one for one. Everything above is
    # a tally, and a tally adds up for a citation that is well formed and
    # points at the wrong place.
    bad += compare_bodies(index, links)
    # and the one field in a record that is not a citation. Everything above
    # reads citations; nothing above reads the ACIP.
    bad += compare_acip(index, links)
    if bad:
        return "WRONG", "\n     ".join(bad)
    return "OK", ("%d links, %d headwords, %d renderings, %d null links, "
                  "%d uncited, %d transferred — all re-derived from the "
                  "bank; and all %d citations in the index BODY recounted "
                  "against it — by depth (%s), case %d, transferred %d, "
                  "across %d course(s)"
                  % (len(links), len(heads), len(pairs), nulls, uncited,
                     xfers, b["total"],
                     " ".join("d%s %d" % (d, depths[d])
                              for d in sorted(depths, key=int)),
                     cased, xfers, len(courses)))


def check_render_marks_transfers():
    """The EXTRACT TEXT is what reaches an analyst, and nothing gated it.

    Everything above this checks the committed JSON. But no analyst reads the
    JSON: build_precedent_index.render() turns one segment's record into the
    plain text that dump_spine_segments.py inlines into every propose and
    reconcile prompt, and that text is where a count is actually QUOTED to
    somebody. It had a live defect of exactly the kind this file exists for --
    the case-particle line counted with c.endswith("c"), so a case particle on
    a transferred span (cited C07:85/5cx, ending in x) would have counted zero
    for precisely those links the day the first transfer landed, silently.
    That instance is fixed. Nothing stopped the next one.

    A FIXTURE, NOT THE LIVE BANK, ON PURPOSE. The bank carries no transfers
    today, so a check that rendered a real segment would assert nothing about
    transfer marking and would go on passing after somebody deleted the
    marking -- the vacuous-pass shape. The record below is written out here
    with known contents, so this check has the same force on the day before
    the first transfer as on the day after.

    The expectations are written from the SPEC -- "a transferred citation
    carries a trailing x", "a count prints x11/8r when links and readings
    differ", "the case line counts the case flag" -- not from the rendering
    code, so agreeing with it is not automatic.
    """
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    try:
        import build_precedent_index as B
    except Exception as e:                                   # noqa: BLE001
        return "FAILED", "cannot import the builder to render with: %s" % e
    ex = {"course": "C09", "seq": 7, "wylie_chars": 40, "headwords": 1,
          "links": 5, "read_links": 2, "xfer_links": 3,
          "own_links_removed": 0,
          "entries": [{"tib": "lam", "acip": "LAM",
                       # read twice, echoed once: the count must say so
                       "eng": [["path", ["C01:25/5", "C03:214/5",
                                         "C07:85/5x"]],
                               # wholly transferred: no reading at all
                               ["road", ["C07:85/6x"]]],
                       # a CASE PARTICLE on a transferred span: ends in x,
                       # so a last-character test counts it as not-a-case
                       "nul": ["C07:85/6cx"]}]}
    text = B.render(ex)
    bad = []
    if "3 of them transferred" not in text:
        bad.append("the header does not state how many of the segment's "
                   "links are transferred copies")
    if "x5/2r" not in text:
        bad.append("the entry count does not print links/readings (x5/2r) "
                   "when 5 links carry only 2 independent readings")
    if 'x3/2r' not in text:
        bad.append('the rendering "path" (3 citations, 2 read) does not '
                   'print x3/2r')
    if 'x1/0r' not in text:
        bad.append('the rendering "road" (1 citation, 0 read) does not '
                   'print x1/0r -- a rendering nobody read is showing as '
                   'though somebody had')
    if "case particle on 1 of those" not in text:
        bad.append("the case-particle line does not count a case particle "
                   "that sits on a TRANSFERRED span (C07:85/6cx). This is "
                   "the endswith() defect returning: the flag must be read "
                   "from the parsed citation, not from its last character")
    if bad:
        return "WRONG", ("\n     ".join(bad)
                         + "\n     the text it rendered was:\n"
                         + "".join("       " + ln + "\n"
                                   for ln in text.splitlines()))
    return "OK", ("a transferred citation is marked x, a count of 5 links "
                  "over 2 readings prints x5/2r, a rendering with no reading "
                  "prints x1/0r, and a case particle on a transferred span "
                  "is still counted a case particle")


CHECKS = [("data/alignment/precedent_index_v1.json", check_in_sync),
          ("its totals and every citation in it, against the bank",
           check_totals),
          ("its renderings ranked by readings, not by citation count",
           check_order),
          ("the extract TEXT an analyst reads, on a fixture that carries "
           "transfers", check_render_marks_transfers)]


def main():
    # "in sync" is a CLAIM, and a check that could not run has not made it.
    # SKIPPED prints as itself, is counted, and is named again at the end.
    bad = skipped = 0
    for name, fn in CHECKS:
        status, detail = fn()
        if status == "OK":
            print("in sync: %s — %s" % (name, detail))
        elif status == "SKIPPED":
            skipped += 1
            print("SKIPPED (NOT verified): %s — %s" % (name, detail))
        else:
            bad += 1
            print("FAIL %s: %s\n     %s" % (name, status, detail))
    if skipped:
        print("%d of %d check(s) could NOT be run" % (skipped, len(CHECKS)))
    if bad:
        print("rebuild with: python3 tools/build_precedent_index.py")
        return 1
    if skipped:
        # ANY skip, not only a clean sweep of them. When this file held two
        # checks, both needed the bank, so "no bank" meant "nothing ran" and
        # 77 was exact. The ordering check needs only the committed index,
        # so on a tree with no bank it would run, pass, and carry the whole
        # suite to exit 0 — ctest printing "Passed" for a run in which the
        # two checks that compare the index to the bank never happened. A
        # run that could not make its claim reports that it could not make
        # it; the lines above say exactly which part did run.
        return 77          # ctest's skip code: this run proved less than it
    return 0               # claims to


if __name__ == "__main__":
    sys.exit(main())
