#include "player.hpp"

#include "scripting/scripting.hpp"
#include "sdlwrapper/sdlwrapper.hpp"
#include "world/level.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace game
{
namespace
{
constexpr auto rotationSpeed{2.4};
constexpr auto movementSpeed{1.8};

bool overlap(double x1, double x2, double y1, double y2)
{
    return std::min(x1, x2) <= std::max(y1, y2) and
           std::min(y1, y2) <= std::max(x1, x2);
}

bool intersect(double box1x1, double box1y1, double box1x2, double box1y2,
               double box2x1, double box2y1, double box2x2, double box2y2)
{
    return overlap(box1x1, box1x2, box2x1, box2x2) and
           overlap(box1y1, box1y2, box2y1, box2y2);
}

double side(double px, double py, double x1, double y1, double x2, double y2)
{
    return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1);
}
}

Player::Player(scripting::Scripting& scripting, int& noiseLevel) :
    scripting(scripting),
    noiseLevel(noiseLevel)
{
    scripting.bind("sanity_set", &Player::setSanity, this);
    scripting.bind("sanity_mod", &Player::modSanity, this);
}

Player::~Player() = default; // Intentionally not unbinding sanity_set and sanity_mod, as the Player destructor
                             // is called after the Scripting destructor. This does, however, sound like a poor
                             // design and should probably be addressed in future.

const Position& Player::getPosition() const
{
    return position;
}

void Player::setSanity(int value)
{
    sanity = std::clamp(value, 0, 100);
    scripting.set("sanity", sanity);
    noiseLevel = 13 + (100 - sanity);
}

void Player::modSanity(int delta)
{
    setSanity(getSanity() + delta);
}

int Player::getSanity() const
{
    return sanity;
}

void Player::move(const world::Level& level, Direction direction)
{
    if (moving or rotating)
    {
        return;
    }

    MoveTarget move{position.x, position.y, position.z};

    switch (direction)
    {
    case Direction::Forward:
        move.x += std::cos(position.angle);
        move.y += std::sin(position.angle);
        move.factor = 1;
        break;
    case Direction::Backward:
        move.x -= std::cos(position.angle);
        move.y -= std::sin(position.angle);
        move.factor = -1;
        break;
    case Direction::Left:
    case Direction::Right:
        break;
    }

    auto bump = enterable(level, move.x, move.y);
    if (bump <= 0)
    {
        moves.emplace(move);
    }
    else
    {
        move.x -= move.factor * std::cos(position.angle) * bump;
        move.y -= move.factor * std::sin(position.angle) * bump;
        moves.emplace(move);
        move.x = position.x;
        move.y = position.y;
        move.factor *= -1;
        moves.emplace(move);
    }

    acquireMove();
}

void Player::acquireMove()
{
    if (moving == 0 and not moves.empty())
    {
        auto& nextMove = moves.front();
        target.x = nextMove.x;
        target.y = nextMove.y;
        target.z = nextMove.z;
        moving = nextMove.factor;
        moves.pop();
    }
}

void Player::animateFov(double fovH, double fovV, uint64_t targetTime)
{
    fovAnimation = {fovH, fovV, targetTime};
}

double Player::enterable(const world::Level& level, double x, double y) const
{
    constexpr static auto canEnter = -1.0;
    constexpr static auto wallBump = 0.77;
    constexpr static auto spriteBump = 0.45;

    const auto& sector = level.sector(position.sector);

    for (const auto& wall : sector.walls)
    {
        if (intersect(position.x, position.y, x, y, wall.xStart, wall.yStart, wall.xEnd, wall.yEnd) and
            side(x, y, wall.xStart, wall.yStart, wall.xEnd, wall.yEnd) < 0)
        {
            if (wall.portal)
            {
                const auto& neighbour = level.sector(wall.portal->sector);
                return std::abs(neighbour.floor - sector.floor) < 0.3 ? canEnter : wallBump;
            }

            return wallBump;
        }
    }

    for (const auto& sprite : sector.sprites)
    {
        if (sprite.blocking and std::abs(sprite.x - x) < 0.1 and std::abs(sprite.y - y) < 0.1)
        {
            return spriteBump;
        }
    }

    return canEnter;
}

void Player::rotate(Rotation rotation)
{
    if (moving or rotating)
    {
        return;
    }

    target = position;

    switch(rotation)
    {
    case Rotation::Left:
        target.angle -= std::numbers::pi / 2;
        rotating = -1;
        break;
    case Rotation::Right:
        target.angle += std::numbers::pi / 2;
        rotating = +1;
        break;
    }
}

void Player::frame(const world::Level& level, double frameTime)
{
    acquireMove();

    const auto& sector = level.sector(position.sector);
    auto targetZ = sector.floor + 0.6;
    if (position.z != targetZ)
    {
        position.z = std::clamp(targetZ, position.z - frameTime, position.z + frameTime * 2);
    }

    if (moving)
    {
        auto deltaX = frameTime * moving * movementSpeed * std::cos(position.angle);
        auto deltaY = frameTime * moving * movementSpeed * std::sin(position.angle);

        for (const auto& wall : sector.walls)
        {
            if (intersect(position.x, position.y, position.x + deltaX, position.y + deltaY,
                          wall.xStart, wall.yStart, wall.xEnd, wall.yEnd))
            {
                if (side(position.x + deltaX, position.y + deltaY, wall.xStart, wall.yStart, wall.xEnd, wall.yEnd) < 0)
                {
                    if (wall.portal.has_value())
                    {
                        const auto& portal = *wall.portal;
                        position.sector = portal.sector;
                        if (portal.transform.has_value())
                        {
                            spdlog::debug("Portal - old: {} {} {}", position.x, position.y, position.z);
                            position.x += portal.transform->x;
                            position.y += portal.transform->y;
                            position.z += portal.transform->z;
                            target.x += portal.transform->x;
                            target.y += portal.transform->y;
                            spdlog::debug("Portal - new: {} {} {}", position.x, position.y, position.z);
                            position.angle += portal.transform->angle;
                        }
                        break;
                    }
                }
            }
        }

        position.x += deltaX;
        position.y += deltaY;

        if ((target.x - position.x) * (moving * std::cos(position.angle)) <= 1e-4 and
            (target.y - position.y) * (moving * std::sin(position.angle)) <= 1e-4)
        {
            position.x = target.x;
            position.y = target.y;
            moving = 0;
            acquireMove();
        }
    }

    if (rotating != 0)
    {
        position.angle += rotating * frameTime * rotationSpeed;

        if ((target.angle - position.angle) * rotating < 0)
        {
            position.angle = target.angle;
            rotating = 0;
        }
    }

    if (fovAnimation)
    {
        auto now = sdl::currentTime();
        if (now >= fovAnimation->targetTime)
        {
            position.fovH = fovAnimation->fovH;
            position.fovV = fovAnimation->fovV;
            fovAnimation.reset();
        }
        else
        {
            double invRemaining = 1.0 / (double)(fovAnimation->targetTime - now);
            position.fovH += (fovAnimation->fovH - position.fovH) * invRemaining;
            position.fovV += (fovAnimation->fovV - position.fovV) * invRemaining;
        }
    }
}
}
