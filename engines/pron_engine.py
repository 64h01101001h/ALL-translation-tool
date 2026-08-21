import re

def wylie_to_acip(w):
    s = w.strip()
    s = re.sub(r'Sh', '⟦sh⟧', s)
    for c in 'TDNSM':
        s = s.replace(c, '⟦'+c.lower()+'⟧')
    s = re.sub(r"(?<=[a-z])A", "⟦'A⟧", s)
    s = re.sub(r"(?<=[a-z])I", "⟦'I⟧", s)
    s = re.sub(r"(?<=[a-z])U", "⟦'U⟧", s)
    s = s.replace('tsh','⟦TS⟧').replace('ts','⟦TZ⟧')
    out, prot = [], False
    for ch in s:
        if ch=='⟦': prot=True; continue
        if ch=='⟧': prot=False; continue
        out.append(ch if prot else ch.upper())
    return ''.join(out)

CLUSTERS = {
 'by':'j','sby':'j','dby':'y','phy':'ch','py':'ch','spy':'ch','dpy':'ch','my':'ny','smy':'ny','dmy':'ny',
 'ky':'ky','sky':'ky','bky':'ky','rky':'ky','bsky':'ky','khy':'ky','mkhy':'ky',
 'gy':'gy','rgy':'gy','sgy':'gy','bgy':'gy','brgy':'gy','bsgy':'gy','mgy':'gy','dgy':'gy',
 'kr':'tr','bkr':'tr','skr':'tr','dkr':'tr','khr':'tr','mkhr':'tr','gr':'dr','sgr':'dr','bgr':'dr','dgr':'dr','mgr':'dr','bsgr':'dr','grw':'dr',
 'pr':'tr','spr':'tr','dpr':'tr','phr':'tr','br':'dr','dbr':'dr','sbr':'dr','dr':'dr','sdr':'dr','mdr':'dr','tr':'tr',
 'sr':'s','bsr':'s','rj':'j','brj':'j','zl':'d','bzl':'d','sl':'l','bsl':'l','bl':'l','rl':'l','kl':'l','gl':'l',
 'lh':'hl','db':'w','dw':'d','tw':'t','tsw':'ts','rw':'r','shw':'sh','sw':'s','zhw':'sh',
 'rm':'m','sm':'m','dm':'m','rn':'n','sn':'n','gn':'n','mn':'n','brn':'n','bsn':'n',
 'rny':'ny','sny':'ny','gny':'ny','mny':'ny','brny':'ny','bsny':'ny',
 'rng':'ng','sng':'ng','dng':'ng','brng':'ng','bsng':'ng','sts':'ts','rts':'ts','brts':'ts','bsts':'ts',
 'rdz':'dz','brdz':'dz','rd':'d','sd':'d','gd':'d','bd':'d','brd':'d','bsd':'d',
 'rb':'b','sb':'b','rg':'g','sg':'g','dg':'g','bg':'g','brg':'g','bsg':'g','mg':'g',
 'rk':'k','sk':'k','bk':'k','brk':'k','bsk':'k','dk':'k',
 'rt':'t','st':'t','gt':'t','bt':'t','brt':'t','bst':'t','mth':'t','mkh':'k','gts':'ts','bts':'ts','btz':'ts',
 'gz':'s','bz':'s','gsh':'sh','bsh':'sh','gs':'s','bs':'s','dp':'p','sp':'p','dph':'p',
 'mch':'ch','mtsh':'ts','gc':'ch','bc':'ch','lc':'ch','lj':'j','mj':'j','mdz':'dz','md':'d',
 'ld':'d','lt':'t','lb':'b','lp':'p','lg':'g','lk':'k','lng':'ng',
}
SIMPLE = {'k':'k','kh':'k','g':'g','ng':'ng','c':'ch','ch':'ch','j':'j','ny':'ny',
 't':'t','th':'t','d':'d','n':'n','p':'p','ph':'p','b':'b','m':'m',
 'ts':'ts','tsh':'ts','dz':'dz','w':'w','zh':'sh','z':'s',"'":'','y':'y','r':'r','l':'l','sh':'sh','s':'s','h':'h','':'','g-y':'y'}
