# Licence texts to ship with the app

`tools/bundle_licenses.py` copies each bundled component's licence text
out of its Homebrew keg into
`…app/Contents/Resources/licenses/<component>/`. Most kegs carry one.
**Three do not:** Homebrew's `qt`, `qtimageformats` and
`qtvirtualkeyboard` kegs contain no licence file at all, so every DMG so
far has shipped Qt without the LGPL-3.0 text that LGPL-3.0 §4 requires
be supplied with the distribution. Those rows read
`LICENSE_TEXT_MISSING` in `BUNDLED_COMPONENTS.tsv` and the press prints
them each time — flagged, not hidden.

## Filling the gap

Drop the official text in this directory, named for its SPDX id:

| file | where to get it |
|---|---|
| `LGPL-3.0-only.txt` | <https://www.gnu.org/licenses/lgpl-3.0.txt> |
| `GPL-3.0-only.txt` | <https://www.gnu.org/licenses/gpl-3.0.txt> |
| `GPL-2.0-only.txt` | <https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt> |
| `BSD-3-Clause.txt` | Qt's own `LICENSES/BSD-3-Clause.txt` in a Qt source tarball |

`bundle_licenses.py` picks up any file here whose name matches an
identifier in that component's SPDX expression, and ships it. Once
`LGPL-3.0-only.txt` is present, `qt` stops reporting a missing text.

**Download them; do not transcribe them.** A licence text reconstructed
from memory is not the licence text, and the whole point of shipping it
is that it is exact. Verify the file you saved is complete — the LGPL-3.0
text ends with the GPL-3.0 reference, not mid-paragraph.

A market-mode press runs `bundle_licenses.py --require-texts` and will
refuse to build until this directory has what the bundle needs.
