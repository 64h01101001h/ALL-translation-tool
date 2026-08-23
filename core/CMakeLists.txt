# allcore — the shared C++20 core: spine access, engine ports, query grammar.
# SQLite: macOS's system libsqlite3 ships FTS5; the smoke test asserts FTS5 at
# runtime. If a platform lacks it, vendor the amalgamation under third_party/
# and flip USE_SYSTEM_SQLITE off.
option(USE_SYSTEM_SQLITE "Link the system sqlite3 library" ON)

add_library(allcore STATIC
  src/spine.cpp
    src/stardict.cpp
  src/analysis.cpp
  src/qc.cpp
  src/lattice.cpp
  src/particles.cpp
  src/sanskrit.cpp
  src/reader.cpp
  src/verbclass.cpp
  src/drills.cpp
  src/progress.cpp
  src/terminology.cpp
  src/outline.cpp
  src/hypfile.cpp
  src/whitney.cpp
  src/colloquial.cpp
  src/backup.cpp
  src/comments.cpp
  src/dossier.cpp
  src/glossary.cpp
  src/proposals.cpp
  src/regenreg.cpp
  src/tibcal_phugpa.cpp
  src/verse.cpp
  src/wilsonparse.cpp
  src/tibexport.cpp
  src/quotation.cpp
  src/verbstems.cpp
  src/lexicon.cpp
  src/tibcal.cpp
  src/tibcal_day.cpp
  src/affixnorm.cpp
  src/searchnorm.cpp
  src/libindex.cpp
  src/gofer.cpp
  src/acip_ewts.cpp
  src/pron_engine.cpp
  src/thl_phonetics.cpp
  src/ewts_unicode.cpp
  src/spellcheck.cpp
  src/refdict.cpp
  src/mvp.cpp
  src/poslex.cpp
  src/abbr.cpp
  src/authorsearch.cpp
  src/subjects.cpp
  src/contractions.cpp
  src/botok_chunks.cpp
  src/botok_trie.cpp
  src/botok_tokenize.cpp
  src/botok_segtrie.cpp
  src/catalog_id.cpp
  src/catalog_audit.cpp
  src/catalog_name.cpp
  src/catalog_list.cpp
  src/title_xlat.cpp
  src/tree_diff.cpp
  src/catalog_register.cpp
  src/worksheet.cpp
  src/catalog_qc.cpp
  src/shelf_suggest.cpp
  src/tm84000.cpp
  src/catalog_actions.cpp
  src/textdna.cpp
  src/unicode_wylie.cpp
)
target_include_directories(allcore PUBLIC include)

if(USE_SYSTEM_SQLITE)
  find_library(SQLITE3_LIB sqlite3 REQUIRED)
  target_link_libraries(allcore PUBLIC ${SQLITE3_LIB} z)
else()
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3.c")
    message(FATAL_ERROR "USE_SYSTEM_SQLITE=OFF needs the vendored SQLite amalgamation at core/third_party/sqlite3.c (not in the repo — download from sqlite.org and place it there)")
  endif()
  target_sources(allcore PRIVATE third_party/sqlite3.c)
  target_include_directories(allcore PRIVATE third_party)
  target_compile_definitions(allcore PRIVATE SQLITE_ENABLE_FTS5)
endif()

add_executable(spine_smoke tests/spine_smoke.cpp)
target_link_libraries(spine_smoke PRIVATE allcore)
add_test(NAME spine_smoke
         COMMAND spine_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(analysis_smoke tests/analysis_smoke.cpp)
target_link_libraries(analysis_smoke PRIVATE allcore)
add_test(NAME analysis_smoke
         COMMAND analysis_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_SOURCE_DIR}/docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md)

add_executable(engines_battery tests/engines_battery.cpp)
target_link_libraries(engines_battery PRIVATE allcore)
add_executable(thl_battery tests/thl_battery.cpp)
target_link_libraries(thl_battery PRIVATE allcore)
add_test(NAME thl_battery COMMAND thl_battery)
add_test(NAME engines_battery
         COMMAND engines_battery ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_SOURCE_DIR}/build/pron_reference.tsv
                 ${CMAKE_SOURCE_DIR}/build/sanskrit_reference.tsv)