PRENASAL_SUB = {"'g":'ng',"'j":'nj',"'d":'nd',"'b":'mb',"'dz":'ndz',
 'mg':'ng','mj':'nj','md':'nd','mdz':'ndz','ld':'nd','lj':'nj',
 "'gy":'ngy',"zl":'nd',"'gr":'ndr',"'khr":'tr',"'kh":'k',"'ph":'p',"'ch":'ch',"'th":'t',"'ts":'ts',"'dr":'ndr',"'by":'nj',"'phy":'ch',"'khy":'ky'}
FIRST_SYL_APOS = {"'g":'g',"'j":'j',"'d":'d',"'b":'b',"'dz":'dz',"'gy":'gy',"'gr":'dr',"'khr":'tr',"'kh":'k',"'ph":'p',"'ch":'ch',"'th":'t',"'ts":'ts',"'dr":'dr',"'by":'j',"'phy":'ch',"'khy":'ky'}
UMLAUT = set(['d','n','l','s'])

def parse_syl(syl):
    s = syl
    for v in 'aeiou':
        s = s.replace("'"+v, chr(0x2D0)+v)
    m = re.match(r"^([^aeiou]*)([aeiou])(.*)$", s)
    if not m: return None
    onset = m.group(1).replace(chr(0x2D0),"'")
    coda = m.group(3).replace(chr(0x2D0),"'")
    return onset, m.group(2), coda

def pron_onset(onset, subsequent):
    o = onset
    table = PRENASAL_SUB if subsequent else FIRST_SYL_APOS
    for k in sorted(table, key=len, reverse=True):
        if o == k: return table[k]
    for k in sorted(CLUSTERS, key=len, reverse=True):
        if o == k: return CLUSTERS[k]
    if o in SIMPLE: return SIMPLE[o]
    for p in ["b","d","g","m","'"]:
        if o.startswith(p) and len(o) > len(p):
            r = o[len(p):]
            if r in CLUSTERS: return CLUSTERS[r]
            if r in SIMPLE: return SIMPLE[r]
    return o

def pron_syllable(syl, subsequent, prenasal_ok=True):
    p = parse_syl(syl)
    if not p: return syl
    onset, vowel, coda = p
    on = pron_onset(onset, subsequent and prenasal_ok)
    gen = False
    if coda == "'i": coda, gen = '', True
    if coda in ("'o","'u","'a","'e"): coda = ''
    if coda.startswith("'"): coda = coda[1:]
    suf1 = 'ng' if coda[:2]=='ng' else (coda[:1] if coda else '')
    suf2 = coda[len(suf1):]
    v = vowel
    if suf1 in UMLAUT:
        if v=='a': v='e'
        elif v=='o': v='u'
    sufmap = {'g':'k','b':'B','d':'','s':'','n':'n','m':'m','r':'r','l':'l','ng':'ng','':''}
    sfx = sufmap.get(suf1,'')
    if sfx=='B' and suf2.startswith('s'): sfx='β'
    # db onset special: silent before u
    if on=='w' and onset in ('db',) and v=='u': on=''
    return on + v + sfx + ('y' if gen else '')

PARTICLES = {"kyi","gyi","gi","yi","kyis","gyis","gis","yis","dang","ni","yang","kyang","'ang","la","nas","las","na","rnams","tsam","du","ru","ste","te","de"}
ENCLITIC_BACK = {"tu"}
ENCLITIC_HYPH = {"su"}
NEG_FORWARD = {"ma","mi"}
ADV_GYIS_HEADS = {"rim","zil"}
SYL_OVERRIDE = {"ston":"ton","dkon":"kon","thob":"top","grub":"drup","sgrib":"drip"}
FORMATIVE = {"pa","ba","po","bo","mo","wa","pa'i","ba'i","po'i","bo'i","par","bar","pas","bas","pa'o"}
HYPHEN_COMPOUNDS = {("chos","sku"),("gzugs","sku"),("sprul","sku"),("longs","sku")}

