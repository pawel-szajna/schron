add_library(
        scripting
        STATIC
        scripting.cpp
        ui_bindings.cpp
)

target_include_directories(
        scripting
        PUBLIC
        ${LUA_INCLUDE_DIR}
)

target_link_directories(
        scripting
        PUBLIC
)

if(NOT MSVC)
    target_compile_options(
        scripting
        PRIVATE
        -Wall -Werror -pedantic -O3
    )
endif()

target_link_libraries(
        scripting
        PRIVATE
        sol2
        ${LUA_LIBRARIES}
        ui
)
