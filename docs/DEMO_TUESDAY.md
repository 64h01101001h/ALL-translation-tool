# TUESDAY — Joel Crawford + John Brady

**Diamond Cutter Translation Tool · Geshe Michael Roach Dictionary**
*Hold this in one hand. Bold = do it. Quoted = say it.*

---

## 0 · PRE-FLIGHT (Monday night, alone, 20 minutes)

Nothing else in this document works if these are not done.

**On the machine you will present from — your own Mac. Not a borrowed one.**

1. **Overlay → left panel → click `presets: reading`.**
   Confirm **HGM definitions** and **phonetics** are TICKED.
   Confirm **Hopkins**, **84000**, **Das** are NOT.
   *Your saved settings currently have HGM definitions turned OFF. If you skip this,
   clicking a word shows Das, Jäschke and Hopkins and zero words of Geshe-la's English.*
   Belt and braces, in Terminal:
   ```
   defaults write com.all.TranslationTool overlay.glosses -int 1
   defaults write com.all.TranslationTool overlay.phonetics -int 1
   defaults write com.all.TranslationTool overlay.hopkins -int 0
   ```

2. **View → "Ribbon: show labels" → ON.** Relaunch. Confirm you can read button names.
   *Without this every control on every pane is an anonymous 20px icon.*

3. **Maximize the window and leave it maximized.** The app always launches at 1180×760;
   at that size buttons clip mid-word. Do not mirror to a 1280×800 projector — extended
   display, or ask the venue for 1600×900 or better.

4. **View → Night mode → decide.** For a projector, turn it **OFF**. Light chrome
   survives a washed-out beamer; dark does not. Look at both before you choose.

5. **Open the demo text once so session restore lands on it:**
   Library → `Illumination of the Path to Freedom, on Valid Perception` (Gyaltsab Darma Rinchen).
   Quit. Relaunch. **Confirm tab 1 opens populated, not blank.**

6. **Tidy the library root:** delete `library/acip_release6/`, delete
   `library/input_work/page_001_*.txt`, rename `my_materials/translation_prep test.txt`.
   *Re-check after any rehearsal — running the app writes these back.*

7. **Rehearse your five click-words and nothing else:**
   `shes rab` · `sems can` · `byang chub` · `bsod nams` · `stong pa nyid`.
   All five resolve to curated or glossary entries with real HGM English.

8. **OCR pane → tick "deskew off (0°)" and leave it ticked.**
   Open `~/Desktop/tharlam seljay images/I1KG9520008.tif` once and Run OCR.
   *Without that checkbox, roughly half of real folios return zero lines and a sideways
   black frame.*

9. **If you intend to show Catalog** (steps 9–10), do this too:
   - Catalog → **Official library…** → point it at
     `/Applications/Diamond Cutter Translation Tool/Diamond Cutter Tool Data/library`.
     **Not a fresh Dropbox folder** — a wrong root makes the bibliography audit report
     347 cited, 0 present, 347 missing, in red.
   - Catalog → **Team…** → create yourself with role **`admin,approver`**, not the
     prefilled `admin`. Plain `admin` cannot approve anything.
   - Build a demo intake folder: copy 8–10 real library texts to `~/Desktop/DEMO INTAKE`
     and rename them scanner-style (`scan_0417_box12_p1.txt`). Point intake there.
     *On your current intake folder, 4 clicks in 5 say the machine cannot identify the file.*

10. **Tuesday morning: cold-launch the installed app three times from Finder**, 60 seconds
    each, then `tail -20 ~/Library/Logs/DiamondCutterTranslationTool-lifecycle.log`.
    Every "startup ok" should have a matching quit line when *you* quit it. If one
    doesn't, tell the orchestrator before the meeting — that is the one failure with no
    stage recovery.

---

## 1 · THE 60-SECOND OPENING

**Start with the Overlay already open on the Gyaltsab text.** Words shaded. Nothing blank.

> "ACIP spent forty years typing this library, keystroke by keystroke.
> Everything here is that work — plus one thing that has never existed before."

**Point at the shaded text.**

