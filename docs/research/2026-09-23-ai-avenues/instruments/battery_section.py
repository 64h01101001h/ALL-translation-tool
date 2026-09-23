def span_census(tb, eb):
    """Independent recount: span tags in a block by regex, not by walk()."""
    return len(re.findall(r'<span class="u', tb)) + \
        len(re.findall(r'<span class="u', eb))


LABS = re.compile(r'<div class="tib"><div class="lab">([^<]*)</div>.*?'
                  r'<div class="eng"><div class="lab">([^<]*)</div>', re.S)

# The two hand-built protocols that open and punctuate every course, named by
# the labels their protocols prescribe (C03_CAMPAIGN.md, PHONETICS PROTOCOL
# rule 1; the E-071 corpus-marker protocol). Their blocks are built outside the
# generator BY DESIGN -- depth-7 sound-to-spelling links hung off a clause, an
# unpaired apparatus clause -- which the generator refuses by rule, so no
# proposal could ever produce them and no compiler should. They recur in every
# new course, which is why they are recognised by the label the page itself
# carries rather than frozen by name.
PROTOCOL_PREFIX = (("eng", "GESHE MICHAEL'S PHONETICS", "PHONETICS_PROTOCOL"),
                   ("tib", "CORPUS MARKER", "MARKER_PROTOCOL"))

# A protocol label excuses a hand-built SHAPE. It never excuses these, which
# would mean the compiler itself misbehaved on the block.
NEVER_EXCUSED = {"RENDER_DIFFERS", "COMPILER_FAULT", "GENERATOR_CRASHED"}


def generator_labels(course, seq):
    """The two pair labels the generator writes, read from its own output.

    Asked of the generator rather than copied here, so a relabelled generator
    cannot leave this battery classifying blocks by yesterday's labels."""
    out = run_generator(course, {"seq": seq, "title": "t", "spans": []})
    m = LABS.search(out)
    if not m:
        raise SystemExit("REFUSED: cannot read the generator's own pair labels")
    return m.group(1), m.group(2)


def block_kind(tlab, elab, gen):
    if (tlab, elab) == gen:
        return "GENERATOR"
    for side, prefix, kind in PROTOCOL_PREFIX:
        if (tlab if side == "tib" else elab).startswith(prefix):
            return kind
    return "OTHER_LABEL"


