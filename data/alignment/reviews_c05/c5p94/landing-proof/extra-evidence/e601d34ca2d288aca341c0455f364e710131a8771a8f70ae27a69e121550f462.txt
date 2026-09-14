"""decode_practices.py — v0.24 Practice batch decode + segment build."""
import json, re
import glyph_decoder as gd
from pron_engine import wylie_to_acip

# --- targeted glyph aliases (validated via round-trip only, never trusted raw) ---
_orig = gd.glyph_component
ALIAS = {'baĸ': ('C', 'bkr'), 'ĸa': ('C', 'kr')}
def _patched(g):
    g2 = re.sub(r'_\d+$', '', g).strip()
    if g2 in ALIAS: return ALIAS[g2]
    return _orig(g)
gd.glyph_component = _patched

STOP = set('the of to and a in i is that it for on with as be this you your my we '
           'all who from or by at are was were which what when how not no'.split())
MANTRA = {'guru','ratna','mandalakam','niryatayami','idam','om','ah','hum','svaha',
          'soha','namo','mani','padme','tayata','gate','paragate','parasamgate','bodhi'}

def keep_english_line(l):
    toks = re.findall(r"[a-zA-Z']+", l.lower())
    if not toks: return False
    if sum(1 for t in toks if t in MANTRA) >= 2: return False          # Sanskrit mantra
    if re.fullmatch(r"[a-z][a-z\-',\. ]*", l) and not any(t in STOP for t in toks):
        return False                                                    # stray phonetic line
    return True

def load_lex():
    d = json.load(open('hgm_dictionary_v23_1.json'))
    LEX = set()
    for e in d['unified_entries']:
        for w in [e.get('wylie')] + (e.get('wylie_variants') or []):
            if w:
                for t in w.split(): LEX.add(t)
    return LEX

ORN = re.compile(r'/zla\.tshes(\.gnyis)?(_\d+)?(?=/| |$)')   # yig-mgo ornament → shad

def strip_ornaments(run):
    return ORN.sub('/shad', run)

def decode_line(run, LEX):
    """Decode a glyph run without phonetic: per-syllable best-lexicon choice.
    Returns (wylie, score, n_syl) or (None, 0, n) if any syllable undecodable."""
    per = gd.decode_run(strip_ornaments(run))
    if any(not c for c in per): return None, 0.0, len(per)
    out, hits = [], 0
    for cands in per:
        best, bs = cands[0], -1
        for c in cands:
            s = 1 if (c in LEX or c.replace('.', '') in LEX) else 0
            if s > bs: best, bs = c, s
        out.append(best); hits += (1 if bs > 0 else 0)
    return ' '.join(out), hits / max(1, len(per)), len(per)

def main():
    LEX = load_lex()
    parsed = json.load(open('practice_parsed.json'))
    order = ['P1','P3','P4','P5','P6','P7','P8','P9','P10']

    # ---- cards: dedupe identical (run, phon) across PDFs, decode once ----
    seen = {}
    card_bank = {}   # tag -> list
    for tag in order:
        card_bank[tag] = []
        for run, phon in parsed[tag]['cards']:
            key = (run, phon)
            if key not in seen:
                w, st = gd.best_decode(strip_ornaments(run), phon, LEX)
                seen[key] = (w, st)
            w, st = seen[key]
            card_bank[tag].append({'phonetic': phon, 'wylie': w, 'status': st})
    run2card = {r: seen[(r, p)] for (r, p) in seen}

    # ---- segments ----
    segs_out, stats = [], {}
    for tag in order:
        kept = dropped_undec = dropped_lowscore = dropped_noeng = 0
        for s in parsed[tag]['segments']:
            eng_lines = [l for l in s['english'] if keep_english_line(l)]
            if not eng_lines:
                dropped_noeng += 1; continue
            wylies, scores, ok = [], [], True
            for run in s['tib_runs']:
                if run in run2card and run2card[run][1] in ('validated', 'validated-prefix'):
                    wylies.append(run2card[run][0]); scores.append(1.0); continue
                w, sc, n = decode_line(run, LEX)
                if w is None: ok = False; break
                wylies.append(w); scores.append(sc)
            if not ok:
                dropped_undec += 1; continue
            score = sum(scores) / len(scores)
            wy = ', '.join(wylies)
            eng = ' '.join(eng_lines)
            eng = re.sub(r'\s+', ' ', eng).strip()
            segs_out.append({'course': tag, 'page': s['page'], 'wylie': wy,
                             'english': eng, 'decode_score': round(score, 3),
                             'n_lines': len(wylies)})
            kept += 1
        stats[tag] = {'kept': kept, 'dropped_undecodable': dropped_undec,
                      'dropped_no_english': dropped_noeng}
    json.dump({'cards': card_bank, 'segments': segs_out, 'stats': stats},
              open('practice_decoded.json', 'w'), ensure_ascii=False, indent=1)
    # report
    from collections import Counter
    for tag in order:
        c = Counter(x['status'] for x in card_bank[tag])
        print(tag, 'cards:', dict(c), 'segs:', stats[tag])
    sc = [s['decode_score'] for s in segs_out]
    import statistics
    print('segments total', len(segs_out), 'score median', statistics.median(sc),
          'mean', round(statistics.mean(sc), 3))
    hist = Counter(round(s, 1) for s in sc)
    print('score hist:', dict(sorted(hist.items())))

if __name__ == '__main__':
    main()
