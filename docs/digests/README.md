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

Each digest goes out by email from Adam's Gmail with three attachments —
the `.md`, a `.txt` (pandoc plain) and a `.docx` (pandoc) — so every
reader can open it in whatever they use. Recipients: Geshe Michael Roach
<geshemichael@gmail.com>, John Brady <john.brady@asianlegacylibrary.org>,
Joel Crawford <joel.crawford@asianlegacylibrary.org>. Subject line:
`Diamond Cutter Translation Tool — daily digest, <weekday> <D Month YYYY>`.
The assistant prepares the email as a Gmail DRAFT with the attachments and
a short cover note; Adam reads and sends.

    pandoc docs/digests/D.md -o docs/digests/D.docx
    pandoc docs/digests/D.md -t plain --columns=78 -o docs/digests/D.txt

## Rolling log (Adam, 2026-09-08)

`docs/digests/LOG.md` is the append-only record of notable actions
between digests, written as they happen. The digest is drafted from it;
after the digest is committed the entries roll into
`docs/digests/LOG_ARCHIVE.md` under the digest's date and LOG.md is
emptied. If it isn't in the log, it doesn't reach the digest.
