#!/usr/bin/env python3
"""transfer_duplicate_segments.py — carry a reading across to a segment that is
byte-identical to one already read, and say permanently that it was carried.

WHY THIS EXISTS. 2,099 of the 6,172 segments still mandated by the C01-C18
campaign are byte-identical, in BOTH the Tibetan and Geshe Michael's English,
to a segment that has already been read or to each other. The liturgical
opening that starts C01, C03, C04 and C05 is the visible case; there are 2,099
of them. Reading each one again costs a full three-agent pass and re-derives an
answer the bank already holds.

The obvious shortcuts are all wrong, and each was rejected before this tool was
written:

  * SKIPPING a duplicate leaves a hole, and the coverage gate in
    build_alignment_layer.py can then no longer prove the course complete.
    Silent omission is not a saving, it is a lie by absence.
  * COPYING the page and editing the course label by hand is how C04's first
    seven pages were made. It worked, and it is unrepeatable at 2,099.
  * TRUSTING that identical text implies identical spans. It does, but a tool
    that assumes it proves nothing, and the whole value here is that the
    transfer is proved rather than believed.

WHAT IT ACTUALLY DOES. Nothing is retyped and nothing is copied blind. For each
candidate pair it reconstructs a page spec FROM THE SOURCE PAGE, regenerates
the source segment through tools/gen_alignment_page.py, and requires the result
to be BYTE-IDENTICAL to the landed source block. That round trip is the proof
that the reconstruction is faithful; a reconstruction that cannot rebuild its
own source is not trusted to build anything else. Only then is the same spec
pointed at the target, where the generator -- which opens the spine itself and
refuses any span not present verbatim in the target row -- re-derives every
span against the target's own text. Rule 1 stays mechanically unreachable
because the generator is still the only thing that writes a page.

WHAT IT REFUSES.
  * a pair whose two rows are not byte-identical in BOTH columns;
  * a source page that cannot rebuild itself byte-for-byte (the legacy
    hand-built pages with a clause layer the current generator cannot emit --
    323 pairs, measured 2026-09-22 -- fall here, and they are reported, not
    quietly dropped);
  * a span that does not land verbatim in the target;
  * a target body that differs from the source body once the labels and the
    segment heading are rewritten.
Every refusal is printed with the pair it belongs to. None is rounded away.

PROVENANCE. A transferred segment carries, in the page, an HTML marker naming
the source segment and the source page, and a visible first sentence in its AI
NOTE saying in plain English that no analyst read this segment. The marker is
what build_alignment_layer.py reads to stamp `xfer` on every span of that
segment in the bank, and `xrefs` on every evidence pair that cites it. So a
reader of the bank cannot mistake a transferred span for an independently read
one, and the mark is re-derived from the page on every build rather than
remembered.

ONE THING FOR ADAM TO RULE ON. The evidence layer ranks renderings by how many
segments attest them. A transferred citation is a real citation -- the text is
there -- but it is not independent evidence, because it is the same reading
counted twice. This tool therefore records `xrefs` and `n_read` alongside the
existing `refs`/`n`, and changes neither. Whether the card should rank on
`n_read` instead of `n` is a judgement about what a count means, not a
mechanical question, and it is left open deliberately.

Usage:
    transfer_duplicate_segments.py                  # measure and report
    transfer_duplicate_segments.py --plan out.json  # also write the plan
    transfer_duplicate_segments.py --write          # actually land the pages
"""
import argparse
import glob
import html
import io
import json
import os
import re
import sqlite3
import subprocess
import sys
from datetime import date

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B          # noqa: E402  (spine + COURSES)

GEN = os.path.join(ROOT, "tools", "gen_alignment_page.py")
WRAP = os.path.join(ROOT, "tools", "wrap_alignment_page.py")
ALIGN = os.path.join(ROOT, "data", "alignment")
REGISTRY = os.path.join(ALIGN, "transferred_pages.json")

# The mandate: C01-C18. C13 is under standing order not to be scanned until
# the upstream column offset is resolved (docs/upstream/C13_COLUMN_OFFSET.md),
# so it is neither a source nor a target here.
EXCLUDED = {"C13"}

