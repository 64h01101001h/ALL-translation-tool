# The Kawachen Tibetan Reader — decoded audio scheme

*Source: `https://www.itibet.org/tibetan_reader/`, the Tibetan Reader built by
Kawachen (川喜多, Tohoku-affiliated). Adam asked for its audio on 2026-09-10,
stating he had previously been given permission to use it. This page records
what the site's file-naming scheme MEANS, decoded by driving its own state
machine rather than by guessing from filenames — so every recording we hold can
be labelled with the syllable it actually says.*

**Licensing: settled 2026-09-10.** The site footer reads
`Copyright © 2020 kawachen All rights Reserved` and carries no open licence.
Adam holds Kawachen's permission to use the recordings **in house**. That is
permission to use, not to redistribute, and this repository is public — so
the audio is a **gitignored local asset** (`data/audio/kawachen/`), harvested
by `tools/kawachen_harvest.py` and never committed. No ledger row is needed
because no file from this source is ever tracked.

## How the site works

One endpoint: `POST page/submit_top_page.php`, body `idd=<buttonId>&cookie=<sid>`.
The `tibetanreader` cookie is a session id; the syllable being built lives on
the server. Each response is almost all JavaScript — it names the mp3s to play
in a hidden `div.tr-audio-debug`, then enables and disables buttons to enforce
Tibetan spelling rules. The assembled Tibetan is composed client-side from the
buttons' own values, which is why a scraper must track the pressed sequence
itself to know what it is hearing.

### The buttons are the Tibetan syllable template

| prefix | n | slot | members |
|---|---|---|---|
| `m` | 30 | root consonant (མིང་གཞི) | ཀ ཁ ག ང … ཨ, in dictionary order |
| `h` | 3 | superscript (མགོ་ཅན) | ར ལ ས |
| `z` | 5 | prefix (སྔོན་འཇུག) | ག ད བ མ འ |
| `f` | 4 | subscript (འདོགས་ཅན) | ྱ ྲ ླ ྭ |
| `d` | 2 | double subscript | ྱྭ ྲྭ |
| `v` | 4 | vowel (དབྱངས) | ི ུ ེ ོ |
| `k` | 10 | suffix (རྗེས་འཇུག) | ག ང ད ན བ མ འ ར ལ ས |
| `s` | 2 | post-suffix (ཡང་འཇུག) | ད ས |
| `a` | 3 | ending | འི འུ འོ |
| `e` | 2 | punctuation | ་ ། |
| `r` | 1 | reset | — |

## The two audio directories

### `sound_file/syllable_mp3/` — the sounds

Names are `{consonant}_{vowel}_{suffix}[_{variant}].mp3`.

- **consonant** `1–54`. `1–30` are the root letters in dictionary order.
  `41+` are combinations that make a *different* sound (below).
- **vowel** `0–4`: `0` = the inherent a, then ི ུ ེ ོ.
- **suffix** `0–10`: `0` = none, then the ten suffixes in the order above
  (ག=1, ང=2, ད=3, ན=4, བ=5, མ=6, འ=7, ར=8, ལ=9).
- **variant** appears only where two spellings share a sound but were recorded
  separately — ས as a suffix gives `_3_2`, i.e. the ད-class final, take two.

A press of a letter plays that letter alone. Closing the syllable with tsheg
(`e1`) plays the **whole syllable**, which is where the suffix digit appears.

### `sound_file/word_mp3/` — the spoken spelling terms

The reader says the *name* of each component as it is added — `vowel_1.mp3`
for གི་གུ, `add_1.mp3` for the ya-tak, and so on. This is the half of the site
that teaches spelling aloud (སྦྱོར་ཀློག), and is the more pedagogically
valuable of the two.

## What the consonant index encodes: a phonetic equivalence map

The index is not orthographic — it is **how the stack is pronounced**. Letters
that sound alike share one index; letters that sound different get their own,
*including where the difference is only tone*. Decoded by driving the reader:

### 1–30 — the root letters, in dictionary order

### 31–40 — the superscript stacks, and they encode TONE

