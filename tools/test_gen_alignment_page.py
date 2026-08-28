#!/usr/bin/env python3
"""Pins for the alignment page generator's REFUSALS.

The generator's value is not that it emits HTML -- it is that it refuses
to emit HTML when the spec disagrees with the spine. Three real authoring
errors were caught by these refusals during the C03 campaign (batches 40,
43, 52). If the refusals silently stopped working, pages would still be
produced and would quietly drift from the corpus.

So these pins test the refusals, not the output. Each was verified to FAIL
when the corresponding guard is removed -- see MUTATION notes.
"""
import json, os, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GEN = os.path.join(ROOT, "tools", "gen_alignment_page.py")


def run(spec):
    p = subprocess.run([sys.executable, GEN], input=json.dumps(spec),
                       capture_output=True, text=True)
    return p.returncode, p.stdout, p.stderr


def spec_with(spans, seq=157):
    return {"course": "C03",
            "segments": [{"seq": seq, "title": "t", "spans": spans}]}


fails = []


def expect_refusal(name, spec, needle):
    rc, out, err = run(spec)
    if rc == 0:
        fails.append("%s: expected refusal, got success" % name)
    elif needle not in err:
        fails.append("%s: refused but message lacks %r\n  got: %s"
                     % (name, needle, err.strip()[:300]))
    else:
        print("  ok   %s" % name)


def expect_success(name, spec, must_contain):
    rc, out, err = run(spec)
    if rc != 0:
        fails.append("%s: expected success, refused: %s" % (name, err.strip()[:300]))
    elif must_contain not in out:
        fails.append("%s: succeeded but output lacks %r" % (name, must_contain))
    else:
        print("  ok   %s" % name)


print("gen_alignment_page refusals:")

# 1. A span that is simply not in the spine text.
#    MUTATION: delete the `if i < 0:` branch -> IndexError/silent bad HTML.
expect_refusal(
    "absent span refuses",
    spec_with([{"id": "w1", "d": 5, "tib": "gser gyi ri bo", "eng": "The nine mental states"}]),
    "NOT PRESENT in the spine text")

# 2. Spans listed out of order. Both ARE in the text; only the order is
#    wrong. This is the error that bit C03 batches 40 and 52, and the one
#    a naive find() would swallow.
#    MUTATION: change `text.find(piece, cur)` to `text.find(piece)` ->
#    this pin fails (the generator happily emits crossed spans).
expect_refusal(
    "out-of-order spans refuse",
    spec_with([{"id": "w1", "d": 5, "tib": "mnyam par 'jog pa", "eng": "reaching deep meditation"},
               {"id": "w2", "d": 5, "tib": "sems 'jog pa", "eng": "setting the mind on the object"}]),
    "OUT OF ORDER")

# 3. The refusal names which side failed, so the author knows where to look.
rc, out, err = run(spec_with(
    [{"id": "w1", "d": 5, "tib": "sems 'jog pa", "eng": "no such english anywhere"}]))
if rc == 0 or "eng span" not in err:
    fails.append("side is not named in the refusal message: %s" % err.strip()[:200])
else:
    print("  ok   refusal names the failing side")

# 4. A segment not in the spine refuses rather than emitting an empty page.
expect_refusal("unknown segment refuses",
               spec_with([], seq=999999), "is not in the spine")

# 5. The happy path still works, and wraps BOTH sides from spine text.
rc, out, err = run(spec_with(
    [{"id": "w1", "d": 5, "tib": "sems 'jog pa", "eng": "setting the mind on the object"}]))
if rc != 0:
    fails.append("happy path refused: %s" % err.strip()[:300])
else:
    for needle in ('data-l="s157w1"', "sems &#x27;jog pa", "setting the mind on the object"):
        if needle not in out:
            fails.append("happy path output lacks %r" % needle)
    if out.count('data-l="s157w1"') != 2:
        fails.append("span should appear on BOTH sides, found %d"
                     % out.count('data-l="s157w1"'))
    else:
        print("  ok   happy path wraps both sides from spine text")

# 6. A null-English morpheme is wrapped on the Tibetan side and emitted as
#    a .nul marker -- never as invented English.
rc, out, err = run(spec_with(
    [{"id": "m1", "d": 6, "tib": "rnams yin la", "eng": None, "nul": "closing; unrendered"}]))
if rc != 0:
    fails.append("null span refused: %s" % err.strip()[:300])
elif 'class="nul"' not in out or "closing; unrendered" not in out:
    fails.append("null span did not emit a .nul marker")
else:
    print("  ok   null morpheme emits a marker, not invented English")

