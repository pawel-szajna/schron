add_library(
        engine
        STATIC
        engine.cpp
        lighting.cpp
        noise.cpp
)

if(NOT MSVC)
    target_compile_options(
        engine
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

if(NOT DISABLE_PARALLELISM)
    if(APPLE)
        find_package(
            oneDPL
            REQUIRED)
        target_link_libraries(
            engine
            PUBLIC
            oneDPL
        )
    elseif(UNIX)
        target_link_libraries(
            engine
            PUBLIC
            tbb
        )
    endif()
else()
    target_compile_definitions(
        engine
        PUBLIC
        DISABLE_PARALLELISM
    )
endif()

target_link_libraries(
        engine
        PUBLIC
        sdlwrapper
)
