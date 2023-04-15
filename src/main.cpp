#include <spdlog/spdlog.h>

#include "game/game.hpp"
#include "sdlwrapper/sdlwrapper.hpp"

int main()
#if defined(RELEASE_BUILD)
try
#endif
{
#if defined(RELEASE_BUILD)
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::debug);
#endif
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
#if defined(RELEASE_BUILD)
catch (std::exception& e)
{
    spdlog::critical("Unhandled {}: {}", typeid(e).name(), e.what());
    throw;
}
#endif
