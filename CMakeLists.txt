cmake_minimum_required(VERSION 3.24...3.30)

project(hover-blast
    VERSION 1.0.0
    DESCRIPTION "Hover Blast Game"
    LANGUAGES C CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for LSP support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection
if(EMSCRIPTEN)
  set(PLATFORM_WEB TRUE)
  add_compile_definitions(PLATFORM_WEB)
  message(STATUS "Building for Web/Emscripten")
elseif(WIN32)
  set(PLATFORM_DESKTOP TRUE)
  add_compile_definitions(PLATFORM_DESKTOP)
  message(STATUS "Building for Windows")
elseif(APPLE)
  set(PLATFORM_DESKTOP TRUE)
  add_compile_definitions(PLATFORM_DESKTOP)
  message(STATUS "Building for macOS")
elseif(UNIX)
  set(PLATFORM_DESKTOP TRUE)
  add_compile_definitions(PLATFORM_DESKTOP)
  message(STATUS "Building for Linux")
endif()

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Dependencies
include(FetchContent)

# Raylib dependency
set(RAYLIB_VERSION 5.5)
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${RAYLIB_VERSION} EXACT
)

# Configure raylib options
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# Create executable
add_executable(${PROJECT_NAME})

# Add source directory
add_subdirectory(src)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
    OUTPUT_NAME ${PROJECT_NAME}
)

# Visual Studio debugger working directory
if(MSVC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY
        VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()

# Resource handling
set(RESOURCES_SRC_DIR ${CMAKE_SOURCE_DIR}/src/resources)

if(PLATFORM_WEB)
  # Web build - copy resources to parent directory for preloading
  set(RESOURCES_DEST_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources)
  add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCES_SRC_DIR} ${RESOURCES_DEST_DIR}
        COMMENT "Copying resources for web build"
  )
else()
  # Desktop build - copy resources to same directory
  set(RESOURCES_DEST_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
  add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RESOURCES_SRC_DIR} ${RESOURCES_DEST_DIR}
        COMMENT "Copying resources for desktop build"
  )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Platform-specific configurations
if(PLATFORM_WEB)
  # Emscripten/Web specific settings
  set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "--preload-file resources --shell-file ${CMAKE_SOURCE_DIR}/src/index.html"
  )

  target_link_options(${PROJECT_NAME} PRIVATE
        -sUSE_GLFW=3
        -sEXPORTED_RUNTIME_METHODS=HEAPF32,requestFullscreen
        -sWASM=1
        -sASYNCIFY
        -sGL_ENABLE_GET_PROC_ADDRESS=1
        --preload-file resources
        --shell-file ${CMAKE_SOURCE_DIR}/src/index.html
  )

  # Add Closure Compiler extern definitions for raylib/miniaudio
  set(CLOSURE_EXTERNS_FILE "${CMAKE_CURRENT_BINARY_DIR}/closure_externs.js")
  file(WRITE ${CLOSURE_EXTERNS_FILE} "
// Closure Compiler externs for raylib and miniaudio
var Module = {};
var miniaudio = {};
var GL = {};
var Browser = {};
var FS = {};
var HEAP8 = {};
var HEAP16 = {};
var HEAP32 = {};
var HEAPF32 = {};
var HEAPF64 = {};
var HEAPU8 = {};
var HEAPU16 = {};
var HEAPU32 = {};
")

  # Debug/Release specific flags for web
  target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-sASSERTIONS=1 -sGL_ASSERTIONS=1 -O1>
        $<$<CONFIG:Release>:-O2>
  )

  # Closure compiler settings - disable for now to avoid issues
  # If you want to enable closure compiler later, use:
  # $<$<CONFIG:Release>:--closure 1 --closure-args=--externs=${CLOSURE_EXTERNS_FILE}>

  # Alternative optimization flags that work better with raylib
  target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O2 -flto --llvm-lto 1>
  )

elseif(APPLE)
  # macOS specific frameworks
  target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
  )

elseif(WIN32)
  # Windows specific libraries
  target_link_libraries(${PROJECT_NAME} PRIVATE
        winmm
        gdi32
        opengl32
  )

elseif(UNIX)
  # Linux specific libraries
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

  target_link_libraries(${PROJECT_NAME} PRIVATE
        ${GTK3_LIBRARIES}
        GL
        m
        pthread
        dl
        rt
        X11
  )
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
  )
elseif(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        $<$<CONFIG:Debug>:/Od /Zi>
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
  )
endif()

# Print build information
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Raylib version: ${RAYLIB_VERSION}")

if(PLATFORM_WEB)
  message(STATUS "Emscripten detected - Web build enabled")
  message(STATUS "Emscripten version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()
