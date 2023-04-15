#include "noise.hpp"

#include "sdlwrapper/renderer.hpp"

namespace engine
{
Noise::Noise(sdl::Renderer& renderer, int& level)
    : renderer(renderer)
    , noise(renderer.createTexture(sdl::Texture::Access::Streaming, noiseWidth, noiseHeight))
    , level(level)
{
    noise.setBlendMode(sdl::BlendMode::Blend);
}

void Noise::render()
{
    if (level > 12)
    {
        generateLinear(level - 4);
    }
    else
    {
        generate();
    }

    noise.update(buffer.data());
    renderer.copy(noise);
}

void Noise::fillWithNoise(const std::function<int()>& transparencyApplier)
{
    for (auto y = 0; y < noiseHeight; ++y)
    {
        for (auto x = 0; x < noiseWidth; ++x)
        {
            auto pixel = rand() % 256;
            pixel      = (pixel << 16) + (pixel << 8) + pixel;
            pixel += transparencyApplier();
            buffer[y * noiseWidth + x] = pixel;
        }
    }
}

void Noise::generate()
{
    fillWithNoise([]() { return ((rand() % 1856) - 1600) << 24; });
}

void Noise::generateLinear(int intensity)
{
    fillWithNoise(
        [intensity]()
        {
            if (intensity <= 64)
            {
                return (rand() % (4 * intensity)) << 24;
            }
            if (intensity == 128)
            {
                return 255 << 24;
            }
            return ((rand() % (512 - 4 * intensity)) + 4 * intensity - 256) << 24;
        });
}
} // namespace engine
