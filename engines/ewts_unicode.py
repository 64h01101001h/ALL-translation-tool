"""ewts_unicode.py — wylie (EWTS + IAST-flavored) → Tibetan Unicode (v0.27.1).
Validated against the dictionary's own (wylie, tibetan) ground-truth pairs."""
import re

# IAST → EWTS normalization (the HGM wylie fields use IAST diacritics)
IAST = {'ā':'A','ī':'I','ū':'U','ṃ':'M','ṁ':'M','ḥ':'H','ṇ':'N','ṭ':'T','ḍ':'D',
        'ṣ':'Sh','ś':'sh','ñ':'ny','ṅ':'ng','ṛ':'r-i','ṝ':'r-I','ḷ':'l-i','ḹ':'l-I'}

CONS = {
 'k':'ཀ','kh':'ཁ','g':'ག','ng':'ང','c':'ཅ','ch':'ཆ','j':'ཇ','ny':'ཉ',
 'T':'ཊ','Th':'ཋ','D':'ཌ','N':'ཎ','t':'ཏ','th':'ཐ','d':'ད','n':'ན',
 'p':'པ','ph':'ཕ','b':'བ','m':'མ','ts':'ཙ','tsh':'ཚ','dz':'ཛ','w':'ཝ',
 'zh':'ཞ','z':'ཟ',"'":'འ','y':'ཡ','r':'ར','l':'ལ','sh':'ཤ','Sh':'ཥ','s':'ས',
 'h':'ཧ','gh':'གྷ','dh':'དྷ','bh':'བྷ','dzh':'ཛྷ','kSh':'ཀྵ'}
SUB = {
 'k':'ྐ','kh':'ྑ','g':'ྒ','ng':'ྔ','c':'ྕ','ch':'ྖ','j':'ྗ','ny':'ྙ',
 'T':'ྚ','Th':'ྛ','D':'ྜ','N':'ྞ','t':'ྟ','th':'ྠ','d':'ྡ','n':'ྣ',
 'p':'ྤ','ph':'ྥ','b':'ྦ','m':'ྨ','ts':'ྩ','tsh':'ྪ','dz':'ྫ','w':'ྭ',
 'zh':'ྮ','z':'ྯ',"'":'ྰ','y':'ྱ','r':'ྲ','l':'ླ','sh':'ྴ','Sh':'ྵ','s':'ྶ',
 'h':'ྷ','gh':'ྒྷ','dh':'ྡྷ','bh':'ྦྷ','dzh':'ྫྷ','kSh':'ྐྵ'}
VOW = {'a':'','i':'ི','u':'ུ','e':'ེ','o':'ོ','A':'ཱ','I':'ཱི','U':'ཱུ',
       '-i':'ྀ','-I':'ཱྀ','ai':'ཻ','au':'ཽ'}
FINALS = {'M':'ཾ','H':'ཿ'}
A_CHEN = 'ཨ'

CONS_KEYS = sorted(CONS, key=len, reverse=True)
VOW_KEYS = sorted(VOW, key=len, reverse=True)

PRE = set("g d b m '".split())
SUFF = set("g ng d n b m ' r l s".split())
SUBJOINABLE = {'y':set('k kh g p ph b m h'.split()),
               'r':set('k kh g t th d p ph b m s h'.split()),
               'l':set('k g b r s z'.split()),
               'w':set('k kh g c ny t d ts tsh zh z r l sh s h'.split())}
SUPERSTACK = {'r':set('k g ng j ny t d n b m ts dz'.split()),
              'l':set('k g ng c j t d p b h'.split()),
              's':set('k g ng ny t d n p b m ts'.split())}

def norm_iast(s):
    for k, v in IAST.items():
        s = s.replace(k, v).replace(k.upper(), v)
    return s

def tokenize(syl):
    """→ list of ('C',letter) / ('V',vowel) / '.' / '+'; None on failure."""
    out = []; i = 0
    while i < len(syl):
        ch = syl[i]
        if ch == '+': out.append('+'); i += 1; continue
        if ch == '.': out.append('.'); i += 1; continue
        if ch in FINALS:
            out.append(('C', ch)); i += 1; continue
        v = next((v for v in VOW_KEYS if syl.startswith(v, i)), None)
        c = next((c for c in CONS_KEYS if syl.startswith(c, i)), None)
        if ch == 'a':                      # inherent vowel a (but not ai/au)
            if v in ('ai', 'au'):
                out.append(('V', v)); i += len(v); continue
            out.append(('V', 'a')); i += 1; continue
        if v and (not c or len(v) > len(c)):
            out.append(('V', v)); i += len(v)
        elif c:
            out.append(('C', c)); i += len(c)
        else:
            return None
    return out

