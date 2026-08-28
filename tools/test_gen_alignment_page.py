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

if fails:
    print("\nFAILED:")
    for f in fails:
        print("  - " + f)
    sys.exit(1)
print("gen_alignment_page: all refusals hold")
