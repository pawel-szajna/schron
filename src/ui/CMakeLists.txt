add_library(
        ui
        STATIC
        fonts.cpp
        editor/editor.cpp
        key_handler.cpp
        mini_map.cpp
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
