# APPROVE — scoped factual correction

The previous review incorrectly called English [1083,1087) the second help. Fresh read-only SQLite source access and the real generator resolver confirm that this is the FIRST help, in “without help”. The second help, in “the help you need”, is [1107,1111). Complete occurrence lists and their source contexts are recorded in scoped-re-review.json.

The Tibetan phan pa occurrences are [564,571) and [584,591). The rejected attempt w37 resolves Tibetan [564,571) and English [1083,1087): both FIRST occurrences. Its real generator rerun exits 0 and exactly reproduces the preserved rejected body. The intended second Tibetan occurrence remains [584,591). The correction withdraws the erroneous English-occurrence explanation; it does not restore the rejected disputed first-phrase mapping.

The original review.md and finalize.py are preserved byte-for-byte in pre-help-occurrence-description-correction/. Both active files now contain the corrected description. Before/after SHA-256 values are recorded below and in scoped-re-review.json.

The unchanged final C05:193,194,195 individual spec/body hashes were independently rechecked against review.json. Their existing approval is reaffirmed: 71 spans, 0 nulls, no retained errata. No broader semantic change or repository mutation was made. Machine approval remains PROVISIONAL, not human acceptance.

- review.md: before `e09c34ed8ad10e62ddea493c596bc2853ec8289ce5d0c471b87461c6f456f247`; after `ed7380ed2b7d04c6a61655732285035961a76490720732e6c187cd5f7adcb318`.
- finalize.py: before `7532dc49869987290724023f4665e0fd22d2e2424688779b01d55ff32cc9d83a`; after `506ed20ffd36b68e25b0513c28616146f6b1a2f69585c84edef8faeaefb584ca`.
