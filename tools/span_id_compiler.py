#!/usr/bin/env python3
"""span_id_compiler.py -- proposals name token POSITIONS; this slices the strings.

WHY THIS EXISTS. Propose and reconcile agents used to write every span as a
RETYPED STRING -- {"tib": "zhi gnas", "eng": "quietude"} -- and
gen_alignment_page.py re-read the segment from the spine and refused any span
that was not verbatim, out of forward order, or outside its parent. That made
a copying error HARMLESS, but not FREE: every refusal is a repair round that
re-sends the agent's whole context. Measured on the campaign: propose agents
ran the generator 4.7 times each and reconcilers 3.7 times, and 98.1% of all
tokens the campaign spends are input-side context re-transport. Two more
burdens fell on the reconciler by hand: `eng_order` (the span ids in ENGLISH
order, required whenever the English crosses the Tibetan) and `subword: true`
(required whenever an English span deliberately sits inside a larger word,
the n't of "doesn't" or the un of "unable").

THE DESIGN. Each segment's wylie and English are cut into numbered tokens with
exact character offsets into the UNMODIFIED spine text. A proposal names token
ranges -- "T3-T5", "E7" -- plus, only where a span is part of a token, the
exact piece in brackets: "T6[s]" (the agentive s of pas), "E9[n't]" (the n't
of doesn't), "T2-T3[bdag pa]" (a word whose last syllable carries a bound s).
This file slices the exact strings out of the spine and emits the spec schema
the generator already consumes. Because it knows the offsets, it DERIVES the
Tibetan order, `eng_order` and `subword` mechanically. Models are poor at
counting characters and good at citing numbered tokens, which is why the
interface is token ids and not offsets.

THE GENERATOR IS THE AUTHORITY. This file sits IN FRONT of
gen_alignment_page.py and never changes it. Every compile ends by running the
generator's own main() on the emitted spec -- imported, never reimplemented --
and reading its HTML back. That is the one invariant everything hangs on,
ROUND-TRIP IDENTITY: every span must come back at EXACTLY the character
offsets the proposal chose, on both sides. It is where the danger lives. The
English word "the" may occur three times and a proposal may pick the second;
the generator, fed the string "the", walks a forward cursor and may land on
the first. A compiler that emitted that spec would produce a page which passes
every downstream gate while banking the wrong occurrence -- worse than no
compiler at all. So when the spec cannot be made to mean what was proposed,
this REFUSES BY NAME. It never emits and hopes.

THE SOURCE HASH. Token numbers are meaningful only for one exact text under
one exact tokenizer. A proposal echoes the segment's `src` hash, and a stale
one is refused by name, so a proposal written against yesterday's text (or
yesterday's tokenizer) cannot compile against today's.

THE BATTERY (`battery`, and the ctest gate tools/test_span_id_compiler.py).
Every landed page is a real spec with a known resolution. Each landed segment
is reverse-compiled: its spans' offsets are read off the page, expressed as a
token proposal, compiled, and regenerated -- and the regenerated Tibetan and
English blocks must be BYTE-IDENTICAL to the blocks on the landed page. That
comparison is against an artefact produced before this compiler existed, so it
is an independent recount, not the generator agreeing with itself. It proves
the compiler LOSES NOTHING. It says nothing about whether any alignment is
CORRECT, and must never be described as if it did: it tests identity, not
truth, which is why it is not a circular battery.

Nothing here calls a model or leaves this machine.

Usage:
  span_id_compiler.py packet C05:169             # what an agent is shown
  span_id_compiler.py compile prop.json [-o spec.json]
  span_id_compiler.py reverse C05:169            # the proposal a landed segment compiles from
  span_id_compiler.py battery [--verbose] [--write-baseline]
  span_id_compiler.py measure                    # packet size vs raw segment, whole bank
"""
import argparse
import copy
import glob
import hashlib
import html
import io
import json
import os
import re
import sqlite3
import sys
import unicodedata

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import gen_alignment_page as G                      # noqa: E402  the authority
from alignment_page_dirs import page_dirs, page_files  # noqa: E402  discovered, never declared

ALIGN = os.path.join(ROOT, "data", "alignment")
BASELINE = os.path.join(ROOT, "tools", "span_id_compiler_baseline.json")

# The tokenizer's name is hashed into every segment's `src`. If the rules below
# change, token numbers change meaning, and every outstanding proposal must be
# refused as stale rather than silently compiled against a different cut.
TOKENIZER = "span-id-tok/1"
SRC_SHOWN = 16          # hex digits of the sha256 an agent echoes


# --------------------------------------------------------------------------
# Refusals

class Refusal(Exception):
    """A named refusal. `code` is stable and machine-readable; the message
    names the span and says what to change. Never a crash."""

    def __init__(self, code, msg, seq=None, span=None):
        Exception.__init__(self, msg)
        self.code, self.msg, self.seq, self.span = code, msg, seq, span

    def __str__(self):
        where = ""
        if self.seq is not None:
            where = "s%s" % self.seq
            if self.span:
                where += " span %s" % self.span
            where += ": "
        return "REFUSED %s: %s%s" % (self.code, where, self.msg)


# --------------------------------------------------------------------------
# Tokenizer

def _tokch(c):
    """A character that belongs inside a token.

    A superset of the generator's WORDCH ([A-Za-z0-9] and Tibetan), so a
    token boundary that has no whitespace at it always has a non-WORDCH
    character on one side. That is what guarantees a plain token range can
    never cut what the generator calls a word -- a word can only be cut by
    the explicit bracket form, and the bracket form IS the declaration of
    intent the generator's `subword` flag stands for.
    """
    return (c.isalnum() or "\u0F00" <= c <= "\u0FFF"
            or unicodedata.category(c).startswith("M"))


def tokenize(text, side):
    """Cut `text` into [(start, end)] tokens; whitespace is never inside one.

    TIBETAN: a token is an optional leading apostrophe plus a run of letters,
    so `pa'i` is `pa` + `'i` and `'di'i` is `'di` + `'i`. The genitive, the
    'o, the 'am and the 'ang are then whole tokens. That split is where the
    landed bank cuts syllables most often -- `mdo sde pa` spanned out of
    `mdo sde pa'i` -- and without it every such span would need a bracket.
    An a-chung that starts a syllable ('gro) stays with it. Every other mark
    (, / - . [ ] and the rest) is a token of its own. The battery reports
    how many landed span sides are plain ranges and how many need a bracket,
    so the choice is re-measured on every run rather than asserted here.

    ENGLISH: a token is a run of letters, joined across an apostrophe that
    sits between letters, so "doesn't" and "Buddha's" are one token each and
    the negative affix is claimed with a bracket (E9[n't]) -- the explicit
    declaration the generator's `subword` flag exists to demand. Hyphens and
    all other marks are tokens of their own, so "self" in "self-grasping"
    is a whole token and cuts nothing.
    """
    toks, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c.isspace():
            i += 1
            continue
        j = i
        if side == "tib" and c == "'" and i + 1 < n and _tokch(text[i + 1]):
            j = i + 1
        if _tokch(text[j]):
            while j < n and _tokch(text[j]):
                j += 1
            if side == "eng":
                while (j + 1 < n and text[j] in "'\u2019"
                       and _tokch(text[j + 1])):
                    j += 1
                    while j < n and _tokch(text[j]):
                        j += 1
            toks.append((i, j))
            i = j
        else:
            toks.append((i, i + 1))
            i += 1
    return toks


def source_hash(wylie, english):
    """sha256 over the tokenizer's name and the exact spine text, both sides.

    The tokenizer name is inside the hash because a token number is only
    meaningful under the rules that numbered it."""
    h = hashlib.sha256()
    h.update(("%s\0%s\0%s" % (TOKENIZER, wylie, english)).encode("utf-8"))
    return h.hexdigest()


# --------------------------------------------------------------------------
# The spine

_DB = None


def spine_row(course, seq):
    """(wylie, english) straight from the spine the generator reads."""
    global _DB
    if _DB is None:
        _DB = sqlite3.connect(G.B.SPINE)
    row = _DB.execute("select wylie, english from corpus_segments "
                      "where course=? and seq=?", (course, seq)).fetchone()
    if not row:
        return None
    return row[0] or "", row[1] or ""


# --------------------------------------------------------------------------
# Packets: what an agent is shown

FORMAT_NOTE = (
    "PROPOSAL FORMAT. Name positions, never retype text. Write JSON "
    '{"course":C,"segments":[{"seq":N,"src":"<the src above>","title":"...",'
    '"note":"...","spans":[{"id":"w1","d":5,"t":"T3-T4","e":"E7"}, ...]}]}. '
    '"t" and "e" are token ranges from the numbered lines above: "T3" is one '
    'token, "T3-T5" runs from the start of T3 to the end of T5. To claim only '
    'PART of the tokens -- a bound morpheme, the n\'t of doesn\'t, the un of '
    'unable, a word whose last syllable carries a bound s -- add the exact '
    'part in brackets: "T6[s]", "E9[n\'t]", "T2-T3[bdag pa]"; it must occur '
    'exactly once in those tokens and touch the first and the last. A '
    'morpheme with no English exponent gets "e": null (optional "nul": why, '
    'and "cls"). ids are a letter then letters/digits, unique in the segment. '
    'd=7 members need no parent key: the depth-5 span whose Tibetan contains '
    'them is their parent. List spans in any order; Tibetan order, eng_order '
    'and subword are derived. Do not supply tib, eng, eng_order or subword.')


