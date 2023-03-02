#pragma once

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/surface.hpp"

#include <array>
#include <functional>

namespace engine
{
class Noise
{
    constexpr static auto noiseWidth{160};
    constexpr static auto noiseHeight{120};

public:

    Noise(int& level);
    void render(sdl::Surface& target);

private:

    void fillWithNoise(const std::function<int()>& transparencyApplier);
    void generate();
    void generateLinear(int intensity);

    int& level;
    sdl::Surface noise{1024, 768};
};
}
