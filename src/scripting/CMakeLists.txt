add_module(
    NAME scripting
    TYPE STATIC
    SOURCES
        scripting.cpp
        ui_bindings.cpp
        world_bindings.cpp
    INCLUDES
        ${LUA_INCLUDE_DIR}
    DEPENDENCIES
        ${LUA_LIBRARIES}
        sdlwrapper
        sol2
        ui
        world
)

if(NOT MSVC)
    target_compile_options(
        scripting
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()