def battery(verbose=False):
    """Reverse-compile every landed segment. Returns a result dict.

    Two measurements, kept apart on purpose:
      * SPAN SIDES -- every span on every readable block whose text is the
        spine's, expressed as a token reference on its own, whatever the rest
        of its segment looks like. This is the token scheme's coverage.
      * BLOCKS -- whole segments reverse-compiled, compiled, regenerated, and
        compared byte for byte with the page. This is the round trip.
    """
    courses = dir_courses()
    res = {"pages": 0, "tags_on_pages": 0, "tags_in_pairs": 0, "pairs": 0,
           "wrappers": 0, "nul_markers": 0, "spec_spans_all": 0,
           "unreadable_tags": 0, "segments_ok": 0, "spans_ok": 0,
           "sides": {}, "side_examples": {}, "side_depths": {},
           "sides_unaddressable": 0, "blocks": {}, "classes": {},
           "census_errors": [], "tokenizer_errors": [], "gen_labels": None}

    def tally_side(key, d, code, example):
        res["sides"][code] = res["sides"].get(code, 0) + 1
        if code not in ("plain", "bracket"):
            res["side_examples"].setdefault(code, "%s  %s" % (key, example))
            dd = res["side_depths"].setdefault(code, {})
            dd[str(d)] = dd.get(str(d), 0) + 1

    def record(key, kind, code, detail, nspec):
        res["blocks"][key] = {"kind": kind, "class": code, "spans": nspec}
        if code == "OK":
            return
        c = res["classes"].setdefault(code, {"segments": 0, "spans": 0,
                                             "kinds": {}, "example": None})
        c["segments"] += 1
        c["spans"] += nspec
        c["kinds"][kind] = c["kinds"].get(kind, 0) + 1
        if c["example"] is None:
            c["example"] = "%s  %s" % (key, detail[:150])

    for d, course in sorted(courses.items()):
        for path in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            res["pages"] += 1
            doc, segs = page_segments(path)
            res["tags_on_pages"] += len(re.findall(r'<span class="u', doc))
            page = os.path.basename(path)[:-5]
            chunks = doc.split('<div class="pair">')[1:]
            seen_here = {}
            for (seq, title, tb, eb, ntags), chunk in zip(segs, chunks):
                res["pairs"] += 1
                res["tags_in_pairs"] += ntags
                key = "%s/%s:%s" % (d, page, seq)
                if seq is not None:
                    seen_here[seq] = seen_here.get(seq, 0) + 1
                    if seen_here[seq] > 1:
                        key += "#%d" % seen_here[seq]
                if res["gen_labels"] is None and seq is not None:
                    res["gen_labels"] = generator_labels(course, seq)
                m = LABS.search('<div class="pair">' + chunk)
                kind = block_kind(m.group(1), m.group(2), res["gen_labels"]) \
                    if m else "OTHER_LABEL"
                if tb is None:
                    res["unreadable_tags"] += ntags
                    record(key, kind, "PAIR_BLOCK_UNREADABLE",
                           "pair block not in the generator's shape", ntags)
                    continue
                try:
                    _, ts = walk(tb)
                    _, es = walk(eb)
                except Unparsed as e:
                    res["unreadable_tags"] += ntags
                    record(key, kind, "PAIR_BLOCK_UNREADABLE", str(e), ntags)
                    continue
                # the independent recount: walk() must see every tag the
                # regex sees, or the battery is blind to part of the page
                if len(ts) + len(es) != span_census(tb, eb):
                    res["census_errors"].append(
                        "%s: walk saw %d spans, the page has %d span tags"
                        % (key, len(ts) + len(es), span_census(tb, eb)))
                wrap = "s%sS" % seq
                res["wrappers"] += sum(1 for s in ts + es if s["label"] == wrap)
                res["nul_markers"] += sum(1 for s in es if s["nul"] is not None)
                sides = [("tib", s) for s in ts if s["label"] != wrap] + \
                        [("eng", s) for s in es
                         if s["label"] != wrap and s["nul"] is None]
                nspec = len(sides)
                res["spec_spans_all"] += nspec
                row = spine_row(course, seq) if seq is not None else None
                if row is None:
                    res["sides_unaddressable"] += nspec
                    record(key, kind, "NOT_IN_SPINE", "no spine row", nspec)
                    continue
                wyl, eng = row
                for side, text in (("tib", wyl), ("eng", eng)):
                    err = check_tokens(text, side)
                    if err:
                        res["tokenizer_errors"].append("%s %s: %s"
                                                       % (key, side, err))
                tp, _ = walk(tb)
                ep, _ = walk(eb)
                if tp == wyl and ep == eng:
                    toks = {"tib": tokenize(wyl, "tib"),
                            "eng": tokenize(eng, "eng")}
                    for side, s in sides:
                        text = wyl if side == "tib" else eng
                        if s["a"] == s["b"]:
                            tally_side(key, s["d"], "EMPTY_SPAN", s["label"])
                            continue
                        try:
                            _, how = express(text, toks[side], s["a"], s["b"],
                                             "T" if side == "tib" else "E")
                            tally_side(key, s["d"], how, "")
                        except Inexpressible as e:
                            tally_side(key, s["d"], e.code,
                                       "%s d=%d %s" % (s["label"], s["d"], e.msg))
                else:
                    res["sides_unaddressable"] += nspec
                try:
                    prop, _t = reverse(course, seq, title, tb, eb, wyl, eng)
                except Inexpressible as e:
                    record(key, kind, e.code, e.msg, nspec)
                    continue
                try:
                    spec, chosen = compile_segment(course, prop)
                    rtb, reb = round_trip(course, spec, chosen, wyl, eng)
                except Refusal as e:
                    record(key, kind, e.code, str(e), nspec)
                    continue
                except Exception as e:     # never crash the battery
                    record(key, kind, "COMPILER_FAULT",
                           "%s: %s" % (type(e).__name__, e), nspec)
                    continue
                if (rtb, reb) != (tb, eb):
                    code = ("NULL_MARKER_ORDER"
                            if _without_nul_order(rtb, reb) ==
                            _without_nul_order(tb, eb) else "RENDER_DIFFERS")
                    record(key, kind, code, "compiled and regenerated; the "
                           "blocks differ from the page%s"
                           % (" only in the ORDER of the null markers, which "
                              "the generator always emits in Tibetan order"
                              if code == "NULL_MARKER_ORDER" else ""), nspec)
                    continue
                record(key, kind, "OK", "", nspec)
                res["segments_ok"] += 1
                res["spans_ok"] += nspec
    return res


