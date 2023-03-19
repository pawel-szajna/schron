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

class Text : public Object
{
public:

    Text(sdl::Renderer& renderer, Fonts& fonts, int width, int height, int x = 0, int y = 0);
    virtual ~Text();

    void render(sdl::Renderer& renderer) override;
    void event(const sdl::event::Event& event) override;

    void clear();
    void write(std::string text, std::string font, int charsPerSecond, bool flashingLetters = true);

    void move(int x, int y);

private:

    void advance();

    struct Request
    {
        std::string text{};
        std::string font{};
        int flashSpeed{100};
        int charsPerSecond{5};
    };

    struct Flash
    {
        std::string part;
        uint64_t finishTime;
        int x, y;
    };

    struct Typing
    {
        explicit Typing(Request&& request, int x, int y) :
            request(request),
            x(x), y(y),
            flashed(c::windowWidth, c::windowHeight)
        {}

        Request request;
        int position{};
        int verified{};
        int x{};
        int y{};
        double lastLetterAdded{};
        std::deque<Flash> flashes{};
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
}