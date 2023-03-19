add_library(
        util
        STATIC
        constants.cpp
)

target_include_directories(
        util
        PRIVATE
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

target_link_libraries(
        util
        PUBLIC
)
