#pragma once

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/surface.hpp"
#include "sdlwrapper/texture.hpp"
#include "util/constants.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <queue>
#include <unordered_set>

namespace game
{
struct Position;
}

namespace sdl
{
class Renderer;
}

namespace world
{
class Level;
class Sector;
class Wall;
}

namespace engine
{
constexpr auto TEXTURE_WIDTH{120};
constexpr auto TEXTURE_HEIGHT{120};

class Engine
{
    struct SectorRenderParams
    {
        int id;
        int leftXBoundary, rightXBoundary;
        int depth{0};
        double offsetX{0}, offsetY{0}, offsetZ{0}, offsetAngle{0};
    };

public:

    Engine(sdl::Renderer& renderer, world::Level& level);

    void frame(const game::Position& player);
    void draw();

private:

    sdl::Surface& texture(const std::string& name);

    void renderWall(const world::Sector& sector,
                    const world::Wall& wall,
                    const game::Position& player);
    void renderSprites(const world::Sector& sector,
                       const game::Position& player);
    void line(int x, int yStart, int yEnd, int color);
    void texturedLine(int x,
                      int wallStart, int wallEnd,
                      int yStart, int yEnd,
                      sdl::Surface& t,
                      int textureX,
                      double distance);

    std::map<std::string, sdl::Surface> textures{};
    std::map<std::string, sdl::Surface> sprites{};
    std::array<int, c::renderWidth> limitTop{}, limitBottom{};
    std::array<sdl::Pixel, c::renderWidth * c::renderHeight> buffer{};
    std::queue<SectorRenderParams> renderQueue{};

    sdl::Renderer& renderer;
    sdl::Texture view;

    world::Level& level;
};
}
