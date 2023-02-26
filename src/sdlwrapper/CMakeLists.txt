add_library(
        sdlwrapper
        sdlwrapper.cpp
)

target_compile_options(
        sdlwrapper
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        sdlwrapper
        PUBLIC
        SDL
        SDL_ttf
)
