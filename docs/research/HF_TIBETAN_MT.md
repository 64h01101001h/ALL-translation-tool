# Hugging Face Landscape: Tibetan→English MT & Related Tibetan NLP
**Survey date: 2026-08-13.** Research for the Diamond Cutter Translation Tool. All licenses quoted verbatim where retrievable; unknowns are marked UNKNOWN rather than guessed.

**Project ground rule this survey serves:** no machine output may reproduce Geshe Michael Roach's (GMR's) published English (CLAUDE.md rule 1: MATCH from corpus evidence, never COMPOSE). Every MT candidate below therefore carries a training-data-contamination assessment (§6) — because a model trained on GMR's published translations could *memorize and emit* his English, which would violate the rule even accidentally.

---

## 1. Buddhist / classical-Tibetan MT models

### 1.1 Dharmamitra / MITRA (UC Berkeley BAIR — Nehrdich, Keutzer) — HF org `buddhist-nlp`

The most serious classical-language MT effort. Two model generations; the org holds **59 models, 26 datasets** as of this survey.

**Generation 2 (current, mid-2026) — Qwen3.5-based "mitra-qwen35" family:**

| HF repo id | Size | License | Task |
|---|---|---|---|
| `buddhist-nlp/mitra-qwen35-it` | 9B | Apache 2.0 | Instruction/chat, closed-book Buddhist Q&A, **translation + translation refinement** |
| `buddhist-nlp/mitra-qwen35-base-stage1` / `-stage2` | 9B | (card not fetched; sibling repos are Apache 2.0 — UNVERIFIED) | base, continued-pretrain |
| `buddhist-nlp/mitra-qwen35-2b-base-stage2` | 2B | (UNVERIFIED, likely Apache 2.0) | base |
| `buddhist-nlp/mitra-qwen35-embedder` | 8B | Apache 2.0 | cross-lingual semantic retrieval/alignment |
| `buddhist-nlp/mitra-qwen35-2b-embedder` | 2B | (UNVERIFIED, likely Apache 2.0) | retrieval/alignment, practical local size |
| `buddhist-nlp/bdrc-mitra-ocr-qwen35` / `-0.8b` | 2B / 0.9B | not stated on 0.8b card — UNKNOWN | Tibetan OCR (with BDRC) |

- `mitra-qwen35-it` details (from model card): base `buddhist-nlp/mitra-qwen35-base-stage2`, which had **~30B tokens of Buddhist-focused continued pretraining**; then multi-turn SFT (TRL, completion-only loss, lr 1e-5) mixing closed-book Q&A and translation/refinement across **Sanskrit (IAST), Tibetan (Wylie), Buddhist Chinese, Pāli**. BF16. **No evaluation results published on the card. No quantization guidance.** Note the Tibetan input convention is **Wylie transliteration** — convenient for us (our `allcore` already produces EWTS).
- **Generation 1 (2024) — Gemma-2-based:** `buddhist-nlp/gemma2-mitra-base` (9B; continued-pretrain 2 epochs / 7B tokens of Buddhist collections in Sanskrit, Tibetan, English, Pāli; license not shown on page), `buddhist-nlp/gemma-2-mitra-it`, `buddhist-nlp/gemma-2-mitra-e` (embeddings), and **`buddhist-nlp/gemma-2-mitra-mt`** — the paper's headline MT model. **The `gemma-2-mitra-mt` repo is gated: unauthenticated fetch of both the page and the HF API returns 401**, so its license could not be read verbatim. As a Gemma-2 derivative it must carry Google's Gemma Terms of Use — **UNVERIFIED, flagged**.
- **Paper:** *MITRA: A Large-Scale Parallel Corpus and Multilingual Pretrained Language Model for Machine Translation and Semantic Retrieval for Pāli, Sanskrit, Buddhist Chinese, and Tibetan*, arXiv:2601.06400 (Jan 2026), https://arxiv.org/abs/2601.06400. Claims: Gemma 2 MITRA-MT "outperforms all other open LLMs by a significant margin" — roughly **+15 GEMBA points** (Gemini 2.0 Flash judge) over the best open baseline across all four source languages. **No separate Tibetan-only or classical-vs-modern breakdown is published.** Trained 8×A100, ~4 weeks, DeepSpeed ZeRO-3 fp16.
- **Training data disclosure (critical for us):**
  - Pretraining: "5% is Tibetan sourced via the Asian Classics Input Project (ACIP)" — i.e. **our own ecosystem's Tibetan input texts are in MITRA's pretraining mix** (Tibetan-side only, as far as disclosed).
  - Parallel data: "2M sentences between Tibetan and English sourced via our collaborative effort with monlam.ai" — **composition undisclosed**.
  - The paper states verbatim: *"The data used for the machine translation benchmark cannot be made accessible since we do not hold the rights to these works."*
  - Released parallel corpus (GitHub `dharmamitra/mitra-parallel`, v2): 1,693,730 aligned records / 2,338,400 segment pairs — but only **Sanskrit–Tibetan, Sanskrit–Chinese, Chinese–Tibetan** (intra-classical). The **Tibetan–English pairs are NOT in the public release**. The HF dataset `buddhist-nlp/mitrasamgraha-released-data-only` (402,680 pairs) is Sanskrit–English; its very name ("released-data-only") confirms a withheld remainder.
