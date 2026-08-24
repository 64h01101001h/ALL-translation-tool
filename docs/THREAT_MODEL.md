# Threat model — Diamond Cutter Translation Tool

*Written 2026-08-24 for the P1 gap "No threat model, and the roster
holds credential material". Grounded in this codebase, not a template:
every claim below cites the file or the finding it comes from.*

The immediate use is practical. The dependency triage
(`tools/vuln_check.py --triage-all`) leaves 16 advisories needing
judgement, and "is this reachable?" cannot be answered without a
written statement of what this app trusts and what it does with it.

---

## 1. What this is not

This is a translation tool for a Buddhist library, distributed free of
charge to a named team and to monastic input centres. It is not a bank,
it holds no payment data, and it is not a target worth a professional
attacker's time. Saying so plainly matters: a threat model that
pretends otherwise gets ignored, and then the two or three things that
*are* real get ignored with it.

Out of scope, deliberately: nation-state adversaries, side-channel
attacks, supply-chain compromise of Homebrew itself, and physical
attacks on a machine an attacker already controls.

---

## 2. What is actually worth protecting

| Asset | Why it matters | Where it lives |
|---|---|---|
| **Geshe Michael Roach's attested English** | The whole point of the project. Rule 1: binding, never composed by a machine. | the spine db, `hgm_gloss` |
| **A translator's unsaved work** | Hours of double-keyed input, unrecoverable. | editor buffers, `GlossaryStore`, `DossierStore` |
| **The catalog roster** | Access control for the in-house team: who may catalog, and as whom, with **salt+hash per member** (SQA FAIL-1). | `library/` roster file |
| **Team judgements** | Proposals, rulings, comments — a permanent record of an authority's decisions. | shared Dropbox folder (`comments.h:4-10`) |
| **Unlicensed reference payload** | `build/reference.db` ships in TEAM builds only; a forwarded team DMG is a licensing incident. | the DMG |

Note the shape: **four of five are integrity and confidentiality of
work, not secrecy of secrets.** This app's realistic harm is losing or
corrupting a translator's judgement, or misrepresenting whose words are
whose — not data theft.

---

## 3. Trust boundaries

Everything below crosses from "someone else's bytes" into this process.

**T1 — Scanned images and OCR input.** Arbitrary image files, decoded
through OpenCV, libpng, libtiff, freetype, jasper. **This is the
largest attack surface in the product** and the only one with a proven
memory-safety defect: MEM-1 was a ~10 KB heap overread whose bytes were
appended to OCR output *as recognised Tibetan*.

**T2 — The OCR model download.** `showOcrModelManager` fetches
`config.json` and the `.onnx` as **two independent GETs**, and
`ocrDownloadOne` discards the return of `f.write()` while validating
only `size() > 0` (MEM-1). A truncated download leaves a model and a
charset that are not a pair.

**T3 — Imported texts.** ACIP/wylie/Unicode files, `.dic`/`.ndx`
payloads from legacy ACIP tools, and release packages. Parsed by our
own C++ engines.

**T4 — The shared Dropbox folder.** Team stores are read from a
directory other machines and other people write to, including
"conflicted copy" siblings. Content there is *not* attacker-controlled
in the usual sense, but it is **not under this process's control**.

**T5 — BDRC / network lookups.** User-initiated only; the deterministic
core stays offline by rule.

**T6 — The DMG itself.** Ad-hoc signed, **not notarised** (SQA BUILD-3).
Gatekeeper cannot vouch for it, so a recipient cannot distinguish our
build from a modified one.

---

## 4. The realistic scenarios, ranked

**S1 — A malformed scan crashes or corrupts the OCR path.**
*Likelihood: high. It has already happened.* T1 plus MEM-1. The
consequence that matters is not the crash: it is **garbage surfaced as
recognised Tibetan**, which is rule 2 wearing a memory bug's clothes.
*Mitigated:* MEM-1's charset/output-layer mismatch is now refused
(`ocrCheckVocabCovers`). *Open:* the 16 triaged advisories, most of
which sit on this path.

**S2 — A translator's work is lost while the app reports success.**
*Likelihood: high, and it is the harm this project has most often
found.* Not an attacker at all — a full disk, an ejected volume, a
disconnected share. FAIL-1 had three stores returning TRUE over writes
that never landed; FAIL-2 the same for rulings.
*Mitigated:* those stores now flush and close before judging, pinned by
`storeflush_smoke`. *Open:* the seven other `QTextStream`-over-`QFile`
sites FAIL-1's recommendation names.

**S3 — A team DMG is forwarded outside the team.** *Likelihood:
moderate; it requires only kindness.* It carries `build/reference.db`,
unlicensed third-party compilations. *Mitigated:* the README says so,
`PAYLOAD_MANIFEST.txt` gates what ships, and a market press refuses to
build while terms are unresolved. *Open:* nothing technical prevents
forwarding, and nothing could.

**S4 — A truncated model download produces confident nonsense.**
*Likelihood: moderate.* T2. *Mitigated:* the mismatch is now refused
rather than read past (MEM-1). *Open:* `ocrDownloadOne` still discards
the write return — the download itself is not verified, only its
consequence.

**S5 — The laptop is lost or the disk dies.** *Likelihood: low per
year, total in consequence.* Until 2026-08-23 there was **no
off-machine copy of this project at all**. *Mitigated:* verified mirror
plus a nightly job. *Open:* the mirror sits on a drive beside the
laptop; that is not disaster recovery.

**S6 — Roster tampering.** *Likelihood: low.* The roster carries
salt+hash per member and lives in a shared folder. This is in-house
identity, not authentication against anything valuable, and the honest
statement is that it is a **record of who did what, not a security
boundary**. It should not be relied on as one.

---

## 5. What follows for the 16 open advisories

Judge each against T1. An advisory in a library on the image-decode
path (libpng, libtiff, freetype, graphite2, harfbuzz, opencv) is
**reachable in principle** and needs a real answer. One in a library we
link but never drive with untrusted bytes (openblas, xz, glib) is lower
priority and should be recorded as such rather than left ambiguous.

One question is already open and unanswered: **libpng is reached only
transitively, through Qt and OpenCV.** Whether the vulnerable push-mode
APNG parser is ever entered depends on what those do with a scanned
PNG. Nobody has traced it. Until someone does, CVE-2026-40930 is
*unresolved*, not dismissed.

---

## 6. What this model does not cover, and should

- No answer for **T3**: our own engines' parsing of hostile ACIP or
  `.dic` payloads has never been fuzzed against malicious input, only
  against the 20,000 deterministic mutations of the engine fuzzer.
- No **incident path**. If a user reports garbled OCR output or a lost
  glossary, there is no defect-intake route (a separate open P1) and no
  diagnostic bundle to ask for.
- **T6 stands unresolved** pending an ALL-owned Developer ID.

---

*Corrections welcome. A threat model that is never argued with is a
document nobody read.*
