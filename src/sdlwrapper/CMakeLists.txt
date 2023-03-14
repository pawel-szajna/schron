add_library(
        sdlwrapper
        STATIC
        converters.cpp
        cursor.cpp
        event.cpp
        font.cpp
        renderer.cpp
        sdlwrapper.cpp
        surface.cpp
        texture.cpp
        vertices.cpp
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
        SDL3-shared
        SDL3_ttf
        SDL3_image
)