- **Serving:** interactive use via dharmanexus.org / dharmamitra.org (site returned 403 to our fetcher; no public statement retrieved about API terms or local-deployment guidance).
- **Local feasibility:** Qwen3.5 / Gemma-2 architectures load with plain `transformers` (MPS backend) on Apple Silicon; 9B BF16 ≈ 18 GB weights → needs a ≥32 GB M-series machine comfortably, or quantization. **No official GGUF/MLX/CTranslate2 conversions of any MITRA model were found** (searched; only stock Gemma-2 GGUFs exist). llama.cpp supports both base architectures, so self-conversion is plausible but unproven for these fine-tunes. **No M-series tokens/sec figures reported anywhere.**

### 1.2 MLotsawa (Jacob Moore, HF `billingsmoore`) — https://github.com/billingsmoore/MLotsawa

Small open T5 models, in collaboration with THL (University of Virginia). Two generations:

| HF repo id | Size | License (verbatim) | Notes |
|---|---|---|---|
| `billingsmoore/mlotsawa-ground-small` | 60M (t5-small) | "MIT" | current "ground" model, `getok-v0` tokenizer, Uchen-script input |
| `billingsmoore/mlotsawa-ground-base` | 223M (t5-base) | MIT (per README; verify at use) | same series |
| `billingsmoore/tibetan-to-english-translation` | 770M (t5-large) | "CC BY-NC 4.0" (Creative Commons Attribution-NonCommercial 4.0 International) | older; author: "prototype … not recommended for ordinary usage" |

- **Training data — fully disclosed (the exemplar of disclosure in this space):**
  - `mlotsawa-ground-small`: **861,417 pairs** from "publicly available material, Monlam AI, Tibetan and Himalayan Library"; 1 epoch span-corruption pretrain + 50 epochs finetune.
  - Older 770M model: ~100k pairs **scraped from Lotsawa House** (lotsawahouse.org), and the model card explicitly inherits the source license — hence CC BY-NC 4.0. **The training-data license question you flagged is real and the author handled it correctly for that model** (NC data → NC model). The newer MIT-licensed ground models mix Monlam AI + THL + public data; whether every component permits MIT relicensing is **not itemized — flag if we redistribute**.
  - Public component datasets: `billingsmoore/LotsawaHouse-bo-en` (108k pairs), `billingsmoore/Hopkins-bo-en` (27.7k pairs — **Jeffrey Hopkins material, which in our project is reference-comparanda-only**), `billingsmoore/TED2020-bo-en` (964), `billingsmoore/Tatoeba-bo-en` (39).
