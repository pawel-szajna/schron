#pragma once

namespace c
{
extern bool frameLimit;
extern double shadowResolution;
extern int shadowDepth;
constexpr auto levelSize{32};
constexpr auto renderWidth{692};
constexpr auto renderHeight{384};
constexpr auto windowWidth{1024};
constexpr auto windowHeight{768};

void loadConfig();
}
