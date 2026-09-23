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
exact piece in brackets: "E9[n't]" (the n't of doesn't), "E4[un]" (the un of
unable), "T2-T3[phyi ma]" (a word out of `phyi mar`, whose last syllable
carries the terminative r). A bracket is how a span declares that it cuts a
word, and the cuts it may make are the ones the campaign's rules allow and
nothing else: rule 8's negative-affix convention in the English, a Wylie
suffix split in the Tibetan, and never the agentive s (rule 9).
This file slices the exact strings out of the spine and emits the spec schema
the generator already consumes. Because it knows the offsets, it DERIVES the
Tibetan order, `eng_order` and `subword` mechanically. Models are poor at
counting characters and good at citing numbered tokens, which is why the
interface is token ids and not offsets.

WHY THE CUT RULES ARE CHECKED HERE. The generator refuses a span that cuts a
word unless the spec says `subword: true`, and that refusal is what catches
an ACCIDENTAL cut (`one-pointed` out of "one-pointedly"). This file derives
the flag from the offsets, so without its own check it would turn every
accidental bracket -- "Exemp" out of "Exemplist" -- into a declared one and
switch the generator's refusal off. So before deriving the flag it applies
the post-landing gates' own lists, imported rather than copied:
test_no_broken_words.ALLOWED_SUBWORD for the English and
test_no_split_syllables.SUFFIX / STEM_OK for the Tibetan.

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
yesterday's tokenizer) cannot compile against today's. That promise holds
only if every change to the tokenizer's behaviour also changes its NAME,
which is inside the hash. A name bump is a manual act, so the gate does not
trust it: the baseline pins the token boundaries of every landed segment and
of a synthetic canary, and a boundary that moves while the name stays put is
reported as a tokenizer changed without a bump.

THE WHOLE PAGE, NOT JUST THE PAIR BLOCK. The bank is built from the whole
page by build_alignment_layer.spans_of, and a title or a note is part of the
page. So after the per-segment round trip the whole emitted spec is run
through the generator once more and read back through the BUILDER's own
reader; what the builder would bank must be exactly what was proposed.

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
import ast
import copy
import glob
import hashlib
import html
import html.parser
import io
import json
import os
import re
import sqlite3
import sys
import tempfile
import unicodedata

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import gen_alignment_page as G                      # noqa: E402  the authority
from alignment_page_dirs import page_dirs, page_files  # noqa: E402  discovered, never declared
# The cut rules are the post-landing gates' own lists, imported so the
# compiler and the gates can never disagree about which cuts are legal. The
# gates themselves are imported too: the battery runs them, unmodified, over
# every page the sweeps compile, so "never disagree" is measured, not claimed.
import test_no_broken_words as NB                   # noqa: E402
import test_no_split_syllables as NS                # noqa: E402
from test_no_broken_words import ALLOWED_SUBWORD    # noqa: E402
from test_no_split_syllables import SUFFIX, STEM_OK  # noqa: E402

ALIGN = os.path.join(ROOT, "data", "alignment")
BASELINE = os.path.join(ROOT, "tools", "span_id_compiler_baseline.json")

# The tokenizer's name is hashed into every segment's `src`. If the rules below
# change, token numbers change meaning, and every outstanding proposal must be
# refused as stale rather than silently compiled against a different cut.
# /2 (2026-09-23): a syllable-final a-chung stays with its syllable (mtha').
TOKENIZER = "span-id-tok/2"
SRC_SHOWN = 16          # hex digits of the sha256 an agent echoes
SRC_FORM = re.compile(r"^[0-9a-f]{%d}$" % SRC_SHOWN)
_ALLOWED_LOWER = {w.lower() for w in ALLOWED_SUBWORD}
SHELL = os.path.join(ROOT, "tools", "wrap_alignment_page.py")


def page_classes():
    """(classes the page gives its own elements, classes its script acts on).

    Read out of the generator's and the page shell's own source, never
    listed here, because a list typed here goes stale the day the shell
    gains a class. A span's `cls` and a note's markup share the page with
    these: a span classed `nul` renders as a grey superscript null marker
    while carrying real English, one classed `pair` renders as a two-column
    grid (C05:255 banks exactly that), and a note element classed `u` or
    carrying a data-l attribute becomes a live linked span to the page's
    script, which selects '.u' and [data-l]. The shell runs its main() on
    import, so its CSS and JS are read with ast rather than imported.

    Returns two sets: ASSIGNED (every class written into a class attribute
    by the generator or the shell, plus every class the script toggles --
    structure and state) and ACTED (the classes the script selects or
    toggles, plus the classes the generator puts on its own alignment
    spans). A class that is only STYLED -- `case`, which the shell colours
    for case particles on purpose -- is in neither."""
    gen = io.open(G.__file__, encoding="utf-8").read()
    shell = io.open(SHELL, encoding="utf-8").read()
    assigned = set()
    for src in (gen, shell):
        for m in re.finditer(r"""class=(["'])([^"'%{}]*)\1""", src):
            assigned |= set(m.group(2).split())
    js = ""
    for node in ast.parse(shell).body:
        if isinstance(node, ast.Assign) and any(
                isinstance(t, ast.Name) and t.id == "JS" for t in node.targets):
            js = ast.literal_eval(node.value)
    acted = set(re.findall(r"""['"`]\.([A-Za-z_][\w-]*)""", js))
    for m in re.finditer(r"classList\.\w+\(([^)]*)\)", js):
        acted |= set(re.findall(r"""['"]([A-Za-z_][\w-]*)['"]""", m.group(1)))
    spans = set()
    for m in re.finditer(r"""<span class="([^"%]+)\"""", gen):
        spans |= set(m.group(1).split())
    return ({c.lower() for c in assigned | acted},
            {c.lower() for c in acted | spans})


