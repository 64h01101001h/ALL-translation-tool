qt_standard_project_setup()

qt_add_executable(ALLTranslationTool
  main.cpp
  thirdparty/diff_match_patch.cpp
  AppIcon.icns
)
set_source_files_properties(AppIcon.icns PROPERTIES
  MACOSX_PACKAGE_LOCATION "Resources")
find_package(Qt6 COMPONENTS Network REQUIRED)
target_link_libraries(ALLTranslationTool PRIVATE allcore Qt6::Widgets Qt6::Network)
# the Scan pane needs allocr (onnxruntime + OpenCV); optional — the app
# builds without it and simply omits the pane
if(TARGET allocr)
  target_link_libraries(ALLTranslationTool PRIVATE allocr)
  target_compile_definitions(ALLTranslationTool PRIVATE ALL_HAVE_OCR)
endif()
set_target_properties(ALLTranslationTool PROPERTIES
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
add_test(NAME app_selftest COMMAND ALLTranslationTool --selftest)
set_tests_properties(app_selftest PROPERTIES
  ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 300)