> "This is a real volume. Every shaded phrase is a dictionary hit —
> 105,634 entries. Twelve thousand of them carry Geshe Michael Roach's own English."

**Click `shes rab`.**

> "Here is the rule the whole thing is built on.
> **The machine never composes English. It matches.**
> It finds where Geshe-la already said it, and shows you the passage he said it in."

**Point at `FROM THE CORPUS — 3 of 827`.**

> "Eight hundred and twenty-seven attestations. His own published translations.
> Underneath this is a parallel corpus — 35,601 aligned Tibetan–English segments
> across sixty of his courses."

**Scroll down to the recorded teachings.**

> "And below that, timecoded links into the actual class recording, at the moment
> he says the word."

> "All of it offline. No account, no server, no telemetry.
> It works in a monastery in Nepal exactly as it works here."

*That is the whole pitch. Everything after this is evidence.*

---

## 2 · THE DEMO PATH

**18 minutes. Natural stops at 6:30, 11:00, 14:00 — take questions there.**

---

### STEP 1 — The card *(Overlay)* · 1:00–4:00

> **VERIFIED 2026-08-23 — click `BSOD NAMS`, not `shes rab`.**
>
> Measured against the shipped dictionary: the phrase `shes rab zil gyis mi non pa`
> in the demo sutra has **no HGM equivalent**, and the card says so in italics.
> That is honest, and it is the wrong first impression.
>
> **The five safe words, all present in the demo text, all carrying Geshe-la's
> own English:**
>
> | Click this | HGM English | Why it lands |
> |---|---|---|
> | **`BSOD NAMS`** | merit · good deeds · **goodness (prayer register)** | **Open with this.** Three renderings, one explicitly marked by REGISTER. One card proves the whole thesis. 545 attestations. |
> | `BCOM LDAN 'DAS` | the transcendent victor · victorious buddha · victor | 10 renderings — shows the depth of the layer. |
> | `SEMS CAN` | sentient beings · living beings · being | 19 renderings, 1,566 attestations. The scale card. |
> | `BYANG CHUB SEMS DPA'` | bodhisattva · the bodhisattvas | 4 renderings, 627 attestations. |
> | `SANGS RGYAS` | Buddha · Buddhahood (contextually) | The contextual note is the point. |
>
> **The line to say on `BSOD NAMS`:**
> > "Three English words for one Tibetan word — and the third one is marked
> > *prayer register*. The machine did not decide that. Geshe Michael rendered it
> > that way in a prayer, and the tool found where. It matches his English; it
> > never composes English."


- **Read the card top to bottom out loud.** Nesting breadcrumb → Tibetan → HGM English
  → corpus → teachings.
- **Narrate the breadcrumb once, deliberately:**
  > "`shes rab ‹ shes` — click again and it steps down to the shorter phrase.
  > The tool shows you the nesting instead of guessing which one you meant."
  *Do this once on purpose. Then if a later click lands on a thin entry, it reads as
  designed depth, not a miss.*
- **Point at the corpus block.** Read one of Geshe-la's lines aloud:
  > "Wisdom not steeped in method is bondage. Wisdom steeped in method is freedom."
- **Point at the italic caveat under the teachings:** *the recording is the authority.*
  > "The honesty labelling is not a disclaimer. It IS the product."

**Only click your five rehearsed words.**

---

### STEP 2 — Governance in two clicks *(Overlay)* · 4:00–5:30

**This is the most important 90 seconds in the demo. Do not skip it.**

- **Click `presets: research`.** The card grows Das 1902, Jäschke 1881, Hopkins, 84000
  (stamped *CC BY 4.0 · reference only*), Monlam.
- **Do not linger.** Do not read the 84000 block aloud.
- **Click `presets: reading` again.** Watch every reference layer collapse away.

> "Everything that just disappeared is reference.
> What's left is binding.
> The machine may match his English. It may never compose it."

---

### STEP 3 — Script and sound *(Overlay)* · 5:30–6:30

- **`text as` → Tibetan script.** The whole document reflows into native script with
  proper stack shaping from the bundled fonts.
  *Eyeball your passage beforehand — `g-yas`, `g-yon`, `g-yu`, `g-yag` still render as
  bracketed Latin.*
