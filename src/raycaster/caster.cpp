#include "caster.hpp"

#include <algorithm>
#include <cmath>
#include <execution>
#include <ranges>
#include <spdlog/spdlog.h>

namespace raycaster
{
struct Sprite
{
    double x;
    double y;
    int texture;
    double distance;
    int dynamicId;
};

std::vector<Sprite> sprites;

namespace
{
enum class Hit : bool
{
    Horizontal,
    Vertical
};

constexpr auto sqr(auto x) { return x * x; }

constexpr auto outOfBounds(int x, int y, int size) { return x < 0 or y < 0 or x >= size or y >= size; }

constexpr auto color(uint32_t red, uint32_t green, uint32_t blue) { return 65536 * red + 256 * green + blue; }
constexpr auto gray(uint32_t brightness) { return color(brightness, brightness, brightness); }

namespace tile
{
constexpr auto type(int tile) { return tile % 16; }
constexpr auto texture(int tile) { return (tile / 16) % 16; }
}
}

void Caster::loadTexture(int id, const std::string& filename)
{
    spdlog::debug("Loading raycaster texture {}", filename);
    FILE *data = fopen(filename.c_str(), "rb");
    if (not data)
    {
        spdlog::warn("File not found: {}!", filename);
    }

    unsigned char dataR, dataG, dataB;
    auto& texture = textures[id];
    for (auto pixel = 0; pixel < TEXTURE_WIDTH * TEXTURE_HEIGHT; ++pixel)
    {
        fread(&dataR, sizeof(char), 1, data);
        fread(&dataG, sizeof(char), 1, data);
        fread(&dataB, sizeof(char), 1, data);
        texture[pixel] = 65536 * dataR + 256 * dataG + dataB;
    }

    fclose(data);
}

Caster::Caster() :
    world(sdl::make_surface(c::renderWidth, c::renderHeight))
{
    spdlog::info("Initializing caster");
    sprites.clear();
}

void Caster::removeSprite(int id)
{
    spdlog::debug("Remove sprite {}", id);
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [id](const auto& sprite) { return sprite.dynamicId == id; }),
                  sprites.end());
}

int Caster::addSprite(double x, double y, int texture)
{
    spdlog::debug("Add dynamic sprite @ {};{}", x, y);
    sprites.push_back(Sprite{x, y, texture, 0, ++dynamicSpritesCounter});
    return dynamicSpritesCounter;
}

void Caster::updateSprite(int id, double x, double y, int texture)
{
    auto sprite = std::find_if(sprites.begin(), sprites.end(), [id](const auto& sprite) { return sprite.dynamicId == id; });
    if (sprite == sprites.end())
    {
        spdlog::warn("Trying to modify non-existing sprite {}", id);
        return;
    }
    sprite->x = x;
    sprite->y = y;
    sprite->texture = texture;
}

void Caster::changeVisibility(double fullRange, double visibleRange)
{
    fadeStart = fullRange;
    fadeEnd = visibleRange;
}

void Caster::frame()
{
    for (auto y = 0; y < c::renderHeight / 2; ++y)
    {
        auto color = gray(std::max(0, 30 - 90 * y / c::renderHeight));
        for (auto x = 0; x < c::renderWidth; ++x)
        {
            buffer[x][y] = color;
            buffer[x][c::renderHeight - y - 1] = color;
        }
    }

    const auto/*&*/ position = util::Position{}; // player.getPosition();

    renderWalls(position);
    renderSprites(position);

    auto pixels = (Uint32*)(world->pixels);

    for (auto y = 0; y < c::renderHeight; ++y)
    {
        for (auto x = 0; x < c::renderWidth; ++x)
        {
            *pixels = buffer[x][y];
            ++pixels;
        }
        pixels += world->pitch / 4;
        pixels -= c::renderWidth;
    }
}

void Caster::fadePixel(uint32_t color, double distance, uint32_t& pixel) const
{
    if (distance >= fadeEnd)
    {
        pixel = 0;
        return;
    }

    if (distance > fadeStart)
    {
        auto r = ((int32_t)color >> 16) % 256;
        auto g = ((int32_t)color >> 8) % 256;
        auto b = (int32_t)color % 256;
        auto fadeLength = fadeEnd - fadeStart;
        r *= (1 - ((distance - fadeStart) / (fadeLength - 0.1)));
        g *= (1 - ((distance - fadeStart) / (fadeLength)));
        b *= (1 - ((distance - fadeStart) / (fadeLength - 0.05)));
        color = std::max(r, 0) * 65536
                + std::max(g, 0) * 256
                + std::max(b, 0);
    }

    pixel = color;
}

