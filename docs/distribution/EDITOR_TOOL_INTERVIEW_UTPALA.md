# The editor's tool — working session with Venerable Utpala

**Purpose.** Diamond Cutter Classics has an editor. The translation tool
does not yet have an editor's workflow: it has a translator's workflow
with editing bolted on. Before anything is designed, I want Utpala's own
process, start to finish, in her words — what reaches her, what she does
to it, what she sends back, and what she has to do by hand today that a
tool could do for her. Everything the tool can already do is on the table
for her; anything missing that she needs goes on the build list.

**Session.** 90 minutes. Screen share both ways: she shows me a real
edit, I show her the tool. Recorded (with her permission) so I do not
have to take notes while listening.

**What I will ask her to bring**
- One text she has edited recently, in the state she received it and the
  state she sent it back.
- Her style sheet, or whatever stands in for it (notes, memory, habits).
- Any checklist, template or macro she uses.
- The last three things that annoyed her while editing.

---

## 1. Her workflow, in her order (open questions — do not lead)

Ask her to walk through one real text end to end. At each step: what
arrives, what she does, what leaves, how long it takes, what goes wrong.

1. **Intake.** How does a text reach her? Email, Drive, Dropbox, a folder?
   In what format — Word, Google Doc, plain text, PDF, the tool's own
   files? Does she get the Tibetan alongside the English?
2. **First pass.** What is she looking for first? Sense, style, grammar,
   consistency, formatting, citation, all at once?
3. **Marking.** How does she mark a change — tracked changes, coloured
   text, comments, a separate list? How does she raise a question for the
   translator rather than make a change?
4. **Terminology.** How does she check that a Tibetan term is rendered the
   same way throughout, and the same way as in the other volumes? What
   does she do when it is not? Who decides?
5. **Style sheet.** What is in it? Spacing, capitalisation, italics for
   Sanskrit, diacritics, quotation style, numbers, dates, honorifics,
   how Tibetan and Sanskrit words appear, how the Geshe's own English is
   preserved. Which rules does she apply by hand every single time?
6. **References and citations.** How are folio citations, scripture
   quotations and cross-references written and checked?
7. **Queries.** How does she ask the translator a question, and how does
   the answer come back? What happens to the query afterwards?
8. **Rounds.** How many passes? What changes between them? How does she
   see what changed since her last pass?
9. **Sign-off.** What has to be true before she calls a text finished?
   Is there a checklist? Who else signs?
10. **Hand-off.** What does the next person (typesetter, publisher, web)
    need from her, and in what format?
11. **The annoyances.** What takes time that should not? What has she
    lost, forgotten, or had to redo? What does she keep in her head that
    a tool should remember?

---

## 2. What the tool can already put at her disposal

Show these; ask which are useful, which are wrong for her, what is
missing beside each. (Everything listed here exists and is tested.)

**Comparing versions and editions**
- Two texts side by side with the differing **syllables** marked, in
  ACIP, Wylie or Tibetan script; rules to ignore spacing, shad variants,
  folio markers or editorial brackets; comparison across scripts.
- Compare a text with its **saved version** or with the **clipboard**.
- **Three-way merge**: two people's edits of one text against the
  original, conflicts listed one by one.
- **Folder compare** for two batches, with a preview before any copy.
- Reports: side-by-side HTML, a patch, and an **apparatus criticus**
  citing every variant by folio and line, as Markdown or CSV.

**Editing surfaces**
- Manuscript pane with house styles, fonts, paragraphs, lists, tables,
  hyperlinks, pictures, and .docx export.
- Draft pane with a footnote/apparatus bank and phrase memory.
- Document Properties with Tibetan-aware statistics: folios, syllables,
  shads, words, revision count, editing time.
- **House Style** switches (currently: two spaces after a sentence,
  colon and semicolon rules) applied on demand, on save, or as she types.
- Find and Replace with regular expressions, whole-word and case
  matching, preserved capitalisation, and find in files.
- Spelling doubts for Tibetan, stepped with a key.
- Snippets and templates; document protection; encodings for legacy
  input-centre files; line-ending control.

**Team and provenance**
- Comments on a text, proposals with an approval workflow, and a name on
  every change.
- Dossiers: one desk per text, reopening where she left off.

**The dictionary behind it**
- Geshe Michael's own English for a term, with how often each rendering
  is attested, and the corpus passages that attest it. Nothing composed.

---

## 3. Candidates to test with her (build only what she confirms)

Each of these is cheap to describe and expensive to build; her reaction
decides. Ask "would you use this, and when?" — not "do you like it?".

1. **Tracked changes she can hand to Word** — her edits exported as a
   .docx with revision marks, so the Geshe or a translator reviews it the
   way they already do.
2. **Queries as first-class objects** — a question attached to a line,
   with a state (asked, answered, resolved), listed for the translator
   and cleared when settled.
3. **Style sheet as switches** — every rule of the Classics style sheet
   in one panel, applied on demand or on save, with a report of what it
   changed and what it refused to change.
4. **Consistency report for one text** — every term rendered more than
   one way, every proper name spelt more than one way, with counts and
   line references; her decision recorded so the tool stops asking.
5. **Register warnings** — the dictionary already knows terms whose
   English changes with the register (prayer versus prose); surface them
   where she is editing.
6. **Document Versions** — every save kept, listed with its statistics,
   restorable, comparable, so "what did I change last Thursday" is one
   click.
7. **Replace across a whole volume with a preview** — every proposed
   change shown as a difference she ticks or unticks before anything is
   written.
8. **A sign-off checklist per text** — hers, not ours, with the state
   stored beside the text and shown in the dossier.
9. **Citation checking** — every folio citation and scripture quotation
   verified against the catalogue and the corpus, with the failures
   listed.
10. **An editor's pane** — one surface holding her queue: texts awaiting
    her, their state, her queries, her checklist, her last pass.

---

## 4. What I want to leave the session with

- Her workflow written down as steps, with the format at each boundary.
- Her style sheet as a list of rules, marked "mechanical" (the tool can
  apply it) or "judgement" (it must not).
- The five things she does by hand most often.
- Her verdict on each candidate above: build, adapt, skip.
- Agreement on how we test it: she edits one real text with a build I
  give her, and tells me where it got in the way.

## 5. After the session

- Write it up as `docs/EDITOR_WORKFLOW.md` (her process) and an
  editor's-tool plan with batches, the way the compare suite was planned.
- Put her style-sheet rules into Preferences ▸ House Style as switches.
- Give her a build early and often; her corrections outrank my design.
