add_library(
        raycaster
        caster.cpp
)

target_compile_options(
        raycaster
        PRIVATE
        -Wall -Werror -pedantic -O3
)

target_link_libraries(
        raycaster
        PUBLIC
)
