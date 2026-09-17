"""HGM Dictionary project tools: ACIP->EWTS conversion + corpus segment parser.
Companion to pron_engine.py (pronounce, wylie_to_acip)."""
import re

def acip_to_ewts(acip):
    """ACIP transliteration -> EWTS Wylie. Handles TS/TZ, Sanskrit lowercase,
    long-vowel apostrophe (K'A -> kA) vs achung (BA'I -> ba'i)."""
    s = acip.strip().strip(',').strip()
    s = re.sub(r'\{[a-zA-Z+]+\}', '', s)

    # V is the ACIP code for WA-ZUR, and wa-zur is a SUBJOINED letter: it
    # cannot open a syllable. So a V with a letter before it is wa-zur and
    # becomes EWTS w (DVAGS -> dwags, RTZVA -> rtswa, GRVA -> grwa), while a
    # V that opens a syllable is left exactly as it is — stray Latin in the
    # corpus ("va", "valid") must not be read aloud as Tibetan, which is a
    # mistake the pron oracle made once already over 263 syllables.
    #
    # Until now V passed through unmapped, producing wylie that is not EWTS
    # at all: wylie_to_unicode('rtsva') flags, wylie_to_unicode('rtswa') gives
    # རྩྭ. Measured over the shipped spine, 1,615 segments carry a subjoined V
    # and NONE of them rendered without a flag; 1,113 do now. Documented as
    # gap 2 of 3 in docs/FINDING_ACIP_EWTS_GAPS.md. Gap 3, the visarga ':',
    # is NOT touched here, and GA-YAS is not understood and not guessed at.
    # ...and the letter before it must be a CONSONANT, because wa-zur
    # subjoins to a consonant and cannot attach to a vowel. The first version
    # of this rule said only "a letter before it" and that was a Rule 3
    # REGRESSION, caught by audit the same day: the acip column also holds
    # ALL-CAPS ENGLISH (292 rows — "DIAMOND MOUNTAIN UNIVERSITY", "THE THIRD
    # PATH: CORRECT VIEW"), and is_acip_line cannot see it because it only
    # tests an uppercase ratio. EVERY became EWERY -> ཨེཝེརཡ and DIVISIONS
    # became དིཝིསིོནས, turning an honest ⟨every⟩ flag into confident
    # nonsense. Requiring a consonant keeps all real wa-zur (DVAGS, GRVA,
    # RTZVA, KVA, ZHVA, TSVA, SV'AH'A, TV'A) and rejects EVERY, DIVISIONS,
    # ACHIEVE, UNIVERSITY, INDIVIDUAL, GIVEN, LIVES, DEVELOPING, ACTIVITY.
    # SERVANT and ADVANCE still leak: rv and dv ARE wa-zur sequences, so no
    # local rule can separate them — that needs the caller to stop feeding
    # English through, which is filed separately.
    s = re.sub(r'(?<=[BCDGHJKLMNPRSTVWYZbcdghjklmnprstvwyz])V', 'W', s)
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