# 7. THE CROSSING. Tibetan "lo stong" is English "a thousand years" -- the
#    two spans are in OPPOSITE order on the two sides. Every real page has
#    crossings; this is the whole point of the alignment. The first version
#    of this generator shared ONE cursor between both sides and refused every
#    real page. Pins 1-6 all used non-crossing spans and missed it.
#    MUTATION: delete the eng_order branch -> this pin fails.
rc, out, err = run({"course": "C01", "segments": [{"seq": 123, "title": "t",
    "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "years"},
              {"id": "w2", "d": 5, "tib": "stong", "eng": "thousand"}],
    "eng_order": ["w2", "w1"]}]})
if rc != 0:
    fails.append("CROSSING refused: %s" % err.strip()[:300])
elif out.count('data-l="s123w1"') != 2 or out.count('data-l="s123w2"') != 2:
    fails.append("crossing: both spans should appear on both sides")
else:
    print("  ok   a crossing generates (separate cursor per side)")

# 8. eng_order that omits a span with English refuses -- otherwise that span
#    would silently vanish from the English block while still being banked.
expect_refusal("eng_order omitting a span refuses",
    {"course": "C01", "segments": [{"seq": 123, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "years"},
                  {"id": "w2", "d": 5, "tib": "stong", "eng": "thousand"}],
        "eng_order": ["w1"]}]},
    "eng_order omits")

# 9. eng_order naming a nonexistent span refuses (typo in the spec).
expect_refusal("eng_order with an unknown id refuses",
    {"course": "C01", "segments": [{"seq": 123, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "years"}],
        "eng_order": ["w1", "w9"]}]},
    "do not exist")

# 10. Out-of-order English is STILL caught when eng_order is given -- the
#     safety property survives the fix. This is the batch-40/52 error.
expect_refusal("out-of-order eng_order still refuses",
    {"course": "C01", "segments": [{"seq": 123, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "years"},
                  {"id": "w2", "d": 5, "tib": "stong", "eng": "thousand"}],
        "eng_order": ["w1", "w2"]}]},
    "OUT OF ORDER")

# 11. Without eng_order, a crossing is derived automatically when the
#     derivation is unambiguous.
rc, out, err = run({"course": "C01", "segments": [{"seq": 123, "title": "t",
    "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "years"},
              {"id": "w2", "d": 5, "tib": "stong", "eng": "thousand"}]}]})
if rc != 0:
    fails.append("unambiguous crossing not derived: %s" % err.strip()[:200])
else:
    print("  ok   unambiguous English order is derived automatically")

# 12. But an AMBIGUOUS derivation refuses instead of guessing. C01:132 has
#     two occurrences of "three"; picking one silently is the bug class that
#     attached 'shog' to the ACIP of 'tshogs'.
#     MUTATION: drop the `if n > 1` branch -> this pin fails.
expect_refusal("ambiguous English refuses derivation",
    {"course": "C01", "segments": [{"seq": 132, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "gsum", "eng": "three"}]}]},
    "cannot be derived without guessing")

# 13. Two spans claiming the same characters refuse.
expect_refusal("overlapping English spans refuse",
    {"course": "C01", "segments": [{"seq": 123, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "lo", "eng": "thousand years"},
                  {"id": "w2", "d": 5, "tib": "stong", "eng": "thousand"}]}]},
    "overlap")

# 14. THE NESTING BUG. C03:160 opens "de ltar sems gnas dgu ..." and ends
#     "... zhi gnas grub pa". The member `gnas` of the compound `sems gnas`
#     must resolve INSIDE its parent. Resolved flat it matches the unrelated
#     `zhi gnas` 130 characters later, and the cursor then rejects every
#     span after it -- which is exactly what happened on the first real
#     batch this tool was used for.
#     MUTATION: make resolve() treat d==7 like any other depth -> fails.
rc, out, err = run({"course": "C03", "segments": [{"seq": 160, "title": "t",
    "spans": [{"id": "w2", "d": 5, "tib": "sems gnas", "eng": "these"},
              {"id": "y2", "d": 7, "tib": "gnas", "eng": None,
               "nul": "member"},
              {"id": "w3", "d": 5, "tib": "dgu", "eng": "nine"}]}]})
if rc != 0:
    fails.append("nesting: %s" % err.strip()[:260])
elif 'data-l="s160w3"' not in out:
    fails.append("nesting: the span after the nested member was lost")
else:
    print("  ok   a depth-7 member resolves inside its compound")

