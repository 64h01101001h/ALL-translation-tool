# Draft workspace — full audit, 2026-09-11

Adam, on finding the two right-hand panes blank with a source loaded:
*"why are the upper right pane and the bottom right pane not displaying
anything? it doesn't make any sense. there should be a full audit done of
the Draft Workspace."*

Six dimensions were audited in parallel — empty states, persisted-state
pollution, dead controls, the `dataRoot_` defect, honesty labelling, and the
workflow a new user actually walks. **60 distinct findings.** The ten most
severe were then handed to independent agents told to *refute* them:
**10 were checked and none were refuted.** One verifier compiled a probe
against the shipped bibliography composer to prove the junk string
character-for-character rather than reasoning about it.

| severity | count |
|---|---|
| high | 18 |
| medium | 31 |
| low | 11 |

**Fixed the same day** (the three that explain the screenshot):

1. `anchors_->clear()` in `load()` destroyed the Evidence Ribbon's own
   instructions at the exact moment the user pressed the pane's primary
   button. It now advances to the next instruction instead.
2. `report_` was never given any content at all — blank and unlabelled from
   launch. It now names itself and the tools that fill it.
3. The bibliography guard `if (entry == ".")` was **dead code**: the composer
   emits `".  ."`, never `"."`, so every accept inserted scaffolding junk
   into the draft under a banner claiming it conformed to the published house
   format — and `sess::remember` persisted it across quit. Both guards now
   test for content, and the buttons disable themselves when there is none.

Both panes also carry a `setPlaceholderText` backstop, so a future `clear()`
cannot blank them again.

---

## Every finding

### [HIGH] Renaming a draft orphans its entire version history, and the Versions window then says "No versions yet"

**Where.** `app/main.cpp:25536 (HEAD: 25534) — DraftPane::renameDraft; key derivation at core/src/versions.cpp:468-483 and app/versions_pane.inc:21-24; the base-name directory rename at app/main.cpp:4194-4201`

**What the user sees.** A translator saves a draft anywhere outside the Diamond Cutter Tool Data folder (the normal case — Save As offers a bare "draft.txt", so it lands in Documents/Desktop), saves it a dozen times building up a version history, then uses Rename. After the rename the Versions window is empty and states "No versions yet: the first Save of this text will keep one." Every earlier version is still on disk but no longer reachable from any UI, and the "Show versions of a file with this name in another folder" escape hatch stays hidden because the directory the window looks in does not exist at all. The user is told, in plain words, that work they did exists nowhere.

**Cause.** renameDraft hands renameFileTo the directory-sidecar pair {dataRoot_ + "/library/versions", "/"}, and renameFileTo (app/main.cpp:4194-4201) renames "<dir>/<oldCompleteBaseName>" -> "<dir>/<newCompleteBaseName>". But the versions store is NOT keyed by base name. docprops::versionsDir (app/versions_pane.inc:24) keys it by allcore::versions::docKeyFor, which (core/src/versions.cpp:468-483) returns the bare stem ONLY when the file lies under dataRoot, and otherwise returns "<stem>~<first 8 hex of sha1(absolute path)>". For a draft outside the data root the path renameFileTo probes ("<versions>/<oldBase>") is not a directory, so QFileInfo(a).isDir() is false and the rename is silently skipped. The new lookup key is then "<newBase>~<sha1 of the NEW absolute path>" — a directory that has never existed.

**Smallest fix.** Stop pretending the versions store is base-name-keyed. In renameDraft, capture the real directory before the rename and move it after: `const QString oldVer = docprops::versionsDir(dataRoot_, draftPath_);` before the renameFileTo call, then after `draftPath_ = nw;` do `if (QFileInfo(oldVer).isDir()) QDir().rename(oldVer, docprops::versionsDir(dataRoot_, nw));`. Drop the `{dataRoot_ + "/library/versions", "/"}` pair from the renameFileTo argument list so the two mechanisms cannot fight; keep the `{dataRoot_ + "/library/properties", ".json"}` pair, which IS base-name-keyed (docprops::sidecarPath, app/main.cpp:4065-4068) and is handled correctly today.


### [HIGH] Moving a draft orphans both dataRoot_-keyed sidecars — moveDraft passes no sidecar directories at all

**Where.** `app/main.cpp:25527 (DraftPane::moveDraft); moveFileTo at app/main.cpp:4146-4167`

**What the user sees.** The translator files a finished draft into a project folder using Move. The draft's version history disappears from the Versions window with the same false line, "No versions yet: the first Save of this text will keep one." Nothing warns that the history was left behind. A move between two folders that are both outside the data root also changes the key, so this happens on every Move, not just moves across the data-root boundary.

**Cause.** moveDraft calls `docprops::moveFileTo(this, draftPath_, dir, nw)` — a four-argument overload that has no sidecar parameter whatsoever (app/main.cpp:4146-4147), so it renames only the file itself. dataRoot_ is never consulted on this path. Because docKeyFor hashes the ABSOLUTE path for any file outside the data root, the versions directory the Versions window looks in after the move is a different, empty directory.

**Smallest fix.** Same shape as the rename fix, in moveDraft: `const QString oldVer = docprops::versionsDir(dataRoot_, draftPath_);` before the moveFileTo call, and after `draftPath_ = nw;` add `if (QFileInfo(oldVer).isDir()) QDir().rename(oldVer, docprops::versionsDir(dataRoot_, nw));`. The properties sidecar needs nothing: it is keyed by completeBaseName only (app/main.cpp:4065-4068), which a Move does not change.


### [HIGH] The "empty bibliography entry" guard is dead code — it inserts ".  ." junk into the draft

**Where.** `app/main.cpp:26853 (and its twin at 26816); composer at core/src/tibexport.cpp:172-193`

**What the user sees.** Exactly the reported junk: "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): (Co-ne Bla-ma) Grags-pa.  .]" pasted at the cursor, once per accept. The draft's "Your English draft…" placeholder is gone for good, and because sess::remember(draft_, "draft/english") (app/main.cpp:25210, restore at app/main.cpp:3244) writes the draft to QSettings, the junk survives a quit and is still there next launch.

**Cause.** composeBibliographyEntry unconditionally appends the sentence-spacing period and the terminal period: `out += ".  " + f.english_title;` … `out += ".";` (core/src/tibexport.cpp:182, 192). So an entirely empty dialog composes to ".  .", never ".". The guard at app/main.cpp:26853 tests `if (entry == ".") return;` — a string the composer cannot produce — so it never fires and every accept inserts whatever partial stub the fields hold. The twin guard at app/main.cpp:26816 is the "Save as candidate" path, so the same dead test lets an empty entry be appended to candidate_bib.json with status "pending GMR approval".

**Smallest fix.** Test the fields, not the composed string. At both sites replace `if (entry == ".") return;` with a check on the substance, e.g. `const auto f = assemble(); if (f.author.empty() && f.english_title.empty() && f.tibetan_title.empty() && f.acip_number.empty()) return;` (for the Sanskrit branch, the same over author_skt/english_title/sanskrit_title/tibetan_title/acip_number). Cheaper still and belt-and-braces: have the dialog disable the Insert/Save buttons whenever that predicate is true, so the refusal is visible before the click.


### [HIGH] Step 2 kills the Evidence Ribbon: load() erases the only on-screen instructions for the top-right pane and puts nothing back

**Where.** `app/main.cpp:25748`

**What the user sees.** Before pressing Load source the top-right pane reads "The Evidence Ribbon — load a source, then click or arrow into any clause…". The instant Load source succeeds, that text vanishes and the pane is permanently, featurelessly blank. This is precisely the screenshot: loaded source, populated clause list, empty top-right. Nothing anywhere on screen now names that quadrant or says how to fill it, so a new user reasonably concludes the feature is broken and stops.

**Cause.** load() ends with anchors_->clear() (25748) and never calls showAnchors(). The placeholder set at 25196-25200 is the ONLY place in the entire pane that names the Evidence Ribbon and gives its instruction, and clear() destroys it at exactly the moment the user has done the thing that earns it. Compare demo() at 25424-25428, which does the right thing — it calls load() and then showAnchors(0) — so the screenshot path and the demo path diverge on the one line that matters.

**Smallest fix.** Replace `anchors_->clear()` at 25748 with `if (!clauses_.empty()) showAnchors(0); else anchors_->setHtml(<the same idle HTML as 25196>);`. Loading a source should land the user on clause 1's evidence, not on an empty box.


### [HIGH] The guard that was meant to stop junk bibliography entries entering the draft is dead code — it can never fire

**Where.** `app/main.cpp:26853`

**What the user sees.** Exactly the reported junk: "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): (Co-ne Bla-ma) Grags-pa.  .]" sitting in the English draft, repeated. Each press of "Insert into draft" with an incomplete form stamps another one in. The text is labeled with a house standard number, so it reads as authoritative output rather than as the malformed stub it is — and the translator has to hand-delete each one out of their prose.

**Cause.** composeBibDialog guards the insertion with `if (entry == ".") return;`. But composeBibliographyEntry never returns ".": with every field empty it returns ".  ." (period, two spaces, period), because core/src/tibexport.cpp:182 unconditionally appends ".  " + english_title and :192 unconditionally appends ".". The equality test therefore never matches, and there is no other validation at all — no required field, no warning, no preview confirmation. The user's exact string is reproduced by filling only Epithets="Co-ne Bla-ma" and Author="Grags-pa": "(Co-ne Bla-ma) " + "Grags-pa" + ".  " + "" + "." = "(Co-ne Bla-ma) Grags-pa.  .".

**Smallest fix.** Replace the equality test at 26853 (and 26816) with a real precondition — refuse when the required fields are blank, e.g. `if (au->text().trimmed().isEmpty() || et->text().trimmed().isEmpty()) { <tell the user which field is missing>; return; }` — and disable the "Insert into draft" button until it passes, rather than inserting and letting the user discover the stub in their prose.


### [HIGH] "Check terminology" over an empty source certifies an unchecked draft as clean

**Where.** `app/main.cpp:27003`

