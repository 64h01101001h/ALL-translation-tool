# ElevenLabs transcription pilot — test plan

*2026-08-16, Adam's directive: "build a test plan to test a few videos
and audio files for transcription accuracy via the ElevenLabs service."
This is the measurement half of the $2,200 Phase One line item — we
prove the engine on our hardest material BEFORE paying to run 8,000
hours through it.*

## What we are actually deciding

1. Is ElevenLabs Scribe accurate enough on Geshe Michael's classes to
   be the Phase One engine?
2. Does **keyterm biasing** with our 1,000-term GMR-phonetic lexicon
   measurably improve the Tibetan terms — the entire reason these
   transcripts feed the translation tool?
3. Which material classes need the Phase One "premium/human review"
   budget, and which are fine on one pass?

## Assets already in hand (nothing to build)

- `data/teaching/transcriber_keyterms.txt` — 1,000 biasing terms in
  GMR's own phonetic convention (kyang, tamche, sunam…).
- `data/teaching/transcriber_lexicon.tsv` — same terms with corpus
  frequency, wylie, and HGM gloss; this is the scoring key.
- Ground truth: 1,261 videos carry human-made captions
  (42 DCC + 1,219 TKB `.en.vtt` files with no auto-caption sibling).
- Baseline: YouTube auto-captions (`.en-orig.vtt`) for the same
  videos — the "do nothing" comparison.

## The sample — 10 items, chosen for hardness, not comfort

| # | class of material | why it is in the test | ground truth |
|---|---|---|---|
| 1–2 | Clean modern lecture (2020s DCC video) | the easy case; calibrates the ceiling | manual captions |
| 3–4 | Term-dense teaching (high keyterm rate — pick from lexicon hit counts) | the case biasing exists for | manual captions |
| 5 | Interpreted class (GMR + consecutive translator) | speaker turns + long pauses break ASR segmenters | manual captions |
| 6 | Q&A / multi-speaker session | far-field audience audio | manual captions |
| 7 | Chanting/prayers opening a class | known ASR failure mode; decides whether prayers need markers | hand-corrected 10-min window |
| 8–9 | **Pre-2010 audio-only** (the actual back catalog: cassette/minidisc era) | the material Phase One exists to rescue; if the engine fails here the plan changes | hand-corrected 10-min window each |
| 10 | Worst available audio (hiss, clipped, room echo) | establishes the floor and the "unusable" threshold | hand-corrected 10-min window |

Items 8–10 come from Adam or the Knowledge Base server — we hold no
pre-2010 audio in the repo. **This is the one blocking input.**

Segment length: score a fixed 10-minute window per item (hand
correction of longer windows costs more than it teaches).

## Conditions per item (3 runs)

- **A. YouTube auto-caption** (already on disk) — the free baseline.
- **B. ElevenLabs Scribe, no biasing.**
- **C. ElevenLabs Scribe + the 1,000-term keyterm list.**

B vs C isolates what biasing buys; A tells us whether paying beats
what we already scraped.

## Metrics (scored per item, per condition)

1. **Overall WER** against ground truth (normalize case/punct;
   standard alignment).
2. **Tibetan-term recall** — of the lexicon terms present in the
   ground-truth window, how many appear in the transcript in a
   recognizable form (exact GMR spelling / fuzzy ≤1 edit / missed).
   **This is the headline number**, not WER: a transcript can be 95%
   accurate English and still lose every dharma term.
3. **Tibetan-term precision** — biasing can hallucinate terms into
   English-only passages; count false insertions of lexicon terms.
4. **Timestamp drift** at 4 spot points (start/¼/½/¾) vs the video —
   the app's click-to-moment links die past ~2s drift.
5. **Speaker attribution** (items 5–6 only): % of turns assigned
   correctly, GMR vs interpreter/questioner.
6. **Cost + wall-clock** per audio hour, from the actual bills.

## Decision gates (agreed before the data comes in, so the numbers
decide, not the mood)

- Term recall (biased) **≥85%** on items 1–6 → engine approved for
  the video-era catalog.
- Biasing lift (C−B term recall) **≥10 points** → biasing goes in
  every Phase One run, as budgeted.
- Items 8–9 WER **≤25%** → pre-2010 audio runs on the same pipeline.
  Worse → those hours shift to the premium/human line, and the
  Phase One budget split gets revisited with real numbers.
- Item 10 defines the floor: below it, a recording is marked
  "preserve now, transcribe when engines improve" — never silently
  skipped.

## Procedure

1. Adam: ElevenLabs account, ~$20 credit (Scribe is ~$0.40/hr; the
   whole pilot is ≈2h of audio ≈ $1–2 — the $20 covers retries and
   a second model if offered).
2. Pull the 6 video items' audio (yt-dlp, already installed);
   receive items 8–10 from Adam.
3. Ground truth prep: manual captions cleaned to plain text; the
   audio-only items hand-corrected once (est. 2–3h of human time —
   this is the pilot's real cost).
4. Run A/B/C; keep raw JSON responses (they carry word timestamps
   and confidence — bank them, they're evidence).
5. Score with a small deterministic script (`tools/pilot_score.py`,
   to be written when data exists — scores banked to
   `docs/research/ELEVENLABS_PILOT_RESULTS.md`).
6. Verdict against the gates; update the Phase One budget notes and
   the donor-facing accounting language if the split changes.

## Rules that carry over

- Ground truth is corrected by a HUMAN before scoring; we never
  score an engine against another engine and call it accuracy.
- Every number in the results doc traces to a file in the run
  directory (rule 4: provenance).
- The recording remains the authority; transcripts ship tagged with
  engine + model + biasing state + date.
