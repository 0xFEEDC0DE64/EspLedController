#pragma once

// system includes
#include <array>

// 3rdparty lib includes
#include <FastLED.h>

extern std::array<CRGB, 5> leds;
extern bool ledAnimationEnabled;

void led_setup();
void led_update();
