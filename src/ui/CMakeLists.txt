add_library(
        ui
        editor/editor.cpp
        key_handler.cpp
        object.cpp
        ui.cpp
)

if(MSVC)
    target_compile_options(
        ui
        PRIVATE
        /Wall /WX /O2
    )
else()
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
