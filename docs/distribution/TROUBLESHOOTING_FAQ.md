# ALL Translation Tool — Troubleshooting & FAQ

*If a question isn't answered here, the self-test (below) will
usually localize the problem — include its output when you ask for
help.*

---

**Q. The app won't open — macOS says it can't be verified.**

The team copy isn't signed through the App Store, so a brand-new Mac
blocks the first double-click. **Right-click (or Control-click) the
app and choose "Open"**, then confirm. macOS asks exactly once; the
app opens normally from then on.

---

**Q. The Scan pane says the recognition models are missing.**

Current DMGs ship the models inside the data folder, so a fresh
install never sees this. If you installed from an older DMG, copy
the `library/ocr_models` folder (from a current DMG's "ALL Tool
Data" folder, or from your coordinator) into your own
`ALL Tool Data/library/` folder, then relaunch.

---

**Q. The app says it can't find its data.**

The **"ALL Tool Data" folder must sit beside the app** — same
folder, side by side, exactly as they came off the DMG. If you moved
the app to Applications but left the data folder behind (or renamed
it), reunite them and relaunch. The app never guesses at a data
location: if it can't find the folder it says so and lets you point
it to the right place.

---

**Q. Something is misbehaving and I don't know what.**

Run the built-in self-test. Open Terminal (Applications →
Utilities → Terminal) and paste:

    "/Applications/ALL Translation Tool.app/Contents/MacOS/ALL Translation Tool" --selftest

(If the app lives somewhere other than Applications, adjust the
path.) **80 checks** print PASS or FAIL, one per line, covering the
data spine, every engine, and every pane's plumbing — the first FAIL
line names the broken piece, which localizes the problem
immediately. Include that output when reporting an issue.

---

**Q. The reading pages look too dark, or washed out.**

This was a display bug in an earlier build and is fixed — update to
the current DMG (see "How do I update" below).

---

**Q. Does the app need the internet?**

Almost never. Everything essential — the dictionary, the corpus,
search, reading, translation, recognition — runs entirely on your
own disk, offline. Exactly two features touch the network, both
clearly marked:

- **SCANS in the Overlay** fetches the original woodblock page
  images from BDRC (and caches them locally).
- **Research → Analysis** sends your pasted passage to the Claude
  AI service to write its report — only when you press the button,
  and the output is always banner-labeled AI.

No connection, no problem: those two features simply wait; nothing
else is affected.

---

**Q. Where do my files actually live?**

- **Settings** (your name, night mode, layer choices, remembered
  folders) live in your macOS user account, the standard way Mac
  apps remember preferences.
- **Your glossaries, manuscripts, drafts, alignment links, drill
  progress, and saved analyses** live in local files on your own
  disk — nothing is uploaded anywhere.
- **Proposals** you file go into the shared proposals folder your
  team set up (e.g. the team Dropbox).

---

**Q. How do I update to a new release?**

Open the new DMG and drag **both** the app and the "ALL Tool Data"
folder over your old ones, replacing them (keep them side by side,
as always). That's the whole update. Your settings survive — they
live in your user account, not in the replaced folders. If you keep
personal texts inside your old data folder's `library/`, copy them
across before replacing it.
