cmake_minimum_required(VERSION 3.21)
project(schron)

set(CMAKE_CXX_STANDARD 23)

if(UNIX AND NOT APPLE)
    include_directories(/usr/include/lua5.3)
elseif(APPLE)
    include_directories(/opt/homebrew/opt/lua@5.3/include/lua5.3)
    link_directories(/opt/homebrew/opt/lua@5.3/lib)
else()
    include_directories(C:/lua)
    link_directories(C:/lua)
endif()

if(UNIX AND NOT APPLE)
    add_compile_definitions(SPDLOG_USE_STD_FORMAT)
else()
    add_compile_definitions(FMT_HEADER_ONLY)
endif()

add_subdirectory(lib)
add_subdirectory(src)
