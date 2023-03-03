#pragma once

#include "common_types.hpp"

#include <SDL.h>

namespace sdl::convert
{
SDL_BlendMode BlendMode_to_SDL_BlendMode(sdl::BlendMode mode);
}