NULMARK = re.compile(r'<span class="u" data-d="\d" data-l="[^"]*">'
                     r'<span class="nul">.*?</span></span>', re.S)


def _without_nul_order(tb, eb):
    """The blocks with their null markers pulled out and sorted."""
    marks = sorted(NULMARK.findall(eb))
    return tb, NULMARK.sub("", eb), marks


def check_tokens(text, side):
    """The tokenizer's contract, checked on real text rather than assumed.

    Tokens are in order, never overlap, never contain whitespace, cover every
    non-space character exactly once, and a boundary with no space at it
    never falls between two generator word characters."""
    toks = tokenize(text, side)
    cur = 0
    for a, b in toks:
        if a < cur or b <= a:
            return "token (%d,%d) out of order or empty" % (a, b)
        if text[cur:a].strip():
            return "characters %r between tokens are not covered" % text[cur:a]
        if any(c.isspace() for c in text[a:b]):
            return "token %r contains whitespace" % text[a:b]
        if a == cur and cur > 0 and _cut(text, a):
            return "adjacent tokens cut a word at %d (%r)" % (a, text[a - 2:a + 2])
        cur = b
    if text[cur:].strip():
        return "trailing characters %r are not covered" % text[cur:]
    return None


def load_baseline():
    if not os.path.exists(BASELINE):
        return None
    return json.load(io.open(BASELINE, encoding="utf-8"))


def _excused(blk):
    return (blk["kind"] in ("PHONETICS_PROTOCOL", "MARKER_PROTOCOL")
            and blk["class"] not in NEVER_EXCUSED)


def judge(res, base):
    """The gate's verdict. Returns a list of failure lines (empty = pass).

    NEW PAGES MUST ROUND-TRIP. A block that fails is excused only if (a) it
    carries one of the two hand-built protocol labels and fails with a
    structural class, or (b) it is named, with that exact class, in the
    frozen baseline of legacy blocks. The baseline may neither grow nor
    quietly shrink: a named block that starts passing is reported, and the
    baseline is re-derived with --write-baseline, never edited by hand. And
    the number of blocks that round-trip may not fall below the number the
    baseline measured -- a ratchet AT the measurement, never above it."""
    bad = []
    if base is None:
        return ["NO BASELINE at %s -- derive it with `battery --write-baseline`"
                % os.path.relpath(BASELINE, ROOT)]
    if base.get("tokenizer") != TOKENIZER:
        bad.append("the baseline was measured under %s, this is %s: re-derive "
                   "it" % (base.get("tokenizer"), TOKENIZER))
    if res["census_errors"]:
        bad.append("CENSUS: the page reader is blind to %d block(s): %s"
                   % (len(res["census_errors"]), res["census_errors"][0]))
    if res["tokenizer_errors"]:
        bad.append("TOKENIZER CONTRACT broken on %d side(s): %s"
                   % (len(res["tokenizer_errors"]), res["tokenizer_errors"][0]))
    parts = (res["wrappers"] + res["nul_markers"] + res["spec_spans_all"] +
             res["unreadable_tags"])
    if res["tags_in_pairs"] != parts:
        bad.append("CENSUS: %d span tags in pair blocks, but wrappers %d + "
                   "null markers %d + spec spans %d + unreadable %d = %d"
                   % (res["tags_in_pairs"], res["wrappers"], res["nul_markers"],
                      res["spec_spans_all"], res["unreadable_tags"], parts))
    if res["segments_ok"] == 0:
        bad.append("NOTHING ROUND-TRIPPED -- a battery that examines nothing "
                   "is not a battery")
    if res["segments_ok"] < base.get("segments_ok", 0):
        bad.append("ROUND TRIP REGRESSED: %d blocks round-trip, the baseline "
                   "measured %d" % (res["segments_ok"], base["segments_ok"]))
    frozen = base.get("segments", {})
    now = {k: b["class"] for k, b in res["blocks"].items()
           if b["class"] != "OK" and not _excused(b)}
    new = sorted(k for k in now if k not in frozen)
    moved = sorted(k for k in now if k in frozen and frozen[k] != now[k])
    healed = sorted(k for k in frozen if k not in now)
    for k in new[:25]:
        bad.append("NEW FAILURE %s  %s (%s block)"
                   % (k, now[k], res["blocks"][k]["kind"]))
    if len(new) > 25:
        bad.append("... and %d more new failures" % (len(new) - 25))
    for k in moved[:25]:
        bad.append("CLASS CHANGED %s  %s -> %s" % (k, frozen[k], now[k]))
    for k in healed[:25]:
        bad.append("NO LONGER FAILS AS FROZEN (re-derive the baseline with "
                   "--write-baseline) %s  was %s, now %s"
                   % (k, frozen[k], res["blocks"].get(k, {}).get("class",
                                                                 "ABSENT")))
    return bad