def render_tokens(text, toks):
    """The text with a [n] marker in front of every token.

    The source spacing is kept, so `pa'i` reads as `[6]pa[7]'i` and the
    reader sees both the word and its cut."""
    out, cur = [], 0
    for n, (a, b) in enumerate(toks, 1):
        if a > cur:
            out.append(" ")
        out.append("[%d]%s" % (n, text[a:b]))
        cur = b
    return "".join(out)


def packet(course, seq, with_format=True):
    row = spine_row(course, seq)
    if row is None:
        raise Refusal("NOT_IN_SPINE", "%s:%s is not in the spine" % (course, seq))
    wyl, eng = row
    tt, et = tokenize(wyl, "tib"), tokenize(eng, "eng")
    lines = [
        "SEGMENT %s:%d  src=%s  (%s)" % (course, seq,
                                        source_hash(wyl, eng)[:SRC_SHOWN],
                                        TOKENIZER),
        "TIBETAN (wylie), tokens T1-T%d:" % len(tt),
        render_tokens(wyl, tt),
        "ENGLISH (Geshe Michael, verbatim), tokens E1-E%d:" % len(et),
        render_tokens(eng, et),
    ]
    if with_format:
        lines += ["", FORMAT_NOTE]
    return "\n".join(lines) + "\n"


# --------------------------------------------------------------------------
# Token references

REF = re.compile(r"^\s*([TE])(\d+)(?:\s*-\s*([TE])?(\d+))?\s*(?:\[(.*)\])?\s*$",
                 re.S)
ID_OK = re.compile(r"^[A-Za-z][A-Za-z0-9_]*$")
SPAN_KEYS = {"id", "d", "t", "e", "cls", "nul"}
SEG_KEYS = {"seq", "src", "title", "note", "spans"}
DERIVED = {"tib": "the Tibetan string is sliced from the spine; give \"t\"",
           "eng": "the English string is sliced from the spine; give \"e\"",
           "subword": "derived from the offsets",
           "eng_order": "derived from the offsets",
           "clauses": "not supported; the generator does not render them"}
CLS_OK = re.compile(r'^[^"<>&]*$')


def _show(text, toks, a, b, letter):
    """`T4-T6 'sde pa'` -- a range named the way a proposal names it."""
    ids = [n for n, (x, y) in enumerate(toks, 1) if x < b and y > a]
    if not ids:
        return "%r" % text[a:b]
    rng = "%s%d" % (letter, ids[0]) if len(ids) == 1 else \
        "%s%d-%s%d" % (letter, ids[0], letter, ids[-1])
    x, y = toks[ids[0] - 1][0], toks[ids[-1] - 1][1]
    if (x, y) != (a, b):
        rng += "[%s]" % text[a:b]
    return "%s %r" % (rng, text[a:b])


def resolve_ref(ref, text, toks, letter, seq, sid):
    """A token reference -> (start, end) in the spine text, or a Refusal."""
    if not isinstance(ref, str):
        raise Refusal("BAD_REF", "%r is not a token reference; write e.g. "
                      "\"%s3\" or \"%s3-%s5\"" % (ref, letter, letter, letter),
                      seq, sid)
    m = REF.match(ref)
    if not m:
        raise Refusal("BAD_REF", "%r is not a token reference; write e.g. "
                      "\"%s3\", \"%s3-%s5\" or \"%s3[s]\""
                      % (ref, letter, letter, letter, letter), seq, sid)
    l1, n1, l2, n2, piece = m.groups()
    if l1 != letter or (l2 and l2 != letter):
        raise Refusal("WRONG_SIDE", "%r names %s tokens in the %s field; the "
                      "Tibetan field \"t\" takes T tokens and the English "
                      "field \"e\" takes E tokens"
                      % (ref, "English" if letter == "T" else "Tibetan",
                         "Tibetan" if letter == "T" else "English"),
                      seq, sid)
    a = int(n1)
    b = int(n2) if n2 is not None else a
    if a < 1 or a > len(toks):
        raise Refusal("UNKNOWN_TOKEN", "%r: there is no token %s%d; this "
                      "side has %s1-%s%d" % (ref, letter, a, letter, letter,
                                             len(toks)), seq, sid)
    if b < a:
        raise Refusal("REVERSED_RANGE", "%r runs backwards; write %s%d-%s%d"
                      % (ref, letter, b, letter, a), seq, sid)
    if b > len(toks):
        raise Refusal("RANGE_OUT_OF_BOUNDS", "%r ends past the last token; "
                      "this side has %s1-%s%d" % (ref, letter, letter,
                                                  len(toks)), seq, sid)
    lo, hi = toks[a - 1][0], toks[b - 1][1]
    if piece is None:
        return lo, hi
    whole = text[lo:hi]
    if piece == "":
        raise Refusal("BAD_REF", "%r has empty brackets; drop them to claim "
                      "the whole range" % ref, seq, sid)
    k = whole.find(piece)
    if k < 0:
        raise Refusal("SUBSTRING_ABSENT", "%r: %r does not occur in %s%s "
                      "%r. Copy the part exactly as the tokens have it."
                      % (ref, piece, letter,
                         n1 if a == b else "%d-%s%d" % (a, letter, b), whole),
                      seq, sid)
    if whole.find(piece, k + 1) >= 0:
        raise Refusal("SUBSTRING_NOT_UNIQUE", "%r: %r occurs %d times in %r, "
                      "so which one is meant cannot be determined. Claim a "
                      "longer part that occurs once, or the whole token."
                      % (ref, piece, whole.count(piece), whole), seq, sid)
    s, e = lo + k, lo + k + len(piece)
    if not (s < toks[a - 1][1] and e > toks[b - 1][0]):
        tight = _show(text, toks, s, e, letter).split(" ")[0]
        raise Refusal("RANGE_NOT_TIGHT", "%r: the bracketed part does not "
                      "touch both ends of the range; name only the tokens it "
                      "lies in: %s" % (ref, tight), seq, sid)
    return s, e


# --------------------------------------------------------------------------
# Compiling

def _cut(text, i):
    """True if offset i lies between two generator word characters."""
    return (0 < i < len(text) and bool(G.WORDCH.match(text[i - 1]))
            and bool(G.WORDCH.match(text[i])))


