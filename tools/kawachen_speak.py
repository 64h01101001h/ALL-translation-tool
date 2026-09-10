#!/usr/bin/env python3
"""Resolve Tibetan syllables to Kawachen recordings, with an honest tier per hit.

This is the prototype of the read-aloud path. It answers one question — "which
recording says this syllable?" — and it answers it in tiers, because some
answers are exact and some are approximations, and the difference must reach
the user rather than be smoothed away.

    TIER 1  exact           the bank has this syllable's reading
    TIER 2  silent-dropped  a post-suffix or wa-zur was removed first. Still
                            exact: Tibetan does not pronounce either, and our
                            own engine agrees with Kawachen's index that it
                            does not.
    TIER 3  base+ending     a genitive contraction played as two recordings,
                            e.g. pa'i as `pa` then `'i`
    TIER 4  reduced         a subscript was dropped to find a match. This is an
                            APPROXIMATION and must be labelled one.
    none    no recording    flagged, never approximated further.

Both sides are read through engines/pron_engine.py, the project's authority on
how a syllable sounds, so this module never invents a second phonology.
"""
import csv
import io
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "engines"))
from pron_engine import pron_syllable, parse_syl              # noqa: E402

MANIFEST = os.path.join(HERE, "..", "data", "audio", "kawachen", "manifest.csv")

TIER_EXACT, TIER_SILENT, TIER_ENDING, TIER_REDUCED = 1, 2, 3, 4
TIER_NAME = {TIER_EXACT: "exact", TIER_SILENT: "exact (silent letter dropped)",
             TIER_ENDING: "two recordings", TIER_REDUCED: "APPROXIMATE (reduced)"}

GENITIVE = re.compile(r"^(.+?)('[iou])$")
POSTSUFFIX = re.compile(r"^(.*[aeiou][gnbmr]?[gnbmrl])([ds])$")
# wa-zur is SUBJOINED, so it can never open an onset: there must be a
# consonant before it. Without that guard a stray Latin token like
# "va" or "valid" — the corpus does contain some — gets its leading v
# stripped and is then read aloud as Tibetan, which is a guess.
WAZUR = re.compile(r"^([^aeiou]+?)[vw]([^aeiou]*[aeiou].*)$")


class Bank(object):
    """Every reading the recordings can say, and which file says it."""

    def __init__(self, manifest=MANIFEST):
        self.syllables, self.endings, self.terms = {}, {}, {}
        for r in csv.DictReader(io.open(manifest, encoding="utf-8")):
            if r["identified"] != "True":
                continue
            if r["dir"] == "syllable_mp3":
                # Keyed on the SPELLING, never on the engine's reading. The
                # reading is the printed-phonetics convention, which flattens
                # aspiration and voicing on purpose — ka and kha both read
                # "ka", sa and za both read "sa". The recordings do not: they
                # are different sounds, recorded separately. Keying audio on
                # the reading made 71.4% of spellings play another syllable's
                # recording and left 1,816 of 2,860 recordings unreachable.
                for spelling in r["wylie"].split("/"):
                    spelling = spelling.strip()
                    if spelling:
                        self.syllables.setdefault(spelling, r["file"])
            elif r["kind"] == "ending name":
                self.endings[r["wylie"]] = r["file"]
            else:
                self.terms[r["wylie"]] = r["file"]

    PREFIX = "gdbm'"
    SUPERSCRIPT = "rls"

    def _silent_candidates(self, syl):
        """The syllable with a silent leading letter removed.

        A prefix is never pronounced. A superscript over an UNVOICED root is
        not pronounced either and does not change the sound — Kawachen's own
        reader serves rka from the ka recording. Over a voiced root it does
        change it, but those stacks (rga, rnga, rda ...) carry their own
        spellings in the bank, so they match exactly and never reach here.
        Each candidate is only accepted if what remains is itself a recorded
        syllable, so a root consonant is never mistaken for a silent one:
        'ga' keeps its g because 'a' would have to be found instead.
        """
        if len(syl) > 2 and syl[0] in self.PREFIX:
            yield syl[1:]
        if len(syl) > 2 and syl[0] in self.SUPERSCRIPT:
            yield syl[1:]
        # both at once: bstan -> stan -> tan
        if len(syl) > 3 and syl[0] in self.PREFIX and syl[1] in self.SUPERSCRIPT:
            yield syl[2:]

    def _hit(self, syl):
        return self.syllables.get(syl)

    def resolve(self, syl):
        """-> (list of files to play, tier) or (None, None) if we have nothing."""
        f = self._hit(syl)
        if f:
            return [f], TIER_EXACT

        # A post-suffix, a wa-zur and a PREFIX are all written but never
        # said. Kawachen's own reader agrees on the prefix: pressing one
        # produces no audio and does not change which recording is served.
        # A prefix is only dropped when what remains is itself a recorded
        # syllable, so a root consonant is never mistaken for one — 'ga' keeps
        # its g, because 'a' alone would have to be found in the bank instead.
        probe = syl
        m = POSTSUFFIX.match(probe)
        if m:
            probe = m.group(1)
        m = WAZUR.match(probe)
        if m:
            probe = m.group(1) + m.group(2)
        if probe != syl:
            f = self._hit(probe)
            if f:
                return [f], TIER_SILENT
        for stripped in self._silent_candidates(probe):
            f = self._hit(stripped)
            if f:
                return [f], TIER_SILENT

        # A genitive contraction is the base syllable plus the ending.
        m = GENITIVE.match(syl)
        if m and m.group(2) in self.endings:
            base = m.group(1)
            if parse_syl(base):
                f = self._hit(base)
                if f:
                    return [f, self.endings[m.group(2)]], TIER_ENDING

        # Last resort: drop a subscript. This changes the sound, so it is
        # labelled an approximation wherever it is used.
        p = parse_syl(probe)
        if p:
            onset, vowel, coda = p
            for sub in ("y", "r", "l"):
                if len(onset) > 1 and onset.endswith(sub):
                    f = self._hit(onset[:-1] + vowel + coda)
                    if f:
                        return [f], TIER_REDUCED
        return None, None


def read_aloud(wylie, bank=None):
    """A whole passage -> the play list, with a tier on every syllable."""
    bank = bank or Bank()
    out = []
    for token in re.split(r"([\s/|]+)", wylie or ""):
        if not token.strip():
            continue
        syl = token.strip(".,;:!?()[]")
        if not syl or not parse_syl(syl):
            out.append({"syllable": syl, "files": [], "tier": None,
                        "note": "not a readable Tibetan syllable"})
            continue
        files, tier = bank.resolve(syl)
        out.append({"syllable": syl, "files": files or [], "tier": tier,
                    "note": TIER_NAME.get(tier, "no recording")})
    return out


if __name__ == "__main__":
    b = Bank()
    print("bank: %d readings, %d endings, %d terms"
          % (len(b.syllables), len(b.endings), len(b.terms)))
    demo = sys.argv[1] if len(sys.argv) > 1 else \
        "thams cad mkhyen pa'i thabs skabs stobs bsgrubs dvags smra"
    for row in read_aloud(demo, b):
        print("  %-10s %-34s %s" % (row["syllable"],
                                    ",".join(os.path.basename(f) for f in row["files"]) or "-",
                                    row["note"]))
