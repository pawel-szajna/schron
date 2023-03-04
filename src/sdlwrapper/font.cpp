#include "font.hpp"

#include <SDL.h>
#include <SDL_ttf.h>

namespace sdl
{
Font::Font(const std::string& file, int size)
{
    assign(TTF_OpenFont(file.c_str(), size),
           "SDL TTF font");
}

Font::~Font()
{
    if (wrapped != nullptr)
    {
        TTF_CloseFont(wrapped);
    }
}
}
