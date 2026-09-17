# Test fixtures the repository does not carry

**What this page is for.** 51 of the 115 registered ctest suites read
(re-counted 2026-09-11; this page said 37 of 73)
data that `.gitignore` keeps out of the repository. Before
2026-08-23 a clean checkout ran them anyway: most aborted with an
uncaught `std::runtime_error`, and three printed `[PASS]` over zero
rows — `shelf_suggest_smoke` announced *"the real shelf leads the list
… (measured 31%)"* after measuring nothing at all, because the number
was a string literal in the assertion label (SQA 2026-08-22, BUILD-7).

Two things changed:

* `cmake/AllFixtureTests.cmake` wraps `add_test()`. Every argument
  naming a path under the source tree is checked at configure time. A
  suite whose fixture is absent is registered as a **ctest SKIP** whose
  single line of output names the exact missing path and how to produce
  it. ctest reports it as `***Skipped` and lists it under *"The
  following tests did not run"*. It is never counted as a pass.
* The three suites that used to pass vacuously now **fail** when handed
  a path that is not there, and every measurement in an assertion label
  is the number that run produced. `tools/no_vacuous_pass_check.py`
  (suite `no_vacuous_pass`) holds that.

## Which battery to run

| command | suites | who runs it |
|---|---|---|
| `ctest -LE fixture` | 36 | a clean checkout, and CI |
| `ctest -L fixture` | 37 | the fixture-dependent half |
| `ctest` | 73 | the press — and `tools/package_macos.sh` **refuses to ship if any suite skipped** |

The configure step prints the split, so you never have to guess:

    -- ALL battery: 36/73 suites can run here; 37 will SKIP for want of
       untracked fixtures — spine_smoke, analysis_smoke, …

## The inventory

Roots `.gitignore` excludes: `build/`, `library/`, `data/extracted/`,
`data/das/`, `data/teaching/`.

Several generators read the **external HGM data project**
(`~/Desktop/HGM DICTIONARY - TRANSLATION APP`) rather than anything in
this repo — see `CLAUDE.md` § REQUIRED ACCESS. Run them from the repo
root.

### `build/` — generated databases and engine-oracle references

| fixture | suites | how to produce it |
|---|---|---|
| `build/hgm_spine_v27_2.db` | analysis, botok_tok, colloquial, contractions, drills, engines_battery, gofer, lattice, poslex, qc, quotation, reader, spellcheck, spine, spine_resilience, terminology | `python3 tools/build_spine.py` |
| `build/sanskrit_reference.tsv` | engines_battery | `python3 tools/build_sanskrit_reference.py` |
| `build/pron_reference.tsv` | engines_battery | `python3 tools/build_pron_reference.py` — the canonical `engines/pron_engine.py` output for all 105,576 distinct wylie headwords in the master. Landed 2026-09-16 from a worktree it had been stranded in since 2026-09-10; verified by regenerating over the dump the battery had been using and diffing: identical, byte for byte |
| `build/forward_reference.tsv` | forward_battery | `python3 tools/build_forward_reference.py` — the canonical `engines/ewts_unicode.py` output over every third master ENTRY at offset 2, wylie sorted, duplicates kept (35,211 rows for 35,207 distinct headwords). Landed 2026-09-16; verified byte for byte against the banked dump |
| `build/towylie_reference.tsv` | towylie_battery | **no generator in this repo** (emitted by the pyewts oracle run) |
| `build/weird_top.tsv` | weird_battery | `python3 tools/weirdness_rank.py` |
| `build/botok_syls_reference.tsv` | botok_smoke | `python3 tools/build_botok_reference.py build/hgm_spine_v27_2.db build/botok_syls_reference.tsv` |
| `build/botok_tok_scenarios.txt`, `build/botok_tok_corpus.tsv` | botok_tok_smoke | `python3 tools/build_botok_tok_reference.py build/botok_tok_scenarios.txt build/botok_tok_corpus.tsv` |
| `build/speak_reference.tsv` | speak_battery | `python3 tools/dump_speak_reference.py` — the Python resolver's answer for all 11,940 distinct syllables in the corpus and dictionary; the C++ port in `allcore::SpeakBank` must reproduce it exactly. Needs the Kawachen audio present (in-house asset, gitignored), and the suite SKIPs without it |
| `build/ocr_ref` | ocr_smoke | `python3 tools/build_ocr_reference.py <path-to-tibetan-ocr-app-clone>` |

The remaining **no generator in this repo** rows are the honest state of
BUILD-7: those oracle dumps exist on one laptop and nothing here can
rebuild them. Committing their generators (or the fixtures themselves,
strided down to a few thousand rows) is the remaining work.

