# TestFlight — everything ready but the signature

Adam chose TestFlight over a public App Store listing on 2026-09-09, so the
build reaches the team and the Mixed Nuts students without putting unreviewed
renderings of Geshe Michael Roach's work on a public store before he has ruled
on them.

## Blocked on exactly one thing

No signing certificate exists in the keychain (`security find-identity -v
-p codesigning` → 0). Adam creates it in **Xcode ▸ Settings ▸ Accounts ▸
[team] ▸ Manage Certificates ▸ + ▸ Apple Development**, then sends the
**Team ID**. Everything else below is done.

## Done

- App: Trainer and Drills modes, 4,000 drills, 1,000 passages, 5.7 MB pack.
- Icon: ཆོས on manuscript cream with a pecha rule, compiled to `Assets.car`,
  legible at 60 px, inside Apple's mask.
- Bundle identifier: `org.asianlegacylibrary.DiamondDrills`.

## The metadata TestFlight asks for

**Beta App Description**

> The Learn tab of the Diamond Cutter Translation Tool, on your phone.
>
> **Trainer** — read a passage of Tibetan yourself, then reveal one layer at a
> time: the chunks, what each particle is doing, the order to read them in, the
> verb that frames the clause, Geshe Michael Roach's English, and the full
> parse.
>
> **Drills** — a chunk is blanked in a real passage; choose which of four fills
> it, and see the grammatical role it was playing.
>
> Every passage and every English line is Geshe Michael Roach's own text from
> his courses. Engine guidance is labelled as guidance. Works entirely offline;
> nothing is collected and nothing leaves the phone.

**What to Test**

> Does the Tibetan render correctly on your device? Are the drills at a useful
> level — too easy, too hard, or about right? Does the six-layer reveal in
> Trainer teach you anything you did not already see? Anything that looks
> wrong in the Tibetan or the English, please screenshot it.
>
> Known and being worked on: about a fifth of drills show a syllable in ⟨angle
> brackets⟩. That is deliberate — it means our converter could not read that
> form and we would rather flag it than guess. The list of forms is known and
> shrinking.

**Beta App Review** — required because we will use external testers. Answer:
sign-in not required, no demo account needed, contact is Adam.

**Privacy** — Data Not Collected. True: no network calls, no analytics, and
progress is stored on the device only.

**Export compliance** — no encryption.

## Build and upload, once the certificate exists

```bash
bash ios/testflight/build.sh <TEAM_ID>
```

That archives for the device, signs against the team, and produces the `.ipa`
to upload with Transporter or `xcrun altool`.