def _segment(syls):
    words, cur, content = [], [], 0
    i = 0
    n = len(syls)
    while i < n:
        s = syls[i]
        if i > 0 and s in ENCLITIC_BACK and cur:
            cur.append(s); words.append(cur); cur=[]; content=0; i+=1; continue
        if i > 0 and s in ENCLITIC_HYPH and cur:
            cur.append('-'+s); words.append(cur); cur=[]; content=0; i+=1; continue
        if s in ("gyis","gyi") and cur and cur[-1] in ADV_GYIS_HEADS:
            cur.append(s); words.append(cur); cur=[]; content=0; i+=1; continue
        if s in PARTICLES and i > 0:
            if cur: words.append(cur); cur=[]; content=0
            words.append([s]); i+=1; continue
        if s in NEG_FORWARD and i+1 < n:
            if cur: words.append(cur); cur=[]; content=0
            neg = [s, syls[i+1]]; i+=2
            while i < n and syls[i] in FORMATIVE:
                neg.append(syls[i]); i+=1
            words.append(neg); continue
        cur.append(s); content += 1
        if s.endswith("'i"):
            words.append(cur); cur=[]; content=0; i+=1; continue
        nxt = syls[i+1] if i+1 < n else None
        if content >= 2:
            if nxt and (nxt.endswith("'i") or nxt in FORMATIVE) and len(cur) < 3:
                pass
            else:
                words.append(cur); cur=[]; content=0
        i += 1
    if cur: words.append(cur)
    return words

def _pron_word(wsyls):
    VOICELESS = ('p','t','k','ts','ch','s','sh','h')
    res = []
    for j, s in enumerate(wsyls):
        prev = res[-1] if res else ''
        prev_open = bool(prev) and prev[-1] not in ('p','B','\u03b2')
        if j > 0 and s in ('ba','bo','bar','bas',"ba'i","bo'i"):
            p = {'ba':'wa','bo':'wo','bar':'war','bas':'we',"ba'i":'way',"bo'i":'woy'}[s]
        else:
            hy = s.startswith('-')
            s0 = s[1:] if hy else s
            if s0 in SYL_OVERRIDE and not (j==0):
                p = SYL_OVERRIDE[s0]
            else:
                p = pron_syllable(s0, j > 0, prenasal_ok=prev_open)
            if s0 in SYL_OVERRIDE and j==0:
                p = SYL_OVERRIDE[s0]
            if hy: p = '-' + p
        if res:
            if prev[-1:] in ('B','\u03b2'):
                nxt0 = p[:2] if p[:2] in ('ts','ch','sh') else p[:1]
                res[-1] = prev[:-1] + ('p' if nxt0 in VOICELESS else 'b')
                prev = res[-1]
            if prev.endswith('k') and p.startswith('gy'):
                pass
            elif prev.endswith('k') and p.startswith('g'):
                p = 'k' + p[1:]
            if prev.endswith('k') and p.startswith('k'):
                res[-1] = prev[:-1]; prev = res[-1]
            if prev.endswith('ng') and p.startswith('gy'):
                p = p[1:]
            if prev.endswith('k') and p.startswith('ng'):
                res[-1] = prev[:-1]; prev = res[-1]
        res.append(p)
    if res and res[-1].endswith('B'): res[-1] = res[-1][:-1] + 'b'
    elif res and res[-1].endswith('\u03b2'): res[-1] = res[-1][:-1] + 'p'
    joiner = '-' if len(wsyls)==2 and tuple(wsyls) in HYPHEN_COMPOUNDS else ''
    return joiner.join(res) if joiner else ''.join(res)

def pronounce(wylie):
    w = wylie.lower().replace('+',' ').replace('/','').strip()
    syls = [s for s in re.split(r"[\s\u00b7]+", w) if s]
    words = _segment(syls)
    return ' '.join(x for x in (_pron_word(ws) for ws in words) if x)
