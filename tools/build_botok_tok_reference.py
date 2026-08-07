#!/usr/bin/env python3
"""Generate the Botok TOKENIZER port's battery references from the live
Python botok oracle (harness-only; pip3 install --user botok).

    python3 tools/build_botok_tok_reference.py \
        build/botok_tok_scenarios.txt build/botok_tok_corpus.tsv

Outputs:
  scenarios file — small tries driven through the upstream tests' exact
    operations (#SC/#OP/#IN/#TK lines; ops replayed verbatim by the C++
    battery, tokens serialized field-for-field);
  corpus file — trie from every distinct spine Tibetan string (ORDER BY
    tibetan, so trie-data overwrites happen in the same order on both
    sides), inputs = corpus segments converted by the CANONICAL
    engines/ewts_unicode.py, one line per segment:
    <input>\\t<token>\\x1e<token>...

Rerun after every spine rebuild, together with build_botok_reference.py.
"""
import os
import sqlite3
import sys
import tempfile

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(REPO, "engines"))

from botok import BoSyl, TokChunks, Tokenize, Trie  # noqa: E402
from ewts_unicode import wylie_to_unicode  # noqa: E402

US = "\x1f"
RS = "\x1e"


def ser_token(t):
    parts = [f"text={t.text}", f"type={t.chunk_type}", f"start={t.start}",
             f"len={t.len}"]
    if t.syls_idx is not None:
        parts.append("syls=" + ";".join(
            ",".join(str(i) for i in syl) for syl in t.syls_idx))
        parts.append("sse=" + ";".join(
            f"{d['start']}-{d['end']}" for d in t.syls_start_end))
    else:
        parts.append("syls=-")
        parts.append("sse=-")
    parts.append("chars=" + ",".join(t.char_types))
    if t.senses is not None:
        parts.append("senses=" + ";".join(
            ",".join(f"{k}:{m[k]}" for k in sorted(m.keys()))
            for m in t.senses))
    else:
        parts.append("senses=-")
    if t.affixation:
        parts.append(f"affix=len:{t.affixation['len']},"
                     f"type:{t.affixation['type']},aa:{t.affixation['aa']}")
    else:
        parts.append("affix=-")
    parts.append(f"skrt={t.skrt}")
    parts.append(f"formfreq={t.form_freq if t.form_freq is not None else '-'}")
    return US.join(parts)


def fresh_tok(scratch):
    # empty main/custom data: the trie is driven entirely through the ops
    return Tokenize(Trie(BoSyl, "empty", {}, {}, pickle_path=scratch))


def run_input(tok, text):
    pre = TokChunks(text)
    pre.serve_syls_to_trie()
    return tok.tokenize(pre)


SCENARIOS = [
    ("upstream-affix", [
        ("mod", "བཀྲ་ཤིས་"),
        ("data", "བཀྲ་ཤིས་\tNOUN\t\tབཀྲ་ཤིས་\t17500"),
        ("mod", "མཐའ་"),
        ("data", "མཐའ་\tNOUN"),
    ], ["མཐའི་བཀྲ་ཤིས། ཀཀ abc མཐའི་རྒྱ་མཚོ་"]),
    ("upstream-non-max2", [
        ("mod", "བཀྲ་ཤིས་"),
        ("data", "བཀྲ་ཤིས་\tNOUN"),
        ("mod", "བཀྲ་ཤིས་བདེ་ལེགས།"),
    ], ["བཀྲ་ཤིས་བདེ་བཀྲ་", "བཀྲ་ཤིས་བདེ་བཀྲ་"]),  # twice: injection persistence
    ("upstream-non-max-end", [
        ("mod", "བཀྲ་ཤིས་"),
        ("mod", "བཀྲ་ཤིས་བདེ་ལེགས།"),
    ], ["བཀྲ་ཤིས་བདེ་"]),
    ("deactivate", [
        ("mod", "བཀྲ་ཤིས་"),
        ("mod_deactivate", "བཀྲ་ཤིས་"),
    ], ["བཀྲ་ཤིས་བདེ།"]),
    ("skrt-word", [
        ("mod_skrt", "པདྨ་"),
    ], ["པདྨ་དཀར་པོ།", "སརྦ་མངྒ་ལཾ།"]),
    ("bare-form-data", [
        ("mod", "མཐའ་"),
        ("data", "མཐའ་"),
    ], ["མཐའ་"]),
    ("non-inflectible", [
        ("add_non_inflectible", "བདེ་ལེགས་"),
    ], ["བདེ་ལེགས་སུ།", "བདེ་ལེགས།"]),
]


def main() -> int:
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    scen_out, corpus_out = sys.argv[1], sys.argv[2]
    limit = int(sys.argv[3]) if len(sys.argv) > 3 else 0

    with tempfile.TemporaryDirectory() as scratch:
        with open(scen_out, "w", encoding="utf-8") as f:
            for name, ops, inputs in SCENARIOS:
                f.write(f"#SC\t{name}\n")
                tok = fresh_tok(scratch)
                for op, arg in ops:
                    f.write(f"#OP {op}\t{arg}\n")
                    if op == "mod":
                        tok.trie.inflect_n_modify_trie(arg)
                    elif op == "mod_deactivate":
                        tok.trie.inflect_n_modify_trie(arg, deactivate=True)
                    elif op == "mod_skrt":
                        tok.trie.inflect_n_modify_trie(arg, skrt=True)
                    elif op == "data":
                        tok.trie.inflect_n_add_data(arg)
                    elif op == "add_non_inflectible":
                        tok.trie.add_non_inflectible(arg)
                for text in inputs:
                    f.write(f"#IN\t{text}\n")
                    for t in run_input(tok, text):
                        f.write(f"#TK\t{ser_token(t)}\n")
        print(f"scenarios -> {scen_out}")

        con = sqlite3.connect(os.path.join(REPO, "build/hgm_spine_v27_2.db"))
        words = [w for (w,) in con.execute(
            "SELECT DISTINCT tibetan FROM entries "
            "WHERE tibetan IS NOT NULL AND tibetan != '' ORDER BY tibetan")]
        tok = fresh_tok(scratch)
        print(f"building oracle trie from {len(words)} spine words ...")
        for w in words:
            tok.trie.inflect_n_modify_trie(w)

        rows = con.execute(
            "SELECT wylie FROM corpus_segments "
            "WHERE wylie IS NOT NULL AND wylie != '' ORDER BY course, seq"
        ).fetchall()
        if limit:
            rows = rows[:limit]
        n = skipped = unconvertible = 0
        with open(corpus_out, "w", encoding="utf-8") as f:
            for (wylie,) in rows:
                try:
                    uni = wylie_to_unicode(wylie)
                except Exception:
                    # the canonical engine raises on onsets it refuses
                    # (e.g. stray capitals in corpus wylie) — counted, never
                    # silently approximated
                    unconvertible += 1
                    continue
                if isinstance(uni, tuple):
                    uni = uni[0]
                if any(c in uni for c in "\t\n\x1e\x1f"):
                    skipped += 1
                    continue
                toks = run_input(tok, uni)
                f.write(uni + "\t" + RS.join(ser_token(t) for t in toks) + "\n")
                n += 1
                if n % 5000 == 0:
                    print(f"  {n} segments ...", flush=True)
        print(f"corpus -> {corpus_out}: {n} rows "
              f"({unconvertible} unconvertible by the canonical engine, "
              f"{skipped} control-char skips)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