- **`text as` → Pronunciation (GMR).** Whole document in Geshe-la's phonetic convention.
- **`text as` → Pronunciation (THL).** The scholarly transcription instead.

> "One dropdown. Whole document. Three ways to read the same page."

**⏸ STOPPING POINT — take questions.**

---

### STEP 4 — The author ledger *(Library)* · 6:30–8:30

> **VERIFIED 2026-08-23 — type these names, not those.**
>
> The audit found "Gyaltsab Je", "Milarepa", "Nagarjuna" and "Khedrup" all
> returned *"No person of that name"*, because every alias in the index was a
> Wylie-caps variant (`RGYAL TSAB DAR MA RIN CHEN`) and nobody types that.
> The phonetic half was already on disk in the Sungbum folder names, so
> **88 people are now searchable the way a human spells them**
> (`tools/add_phonetic_aliases.py`).
>
> **SAFE — these resolve:** `Gyaltsab Je` (P65) · `Tsongkapa` (P64) ·
> `Kedrup Je` (P55) · `Sakya Pandita` (P1056) · `Panchen Sonam Drakpa` (P101)
>
> **AVOID — honest misses, not bugs:**
> - **`Milarepa`** — genuinely absent from this catalog. The library is the
>   Gelug Sungbum plus Kangyur/Tengyur; his collected works are not in it.
>   If asked, that is the right answer and a good one: *"the tool only
>   claims what ALL has actually catalogued."*
> - **`Nagarjuna`** — present three times as `KLU SGRUB` with **zero** local
>   works. Tibetan names are homonym-dense and we will not guess which one a
>   visitor means, so no phonetic alias was invented for him.
>
> **Spelling note:** it is `Tsongkapa` in this index, not `Tsongkhapa` —
> the catalog's own spelling. Type it as the ledger has it.


- **Read tab → Library.**
- **Author box (STUDY group) → type `Tsongkhapa` → Enter → click the candidate card.**

The panel reads: **P64 · RJE TZONG KHA PA BLO BZANG GRAGS PA · 1357–1419 ·
161 works attributed · 139 in your Library · spelled 6 ways in the catalog** —
then 139 clickable English titles.

> "Spelled six ways in the catalog. Grouped by BDRC's own per-text linkage,
> not by name similarity."

**Then the honesty moment — misspell it on purpose.**

- **Type `tsongkapa` → Enter.** A cream banner appears across the top:
  *"≈ You arrived here by sound. You typed 'tsongkapa'… Nothing has confirmed it is the
  person you meant."*
- **Type `Tsongkhapa` again.** No banner.

> "An exact arrival is not slandered with doubt. A guess is labelled as a guess.
> That distinction runs through every surface in this tool."

**Safe third query: `mkhas grub`.** Four people with texts here, tiered, nothing orange.

---

### STEP 5 — Scale and the catalog card *(Library)* · 8:30–10:00

- **FIND box (right end of ribbon) → `Diamond Cutter` → Enter.** 5 texts, one of them
  the app's namesake sutra. Then `Lam Rim` → 39. Then `Bodhisattva` → 60, honestly
  labelled *(capped)*.
  > "This is the one thing in the app either of you can drive yourselves."
- **List view (ledger icon, right of ribbon).** 9,012 rows, seven columns, sortable.
  **554 ms** to walk the tree and decode every ACIP filename.
- **Click the Verification column header.** ACIP's own L/M/N/F proofreading tiers across
  9,000 texts.
- **Single-click any Tengyur text.** The catalog card: English title, Sungbum subject,
  ACIP Release-6 subject, author with dates, live **P-number at BDRC**, collection,
  text number, verification level, language, scan link, ACIP source preview.

**For Joel:**
> "That card is the ACIP catalog and BDRC's authority records rendered as one object.
> 8,985 texts installed — Kangyur 1,011, Tengyur 3,439, Sungbum 4,535.
> 462 people indexed, 127 of them with texts here."

---

### STEP 6 — Search *(Search)* · 10:00–11:00

