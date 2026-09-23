#!/usr/bin/env python3
"""dump_spine_segments.py — hand the alignment workflow its source text once,
from one deterministic place, instead of making every agent fetch it.

WHY THIS EXISTS. A workflow script is plain JavaScript with no filesystem and
no database: the only thing it can receive is its `args` object. So the C05
alignment harness could not read the spine itself, and the brief told each of
the three agents per segment to run its own `sqlite3` query. A transport audit
of the campaign (2026-09-20) measured what that costs -- 2,644 sqlite
round-trips, 22% of all tool calls -- for text that is a few hundred bytes and
identical every time. THAT AUDIT WAS RUN OVER SESSION TRANSCRIPTS THAT ARE NOT
IN THIS REPOSITORY, so its figures cannot be re-derived here; they are quoted
as the reason this tool was written, never as a fact this tree can prove. What
this tree CAN prove is what the harness now sends, and
tools/workflow_prompt_size.py measures exactly that, on demand.

This tool produces that text once, as the exact `args` object the workflow is
launched with. One process, one query, no model in the loop between the
database and the JSON.

    tools/dump_spine_segments.py --course C05 --seqs 172,173,174
    tools/dump_spine_segments.py --course C05 --seqs 166-171 --pretty

TWO CHECKS, AND THEY PROVE DIFFERENT THINGS. Keep them apart; the first one
used to be described as if it were the second, which is the fault this
docstring was rewritten to close.

    tools/dump_spine_segments.py --verify C05 172 <sha> <sha> <sha>

  ARGS-BLOB STALENESS ONLY. Each field of a dump carries the first 12 hex of
  its sha256, and this compares the hashes AS THEY WERE PRINTED BY THIS TOOL
  against the spine as it stands right now. It catches a blob reused from an
  earlier run and a spine rebuilt underneath a queued batch. It says NOTHING
  about the copy of the text that reached a prompt, because the hashes travel
  BESIDE that copy and nothing binds one to the other: corrupt the text in the
  args, leave the hashes alone, and this prints "spine ok".

    tools/dump_spine_segments.py --verify-inlined C05 172 <tok> <tok> <tok>

  THE TEXT ITSELF. Each token is `<utf-8 byte length>:<FNV-1a 32 hex>` and is
  computed BY THE HARNESS FROM THE VERY STRING IT INLINED, in JavaScript, with
  no model and no second copy in between (c05-align.js, fnv1a/digestToken).
  This recomputes both from the spine row and compares. It therefore does
  compare the prompt's copy against the spine, which is what the pre-flight
  tells the agent it does. A 32-bit checksum beside an exact byte length
  catches corruption, mistranscription and staleness; it is not a signature
  and does not pretend to stop a forger.

WHAT NEITHER OF THEM IS. The backstop for a BANKED SPAN is downstream, not
here: tools/gen_alignment_page.py opens the spine itself and refuses any span,
Tibetan or English, that is not verbatim in the spine's own text. So a corrupt
inlined text cannot bank a false SPAN. It is not a total defence and must not
be described as one -- a segment's `note` and `title` are free prose that
nothing compares to the spine, so a false QUOTATION inside an AI NOTE can be
banked; and an alignment judged against the wrong sentence can produce spans
that are each present in the spine and still wrongly matched. The generator
sees presence, not correctness. --verify-inlined exists because that residue
is real, and it is what narrows it.

The verify is chained into the generator command the agent already runs, so it
costs no extra round-trip.

--precedent adds the other thing every agent used to fetch for itself. It folds
in build_precedent_index.precedent_for(), so one command produces the whole
args object — source text and precedent extract together — and the workflow
needs nothing else. The extract carries the bank sha256 and link count it was
built from, because a precedent total is exactly the kind of figure that goes
stale quietly and then justifies a wrong span.
"""
import argparse
import hashlib
import io
import json
import os
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
SHALEN = 12
FIELDS = ("wylie", "english", "acip")


def short_sha(text):
    """None and '' are distinct states and must not hash alike."""
    if text is None:
        return "-none-"
    return hashlib.sha256(text.encode("utf-8")).hexdigest()[:SHALEN]


def utf8(text, what):
    """Rule 3: a string that will not encode is refused by name, never guessed."""
    try:
        return text.encode("utf-8")
    except UnicodeEncodeError as e:
        sys.exit("REFUSED: the spine's %s field does not encode as UTF-8 (%s). "
                 "Nothing is checksummed on a guess." % (what, e))