- **Quality (honest self-reported):** ground-small: **BLEU 3.54, chrF 19.89, TER 87.58** — weak in absolute terms. The older 770M model's "BLEU 59.34" was measured on held-out Lotsawa House data (same-domain, same-register liturgical text) and is not comparable. Card warning, verbatim: "all translations should be treated as preliminary and should never be used without the input of an experienced human translator."
- **Local feasibility: trivial.** `pip install transformers torch`, `pipeline('translation', 'billingsmoore/mlotsawa-ground-small')`; 60M–770M runs CPU-only or MPS on any Mac. No published tok/s (irrelevant at this size — fast).
- **No classical-vs-modern evaluation published** (training data is essentially all classical/liturgical).

### 1.3 Monlam AI — HF org `MonlamAI`

- Their production MT (translate.monlam.ai, launched 2023) is **not published as weights on HF**. The public `MonlamAI` org holds only: `MonlamAI/tibetan_RoBERTa_S_e6` (83.5M fill-mask), plus datasets: `MonlamAI/Bo-voice-v1.0.0` (STT benchmark), `MonlamAI/Bo-bench-v1.0.0` (LLM reasoning benchmark), `MonlamAI/OCR-Dergetenjur` (845k OCR samples), `MonlamAI/tibetan-voice-benchmark`. Their "Bo-Eval v1" collection is the premier open Tibetan AI benchmark.
- **Monlam Melong** (their LLM): preview at **`TenzinGayche/Monlam_Melong_preview`** — 9B, **fine-tuned from `buddhist-nlp/gemma2-mitra-base`** (i.e., built on MITRA's base — the two projects are intertwined), claims MT for 200+ languages ↔ Tibetan plus NER/QA/segmentation. License field, verbatim: **"Open-Source License (details to be added)"** — i.e., **no usable license as of this survey; do not build on it**. No benchmarks; training data "primarily trained on Tibetan text", otherwise undisclosed.
- Note the circularity: **Monlam supplied MITRA's 2M Tibetan–English pairs; Melong is fine-tuned from MITRA's base.** The Tibetan–English parallel data at the heart of both ecosystems has a single, undisclosed composition.

### 1.4 Newer entrants (2026)

- **Banzhida / "From Curated Data to Scalable Models" (ACL 2026, arXiv:2507.09205):** Tibetan continual-pretrain LLMs, 7B dense + 50B-A10B MoE; reports top FLORES-200 bo→en BLEU among compared systems. **Weights not yet on HF** ("We will release the model weights … in the follow-up"); license unstated. Watch, don't plan around. Training data includes **41.13 GB "private data"** — undisclosed.
- **DeepZang** (China, unveiled Lhasa 2026-03): Tibetan/Chinese/English LLM with translation; state-backed; **not on HF**, no open weights found. Not a candidate.

---

## 2. General multilingual MT baselines with Tibetan

| Model | HF repo | Sizes | License (verbatim) | Tibetan | Local on M-series |
|---|---|---|---|---|---|
| NLLB-200 | `facebook/nllb-200-distilled-600M` (also 1.3B, 3.3B; 54B MoE) | 600M–54B | "cc-by-nc-4.0" | Yes — `bo_Tibt` in the model-card language list (FLORES-200 code `bod_Tibt`) | Excellent via CTranslate2 int8; 512-token limit |
| MADLAD-400 | `google/madlad400-3b-mt` (also 7.2B, 10.7B) | 3B/7.2B/10.7B | "apache-2.0" | Yes — `bo` among 419 audited languages (dataset even documents Tibetan virama-handling fixes) | Good: community CTranslate2 (`santhosh/madlad400-3b-ct2`) and GGUF (~1.65 GB quantized vs 11.8 GB) |

- NLLB-200 card, verbatim: **"NLLB-200 is a research model and is not released for production deployment"** — combined with CC-BY-NC this restricts it to internal research/cross-check use for us; fine for divergence checking, not for anything we ship.
- Both are trained overwhelmingly on **modern web Tibetan**; neither publishes any classical-Tibetan evaluation. Expect degraded performance on canonical/commentarial register — **UNKNOWN quantitatively, no published numbers**.
- MADLAD-400 is T5-architecture, Apache 2.0 end-to-end (model + dataset audit) — the cleanest-licensed baseline in the whole survey.

---

## 3. Embeddings (sentence-alignment candidates) and OCR/ASR notes

### Embeddings for Tibetan↔English alignment
- **`buddhist-nlp/mitra-qwen35-embedder`** — 8B, **Apache 2.0**. Embeds Tibetan in **native script OR Wylie "with identical retrieval quality"** (card), plus Sanskrit/Chinese/Pāli/English. Last-token (EOS) pooling, L2-normalized, 512-token max; asymmetric retrieval uses `<instruct>…\n<query>…` wrapping. Reported (hardened setting, 25,000 distractors): **R@1 0.846 multilingual parallels, 0.788 asymmetric, 0.523 English-question→passage**. This is exactly the sentence-alignment tool profile we'd want for corpus work.
- **`buddhist-nlp/mitra-qwen35-2b-embedder`** — 2B version; the practical local choice (≈4 GB BF16). Card not individually fetched — verify license at use (UNVERIFIED, expected Apache 2.0).
- Older: `buddhist-nlp/gemma-2-mitra-e` (9B, Gemma-2-based) — superseded by the Qwen3.5 embedders.
- Nothing else Tibetan-specific and credible was found for sentence embeddings; general models (LaBSE etc.) predate decent Tibetan coverage.

### OCR — one clear upgrade candidate over legacy BDRC OCR
- **`buddhist-nlp/bdrc-mitra-ocr-qwen35-0.8b`** (0.9B) and `-qwen35` (2B): built **in collaboration with BDRC**; handles "both dbu can (uchen) and dbu med (umed) scripts across woodblock prints, manuscripts, and modern print"; outputs plain Unicode Tibetan one line per physical line; wants images ≤5 MP, longest edge 1500–2000 px. Caveats: **no CER/accuracy published, no comparison to prior BDRC OCR, license not stated on the card (UNKNOWN)**, and the card's recipes assume CUDA/vLLM — MPS feasibility untested. Worth a bake-off against our existing BDRC OCR when OCR work resumes; not an automatic switch.

### ASR — modest whisper upgrades only
- `billingsmoore/tibetan-asr-nict-tib1-whisper-{tiny,base,small}` (+LoRA and 4/8-bit variants; July 2026): whisper fine-tunes on the `nict-tib1` dataset (16.6k). Drop-in replacements for stock whisper on Tibetan; no WER vs our current setup retrieved — **evaluate before adopting; not clearly an upgrade**, merely candidates. MonlamAI's `Bo-voice-v1.0.0` / `tibetan-voice-benchmark` datasets are the right yardstick for any such test. (An OpenPecha forum "Tibetan STT Training and Benchmark Report", Aug 2025, exists: https://forum.openpecha.org/t/tibetan-speech-to-text-model-training-and-benchmark-report/423 — not fetched in detail.)

---

## 4. Local-inference recipes (macOS / Apple Silicon), top candidates

**No M-series tokens/sec figures are published for any model below; where stated, throughput is UNREPORTED, not estimated.**

### 4.1 `buddhist-nlp/mitra-qwen35-it` (9B, Apache 2.0)
```bash
pip install torch transformers accelerate  # torch ≥2.x with MPS
```
```python
import torch
from transformers import AutoModelForCausalLM, AutoTokenizer
tok = AutoTokenizer.from_pretrained("buddhist-nlp/mitra-qwen35-it")
model = AutoModelForCausalLM.from_pretrained(
    "buddhist-nlp/mitra-qwen35-it", dtype=torch.bfloat16, device_map="mps")
messages = [{"role": "user", "content": "Translate into English: <wylie sentence>"}]
inputs = tok.apply_chat_template(messages, add_generation_prompt=True,
                                 return_dict=True, return_tensors="pt").to(model.device)
out = model.generate(**inputs, max_new_tokens=512)
```
- BF16 weights ≈ 18 GB → practical on 32 GB+ unified memory. Input Tibetan as **Wylie** (card's convention).
- Quantization: **no official GGUF/MLX**; llama.cpp `convert_hf_to_gguf.py` should handle the Qwen3.5 arch (self-convert, then Q4_K_M ≈ 5–6 GB) — unproven for this fine-tune, verify output parity on a battery before trusting.
- Alternative smaller path: `mitra-qwen35-2b-base-stage2` exists but is a **base** model (no instruction tuning) — not directly usable for MT.

### 4.2 `google/madlad400-3b-mt` (3B, Apache 2.0)
```bash
pip install ctranslate2 sentencepiece huggingface_hub
```
- Use the community CTranslate2 conversion (`santhosh/madlad400-3b-ct2`) or convert with `ct2-transformers-converter --quantization int8`. CTranslate2 runs int8 on Apple Silicon CPU fast (small-seconds per sentence class of model); GGUF also exists (~1.65 GB quantized). Prompt format: `<2en> ཐུགས་རྗེ་ཆེ།` → English.

### 4.3 `billingsmoore/mlotsawa-ground-small` (60M, MIT)
```bash
pip install transformers torch
```
```python
from transformers import pipeline
pipe = pipeline('translation', 'billingsmoore/mlotsawa-ground-small')
pipe(["ཁྱེད་ལ་བསྟོད་ཅིང་གསོལ་བ་བཏབ་པའི་མཐུས༔"])
```
- Runs anywhere instantly; quality ceiling is low (chrF ~20). Input is **Uchen script**, not Wylie.

### 4.4 NLLB-200 600M (cc-by-nc-4.0, research-only) — cross-check harness
```bash
pip install ctranslate2 transformers sentencepiece
ct2-transformers-converter --model facebook/nllb-200-distilled-600M --quantization int8 --output_dir nllb600m-int8
```
- Source lang `bod_Tibt`, target `eng_Latn`; 512-token cap; int8 on M-series CPU is fast. Internal research use only.

---

## 5. Comparison table

| Candidate | Repo | Size | License (verbatim) | Classical-Tib quality evidence | Training-data disclosure | Mac-local | GMR-contamination risk (§6) |
|---|---|---|---|---|---|---|---|
| MITRA Qwen3.5 IT | `buddhist-nlp/mitra-qwen35-it` | 9B | Apache 2.0 | Strongest claim in class (predecessor +15 GEMBA, paper); **no per-language numbers on card** | Partial: sources named (ACIP, monlam.ai) but Tib-En pairs withheld | transformers/MPS BF16 (18 GB); no official quant | **UNKNOWN — highest structural risk** (undisclosed 2M Tib-En pairs; rights-encumbered benchmark) |
| MITRA Gemma-2 MT | `buddhist-nlp/gemma-2-mitra-mt` | 9B | **UNVERIFIED (gated repo, 401)**; Gemma ToU expected | Paper headline model, +15 GEMBA | same as above | Gated; Gemma-2 arch fine locally if granted | same as above |
| MLotsawa ground | `billingsmoore/mlotsawa-ground-small`/`-base` | 60M/223M | "MIT" (mixed-source caveat) | chrF 19.89 — weak but honestly measured | **Full** (all datasets public on HF) | Trivial (CPU) | Low-and-inspectable: datasets are public; **Hopkins material included**; Monlam AI subset composition UNKNOWN |
| MLotsawa T5-large | `billingsmoore/tibetan-to-english-translation` | 770M | "CC BY-NC 4.0" | BLEU 59.3 in-domain (Lotsawa House) only | Full (Lotsawa House scrape) | Trivial | Low: Lotsawa House corpus is public/enumerable; no GMR titles known there (UNVERIFIED) |
| Monlam Melong | `TenzinGayche/Monlam_Melong_preview` | 9B | **"Open-Source License (details to be added)"** — unusable | None published | Not disclosed | transformers-loadable but blocked on license | UNKNOWN (built on MITRA base + Monlam data) |
| NLLB-200 | `facebook/nllb-200-distilled-600M` … | 0.6–54B | "cc-by-nc-4.0" + "research model … not released for production deployment" | None on classical (UNKNOWN) | Web-mined (FLORES/NLLB paper); title-level content UNKNOWN | CT2 int8, excellent | UNKNOWN (web crawl could include GMR's widely-web-published translations) |
| MADLAD-400 | `google/madlad400-3b-mt` … | 3/7.2/10.7B | "apache-2.0" | None on classical (UNKNOWN) | CommonCrawl-derived, audited at language level, not title level | CT2/GGUF, excellent | UNKNOWN (same web-crawl reasoning) |
| MITRA embedder | `buddhist-nlp/mitra-qwen35-embedder` (/-2b-) | 8B/2B | Apache 2.0 (8B; 2B UNVERIFIED) | R@1 0.846 parallels (hardened) | Partial (as MITRA) | 2B fine on MPS | N/A for embeddings (no English generation) — safest MITRA artifact to adopt |

---

## 6. Training-data contamination risk (GMR's published English)

What is *documented*, per candidate — no speculation beyond sources:

1. **MITRA (both generations) — the central unknown.** Documented facts: (a) pretraining includes Tibetan "sourced via the Asian Classics Input Project (ACIP)" (5% of mix) — ACIP is the organization GMR founded, though the disclosed use is Tibetan-side input text, not English; (b) the Tibetan–English training pairs are "2M sentences … sourced via our collaborative effort with monlam.ai" with **no composition disclosure**; (c) the team states outright that their MT benchmark data "cannot be made accessible since we do not hold the rights to these works" — i.e., **rights-encumbered published translations are demonstrably inside this ecosystem's data pipeline**. Whether GMR's published English specifically is among the 2M pairs: **UNKNOWN and not determinable from public disclosures.** This is the single biggest contamination-risk finding of the survey.
2. **Monlam Melong:** training data undisclosed; inherits MITRA-base pretraining; Monlam's own translation-pair corpus is unpublished. **UNKNOWN.**
3. **MLotsawa:** the only candidate whose training data can be audited line-by-line (public HF datasets). Lotsawa House hosts Rigpa-ecosystem translators; no GMR titles are known to be hosted there (**UNVERIFIED — enumerable if we ever need certainty**). The `Hopkins-bo-en` component (27.7k pairs) means Hopkins-register English is in-weights — relevant to our "Hopkins is reference-only" rule, though Hopkins ≠ GMR. The "Monlam AI" slice of the 861k ground-model pairs is **UNKNOWN** in composition.
4. **NLLB-200 / MADLAD-400:** web-crawl-derived (CommonCrawl lineage). GMR's translations (Diamond Cutter, ACIP-published material, course transcripts) circulate widely on the open web, so inclusion is *plausible*; neither project discloses title-level contents for Tibetan/English. **UNKNOWN.**

**Operational consequence (recommendation, independent of which model we pick):** treat *every* MT output as potentially contaminated. Before any machine English is displayed or stored, screen it for n-gram overlap against our own 42,199-segment parallel corpus (we uniquely *have* GMR's English to screen against — the corpus is the contamination detector). Overlap above a threshold ⇒ flag, never auto-accept. This converts an unresolvable unknown into a mechanical guard, and it aligns with rule 1 (MATCH, never COMPOSE).

---

## 7. Recommendations

### (a) Pilot for Geshe Chodrak PD definition translation
**Pick: `buddhist-nlp/mitra-qwen35-it` (9B, Apache 2.0).**
- Best available classical-Tibetan capability claim; permissive license; Wylie input matches our pipeline; runs in `transformers` BF16 on a ≥32 GB M-series Mac today; the source text (Geshe Chodrak's dictionary) is public-domain, so the *input* side is clean.
- Guards: (1) run the §6 overlap screen on every output; (2) all outputs land as PROVISIONAL-tier with model+version provenance tags (e.g. `gloss_source: mt-mitra-qwen35-it`), consistent with rule 4; (3) never promoted into `hgm_gloss` (rule 1).
- Fallback if 9B is too heavy for target machines: MADLAD-400 3B via CTranslate2 int8 (Apache 2.0) — but expect a classical-register quality drop that must be measured, not assumed.
- Explicitly **not** recommended: `gemma-2-mitra-mt` (gated, license unverified), Monlam Melong (no usable license), MLotsawa ground (quality ceiling too low for definition prose, per its own metrics).

### (b) Divergence cross-checks (flagging where translations disagree)
**Pick: a 2-model jury of maximally independent systems — `buddhist-nlp/mitra-qwen35-it` + `google/madlad400-3b-mt`** (different architecture, different training-data lineage: curated Buddhist corpus vs audited web crawl). Disagreement between them is the signal; neither output is ever adopted as text, so NC/research-only restrictions and memorization risk are both largely neutralized — but keep NLLB-200 (cc-by-nc-4.0, research-only) as an optional third juror for internal experiments only.
- For the *alignment* substrate of cross-checking, adopt **`buddhist-nlp/mitra-qwen35-2b-embedder`** (verify its license on first download; the 8B sibling is Apache 2.0) — purpose-built for Tibetan↔English parallel retrieval (R@1 0.846 class), embedding-only so it cannot emit English prose at all.

### Watchlist
- Banzhida weight release (ACL 2026 paper promises HF weights; 7B dense could become the best open bo→en if the license lands permissive).
- `bdrc-mitra-ocr-qwen35-0.8b` — bake off vs current BDRC OCR when OCR work resumes (license currently unstated).
- Monlam Melong license finalization.
- MITRA GGUF/MLX conversions appearing (would unlock 16 GB Macs for the 9B).

---

## Sources
- MITRA org: https://huggingface.co/buddhist-nlp · paper: https://arxiv.org/abs/2601.06400 (HTML: https://arxiv.org/html/2601.06400v1) · corpus: https://github.com/dharmamitra/mitra-parallel · https://huggingface.co/buddhist-nlp/mitra-qwen35-it · https://huggingface.co/buddhist-nlp/mitra-qwen35-embedder · https://huggingface.co/buddhist-nlp/gemma2-mitra-base · https://huggingface.co/buddhist-nlp/gemma-2-mitra-mt (gated, 401) · https://huggingface.co/buddhist-nlp/bdrc-mitra-ocr-qwen35-0.8b · https://huggingface.co/datasets/buddhist-nlp/mitrasamgraha-released-data-only
- MLotsawa: https://github.com/billingsmoore/MLotsawa · https://huggingface.co/billingsmoore/mlotsawa-ground-small · https://huggingface.co/billingsmoore/tibetan-to-english-translation · https://huggingface.co/billingsmoore (profile; ASR models, LotsawaHouse-bo-en, Hopkins-bo-en datasets)
- Monlam AI: https://huggingface.co/MonlamAI · https://huggingface.co/TenzinGayche/Monlam_Melong_preview · https://monlam.ai/about
- Baselines: https://huggingface.co/facebook/nllb-200-distilled-600M · https://huggingface.co/google/madlad400-3b-mt · https://huggingface.co/datasets/allenai/MADLAD-400 · MADLAD paper: https://arxiv.org/abs/2309.04662
- 2026 entrants: Banzhida: https://arxiv.org/html/2507.09205v5 / https://aclanthology.org/2026.acl-long.1866/ · DeepZang: https://english.news.cn/20260316/55fbcce2023642eab3ce12e258deb3b5/c.html
- ASR context: https://forum.openpecha.org/t/tibetan-speech-to-text-model-training-and-benchmark-report/423