- **Research → Search.** Put `chos` / `sems can` / `bde ba` in three fields.
  Combiner: **AND (same file)**. **Click Find** — *Enter does not work, use the button.*
- It returns in about **2.5 seconds**. The window is briefly frozen — talk through it.

> "This same query used to take five minutes forty-six seconds and eighteen gigabytes
> of RAM. It is now two and a half seconds and a third of a gigabyte —
> over an index of 14,077,690 lines across 8,986 files."

- **Click any file name in the results.** The whole Kangyur volume opens in the Overlay
  at the hit line — **901 ms**, 214,771 tokens, 325,689 shaded spans, 9,526 dictionary
  entries attached, 496 spelling flags raised.

> "Search, dictionary, segmenter and proofing engine, all landing at once on a
> canonical text, in under a second."

**Do not read the hit counts aloud.** They currently print display caps, not totals.

- **Search Setting → Citation web…** 500 connected pairs of works: every exact Tibetan
  passage shared between two courses, with Geshe-la's published English from both sides.
  > "Who quotes whom, provably. Attested reuse only — no fuzzy matching."

**⏸ STOPPING POINT — take questions.**

---

### STEP 7 — The engines *(Convert)* · 11:00–12:30

- **Research → Convert.** Paste:
  `BSOD NAMS KYI TSOGS DANG , YE SHES KYI TSOGS`
  The card assembles live: detected **ACIP** · wylie · **Tibetan at 26px** · phonetics
  (GMR) · THL phonetics with the Germano & Tournadre attribution.

> "The input centres type ACIP. That's the same line in four notations, live."

- **Select the Tibetan in the card, copy it, paste it back into Input.**
  The card flips to *detected: Tibetan script (reverse conversion)* and now prints an
  **ACIP row** too, with the footer: *reverse engine: pyewts-parity port, proven
  100.000% on 109,490 fixtures.*
- **Edit one Tibetan character.** The transliteration follows live.
- **Type `Joel Crawford`.** Output: `⟨Joel⟩་⟨Crawford⟩` with the red caption
  *"⟨…⟩ marks syllables the engine cannot legally convert — flagged, never guessed."*
  > "That's the discipline, in one glyph."
- **Type the wylie `byang chub sems dpa'`.** The card adds:
  *Mahāvyutpatti (DILA TEI, CC BY-SA 3.0) — MVP 625 · bodhisattvaḥ.*
  > "A ninth-century imperial Sanskrit–Tibetan glossary, answering a query typed live."

**Do NOT type lowercase wylie as your opener here** — the forward card has no ACIP row.
**Do NOT type `pramāṇa`, `buddha`, `dharma` or `vajra` in the Sanskrit pane** — they
return nothing. If you show Sanskrit, use **`śūnyatā`** and **`bhū`**.

---

### STEP 8 — OCR and input *(OCR, then Input)* · 12:30–14:30

**This is Joel's section. Confirm "deskew off (0°)" is still ticked.**

- **OCR pane → Open scan image… → `I1KG9520008.tif` → Run OCR.**
  In about **2.3 seconds**: six numbered rows, Wylie in monospace, Tibetan unicode at
  20pt beneath each, and a green *0/N syllables fail legality* count.
  Line 0 comes back as a real homage verse, read off a carving.
- **Click any Wylie word in the results.** The page viewer scrolls to it and draws an
  **orange box around that exact word on the woodblock.**
  > "That's frame-span tracking that BDRC's own pipeline discards. We keep it and turn
  > it into a locator."
- **Point at `⟨@##⟩`** sitting inside otherwise perfect Tibetan.
  > "It could not convert that folio marker, so it refused to guess."
- **Point at the orange banner:** *all output is OCR-DERIVED review material; never
  corpus-bound* — and the BDRC CC BY-NC 4.0 credit on every output file.

**Then the input side:**

- **Input pane → Open scan… → same TIFF → "Pre-fill from OCR (draft)".**
  *(This path forces 0° internally — it works on every page.)*
  The editor fills with uppercase ACIP, doubtful syllables underlined, and a status line
  saying the draft is OCR-derived and must be **corrected, not accepted**.
  > "Your typist stops keying from scratch and starts correcting.
  > The double-keying pass is unchanged."
