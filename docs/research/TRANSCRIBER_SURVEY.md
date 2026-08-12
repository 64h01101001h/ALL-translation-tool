# AI Transcriber Survey — GMR Teaching Archive (~3,600 videos, est. 3,000–5,000 hrs)

**Researched live: 2026-08-12.** Question: the single most reliable AI speech-to-text service for
transcribing English teachings densely peppered with spoken Tibetan/Sanskrit terms
("bsod nams" spoken *sönam*, "skabs 'dir" spoken *kamdir*), where YouTube auto-captions corrupt
those terms. We have a 105,634-entry dictionary WITH English-phonetic renderings to feed a
custom-vocabulary mechanism — so **custom-vocab capacity matters more than baseline WER**.

Cost figures below assume a 4,000-hour archive.

---

## 1. Benchmark consensus (August 2026)

Primary independent source: the **Artificial Analysis Speech-to-Text leaderboard**
(https://artificialanalysis.ai/speech-to-text), which ranks batch models by **AA-WER** — an
audio-duration-weighted WER over ~8 hrs from three manually-corrected datasets (AA-AgentTalk 50%,
VoxPopuli-Cleaned-AA 25%, Earnings22-Cleaned-AA 25%). They built their own ground truths because
public WER datasets contain systematic human errors (AssemblyAI makes the same point:
https://www.assemblyai.com/blog/word-error-rate-is-broken).

Top of the AA batch leaderboard as fetched 2026-08-12 (AA-WER / price per 1,000 min):

| # | Model | Provider | AA-WER | Price /1k min |
|---|-------|----------|--------|---------------|
| 1 | Fun-Realtime-ASR-preview | (Alibaba/FunAudio, preview) | 1.7% | n/a |
| 2 | **Scribe v2** | **ElevenLabs** | **2.2%** | $3.67 (≈$0.22/hr) |
| 3 | MAI-Transcribe-1.5 | Microsoft Azure | 2.4% | $6.00 |
| 4 | Pulse Pro | Smallest.ai | 2.4% | $4.00 |
| 6 | Voxtral Small | Mistral | 2.8% | $4.00 |
| 7–9 | Gemini 3.1 Pro / 3 Flash / 2.5 Pro | Google | 2.8–2.9% | $11–18 |
| 10 | Solaria-3 | Gladia | 3.2% | $10.16 |
| 11 | GPT Transcribe | OpenAI | 3.3% | $4.50 |
| — | Universal (AssemblyAI) | AssemblyAI | 3.8% | $2.50 |
| — | Soniox v5 Async | Soniox | 3.8% | $1.66 (≈$0.10/hr) |
| — | Speechmatics Enhanced | Speechmatics | 4.0% | $12.50 |
| — | Whisper large-v3 (API) | OpenAI | 4.1% | $6.00 |
| — | Speechmatics Melia | Speechmatics | 4.9% | $4.00 |
| — | Deepgram Nova-3 | Deepgram | 5.2% | $4.30 |
| — | Rev AI | Rev | 5.9% | $3.33 |

Notes and caveats:
- **ElevenLabs Scribe v2 is the accuracy leader among mainstream production APIs** (the #1 entry
  is a preview model with no published price/production batch story).
- Rankings vary with test material. A July-2026 third-party test (Future AGI,
  https://futureagi.com/blog/speech-to-text-apis-in-2026-benchmarks-pricing-developer-s-decision-guide/)
  put Speechmatics Melia-1 at 6.4% and AssemblyAI "Universal-3.5" at 7.0% on *their* mixed
  real-world set, and Deepgram Nova-3 much worse on hard audio (≈18%) than its clean-audio 5.26%.
  Directionally consistent: ElevenLabs/AssemblyAI/Speechmatics top tier, Deepgram mid-tier on
  batch accuracy (its strength is streaming latency).
- Vendor blogs (Deepgram's "best APIs 2026", AssemblyAI's pricing comparisons) each place
  themselves first — treated as marketing, not evidence.
- **None of these benchmarks measure our actual failure mode** (English speech with embedded
  Tibetan/Sanskrit vocabulary). Baseline WER differences of 1–2 points matter far less to us than
  §3. A pilot on ~10 known-hard videos is the only real test.

## 2. Comparison table

"4,000-hr cost" = list price × 4,000, before volume/enterprise discounts (every vendor offers
negotiated discounts at this scale).

| Service / model | AA-WER | Custom vocab (batch) | Word timestamps | List price /hr | ~4,000 hr | Batch API practicality |
|---|---|---|---|---|---|---|
| **ElevenLabs Scribe v2** | **2.2%** | **Keyterm prompting, up to 1,000 keyterms (≤50 chars each)**; +$0.05/hr premium | Yes | $0.22 (+$0.05 keyterms) | **≈$1,080** | Async + webhooks; files up to 10 hrs; diarization to 32 speakers |
| **AssemblyAI Universal-3.5 Pro** | 3.8% (older Universal listing) | **keyterms_prompt, up to 1,000 words/phrases (≤6 words each)**; tokenization can reduce effective capacity; no listed surcharge | Yes | $0.21 | **≈$840** | Mature async API, webhooks, URL ingest; per-second billing |
| **Speechmatics (Enhanced / Melia-1)** | 4.0% / 4.9% | **additional_vocab, up to 1,000 entries — each with `sounds_like` pronunciation hints** (unique; exactly matches our phonetics problem) | Yes | Enhanced $0.75–1.04 (sources conflict, see §3); Melia ≈$0.24 | ≈$3,000–4,160 (Enhanced); ≈$960 (Melia, UNVERIFIED that custom dictionary works on Melia) | Batch API; auto volume discounts >500 hrs/mo, more at 24k hrs/yr |
| Soniox v5 Async | 3.8% | Free-text **context up to 8,000 tokens** (largest raw budget; can hold several thousand short terms) | Yes (token-level) | ≈$0.10 | **≈$400** | Async API; v5 is current (v4 retired 2026-06-30) |
| Deepgram Nova-3 | 5.2% | Keyterm prompting: **500-token cap, ~100 terms max, 20–50 recommended**; paid add-on ≈$0.078/hr | Yes | ≈$0.26 (+keyterms) | ≈$1,340 | Good batch API — but vocab capacity is 10× too small for us |
| OpenAI GPT Transcribe (gpt-4o-transcribe family) | 3.3% | Prompt-based biasing only; whisper-1 prompt capped at **224 tokens** | **No** (gpt-4o-transcribe: JSON/text only; whisper-1 has word timestamps but is the weaker model) | ≈$0.27 (whisper-1 $0.36) | ≈$1,080 | Disqualified: no word timestamps on the accurate model, tiny vocab budget |
| Google (Gemini 3.x as ASR) | 2.8–2.9% | Arbitrary long prompt (could include large term list) — but word-level timestamp fidelity from an LLM is UNVERIFIED/unreliable | Unreliable | $0.68–1.09 | ≈$2,700–4,400 | Expensive; timestamp story weak |
| Rev AI | 5.9% | Custom vocabulary (~6,000 phrases historically; not re-verified) | Yes | $0.20 | ≈$800 | Accuracy trails the leaders |
| **Local: whisper.cpp / faster-whisper large-v3** | 4.1% (API-equivalent) | `initial_prompt` only — **224 tokens** (~50–80 terms), per file | Yes (word-level, quality varies) | $0 | **$0 + ≈4–8 weeks of one Mac's compute** | See §4 |

Timestamps: every recommended service returns word-level timestamps suitable for the planned
term→video-moment index. The only major casualty is OpenAI's accurate model.

Direct YouTube-URL ingestion: **no major API takes YouTube URLs directly** (they take audio
uploads or plain audio URLs). Plan on a `yt-dlp -x` audio-extraction step regardless of vendor —
the project already uses yt-dlp for captions, so this is a non-issue. At ~64 kbps opus, 4,000 hrs
≈ 115 GB of audio.

## 3. Custom-vocabulary deep dive (the deciding feature)

The 105k dictionary can never be fed whole to any service — **every mechanism tops out around
1,000 terms** (Soniox's 8k-token context is the only larger budget). The workable strategy is the
same everywhere: select the ~500–1,000 highest-value terms (frequency-ranked from the corpus, or
per-video lists derived from existing captions/topics — the spine's FTS index makes this easy),
submit them per job, then run a dictionary-driven post-pass that normalizes phonetic renderings to
canonical forms.

Crucial subtlety: **submit terms in the spelling you want in the transcript, in roughly the form
the term sounds** (e.g. `sönam`, `kamdir`, `bodhicitta`, `Madhyamika`) — not raw Wylie. A keyterm
"bsod nams" is orthographically unrelated to the sound *sönam* and will bias nothing. Our
dictionary's English-phonetic renderings are exactly the right feed; the phonetics→Wylie mapping
happens in our own post-pass. This makes the 105k dictionary an asset twice: once to build the
keyterm lists, once to canonicalize the output.

Per vendor:

- **ElevenLabs Scribe v2** — "Batch supports up to 1000 keyterms (50 characters each)"
  (https://elevenlabs.io/docs/overview/capabilities/speech-to-text). Context-aware biasing (uses
  surrounding audio, doesn't blindly insert). Priced add-on: +$0.05/hr
  (https://elevenlabs.io/pricing/api); requests >100 keyterms have a 20-second minimum billable
  unit (irrelevant for hour-long files). Largest capacity **on the most accurate model** — the
  winning combination.
- **AssemblyAI Universal-3.5 Pro** — `keyterms_prompt`: up to 1,000 words/phrases, ≤6 words per
  phrase; each word of a phrase counts against the 1,000; capitalization and word length consume
  extra internal-token capacity, so effective capacity "may be lower"
  (https://www.assemblyai.com/docs/pre-recorded-audio/improving-transcript-results-with-keyterms-prompting).
  The older Slam-1 (the original 1,000-keyterm model) is **deprecated** per the pricing page;
  Universal-3.5 Pro inherited the capability. Older `word_boost` (200-term Universal limit) is
  legacy.
- **Speechmatics** — `additional_vocab`: up to 1,000 entries, and uniquely each entry takes
  **`sounds_like` alternative pronunciations**
  (https://docs.speechmatics.com/speech-to-text/features/custom-dictionary), e.g.
  `{content: "bsod nams", sounds_like: ["so nam", "sö nam"]}` — the only mechanism that could emit
  canonical Wylie directly, collapsing our post-pass. `sounds_like` is limited to
  en/de/es/fr/it/pt language packs (fine — jobs run as English). Pricing is the muddle: their AA
  listing implies Enhanced ≈$0.75/hr, third-party 2026 summaries say $1.04/hr batch Enhanced /
  $0.80 Standard, and the pricing page fetched today showed a "Pro tier $0.129/hr" figure that
  doesn't match either — treat exact pricing as UNVERIFIED pending a sales quote; all readings put
  Enhanced at 3–5× ElevenLabs/AssemblyAI. Whether `additional_vocab` applies to the new Melia-1
  model (their cheap multilingual flagship) is UNVERIFIED.
- **Soniox v5** — free-text `context` up to 8,000 tokens including a vocabulary section
  (https://soniox.com/docs/stt/concepts/context) — could hold ~3,000 short terms, the biggest raw
  budget anywhere, at the lowest price (≈$0.10/hr). But it's prompt-style biasing (soft), the
  company is a much smaller vendor, and independent evidence on biasing effectiveness is thin.
  Worth including in the pilot as the value dark horse.
- **Deepgram Nova-3** — keyterm prompting capped at 500 tokens / ~100 terms, 20–50 recommended
  (https://developers.deepgram.com/docs/keywords, confirmed by SDK issue #503). An order of
  magnitude too small. Eliminated.
- **OpenAI** — whisper-1 `prompt` is 224 tokens; gpt-4o-transcribe/GPT Transcribe support
  prompting but return **no word timestamps / SRT / VTT**
  (https://developers.openai.com/api/docs/guides/speech-to-text). Eliminated.
- **ElevenLabs vs vendors' own claims** — no material contradictions found; ElevenLabs' accuracy
  claims are corroborated by Artificial Analysis rather than only self-published.

## 4. Local option: whisper.cpp / faster-whisper large-v3 on Apple Silicon

Honest numbers (community benchmarks, 2026 — directional, thermals/chip-tier dependent):
- large-v3 with Metal: roughly **2–7× realtime** depending on chip (M3 Air ≈7×; RTF 0.33–0.5
  commonly reported). large-v3-turbo: ~5× faster with a small accuracy loss.
  (https://justvoice.ai/blog/whisper-benchmark-apple-silicon-m3-m4,
  https://www.digitalapplied.com/blog/local-speech-to-text-whisper-self-hosted-transcription-2026)
- At an optimistic 5× realtime, 4,000 hrs = **800 hours ≈ 5 weeks of a Mac running flat-out**
  (large-v3-turbo could cut that to ~1–2 weeks at some accuracy cost).
- Cost $0, full privacy/control, re-runnable forever.
- **Vocabulary biasing is the killer**: `initial_prompt` is 224 tokens (~50–80 terms) and decays
  over long files — the weakest biasing of any option, on exactly the dimension that matters most.
- Whisper's known hallucination-on-silence behavior adds QA burden across thousands of hours.
- Verdict: fine as a $0 fallback or for re-runs on a curated hard subset; **not the right primary
  engine** for this project because its custom-vocab capacity is ~10× smaller than the API
  leaders' while its WER (4.1%) is also worse than Scribe v2's.

## 5. FINAL RECOMMENDATION

**Primary: ElevenLabs Scribe v2 (batch API).**
Reasoning:
1. **Best measured accuracy of any production service** — 2.2% AA-WER on the independent
   Artificial Analysis leaderboard, clearly ahead of AssemblyAI (3.8%), Speechmatics Enhanced
   (4.0%), Whisper large-v3 (4.1%), Deepgram Nova-3 (5.2%).
2. **Ties for the largest custom-vocabulary capacity** — 1,000 keyterms per batch job, context-aware
   biasing — and it's the only service offering that capacity *on* the accuracy leader.
3. Everything else the pipeline needs: word-level timestamps (term→moment index), async batch with
   webhooks, 10-hour file limit (no chunking for any single teaching), diarization included.
4. Cost is a non-issue: ($0.22 + $0.05) × 4,000 ≈ **$1,080 list**, likely less with a volume quote.

**Runner-up: AssemblyAI Universal-3.5 Pro** — same 1,000-term keyterms capacity, no keyterm
surcharge, slightly cheaper (≈$840 list), the most mature batch-transcription tooling in the
industry; baseline WER a tier behind Scribe v2 on AA's data (though ahead on some third-party
tests — another reason to pilot both).

**Wildcard worth one pilot run: Speechmatics** — `sounds_like` is the only mechanism purpose-built
for "spelled one way, pronounced another," which is precisely the Wylie problem; if the pilot shows
keyterm biasing on phonetic renderings isn't landing Tibetan terms, Speechmatics Enhanced at
≈$3–4k total is still affordable and could emit canonical Wylie directly.

**Recommended process:** pick ~10 videos with dense Tibetan vocabulary and existing good captions
as ground truth; build a frequency-ranked 1,000-term phonetic keyterm list from the HGM
dictionary + corpus; run ElevenLabs, AssemblyAI, (optionally Soniox and Speechmatics) with and
without keyterms; score specifically on Tibetan/Sanskrit term recall, not overall WER; then commit
the archive to the winner. Total pilot cost: under $20.

---
*All prices/limits fetched live 2026-08-12; vendors change both frequently. Key sources:
https://artificialanalysis.ai/speech-to-text ·
https://elevenlabs.io/docs/overview/capabilities/speech-to-text · https://elevenlabs.io/pricing/api ·
https://www.assemblyai.com/pricing ·
https://www.assemblyai.com/docs/pre-recorded-audio/improving-transcript-results-with-keyterms-prompting ·
https://docs.speechmatics.com/speech-to-text/features/custom-dictionary ·
https://developers.deepgram.com/docs/keywords · https://deepgram.com/pricing ·
https://soniox.com/docs/stt/concepts/context ·
https://developers.openai.com/api/docs/guides/speech-to-text*