void Caster::renderWalls(const util::Position& position)
{
    auto columns = std::ranges::views::iota(0, c::renderWidth);
    std::transform(std::execution::par,
                   std::cbegin(columns),
                   std::cend(columns),
                   std::begin(buffer),
                   [&] (auto x)
                   {
                       decltype(buffer)::value_type column{};

                       auto camX = 2 * x / (double)c::renderWidth - 1;
                       auto rayPX = position.x;
                       auto rayPY = position.y;
                       auto rayDX = position.dirX + position.planeX * camX;
                       auto rayDY = position.dirY + position.planeY * camX;
                       auto mapX = (int) rayPX;
                       auto mapY = (int) rayPY;
                       auto deltadX = sqrt(1 + sqr(rayDY) / sqr(rayDX));
                       auto deltadY = sqrt(1 + sqr(rayDX) / sqr(rayDY));
                       auto stepX = (rayDX < 0) ? -1 : 1;
                       auto stepY = (rayDY < 0) ? -1 : 1;
                       auto sidedX = (stepX < 0) ? (rayPX - mapX) * deltadX : (mapX + 1.0 - rayPX) * deltadX;
                       auto sidedY = (stepY < 0) ? (rayPY - mapY) * deltadY : (mapY + 1.0 - rayPY) * deltadY;
                       Hit side{};

                       while (not outOfBounds(mapX, mapY, c::levelSize))
                       {
                           if (sidedX < sidedY)
                           {
                               sidedX += deltadX;
                               mapX += stepX;
                               side = Hit::Horizontal;
                           }
                           else
                           {
                               sidedY += deltadY;
                               mapY += stepY;
                               side = Hit::Vertical;
                           }

                           if (tile::type(/*level.at(mapX, mapY)*/1) == 1)
                           {
                               break;
                           }
                       }

                       if (outOfBounds(mapX, mapY, c::levelSize))
                       {
                           return column;
                       }

                       auto wallDist = side == Hit::Vertical
                                       ? fabs((mapY - rayPY + (1 - stepY) / 2) / rayDY)
                                       : fabs((mapX - rayPX + (1 - stepX) / 2) / rayDX);

                       if (wallDist > fadeEnd)
                       {
                           zBuffer[x] = fadeEnd;
                           return column;
                       }

                       auto lineHeight = abs((int) (c::renderHeight / wallDist));
                       auto dStart = std::max(-lineHeight / 2 + c::renderHeight / 2, 0);
                       auto dEnd = std::min(lineHeight / 2 + c::renderHeight / 2, c::renderHeight - 1);

                       auto tex = (1024/*level.at(mapX, mapY)*/ >> 4);
                       if (side == Hit::Horizontal and position.x < mapX)
                       {
                           tex >>= 8;
                       }
                       if (side == Hit::Vertical)
                       {
                           if (position.y < mapY) tex >>= 12;
                           else tex >>= 4;
                       }
                       tex %= 16;

                       auto wallX = side == Hit::Vertical
                                    ? rayPX + ((mapY - rayPY + (1 - stepY) / 2) / rayDY) * rayDX
                                    : rayPY + ((mapX - rayPX + (1 - stepX) / 2) / rayDX) * rayDY;
                       wallX -= floor(wallX);
                       auto texX = (int) (wallX * (double) TEXTURE_WIDTH);
                       if ((side == Hit::Horizontal and rayDX > 0) or (side == Hit::Vertical && rayDY < 0))
                       {
                           texX = TEXTURE_WIDTH - texX - 1;
                       }

                       for (auto y = dStart; y < dEnd; ++y)
                       {
                           auto d = y * 256 - c::renderHeight * 128 + lineHeight * 128;
                           auto texY = ((d * TEXTURE_HEIGHT) / lineHeight) / 256;
                           auto color = textures[tex][TEXTURE_HEIGHT * texY + texX];
                           fadePixel(color, wallDist, column[y]);
                       }

                       zBuffer[x] = wallDist;
                       return column;
                   });
}

void Caster::renderSprites(const util::Position& position)
{
    std::transform(sprites.begin(), sprites.end(), sprites.begin(),
                   [&position](auto& sprite) {
                       sprite.distance = sqr(position.x - sprite.x) + sqr(position.y - sprite.y);
                       return sprite;
                   });

    std::sort(std::execution::par,
              sprites.begin(), sprites.end(),
              [](const auto& a, const auto& b) { return a.distance > b.distance; });

    for (const auto &sprite: sprites)
    {
        if (sprite.distance > (fadeEnd * fadeEnd))
        {
            continue;
        }

        auto invDet = 1.0 / (position.planeX * position.dirY - position.dirX * position.planeY);

        auto transX = invDet * (position.dirY * (sprite.x - position.x) - position.dirX * (sprite.y - position.y));
        auto transY = invDet * (-position.planeY * (sprite.x - position.x) + position.planeX * (sprite.y - position.y));

        auto spriteScreenX = (int) ((c::renderWidth / 2) * (1 + transX / transY));

        auto spriteHeight = abs((int) (c::renderHeight / transY));
        auto dStart = std::max(-spriteHeight / 2 + c::renderHeight / 2, 0);
        auto dEnd = std::min(spriteHeight / 2 + c::renderHeight / 2, c::renderHeight - 1);

        auto spriteWidth = abs((int) (c::renderHeight / transY));
        auto dStartX = std::max(-spriteWidth / 2 + spriteScreenX, 0);
        auto dEndX = std::min(spriteWidth / 2 + spriteScreenX, c::renderWidth - 1);

        for (auto x = dStartX; x < dEndX; ++x)
        {
            if (transY > 0 && x > 0 && x < c::renderWidth && transY < zBuffer[x])
                for (auto y = dStart; y < dEnd; ++y)
                {
                    auto texX = (int) (256 * (x - (-spriteWidth / 2 + spriteScreenX)) * TEXTURE_WIDTH / spriteWidth) / 256;
                    auto d = y * 256 - c::renderHeight * 128 + spriteHeight * 128;
                    auto texY = ((d * TEXTURE_HEIGHT) / spriteHeight) / 256;
                    auto color = textures[sprite.texture][TEXTURE_WIDTH * texY + texX];
                    if (color != 0x00FFFFFF)
                    {
                        fadePixel(color, transY, buffer[x][y]);
                    }
                }
        }
    }
}

void Caster::draw(sdl::Surface& target)
{
    world.draw(target);
}
}