**What the user sees.** A translator with English in the draft box but no source loaded (or who cleared the source) presses Check terminology and reads: "0 term(s) · 0 without an GMR equivalent in the draft · 0 with mixed renderings · 0 drafted from PROVISIONAL glosses". Three zeros and no warning read as a pass. Nothing on the card says the check had no source to check against.

**Cause.** check() has no empty-input guard. The terms come from the SOURCE text, so an empty source yields an empty rep.terms, and the header at 27013-27017 renders that as a row of zeros — a clean bill of health computed over nothing. The pane's two neighbouring commands both guard this case correctly (aiCheck at 26991-26993 prints "paste both the source and a draft first"; detectQuotes at 26532-26535 prints "paste the source text first"), so this is an inconsistency within the same class of button, not a house convention.

**Smallest fix.** Add the same guard the sibling commands use at the top of check(): if the source is empty, `report_->setHtml("<i>load the source first — the terminology check compares the draft against the source's terms</i>"); return;`.


### [HIGH] The junk in the draft box is stamped as conforming to the published house standard (STD-007) when it is a skeleton with no title

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:26925-26929 (guard at 26926); composer at /Users/adamderickandrade/ALL-translation-tool/core/src/tibexport.cpp:172-194`

**What the user sees.** The exact text the user reported: "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): (Co-ne Bla-ma) Grags-pa.  .]". Because the bracket asserts "house format (STD-007 / DCC guide)", a reader — or a later publishing pass grepping for that marker — is told this stub IS a house-format entry. It has no English title, no Tibetan title, no ACIP number, no folios.

**Cause.** composeBibDialog's only completeness gate is `if (entry == ".") return;` (main.cpp:26926), which fires only when EVERY field is blank. With one field typed, composeBibliographyEntry emits `out += ".  " + f.english_title;` then `out += ".";` (tibexport.cpp:182,193) — giving literally "(Co-ne Bla-ma) Grags-pa.  ." for epithets+author only, which is character-for-character the user's junk string. The guard also misses the Sanskrit branch entirely: an all-blank Sanskrit entry composes to ".  .", which is != "." and so passes. The repetition is the dialog's default button: "Insert into draft" is the first AcceptRole button on the QDialogButtonBox (main.cpp:26880), so pressing Return in any QLineEdit — the natural way to move between form fields — accepts the dialog and inserts, and doing it again on the next attempt appends another copy.

**Smallest fix.** Replace the `entry == "."` guard with a real completeness check on the fields the format requires — at minimum author and english_title non-empty — and disable the "Insert into draft" button until they are, so Return cannot fire it. Keep the STD-007 wording only for entries that actually satisfy the template; otherwise insert nothing and say why in the dialog.


### [HIGH] "Save as candidate" banks a different entry from the one the preview showed, and reports success

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:26885-26889 (vs composedNow at 26828-26861, preview at 26863)`

**What the user sees.** With "Sanskrit work — full canonical citation" ticked, the live preview shows the full Sanskrit form (author, Tib: author, Sanskrit title, volume, section, collection, edition). Pressing "Save as candidate (pending GMR approval)" then reports "Bibliography candidate saved — pending until published and approved; it appears in the review sheet". What was actually written to candidate_bib.json is the Tibetan-template form with every Sanskrit-only field silently dropped. The user has a green receipt for text he never saw, queued for GMR's approval.

**Cause.** The candB lambda captures only `assemble` and calls `allcore::composeBibliographyEntry(f)` unconditionally (main.cpp:26887-26888). `assemble()` (main.cpp:26817-26827) fills only BibliographyFields — it never reads sat/sst/svn/svl/sse/sss/sstb/sco/sed. The preview and the Insert path both go through `composedNow()` (main.cpp:26828), which branches on skC->isChecked() and calls composeSanskritBibEntry. Only the candidate path forgot the branch.

**Smallest fix.** In the candB lambda, capture and call `composedNow()` instead of `composeBibliographyEntry(assemble())`, and record which composer produced it in the JSON object alongside `status`.


### [HIGH] The AI back-check's completion banner claims the model's citations are engine-verified; nothing verifies them

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:27053-27056`

**What the user sees.** Over the model's prose: "AI coverage diff — model output, labeled AI; the term anchors it cites are engine-verified." A translator reads the second clause as a guarantee that every Tibetan term and every "HGM gloss" appearing inside the AI paragraph was checked against the dictionary. If the model invents a Tibetan headword, or attributes an English equivalent to Geshe Michael Roach that he never used, it arrives under that guarantee — which is exactly the COMPOSE-not-MATCH line rule 1 draws.

**Cause.** The anchors are engine-verified on the way IN, not on the way OUT. buildBackTranslationPrompt (core/src/analysis.cpp:277-317) writes the pre-pass anchors into the prompt under the heading "## ENGINE: HGM term anchors (verified)" and the system prompt instructs the model to "never invent English equivalents for terms without HGM glosses" (analysis.cpp:294-296). That is an instruction, not a check. The finished-stream handler does no cross-reference of aiAccum_ against pre.anchors — it html-escapes the text and prints it.

**Smallest fix.** State what is true: "model output, labeled AI — it was GIVEN engine-verified anchors; its own citations are NOT checked against the dictionary." (Or capture `pre` and mark any Tibetan term in the output that is not in pre.anchors.)


### [HIGH] An empty AI response renders as a confident, finding-free coverage diff

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:27060-27063`

**What the user sees.** The "AI coverage diff — model output, labeled AI; the term anchors it cites are engine-verified" banner with an empty body underneath. There is no error, no TRUNCATED warning, no "nothing came back". The pane's own idle text is gone by then. A reviewer reads an empty diff under a completed-looking header as "the model found no omissions and no additions" — an all-clear on a run that produced nothing.

**Cause.** Both guard branches require a non-empty condition that this case fails. Branch 1 needs `error() != NoError` AND `aiAccum_.empty()`; branch 2 needs `!aiComplete_` AND `!aiAccum_.empty()`. A 200 stream that emits an `event: error` (overloaded_error / api_error mid-stream), or whose deltas sseTextDelta fails to parse, leaves error()==NoError, aiAccum_ empty, aiComplete_ false — neither branch fires, and the banner stands alone.

**Smallest fix.** Add a first branch before the banner is built: `if (aiAccum_.empty()) { report_->setHtml("<b>NO REPORT RECEIVED</b> — the request returned no text. This is not a clean result; run it again."); ... return; }`.