def compile_segment(course, seg):
    """One proposal segment -> (spec segment, chosen offsets). Refuses by name.

    `chosen` maps span id -> {"tib": (a, b), "eng": (c, d) or None}, which is
    what the round trip holds the generator to."""
    if not isinstance(seg, dict):
        raise Refusal("BAD_PROPOSAL", "a segment must be a JSON object")
    seq = seg.get("seq")
    if not isinstance(seq, int) or isinstance(seq, bool):
        raise Refusal("BAD_PROPOSAL", "segment has no integer \"seq\"")
    for k in seg:
        if k in DERIVED:
            raise Refusal("DERIVED_KEY_SUPPLIED", "remove %r: %s"
                          % (k, DERIVED[k]), seq)
        if k not in SEG_KEYS:
            raise Refusal("UNKNOWN_KEY", "segment key %r is not part of the "
                          "proposal format (%s)" % (k, ", ".join(sorted(SEG_KEYS))),
                          seq)
    row = spine_row(course, seq)
    if row is None:
        raise Refusal("NOT_IN_SPINE", "%s:%d is not in the spine" % (course, seq),
                      seq)
    wyl, eng = row
    full = source_hash(wyl, eng)
    src = seg.get("src")
    if not src:
        raise Refusal("MISSING_HASH", "the proposal must echo the segment's "
                      "src hash from its packet; token numbers mean nothing "
                      "without it", seq)
    src = str(src).strip().lower()
    if len(src) < SRC_SHOWN or not full.startswith(src):
        raise Refusal("STALE_HASH", "src %r does not match this segment's "
                      "text under %s (current src=%s). The proposal was "
                      "written against a different text or tokenizer; "
                      "re-read the packet and re-propose."
                      % (seg.get("src"), TOKENIZER, full[:SRC_SHOWN]), seq)
    title = seg.get("title")
    if not isinstance(title, str) or not title.strip():
        raise Refusal("MISSING_TITLE", "give the segment a short lowercase "
                      "\"title\"; the generator requires one", seq)
    if seg.get("note") is not None and not isinstance(seg["note"], str):
        raise Refusal("BAD_NOTE", "\"note\" must be text", seq)
    spans = seg.get("spans")
    if not isinstance(spans, list):
        raise Refusal("NO_SPANS", "\"spans\" must be a list (it may be empty: "
                      "a segment that claims nothing is a legitimate page)", seq)

    tt, et = tokenize(wyl, "tib"), tokenize(eng, "eng")
    got, ids = [], set()
    for k, sp in enumerate(spans):
        if not isinstance(sp, dict):
            raise Refusal("BAD_SPAN", "span #%d is not an object" % (k + 1), seq)
        sid = sp.get("id")
        if not isinstance(sid, str) or not ID_OK.match(sid) or sid == "S":
            raise Refusal("BAD_ID", "span #%d id %r: an id is a letter followed "
                          "by letters, digits or _ (and not \"S\", which is the "
                          "sentence wrapper). A hyphen would turn the span into "
                          "apparatus the builder drops; a leading digit would "
                          "collide with the segment number in the page label."
                          % (k + 1, sid), seq)
        if sid in ids:
            raise Refusal("DUPLICATE_ID", "id %r is used twice" % sid, seq, sid)
        ids.add(sid)
        for key in sp:
            if key in DERIVED:
                raise Refusal("DERIVED_KEY_SUPPLIED", "remove %r: %s"
                              % (key, DERIVED[key]), seq, sid)
            if key not in SPAN_KEYS:
                raise Refusal("UNKNOWN_KEY", "span key %r is not part of the "
                              "proposal format (%s)"
                              % (key, ", ".join(sorted(SPAN_KEYS))), seq, sid)
        d = sp.get("d")
        if not isinstance(d, int) or isinstance(d, bool) or not 1 <= d <= 7:
            raise Refusal("BAD_DEPTH", "\"d\" must be an integer 1-7, got %r"
                          % (d,), seq, sid)
        if sp.get("t") is None:
            raise Refusal("NO_TIBETAN", "every span names its Tibetan with "
                          "\"t\". English with no Tibetan under it stays "
                          "unwrapped (rule 7).", seq, sid)
        if sp.get("e") is not None and "nul" in sp:
            raise Refusal("NUL_WITH_ENGLISH", "has an English range %r and a "
                          "\"nul\" reason. A null span has no English: drop "
                          "\"e\" (make it null) or drop \"nul\"."
                          % (sp.get("e"),), seq, sid)
        cls = sp.get("cls")
        if cls is not None and (not isinstance(cls, str) or not CLS_OK.match(cls)):
            raise Refusal("BAD_CLS", "\"cls\" %r must be plain text without "
                          "quotes or angle brackets; it is written into an "
                          "HTML attribute" % (cls,), seq, sid)
        nul = sp.get("nul")
        if nul is not None and not isinstance(nul, str):
            raise Refusal("BAD_NUL", "\"nul\" must be text", seq, sid)
        ta = resolve_ref(sp["t"], wyl, tt, "T", seq, sid)
        ea = (resolve_ref(sp["e"], eng, et, "E", seq, sid)
              if sp.get("e") is not None else None)
        got.append({"id": sid, "d": d, "t": ta, "e": ea, "cls": cls,
                    "nul": nul, "k": k})

    def at(sp, side):
        a, b = sp[side]
        text, toks, L = (wyl, tt, "T") if side == "t" else (eng, et, "E")
        return "%s (%s)" % (sp["id"], _show(text, toks, a, b, L))

    # -- the flat layer: one forward cursor, so no overlap and no nesting
    flat = sorted([s for s in got if s["d"] != 7],
                  key=lambda s: (s["t"][0], -(s["t"][1] - s["t"][0]), s["k"]))
    for x, y in zip(flat, flat[1:]):
        if y["t"][0] < x["t"][1]:
            if y["t"][1] <= x["t"][1]:
                raise Refusal("FLAT_NESTED", "%s lies inside %s. Depth 1-6 "
                              "spans are walked by ONE forward cursor, so one "
                              "cannot contain another. If it is a member of a "
                              "depth-5 word make it d=7; otherwise drop one."
                              % (at(y, "t"), at(x, "t")), seq, y["id"])
            raise Refusal("FLAT_OVERLAP", "%s overlaps %s in the Tibetan. Two "
                          "spans cannot own the same characters."
                          % (at(y, "t"), at(x, "t")), seq, y["id"])

    # -- members: the depth-5 span whose Tibetan contains them is the parent
    kids = {}
    for m in [s for s in got if s["d"] == 7]:
        a, b = m["t"]
        over = [p for p in flat if p["t"][0] < b and a < p["t"][1]]
        inside = [p for p in over if p["t"][0] <= a and b <= p["t"][1]]
        if not inside:
            if over:
                raise Refusal("MEMBER_NOT_INSIDE_PARENT", "the d=7 member %s "
                              "crosses the edge of %s. A member must lie wholly "
                              "inside the depth-5 word it belongs to."
                              % (at(m, "t"), at(over[0], "t")), seq, m["id"])
            raise Refusal("D7_NO_PARENT", "the d=7 member %s lies inside no "
                          "depth-5 span. Add the word it belongs to at d=5, or "
                          "make this a d=5/d=6 span itself." % at(m, "t"),
                          seq, m["id"])
        p = inside[0]
        if p["d"] != 5:
            raise Refusal("D7_NO_PARENT", "the d=7 member %s lies inside %s, "
                          "which is depth %d. A member hangs off a depth-5 "
                          "word, never a clause or a particle."
                          % (at(m, "t"), at(p, "t"), p["d"]), seq, m["id"])
        if m["t"] == p["t"]:
            raise Refusal("MEMBER_NOT_TIGHTER", "the d=7 member %s is its "
                          "parent's ENTIRE Tibetan; a member must mark a "
                          "tighter boundary. Drop the member." % at(m, "t"),
                          seq, m["id"])
        ptxt, mtxt = wyl[p["t"][0]:p["t"][1]], wyl[a:b]
        if ptxt.count(mtxt) > 1:
            raise Refusal("MEMBER_NOT_UNIQUE_IN_PARENT", "the d=7 member %s "
                          "occurs %d times inside its parent %s. The generator "
                          "places a member by its text within the parent, so "
                          "which one is meant cannot be pinned. Split the "
                          "parent differently, or drop the member."
                          % (at(m, "t"), ptxt.count(mtxt), at(p, "t")),
                          seq, m["id"])
        if m["e"] is not None:
            if p["e"] is None:
                raise Refusal("MEMBER_ENGLISH_UNDER_NULL_PARENT", "the d=7 "
                              "member %s has English but its parent %s has "
                              "none. The generator renders a member's English "
                              "only inside its parent's, and would silently "
                              "DROP this one. Null the member, or give the "
                              "parent its English." % (at(m, "t"), p["id"]),
                              seq, m["id"])
            if not (p["e"][0] <= m["e"][0] and m["e"][1] <= p["e"][1]):
                raise Refusal("MEMBER_ENGLISH_OUTSIDE_PARENT", "the English of "
                              "the d=7 member %s is not inside its parent's "
                              "English %s. A member whose English is elsewhere "
                              "in the sentence is not a member: null it (rule 4)."
                              % (at(m, "e"), at(p, "e")), seq, m["id"])
            if m["e"] == p["e"]:
                raise Refusal("MEMBER_NOT_TIGHTER", "the English of the d=7 "
                              "member %s is its parent's entire English; a "
                              "member must mark a tighter boundary. Drop it or "
                              "null its English." % at(m, "e"), seq, m["id"])
        kids.setdefault(p["id"], []).append(m)
    for pid, ms in kids.items():
        ms.sort(key=lambda s: (s["t"][0], s["k"]))
        for x, y in zip(ms, ms[1:]):
            if y["t"][0] < x["t"][1]:
                raise Refusal("MEMBER_OVERLAP", "members %s and %s of %s "
                              "overlap in the Tibetan" % (at(x, "t"), at(y, "t"),
                                                          pid), seq, y["id"])
        me = sorted([s for s in ms if s["e"]], key=lambda s: s["e"][0])
        for x, y in zip(me, me[1:]):
            if y["e"][0] < x["e"][1]:
                raise Refusal("MEMBER_OVERLAP", "members %s and %s of %s "
                              "overlap in the English" % (at(x, "e"), at(y, "e"),
                                                          pid), seq, y["id"])

    # -- the English flat layer
    fe = sorted([s for s in flat if s["e"]],
                key=lambda s: (s["e"][0], -(s["e"][1] - s["e"][0]), s["k"]))
    for x, y in zip(fe, fe[1:]):
        if y["e"][0] < x["e"][1]:
            raise Refusal("ENGLISH_OVERLAP", "%s and %s overlap in the English. "
                          "Two spans cannot own the same characters, and "
                          "neither is a d=7 member of the other."
                          % (at(x, "e"), at(y, "e")), seq, y["id"])

    # -- emit, in Tibetan order, members straight after their parent
    order = []
    for p in flat:
        order.append(p)
        order.extend(kids.get(p["id"], []))
    out_spans, chosen = [], {}
    for s in order:
        o = {"id": s["id"], "d": s["d"],
             "tib": wyl[s["t"][0]:s["t"][1]],
             "eng": eng[s["e"][0]:s["e"][1]] if s["e"] else None}
        if s["cls"] is not None:
            o["cls"] = s["cls"]
        if s["e"] is None and s["nul"] is not None:
            o["nul"] = s["nul"]
        # subword is DERIVED: set exactly when the chosen offsets cut a word
        # the generator's boundary rule protects. It is shared by both sides
        # in the generator (one flag per span), so a cut on either side sets
        # it. Members ignore it -- they are searched inside their parent.
        if s["d"] != 7:
            cut = (_cut(wyl, s["t"][0]) or _cut(wyl, s["t"][1]) or
                   (s["e"] is not None and
                    (_cut(eng, s["e"][0]) or _cut(eng, s["e"][1]))))
            if cut:
                o["subword"] = True
        out_spans.append(o)
        chosen[s["id"]] = {"tib": s["t"], "eng": s["e"], "d": s["d"],
                           "cls": s["cls"],
                           "nul": o.get("nul") if s["e"] is None else None}
    spec = {"seq": seq, "title": title, "spans": out_spans}
    # eng_order is DERIVED from the English offsets and always emitted when
    # any flat span has English. Omitting it makes the generator derive the
    # order by COUNTING occurrences, which refuses any string that occurs more
    # often than it is claimed ("the" three times, one span); the explicit
    # order never does, and resolves the same list through the same cursor.
    if fe:
        spec["eng_order"] = [s["id"] for s in fe]
    if seg.get("note") is not None:
        spec["note"] = seg["note"]
    return spec, chosen


