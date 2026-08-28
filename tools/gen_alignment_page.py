#!/usr/bin/env python3
"""Generate one alignment page body from the spine, both sides at once.

THE METHOD, and why it is the method (Adam 2026-08-27):
both the Tibetan and the English block are generated FROM THE SPINE TEXT
by wrapping spans in place. Neither side is retyped. A span that is not
found verbatim in the spine text refuses the build, and spans must appear
in the order the spec lists them -- a cursor walks forward and never
rewinds. So a page cannot drift from the corpus by construction, and a
span silently attaching to a LATER identical substring is impossible.

This refusal has caught three real authoring errors (C03 batches 40, 43,
52). It lived in a scratchpad heredoc until 2026-08-28; it is a tool now
because the thing that catches the errors should not be ephemeral.

TWO ORDERINGS, and why (found the hard way, 2026-08-28):
Tibetan and English word order differ -- that is the entire point of the
exercise -- so the English spans routinely CROSS. A single forward-only
cursor shared by both sides refuses every real page. The spec therefore
carries the spans in TIBETAN order, and an optional "eng_order" listing
the same span ids in ENGLISH order. Each side then gets its own
forward-only cursor and keeps its own out-of-order refusal. Omit
eng_order only when the segment genuinely has no crossing.

Spec format (JSON on stdin):
  { "course": "C03",
    "segments": [
      { "seq": 160,
        "title": "the ninth state, and quietude",
        "clauses": [ {"id":"c1","tib":"...","eng":"..."} ],
        "spans": [ {"id":"w1","d":5,"tib":"zhi gnas","eng":"quietude"},
                   {"id":"m2","d":6,"tib":"ma","eng":"n't",
                    "subword":true},   # declared sub-word: see find_word
                   {"id":"m1","d":6,"tib":"kyis","eng":null,
                    "cls":"case","nul":"instrumental; unrendered"} ],
        "eng_order": ["w2","w1"],
        "note": "AI NOTE body, may contain HTML" } ] }

A span with "eng": null is a Tibetan morpheme with no English exponent;
it is wrapped on the Tibetan side and emitted as a .nul marker on the
English side. Null markers are stripped before the verbatim gate in
build_alignment_layer.py, so they never affect the proof.
"""
import sys, io, json, os, re, sqlite3, html

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402  (for SPINE only)


def die(msg):
    sys.exit("REFUSED: " + msg)


WORDCH = re.compile(r"[A-Za-z0-9\u0F00-\u0FFF]")


def find_word(text, piece, start, stop=None, subword=False):
    """find(), but a FLAT span may not match inside a larger word.

    `par shing` is rendered "block", and "block" also sits inside
    "blockprint" twenty characters earlier. A raw substring search takes the
    interior hit, consumes half of another word, and cascades. This is the
    same failure that once attached the ACIP of `tshogs` to the span `shog`.

    Depth-7 members are exempt: a member IS a sub-part of its parent, and
    the genitive 'i inside pa'i has a letter on its left by definition.

    A span may also opt in with "subword": true, which is how the campaign
    aligns English negative affixes. The Tibetan negation mi/ma maps to the
    "n't" of "doesn't" and to the "un" of "unnecessary", so the positive
    stem is spanned on its own and deliberately sits inside a larger word.
    That convention is used on 14 spans across C01 and is correct. The
    default refusal exists to catch the ACCIDENT -- `one-pointed` cut out
    of "one-pointedly" -- while a declared intent passes.
    """
    if subword:
        i = text.find(piece, start, len(text) if stop is None else stop)
        return i
    stop = len(text) if stop is None else stop
    i = start
    while True:
        i = text.find(piece, i, stop)
        if i < 0:
            return -1
        # A boundary is required only on an edge whose OWN character is
        # alphanumeric -- that is the only way a span can sit inside a
        # larger word. A span that starts with an apostrophe ('i) or ends
        # with a slash (1a/) has no such edge and needs no boundary there,
        # which is why the naive rule broke every genitive and folio marker
        # the first time it ran.
        j = i + len(piece)
        need_before = bool(WORDCH.match(piece[0]))
        need_after = bool(WORDCH.match(piece[-1]))
        before = (not need_before) or i == 0 or not WORDCH.match(text[i - 1])
        after = (not need_after) or j >= len(text) or not WORDCH.match(text[j])
        if before and after:
            return i
        i += 1