| index | spellings | why it is separate |
|---|---|---|
| 31 | རྒ ལྒ | "ga" — but high tone, so not index 3 |
| 32 | རྔ ལྔ སྔ | "nga" high |
| 33 | རྗ ལྗ | "ja" high |
| 34 | རྙ སྙ | "nya" high |
| 35 | རྡ ལྡ སྡ | "da" high |
| 36 | རྣ སྣ | "na" high |
| 37 | རྦ ལྦ སྦ | "ba" high |
| 38 | རྨ སྨ | "ma" high |
| 39 | རྫ | "dza" high |
| 40 | ལྷ | *lha* |

Exactly the nine voiced letters a superscript raises, plus ལྷ. Where a
superscript does *not* change the sound — རྐ is "ka" and so is ཀ — the site
returns the plain root index. That is a real phonological judgement, not a
lookup table.

### 41–54 — the subscript stacks

| index | spellings | reading |
|---|---|---|
| 41 / 42 / 43 / 44 | ཀྱ / ཁྱ / གྱ / ཧྱ | the *kya* series |
| 45 | ཀྲ, པྲ | *tra* |
| 46 | ཁྲ, ཐྲ, ཕྲ | |
| 47 | གྲ, དྲ | |
| 48 | ཧྲ | |
| 49 | ཀླ, གླ, བླ, རླ, སླ | all simply *la* |
| 51 | སྒྲ | |
| 53 / 54 | བྱ / བྲ | *ja* / *dra* |
| 5 / 6 / 8 | པྱ → ཅ, ཕྱ → ཆ, མྱ → ཉ | folded onto a root letter |

**50 and 52 are unidentified.** Both files exist and download, but no spelling
we drove produces them; they are labelled as unidentified in the manifest
rather than guessed at.

### Silent by design

ྭ (wa-zur) never changes the index. Prefixes and post-suffixes produce no
audio at all — ཀགས is served the same recording as ཀག. The site agrees with
our own `pron_engine.py` that these are written but not said.

## The spelling terms in `word_mp3/`

| family | n | what it is |
|---|---|---|
| `ta_1` … `ta_30` | 30 | the name of each root letter |
| `vowel_1` … `vowel_4` | 4 | གི་གུ, ཞབས་ཀྱུ, འགྲེང་བུ, ན་རོ |
| `add_1`, `add_2`, `add_4` | 3 | the subscripts, as spoken |
| `prefix_1` … `prefix_5` | 5 | the five prefixes |
| `zk_1`, `zk_2`, `zk_4` | 3 | the འི / འུ / འོ endings |

## Why this is worth more than the audio

The index map is an **independent encoding of the Lhasa reading rules** —
which stacks collapse, which raise the tone, which letters are silent — built
by people with no connection to the ACIP conventions our engines follow. We
have never had a second opinion on `pron_engine.py`'s stack handling. This is
one, and it is machine-checkable: run every one of the site's equivalence
classes through our engine and see where the two disagree. Those disagreements
are exactly the places worth a human ruling.

## What we hold, as of 2026-09-10

Harvested in full by `tools/kawachen_harvest.py` into `data/audio/kawachen/`
(gitignored), with `manifest.csv` beside it — one row per recording, carrying
the consonant/vowel/suffix indices, the Tibetan spelling(s) it serves, Wylie,
size and sha256.

| | |
|---|---|
| files | **3,002** |
| distinct recordings | **3,002** — no two files share a hash |
| total size | 22.3 MB |
| format | MP3, 44.1 kHz stereo, 0.37–0.65 s each |
| syllable recordings | 2,970 — the complete 54 × 5 × 11 grid, every name present |
| spelling-term recordings | 32 — letter names, vowel names, subscripts, prefixes, endings |
| labelled with what they say | 2,891 |
| unlabelled, honestly | 111 — consonant indices 50 and 52 across all vowels and suffixes, plus `add_5` |

The 111 are the ones no spelling we drove could reach. `add_5.mp3` is a
bonus: a fifth subscript-name recording that sits on their server but that
no button on their page triggers, so we have the audio without knowing which
term it speaks. They are kept, and the
manifest marks them `identified=False` rather than carrying a guess.