# --------------------------------------------------------------------------
# Reading generator output (and landed pages) back

PAIR = re.compile(
    r'<div class="pair">\n<div class="tib"><div class="lab">[^<]*</div>\n'
    r'(.*?)\n</div>\n<div class="eng"><div class="lab">[^<]*</div>\n'
    r'(.*?)\n</div>\n</div>', re.S)
TAGTEXT = re.compile(r"<[^>]*>|[^<]+")
UOPEN = re.compile(r'^<span class="(u(?: [^"]*)?)" data-d="(\d)" '
                   r'data-l="([^"]*)">$')


class Unparsed(Exception):
    pass


def walk(block):
    """A wrapped block -> (plain text, [span]) with exact offsets.

    Each span is a dict: label, d, cls (None for a bare "u"), a, b (offsets
    in the plain text) and nul (the text of a null marker, else None). Tags
    contribute nothing to the plain text and entities are decoded, which is
    how the builder reconstructs the spine text (strip_tags). Anything that is
    not a u-span, a nul marker, or text is refused rather than guessed at.
    """
    plain, pos, stack, out = [], 0, [], []
    for m in TAGTEXT.finditer(block):
        t = m.group(0)
        if not t.startswith("<"):
            txt = html.unescape(t)
            if stack and stack[-1] == "nul":
                owner = next((f for f in reversed(stack)
                              if isinstance(f, dict)), None)
                if owner is None:
                    raise Unparsed("null marker outside any span")
                owner["nul"] = (owner["nul"] or "") + txt
                continue
            plain.append(txt)
            pos += len(txt)
            continue
        if t == "</span>":
            if not stack:
                raise Unparsed("unbalanced </span>")
            f = stack.pop()
            if isinstance(f, dict):
                f["b"] = pos
                out.append(f)
            continue
        if t == '<span class="nul">':
            stack.append("nul")
            continue
        m2 = UOPEN.match(t)
        if not m2:
            raise Unparsed("unexpected tag %r" % t[:60])
        cls = m2.group(1)
        stack.append({"label": m2.group(3), "d": int(m2.group(2)),
                      "cls": cls[2:] if cls != "u" else None,
                      "a": pos, "b": None, "nul": None})
    if stack:
        raise Unparsed("unclosed span")
    return "".join(plain), out


def run_generator(course, spec_seg):
    """The generator's own main(), in-process, on one segment.

    Returns its stdout. Raises Refusal GENERATOR_REFUSED with the generator's
    own message when it refuses, and GENERATOR_CRASHED on anything else."""
    spec = {"course": course, "segments": [copy.deepcopy(spec_seg)]}
    old_in, old_out = sys.stdin, sys.stdout
    sys.stdin = io.StringIO(json.dumps(spec, ensure_ascii=False))
    sys.stdout = buf = io.StringIO()
    try:
        G.main()
    except SystemExit as e:
        msg = str(e.code) if e.code is not None else ""
        raise Refusal("GENERATOR_REFUSED", msg.replace("REFUSED: ", "", 1),
                      spec_seg.get("seq"))
    except Exception as e:   # the generator crashing is itself a finding
        raise Refusal("GENERATOR_CRASHED", "%s: %s" % (type(e).__name__, e),
                      spec_seg.get("seq"))
    finally:
        sys.stdin, sys.stdout = old_in, old_out
    return buf.getvalue()


def round_trip(course, spec_seg, chosen, wyl, eng):
    """Hold the generator to the chosen offsets. Returns (tib_block, eng_block).

    The spec is fed through the generator and its HTML read back. Every span
    must come back at exactly the chosen offsets on both sides, every null
    span must come back as a null marker and nothing else, and the plain
    text of both blocks must be the spine text. Anything else is a spec
    that MEANS something other than what was proposed, and is refused."""
    seq = spec_seg["seq"]
    flagged = {s["id"] for s in spec_seg["spans"] if s.get("subword")}

    def why(sid):
        # The generator keeps ONE subword flag per span and applies it to
        # both sides. A flag derived from an English cut (the n't of
        # doesn't) therefore also switches off the word-boundary check on
        # the Tibetan search, which can then stop inside an earlier word.
        if sid in flagged:
            return (" This span carries subword (derived because it cuts a "
                    "word), and the generator applies that flag to BOTH "
                    "sides, so neither side's search respects word "
                    "boundaries for it.")
        return ""
    body = run_generator(course, spec_seg)
    pairs = PAIR.findall(body)
    if len(pairs) != 1:
        raise Refusal("ROUND_TRIP_MISMATCH", "the generator's output did not "
                      "contain exactly one pair block", seq)
    tb, eb = pairs[0]
    try:
        tp, ts = walk(tb)
        ep, es = walk(eb)
    except Unparsed as e:
        raise Refusal("ROUND_TRIP_MISMATCH", "generator output unreadable: %s"
                      % e, seq)
    if tp != wyl or ep != eng:
        raise Refusal("ROUND_TRIP_MISMATCH", "the regenerated %s block is not "
                      "the spine text -- spans overlap in a way the generator "
                      "renders by duplicating text"
                      % ("Tibetan" if tp != wyl else "English"), seq)
    wrap = "s%dS" % seq
    tt, et = tokenize(wyl, "tib"), tokenize(eng, "eng")
    seen_t = {s["label"]: s for s in ts if s["label"] != wrap}
    seen_e = [s for s in es if s["label"] != wrap]
    seen_en = {s["label"]: s for s in seen_e if s["nul"] is None}
    seen_nul = {s["label"]: s for s in seen_e if s["nul"] is not None}
    if len(seen_t) != len([s for s in ts if s["label"] != wrap]) or \
            len(seen_en) + len(seen_nul) != len(seen_e):
        raise Refusal("ROUND_TRIP_MISMATCH", "a label was rendered twice", seq)
    want = set("s%d%s" % (seq, i) for i in chosen)
    if set(seen_t) != want:
        raise Refusal("ROUND_TRIP_MISMATCH", "Tibetan spans rendered %s, "
                      "proposed %s" % (sorted(set(seen_t) - want),
                                       sorted(want - set(seen_t))), seq)
    for sid, c in chosen.items():
        lab = "s%d%s" % (seq, sid)
        r = seen_t[lab]
        if (r["a"], r["b"]) != c["tib"]:
            raise Refusal(
                "ROUND_TRIP_MISMATCH",
                "the proposal chose Tibetan %s but the generator resolves the "
                "string to %s. It places a span by searching for its TEXT "
                "forward from the previous span, so it lands on the first "
                "occurrence it meets. Claim the text in between too, or choose "
                "that earlier occurrence.%s"
                % (_show(wyl, tt, c["tib"][0], c["tib"][1], "T"),
                   _show(wyl, tt, r["a"], r["b"], "T"), why(sid)), seq, sid)
        if r["d"] != c["d"] or r["cls"] != c["cls"]:
            raise Refusal("ROUND_TRIP_MISMATCH", "depth or class rendered as "
                          "%s/%s, proposed %s/%s" % (r["d"], r["cls"], c["d"],
                                                     c["cls"]), seq, sid)
        if c["eng"] is None:
            n = seen_nul.get(lab)
            if lab in seen_en or n is None:
                raise Refusal("ROUND_TRIP_MISMATCH", "the null span did not "
                              "come back as exactly one null marker", seq, sid)
            if n["d"] != c["d"]:
                raise Refusal("ROUND_TRIP_MISMATCH", "null marker depth %s, "
                              "proposed %s" % (n["d"], c["d"]), seq, sid)
            continue
        r = seen_en.get(lab)
        if r is None or lab in seen_nul:
            raise Refusal("ROUND_TRIP_MISMATCH", "the English %s did not come "
                          "back at all -- the generator dropped it"
                          % _show(eng, et, c["eng"][0], c["eng"][1], "E"),
                          seq, sid)
        if (r["a"], r["b"]) != c["eng"]:
            raise Refusal(
                "ROUND_TRIP_MISMATCH",
                "the proposal chose English %s but the generator resolves the "
                "string to %s. It places a span by searching for its TEXT "
                "forward from the previous English span, so it lands on the "
                "first occurrence it meets. Claim the text in between too, or "
                "choose that earlier occurrence.%s"
                % (_show(eng, et, c["eng"][0], c["eng"][1], "E"),
                   _show(eng, et, r["a"], r["b"], "E"), why(sid)), seq, sid)
    extra = (set(seen_en) | set(seen_nul)) - want
    if extra:
        raise Refusal("ROUND_TRIP_MISMATCH", "English spans rendered that were "
                      "not proposed: %s" % sorted(extra), seq)
    return tb, eb