### [HIGH] The live terminology chip prints a green all-clear when zero terms were checked

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:25263-25273`

**What the user sees.** Under the draft box, in green: "✓ every established term is rendered; no shared-English collapses". The same green line appears whether the check examined forty terms and matched them all, or examined nothing at all. The user is told his terminology is clean on the strength of a check that found no terminology.

**Cause.** The success branch tests `!unmatched && rep.shared.empty()`. Both are vacuously true when `rep.terms` is empty — which happens whenever buildOverlay yields no span carrying an hgm_gloss: source pasted as Tibetan unicode rather than ACIP/Wylie, a source the tokenizer does not parse, a passage of only particles (checkTerminology drops freestanding particles at core/src/terminology.cpp:73-78), or a spine that failed to match. The only entry condition is `src` non-empty and the draft ≥20 characters (main.cpp:25225).

**Smallest fix.** Before the green branch, add `if (rep.terms.empty()) { termLive_->setStyleSheet("font-size:11px;color:#777"); termLive_->setText("no established terms recognised in the source — nothing was checked"); return; }` — neutral grey, not green.


### [HIGH] "Send to Manuscript →" silently replaces the entire Manuscript — formatting, undo history and all — even when the draft is empty

**Where.** `app/main.cpp:25308 (handler) → app/main.cpp:42719 (hook) → app/main.cpp:37683 (setManuscriptText)`

**What the user sees.** They press the WORKBENCH button, the app jumps to the Manuscript pane, and whatever they had written there is gone. Bold and italics applied with the Manuscript's own toolbar are flattened. Cmd-Z does not bring it back. If the draft happened to be empty, the Manuscript is now blank. No dialog, no warning, no status line.

**Cause.** The handler is an unconditional pass-through: `connect(toMssBtn, &QPushButton::clicked, [this]{ if (g_sendToManuscript) g_sendToManuscript(draft_->toPlainText()); });` (25308-25311). g_sendToManuscript is `manuscriptPane->setManuscriptText(t)` (42720), and setManuscriptText is `editor_->setPlainText(t)` (37684). editor_ is a QTextEdit (app/main.cpp:37362), so setPlainText discards all character formatting and resets the document's undo stack. There is no empty-draft guard, no dirty check on the destination, and no confirmation.

**Smallest fix.** Two lines in the handler at 25308: refuse an empty draft with a reason on the pane's one always-visible status surface — `if (draft_->toPlainText().trimmed().isEmpty()) { termLive_->setText("Nothing to send — the draft is empty."); return; }` — and, when g_manuscriptText() returns non-empty text that differs from the draft, ask before replacing (QMessageBox::question, skipped under g_harnessRun as confirmDiscardDraft does at 25483).


### [HIGH] "Save as candidate" in the bibliography dialog ignores the Sanskrit-work checkbox and silently discards nine fields the user filled in

**Where.** `app/main.cpp:26885-26888`

**What the user sees.** They tick "Sanskrit work — full canonical citation", fill in the Sanskrit title, author-in-Tibetan, volume number and letter, section names, collection and edition, watch the live preview assemble the full Sanskrit citation, then press "Save as candidate". What is written to candidate_bib.json is the short Tibetan-template entry. Every Sanskrit-only field is gone, and nothing says so. The candidate later surfaces in the review sheet as a different entry from the one they composed.

**Cause.** The candidate handler calls `allcore::composeBibliographyEntry(assemble())` (26887-26888). `assemble()` (26817-26827) builds a BibliographyFields carrying only the seven basic fields and cannot represent the Sanskrit ones. The preview and the Insert path both go through `composedNow()` (26828-26861), which branches on `skC->isChecked()` and calls composeSanskritBibEntry. The candidate path is the one place that never consults the checkbox.

**Smallest fix.** Change the candB lambda at 26885 to capture and call the same function the preview uses: `[this, composedNow]` and `const QString entry = composedNow();`, dropping the `assemble()` call. The acip_no field written at 26898 should then come from `ac->text().trimmed()` directly rather than from f.


### [HIGH] Tools ▸ Protect Document says "read-only until you turn this off", but six DraftPane paths write into the protected draft anyway

**Where.** `app/main.cpp:25502 (setProtected) vs 25413, 26597, 26927, 26996, 27005 (writers) and 25490, 25510 (clearers)`

**What the user sees.** They protect the document and get the status message "Document protected: read-only until you turn this off." Typing is indeed blocked. Then clicking "[+ technical spelling]" in the Evidence Ribbon, or "[insert]" on an apparatus result, or Insert-into-draft in the bibliography dialog, still writes into the draft. File ▸ New still empties it. The protection the app announced did not hold, and the user has no reason to re-check.

**Cause.** `setProtected` is only `draft_->setReadOnly(on)` (25502). Qt's read-only flag is a widget input property; it does not guard the underlying QTextDocument. Every insertion in this pane goes through `draft_->textCursor().insertText(...)`, which operates on the document and bypasses the flag entirely. `newDraft()` calls `draft_->clear()` (25490) and `revertDraft()` calls `draft_->setPlainText(...)` (25510); both are document operations and both work while read-only.

**Smallest fix.** Add one early return to each of the six paths, using the accessor that already exists: `if (isProtected()) { termLive_->setText("The document is protected — unprotect it (Tools ▸ Protect Document) to insert."); return; }`. Cheaper still, factor the five insertions through a single private `insertIntoDraft(const QString&)` that carries the guard, the insert, `draft_->setFocus()` and the missing `ensureCursorVisible()` (see the separate finding on invisible insertions).


### [HIGH] "Check terminology" with no source loaded reports a clean zero instead of refusing — a false all-clear from the pane's flagship safety net

**Where.** `app/main.cpp:27076`

**What the user sees.** With English in the draft but nothing in the source box (the common state after restoring a session, or when working from a paper text), pressing EVIDENCE ▸ Check terminology fills the report pane with "0 term(s) · 0 without an GMR equivalent in the draft · 0 with mixed renderings · 0 drafted from PROVISIONAL glosses". That reads as "your draft is clean". Nothing was checked at all.

**Cause.** `check()` (27076-27079) calls allcore::checkTerminology on whatever the two boxes hold and renders the result unconditionally. There is no empty-input guard, so an empty source produces an empty term list, which renders as a zero-count report rather than as a refusal.

**Smallest fix.** Mirror the live guard at the top of check() (27077): if the source is empty, `report_->setHtml("<i>Load or paste the ACIP source first — the terminology check compares the draft against the source's terms, and with no source there is nothing to compare.</i>"); return;` and likewise for an empty draft. Never render a zero-count report for an input that was never supplied.


### [HIGH] DraftPane's selftest writes real save/version/properties records into the translator's live data root

**Where.** `app/main.cpp:25739 (block 25735-25776; saves at 25744 and 25754; the correct pattern already sits at 25761)`

**What the user sees.** File > Properties and the Versions window for drafts show a document history nobody wrote. The translator's own library/properties/ and library/versions/ folders hold six all_*selftest* records; the draft one reads revision 396, "last saved by Adam".

**Cause.** DraftPane::selfTest's Save/Save As block runs saveDraftAs() (25744) and saveDraft() (25754) while dataRoot_ still points at the real data root, so the docprops::noteSave and docprops::noteVersion calls inside DraftPane::saveDraft write a properties sidecar and version snapshots into exactly the stores a real draft uses. The block cleans up only the /tmp .txt (QFile::remove at the end). The F0 sub-block immediately below at 25761 already knows the right pattern — it swaps dataRoot_ to a temp dir and removes it — but it wraps only its own single save. Because shot/selftest/sweep mode forces g_userName = "Adam", every one of those fabricated saves is attributed to him by name.

**Smallest fix.** Hoist the dataRoot_ swap that already exists at 25761 so it wraps the whole block: put `const QString keepRoot = dataRoot_; dataRoot_ = QDir::temp().filePath("all_selftest_draft_root");` immediately after the QFile::remove(outP) at 25740, and end the block with `QDir(dataRoot_).removeRecursively(); dataRoot_ = keepRoot;`. The F0 assertion that revision == 1 still holds because it already calls removeRecursively() on the temp root before its own save. Then delete the six stray records from the live data root by hand.


### [HIGH] overlay/lastFile is written with no harness guard, so every battery run repoints the translator's remembered document

**Where.** `app/main.cpp:9166`

**What the user sees.** After any battery run, the next real launch opens a harness temp file in the reading pane instead of the document they were working on — or, once that temp file has been reaped, opens nothing at all. The "where I was" is gone with no message.

**Cause.** OverlayPane::openFile guards its Recent-Files write with `if (!g_harnessRun)` forty lines earlier (9123), and both saveSession() and restoreSession() carry `if (g_harnessRun) return;`. But the write at 9166 is a bare `QSettings("ALL","TranslationTool").setValue("overlay/lastFile", fn);` with no guard at all. So every --selftest / --sweep / --screenshots run that opens a fixture (overlay->openFile(tmp) in the sweep and in the screenshot driver) silently overwrites the key the next real launch restores from.

**Smallest fix.** Guard it exactly like its neighbour at 9123: wrap line 9166 in `if (!g_harnessRun) { ... }`. Better still, route it through sess::put("overlay/lastFile", fn), which carries the guard by construction and is where every other "where I was" key already lives.


### [HIGH] composeBibDialog's empty-form guard is dead code: ".  ." reaches the draft, and can reach the pending-GMR queue

**Where.** `app/main.cpp:26936 (same dead guard on the candidate path at 26899; insertion at 26938)`

**What the user sees.** Exactly the junk reported. Pressing OK on a blank Compose-bibliography form inserts `[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): .  .]` into the English draft. Because the draft is session-remembered (sess::remember at 25206), it comes back on every launch and the translator cannot tell where it came from.

**Cause.** The guard is `if (entry == ".") return;`. allcore::composeBibliographyEntry (core/src/tibexport.cpp:172) builds `out += ".  " + f.english_title;` before appending the trailing `"."`, so an all-empty record renders as ".  ." — three characters, never equal to ".". The guard has therefore never matched a single empty compose. The identical dead guard sits on the "Save as candidate (pending GMR approval)" handler at 26899, which appends to data/candidate_bib.json — the queue that feeds tools/build_pending_review.py — so a content-free entry can be queued for GMR review.

**Smallest fix.** Compare against the empty render rather than a literal, at both 26899 and 26936: `if (entry == QString::fromStdString(allcore::composeBibliographyEntry({}))) return;` (and the Sanskrit branch against composeSanskritBibEntry({})). That cannot drift when the house template changes. Insert-into-draft should also say why nothing was inserted rather than returning silently.


### [MEDIUM] The selftest written to pin the dataRoot_ fix overwrites dataRoot_ before testing it, so deleting the fix leaves the battery green

**Where.** `app/main.cpp:25750-25760 (HEAD: 25681-25690); the line it is meant to protect is app/main.cpp:25011`

**What the user sees.** Nothing, until the regression returns. The guard that was added on 2026-09-09 to stop DraftPane sidecar writes from silently becoming no-ops again cannot detect that regression, so the next refactor can reintroduce it and every gate will still report PASS. The user then loses draft revision counts, Properties statistics and version history with no test firing.

**Cause.** The block saves dataRoot_ into keepRoot at 25751 and immediately replaces it with a temp path at 25752, then asserts that saveDraft writes a sidecar under THAT temp path. What it proves is 'saveDraft writes a sidecar when given a data root' — a property of docprops, not of the constructor. It never asserts that the constructor wired dataRoot_ at all. Comment out line 25011 (`dataRoot_ = root;`) and this check still passes, because the value it tests is the one the test itself assigned.

**Smallest fix.** Add one line immediately before 25751, while dataRoot_ still holds what the constructor set: `check(!dataRoot_.isEmpty() && dataRoot_ == root_, "the constructor wires dataRoot_ (F0) — sidecar writes are real, not silent no-ops");`. That is the assertion the temp-root block cannot make, and it fails the moment line 25011 is removed.


### [MEDIUM] report_ (BOTTOM-RIGHT) is never given any content — it is a blank, unlabelled box on first run

**Where.** `app/main.cpp:25360-25372`

**What the user sees.** A blank cream panel filling the bottom-right quarter of the pane, with no title, no border label, no tooltip and no text. Nothing on screen names it: the pane banner at app/main.cpp:24998 mentions the source, the clauses and the concordance, and never this box. The user reasonably concludes it is broken.

**Cause.** CONFIRMED — the parent's suspicion is right. report_ is constructed at app/main.cpp:25360, given setOpenLinks(false) and an anchorClicked handler, and added to the layout at 25372. No setHtml, setPlainText, setPlaceholderText, setToolTip or setAccessibleName is ever called on it at construction. It stays empty until one of the ~20 ribbon actions writes to it (first possible write is app/main.cpp:26144).

**Smallest fix.** Already correct in the working tree; commit it. If you want it undestroyable, prefer `report_->setPlaceholderText(...)` (QTextBrowser inherits QTextEdit's placeholder, shown only while the document is empty) over setHtml, so the idle text cannot be lost and returns if the box is ever cleared.


### [MEDIUM] anchors_->clear() blanks the Evidence Ribbon at the exact moment "Load source" is pressed

**Where.** `app/main.cpp:25748 (inside load(), app/main.cpp:25724)`

**What the user sees.** On launch the top-right reads "The Evidence Ribbon — load a source, then click or arrow into any clause…". The user follows step one of that instruction, presses Load source, and the panel goes completely blank — its own step two is erased before it can be acted on. From then on the top-right is an empty box with nothing explaining it, which is what the user is reporting.

**Cause.** CONFIRMED — the parent's suspicion is right. load() ends with `clauseView_->setHtml(...)` then `anchors_->clear(); lastClause_ = -1;` (app/main.cpp:25747-25749). clear() destroys the constructor placeholder set at app/main.cpp:25196-25200. Nothing repopulates it automatically: showAnchors is only reached by clicking a clause link (app/main.cpp:25385) or by the 350 ms evTimer_ (app/main.cpp:25073-25090), and that timer only starts on source_'s cursorPositionChanged — which does not fire as a result of pressing the button, so after a load the ribbon stays blank until the user happens to click something.

**Smallest fix.** Already correct in the working tree; commit it. Same hardening as above applies: setting the idle text via setPlaceholderText in the constructor would make clear() do the right thing by itself.


### [MEDIUM] clauseView_'s instructions are destroyed by an empty Load source and replaced with an unexplained "no clauses"

**Where.** `app/main.cpp:25747`

**What the user sees.** The middle column's first-run text is the pane's only on-screen instruction: "The source appears here as numbered, clickable clauses once you press Load source…". If the user presses Load source before pasting anything — and the button next to it is labelled "Start here." (app/main.cpp:25048) — that instruction is replaced by a bare italic "no clauses", with no colour, no explanation of why, and no way back. The pane now has zero instructions on screen.

**Cause.** `clauseView_->setHtml(h.isEmpty() ? "<i>no clauses</i>" : h);` overwrites the constructor placeholder (app/main.cpp:25187-25192) whenever the built overlay produces no clauses — which is the case for an empty source box, and also for a source the tokenizer cannot split. "no clauses" does not distinguish "you have not pasted anything" from "this text did not parse".

**Smallest fix.** Make the empty branch re-state the instruction instead of erasing it, and name the cause: `clauseView_->setHtml(h.isEmpty() ? (source_->toPlainText().trimmed().isEmpty() ? <the constructor placeholder string> : "<i style='color:#6F6F6F'>Nothing here parsed as Tibetan clauses — check that the source box holds ACIP or Wylie.</i>") : h);`. Hoist the constructor's placeholder into a small static helper (the same pattern the working tree already introduced for anchors_ and report_) so the two copies cannot drift.


### [MEDIUM] "New draft" leaves the previous draft's verdict standing in the bottom-right

**Where.** `app/main.cpp:25486-25494 (newDraft), and app/main.cpp:25504-25512 (revertDraft)`

**What the user sees.** After File ▸ New draft the English box is empty with its placeholder back and the chip under it reads "New draft.", but the bottom-right still displays the terminology report computed from the draft that was just discarded — e.g. a green "✓ every established term is rendered" about text that no longer exists on screen.

**Cause.** newDraft() clears draft_, draftPath_, savedDigest_, editTimer_ and termLive_ (app/main.cpp:25488-25492) but never touches report_. The terminology report is a function of source_ AND draft_ (check() at app/main.cpp:27005-27067 passes both into allcore::checkTerminology), so clearing one of its two inputs invalidates it. revertDraft() (app/main.cpp:25508) has the same gap.

**Smallest fix.** One line in newDraft(), after draft_->clear(): reset the report to its idle text — `report_->setHtml(reportIdleHtml());` once the working-tree helper is committed (before then, a literal "<i>Nothing has been run against this draft yet.</i>"). Add the same line to revertDraft() after the setPlainText.


### [MEDIUM] The bottom-right quadrant ships blank and unlabeled, and nothing in the pane says what fills it

**Where.** `app/main.cpp:25360`

**What the user sees.** A quarter of the workspace is empty white from launch and stays empty through the entire walked path — paste, Load source, click a clause, type a draft. It is the second of the two blank panes in the report. There is no title, no placeholder, no hint; the user cannot tell whether it is broken, not yet implemented, or waiting for something.

**Cause.** report_ is constructed at 25360-25361 with setOpenLinks(false) and an anchorClicked connection, and never given initial content — unlike clauseView_ (25187) and anchors_ (25196), which both ship with placeholder instructions. It is also the only output surface with no heading widget: none of the five boxes has a QLabel title, so the only naming anywhere is the banner sentence at 25015-25018 and the placeholder text inside the boxes themselves. Ten different commands write into report_ (outline, structural units, verse, quotations, apparatus search, style/export status, terminology, AI back-check), and none of them is discoverable from the blank box.

**Smallest fix.** Give report_ an initial setHtml immediately after 25361, in the same voice as the other two placeholders, naming the panel and listing what fills it — e.g. "Reports appear here: Check terminology, Detect quotations, Extract outline, Verse meter, and the apparatus search below the draft."


### [MEDIUM] The advertised "evidence follows the cursor, zero clicks" never fires on the path a new user actually takes

**Where.** `app/main.cpp:25091`

**What the user sees.** The code promises evidence that appears on its own as you move through the source. In practice, after Load source the Evidence Ribbon stays blank and no amount of reading, scrolling or clicking in the clause list wakes it. It only ever fires if the user happens to click back into the source box afterwards — an action nothing suggests, and the least likely next move once the clause list has appeared beside it.

**Cause.** The only trigger is source_'s cursorPositionChanged (25091-25092). load() does not move the source cursor, so pressing Load source raises no event. Worse, the events that DO occur during paste are discarded: the timer callback returns immediately when clauses_ is empty (25074), which is the state throughout paste and up until load() finishes. And clauseView_ is a read-only QTextBrowser with no cursor signal wired, so arrowing through clauses raises nothing either. The net effect is that between the last paste keystroke and a stray click back into the source box, the feature is dead.

**Smallest fix.** Fire the same resolution once at the end of load(): after clauses_ is populated, run the evTimer_ body immediately (or simply `showAnchors(0)`), so loading a source produces evidence without requiring a click the UI never asks for. This is the same one-line fix as the first finding.


### [MEDIUM] Load source reports nothing — no clause count, no next step — and its failure case is an undiagnosed dead end

**Where.** `app/main.cpp:25747`

**What the user sees.** Pressing the pane's primary button produces no confirmation of any kind: no "14 clauses", no "now click a clause", no status line change. If the parse yields nothing the user gets the bare italic phrase "no clauses" in the middle column and nothing else — no statement of what was expected, what was found, or what to try. A user who pasted English prose, Unicode Tibetan, or a file with unusual punctuation is stopped dead at step 2 with two words of diagnosis.

**Cause.** load() writes clauseView_ and nothing else; the pane's one status surface (termLive_, 25215) is never touched by it, and report_ is left blank. The information needed for a real diagnosis is already computed and then thrown away: docIsWylie_ is set at 25726 from allcore::looksLikeWylie and is used only for token conversion at 25722 — it is never surfaced, even though "this looks like Wylie, not ACIP" is the single most useful thing to tell a user staring at "no clauses".

**Smallest fix.** At the end of load(), set termLive_ to the count and the next step ("14 clauses — click one for its anchors"). In the empty branch at 25747, say what was read and what was expected, using the docIsWylie_ value already in hand.


### [MEDIUM] The terminology check the banner names has no control anywhere near the draft — it exists only on the ribbon

**Where.** `app/main.cpp:25301`

**What the user sees.** The banner tells the user "The terminology check matches your draft against Geshe Michael Roach's equivalents", and the live status line under the draft ends with "— press Check terminology for the detail". But there is no such button in the bottom-left column. Under the draft box the user finds, in order: a grey status line, an apparatus search field, and a greyed-out AI button. The named final step of the workflow is off in a ribbon group (EVIDENCE) at the top of the window, and neither prompt says where to look.

**Cause.** checkBtn is added to gEvid on the ribbon at 25301-25305 and nothing is added to draftCol. The two places that instruct the user to press it (25016-25018, 25297) name it but do not locate it. Contrast the source column, which got exactly this treatment right: startBtn duplicates the ribbon's Load source directly under the source box (25031-25042) with the explicit hint "Start here" (25049-25050) and a tooltip saying it is the same action as the ribbon's.

**Smallest fix.** Mirror the startBtn pattern: add a second "Check terminology" button to draftCol next to aiBtn_, wired to the same `check()` slot, with the tooltip noting it is the same action as EVIDENCE on the ribbon.


### [MEDIUM] Every File-menu status message on this pane erases itself about 1.5 seconds after it appears

**Where.** `app/main.cpp:25222`

**What the user sees.** Press New draft and "New draft." appears under the draft box, then silently disappears a second and a half later. Same for "Reverted to the saved <file>" and, when the house-style-on-save preference is on, for the save receipt and for "NOT SAVED — <path>". The status line the code itself calls "the pane's one always-visible status surface" cannot hold a message long enough to be read and acted on, and in the NOT-SAVED case it later displays a cheerful terminology verdict where a failure notice used to be.

**Cause.** termLive_ is shared between two owners that do not know about each other. Each of these operations mutates the draft document and THEN writes its message: newDraft() clears the draft at 25488 and sets the text at 25492; revertDraft() setPlainText at 25508 and sets the text at 25510; saveDraft() runs applyHouseStyleSpacing on the document at 25438 before writing its verdict at 25441/25444. Every one of those mutations fires QPlainTextEdit::textChanged, which restarts termTimer_ (25222-25223). 1.5 s later the timeout takes ownership of the same label and either clears it (25229-25232, when the draft is under 20 characters — the New-draft case, guaranteed) or overwrites it with the terminology line (25267-25298).

**Smallest fix.** Give the status line an explicit owner check — e.g. stamp a monotonic `statusSeq_` when a File-menu message is written and have the termTimer_ timeout skip the label while a message younger than a few seconds is showing — or give File-menu verdicts their own label so a background timer cannot overwrite a save failure.


### [MEDIUM] The terminology live-guard goes silent below 20 draft characters without saying why

**Where.** `app/main.cpp:25229`

**What the user sees.** A new user types the first few words of their English translation and watches the status line under the draft stay stubbornly empty. Once they cross an invisible threshold it springs to life. During the exact period when a first-time user most needs a sign that the tool is watching their draft, the pane is silent, and there is no indication that a threshold exists.

**Cause.** The timer callback bails out on `src.empty() || dr.size() < 20` and clears the label rather than explaining the condition. The 20-character floor is a sensible throttle but it is invisible, and it shares its silence with a genuinely different situation (no source loaded), so one blank line means two unrelated things.

**Smallest fix.** Distinguish the two cases instead of clearing: when the source is empty, say "load a source to check terminology as you type"; when the draft is merely short, leave whatever was there rather than clearing, or say "checking begins once the draft is under way".


### [MEDIUM] Pressing Return while filling the bibliography form fires a default button that no field asked for

**Where.** `app/main.cpp:26722`

**What the user sees.** Typing a field and pressing Return — the reflex in any form — does not move to the next field and does not simply do nothing. It activates the dialog's default push button. The user gets either an unrequested catalog auto-fill, an unrequested rewrite of the author field, or an insertion into the draft, depending on which plain QPushButton Qt resolved as default. This is the most likely route by which a half-filled form reached the draft repeatedly.

**Cause.** The form contains three plain QPushButtons — fillBtn "Auto-fill from catalog" (26722, added to the form at 26730), hyBtn "Pair-hyphenate author" (26735/26737), and the button box's "Insert into draft" (26808). QPushButtons parented to a QDialog are autoDefault by default, QLineEdit does not consume Return, and the dialog therefore routes Return to whichever of the three is default. None of the three is a safe mid-typing action; two of them silently rewrite the user's fields and one commits to the draft.

**Smallest fix.** Call `setAutoDefault(false)` on fillBtn and hyBtn where they are created, and make the intent explicit with `insertB->setDefault(true)` — combined with the field validation from the second finding, so Return commits only a complete entry.


### [MEDIUM] "Shared apparatus (official tier: published + GMR-approved only)" heads a list that includes unapproved candidates

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:26472-26474, contradicted at 26510-26525`

