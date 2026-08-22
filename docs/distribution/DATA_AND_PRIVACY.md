# Diamond Cutter Translation Tool — Data & Privacy

*A plain statement of what leaves your computer. Short version:
almost nothing, and never without you pressing a button.*

## Everything essential runs on your own disk

The dictionary, the aligned corpus, the Library's texts, search,
lookup, the Sanskrit stack, text recognition, the trainer and
drills, the input workstation — all of it runs locally, offline,
from the "Diamond Cutter Tool Data" folder beside the app. Unplug the network
and the tool keeps working.

## Exactly two features touch the network

1. **The Analysis pane (Research → Analysis).** When you press the
   button, the passage you pasted is sent to the Claude API (an AI
   service run by Anthropic) to write the grammatical report. That
   is the only thing sent, it is sent only when you press the
   button, and it costs cents per passage. The result is always
   banner-labeled as AI output.

2. **Woodblock images from BDRC.** The Overlay's SCANS feature
   fetches the original page images from BDRC's public archive.
   Fetched pages are cached locally, so a page you've seen once
   doesn't need the network again.

That is the complete list. Nothing else in the application makes a
network connection.

## No accounts, no telemetry, no server

There is no login, no account, no registration. The application
sends no usage data, no analytics, no crash reports — nothing —
to anyone. There is no server behind the tool at all.

## The shared proposals folder is just a folder

Team proposals travel through a plain shared folder that the team
itself controls — typically the team Dropbox. The tool writes
proposal files into it and reads rulings out of it; no third party
is involved beyond whatever folder-sharing service the team chose.
Each proposal carries the proposer's name (the name you entered in
Settings) — that is deliberate provenance, so the authority always
knows who proposed what, and every approved item records who ruled
and when.

## Your own work stays yours

Your manuscripts, drafts, per-text glossaries, alignment links,
drill progress, and saved analyses are local files on your own
disk. They are never uploaded, synchronized, or shared unless you
yourself put them somewhere shared.

## The one log file

The application keeps a single lifecycle log at

    ~/Library/Logs/DiamondCutterTranslationTool-lifecycle.log

It records only launch and quit timestamps — nothing about what you
read, typed, searched, or translated. You can open it in any text
editor and see for yourself; you can delete it at any time.
