add_library(
        game
        game.cpp
        mode_main_menu.cpp
        mode_in_game.cpp
        player.cpp
)

target_include_directories(
        game
        PRIVATE
        ../../lib/sol2/include
)

target_compile_options(
        game
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        game
        PUBLIC
        engine
        scripting
        ui
        world
)