def compile_proposal(prop):
    """A whole proposal file -> a generator spec. Refuses on the first error.

    Each segment is compiled and then round-tripped through the generator on
    its own, so a refusal names the segment it belongs to."""
    if not isinstance(prop, dict) or not isinstance(prop.get("course"), str):
        raise Refusal("BAD_PROPOSAL", "a proposal is {\"course\": ..., "
                      "\"segments\": [...]}")
    course = prop["course"]
    extra = set(prop) - {"course", "segments"}
    if extra:
        raise Refusal("UNKNOWN_KEY", "top-level key(s) %s are not part of the "
                      "proposal format" % sorted(extra))
    segs = prop.get("segments")
    if not isinstance(segs, list) or not segs:
        raise Refusal("BAD_PROPOSAL", "\"segments\" must be a non-empty list")
    out = []
    for seg in segs:
        try:
            spec, chosen = compile_segment(course, seg)
            wyl, eng = spine_row(course, spec["seq"])
            round_trip(course, spec, chosen, wyl, eng)
        except Refusal:
            raise
        except Exception as e:     # a compiler fault is refused, never raised
            raise Refusal("COMPILER_FAULT", "%s: %s" % (type(e).__name__, e),
                          seg.get("seq") if isinstance(seg, dict) else None)
        out.append(spec)
    return {"course": course, "segments": out}


# --------------------------------------------------------------------------
# The battery: reverse-compile every landed segment

def dir_courses():
    """page directory -> course, from the builder's own registry.

    The builder knows which directory holds which course (C02 lives in the
    historical `pages`). Directories are DISCOVERED on disk; one the registry
    cannot place is refused rather than skipped, because a skipped directory
    is how six gates missed Course 5."""
    m = {}
    for course, cfg in G.B.COURSES.items():
        m[os.path.basename(os.path.normpath(cfg["dir"]))] = course
    out = {}
    for d in page_dirs():
        if d not in m:
            raise SystemExit("REFUSED: page directory %s is on disk but the "
                             "builder registers no course for it" % d)
        out[d] = m[d]
    return out


H2 = re.compile(r"<h2>Segment (\d+) &mdash; (.*?)</h2>\s*$", re.S)


def page_segments(path):
    """Every pair block on a landed page: (seq, title, tib_block, eng_block,
    span_tag_count). Blocks the PAIR shape cannot read are returned with
    tib_block None so the census still accounts for them."""
    doc = io.open(path, encoding="utf-8").read()
    chunks = doc.split('<div class="pair">')
    out = []
    for k in range(1, len(chunks)):
        chunk = '<div class="pair">' + chunks[k]
        m = PAIR.match(chunk)
        before = chunks[k - 1]
        h = H2.search(before)
        title = h.group(2) if h else "untitled"
        if not m:
            out.append((None, title, None, None,
                        len(re.findall(r'<span class="u', chunk))))
            continue
        tb, eb = m.group(1), m.group(2)
        n = len(re.findall(r'<span class="u', tb)) + \
            len(re.findall(r'<span class="u', eb))
        mm = re.match(r'<span class="u" data-d="1" data-l="s(\d+)S">', tb)
        seq = int(mm.group(1)) if mm else (int(h.group(1)) if h else None)
        out.append((seq, title, tb, eb, n))
    return doc, out


class Inexpressible(Exception):
    def __init__(self, code, msg):
        Exception.__init__(self, msg)
        self.code, self.msg = code, msg


def express(text, toks, a, b, letter):
    """Offsets -> the token reference a proposal would write for them."""
    ids = [n for n, (x, y) in enumerate(toks, 1) if x < b and y > a]
    if not ids:
        raise Inexpressible("NO_TOKEN", "span %r covers no token" % text[a:b])
    x, y = toks[ids[0] - 1][0], toks[ids[-1] - 1][1]
    rng = "%s%d" % (letter, ids[0]) if len(ids) == 1 else \
        "%s%d-%s%d" % (letter, ids[0], letter, ids[-1])
    if (x, y) == (a, b):
        return rng, "plain"
    if a < x or b > y:
        raise Inexpressible("WHITESPACE_EDGE", "span %r begins or ends in "
                            "whitespace" % text[a:b])
    piece = text[a:b]
    if text[x:y].count(piece) != 1:
        raise Inexpressible("SUBSTRING_NOT_UNIQUE", "%r occurs %d times in %r"
                            % (piece, text[x:y].count(piece), text[x:y]))
    return "%s[%s]" % (rng, piece), "bracket"


def reverse(course, seq, title, tb, eb, wyl, eng):
    """A landed segment -> (proposal segment, per-side expressibility tally).

    The page's spans are read at their exact offsets and written back the way
    a proposal would name them. Structural shapes the proposal format cannot
    carry (English-only spans, a Tibetan span whose English vanished) raise
    Inexpressible with a named class."""
    tp, ts = walk(tb)
    ep, es = walk(eb)
    if tp != wyl or ep != eng:
        raise Inexpressible("PAGE_TEXT_NOT_SPINE",
                            "the page's %s text is not the spine's"
                            % ("Tibetan" if tp != wyl else "English"))
    wrap = "s%dS" % seq
    ts = [s for s in ts if not (s["label"] == wrap and s["d"] == 1)]
    es = [s for s in es if not (s["label"] == wrap and s["d"] == 1)]
    tt, et = tokenize(wyl, "tib"), tokenize(eng, "eng")
    pre = "s%d" % seq
    tib_by, eng_by, nul_by = {}, {}, {}
    for s in ts:
        if s["label"] in tib_by:
            raise Inexpressible("DUPLICATE_LABEL", "label %s twice in the "
                                "Tibetan" % s["label"])
        tib_by[s["label"]] = s
    for s in es:
        tgt = nul_by if (s["nul"] is not None and s["a"] == s["b"]) else eng_by
        if s["label"] in tgt or (s["label"] in nul_by and tgt is eng_by):
            raise Inexpressible("DUPLICATE_LABEL", "label %s twice in the "
                                "English" % s["label"])
        tgt[s["label"]] = s
    # SHAPE FIRST. A shape the generator cannot produce at all is the root
    # cause of everything else that goes wrong on a block, so it is named
    # before any symptom (a clause span's trailing space, its English-only
    # twin) can be reported in its place. The generator walks depth 1-6 with
    # ONE forward cursor, so no flat span may contain or overlap another, and
    # it resolves a depth-7 member only inside a depth-5 word.
    flat = sorted([s for s in ts if s["d"] != 7],
                  key=lambda s: (s["a"], -(s["b"] - s["a"])))
    for x, y in zip(flat, flat[1:]):
        if y["a"] < x["b"]:
            code = "FLAT_NESTED" if y["b"] <= x["b"] else "FLAT_OVERLAP"
            raise Inexpressible(code, "%s d=%d (%r) %s %s d=%d (%r)"
                                % (y["label"], y["d"], wyl[y["a"]:y["b"]][:40],
                                   "lies inside" if code == "FLAT_NESTED"
                                   else "overlaps", x["label"], x["d"],
                                   wyl[x["a"]:x["b"]][:40]))
    for m in [s for s in ts if s["d"] == 7]:
        if not any(p["d"] == 5 and p["a"] <= m["a"] and m["b"] <= p["b"]
                   for p in flat):
            raise Inexpressible("D7_NO_PARENT", "%s d=7 (%r) lies inside no "
                                "depth-5 span" % (m["label"],
                                                   wyl[m["a"]:m["b"]]))
    for lab in list(eng_by) + list(nul_by):
        if lab not in tib_by:
            raise Inexpressible("ENGLISH_ONLY_SPAN", "%s is wrapped in the "
                                "English with no Tibetan" % lab)
    spans, tally = [], {"plain": 0, "bracket": 0}
    for s in sorted(ts, key=lambda s: (s["a"], -(s["b"] - s["a"]))):
        lab = s["label"]
        if not lab.startswith(pre) or lab == pre:
            raise Inexpressible("LABEL_NOT_THIS_SEGMENT", "label %s does not "
                                "belong to segment %d" % (lab, seq))
        sid = lab[len(pre):]
        if s["a"] == s["b"]:
            raise Inexpressible("EMPTY_SPAN", "%s wraps no Tibetan" % lab)
        tref, kind = express(wyl, tt, s["a"], s["b"], "T")
        tally[kind] += 1
        o = {"id": sid, "d": s["d"], "t": tref}
        if s["cls"] is not None:
            o["cls"] = s["cls"]
        if lab in eng_by:
            e = eng_by[lab]
            if e["d"] != s["d"]:
                raise Inexpressible("DEPTH_DIFFERS_BETWEEN_SIDES", "%s is d=%d "
                                    "in the Tibetan, d=%d in the English"
                                    % (lab, s["d"], e["d"]))
            if e["cls"] != s["cls"]:
                raise Inexpressible("CLASS_DIFFERS_BETWEEN_SIDES", "%s has "
                                    "class %r / %r" % (lab, s["cls"], e["cls"]))
            eref, kind = express(eng, et, e["a"], e["b"], "E")
            tally[kind] += 1
            o["e"] = eref
        elif lab in nul_by:
            n = nul_by[lab]
            if n["d"] != s["d"]:
                raise Inexpressible("NULL_MARKER_DEPTH_DIFFERS", "%s is d=%d in "
                                    "the Tibetan, its null marker d=%d"
                                    % (lab, s["d"], n["d"]))
            if n["cls"] is not None:
                raise Inexpressible("NULL_MARKER_HAS_CLASS", "%s null marker "
                                    "carries class %r" % (lab, n["cls"]))
            o["e"] = None
            if n["nul"] != wyl[s["a"]:s["b"]] + "\u2192\u2205":
                o["nul"] = n["nul"]
        else:
            raise Inexpressible("NO_ENGLISH_NO_MARKER", "%s (%r) has neither "
                                "an English span nor a null marker"
                                % (lab, wyl[s["a"]:s["b"]]))
        spans.append(o)
    return ({"seq": seq, "src": source_hash(wyl, eng)[:SRC_SHOWN],
             "title": title, "spans": spans}, tally)


