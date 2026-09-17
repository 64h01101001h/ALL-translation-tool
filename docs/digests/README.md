# Daily digests

Standing order (Adam, 2026-09-08): every day we do real work on the
project, an end-of-day digest is written here as `YYYY-MM-DD.md` and sent
to Geshe Michael Roach and John Brady. Its purpose is to show the work
being done day to day and the progress toward release.

Rules for a digest:
- Written for readers who are not engineers. Plain words; a file name or
  number only when it matters to the reader.
- Honest: what shipped, what is provisional, what failed or was undone.
  The app's honesty labels (PROVISIONAL, TENTATIVE) are never softened.
- Drafted from the day's commits, ledgers and decisions, not from memory.
- Sections, in order: **Headline** (one sentence) · **What got done** ·
  **Adam's decisions and actions** · **Numbers** (a short table if any
  changed) · **Open questions for leadership** · **Tomorrow**.
- Kept short. A reader should finish it in two minutes.

## Distribution (Adam, 2026-09-08)

Each digest goes out by email from Adam's Gmail with **one attachment,
the `.docx`** (Adam, 2026-09-10 — the .md and .txt are working files and
are not sent).

### Numbering — STANDING ORDER (Adam, 2026-09-11)

Digests carry a **running number**, in the subject line and in the `.docx`
filename. The numbering counts every digest in the series: 8 September 2026
was **#1**, and 12 September 2026 is **#5**.

    Subject:  Diamond Cutter Translation Tool Daily Digest #5 — Saturday, September 12 2026
    Filename: Translation Tool - Daily Digest #5 - 20260912.docx

The `.md`, `.txt` and `.html` keep the ISO `YYYY-MM-DD` name so they sort by
date in this directory, and the `.docx` keeps its `YYYYMMDD` stamp for the
same reason — a recipient's downloads folder sorts the series chronologically,
which "Saturday, September 12 2026" would not.

**The number is derived, never typed.** `docs/digests/NUMBERS.tsv` is the
ledger and `tools/digest_number.py` reads it. The number is the one figure in
a digest that cannot be recomputed from the day's work — it is a running
count, and a running count kept in someone's head is wrong eventually.
`tools/build_digest.sh` assigns it, builds the title the subject line must
carry (weekday included, so the weekday cannot be wrong either), and
**refuses to build at all** when the digest's own first line disagrees,
printing the line it expected. Two further refusals: a date already in the
ledger keeps its number forever, because those subject lines are in other
people's mailboxes; and a date earlier than the last one is refused rather
than inserted, because inserting it would shift every number after it.

The first four went out before the numbering existed. They are numbered
retroactively in the ledger so that #5 lands where Adam said it should; their
sent subject lines are **not** rewritten, because they are already delivered.

### STANDING ORDER (Adam, 2026-09-17): "generate the digest" means all of it

When Adam asks for the digest, he is asking for the whole thing without
further prompting:

1. write it,
2. build the `.docx` (via `tools/build_digest.sh`, never pandoc directly),
3. **put it in the Gmail drafts folder**, and
4. **attach the .docx to that draft** where the tooling allows it.

**Step 4 is where it fails, and it fails silently. Measured 2026-09-17.**
The Gmail connector takes an attachment only as base64 *inline in the tool
call* - there is no path parameter, nothing that reads the file from disk. So
the whole file has to be retyped through the model, and a digest `.docx` is
about 15 KB, or ~20,500 base64 characters. That does not survive the trip.
Digest #9 was attached twice and truncated both times: first at 9,690
characters of 20,528, then at ~1,100. Neither attempt reported an error.

**The failure looks exactly like success.** A truncated ZIP still begins with
the `PK` magic bytes, so `create_draft` accepts it, the draft shows an
attachment of a plausible size, and Word refuses to open it - at the far end,
in front of leadership. Worse, `get_draft` does not report a draft's
attachments *at all* (confirmed against a 36-character attachment that landed
correctly), so the obvious check says "no attachment" whether one is there or
not. **The only honest verification is to read back what was actually sent**
and compare it byte for byte with the file on disk:
`tools/check_draft_attachment.py`.

So step 4 reads, in practice: attach it only if the base64 verifies; otherwise
leave the draft clean, hand Adam the `.docx` directly, and say so. An
unattached draft costs him one drag. A corrupt attachment costs the audience
the document, and costs us the claim that our artifacts can be trusted - which
is the one thing this project cannot spend.

He reads and sends. Nothing is ever sent on his behalf — the draft is the
deliverable, and asking "shall I draft it?" after he has asked for the digest
is asking twice.

### Recipients and cover note

Recipients: Geshe Michael Roach
<geshemichael@gmail.com>, John Brady <john.brady@asianlegacylibrary.org>,
Joel Crawford <joel.crawford@asianlegacylibrary.org>.
The assistant prepares the email as a Gmail DRAFT with the attachments and
a short cover note; Adam reads and sends.

