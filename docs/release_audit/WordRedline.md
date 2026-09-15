# Tracked-changes .docx export (F6, analysis suite batch 4) — release audit

*Added 2026-09-09. Files: `core/include/allcore/zipstore.h` + `core/src/zipstore.cpp` (STORED zip, CRC-32, no zlib), `core/include/allcore/docx_redline.h` + `core/src/docx_redline.cpp` (WordprocessingML redline writer), `core/tests/zipstore_smoke.cpp` (5 checks), `core/tests/docx_redline_smoke.cpp` (14 checks), `tools/test_zipstore.sh`, the `--compare A B out.docx` route in `app/main.cpp`. Sample for the Word check: `docs/release_audit/redline_sample.docx`.*

## 0. Correction of the research premise
`docs/TEXT_ANALYSIS_SUITE_RESEARCH.md` §3(a) said the Manuscript "already exports .docx; add w:ins/w:del runs". Verified false at HEAD: there was no OOXML writer anywhere; the Manuscript exports RTF through `textutil`. The writer is therefore built here, in `allcore`, so the Manuscript export can adopt it later (CLOSER #49).

## 1. Justification
Geshe Michael reviews in Word. A redline to Accept or Reject change by change, with a first paragraph that says exactly what is compared, under what rules, by whom, and that the marks are machine-computed, replaces a side-by-side HTML Geshe Michael cannot act on.

## 2. What it does

| Function | Where | Behaviour | Evidence |
|---|---|---|---|
| `zipstore::writeStored` / `listStored` / `crc32` | core | Deterministic STORED archive (fixed DOS stamps), central directory read back; no zlib | zipstore_smoke 1–5; `tools/test_zipstore.sh` → `unzip -t`: No errors detected (8 parts) |
| `docx::buildRedlineDocx` | core | Eight OPC parts; one paragraph per line; 1:1 Change → inline w:del/w:ins spans by syllable/word; unequal Change → whole deleted then inserted paragraphs; paragraph-mark revisions with id/author/date; minor hunks ALWAYS marked and counted; moved blocks as delete+insert (declared); Tibetan runs get the complex-script font slot; control characters → U+FFFD counted; first paragraph = the untracked provenance note; `<w:trackRevisions/>` on | docx_redline_smoke 1–13 (textutil reads it back; Accept All = text B, Reject All = text A in the smoke's reader) |
| CLI `--compare A B out.docx` | `app/main.cpp` | Author "<Team name or login> via Diamond Cutter compare"; provenance sentence with rules, summary, minor and raw counts, font, runner; prints the insertion/deletion counts | manual run 2026-09-09: exit 1, 8,421 bytes, PK, `unzip -t` clean |

## 3. Honesty
- No Save Report filter, no Versions button, no Tools item until the Word verification below is recorded — a control that cannot be trusted is a dead control.
- The author string names the machine; a bare person's name would present machine hunks as that person's edits.
- Minor differences are always included and counted; the pane's "Minor = differences" checkbox never filters the export.
- The document holds only the two texts and the note; no dictionary material is ever written.
- Known Word behaviour, declared in the code and the smoke: Word never removes the final paragraph mark, so Reject All can leave one empty trailing paragraph when the last paragraph was an insertion; Accept All is exact.

## 4. Release gate (open — needs Adam)
Open `redline_sample.docx` in Microsoft Word (note the version): Review must show the insertion and deletion attributed to "… via Diamond Cutter compare"; Accept All must give text B (`@001A / BLA MA LA / SEMS CAN KUN / BDE BA`); Reject All must give text A. Record the result here; only then add the Save Report filter, `reportBytes("docx")`, Tools ▸ Compare ▸ Send Comparison to Word…, and Versions ▸ Send Changes to Word….
