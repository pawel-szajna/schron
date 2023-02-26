add_library(
        scripting
        scripting.cpp
        ui_bindings.cpp
)

target_include_directories(
        scripting
        PUBLIC
        ../../lib/sol2/include
        /usr/include/lua5.3
)

target_compile_options(
        scripting
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        scripting
        PUBLIC
        lua5.3
)
