#!/usr/bin/env python3
"""Regenerate core/src/unicode_wylie_tables.inc from pyewts itself.

The tables ARE the canonical module's runtime maps (OpenPecha/pyewts
master, Apache-2.0) - generating them from the imported module is the
strongest form of "port from the file, never from spec" (rule 2).

Usage: python3 tools/gen_unicode_wylie_tables.py <dir-containing-pyewts.py>
"""
import sys

sys.path.insert(0, sys.argv[1])
import pyewts

p = pyewts.pyewts()

def cesc(s):
    out = ""
    for ch in s:
        o = ord(ch)
        if ch == "\\": out += "\\\\"
        elif ch == '"': out += '\\"'
        elif 32 <= o < 127: out += ch
        else:
            out += "".join('\\x%02x""' % b for b in ch.encode("utf-8"))
    return '"' + out + '"'

lines = [
    "// AUTO-GENERATED from pyewts (OpenPecha/pyewts master, Apache-2.0)",
    "// by tools/gen_unicode_wylie_tables.py - the tables ARE the",
    "// canonical module's own runtime maps; do not edit by hand.",
]

def dump_map(name, d):
    lines.append(f"static const std::unordered_map<std::string, std::string> {name} = {{")
    for k, v in d.items():
        lines.append(f"    {{{cesc(k)}, {cesc(v)}}},")
    lines.append("};")

def dump_set(name, s):
    lines.append(f"static const std::unordered_set<std::string> {name} = {{")
    for k in sorted(s):
        lines.append(f"    {cesc(k)},")
    lines.append("};")

def dump_pairset(name, d):
    lines.append(f"static const std::unordered_set<std::string> {name} = {{")
    for k, vs in d.items():
        for v in sorted(vs):
            lines.append(f"    {cesc(k + '|' + v)},")
    lines.append("};")

dump_map("kTibTop", p.m_tib_top)
dump_map("kTibSubjoined", p.m_tib_subjoined)
dump_map("kTibVowel", p.m_tib_vowel)
dump_map("kTibVowelLong", p.m_tib_vowel_long)
dump_map("kTibFinalWylie", p.m_tib_final_wylie)
dump_map("kTibFinalClass", p.m_tib_final_class)
dump_map("kTibCaret", p.m_tib_caret)
dump_map("kTibOther", p.m_tib_other)
dump_set("kTibStacks", p.m_tib_stacks)
dump_pairset("kPrefixPairs", p.m_prefixes)
dump_set("kSuffixes", set(p.m_suffixes))
dump_pairset("kSuff2Pairs", p.m_suff2)
lines.append("static const std::unordered_map<std::string, int> kAmbiguousKey = {")
for k, v in p.m_ambiguous_key.items():
    lines.append(f"    {{{cesc(k)}, {v}}},")
lines.append("};")

open("core/src/unicode_wylie_tables.inc", "w").write("\n".join(lines) + "\n")
print("written")