SPAN_RESERVED, NOTE_RESERVED = page_classes()


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
    An a-chung that starts a syllable ('gro) stays with it, and so does one
    that ENDS a syllable (mtha', dga'): an apostrophe after a letter and not
    before one belongs to the syllable in front of it. span-id-tok/1 split
    it off, so the plain range `grub mtha` silently dropped the a-chung of
    `grub mtha'` and nothing downstream refused it. The landed bank's spans
    always include such an a-chung, never stop before it; check_tokens holds
    the tokenizer to that on every landed segment, every run.
    Every other mark (, / - . [ ] and the rest) is a token of its own. The
    battery reports how many landed span sides are plain ranges and how many
    need a bracket, so the choice is re-measured on every run rather than
    asserted here.

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
            if (side == "tib" and j < n and text[j] == "'"
                    and (j + 1 >= n or not _tokch(text[j + 1]))):
                j += 1          # the syllable-final a-chung: mtha'
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
    meaningful under the rules that numbered it. The contract -- name, NUL,
    wylie, NUL, English, UTF-8 -- is restated by an independent pin in the
    gate, because a hash that quietly stopped covering one side would let an
    edit to that side leave every outstanding proposal looking current."""
    h = hashlib.sha256()
    h.update(("%s\0%s\0%s" % (TOKENIZER, wylie, english)).encode("utf-8"))
    return h.hexdigest()


def token_digest(wylie, english):
    """16 hex digits over every token boundary of both sides.

    The baseline stores this beside `src` for every landed segment. The
    same src with a different digest can mean only one thing: the
    tokenizer's behaviour changed and its name did not, so outstanding
    proposals would compile against token numbers that now mean something
    else while their hash still says they are current."""
    h = hashlib.sha256()
    h.update(repr((tokenize(wylie, "tib"), tokenize(english, "eng")))
             .encode("utf-8"))
    return h.hexdigest()[:SRC_SHOWN]


# Every character class the tokenizer distinguishes, in every position that
# can change a boundary. The landed bank exercises only the characters it
# happens to contain; this exercises the rules themselves, so a change that
# no landed segment happens to hit is still seen.
CANARY_CHARS = ("".join(chr(c) for c in range(33, 127)) +
                " \u00a0\u2019\u2018\u201c\u201d\u2014\u2013\u00e9\u0301"
                "\u0f40\u0f72\u0f0b\u0f0d")
CANARY_FORMS = ("a@b", "@a", "a@", "a @ b", "@@", "ab@@cd", "a'@", "@'a",
                "a@'", "'@a", "1@2", "a@1")


def canary_digest():
    """Token boundaries of the canary strings on both sides, as a digest."""
    h = hashlib.sha256()
    for side in ("tib", "eng"):
        for c in CANARY_CHARS:
            for form in CANARY_FORMS:
                s = form.replace("@", c)
                h.update(repr((side, s, tokenize(s, side))).encode("utf-8"))
    return h.hexdigest()[:SRC_SHOWN]


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
    'token, "T3-T5" runs from the start of T3 to the end of T5. Echo "src" '
    'exactly as shown (16 hex digits). To claim only '
    'PART of the tokens -- the n\'t of doesn\'t or the un of unable (rule 8), '
    'a word whose last syllable carries the terminative r, like "phyi ma" '
    'out of "phyi mar", or a word before its apostrophe, like Buddha out of '
    'Buddha\'s -- add the exact part in brackets: "E9[n\'t]", "E4[un]", '
    '"T2-T3[phyi ma]", "E6[Buddha]"; it must occur exactly once in those '
    'tokens and touch the first and the last. No other cut compiles: the '
    'agentive s stays with its word -- span gis, kyis, pas whole (rule 9) -- '
    'an English word is cut only by the negative-affix convention or at an '
    'apostrophe, and a depth 1-6 span starts a syllable (a bound \'i is a '
    'd=7 member of its word). Optional "t_echo" and "e_echo": the text you '
    'mean, copied from the line above without the [n] markers; a range that '
    'does not cover exactly that text is refused, so an off-by-one citation '
    'cannot compile. A morpheme with no English exponent gets "e": null '
    '(optional "nul": why). Optional "cls": a word or two, never one of the '
    'page\'s own classes (' + ", ".join(sorted(SPAN_RESERVED)) + '). ids are '
    'a letter then letters/digits, unique in the segment. d=7 members need '
    'no parent key: the depth-5 span whose Tibetan contains them is their '
    'parent. List spans in any order; Tibetan order, eng_order and subword '
    'are derived. Do not supply tib, eng, eng_order or subword.')


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


PACKET_HEAD = re.compile(r"^SEGMENT (\S+):(\d+)  src=([0-9a-f]+)  \((.*)\)$")
PACKET_MARK = re.compile(r"\[(\d+)\]")


def _read_marked(line, text):
    """[(n, start, end)] read off one numbered packet line, or what is wrong.

    Reads only the TEXT an agent is shown: the [n] markers and the
    characters after each, located one after another in the spine text. It
    shares no code with render_tokens or tokenize, which is the point. A
    marker is never ambiguous with the text: every mark that is not a
    letter or digit is a token of its own, so a literal "[" is always
    followed by the next token's marker or a space, never by digits and "]"."""
    marks = list(PACKET_MARK.finditer(line))
    if not marks:
        return [] if not text.strip() else "no [n] markers on a line whose " \
            "text is %r" % text[:40]
    if line[:marks[0].start()].strip():
        return "text %r stands before the first marker" % line[:marks[0].start()]
    out, cur = [], 0
    for i, m in enumerate(marks):
        n = int(m.group(1))
        if n != i + 1:
            return "the markers run [%d] where [%d] was due" % (n, i + 1)
        end = marks[i + 1].start() if i + 1 < len(marks) else len(line)
        t = line[m.end():end]
        if i + 1 < len(marks) and t.endswith(" "):
            t = t[:-1]
        if not t or any(c.isspace() for c in t):
            return "[%d] reads %r: a marker is missing or a token is empty" % (n, t)
        while cur < len(text) and text[cur].isspace():
            cur += 1
        if text[cur:cur + len(t)] != t:
            return "[%d] reads %r where the spine has %r" % (
                n, t, text[cur:cur + len(t)])
        out.append((n, cur, cur + len(t)))
        cur += len(t)
    if text[cur:].strip():
        return "the spine's %r is shown under no marker" % text[cur:cur + 30]
    return out


def packet_contract(course, seq, wyl, eng):
    """None if the numbers an agent is shown ARE the numbers the compiler
    resolves, else the first place they part.

    WHY. The packet is the agent's whole interface: it cites what it sees.
    A renderer that numbered only the words, or ran one number ahead after
    the first token, would leave every other check green while each
    citation compiled to the neighbouring word -- the verifier's V4a and V4b
    did exactly that, and only token #1 of one segment was pinned. So every
    landed segment's packet is read back by _read_marked and each shown
    number is resolved through resolve_ref, the compile path itself; the
    header's src, tokenizer and token count are held to the same account,
    and one number past the last shown must not resolve.

    Returns (problem or None, how many shown numbers were resolved). The
    battery holds that count to its own tally of tokens, so a contract that
    is skipped, or stops looking after the first number, is seen."""
    return _packet_contract(course, seq, wyl, eng)


def _packet_contract(course, seq, wyl, eng):
    checked = 0
    lines = packet(course, seq, with_format=False).split("\n")
    if len(lines) < 5:
        return "the packet has %d lines" % len(lines), checked
    h = PACKET_HEAD.match(lines[0])
    if not h or h.group(1) != course or int(h.group(2)) != seq:
        return ("the header %r does not name %s:%d"
                % (lines[0][:60], course, seq)), checked
    src = source_hash(wyl, eng)[:SRC_SHOWN]
    if h.group(3) != src or h.group(4) != TOKENIZER:
        return ("the header shows src=%s (%s); the segment's is %s (%s)"
                % (h.group(3), h.group(4), src, TOKENIZER)), checked
    for letter, side, head, line, text in (("T", "tib", lines[1], lines[2], wyl),
                                           ("E", "eng", lines[3], lines[4], eng)):
        shown = _read_marked(line, text)
        if isinstance(shown, str):
            return "%s line: %s" % (side, shown), checked
        hm = re.search(r"tokens %s1-%s(\d+):$" % (letter, letter), head)
        if not hm or int(hm.group(1)) != len(shown):
            return ("%s: the heading %r, but %d tokens are shown"
                    % (side, head, len(shown))), checked
        toks = tokenize(text, side)
        for n, a, b in shown:
            try:
                got = resolve_ref("%s%d" % (letter, n), text, toks, letter,
                                  seq, "packet")
            except Refusal as e:
                return ("the packet shows %s%d but the compiler refuses it: "
                        "%s" % (letter, n, e.code)), checked
            checked += 1
            if got != (a, b):
                return ("the packet shows %s%d as %r, the compiler resolves "
                        "%s%d to %r" % (letter, n, text[a:b], letter, n,
                                        text[got[0]:got[1]])), checked
        try:
            resolve_ref("%s%d" % (letter, len(shown) + 1), text, toks, letter,
                        seq, "packet")
            return ("the compiler resolves %s%d, which the packet never "
                    "shows" % (letter, len(shown) + 1)), checked
        except Refusal:
            pass
    return None, checked


# --------------------------------------------------------------------------
# Token references

REF = re.compile(r"^\s*([TE])(\d+)(?:\s*-\s*([TE])?(\d+))?\s*(?:\[(.*)\])?\s*$",
                 re.S)
ID_OK = re.compile(r"^[A-Za-z][A-Za-z0-9_]*$")
SPAN_KEYS = {"id", "d", "t", "e", "cls", "nul", "t_echo", "e_echo"}
SEG_KEYS = {"seq", "src", "title", "note", "spans"}
DERIVED = {"tib": "the Tibetan string is sliced from the spine; give \"t\"",
           "eng": "the English string is sliced from the spine; give \"e\"",
           "subword": "derived from the offsets",
           "eng_order": "derived from the offsets",
           "clauses": "not supported; the generator does not render them"}
# Plain text for an HTML class attribute, with no edge space and not empty.
# The empty string used to pass and then come back from the generator as no
# class at all, which the round trip reported as a mismatch instead of naming
# the real problem. (Landed classes are free text -- 'quotative + connective
# (citation formula)' -- so the characters allowed stay as they were.)
CLS_OK = re.compile(r'^[^"<>&\s](?:[^"<>&]*[^"<>&\s])?$')

# The inline formatting landed notes actually use. A note is written into the
# page raw and the builder banks from the whole page, so anything structural
# -- a div, a heading, a u-span or a null marker, a comment (the builder reads
# TRANSFER comments) -- is refused rather than trusted.
NOTE_TAGS = {"i", "b", "em", "strong", "code", "br", "p", "q", "tt", "sup",
             "sub", "span"}
NOTE_TAG = re.compile(r"<\s*(/?)\s*([A-Za-z][A-Za-z0-9]*)([^>]*)>")
# The only attributes the 1,696 landed notes carry. Everything else is
# refused: data-l and data-d make an element a peer of a real span to the
# page's script, an id can shadow the depth slider, an on* handler runs
# script in the published page.
NOTE_ATTRS = {"class", "style"}
NOTE_ATTR = re.compile(r"""([^\s=/>"']+)(?:\s*=\s*("[^"]*"|'[^']*'|[^\s>"']+))?""")


def note_markup_problem(note):
    """None if the note's markup is inline formatting only, else what is not.

    Names are compared without case. The first version matched `data-` and
    `class="u"` with case, so `<SPAN CLASS="u" DATA-L="s169a">` compiled --
    and browsers lower-case attribute names, so the page's script, which
    selects '.u' and [data-l], treated the note text as a live linked span.
    It also only looked for u as the FIRST class, so `class="case u"`
    passed."""
    if "<!--" in note:
        return "an HTML comment"
    for m in NOTE_TAG.finditer(note):
        name, attrs = m.group(2).lower(), m.group(3)
        if name not in NOTE_TAGS:
            return "the tag %r" % m.group(0)[:40]
        for a in NOTE_ATTR.finditer(attrs):
            aname = a.group(1).lower()
            if aname not in NOTE_ATTRS:
                return ("the attribute %r in %r (a note may carry only %s)"
                        % (a.group(1), m.group(0)[:60],
                           " and ".join(sorted(NOTE_ATTRS))))
            if aname == "class":
                val = (a.group(2) or "").strip("\"'")
                hit = [c for c in val.split() if c.lower() in NOTE_RESERVED]
                if hit:
                    return ("the class %r in %r, which the page's script or "
                            "the builder reads as alignment"
                            % (hit[0], m.group(0)[:60]))
    # A "<" that forms no complete tag still opens one in a browser: `<!` or
    # `<?` starts a bogus comment and `<x` a tag, each running on to the next
    # ">" on the page -- which can be the one closing the note's own div. A
    # bare "<" before a space or a digit is text, and so is any ">"
    # ("ma -> not", in 95 landed notes).
    rest = NOTE_TAG.sub("", note)
    m = re.search(r"<[A-Za-z/!?]", rest)
    if m:
        return ("an unclosed tag opener %r, which a browser reads as markup "
                "up to the next '>' on the page; write &lt; for a literal <"
                % rest[m.start():m.start() + 20])
    return None


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


def _glued(text, a):
    """True if a Tibetan span starting at `a` would start INSIDE a syllable.

    The character before it belongs to the same syllable when it is a
    letter or digit, an a-chung, a stacking +, or a disambiguation mark
    between a prefix letter and its root. EWTS writes that mark as a dot
    (g.yag) and the spine, from its ACIP source, writes it as a hyphen:
    `g-yo`, `g-yon`, `g-yas` -- 1,256 times after a lone g. The hyphen also
    joins whole syllables (`rdo-rje`, `byang-chub`, folio ranges like
    `174a-174b`), so it glues only when what precedes it is ONE letter
    standing at the start of its syllable. The first version knew only the
    dot, and a d=6 `yam` cut out of `g-yam` compiled."""
    if a <= 0:
        return False
    p = text[a - 1]
    if G.WORDCH.match(p) or p in "'+":
        return True
    if p == "." and a >= 2 and text[a - 2].isalpha() \
            and a < len(text) and text[a].isalpha():
        return True
    return (p == "-" and a >= 2 and text[a - 2].isalpha()
            and (a == 2 or not (text[a - 3].isalnum() or text[a - 3] in "'+.-"))
            and a < len(text) and text[a].isalpha())


def check_cuts(got, wyl, eng, seq, at):
    """Refuse the first cut the campaign's rules do not allow (cut_problems)."""
    probs = cut_problems(got, wyl, eng, at, first=True)
    if probs:
        probs[0].seq = seq
        raise probs[0]


def cut_problems(got, wyl, eng, at, first=False):
    """Every word or syllable cut the campaign's rules do not allow.

    Returned as unraised Refusals, in the order check_cuts meets them, so
    compiling refuses the first and the battery's rule-shape census can
    count them all on landed pages that were never compiled.

    The subword flag this file derives switches OFF the generator's refusal
    of a span that cuts a word -- the refusal that exists to catch an
    accident. So the flag may only be derived for a cut the rules DECLARE
    legal, and every other cut is refused here, by name, first:

      * ENGLISH (rule 8). A word may be cut only by the negative-affix
        convention: the span is itself one of test_no_broken_words'
        ALLOWED_SUBWORD ("n't", "un", "im", "less", and the listed stems),
        compared EXACTLY as that gate compares it; or another span meets
        the cut and one of the two is on the list ("able" beside a claimed
        "un" -- the same convention seen from the stem's end). Case is
        ignored only in that second clause, for C03:229's "Un|certain". The
        first version ignored it in the first clause too, and a capitalised
        "Un" standing alone compiled -- and test_no_broken_words, which
        compares with case, refuses that very page. Where another span meets
        the cut the gate sees a tag there and passes the edge whatever the
        case, so the second clause can never admit what it refuses.
      * TIBETAN END. A span may stop inside a syllable only before one of
        test_no_split_syllables' SUFFIX, and a stem of two letters or fewer
        only when it is in STEM_OK -- exactly that gate's rule. A depth 1-6
        span may not stop before a bare s at all: rule 9 says the agentive s
        is not separable (span gis, kyis, pas whole), and a flat span cannot
        tell an agentive s from a past-stem s, both of which it would drop.
        Inside a d=5 word that keeps the s, a d=7 member may still name the
        stem, as the SUFFIX gate allows. No span may stop before a
        syllable-final a-chung, which is spelling, not a morpheme.
      * TIBETAN START, depth 1-6 (rule 3). A standalone word or particle
        starts a syllable; only a d=7 member may start inside one. The
        generator's own BOUND check catches this only when the morpheme also
        sits in the d=5 span listed above it, so a d=6 `'i` right after its
        word used to pass."""
    out = []

    def add(code, msg, sid):
        out.append(Refusal(code, msg, None, sid))
        return first
    eng_spans = [s for s in got if s["e"] is not None]
    # Tibetan order, never proposal order: "list spans in any order" must
    # hold for WHICH refusal a proposal gets, not only for whether it gets
    # one. Walked in proposal order, c3p55:164 was SYLLABLE_SPLIT listed
    # forwards and FLAT_STARTS_MID_SYLLABLE listed backwards.
    for s in sorted(got, key=lambda s: (s["t"][0], s["t"][0] - s["t"][1],
                                        s["d"], s["id"])):
        a, b = s["t"]
        if s["d"] != 7 and _glued(wyl, a):
            if add("FLAT_STARTS_MID_SYLLABLE", "%s is depth %d but "
                   "starts inside a syllable (%r|%r). Depth 1-6 spans "
                   "start a syllable (rule 3); a bound morpheme is a "
                   "d=7 member of the d=5 word it is bound to (claim "
                   "the word through it), or it is left unwrapped."
                   % (at(s, "t"), s["d"], wyl[max(0, a - 8):a],
                      wyl[a:b]), s["id"]):
                return out
        if (b < len(wyl) and wyl[b] == "'" and G.WORDCH.match(wyl[b - 1])
                and (b + 1 >= len(wyl) or not _tokch(wyl[b + 1]))):
            if add("SYLLABLE_SPLIT", "%s stops before the final "
                   "a-chung of its syllable (%r). The a-chung is "
                   "spelling, not a morpheme: claim the whole token."
                   % (at(s, "t"), wyl[a:b + 1]), s["id"]):
                return out
        if _cut(wyl, b):
            rest = re.match(r"[A-Za-z']*", wyl[b:]).group(0)
            t = wyl[a:b]
            if s["d"] != 7 and rest == "s":
                if add("SYLLABLE_SPLIT", "%s is depth %d and stops "
                       "before the s of %r. The agentive s is not "
                       "separable (rule 9): span gis, kyis, pas whole. "
                       "A d=7 member may name the stem inside a d=5 "
                       "word that keeps the s."
                       % (at(s, "t"), s["d"], wyl[a:b] + rest), s["id"]):
                    return out
            elif rest not in SUFFIX or (len(t) <= 2 and t not in STEM_OK):
                if add("SYLLABLE_SPLIT", "%s stops inside the "
                       "syllable %r, before %r. A span may stop "
                       "mid-syllable only before a Wylie suffix (%s), "
                       "and a stem of two letters or fewer only if it "
                       "is a listed stem. Claim the whole syllable."
                       % (at(s, "t"), wyl[a:b] + rest, rest,
                          " ".join(sorted(SUFFIX))), s["id"]):
                    return out
        if s["e"] is None:
            continue
        c, d = s["e"]
        for edge in (c, d):
            if not _cut(eng, edge):
                continue
            # exactly the post-landing gate's comparison: case matters
            if eng[c:d] in ALLOWED_SUBWORD:
                continue
            other = [o for o in eng_spans if o is not s and
                     (o["e"][1] == edge if edge == c else o["e"][0] == edge)]
            if other and (eng[c:d].lower() in _ALLOWED_LOWER or any(
                    eng[o["e"][0]:o["e"][1]].lower() in _ALLOWED_LOWER
                    for o in other)):
                continue
            if add("SUBWORD_NOT_ALLOWED", "the English %s cuts the "
                   "word %r. A span may cut an English word only by "
                   "the negative-affix convention (rule 8): the "
                   "affix or stem is exactly one of %s, or another span "
                   "meets the cut and one of the two is. Claim the whole "
                   "word." % (at(s, "e"), _word_at(eng, edge),
                              ", ".join(sorted(ALLOWED_SUBWORD))), s["id"]):
                return out
    return out


def _fold(s):
    return " ".join(s.split())


def check_echo(sp, ta, ea, wyl, eng, tt, et, seq, sid):
    """Refuse a range that does not cover the text its optional echo names.

    WHY. Under retyped strings a copying error was refused; under token ids
    an off-by-one citation that lands on a legal boundary compiles silently
    to the NEIGHBOURING word, and every later gate passes it, because every
    string on the page is verbatim. The packet makes that likelier, not
    less: its [n] markers sit among literal brackets and digits
    ('[1][[2]iv[3].[4]262'). An agent that also writes what it MEANS gives
    the compiler something to hold the citation to. It is optional, so the
    token interface still costs nothing when it is not used. Whitespace is
    folded because the packet folds it: an agent never sees a double space."""
    for key, rng, text, toks, letter in (("t_echo", ta, wyl, tt, "T"),
                                         ("e_echo", ea, eng, et, "E")):
        if key not in sp:
            continue
        v = sp[key]
        if not isinstance(v, str) or not v.strip():
            raise Refusal("BAD_ECHO", "%r must be the text the range covers, "
                          "copied from the packet" % key, seq, sid)
        if rng is None:
            raise Refusal("ECHO_WITHOUT_RANGE", "%r is given but \"e\" is "
                          "null: there is no English range for it to check. "
                          "Drop %r, or give the range." % (key, key), seq, sid)
        got = text[rng[0]:rng[1]]
        if _fold(v) == _fold(got):
            continue
        where = []
        for i, (x, _y) in enumerate(toks):
            for j in range(i, len(toks)):
                if _fold(text[x:toks[j][1]]) == _fold(v):
                    where.append(_show(text, toks, x, toks[j][1], letter)
                                 .split(" ")[0])
                if toks[j][1] - x > len(v) + 8:
                    break
        raise Refusal("ECHO_MISMATCH", "%s names %s, but %s says %r. The "
                      "range and the words disagree, so neither is trusted: "
                      "cite the tokens of the words you mean%s."
                      % ("\"t\"" if letter == "T" else "\"e\"",
                         _show(text, toks, rng[0], rng[1], letter), key, v,
                         (" (%r sits at %s)" % (v, ", ".join(where[:4])))
                         if where else ""), seq, sid)


def _word_at(text, i):
    """The whole generator word around offset i."""
    a = i
    while a > 0 and G.WORDCH.match(text[a - 1]):
        a -= 1
    b = i
    while b < len(text) and G.WORDCH.match(text[b]):
        b += 1
    return text[a:b]


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
    if src is None or src == "":
        raise Refusal("MISSING_HASH", "the proposal must echo the segment's "
                      "src hash from its packet; token numbers mean nothing "
                      "without it", seq)
    # Exactly the 16 hex digits the packet shows. A shorter prefix is a
    # weaker check, and a contract that accepts several forms is one nobody
    # can pin: the first gate let an 8-digit prefix through unnoticed.
    if not isinstance(src, str) or not SRC_FORM.match(src):
        raise Refusal("BAD_HASH", "src %r is not in the packet's form: "
                      "exactly %d lowercase hex digits, copied from the "
                      "packet's src= field" % (src, SRC_SHOWN), seq)
    if src != full[:SRC_SHOWN]:
        raise Refusal("STALE_HASH", "src %r does not match this segment's "
                      "text under %s (current src=%s). The proposal was "
                      "written against a different text or tokenizer; "
                      "re-read the packet and re-propose."
                      % (src, TOKENIZER, full[:SRC_SHOWN]), seq)
    title = seg.get("title")
    if not isinstance(title, str) or not title.strip():
        raise Refusal("MISSING_TITLE", "give the segment a short lowercase "
                      "\"title\"; the generator requires one", seq)
    # The generator writes the title into the page raw. Markup there would
    # be read by the builder, which banks from the whole page.
    if "<" in title or ">" in title:
        raise Refusal("MARKUP_IN_TITLE", "the title %r contains markup. The "
                      "generator writes it into the page unescaped, where "
                      "the builder would read it; write plain words (an "
                      "entity such as &mdash; is fine)" % title, seq)
    if seg.get("note") is not None and not isinstance(seg["note"], str):
        raise Refusal("BAD_NOTE", "\"note\" must be text", seq)
    if seg.get("note"):
        bad = note_markup_problem(seg["note"])
        if bad:
            raise Refusal("MARKUP_IN_NOTE", "the note contains %s. A note may "
                          "carry inline formatting (%s), never page "
                          "structure: the generator writes it raw and the "
                          "builder banks from the whole page."
                          % (bad, ", ".join("<%s>" % t
                                            for t in sorted(NOTE_TAGS))), seq)
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
            raise Refusal("BAD_CLS", "\"cls\" %r must be non-empty plain "
                          "text without quotes, angle brackets, & or edge "
                          "spaces; it is written into an HTML class "
                          "attribute. Leave the key out for no class."
                          % (cls,), seq, sid)
        hit = [c for c in (cls or "").split() if c.lower() in SPAN_RESERVED]
        if hit:
            raise Refusal("RESERVED_CLS", "\"cls\" %r names %r, one of the "
                          "page's own classes (%s). The page styles or "
                          "scripts those, so the span would render as "
                          "something it is not -- \"nul\" as a null marker "
                          "while it carries English, \"pair\" as a "
                          "two-column grid. Choose another word."
                          % (cls, hit[0], ", ".join(sorted(SPAN_RESERVED))),
                          seq, sid)
        nul = sp.get("nul")
        if nul is not None and not isinstance(nul, str):
            raise Refusal("BAD_NUL", "\"nul\" must be text", seq, sid)
        ta = resolve_ref(sp["t"], wyl, tt, "T", seq, sid)
        ea = (resolve_ref(sp["e"], eng, et, "E", seq, sid)
              if sp.get("e") is not None else None)
        check_echo(sp, ta, ea, wyl, eng, tt, et, seq, sid)
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

    # -- the cut rules, BEFORE the subword flag is derived from the cuts
    check_cuts(got, wyl, eng, seq, at)

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
    """The generator's own main(), in-process, on one segment."""
    return run_generator_spec({"course": course,
                               "segments": [copy.deepcopy(spec_seg)]},
                              spec_seg.get("seq"))


def run_generator_spec(spec, seq=None):
    """The generator's own main(), in-process, on a whole spec.

    Returns its stdout. Raises Refusal GENERATOR_REFUSED with the generator's
    own message when it refuses, and GENERATOR_CRASHED on anything else."""
    spec = copy.deepcopy(spec)
    old_in, old_out = sys.stdin, sys.stdout
    sys.stdin = io.StringIO(json.dumps(spec, ensure_ascii=False))
    sys.stdout = buf = io.StringIO()
    try:
        G.main()
    except SystemExit as e:
        msg = str(e.code) if e.code is not None else ""
        raise Refusal("GENERATOR_REFUSED", msg.replace("REFUSED: ", "", 1),
                      seq)
    except Exception as e:   # the generator crashing is itself a finding
        raise Refusal("GENERATOR_CRASHED", "%s: %s" % (type(e).__name__, e),
                      seq)
    finally:
        sys.stdin, sys.stdout = old_in, old_out
    return buf.getvalue()


# What to do when the generator cannot reach the occurrence a proposal chose.
# It is the commonest refusal by far, so its advice matters: the two easy
# "fixes" are both wrong. Moving the span to the occurrence the generator DID
# reach banks a place nobody judged (rule 10), and widening a neighbour to
# cover the gap is over-capture (rule 7).
UNREACHABLE = {
    "T": ("The generator places a span by searching for its TEXT forward from "
          "the previous span, so it cannot reach the occurrence you chose. Do "
          "not move the span to that earlier occurrence (rule 10) and do not "
          "widen a neighbouring span over the gap (rule 7). Drop this span, "
          "or add a span for a word in between ONLY if that word genuinely "
          "aligns."),
    "E": ("The generator places a span's English by searching for its TEXT "
          "forward from the previous English span, so it cannot reach the "
          "occurrence you chose. Do not move the English to that earlier "
          "occurrence (rule 10) and do not widen a neighbouring span over the "
          "gap (rule 7). Null this span's English (\"e\": null), or drop the "
          "span, or add a span for a word in between ONLY if that word "
          "genuinely aligns."),
    "M": ("The generator places a d=7 member's English by the first place "
          "its TEXT occurs inside its parent's English, after the member "
          "before it, with no word boundary, so it cannot reach the "
          "occurrence you chose. Do not move the English to that earlier "
          "occurrence (rule 10) and do not widen the parent (rule 7). Null "
          "this member's English (\"e\": null), or drop the member."),
}


def member_unreachable(spec_seg, chosen, eng):
    """(member id, advice) when a d=7 member's English also sits EARLIER
    inside its parent than the place chosen, else None.

    WHY. The generator orders a parent's members by the first place each
    member's text occurs in the parent's English, and places each by a raw
    find after the member before it, with no word boundary. A member "a"
    chosen as the word a in "as a burden" is found inside "as" and put
    first, and then "as" itself is out of reach -- and the generator's
    refusal blames the innocent member ("'as' is not inside its compound").
    The whole-bank member re-point sweep found eight such refusals passing
    through unnamed. This is consulted only AFTER the generator has refused
    or misplaced a member, so it never refuses a spec the generator
    resolves correctly (two "of" members in order are fine); it names the
    member to change."""
    seq = spec_seg["seq"]
    by_d = {s["id"]: s["d"] for s in spec_seg["spans"]}
    for sid, c in sorted(chosen.items(), key=lambda kv: kv[1]["eng"] or (0, 0)):
        if by_d.get(sid) != 7 or c["eng"] is None:
            continue
        par = [pid for pid, pc in chosen.items()
               if by_d.get(pid) == 5 and pc["eng"] is not None
               and pc["tib"][0] <= c["tib"][0] and c["tib"][1] <= pc["tib"][1]]
        if not par:
            continue
        pa, pb = chosen[par[0]]["eng"]
        text = eng[c["eng"][0]:c["eng"][1]]
        k = eng.find(text, pa, pb)
        if 0 <= k < c["eng"][0]:
            return sid, (
                "the d=7 member %s's English %r also occurs earlier inside "
                "its parent %s (%r, at %r). The generator orders and places "
                "a member's English by the FIRST place its text occurs in "
                "the parent, with no word boundary, so it cannot put this "
                "one where you chose, and the members after it can fall out "
                "of reach. Do not move it to the earlier place (rule 10). "
                "Null this member's English (\"e\": null), or drop it."
                % (sid, text, par[0], eng[pa:pb],
                   eng[max(pa, k - 6):k + len(text) + 6]))
    return None


def round_trip(course, spec_seg, chosen, wyl, eng):
    """Hold the generator to the chosen offsets. Returns (tib_block, eng_block).

    The spec is fed through the generator and its pair block handed to
    check_blocks, which does the holding. A generator refusal that
    member_unreachable can explain is re-named after the member to change,
    with the generator's own words kept."""
    try:
        body = run_generator(course, spec_seg)
    except Refusal as e:
        hit = member_unreachable(spec_seg, chosen, eng) \
            if e.code == "GENERATOR_REFUSED" else None
        if hit:
            raise Refusal("MEMBER_ENGLISH_UNREACHABLE", "%s The generator's "
                          "own words: %s" % (hit[1], e.msg), e.seq, hit[0])
        raise
    pairs = PAIR.findall(body)
    if len(pairs) != 1:
        raise Refusal("ROUND_TRIP_MISMATCH", "the generator's output did not "
                      "contain exactly one pair block", spec_seg["seq"])
    tb, eb = pairs[0]
    check_blocks(tb, eb, spec_seg, chosen, wyl, eng)
    return tb, eb


def check_blocks(tb, eb, spec_seg, chosen, wyl, eng):
    """The round-trip checks on one rendered pair block, and nothing else.

    Every span must come back at exactly the chosen offsets on both sides,
    every null span as a null marker and nothing else, and the plain text of
    both blocks must be the spine text. Anything else is a spec that MEANS
    something other than what was proposed, and is refused.

    Kept apart from running the generator so the gate can hold these checks
    to STUB output. Several of them cannot be reached by any proposal the
    earlier checks admit -- today's generator never renders text twice or an
    unproposed span -- so only a stub can prove they still fire; without
    one, deleting them would pass every test."""
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
                "string to %s. %s%s"
                % (_show(wyl, tt, c["tib"][0], c["tib"][1], "T"),
                   _show(wyl, tt, r["a"], r["b"], "T"), UNREACHABLE["T"],
                   why(sid)), seq, sid)
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
                "string to %s. %s%s"
                % (_show(eng, et, c["eng"][0], c["eng"][1], "E"),
                   _show(eng, et, r["a"], r["b"], "E"),
                   UNREACHABLE["M" if c["d"] == 7 else "E"],
                   why(sid)), seq, sid)
    extra = (set(seen_en) | set(seen_nul)) - want
    if extra:
        raise Refusal("ROUND_TRIP_MISMATCH", "English spans rendered that were "
                      "not proposed: %s" % sorted(extra), seq)
    return tb, eb


