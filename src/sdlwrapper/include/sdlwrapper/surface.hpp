#pragma once

#include "common_types.hpp"
#include "wrapped.hpp"

#include <optional>
#include <string>

struct SDL_Surface;

namespace sdl
{
class Texture;

/**
 * @class Surface
 * @brief A low level rendering surface.
 *
 * This wraps a SDL_Surface, giving direct access to the surface pixels.
 */
class Surface : public Wrapped<SDL_Surface>
{
    friend class Font;

public:

    /**
     * @brief Constructs an empty surface with given dimensions.
     * @param width
     * @param height
     *
     * Uses SDL_CreateSurface to construct the Surface.
     */
    Surface(int width, int height);

    /**
     * @brief Constructs a surface from image file.
     * @param filename
     *
     * Uses SDL_IMG to load the image and then transform it to a Surface.
     */
    explicit Surface(const std::string& filename);

    /**
     * @brief Wraps an externally constructed surface.
     * @param surfacePtr Pointer to an existing SDL_Surface.
     *
     * This class assumes ownership of the provided SDL_Surface. This
     * surface is not to be destroyed manually.
     */
    explicit Surface(SDL_Surface* surfacePtr);

    Surface(Surface&& other) noexcept;
    Surface& operator=(Surface&& other) noexcept;
    ~Surface();

    /**
     * @brief Clears the surface.
     */
    void empty();

    /**
     * @brief Updates the target Texture with this Surface raw pixel data.
     * @param target
     */
    void render(Texture& target);

    /**
     * @brief Renders this surface contents onto another Surface.
     * @param target
     * @param where Target coordinates.
     *
     * This calls SDL_BlitSurface.
     */
    void render(Surface& target,
                std::optional<Rectangle> where = std::nullopt);

    /**
     * @brief Renders this surface onto another Surface, applying scaling.
     * @param target
     * @param where Target coordinates.
     */
    void renderScaled(Surface& target, Rectangle where);

    /**
     * Provides access to raw pixel data.
     * @return Pointer to the pixels array.
     */
    uint32_t* pixels();
    [[nodiscard]] const uint32_t* pixels() const;
    uint32_t& operator[](int index);

    int width, height;
};
}