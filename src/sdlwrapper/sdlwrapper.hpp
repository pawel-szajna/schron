#pragma once

#include <cstdint>

namespace sdl
{
void initialize();
void teardown();

void delay(int ms);
double currentTime();
void pollEvents();
bool keyPressed(int key);
const uint8_t* keyboard();
bool quitEvent();
}
