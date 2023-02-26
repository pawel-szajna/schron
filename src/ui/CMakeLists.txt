add_library(
        ui
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
