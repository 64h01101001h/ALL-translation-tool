#!/usr/bin/env python3
"""check_draft_attachment.py - did the attachment we *sent* match the file?

WHY THIS EXISTS. The Gmail connector accepts an attachment only as base64
inline in the tool call; there is no path parameter. A 15 KB digest .docx is
~20,500 base64 characters, and that does not reliably survive being retyped
through a model. On 2026-09-17 digest #9 was attached twice and truncated both
times - at 9,690 characters of 20,528, then at ~1,100 - with no error either
time.

Truncation is invisible from every direction you would naturally look:

  * a truncated ZIP still starts with the `PK` magic bytes, so the send is
    accepted and the draft shows an attachment of a plausible size;
  * `get_draft` does not report a draft's attachments at all - verified
    against a 36-character attachment that DID land - so its silence means
    nothing either way.

The only thing that settles it is comparing the bytes that were actually sent
against the bytes on disk. The sent payload is recorded in the session
transcript, which is what this reads. A file that "looks attached" and will
not open is worse than no attachment: it fails at the far end, in front of
whoever the digest went to.

THE BODY HAS THE SAME PROBLEM. The draft's text is retyped through the model
too, and a digest body runs to ~13,000 characters - larger than the payload
that truncated. A dropped or altered sentence in prose is worse than a broken
attachment, because it does not look broken: it goes out over Adam's name to
leadership reading it as his words. So --body checks the text the same way,
against the file it was built from.

Usage:
    python3 tools/check_draft_attachment.py <transcript.jsonl> <file> [...]
    python3 tools/check_draft_attachment.py <transcript.jsonl> --body <file>

Exit 0 only if what was sent matches the named file exactly.
"""
import base64
import hashlib
import io
import json
import os
import sys


def bodies(transcript):
    """Every draft body handed to a draft tool, in order."""
    out = []
    with io.open(transcript, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            if '"body"' not in line:
                continue
            try:
                rec = json.loads(line)
            except ValueError:
                continue
            msg = rec.get("message") or {}
            for c in msg.get("content") or []:
                if not isinstance(c, dict) or c.get("type") != "tool_use":
                    continue
                if "draft" not in (c.get("name") or ""):
                    continue
                b = (c.get("input") or {}).get("body")
                if isinstance(b, str) and b:
                    out.append((c.get("name"), b))
    return out


def check_body(transcript, path):
    want = io.open(path, encoding="utf-8").read()
    sent = bodies(transcript)
    print("draft bodies found in transcript: %d" % len(sent))
    print("the file on disk: %d characters" % len(want))
    if not sent:
        print("  nothing was ever sent")
        return 1
    tool, got = sent[-1]
    # Trailing whitespace is not a difference worth failing on; a dropped
    # sentence is. Compare the text proper.
    a, b = got.rstrip(), want.rstrip()
    if a == b:
        print("  the last body (%s): %d chars, IDENTICAL" % (tool, len(got)))
        return 0
    k = 0
    lim = min(len(a), len(b))
    while k < lim and a[k] == b[k]:
        k += 1
    print("  the last body (%s): %d chars, DIVERGES after %d of %d"
          % (tool, len(got), k, len(b)))
    print("  file has : %r" % b[k:k + 90])
    print("  sent has : %r" % a[k:k + 90])
    print("  do not treat this draft as the digest")
    return 1


def payloads(transcript):
    """Every attachment payload handed to a draft tool, newest last."""
    out = []
    with io.open(transcript, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            if "attachments" not in line:
                continue
            try:
                rec = json.loads(line)
            except ValueError:
                continue
            msg = rec.get("message") or {}
            for c in msg.get("content") or []:
                if not isinstance(c, dict) or c.get("type") != "tool_use":
                    continue
                if "draft" not in (c.get("name") or ""):
                    continue
                atts = (c.get("input") or {}).get("attachments")
                if isinstance(atts, str):          # some clients stringify it
                    try:
                        atts = json.loads(atts)
                    except ValueError:
                        continue
                for a in atts or []:
                    if isinstance(a, dict) and a.get("content"):
                        out.append((c.get("name"), a))
    return out


def main(argv):
    if len(argv) < 3:
        sys.stderr.write(__doc__)
        return 2
    transcript, files = argv[1], argv[2:]
    if files and files[0] == "--body":
        return check_body(transcript, files[1])
    sent = payloads(transcript)
    print("attachment payloads found in transcript: %d" % len(sent))
    if not sent:
        print("  nothing was ever sent - the draft has no attachment")
        return 1

    ok = False
    for path in files:
        raw = io.open(path, "rb").read()
        want = base64.b64encode(raw).decode()
        name = os.path.basename(path)
        print("\n%s" % name)
        print("  on disk: %d bytes, %d base64 chars, sha256 %s"
              % (len(raw), len(want), hashlib.sha256(raw).hexdigest()[:16]))
        for n, (tool, a) in enumerate(sent):
            got = (a.get("content") or "").strip()
            if a.get("filename") and a["filename"] != name:
                continue
            k = 0
            lim = min(len(got), len(want))
            while k < lim and got[k] == want[k]:
                k += 1
            same = got == want
            ok = ok or same
            print("  payload %d (%s): %d chars, %s"
                  % (n, tool, len(got),
                     "IDENTICAL" if same
                     else "TRUNCATED/ALTERED after %d of %d chars"
                          % (k, len(want))))
            if not same:
                try:
                    dec = base64.b64decode(got, validate=False)
                    print("      decodes to %d of %d bytes; starts with PK: %s"
                          " - it will look like a real file and will not open"
                          % (len(dec), len(raw), dec[:2] == b"PK"))
                except Exception as exc:
                    print("      does not even decode: %s" % exc)
    print("\nverdict: %s" % ("a correct copy was sent" if ok else
                             "NO correct copy was sent - do not claim it is attached"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
