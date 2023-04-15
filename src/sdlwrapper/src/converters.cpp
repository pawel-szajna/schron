#include "converters.hpp"

#include <stdexcept>

namespace sdl::convert
{
SDL_BlendMode BlendMode_to_SDL_BlendMode(BlendMode blendMode)
{
    switch (blendMode)
    {
    case BlendMode::None:
        return SDL_BLENDMODE_NONE;
    case BlendMode::Blend:
        return SDL_BLENDMODE_BLEND;
    case BlendMode::Add:
        return SDL_BLENDMODE_ADD;
    case BlendMode::Mod:
        return SDL_BLENDMODE_MOD;
    default:
        throw std::invalid_argument{"unsupported sdl::BlendMode value"};
    }
}
} // namespace sdl::convert
