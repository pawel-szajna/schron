#include "cursor.hpp"

#include <SDL3/SDL.h>

namespace sdl
{
Cursor::Cursor(int id)
{
    assign(SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(id)), "SDL system cursor");
}

Cursor::~Cursor()
{
    if (wrapped != nullptr)
    {
        SDL_DestroyCursor(wrapped);
    }
}

void Cursor::activate() const
{
    SDL_SetCursor(wrapped);
}
} // namespace sdl
