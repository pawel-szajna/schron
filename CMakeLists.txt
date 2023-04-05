cmake_minimum_required(VERSION 3.21)
if(APPLE)
    project(schron C CXX OBJC)
else()
    project(schron C CXX)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option(DISABLE_PARALLELISM "Disable parallel STL algorithms")
option(DISABLE_DOCS "Disable documentation generation")

find_package(Lua REQUIRED)

include(modules)
include(CheckIncludeFileCXX)

CHECK_INCLUDE_FILE_CXX("format" HAVE_STD_FORMAT)
if(HAVE_STD_FORMAT AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang")) # Clang provides an empty <format> header
    message("Using standard library <format> header")
    add_compile_definitions(SPDLOG_USE_STD_FORMAT)
else()
    message("No usable <format> header found, using bundled fmtlib")
    add_compile_definitions(FMT_HEADER_ONLY)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(RELEASE_BUILD)
endif()

add_subdirectory(docs)
add_subdirectory(lib)
add_subdirectory(src)
