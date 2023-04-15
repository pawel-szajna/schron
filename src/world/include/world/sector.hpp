#pragma once

#include <map>
#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

namespace world
{
/**
 * @class Wall
 * @brief A wall on a sector edge.
 *
 * The wall, when the level map is viewed from a top-down perspective, is an edge
 * of the convex polygon representing a single Sector.
 *
 * The wall is defined by its start and end X/Y-coordinates.
 *
 * The wall is rendered as a 2D rectangle in the 3D space, with the following
 * vertices:
 *
 * - [xStart, yStart, sector.floor]
 * - [xStart, yStart, sector.ceiling]
 * - [xEnd, yEnd, sector.ceiling]
 * - [xEnd, yEnd, sector.floor]
 *
 * The wall might be marked as a Portal.
 */
class Wall
{
public:

    /**
     * @struct Portal
     * @brief A portal to another Sector.
     *
     * A Portal is a special kind of wall, which is connected to another Sector. When
     * a Wall is a Portal, instead of the wall texture, the target Sector is rendered.
     * If the target floor is higher than the current sector floor, or the target
     * ceiling is lower than the current sector ceiling, a part of the wall is rendered
     * with the wall texture, creating a window looking into the neighbouring sector.
     *
     * A Portal might have a Transformation defined.
     */
    struct Portal
    {
        /**
         * @struct Transformation
         * @brief A Transformation of the Portal.
         *
         * If a Transformation is defined, anything (light, player, vision), when passing
         * through the Portal, has its coordinates transformed. This allows for things such
         * as infinite corridors, non-euclidean geometry, etc.
         */
        struct Transformation
        {
            double x;
            double y;
            double z;
            double angle;
        };

        int sector;
        std::optional<Transformation> transform{std::nullopt};
    };

    double xStart, yStart, xEnd, yEnd;
    std::optional<Portal> portal{std::nullopt};
    std::string texture{"wall"};

    [[nodiscard]] std::string toLua(int sectorId) const;
};

/**
 * @class Sprite
 * @brief A 2D sprite.
 */
class Sprite
{
    struct Texture
    {
        double angle;
        std::string texture;
    };

public:

    int id;
    std::vector<Texture> textures;
    double x, y, z{0.5};
    double w{1.0}, h{1.0};
    double offset{0};
    bool shadows{true};
    double lightCenter{0.5};
    bool blocking{true};

    [[nodiscard]] std::string toLua(int sectorId) const;
    const std::string& texture(double angle) const;
};

/**
 * @class Light
 * @brief A light source.
 */
class Light
{
public:

    double x, y, z;
    double r, g, b;

    [[nodiscard]] std::string toLua(int sectorId) const;
};

/**
 * @class Sector
 * @brief The largest building block of a Level.
 *
 * A sector is a piece of the map, which is a concave polygon on the X/Y plane,
 * and has a ceiling and a floor, both of which have some set Z height.
 *
 * The sector is bounded by walls, any of which might be a portal looking into
 * another sector. In order to be rendered properly, walls have to be placed in
 * clockwise order (as any wall is defined by its vertices). Most of the engine
 * calculations assume this order and breaking this constraint will most likely
 * break the engine.
 *
 * The sector, apart from the walls, contains sprites and lights. Player, at any
 * given time, is present in exactly one sector. A position of any entity in the
 * world is defined by [Sector ID; X; Y; Z] coordinates, as the sectors/portals
 * system allows for one set of [X; Y; Z] coordinates to be part of many sectors.
 */
class Sector
{
public:

    int id{};
    std::vector<Wall> walls{};
    std::vector<Sprite> sprites{};
    std::vector<Light> lights{};
    double ceiling{1.0};
    double floor{0.0};
    std::string ceilingTexture{"ceiling"};
    std::string floorTexture{"floor"};

    Sector(int id,
           std::vector<Wall> walls,
           std::vector<Sprite> sprites,
           std::vector<Light> lights,
           double ceiling,
           double floor,
           std::string ceilingTexture,
           std::string floorTexture);
    ~Sector();

    /**
     * @brief Calculate outer bounds of the Sector.
     *
     * The member fields boundsTop, boundsLeft, boundsRight, boundsBottom will
     * point to the, respectively, topmost Y (northmost), leftmost X (westmost),
     * rightmost X (eastmost), and bottommost Y (southmost) vertices of sector
     * walls.
     */
    void recalculateBounds();
    double boundsTop{}, boundsLeft{}, boundsRight{}, boundsBottom{};

    [[nodiscard]] std::string toLua() const;
};
} // namespace world