MARK = ('<!-- TRANSFER v1 target="%s:%d" source="%s:%d" source_page="%s/%s" '
        'spans="%d" date="%s" tool="tools/transfer_duplicate_segments.py" -->')
MARK_RE = re.compile(
    r'<!-- TRANSFER v1 target="([A-Za-z0-9]+):(\d+)" '
    r'source="([A-Za-z0-9]+):(\d+)" source_page="([^"]+)" '
    r'spans="(\d+)" date="([^"]*)" tool="[^"]*" -->')

NOTE_PREFIX = (
    '<b>TRANSFERRED &mdash; NOT INDEPENDENTLY READ.</b> %s:%d is byte-identical '
    'to %s:%d, in the Tibetan and in Geshe Michael&#x27;s English alike. Every '
    'span below was carried over verbatim from the reading of %s:%d on page %s; '
    'no analyst read this segment. ')

WORDCH = re.compile(r"[A-Za-z0-9ༀ-࿿]")
TAG = re.compile(r'<span\b([^>]*)>|</span>')


def die(msg):
    sys.exit("REFUSED: " + msg)


# ---------------------------------------------------------------- the spine

def mandate_courses():
    """Every C01-C18 course code the spine actually carries, less the excluded.

    Discovered, not declared: a course that appears in the spine appears here.
    The six alignment gates that were blind to Course 5 for weeks were blind
    because each carried its own written-down list (tools/alignment_page_dirs.py).
    """
    con = sqlite3.connect(B.SPINE)
    got = {r[0] for r in con.execute(
        "SELECT DISTINCT course FROM corpus_segments")}
    con.close()
    out = [c for c in sorted(got)
           if re.fullmatch(r"C\d\d", c) and 1 <= int(c[1:]) <= 18
           and c not in EXCLUDED]
    return out


def spine_rows(courses):
    con = sqlite3.connect(B.SPINE)
    rows = {}
    for c in courses:
        for q, w, e in con.execute(
                "SELECT seq, wylie, english FROM corpus_segments "
                "WHERE course=?", (c,)):
            rows[(c, q)] = (w, e)
    con.close()
    return rows


def landed_index():
    """(course, seq) -> (page id, page directory), from the builder's registry."""
    out = {}
    for course, cfg in sorted(B.COURSES.items()):
        for pg, seqs in cfg["pages"].items():
            for q in seqs:
                out[(course, q)] = (pg, cfg["dir"])
    return out


def duplicate_pairs(courses, rows, landed):
    """-> ([(target, source)], groups) over the mandated, not-yet-landed segments.

    A group is a set of segments whose (wylie, english) are byte-identical. Its
    representative is the first LANDED member, or, if none has been read yet,
    the first member in course order -- that one is the segment a human pass
    should read, and every other member of the group then transfers from it.
    """
    order = {c: i for i, c in enumerate(courses)}

    def ordk(k):
        return (order[k[0]], k[1])

    groups = {}
    for k in rows:
        groups.setdefault(rows[k], []).append(k)
    pairs, plan_groups = [], []
    for members in groups.values():
        if len(members) == 1:
            continue
        members = sorted(members, key=ordk)
        already = [k for k in members if k in landed]
        rep = sorted(already, key=ordk)[0] if already else members[0]
        targets = [k for k in members if k != rep and k not in landed]
        if not targets:
            continue
        plan_groups.append({"rep": rep, "rep_landed": rep in landed,
                            "targets": targets})
        pairs.extend((t, rep) for t in targets)
    return pairs, plan_groups


# ------------------------------------------------- reading a landed page back

def page_body(doc):
    """The generator's own output, cut back out of a wrapped page.

    wrap_alignment_page.py puts the body between the depth controls and the
    legend, both of which are fixed strings it writes itself.
    """
    # A transferred page opens each segment with its provenance stamp, so the
    # body does not necessarily start with <h2>. Allow the comment between.
    m = re.search(r'</div>\n(?=(?:<!-- TRANSFER[^>]*-->\n)?<h2>Segment)', doc)
    b = doc.find('\n<div class="legend">')
    if not m or b < 0:
        return None
    return doc[m.end():b]