# 15. A depth-7 span that is NOT inside its compound refuses, rather than
#     silently reaching outside it.
expect_refusal("depth-7 outside its compound refuses",
    {"course": "C03", "segments": [{"seq": 160, "title": "t",
        "spans": [{"id": "w2", "d": 5, "tib": "sems gnas", "eng": "these"},
                  {"id": "y9", "d": 7, "tib": "bsam gtan", "eng": None,
                   "nul": "not a member"}]}]},
    "is not inside its compound")

# 16. Two spans sharing a start offset must not crash the sort. A tuple
#     comparison that reaches the span dict raises TypeError, which is a
#     crash rather than a refusal -- and a crash after registration leaves
#     the builder pointing at a page that was never written.
#     MUTATION: drop the sort key in emit() -> TypeError, this pin fails.
rc, out, err = run({"course": "C03", "segments": [{"seq": 160, "title": "t",
    "spans": [{"id": "w2", "d": 5, "tib": "sems gnas", "eng": "nine states"},
              {"id": "y1", "d": 7, "tib": "sems", "eng": "nine"},
              {"id": "y2", "d": 7, "tib": "gnas", "eng": "states"}]}]})
if rc != 0 and "Traceback" in err:
    fails.append("shared-start spans CRASH instead of refusing: %s"
                 % err.strip()[-200:])
elif rc != 0:
    fails.append("shared-start spans refused: %s" % err.strip()[:200])
else:
    print("  ok   spans sharing a start offset sort without crashing")

# 17. The commonest authoring mistake gets a NAMED diagnostic, not a cursor
#     offset. A bound morpheme marked d=6 instead of d=7 took down four of
#     six segments on the first real batch; the refusal said only "cursor is
#     already at 53", which is true and useless.
#     MUTATION: remove the prev5 branch -> the message reverts to OUT OF
#     ORDER and this pin fails.
expect_refusal("a bound morpheme marked d=6 is named as such",
    {"course": "C03", "segments": [{"seq": 161, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "grub pa'i", "eng": "achieving"},
                  {"id": "m2", "d": 6, "tib": "'i", "eng": None,
                   "nul": "genitive"},
                  {"id": "w2", "d": 5, "tib": "rjes su", "eng": "after"}]}]},
    "must be depth 7")

# 18. A FLAT span must not match inside a larger word. C03:166 renders
#     `par shing` as "block", and "block" also sits inside "blockprint"
#     earlier in the same sentence. A raw substring search takes the
#     interior hit and consumes half of another word. Same failure as the
#     ACIP of `tshogs` once attaching to the span `shog`.
#     MUTATION: use text.find instead of find_word -> this pin fails.
rc, out, err = run({"course": "C03", "segments": [{"seq": 166, "title": "t",
    "spans": [{"id": "w1", "d": 5, "tib": "par shing", "eng": "block"},
              {"id": "w2", "d": 5, "tib": "bzhugs", "eng": "stored"}]}]})
if rc != 0:
    fails.append("word-boundary: %s" % err.strip()[:220])
else:
    import re as _re
    m = _re.search(r'<span[^>]*s166w1">([^<]*)</span>', out)
    ctx = out[max(0, out.find('s166w1') - 60):out.find('s166w1') + 90]
    if "blockprint" in ctx and "block</span>print" in out:
        fails.append("word-boundary: span landed INSIDE 'blockprint'")
    else:
        print("  ok   a flat span will not match inside a larger word")

# 19. The negative-affix convention: the Tibetan negation maps to the
#     "n't" inside "doesn't", so the span deliberately sits within a larger
#     word. Declared with "subword": true, it must be ALLOWED -- 14 spans
#     across C01 rely on this and the default refusal would break them all.
#     MUTATION: ignore the subword flag -> this pin fails.
rc, out, err = run({"course": "C01", "segments": [{"seq": 123, "title": "t",
    "spans": [{"id": "m1", "d": 6, "tib": "mi", "eng": "n't",
               "subword": True}]}]})
if rc != 0:
    fails.append("declared subword span refused: %s" % err.strip()[:220])
else:
    print("  ok   a declared sub-word span is allowed")

# 20. But an UNDECLARED one still refuses. This is the accident the rule
#     exists for: `one-pointed` cut out of "one-pointedly" on a real page.
expect_refusal("an undeclared sub-word span still refuses",
    {"course": "C03", "segments": [{"seq": 161, "title": "t",
        "spans": [{"id": "w1", "d": 5, "tib": "rtse gcig",
                   "eng": "one-pointed"}]}]},
    "NOT PRESENT")

if fails:
    print("\nFAILED:")
    for f in fails:
        print("  - " + f)
    sys.exit(1)
print("gen_alignment_page: all refusals hold")
