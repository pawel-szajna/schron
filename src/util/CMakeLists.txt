add_module(
    NAME util
    TYPE STATIC
    SOURCES
        constants.cpp
    INCLUDES
        ../../lib/mini-yaml/yaml
        ../../lib/sol2/include
        ${LUA_INCLUDE_DIR}
)

if(NOT MSVC)
    target_compile_options(
        util
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()
