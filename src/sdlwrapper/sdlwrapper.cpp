#include "sdlwrapper.hpp"

#include <SDL.h>
#include <spdlog/spdlog.h>

namespace sdl
{

void initialize()
{
    spdlog::info("SDL initialization");
    SDL_Init(SDL_INIT_EVERYTHING);
    // TTF_Init();
    SDL_ShowCursor(SDL_DISABLE);
}

void teardown()
{
    spdlog::debug("Quitting SDL");
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
    if (event.type == SDL_KEYDOWN and
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
    return event.type == SDL_QUIT;
}
}
