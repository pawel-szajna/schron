add_library(
        game
        STATIC
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

if(NOT MSVC)
    target_compile_options(
        game
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_link_libraries(
        game
        PUBLIC
        engine
        scripting
        ui
        world
)