def count_word(text, piece):
    """How many times `piece` occurs as a whole word."""
    n, i = 0, 0
    while True:
        i = find_word(text, piece, i)
        if i < 0:
            return n
        n += 1
        i += 1


def with_members(spec_spans, flat, text=None):
    """Re-insert depth-7 members after the parent the SPEC gave them.

    Parenthood is a fact about the spec's Tibetan order, not about whichever
    depth-5 span happens to sort in front of a member on the other side.
    Sorting the English flat put the member `bring your mind back to the
    object` behind the unrelated span `effort` and the nesting check
    correctly refused it. Found 2026-08-28 on C03:173.

    Members can also CROSS inside their own parent: `kha dog nag pa` is
    "black color", where the member `nag pa` (black) precedes `kha dog`
    (color) in the English but follows it in the Tibetan. So when a `text`
    is given -- the English -- the members of each parent are ordered by
    their position in that text rather than by Tibetan order.
    """
    kids = {}
    parent = None
    for sp in spec_spans:
        if sp["d"] == 7:
            if parent is not None:
                kids.setdefault(parent["id"], []).append(sp)
        else:
            parent = sp
    out = []
    for sp in flat:
        out.append(sp)
        ks = [k for k in kids.get(sp["id"], []) if k.get("eng") is not None]
        if text is not None:
            ks.sort(key=lambda k: text.find(k["eng"]))
        out.extend(ks)
    return out


def resolve(text, spans, side, seq):
    """Resolve each span to a (start, end) range in `text`.

    Depth 5 and 6 spans form a FLAT sequence walked by a forward-only
    cursor -- that is the safety property, and it refuses out-of-order
    specs.

    Depth 7 spans are members INSIDE a depth-5 compound, so they must be
    resolved within their parent's range, not from the flat cursor.
    Searching them flat is actively wrong: the member `gnas` of the
    compound `sems gnas` matched the unrelated `zhi gnas` 130 characters
    later, and the cursor then rejected everything after it. Found
    2026-08-28 on C03:160.
    """
    ranges, cur, parent = {}, 0, None
    prev5 = None
    for sp in spans:
        piece = sp.get(side)
        if piece is None:
            continue
        if sp["d"] == 7:
            if parent is None:
                die("s%d %s span %s is depth 7 but no depth-5 compound "
                    "precedes it to nest inside." % (seq, side, sp["id"]))
            lo, hi = parent
            i = text.find(piece, ranges.get("_inner", lo), hi)
            if i < 0:
                die("s%d %s member %r (%s) is not inside its compound %r. A "
                    "depth-7 span must be a member of the depth-5 span above "
                    "it." % (seq, side, piece, sp["id"], text[lo:hi]))
            ranges["_inner"] = i + len(piece)
        else:
            # The commonest authoring mistake by far, and it must be caught
            # BEFORE searching: a morpheme BOUND inside the word above it,
            # marked depth 6 (flat, standalone) instead of depth 7 (member).
            # Listed flat, a bare syllable like 'i or pa or s usually MATCHES
            # -- somewhere else in the segment entirely -- so waiting for the
            # cursor to fail catches it only by luck, and blames whichever
            # innocent span fails next. Refuse on the depth itself.
            if (side == "tib" and prev5 is not None
                    and piece in prev5.get("tib", "")
                    and piece != prev5.get("tib")):
                die("s%d span %s (%r) is marked depth %d, but it is part of "
                    "%r -- the depth-5 span listed above it. A BOUND "
                    "morpheme must be depth 7, so it resolves inside its "
                    "parent. Listed flat it matches a different occurrence "
                    "elsewhere in the segment and silently mis-attaches. "
                    "Change %s to \"d\": 7."
                    % (seq, sp["id"], piece, sp["d"], prev5["tib"], sp["id"]))
            i = find_word(text, piece, cur, subword=sp.get("subword"))
            if i < 0:
                later = find_word(text, piece, 0, subword=sp.get("subword"))
                if later >= 0:
                    die("s%d %s span %r appears at offset %d but the cursor "
                        "is already at %d -- the spans are listed OUT OF "
                        "ORDER relative to the text. Fix the order in the "
                        "spec; do not reorder the text."
                        % (seq, side, piece, later, cur))
                die("s%d %s span %r is NOT PRESENT in the spine text. The "
                    "spine is the source of record -- correct the span, "
                    "never the text.\n  text: %r" % (seq, side, piece, text))
            cur = i + len(piece)
            if sp["d"] == 5:
                parent = (i, cur)
                prev5 = sp
                ranges["_inner"] = i
        ranges[sp["id"]] = (i, i + len(piece))
    ranges.pop("_inner", None)
    return ranges