A manifest row for a merged class names every spelling it serves, e.g.

    49_2_3.mp3  ཀླུད / གླུད / བླུད / རླུད / སླུད   klud/glud/blud/rlud/slud

which is the honest rendering: one recording, five spellings, because in
speech they are one word.

## How the files are named

    syllable_mp3/49_2_3--klud-glud-blud-rlud-slud.mp3
    by-sound/klud--49_2_3.mp3 -> ../syllable_mp3/49_2_3--klud-…

**The site's own key comes first, verbatim.** It is the URL segment, so
re-verifying a recording against the source is a string identity rather than a
lookup; it is what `tools/kawachen_harvest.py` resumes on, so renaming the
store never causes a re-download; it sorts the folder into **Tibetan alphabet
order** rather than Latin; and it is the one part of the name that does not
change when our reading of a recording changes.

**Then every spelling the recording serves**, joined by `-`. The site indexes
by sound, so one file is legitimately five spellings; listing them all is the
honest rendering and makes `ls *klud*` find it.

### Why `-` and not `.`

The obvious separator is a dot. It is the wrong one, and our own engine says
so — `engines/ewts_unicode.py`, run on the candidates:

| joined name | parses to | |
|---|---|---|
| `klud.glud` | ཀླུདྒླུད | silently **wrong**, success=True |
| `klud+glud` | ལཀུདལགུད | silently **wrong** |
| `klud-glud` | ⟨klud-glud⟩ | **fails loudly**, in our own flag convention |

The dot is a live EWTS operator — `g.ya` is གཡ where `gya` is གྱ — so a
dot-joined filename reads as valid but incorrect Tibetan, with no flag raised.
`-` does not occur in EWTS at all, so it fails in exactly the way inviolable
rule 3 asks for. The same test picked the sentinel: `unknown` parses to
ཨུནྐྣོཝན, a manufactured Tibetan word for the very files whose defining
property is that we do not know what they say — so the sentinel is **`UNKNOWN`**,
which does not parse.

Spaces in the 13 term names become `_` (`prefix_1--ga_sngon_jug.mp3`), which
also fails EWTS loudly. Apostrophes are **kept**: `'` is correct Wylie for འ,
and `ka'` really is ཀའ. Nothing is made harder by that, because the ASCII-safe
key always leads the name — `ls 23_0_7--*` addresses any file without typing one.

### The second view

`by-sound/` holds one relative symlink per spelling, named sound-first:
**3,936 links over 2,891 recordings**. Files sort by Tibetan alphabet; links
sort by sound, so `ls by-sound/kla*` works with no CSV. A merged class is
legible either way — five links ending `--49_2_3.mp3` are plainly one
recording. Unidentified recordings get **no link at all**, because a link
named for a spelling would assert the guess we refuse to make.

Rebuild or audit both views with:

    python3 tools/kawachen_index.py --check      # nonzero on any drift
    python3 tools/kawachen_index.py --rebuild

## Where the audio may and may not go

Adam's permission covers **in-house use**. Two consequences follow, and the
second is the one worth watching:

- **The desktop tool: yes.** The audio sits in `data/audio/kawachen/`,
  gitignored, and is read locally. Nothing leaves the machine.
- **The iPhone app: not without asking Kawachen again.** TestFlight puts a
  build in the hands of the team *and of students*, which is distribution,
  not in-house use. Until Kawachen says otherwise, the drill pack should ship
  without these recordings. Flagging it here rather than discovering it after
  a build goes out.

## A defect in their own site, in our favour

The reader asks for a four-number filename when a syllable ends in ས —
`1_1_3_2.mp3` for ཀིས. **That file does not exist**; the server answers with a
redirect, so on their own site a syllable ending in ས plays nothing at all.
The recordings are there, at suffix slot `10`: `1_1_10.mp3` is a real,
distinct file. We fetch slot 10 directly and so recover audio their own
front end cannot reach.
