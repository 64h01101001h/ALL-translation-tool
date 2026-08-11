# Release checklist — pressing a DMG

The operator's list for cutting a release. The machine half is
`tools/package_macos.sh` (one command; it enforces its own gates and
refuses to produce a DMG from a bad build or a bad data drop). The
human half is everything the script cannot know.

## 0. Before you run the script

- [ ] Working tree committed; batteries green locally
      (`ctest --test-dir cmake-build` — 38 suites — and
      `./ALLTranslationTool --selftest` — 80 checks).
- [ ] If the selftest or suite count changed this cycle, update the
      references in the docs that quote them (README.md,
      docs/TEST_PLAN.md, docs/RELEASE_READINESS.md, the release
      notes) — the numbers are quoted as facts, keep them true.
- [ ] Write/refresh `docs/distribution/RELEASE_NOTES_<version>.md`
      from the git history since the last press. Honest and concrete;
      end with Known limits from docs/FEATURE_STATUS.md.

## 1. Run the packaging script

    bash tools/package_macos.sh            # full press
    bash tools/package_macos.sh --skip-build   # repackage as-built

What it enforces, in order (any failure aborts — do not hand-wave
past one):

- [ ] **Release gate** — `tools/validate_release.py` must pass on the
      banked data release; the version string is read from the
      release itself.
- [ ] **Release build + batteries** — fresh Release-configuration
      build, then the full ctest battery ON THAT BUILD (not the dev
      build).
- [ ] **Staging** — the .app copied to `dist/stage/` under its
      distribution name.
- [ ] **macdeployqt + dylib fixup** — Qt frameworks bundled, then
      step 4b closes macdeployqt's two escape hatches (surviving
      /opt/homebrew LC_RPATHs; unrewritten inter-dylib @rpath refs)
      and HARD-FAILS if any homebrew rpath remains anywhere in the
      bundle. Background: docs/PACKAGING.md (the OMP Error #15
      story).
- [ ] **Ad-hoc codesign** — signed and verified (`codesign -s -`;
      right-click → Open on first launch until ALL provides a
      Developer ID).
- [ ] **Data manifest** — "ALL Tool Data/" staged beside the app:
      spine db, reference.db, `library/ocr_models` (the BDRC OCR
      models MUST ship — omitting them regresses to the
      download-it-yourself fallback), and the runtime data folders
      the panes read: fonts, honorifics, pron_colloquial,
      abbreviations, extracted, **idioms**, botok, spellcheck,
      soas_pos, whitney, candidate_alignments; plus the analysis
      template and the generated README.txt.
- [ ] **OPEN_SOURCE_NOTICES.md staged** — every release carries the
      open-source & data notices (Adam, 2026-08-11).
- [ ] **Launch test from the staged layout** — the staged app must
      come up on the real cocoa platform and find its data beside
      itself; up to 3 attempts (one observed transient flake),
      two-in-a-row failure still fails the gate with exit codes
      recorded.
- [ ] **DMG** — `dist/ALL-Translation-Tool-<version>.dmg` (UDZO).

## 2. After the script

- [ ] Regenerate the four distribution .docx with pandoc from their
      .md sources in `docs/distribution/`:

          for f in WHITE_PAPER TECHNICAL_PAPER \
                   COMPLETE_FEATURE_GUIDE OPEN_SOURCE_NOTICES; do
            pandoc docs/distribution/$f.md -o docs/distribution/$f.docx
          done

- [ ] Refresh the Desktop hand-off folder
      (`~/Desktop/ALL Tool Distribution`): the new DMG, the four
      .docx, the release notes, INSTALL_QUICK_START.
- [ ] Verify the DMG on a clean account or machine when possible —
      ideally one that never had the dev tools or Homebrew (the dev
      machine's Homebrew has masked bundling gaps before). Drag both
      items together, right-click → Open, confirm data found, then
      run the manual checklist at the end of docs/TEST_PLAN.md
      (five random panes at minimum).
- [ ] Commit the release-notes and doc updates with the press.

## If a pressed build won't launch

First diagnostic: the lifecycle log —
`~/Library/Logs/ALLTranslationTool-lifecycle.log`. It records
startup, lastWindowClosed, and aboutToQuit with timestamps, so a
silent exit-0 explains itself. Then `/tmp/all_stage_launch.log` from
the packaging launch test, then `DYLD_PRINT_LIBRARIES` per
docs/PACKAGING.md.


## Version bump (every release)

1. Edit `VERSION` at the repo root — every update increments it
   (patch for fixes, minor for features). The About box, window
   title, and DMG filename all read from it.
2. Update RELEASE_NOTES for the new number.
