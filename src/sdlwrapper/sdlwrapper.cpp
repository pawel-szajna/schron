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
    spdlog::info("SDL initialization");
    spdlog::debug("SDL main init");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL: {}", SDL_GetError())};
    }
    spdlog::debug("SDL_image init");
    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL_image: {}", IMG_GetError())};
    }
    spdlog::debug("SDL_ttf init");
    if (TTF_Init() != 0)
    {
        throw std::runtime_error{std::format("could not initialize SDL_ttf: {}", TTF_GetError())};
    }
    spdlog::debug("SDL up and running!");
    SDL_HideCursor();
}

void teardown()
{
    spdlog::debug("Quitting SDL");
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void delay(int ms)
{
    SDL_Delay(ms);
}

SDL_Event event{};

void pollEvents()
{
    SDL_PollEvent(&event);
}

double currentTime()
{
    return SDL_GetTicks();
}

bool keyPressed(int key)
{
    if (event.type == SDL_EVENT_KEY_DOWN and
        event.key.keysym.sym == key)
    {
        event.type = 0;
        return true;
    }

    return false;
}

const uint8_t* keyboard()
{
    return SDL_GetKeyboardState(nullptr);
}

bool quitEvent()
{
    return event.type == SDL_EVENT_QUIT;
}
}
