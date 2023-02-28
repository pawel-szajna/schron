#include "editor.hpp"

#include "sdlwrapper/sdlwrapper.hpp"
#include "world/level.hpp"
#include "world/sector.hpp"

#include <spdlog/spdlog.h>

namespace ui::editor
{
Editor::Editor(world::Level& level, double& playerX, double& playerY) :
    Object(-1, 1024 - 512 - 16, 768 - 384 - 16),
    playerX(playerX),
    playerY(playerY),
    level(level),
    surface(sdl::make_alpha_surface(512, 384))
{}

Editor::~Editor() = default;

void Editor::render(sdl::Surface& target, SDL_Rect coords)
{
    constexpr static int scale = 32;
    // TODO: raw SDL calls
    SDL_FillRect(*surface, nullptr, 0x66aaaaaa);
/*
    auto rect = [&](int x, int y, int w, int h, bool north = false, bool east = false, bool south = false, bool west = false)
    {
        constexpr static auto lineWeight = 3;

        SDL_Rect pos{};

        pos = {(int16_t)x, (int16_t)y, (uint16_t)w, (uint16_t)h};
        SDL_FillRect(*surface, &pos, 0x88888888);

        pos = {(int16_t)(x - 1), (int16_t)(y - 1), (uint16_t)(w + 1), (uint16_t)lineWeight};
        SDL_FillRect(*surface, &pos, north ? 0x88777777 : 0x88555555);

        pos = {(int16_t)(x + w - lineWeight + 1), (int16_t)(y - 1), (uint16_t)lineWeight, (uint16_t)(h + 1)};
        SDL_FillRect(*surface, &pos, east ? 0x88777777 : 0x88555555);

        pos = {(int16_t)x, (int16_t)(y + h - lineWeight + 1), (uint16_t)w, (uint16_t)lineWeight};
        SDL_FillRect(*surface, &pos, south ? 0x88777777 : 0x88555555);

        pos = {(int16_t)(x - 1), (int16_t)(y - 1), (uint16_t)lineWeight, (uint16_t)(h + 1)};
        SDL_FillRect(*surface, &pos, west ? 0x88777777 : 0x88555555);
    };
*/
    //for (auto& [_, sector] : level.map)
//    {
//        rect(scale * sector.x1 - mapX,
//             scale * sector.y1 - mapY,
//             scale * (sector.x2 - sector.x1),
//             scale * (sector.y2 - sector.y1),
//             sector.north != world::Sector::NoNeighbour,
//             sector.east  != world::Sector::NoNeighbour,
//             sector.south != world::Sector::NoNeighbour,
//             sector.west  != world::Sector::NoNeighbour);
//    }

    SDL_Rect playerPos{(int16_t)(playerX * scale - mapX - 2), (int16_t)(playerY * scale - mapY - 2), 4, 4};
    SDL_FillRect(*surface, &playerPos, 0xaaff0000);

    surface.render(target, coords);
}
}