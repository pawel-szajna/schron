cmake_minimum_required(VERSION 3.21)
project(schron)

set(CMAKE_CXX_STANDARD 23)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

if(APPLE)
    include_directories(
        /opt/homebrew/include
        /opt/homebrew/opt/lua@5.3/include/lua
    )
    link_directories(
        /opt/homebrew/opt/lua@5.3/lib
        /opt/homebrew/opt/sdl12-compat/lib
        /opt/homebrew/opt/sdl_ttf/lib
    )
endif()

if(UNIX AND NOT APPLE)
    add_compile_definitions(SPDLOG_USE_STD_FORMAT)
else()
    add_compile_definitions(FMT_HEADER_ONLY)
endif()

add_subdirectory(lib/sprig)
add_subdirectory(src)
