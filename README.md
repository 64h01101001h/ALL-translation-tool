# ALL Translation Tool

A macOS desktop suite for Tibetan digital texts and translation, built on the
HGM Dictionary releases. C++20 core (`allcore`) + Qt 6 UI. **Everything except
the AI Analysis sections works fully offline.**

## Launch

Double-click `cmake-build/app/ALLTranslationTool.app`, or from a terminal:

    ./cmake-build/app/ALLTranslationTool.app/Contents/MacOS/ALLTranslationTool

The app finds its data automatically (spine, template, spellcheck, reference
layers). For the **Analysis** tab you need the Claude API key in the
environment, so launch from a terminal where `ANTHROPIC_API_KEY` is exported.

## The panes

| Pane | What it does |
|---|---|
| **Overlay** | Paste or open an ACIP document. Every dictionary match gets a purple depth-wash (darker = more deeply nested word-in-phrase); provisional glosses get a dashed amber underline; illegal syllables (typos) get a red wave; a variant particle that disagrees with the preceding suffix (Wilson sandhi tables) gets a dotted amber underline — a soft review flag, never auto-corrected (da-drag stems like *shin tu* / *yin kyang* are recognized, and particles inside longer dictionary phrases are skipped). Words are also found through fused particles (`PA'I` → *pa* + genitive `'i`, `PAS` → *pa* + agentive `s`, `PAR` → *pa* + la-don `r` — the last two only off vowel-final stems). Click a word for its entry + HGM corpus concordance + reference definitions (fused-particle and suffix-agreement notes included); click a bare particle for its paradigm card; click again to cycle outward through containing phrases. For Derge/Lhasa Kangyur and Derge Tengyur texts, "Follow along in scans" opens the woodblock page images from BDRC right under the document and keeps them in step with the @folio marker at your cursor — the folio↔image mapping is BDRC's own IIIF manifest labeling (nothing guessed), pages cache locally (`library/scan_cache/`), the license (public domain mark) is shown, and if BDRC is unreachable you get a browser link instead. Texts without a catalog mapping (e.g. Sungbum) get "Find scans on BDRC (title search)": the query is pre-filled from the text's own opening line (converted ACIP→EWTS), you confirm the matching edition from the candidate list — never auto-asserted — and the link is remembered per file. A Display panel controls information density, persisted across sessions: text as Tibetan script / ACIP / Wylie, and independent toggles for phonetics, HGM definitions, corpus usage, grammar marks, Sanskrit reference, Hopkins reference, and the reference dictionaries. |
| **Analysis** | Paste a passage (+ optional draft translation), press Analyze. The engine injects verified HGM term anchors and corpus hits into your 18-section analysis template, streams Claude Opus 5's report, then machine-checks it (segment reconstruction, dictionary quoting, ACIP integrity). Every report auto-saves to `analyses/`. Costs cents per passage. |
| **Trainer** | The reading tutor (docs/TRAINER_DESIGN.md). Paste a passage, try to read it yourself, then reveal one layer at a time: ① clause/phrase chunks, ② particle roles (Wilson labels), ③ suggested reading order (verb first — spotted via HGM's own tense data and "to …" glosses, or honestly marked unverified; genitive/dang chunks get "+" = read with the next chunk, right to left), ④ tier-labeled vocabulary, ⑤ the answer key — HGM's actual English when the clause is in the corpus, otherwise a pointer to the (clearly labeled) AI Analysis pane, ⑥ the full Wilson parse: every unit designated — case particles with their eight-case number assigned contextually by the clause's verb class (la = 2nd-case destination under a motion verb, 7th-case referential under an attitude verb; gis-family under a verb of absence is marked NOT-agentive), syntactic/lexical particles, verbs with class and tense row, verbal nouns (only when the stem shows verb evidence), pronouns, numbers, postpositions — and anything undecidable honestly labeled unresolved. Engine guidance is guidance; only corpus lines are answers. |
| **Drills** | Corpus-generated exercises (docs/PEDAGOGY_ROADMAP.md): chunk-ordering (restore a real clause's order, then see HGM's English + the reading-order guidance), cloze (which Tibetan chunk fills the blank, given HGM's English), particle choice (which family variant belongs here — explained from the agreement tables, including attested-variant honesty), parallel reading (segment by segment, English on demand; reveals count as peeks), vocabulary review — a spaced-repetition deck that builds itself from the words you click in the Overlay — and Translate & compare: render an unseen, level-appropriate corpus segment yourself, then see HGM's own translation with a terminology diff (which of the source's terms found an HGM equivalent in your draft). Nothing grades style; the master's version teaches. Every answer is HGM's own text; Tibetan-script toggle; progress lives in a local `progress.db` (your data, never in the release). |
| **Draft** | The working translator's workspace. Paste the source: it splits into clauses; click a clause for its tier-labeled HGM anchors, click a term for its full corpus concordance (per-course counts + real HGM renderings). Write your English below and press Check terminology: every source term is matched against HGM's binding equivalents — terms with no HGM equivalent in the draft, mixed renderings of a repeated term, drafts leaning on PROVISIONAL glosses, and one English word serving several Tibetan terms are all flagged for review. The engine anchors and checks; it never writes English for you. "Extract outline (sa bcad)" rebuilds the commentary's own topical tree from its "… la gsum ste / dang po ni …" markers (compound teens like bcu gcig handled; out-of-sequence ordinals flagged, never reshuffled; lexical ordinals like "sa bcu gcig pa" correctly ignored). "Verse meter" counts syllables per line, names the dominant meter (7/9/11…), and highlights lines that break it — with a register reminder that verse translates differently than prose. "AI back-check" (needs `ANTHROPIC_API_KEY`) asks Claude for a coverage diff only — omissions, additions, anchor mismatches, verdict — never a retranslation, and the output is banner-labeled as model output. "Detect quotations" finds passages that exactly match corpus segments over 7+ syllables — attested quotations with their published English, never inferred (lesser overlaps stay in the concordance) — and recommends the matching published bibliography entries for one-click insertion. The shared-apparatus search (STD-008) finds published footnotes (344) and bibliography entries (138) from the released Mixed Nuts volumes and inserts them with their citation — GMR's directive: work performed once is never redone. "Export draft (RTF, *italics*)…" writes the draft as RTF for the publishing workflow — text between *asterisks* becomes true italics (STD-004: name-parts woven into a verse are italicized), Tibetan Unicode survives exactly, and an unclosed asterisk is warned about. "Compose bibliography entry…" assembles a new entry in the published house format (STD-007) from a live-preview form — type the ACIP number and "Auto-fill from catalog" drafts the author, dates, and both titles from the catalog's 1,846 works (labeled for review; regenerate the join with `tools/extract_catalog_works.py`); fields print exactly as typed (with an optional pair-hyphenation helper for author names), the ACIP number derives the BDRC scan link, and the insertion is labeled pending publication. "Propose footnote" saves your own candidate to the pending queue, clearly marked ⚠ PENDING; the official MAIN documents (`docs/apparatus/`) are compiled only from published, GMR-approved volumes (regenerate with `tools/extract_mixed_nuts_notes.py`, `tools/extract_mixed_nuts_bibliography.py`, `tools/build_main_apparatus.py` per release). |
| **Library** | The user's text collection. "Install collection ZIP…" unpacks the Kangyur / Tengyur / Sungbum archives downloaded from asianlegacylibrary.org/library into `library/`; "Import my materials…" brings in the user's own files (.docx/.rtf auto-converted to text via macOS textutil). A sortable tree browser shows everything, with per-file ACIP catalog provenance decoded from the filename — including, for Derge/Lhasa Kangyur and Derge Tengyur texts, a direct "view the original scans on BDRC" link (the Tohoku number in the filename maps deterministically to BUDA's work IDs) — a text preview, and double-click-to-open in the Overlay. A name-search box lists matches as clickable links (combinable with decoder-driven filters: collection, verification status, language); a recently-opened list (from the local progress data) greets you on entry; "Send to OCR…" hands scans to BDRC's free offline Tibetan OCR app (or links its download page), with `library/ocr_out/` as the convention for OCR text output — review material until verified. The Search pane gains a "search the Library" button pointing its file-tree search at the whole installed collection; "Update search index" builds a prebuilt FTS index (incremental by file mtime) so library-wide queries — even across a full Kangyur — answer instantly, with token/phrase semantics matching corpus search. |
| **Search** | Gofer grammar over the 35,601-segment corpus: phrases, `OR`, `NEAR/N` (within N lines, same source), parentheses, course filter. Type a folder path in the second box to search raw ACIP file trees instead. |
| **Convert** | Type ACIP, wylie, Sanskrit IAST, or Devanagari (auto-detected): ACIP/wylie give wylie, Tibetan script, and GMR phonetics; Sanskrit input (e.g. `pramāṇa` or `प्रमाण`) gives IAST, Tibetanized ACIP, both official ACIP code styles (`prama#n%a` / `prama-n.a`), wylie, Tibetan script, Devanagari, classical IPA (standard scholarly mapping, with anusvāra assimilation), the provisional simplified pronunciation, and a per-letter "how to say it" guide following the FPMT transliteration standard. |
| **Lookup** | Headword in wylie/Tibetan/ACIP → HGM entry (tier-labeled) + stacked reference-dictionary layers + English→Tibetan reverse lookup. |

## Honesty rules (visible everywhere)

- `hgm_gloss` is binding; auto-aligned glosses always display as
  **PROVISIONAL**. Hopkins/{LC}/reference layers are labeled reference.
- The extracted reference dictionaries (TD, THL bundle) are **unlicensed
  compilations — local lookup only, never for release data** (see
  `docs/extractions/TIBETAN_DICTIONARY_DIC_DOSSIER.md`).
- Failed conversions render as `⟨wylie⟩` markers, never guessed.

## Rebuilding data (when a new HGM release lands)

    python3 tools/build_spine.py            # release + corpus → build/hgm_spine_v27_2.db
    python3 tools/build_reference_db.py     # extracted dictionaries → build/reference.db

Re-run the batteries afterwards (below). To re-extract the reference
dictionaries from their original binaries: `tools/extract_hypercontext_dic.py`,
`tools/extract_thl_dicts.py`.

## Building the app

    cmake -B cmake-build -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
    cmake --build cmake-build -j

## Tests — the trust anchor

    ctest --test-dir cmake-build

7 suites, ~170k verified data points. The engine batteries enforce founding
rule 2 (a port that fails its battery does not ship): `acipToEwts` must match
all 35,601 corpus conversions (100.000%), `wylieToUnicode` must hit the
canonical 99.03% on 26,318 ground-truth pairs, `pronounce` must be identical
to the Python engine on every distinct headword (105,576/105,576), the
Sanskrit converter must pass the official standard-doc examples and be
identical to the Python engine on all 79,452 distinct Lokesh Chandra IAST
terms (including failing on exactly the same malformed inputs), the
Devanagari→IAST engine must round-trip exactly over all 73,437 LC Devanagari
renderings, and the indexed overlay lattice must equal the reference
implementation span-for-span.

Regenerate the pronunciation reference after a spine rebuild:

    python3 - <<'EOF'
    import sys, sqlite3
    sys.path.insert(0, 'engines')
    from pron_engine import pronounce
    con = sqlite3.connect('build/hgm_spine_v27_2.db')
    with open('build/pron_reference.tsv', 'w') as f:
        for (w,) in con.execute("SELECT DISTINCT wylie FROM entries WHERE wylie IS NOT NULL AND wylie != ''"):
            if '\t' in w or '\n' in w: continue
            p = pronounce(w)
            if '\t' not in p and '\n' not in p: f.write(w + '\t' + p + '\n')
    EOF

Regenerate the Sanskrit reference after an LC-extract update:

    python3 tools/build_sanskrit_reference.py

## Repository map

    core/            allcore C++20 library + tests (spine, engines, lattice,
                     gofer, qc, spellcheck, refdict, analysis)
    app/             Qt 6 GUI + allanalyze CLI
    tools/           Python: spine/reference builders, extractors, validators
    engines/         canonical Python engines (the oracles — never edit casually)
    data/            release data, spellcheck data (CC0), extracted dictionaries
    docs/            roadmap, overlay design, analysis template, dossiers
    analyses/        auto-saved analysis reports

Project rules and history: `CLAUDE.md`, `ALL_TRANSLATION_TOOL_ROADMAP.md`.
