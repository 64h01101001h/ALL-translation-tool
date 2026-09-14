"""practice_parser.py — Daily Practice PDF dual-channel parser (v0.24 session).

Channel A (pypdf): TibetanMachine glyph-name runs (exact-spelling channel).
Channel B (pypdfium2): clean text lines (English, phonetics, headings) with
  proper spacing; Tibetan lines appear as control-char garbage.

Per page the two channels list Tibetan lines in the same order, so the k-th
glyph run (A) replaces the k-th garbage line (B), giving one clean ordered
line sequence per page. Pages whose Tibetan counts disagree are flagged and
skipped for corpus work (never guessed).

Outputs per PDF:
  cards    — [(glyph_run, phonetic)] where a run is followed by a phonetic line
  segments — passage-level [{tib_runs:[...], english:[...], page:int}]
"""
import re
from pypdf import PdfReader
import pypdfium2 as pdfium

GLY = re.compile(r"/(tsheg|shad|gi\.gu|sna\.ro|zhabs\.kyu|'greng\.bu|achung|achen|anusvara|zla\.tshes)")
PHON = re.compile(r"[a-z][a-z\-',\. ]*")
CTRL = re.compile(r"[\x00-\x08\x0b\x0c\x0e-\x1f]")

HDR = [
    re.compile(r"^Practice [IVX0-9]+[:.]?", re.I),
    re.compile(r"^Reading (One|Two|Three|Four|Five|Six|Seven|Eight|Nine|Ten|[0-9]+)$", re.I),
    re.compile(r"^[0-9]{1,4}$"),
    re.compile(r"^Daily Practice", re.I),
    re.compile(r"^Diamond Cutter Sutra\b", re.I),
]

def is_header(l):
    return any(p.match(l.strip()) for p in HDR)

def page_lines_A(page):
    t = page.extract_text() or ''
    out = []
    for l in t.split('\n'):
        l = l.strip()
        if not l: continue
        if '/' in l and GLY.search(l):
            out.append(('T', l[l.index('/'):]))
        else:
            out.append(('X', l))
    return out

def page_lines_B(tp):
    t = tp.get_text_bounded()
    out = []
    for l in re.split(r'\r\n|\n|\r', t):
        raw = l
        l = l.strip()
        if not l: continue
        # Tibetan garbage line: any control chars, or majority non-word chars
        if CTRL.search(raw):
            out.append(('T', l))
        else:
            out.append(('X', l))
    return out

def blockify(lines):
    """Merge consecutive T lines into single T blocks (list of parts)."""
    out = []
    for k, v in lines:
        if k == 'T' and out and out[-1][0] == 'T':
            out[-1][1].append(v)
        elif k == 'T':
            out.append(['T', [v]])
        else:
            out.append(['X', v])
    return out

def parse_pdf(path):
    """Return (cards, segments, page_flags). Walk is document-level so a
    Tibetan passage at a page bottom picks up its English overleaf."""
    ra = PdfReader(path)
    rb = pdfium.PdfDocument(path)
    cards, segments, flags = [], [], []
    doc = []   # unified document-level sequence [('T',[runs],page) | ('X',line,page)]
    npages = len(ra.pages)
    for pi in range(npages):
        rawA = page_lines_A(ra.pages[pi])
        A = blockify(rawA)
        B = blockify(rb and page_lines_B(rb[pi].get_textpage()))
        ablocks = [v for k, v in A if k == 'T']
        alines = [v for k, v in rawA if k == 'T']
        bcount = sum(1 for k, _ in B if k == 'T')
        if len(ablocks) == bcount:
            it = iter(ablocks)
            unified = [('T', next(it)) if k == 'T' else ('X', v) for k, v in B]
        elif len(alines) == bcount:
            # pypdf grouped runs out of order; B's T-block structure governs
            it = iter(alines)
            unified = [('T', [next(it)]) if k == 'T' else ('X', v) for k, v in B]
        else:
            flags.append((pi, len(ablocks), len(alines), bcount))
            # channel mismatch: English would come space-less from A; never
            # guess the T/X interleave — skip the page, close any open segment
            doc.append(('FLAG', None, pi + 1))
            continue
        for k, v in unified:
            doc.append((k, v, pi + 1))
    # document-level walk
    cur_tib, cur_eng, cur_page = [], [], None
    def close():
        nonlocal cur_tib, cur_eng, cur_page
        if cur_tib:
            segments.append({'page': cur_page, 'tib_runs': cur_tib,
                             'english': cur_eng})
        cur_tib, cur_eng = [], []
    prev_kind = None
    prev_block = None
    for k, v, pg in doc:
        if k == 'FLAG':
            close()
            prev_kind = None
            prev_block = None
            continue
        if k == 'T':
            if cur_tib and cur_eng:
                close()
            if not cur_tib:
                cur_page = pg
            cur_tib.extend(v)
            prev_block = v
            prev_kind = 'T'
        else:
            if is_header(v):
                continue
            if prev_kind == 'T' and PHON.fullmatch(v) and len(prev_block) == 1:
                cards.append((prev_block[0], v))
                prev_kind = 'P'
                continue
            cur_eng.append(v)
            prev_kind = 'X'
    close()
    return cards, segments, flags