- **Tick "Predictive typing" → start typing `BSOD NAM`.** It completes from the
  dictionary's own 105k ACIP headwords.
  > "Every syllable a typist keys is a syllable the dictionary already knows.
  > Deterministic inventory. Nothing invented."
- **Compare with partner file…** — *the button is at the far right of the ribbon.
  Drag the ribbon scrollbar right first, or you will hunt for it live.*
  Identical files → *double-keying PASS*. Divergent → discrepancies in orange, seams in
  green, `◀ disc / disc ▶` to walk them.
  > "This is ACE's core workflow, rebuilt."

**⏸ STOPPING POINT — take questions.**

---

### STEP 9 — The institutional ask *(Catalog)* · 14:30–16:00

**Only if pre-flight item 9 is done. If it is not, skip to Step 10.**

- **Catalog → click a file in the prepared intake folder.**
  The card reads *uncataloged — the filename matches no ACIP convention*, three STATES
  lights, then **SUGGESTED IDENTITY**: title read from the text, the Sanskrit side of
  the head, *matched against 8737 known titles*, then
  *Candidates — machine suggestions, not catalog entries. Nothing below is filed until
  a cataloger approves it* → **92% TD03995 …**
- **Sign in → Audit bibliographies…**
  **347 distinct works cited** in the published Mixed Nuts bibliographies ·
  **297 present** · **50 missing** · computed in **67 ms**. Each missing work carries a
  BDRC witness link and an "Export missing list…" handout.

> "Geshe Michael gave the cataloging class this assignment in December.
> That's the answer, as a button.
> **Those 50 missing works are the BDRC conversation.**"

**Do NOT click "Move to shelf…" while signed in as approver** — it performs a real move
and then offers to rename a real library folder.

---

### STEP 10 — The closer *(Approval)* · 16:00–17:30

- **Community → Approval.**
- **Set the filter to `Honorifics` BEFORE the pane is on screen.**
  Thirteen rows with real citations: *marked (hon.) in Hackett, A Tibetan Verb Lexicon,
  p.90* · *marked (H) in Choephel, Manual of Authentic Tibetan, p.39; textbook endorsed
  by Prof. U. Roesler (Oxford); CORROBORATED: Goldstein, Essentials (1991) p.95.*
- **Then switch to `All kinds`.** The badge lands: **225 pending.**

> "Two double-honorific, two high-honorific, thirteen honorific, three humilific —
> and 205 the engine derived from the register tables.
> Which is exactly why the authority still has to say yes."

**The closer — show a ruling that already landed.**
Open `data/pron_colloquial/colloquial_pron.tsv`, lines 312–313:

```
tsema namdrel   tshad ma rnam 'grel        approved   # approved by Adam 2026-08-12
```

> "Geshe Michael rules. The app changes. The ruling is signed and dated.
> And it did not touch the corpus."

**Then go straight into the ask (§7).**

---

## 3 · WHAT TO SHOW EACH MAN

### JOEL — BDRC, input centres, Apple ID, the catalog

| Show him | Where | The line |
|---|---|---|
| **The author strip + P-number** | Overlay, under the document | "P209 at BDRC · Treasury of Lives · 108 works by this author." |
| **The catalog card** | Library, single-click a Tengyur text | "ACIP's codes and BDRC's authority records as one object." |
| **The bibliography audit** | Catalog | "347 cited, 297 present, **50 missing**. That's our list." |
| **OCR on a real woodblock** | OCR pane | "2.3 seconds a folio, on your models, credited." |
| **Pre-fill + double-keying** | Input pane | "Your typist corrects instead of keying. The QC pass is unchanged." |
| **The Scans pane — as an ASK, never as a feature** | Scans | See below. |

**The Scans pane, said deliberately, not stumbled into:**
> "This pane links a text to its woodblock scans through your IIIF manifests.
> As of July it returns 401 to everyone — `iiifpres.bdrc.io` moved behind Auth0.
> Here is the exact request. Here is the exact response.
> **I need a service credential and this whole pane comes alive.**"

