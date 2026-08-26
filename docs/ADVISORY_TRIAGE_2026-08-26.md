# Dependency-advisory triage — 2026-08-26

*The 16 "needs attention" rows from `tools/vuln_check.py --triage-all`,
each given a disposition with evidence. Method: fetch the advisory's
substance from OSV, establish version-affectedness, then apply the
app's actual attack surface — untrusted input is scanned images and
ACIP text; fonts are bundled or system; QtPdf renders only the app's
own output and one bundled dictionary PDF (verified in code, not
assumed).*

| advisory | component | disposition | evidence |
|---|---|---|---|
| ALPINE/BELL-CVE-2026-40930 (2 rows) | libpng 1.6.58 | **unreachable — resolved** | push-mode parser only; `nm -u` shows every consumer imports `png_read_image`, none `png_process_data`; bundled OpenCV has no imgcodecs at all. THREAT_MODEL updated. |
| GHSA-f698-m2v9-5fh3 | "opencv" | **not our component** | advisory is for **node-opencv**, the npm wrapper — name collision |
| GHSA-mc7w-4cjf-c973 (CVE-2019-10061) | "opencv" | **not our component** | ditto |
| DEBIAN-CVE-2026-50593 | graphite2 1.3.15 | **fixed in our version** | "Graphite **before 1.3.15**"; we ship 1.3.15 exactly |
| UBUNTU-CVE-2023-25193 | harfbuzz 14.3.0 | **fixed in our version** | "through 6.0.0"; we ship 14.3.0. DoS-only besides, and fonts are trusted |
| RHSA-2026:36841 · RHSA-2026:7933 | freetype | **packaging noise** | "Red Hat Hardened Images RPM" rebuilds, not upstream vulns |
| RHSA-2026:7647 | xz | **packaging noise** | ditto |
| openSUSE-SU-2026:10492-1 | xz | **packaging noise** | distro rebuild of 5.8.3 |
| openSUSE-SU-2026:10982-1 | graphite2 | **packaging noise** | distro rebuild of 1.3.15 |
| DEBIAN-CVE-2026-50811 | freetype 2.14.3 | **affected version, unreachable in our usage — WATCH** | OOB read is real in 2.14.3; but freetype here renders bundled OFL fonts, system fonts, and fonts inside PDFs *we generate* — no attacker-controlled font path exists (QPdfDocument load sites traced: selftest temp files, pechaWritePdf output, bundled Jäschke 1881). Upgrade when brew ships the fixed commit. |
| CVE-2026-58016 (BELL) | glib 2.88.2 | **unreachable in our usage** | flaw is in `g_dbus_node_info_new_for_xml` — D-Bus introspection parsing; nothing feeds it untrusted XML here |
| BELL-CVE-2026-15588 | glib | **undetermined — WATCH** | not in OSV mainline; no substance retrievable yet |
| DEBIAN-CVE-2021-4048 | openblas 0.3.34 | **unreachable in our usage** | OOB read in LAPACK ?LARRV (tridiagonal eigenvectors); our OpenCV use is imgproc on self-produced frames — no eigen path on untrusted data |
| MGASA-2026-0246 (CVE-2026-36849) | libtiff 4.7.2 | **REAL, accepted for now — WATCH** | DoS via large SamplesPerPixel; the app opens user TIFF scans through Qt's tiff plugin, so a malicious scan can crash it. Impact is local DoS, no code execution. No fixed brew version exists yet (stable is 4.7.2). Re-check: `brew info libtiff`. |

## Net position

- **0 advisories require action today.** None is an exploitable-for-code-execution path in this app's usage.
- **3 on WATCH**, with the re-check written down: freetype (upgrade when fixed build ships), libtiff (ditto — the one real user-facing risk, DoS on a malicious scan), glib BELL-15588 (substance not yet published).
- The Joel-notes line "16 advisories needing judgement" is now
  "16 judged; 3 on watch; 0 requiring action."

*Re-run after any dependency upgrade: `python3 tools/vuln_check.py
--triage-all "dist/stage/Diamond Cutter Translation Tool.app"` and
reconcile against this table.*
