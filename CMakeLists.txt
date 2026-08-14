cmake_minimum_required(VERSION 3.24)
# the project version tracks the VERSION file (single source);
# a hardcoded number here went stale at 0.1.0 for months
file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" ALL_ROOT_VERSION LIMIT_COUNT 1)
project(ALLTranslationTool VERSION ${ALL_ROOT_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(core)
add_subdirectory(ocr)

# The Qt 6 app shell is optional so the core + batteries always build.
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  add_subdirectory(app)
else()
  message(STATUS "Qt6 not found — building allcore + tests only")
endif()