**What the user sees.** The apparatus search results open with "Shared apparatus (official tier: published + GMR-approved only; insertions carry their citation)" and then, below the published notes and bibliography, list pending candidate notes. The individual candidate rows are correctly marked "⚠ PENDING — not GMR-approved", so the page contradicts itself: the header says the list contains only approved material, the rows say otherwise. A reader who trusts the header and skims the rows inserts unapproved text believing it is published.

**Cause.** The header string is written once at the top of searchNotesBank as if it described the whole result, then a third loop appends candBank_ rows to the same `h`.

**Smallest fix.** Move the parenthetical off the header and onto the published sections, e.g. header "Shared apparatus" and a sub-heading "Published + GMR-approved" before the notes/bibliography loops, with "Pending — not approved" before the candidate loop.


### [MEDIUM] The terminology check's per-term verdict is a whole-draft substring hit, presented as "draft uses" and as flagging disagreement

**Where.** `/Users/adamderickandrade/ALL-translation-tool/core/src/terminology.cpp:52-56 and :41; claims at /Users/adamderickandrade/ALL-translation-tool/app/main.cpp:25015-25018 and 25299-25300`

**What the user sees.** The always-visible banner says "The terminology check matches your draft against Geshe Michael Roach's equivalents"; the button tooltip says "It flags what disagrees". The report then prints a green ● and "draft uses: <gloss>" per term. In fact a term is marked rendered if any gloss alternative of three or more characters appears ANYWHERE in the draft, with no word boundary and no positional correspondence to the clause — so a gloss occurring in an unrelated paragraph, or as a fragment inside a longer word ("art" inside "particle", "one" inside "alone"), scores the term as rendered. And nothing in the check can detect a rendering that disagrees; it only detects one that is absent.

