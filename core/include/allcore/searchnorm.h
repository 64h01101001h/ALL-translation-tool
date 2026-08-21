#pragma once
// searchnorm.h — the rest of the lucene-bo analyzer family (Adam
// approved full inclusion 2026-08-07), integrated where our pipeline
// needs it:
//
//  * paBaFoldWylie — PaBaFilter: standalone ba/bo fold to pa/po,
//    applied AFTER affix stripping, exactly as upstream chains it.
//  * LemmaFold — the TibSyllableLemmatizer capability driven by OUR
//    CC0 verbs bank (data/extracted/verb_lemmas.tsv): unambiguous
//    past/future/imperative forms fold to their present stem.
//    Ambiguous forms were skipped at table build, never guessed.
//  * normalizeTibetanUnicode — TibCharFilter (strict + lenient maps)
//    + TibPattFilter (vowel/subscript reorder, Old-Tibetan merged-
//    syllable splits 1–3 as upstream defaults, Sanskrit ts→c folds).
//    For UNICODE entry points: attested-layer lookups, future OCR
//    text. (MergedSylFilter4 is disabled upstream; mirrored here.)
//
// Deliberately NOT ported (covered natively): TibEwtsFilter (our
// proven converters), TibSyllableTokenizer (we tokenize in wylie
// space; FTS tokenizers configured to match). The phonetics family is
// queued as its own feature (pronunciation search).
//
// Fold direction is canonical (ba→pa, non-present→present) on the
// INDEX side; a query in canonical form reaches every variant.

#include <string>
#include <unordered_map>

namespace allcore {

// standalone ba → pa, bo → po (post-affix-strip semantics)
std::string paBaFoldWylie(const std::string& token);

class LemmaFold {
public:
    bool load(const std::string& tsv_path);   // verb_lemmas.tsv
    // non-present verb form -> its unique present stem ("" = no fold)
    const std::string* fold(const std::string& wylie) const;
    size_t size() const { return map_.size(); }

private:
    std::unordered_map<std::string, std::string> map_;
};

// process-wide fold used by the index builders (wynorm, libindex);
// call once at startup where the table's path is known. Unconfigured
// = affix+paba only.
void configureLemmaFold(const std::string& tsv_path);
const LemmaFold* globalLemmaFold();

// full per-token index fold: affix strip → paba → lemma (if loaded)
std::string searchFoldWylie(const std::string& token);

// lucene-bo unicode normalization (see header comment). lenient adds
// the search-lenience map (retroflex folding, Sanskrit stack
// respellings such as པདྨ → པད་མ).
std::string normalizeTibetanUnicode(const std::string& utf8,
                                    bool lenient = true);

}  // namespace allcore
