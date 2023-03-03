#include "editor.hpp"

#include "sdlwrapper/sdlwrapper.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, const double& playerX, const double& playerY) :
    playerX(playerX),
    playerY(playerY),
    level(level)
{}

Editor::~Editor() = default;

void Editor::render(sdl::Renderer&)
{
    for (auto& [_, sector] : level.map)
    {
        (void)sector;
    }
}
}