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
                   {"id":"m1","d":6,"tib":"kyis","eng":null,
                    "cls":"case","nul":"instrumental; unrendered"} ],
        "eng_order": ["w2","w1"],
        "note": "AI NOTE body, may contain HTML" } ] }

A span with "eng": null is a Tibetan morpheme with no English exponent;
it is wrapped on the Tibetan side and emitted as a .nul marker on the
English side. Null markers are stripped before the verbatim gate in
build_alignment_layer.py, so they never affect the proof.
"""
import sys, io, json, os, sqlite3, html

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402  (for SPINE only)


def die(msg):
    sys.exit("REFUSED: " + msg)


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
            i = text.find(piece, cur)
            if i < 0:
                later = text.find(piece)
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
            need = {sp["id"] for sp in spans if sp.get("eng") is not None}
            got = set(order)
            if need - got:
                die("s%d eng_order omits span(s) that have English: %s. Every "
                    "span with an eng value must appear, or it would be "
                    "silently dropped from the English block."
                    % (seq, ", ".join(sorted(need - got))))
            if got - need:
                die("s%d eng_order names span(s) with no English: %s"
                    % (seq, ", ".join(sorted(got - need))))
            eng_spans = [by_id[i] for i in order]
        else:
            # Derive the English order -- but ONLY where the derivation is
            # not a guess. If a span's English occurs more than once, or two
            # spans would claim overlapping characters, the generator cannot
            # know the author's intent and refuses rather than picking. This
            # is the "shog also occurs inside tshogs" class of error, and it
            # is precisely where silent guessing does damage.
            have = [sp for sp in spans if sp.get("eng") is not None]
            pos = []
            for sp in have:
                n = eng.count(sp["eng"])
                if n == 0:
                    die("s%d eng span %r is NOT PRESENT in the spine text."
                        % (seq, sp["eng"]))
                if n > 1:
                    die("s%d eng span %r occurs %d times; the English order "
                        "cannot be derived without guessing which one span "
                        "%s means. Supply an explicit \"eng_order\"."
                        % (seq, sp["eng"], n, sp["id"]))
                i = eng.find(sp["eng"])
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
            eng_spans = [t[2] for t in pos]
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
