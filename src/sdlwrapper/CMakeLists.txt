add_library(
        sdlwrapper
        renderer.cpp
        sdlwrapper.cpp
        surface.cpp
        texture.cpp
        window.cpp
)

if(NOT MSVC)
    target_compile_options(
        sdlwrapper
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_include_directories(
        sdlwrapper
        PUBLIC
        ../../lib/SDL/include
)

target_link_libraries(
        sdlwrapper
        PUBLIC
        SDL2
        SDL2_ttf
)
