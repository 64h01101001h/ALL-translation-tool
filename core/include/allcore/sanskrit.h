// allcore/sanskrit.h — C++ port of the canonical engines/sanskrit_convert.py
// (founding rule 2: transcribed from the file, proven by battery + full
// diff-test against the Python oracle over the Lokesh Chandra IAST data).
//
// Converts between: IAST transliteration · ACIP Sanskrit code (Tibetanized) ·
// the two official ACIP standard styles (input-code prama#n%a, next-letter
// prama-n.a) · EWTS · Tibetan unicode · Devanagari · reader pronunciation.
//
// Functions mirror the Python API. Where Python returns None (IAST that fails
// to tokenize), the pair-returning functions give {"", false}; string-table
// styles (pronunciation, next-letter, input-code) never fail, exactly like the
// Python.
#pragma once

#include <string>
#include <utility>
#include <vector>

namespace allcore {

// iast_to_acip: IAST → Tibetanized ACIP Sanskrit code ("pramāṇa" → "PRA M'A nA")
std::pair<std::string, bool> iastToAcip(const std::string& iast);

// iast_to_devanagari ("oṃ maṇi padme hūṃ" → "ओं मणि पद्मे हूं")
std::pair<std::string, bool> iastToDevanagari(const std::string& iast);

// iast_to_tibetan: IAST → Tibetan-script unicode via ACIP → EWTS → unicode
std::pair<std::string, bool> iastToTibetan(const std::string& iast);

// iast_to_pronunciation: simplified reader pronunciation (table style; the
// pronunciation standard doc is still pending — provisional, as in Python)
std::string iastToPronunciation(const std::string& iast);

// iast_to_nextletter: official "next letter position" style ("prama-n.a")
std::string iastToNextletter(const std::string& iast);

// iast_to_inputcode: official ACIP Sanskrit Input Code style ("prama#n%a")
std::string iastToInputcode(const std::string& iast);

// inputcode_to_iast: reverse of the input-code style
std::string inputcodeToIast(const std::string& code);

// acip_to_iast: ACIP Sanskrit code → IAST via the EWTS bridge
std::string acipSanskritToIast(const std::string& acip);

// ---- authentic pronunciation layer (2026-08-06) ----------------------------
// Sources: the standard scholarly Classical-Sanskrit IPA mapping
// (Help:IPA/Sanskrit) and FPMT Translation Services' "A Guide to Sanskrit
// Transliteration and Pronunciation" (Nov 2020) for the reader-facing letter
// anchors and the anusvāra/visarga context rules.

// Devanagari → IAST (inverse of iastToDevanagari; original engine — validated
// by round-trip over the full LC term set). ok=false when a codepoint is not
// Sanskrit Devanagari.
std::pair<std::string, bool> devanagariToIast(const std::string& devanagari);

// IAST → Classical Sanskrit IPA. Context rules applied: anusvāra assimilates
// to the nasal of the following consonant's row (word-finally [m]; before
// sibilants/semivowels/h it nasalizes the preceding vowel); visarga = [h]
// (with its echo noted in the letter guide, not invented in the output).
std::pair<std::string, bool> iastToIpa(const std::string& iast);

// FPMT-style letter guide: for each distinct Sanskrit letter present in the
// word, one line "letter — English anchor" (ā — as in "father"; ṭ — curled-
// tongue "tra"; …). Empty for plain-ASCII letters that need no guidance.
std::vector<std::pair<std::string, std::string>> sanskritLetterGuide(
    const std::string& iast);

}  // namespace allcore
