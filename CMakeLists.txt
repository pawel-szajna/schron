cmake_minimum_required(VERSION 3.21)
project(schron)

set(CMAKE_CXX_STANDARD 23)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_compile_definitions(SPDLOG_USE_STD_FORMAT)

add_subdirectory(src/sdlwrapper)

include_directories(src)

add_executable(
        schron
        main.cpp
)

target_link_libraries(
        schron
        PRIVATE
        sdlwrapper
)
