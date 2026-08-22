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


qt_add_executable(DiamondCutterTranslationTool
  main.cpp
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
set_target_properties(DiamondCutterTranslationTool PROPERTIES
  MACOSX_BUNDLE ON
  MACOSX_BUNDLE_ICON_FILE AppIcon.icns
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
         COMMAND DiamondCutterTranslationTool --gauntlet 1 300)
set_tests_properties(gauntlet_walk PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 420)
set_tests_properties(app_selftest PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 300)
