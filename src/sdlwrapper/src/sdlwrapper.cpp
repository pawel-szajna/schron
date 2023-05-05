#include "sdlwrapper.hpp"

#include "util/format.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>
#include <spdlog/spdlog.h>

namespace sdl
{

void initialize()
{
    SPDLOG_INFO("SDL initialization");
    SPDLOG_DEBUG("SDL main init");

    auto drivers = SDL_GetNumVideoDrivers();
    for (int i = 0; i < drivers; ++i)
    {
        SPDLOG_DEBUG("Available driver #{}: {}", i, SDL_GetVideoDriver(i));
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL: {}", SDL_GetError())};
    }
    SPDLOG_DEBUG("SDL_image init");
    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL_image: {}", IMG_GetError())};
    }
    SPDLOG_DEBUG("SDL_ttf init");
    if (TTF_Init() != 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL_ttf: {}", TTF_GetError())};
    }
    SPDLOG_DEBUG("SDL up and running!");
    hideCursor();
}

void teardown()
{
    SPDLOG_DEBUG("Quitting SDL");
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void delay(int ms)
{
    SDL_Delay(ms);
}

uint64_t currentTime()
{
    return SDL_GetTicks();
}

uint64_t currentTimeNs()
{
    return SDL_GetTicksNS();
}

void resetCursor()
{
    SDL_SetCursor(SDL_GetDefaultCursor());
}

const uint8_t* keyboard()
{
    return SDL_GetKeyboardState(nullptr);
}

void showCursor()
{
    SDL_ShowCursor();
}

void hideCursor()
{
    SDL_HideCursor();
}
} // namespace sdl
