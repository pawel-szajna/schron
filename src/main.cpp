#include <spdlog/spdlog.h>
#include <memory>

#include "game/game.hpp"

int main()
{
    spdlog::set_level(spdlog::level::debug);
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