**Cause.** glossMatches does `draft_lower.find(alt) != std::string::npos` with no boundary test, over the whole lowercased draft; glossAlternatives admits any alternative of length ≥3.

**Smallest fix.** Require the characters either side of the hit to be non-alphanumeric before counting a match, and reword the two claims to what is measured: "flags established terms whose equivalent does not appear in the draft" rather than "flags what disagrees", and "appears in the draft" rather than "draft uses".


### [MEDIUM] Geshe Michael Roach's published English is silently truncated mid-sentence in five places, bypassing the house's own honest-elision helper

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:26036, 26063, 26105, 26635, 27184, 27188`

**What the user sees.** Every English quotation in the Evidence Ribbon, the quotation detector and phrase memory stops dead at a fixed character count with no ellipsis and no mark of any kind — so a truncated fragment of his published rendering looks like the whole rendering. Under headings that explicitly attribute it ("the master has translated this clause", "published English:", "Geshe Michael Roach corpus", "84000 rendered a matching passage"), a cut sentence reads as his complete sentence and can be copied into a draft as such.

**Cause.** These six sites call `QString::left(N).toHtmlEscaped()` directly instead of ux::snip/ux::snipStd, the helper the project added for exactly this after Adam's 2026-08-22 screenshot of an 84000 definition cut off mid-word. showConcordance in the same class already uses it correctly.

**Smallest fix.** Replace each `QString::fromStdString(x).left(N).toHtmlEscaped()` with `ux::snipStd(x, N)` — same call site, same cap, adds the visible ellipsis and the word-boundary back-off.


### [MEDIUM] "the master has translated this clause" overstates an FTS containment match

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:26023-26038`

**What the user sees.** A bold heading, "the master has translated this clause", followed by English. The English is the whole corpus segment's English — which may render a great deal more than the clause, and whose correspondence to the clause is nowhere stated. The heading invites the translator to treat that English as his rendering OF THIS CLAUSE and lift it.

**Cause.** `spine_.corpusSearch('"' + q + '"', "", 3)` is an FTS5 phrase query: it returns segments that CONTAIN the clause, not segments equal to it. The code treats a containment hit as an identity hit and prints the container's English.

**Smallest fix.** Reword the heading to what the query proves — "a corpus segment contains this clause" — and label the text below it "the segment's English" so the reader knows the span shown is larger than the clause.


