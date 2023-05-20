#pragma once

#include "object.hpp"
#include "sdlwrapper/surface.hpp"
#include "sdlwrapper/texture.hpp"
#include "util/constants.hpp"

#include <queue>
#include <string>

namespace ui
{
class Fonts;

/**
 * @brief An animated text widget.
 */
class Text : public Object
{
public:

    constexpr static auto NonAnimated{-1};

    Text(sdl::Renderer& renderer, Fonts& fonts, int width, int height, int x = 0, int y = 0);
    virtual ~Text();

    void render(sdl::Renderer& renderer) override;
    bool event(const sdl::event::Event& event) override;

    void clear();
    void write(std::string text, std::string font, int charsPerSecond, uint8_t color = 255);

    void move(int x, int y);

    [[nodiscard]] bool done() const;
    void finish();

private:

    void advance();
    void processQueue();

    struct Request
    {
        std::string text{};
        std::string font{};
        int charsPerSecond{5};
        uint8_t color{255};
    };

    struct Typing
    {
        explicit Typing(Request&& request, int x, int y)
            : request(request)
            , x(x)
            , y(y)
            , flashed(c::windowWidth, c::windowHeight)
        {
        }

        Request request;
        std::string::size_type position{};
        std::string::size_type verified{};
        int x{};
        int y{};
        uint64_t lastLetterAdded{};
        sdl::Surface flashed;
    };

    int width, height;
    int x, y;
    int lastX, lastY;

    std::optional<Typing> current{std::nullopt};
    std::queue<Request> requests{};

    sdl::Surface buffer;
    sdl::Texture texture;

    Fonts& fonts;
};
} // namespace ui
