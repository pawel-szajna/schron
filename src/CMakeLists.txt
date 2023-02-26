include_directories(.)

add_subdirectory(game)
add_subdirectory(raycaster)
add_subdirectory(scripting)
add_subdirectory(sdlwrapper)
add_subdirectory(ui)
add_subdirectory(util)
add_subdirectory(world)

add_executable(
        schron
        main.cpp
)

target_link_libraries(
        schron
        PRIVATE
        game
        raycaster
        scripting
        sdlwrapper
        ui
        world
)
