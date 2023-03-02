add_library(
        sdlwrapper
        renderer.cpp
        sdlwrapper.cpp
        surface.cpp
        texture.cpp
        window.cpp
)

target_compile_options(
        sdlwrapper
        PRIVATE
        -Wall -Wno-error -pedantic -O3 # libsprig has conversion warnings :(
)

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