*The title-search half still works live against `purl.bdrc.io` — show a real BDRC
catalogue query returning real MW records in the same breath. It proves the plumbing is
right and only the credential is missing.*

### JOHN BRADY — the director

| Show him | Where | The line |
|---|---|---|
| **The corpus card** | Overlay, `shes rab` | "827 attestations of the master's own English." |
| **The two-click governance toggle** | Overlay, reading ↔ research | "What disappeared is reference. What's left is binding." |
| **Scale, as one number** | Library List view | "9,012 texts. 554 milliseconds. Offline." |
| **The approval channel** | Approval pane | "225 machine proposals. Two approved. Nothing enters the dictionary without him." |
| **A signed ruling in a file** | `colloquial_pron.tsv` | "Signed, dated, and it did not touch the corpus." |
| **The honesty banner** | Library, `tsongkapa` | "A guess is labelled as a guess." |

**The mission line for John:**
> "ALL's job is to preserve this library and make it usable.
> This is the first tool that carries a *named living master's* English through the
> whole stack — and refuses to invent a single word of it.
> Everything a machine did is labelled as machine. Everything he said is labelled as his."

---

## 4 · DO NOT TOUCH

*Each of these has a specific way of embarrassing you. Read this section twice.*

**Buttons that hang or take the app hostage**

- **"Analyze" (Analysis pane)** — no timeout, no cancel; on bad wifi it spins forever
  and the button greys out so you cannot even retry. Same for the **Draft pane's AI
  coverage diff**.
- **"Collections…" (Library ribbon)** — up to **65 seconds** of frozen app on bad wifi,
  then "3 of 3 needed attention" and three quarter-gigabyte **Download** buttons.
- **"Batch folder…" (OCR)** on your real scan folder — **32 minutes** of modal progress
  on 872 pages, and you have to hit Stop in front of the room.
- **"Download…" in the OCR models dialog** — tens of megabytes over conference wifi.
  Open the dialog to show the BDRC permission banner. Do not click Download.

**Buttons that do something irreversible**

- **"Approve all in filter…" (Approval)** — two clicks zeroes the 225 badge. No undo.
- **"Move to shelf…" while signed in as approver (Catalog)** — a real move on the real
  library, then a prompt to rename a real shelf folder.
- **Do not reset preferences** before the demo. It repoints Search at an unindexed
  library and quietly makes every query slower and capped.

**Surfaces that will look broken**

- **The Scans pane, opened cold** — four buttons, four dead ends. Only open it as the ask.
- **"Find scans on BDRC" from the Overlay** — prints a raw Qt error onto the flagship pane.
- **Help → "Check for Updates…"** — pops a folder picker, then says you are up to date
  regardless. The version regex cannot match this build.
- **The Spotlight scope row in Search** — leave it unticked. It lists documents from
  anywhere on your disk, with full paths, on the projector.
- **The big grey "open a page scan" box** (Input and OCR) — it looks like a drop zone.
  It is a label. It is not clickable and does not accept drags. Use the menu button.
- **The Sanskrit pane's own placeholder `pramāṇa`** — and `buddha`, `dharma`,
  `bodhisattva`, `vajra`. All return nothing. Use `śūnyatā` and `bhū`.
- **Lowercase wylie in Convert** — the forward card has no ACIP row. Lead with ACIP.
- **The View → "Ribbon: show labels" toggle, on stage** — with labels on, the Catalog
  ribbon needs 2,572px and Approvals… / Move to shelf… fall off the right edge. Set it
  Monday; on Catalog, drag the ribbon scrollbar right before you need those buttons.

**Author names that return nothing**

- **`Gyaltsab Je`, `Milarepa`, `Nagarjuna`, `Khedrup`, `Shantideva`, `Longchenpa`,
  `Sakya Pandita`, `Dalai Lama`** — all return "No person of that name."
  The index carries the catalog's own Tibetan spellings only.
- **`gyaltsab`** is worse — 39 orange "SOUNDS LIKE" guesses with the wrong man on top.
- **Safe queries only: `Tsongkhapa` · `tsongkapa` · `mkhas grub`.**

