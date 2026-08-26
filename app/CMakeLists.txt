qt_standard_project_setup()

# derive the test-battery count from the actual registered suites, so
# the About line can never go stale again (it drifted twice at 39)
set(_bat_count 0)
foreach(_f core/CMakeLists.txt app/CMakeLists.txt ocr/CMakeLists.txt)
  if(EXISTS "${CMAKE_SOURCE_DIR}/${_f}")
    file(READ "${CMAKE_SOURCE_DIR}/${_f}" _bat_txt)
    string(REGEX MATCHALL "add_test\\(" _bat_hits "${_bat_txt}")
    list(LENGTH _bat_hits _bat_n)
    math(EXPR _bat_count "${_bat_count} + ${_bat_n}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
                 "${CMAKE_SOURCE_DIR}/${_f}")
  endif()
endforeach()
add_compile_definitions(ALL_TEST_BATTERIES="${_bat_count}")
# SQA BUILD-19: publish the WRITTEN count so the top level can compare
# it against the count actually registered. A textual count that no
# one checks is how six gates went missing without a word.
set(ALL_BAT_WRITTEN "${_bat_count}" CACHE INTERNAL "suites written")


qt_add_executable(DiamondCutterTranslationTool
  main.cpp
  ux_tokens.h            # design tokens - first R7 extraction, 2026-08-25
  textreport.h           # honest-line helpers - second R7 extraction
  thirdparty/diff_match_patch.cpp
  AppIcon.icns
)
set_source_files_properties(AppIcon.icns PROPERTIES
  MACOSX_PACKAGE_LOCATION "Resources")
find_package(Qt6 COMPONENTS Network REQUIRED)
find_package(Qt6 COMPONENTS Pdf PdfWidgets QUIET)
file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" ALL_APP_VERSION LIMIT_COUNT 1)
# re-configure when VERSION changes — otherwise the About pane
# ships a stale number (bit us at 0.9.0 → 0.11.0)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
             "${CMAKE_SOURCE_DIR}/VERSION")
add_compile_definitions(ALL_APP_VERSION="${ALL_APP_VERSION}")
target_link_libraries(DiamondCutterTranslationTool PRIVATE allcore Qt6::Widgets Qt6::Network)
if(Qt6Pdf_FOUND)
  target_link_libraries(DiamondCutterTranslationTool PRIVATE Qt6::Pdf Qt6::PdfWidgets)
  target_compile_definitions(DiamondCutterTranslationTool PRIVATE ALL_HAVE_QTPDF)
endif()
# the Scan pane needs allocr (onnxruntime + OpenCV); optional — the app
# builds without it and simply omits the pane
if(TARGET allocr)
  target_link_libraries(DiamondCutterTranslationTool PRIVATE allocr)
  target_compile_definitions(DiamondCutterTranslationTool PRIVATE ALL_HAVE_OCR)
endif()
# BUILD-15: with no explicit identifier, Qt derived
# com.appelegant.DiamondCutterTranslationTool from a machine-local
# default - a stranger's reversed domain in the org's shipping
# artifact, and CFBundleShortVersionString sat at a hardcoded "1.0"
# while VERSION said 1.0.0-rc.1. The identity now names the Asian
# Legacy Library and the version is read from the VERSION file the
# release ritual maintains.
file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" ALL_APP_VERSION LIMIT_COUNT 1)
set_target_properties(DiamondCutterTranslationTool PROPERTIES
  MACOSX_BUNDLE ON
  MACOSX_BUNDLE_ICON_FILE AppIcon.icns
  MACOSX_BUNDLE_GUI_IDENTIFIER "org.asianlegacylibrary.DiamondCutterTranslationTool"
  MACOSX_BUNDLE_BUNDLE_VERSION "${ALL_APP_VERSION}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING "${ALL_APP_VERSION}"
)

find_package(Qt6 COMPONENTS Network REQUIRED)
qt_add_executable(allanalyze
  analyze_cli.cpp
)
target_link_libraries(allanalyze PRIVATE allcore Qt6::Core Qt6::Network)

qt_add_executable(dmp_smoke dmp_smoke.cpp thirdparty/diff_match_patch.cpp)
target_link_libraries(dmp_smoke PRIVATE Qt6::Core)
add_test(NAME dmp_smoke COMMAND dmp_smoke)

qt_add_executable(collation_smoke collation_smoke.cpp)
target_link_libraries(collation_smoke PRIVATE Qt6::Core)
add_test(NAME collation_smoke COMMAND collation_smoke)

qt_add_executable(font_smoke font_smoke.cpp)
target_link_libraries(font_smoke PRIVATE allcore Qt6::Gui)
add_test(NAME font_smoke
         COMMAND font_smoke ${CMAKE_SOURCE_DIR}/data/fonts
                 ${CMAKE_SOURCE_DIR}/data/hypercontext_legacy/syllable-table-with-unicode.txt
                 ${CMAKE_SOURCE_DIR}/data/hypercontext_legacy/syllablesThatFailToRender_testFile_HyperContext.txt)

# suite 38: the app's own pane-level selftest (docs/TEST_PLAN.md)
add_test(NAME app_selftest COMMAND DiamondCutterTranslationTool --selftest)
# G1 fossil: the monkey's canonical walk — seed 1, 300 steps, the walk
# that drew first blood (unguarded native panels) now guards forever
add_test(NAME gauntlet_walk
         COMMAND DiamondCutterTranslationTool --gauntlet auto 300)
set_tests_properties(gauntlet_walk PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 420)
set_tests_properties(app_selftest PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 300)

# BUS FACTOR / BUILD-7, found 2026-08-25 by actually following
# docs/DEVELOPER_ONBOARDING.md on a clean clone. The doc calls
# `ctest -LE fixture` "the clean-checkout gate"; it exited 8, because
# these two suites need the spine database and were NOT labelled
# `fixture`.
#
# The label is applied by cmake/AllFixtureTests.cmake's add_test
# override, which detects a fixture dependency by inspecting the test's
# COMMAND ARGUMENTS for paths under an untracked root. These two take
# no path argument at all - the app locates the spine at runtime - so
# the detection was blind to them by construction, and no amount of
# care in the override would have caught it.
#
# Two properties, doing two different jobs:
#   LABELS fixture            - so `-LE fixture` excludes them, which
#                               is what makes the documented gate true
#   SKIP_REGULAR_EXPRESSION   - so a plain `ctest` on a clean checkout
#                               records an honest SKIP naming the
#                               missing database, rather than a failure
#                               that reads like a broken build. The
#                               app already prints exactly this line.
set_tests_properties(app_selftest gauntlet_walk PROPERTIES
  LABELS "fixture"
  SKIP_REGULAR_EXPRESSION "cannot open spine db")

# STATIC-11: diff_match_patch is upstream code compiled into our
# targets; its warnings are not ours to fix and must not trip the
# STATIC-1 wall (the wall's path filter also knows this spelling).
set_source_files_properties(thirdparty/diff_match_patch.cpp
  PROPERTIES COMPILE_OPTIONS "-w")
