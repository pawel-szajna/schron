#include "noise.hpp"

namespace engine
{
Noise::Noise(int& level) :
    level(level),
    noise(sdl::make_alpha_surface(noiseWidth, noiseHeight))
{}

void Noise::render(sdl::Surface& target)
{
    if (level > 12)
    {
        generateLinear(level);
    }
    else
    {
        generate();
    }

    auto noiseBig = sdl::transform(noise, 4);
    noiseBig.render(target);
}

void Noise::fillWithNoise(const std::function<int()>& transparencyApplier)
{
    auto pixels = (uint32_t*)noise->pixels;
    for (auto y = 0; y < noiseHeight; ++y)
    {
        for (auto x = 0; x < noiseWidth; ++x)
        {
            auto pixel = rand() % 256;
            pixel = (pixel << 16) + (pixel << 8) + pixel;
            pixel += transparencyApplier();
            *pixels = pixel;
            ++pixels;
        }

        pixels += noise->pitch / 4;
        pixels -= noiseWidth;
    }
}

void Noise::generate()
{
    fillWithNoise([]() { return ((rand() % 1856) - 1600) << 24; });
}

void Noise::generateLinear(int intensity)
{
    fillWithNoise([intensity]()
                  {
                      if (intensity <= 64) return (rand() % (4 * intensity)) << 24;
                      if (intensity == 128) return 255 << 24;
                      return ((rand() % (512 - 4 * intensity)) + 4 * intensity - 256) << 24;
                  });
}
}
