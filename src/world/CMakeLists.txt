add_library(
        world
        STATIC
        builders.cpp
        level.cpp
        sector.cpp
        world.cpp
)

if(NOT MSVC)
    target_compile_options(
        world
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_include_directories(
        world
        PUBLIC
        ${LUA_INCLUDE_DIR}
)

target_link_libraries(
        world
        PUBLIC
        sol2
        ${LUA_LIBRARIES}
)
