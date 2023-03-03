add_library(
        engine
        STATIC
        engine.cpp
        noise.cpp
)

if(NOT MSVC)
    target_compile_options(
        engine
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_link_libraries(
        engine
        PUBLIC
        sdlwrapper
)
