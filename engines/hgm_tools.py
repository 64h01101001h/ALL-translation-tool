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
    # '+' is ACIP's explicit-stack mark, and a wa-zur subjoined to a stack sits
    # behind one: T+V'Am is the same word as tV'Am, which renders ཊྭཱཾ, and it
    # was flagging ⟨T+vAM⟩ because the lookbehind saw the '+' and not the
    # consonant in front of it. 12 corpus segments spell it that way against 40
    # the other. Look past any run of '+'.
    s = re.sub(r'(?<=[BCDGHJKLMNPRSTVWYZbcdghjklmnprstvwyz])\+*V', 
               lambda m: m.group(0)[:-1] + 'W', s)
    # VISARGA. ACIP writes it ':' and EWTS writes it 'H' — gap 3 of the three
    # in docs/FINDING_ACIP_EWTS_GAPS.md. Both engines already render hoH as
    # ཧོཿ, so nothing new is being invented here; the colon simply never
    # reached them and wylieToUnicode refused the syllable.
    #
    # A visarga follows a VOWEL, or the anusvara 'm'. That one restriction is
    # what separates it from a colon used as punctuation, and the corpus is
    # unambiguous: of 1,492 colons outside editorial braces, 1,473 follow a
    # vowel and every one is a mantra syllable (DHARM'A:, HR'I:, SHUDDH'A:,
    # BHYO:, A'A:), while the 19 that do not include "THE FIRST PATH:",
    # "The text is saying:", "Includes:", a folio marker [107a]:, an
    # ERROR_NO_SUCH_ACIP: marker and editorial ':-'. Adding 'm' picks up
    # HRIm:, AAm: and BAm:, which are an anusvara followed by a visarga.
    #
    # EDITORIAL BRACES ARE MASKED. The strip above only removes {TIB}-shaped
    # blocks; {%Levi: GYIS} and {NOTE TO LO: ...} survive, and their colons
    # follow vowels too. Nine such blocks carry a colon.
    #
    # Protected, because EWTS visarga is an UPPERCASE H and the case-folding
    # pass below would otherwise turn it into the letter ha.
    s = ''.join(part if part.startswith('{')
                else re.sub(r'(?<=[AEIOUaeioum]):', '\x03', part)
                for part in re.split(r'(\{[^}]*\})', s))

    # A-CHEN'S OWN LONG VOWEL. ACIP marks a long vowel with an apostrophe
    # before it — K'A is kA, M'A is mA — but the rule below requires a
    # CONSONANT before the apostrophe, and for a-chen the letter IS the vowel,
    # so A'A fell through as a'a and rendered ཨའ: a-chen plus an a-chung,
    # silently, with ok=True. A'A is ཨཱ.
    #
    # Only where it OPENS a syllable. Mid-word, vowel-apostrophe-vowel is the
    # achung particle and must not be touched — PA'AM, SLA'AM, LA'ANG, and
    # above all PA'I, the genitive, which alone occurs 53,651 times. Measured
    # on the shipped spine: 724 syllable-initial A'A, every one a mantra
    # (AOm A'A:H'Um — om ah hum — and A'A LO KE, aloke), against 2,094
    # mid-word, every one a particle.
    s = re.sub(r"(?<![A-Za-z])A'A", '⟦A⟧', s)
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
    # The visarga sentinel becomes EWTS 'H' only now, after case folding.
    # It cannot be a ⟦H⟧ protected span: the long-vowel apostrophe rule's
    # lookbehind class contains the closing marker ⟧ itself, so a protected
    # visarga reads as a consonant to it and ka:'i came out kaHI — the
    # genitive 'i swallowed into a long vowel — instead of kaH'i.
    return re.sub(r'\s+',' ',''.join(out)).strip().replace('\x03','H')

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
