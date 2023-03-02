#include "noise.hpp"

namespace engine
{
Noise::Noise(int& level) :
    level(level)
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

    noise.render(target);
}

void Noise::fillWithNoise(const std::function<int()>& transparencyApplier)
{
    auto pixels = noise.pixels();
    for (auto y = 0; y < 768; y += 8)
    {
        for (auto x = 0; x < 1024; x += 8)
        {
            auto pixel = rand() % 256;
            pixel = (pixel << 16) + (pixel << 8) + pixel;
            pixel += transparencyApplier();
            for (int i = 0; i < 8; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    pixels[(y + j) * 1024 + (x + i)] = pixel;
                }
            }
        }
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
