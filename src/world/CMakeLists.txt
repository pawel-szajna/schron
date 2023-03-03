add_library(
        world
        STATIC
        builders.cpp
        level.cpp
        world.cpp
)

if(NOT MSVC)
    target_compile_options(
        world
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_link_libraries(
        world
        PUBLIC
)