def report(res, out=None):
    out = out or sys.stdout
    w = out.write
    sides = res["sides"]
    expr = sides.get("plain", 0) + sides.get("bracket", 0)
    allsides = sum(sides.values())
    w("span-id compiler battery (%s), whole bank\n" % TOKENIZER)
    w("  pages %d, pair blocks %d, span tags on pages %d (%d inside pair "
      "blocks)\n" % (res["pages"], res["pairs"], res["tags_on_pages"],
                     res["tags_in_pairs"]))
    w("  inside pair blocks: %d sentence wrappers (generated, not spans of a "
      "spec) + %d null markers (the English echo of a null span) + %d spec "
      "span sides + %d tags in unreadable blocks\n"
      % (res["wrappers"], res["nul_markers"], res["spec_spans_all"],
         res["unreadable_tags"]))
    w("\n  TOKEN-SCHEME COVERAGE, per span side, on blocks whose text is the "
      "spine's: %d of %d expressible (%.3f%%)\n"
      % (expr, allsides, 100.0 * expr / max(1, allsides)))
    w("    plain token range %d, bracketed part %d\n"
      % (sides.get("plain", 0), sides.get("bracket", 0)))
    for code in sorted(k for k in sides if k not in ("plain", "bracket")):
        w("    NOT EXPRESSIBLE %-22s %6d  by depth %s\n      e.g. %s\n"
          % (code, sides[code], json.dumps(res["side_depths"][code],
                                           sort_keys=True),
             res["side_examples"][code][:170]))
    w("    span sides on blocks whose text is NOT the spine's, or unreadable "
      "(no spine position exists to name): %d + %d tags\n"
      % (res["sides_unaddressable"], res["unreadable_tags"]))
    kinds = {}
    for b in res["blocks"].values():
        k = kinds.setdefault(b["kind"], [0, 0])
        k[0] += 1
        k[1] += b["class"] == "OK"
    w("\n  ROUND TRIP (reverse-compile, compile, regenerate, compare bytes): "
      "%d blocks, %d spec span sides byte-identical (%.2f%% of spec span "
      "sides)\n" % (res["segments_ok"], res["spans_ok"],
                    100.0 * res["spans_ok"] / max(1, res["spec_spans_all"])))
    for k in sorted(kinds):
        w("    %-20s %5d blocks, %5d round-trip\n" % (k, kinds[k][0],
                                                     kinds[k][1]))
    if res["classes"]:
        w("  NOT round-tripped, by named class (blocks / spec span sides / "
          "block kinds):\n")
        for code, c in sorted(res["classes"].items(),
                              key=lambda kv: -kv[1]["spans"]):
            w("    %-32s %5d %7d  %s\n      e.g. %s\n"
              % (code, c["segments"], c["spans"],
                 json.dumps(c["kinds"], sort_keys=True), c["example"]))
    w("  census errors %d, tokenizer contract errors %d\n"
      % (len(res["census_errors"]), len(res["tokenizer_errors"])))


def write_baseline(res):
    frozen = {k: b["class"] for k, b in sorted(res["blocks"].items())
              if b["class"] != "OK" and not _excused(b)}
    data = {
        "_what": "Landed pair blocks the span-id compiler cannot reverse-compile "
                 "to a byte-identical page, frozen by name with their class, "
                 "plus the round-trip count measured in the same run. Written "
                 "by `tools/span_id_compiler.py battery --write-baseline`; "
                 "never edit by hand.",
        "_why": "New pages must round-trip. Legacy shapes (hand-built C01/C02 "
                "pages, early C03 pages) are named here so the gate holds "
                "them fixed: the set may neither grow nor quietly shrink. "
                "Blocks carrying a hand-built protocol label (phonetics, "
                "corpus marker) are excused by that label, not listed.",
        "tokenizer": TOKENIZER,
        "segments_ok": res["segments_ok"],
        "classes": {k: {"segments": v["segments"], "spans": v["spans"],
                        "kinds": v["kinds"]}
                    for k, v in sorted(res["classes"].items())},
        "segments": frozen,
    }
    io.open(BASELINE, "w", encoding="utf-8").write(
        json.dumps(data, indent=1, ensure_ascii=False, sort_keys=False) + "\n")


