# Ven. Phil's catalog toolchain — mining survey (2026-08-08)

Read-only survey of the seven Objective-C/Cocoa tools (2012–2015, MySQL
via a hand-rolled `MySqlCocoaConnector`, several literally half-ported
from C#) that BUILT the ACIP catalogs our Library pane consumes. This
document banks the domain knowledge for the data project; nothing here
is app code. Paths under `~/ACIP Software Developmeent/Ven. Phil's
Development Folder/Mac_OSX/`.

**Also present, not yet surveyed: `ACIPMaintenance` (19,269 LOC)** — the
superset containing originals of several of these workers
(CatalogNumber.m, ImportFromDropboxWorker, ChkTengyurSequence,
AuthorsMaintenance, BldSubjItemTbl, CopyGMRSungbum…). Highest-value
follow-up target.

## The tools at a glance

| Tool | LOC | Purpose |
|---|---|---|
| Build ACIP Catalog | 22,977 | The flagship: intake raw ACIP files (whole or folio-group), verify folios, concatenate DOS double-return lines, split collections into texts, edit ~85-column `aciptbl`, emit Lucene XML |
| MakeGMRCatalog | 5,560 | Reverse-engineers NINE legacy fixed-layout catalog dumps (3 formats) into `gmrproject`; emits the GMR ASCII master catalog |
| MakeDergeCatalog | 2,170 | Parses the printed Tohoku/Derge Tengyur catalog text → `dergecatalog` (TD numbers) |
| MakeKangyurDropBoxCatalog | 2,652 | Parses the Tohoku Kangyur catalog (KL numbers) + walks the Lhasa Kangyur Dropbox tree (directory depth = section/volume) |
| FixTengyurCatalogInDropBox | 6,800 | Renames Dropbox Tengyur files to `<catNo>_<tibTitle>_@<sktTitle>.txt`; carries a full IAST→phonetics table + LC-dictionary Sanskrit-title fallback |
| ScanKangyurVolumes | 1,171 | Splits the 88 Lhasa Kangyur volumes into texts by scanning for RGYA GAR SKAD; carries the section-name gloss table |
| AddTGAuthorTitle | 1,023 | One-shot CSV enricher from `aciptbl` (baseCatNo join) |

## Crown-jewel domain knowledge (verbatim-sourced)

### 1. ACIP filename → catalog number grammar (ParseFileName.m)
`ppnnnnnsSSP<remainder>`: `pp` = 1–2 letters, first = collection
(S Sungbum, T Tengyur, R Reference); `nnnnn` = 4–5 digit main number;
`s` = single check-level char **A–M** (ignored for the catalog number;
optional); `SS` = sub-number, 1–2 digits for 1–99, and for ≥100 a
LETTER+digit where each letter adds 10 starting A=100 (implemented
`subNumBase = ((c-'A')*10)+100`); `P` = sub-part 'A'/'B'; remainder
after `.` or `_` ignored. Kangyur side (CatalogNumber.m):
`KLdddd<chk><s>[INC]`, sub-number `d|dc|dcd`; emitted as
`KL%05d[-sub](<chk-lowercase>TB)` e.g. `KL00127-3A(eTB)`; **check
level 'I' is silently rewritten to 'E'**; default chk 'E'.

### 2. Folio-group letters vs check levels — TWO separate axes
- Folio-group letters A–F (`TFolioGroup`) = independent DATA-ENTRY
  PASSES of the same folios (A-file, B-file… operators), stored as
  `catalogingStatus` FGA…FGF with per-letter comment columns
  `AFileComments…EFileComments`. Group selection: the group with the
  MOST files wins, ties to the higher letter.
- `chkLevel` (the letter inside the catalog number, A–E/I/M/N…) is the
  verification level our Library decoder shows; default 'E'.

### 3. Folio verification spec (VerifyFileWorker.h — lift whole)
`@` in column 1; `@dddA/@dddB` (3-digit zero-padded + side; 4 digits
allowed >999); Western paging `@ddd` no side (auto-set when >80% of
markers are sideless); roman-numeral front matter via charset `ivxlcd`;
alternate pagination `@dddA/ddd`; `@F` = unknown-folio placeholder;
`PAGINAT` accepted. Sequence tracking A→B→next; a restart at folio 1
signals "separate text starting around line N" (the split signal);
halt at >10 folio errors. Lines/page ≤70 (150 if long-pages flag);
modal value stored. Pass 0 strips DOS `\x1A`, drops `\r`, collapses
blank runs ("the ACIP standard specifies syllables separated by single
blanks"), reports unbalanced ()/[].

### 4. The DOS double-return convention (ConcatenateFolioFilesWorker.h)
Data-entry operators on non-scrollable DOS screens hit return to
continue a folio line and return TWICE to end it — so adjacent
non-blank file lines are ONE folio line (join with single space);
blank line = folio line end. >1000-char joined line ⇒ file corrupt.
(This is why our line-band follow-along's newline arithmetic works.)

### 5. Title extraction markers (three implementations)
- BZHUGS family ⇒ indigenous work ⇒ Sungbum (`collection = SB` not
  TG!): "BZHUGS SO," "BZHUGS," + misspellings "BZHUG SO," "ZHUGS,"
  "ZUGS SO," "ZHUGSSO," "ZHUGSO," — back up to the preceding comma for
  the title start.
- RGYA GAR SKAD DU (Sanskrit title follows) / RGYA DKAR variant / BOD
  SKAD DU (Tibetan title follows, ends at next comma) ⇒ translated
  canon text. FixTengyur handles the markers SPLIT ACROSS LINE BREAKS
  ("RGYA\nGAR SKAD DU," etc.). ScanKangyurVolumes uses bare
  "RGYA GAR SKAD" occurrences as TEXT BOUNDARIES in Kangyur volumes.
- Collection split rule (SplitVols): folio-number restart with
  thisFolioNo ≤ 12 = candidate boundary, confirmed by a BZHUGS title
  within ±4 lines. Child numbers: `.cp` colophon, `.kc` karchak,
  `.pf` preface, `.wNN` works.

### 6. Collection codes (AddWholeFiles.m, verbatim)
ACM ACI Course Materials · GA ACIP Graphics · GM Mongolia Graphics ·
GSP St. Petersburg Graphics · KG Kangyur · RCE/RCS/RCT Ref-Catalogs
(Eng/Skt/Tib) · RDC/RDCS/RDCT Ref-Dictionaries · RDO Ref-Documents ·
RGS Ref-Sanskrit Grammars · RH/RHT Ref-Histories · SKB/SKO Sanskrit
Buddhist/Non-Buddhist · SB Sungbum · TG Tengyur. Media types:
X transliteration, P PDF, R RTF, W Word, O other, PS/TS scans,
A audio, V video; textFormat B book / MS manuscript / PC pecha /
PL palm leaf; condition G/M/P/D(=endangered).

### 7. Printed-catalog line grammar (Derge + Tohoku parsers)
Field tags in column 1: `NO. ` catalog number (`NO. dddd. [vol.
folio-info]`), `1 ` Tibetan title, `4 ` Sanskrit title, `6 `/`8 `
author, `7 ` translator, `[Section]` / `[$Section]`, `!` volume
(ignored), `(` footnote, `@` folio (transparent), `----`/`____`
ignored; untagged lines are continuations. NA/NGA/NYA volume-letter
disambiguation: a bare trailing N after a G-volume ⇒ NG?, after a
J-volume ⇒ NY?. Order-check reports (missing ranges / duplicate
instances) → Desktop txt files. TD numbers `TD%05d`; parse stops at
JO BO'I CHOS CHUNG.

### 8. Lhasa Kangyur section glosses (ScanKangyurVolumes — only
English gloss table in the toolchain)
'DUL BA Vowed Morality · 'BUM PoW 100k · NYI KHRI PoW 20k · SHER
KHRI (cont.) · SHER PHYIN SNA TSOGS PoW various · DKON BRTZEGS Pile
of Jewels · PHAL CHEN Avatamsaka · MDO Discourses · MYANG 'DAS
Parinirvana · RGYUD Secret Teachings.

### 9. Schema knowledge (no CREATE TABLE anywhere; reconstructed)
Databases: `acipmaintenance` (master: aciptbl ~85 cols + aciptbldata
COMPRESS'd text + foliogroupfileinfo + authors + 3 per-language author
indexes + dergetengyurtextslist), `gmrproject`, `kangyurcatalog`,
`tengyurproject`. TWO column-name generations coexist (`catalog
number`/`title-tibetan` vs `catalogNumber`/`titleTib`) — a migration
happened mid-toolchain. `controldata` holds next-catRef + all UI
pick lists. Lucene XML output: flat `<ACIPDoc>` elements (catRef,
catNum, collection, titles, authors, subjects, body…) — the ancestor
of our own library index.

### 10. Warnings for any reuse
- SQL built by stringWithFormat + a copy-pasted `escapedString()` —
  treat as format documentation, never as code to port.
- ConvertSktToPhonetics.m (FixTengyur) has a TRANSPOSITION BUG:
  n→"l" and l→"n". Its IAST map is otherwise a useful comparandum for
  our sanskrit engine (which is battery-proven; theirs is not).
- Hardcoded user paths (venphil/philipbaker) and credentials
  throughout; the MySQL databases themselves were NOT found on this
  machine — the .sql dumps, if they survive, would be with Ven. Phil
  or in ACIP backups. ASK ADAM whether the acipmaintenance DB dump
  exists anywhere — `aciptbl`'s 85 columns are the richest catalog
  metadata in the project's history.

## What the data project should take from this
1. The filename→catalog-number grammar (§1) — decoder-grade, more
   complete than what our Library decoder currently implements
   (letter-hundreds sub-numbers, the (eTB) suffix form).
2. The folio verification + concatenation specs (§3–4) as the
   authoritative statement of input-center file structure.
3. The BZHUGS/RGYA GAR marker misspelling lists (§5) for robust title
   extraction in future ingests.
4. The section-gloss table (§8) and collection codes (§6) as UI
   reference data.
5. The pursuit of the lost MySQL dumps (§10).