def expected_view(views):
    """What the builder must bank from the page: [(d, label, text)] per side.

    Computed from the CHOSEN offsets and the spine text alone -- never from
    the generator's output -- with the builder's own whitespace folding. A
    null span banks an empty text on the English side (the builder strips the
    null marker's annotation), and each segment carries its d=1 wrapper."""
    tib, eng_side = [], []

    def fold(s):
        return " ".join(s.split())
    for seq, wyl, eng, chosen in views:
        tib.append((1, "s%dS" % seq, fold(wyl)))
        eng_side.append((1, "s%dS" % seq, fold(eng)))
        for sid, c in chosen.items():
            lab = "s%d%s" % (seq, sid)
            tib.append((c["d"], lab, fold(wyl[c["tib"][0]:c["tib"][1]])))
            eng_side.append((c["d"], lab,
                             fold(eng[c["eng"][0]:c["eng"][1]])
                             if c["eng"] is not None else ""))
    return sorted(tib), sorted(eng_side)


def check_page(page, blocks, views):
    """The WHOLE emitted page, read the way the bank will read it.

    The per-segment round trip looks only inside each pair block, but the
    builder (build_alignment_layer.spans_of) reads the whole page, titles and
    notes included. A note carrying its own pair block once compiled, and the
    builder banked a second English under a real label; two segments with
    the same seq merged their Englishes the same way. So the page is read
    back through the builder's own reader and must yield exactly the spans
    proposed -- no more, no fewer, no other text."""
    pairs = PAIR.findall(page)
    if pairs != list(blocks):
        raise Refusal("BUILDER_VIEW_MISMATCH", "the whole page carries %d "
                      "pair block(s) that are not the %d per-segment block(s) "
                      "the round trip proved" % (len(pairs), len(blocks)))
    want_t, want_e = expected_view(views)
    for side, want in (("tib", want_t), ("eng", want_e)):
        labels = [lab for _d, lab, _t in want]
        twice = sorted(set(x for x in labels if labels.count(x) > 1))
        if twice:
            raise Refusal("BUILDER_VIEW_MISMATCH", "the builder would bank "
                          "label %s twice on the %s side, merging two "
                          "readings under one name" % (twice[0], side))
        got = sorted(G.B.spans_of(page, side))
        if got != want:
            extra = sorted(set(got) - set(want)) or \
                [x for x in got if got.count(x) > want.count(x)][:1]
            miss = sorted(set(want) - set(got))
            raise Refusal("BUILDER_VIEW_MISMATCH", "the builder would bank "
                          "%s side %s%s, which is not what was proposed. "
                          "Something on the page outside the proved pair "
                          "blocks (a title or a note) is being read as "
                          "alignment."
                          % ("the Tibetan" if side == "tib" else "the English",
                             ("extra %r" % (extra[0],)) if extra else "",
                             (" and misses %r" % (miss[0],)) if miss else ""))


