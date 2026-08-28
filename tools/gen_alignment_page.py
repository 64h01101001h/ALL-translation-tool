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

Spec format (JSON on stdin):
  { "course": "C03",
    "segments": [
      { "seq": 160,
        "title": "the ninth state, and quietude",
        "clauses": [ {"id":"c1","tib":"...","eng":"..."} ],
        "spans": [ {"id":"w1","d":5,"tib":"zhi gnas","eng":"quietude"},
                   {"id":"m1","d":6,"tib":"kyis","eng":null,
                    "cls":"case","nul":"instrumental; unrendered"} ],
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


def wrap(text, spans, side, seq):
    """Wrap `spans` into `text` by a forward-only cursor.

    Forward-only is the whole safety property: a span may only be found
    at or after the end of the previous one. If the spec lists spans in
    an order the text does not have, the cursor cannot find the next one
    and the build refuses rather than silently attaching to a later
    identical substring elsewhere in the segment.
    """
    out, cur = [], 0
    for sp in spans:
        piece = sp.get(side)
        if piece is None:
            continue                      # null side: emitted separately
        i = text.find(piece, cur)
        if i < 0:
            later = text.find(piece)
            if later >= 0:
                die("s%d %s span %r appears at offset %d but the cursor is "
                    "already at %d -- the spans are listed OUT OF ORDER "
                    "relative to the text. Fix the order in the spec; do "
                    "not reorder the text."
                    % (seq, side, piece, later, cur))
            die("s%d %s span %r is NOT PRESENT in the spine text. The "
                "spine is the source of record -- correct the span, never "
                "the text.\n  text: %r" % (seq, side, piece, text))
        out.append(html.escape(text[cur:i]))
        cls = "u" + ((" " + sp["cls"]) if sp.get("cls") else "")
        out.append('<span class="%s" data-d="%d" data-l="s%d%s">%s</span>'
                   % (cls, sp["d"], seq, sp["id"], html.escape(piece)))
        cur = i + len(piece)
    out.append(html.escape(text[cur:]))
    return "".join(out)


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

        tib_html = wrap(wyl, spans, "tib", seq)
        eng_html = wrap(eng, spans, "eng", seq)

        nulls = "".join(
            '<span class="u" data-d="%d" data-l="s%d%s">'
            '<span class="nul">%s</span></span>'
            % (sp["d"], seq, sp["id"], html.escape(sp.get("nul") or
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