### [MEDIUM] The technical-spelling insert drops the pronunciation's provenance into the English draft

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:25396-25414`

**What the user sees.** Clicking "[+ technical spelling]" in the Evidence Ribbon writes something like "sempa (technical spelling: Sems-dpa')" into the draft. Nothing on the page or in the inserted text says whether that pronunciation spelling came from Geshe Michael Roach's own course cards or was produced by the phonetics engine — yet everything else this pane writes into the draft carries a bracketed provenance marker ("[NOTE: … — reused from …]", "[NOTE — PENDING, not GMR-approved: …]", "[BIBLIOGRAPHY: … as published in …]"). This one is bare, and it goes into a manuscript.

**Cause.** The handler takes `es.front().pronunciation` and never consults `Entry::pronunciation_card_attested`, the flag that exists precisely to separate his card spellings from generated ones. It also takes front() from a multi-entry lookup with no disambiguation.

**Smallest fix.** Insert the pronunciation only when `es.front().pronunciation_card_attested` is true; otherwise insert the technical spelling alone, or mark the generated spelling the way the rest of the app does.


### [MEDIUM] The apparatus, candidate and catalog files are found by a hard-coded relative path that ignores the app's own data root

**Where.** `app/main.cpp:26455-26460`

**What the user sees.** Depending on where the app was launched from, "search the shared apparatus" answers "apparatus banks not found — run tools/extract_mixed_nuts_notes.py and tools/extract_mixed_nuts_bibliography.py" (a developer instruction a translator cannot act on), "Auto-fill from catalog" reports "no catalog record for this number" for every number ever typed, and "Propose footnote" raises a write-failure warning. Meanwhile the Apparatus pane, one tab away, has the very same notes loaded and searchable.

**Cause.** `dataFile()` (26455-26460) resolves to `applicationDirPath() + "/../../../../../data/" + name`, falling back to `QDir::currentPath() + "/data/" + name`. Both assume a specific build-tree layout or a specific working directory. A Dock-launched or installed copy satisfies neither, so every bank load silently opens nothing. `loadCatalogWorks()` (26933-26939) compounds it by setting `worksLoaded_ = true` before the open succeeds, so one miss disables catalog auto-fill for the life of the session.

**Smallest fix.** Make dataFile() use the root the pane was handed: `return (root_.isEmpty() ? QDir::currentPath() : root_) + "/data/" + name;` (26455). Also move `worksLoaded_ = true` in loadCatalogWorks (26935) to after the successful open, so a transient miss does not poison the session.


### [MEDIUM] Four STRUCTURE tools answer confidently about a source that was never pasted

**Where.** `app/main.cpp:26178 (outline), 26225 (structuralUnits), 26284 (verse), 26317 (verseReading)`

**What the user sees.** With an empty source box, "Extract outline (sa bcad)" reports "no sa bcad markers found in this text"; "Structural units" reports "0 syllables · shloka estimate 0.0 · bampo estimate 0.00" and "no explicit bam po markers in this text"; "Verse meter" reports "no dominant meter — reads as prose"; "Verse reading order" reports "no dominant meter — this reads as prose; use the Trainer's reading order instead." Four findings about a text that does not exist, phrased as findings about the text.

**Cause.** All four read `source_->toPlainText()` straight into the engine with no emptiness test (26180-26183, 26227-26230, 26285, 26318-26319) and render the engine's empty result with wording that presupposes a text was examined.

**Smallest fix.** One guard each, at the top of the four functions, in the wording detectQuotes already uses: `if (source_->toPlainText().trimmed().isEmpty()) { report_->setHtml("<i>Paste or load the ACIP source first — this reads the source box.</i>"); return; }`.


### [MEDIUM] Every insertion into the draft happens off-screen, so the control looks like it did nothing — which is how the same bibliography stub landed in the draft repeatedly

**Where.** `app/main.cpp:26927-26930 (and 25413, 26597, 26996, 27005)`

**What the user sees.** They compose a bibliography entry, press "Insert into draft", the dialog closes, and the draft looks unchanged — because the text went in at the draft's last cursor position, which may be scrolled far out of view. So they open the dialog and press Insert again. And again. The result is the junk the user reported: "[BIBLIOGRAPHY — NEW ENTRY, house format (STD-007 / DCC guide): (Co-ne Bla-ma) Grags-pa. .]" repeated down the draft.

**Cause.** All five insertion paths are `draft_->textCursor().insertText(...)` followed by `draft_->setFocus()` and nothing else. QPlainTextEdit does not scroll to the cursor on a programmatic document change; `ensureCursorVisible()` is required and is absent from all five. There is also no status-line confirmation, so the pane offers no second channel by which the user could learn the insert succeeded.

**Smallest fix.** At each of the five sites, take the cursor by value, insert through it, write it back, then reveal it: `QTextCursor c = draft_->textCursor(); c.insertText(ins); draft_->setTextCursor(c); draft_->ensureCursorVisible(); draft_->setFocus();`. Best done once in a shared private helper, which also gives the Protect-Document guard a single home.


### [MEDIUM] "Insert into draft" accepts a bibliography form with no title and emits a malformed stub — the only guard catches a wholly empty form

**Where.** `app/main.cpp:26926`

**What the user sees.** Fill in only Epithets and Author, press Insert, and the draft receives "(Co-ne Bla-ma) Grags-pa.  ." — an entry with a dangling empty title and a stray period. The button gave no hint that a required field was missing; nothing was disabled and nothing was said. This is exactly the string the user found in their draft.

**Cause.** The sole validation is `if (entry == ".") return;` (26926, and the same sentinel at 26889 for the candidate path). composeBibliographyEntry appends `".  " + f.english_title` unconditionally (core/src/tibexport.cpp:182), so the sentinel only matches when every field is blank. One character in any field defeats it, and the entry is inserted as house-format output.

**Smallest fix.** Inside the existing refresh lambda (26862), gate the button that is already captured: `insertB->setEnabled(!au->text().trimmed().isEmpty() && !et->text().trimmed().isEmpty());` and set `insertB->setToolTip("Author and English title are required — an entry without a title is not a house-format entry.")` when disabled. The `(void)insertB;` at 26922 can then go.


### [MEDIUM] "Save as candidate" puts its only confirmation in a pane hidden behind the modal dialog, so a success looks like a no-op and repeat presses append duplicates

**Where.** `app/main.cpp:26911-26914`

**What the user sees.** They press "Save as candidate (pending GMR approval)". The dialog stays open (correctly — it is an ActionRole button) and absolutely nothing visible changes. So they press it again. Each press appends another copy of the entry to candidate_bib.json. They only discover the confirmation, and the duplicates, after closing the dialog.

**Cause.** The success message is written with `report_->setHtml(...)` (26911-26914), and report_ is the bottom-right pane of the Draft workspace, behind the modal QDialog. The dialog has a preview label and a scanLink label (26811-26816) but no status row of its own. The write itself is `arr.append(o)` (26902) with no dedupe against an identical existing entry.

**Smallest fix.** Add a status QLabel to the dialog's form (next to scanLink at 26816) and write the candidate verdict there — `status->setText("Saved as candidate — pending GMR approval (N in the queue).")` — instead of, or as well as, into report_. Skipping the append when an identical `entry` string is already in the array (26902) makes a double press harmless.


### [MEDIUM] "Auto-fill from catalog" does nothing at all on a blank or malformed ACIP number, and reports a miss only as a tooltip on a different field

**Where.** `app/main.cpp:26946 and 26949-26951`

**What the user sees.** Press "Auto-fill from catalog (by ACIP number — review result)" before typing an ACIP number, or after typing one in an unexpected shape, and nothing whatsoever happens: no fields change, no message, no complaint. When the number does parse but the catalog has no record, the only notice is a tooltip quietly attached to the ACIP field — discoverable solely by hovering the field they just typed into.

**Cause.** `autoFillBib` returns silently when the regex `^([A-Za-z]+)0*(\d+)` fails to match (26944-26946), and on a catalog miss it takes the else branch `ac->setToolTip("no catalog record for this number")` (26949-26951) with no visible output. The Epithets field is passed in and explicitly discarded (`(void)ep;` at 26954), so that field is never filled regardless.

**Smallest fix.** Replace the bare return at 26946 and the tooltip at 26950 with writes to a visible label in the dialog: "Type an ACIP number first (for example S00184 or TD04158)." and "No catalog record for <number> — fill the fields by hand." Keep the tooltip if you like, but it must not be the only channel.


### [MEDIUM] The apparatus search field fires only on Return, has no button and no tooltip, and an empty Return is a silent no-op

**Where.** `app/main.cpp:25312-25321 and 26465`

**What the user sees.** A wide field reading "search the shared apparatus: footnotes + bibliography (GMR: reuse released work)…" sits under the draft. Typing into it does nothing. There is no adjacent Search button. Nothing on screen says Return is the trigger. Pressing Return on an empty field also does nothing, so a user who presses Return to "see what's in there" gets no response and reasonably concludes the field is broken.

**Cause.** The only connection is `QObject::connect(notesSearch_, &QLineEdit::returnPressed, [this]{ searchNotesBank(); });` (25320-25321) — no textChanged live search, no companion button, and `notesSearch_->setToolTip(...)` is never called (25312-25319 sets only the placeholder). searchNotesBank then returns immediately on an empty query (26465) without touching report_.

**Smallest fix.** Add the missing affordance to the placeholder at 25314 — "…press Return to search" — plus `notesSearch_->setToolTip("Searches the published footnote and bibliography banks; results appear in the pane on the right, where you can click to insert one.")`. At 26465, replace the silent return with `report_->setHtml(reportIdleHtml()); return;` so an empty Return restores the pane's own explanation rather than doing nothing.


### [MEDIUM] "AI back-check" samples the API key once at construction, so the remedy its own tooltip gives cannot take effect without a relaunch — and the tooltip is the only place the reason appears

**Where.** `app/main.cpp:25347-25353`

**What the user sees.** The button is greyed out with no visible reason anywhere on the pane. If the user finds the tooltip, it says "No Anthropic API key (env or the Application Support key file)" — without naming the file's path. They create the key file as instructed, come back, and the button is still dead. Nothing tells them a relaunch is required.

**Cause.** `if (anthropicKey().isEmpty()) { aiBtn_->setEnabled(false); … }` runs once in the constructor (25348-25353). Nothing re-evaluates it afterwards — no showEvent hook, no timer, no settings-changed signal — even though anthropicKey() (2946-2964) reads a file that can appear at any moment during the session. Separately, the explanation lives only in a tooltip on a disabled control, and no tooltip at all is set when a key IS present.

**Smallest fix.** Re-evaluate on show: override showEvent (or connect the pane's showEvent hook) to run `aiBtn_->setEnabled(!anthropicKey().isEmpty());` so a key file created mid-session takes effect on the next visit to the pane. And copy AnalysisPane's wording into the tooltip at 25350 so it names the path and says "relaunch" — or better, surface it on termLive_, the pane's one always-visible status line.


### [MEDIUM] After Load source on an empty or untokenizable source, the Evidence Ribbon tells the user to click a clause that the middle column says does not exist

**Where.** `app/main.cpp:25816 and 25821`

**What the user sees.** Press "Load source" with the source box empty (or with text the tokenizer yields nothing from). The middle column says "no clauses". The right-hand Evidence Ribbon says, in bold, "Click any clause in the middle column." Two panes giving contradictory instructions, and neither says the actual problem: there is no source.

**Cause.** `load()` (25794) has no emptiness guard. It renders "<i>no clauses</i>" when the clause list came back empty (25816) and then unconditionally advances the ribbon to its source-loaded state (25821, `anchorsIdleHtml(true)`), which is written on the assumption that clauses exist. splitClauses drops empty spans (core/src/reader.cpp:102-104), so zero tokens really does yield zero clauses.

**Smallest fix.** Guard load() at 25795: if the source box is empty, set clauseView_ and anchors_ back to their idle text and say why — "Paste or load ACIP into the source box on the left, then press Load source." Otherwise pass the real state through: `anchors_->setHtml(anchorsIdleHtml(!clauses_.empty()));` at 25821, and give clauseView_'s empty case at 25816 a diagnosis rather than "no clauses".


### [MEDIUM] House style check reports "No mechanical findings" without saying that rules were switched off in Preferences

**Where.** `app/main.cpp:24983`

**What the user sees.** A translator who once turned off a few style rules in Preferences runs the House style check weeks later. The dialog title says "House style check — 0 finding(s) (Diamond Cutter Classics guide, mechanical rules only)" and the list says "No mechanical findings — the subtler rules … remain yours to check". They read that as the guide's mechanical rules having passed. Some of them never ran.

**Cause.** Every scan in styleCheck is wrapped in `editops::styleRuleOn("…")` (24858, 24862, 24874, 24880, 24884, 24887, 24893, 24896, 24899, 24902, 24927), which reads a per-rule QSettings flag. The empty-result message at 24983-24986 and the finding count in the title at 24967-24970 are both blind to how many rules were actually enabled.

**Smallest fix.** Count the enabled rules alongside the findings and state it, e.g. append to the title at 24969 and to the empty message at 24983: "— N of 11 mechanical rules are enabled (Preferences ▸ Style)". A zero-finding report must say what it looked for.


### [MEDIUM] The three `g_harnessRun = false` windows leak whatever the battery touched, and recordRecentScan has no guard at all

**Where.** `app/main.cpp:7874 (also 7909 and 46838); app/main.cpp:34028`

**What the user sees.** File > Open Recent lists /tmp files the translator never opened, and the Input pane's recent-scans list offers a folder that only ever existed as a test probe.

**Cause.** Direct answer to the question posed: test data does NOT reach sess/draft/english. sess::put (2979) returns early under g_harnessRun, the flag is set for all nine GUI driver flags before any pane is constructed, and the 1500 ms debounce bound to draft_ (25206) fires during the intervening batteries with the guard still up, so the sweep's prime of every editable QPlainTextEdit (46208) and DraftPane::selfTest's seven draft_->setPlainText calls never persist. The leak is elsewhere: three selftest blocks clear the global guard (7875, 7910, 46839) so they can exercise a production path, then restore only the handful of keys they explicitly named. Anything else the production path writes on the way through is kept. Block 7874 lifts the guard, calls restoreSession() -> openFile(), which reaches the Recent-Files write at 9123 now unguarded, and the block restores only overlay/lastFile (7895). Separately, InputPane::recordRecentScan (34028) has no g_harnessRun check of any kind, so the Input probe's folder is recorded even with the guard up.

**Smallest fix.** Two parts, both small. Give recordRecentScan the `if (g_harnessRun) return;` its siblings already have. And stop clearing the global: give OverlayPane::restoreSession and InputPane::restoreSession a `bool force = false` parameter that the probe passes, so the guard is never down process-wide. If a global lift is genuinely unavoidable, snapshot and restore the entire QSettings tree around the window rather than a named subset.


### [MEDIUM] The selftest erases the translator's Features and workflow choices instead of restoring them

**Where.** `app/main.cpp:46406 (writes at 46388)`

**What the user sees.** Every pane they had switched off under Features is switched back on after a battery run, and a chosen workflow is undone. If the process dies mid-block, the stored state is "every pane hidden".

**Cause.** The Features block writes features/pane/<title> = false for every pane in the build (46388) without ever reading the prior value, then at 46406 calls st.remove() on those keys plus features/group/<group>. Removal means "unset", and unset defaults to on — so a deliberate hidden pane is silently turned back on. These are plain QSettings writes outside the sess:: namespace, so the g_harnessRun guard does not apply to them at all.

**Smallest fix.** Capture the prior QVariant for each key before the first write and put it back the way the 7909 block already does: set it if valid, remove it only if it was absent.


### [MEDIUM] The Draft restores its source text but not the work derived from it, and report_ is the one panel that never says anything

**Where.** `app/main.cpp:25029 (report_ constructed at 25356)`

**What the user sees.** On launch the source box is full of yesterday's ACIP, but the clause list still reads "press Load source", the Evidence Ribbon still reads "load a source", and the bottom-right panel is completely blank. The pane looks as if it lost the work — which is precisely the reported symptom.

**Cause.** sess::remember(source_, "draft/source") at 25029 puts the remembered ACIP straight into the widget during construction, but load() is only ever invoked from the two Load-source buttons (the only two connects to it in the whole pane), so clauses_ stays empty and both top placeholders stand unchanged over a full source box. Separately, report_ at 25356 is the only one of the pane's three QTextBrowsers built without an initial setHtml — clauseView_ and anchors_ each get idle guidance text, report_ gets only setOpenLinks(false) and the anchorClicked connect — so the bottom-right pane is blank from launch with nothing to explain it.

**Smallest fix.** At the end of the constructor add `QTimer::singleShot(0, this, [this]{ if (!source_->toPlainText().trimmed().isEmpty()) load(); });` so a restored source comes back as clauses rather than as an apparently-ignored box. And give report_ one idle line at 25356 the way its two neighbours have one.


### [MEDIUM] The properties sidecar is keyed on the file's bare name, so same-named drafts in different folders share one history

**Where.** `app/main.cpp:4065`

**What the user sees.** Two drafts both called draft.txt in different folders share one revision number, one accumulated editing time and one "last saved by". Saving either bumps the other's recorded history, and File > Properties reports the wrong figures for both.

**Cause.** docprops::sidecarPath returns `dataRoot + "/library/properties/" + QFileInfo(docPath).completeBaseName() + ".json"` — the path is discarded entirely. The version store sitting next to it keys on docKeyFor(absoluteFilePath, dataRoot), which is basename plus a hash of the path, so the two persistence stores disagree about what counts as one document. This is also the mechanism by which the stray selftest sidecar from finding 1 could be inherited by a real file of the same name.

**Smallest fix.** Make sidecarPath use the same identity as the version store: `return dataRoot + "/library/properties/" + versionDocKey(docPath, dataRoot) + ".json";`. Existing sidecars need a one-time migration or will simply start over at revision 1.


### [LOW] Running Help ▸ Self-test writes a phantom document's sidecar and version history into the user's real data root and never cleans them up

**Where.** `app/main.cpp:25734 and 25744 (the two saves against the real dataRoot_); cleanup at 25766 removes only the .txt. Precedent for the correct cleanup is in the same file at app/main.cpp:8380 and 8426`

**What the user sees.** After running the self-test, the user's data folder permanently contains library/properties/all_selftest_draft_save.json and library/versions/all_selftest_draft_save~<hash>/ holding compressed copies of the test string "Consider sound." — stamped with the user's Team name as author. They are visible through Settings ▸ Locations ▸ Document properties ▸ Show in Finder. Each further self-test run bumps the phantom document's revision counter and adds more versions. The user sees a document they never wrote, with a save history attributed to them.

**Cause.** The F0 fix made these writes real. saveDraftAs() at 25734 and saveDraft() at 25744 run with the genuine dataRoot_ (the temp-root swap only starts at 25752), so each one calls docprops::noteSave(sidecarPath(dataRoot_, ...)) at 25451 and docprops::noteVersion(dataRoot_, ...) at 25454 against the live library. The block's teardown at 25766 is `QFile::remove(outP);` — the draft file only. Before F0 both writes were no-ops, so the missing teardown cost nothing; it costs something now.

**Smallest fix.** Extend the teardown at app/main.cpp:25766 to match the Overlay precedent: `QFile::remove(outP); QFile::remove(docprops::sidecarPath(dataRoot_, outP)); QDir(docprops::versionsDir(dataRoot_, outP)).removeRecursively();`.


### [LOW] docs/ANALYSIS_SUITE_PLAN.md still asserts that DraftPane::dataRoot_ is never assigned — stale since 2026-09-09

**Where.** `docs/ANALYSIS_SUITE_PLAN.md:117, :308, :429, :2190 (the claim); app/main.cpp:25011 (the fix, present in HEAD)`

**What the user sees.** Not user-facing, but it sends the next engineer to fix a bug that is already fixed, and — worse — it describes the two live defects above as consequences that "would" follow, which invites the reader to believe they were closed along with the assignment. They were not.

**Cause.** The F0 fix landed as a constructor-body assignment rather than the initialiser-list entry the plan specified, and the plan's Risks section, verification note and task table were never updated. The plan also records line anchors (23759, 23770, 21686, 22069, 22132) from a much older revision of main.cpp; none of them resolve to DraftPane at HEAD.

**Smallest fix.** Mark the dataRoot_ item done in docs/ANALYSIS_SUITE_PLAN.md:117/:2190 with the HEAD anchor (app/main.cpp:25011), and replace the retired risk text with the two that are still open: the versions-directory key mismatch on Rename and the absent sidecar handling on Move.


### [LOW] DraftPane carries two members holding the identical data root, and no setDataRoot — a trap for the next change

**Where.** `app/main.cpp:25010 and 27247 (root_), app/main.cpp:25011 and 27259 (dataRoot_)`

**What the user sees.** Nothing today. The risk is that the two diverge: a future setDataRoot, or a future initialiser-list cleanup applied to only one of them, would leave the file-loading paths and the sidecar/version paths pointing at different folders, and nothing would report the mismatch.

**Cause.** The F0 fix added dataRoot_ alongside the pre-existing root_ instead of reusing it. root_ serves scan URLs and data-file loads (app/main.cpp:26866, 26957, 27198-27202); dataRoot_ serves docprops only (25451, 25454, 25466, 25536, 25547, 25550). They are always equal because both are initialised from the same constructor argument, but nothing enforces that. DraftPane also has no setDataRoot (only ManuscriptPane does, app/main.cpp:37455-ish, called from main()), which is currently harmless because Settings ▸ Locations states "Changes to the data folder take effect at the next start" (app/main.cpp:40823).

**Smallest fix.** Delete the dataRoot_ member (app/main.cpp:27259) and line 25011, and use root_ at the six docprops call sites. If the duplicate is kept for symmetry with OverlayPane/ManuscriptPane, pin it instead with the selftest line proposed above (`dataRoot_ == root_`), which makes divergence a FAIL rather than a silent wrong folder.


### [LOW] The source box's placeholder advertises ACIP only, though the pane auto-detects Wylie

**Where.** `app/main.cpp:25028`

**What the user sees.** An empty top-left box reading "Source ACIP…". A translator working in Wylie has no on-screen reason to believe the pane will accept their text, and may go looking for a converter first.

**Cause.** `source_->setPlaceholderText("Source ACIP…")` names one of the two transliterations the pane handles. load() opens by calling `allcore::looksLikeWylie(source_->toPlainText())` and stores the answer in docIsWylie_, which tokEwts then uses to decide whether to run acipToEwts (app/main.cpp:25721-25722).

**Smallest fix.** `source_->setPlaceholderText("Paste the source here — ACIP or Wylie, detected automatically…");`


### [LOW] The terminology chip is blank on first run and silently blanks again for drafts under 20 characters

**Where.** `app/main.cpp:25215-25218 (constructed empty) and app/main.cpp:25229-25232 (cleared)`

**What the user sees.** A one-line gap between the draft box and the apparatus search that shows nothing at all on first run. Later, if the source box is empty or the draft drops below 20 characters, an existing chip disappears with no explanation — the user cannot tell "not checked" from "checked, nothing to report".

**Cause.** termLive_ is created with no initial text (app/main.cpp:25215-25218), and the debounced handler bails with `if (src.empty() || dr.size() < 20) { termLive_->clear(); return; }` (app/main.cpp:25229-25232). Both states render as the same nothing, and the threshold is invisible to the user.

**Smallest fix.** Give it a resting sentence instead of nothing — set `termLive_->setText("Terminology check runs as you type, once there is a source and a few lines of English.")` at construction, and replace the clear() at app/main.cpp:25230 with that same sentence so the reason is always on screen.


### [LOW] Evidence inserted from the anchors pane lands at an off-screen point in the draft with no confirmation

**Where.** `app/main.cpp:25411`

**What the user sees.** The user is reading the Evidence Ribbon, clicks "[+ technical spelling]", and text is injected into the draft at wherever that box's cursor last sat. On a draft the user has not yet clicked into, that is character position 0 — the text goes to the very top, above everything they have written, and the draft is not scrolled to show it. The same is true of the bibliography insertion, which is why repeated attempts stack at the top of the box.

**Cause.** Both insertion sites use `draft_->textCursor().insertText(...)` followed by `draft_->setFocus()`, with no ensureCursorVisible() and no check that the draft has a meaningful cursor position. setFocus moves the keyboard focus but does not scroll the viewport to the insertion point.

**Smallest fix.** Add `draft_->ensureCursorVisible();` after each `draft_->setFocus();` so the user sees where the text landed — the pattern already used correctly in showStyleCheck at 24993-24995.


### [LOW] The AI back-check sits disabled under the draft with its reason hidden in a tooltip

**Where.** `app/main.cpp:25352`

**What the user sees.** The most prominent-looking control in the bottom-left column is permanently greyed out for any user without an API key. The label still reads "AI back-check (API, labeled AI)", which gives no clue why it cannot be pressed; the explanation is hover-only, and a user who never hovers never learns that a key is what is missing.

**Cause.** The disabled state is communicated solely through setToolTip at 25354-25356. Nothing is written to the visible label or to the status line, and a disabled QPushButton on macOS does not reliably show tooltips at all.

**Smallest fix.** Put the reason in the visible label when the key is absent — `aiBtn_->setText("AI back-check — needs an Anthropic API key")` alongside the existing tooltip — so the blocker is readable without hovering.


### [LOW] Pressing Return in an empty apparatus search box does nothing at all

**Where.** `app/main.cpp:26392`

**What the user sees.** A user who notices the apparatus field under the draft and presses Return to see what it offers gets complete silence — no results, no prompt, no indication the field is even wired up. The field is the only affordance for a bank of 1,569 published notes plus the bibliography, and its empty state teaches the user it is inert.

**Cause.** searchNotesBank returns early on an empty query with no feedback (26392), after having already loaded the banks. Every other no-result path in the same function does speak — 26393-26397 for missing bank files, 26453-26454 for no matches — so the empty-query case is the single silent branch.

**Smallest fix.** Replace the bare `return` at 26392 with a line into report_ saying what the field searches and how many notes and entries are loaded — which doubles as the first thing that ever appears in the blank bottom-right panel.


### [LOW] The AI paragraph is the one artefact in the pane with no provenance stamp

**Where.** `/Users/adamderickandrade/ALL-translation-tool/app/main.cpp:27053-27058`

**What the user sees.** The coverage diff is labelled "AI" but records no model, no date, and no prompt version. Copied out of the pane — which is the point of a back-check — it becomes an unattributable paragraph of English about the translation, in a project whose fourth rule is that generated data is tagged with what produced it.

**Cause.** The finished handler renders only aiAccum_. The serving model is available and ignored: the readyRead loop parses only text deltas and message_stop, never the `message_start` event that carries the model id — which matters here because the request enables server-side fallbacks, so the model that answers need not be the one asked for.

**Smallest fix.** Capture the model id from the `message_start` SSE event and append it plus the local timestamp to the banner: "model output (<model>, <date>), labeled AI".


### [LOW] "Pair-hyphenate author" overwrites the field in place with no tooltip and no way back

**Where.** `app/main.cpp:26785-26787 and 26874-26878`

**What the user sees.** A button labelled "Pair-hyphenate author (STD-002 helper — review result)" sits under the form. Pressing it rewrites the Author field. If the result is wrong, Cmd-Z inside the field does not restore what they typed. Nothing explains what pair-hyphenation does before they press it.

**Cause.** The button is created at 26785-26787 with no setToolTip — the sibling fillBtn at 26772-26779 has a four-line one. The handler at 26874-26878 calls `au->setText(...)`, and QLineEdit::setText clears that field's undo history.

**Smallest fix.** Add the missing tooltip at 26786, in fillBtn's voice: "Rewrites the Author field in house technical spelling — Grags pa bshad sgrub becomes Grags-pa bshad-sgrub. Review the result; this replaces what you typed." If cheap, stash the pre-click text and offer an undo, but the tooltip is the required part.


### [LOW] Two pairs of DraftPane ribbon buttons share an icon, and are indistinguishable when ribbon labels are off

**Where.** `app/main.cpp:25307 and 25325; 25127 and 25333`

**What the user sees.** With ribbon labels turned off (Preferences, ui/ribbonLabels), WORKBENCH's "Send to Manuscript →" and PUBLISH's "Export draft (RTF)" are the same green arrow, and STRUCTURE's "Verse reading order" and PUBLISH's "Compose bibliography entry…" are the same blue book. Four actions, two icons, and one of the pair is the destructive Manuscript overwrite.

**Cause.** `gWork->addBig(toMssBtn, "out")` (25307) and `gPub->addBig(rtfBtn, "out")` (25325) request the same glyph; likewise `gStruct->addBig(verseReadBtn, "book")` (25127) and `gPub->addBig(composeBib, "book")` (25333). miniIcon paints purely from the kind string, so identical kinds give identical pixmaps.

**Smallest fix.** Give each of the four a distinct kind from miniIcon's existing vocabulary — for example "out" stays with Export RTF, "stack" or "shelf" for Send to Manuscript; "book" stays with Compose bibliography, "strip" or "pecha" for Verse reading order (25127, 25307).


---

## Status, 2026-09-12

**All 18 HIGH findings are closed.** Roughly 22 of the 31 MEDIUM are closed;
the LOW tier is untouched. Twenty-two commits, each with its own gates. The
suite stayed green throughout: 114/114 on the core batteries, app selftest ALL
PASS, constitution holding.

Three findings were **examined and deliberately not fixed**, which is recorded
rather than left as a silent gap:

- **The properties sidecar's key** — two same-named files in different folders
  share one revision counter. Re-keying needs an adoption step that cannot live
  where it naturally goes, and would start orphaning properties on every move
  until every pane carried history. `docs/SIDECAR_KEYING_DECISION.md` carries
  the full reasoning and the design a later attempt needs. The check's urgent
  finding — the Manuscript carrying no history at all — was fixed the same day.
- **`C01_CAMPAIGN.md`'s stale header table** — a campaign ledger is an
  append-only work record, and rewriting one falsifies the history it exists to
  hold.
- **The red-flag licence list** — a standing prohibition, not a task. Marked
  `[—]` so it stops appearing as work to be done.

### What the audit was actually about

Almost none of these were crashes. The recurring defect in this pane is **a
control that cannot act failing to say why before it is pressed, rather than
after** — and its sibling, **a result reported over nothing at all**. A check
that examined nothing printed a clean zero. Four structure tools answered
confidently about an empty box. An insertion landed off-screen so a working
control looked broken. A save reported to a pane behind its own modal.

The one that mattered most was not in the original 60. Probing the terminology
matcher rather than reading it showed `"mind"` matching `"reminded"`, `"art"`
matching `"departure"`, `"one"` matching `"honest"` — a bare substring search
under the pane's headline verdict, reporting terms as *rendered* when the
equivalent never appeared as a word. That number is what a translator trusts,
and it was flattering their draft.

### Three lessons about the gates themselves

1. **A gate that only checks its own block agrees with the bug.** The first
   library-pollution gate passed cleanly while five other panes were still
   writing phantom records.
2. **A gate can encode the defect as a requirement.** The ladder gate asserted
   that the Manuscript holds *exactly* the draft that was sent — which is the
   data loss, written down as a specification.
3. **A flaky gate is worse than a missing one**, because it teaches you to
   discount a red result. Five Overlay gates went red from debris left by an
   interrupted run; the same binary passed them on a re-run.

---

## Dispositions, 2026-09-12

Every finding in this audit is now either fixed with a mutation-proved gate, or
recorded below with the reason it was not. Nothing is marked closed because
code near it was edited — that error is what produced the verification pass in
the first place.

### HIGH and MEDIUM — all closed

| finding | how |
|---|---|
| the `draft uses:` claim | now "found in your draft"; the register spread and the header corrected too; the old gate REQUIRED the defect and was replaced |
| `recordRecentScan` guard did not hold | the guard was there; the caller had lowered the global two frames up |
| three `g_harnessRun = false` windows leaking | `ProbeSessionKeys` opens the session keys alone; the global never drops. Proved from Adam's live plist, which held three temp paths |
| Return fires a button in the bibliography form | `ReturnMovesFocus` on every field. The recorded fix was inert — `QDialogButtonBox` re-asserts the default on show, proved by `tools/probes/return_default_probe.cpp` |
| the middle column replaced its own instruction | restored, plus a placeholder backstop |

### LOW — closed

| finding | how |
|---|---|
| self-test wrote a phantom sidecar into the real data root | `TempDataRoot` scope guard, seven sites |
| ANALYSIS_SUITE_PLAN asserts `dataRoot_` is never assigned | correction note in place, naming the two items that ARE still true |
| two members hold the identical data root | **pinned, not merged** — see below |
| placeholder advertises ACIP only | now names Wylie and says detection is automatic |
| terminology chip blank on first run | half of this was already fixed and read as whole; the label was still constructed empty |
| evidence lands off-screen with no confirmation | `insertIntoDraft` does `ensureCursorVisible` and announces |
| AI back-check's reason hidden in a tooltip | now in the label, because a disabled QPushButton on macOS does not reliably show a tooltip at all |
| two pairs of ribbon buttons share an icon | distinct kinds; the gate counts duplicates. Two earlier versions of that gate were GREEN while the defect sat in front of them |

### LOW — deliberately not done, with the reason

**Two members holding the identical data root — pinned rather than merged.**
The audit offered deleting `dataRoot_` and using `root_` at the six docprops
sites. `dataRoot_` exists *because* of finding F0: it was never assigned at
all, and every Draft sidecar write was a silent no-op until 2026-09-09.
Rewriting those six sites to close a risk that has never fired, in the lines
that carried a live bug three days earlier, trades a theoretical divergence for
a real chance of reintroducing it. A gate now makes divergence a FAIL, which is
what the finding asks for. Revisit when something else needs those lines open.

**Return in an empty apparatus search box does nothing.** Not done. It is the
correct behaviour for an empty box; the finding is that it gives no feedback.
That is a real gap and the fix is a message, but the same pane's search already
answers "Type something to search" in the report panel, so the reader is not
without information. Left for a pass that looks at the whole pane's feedback
rather than one control of it.

**"Pair-hyphenate author" overwrites the field with no way back.** Partly done:
it has a tooltip now. The "no way back" half is real and NOT fixed — the honest
fix is an undo, and a one-field undo bolted onto one button is the kind of
half-mechanism that later gets mistaken for a general one. It belongs with the
form's validation work.

**The AI paragraph carries no provenance stamp.** Open, and the most
substantial of the remainder. It is not a labelling tweak: it needs the
inserted text to carry a stamp that survives save, reload and export, which
means deciding where a per-paragraph provenance mark lives in a plain-text
draft. That is a design question, not a fix.

