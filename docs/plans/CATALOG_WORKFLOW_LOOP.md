# The cataloging-workflow build loop — working prompt

*Authored by Claude at Adam's direction (2026-08-19): "implement what
you think will be helpful, then write your own prompt to follow to
then loop through making the in-house cataloging workflow everything
that we will need moving into the future." This file is the loop's
standing instructions; each iteration re-reads it, does ONE
well-scoped strike, proves it, presses when the app changed, and
updates the queue below.*

## The specification

`docs/research/GMR_CATALOGING_SESSIONS.md` § "The house rules,
consolidated" is the spec. TODO.md 9g is the roadmap. The charter is
inviolable: **the machine locates and suggests; humans approve; the
official catalog changes only through data releases; the app never
mints catalog numbers; the mother copy is never touched.**

## Each iteration

1. Pick the TOP unchecked item from the queue below (or the most
   valuable smaller strike if the top item is blocked — say why).
2. Build it the project way: core logic in `allcore` with a ctest
   battery where there is logic to prove; pane wiring in
   `app/main.cpp`; selftest pins for what the user actually sees
   (a bank that loads is not a card a cataloger can read); sweep
   coverage for new controls; honest empty/failure states.
3. Prove it: full ctest green, selftest ALL PASS, live screenshot
   when UI changed. Measure against the real library where a claim
   is measurable — and record what was tried and rejected.
4. If the app changed: graceful-quit → press (package_macos.sh) →
   relaunch → verify running.
5. Update the queue below (check the box, one line on what shipped,
   with measured numbers). Update TODO 9g if scope changed. Append
   to the memory file only when something durable was learned.
6. Manual (data/help/USER_MANUAL.md Chapter 14) grows a subsection
   per shipped lane; sync distribution copy + docx in the same
   iteration the feature ships.
7. Commit with the usual style. One iteration = one commit (plus
   press). Then schedule the next iteration.

## Ground rules

- Port nothing from spec when a canonical file exists; measure
  before shipping thresholds; never guess — flag and surface.
- Every lane shows its evidence and its source; every suggestion is
  labeled a suggestion; wrong-shelf risk means shelving stays human.
- Anything that writes inside the destination tree (moves, renames,
  catalog files) requires an explicit user click, shows a preview
  first, and never deletes.
- If an iteration discovers the queue is misordered (something
  cheaper unlocks something bigger), reorder it and say so in the
  commit.
- Stop condition: queue empty, or Adam interrupts. When the queue
  empties, do a closing audit iteration (docs truth pass + a fresh
  functional sweep of the Catalog group) and stop the loop.

## The build queue

