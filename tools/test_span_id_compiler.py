#!/usr/bin/env python3
"""Gate for the span-id compiler: its refusals, and the whole-bank round trip.

WHY BOTH HALVES. tools/span_id_compiler.py turns token-position proposals into
the spec gen_alignment_page.py consumes. It can fail in two different ways,
and each half of this gate watches one:

  1. It can ACCEPT what it should refuse -- worst of all, emit a spec that the
     generator resolves to a DIFFERENT occurrence than the one proposed (the
     second "the" chosen, the first one banked). Such a page passes every
     downstream gate, because every string on it is verbatim. So the PINS
     below feed it proposals that must be refused and require the refusal BY
     ITS NAME. A refusal for the wrong reason is not evidence.

  2. It can LOSE something on the way through. So the BATTERY reverse-compiles
     every landed segment and requires the regenerated blocks to be
     byte-identical to the page on disk -- an artefact produced before the
     compiler existed, which is what makes the comparison an independent
     recount rather than the generator agreeing with itself. The last round
     of work shipped a gate whose two checks both passed while 40% of its
     body was false, because one compared the generator with itself and the
     other checked only totals; this one compares with the landed bytes.

  3. It can emit a spec that means something else in a way no landed block
     exercises, because every landed block chose the occurrence the
     generator reaches. So the battery also runs an OCCURRENCE SWEEP over
     the whole bank -- every round-tripping block with one span moved to a
     later copy of its own text -- and reads every compiled variant back with
     a second reader that shares no code with the round trip's. And the
     round trip's own checks are pinned against STUB generator output below,
     because several of them are unreachable through today's generator and
     would otherwise pass while deleted. A MEMBER RE-POINT does the same for
     d=7 members, whose English the generator finds inside the parent with
     no word boundary.

  4. It can derive a subword flag for a cut no rule allows, which switches
     the generator's own accident check off. So a CUT SWEEP shortens a span
     on every round-tripping block and requires the rule's own refusal, and
     test_no_broken_words and test_no_split_syllables -- unmodified -- are
     run over every page the sweeps compile.

  5. It can show an agent numbers it does not resolve. So every landed
     segment's PACKET is read back as text and each number resolved.

And, beside the compiler, the RULE-SHAPE CENSUS: every cut on every landed
block that the written rules refuse, whatever that block's first refusal --
the scope of the rulings those shapes await, pinned in the baseline.

The battery proves the compiler loses nothing. It proves nothing about
whether an alignment is correct, and must never be read as if it did.
"""
import hashlib
import io
import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import span_id_compiler as S   # noqa: E402

ROOT = os.path.dirname(HERE)
fails = []

# A real segment, read from the spine, so every pin runs against the text the
# campaign actually aligns. C05:169 has crossing English (Geshe Michael's
# "Others" leads the English but gzhan dag sits mid-sentence in the Tibetan),
# three "the"s, and repeated Tibetan (nges pa five times).
COURSE, SEQ = "C05", 169
WYL, ENG = S.spine_row(COURSE, SEQ)
TT, ET = S.tokenize(WYL, "tib"), S.tokenize(ENG, "eng")
SRC = S.source_hash(WYL, ENG)[:S.SRC_SHOWN]


def _nth(text, toks, word, nth, where):
    """Token number of the nth token that is exactly `word`.

    A fixture word that is no longer a token (the tokenizer changed under
    it) is a named failure, never a traceback: the pin then cites a token
    that does not exist and is refused as UNKNOWN_TOKEN, and the gate goes
    on to say everything else it knows."""
    hits = [n for n, (a, b) in enumerate(toks, 1) if text[a:b] == word]
    if len(hits) < nth:
        fails.append("fixture: %s has no token #%d exactly %r -- did the "
                     "tokenizer change?" % (where, nth, word))
        return 999999
    return hits[nth - 1]


def T(word, nth=1):
    return _nth(WYL, TT, word, nth, "C05:169 Tibetan")


def E(word, nth=1):
    return _nth(ENG, ET, word, nth, "C05:169 English")


def seg(spans, **kw):
    s = {"seq": SEQ, "src": SRC, "title": "pin", "spans": spans}
    s.update(kw)
    return {"course": COURSE, "segments": [s]}


def expect_refusal(name, prop, code, needle=None, fn=None):
    try:
        (fn or S.compile_proposal)(prop)
    except S.Refusal as e:
        if e.code != code:
            fails.append("%s: refused as %s, expected %s\n    %s"
                         % (name, e.code, code, e))
        elif needle and needle not in str(e):
            fails.append("%s: refused as %s but the message lacks %r\n    %s"
                         % (name, code, needle, e))
        else:
            print("  ok   %-44s %s" % (name, code))
        return
    fails.append("%s: expected refusal %s, got a compiled spec" % (name, code))


def expect_ok(name, prop):
    try:
        out = S.compile_proposal(prop)
    except S.Refusal as e:
        fails.append("%s: expected success, refused: %s" % (name, e))
        return None
    print("  ok   %-44s compiled" % name)
    return out


print("span-id compiler pins (C05:169, %d Tibetan / %d English tokens):"
      % (len(TT), len(ET)))

# ---- the tokenizer's contract on the cases it exists for -----------------
for text, side, want in (("mdo sde pa'i sde pa", "tib",
                          ["mdo", "sde", "pa", "'i", "sde", "pa"]),
                         # a syllable-final a-chung stays with its syllable;
                         # one that carries a vowel starts the next token
                         ("'di'i dga' g.yag grub mtha', mtha'i", "tib",
                          ["'di", "'i", "dga'", "g", ".", "yag", "grub",
                           "mtha'", ",", "mtha", "'i"]),
                         ("doesn't the Buddha's self-grasping, (1)", "eng",
                          ["doesn't", "the", "Buddha's", "self", "-",
                           "grasping", ",", "(", "1", ")"])):
    got = [text[a:b] for a, b in S.tokenize(text, side)]
    if got != want:
        fails.append("tokenize %r: %r, expected %r" % (text, got, want))
    else:
        print("  ok   tokenize %-37r %d tokens" % (text[:37], len(got)))

