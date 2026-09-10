# Getting the Tibetan Trainer onto your iPhone

*Written 2026-09-10. Everything on the machine side is done. What is left are
two actions only you can take, because both need your Apple ID.*

## Where things stand

| | |
|---|---|
| Xcode 26.6 | installed, and selected |
| The app's source | compiles clean for iOS |
| **`ios/TibetanTrainer.xcodeproj`** | **built and opens — this is new** |
| Your iPhone (བདེ་མཆོག་, iPhone 16 Pro Max) | connected and paired already |
| The drill pack | fresh: 4,000 drills + 1,000 trainer passages, 5.7 MB |
| Simulator build through the project | **BUILD SUCCEEDED**, app installed and both modes verified |
| Signing certificate on THIS Mac | **still none — the one remaining step** |
| Xcode's iOS 26.5 platform | installed ✓ — your phone is now an available destination |
| Apple Developer Program | **paid, active** — enrolled 10 Sep 2026, renews 2027 |
| Team ID | on file at `ios/DeveloperTeam.txt`, already in the project |

## ~~Action 1 — the iOS 26.5 platform~~  DONE

## The one remaining step — sign in to Xcode

A certificate existing in your **account** is not the same as one existing in
**this Mac's keychain**, and it is the keychain that signs. `security
find-identity` still reports 0 valid identities here.

Do not create the certificate in the web portal. A certificate made there is
bound to a private key that must already exist on the machine, and going that
route is how people end up with a certificate they cannot use. Let Xcode do
it: it generates the key pair locally and keeps the private key where the
signer can reach it.

The keychain has no signing identity, so nothing can be installed on a real
device yet. You do not need to create certificates by hand; Xcode does it once
it knows who you are.

1. **Xcode ▸ Settings ▸ Accounts ▸ +  ▸ Apple ID** — sign in.
2. Your team appears in the list. Note whether it says **(Personal Team)** or
   your organisation's name — that distinction decides what is possible below.

## Then: run it

1. Open `ios/TibetanTrainer.xcodeproj`.
2. Select the **TibetanTrainer** target ▸ **Signing & Capabilities**.
3. Leave *Automatically manage signing* ticked, and choose your **Team**.
   Xcode mints the certificate and the provisioning profile itself.
4. Choose **བདེ་མཆོག་** as the run destination, top of the window.
5. Press **▶**.
6. First run only, the phone will refuse to open it: **Settings ▸ General ▸
   VPN & Device Management ▸ [your Apple ID] ▸ Trust**.

## Your Team ID — done

`R2A2V4U62T` is on file at `ios/DeveloperTeam.txt` and is already written into
the project, so it survives every regeneration and the TestFlight archive step
has what it needs. That file is gitignored: a Team ID is not a secret — it is
stamped into every provisioning profile Apple issues — but this repository is
public, so it stays out of the tree.

### What never to send

An Apple ID password, an App Store Connect API key (the `.p8` file, its Key ID
or Issuer ID), or a 2FA code. None are needed to build this, none belong in
this repository, and nothing here will ask for them. Signing happens entirely
inside your own Xcode session.

## You are on the paid programme, so TestFlight is live

Enrolled **10 September 2026** as an **Individual**, renewing 2027. That
settles the question that was open this morning:

| | |
|---|---|
| On your own phone | yes, and the build lasts a year rather than 7 days |
| Apps at once | unlimited |
| TestFlight to the team and students | **yes** |
| App Store | available, though we have ruled against it for now |

Two consequences worth knowing before we lean on it.

**Enrolled as an Individual, not an Organization.** TestFlight works exactly
the same either way. What differs is the name a tester sees as the publisher —
yours, rather than Asian Legacy Library. Switching to an Organization later
needs a D-U-N-S number for ALL and is a separate piece of paperwork; it is not
a blocker for anything we want to do now, but it is a decision that gets more
awkward the longer an app has been published under a personal name. Worth
raising with John Brady rather than deciding alone.

**The device list resets 10 September 2027.** Registered test devices can only
be removed once a year, on that date. It costs nothing now; it matters when a
hundred students have come and gone.

## Why the project file is generated

`ios/TibetanTrainer.xcodeproj` is produced by `python3 tools/make_ios_project.py`
rather than hand-built. An Xcode project is a merge-hostile file that Xcode
rewrites whenever you click something incidental; generating it keeps the build
settings readable in one script, and a project mangled by a stray drag can be
thrown away and remade in a second. It uses a *synchronized folder*, so adding
a Swift file or an image to `ios/DiamondDrills/` needs no project edit at all.

If Xcode ever asks to "update to recommended settings", let it, then run the
script again and keep whichever you prefer — the script is the source of truth.

## One thing that is decided and should not drift

The bundle identifier is **`org.asianlegacylibrary.TibetanTrainer`**. It was
`...DiamondDrills`, which was the old name from when the app was only the
drills. Changing it now costs nothing because nothing has been registered with
Apple yet. After the first TestFlight upload it is **permanent**, so it was
worth fixing today rather than living with it.

## What is NOT in this app, deliberately

The Kawachen recordings. Kawachen's permission covers in-house use, and
TestFlight puts a build in students' hands, which is distribution. The phone
app stays silent until we ask them. See `docs/KAWACHEN_READER_FORMAT.md`.
