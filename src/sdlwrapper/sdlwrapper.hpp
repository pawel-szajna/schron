#pragma once

#include <cstdint>

namespace sdl
{
void initialize();
void teardown();

void delay(int ms);
void resetCursor();
uint64_t currentTime();
const uint8_t* keyboard();
void showCursor();
void hideCursor();
}