**On the card**

- **Do not read the 84000 block aloud.** Definitions 1 and 2 are the same Conze
  paragraph, and it prints spaces before punctuation. The `reading` preset hides it.
- **Do not zoom into the source badges.** One reads `wisdomMACHINE` with no space —
  the fix is queued, but do not put it on the projector at full size.

---

## 5 · IF IT BREAKS

*Each of these costs five seconds if you have the line ready.*

| What happens | Say this, then do this |
|---|---|
| **The card shows Das and Hopkins but no HGM English** | "Let me put it back in reading mode —" **click `presets: reading`.** Keep talking. |
| **The app opens on a blank Overlay** | "Let me pull up a real one." **Library tab → double-click the Gyaltsab text.** |
| **Enter does nothing in the search fields** | Say nothing. **Click Find.** Enter is not wired to those fields. |
| **The window freezes 2–3 seconds after Find** | "That's fourteen million lines being scanned." It comes back. |
| **OCR returns 0 lines and a sideways black frame** | "That's the deskew estimator — the pipeline has an override." **Tick "deskew off (0°)" → Run OCR again.** It will be clean. |
| **BDRC returns "unreachable" / 401** | "That's the Auth0 migration in July — the archive is up, the endpoint moved. That's one of my asks." **Move straight into the credential ask.** |
| **An author search says "No person of that name"** | "The index carries the catalog's own Tibetan spellings; the Western-name layer is the next data release. It won't guess, so an empty answer means exactly that." **Retype in Wylie or move on.** |
| **You click Approve by accident** | Nothing visible goes wrong. Afterwards: `git checkout data/proposals/proposals.tsv`. |
| **A button you named is not on screen** | Drag the thin scrollbar under the ribbon. ⌘K finds *content*, not commands — do not reach for it here. |
| **NO INTERNET AT ALL** | **Say it first, before anyone notices:** "Everything you're about to see runs offline — no account, no server. There are exactly two features that touch the network and I'll skip both." Then skip **Analyze**, **Collections…**, and **Scans**. Nothing else changes. The Das 1902 page scans open from disk. |
| **You are forced onto a borrowed Mac** | Do not download the DMG — copy the `.app` by USB. Then **before the room arrives**: `xattr -dr com.apple.quarantine '/path/to/Diamond Cutter Translation Tool.app'` and launch it once. **"Right-click → Open" no longer works** — Apple removed that in Sequoia. The manual path is System Settings → Privacy & Security → scroll down → "Open Anyway" → authenticate → "Open Anyway" again. Also turn ribbon labels back OFF there, and expect a different Tibetan font. |
| **The app quits silently at launch** | No stage recovery. This is why you cold-launch three times Tuesday morning. If it happens in rehearsal, stop and escalate. |

---

## 6 · THE THREE QUESTIONS THEY WILL ASK

### Q1 · "What can ALL actually ship? Who owns this data?"

**Every layer's licence is recorded in `docs/distribution/OPEN_SOURCE_NOTICES.md`, and
the tool prints the licence on the card.**

- SQLite + FTS5 — public domain
- Bundled Tibetan fonts — SIL OFL, each face banked in `data/fonts/FONTS.md`
- 84000 glossary (29,383 terms) and Translation Memory (400,745 aligned segments) —
  **CC BY 4.0**, per 84000's own Terms of Use table
- Mahāvyutpatti (DILA TEI) — **CC BY-SA 3.0**
- Monlam word lists (via openpecha) — **Apache 2.0**
- SOAS POS lexicon — **CC BY 4.0**, Zenodo DOI 10.5281/zenodo.574876
- Whitney *Roots* (1885) — public domain via CDSL
- **BDRC line-detection and woodblock models — CC BY-NC 4.0, used with BDRC's
  permission**, credited in the interface wherever output appears, all output marked
  review material

**Then be honest about the two open ones:**
> "Two things are not settled and I won't pretend otherwise.
> Geshe Michael has blessed the name in person, but I need one written line from him on
> redistribution of his glosses. And four reference layers — THL, PKTC, CIHTS, and part
> of the 84000 stack — are marked *local only* in the build and do not ship until a
> licensing line comes back. **That's a letter I'd like ALL to send, not me.**"