def compile_with_blocks(prop):
    """A whole proposal -> (spec, [(tib_block, eng_block)], [view]).

    A view is (seq, wylie, English, chosen offsets) for one segment.

    Each segment is compiled and round-tripped through the generator on its
    own, so a refusal names the segment it belongs to; then the whole spec
    is generated once more and checked as a page (check_page)."""
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
    seen = set()
    for seg in segs:
        q = seg.get("seq") if isinstance(seg, dict) else None
        if isinstance(q, int) and not isinstance(q, bool):
            if q in seen:
                raise Refusal("DUPLICATE_SEQ", "segment %d appears twice in "
                              "one proposal. Each segment is proved on its "
                              "own, and the builder would merge the two under "
                              "the same labels; give each segment once."
                              % q, q)
            seen.add(q)
    out, blocks, views = [], [], []
    for seg in segs:
        try:
            spec, chosen = compile_segment(course, seg)
            wyl, eng = spine_row(course, spec["seq"])
            tb, eb = round_trip(course, spec, chosen, wyl, eng)
        except Refusal:
            raise
        except Exception as e:     # a compiler fault is refused, never raised
            raise Refusal("COMPILER_FAULT", "%s: %s" % (type(e).__name__, e),
                          seg.get("seq") if isinstance(seg, dict) else None)
        out.append(spec)
        blocks.append((tb, eb))
        views.append((spec["seq"], wyl, eng, chosen))
    whole = {"course": course, "segments": out}
    try:
        check_page(run_generator_spec(whole), blocks, views)
    except Refusal:
        raise
    except Exception as e:
        raise Refusal("COMPILER_FAULT", "%s: %s" % (type(e).__name__, e))
    return whole, blocks, views


