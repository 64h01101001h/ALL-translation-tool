# Brewfile — the build dependencies, declared instead of remembered.
#
# SQA 2026-08-22, BUILD-6: there was no lockfile of any kind, so the
# release was a snapshot of one laptop's Homebrew and no two presses were
# guaranteed to produce the same artifact.
#
#   brew bundle --file=Brewfile          # install / verify
#   brew bundle check --file=Brewfile    # "are they all here?"
#   brew bundle dump --file=Brewfile --force   # regenerate (careful: dumps
#                                              # everything installed)
#
# Homebrew pins by *lockfile*, not by line: `brew bundle` writes
# Brewfile.lock.json beside this file with the exact versions it
# resolved. Commit that file with the release commit — it and the DMG's
# BUILD_MANIFEST.txt (tools/build_manifest.py) are the two records that
# answer "which OpenSSL did release X carry?".
#
# Only DIRECT dependencies belong here. The ~36 upstream projects that
# actually ship inside the DMG arrive transitively; they are enumerated,
# with version and SPDX licence, in BUILD_MANIFEST.txt.

# Toolchain
brew "cmake"

# The UI. Qt 6 also supplies the HarfBuzz that shapes Tibetan, and
# macdeployqt, which the press uses to bundle the frameworks.
brew "qt"

# The Scan pane (allocr). Both are optional at configure time — without
# them ocr/CMakeLists.txt skips allocr and the app omits the pane — but
# a release build is expected to have them.
brew "onnxruntime"
brew "opencv"

# Optional, documented in docs/DEVELOPER_ONBOARDING.md: .docx generation
# for the gallery/overview builders. Not linked into the app.
# brew "pandoc"
#
# NOT a dependency, deliberately: sqlite. allcore links macOS's own
# /usr/lib/libsqlite3.dylib (which ships FTS5). CLAUDE.md and
# docs/PACKAGING.md still describe a vendored amalgamation — that is not
# true today and is tracked as part of BUILD-6.