# ---- a valid proposal, checked against literal strings -------------------
# The expected strings are typed here, not computed: an independent check of
# the slicing, the derived order and the derived flag.
w1 = "T%d-T%d" % (T("dpe"), T("pa"))            # dpe ston pa
w4 = "T%d-T%d" % (T("gzhan"), T("dag"))         # gzhan dag
W1 = {"id": "w1", "d": 5, "t": w1, "e": "E%d" % E("Exemplist")}
good = expect_ok("crossing English, derived eng_order", seg([
    {"id": "w1", "d": 5, "t": w1, "e": "E%d" % E("Exemplist")},
    {"id": "m1", "d": 7, "t": "T%d" % T("dpe"), "e": None},
    {"id": "w4", "d": 5, "t": w4, "e": "E%d" % E("Others")},
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": None, "cls": "topic",
     "nul": "topic marker; unrendered"}]))
if good:
    sp = {s["id"]: s for s in good["segments"][0]["spans"]}
    order = [s["id"] for s in good["segments"][0]["spans"]]
    for got, want, what in (
            (sp["w1"]["tib"], "dpe ston pa", "w1 tib"),
            (sp["w1"]["eng"], "Exemplist", "w1 eng"),
            (sp["w4"]["eng"], "Others", "w4 eng"),
            (sp["m1"]["tib"], "dpe", "m1 tib"),
            (order, ["w1", "m1", "w4", "p1"], "Tibetan order, member after parent"),
            (good["segments"][0].get("eng_order"), ["w4", "w1"], "eng_order"),
            (sp["p1"].get("nul"), "topic marker; unrendered", "nul carried"),
            ("subword" in sp["w1"], False, "no subword where nothing is cut")):
        if got != want:
            fails.append("valid proposal: %s = %r, expected %r" % (what, got, want))
    # and the REAL generator program, as a separate process, accepts it
    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False,
                                     encoding="utf-8") as f:
        json.dump(good, f, ensure_ascii=False)
    p = subprocess.run([sys.executable,
                        os.path.join(ROOT, "tools", "gen_alignment_page.py")],
                       stdin=io.open(f.name, encoding="utf-8"),
                       capture_output=True, text=True, cwd=ROOT)
    os.unlink(f.name)
    if p.returncode != 0 or 'data-l="s169w4">Others</span>' not in p.stdout:
        fails.append("the emitted spec does not pass the generator CLI: %s"
                     % p.stderr.strip()[:200])
    else:
        print("  ok   %-44s exit 0" % "the generator program accepts the spec")

# ---- a derived subword flag, on a landed n't -----------------------------
# C05:34 banks med -> the n't of a contraction; the compiler must set the
# flag itself, because the n't sits inside a larger word.
w34, e34 = S.spine_row("C05", 34)
t34, et34 = S.tokenize(w34, "tib"), S.tokenize(e34, "eng")
med = _nth(w34, t34, "med", 1, "C05:34 Tibetan")
nt = ([n for n, (a, b) in enumerate(et34, 1) if "n't" in e34[a:b]]
      or [999999])[0]
sub = expect_ok("derived subword on E[n't]", {"course": "C05", "segments": [{
    "seq": 34, "src": S.source_hash(w34, e34)[:S.SRC_SHOWN], "title": "pin",
    "spans": [{"id": "w2", "d": 5, "t": "T%d" % med, "e": "E%d[n't]" % nt}]}]})
if sub:
    s0 = sub["segments"][0]["spans"][0]
    if s0.get("subword") is not True or s0["eng"] != "n't":
        fails.append("derived subword: got %r" % s0)