def compile_proposal(prop):
    """A whole proposal file -> a generator spec. Refuses on the first error."""
    return compile_with_blocks(prop)[0]


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


class _Loose(html.parser.HTMLParser):
    """Spans and text of a block, read by the standard library's parser.

    Loose on purpose: the rule-shape census must see the hand-built C01 and
    C02 blocks that walk() refuses to read, because those are most of what
    the rulings affect."""

    def __init__(self):
        html.parser.HTMLParser.__init__(self, convert_charrefs=True)
        self.text, self.pos, self.stack, self.spans, self.nul = [], 0, [], [], 0

    def handle_starttag(self, tag, attrs):
        if tag != "span":
            return
        a = dict(attrs)
        if (a.get("class") or "").split() == ["nul"]:
            self.nul += 1
            self.stack.append(None)
        elif a.get("data-l"):
            d = a.get("data-d") or ""
            self.stack.append([a["data-l"], int(d) if d.isdigit() else 0,
                               self.pos])
        else:
            self.stack.append(False)

    def handle_endtag(self, tag):
        if tag != "span" or not self.stack:
            return
        f = self.stack.pop()
        if f is None:
            self.nul -= 1
        elif f:
            self.spans.append((f[0], f[1], f[2], self.pos))

    def handle_data(self, data):
        if not self.nul:
            self.text.append(data)
            self.pos += len(data)


def _loose(part):
    p = _Loose()
    p.feed(re.sub(r'<div class="lab">.*?</div>', "", part, flags=re.S))
    p.close()
    return "".join(p.text), p.spans


def rule_shapes(pair_chunk):
    """{code: count} of every cut on one landed block that cut_problems
    would refuse, read loosely from the page.

    WHY. The round trip names only the FIRST refusal of a block, so a block
    that breaks rule 3 but is also FLAT_NESTED is counted as FLAT_NESTED and
    its rule-3 shape disappears from view. The first report put 15 blocks in
    front of Adam for a ruling on rule 3 and rule 9; an independent scan
    found the rule-3 shape alone in 164. A ruling must be asked with its
    whole scope, so every landed block is examined here with the compiler's
    OWN cut rules (cut_problems), whatever its first refusal is."""
    ti = pair_chunk.find('<div class="tib">')
    ei = pair_chunk.find('<div class="eng">')
    if ti < 0 or ei < ti:
        return {}
    eng_part = pair_chunk[ei:]
    for stop in ('<div class="note"', '<h2', '<div class="legend"'):
        k = eng_part.find(stop)
        if k >= 0:
            eng_part = eng_part[:k]
    ttext, tspans = _loose(pair_chunk[ti:ei])
    etext, espans = _loose(eng_part)
    eng_at = {}
    for lab, _d, a, b in espans:
        if b > a:                    # a null marker wraps no English text
            eng_at.setdefault(lab, (a, b))
    got = [{"id": lab, "d": d, "t": (a, b), "e": eng_at.get(lab)}
           for lab, d, a, b in tspans if d != 1 and b > a]
    out = {}
    for r in cut_problems(got, ttext, etext, lambda s, side: s["id"]):
        out[r.code] = out.get(r.code, 0) + 1
    return out


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