def fnv1a32(data):
    """FNV-1a, 32-bit, over bytes. Kept to the shift-and-add decomposition of
    the prime (0x01000193 = 2^24+2^8+2^7+2^4+2^1+1) because the other half of
    this check runs in a workflow script, where there is no crypto module, no
    Buffer and no TextEncoder -- only operators. The two implementations must
    agree byte for byte, so this one is written the way that one has to be."""
    h = 0x811c9dc5
    for b in bytearray(data):
        h ^= b
        h = (h + ((h << 1) + (h << 4) + (h << 7) + (h << 8) + (h << 24))
             ) & 0xFFFFFFFF
    return "%08x" % h


def digest_token(text, what):
    """`<utf-8 byte length>:<fnv1a32>` -- the token the prompt carries.

    The length is there because a 32-bit checksum alone is thin: a corruption
    would have to preserve the exact byte count as well to slip through.
    """
    if text is None:
        return "-none-"
    raw = utf8(text, what)
    return "%d:%s" % (len(raw), fnv1a32(raw))


def connect(path):
    if not os.path.exists(path):
        sys.exit("spine not built: %s — run tools/build_spine.py first" % path)
    con = sqlite3.connect("file:%s?mode=ro" % path, uri=True)
    con.row_factory = sqlite3.Row
    return con


def parse_seqs(spec):
    """'166-171,175' -> [166,167,168,169,170,171,175], in the order given."""
    out = []
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" in part.lstrip("-"):
            lo, _, hi = part.partition("-")
            lo, hi = int(lo), int(hi)
            if hi < lo:
                sys.exit("range %s runs backwards" % part)
            out.extend(range(lo, hi + 1))
        else:
            out.append(int(part))
    seen, uniq = set(), []
    for s in out:
        if s not in seen:
            seen.add(s)
            uniq.append(s)
    return uniq


def fetch(con, course, seq):
    row = con.execute(
        "select wylie, english, acip from corpus_segments "
        "where course=? and seq=?", (course, seq)).fetchone()
    return row


def precedent_blobs(course, seqs, spine):
    """The per-segment precedent extracts, or a named reason there are none.

    Deliberately not fatal: the index is a separate artifact with its own
    build gate, and a batch that runs without it is a batch whose agents keep
    the brief's count-it-yourself instruction. That is worse but it is honest,
    and it is said out loud on stderr rather than discovered later.
    """
    try:
        sys.path.insert(0, os.path.join(ROOT, "tools"))
        import build_precedent_index as P
    except Exception as e:
        sys.stderr.write("precedent: tools/build_precedent_index.py did not "
                         "import (%s); no extracts inlined\n" % e)
        return {}
    try:
        raw = json.load(io.open(P.OUT, encoding="utf-8"))
    except Exception as e:
        sys.stderr.write("precedent: %s is not readable (%s); no extracts "
                         "inlined. Build it with tools/build_precedent_index.py\n"
                         % (os.path.relpath(P.OUT, ROOT), e))
        return {}
    index, meta = raw["index"], raw.get("meta", {})
    prov = ("%s, built from %s at %s links (bank sha256 %s). Anything you rely "
            "on, re-derive against the bank itself."
            % (os.path.relpath(P.OUT, ROOT), meta.get("built_from", "?"),
               meta.get("bank_links", "?"),
               str(meta.get("bank_sha256", "?"))[:12]))
    buckets = P.buckets_of(index)
    out = {}
    for seq in seqs:
        try:
            out[str(seq)] = {
                "text": P.render(P.extract(index, buckets, spine, course, seq)),
                "provenance": prov,
            }
        except Exception as e:
            sys.stderr.write("precedent: no extract for %s:%d (%s)\n"
                             % (course, seq, e))
    return out


def cmd_dump(a):
    con = connect(a.spine)
    seqs = parse_seqs(a.seqs)
    segments, missing = {}, []
    for seq in seqs:
        row = fetch(con, a.course, seq)
        if row is None:
            missing.append(seq)
            continue
        rec = {}
        for f in FIELDS:
            rec[f] = row[f]
        rec["sha"] = {f: short_sha(row[f]) for f in FIELDS}
        segments[str(seq)] = rec
    if missing:
        # Rule 3: a segment the spine does not have is reported, never
        # silently dropped from the batch the workflow thinks it covers.
        sys.stderr.write(
            "REFUSED: %s has no row in %s for seq %s\n"
            % (a.course, os.path.relpath(a.spine, ROOT),
               ", ".join(str(m) for m in missing)))
        return 1
    blob = {"course": a.course,
            "seqs": [s for s in seqs],
            "segments": segments}
    if a.precedent:
        pre = precedent_blobs(a.course, seqs, a.spine)
        if pre:
            blob["precedent"] = pre
        sys.stderr.write("precedent: %d of %d segments carry an extract\n"
                         % (len(pre), len(seqs)))
    if a.note:
        blob["note"] = a.note
    if a.scratch:
        blob["scratch"] = a.scratch
    sys.stdout.write(json.dumps(blob, ensure_ascii=False,
                                indent=1 if a.pretty else None,
                                sort_keys=False))
    sys.stdout.write("\n")
    return 0