def native_onset(ons, dotted):
    """Return unicode for a native Tibetan onset cluster, or None."""
    n = len(ons)
    if n == 1:
        return CONS[ons[0]]
    if n == 2:
        a, b = ons
        if dotted:                          # g.y — disjoint letters
            return CONS[a] + CONS[b]
        if a in SUPERSTACK and b in SUPERSTACK[a]:
            return CONS[a] + SUB[b]
        if b in SUBJOINABLE and a in SUBJOINABLE[b]:
            return CONS[a] + SUB[b]
        if a in PRE:
            return CONS[a] + CONS[b]
        return None
    if n == 3:
        a, b, c = ons
        if a in SUPERSTACK and b in SUPERSTACK[a] and c in SUBJOINABLE and b in SUBJOINABLE[c]:
            return CONS[a] + SUB[b] + SUB[c]
        if a in PRE and b in SUPERSTACK and c in SUPERSTACK[b]:
            return CONS[a] + CONS[b] + SUB[c]
        if a in PRE and c in SUBJOINABLE and b in SUBJOINABLE[c]:
            return CONS[a] + CONS[b] + SUB[c]
        return None
    if n == 4:
        a, b, c, d = ons
        if a in PRE and b in SUPERSTACK and c in SUPERSTACK[b] and d in SUBJOINABLE and c in SUBJOINABLE[d]:
            return CONS[a] + CONS[b] + SUB[c] + SUB[d]
    return None

def syl_to_uni(syl):
    t = tokenize(syl)
    if not t: return None
    if '+' in t:
        # explicit stacks: split into chunks at vowels; '+' joins stack members
        out = ''; stack = []; expect = True
        i = 0
        while i < len(t):
            x = t[i]
            if x in ('+', '.'):
                i += 1; continue
            k, v = x
            if k == 'C':
                joined = (i > 0 and t[i-1] == '+')
                if expect or joined:
                    stack.append(v)
                    if not (i+1 < len(t) and t[i+1] == '+'):
                        expect = False
                else:
                    out += CONS.get(v, '')   # suffix
            else:
                if stack:
                    out += CONS[stack[0]] + ''.join(SUB[u] for u in stack[1:])
                    stack = []
                elif not out:
                    out += A_CHEN
                out += VOW[v]
                expect = False
            i += 1
        if stack:
            out += CONS[stack[0]] + ''.join(SUB[u] for u in stack[1:])
        return out
    # group into CV units: (consonant-cluster, vowel); trailing consonants = suffixes
    groups = []; cur = []; dotted = False
    vowel_seen_after = []
    for x in t:
        if x == '.':
            dotted = True; continue
        k, v = x
        if k == 'C':
            cur.append(v)
        else:
            groups.append((cur, v, dotted)); cur = []; dotted = False
    suffixes = cur
    if not groups:
        return None
    out = ''
    for gi, (ons, vowel, dot) in enumerate(groups):
        if not ons:
            if gi == 0:
                out += A_CHEN + VOW[vowel]
            else:
                out += VOW[vowel]      # second vowel on same base (aoM → ཨོཾ)
        elif gi == 0:
            u = native_onset(ons, dot)
            if u is None:                    # Sanskrit-style stack fallback
                if all(c in SUB for c in ons[1:]) and ons[0] in CONS:
                    u = CONS[ons[0]] + ''.join(SUB[c] for c in ons[1:])
                else:
                    return None
            out += u + VOW[vowel]
        else:
            # medial cluster: first letter full form, rest subjoined
            if ons[0] not in CONS or any(c not in SUB for c in ons[1:]):
                return None
            out += CONS[ons[0]] + ''.join(SUB[c] for c in ons[1:]) + VOW[vowel]
    for s in suffixes:
        if s in FINALS:
            out += FINALS[s]
        elif s in CONS:
            out += CONS[s]
        else:
            return None
    return out

PUNCT = {',': '།', '/': '།', ';': '༔'}

def wylie_to_unicode(w):
    """Returns (unicode, ok)."""
    w = norm_iast((w or '').strip())
    w = w.replace('...', ' ').replace('…', ' ')
    if not w: return '', False
    ok = True; res = []
    for p in re.split(r'(\s+|,|/|;)', w):
        if not p or p.isspace(): continue
        if p in PUNCT:
            if res and res[-1] == '་': res.pop()
            res.append(PUNCT[p] + ' ')
            continue
        # per-syllable trailing M/H (anusvara/visarga)
        u = syl_to_uni(p)
        if u is None:
            ok = False; u = '⟨' + p + '⟩'
        res.append(u); res.append('་')
    if res and res[-1] == '་': res.pop()
    return ''.join(res).strip(), ok
