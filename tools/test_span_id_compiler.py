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

The battery proves the compiler loses nothing. It proves nothing about
whether an alignment is correct, and must never be read as if it did.
"""
import io
import json
import os
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


def T(word, nth=1):
    """Token number of the nth Tibetan token that is exactly `word`."""
    hits = [n for n, (a, b) in enumerate(TT, 1) if WYL[a:b] == word]
    return hits[nth - 1]


def E(word, nth=1):
    hits = [n for n, (a, b) in enumerate(ET, 1) if ENG[a:b] == word]
    return hits[nth - 1]


def seg(spans, **kw):
    s = {"seq": SEQ, "src": SRC, "title": "pin", "spans": spans}
    s.update(kw)
    return {"course": COURSE, "segments": [s]}


def expect_refusal(name, prop, code, needle=None):
    try:
        S.compile_proposal(prop)
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
                         ("'di'i dga' g.yag", "tib",
                          ["'di", "'i", "dga", "'", "g", ".", "yag"]),
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
med = [n for n, (a, b) in enumerate(t34, 1) if w34[a:b] == "med"][0]
nt = [n for n, (a, b) in enumerate(et34, 1) if "n't" in e34[a:b]][0]
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
ma = [n for n, (a, b) in enumerate(t427, 1) if w427[a:b] == "ma"][0]
sh = [n for n, (a, b) in enumerate(et427, 1) if e427[a:b] == "shouldn't"][0]
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
other = __import__("hashlib").sha256(
    ("span-id-tok/0\0%s\0%s" % (WYL, ENG)).encode()).hexdigest()[:16]
expect_refusal("hash from another tokenizer", seg([], src=other), "STALE_HASH")

# ---- members ------------------------------------------------------------
W1 = {"id": "w1", "d": 5, "t": w1, "e": "E%d" % E("Exemplist")}
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
# something only the generator knows: a bound morpheme listed flat. The
# compiler has no rule of its own for it; the generator refuses, and the
# refusal must come back named, never as a crash and never as a spec.
expect_refusal("generator's own refusal passes through by name", seg([
    W1, {"id": "p9", "d": 6, "t": "T%d[a]" % T("pa", 2), "e": None}]),
    "GENERATOR_REFUSED", "BOUND")

# ---- the packet ---------------------------------------------------------
pk = S.packet(COURSE, SEQ)
for need in ("src=%s" % SRC, "[1]dpe", "[%d]Others" % E("Others"),
             "PROPOSAL FORMAT"):
    if need not in pk:
        fails.append("packet lacks %r" % need)
print("  ok   %-44s %d bytes (raw segment %d)"
      % ("packet", len(pk.encode()), len((WYL + ENG).encode())))

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
      "round-trips byte-identical or is a named, frozen legacy class")
