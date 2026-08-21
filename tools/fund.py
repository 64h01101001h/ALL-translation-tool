#!/usr/bin/env python3
"""Donation tracker for transcriptionfund.org.

The site is static hosting, so there is no server to receive a
webhook and no API that will tell us what landed in a personal
PayPal, Venmo or Zelle account. This is therefore an honest LEDGER
rather than a live feed: gifts are recorded here, and the pages read
the total.

    python3 tools/fund.py status
    python3 tools/fund.py add 50 --via venmo --date 2026-08-16
    python3 tools/fund.py add 25 --via zelle --note "from the retreat"
    python3 tools/fund.py set 1234.56          # correct the total outright

Every command rewrites BOTH:
  · donations.json  - the source of truth the pages fetch, and
  · the numbers baked into the two HTML files, which are what shows
    if the fetch ever fails.

Keeping those two in step is the whole point of this script; editing
one and forgetting the other is how a fundraiser ends up showing two
different totals on the same page.

After running it, re-upload the deploy folder to Cloudflare Pages.
"""

import argparse
import io
import json
import os
import re
import sys

SITE = os.path.expanduser("~/Desktop/transcription-fund-site")
LEDGER = os.path.join(SITE, "donations.json")
PAGES = [os.path.join(SITE, "index.html"),
         os.path.join(SITE, "present", "index.html")]


def money(n):
    return "{:,.2f}".format(n)


def load():
    if not os.path.exists(LEDGER):
        sys.exit("no donations.json at " + LEDGER)
    with io.open(LEDGER, encoding="utf-8") as f:
        return json.load(f)


def total(d):
    return round(sum(g.get("amount", 0) for g in d.get("gifts", [])), 2)


def save(d, when):
    d["raised"] = total(d)
    d["updated"] = when
    with io.open(LEDGER, "w", encoding="utf-8") as f:
        f.write(json.dumps(d, indent=2, ensure_ascii=False) + "\n")
    return d


def repage(d):
    """Rewrite the baked-in fallback numbers in both pages."""
    raised = d["raised"]
    goal = d.get("goal_phase_one", 5000)
    togo = max(0.0, goal - raised)
    pct = max(0.0, min(100.0, raised / goal * 100)) if goal else 0.0
    touched = []
    for p in PAGES:
        if not os.path.exists(p):
            continue
        with io.open(p, encoding="utf-8") as f:
            s = f.read()
        orig = s
        # the raised / to-go sentence, whichever page idiom it uses
        s = re.sub(r"<b>\$[\d,]+\.\d\d(?: raised)?</b>",
                   lambda m: ("<b>$" + money(raised) +
                              (" raised</b>" if "raised" in m.group(0)
                               else "</b>")), s, count=1)
        s = re.sub(r"\$[\d,]+\.\d\d to go\.",
                   "$" + money(togo) + " to go.", s, count=1)
        # the bar widths (inline style on the sober page, JS on the other)
        s = re.sub(r'(id="fill"[^>]*style="width:)[\d.]+%',
                   lambda m: m.group(1) + "{:.2f}%".format(pct), s, count=1)
        s = re.sub(r"var w = \([\d.]+ / \d+ \* 100\)",
                   "var w = ({:.2f} / {} * 100)".format(raised, goal),
                   s, count=1)
        if s != orig:
            with io.open(p, "w", encoding="utf-8") as f:
                f.write(s)
            touched.append(os.path.relpath(p, SITE))
        # the pure-ASCII rule that fixed the mojibake still holds
        bad = sum(1 for c in s if ord(c) > 127)
        if bad:
            print("  WARNING: {} now has {} non-ASCII char(s)"
                  .format(os.path.relpath(p, SITE), bad))
    return touched


def show(d):
    goal = d.get("goal_phase_one", 5000)
    raised = d["raised"]
    pct = raised / goal * 100 if goal else 0
    bar = int(round(pct / 100 * 32))
    print("\n  Phase One  [{}{}]  {:.1f}%".format(
        "#" * bar, "." * (32 - bar), pct))
    print("  ${} of ${:,}   (${} to go)".format(
        money(raised), goal, money(max(0.0, goal - raised))))
    print("  {} gift(s), last updated {}\n".format(
        len(d.get("gifts", [])), d.get("updated", "?")))
    for g in d.get("gifts", [])[-8:]:
        print("    {}  ${:>10}  {:<8} {}".format(
            g.get("date", "?"), money(g.get("amount", 0)),
            g.get("via", ""), g.get("note", "")))
    print()


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("status", help="show the current total")
    a = sub.add_parser("add", help="record a gift")
    a.add_argument("amount", type=float)
    a.add_argument("--via", default="", help="paypal / venmo / zelle / cash")
    a.add_argument("--date", default="", help="YYYY-MM-DD (required)")
    a.add_argument("--note", default="")
    s = sub.add_parser("set", help="overwrite the total with one figure")
    s.add_argument("amount", type=float)
    s.add_argument("--date", default="", help="YYYY-MM-DD (required)")
    args = ap.parse_args()

    d = load()
    if args.cmd == "status":
        d["raised"] = total(d)
        show(d)
        return

    if not args.date:
        sys.exit("pass --date YYYY-MM-DD (this script has no clock of its "
                 "own, and a guessed date in a donation ledger is worse "
                 "than no date)")

    if args.cmd == "add":
        d.setdefault("gifts", []).append({
            "date": args.date, "amount": round(args.amount, 2),
            "via": args.via, "note": args.note})
    else:  # set
        d["gifts"] = [{"date": args.date, "amount": round(args.amount, 2),
                       "via": "adjusted",
                       "note": "total set directly, replacing the itemised "
                               "list"}]
    d = save(d, args.date)
    touched = repage(d)
    show(d)
    print("  rewrote: donations.json" +
          ("" if not touched else ", " + ", ".join(touched)))
    print("  now re-upload ~/Desktop/transcription-fund-site to "
          "Cloudflare Pages.\n")


if __name__ == "__main__":
    main()
