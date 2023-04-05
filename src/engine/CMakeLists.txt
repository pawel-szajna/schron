add_module(
    NAME engine
    TYPE STATIC
    SOURCES
        engine.cpp
        lighting.cpp
        noise.cpp
    DEPENDENCIES
        game
        sdlwrapper
        world
)

if(NOT DISABLE_PARALLELISM)
    find_package(Boost REQUIRED)
    target_include_directories(
        engine
        PRIVATE
            ${Boost_INCLUDE_DIR}
    )
    if(APPLE)
        find_package(oneDPL REQUIRED)
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

if(NOT MSVC)
    target_compile_options(
        engine
        PRIVATE
            -Wall -Werror -pedantic -O3
    )
endif()
