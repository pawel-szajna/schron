add_module(
    NAME world
    TYPE STATIC
    SOURCES
        builders.cpp
        level.cpp
        sector.cpp
        world.cpp
    INCLUDES
        ${LUA_INCLUDE_DIR}
    DEPENDENCIES
        ${LUA_LIBRARIES}
        scripting
        sol2
        util
)

if(NOT MSVC)
    target_compile_options(
        world
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()