# A protocol label excuses ONLY the shape that defines its protocol, named
# here one by one. It used to be a deny-list (everything but three classes),
# which quietly excused ROUND_TRIP_MISMATCH and GENERATOR_REFUSED on any
# block that carried the label. Any other failure on a protocol block must be
# frozen by name in the baseline like every other legacy block, or it fails.
#   PHONETICS -- sound-to-spelling links are d=7 hung off a clause, which no
#                depth-5 parent contains (PHONETICS PROTOCOL rule 1)
#   MARKER    -- the apparatus clause is wrapped in the English with no
#                Tibetan under it (E-071)
PROTOCOL_SHAPES = {"PHONETICS_PROTOCOL": {"D7_NO_PARENT"},
                   "MARKER_PROTOCOL": {"ENGLISH_ONLY_SPAN"}}


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
    And, on every block that round-trips:
      * the OCCURRENCE SWEEP (occurrence_variants, member_repoints): the
        landed proposal with one span moved to a later occurrence of its own
        text, or a d=7 member's English re-pointed to a word inside its
        parent whose text also sits earlier in that parent. Each must be
        refused or resolve exactly as moved. It puts the round trip's
        wrong-occurrence refusal under whole-bank evidence, not pins.
      * the CUT SWEEP (cut_variants): the landed proposal with one span
        shortened by a character so it cuts a word or a syllable, which
        must be refused under the rule's own name.
    Every page those sweeps compile is then run through the post-landing
    gates themselves (post_gates). The round trip also compiles every block
    with its spans listed BACKWARDS, because the packet promises "list spans
    in any order" and every landed proposal is otherwise in Tibetan order.
    Beside all that, on every landed block whatever its fate: the packet
    contract (packet_contract) and the rule-shape census (rule_shapes).
    """
    courses = dir_courses()
    res = {"pages": 0, "tags_on_pages": 0, "tags_in_pairs": 0, "pairs": 0,
           "wrappers": 0, "nul_markers": 0, "spec_spans_all": 0,
           "unreadable_tags": 0, "segments_ok": 0, "spans_ok": 0,
           "sides": {}, "side_examples": {}, "side_depths": {},
           "sides_unaddressable": 0, "blocks": {}, "classes": {},
           "census_errors": [], "tokenizer_errors": [], "gen_labels": None,
           "derived_crossing": 0, "derived_subword": 0,
           "side_instances": {}, "token_digests": {},
           "packets": 0, "packet_errors": [], "packet_tokens": 0,
           "tokens": 0, "shapes": {},
           "sweep": {"variants": 0, "compiled": 0, "refused": {},
                     "mismatch": [], "kinds": {}},
           "cuts": {"variants": 0, "refused": {}, "compiled": [],
                    "wrong_name": [], "kinds": {}},
           "bodies": [], "post_gates": {}}
    packeted = set()

    def tally_side(key, d, code, example):
        res["sides"][code] = res["sides"].get(code, 0) + 1
        if code not in ("plain", "bracket"):
            res["side_examples"].setdefault(code, "%s  %s" % (key, example))
            dd = res["side_depths"].setdefault(code, {})
            dd[str(d)] = dd.get(str(d), 0) + 1
            res["side_instances"].setdefault(code, []).append(
                "%s  %s" % (key, example))

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
                # token boundaries of every landed segment, readable or not:
                # the pin that catches a tokenizer changed without a bump
                row0 = spine_row(course, seq) if seq is not None else None
                if row0 is not None:
                    res["token_digests"]["%s:%d" % (course, seq)] = "%s:%s" % (
                        source_hash(*row0)[:SRC_SHOWN], token_digest(*row0))
                    if (course, seq) not in packeted:
                        packeted.add((course, seq))
                        res["packets"] += 1
                        err, n = packet_contract(course, seq, *row0)
                        res["packet_tokens"] += n
                        # the tally the contract is held to, counted here
                        res["tokens"] += len(tokenize(row0[0], "tib")) + \
                            len(tokenize(row0[1], "eng"))
                        if err:
                            res["packet_errors"].append(
                                "%s:%d %s" % (course, seq, err))
                shapes = rule_shapes(chunk)
                if shapes:
                    res["shapes"][key] = dict(sorted(shapes.items()))
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
                # backwards: "list spans in any order" is held on every block
                back = dict(prop, spans=list(reversed(prop["spans"])))
                try:
                    whole, blks, views = compile_with_blocks(
                        {"course": course, "segments": [back]})
                    spec, (rtb, reb) = whole["segments"][0], blks[0]
                    chosen = views[0][3]
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
                sweep_block(res, key, course, prop, wyl, eng, chosen)
                cut_block(res, key, course, prop, wyl, eng, chosen)
    post_gates(res)
    return res


def occurrence_variants(prop, wyl, eng, chosen):
    """The landed proposal with ONE span side moved to a later occurrence.

    For every span side named by a plain token range, find the next place
    after it where the same text starts and ends on token boundaries, and
    yield the proposal with that side pointing there, together with the
    offsets every span is then INTENDED to have. Spans that would collide
    with the moved side are dropped from the variant (and a dropped or moved
    word takes its d=7 members with it), so the variant is a proposal the
    earlier checks can admit and the round trip has to judge. This is the
    danger case made systematic: the generator, fed the string, walks a
    forward cursor and may land on the occurrence the span was moved away
    from. Each variant must be refused by name, or compile to a spec that
    resolves exactly as moved. Bracketed sides are left alone: their text is
    not a whole-token run, so a token-aligned later copy is a different
    kind of claim."""
    toks = {"tib": tokenize(wyl, "tib"), "eng": tokenize(eng, "eng")}
    byid = {sp["id"]: sp for sp in prop["spans"]}

    def members(pid):
        pa, pb = chosen[pid]["tib"]
        return {sid for sid, c in chosen.items()
                if byid[sid]["d"] == 7 and sid != pid
                and pa <= c["tib"][0] and c["tib"][1] <= pb}
    for side, key, text, letter in (("tib", "t", wyl, "T"),
                                     ("eng", "e", eng, "E")):
        starts = {a: n for n, (a, _b) in enumerate(toks[side], 1)}
        ends = {b: n for n, (_a, b) in enumerate(toks[side], 1)}
        for sp in prop["spans"]:
            ref = sp.get(key)
            if ref is None or "[" in ref:
                continue
            a, b = chosen[sp["id"]][side]
            piece = text[a:b]
            k = text.find(piece, a + 1)
            while k >= 0 and not (k in starts and k + len(piece) in ends):
                k = text.find(piece, k + 1)
            if k < 0:
                continue
            i, j = starts[k], ends[k + len(piece)]
            new = "%s%d" % (letter, i) if i == j else \
                "%s%d-%s%d" % (letter, i, letter, j)
            drop = {sid for sid, c in chosen.items()
                    if sid != sp["id"] and c[side] is not None
                    and c[side][0] < k + len(piece) and k < c[side][1]
                    and not (sp["d"] == 7 and byid[sid]["d"] == 5
                             and sid in [q for q in chosen
                                         if sp["id"] in members(q)])}
            for pid in list(drop) + ([sp["id"]] if sp["d"] == 5 else []):
                if byid[pid]["d"] == 5:
                    drop |= members(pid)
            drop.discard(sp["id"])
            var = copy.deepcopy(prop)
            var["spans"] = [s for s in var["spans"] if s["id"] not in drop]
            for s in var["spans"]:
                if s["id"] == sp["id"]:
                    s[key] = new
            want = {sid: dict(c) for sid, c in chosen.items()
                    if sid not in drop}
            want[sp["id"]][side] = (k, k + len(piece))
            yield ("%s-%s" % ("member" if sp["d"] == 7 else "flat", side),
                   "%s %s %s->%s (dropped %d)"
                   % (sp["id"], side, ref, new, len(drop)), var, want)


def member_repoints(prop, wyl, eng, chosen):
    """Each d=7 member's English re-pointed to a word inside its parent whose
    text ALSO sits earlier in that parent.

    WHY. The generator places a member's English by a raw find inside its
    parent, starting after the member before it, with no word boundary --
    so "of" chosen second in "consisting of movement of the mind" is placed
    on the first, and "in" can land inside "mind". Only the round trip's
    member-English offset comparison refuses that, and the occurrence sweep
    never reaches it: no landed member's English recurs LATER inside its
    parent, so every member move it makes leaves the parent. With that
    comparison switched off the verifier compiled C05:103's member onto
    E51 and banked E49, every gate green. This makes the case systematic:
    one variant per member, the last such word, and each must be refused
    or resolve exactly as re-pointed."""
    et = tokenize(eng, "eng")
    byid = {sp["id"]: sp for sp in prop["spans"]}
    for sp in prop["spans"]:
        c = chosen[sp["id"]]
        if sp["d"] != 7 or c["eng"] is None:
            continue
        par = [pid for pid, pc in chosen.items()
               if byid[pid]["d"] == 5 and pc["eng"] is not None
               and pc["tib"][0] <= c["tib"][0] and c["tib"][1] <= pc["tib"][1]]
        if not par:
            continue
        pa, pb = chosen[par[0]]["eng"]
        taken = [oc["eng"] for sid, oc in chosen.items()
                 if sid not in (sp["id"], par[0]) and oc["eng"] is not None
                 and pa <= oc["eng"][0] and oc["eng"][1] <= pb]
        best = None
        for n, (x, y) in enumerate(et, 1):
            if not (pa <= x and y <= pb) or (x, y) in ((pa, pb), c["eng"]):
                continue
            if not _tokch(eng[x]) or any(u < y and x < v for u, v in taken):
                continue
            if eng.find(eng[x:y], pa, pb) < x:
                best = (n, x, y)
        if best is None:
            continue
        n, x, y = best
        var = copy.deepcopy(prop)
        for s in var["spans"]:
            if s["id"] == sp["id"]:
                s["e"] = "E%d" % n
        want = {sid: dict(oc) for sid, oc in chosen.items()}
        want[sp["id"]]["eng"] = (x, y)
        yield ("member-eng-repoint", "%s eng %s->E%d %r (parent %s)"
               % (sp["id"], sp.get("e"), n, eng[x:y], par[0]), var, want)


def readback(block):
    """{label: [(start, end, is_null)]} from a rendered block, by a plain scan.

    A SECOND reader, sharing no code with walk() or check_blocks(), so the
    occurrence sweep can hold the round trip's own comparisons to account: a
    compiler whose offset check was switched off would pass its own check,
    but not this one. Characters inside a null marker do not advance the
    position, because they are annotation, not spine text."""
    out, stack, pos, i, n = {}, [], 0, 0, len(block)
    while i < n:
        if block[i] != "<":
            j = block.find("<", i)
            j = n if j < 0 else j
            if not any(f[0] == "nul" for f in stack):
                pos += len(html.unescape(block[i:j]))
            i = j
            continue
        j = block.index(">", i) + 1
        tag = block[i:j]
        i = j
        if tag.startswith("</"):
            kind, lab, start, had_nul = stack.pop()
            if kind == "u":
                out.setdefault(lab, []).append((start, pos, had_nul))
            continue
        if 'class="nul"' in tag:
            for f in reversed(stack):
                if f[0] == "u":
                    f[3] = True
                    break
            stack.append(["nul", None, pos, False])
        else:
            lab = re.search(r'data-l="([^"]*)"', tag)
            stack.append(["u", lab.group(1) if lab else None, pos, False])
    return out


def sweep_block(res, key, course, prop, wyl, eng, chosen):
    """Run one landed block's occurrence variants; tally them into res."""
    sw = res["sweep"]
    seq = prop["seq"]
    variants = list(occurrence_variants(prop, wyl, eng, chosen)) + \
        list(member_repoints(prop, wyl, eng, chosen))
    for kind, what, var, want in variants:
        sw["variants"] += 1
        kk = sw["kinds"].setdefault(kind, {"variants": 0, "compiled": 0,
                                           "refused": {}})
        kk["variants"] += 1
        try:
            _whole, blks, _v = compile_with_blocks(
                {"course": course, "segments": [var]})
        except Refusal as e:
            sw["refused"][e.code] = sw["refused"].get(e.code, 0) + 1
            kk["refused"][e.code] = kk["refused"].get(e.code, 0) + 1
            continue
        sw["compiled"] += 1
        kk["compiled"] += 1
        tb, eb = blks[0]
        res["bodies"].append(("%s %s" % (key, what), tb, eb))
        got_t, got_e = readback(tb), readback(eb)
        for sid, c in want.items():
            lab = "s%d%s" % (seq, sid)
            t_ok = got_t.get(lab) == [(c["tib"][0], c["tib"][1], False)]
            if c["eng"] is None:
                e_ok = len(got_e.get(lab, [])) == 1 and got_e[lab][0][2]
            else:
                e_ok = got_e.get(lab) == [(c["eng"][0], c["eng"][1], False)]
            if not (t_ok and e_ok):
                sw["mismatch"].append(
                    "%s  moved %s: span %s was proposed at tib %s eng %s, "
                    "the generator put it at tib %s eng %s"
                    % (key, what, sid, c["tib"], c["eng"], got_t.get(lab),
                       got_e.get(lab)))
                break


