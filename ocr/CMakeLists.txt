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

add_executable(ocr_smoke tests/ocr_smoke.cpp)
target_link_libraries(ocr_smoke PRIVATE allocr)
add_test(NAME ocr_smoke
         COMMAND ocr_smoke
                 ${CMAKE_SOURCE_DIR}/library/ocr_models/BDRC_PhotiLines/PhotiLines.onnx
                 ${CMAKE_SOURCE_DIR}/build/ocr_ref)