def segment_blocks(body):
    """-> {seq: (seq, title, tib_block, eng_block, note, whole_block)}."""
    out = {}
    for part in re.split(r'(?=<h2>Segment )', body):
        # a following segment's provenance stamp trails this one's block
        part = re.sub(r'<!-- TRANSFER[^>]*-->\n?\s*$', "", part)
        m = re.match(r'<h2>Segment (\d+) &mdash; (.*?)</h2>\n'
                     r'<div class="pair">\n', part)
        if not m:
            continue
        seq = int(m.group(1))
        rest = part[m.end():]
        note = None
        nm = re.search(r'<div class="note"><b>AI NOTE</b> &mdash; (.*)</div>\s*$',
                       rest, re.S)
        if nm:
            note = nm.group(1)
            rest = rest[:nm.start()]
        rest = rest.rstrip()
        if rest.endswith('</div>'):
            rest = rest[:-len('</div>')].rstrip()
        j = rest.find('<div class="eng">')
        if j < 0:
            continue
        out[seq] = (seq, html.unescape(m.group(2)), rest[:j].rstrip(),
                    rest[j:], note, part.rstrip("\n"))
    return out


def parse_side(block):
    """-> (plain text, [(depth, label, class, start, end)]) for one page column.

    The offsets are into the plain text, which is why this does not reuse
    build_alignment_layer.spans_of: that one normalises whitespace, and a
    reconstruction that normalises whitespace cannot rebuild its own source.
    """
    block = re.sub(r'^\s*<div class="(?:tib|eng)"><div class="lab">.*?</div>\s*',
                   '', block, flags=re.S)
    block = re.sub(r'\s*</div>\s*$', '', block)
    out, stack, text, i = [], [], [], 0
    for m in TAG.finditer(block):
        text.append(html.unescape(block[i:m.start()]))
        pos = len("".join(text))
        i = m.end()
        if m.group(0).startswith("</"):
            if not stack:
                return None, None
            d, lab, cls, st = stack.pop()
            out.append((d, lab, cls, st, pos))
        else:
            attrs = m.group(1)
            dm = re.search(r'data-d="(\d)"', attrs)
            lm = re.search(r'data-l="([^"]*)"', attrs)
            cm = re.search(r'class="([^"]*)"', attrs)
            stack.append((int(dm.group(1)) if dm else None,
                          lm.group(1) if lm else None,
                          cm.group(1) if cm else "u", pos))
    text.append(html.unescape(block[i:]))
    if stack:
        return None, None
    out = [o for o in out if o[0] is not None and o[1] is not None]
    out.sort(key=lambda t: (t[3], -(t[4] - t[3])))
    return "".join(text), out


def needs_subword(text, piece, lo, hi):
    """Did this span deliberately sit INSIDE a larger word on the source page?

    gen_alignment_page.find_word refuses a flat span that matches inside a
    bigger word, unless the spec declares `subword`. The declaration is not
    recoverable from the markup directly, but the PLACEMENT is: a span whose
    own edge is alphanumeric and whose neighbour there is alphanumeric too is
    exactly the case the refusal exists for. Deriving it this way can be wrong
    in principle; the source round trip is what makes being wrong harmless,
    because a wrong flag cannot rebuild the source page.
    """
    if WORDCH.match(piece[0]) and lo > 0 and WORDCH.match(text[lo - 1]):
        return True
    if WORDCH.match(piece[-1]) and hi < len(text) and WORDCH.match(text[hi]):
        return True
    return False