Build everything with `tools/build_digest.sh YYYY-MM-DD` rather than calling
pandoc directly — it is what assigns the number, enforces the title and names
the `.docx`. A bare pandoc run produces an unnumbered file that will not match
its subject line.

## Rolling log (Adam, 2026-09-08)

`docs/digests/LOG.md` is the append-only record of notable actions
between digests, written as they happen. The digest is drafted from it;
after the digest is committed the entries roll into
`docs/digests/LOG_ARCHIVE.md` under the digest's date and LOG.md is
emptied. If it isn't in the log, it doesn't reach the digest.

## Cover-note rules (Adam, 2026-09-08)
- The digest text goes **inline** in the email body; the three files are
  attached (or Adam attaches anything over ~40 KB himself).
- **Recommend the .docx.** Whenever a digest or a technical document goes
  out as .md/.txt/.docx, the cover note highly recommends opening the
  .docx: it is properly formatted and displays far better than .md or .txt.
- No mention of the tooling used to build the project, in any outgoing
  email or document.
- A next-day preview may be kept at `YYYY-MM-DD.draft.md`; rerun and
  update it when more work lands, then promote it to the dated digest.

## Voice (Adam and Jane, 2026-09-08 evening)
The digest is **Adam's report of Adam's work**, sent from his account.
Write it in the first person, with Adam as the actor of every item ("I
brought the menus to parity…", "I built…"). There is no separate
"Adam's decisions" section — that framing made the rest of the work read
as if it happened outside his direction, which is false: nothing in a
digest happened outside his instruction. Decisions appear as "Decisions I
made along the way", inside the narrative. Section order: Headline · What
I did · Decisions I made along the way · Numbers · Open questions for
leadership · Next.

## Visuals (Adam, 2026-09-08 late — standing format)
Every digest carries visuals: screenshots of every surface that changed
that day (captured headlessly with the app's own `--screenshots` driver so
they are reproducible), and a chart or diagram wherever it explains the
numbers better than a table. Images live in `docs/digests/img/YYYY-MM-DD/`
and are referenced with relative paths from the `.md`; pandoc embeds them
in the `.docx` and the HTML render; the `.txt` carries the captions. The
cover email attaches the `.docx` (images embedded) — inline images in the
Gmail draft are not attempted. Build: `tools/build_digest.sh YYYY-MM-DD`
(captures → charts → md/txt/docx/html).

## Readability — STANDING ORDER (Adam, raised twice, 2026-09-08 and 09)

Be hypercritical about spacing. Nothing may touch anything else: not a
heading and its text, not a picture and its caption, not a caption and the
next section. Three mechanisms enforce it, and all three must stay true.

**1. Word styles** — `tools/digest_reference.docx`, generated by
`tools/make_digest_reference.py`. The measured house values (20 twips =
1 point):

| Style | Before | After | Line |
|---|---|---|---|
| Normal / Body / FirstParagraph | — | 10 pt | 1.20 |
| **Compact** (every paragraph inside a list — most of a digest) | — | 7 pt | 1.15 |
| Heading 1 | 28 pt | 12 pt | — |
| Heading 2 | 26 pt | 12 pt | — |
| Heading 3 | 20 pt | 8 pt | — |
| Figure | 22 pt | 6 pt | keep with caption |
| Image caption | 6 pt | **24 pt** | — |

The trap: pandoc styles list content as **Compact**, so setting Normal
alone changes almost nothing in a digest. Regenerate the reference if the
converter is upgraded, and check the built `.docx` before sending.

**2. Image size** — `tools/digest_image_dpi.py`, run by the build. The
captures are taken at device pixel ratio 2, and a document converter reads
the density from the PNG itself, not from a command-line flag. Every image
is stamped 192 dpi, except one whose width would overflow the 6.3-inch
text column, which is stamped with exactly the density that makes it fill
the column. Without this a small menu capture fills a whole page and the
document reads as crowded no matter what the paragraph spacing says.

**3. HTML** — the stylesheet in `tools/build_digest.sh`: 62-em column,
1.55 line height, 2 em above and 2.2 em below every figure, captions set
apart in a smaller face.

**Check before sending.** Render the first page (`qlmanage -t -s 1500 -o
<dir> <file>.docx`) and look at it. If anything is cramped, fix the style,
not the one document.

## The email itself (Adam, 2026-09-09)

The digest goes in the BODY of the email, in full, formatted the same way —
not a short description of it. The three files are still attached, and the
cover note recommends the .docx above everything else, including the copy in
the body, because the pictures are only in the .docx.

Two practical limits, recorded so nobody re-litigates them:
- The screenshots cannot be embedded in the body from here: sixteen figures
  come to about five megabytes of encoded data, far past what can be written
  into a message. Each figure's place in the body carries its caption and
  says the picture is in the attachment.
- The attachments have to be added by hand for the same reason. Opening the
  .docx, selecting all and pasting into the Gmail draft is the one-minute
  route if the pictures are wanted in the body on a given day.

