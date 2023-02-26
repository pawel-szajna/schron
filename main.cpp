#include <iostream>

#include "sdlwrapper/sdlwrapper.hpp"

int main()
{
    sdl::initialize();
    auto screen = sdl::make_main_window(800, 600, false);
    sdl::delay(1000);
    sdl::teardown();
    return 0;
}
