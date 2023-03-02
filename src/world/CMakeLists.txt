add_library(
        world
        builders.cpp
        level.cpp
        world.cpp
)

target_compile_options(
        world
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        world
        PUBLIC
)
