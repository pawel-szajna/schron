#pragma once

#include "common_types.hpp"
#include "wrapped.hpp"

#include <cstdint>

struct SDL_Texture;

namespace sdl
{
class Renderer;

/**
 * @class Texture
 * @brief A texture object.
 *
 * Wraps a SDL_Texture.
 */
class Texture : public Wrapped<SDL_Texture>
{
public:

    enum class Access
    {
        Static,
        Streaming,
        Target,
    };

    Texture(Renderer& renderer, Access access, int width, int height);
    ~Texture();

    void setBlendMode(BlendMode blendMode);

    void update(uint32_t* pixels);

private:

    int width;
};
}