# ---- THE DANGER CASE: a spec that would mean something else --------------
# Choosing the THIRD "the" with nothing claimed before it. Fed the string
# "the", the generator's forward cursor lands on the first. Must be refused.
expect_refusal("English 'the' resolves to an earlier occurrence", seg([
    {"id": "w4", "d": 5, "t": w4, "e": "E%d" % E("Others")},
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": "E%d" % E("the", 3)}]),
    "ROUND_TRIP_MISMATCH", "resolves the string to E%d" % E("the"))
expect_refusal("Tibetan 'nges pa' resolves to an earlier occurrence", seg([
    {"id": "w9", "d": 5, "t": "T%d-T%d" % (T("nges", 4), T("nges", 4) + 1),
     "e": None}]),
    "ROUND_TRIP_MISMATCH", "resolves the string to T%d" % T("nges"))

# The generator keeps ONE subword flag per span and applies it to both sides.
# C03:427: ma -> the n't of "shouldn't" needs the flag for its English, and
# the flag then lets the Tibetan search for "ma" stop inside `mar`, eleven
# syllables early. The compiler must refuse, and say why.
w427, e427 = S.spine_row("C03", 427)
t427, et427 = S.tokenize(w427, "tib"), S.tokenize(e427, "eng")
ma = _nth(w427, t427, "ma", 1, "C03:427 Tibetan")
sh = _nth(e427, et427, "shouldn't", 1, "C03:427 English")
expect_refusal("subword flag relaxes the Tibetan search too", {
    "course": "C03", "segments": [{
        "seq": 427, "src": S.source_hash(w427, e427)[:S.SRC_SHOWN],
        "title": "pin", "spans": [{"id": "w1", "d": 5, "t": "T%d" % ma,
                                   "e": "E%d[n't]" % sh}]}]},
    "ROUND_TRIP_MISMATCH", "applies that flag to BOTH")

# ---- token references ---------------------------------------------------
for name, ref, code in (
        ("unknown token id", "T%d" % (len(TT) + 5), "UNKNOWN_TOKEN"),
        ("range out of bounds", "T%d-T%d" % (len(TT) - 1, len(TT) + 3),
         "RANGE_OUT_OF_BOUNDS"),
        ("reversed range", "T5-T3", "REVERSED_RANGE"),
        ("English token in the Tibetan field", "E3", "WRONG_SIDE"),
        ("malformed reference", "3-5", "BAD_REF"),
        ("sub-token substring absent", "T%d[xyz]" % T("pa"), "SUBSTRING_ABSENT"),
        ("bracket that touches only the middle token",
         "T%d-T%d[ston]" % (T("dpe"), T("pa")), "RANGE_NOT_TIGHT")):
    expect_refusal(name, seg([{"id": "w1", "d": 5, "t": ref, "e": None}]), code)
expect_refusal("sub-token substring not unique within its token", seg([
    {"id": "w5", "d": 5, "t": "T%d" % T("mu"),
     "e": "E%d[o]" % E("combinations")}]), "SUBSTRING_NOT_UNIQUE")

# ---- the source hash ----------------------------------------------------
expect_refusal("stale hash", seg([], src="0" * 16), "STALE_HASH")
expect_refusal("missing hash", seg([], src=""), "MISSING_HASH")
other = hashlib.sha256(
    ("span-id-tok/0\0%s\0%s" % (WYL, ENG)).encode()).hexdigest()[:16]
expect_refusal("hash from another tokenizer", seg([], src=other), "STALE_HASH")
# The contract, restated independently: sha256 over the tokenizer name, NUL,
# the wylie, NUL, the English, and 16 hex digits of it shown and echoed. A
# hash that stopped covering one side would let an edit there leave every
# outstanding proposal looking current.
indep = hashlib.sha256(("%s\0%s\0%s" % (S.TOKENIZER, WYL, ENG))
                       .encode("utf-8")).hexdigest()[:16]
if SRC != indep or "src=%s " % indep not in S.packet(COURSE, SEQ):
    fails.append("the src contract moved: packet/compiler say %s, the "
                 "documented contract gives %s" % (SRC, indep))
else:
    print("  ok   %-44s %s" % ("src hash contract, recomputed independently",
                               indep))
full = hashlib.sha256(("%s\0%s\0%s" % (S.TOKENIZER, WYL, ENG))
                      .encode("utf-8")).hexdigest()
expect_refusal("8-digit prefix of the right hash", seg([], src=full[:8]),
               "BAD_HASH")
expect_refusal("the full 64-digit hash", seg([], src=full), "BAD_HASH")
expect_refusal("upper-case hash", seg([], src=SRC.upper()), "BAD_HASH")
# a spine edit on EITHER side must stale a proposal written before it
_real_row = S.spine_row
for side_name, edited in (("English", (WYL, ENG + " x")),
                          ("wylie", (WYL + " x", ENG))):
    S.spine_row = (lambda e: lambda c, q: e if (c, q) == (COURSE, SEQ)
                   else _real_row(c, q))(edited)
    expect_refusal("spine %s edited after the packet" % side_name,
                   seg([W1]), "STALE_HASH")
    S.spine_row = _real_row

# ---- members ------------------------------------------------------------
expect_refusal("d=7 member with no parent", seg([
    {"id": "m1", "d": 7, "t": "T%d" % T("ni"), "e": None}]), "D7_NO_PARENT")
expect_refusal("d=7 member not inside its parent", seg([
    W1, {"id": "w2", "d": 5, "t": "T%d-T%d" % (T("mdo"), T("pa", 2)), "e": None},
    {"id": "m1", "d": 7, "t": "T%d-T%d" % (T("pa"), T("mdo")), "e": None}]),
    "MEMBER_NOT_INSIDE_PARENT")
expect_refusal("d=7 member not tighter than its parent", seg([
    W1, {"id": "m1", "d": 7, "t": w1, "e": None}]), "MEMBER_NOT_TIGHTER")
expect_refusal("d=7 member English is the parent's whole English", seg([
    W1, {"id": "m1", "d": 7, "t": "T%d" % T("ston"),
         "e": "E%d" % E("Exemplist")}]), "MEMBER_NOT_TIGHTER")
expect_refusal("d=7 member not unique in its parent", seg([
    {"id": "w8", "d": 5, "t": "T%d-T%d" % (T("rnam"), T("smin")), "e": None},
    {"id": "m1", "d": 7, "t": "T%d[m]" % T("rnam"), "e": None}]),
    "MEMBER_NOT_UNIQUE_IN_PARENT")
expect_refusal("d=7 member English outside its parent's", seg([
    W1, {"id": "m1", "d": 7, "t": "T%d" % T("ston"),
         "e": "E%d" % E("group")}]), "MEMBER_ENGLISH_OUTSIDE_PARENT")
# the shape the generator renders by silently DROPPING the member's English
expect_refusal("d=7 member English under a null parent", seg([
    {"id": "w1", "d": 5, "t": w1, "e": None},
    {"id": "m1", "d": 7, "t": "T%d" % T("ston"), "e": "E%d" % E("Exemplist")}]),
    "MEMBER_ENGLISH_UNDER_NULL_PARENT")

# ---- null spans, the flat layer, and the format --------------------------
expect_refusal("English range supplied for a null span", seg([
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": "E%d" % E("the"),
     "nul": "unrendered"}]), "NUL_WITH_ENGLISH")
expect_refusal("flat span inside a flat span", seg([
    {"id": "c1", "d": 2, "t": "T1-T%d" % T("dag"), "e": None}, W1]),
    "FLAT_NESTED")
expect_refusal("flat spans overlapping", seg([
    W1, {"id": "w2", "d": 5, "t": "T%d-T%d" % (T("pa"), T("sde")), "e": None}]),
    "FLAT_OVERLAP")
expect_refusal("English spans overlapping", seg([
    {"id": "w1", "d": 5, "t": w1,
     "e": "E%d-E%d" % (E("the"), E("Exemplist"))},
    {"id": "w3", "d": 5, "t": "T%d-T%d" % (T("sde", 2), T("pa", 3)),
     "e": "E%d-E%d" % (E("Exemplist"), E("group"))}]), "ENGLISH_OVERLAP")
expect_refusal("duplicate id", seg([W1, dict(W1, t="T%d" % T("mu"), e=None)]),
               "DUPLICATE_ID")
for bad_id in ("w-1", "1w", "S"):
    expect_refusal("bad id %r" % bad_id, seg([dict(W1, id=bad_id)]), "BAD_ID")
expect_refusal("retyped tib supplied", seg([dict(W1, tib="dpe ston pa")]),
               "DERIVED_KEY_SUPPLIED")
expect_refusal("eng_order supplied", seg([W1], eng_order=["w1"]),
               "DERIVED_KEY_SUPPLIED")
expect_refusal("unknown span key", seg([dict(W1, parent="w0")]), "UNKNOWN_KEY")
expect_refusal("depth outside 1-7", seg([dict(W1, d=8)]), "BAD_DEPTH")
expect_refusal("span with no Tibetan", seg([dict(W1, t=None)]), "NO_TIBETAN")
expect_refusal("missing title", seg([W1], title=""), "MISSING_TITLE")
expect_refusal("segment not in the spine", {"course": COURSE, "segments": [
    {"seq": 999999, "src": SRC, "title": "t", "spans": []}]}, "NOT_IN_SPINE")
# The generator's own refusal, and its crash, must come back named -- never
# as a traceback and never as a spec. Every shape the generator refuses on
# its own is now caught earlier by a named check here, so the pass-through
# is pinned by standing a refusing (and a crashing) generator in its place.
_real_main = S.G.main


def _refusing():
    raise SystemExit("REFUSED: s169 stand-in refusal for the pass-through pin")


def _crashing():
    raise ValueError("stand-in crash")


S.G.main = _refusing
expect_refusal("generator's own refusal passes through by name", seg([W1]),
               "GENERATOR_REFUSED", "stand-in refusal")
S.G.main = _crashing
expect_refusal("generator crash comes back named", seg([W1]),
               "GENERATOR_CRASHED", "stand-in crash")
S.G.main = _real_main
expect_refusal("empty class", seg([dict(W1, cls="")]), "BAD_CLS")
expect_refusal("class with an edge space", seg([dict(W1, cls=" noun")]),
               "BAD_CLS")

# ---- what a refusal tells the agent to do ---------------------------------
# The commonest refusal is the unreachable occurrence. Its advice must never
# be "choose that earlier occurrence" (banking a place nobody judged, rule 10)
# or "claim the text in between" (over-capture, rule 7).
W4 = {"id": "w4", "d": 5, "t": w4, "e": "E%d" % E("Others")}
for name, prop, need in (
        ("English", seg([W4, {"id": "p1", "d": 6, "t": "T%d" % T("ni"),
                              "e": "E%d" % E("the", 3)}]),
         "Null this span's English"),
        ("Tibetan", seg([{"id": "w9", "d": 5, "t": "T%d-T%d"
                          % (T("nges", 4), T("nges", 4) + 1), "e": None}]),
         "Drop this span")):
    try:
        S.compile_proposal(prop)
        fails.append("unreachable %s occurrence: compiled" % name)
    except S.Refusal as e:
        msg = str(e)
        wrong = [x for x in ("choose that earlier", "Claim the text in between")
                 if x in msg]
        if e.code != "ROUND_TRIP_MISMATCH" or wrong or need not in msg \
                or "rule 7" not in msg or "rule 10" not in msg:
            fails.append("unreachable %s occurrence: the advice is wrong "
                         "(%s): %s" % (name, wrong or "missing %r" % need, msg))
        else:
            print("  ok   %-44s names rules 7 and 10" %
                  ("advice for an unreachable %s occurrence" % name))


# ---- the cut rules, before the subword flag is derived --------------------
def other(course, seq, spans, **kw):
    w, e = S.spine_row(course, seq)
    sg = {"seq": seq, "src": S.source_hash(w, e)[:S.SRC_SHOWN],
          "title": "pin", "spans": spans}
    sg.update(kw)
    return {"course": course, "segments": [sg]}


def tok(course, seq, side, word, nth=1):
    w, e = S.spine_row(course, seq)
    text = w if side == "tib" else e
    return _nth(text, S.tokenize(text, side), word, nth,
                "%s:%d %s" % (course, seq, side))


# accidental cuts the derived flag used to turn into declared ones
expect_refusal("English cut outside the affix convention", seg([
    {"id": "w1", "d": 5, "t": w1, "e": "E%d[Exemp]" % E("Exemplist")}]),
    "SUBWORD_NOT_ALLOWED", "Exemplist")
expect_refusal("Tibetan cut not before a suffix", seg([
    {"id": "w1", "d": 5, "t": "T%d[dp]" % T("dpe"), "e": None}]),
    "SYLLABLE_SPLIT", "'dpe'")
# rule 9 on C05:73 `ston pas`, `mi nus pas` -> "unable"
C73 = ("C05", 73)
ston, pas = tok(*C73, "tib", "ston"), tok(*C73, "tib", "pas")
mi, nus = tok(*C73, "tib", "mi"), tok(*C73, "tib", "nus")
unable = tok(*C73, "eng", "unable")
expect_refusal("agentive s split off a short stem", other(*C73, [
    {"id": "w1", "d": 5, "t": "T%d[pa]" % pas, "e": None}]), "SYLLABLE_SPLIT")
expect_refusal("agentive s split off a flat word (rule 9)", other(*C73, [
    {"id": "w1", "d": 5, "t": "T%d-T%d[ston pa]" % (ston, pas), "e": None}]),
    "SYLLABLE_SPLIT", "rule 9")
expect_ok("member names the stem inside a word keeping its s", other(*C73, [
    {"id": "w1", "d": 5, "t": "T%d-T%d" % (ston, pas), "e": None},
    {"id": "m1", "d": 7, "t": "T%d-T%d[ston pa]" % (ston, pas), "e": None}]))
# (the `min` span keeps the flagged "mi" search from stopping inside `min`
# eleven syllables early -- the C03:427 class, refused when it is missing)
un = expect_ok("negative affix and its stem, both sides of one cut",
               other(*C73, [
                   {"id": "w0", "d": 5, "t": "T%d" % tok(*C73, "tib", "min"),
                    "e": None},
                   {"id": "w1", "d": 5, "t": "T%d" % mi,
                    "e": "E%d[un]" % unable},
                   {"id": "w2", "d": 5, "t": "T%d" % nus,
                    "e": "E%d[able]" % unable}]))
if un and [x.get("subword") for x in un["segments"][0]["spans"]] != \
        [None, True, True]:
    fails.append("un|able: subword not derived on both: %r" % un)
expect_refusal("a stem cut with no affix claimed beside it", other(*C73, [
    {"id": "w2", "d": 5, "t": "T%d" % nus, "e": "E%d[able]" % unable}]),
    "SUBWORD_NOT_ALLOWED", "unable")
# the terminative r: `phyi ma` out of `phyi mar` (C03:235) is a legal cut,
# and it is the Tibetan bracket example the packet itself shows
C235 = ("C03", 235)
tr = expect_ok("terminative r split off a flat word", other(*C235, [
    {"id": "w1", "d": 5, "t": "T%d-T%d[phyi ma]"
     % (tok(*C235, "tib", "phyi"), tok(*C235, "tib", "mar")), "e": None}]))
if tr and (tr["segments"][0]["spans"][0]["tib"] != "phyi ma"
           or tr["segments"][0]["spans"][0].get("subword") is not True):
    fails.append("phyi ma out of phyi mar: %r" % tr["segments"][0]["spans"][0])
# rule 3: a bound 'i is never a flat span, at depth 6 or dressed up as 5
gi = T("'i")
for d in (6, 5):
    expect_refusal("bound 'i as a flat d=%d span" % d, seg([
        {"id": "p1", "d": d, "t": "T%d" % gi, "e": None}]),
        "FLAT_STARTS_MID_SYLLABLE", "rule 3")
expect_ok("bound 'i as a d=7 member of its word", seg([
    {"id": "w2", "d": 5, "t": "T%d-T%d" % (T("mdo"), gi), "e": None},
    {"id": "m1", "d": 7, "t": "T%d" % gi, "e": None}]))

def rng(course, seq, side, phrase, nth=1):
    """The token range a proposal would cite for the nth token-aligned copy
    of `phrase`. A fixture that no longer exists is a named failure."""
    w, e = S.spine_row(course, seq)
    text = w if side == "tib" else e
    toks = S.tokenize(text, side)
    starts = {a: n for n, (a, _b) in enumerate(toks, 1)}
    ends = {b: n for n, (_a, b) in enumerate(toks, 1)}
    k, hit = -1, 0
    while True:
        k = text.find(phrase, k + 1)
        if k < 0:
            fails.append("fixture: %s:%d %s has no token-aligned copy #%d of "
                         "%r" % (course, seq, side, nth, phrase))
            return "T999999" if side == "tib" else "E999999"
        if k in starts and k + len(phrase) in ends:
            hit += 1
            if hit == nth:
                L = "T" if side == "tib" else "E"
                i, j = starts[k], ends[k + len(phrase)]
                return "%s%d" % (L, i) if i == j else "%s%d-%s%d" % (L, i, L, j)


# ---- the cut rules at every depth, not only two d=5 examples --------------
# Two mutants survived the last gate: every English cut of three characters
# or fewer allowed, and depth 6 exempted. The battery's CUT SWEEP holds the
# rule bank-wide; these pin the verifier's own examples.
expect_refusal("'the' cut out of 'Others' (3 characters)", seg([
    {"id": "w4", "d": 5, "t": w4, "e": "E%d[the]" % E("Others")}]),
    "SUBWORD_NOT_ALLOWED", "Others")
C103 = ("C05", 103)
expect_refusal("'sis' cut out of 'consisting'", other(*C103, [
    {"id": "w1", "d": 5, "t": rng(*C103, "tib", "sems pa"),
     "e": rng(*C103, "eng", "consisting") + "[sis]"}]),
    "SUBWORD_NOT_ALLOWED", "consisting")
expect_refusal("an English cut on a depth-6 span", seg([
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": "E%d[the]" % E("Others")}]),
    "SUBWORD_NOT_ALLOWED", "Others")
expect_refusal("an English cut on a d=7 member", seg([
    W1, {"id": "m1", "d": 7, "t": "T%d" % T("ston"),
         "e": "E%d[plist]" % E("Exemplist")}]),
    "SUBWORD_NOT_ALLOWED", "Exemplist")

# ---- case: exactly as test_no_broken_words compares -----------------------
# A capitalised "Un" standing alone is refused by that gate, so the compiler
# must refuse it too (C03:42 "Unsurpassed", on med) ...
C42 = ("C03", 42)
expect_refusal("a capitalised 'Un' standing alone", other(*C42, [
    {"id": "w1", "d": 5, "t": "T%d" % tok(*C42, "tib", "med"),
     "e": "E%d[Un]" % tok(*C42, "eng", "Unsurpassed")}]),
    "SUBWORD_NOT_ALLOWED", "Unsurpassed")
# ... while C03:229's "Un|certain", two members meeting at the cut, which
# that gate passes because it sees a tag there, still compiles.
C229 = ("C03", 229)
hit229 = S.find_landed(*C229)
if hit229 is None:
    fails.append("fixture: C03:229 is on no landed page")
else:
    p229, _t = S.reverse(C229[0], C229[1], "pin", hit229[2], hit229[3],
                         *S.spine_row(*C229))
    if not any(s.get("e", "") and s["e"].endswith("[Un]")
               for s in p229["spans"]):
        fails.append("fixture: C03:229 no longer carries the member 'Un'")
    expect_ok("'Un|certain': two members meeting at the cut",
              {"course": "C03", "segments": [p229]})

# ---- an apostrophe cut is legal, and flags nothing -------------------------
# The packet says so; 30 landed English brackets cut there (Buddha|'s).
C332 = ("C05", 332)
hit332 = S.find_landed(*C332)
if hit332 is None:
    fails.append("fixture: C05:332 is on no landed page")
else:
    p332, _t = S.reverse(C332[0], C332[1], "pin", hit332[2], hit332[3],
                         *S.spine_row(*C332))
    ids = [s["id"] for s in p332["spans"]
           if (s.get("e") or "").endswith("[someone else]")]
    if not ids:
        fails.append("fixture: C05:332 no longer cuts 'someone else' out of "
                     "\"someone else's\"")
    ap = expect_ok("an English span cut at an apostrophe",
                   {"course": "C05", "segments": [p332]})
    if ap and ids and [s.get("subword") for s in ap["segments"][0]["spans"]
                       if s["id"] == ids[0]] != [None]:
        fails.append("an apostrophe cut derived a subword flag")

# ---- the hyphen and dot disambiguators (rule 3) ----------------------------
expect_refusal("d=6 'yam' cut out of 'g-yam' (hyphen form)", other("C03", 30, [
    {"id": "p1", "d": 6, "t": "T%d" % tok("C03", 30, "tib", "yam"),
     "e": None}]), "FLAT_STARTS_MID_SYLLABLE", "rule 3")
expect_refusal("d=6 'yel' cut out of 'g.yel' (dot form)", other("P1", 38, [
    {"id": "p1", "d": 6, "t": "T%d" % tok("P1", 38, "tib", "yel"),
     "e": None}]), "FLAT_STARTS_MID_SYLLABLE", "rule 3")
expect_ok("d=6 'rje' after the compound hyphen of 'rdo-rje'",
          other("TITLS", 686, [{"id": "p1", "d": 6,
                                "t": rng("TITLS", 686, "tib", "rje"),
                                "e": None}]))

# ---- member English: the generator's raw find inside the parent ------------
# C05:103's parent 'deeds consisting of movement of the mind,' holds "of"
# twice. The generator places a member's English by the first find inside
# its parent, so the second "of" is unreachable; only the round trip's
# member-English offset check refuses it, and switching that check off
# compiled this very proposal onto E49 with every gate green.
par103 = {"id": "w", "d": 5, "t": rng(*C103, "tib", "sems pa'i"),
          "e": rng(*C103, "eng", "deeds consisting of movement of the mind,")}
sems103 = par103["t"].split("-")[0]        # the member `sems`
expect_refusal("member English on the second 'of' in its parent",
               other(*C103, [par103, {"id": "m", "d": 7, "t": sems103,
                                      "e": rng(*C103, "eng", "of", 5)}]),
               "ROUND_TRIP_MISMATCH",
               "resolves the string to %s" % rng(*C103, "eng", "of", 4))
m103 = expect_ok("member English on the first 'of' in its parent",
                 other(*C103, [par103, {"id": "m", "d": 7, "t": sems103,
                                        "e": rng(*C103, "eng", "of", 4)}]))
# the same check held to STUB output: the member put on the second "of"
if m103:
    whole103, blk103, v103 = S.compile_with_blocks(other(*C103, [
        par103, {"id": "m", "d": 7, "t": sems103,
                 "e": rng(*C103, "eng", "of", 4)}]))
    w103, e103 = S.spine_row(*C103)
    tag = '<span class="u" data-d="7" data-l="s103m">of</span>'
    tb103, eb103 = blk103[0]
    if eb103.count(tag) != 1 or eb103.count(" of the mind") != 1:
        fails.append("stub fixture: C05:103's member tag moved: %r" % eb103[:200])
    moved = eb103.replace(tag, "of").replace(" of the mind",
                                             " " + tag + " the mind")
    expect_refusal("stub: a member's English at another place in its parent",
                   (tb103, moved), "ROUND_TRIP_MISMATCH",
                   "resolves the string to",
                   fn=lambda b: S.check_blocks(b[0], b[1],
                                               whole103["segments"][0],
                                               v103[0][3], w103, e103))
# a member whose English also sits inside an earlier word of the parent:
# the generator blames the WRONG member ("'as' is not inside its compound"),
# so the compiler names the one to change
C374 = ("C03", 374)
hit374 = S.find_landed(*C374)
if hit374 is None:
    fails.append("fixture: C03:374 is on no landed page")
else:
    w374, e374 = S.spine_row(*C374)
    p374, _t = S.reverse(C374[0], C374[1], "pin", hit374[2], hit374[3],
                         w374, e374)
    ch374 = S.compile_with_blocks({"course": "C03",
                                   "segments": [p374]})[2][0][3]
    rp = [v for k, what, v, _w in S.member_repoints(p374, w374, e374, ch374)
          if what.startswith("y3 ")]
    if not rp:
        fails.append("fixture: C03:374 member y3 has no re-point variant")
    else:
        expect_refusal("member 'a' found inside 'as' by the generator",
                       {"course": "C03", "segments": [rp[0]]},
                       "MEMBER_ENGLISH_UNREACHABLE", "member y3")

# ---- the syllable-final a-chung (C05:24 `grub mtha' nas`) ------------------
C24 = ("C05", 24)
grub, mtha = tok(*C24, "tib", "grub"), tok(*C24, "tib", "mtha'")
ac = expect_ok("plain range keeps a syllable-final a-chung", other(*C24, [
    {"id": "w1", "d": 5, "t": "T%d-T%d" % (grub, mtha), "e": None}]))
if ac and ac["segments"][0]["spans"][0]["tib"] != "grub mtha'":
    fails.append("grub mtha': sliced as %r" % ac["segments"][0]["spans"][0])
expect_refusal("bracket that drops the final a-chung", other(*C24, [
    {"id": "w1", "d": 5, "t": "T%d[mtha]" % mtha, "e": None}]),
    "SYLLABLE_SPLIT", "a-chung")

# ---- the packet's own format text ----------------------------------------
# It is what every agent is shown, so its examples must be cuts that compile.
pieces = re.findall(r'"[TE]\d+(?:-[TE]\d+)?\[([^\]]+)\]"', S.FORMAT_NOTE)
if not pieces or [x for x in pieces if x in S.SUFFIX] \
        or "rule 9" not in S.FORMAT_NOTE or "16 hex" not in S.FORMAT_NOTE:
    fails.append("FORMAT_NOTE advertises a bare suffix split or omits rule 9 "
                 "/ the src form: bracket examples %r" % pieces)
else:
    print("  ok   %-44s %s" % ("format note examples are legal cuts", pieces))

# ---- the whole page: titles, notes, and one segment twice -----------------
INJECT = ('<div class="pair"><div class="eng"><span class="u" data-d="5" '
          'data-l="s169w1">group</span></div>')
expect_refusal("markup in the title", seg([W1], title="x " + INJECT),
               "MARKUP_IN_TITLE")
expect_refusal("a pair block smuggled in a note", seg([W1], note=INJECT),
               "MARKUP_IN_NOTE")
expect_refusal("a u-span smuggled in a note", seg([W1], note=(
    'see <span class="u" data-d="5" data-l="s169w1">group</span>')),
    "MARKUP_IN_NOTE")
expect_refusal("a TRANSFER comment in a note", seg([W1], note=(
    '<!-- TRANSFER v1 target="C05:169" source="C05:1" source_page="x" -->')),
    "MARKUP_IN_NOTE")
expect_ok("inline formatting in a note", seg([W1], note=(
    "<i>dpe ston pa</i> -&gt; <b>Exemplist</b>; ma -> not <code>x</code>")))
# names without case: browsers lower-case attribute names, and the page's
# script selects '.u' and [data-l]
for name, note in (
        ("upper-case attributes", '<SPAN CLASS="u" DATA-D="5" '
         'DATA-L="s169w1">group</SPAN>'),
        ("u as a second class", '<span class="case u">group</span>'),
        ("a script handler", '<b onmouseover="alert(1)">x</b>'),
        ("an unclosed tag opener", "regex (?<![a-z])zhi gnas")):
    expect_refusal("note: %s" % name, seg([W1], note=note), "MARKUP_IN_NOTE")
expect_ok("note markup landed notes carry", seg([W1], note=(
    '<span class="tib">dpe</span> <span class="case">ni</span> '
    '<span class=q>x</span> <span style="font-family:monospace">y</span>')))
twice = seg([W1])
twice["segments"].append(dict(twice["segments"][0], spans=[W4]))
expect_refusal("one segment twice in one proposal", twice, "DUPLICATE_SEQ")
# The page check must be CALLED on the compile path, not only work when a pin
# calls it directly: with the named note check switched off, a smuggled pair
# block must still be refused by the whole-page read.
_real_note = S.note_markup_problem
S.note_markup_problem = lambda note: None
expect_refusal("page check runs on the compile path", seg([W1], note=INJECT),
               "BUILDER_VIEW_MISMATCH")
S.note_markup_problem = _real_note

# ---- classes: never one of the page's own ---------------------------------
for c in ("nul", "pin hl", "NUL", "coordinated clause pair"):
    expect_refusal("cls %r" % c, seg([dict(W1, cls=c)]), "RESERVED_CLS")
expect_ok("cls 'case', which the page styles on purpose",
          seg([dict(W1, cls="case")]))
need_span = {"u", "nul", "pair", "tib", "eng", "lab", "note", "pin", "hl",
             "active"}
need_note = {"u", "nul", "pin", "hl", "active"}
if not need_span <= S.SPAN_RESERVED or not need_note <= S.NOTE_RESERVED \
        or "case" in S.SPAN_RESERVED | S.NOTE_RESERVED:
    fails.append("page-class discovery is wrong: span %r, note %r"
                 % (sorted(S.SPAN_RESERVED), sorted(S.NOTE_RESERVED)))
else:
    print("  ok   %-44s %d / %d classes" % ("page classes read from the "
                                             "generator and the shell",
                                             len(S.SPAN_RESERVED),
                                             len(S.NOTE_RESERVED)))

# ---- the optional echo: an off-by-one citation cannot compile -------------
expect_ok("echo that matches both ranges", seg([
    dict(W1, t_echo="dpe ston pa", e_echo="Exemplist")]))
expect_refusal("echo naming the neighbouring word", seg([
    dict(W1, e="E%d" % (E("Exemplist") + 1), e_echo="Exemplist")]),
    "ECHO_MISMATCH", "sits at E%d" % E("Exemplist"))
expect_refusal("echo for a null English", seg([
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": None, "e_echo": "the"}]),
    "ECHO_WITHOUT_RANGE")
expect_refusal("echo that is not text", seg([dict(W1, t_echo=3)]), "BAD_ECHO")

# ---- "list spans in any order" -------------------------------------------
# The packet promises it; every landed proposal is in Tibetan order, so the
# battery also compiles every block backwards. Here: the first pin's
# proposal scrambled, members before their parent, must compile to the
# very same spec.
scr = expect_ok("the first pin's spans listed scrambled", seg([
    {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": None, "cls": "topic",
     "nul": "topic marker; unrendered"},
    {"id": "m1", "d": 7, "t": "T%d" % T("dpe"), "e": None},
    {"id": "w4", "d": 5, "t": w4, "e": "E%d" % E("Others")},
    {"id": "w1", "d": 5, "t": w1, "e": "E%d" % E("Exemplist")}]))
if scr is not None and good is not None and scr != good:
    fails.append("scrambled order compiled to a different spec: %r"
                 % [s["id"] for s in scr["segments"][0]["spans"]])

# ---- the round trip's own checks, held to STUB output ----------------------
# Today's generator never renders text twice, an unproposed span, or a
# vanished null marker, and a landed block never exercises a wrong
# occurrence -- so these checks are pinned against doctored output, or
# deleting them would pass every other test.
GOOD = seg([W1, {"id": "m1", "d": 7, "t": "T%d" % T("dpe"), "e": None}, W4,
            {"id": "p1", "d": 6, "t": "T%d" % T("ni"), "e": None,
             "cls": "topic", "nul": "topic marker; unrendered"}])
whole, blocks, views = S.compile_with_blocks(GOOD)
TB, EB = blocks[0]
SPEC, CHOSEN = whole["segments"][0], views[0][3]
PAGE = S.run_generator_spec(whole)
W4TAG = '<span class="u" data-d="5" data-l="s169w4">'
NULP1 = re.compile(r'<span class="u" data-d="6" data-l="s169p1"><span '
                   r'class="nul">[^<]*</span></span>')


def stub(name, tb, eb, needle):
    expect_refusal(name, (tb, eb), "ROUND_TRIP_MISMATCH", needle,
                   fn=lambda b: S.check_blocks(b[0], b[1], SPEC, CHOSEN,
                                               WYL, ENG))


try:
    S.check_blocks(TB, EB, SPEC, CHOSEN, WYL, ENG)
    print("  ok   %-44s passes" % "the undoctored block")
except S.Refusal as e:
    fails.append("the undoctored stub block is refused: %s" % e)
# the doctoring below needs these; if the generator's output shape moves,
# say so instead of doctoring nothing and "passing"
fixture = [W4TAG + "Others</span>" in EB, W4TAG + "gzhan dag</span>" in TB,
           bool(NULP1.search(EB)), ENG.count("namely") == 1,
           WYL.count("bzhi") == 1]
if not all(fixture):
    fails.append("stub fixture no longer matches the generator's output: %r"
                 % fixture)
stub("stub: English text not the spine's", TB,
     EB.replace(W4TAG + "Others<", W4TAG + "Others!<"), "is not the spine")
stub("stub: an English span nobody proposed", TB,
     EB.replace("namely", '<span class="u" data-d="5" data-l="s169zz">'
                "namely</span>", 1), "not proposed")
stub("stub: a null marker vanished", TB, NULP1.sub("", EB, 1), "null marker")
stub("stub: English at another place", TB,
     EB.replace(W4TAG + "Others</span>", "Others").replace(
         "namely", W4TAG + "namely</span>", 1), "resolves the string to")
stub("stub: Tibetan at another place",
     TB.replace(W4TAG + "gzhan dag</span>", "gzhan dag").replace(
         "bzhi", W4TAG + "bzhi</span>", 1), EB, "resolves the string to")
# the page check behind the per-segment one: what the builder would bank
expect_refusal("stub: a page carrying an extra pair block",
               PAGE + INJECT, "BUILDER_VIEW_MISMATCH",
               fn=lambda pg: S.check_page(pg, blocks, views))
expect_refusal("stub: a page carrying one segment twice",
               PAGE + PAGE, "BUILDER_VIEW_MISMATCH", "twice",
               fn=lambda pg: S.check_page(pg, blocks * 2, views * 2))

# ---- the packet ---------------------------------------------------------
pk = S.packet(COURSE, SEQ)
for need in ("src=%s" % SRC, "[1]dpe", "[%d]Others" % E("Others"),
             "PROPOSAL FORMAT"):
    if need not in pk:
        fails.append("packet lacks %r" % need)
print("  ok   %-44s %d bytes (raw segment %d)"
      % ("packet", len(pk.encode()), len((WYL + ENG).encode())))
# The numbers an agent sees must be the numbers the compiler resolves. The
# battery checks every landed segment; these pin the folio case, where the
# markers sit among literal brackets and digits, and prove the check can
# fail by standing a renderer one number ahead in for the real one.
for c, q in (("C05", 120), (COURSE, SEQ), ("C03", 28)):
    err, _n = S.packet_contract(c, q, *S.spine_row(c, q))
    if err:
        fails.append("packet contract %s:%d: %s" % (c, q, err))
    else:
        print("  ok   %-44s every number resolves as shown"
              % ("packet contract %s:%d" % (c, q)))
_real_render = S.render_tokens


def _ahead(text, toks):
    return re.sub(r"\[(\d+)\]", lambda m: "[%d]" % (int(m.group(1)) + (
        m.group(1) != "1")), _real_render(text, toks))


S.render_tokens = _ahead
err, _n = S.packet_contract(COURSE, SEQ, WYL, ENG)
S.render_tokens = _real_render
if not err or "markers run" not in err:
    fails.append("packet contract did not see a renderer one number ahead: "
                 "%r" % err)
else:
    print("  ok   %-44s %s" % ("packet contract fails on a bad renderer",
                               err[:40]))

# ---- the whole-bank battery ---------------------------------------------
# Run even when a pin failed: the two halves watch different failures, and a
# red gate should say everything it knows, not just the first thing.
print()
res = S.battery()
S.report(res)
bad = S.judge(res, S.load_baseline())

if fails:
    print("\nFAILED -- %d pin(s):" % len(fails))
    for f in fails:
        print("  " + f)
if bad:
    print("\nFAILED -- the whole-bank round trip:")
    for b in bad:
        print("  " + b)
if fails or bad:
    sys.exit(1)
print("\nspan-id compiler: every pin refuses by name; every landed block "
      "round-trips byte-identical or is named, with its class, in the frozen "
      "baseline")
