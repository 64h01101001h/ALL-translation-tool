# allocr — OCR stage 2 (OCR_DESIGN.md): ML stays OUT of allcore. Links
# onnxruntime (brew). The whole target is optional: without onnxruntime
# the rest of the project builds and tests exactly as before.
find_path(ONNXRUNTIME_INCLUDE onnxruntime_cxx_api.h
          PATHS /opt/homebrew/opt/onnxruntime/include/onnxruntime
                /usr/local/opt/onnxruntime/include/onnxruntime)
find_library(ONNXRUNTIME_LIB onnxruntime
             PATHS /opt/homebrew/opt/onnxruntime/lib
                   /usr/local/opt/onnxruntime/lib)

if(NOT ONNXRUNTIME_INCLUDE OR NOT ONNXRUNTIME_LIB)
  message(STATUS "onnxruntime not found — skipping allocr (OCR stage 2)")
  return()
endif()

add_library(allocr STATIC
  src/linedet.cpp
)
target_include_directories(allocr PUBLIC include PRIVATE ${ONNXRUNTIME_INCLUDE})
target_link_libraries(allocr PUBLIC ${ONNXRUNTIME_LIB})

# increment B (line building) links OpenCV C++ — the same native code the
# canonical cv2 calls, so the geometry stage is bit-identical.
find_package(OpenCV QUIET COMPONENTS core imgproc OPTIONAL_COMPONENTS geometry
             PATHS /opt/homebrew/opt/opencv /usr/local/opt/opencv)
if(OpenCV_FOUND)
  message(STATUS "allocr: OpenCV ${OpenCV_VERSION} — line building enabled")
  target_sources(allocr PRIVATE src/linebuild.cpp src/recognize.cpp)
  target_include_directories(allocr PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(allocr PUBLIC ${OpenCV_LIBS})
  target_compile_definitions(allocr PUBLIC ALLOCR_HAVE_OPENCV)
else()
  message(STATUS "allocr: OpenCV not found — line building skipped")
endif()

add_executable(ocr_smoke tests/ocr_smoke.cpp)
target_link_libraries(ocr_smoke PRIVATE allocr)
add_test(NAME ocr_smoke
         COMMAND ocr_smoke
                 ${CMAKE_SOURCE_DIR}/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx
                 ${CMAKE_SOURCE_DIR}/build/ocr_ref
                 ${CMAKE_SOURCE_DIR}/library/ocr_models/BDRC_Woodblock)
