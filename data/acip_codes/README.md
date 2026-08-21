# ACIP input-code inventory (banked source)

`Acipcode.txt` — found in `~/ACIP Software Developmeent` (survey
2026-08-07): the complete ACIP stack-code inventory (~430 codes) in
`[CODE ]CODE` pairs, including a separate **"ADDITIONAL CODES SUPPORTED
BY SAMBHOTA"** section. This is the substance of the Release IV
manual's input-code chart pages (153–160), which are empty image
placeholders in our copy of the PDF.

Use: the converter-completeness battery (`acipcodes_smoke`) asserts
that every alphabetic code is handled by a proven engine — the native
ACIP→EWTS→Unicode chain or the Sanskrit converter — and reports the
split. The list also serves the input-center formatter as the
authoritative code inventory.
