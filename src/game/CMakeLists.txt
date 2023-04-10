add_module(
    NAME game
    TYPE STATIC
    SOURCES
        dialogue.cpp
        game.cpp
        mode_main_menu.cpp
        mode_in_game.cpp
        player.cpp
    INCLUDES
        ../../lib/sol2/include
        ../../lib/SDL/include # TODO: nothing should depend on SDL outside of sdlwrapper lib
        ${LUA_INCLUDE_DIR}
    DEPENDENCIES
        scripting
        ui
        util
        world
    PUBLIC_DEPENDENCIES
        engine
        sdlwrapper
)

if(NOT MSVC)
    target_compile_options(
        game
        PRIVATE
        -Wall -Werror -pedantic -O3
        -Wno-error=deprecated-declarations # TODO: needs removal after refactoring
    )
endif()
