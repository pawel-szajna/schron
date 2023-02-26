#include <spdlog/spdlog.h>
#include <memory>

#include "game/game.hpp"

int main()
{
    spdlog::info("Starting Schron!");
    sdl::initialize();

    {
        game::Game g;
        g.start();
    }

    sdl::teardown();
    spdlog::info("Done");

    return 0;
}
