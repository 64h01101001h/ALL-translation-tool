#!/usr/bin/env python3
"""Every quote in the errata register must still be in the spine.

The register is the artifact that leaves this building: ALL's document
editors, the data-pipeline maintainers and the director are each meant to
act on a table of "what it says / what it should say". A quote that has
gone stale — because the spine was rebuilt, a segment renumbered, or the
entry was recovered from prose — makes us the unreliable party in a
conversation about someone else's errors.

merge_errata.py checks a quote on the way IN. Nothing re-checked one after
the spine moved underneath it. This does, on every run.

The register quotes with conventions, and the gate has to know them or it
will cry stale over perfectly good rows:

  composite   wylie (ACIP: X)   ·   acip: X | wylie: Y
  elision     A ... B           — two non-adjacent parts of one segment
  enumerated  (a) "A" (b) "B"   — several quoted fragments in one row
  narrative   prose with 'quoted spans' inside it

So it splits a row into fragments and requires EVERY fragment to be present
at the cited segment, in the wylie, the English or the ACIP. A row it cannot
parse into a checkable fragment is counted and NAMED, never silently passed —
the register's whole value is that it does not quietly overstate itself.
"""
import io, json, os, re, sqlite3, sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SIDE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    ROOT, 'docs/errata_register.json')
SPINE = sys.argv[2] if len(sys.argv) > 2 else os.path.join(
    ROOT, 'build/hgm_spine_v27_2.db')

# Measured 2026-08-28 over 91 entries: 84 carry a single-segment citation and
# every fragment of all 84 is present. The other 7 are 4 UNVERIFIED rows and
# 3 class entries that cite an extent rather than one segment.
# not-a-single-segment-citation moved 5 -> 10 on 2026-09-17, and it is a
# RE-BASE rather than slack: five LAYER_ARTEFACT rows that cite an extent
# ("class entry (29 fields)", "C01:45, C01:38") were previously hidden by a
# skip keyed on KIND, so the ceiling was set while they were invisible. They
# are a real and permanent category -- an extent cannot be quote-checked
# against one segment -- and the seven CHECKABLE rows the same fix exposed
# are now checked and pass.
CEILING = {
    'QUOTE-NOT-AT-SEGMENT': 0,   # a stale quote. Never raise this.
    'SEGMENT-GONE': 0,           # cites a segment the spine no longer has
    'unparseable-quote': 0,      # no checkable fragment could be derived
    # The 4 UNVERIFIED rows. Class entries that cite an extent used to land
    # here too, but they are all LAYER_ARTEFACT and are now skipped above, so
    # the ceiling comes back down from 9 to 4 rather than being left loose
    # enough to hide real growth. It rises only when a non-layer class entry
    # is added deliberately, never to quiet a failure.
    # 4 -> 5 on 2026-09-01 for E-107, an INGEST_ARTEFACT class entry (186
    # segments end at exactly 1,500 characters because engines/hgm_tools.py
    # caps the English at [:1500]). It cites an extent, not a segment, and it
    # is OURS, so no spine quote can hold for it; it is named in the output.
    'not-a-single-segment-citation': 10,
}
# 3, not 8. The first draft used 8 and threw away '[33}', '[17]' and
# 'krma pa' as unparseable - the SHORTEST errata are the most checkable,
# not the least, and a length floor quietly excused the gate from them.
# LAYER_ARTEFACT rows describe OUR OWN layer, not an ALL document. Their
# "found" field quotes a MAPPING - `blos -> "on"` - which is by construction
# not a string in the spine, so checking it there is a category error. The
# witness gate already skipped this kind; this one did not, and the two must
# agree. They are counted and named in the output rather than quietly dropped.
# 2026-09-17, gate audit: this was SKIP_KINDS = ('LAYER_ARTEFACT',), an
# exemption keyed on KIND when the rationale is about SHAPE. The comment below
# states the mechanism exactly -- a mapping "x -> y" is by construction not a
# string in the spine -- but keying it on kind swept in every later row filed
# under the same label. Measured: 16 LAYER_ARTEFACT rows, of which only FOUR
# are mappings. Five more cite an extent rather than a segment and are skipped
# by that rule anyway. The remaining SEVEN (E-183/184/187/188/189/190/191) are
# ordinary single-segment English quotes that were inheriting an exemption
# written 17 days before they existed, and a stale quote on any of them
# produced byte-identical passing output.
#
# It matters because build_errata_register.py renders LAYER_ARTEFACT into the
# SHIPPED docs/ERRATA_REGISTER.md, whose section 1 asserts of every entry that
# "the quoted strings were retrieved from the spine, not from a note". The
# gate meant to keep that promise true was skipping the rows it is made about.
#
# So: skip the SHAPE. A `found` carrying the mapping arrow is not a spine
# string; anything else is checked like any other row.
SKIP_KINDS = ()
MAPPING = (' -> ', ' → ')
MIN_FRAG = 3
TR = str.maketrans({'‘': "'", '’': "'", '“': '"', '”': '"',
                    '–': '-', '—': '-', '\xa0': ' '})

def fold(s):
    return re.sub(r'\s+', ' ', (s or '').translate(TR)).strip()

def bare(s):
    return re.sub(r'[^a-z0-9ༀ-࿿]', '', fold(s).lower())

