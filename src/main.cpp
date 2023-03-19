#include <spdlog/spdlog.h>

#include "game/game.hpp"
#include "sdlwrapper/sdlwrapper.hpp"

int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting Schron!");

    c::loadConfig();

    sdl::initialize();

    {
        game::Game g;
        g.start();
    }

    sdl::teardown();
    spdlog::info("Done");

    return 0;
}