def span_census(tb, eb):
    """Independent recount: span tags in a block by regex, not by walk()."""
    return len(re.findall(r'<span class="u', tb)) + \
        len(re.findall(r'<span class="u', eb))


LABS = re.compile(r'<div class="tib"><div class="lab">([^<]*)</div>.*?'
                  r'<div class="eng"><div class="lab">([^<]*)</div>', re.S)

# The two hand-built protocols that open and punctuate every course, named by
# the labels their protocols prescribe (C03_CAMPAIGN.md, PHONETICS PROTOCOL
# rule 1; the E-071 corpus-marker protocol). Their blocks are built outside the
# generator BY DESIGN -- depth-7 sound-to-spelling links hung off a clause, an
# unpaired apparatus clause -- which the generator refuses by rule, so no
# proposal could ever produce them and no compiler should. They recur in every
# new course, which is why they are recognised by the label the page itself
# carries rather than frozen by name.
PROTOCOL_PREFIX = (("eng", "GESHE MICHAEL'S PHONETICS", "PHONETICS_PROTOCOL"),
                   ("tib", "CORPUS MARKER", "MARKER_PROTOCOL"))

# A protocol label excuses a hand-built SHAPE. It never excuses these, which
# would mean the compiler itself misbehaved on the block.
NEVER_EXCUSED = {"RENDER_DIFFERS", "COMPILER_FAULT", "GENERATOR_CRASHED"}


def generator_labels(course, seq):
    """The two pair labels the generator writes, read from its own output.

    Asked of the generator rather than copied here, so a relabelled generator
    cannot leave this battery classifying blocks by yesterday's labels."""
    out = run_generator(course, {"seq": seq, "title": "t", "spans": []})
    m = LABS.search(out)
    if not m:
        raise SystemExit("REFUSED: cannot read the generator's own pair labels")
    return m.group(1), m.group(2)


def block_kind(tlab, elab, gen):
    if (tlab, elab) == gen:
        return "GENERATOR"
    for side, prefix, kind in PROTOCOL_PREFIX:
        if (tlab if side == "tib" else elab).startswith(prefix):
            return kind
    return "OTHER_LABEL"


def battery():
    """Reverse-compile every landed segment. Returns a result dict.

    Two measurements, kept apart on purpose:
      * SPAN SIDES -- every span on every readable block whose text is the
        spine's, expressed as a token reference on its own, whatever the rest
        of its segment looks like. This is the token scheme's coverage.
      * BLOCKS -- whole segments reverse-compiled, compiled, regenerated, and
        compared byte for byte with the page. This is the round trip.
    """
    courses = dir_courses()
    res = {"pages": 0, "tags_on_pages": 0, "tags_in_pairs": 0, "pairs": 0,
           "wrappers": 0, "nul_markers": 0, "spec_spans_all": 0,
           "unreadable_tags": 0, "segments_ok": 0, "spans_ok": 0,
           "sides": {}, "side_examples": {}, "side_depths": {},
           "sides_unaddressable": 0, "blocks": {}, "classes": {},
           "census_errors": [], "tokenizer_errors": [], "gen_labels": None,
           "derived_crossing": 0, "derived_subword": 0}

    def tally_side(key, d, code, example):
        res["sides"][code] = res["sides"].get(code, 0) + 1
        if code not in ("plain", "bracket"):
            res["side_examples"].setdefault(code, "%s  %s" % (key, example))
            dd = res["side_depths"].setdefault(code, {})
            dd[str(d)] = dd.get(str(d), 0) + 1

    def record(key, kind, code, detail, nspec):
        res["blocks"][key] = {"kind": kind, "class": code, "spans": nspec}
        if code == "OK":
            return
        c = res["classes"].setdefault(code, {"segments": 0, "spans": 0,
                                             "kinds": {}, "example": None})
        c["segments"] += 1
        c["spans"] += nspec
        c["kinds"][kind] = c["kinds"].get(kind, 0) + 1
        if c["example"] is None:
            c["example"] = "%s  %s" % (key, detail[:150])

    for d, course in sorted(courses.items()):
        for path in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            res["pages"] += 1
            doc, segs = page_segments(path)
            res["tags_on_pages"] += len(re.findall(r'<span class="u', doc))
            page = os.path.basename(path)[:-5]
            chunks = doc.split('<div class="pair">')[1:]
            seen_here = {}
            for (seq, title, tb, eb, ntags), chunk in zip(segs, chunks):
                res["pairs"] += 1
                res["tags_in_pairs"] += ntags
                key = "%s/%s:%s" % (d, page, seq)
                if seq is not None:
                    seen_here[seq] = seen_here.get(seq, 0) + 1
                    if seen_here[seq] > 1:
                        key += "#%d" % seen_here[seq]
                if res["gen_labels"] is None and seq is not None:
                    res["gen_labels"] = generator_labels(course, seq)
                m = LABS.search('<div class="pair">' + chunk)
                kind = block_kind(m.group(1), m.group(2), res["gen_labels"]) \
                    if m else "OTHER_LABEL"
                if tb is None:
                    res["unreadable_tags"] += ntags
                    record(key, kind, "PAIR_BLOCK_UNREADABLE",
                           "pair block not in the generator's shape", ntags)
                    continue
                try:
                    _, ts = walk(tb)
                    _, es = walk(eb)
                except Unparsed as e:
                    res["unreadable_tags"] += ntags
                    record(key, kind, "PAIR_BLOCK_UNREADABLE", str(e), ntags)
                    continue
                # the independent recount: walk() must see every tag the
                # regex sees, or the battery is blind to part of the page
                if len(ts) + len(es) != span_census(tb, eb):
                    res["census_errors"].append(
                        "%s: walk saw %d spans, the page has %d span tags"
                        % (key, len(ts) + len(es), span_census(tb, eb)))
                wrap = "s%sS" % seq
                res["wrappers"] += sum(1 for s in ts + es if s["label"] == wrap)
                res["nul_markers"] += sum(1 for s in es if s["nul"] is not None)
                sides = [("tib", s) for s in ts if s["label"] != wrap] + \
                        [("eng", s) for s in es
                         if s["label"] != wrap and s["nul"] is None]
                nspec = len(sides)
                res["spec_spans_all"] += nspec
                row = spine_row(course, seq) if seq is not None else None
                if row is None:
                    res["sides_unaddressable"] += nspec
                    record(key, kind, "NOT_IN_SPINE", "no spine row", nspec)
                    continue
                wyl, eng = row
                for side, text in (("tib", wyl), ("eng", eng)):
                    err = check_tokens(text, side)
                    if err:
                        res["tokenizer_errors"].append("%s %s: %s"
                                                       % (key, side, err))
                tp, _ = walk(tb)
                ep, _ = walk(eb)
                if tp == wyl and ep == eng:
                    toks = {"tib": tokenize(wyl, "tib"),
                            "eng": tokenize(eng, "eng")}
                    for side, s in sides:
                        text = wyl if side == "tib" else eng
                        if s["a"] == s["b"]:
                            tally_side(key, s["d"], "EMPTY_SPAN", s["label"])
                            continue
                        try:
                            _, how = express(text, toks[side], s["a"], s["b"],
                                             "T" if side == "tib" else "E")
                            tally_side(key, s["d"], how, "")
                        except Inexpressible as e:
                            tally_side(key, s["d"], e.code,
                                       "%s d=%d %s" % (s["label"], s["d"], e.msg))
                else:
                    res["sides_unaddressable"] += nspec
                try:
                    prop, _t = reverse(course, seq, title, tb, eb, wyl, eng)
                except Inexpressible as e:
                    record(key, kind, e.code, e.msg, nspec)
                    continue
                try:
                    spec, chosen = compile_segment(course, prop)
                    rtb, reb = round_trip(course, spec, chosen, wyl, eng)
                except Refusal as e:
                    record(key, kind, e.code, str(e), nspec)
                    continue
                except Exception as e:     # never crash the battery
                    record(key, kind, "COMPILER_FAULT",
                           "%s: %s" % (type(e).__name__, e), nspec)
                    continue
                if (rtb, reb) != (tb, eb):
                    code = ("NULL_MARKER_ORDER"
                            if _without_nul_order(rtb, reb) ==
                            _without_nul_order(tb, eb) else "RENDER_DIFFERS")
                    record(key, kind, code, "compiled and regenerated; the "
                           "blocks differ from the page%s"
                           % (" only in the ORDER of the null markers, which "
                              "the generator always emits in Tibetan order"
                              if code == "NULL_MARKER_ORDER" else ""), nspec)
                    continue
                record(key, kind, "OK", "", nspec)
                res["segments_ok"] += 1
                res["spans_ok"] += nspec
                # what the reconciler no longer supplies by hand, counted on
                # blocks that round-tripped: an eng_order that differs from
                # the Tibetan order (a real crossing), and subword flags
                tib_order = [x["id"] for x in spec["spans"]
                             if x["d"] != 7 and x["eng"] is not None]
                if spec.get("eng_order") and spec["eng_order"] != tib_order:
                    res["derived_crossing"] += 1
                res["derived_subword"] += sum(1 for x in spec["spans"]
                                              if x.get("subword"))
    return res


