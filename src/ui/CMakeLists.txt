add_library(
        ui
        key_handler.cpp
        object.cpp
        ui.cpp
)

target_compile_options(
        ui
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        ui
        PUBLIC
)
