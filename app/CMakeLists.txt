qt_standard_project_setup()

qt_add_executable(ALLTranslationTool
  main.cpp
)
find_package(Qt6 COMPONENTS Network REQUIRED)
target_link_libraries(ALLTranslationTool PRIVATE allcore Qt6::Widgets Qt6::Network)
set_target_properties(ALLTranslationTool PROPERTIES
  MACOSX_BUNDLE ON
)

find_package(Qt6 COMPONENTS Network REQUIRED)
qt_add_executable(allanalyze
  analyze_cli.cpp
)
target_link_libraries(allanalyze PRIVATE allcore Qt6::Core Qt6::Network)
