cmake_minimum_required(VERSION 3.21)
project(schron)

set(CMAKE_CXX_STANDARD 23)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_compile_definitions(SPDLOG_USE_STD_FORMAT)

include_directories(src)

add_subdirectory(src/game)
add_subdirectory(src/raycaster)
add_subdirectory(src/sdlwrapper)
add_subdirectory(src/util)
add_subdirectory(src/world)

add_executable(
        schron
        src/main.cpp
)

target_link_libraries(
        schron
        PRIVATE
        game
        sdlwrapper
        raycaster
        world
)
