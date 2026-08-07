# allcore — the shared C++20 core: spine access, engine ports, query grammar.
# SQLite: macOS's system libsqlite3 ships FTS5; the smoke test asserts FTS5 at
# runtime. If a platform lacks it, vendor the amalgamation under third_party/
# and flip USE_SYSTEM_SQLITE off.
option(USE_SYSTEM_SQLITE "Link the system sqlite3 library" ON)

add_library(allcore STATIC
  src/spine.cpp
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
  src/verse.cpp
  src/wilsonparse.cpp
  src/tibexport.cpp
  src/quotation.cpp
  src/verbstems.cpp
  src/lexicon.cpp
  src/tibcal.cpp
  src/libindex.cpp
  src/gofer.cpp
  src/acip_ewts.cpp
  src/pron_engine.cpp
  src/ewts_unicode.cpp
  src/spellcheck.cpp
  src/refdict.cpp
  src/botok_chunks.cpp
  src/botok_trie.cpp
  src/botok_tokenize.cpp
)
target_include_directories(allcore PUBLIC include)

if(USE_SYSTEM_SQLITE)
  find_library(SQLITE3_LIB sqlite3 REQUIRED)
  target_link_libraries(allcore PUBLIC ${SQLITE3_LIB})
else()
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

add_executable(tibexport_smoke tests/tibexport_smoke.cpp)
target_link_libraries(tibexport_smoke PRIVATE allcore)
add_test(NAME tibexport_smoke COMMAND tibexport_smoke)

add_executable(libindex_smoke tests/libindex_smoke.cpp)
target_link_libraries(libindex_smoke PRIVATE allcore)
add_test(NAME libindex_smoke COMMAND libindex_smoke)

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