def reconstruct(block):
    """A landed segment block -> the page spec that would rebuild it.

    Returns (segment spec, None) or (None, reason). The reasons are the
    interesting output: they name which landed pages the current generator
    cannot reproduce, which is a fact worth knowing quite apart from transfer.
    """
    seq, title, tib_block, eng_block, note, _whole = block
    tp, ts = parse_side(tib_block)
    ep, es = parse_side(eng_block)
    if tp is None or ep is None:
        return None, "span markup does not balance"
    pre = "s%d" % seq
    if not ts or ts[0][:2] != (1, pre + "S") or ts[0][3] != 0 \
            or ts[0][4] != len(tp):
        return None, ("the Tibetan column is not the generator's shape: its "
                      "outermost span is not the depth-1 sentence wrapper "
                      "over the whole row (a hand-built page, or one carrying "
                      "a clause layer the generator cannot emit)")
    if not es or es[0][:2] != (1, pre + "S"):
        return None, ("the English column is not the generator's shape: its "
                      "outermost span is not the depth-1 sentence wrapper")

    nul = {}
    for d, lab, body in re.findall(
            r'<span class="u(?:[^"]*)" data-d="(\d)" data-l="([^"]*)">'
            r'<span class="nul">(.*?)</span></span>', eng_block, re.S):
        nul[lab] = html.unescape(body)

    eng, order = {}, []
    for d, lab, cls, lo, hi in es[1:]:
        if lab in nul:
            continue
        frag = ep[lo:hi]
        if not frag:
            continue
        if lab in eng:
            return None, ("the English of span %s is discontinuous; the page "
                          "spec has one English string per span and cannot "
                          "express it" % lab)
        eng[lab] = (frag, lo, hi)
        order.append(lab)

    spans = []
    for d, lab, cls, lo, hi in ts[1:]:
        if not lab.startswith(pre):
            return None, "span label %r does not belong to segment %d" % (lab, seq)
        piece = tp[lo:hi]
        if not piece:
            return None, "span %s has no Tibetan text" % lab
        sp = {"id": lab[len(pre):], "d": d, "tib": piece}
        parts = cls.split()
        if parts[:1] == ["u"] and len(parts) > 1:
            sp["cls"] = " ".join(parts[1:])
        if lab in eng:
            sp["eng"] = eng[lab][0]
        else:
            sp["eng"] = None
            if lab in nul:
                sp["nul"] = nul[lab]
        sub = needs_subword(tp, piece, lo, hi)
        if sp["eng"] is not None:
            frag, elo, ehi = eng[lab]
            sub = sub or needs_subword(ep, frag, elo, ehi)
        if sub and d != 7:
            sp["subword"] = True
        spans.append(sp)
    if not spans:
        return None, "the segment carries no spans"

    depth = {sp["id"]: sp["d"] for sp in spans}
    eng_order = [lab[len(pre):] for lab in order]
    unknown = [x for x in eng_order if x not in depth]
    if unknown:
        return None, ("the English column carries span(s) the Tibetan column "
                      "does not: %s" % ", ".join(unknown))
    seg = {"seq": seq, "title": title, "spans": spans,
           "eng_order": [x for x in eng_order if depth[x] != 7]}
    if note:
        seg["note"] = note
    return seg, None


# --------------------------------------------------------------- the generator