### Q2 · "What's machine-generated and what's authored?"

**The one-sentence answer:** *Machine work may MATCH his English from corpus evidence.
It may never COMPOSE it.*

- **105,634** unified entries. **12,004** carry an HGM gloss.
- Those are tiered — **curated > glossary > auto-aligned provisional** — and the
  provisional tier is labelled provisional on screen, in the UI, in front of the user.
- **79,305** are Lokesh Chandra layer entries, awaiting.
- Everything else on the card is stamped **reference only** with its source and licence.
- **The proof is the queue:** 225 machine-derived proposals sitting unapproved since
  August 9. Two approved rulings, signed and dated. The machine mined them; it cannot
  file them.
- Single-register terms carry register warnings and are **never auto-resolved** —
  `bsod nams` shows "goodness (prayer register)" beside "merit", and the tool refuses to
  pick.

### Q3 · "What does it cost to run, and when does it ship?"

**Cost to run: essentially nothing.**
> "No login, no account, no registration, no server, no telemetry. The dictionary, the
> corpus, the library, search, lookup, Sanskrit, OCR, the trainer, the input
> workstation — all on local disk. Unplug the network and it keeps working.
> **Exactly two features touch the network:** the Analysis pane, which sends only the
> passage you pasted to the Claude API when you press the button and costs cents per
> passage; and woodblock images from BDRC, which cache locally after the first fetch.
> That is the complete list."

**Shipping: the build exists today.**
> "`Diamond-Cutter-Translation-Tool-1.0.0-rc.1.dmg`, 424 MB, pressed this morning.
> App plus data folder, self-contained, drag-to-install."

**Four things stand between that and release — and three of them are yours:**

1. My own test campaign against the RC — mine to walk, days not weeks.
2. Authority sign-off on the data-facing surfaces — Geshe Michael or me.
3. **A clean-Mac install test** — this has only ever run on my machine.
4. **An Apple Developer ID.** Without it the app is ad-hoc signed, and on current macOS
   the first launch shows *"Apple could not verify this app is free of malware"* with
   **[Move to Trash]** and **[Done]** — and no Open button. **The signing pipeline is
   already built and dormant. It's one flag and $99 a year.**

---

## 7 · THE ASK

**Say this at the end, straight, with the list in your hand.**

> "Four things, and this ships."

### 1 · The BDRC introduction — and a service credential

- **A To: address** for the OCR-model confirmation letter, and a decision whether it
  goes from me or from ALL. *(One approach, not two.)*
- **A service credential for `iiifpres.bdrc.io`.** It moved behind Auth0 in July; the
  Scans pane returns 401 to everyone. One credential and the woodblock-follow-along
  feature is live.

### 2 · The Apple Developer ID — $99/year

- **Decision needed: does it live with ALL, or with me?**
- Until then every user's first launch is a malware warning with no Open button.
- The signing and notarization pipeline is built. It is one flag in the press script.

### 3 · A pilot user at an input centre

- **One named person. One week. Ledger on.**
- The OCR pre-fill, predictive ACIP typing, block workflow and double-keying pass are
  all built and unexercised by anyone but me.
- **It also answers whether ALL needs Windows builds** — the core is cross-platform
  clean, but nobody has asked for it yet.

### 4 · A decision on the catalog

- **Which catalog is the real one**, and is the in-house workflow the shape ALL wants —
  a roster, staged actions, and an approval channel where nothing enters the official
  tree without a named ruling?
- **And the 50 missing works from the bibliography audit** — those are already a
  concrete, exportable list.

### And one housekeeping item

- **The GitHub org.** 790+ commits currently have no offsite copy under ALL's control.

**Hand Joel the documentation package before you leave the room:**
`~/Desktop/ALL-Translation-Tool-Documentation.zip`

---

## THE LAST THING

If everything else falls apart, this is the demo:

**Open a real text. Click `shes rab`. Point at "827".**

> "The machine never invents his English.
> It only shows you what the master already said —
> and tells you exactly where he said it."