add_executable(spellcheck_smoke tests/spellcheck_smoke.cpp)
target_link_libraries(spellcheck_smoke PRIVATE allcore)
add_test(NAME spellcheck_smoke
         COMMAND spellcheck_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_SOURCE_DIR}/data/spellcheck)

add_executable(gofer_smoke tests/gofer_smoke.cpp)
target_link_libraries(gofer_smoke PRIVATE allcore)
add_test(NAME gofer_smoke
         COMMAND gofer_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/acipdocs)

add_executable(lattice_smoke tests/lattice_smoke.cpp)
target_link_libraries(lattice_smoke PRIVATE allcore)
add_test(NAME lattice_smoke
         COMMAND lattice_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(reader_smoke tests/reader_smoke.cpp)
target_link_libraries(reader_smoke PRIVATE allcore)
add_test(NAME reader_smoke
         COMMAND reader_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(drills_smoke tests/drills_smoke.cpp)
target_link_libraries(drills_smoke PRIVATE allcore)
add_test(NAME drills_smoke
         COMMAND drills_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(progress_smoke tests/progress_smoke.cpp)
target_link_libraries(progress_smoke PRIVATE allcore)
add_test(NAME progress_smoke COMMAND progress_smoke)

add_executable(terminology_smoke tests/terminology_smoke.cpp)
target_link_libraries(terminology_smoke PRIVATE allcore)
add_test(NAME terminology_smoke
         COMMAND terminology_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(outline_smoke tests/outline_smoke.cpp)
target_link_libraries(outline_smoke PRIVATE allcore)
add_test(NAME outline_smoke COMMAND outline_smoke)

add_executable(hyp_smoke tests/hyp_smoke.cpp)
target_link_libraries(hyp_smoke PRIVATE allcore)
add_test(NAME hyp_smoke
         COMMAND hyp_smoke
                 ${CMAKE_SOURCE_DIR}/data/hypercontext_legacy/tutorial.hyp)

add_executable(whitney_smoke tests/whitney_smoke.cpp)
target_link_libraries(whitney_smoke PRIVATE allcore)
add_test(NAME whitney_smoke
         COMMAND whitney_smoke
                 ${CMAKE_SOURCE_DIR}/data/whitney/whitney_roots.tsv)

add_executable(colloquial_smoke tests/colloquial_smoke.cpp)
target_link_libraries(colloquial_smoke PRIVATE allcore)
add_test(NAME colloquial_smoke
         COMMAND colloquial_smoke
                 ${CMAKE_SOURCE_DIR}/data/pron_colloquial/colloquial_pron.tsv
                 ${CMAKE_SOURCE_DIR}/data/honorifics/honorific_register.tsv
                 ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(phugpa_smoke tests/phugpa_smoke.cpp)
target_link_libraries(phugpa_smoke PRIVATE allcore)
add_test(NAME phugpa_smoke
         COMMAND phugpa_smoke
                 ${CMAKE_SOURCE_DIR}/third_party/tcg_henning/fixtures)

add_executable(regenreg_smoke tests/regenreg_smoke.cpp)
target_link_libraries(regenreg_smoke PRIVATE allcore)
add_test(NAME regenreg_smoke COMMAND regenreg_smoke)

add_executable(proposals_smoke tests/proposals_smoke.cpp)
target_link_libraries(proposals_smoke PRIVATE allcore)
add_test(NAME proposals_smoke
         COMMAND proposals_smoke ${CMAKE_CURRENT_BINARY_DIR})

add_executable(verse_smoke tests/verse_smoke.cpp)
target_link_libraries(verse_smoke PRIVATE allcore)
add_test(NAME verse_smoke COMMAND verse_smoke)

add_executable(quotation_smoke tests/quotation_smoke.cpp)
target_link_libraries(quotation_smoke PRIVATE allcore)
add_test(NAME quotation_smoke
         COMMAND quotation_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(verbstems_smoke tests/verbstems_smoke.cpp)
target_link_libraries(verbstems_smoke PRIVATE allcore)
add_test(NAME verbstems_smoke
         COMMAND verbstems_smoke ${CMAKE_SOURCE_DIR}/data/extracted/verb_stems.tsv)

add_executable(lexicon_smoke tests/lexicon_smoke.cpp)
target_link_libraries(lexicon_smoke PRIVATE allcore)
add_test(NAME lexicon_smoke
         COMMAND lexicon_smoke
                 ${CMAKE_SOURCE_DIR}/data/extracted/monlam_lexicon_1.txt
                 ${CMAKE_SOURCE_DIR}/data/extracted/monlam_lexicon_2.txt)

add_executable(tibcal_smoke tests/tibcal_smoke.cpp)
target_link_libraries(tibcal_smoke PRIVATE allcore)
add_test(NAME tibcal_smoke COMMAND tibcal_smoke)

add_executable(libindex_cli tools_libindex.cpp)
target_link_libraries(libindex_cli PRIVATE allcore)

add_executable(wynorm tools_wynorm.cpp)
target_link_libraries(wynorm PRIVATE allcore)

add_executable(searchnorm_smoke tests/searchnorm_smoke.cpp)
target_link_libraries(searchnorm_smoke PRIVATE allcore)
add_test(NAME searchnorm_smoke
         COMMAND searchnorm_smoke
                 ${CMAKE_SOURCE_DIR}/data/extracted/verb_lemmas.tsv)

add_executable(affixnorm_smoke tests/affixnorm_smoke.cpp)
target_link_libraries(affixnorm_smoke PRIVATE allcore)
add_test(NAME affixnorm_smoke COMMAND affixnorm_smoke)

add_executable(tibcal_day_smoke tests/tibcal_day_smoke.cpp)
target_link_libraries(tibcal_day_smoke PRIVATE allcore)
add_test(NAME tibcal_day_smoke
         COMMAND tibcal_day_smoke
                 ${CMAKE_SOURCE_DIR}/third_party/kck_henning/kck_fixtures.tsv
                 ${CMAKE_SOURCE_DIR}/third_party/kck_henning/kck_year_fixtures.tsv)

add_executable(tibexport_smoke tests/tibexport_smoke.cpp)
target_link_libraries(tibexport_smoke PRIVATE allcore)
add_test(NAME tibexport_smoke COMMAND tibexport_smoke)

add_executable(libindex_smoke tests/libindex_smoke.cpp)
target_link_libraries(libindex_smoke PRIVATE allcore)
add_test(NAME libindex_smoke COMMAND libindex_smoke)

# SQA PERF-2: stopping an index update must be safe, not just possible
add_executable(libindex_cancel_smoke tests/libindex_cancel_smoke.cpp)
target_link_libraries(libindex_cancel_smoke PRIVATE allcore)
add_test(NAME libindex_cancel_smoke COMMAND libindex_cancel_smoke)

add_executable(qc_smoke tests/qc_smoke.cpp)
target_link_libraries(qc_smoke PRIVATE allcore)
add_test(NAME qc_smoke
         COMMAND qc_smoke ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/analysis598.md)

add_executable(botok_smoke tests/botok_smoke.cpp)
target_link_libraries(botok_smoke PRIVATE allcore)
add_test(NAME botok_smoke
         COMMAND botok_smoke ${CMAKE_SOURCE_DIR}/data/botok
                 ${CMAKE_SOURCE_DIR}/build/botok_syls_reference.tsv)

add_executable(botok_tok_smoke tests/botok_tok_smoke.cpp)
target_link_libraries(botok_tok_smoke PRIVATE allcore)
add_test(NAME botok_tok_smoke
         COMMAND botok_tok_smoke ${CMAKE_SOURCE_DIR}/data/botok
                 ${CMAKE_SOURCE_DIR}/build/botok_tok_scenarios.txt
                 ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db
                 ${CMAKE_SOURCE_DIR}/build/botok_tok_corpus.tsv)

add_executable(mvp_smoke tests/mvp_smoke.cpp)
target_link_libraries(mvp_smoke PRIVATE allcore)
add_test(NAME mvp_smoke
         COMMAND mvp_smoke ${CMAKE_SOURCE_DIR}/data/extracted/mahavyutpatti.tsv)

add_executable(poslex_smoke tests/poslex_smoke.cpp)
target_link_libraries(poslex_smoke PRIVATE allcore)
add_test(NAME poslex_smoke
         COMMAND poslex_smoke ${CMAKE_SOURCE_DIR}/data/soas_pos/classical-lexicon.txt
                 ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(abbr_smoke tests/abbr_smoke.cpp)
target_link_libraries(abbr_smoke PRIVATE allcore)
add_test(NAME abbr_smoke
         COMMAND abbr_smoke ${CMAKE_SOURCE_DIR}/data/abbreviations/tibschol_abbr.csv
                 ${CMAKE_SOURCE_DIR}/data/spellcheck
                 ${CMAKE_SOURCE_DIR}/data/abbreviations/rkts_abb.csv)

add_executable(shelf_suggest_smoke tests/shelf_suggest_smoke.cpp)
target_link_libraries(shelf_suggest_smoke PRIVATE allcore)
add_test(NAME shelf_suggest_smoke
         COMMAND shelf_suggest_smoke ${CMAKE_SOURCE_DIR}/library/sungbum)

add_executable(catalog_qc_smoke tests/catalog_qc_smoke.cpp)
target_link_libraries(catalog_qc_smoke PRIVATE allcore)
add_test(NAME catalog_qc_smoke
         COMMAND catalog_qc_smoke ${CMAKE_CURRENT_BINARY_DIR}
                 ${CMAKE_SOURCE_DIR}/library/sungbum)

add_executable(worksheet_smoke tests/worksheet_smoke.cpp)
target_link_libraries(worksheet_smoke PRIVATE allcore)
add_test(NAME worksheet_smoke COMMAND worksheet_smoke)

add_executable(register_smoke tests/register_smoke.cpp)
target_link_libraries(register_smoke PRIVATE allcore)
add_test(NAME register_smoke
         COMMAND register_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(cleanup_smoke tests/cleanup_smoke.cpp)
target_link_libraries(cleanup_smoke PRIVATE allcore)
add_test(NAME cleanup_smoke
         COMMAND cleanup_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(tree_diff_smoke tests/tree_diff_smoke.cpp)
target_link_libraries(tree_diff_smoke PRIVATE allcore)
add_test(NAME tree_diff_smoke
         COMMAND tree_diff_smoke ${CMAKE_CURRENT_BINARY_DIR}
                 ${CMAKE_SOURCE_DIR}/library/kangyur)

add_executable(title_xlat_smoke tests/title_xlat_smoke.cpp)
target_link_libraries(title_xlat_smoke PRIVATE allcore)
add_test(NAME title_xlat_smoke
         COMMAND title_xlat_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(catalog_list_smoke tests/catalog_list_smoke.cpp)
target_link_libraries(catalog_list_smoke PRIVATE allcore)
add_test(NAME catalog_list_smoke
         COMMAND catalog_list_smoke ${CMAKE_SOURCE_DIR}/library/kangyur)

add_executable(catalog_name_smoke tests/catalog_name_smoke.cpp)
target_link_libraries(catalog_name_smoke PRIVATE allcore)
add_test(NAME catalog_name_smoke
         COMMAND catalog_name_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(volsplit_smoke tests/volsplit_smoke.cpp)
target_link_libraries(volsplit_smoke PRIVATE allcore)
add_test(NAME volsplit_smoke
         COMMAND volsplit_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(catalog_audit_smoke tests/catalog_audit_smoke.cpp)
target_link_libraries(catalog_audit_smoke PRIVATE allcore)
add_test(NAME catalog_audit_smoke
         COMMAND catalog_audit_smoke
                 ${CMAKE_SOURCE_DIR}/data/extracted/mixed_nuts_bibliography.json
                 ${CMAKE_SOURCE_DIR}/data/extracted/apparatus_bibliography.json
                 ${CMAKE_SOURCE_DIR}/library)

add_executable(catalog_id_smoke tests/catalog_id_smoke.cpp)
target_link_libraries(catalog_id_smoke PRIVATE allcore)
add_test(NAME catalog_id_smoke
         COMMAND catalog_id_smoke ${CMAKE_SOURCE_DIR}/library)

add_executable(stardict_smoke tests/stardict_smoke.cpp)
target_link_libraries(stardict_smoke PRIVATE allcore)
add_test(NAME stardict_smoke COMMAND stardict_smoke)

add_executable(pron_standard_smoke tests/pron_standard_smoke.cpp)
target_link_libraries(pron_standard_smoke PRIVATE allcore)
add_test(NAME pron_standard_smoke COMMAND pron_standard_smoke)

add_executable(subjects_smoke tests/subjects_smoke.cpp)
target_link_libraries(subjects_smoke PRIVATE allcore)
add_test(NAME subjects_smoke COMMAND subjects_smoke ${CMAKE_SOURCE_DIR}/data/extracted/work_subjects.tsv)

add_executable(authorsearch_smoke tests/authorsearch_smoke.cpp)
target_link_libraries(authorsearch_smoke PRIVATE allcore)
add_test(NAME authorsearch_smoke COMMAND authorsearch_smoke)

add_executable(contractions_smoke tests/contractions_smoke.cpp)
target_link_libraries(contractions_smoke PRIVATE allcore)
add_test(NAME contractions_smoke
         COMMAND contractions_smoke ${CMAKE_SOURCE_DIR}/data/extracted/bsdus_tshig.tsv
                 ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(acipcodes_smoke tests/acipcodes_smoke.cpp)
target_link_libraries(acipcodes_smoke PRIVATE allcore)
add_test(NAME acipcodes_smoke
         COMMAND acipcodes_smoke ${CMAKE_SOURCE_DIR}/data/acip_codes/Acipcode.txt)

add_executable(tm84000_smoke tests/tm84000_smoke.cpp)
target_link_libraries(tm84000_smoke PRIVATE allcore)
add_test(NAME tm84000_smoke
         COMMAND tm84000_smoke ${CMAKE_SOURCE_DIR}/data/84000/tm_84000.tsv.gz
                 ${CMAKE_BINARY_DIR})

add_executable(catalog_actions_smoke tests/catalog_actions_smoke.cpp)
target_link_libraries(catalog_actions_smoke PRIVATE allcore)
add_test(NAME catalog_actions_smoke
         COMMAND catalog_actions_smoke ${CMAKE_BINARY_DIR})

add_executable(textdna_smoke tests/textdna_smoke.cpp)
target_link_libraries(textdna_smoke PRIVATE allcore)
add_test(NAME textdna_smoke COMMAND textdna_smoke)

add_executable(forward_battery tests/forward_battery.cpp)
target_link_libraries(forward_battery PRIVATE allcore)
add_test(NAME forward_battery
         COMMAND forward_battery
                 ${CMAKE_SOURCE_DIR}/build/forward_reference.tsv)

add_executable(weird_battery tests/weird_battery.cpp)
target_link_libraries(weird_battery PRIVATE allcore)
add_test(NAME weird_battery
         COMMAND weird_battery
                 ${CMAKE_SOURCE_DIR}/build/weird_top.tsv)

add_executable(fuzz_smoke tests/fuzz_smoke.cpp)
target_link_libraries(fuzz_smoke PRIVATE allcore)
add_test(NAME fuzz_smoke COMMAND fuzz_smoke)

add_executable(spine_resilience_smoke tests/spine_resilience_smoke.cpp)
target_link_libraries(spine_resilience_smoke PRIVATE allcore)
add_test(NAME spine_resilience_smoke
         COMMAND spine_resilience_smoke
                 ${CMAKE_SOURCE_DIR}/build/hgm_spine_v27_2.db)

add_executable(glossary_smoke tests/glossary_smoke.cpp)
target_link_libraries(glossary_smoke PRIVATE allcore)
add_test(NAME glossary_smoke COMMAND glossary_smoke)

add_executable(dossier_smoke tests/dossier_smoke.cpp)
target_link_libraries(dossier_smoke PRIVATE allcore)
add_test(NAME dossier_smoke COMMAND dossier_smoke)

add_executable(comments_smoke tests/comments_smoke.cpp)
target_link_libraries(comments_smoke PRIVATE allcore)
add_test(NAME comments_smoke COMMAND comments_smoke)

# SQA FAIL-2: the first test that can see a write which failed AFTER a
# successful open. Driven by a script because it needs a RAM disk to
# stage a real short write (an unwritable directory only exercises the
# open guard, which was always correct — see the script's header).
add_executable(shortwrite_probe tests/shortwrite_probe.cpp)
target_link_libraries(shortwrite_probe PRIVATE allcore)
add_test(NAME shortwrite
         COMMAND ${CMAKE_SOURCE_DIR}/tools/test_shortwrite.sh
                 $<TARGET_FILE:shortwrite_probe>)

add_executable(backup_smoke tests/backup_smoke.cpp)
target_link_libraries(backup_smoke PRIVATE allcore)
add_test(NAME backup_smoke COMMAND backup_smoke)

add_executable(towylie_battery tests/towylie_battery.cpp)
target_link_libraries(towylie_battery PRIVATE allcore)
add_test(NAME towylie_battery
         COMMAND towylie_battery ${CMAKE_SOURCE_DIR}/build/towylie_reference.tsv)

find_package(Python3 COMPONENTS Interpreter QUIET)

# F3 link truth: outbound claims verified at the index level
if(Python3_FOUND)
  add_test(NAME link_truth
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/link_truth_check.py
                   ${CMAKE_SOURCE_DIR})
endif()

# The constitution (Fidelity engine, track C): process lessons as gates
if(Python3_FOUND)
  add_test(NAME constitution
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/constitution_check.py
                   ${CMAKE_SOURCE_DIR})
endif()

# BUILD-7: the battery must say what it did NOT run. fixture_gate holds
# the skip machinery (cmake/AllFixtureTests.cmake) and keeps
# docs/FIXTURES.md in step with the fixtures the suites actually read;
# it carries its own hermetic proof, so it goes red if the wrapper is
# removed. Neither suite reads an untracked fixture, so both run from a
# clean checkout — which is the point.
if(Python3_FOUND)
  add_test(NAME fixture_gate
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/fixture_gate_check.py
                   ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
                   ${CMAKE_COMMAND} ${CMAKE_CTEST_COMMAND})
endif()

# BUILD-7 / house rule 4: a suite handed no data FAILS. Before
# 2026-08-23 these three exited 0 against a path that does not exist,
# one of them printing "(measured 31%)" from a string literal.
if(Python3_FOUND)
  add_test(NAME no_vacuous_pass
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/no_vacuous_pass_check.py
                   ${CMAKE_CURRENT_BINARY_DIR}
                   $<TARGET_FILE:catalog_id_smoke>
                   $<TARGET_FILE:catalog_qc_smoke>
                   $<TARGET_FILE:shelf_suggest_smoke>)
endif()

# BUILD-6: the release must be able to say what it was built from.
if(Python3_FOUND)
  add_test(NAME build_manifest
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/build_manifest_check.py
                   ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
endif()

# SQA BUILD-2/4/5: the payload gate that actually opens the payload.
# The selftest stages synthetic trees in a temp dir and asserts the
# walk's verdict on each: an unmanifested file inside a CREDITED
# directory must fail (that is BUILD-4 exactly — the old rule credited
# 102 MB of data/extracted on the keyword "84000"), a `drop` tree that
# survived the prune must fail (BUILD-5 — THL records in a DMG whose
# notices said they were not redistributed), team-only payload must
# fail a market press, and the pre-fix eight-line README must fail the
# team stamp check. Reverting any of those behaviours turns this red;
# it was mutation-tested that way before it was committed.
if(Python3_FOUND)
  add_test(NAME payload_manifest
           COMMAND ${Python3_EXECUTABLE}
                   ${CMAKE_SOURCE_DIR}/tools/manifest_check.py
                   --selftest ${CMAKE_SOURCE_DIR})
endif()
