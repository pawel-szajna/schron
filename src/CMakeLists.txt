include_directories(
        .
        ../lib/spdlog/include
)

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(scripting)
add_subdirectory(sdlwrapper)
add_subdirectory(ui)
add_subdirectory(util)
add_subdirectory(world)

add_module(
    NAME schron
    TYPE EXECUTABLE
    SOURCES
        main.cpp
    DEPENDENCIES
        game
        sdlwrapper
        util
)