def emit(text, spans, ranges, side):
    """Emit `text` with spans wrapped, nesting by containment."""
    items = []
    for sp in spans:
        if sp["id"] in ranges:
            lo, hi = ranges[sp["id"]]
            items.append((lo, -(hi - lo), sp, hi))
    # sort outermost-first at a shared start; the explicit key keeps the
    # comparison off the dicts, which raises TypeError on any tie.
    items.sort(key=lambda t: (t[0], t[1], t[2]["id"]))

    def render(lo, hi, pool):
        out, cur = [], lo
        i = 0
        while i < len(pool):
            a, _, sp, b = pool[i]
            if a < cur:
                i += 1
                continue
            kids = []
            j = i + 1
            while j < len(pool) and pool[j][0] < b:
                kids.append(pool[j])
                j += 1
            cls = "u" + ((" " + sp["cls"]) if sp.get("cls") else "")
            out.append(html.escape(text[cur:a]))
            out.append('<span class="%s" data-d="%d" data-l="%s">%s</span>'
                       % (cls, sp["d"], sp["_lab"], render(a, b, kids)))
            cur = b
            i = j
        out.append(html.escape(text[cur:hi]))
        return "".join(out)

    return render(0, len(text), items)


def nest(inner, seq, clauses, text, side):
    """Wrap the clause (d=2) and sentence (d=1) layers around `inner`."""
    if not clauses:
        return ('<span class="u" data-d="1" data-l="s%dS">%s</span>'
                % (seq, inner))
    # clause boundaries are located in the ORIGINAL text, then the same
    # offsets are used to cut the already-wrapped string -- so we cut on
    # markers instead, which the caller guarantees are span boundaries.
    return ('<span class="u" data-d="1" data-l="s%dS">%s</span>' % (seq, inner))


