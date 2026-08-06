"""sanskrit_convert.py — Sanskrit representation converter (ALL Translation Tool).

Converts between:  IAST transliteration · ACIP Sanskrit code · EWTS ·
Tibetan Unicode rendering · Devanagari · simplified reader pronunciation.

IAST→ACIP conventions learned & validated against 4,911 (IAST, ACIP) pairs
embedded in the Lokesh Chandra dictionary (v0.27 extraction).
"""
import re
try:
    from hgm_tools import acip_to_ewts
    from ewts_unicode import wylie_to_unicode
except Exception:
    acip_to_ewts = None; wylie_to_unicode = None

# ---------- IAST tokenization ----------
IAST_CONS = ['kṣ','tr','jñ',  # convenience digraphs handled via cluster logic anyway
             'kh','gh','ch','jh','ṭh','ḍh','th','dh','ph','bh',
             'k','g','ṅ','c','j','ñ','ṭ','ḍ','ṇ','t','d','n','p','b','m',
             'y','r','l','v','ś','ṣ','s','h']
IAST_VOW = ['ai','au','a','ā','i','ī','u','ū','ṛ','ṝ','ḷ','ḹ','e','o']
FINAL = {'ṃ':'m','ḥ':':','ṁ':'m'}

C_ACIP = {'k':'K','kh':'KH','g':'G','gh':'GH','ṅ':'NG','c':'TZ','ch':'TSH','j':'DZ','jh':'DZH',
          'ñ':'NY','ṭ':'t','ṭh':'th','ḍ':'d','ḍh':'dh','ṇ':'n','t':'T','th':'TH','d':'D','dh':'DH',
          'n':'N','p':'P','ph':'PH','b':'B','bh':'BH','m':'M','y':'Y','r':'R','l':'L','v':'W',
          'ś':'SH','ṣ':'sh','s':'S','h':'H'}
V_ACIP = {'a':'A','ā':"'A",'i':'I','ī':"'I",'u':'U','ū':"'U",'e':'E','o':'O',
          'ai':'EE','au':'OO','ṛ':'Ri','ṝ':"R'I",'ḷ':'Li','ḹ':"L'I"}
# clusters that stay as syllable onsets (rendered as stacks in Tibetan)
ONSET_2ND = set('r y w v l'.split())

C_DEVA = {'k':'क','kh':'ख','g':'ग','gh':'घ','ṅ':'ङ','c':'च','ch':'छ','j':'ज','jh':'झ',
          'ñ':'ञ','ṭ':'ट','ṭh':'ठ','ḍ':'ड','ḍh':'ढ','ṇ':'ण','t':'त','th':'थ','d':'द','dh':'ध',
          'n':'न','p':'प','ph':'फ','b':'ब','bh':'भ','m':'म','y':'य','r':'र','l':'ल','v':'व',
          'ś':'श','ṣ':'ष','s':'स','h':'ह'}
V_DEVA_IND = {'a':'अ','ā':'आ','i':'इ','ī':'ई','u':'उ','ū':'ऊ','ṛ':'ऋ','ṝ':'ॠ','ḷ':'ऌ','ḹ':'ॡ',
              'e':'ए','ai':'ऐ','o':'ओ','au':'औ'}
V_DEVA_DEP = {'a':'','ā':'ा','i':'ि','ī':'ी','u':'ु','ū':'ू','ṛ':'ृ','ṝ':'ॄ','ḷ':'ॢ','ḹ':'ॣ',
              'e':'े','ai':'ै','o':'ो','au':'ौ'}
VIRAMA='्'; ANUSVARA='ं'; VISARGA='ः'

PRON = {'ā':'a','ī':'i','ū':'u','ṛ':'ri','ṝ':'ri','ḷ':'li','ḹ':'li','ṃ':'m','ṁ':'m','ḥ':'h',
        'ṅ':'ng','ñ':'ny','ṭ':'t','ḍ':'d','ṇ':'n','ś':'sh','ṣ':'sh','c':'ch'}

