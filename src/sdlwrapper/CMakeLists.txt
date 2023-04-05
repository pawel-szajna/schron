add_module(
    NAME sdlwrapper
    TYPE STATIC
    SOURCES
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
    INCLUDES
        ../../lib/SDL/include
    DEPENDENCIES
        SDL3-shared
        SDL3_ttf
        SDL3_image
    PUBLIC_DEPENDENCIES
        util
)

if(NOT MSVC)
    target_compile_options(
        sdlwrapper
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()
