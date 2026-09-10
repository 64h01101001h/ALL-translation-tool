# Getting Tibetan Translation Trainer onto the App Store

Written 2026-09-09. Everything marked **YOU** needs your Apple ID or your
password and cannot be done for you. Everything marked **ME** I can do once the
YOU items are in place.

---

## Where we actually are tonight

| | |
|---|---|
| App builds and runs | ✅ Trainer and Drills modes, 4,000 drills, 1,000 passages |
| `sudo xcode-select` | ✅ done |
| Developer Program | ✅ you purchased it |
| iPhone connected and trusted | ✅ your 16 Pro Max is visible over the cable |
| **Signing certificate in your keychain** | ❌ **0 found — this is the blocker** |
| App icon | ❌ not made yet |
| App Store Connect record | ❌ not created |
| Privacy policy URL | ❌ needed, we have none |

---

## Step 1 — the certificate (YOU, 3 minutes)

**Xcode ▸ Settings ▸ Accounts** → **+** → **Apple ID** → sign in. Then select
your team → **Manage Certificates…** → **+** → **Apple Development**.

Then send me your **Team ID** — ten characters like `ABCDE12345`, shown beside
your team name. It is not a secret. It is the only thing I need from you.

With that I can sign and install over the cable, and the app is on your phone.
**That gets it on YOUR phone. The App Store is the rest of this document.**

## Step 2 — the icon (ME, then YOU approve)

Apple requires a 1024×1024 icon with no transparency and no rounded corners.
I will draft something in the manuscript palette — the app is a scholarly tool,
not a game — and you tell me if it is right.

## Step 3 — App Store Connect record (YOU, ~15 minutes)

At **appstoreconnect.apple.com** → My Apps → **+** → New App:

- **Platform** iOS · **Name** Tibetan Translation Trainer (must be unique across
  the whole store — have a second choice ready)
- **Primary language** English (U.S.)
- **Bundle ID** — I will give you the exact string to register first at
  developer.apple.com → Identifiers
- **SKU** anything, e.g. `TTT-001`

## Step 4 — the two questionnaires (YOU, ~10 minutes)

**App Privacy.** Ours is the easy case: the app collects nothing, sends nothing,
has no analytics and no network calls at all. Answer **"Data Not Collected"**.
That earns the best possible privacy label, and it is true.

**Export compliance.** The app uses no encryption. Answer **no** to the
encryption question. (Say yes only if that changes.)

**Age rating.** Straightforward, but there is a religious-content question —
answer it honestly; it will not restrict the rating meaningfully.

## Step 5 — privacy policy (YOU, or ME to draft)

Apple requires a **public URL**, even for an app that collects nothing. If
asianlegacylibrary.org can host a page, I will write it — it will be four
sentences saying the app collects nothing and stores your progress only on your
own device.

## Step 6 — screenshots (ME)

Required at 6.9" and 6.5". I can generate them from the simulator.

## Step 7 — build, sign, upload (ME, once step 1 and 3 are done)

Archive, sign with your team, upload to App Store Connect, and it appears for
you to submit.

## Step 8 — submit and wait (YOU click submit)

Review is typically 24–48 hours now.

---

## Two things you must decide before we submit, and I will not decide them for you

**1. The content rights.** The app ships Geshe Michael Roach's copyrighted
English translations — 4,000 drills and 1,000 passages of them — under an
Asian Legacy Library account, on a public store, worldwide. Apple makes you
attest that you hold the rights. You almost certainly do, but this should be an
explicit decision with whoever holds them, not an assumption made at eleven at
night by me.

**2. The tier.** Every alignment in this app is `TENTATIVE (machine-matched,
unreviewed)` and awaiting Geshe Michael Roach's ruling. The desktop says so
plainly because its audience is a small team who understand what that means.
The App Store audience is the public. Three options, and it is your call:

- ship as is, with the tier label prominent in the app and in the description;
- ship only the drills whose material has been reviewed, which is a much
  smaller app but an unimpeachable one;
- ship as **TestFlight** first — up to 10,000 external testers, no public
  listing, no review of the content claim — and let the team and the Mixed Nuts
  students use it while Geshe Michael Roach rules.

**My recommendation is TestFlight.** It gets the app into your students' hands
this week, it is the same build and the same upload, it costs nothing extra,
and it does not put unreviewed renderings of a teacher's work on a public store
before he has seen them. Nothing is lost: the App Store submission is the same
artefact whenever you want it.
