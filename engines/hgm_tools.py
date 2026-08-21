"""HGM Dictionary project tools: ACIP->EWTS conversion + corpus segment parser.
Companion to pron_engine.py (pronounce, wylie_to_acip)."""
import re

def acip_to_ewts(acip):
    """ACIP transliteration -> EWTS Wylie. Handles TS/TZ, Sanskrit lowercase,
    long-vowel apostrophe (K'A -> kA) vs achung (BA'I -> ba'i)."""
    s = acip.strip().strip(',').strip()
    s = re.sub(r'\{[a-zA-Z+]+\}', '', s)
    s = s.replace('sh','⟦Sh⟧').replace('th','⟦Th⟧')
    for lo,hi in [('t','⟦T⟧'),('d','⟦D⟧'),('n','⟦N⟧'),('s','⟦S⟧'),('m','⟦M⟧')]:
        s = s.replace(lo,hi)
    s = s.replace('TS','⟦tsh⟧').replace('TZ','⟦ts⟧')
    s = re.sub(r"(?<=[BCDGHJKLMNPRSTVWYZ⟧])'([AEIOU])", lambda m:'⟦'+m.group(1)+'⟧', s)
    out,prot=[],False
    for ch in s:
        if ch=='⟦': prot=True; continue
        if ch=='⟧': prot=False; continue
        out.append(ch if prot else ch.lower())
    return re.sub(r'\s+',' ',''.join(out)).strip()

def is_acip_line(l):
    s = l.strip().strip('*#`').strip()
    letters = re.findall(r'[A-Za-z]', s)
    if not letters: return False
    return sum(1 for c in letters if c.isupper())/len(letters) > 0.6

def parse_aligned(path, course_id, drop_cjk=True):
    """Parse an ACI-style ASCII file (ACIP blocks alternating with English)
    into aligned segments: [{course, acip, wylie, english}]. Normalizes curly quotes."""
    raw = open(path, encoding='utf-8', errors='replace').read()
    raw = raw.replace('\u2018',"'").replace('\u2019',"'")
    lines = [l.rstrip() for l in raw.replace('\r\n','\n').split('\n')]
    def is_meta(l): return bool(re.fullmatch(r'\[[0-9K\-\, ]+\]|\[folio [^\]]+\]', l.strip()))
    def is_cjk(l): return drop_cjk and bool(re.search(r'[\u4E00-\u9FFF]', l))
    segs, i = [], 0
    while i < len(lines):
        l = lines[i]
        if is_meta(l) or is_cjk(l): i += 1; continue
        if is_acip_line(l):
            tib = []
            while i < len(lines) and (is_acip_line(lines[i]) or not lines[i].strip() or is_meta(lines[i]) or is_cjk(lines[i])):
                if is_acip_line(lines[i]): tib.append(lines[i].strip().strip('*#`').strip())
                i += 1
            eng = []
            while i < len(lines) and not is_acip_line(lines[i]):
                s2 = lines[i].strip()
                if s2 and not is_meta(lines[i]) and not is_cjk(lines[i]): eng.append(s2)
                i += 1
            acip = ' '.join(tib)
            segs.append({"course": course_id, "acip": acip,
                         "wylie": acip_to_ewts(acip), "english": ' '.join(eng)[:1500]})
        else:
            i += 1
    return segs
