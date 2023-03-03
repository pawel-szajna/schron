add_library(
        scripting
        scripting.cpp
        ui_bindings.cpp
)

target_include_directories(
        scripting
        PUBLIC
)

target_link_directories(
        scripting
        PUBLIC
)

if(MSVC)
    target_compile_options(
        scripting
        PRIVATE
        /Wall /WX /O2
    )
else()
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
        lua
        ui
)
