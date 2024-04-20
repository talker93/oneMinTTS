cmake_minimum_required(VERSION 3.10)
project(fewByteTTS VERSION 0.1 DESCRIPTION "Supports HuggingFace🤗 MMS-TTS models inferencing on multi-platforms.")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for ONNX and nlohmann_json
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(${PROJECT_NAME} src/main.cpp)

# Detect operating system
if(WIN32)
    add_definitions(-DWINDOWS)
elseif(APPLE)
    add_definitions(-DAPPLE)
elseif(UNIX AND NOT APPLE)
    add_definitions(-DLINUX)
endif()

# ONNX Runtime Configuration
if(WIN32)
    set(ONNXRUNTIME_LIB_NAME "onnxruntime.lib")
    set(ONNXRUNTIME_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-win-x64-1.17.3/include")
    set(ONNXRUNTIME_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-win-x64-1.17.3/lib")
elseif(APPLE)
    set(ONNXRUNTIME_LIB_NAME "libonnxruntime.dylib")
    set(ONNXRUNTIME_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-osx-universal2-1.17.3/include")
    set(ONNXRUNTIME_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-osx-universal2-1.17.3/lib")
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(ONNXRUNTIME_LIB_NAME "libonnxruntime.so")
        set(ONNXRUNTIME_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-linux-x64-1.17.3/include")
        set(ONNXRUNTIME_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-linux-x64-1.17.3/lib")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(ONNXRUNTIME_LIB_NAME "libonnxruntime.so")
        set(ONNXRUNTIME_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-linux-aarch64-1.17.3/include")
        set(ONNXRUNTIME_LIBRARY_DIR "${PROJECT_SOURCE_DIR}/platforms/onnxruntime-linux-aarch64-1.17.3/lib")
    endif()
endif()
# Linking ONNX Runtime libraries
include_directories(${ONNXRUNTIME_INCLUDE_DIR})
link_directories(${ONNXRUNTIME_LIBRARY_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ONNXRUNTIME_LIBRARY_DIR}/${ONNXRUNTIME_LIB_NAME})

# Post-building actions for assembling the executable bundle
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    # Create the executable bundle folder
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/fewByte
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/fewByte/assets
    # Move the binary executable into a specific directory
    COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_BINARY_DIR}/fewByte/${PROJECT_NAME}
    # Copy necessary files to the executable bundle
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/files/model_download.py ${CMAKE_BINARY_DIR}/fewByte/assets/model_download.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/files/mms_vocab.json ${CMAKE_BINARY_DIR}/fewByte/assets/mms_vocab.json
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/files/models ${CMAKE_BINARY_DIR}/fewByte/assets/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/files/uroman ${CMAKE_BINARY_DIR}/fewByte/assets/uroman
    COMMAND ${CMAKE_COMMAND} -E copy ${ONNXRUNTIME_LIBRARY_DIR}/${ONNXRUNTIME_LIB_NAME} ${CMAKE_BINARY_DIR}/fewByte/assets/${ONNXRUNTIME_LIB_NAME}
)
