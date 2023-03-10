cmake_minimum_required(VERSION 3.21)
project(schron)

set(CMAKE_CXX_STANDARD 23)

find_package(Lua REQUIRED)

if(APPLE)
	add_compile_definitions(FMT_HEADER_ONLY)
else()
    add_compile_definitions(SPDLOG_USE_STD_FORMAT)
endif()

add_subdirectory(lib)
add_subdirectory(src)
