cmake_minimum_required(VERSION 3.24)
project(ALLTranslationTool VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(core)

# The Qt 6 app shell is optional so the core + batteries always build.
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
  add_subdirectory(app)
else()
  message(STATUS "Qt6 not found — building allcore + tests only")
endif()
