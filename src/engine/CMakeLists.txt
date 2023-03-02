add_library(
        engine
        engine.cpp
        noise.cpp
)

target_compile_options(
        engine
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        engine
        PUBLIC
        sdlwrapper
)
