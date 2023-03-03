add_library(
        engine
        engine.cpp
        noise.cpp
)

if(MSVC)
    target_compile_options(
        engine
        PRIVATE
        /Wall /WX /O2
    )
else()
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