NULMARK = re.compile(r'<span class="u" data-d="\d" data-l="[^"]*">'
                     r'<span class="nul">.*?</span></span>', re.S)


def _without_nul_order(tb, eb):
    """The blocks with their null markers pulled out and sorted."""
    marks = sorted(NULMARK.findall(eb))
    return tb, NULMARK.sub("", eb), marks


def check_tokens(text, side):
    """The tokenizer's contract, checked on real text rather than assumed.

    Tokens are in order, never overlap, never contain whitespace, cover every
    non-space character exactly once, and a boundary with no space at it
    never falls between two generator word characters."""
    toks = tokenize(text, side)
    cur = 0
    for a, b in toks:
        if a < cur or b <= a:
            return "token (%d,%d) out of order or empty" % (a, b)
        if text[cur:a].strip():
            return "characters %r between tokens are not covered" % text[cur:a]
        if any(c.isspace() for c in text[a:b]):
            return "token %r contains whitespace" % text[a:b]
        if a == cur and cur > 0 and _cut(text, a):
            return "adjacent tokens cut a word at %d (%r)" % (a, text[a - 2:a + 2])
        cur = b
    if text[cur:].strip():
        return "trailing characters %r are not covered" % text[cur:]
    return None


def load_baseline():
    if not os.path.exists(BASELINE):
        return None
    return json.load(io.open(BASELINE, encoding="utf-8"))


def _excused(blk):
    return (blk["kind"] in ("PHONETICS_PROTOCOL", "MARKER_PROTOCOL")
            and blk["class"] not in NEVER_EXCUSED)


def judge(res, base):
    """The gate's verdict. Returns a list of failure lines (empty = pass).

    NEW PAGES MUST ROUND-TRIP. A block that fails is excused only if (a) it
    carries one of the two hand-built protocol labels and fails with a
    structural class, or (b) it is named, with that exact class, in the
    frozen baseline of legacy blocks. The baseline may neither grow nor
    quietly shrink: a named block that starts passing is reported, and the
    baseline is re-derived with --write-baseline, never edited by hand. And
    the number of blocks that round-trip may not fall below the number the
    baseline measured -- a ratchet AT the measurement, never above it."""
    bad = []
    if base is None:
        return ["NO BASELINE at %s -- derive it with `battery --write-baseline`"
                % os.path.relpath(BASELINE, ROOT)]
    if base.get("tokenizer") != TOKENIZER:
        bad.append("the baseline was measured under %s, this is %s: re-derive "
                   "it" % (base.get("tokenizer"), TOKENIZER))
    # Independent recount of what exists, from a plain glob and a plain
    # string count, not from the walk the battery did.
    files = page_files()
    pairs = sum(io.open(f, encoding="utf-8").read().count('<div class="pair">')
                for f in files)
    if res["pages"] != len(files) or res["pairs"] != pairs:
        bad.append("BLIND: the battery read %d pages / %d pair blocks, but %d "
                   "pages / %d pair blocks exist" % (res["pages"], res["pairs"],
                                                     len(files), pairs))
    if res["census_errors"]:
        bad.append("CENSUS: the page reader is blind to %d block(s): %s"
                   % (len(res["census_errors"]), res["census_errors"][0]))
    if res["tokenizer_errors"]:
        bad.append("TOKENIZER CONTRACT broken on %d side(s): %s"
                   % (len(res["tokenizer_errors"]), res["tokenizer_errors"][0]))
    parts = (res["wrappers"] + res["nul_markers"] + res["spec_spans_all"] +
             res["unreadable_tags"])
    if res["tags_in_pairs"] != parts:
        bad.append("CENSUS: %d span tags in pair blocks, but wrappers %d + "
                   "null markers %d + spec spans %d + unreadable %d = %d"
                   % (res["tags_in_pairs"], res["wrappers"], res["nul_markers"],
                      res["spec_spans_all"], res["unreadable_tags"], parts))
    if res["segments_ok"] == 0:
        bad.append("NOTHING ROUND-TRIPPED -- a battery that examines nothing "
                   "is not a battery")
    if res["segments_ok"] < base.get("segments_ok", 0):
        bad.append("ROUND TRIP REGRESSED: %d blocks round-trip, the baseline "
                   "measured %d" % (res["segments_ok"], base["segments_ok"]))
    frozen = base.get("segments", {})
    now = {k: b["class"] for k, b in res["blocks"].items()
           if b["class"] != "OK" and not _excused(b)}
    new = sorted(k for k in now if k not in frozen)
    moved = sorted(k for k in now if k in frozen and frozen[k] != now[k])
    healed = sorted(k for k in frozen if k not in now)
    for k in new[:25]:
        bad.append("NEW FAILURE %s  %s (%s block)"
                   % (k, now[k], res["blocks"][k]["kind"]))
    if len(new) > 25:
        bad.append("... and %d more new failures" % (len(new) - 25))
    for k in moved[:25]:
        bad.append("CLASS CHANGED %s  %s -> %s" % (k, frozen[k], now[k]))
    for k in healed[:25]:
        bad.append("NO LONGER FAILS AS FROZEN (re-derive the baseline with "
                   "--write-baseline) %s  was %s, now %s"
                   % (k, frozen[k], res["blocks"].get(k, {}).get("class",
                                                                 "ABSENT")))
    return bad


