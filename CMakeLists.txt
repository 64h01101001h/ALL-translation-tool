cmake_minimum_required(VERSION 3.24)

# BUILD-1 (SQA 2026-08-22): the shipped app was arm64-only with a macOS
# minimum recorded nowhere — LSMinimumSystemVersion in the bundle was
# the empty string, so Finder could not warn and the DMG simply failed
# to launch. The deployment target must be set BEFORE project(), and Qt's
# Info.plist.app.in feeds ${CMAKE_OSX_DEPLOYMENT_TARGET} straight into
# LSMinimumSystemVersion, so this one line is also the plist fix.
#
# 26.0 is the measured floor, not an aspiration: 128 of the 129 dylibs
# macdeployqt bundles are macOS 26 (Tahoe) Homebrew bottles, so the
# artifact cannot run lower whatever this says. Building against an
# older stack is a real project (rebuild every dependency) — override
# with -DALL_MACOS_MIN=<x> when that day comes.
set(ALL_MACOS_MIN "26.0" CACHE STRING
    "macOS deployment target = the floor stated to users (BUILD-1)")
# empty is CMake's default AND the value a pre-BUILD-1 build dir has
# cached, so FORCE is needed to repair an existing cache; an explicit
# -DCMAKE_OSX_DEPLOYMENT_TARGET=<x> is non-empty and survives untouched.
if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${ALL_MACOS_MIN}" CACHE STRING
      "Minimum macOS version this build runs on" FORCE)
endif()

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

# MEM-2 (SQA 2026-08-22): a sanitizer build a maintainer can actually
# run. Nothing in this tree had ever been wired to one — `grep -rn
# fsanitize` across every CMake/sh/py file returned 0 — so every
# memory-safety property in the product was held by memory rather than
# by machinery, and MEM-1's pin (which only fires under ASan) was dead
# code. OFF by default so the everyday gate stays fast; one flag turns
# allcore, allocr, the app and every ctest suite into an ASan + UBSan
# build:
#
#     tools/sanitized_battery.sh            # configure + build + ctest
#     cmake -S . -B cmake-build-asan -DALL_SANITIZE=ON     # by hand
#
# Leak detection is left off: macOS/arm64 has no usable LSan.
option(ALL_SANITIZE "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
if(ALL_SANITIZE)
  if(MSVC)
    message(FATAL_ERROR "ALL_SANITIZE needs clang or gcc")
  endif()
  add_compile_options(-fsanitize=address,undefined
                      -fno-sanitize-recover=undefined
                      -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address,undefined)
  message(STATUS "ALL_SANITIZE=ON — ASan + UBSan, UB is fatal, "
                 "leak detection off (no usable LSan on macOS/arm64)")
endif()

enable_testing()

# BUILD-7: add_test() is wrapped so a suite whose untracked fixture is
# absent becomes an honest ctest SKIP naming the missing path, instead
# of aborting or printing [PASS] over zero rows. Must be included before
# the first add_subdirectory() that registers a test.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(AllFixtureTests)

add_subdirectory(core)
add_subdirectory(ocr)

# The Qt 6 app shell is optional so the core + batteries always build.
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  add_subdirectory(app)
else()
  message(STATUS "Qt6 not found — building allcore + tests only")
endif()

# say out loud, at configure time, how much of the battery this
# checkout can actually run (BUILD-7)
all_report_fixture_status()

if(APPLE)
  set(_all_arch "${CMAKE_OSX_ARCHITECTURES}")
  if(NOT _all_arch)
    set(_all_arch "${CMAKE_SYSTEM_PROCESSOR} (host default, not pinned)")
  endif()
  message(STATUS "ALL build floor: macOS ${CMAKE_OSX_DEPLOYMENT_TARGET}, "
                 "architecture ${_all_arch}")
endif()
