#pragma once

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/texture.hpp"

#include <array>
#include <functional>

namespace sdl
{
class Renderer;
}

namespace engine
{
class Noise
{
    constexpr static auto noiseWidth{160};
    constexpr static auto noiseHeight{120};

public:

    Noise(sdl::Renderer& renderer, int& level);
    void render();

private:

    void fillWithNoise(const std::function<int()>& transparencyApplier);
    void generate();
    void generateLinear(int intensity);

    sdl::Renderer& renderer;
    sdl::Texture noise;
    int& level;

    std::array<sdl::Pixel, noiseWidth * noiseHeight> buffer{};
};
}