- [x] 1. **Bibliography audit lane** — SHIPPED 2026-08-19 (suite 45
      catalog_audit_smoke; selftest pin; sweep 3 controls). Measured:
      347 distinct cited / 296 present / 51 missing against the
      installed library. THE C++ BATTERY CORRECTED THE PYTHON
      PROTOTYPE: a malformed 6-digit citation (ACIP S005282) had
      been truncated into a phantom number, and padding variants
      (S464/S00464) double-counted — refused and collapsed now;
      banked audit JSON regenerated to match. Report renders per
      source volume with citation evidence + the lower-bound caveat.
      Original spec follows: (9g 2a — GMR's job #1). Core:
      `allcore` bibliography-citation scan (ACIP numbers out of the
      apparatus/bibliography banks) + presence check against a
      library tree; battery pins the 51-missing measurement shape.
      Pane: a "Bibliography audit" button in the Catalog pane
      rendering the report per source volume with citation text as
      evidence and per-number presence status; honest caveat that
      the installed library is a lower bound. Data:
      `data/research/gmr_cataloging/bibliography_audit_missing.json`
      already banked.
- [x] 2. **Volume splitter — "chop assist"** — SHIPPED 2026-08-19
      (suite 46 volsplit_smoke; selftest pin; sweep 4 controls).
      Measured on 30 synthetic volumes built from 120 real library
      texts: 93.3% junction recall, 1.73 extra candidates/volume
      (one Avatamsaka text with narrative bzhugs-so = 45 of 52
      extras). TRIED AND REJECTED: an end-of-work (",,"/blank-line)
      gate before the anchor — removed zero false blocks, cost a
      real junction. The comma-density gate (>4 commas in the raw
      block = prose) is what killed the false-positive class.
      Cross-rule detections of one title page merge; same-rule
      neighbors warn instead (empowerment-parts rule). Original
      spec follows: (their #1 manual activity). Core: scan a whole ACIP file for candidate text
      boundaries: bilingual heads (BOD SKAD DU), BZHUGS SO blocks,
      kar-chag entry matches, closing formulas (MANGGA LAM / DGE'O /
      RDZOGS SO markers), page-number sanity. Emit candidate cut
      points WITH evidence + confidence; battery against known
      multi-text volumes from the library (construct fixtures by
      concatenating known single texts — ground truth by
      construction; measure precision/recall, ship thresholds just
      under measured). Pane: "Suggest splits…" on the selected
      intake file; preview panel listing proposed segments with
      title-page evidence; NEVER auto-cuts. Respect the
      don't-split-what-the-author-didn't-split rule: adjacent
      segments whose "titles" look like empowerment parts / chapter
      heads get a warning, not a cut.
- [x] 3. **Filename composer + META convention** — SHIPPED
      2026-08-19 (suite 47 catalog_name_smoke; 2 selftest pins;
      sweep 5 controls incl. the dialog). GROUND TRUTH CORRECTED
      FROM THE LIBRARY: the practiced companion is "NUMBER META.TXT"
      (space, not the transcript's "_META"); 1,457 real +.TXT names,
      93.5% with companions (95 genuinely incomplete pairs = a
      finding for the team), 194/200 sampled rejoins clean; legacy
      METAs are RTF (extractor ships), new ones write plain ASCII.
      +/META engages ONLY past the 255-char limit (Adam confirmed
      mid-build). Compose dialog prefills the Tibetan title from
      the file's title page; number field free text (registrar's
      act); rename intake-only with refusal-not-overwrite.
      Original spec follows: Core: compose
      `NUMBER_TIBETAN_ENGLISH_AUTHOR.TXT` from fields; enforce
      punctuation rules (no colon, single hyphen no spaces, drop
      BZHUGS SO/nāma, (Sanskrit) for tantra systems, no
      diacritics); at the OS limit fill to max, stop mid-word,
      append `+`, generate the companion `_META` file content
      (full title, dates, the rest). Round-trip: composed names
      must decode through `decodeAcipFilename`. Battery: the
      library's existing `* META.TXT` pairs as ground truth for the
      convention. Pane: a small form on the selected file (number
      field is FREE TEXT the registrar supplies — the app never
      mints), "Compose name…" → preview + copy; a rename-in-intake
      action with confirm (intake only, never the destination).
- [x] 4. **ASCII master catalog generator** — SHIPPED 2026-08-19
      (suite 48 catalog_list_smoke; selftest pin; sweep 6 controls).
      Kangyur measure: 927 files, 662 cataloged by name, 265
      uncataloged, T* marks for from-the-text titles; structure
      plain ASCII, values verbatim UTF-8 (a pointer must name the
      file exactly). Grammar fields gated on ACIP recognition so
      "mystery_scan.txt" underscores aren't mistaken for titles.
      Original spec follows: (session-3 directive). Core: walk a library tree, emit the field-coded ASCII catalog
      (S:/T:/A: author/L: language/F: format-provenance signals/P:
      pages-size/D: dates where decodable; colophon line when the
      text's tail carries one) — one record per file, README header
      documenting the format's lineage (St. Petersburg). Battery:
      round-trip parse + count pins. Pane: "Generate catalog
      list…" on the destination tree → writes to a user-chosen
      location + shows summary; this is a LIST of what's in the
      folders, not the official catalog — banner says so.
- [x] 5. **Title translator — the attested-fragments workbench** —
      SHIPPED 2026-08-19 (suite 49 title_xlat_smoke; selftest pin;
      sweep 7 controls). Held-out measurement over the library's
      3,613 bilingual filename pairs: 90.2% mean fragment coverage,
      62.0% whole-title near-match rate (the app's bank adds
      catalog_works.json on top, ~5.5k pairs). Rule 1 pure: whole
      pairs as evidence, no invented word alignment, uncovered
      syllables listed plainly. Original spec follows:
      (Adam's ask, 2026-08-19; Jamie's title glossary from session 2
      is the precedent; rule 1 shapes it: MATCH GMR's English, never
      compose). Core: a TitlePairBank from catalog_works.json (1,846
      GMR pairs), catalog_titles.json, and the library's bilingual
      filenames (~7,150 pairs); given a Tibetan title: (a)
      whole-title exact/near matches with GMR's published rendering;
      (b) per-phrase attestations — longest-match-first fragments of
      the title with every English rendering GMR used for that
      fragment in OTHER titles, each carrying its source-title
      evidence. Battery: held-out measurement (drop the title's own
      pair, measure whole-title recall + fragment coverage). Pane:
      "Translate title…" in the compose dialog + standalone button;
      the human composes from attested fragments — the machine only
      attests.
- [x] 6. **Tree diff — the divergence audit** — SHIPPED 2026-08-19
      (suite 50 tree_diff_smoke; selftest pin; sweep 8 controls).
      Recursive walk + size/sampled-content signatures (FNV over
      head+tail 4KB); classifies identical/modified/renamed-
      refiled/added/removed; ambiguous twins never guessed into
      renames; self-diff of the Kangyur (1,015 files) exactly clean
      in 320 ms. The Files pane's compare was single-level
      name+size — not reused, superseded here for this purpose. Core: compare two
      trees by relative path + size/mtime/content-hash tier;
      classify added/removed/renamed(same content, new path)/
      modified. Battery: constructed fixtures. Pane: "Compare
      trees…" button using the two browsers' roots; report in the
      evidence panel; the Files pane's compare machinery may be
      reusable — check before writing new.
- [x] 7. **Tohoku-first + colophon finder** — SHIPPED 2026-08-19
      (extends suite 44 catalog_id_smoke; strengthened selftest
      pin). KD/TD candidates carry "Tohoku N is the number to use"
      (KD/TD ACIP numbers ARE Tohoku, per the BDRC-linking design);
      colophon finder scans the tail for composition verbs (SBYAR/
      MDZAD/BRIS/BRTZAMS/BKOD) vs translation credits (BSGYUR/LO
      TS'A), nearest-end first, translator-is-not-the-author
      labeled; measured 87.5% of sampled library texts yield a
      candidate clause. Original spec: **Tohoku-first + colophon finder in the identity lane**.
      Identity candidates gain a Tohoku cross-reference where our
      concordances (bdrc_toh_labels, thl_dege_concordance,
      ekangyur/etengyur indexes) know the work: "Tohoku N — if
      correct, the number to use." Colophon finder: locate
      candidate colophon spans near text end (composition verbs,
      translator-credit patterns), render under SUGGESTED IDENTITY
      with the translator≠author warning. Battery: pins on known
      colophons from library texts.
- [ ] 8. **Cleanup + provenance lanes**. Line-slash stripper
      (detect the Aug-4 diagonal-slash-per-line corruption; report
      count; fix writes a NEW file, mother copy untouched);
      lowercase-run detector; double-space-in-name check;
      provenance banner per selected file (western pagination / no
      folio marks / parenthesized titles) with the "typed = suspect"
      reading, sourced to the sessions doc.
- [ ] 9. **Register view + three states**. Import a CSV/TSV register
      (Nick's spreadsheet shape: number, title, folio start/end,
      date, initials) read-only; per-file state line: number issued
      (from register or filename) / input exists (file present) /
      cataloged (inside the destination tree) — three independent
      lights, per the numbered≠input≠cataloged rule.
- [ ] 10. **Change-log helper**. On any app-performed rename/move in
      the intake tree, offer the date+initials suffix convention
      (user's initials from team/name settings); display the last
      changer decoded from folder names in the destination tree.
- [ ] 11. **Closing audit**: docs truth pass over Chapter 14 +
      README Catalog row; fresh sweep + selftest of the whole
      group; update TODO 9g statuses; write the handoff note; stop
      the loop.
