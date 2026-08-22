cmake_minimum_required(VERSION 3.24)
# the project version tracks the VERSION file (single source);
# a hardcoded number here went stale at 0.1.0 for months
file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" ALL_ROOT_VERSION LIMIT_COUNT 1)
# project(VERSION) accepts only numerics — strip pre-release suffixes
# ("1.0.0-rc.1" → 1.0.0); the FULL string still reaches the app via
# ALL_APP_VERSION (app/CMakeLists reads the VERSION file directly)
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" ALL_NUMERIC_VERSION
       "${ALL_ROOT_VERSION}")
if(NOT ALL_NUMERIC_VERSION)
  message(FATAL_ERROR "VERSION '${ALL_ROOT_VERSION}' has no numeric prefix")
endif()
project(DiamondCutterTranslationTool VERSION ${ALL_NUMERIC_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# T1 (total-quality engine): the wall. Warnings were driven to zero on
# 2026-08-20 (a collapsed-escape regex bug hid among them); -Wall keeps
# the count at zero. (-Wextra evaluated separately before adoption.)
if(NOT MSVC)
  add_compile_options(-Wall)
endif()

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