def cut_variants(prop, wyl, eng, chosen):
    """The landed proposal with one span one character short, so it cuts.

    WHY. The subword flag is derived from the offsets, and deriving it turns
    OFF the generator's refusal of an accidental cut. So check_cuts is the
    only thing between an off-by-one bracket and a banked fragment, and two
    d=5 pins were all that held it: a mutant letting through every English
    cut of three characters or fewer, and one exempting depth 6, both
    passed every gate. This holds the rules on every round-tripping block
    instead. One variant per (kind, depth) per block, the first eligible
    span in Tibetan order, each a cut no rule allows:
      eng-end / eng-start  an English span losing its last / first letter
                           inside a word, never leaving an allowed affix or
                           stem          -> SUBWORD_NOT_ALLOWED
      tib-start            a depth 1-6 span starting one letter into its
                           syllable       -> FLAT_STARTS_MID_SYLLABLE
      tib-end              a depth 1-6 span stopping before a letter that is
                           not a Wylie suffix, before a bare s (rule 9), or
                           before its own final a-chung -> SYLLABLE_SPLIT
    A d=5 span with members is skipped: shortening it would put a member
    outside it, which is refused earlier, under another name."""
    tt, et = tokenize(wyl, "tib"), tokenize(eng, "eng")
    byid = {sp["id"]: sp for sp in prop["spans"]}
    parents = {pid for pid, pc in chosen.items() if byid[pid]["d"] == 5 and any(
        byid[m]["d"] == 7 and pc["tib"][0] <= mc["tib"][0]
        and mc["tib"][1] <= pc["tib"][1] for m, mc in chosen.items())}
    seen = set()
    order = sorted(prop["spans"], key=lambda s: chosen[s["id"]]["tib"])
    for sp in order:
        sid, c = sp["id"], chosen[sp["id"]]
        if sid in parents:
            continue
        cands = []
        if c["eng"] is not None and "[" not in (sp.get("e") or "["):
            a, b = c["eng"]
            for kind, na, nb, edge in (("eng-end", a, b - 1, b - 1),
                                       ("eng-start", a + 1, b, a + 1)):
                if nb > na and _cut(eng, edge) and \
                        eng[na:nb].lower() not in _ALLOWED_LOWER:
                    cands.append((kind, "e", eng, et, "E", na, nb,
                                  "SUBWORD_NOT_ALLOWED"))
        if sp["d"] != 7 and "[" not in sp["t"]:
            a, b = c["tib"]
            if b - a > 1 and _glued(wyl, a + 1):
                cands.append(("tib-start", "t", wyl, tt, "T", a + 1, b,
                              "FLAT_STARTS_MID_SYLLABLE"))
            nb = b - 1
            if nb > a:
                rest = re.match(r"[A-Za-z']*", wyl[nb:]).group(0)
                final_ac = (wyl[nb] == "'" and G.WORDCH.match(wyl[nb - 1])
                            and (nb + 1 >= len(wyl)
                                 or not _tokch(wyl[nb + 1])))
                if final_ac or (_cut(wyl, nb) and
                                (rest not in SUFFIX or rest == "s")):
                    cands.append(("tib-end", "t", wyl, tt, "T", a, nb,
                                  "SYLLABLE_SPLIT"))
        for kind, key, text, toks, letter, na, nb, expect in cands:
            if (kind, sp["d"]) in seen:
                continue
            try:
                ref, how = express(text, toks, na, nb, letter)
            except Inexpressible:
                continue
            if how != "bracket":
                continue
            seen.add((kind, sp["d"]))
            var = copy.deepcopy(prop)
            for s in var["spans"]:
                if s["id"] == sid:
                    s[key] = ref
            yield (kind, "%s d=%d %s->%s" % (sid, sp["d"], sp[key], ref),
                   var, expect)


def cut_block(res, key, course, prop, wyl, eng, chosen):
    """Run one landed block's cut variants; tally them into res."""
    cs = res["cuts"]
    for kind, what, var, expect in cut_variants(prop, wyl, eng, chosen):
        cs["variants"] += 1
        kk = cs["kinds"].setdefault(kind, {"variants": 0, "refused": {}})
        kk["variants"] += 1
        try:
            _whole, blks, _v = compile_with_blocks(
                {"course": course, "segments": [var]})
        except Refusal as e:
            cs["refused"][e.code] = cs["refused"].get(e.code, 0) + 1
            kk["refused"][e.code] = kk["refused"].get(e.code, 0) + 1
            if e.code != expect:
                cs["wrong_name"].append("%s  %s %s: refused as %s, the rule "
                                        "is %s" % (key, kind, what, e.code,
                                                   expect))
            continue
        cs["compiled"].append("%s  %s %s (the rule refuses it as %s)"
                              % (key, kind, what, expect))
        res["bodies"].append(("%s %s %s" % (key, kind, what),) + blks[0])


def post_gates(res):
    """Run test_no_broken_words and test_no_split_syllables, UNMODIFIED, over
    every page the sweeps compiled.

    WHY. The compiler imports those gates' lists and promises it can never
    admit a cut they refuse. A promise is not evidence: the first version
    folded case where test_no_broken_words does not, and a lone "Un"
    compiled onto a page that gate refuses. So each compiled variant is
    written out as a page and the two gates' own main() is run over them,
    with only their page directory pointed at a scratch folder. What they
    report is recorded as they report it."""
    out = res["post_gates"]
    with tempfile.TemporaryDirectory() as tmp:
        d = os.path.join(tmp, "data", "alignment", "sweep")
        os.makedirs(d)
        names = {}
        for i, (what, tb, eb) in enumerate(res["bodies"]):
            name = "v%05d" % i
            names[name] = what
            io.open(os.path.join(d, name + ".html"), "w", encoding="utf-8") \
                .write('<div class="pair">\n<div class="tib"><div class="lab">'
                       'T</div>\n%s\n</div>\n<div class="eng"><div class="lab">'
                       'E</div>\n%s\n</div>\n</div>\n' % (tb, eb))
        for mod in (NB, NS):
            saved = (mod.ROOT, mod.page_dirs)
            old_out = sys.stdout
            sys.stdout = buf = io.StringIO()
            mod.ROOT, mod.page_dirs = tmp, (lambda: ["sweep"])
            try:
                mod.main()
                code = 0
            except SystemExit as e:
                code = e.code if isinstance(e.code, int) else 1
            finally:
                mod.ROOT, mod.page_dirs = saved
                sys.stdout = old_out
            text = buf.getvalue()
            m = re.search(r"across (\d+) pages", text)
            bad = [ln for ln in text.splitlines() if ln.startswith("  v")]
            out[mod.__name__] = {
                "exit": code, "pages": int(m.group(1)) if m else None,
                "written": len(names),
                "findings": ["%s  (%s)" % (ln.strip(), names.get(
                    ln.split()[0].replace(".html", ""), "?")) for ln in bad]}
    res["bodies"] = len(res["bodies"])


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
        # a syllable-final a-chung (mtha') is never a token of its own: a
        # plain range must not be able to drop it
        if (side == "tib" and text[a:b] == "'" and a > 0
                and G.WORDCH.match(text[a - 1])):
            return ("the syllable-final a-chung at %d is split from its "
                    "syllable (%r)" % (a, text[max(0, a - 6):b + 1]))
        cur = b
    if text[cur:].strip():
        return "trailing characters %r are not covered" % text[cur:]
    return None


def load_baseline():
    if not os.path.exists(BASELINE):
        return None
    return json.load(io.open(BASELINE, encoding="utf-8"))


def _excused(blk):
    return blk["class"] in PROTOCOL_SHAPES.get(blk["kind"], ())


