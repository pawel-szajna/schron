#include "editor.hpp"

#include "sdlwrapper/common_types.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "util/constants.hpp"
#include "world/level.hpp"

#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, const double& playerX, const double& playerY) :
    playerX(playerX),
    playerY(playerY),
    level(level)
{
    sdl::showCursor();
}

Editor::~Editor()
{
    sdl::hideCursor();
}

void Editor::event(const sdl::event::Event& event)
{}

void Editor::render(sdl::Renderer& renderer)
{
    constexpr auto x = c::windowWidth - 300;
    constexpr auto y = 300;
    constexpr auto mapScale = 32;

    std::vector<sdl::Vertex> vertices{};

    for (auto& [_, sector] : level.map)
    {
        vertices.clear();
        std::transform(sector.walls.begin(),
                       sector.walls.end(),
                       std::back_inserter(vertices),
                       [&](const auto& wall)
                       {
                           float vertexX = x + (wall.xStart - playerX) * mapScale;
                           float vertexY = y + (wall.yStart - playerY) * mapScale;
                           return sdl::Vertex{vertexX, vertexY, 100, 100, 100, 255};
                       });
        renderer.setColor(255, 255, 255, 130);
        renderer.renderGeometry(vertices);
    }
}
}