def generate(course, seg):
    """Run the real generator on one segment. The exit code is the proof."""
    r = subprocess.run([sys.executable, GEN],
                       input=json.dumps({"course": course, "segments": [seg]},
                                        ensure_ascii=False),
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode != 0:
        return None, r.stderr.strip().replace("\n", " ")[:400]
    return r.stdout.rstrip("\n"), None


def pair_part(block):
    """The aligned columns of a segment block, without the AI NOTE.

    The note is the one part that legitimately differs between a source and its
    transfer -- the transfer's note opens by saying it is a transfer -- so the
    body comparison is made on everything above it.
    """
    i = block.find('<div class="note">')
    return block[:i] if i >= 0 else block


def prove(src, tgt, block, rows, source_page):
    """Reconstruct, rebuild the source, then build the target. All or nothing.

    Returns (target block, span count, None) or (None, 0, reason).
    """
    if rows[src] != rows[tgt]:
        return None, 0, ("the two rows are not byte-identical in both columns; "
                         "this pair is not a duplicate")
    seg, why = reconstruct(block)
    if why:
        return None, 0, why
    built, why = generate(src[0], seg)
    if why:
        return None, 0, "rebuilding the SOURCE segment was refused: " + why
    if pair_part(built) != pair_part(block[5]):
        return None, 0, ("the rebuilt source block is not byte-identical to "
                         "the landed page, so the reconstruction is not "
                         "faithful and nothing may be built from it")

    seg_t = dict(seg)
    seg_t["seq"] = tgt[1]
    seg_t["note"] = (NOTE_PREFIX % (tgt[0], tgt[1], src[0], src[1],
                                    src[0], src[1], source_page)
                     + (seg.get("note") or ""))
    out, why = generate(tgt[0], seg_t)
    if why:
        return None, 0, "building the TARGET segment was refused: " + why
    want = (pair_part(built)
            .replace('data-l="s%d' % src[1], 'data-l="s%d' % tgt[1])
            .replace('<h2>Segment %d ' % src[1], '<h2>Segment %d ' % tgt[1]))
    if pair_part(out) != want:
        return None, 0, ("the target block differs from the source block once "
                         "the labels are rewritten; the transfer is not "
                         "verbatim")
    mark = MARK % (tgt[0], tgt[1], src[0], src[1], src[0], source_page,
                   len(seg["spans"]), date.today())
    return mark + "\n" + out, len(seg["spans"]), None


# ------------------------------------------------------------------- reporting

def bucket(reason):
    """Collapse a refusal into the CLASS it belongs to.

    The generator names the exact span it refused, so counting raw messages
    gives one "class" per segment and hides the shape of the problem: 90
    classes, 88 of which were the same defect wearing a different Tibetan
    word. The message stays verbatim in the plan JSON and under --verbose;
    this is only for the count.
    """
    if "OUT OF ORDER" in reason:
        return ("the source page nests depth-5 spans inside a depth-2 clause "
                "span. The generator walks one forward-only cursor over every "
                "span of depth 1-6, so the clause consumes the text its own "
                "children need and the rebuild is refused. These are the "
                "hand-built pages from before the generator existed.")
    if reason.startswith("the English column carries span(s)"):
        return ("the source page's English column carries spans with no "
                "Tibetan counterpart (English-only apparatus and expansion "
                "labels). The page spec has no way to express them.")
    if reason.startswith("rebuilding the SOURCE segment was refused"):
        return "rebuilding the source segment was refused for another reason."
    if reason.startswith("the source page has no block"):
        return ("the source page has no block of its own for that segment: it "
                "shares one aligned block with a neighbour under a merged "
                "heading, a layout the generator never emits.")
    return reason


def measure(verbose=False):
    courses = mandate_courses()
    rows = spine_rows(courses)
    landed = landed_index()
    pairs, groups = duplicate_pairs(courses, rows, landed)

    mandated = len(rows)
    landed_n = sum(1 for k in rows if k in landed)
    remaining = mandated - landed_n

    print("MANDATE (discovered from the spine, C13 excluded by standing order)")
    print("  courses            %s" % " ".join(courses))
    print("  mandated segments  %d" % mandated)
    print("  already landed     %d" % landed_n)
    print("  remaining          %d" % remaining)
    print("  duplicate targets  %d" % len(pairs))
    print("  unique remaining   %d" % (remaining - len(pairs)))
    print()

    have = [(t, s) for t, s in pairs if s in landed]
    later = [(t, s) for t, s in pairs if s not in landed]
    print("Of the %d duplicate targets:" % len(pairs))
    print("  %d have a source that is landed TODAY" % len(have))
    print("  %d wait on their group representative, which is itself part of "
          "the %d unique segments still to be read"
          % (len(later), remaining - len(pairs)))
    print()

    cache, results, reasons = {}, [], {}
    spans_total = 0
    for tgt, src in have:
        pg, pdir = landed[src]
        path = os.path.join(pdir, pg + ".html")
        if path not in cache:
            doc = io.open(path, encoding="utf-8").read()
            body = page_body(doc)
            cache[path] = (segment_blocks(body) if body else {},
                           re.findall(r'<h2>Segments? [^<]*</h2>', doc))
        blocks, headings = cache[path]
        block = blocks.get(src[1])
        if block is None:
            merged = [h for h in headings if h.startswith("<h2>Segments ")]
            why = ("the source page has no block of its own for that segment: "
                   "it shares one aligned block with its neighbour under a "
                   "merged heading (%s), a layout the generator never emits"
                   % merged[0] if merged else
                   "the source page has no block for that segment in the "
                   "generator's layout")
            reasons.setdefault(bucket(why), []).append((tgt, src, why))
            continue
        out, n, why = prove(src, tgt, block, rows, pg)
        if why:
            reasons.setdefault(bucket(why), []).append((tgt, src, why))
            continue
        results.append({"target": list(tgt), "source": list(src),
                        "source_page": "%s/%s" % (src[0], pg), "spans": n})
        spans_total += n

    # A PAIR IS NOT A SOURCE SEGMENT, and the difference is not small: one
    # reading can be carried to several targets, so the 399 pairs proved today
    # come from 353 distinct source segments -- C05 alone supplies 233 pairs
    # from 187 segments. Adding the per-course PAIR counts and calling the
    # total a count of source segments is a figure nobody can re-derive, and it
    # was written that way once. Both are printed, per course, and neither is
    # ever called the other. Every number on this page comes back out of the
    # tool on the next run.
    by_src = {}
    for r in results:
        by_src.setdefault(r["source"][0], []).append(tuple(r["source"]))
    by_tgt = {}
    for r in results:
        by_tgt.setdefault(r["target"][0], []).append(tuple(r["target"]))
    print("PROVED TRANSFERABLE NOW: %d pair(s) of %d  (%d spans), carrying "
          "%d distinct source segment(s)"
          % (len(results), len(have), spans_total,
             len({tuple(r["source"]) for r in results})))
    for co in sorted(by_src):
        print("    source %s  %4d pair(s) from %4d distinct segment(s)"
              % (co, len(by_src[co]), len(set(by_src[co]))))
    for co in sorted(by_tgt):
        print("    target %s  %4d pair(s) to   %4d distinct segment(s)"
              % (co, len(by_tgt[co]), len(set(by_tgt[co]))))
    print("REFUSED: %d, in %d class(es)"
          % (sum(len(v) for v in reasons.values()), len(reasons)))
    for why, items in sorted(reasons.items(), key=lambda kv: -len(kv[1])):
        srcs = sorted({s for _t, s, _m in items})
        print("  %4d pair(s) from %d source segment(s)" % (len(items), len(srcs)))
        print("       %s" % why)
        print("       e.g. %s" % ", ".join("%s:%d<-%s:%d" % (t[0], t[1], s[0], s[1])
                                           for t, s, _m in items[:4]))
        if verbose:
            for t, s, msg in items:
                print("         %s:%d <- %s:%d  %s" % (t[0], t[1], s[0], s[1], msg))
    return {"courses": courses, "mandated": mandated, "landed": landed_n,
            "remaining": remaining, "duplicate_targets": len(pairs),
            "unique_remaining": remaining - len(pairs),
            # named so the two can never be read as each other
            "transferable_pairs": len(results),
            "transferable_distinct_sources":
                len({tuple(r["source"]) for r in results}),
            "transferable_pairs_by_source_course":
                {co: len(v) for co, v in sorted(by_src.items())},
            "transferable_distinct_sources_by_course":
                {co: len(set(v)) for co, v in sorted(by_src.items())},
            "transferable_now": results,
            "waiting_on_representative": [
                {"rep": list(g["rep"]), "targets": [list(t) for t in g["targets"]]}
                for g in groups if not g["rep_landed"]],
            "refused": [{"class": why,
                         "pairs": [[list(t), list(s), msg]
                                   for t, s, msg in items]}
                        for why, items in sorted(reasons.items(),
                                                 key=lambda kv: -len(kv[1]))]}


# ---------------------------------------------------------------- landing them

def write_pages(plan, per_page=3):
    """Write the transferred pages and register them, so coverage still holds.

    Registration goes into data/alignment/transferred_pages.json rather than
    into the COURSES literal in build_alignment_layer.py. Editing that literal
    from a script is how a page id gets mistyped at scale, and a sidecar the
    builder DISCOVERS cannot go stale the way a written-down list does.
    """
    by_course = {}
    for item in plan["transferable_now"]:
        by_course.setdefault(item["target"][0], []).append(item)
    registry = {}
    if os.path.exists(REGISTRY):
        registry = json.load(io.open(REGISTRY, encoding="utf-8"))
    courses = mandate_courses()
    rows = spine_rows(courses)
    landed = landed_index()
    written = 0
    for course, items in sorted(by_course.items()):
        items.sort(key=lambda i: i["target"][1])
        num = int(course[1:])
        # DISCOVERED, NOT COMPUTED. C02's pages live in data/alignment/pages,
        # not in pages_c02, so computing the name would put a C02 transfer in a
        # directory that course does not use -- and build_alignment_layer's
        # _merge_transferred then refuses the whole build on the directory
        # mismatch. Loud and safe, but it would have left C02 silently
        # un-transferable-to. Where a course already has a home, the transfer
        # goes to it; only a course with no landed pages at all gets a new one.
        cfg0 = B.COURSES.get(course)
        dirname = (os.path.basename(os.path.normpath(cfg0["dir"])) if cfg0
                   else "pages_c%02d" % num)
        outdir = os.path.join(ALIGN, dirname)
        os.makedirs(outdir, exist_ok=True)
        cfg = registry.setdefault(course, {"dir": dirname, "pages": {}})
        if cfg["dir"] != dirname:
            die("%s is already registered under directory %r"
                % (course, cfg["dir"]))
        # contiguous runs first, then chunks of per_page, so a transferred page
        # reads like any other campaign page
        runs, run = [], []
        for item in items:
            if run and item["target"][1] != run[-1]["target"][1] + 1:
                runs.append(run)
                run = []
            run.append(item)
            if len(run) == per_page:
                runs.append(run)
                run = []
        if run:
            runs.append(run)
        for k, run in enumerate(runs, 1):
            page = "c%dx%d" % (num, k)
            if page in cfg["pages"]:
                die("%s/%s is already registered; refusing to overwrite a "
                    "landed page" % (course, page))
            # and refuse before writing, not at the next build: a transferred
            # page that collides with a hand-registered one of the same name
            # would be written to disk first and rejected afterwards.
            if cfg0 and page in cfg0["pages"]:
                die("%s/%s is already registered in COURSES; refusing to write "
                    "a transferred page over a landed one" % (course, page))
            if os.path.exists(os.path.join(outdir, page + ".html")):
                die("%s already exists; refusing to overwrite it"
                    % os.path.relpath(os.path.join(outdir, page + ".html"),
                                      ROOT))
            bodies, seqs = [], []
            for item in run:
                src = tuple(item["source"])
                tgt = tuple(item["target"])
                pg, pdir = landed[src]
                doc = io.open(os.path.join(pdir, pg + ".html"),
                              encoding="utf-8").read()
                block = segment_blocks(page_body(doc)).get(src[1])
                out, n, why = prove(src, tgt, block, rows, pg)
                if why:
                    die("%s:%d <- %s:%d no longer proves: %s"
                        % (tgt[0], tgt[1], src[0], src[1], why))
                bodies.append(out)
                seqs.append(tgt[1])
            title = ("Transferred &mdash; segments %s"
                     % ", ".join(str(q) for q in seqs))
            sub = ("carried verbatim from %s"
                   % ", ".join("%s:%d" % (i["source"][0], i["source"][1])
                               for i in run))
            path = os.path.join(outdir, page + ".html")
            w = subprocess.run(
                [sys.executable, WRAP, path,
                 "C%d&middot;%s &mdash; %s" % (num, page.upper(), title),
                 sub, "-", "-", str(num)],
                input="\n".join(bodies) + "\n",
                capture_output=True, text=True, cwd=ROOT)
            if w.returncode != 0:
                die("wrapper exited %d: %s" % (w.returncode, w.stderr[:300]))
            cfg["pages"][page] = seqs
            written += 1
            print("  wrote %s (%d segment(s))"
                  % (os.path.relpath(path, ROOT), len(seqs)))
    with io.open(REGISTRY, "w", encoding="utf-8") as f:
        json.dump(registry, f, ensure_ascii=False, indent=1, sort_keys=True)
    print("  registered %d page(s) in %s"
          % (written, os.path.relpath(REGISTRY, ROOT)))
    print("  now run: python3 tools/build_alignment_layer.py")
    return written


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--plan", help="write the measured plan to this JSON file")
    ap.add_argument("--verbose", action="store_true",
                    help="list every refused pair, not a sample")
    ap.add_argument("--write", action="store_true",
                    help="actually write and register the transferred pages")
    a = ap.parse_args()
    plan = measure(verbose=a.verbose)
    if a.plan:
        with io.open(a.plan, "w", encoding="utf-8") as f:
            json.dump(plan, f, ensure_ascii=False, indent=1)
        print("\nplan -> %s" % a.plan)
    if a.write:
        print()
        write_pages(plan)
    else:
        print("\n(report only — nothing written. --write lands the pages.)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