def fragments(found):
    """Split a register quote into the pieces that must each be in the spine."""
    found = (found or '').strip()
    if not found:
        return []
    quoted = re.findall(r'"([^"]{%d,})"' % MIN_FRAG, found)
    if len(quoted) < 2:
        # Narrative rows quote with single marks, and wylie is full of
        # apostrophes. A CLOSING quote is an apostrophe with no letter on
        # either side of the pair - so pa'i and 'dzin keep theirs, and only
        # a real span boundary matches.
        quoted = [q for q in re.findall(r"(?<![A-Za-z])'(.+?)'(?![A-Za-z])",
                                        found, re.S) if len(q) >= MIN_FRAG]
    if len(quoted) >= 2:          # enumerated / narrative: use the quoted spans
        raw = quoted
    else:
        raw = []
        for chunk in re.split(r'\s*\|\s*', found):
            chunk = re.sub(r'^(?:acip|wylie|english)\s*:\s*', '', chunk.strip(),
                           flags=re.I)
            m = re.match(r'^(.*?)\s*\((?:ACIP(?:\s+source)?)\s*:\s*(.*?)\)?\s*$',
                         chunk, re.S)
            raw += [m.group(1), m.group(2)] if m else [chunk]
    out = []
    for piece in raw:
        for part in re.split(r'\s*\.\.\.\s*|\s+—\s+', piece):   # elisions
            part = part.strip().strip(' ",‘’')
            if len(part) >= MIN_FRAG:
                out.append(part)
    return out

def classify(reg, rows):
    seg = {(c, s): (w or '', e or '', a or '') for c, s, w, e, a in rows}
    t, bad = Counter(), defaultdict(list)
    for e in reg:
        if e.get('kind') in SKIP_KINDS or any(m in str(e.get('found') or '') for m in MAPPING):
            t['ours (describe the layer, not a document)'] += 1
            continue
        sid = (e.get('segment') or '').strip()
        # The old pattern here was [A-Z]\d\d, which matches C01..C18 and
        # nothing else — 19 of the spine's 75 course codes. Every entry citing
        # AK, ILL, GIE, SVN, SSL, OSE, P1..P10, TCS.., Sunlight or TITL* fell
        # into 'not-a-single-segment-citation' and its quote was NEVER CHECKED,
        # which is the worst kind of pass: a tick that means "not examined".
        # It held at a ceiling of 5 only because no entry had yet cited one.
        # Widening it is a strengthening, not a loosening — a code that does
        # not exist now lands in SEGMENT-GONE, whose ceiling is 0.
        m = re.fullmatch(r'([A-Za-z][A-Za-z0-9_]*):(\d+)', sid)
        if not m:
            t['not-a-single-segment-citation'] += 1
            bad['not-a-single-segment-citation'].append((e.get('item_id'), sid))
            continue
        k = (m.group(1), int(m.group(2)))
        if k not in seg:
            t['SEGMENT-GONE'] += 1
            bad['SEGMENT-GONE'].append((e.get('item_id'), sid))
            continue
        blob = ' ¶ '.join(seg[k])
        fb, bb = fold(blob), bare(blob)
        frags = fragments(e.get('found'))
        if not frags:
            t['unparseable-quote'] += 1
            bad['unparseable-quote'].append(
                (e.get('item_id'), sid, (e.get('found') or '')[:70]))
            continue
        miss = [p for p in frags
                if p not in blob and fold(p) not in fb and bare(p) not in bb]
        if miss:
            t['QUOTE-NOT-AT-SEGMENT'] += 1
            bad['QUOTE-NOT-AT-SEGMENT'].append(
                (e.get('item_id'), sid, e.get('kind'), [x[:90] for x in miss]))
        else:
            t['every fragment present'] += 1
            t['fragments checked'] += len(frags)
    return t, bad

def _spine_usable(path):
    """Present is not enough: sqlite3.connect CREATES the file, so the first
    check to run leaves an empty database behind and every later one finds it
    'there'. That is how all five of these went from a clean skip to an
    unhandled OperationalError on a fresh clone. Ask for the table."""
    import os as _os
    import sqlite3 as _sq
    if not _os.path.exists(path):
        return False
    try:
        con = _sq.connect("file:%s?mode=ro" % path, uri=True)
        con.execute("SELECT 1 FROM corpus_segments LIMIT 1").fetchone()
        con.close()
        return True
    except Exception:
        return False


def main():
    if not _spine_usable(SPINE):
        print("  SKIPPED: no usable spine at %s — build/ is gitignored, so a\n             fresh clone has none. NOT a pass: nothing was checked." % SPINE)
        return 77
    reg = json.load(io.open(SIDE, encoding='utf-8'))
    rows = list(sqlite3.connect(SPINE).execute(
        'select course,seq,wylie,english,acip from corpus_segments'))
    t, bad = classify(reg, rows)
    fail = False
    for cls, cap in CEILING.items():
        n = t.get(cls, 0)
        if n > cap:
            fail = True
            print('FAIL %s: %d > ceiling %d' % (cls, n, cap))
            for r in bad[cls][:10]:
                print('     ', r)
    if fail:
        return 1
    print('errata quotes hold: %d entries, %d with a single-segment citation, '
          '%d fragments all present in the spine; %d cite an extent or are '
          'UNVERIFIED and %d describe our own layer rather than a document - '
          'none of those are checked here'
          % (len(reg), t['every fragment present'], t['fragments checked'],
             t['not-a-single-segment-citation'],
             t['ours (describe the layer, not a document)']))
    return 0

if __name__ == '__main__':
    sys.exit(main())
