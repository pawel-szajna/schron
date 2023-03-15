add_library(
        ui
        STATIC
        fonts.cpp
        editor/editor.cpp
        editor/widgets.cpp
        editor/window_sector.cpp
        mini_map.cpp
        text.cpp
        ui.cpp
)

if(NOT MSVC)
    target_compile_options(
        ui
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_link_libraries(
        ui
        PUBLIC
        sdlwrapper
)
