add_library(
        sdlwrapper
        sdlwrapper.cpp
)

target_compile_options(
        sdlwrapper
        PRIVATE
        -Wall -Wno-error -pedantic -O3 # libsprig has conversion warnings :(
)

target_include_directories(
        sdlwrapper
        PUBLIC
        ../../sprig/source
)

target_link_libraries(
        sdlwrapper
        PUBLIC
        SDL
        SDL_ttf
        sprig
)
