add_module(
    NAME ui
    TYPE STATIC
    SOURCES
        fonts.cpp
        editor/editor.cpp
        editor/widgets.cpp
        editor/window_sector.cpp
        editor/window_sprite.cpp
        editor/window_texture.cpp
        mini_map.cpp
        text.cpp
        ui.cpp
        widget.cpp
    DEPENDENCIES
        game
        sdlwrapper
        world
)

if(NOT MSVC)
    target_compile_options(
        ui
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()