`build/pron_reference.tsv` came off that list on 2026-09-16. Its
generator had been WRITTEN on 2026-09-10, specifically to close this gap
— its own docstring says so — and then never landed in the main tree; it
sat in a worktree, and `git log --all` had no commit for it anywhere. So
the gate that proves Rule 2 for the pronunciation engine depended on a
file no clone could produce. It fails loudly rather than skipping when
the file is absent, which is the right behaviour and is why this was a
reproducibility hole rather than a silent pass.

Regenerating it also settled a question worth asking of any fixture: the
reference could have been a snapshot of the C++ port's OWN output, which
would make the battery circular and worthless. It is not. The canonical
Python reproduces it byte for byte across all 105,576 rows, and
`build/sanskrit_reference.tsv` likewise across 79,452 terms.

### `data/extracted/` — derived extracts

| fixture | suites | how to produce it |
|---|---|---|
| `data/extracted/mahavyutpatti.tsv` | mvp_smoke | `python3 tools/extract_mahavyutpatti.py` |
| `data/extracted/bsdus_tshig.tsv` | contractions_smoke | `python3 tools/derive_bsdus_tshig.py` |
| `data/extracted/verb_stems.tsv` | verbstems_smoke | `python3 tools/import_verbs_db.py` |
| `data/extracted/verb_lemmas.tsv` | searchnorm_smoke | `python3 tools/gen_lemma_table.py` |
| `data/extracted/work_subjects.tsv` | subjects_smoke | `python3 tools/build_subject_index.py` |
| `data/extracted/apparatus_bibliography.json`, `data/extracted/mixed_nuts_bibliography.json` | catalog_audit_smoke | `python3 tools/extract_apparatus.py` |
| `data/extracted/monlam_lexicon_1.txt`, `data/extracted/monlam_lexicon_2.txt` | lexicon_smoke | **no generator in this repo** — licensed source text, obtained from the data project |

### `library/` — the translator's own text collections

Never in git: these are the user's materials, and several carry
third-party licensing (see `docs/distribution/OPEN_SOURCE_NOTICES.md`).

| fixture | suites | how to produce it |
|---|---|---|
| `library` | catalog_audit, catalog_id, catalog_name, cleanup, register, title_xlat, volsplit | install a text collection under `library/` (the DMG ships one as *Diamond Cutter Tool Data/library*) |
| `library/kangyur` | catalog_list, tree_diff | as above |
| `library/sungbum` | catalog_qc, shelf_suggest | as above |
| `library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx`, `library/ocr_models/BDRC_Woodblock` | ocr_smoke | BDRC models (CC BY-NC 4.0): the Scan pane's in-app download, or a team drop |

## Adding a suite that needs a fixture

Nothing to do — `add_test()` already checks it. Pass the fixture as a
`${CMAKE_SOURCE_DIR}/…` argument (never open it by a path baked into
the C++), then **add its row to the table above**: the `fixture_gate`
suite fails if a fixture path the battery references is missing from
this page.

Inside the test binary, an absent fixture is a **failure**, not a
`[SKIP]` that returns 0. Whether the suite runs at all is a
configure-time decision; by the time the binary starts, the data is
supposed to be there.

---

## Which of these are actually committed — checked 2026-09-17

The roots above say `.gitignore` excludes `build/`, `library/` and
`data/extracted/`, and a reader takes from that that the fixtures listed here
will be absent from a fresh clone. **Eight of the 23 paths named in this file
are committed anyway**, forced in past the ignore rules:

`build/forward_reference.tsv` · `build/towylie_reference.tsv` ·
`build/weird_top.tsv` · `data/das/` · `data/extracted/` ·
`data/extracted/apparatus_bibliography.json` ·
`data/extracted/work_subjects.tsv` · `data/teaching/`

So the batteries that consume them RUN on a fresh clone rather than skipping,
which is the opposite of what this page implied. Re-derive the list with:

    git ls-files --error-unmatch <path>

### `towylie_reference.tsv`: not circular as far as can be shown, and frozen

Its own header records the provenance — *"pyewts.toWylie reference fixtures
(OpenPecha/pyewts master, Apache-2.0)"*, over the master's tibetan column plus
a strided sample of 84000 TM native unicode — so the expected values come from
a third-party implementation and not from our port.

**But it matches 109,490 of 109,490 rows, 100.000%**, and that is the exact
shape a fixture snapshotted from the port under test would have. The two
cannot be told apart from inside this repository, because pyewts is not
installed here and regenerating is the only test that would separate them.

What can be said: the round-trip half of the same battery reads 4,875/5,000,
so the battery is not passing everything put in front of it; and the fixture's
attribution is recorded in the file rather than asserted here. What cannot be
said is that the 100% has been independently reproduced. Anyone who installs
pyewts should regenerate and diff — that single act settles it.

This is BUILD-7 for this fixture: no generator in the repo, so it is frozen at
whatever it was, and the only proof for that engine rests on it.