def tokenize_iast(word):
    """→ list of ('C',c)/('V',v)/('F',f); None on failure."""
    w = word.lower().strip()
    out=[]; i=0
    CK = sorted(C_ACIP, key=len, reverse=True)
    VK = sorted(V_ACIP, key=len, reverse=True)
    while i < len(w):
        ch = w[i]
        if ch in FINAL: out.append(('F', ch)); i += 1; continue
        if ch in " -–'’·.": out.append(('B', ' ')); i += 1; continue
        v = next((v for v in VK if w.startswith(v, i)), None)
        c = next((c for c in CK if w.startswith(c, i)), None)
        # 'a' vs 'ai/au' priority already by length sort; consonant priority when both (e.g. 'y')
        if c and (not v or len(c) >= len(v) or w[i] not in 'aāiīuūṛṝḷḹeo'):
            out.append(('C', c)); i += len(c)
        elif v:
            out.append(('V', v)); i += len(v)
        else:
            return None
    return out

def syllabify(toks):
    """Group into syllables: [(onset_cons_list, vowel, final_list)]. Coda consonants
    attach to preceding syllable except last-consonant(+r/y/v/l cluster) → next onset."""
    syls=[]; cur_on=[];
    i=0; n=len(toks)
    while i < n:
        k,v = toks[i]
        if k=='B':
            i+=1; continue
        if k=='C':
            cur_on.append(v); i+=1; continue
        if k=='V':
            # close syllable: onset=cur_on, vowel=v; then collect finals/codas
            syl=[list(cur_on), v, []]
            cur_on=[]
            i+=1
            # attach F immediately
            while i<n and toks[i][0]=='F':
                syl[2].append(toks[i][1]); i+=1
            # lookahead: consonant run until next vowel
            j=i; run=[]
            while j<n and toks[j][0]=='C':
                run.append(toks[j][1]); j+=1
            nxt_vowel = j<n and toks[j][0]=='V'
            if run and nxt_vowel:
                # split run: last cons (+preceding if forms cluster with r/y/v/l...) → onset
                if len(run)==1:
                    on=[run[0]]; coda=[]
                elif run[-1] in ONSET_2ND and len(run)>=2:
                    # e.g. 'sra','dra','sya': take last two as onset if len>=2
                    on=run[-2:]; coda=run[:-2]
                else:
                    on=[run[-1]]; coda=run[:-1]
                syl[2].extend(coda)
                cur_on=on
                i=j
            elif run and not nxt_vowel:
                syl[2].extend(run); i=j
            syls.append(syl)
            continue
        i+=1
    if cur_on and syls:
        syls[-1][2].extend(cur_on)
    return syls

def iast_to_acip(word):
    toks = tokenize_iast(word)
    if toks is None: return None
    parts=[]
    AMBIG={('n','y'),('N','y'),('t','s'),('t','sh'),('T','h'),('d','h'),('k','h'),('g','h'),
           ('p','h'),('b','h'),('ṇ','y'),('d','z'),('t','z')}
    for on, v, fin in syllabify(toks):
        # v inside a cluster keeps V (not W)
        acs=[]
        for idx,c in enumerate(on):
            a=C_ACIP[c]
            if c=='v' and idx>0: a='V'
            acs.append(a)
        s=''
        for idx,a in enumerate(acs):
            if idx>0 and (on[idx-1],on[idx]) in AMBIG: s+='+'
            s+=a
        vv = V_ACIP[v]
        if on:
            # long vowels: apostrophe between consonant and vowel: H + 'A -> H'A
            s = s + vv if not vv.startswith("'") else s + vv
        else:
            s = 'A'+vv if vv in ('A',) else ('A'+vv if not vv.startswith("'") else "A"+vv)
            if v=='a': s='AA'
            elif v in V_ACIP and not V_ACIP[v].startswith("'"): s='A'+V_ACIP[v]
            else: s='A'+V_ACIP[v]
        for f in fin:
            if f in FINAL: s += {'ṃ':'m','ṁ':'m','ḥ':':'}[f]
            else: s += C_ACIP[f]
        parts.append(s)
    return ' '.join(parts)

