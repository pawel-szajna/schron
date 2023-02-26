add_library(
        game
        game.cpp
)

target_compile_options(
        game
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        game
        PUBLIC
)