def report(res, out=None):
    out = out or sys.stdout
    w = out.write
    sides = res["sides"]
    expr = sides.get("plain", 0) + sides.get("bracket", 0)
    allsides = sum(sides.values())
    w("span-id compiler battery (%s), whole bank, across %d pages\n"
      % (TOKENIZER, res["pages"]))
    w("  pages %d, pair blocks %d, span tags on pages %d (%d inside pair "
      "blocks)\n" % (res["pages"], res["pairs"], res["tags_on_pages"],
                     res["tags_in_pairs"]))
    w("  inside pair blocks: %d sentence wrappers (generated, not spans of a "
      "spec) + %d null markers (the English echo of a null span) + %d spec "
      "span sides + %d tags in unreadable blocks\n"
      % (res["wrappers"], res["nul_markers"], res["spec_spans_all"],
         res["unreadable_tags"]))
    w("\n  TOKEN-SCHEME COVERAGE, per span side, on blocks whose text is the "
      "spine's: %d of %d expressible (%.3f%%)\n"
      % (expr, allsides, 100.0 * expr / max(1, allsides)))
    w("    plain token range %d, bracketed part %d\n"
      % (sides.get("plain", 0), sides.get("bracket", 0)))
    for code in sorted(k for k in sides if k not in ("plain", "bracket")):
        w("    NOT EXPRESSIBLE %-22s %6d  by depth %s\n      e.g. %s\n"
          % (code, sides[code], json.dumps(res["side_depths"][code],
                                           sort_keys=True),
             res["side_examples"][code][:170]))
    w("    span sides on blocks whose text is NOT the spine's, or unreadable "
      "(no spine position exists to name): %d + %d tags\n"
      % (res["sides_unaddressable"], res["unreadable_tags"]))
    kinds = {}
    for b in res["blocks"].values():
        k = kinds.setdefault(b["kind"], [0, 0])
        k[0] += 1
        k[1] += b["class"] == "OK"
    w("\n  ROUND TRIP (reverse-compile, compile, regenerate, compare bytes): "
      "%d blocks, %d spec span sides byte-identical (%.2f%% of spec span "
      "sides)\n" % (res["segments_ok"], res["spans_ok"],
                    100.0 * res["spans_ok"] / max(1, res["spec_spans_all"])))
    for k in sorted(kinds):
        w("    %-20s %5d blocks, %5d round-trip\n" % (k, kinds[k][0],
                                                     kinds[k][1]))
    w("    derived by the compiler on those blocks: a crossing eng_order on "
      "%d, a subword flag on %d spans\n" % (res["derived_crossing"],
                                            res["derived_subword"]))
    if res["classes"]:
        w("  NOT round-tripped, by named class (blocks / spec span sides / "
          "block kinds):\n")
        for code, c in sorted(res["classes"].items(),
                              key=lambda kv: -kv[1]["spans"]):
            w("    %-32s %5d %7d  %s\n      e.g. %s\n"
              % (code, c["segments"], c["spans"],
                 json.dumps(c["kinds"], sort_keys=True), c["example"]))
    w("  census errors %d, tokenizer contract errors %d\n"
      % (len(res["census_errors"]), len(res["tokenizer_errors"])))


def write_baseline(res):
    frozen = {k: b["class"] for k, b in sorted(res["blocks"].items())
              if b["class"] != "OK" and not _excused(b)}
    data = {
        "_what": "Landed pair blocks the span-id compiler cannot reverse-compile "
                 "to a byte-identical page, frozen by name with their class, "
                 "plus the round-trip count measured in the same run. Written "
                 "by `tools/span_id_compiler.py battery --write-baseline`; "
                 "never edit by hand.",
        "_why": "New pages must round-trip. Legacy shapes (hand-built C01/C02 "
                "pages, early C03 pages) are named here so the gate holds "
                "them fixed: the set may neither grow nor quietly shrink. "
                "Blocks carrying a hand-built protocol label (phonetics, "
                "corpus marker) are excused by that label, not listed.",
        "tokenizer": TOKENIZER,
        "segments_ok": res["segments_ok"],
        "classes": {k: {"segments": v["segments"], "spans": v["spans"],
                        "kinds": v["kinds"]}
                    for k, v in sorted(res["classes"].items())},
        "segments": frozen,
    }
    io.open(BASELINE, "w", encoding="utf-8").write(
        json.dumps(data, indent=1, ensure_ascii=False, sort_keys=False) + "\n")


# --------------------------------------------------------------------------
# Measuring the packet

def measure():
    """Packet size against the raw segment, over every landed segment."""
    courses = dir_courses()
    rows, fixed = [], len(FORMAT_NOTE.encode("utf-8")) + 2
    seen = set()
    for d, course in sorted(courses.items()):
        for path in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            _, segs = page_segments(path)
            for seq, _t, tb, _e, _n in segs:
                if seq is None or (course, seq) in seen:
                    continue
                seen.add((course, seq))
                row = spine_row(course, seq)
                if row is None:
                    continue
                raw = len((row[0] + "\n" + row[1]).encode("utf-8"))
                pk = len(packet(course, seq, with_format=False).encode("utf-8"))
                rows.append((raw, pk))
    rows.sort(key=lambda r: r[1] / r[0])
    ratio = [pk / raw for raw, pk in rows]
    tr = sum(r for r, _ in rows)
    tp = sum(p for _, p in rows)
    print("packet size vs raw segment (wylie + newline + english), %d distinct "
          "landed segments" % len(rows))
    print("  raw bytes total %d, packet bytes total %d (without the format "
          "paragraph): x%.2f" % (tr, tp, tp / tr))
    print("  per segment ratio: median x%.2f, p10 x%.2f, p90 x%.2f"
          % (ratio[len(ratio) // 2], ratio[len(ratio) // 10],
             ratio[(9 * len(ratio)) // 10]))
    print("  the format paragraph adds a fixed %d bytes per packet" % fixed)


# --------------------------------------------------------------------------
# CLI

def _cs(arg):
    m = re.match(r"^([A-Za-z0-9]+):(\d+)$", arg)
    if not m:
        raise SystemExit("REFUSED: give a segment as COURSE:SEQ, e.g. C05:169")
    return m.group(1), int(m.group(2))


def find_landed(course, seq):
    for d, c in dir_courses().items():
        if c != course:
            continue
        for path in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            _, segs = page_segments(path)
            for s, title, tb, eb, _n in segs:
                if s == seq and tb is not None:
                    return path, title, tb, eb
    return None


def dump_proposal(prop):
    """JSON with one span per line: readable, and diffable span by span."""
    lines = ['{"course": %s, "segments": [' % json.dumps(prop["course"])]
    for i, seg in enumerate(prop["segments"]):
        head = {k: v for k, v in seg.items() if k != "spans"}
        lines.append(' {' + json.dumps(head, ensure_ascii=False)[1:-1] +
                     ', "spans": [')
        sp = seg.get("spans") or []
        for j, s in enumerate(sp):
            lines.append('   ' + json.dumps(s, ensure_ascii=False) +
                         ("," if j < len(sp) - 1 else ""))
        lines.append(' ]}' + ("," if i < len(prop["segments"]) - 1 else ""))
    lines.append(']}')
    return "\n".join(lines) + "\n"


def main(argv):
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    p = sub.add_parser("packet")
    p.add_argument("seg")
    p = sub.add_parser("compile")
    p.add_argument("proposal")
    p.add_argument("-o", "--out")
    p = sub.add_parser("reverse")
    p.add_argument("seg")
    p = sub.add_parser("battery")
    p.add_argument("--verbose", action="store_true")
    p.add_argument("--write-baseline", action="store_true")
    sub.add_parser("measure")
    a = ap.parse_args(argv)

    if a.cmd == "packet":
        course, seq = _cs(a.seg)
        try:
            sys.stdout.write(packet(course, seq))
        except Refusal as e:
            print(e, file=sys.stderr)
            return 1
        return 0
    if a.cmd == "compile":
        try:
            prop = json.load(io.open(a.proposal, encoding="utf-8"))
        except (OSError, ValueError) as e:
            print("REFUSED BAD_PROPOSAL: cannot read %s: %s" % (a.proposal, e),
                  file=sys.stderr)
            return 1
        try:
            spec = compile_proposal(prop)
        except Refusal as e:
            print(e, file=sys.stderr)
            return 1
        text = json.dumps(spec, ensure_ascii=False, indent=1) + "\n"
        if a.out:
            io.open(a.out, "w", encoding="utf-8").write(text)
            print("compiled %d segment(s), round-tripped through the generator "
                  "-> %s" % (len(spec["segments"]), a.out), file=sys.stderr)
        else:
            sys.stdout.write(text)
        return 0
    if a.cmd == "reverse":
        course, seq = _cs(a.seg)
        hit = find_landed(course, seq)
        if not hit:
            print("REFUSED: %s:%d is on no landed page" % (course, seq),
                  file=sys.stderr)
            return 1
        path, title, tb, eb = hit
        wyl, eng = spine_row(course, seq)
        try:
            prop, _ = reverse(course, seq, title, tb, eb, wyl, eng)
        except Inexpressible as e:
            print("INEXPRESSIBLE %s: %s" % (e.code, e.msg), file=sys.stderr)
            return 1
        sys.stdout.write(dump_proposal({"course": course, "segments": [prop]}))
        return 0
    if a.cmd == "battery":
        res = battery()
        report(res)
        if a.verbose:
            print("\n  every block that does not round-trip:")
            for k, b in sorted(res["blocks"].items()):
                if b["class"] != "OK":
                    print("    %-34s %-22s %s%s" % (k, b["kind"], b["class"],
                                                  "  (excused by protocol label)"
                                                  if _excused(b) else ""))
        if a.write_baseline:
            write_baseline(res)
            print("  baseline written: %s" % os.path.relpath(BASELINE, ROOT))
            return 0
        bad = judge(res, load_baseline())
        if bad:
            print("\nFAILED:")
            for b in bad:
                print("  " + b)
            return 1
        print("\n  every landed segment round-trips or is a named, frozen "
              "legacy class")
        return 0
    if a.cmd == "measure":
        measure()
        return 0
    ap.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