def main():
    spec = json.load(sys.stdin)
    course = spec["course"]
    db = sqlite3.connect(B.SPINE)
    c = db.cursor()
    body = []
    for seg in spec["segments"]:
        seq = seg["seq"]
        c.execute("select wylie, english from corpus_segments "
                  "where course=? and seq=?", (course, seq))
        row = c.fetchone()
        if not row:
            die("%s:%d is not in the spine" % (course, seq))
        wyl, eng = row
        spans = seg["spans"]

        for sp in spans:
            sp["_lab"] = "s%d%s" % (seq, sp["id"])
        tib_html = emit(wyl, spans, resolve(wyl, spans, "tib", seq), "tib")

        order = seg.get("eng_order")
        if order:
            by_id = {sp["id"]: sp for sp in spans}
            unknown = [i for i in order if i not in by_id]
            if unknown:
                die("s%d eng_order names span(s) that do not exist: %s"
                    % (seq, ", ".join(unknown)))
            # depth-7 members are positioned by their PARENT, not by the
            # flat order, so eng_order neither needs nor accepts them.
            need = {sp["id"] for sp in spans
                    if sp.get("eng") is not None and sp["d"] != 7}
            got = set(order)
            if need - got:
                die("s%d eng_order omits span(s) that have English: %s. Every "
                    "span with an eng value must appear, or it would be "
                    "silently dropped from the English block."
                    % (seq, ", ".join(sorted(need - got))))
            if got - need:
                die("s%d eng_order names span(s) with no English: %s"
                    % (seq, ", ".join(sorted(got - need))))
            eng_spans = with_members(spans, [by_id[i] for i in order], eng)
        else:
            # Derive the English order -- but ONLY where the derivation is
            # not a guess. If a span's English occurs more than once, or two
            # spans would claim overlapping characters, the generator cannot
            # know the author's intent and refuses rather than picking. This
            # is the "shog also occurs inside tshogs" class of error, and it
            # is precisely where silent guessing does damage.
            # depth-7 members are re-inserted after their parent by
            # with_members(); including them in the flat sort would place
            # them twice and under the wrong parent.
            have = [sp for sp in spans
                    if sp.get("eng") is not None and sp["d"] != 7]
            pos = []
            for sp in have:
                n = (eng.count(sp["eng"]) if sp.get("subword")
                     else count_word(eng, sp["eng"]))
                if n == 0:
                    die("s%d eng span %r is NOT PRESENT in the spine text."
                        % (seq, sp["eng"]))
                if n > 1:
                    die("s%d eng span %r occurs %d times; the English order "
                        "cannot be derived without guessing which one span "
                        "%s means. Supply an explicit \"eng_order\"."
                        % (seq, sp["eng"], n, sp["id"]))
                i = find_word(eng, sp["eng"], 0, subword=sp.get("subword"))
                pos.append((i, i + len(sp["eng"]), sp))
            # outermost first at a tie, so a compound precedes its members;
            # never let the tuple comparison fall through to the dicts.
            pos.sort(key=lambda t: (t[0], -(t[1] - t[0]), t[2]["id"]))
            for k in range(1, len(pos)):
                a0, a1, A = pos[k - 1]
                b0, b1, Bsp = pos[k]
                if b0 >= a1:
                    continue
                # a depth-7 member legitimately sits inside its compound
                if Bsp["d"] == 7 and b0 >= a0 and b1 <= a1:
                    continue
                if A["d"] == 7 and a0 >= b0 and a1 <= b1:
                    continue
                die("s%d English spans %s (%r) and %s (%r) overlap. Two "
                    "spans cannot own the same characters, and neither is a "
                    "depth-7 member of the other."
                    % (seq, A["id"], A["eng"], Bsp["id"], Bsp["eng"]))
            eng_spans = with_members(spans, [t[2] for t in pos], eng)
        eng_html = emit(eng, eng_spans,
                        resolve(eng, eng_spans, "eng", seq), "eng")

        nulls = "".join(
            '<span class="u" data-d="%d" data-l="%s">'
            '<span class="nul">%s</span></span>'
            % (sp["d"], sp["_lab"], html.escape(sp.get("nul") or
               (sp["tib"] + "→∅")))
            for sp in spans if sp.get("eng") is None)

        body.append('<h2>Segment %d &mdash; %s</h2>' % (seq, seg["title"]))
        body.append('<div class="pair">')
        body.append('<div class="tib"><div class="lab">TIBETAN (WYLIE)</div>')
        body.append(nest(tib_html, seq, seg.get("clauses"), wyl, "tib"))
        body.append('</div>')
        body.append('<div class="eng"><div class="lab">'
                    'GESHE MICHAEL\'S ENGLISH (VERBATIM)</div>')
        body.append(nest(eng_html + nulls, seq, seg.get("clauses"), eng, "eng"))
        body.append('</div>')
        body.append('</div>')
        if seg.get("note"):
            body.append('<div class="note"><b>AI NOTE</b> &mdash; %s</div>'
                        % seg["note"])
    sys.stdout.write("\n".join(body) + "\n")


if __name__ == "__main__":
    main()
