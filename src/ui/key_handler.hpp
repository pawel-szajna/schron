#pragma once

#include <functional>
#include <string>
#include <unordered_map>

namespace ui
{
class KeyHandler
{
public:

    using KeyCallback = std::function<void(void)>;

    void reset();

    void map(int keycode, KeyCallback action);
    void unmap(int keycode);

    void check();

private:

    std::unordered_map<int, KeyCallback> keyBindings{};

};
}