#include <spdlog/spdlog.h>

#include "game/game.hpp"

int main()
{
    spdlog::info("Starting Schron!");

    game::Game g;
    g.start();

    spdlog::info("Done");

    return 0;
}
