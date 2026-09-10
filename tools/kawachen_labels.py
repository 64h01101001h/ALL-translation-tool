#!/usr/bin/env python3
"""What each Kawachen recording says, and what its file is called.

One module owns both, because the labels used to live in three places and had
already drifted apart once.  The tables are data (data/kawachen_labels.json),
decoded by driving the reader's own state machine — see
docs/KAWACHEN_READER_FORMAT.md.

THE FILENAME RULE

    {site_key}--{spellings}.mp3        e.g. 49_2_3--klud-glud-blud-rlud-slud.mp3

`site_key` is the source site's own name for the file, byte for byte, and comes
first.  It is the URL segment, so re-verification is a string identity rather
than a lookup; it is the harvester's resume key; it sorts the folder into
Tibetan alphabet order rather than Latin; and it is the one part of the name
that does not change when our reading of a recording changes.

`spellings` is every Wylie spelling the site serves from that recording, joined
by `-`.  The site indexes by SOUND, so one file can legitimately be five
spellings; listing them all is the honest rendering, and it means a plain
`ls *klud*` finds the file.

WHY `-` AND `--`, AND NOT `.`

Checked against our own engines/ewts_unicode.py, which is the authority:

    wylie_to_unicode('klud.glud')  -> ('ཀླུདྒླུད', True)   silently WRONG
    wylie_to_unicode('klud+glud')  -> ('ལཀུདལགུད', True)   silently WRONG
    wylie_to_unicode('klud-glud')  -> ('⟨klud-glud⟩', False)  fails loudly

The dot is a live EWTS operator — `g.ya` is གཡ where `gya` is གྱ — so a
dot-joined name parses as valid but incorrect Tibetan, with success=True and no
flag.  `-` is absent from EWTS and fails in the project's own ⟨wylie⟩ flag
convention, which is what inviolable rule 3 asks for.  The same test rules out
the obvious sentinel: `unknown` parses to ཨུནྐྣོཝན, so the sentinel is
uppercase `UNKNOWN`, which does not parse.

Spaces (13 term names such as `ga sngon jug`) become `_`, which also fails EWTS
loudly.  Apostrophes are KEPT: `'` is correct Wylie for འ, and `ka'` really is
ཀའ.  A file is always addressable without typing one, because the ASCII-safe
site key leads the name — `ls 23_0_7--*`.
"""
import io
import json
import os
import re

ROOT_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LABELS_PATH = os.path.join(ROOT_DIR, "data", "kawachen_labels.json")

UNKNOWN = "UNKNOWN"
SEP = "--"          # between the site key and the spellings
JOIN = "-"          # between co-spellings of one recording

_d = json.load(io.open(LABELS_PATH, encoding="utf-8"))

ROOT = [r["tibetan"] for r in _d["roots"]]
STEM = [r["stem"] for r in _d["roots"]]
VOWEL = {int(k): (v["tibetan"], v["wylie"]) for k, v in _d["vowels"].items()}
SUFFIX = {int(k): (v["tibetan"], v["wylie"]) for k, v in _d["suffixes"].items()}
STACKS = {int(k): (v["spellings"], v["stem"]) for k, v in _d["stacks"].items()}
TERMS = {k: (v["kind"], v["tibetan"], v["wylie"]) for k, v in _d["terms"].items()}

SITE_KEY_RE = re.compile(r"^(?:\d+_\d+_\d+|[a-z]+_\d+)$")


def spellings_for(consonant_index):
    """The Tibetan spellings a consonant index serves, and its Wylie stem."""
    c = consonant_index
    if 1 <= c <= 30:
        return [ROOT[c - 1]], STEM[c - 1]
    return STACKS.get(c, ([], ""))


def slug(wylie):
    """The descriptive half of a filename, from a manifest `wylie` field.

    Returns UNKNOWN for an empty label rather than a bare or invented one: a
    missing label must look different from a file the renamer never reached.
    """
    w = (wylie or "").strip()
    if not w:
        return UNKNOWN
    parts = [p.strip().replace(" ", "_") for p in w.split("/") if p.strip()]
    if not parts:
        return UNKNOWN
    return JOIN.join(parts)


def descriptive_name(site_key, wylie):
    """`49_2_3` + `klud/glud/...` -> `49_2_3--klud-glud-blud-rlud-slud.mp3`."""
    return "%s%s%s.mp3" % (site_key, SEP, slug(wylie))


def site_key_of(filename):
    """The source site's name for a file, whether or not it has been renamed.

    `49_2_3--klud-glud.mp3` -> `49_2_3`;  `49_2_3.mp3` -> `49_2_3`.
    Returns None if the name is not one of ours, so a stray file in the
    directory is skipped rather than silently mis-parsed.
    """
    base = filename[:-4] if filename.endswith(".mp3") else filename
    key = base.split(SEP, 1)[0]
    return key if SITE_KEY_RE.match(key) else None


def source_url(sub, site_key):
    """Where a recording came from — the point of keeping the key verbatim."""
    return ("https://www.itibet.org/tibetan_reader/sound_file/%s/%s.mp3"
            % (sub, site_key))