def judge(res, base):
    """The gate's verdict. Returns a list of failure lines (empty = pass).

    NEW PAGES MUST ROUND-TRIP. A block that fails is excused only if (a) it
    carries one of the two hand-built protocol labels and fails with THE
    shape that defines that protocol (PROTOCOL_SHAPES), or (b) it is named,
    with that exact class, in the frozen baseline of legacy blocks. The baseline may neither grow nor
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
    else:
        # The name is what stales an outstanding proposal, and bumping it is
        # a manual act. So the boundaries are checked, not the name: under
        # the SAME name the canary and every landed segment whose src is
        # unchanged must cut exactly as they did when the baseline was taken.
        if base.get("canary") != canary_digest():
            bad.append("TOKENIZER CHANGED WITHOUT A NAME BUMP: the canary "
                       "strings now cut differently (digest %s, baseline %s) "
                       "but the tokenizer is still called %s. Bump TOKENIZER "
                       "so every outstanding proposal is refused as stale, "
                       "then re-derive the baseline."
                       % (canary_digest(), base.get("canary"), TOKENIZER))
        moved, respun = [], []
        for k, v in sorted(base.get("token_digests", {}).items()):
            now = res["token_digests"].get(k)
            if now is None or now == v:
                continue
            if now.split(":")[0] == v.split(":")[0]:
                moved.append(k)
            else:
                respun.append(k)
        if moved:
            bad.append("TOKENIZER CHANGED WITHOUT A NAME BUMP: %d landed "
                       "segment(s) keep the same src but cut into different "
                       "tokens, e.g. %s. A proposal written yesterday would "
                       "compile against token numbers that now mean something "
                       "else. Bump TOKENIZER, then re-derive the baseline."
                       % (len(moved), ", ".join(moved[:5])))
        if respun:
            bad.append("SPINE TEXT CHANGED under %d landed segment(s) since "
                       "the baseline, e.g. %s: their src no longer matches. "
                       "Find out why before re-deriving the baseline."
                       % (len(respun), ", ".join(respun[:5])))
        if not base.get("token_digests"):
            bad.append("the baseline pins no token boundaries: re-derive it")
    sw = res["sweep"]
    if sw["mismatch"]:
        bad.append("OCCURRENCE SWEEP: %d variant(s) compiled to a spec that "
                   "resolves somewhere other than proposed -- the compiler "
                   "emitted a spec that means something else. First: %s"
                   % (len(sw["mismatch"]), sw["mismatch"][0]))
    if sw["variants"] == 0 or sw["compiled"] == 0 or \
            not sw["refused"].get("ROUND_TRIP_MISMATCH"):
        bad.append("OCCURRENCE SWEEP examined too little to mean anything: "
                   "%d variants, %d compiled and read back, %d refused as "
                   "unreachable. It must meet both outcomes."
                   % (sw["variants"], sw["compiled"],
                      sw["refused"].get("ROUND_TRIP_MISMATCH", 0)))
    # A generator refusal the compiler did not name is a shape it lets
    # through to the authority unexplained: the task is to catch those at
    # compile time with a message that says what to change.
    for code in ("COMPILER_FAULT", "GENERATOR_CRASHED", "GENERATOR_REFUSED"):
        if sw["refused"].get(code):
            bad.append("OCCURRENCE SWEEP: %d variant(s) ended in %s"
                       % (sw["refused"][code], code))
        if res["cuts"]["refused"].get(code):
            bad.append("CUT SWEEP: %d variant(s) ended in %s"
                       % (res["cuts"]["refused"][code], code))
    rp = sw["kinds"].get("member-eng-repoint", {})
    if not rp.get("variants") or not rp.get("refused", {}).get(
            "ROUND_TRIP_MISMATCH"):
        bad.append("MEMBER RE-POINT examined too little to mean anything: %d "
                   "variants, %d refused as unreachable. The member-English "
                   "offset check must meet its case on whole-bank evidence."
                   % (rp.get("variants", 0),
                      rp.get("refused", {}).get("ROUND_TRIP_MISMATCH", 0)))
    # the packet: every landed segment, read back as an agent reads it
    if res["packets"] == 0 or res["packets"] != len(res["token_digests"]):
        bad.append("PACKET CONTRACT examined %d packets for %d landed "
                   "segments" % (res["packets"], len(res["token_digests"])))
    if res["packet_errors"]:
        bad.append("PACKET CONTRACT: on %d segment(s) the numbers an agent is "
                   "shown are not the numbers the compiler resolves: %s"
                   % (len(res["packet_errors"]), res["packet_errors"][0]))
    elif res["packet_tokens"] != res["tokens"] or not res["tokens"]:
        bad.append("PACKET CONTRACT resolved %d shown numbers, but the landed "
                   "segments hold %d tokens: it did not look at them all"
                   % (res["packet_tokens"], res["tokens"]))
    # the cut sweep: every forbidden cut refused, under the rule's own name
    cs = res["cuts"]
    for kind in ("eng-end", "eng-start", "tib-start", "tib-end"):
        if not cs["kinds"].get(kind, {}).get("variants"):
            bad.append("CUT SWEEP made no %s variant: it examined nothing "
                       "there" % kind)
    if cs["compiled"]:
        bad.append("CUT SWEEP: %d variant(s) with a cut no rule allows "
                   "COMPILED, and the subword flag derived for them switches "
                   "off the generator's own refusal. First: %s"
                   % (len(cs["compiled"]), cs["compiled"][0]))
    if cs["wrong_name"]:
        bad.append("CUT SWEEP: %d forbidden cut(s) refused under the wrong "
                   "name. First: %s" % (len(cs["wrong_name"]),
                                        cs["wrong_name"][0]))
    # the post-landing gates, run unmodified over what the sweeps compiled
    for name in (NB.__name__, NS.__name__):
        if name not in res["post_gates"]:
            bad.append("POST-LANDING %s was never run over the sweeps' pages"
                       % name)
    for name, g in sorted(res["post_gates"].items()):
        if g["written"] == 0:
            bad.append("POST-LANDING %s was given no compiled page" % name)
        elif g["exit"] != 0 or g["findings"]:
            bad.append("POST-LANDING %s REFUSES %d span(s) the compiler "
                       "admitted: %s" % (name, len(g["findings"]),
                                         (g["findings"] or ["exit %s" %
                                                            g["exit"]])[0]))
        elif g["pages"] != g["written"]:
            bad.append("POST-LANDING %s read %s of the %d compiled pages"
                       % (name, g["pages"], g["written"]))
    # the rule-shape census is the scope of a ruling; it may not drift
    if base.get("shapes") is None:
        bad.append("the baseline carries no rule-shape census: re-derive it")
    elif res["shapes"] != base["shapes"]:
        gone = sorted(set(base["shapes"]) - set(res["shapes"]))
        came = sorted(set(res["shapes"]) - set(base["shapes"]))
        diff = sorted(k for k in set(res["shapes"]) & set(base["shapes"])
                      if res["shapes"][k] != base["shapes"][k])
        bad.append("RULE-SHAPE CENSUS CHANGED (re-derive the baseline with "
                   "--write-baseline once the change is understood): %d "
                   "block(s) new, e.g. %s; %d gone, e.g. %s; %d changed, "
                   "e.g. %s" % (len(came), came[:3], len(gone), gone[:3],
                                len(diff), diff[:3]))
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
        inst = res["side_instances"][code]
        # where the sides sit: the class and kind of the block holding each,
        # so a description of a class can be checked, not taken on trust
        where, per_dir = {}, {}
        for x in inst:
            key = x.split("  ", 1)[0]
            b = res["blocks"].get(key, {})
            wk = "%s/%s" % (b.get("class"), b.get("kind"))
            where[wk] = where.get(wk, 0) + 1
            dd = key.split("/", 1)[0]
            per_dir[dd] = per_dir.get(dd, 0) + 1
        w("    NOT EXPRESSIBLE %-22s %6d  by depth %s\n"
          % (code, sides[code], json.dumps(res["side_depths"][code],
                                           sort_keys=True)))
        w("      sides by directory %s\n      sides by the class/kind of the "
          "block holding them %s\n" % (json.dumps(per_dir, sort_keys=True),
                                  json.dumps(where, sort_keys=True)))
        if len(inst) <= 10:
            for x in inst:
                w("      %s\n" % x[:170])
        else:
            w("      e.g. %s\n" % inst[0][:170])
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
    sw = res["sweep"]
    w("\n  OCCURRENCE SWEEP (one span side moved to a later occurrence of its "
      "own text, on every block that round-trips): %d variants\n"
      % sw["variants"])
    w("    compiled, and read back by an independent reader at exactly the "
      "moved offsets: %d; resolved elsewhere: %d\n"
      % (sw["compiled"] - len(sw["mismatch"]), len(sw["mismatch"])))
    w("    refused by name: %s\n" % json.dumps(sw["refused"], sort_keys=True))
    for kind, kk in sorted(sw["kinds"].items()):
        w("      %-20s %5d variants, %5d compiled, refused %s\n"
          % (kind, kk["variants"], kk["compiled"],
             json.dumps(kk["refused"], sort_keys=True)))
    cs = res["cuts"]
    w("\n  CUT SWEEP (one span one character short, cutting a word or a "
      "syllable no rule allows): %d variants, %d compiled, %d refused under "
      "another name\n" % (cs["variants"], len(cs["compiled"]),
                          len(cs["wrong_name"])))
    for kind, kk in sorted(cs["kinds"].items()):
        w("      %-12s %5d variants, refused %s\n"
          % (kind, kk["variants"], json.dumps(kk["refused"], sort_keys=True)))
    w("\n  POST-LANDING GATES, unmodified, over the %s pages the sweeps "
      "compiled:\n" % res["bodies"])
    for name, g in sorted(res["post_gates"].items()):
        w("    %-24s exit %s, read %s of %d pages, %d finding(s)\n"
          % (name, g["exit"], g["pages"], g["written"], len(g["findings"])))
    w("\n  PACKET CONTRACT (each landed segment's packet read back as text, "
      "every number resolved by the compiler): %d packets, %d of %d tokens "
      "resolved as shown, %d packets disagree\n"
      % (res["packets"], res["packet_tokens"], res["tokens"],
         len(res["packet_errors"])))
    w("\n  RULE SHAPES ON EVERY LANDED BLOCK, whatever its first refusal "
      "(the scope of the rulings on rules 3, 8 and 9):\n")
    for code in sorted({c for v in res["shapes"].values() for c in v}):
        keys = sorted(k for k, v in res["shapes"].items() if code in v)
        per_dir, per_cls = {}, {}
        for k in keys:
            dd = k.split("/", 1)[0]
            per_dir[dd] = per_dir.get(dd, 0) + 1
            fate = res["blocks"].get(k, {}).get("class", "?")
            per_cls[fate] = per_cls.get(fate, 0) + 1
        w("    %-26s %4d blocks, %5d spans\n      by directory %s\n      "
          "by what the round trip calls the block %s\n"
          % (code, len(keys), sum(res["shapes"][k][code] for k in keys),
             json.dumps(per_dir, sort_keys=True),
             json.dumps(per_cls, sort_keys=True)))
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
                "corpus marker) are excused by that label only when they "
                "fail with that protocol's defining shape; any other failure "
                "on them is listed here like every other legacy block. The "
                "FLAT_STARTS_MID_SYLLABLE, SYLLABLE_SPLIT, SUBWORD_NOT_ALLOWED "
                "and RESERVED_CLS entries are different: landed blocks whose "
                "shape breaks a written rule, which the compiler refuses. "
                "They are frozen so the gate can run, not accepted; they "
                "await a ruling, and the scope of that ruling is `shapes`, "
                "not this list.",
        "_shapes": "block -> {cut rule it breaks: spans}, for EVERY landed "
                   "block, read loosely from the page whatever the block's "
                   "first refusal. The round trip names only the first "
                   "refusal, so this, not `segments`, is how many blocks a "
                   "ruling on rules 3, 8 or 9 touches.",
        "shapes": dict(sorted(res["shapes"].items())),
        "tokenizer": TOKENIZER,
        "canary": canary_digest(),
        "segments_ok": res["segments_ok"],
        "classes": {k: {"segments": v["segments"], "spans": v["spans"],
                        "kinds": v["kinds"]}
                    for k, v in sorted(res["classes"].items())},
        "segments": frozen,
        "_token_digests": "course:seq -> src:boundaries, for every landed "
                          "segment. Same src with different boundaries is a "
                          "tokenizer that changed without a name bump.",
        "token_digests": dict(sorted(res["token_digests"].items())),
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
        print("\n  every landed segment round-trips or is named, with its "
              "class, in the frozen baseline")
        return 0
    if a.cmd == "measure":
        measure()
        return 0
    ap.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