def iast_to_devanagari(word):
    toks = tokenize_iast(word)
    if toks is None: return None
    out=''; prev_cons=False
    i=0
    for k,v in toks:
        if k=='B': out+=' '; prev_cons=False; continue
        if k=='C':
            if prev_cons: out+=VIRAMA
            out+=C_DEVA[v]; prev_cons=True
        elif k=='V':
            if prev_cons: out+=V_DEVA_DEP[v]
            else: out+=V_DEVA_IND[v]
            prev_cons=False
        else:
            out += ANUSVARA if v in ('ṃ', 'ṁ') else VISARGA
            prev_cons = False
    if prev_cons: out+=VIRAMA
    return out

def iast_to_pronunciation(word):
    s=word.lower()
    for k,v in PRON.items(): s=s.replace(k,v)
    return s

def iast_to_tibetan(word):
    """IAST → Tibetan-script unicode, via ACIP → EWTS → unicode chain."""
    if acip_to_ewts is None or wylie_to_unicode is None: return None
    a = iast_to_acip(word)
    if a is None: return None
    w = acip_to_ewts(a).replace('v', 'w')   # Sanskrit va renders with wazur
    u, ok = wylie_to_unicode(w)
    return u if ok else None

def acip_to_iast(acip):
    """ACIP Sanskrit code → IAST (via EWTS bridge)."""
    if acip_to_ewts is None: return None
    w = acip_to_ewts(acip)
    REV = {'A':'ā','I':'ī','U':'ū','M':'ṃ','H':'ḥ','T':'ṭ','Th':'ṭh','D':'ḍ','N':'ṇ','Sh':'ṣ',
           'sh':'ś','ny':'ñ','ng':'ṅ','tsh':'ch','ts':'c','dz':'j','w':'v',':':'ḥ'}
    s=w
    for k in sorted(REV,key=len,reverse=True): s=s.replace(k,REV[k])
    return re.sub(r'\s+','',s)

def convert(word, frm='iast'):
    """One-stop: from IAST give all representations."""
    return {'iast': word,
            'acip_tibetanized': iast_to_acip(word),
            'acip_input_code': iast_to_inputcode(word),
            'acip_nextletter': iast_to_nextletter(word),
            'acip': iast_to_acip(word),
            'ewts': acip_to_ewts(iast_to_acip(word)) if acip_to_ewts and iast_to_acip(word) else None,
            'tibetan': iast_to_tibetan(word),
            'devanagari': iast_to_devanagari(word),
            'pronunciation': iast_to_pronunciation(word)}


# ---------- Official ACIP standards (ACIP Sanskrit Input Code Standards doc) ----------
# 1) "next letter position" style (restricted fonts):  pramāṇa → prama-n.a
NEXTLETTER = {'ā':'a-','ī':'i-','ū':'u-','ṛ':'r.','ṝ':'r.-','ḷ':'l.','ḹ':'l.-',
              'ṃ':'m.','ḥ':'h.','ṅ':'n*','ñ':'n~','ṭ':'t.','ḍ':'d.','ṇ':'n.','ś':'sh','ṣ':'s.'}
# 2) ACIP Sanskrit Input Code Standard (searchable database standard): pramāṇa → prama#n%a
INPUTCODE  = {'ā':'a#','ī':'i#','ū':'u#','ṛ':'r%','ṝ':'r%#','ḷ':'l%','ḹ':'l%#',
              'ṃ':'m%','ḥ':'h%','ṅ':'n*','ñ':'n~','ṭ':'t%','ḍ':'d%','ṇ':'n%','ś':'sh','ṣ':'s%'}

def _apply_table(word, table):
    s = word
    for k in sorted(table, key=len, reverse=True):
        s = s.replace(k, table[k])
    return s

def iast_to_nextletter(word):
    return _apply_table(word.lower(), NEXTLETTER)

def iast_to_inputcode(word):
    return _apply_table(word.lower(), INPUTCODE)

def inputcode_to_iast(code):
    rev = sorted(INPUTCODE.items(), key=lambda kv: len(kv[1]), reverse=True)
    s = code.lower()
    for k, v in rev:
        s = s.replace(v, k)
    return s