def _compare(a, argv, live_of, what, ok_line):
    """The shared half of the two checks. They differ only in what is hashed
    and in what the failure is allowed to claim."""
    con = connect(a.spine)
    course, seq = argv[0], int(argv[1])
    claimed = dict(zip(FIELDS, argv[2:5]))
    row = fetch(con, course, seq)
    if row is None:
        print("SPINE MISMATCH: %s:%d has no row in the spine" % (course, seq))
        return 1
    bad = []
    for f in FIELDS:
        live = live_of(row[f], f)
        if live != claimed[f]:
            bad.append((f, claimed[f], live))
    if bad:
        print("SPINE MISMATCH at %s:%d — %s" % (course, seq, what))
        for f, want, live in bad:
            print("    %-8s prompt says %s, spine says %s" % (f, want, live))
        print("Do not align against the prompt copy. Re-fetch the row and say "
              "so in your report:")
        print('    sqlite3 %s "select wylie, english, acip from '
              'corpus_segments where course=\'%s\' and seq=%d"'
              % (os.path.relpath(a.spine, ROOT), course, seq))
        return 1
    print(ok_line % (course, seq))
    return 0


def cmd_verify(a):
    # The weaker of the two, and its message says so: these hashes were
    # printed by this tool beside the text, not derived from the copy that
    # reached the prompt.
    return _compare(
        a, a.verify, lambda v, f: short_sha(v),
        "the ARGS BLOB your prompt was built from was read from a DIFFERENT "
        "spine state than the one on disk now.",
        "args blob ok: %s:%d wylie/english/acip hashes match the spine "
        "(sha256-12). This checked the DUMP, not the copy in your prompt — "
        "use --verify-inlined for that.")


def cmd_verify_inlined(a):
    return _compare(
        a, a.verify_inlined, lambda v, f: digest_token(v, f),
        "THE TEXT INLINED IN YOUR PROMPT IS NOT WHAT THE SPINE HOLDS. The "
        "tokens were computed from that text itself, so this is the copy "
        "you are reading, not a hash that travelled beside it.",
        "spine ok: %s:%d — the wylie/english/acip inlined in your prompt are "
        "byte-identical to the spine row as it stands now (utf-8 length + "
        "FNV-1a 32; a checksum, not a signature)")


def main():
    p = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    p.add_argument("--spine", default=SPINE)
    p.add_argument("--course")
    p.add_argument("--seqs", help="e.g. 172,173,174 or 166-171")
    p.add_argument("--pretty", action="store_true")
    p.add_argument("--precedent", action="store_true",
                   help="also inline each segment's precedent extract "
                        "from data/alignment/precedent_index_v1.json")
    p.add_argument("--note", help="copied into args.note for the workflow")
    p.add_argument("--scratch", help="copied into args.scratch")
    p.add_argument("--verify", nargs=5,
                   metavar=("COURSE", "SEQ", "WYLIE_SHA", "ENGLISH_SHA",
                            "ACIP_SHA"),
                   help="compare the hashes THIS TOOL PRINTED against the "
                        "spine as it stands now (args-blob staleness only)")
    p.add_argument("--verify-inlined", nargs=5,
                   metavar=("COURSE", "SEQ", "WYLIE_TOK", "ENGLISH_TOK",
                            "ACIP_TOK"),
                   help="compare `<utf8 len>:<fnv1a32>` tokens computed from "
                        "the text actually inlined in a prompt against the "
                        "spine as it stands now")
    a = p.parse_args()
    if a.verify and a.verify_inlined:
        p.error("--verify and --verify-inlined check different things; "
                "run one or the other")
    if a.verify_inlined:
        return cmd_verify_inlined(a)
    if a.verify:
        return cmd_verify(a)
    if not (a.course and a.seqs):
        p.error("--course and --seqs are required (or use --verify)")
    return cmd_dump(a)


if __name__ == "__main__":
    sys.exit(main())
