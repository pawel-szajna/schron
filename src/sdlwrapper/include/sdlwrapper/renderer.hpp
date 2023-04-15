#pragma once

#include "common_types.hpp"
#include "texture.hpp"
#include "wrapped.hpp"

#include <optional>
#include <vector>

struct SDL_Renderer;

namespace sdl
{
class Surface;
class Vertices;
class Window;

/**
 * @class Renderer
 * @brief A renderer object, tied to a Window or Surface.
 *
 * This object wraps a SDL_Renderer and provides an interface to several
 * SDL functions operating on a SDL_Renderer.
 */
class Renderer : public Wrapped<SDL_Renderer>
{
public:

    ~Renderer();

    /**
     * @brief Constructs a Renderer tied to a Window.
     * @param window Window rendering target.
     * @param flags Renderer flags.
     */
    explicit Renderer(Window& window, uint32_t flags = 0);

    /**
     * @brief Constructs a Renderer tied to a Surface.
     * @param surface Surface rendering target.
     */
    explicit Renderer(Surface& surface);

    /**
     * @brief Clears the rendering target.
     *
     * Calls the SDL_RenderClear function.
     */
    void clear();

    /**
     * @brief Copies a texture to the rendering target.
     * @param texture Texture to render.
     * @param source Source clipping rectangle.
     * @param target Target clipping rectangle.
     *
     * Calls the SDL_RenderTexture function.
     */
    void copy(Texture& texture,
              std::optional<FRectangle> source = std::nullopt,
              std::optional<FRectangle> target = std::nullopt);

    /**
     * @brief Presents the rendering output.
     *
     * Calls the SDL_RenderPresent function.
     */
    void present() noexcept;

    /**
     * @brief Changes the current drawing color.
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     * @param a Alpha channel.
     *
     * Calls the SDL_SetRenderDrawColor function.
     */
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /**
     * @brief Renders a shape.
     * @param vertices A vector of shape vertices.
     *
     * Uses the SDL_RenderGeometry function to render a convex polygon on the screen.
     * For polygons with more than three vertices, fan triangulation is used.
     */
    void renderGeometry(const std::vector<Vertex>& vertices);

    void renderLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void renderRectangle(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void renderBox(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /**
     * @brief Creates a Texture tied to this renderer.
     * @param access Texture access mode
     * @param width
     * @param height
     * @return Created Texture.
     */
    Texture createTexture(Texture::Access access, int width, int height);
};
} // namespace sdl
