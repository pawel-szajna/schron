#include "editor.hpp"

#include "sdlwrapper/sdlwrapper.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, const double& playerX, const double& playerY) :
    Object(-1, 1024 - 512 - 16, 768 - 384 - 16),
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