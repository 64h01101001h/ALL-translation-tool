# Getting the Drills app onto your iPhone — your four steps

Everything here needs your password or your Apple ID, which is why it is yours
and not mine. Total time: about ten minutes, most of it Apple's checkout page.
Do them in order. After step 4, tell me your iPhone model and iOS version and I
take it from there.

---

## Step 1 — point the tools at Xcode (30 seconds)

Xcode 26.6 is already installed. Your Mac is just pointing at the smaller
command-line tools instead of at it. One command, then your password:

```bash
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
```

Check it worked — this should print a path ending in `Xcode.app/Contents/Developer`:

```bash
xcode-select -p
```

## Step 2 — the Apple Developer Program ($99/year)

Go to **developer.apple.com/programs/enroll**, sign in with your Apple ID, and
enrol as an **Individual** (unless you want it in Asian Legacy Library's name,
which needs a D-U-N-S number and takes days — start Individual, we can move it
later).

Apple usually approves within minutes to 48 hours. **You do not have to wait
for approval to keep going.**

*Why paid rather than free:* a free Apple ID can put the app on your own phone,
but it stops working after 7 days and you have to reinstall it. The paid
account signs for a year and is what lets you put this on the input centres'
and the students' phones later.

## Step 3 — sign in inside Xcode (2 minutes)

1. Open **Xcode**.
2. **Xcode ▸ Settings ▸ Accounts** (or ⌘ ,).
3. Click **+**, choose **Apple ID**, sign in with the same Apple ID.
4. You should see your name with a team under it. If it says *Personal Team*,
   that is the free tier and it is fine to start with — it will change to your
   paid team once Apple approves.

## Step 4 — the iPhone itself (2 minutes)

1. Plug the iPhone into the Mac with a cable. Unlock it, and tap **Trust This
   Computer**, then enter your phone passcode.
2. On the iPhone: **Settings ▸ Privacy & Security**, scroll to the bottom, turn
   on **Developer Mode**. The phone will restart and ask you to confirm.
3. Leave it plugged in.

---

## Then tell me two things

- **Which iPhone** (Settings ▸ General ▸ About ▸ Model Name).
- **Which iOS version** (same screen, Software Version).

That is all I need. I will build it, sign it against your team, install it over
the cable, and it will be sitting on your home screen.

---

## What you will have in the morning

The drills, running natively, offline, with the Tibetan rendering properly —
and everything today's fixes put right: the whole segment shown against the
English rather than a quarter of it, no ACIP leaking into the script, title
lines badged as titles, and the title catalogue kept out of the draw entirely.

## If something goes wrong

- **"xcode-select: error: tool 'xcodebuild' requires Xcode"** — step 1 did not
  take. Run it again and check with `xcode-select -p`.
- **Xcode wants to install extra components on first launch** — let it, it is a
  few minutes and only happens once.
- **The phone does not show Developer Mode** — it only appears after a Mac has
  tried to install something, or after you plug in and trust. Do step 4.1
  first, then look again.
