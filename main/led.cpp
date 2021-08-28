#include "led.h"

// system includes
#include <array>

// esp-idf includes
#include <driver/ledc.h>

// Arduino includes
#include <Arduino.h>

// 3rdparty lib includes
#include <FastLED.h>
#include <espchrono.h>

using namespace std::chrono_literals;

#define LEDC_TIMER_13_BIT  13
#define LEDC_BASE_FREQ     5000

namespace {
std::array<CRGB, 5> leds;

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

const TProgmemPalette16 myRedWhiteBluePalette_p =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,

    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255);
void ChangePalettePeriodically();
void FillLEDsFromPaletteColors( uint8_t colorIndex);

espchrono::millis_clock::time_point lastRedraw{};
}

void led_setup()
{
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;

    ledcSetup(0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    ledcSetup(3, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(4, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(5, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    ledcSetup(6, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(7, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(8, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    ledcSetup(9, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(10, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(11, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    ledcSetup(12, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(13, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcSetup(14, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

    ledcAttachPin(27, 0);
    ledcAttachPin(33, 1);
    ledcAttachPin(32, 2);

    ledcAttachPin(23, 3);
    ledcAttachPin(25, 4);
    ledcAttachPin(26, 5);

    ledcAttachPin(19, 6);
    ledcAttachPin(21, 7);
    ledcAttachPin(22, 8);

    ledcAttachPin(18, 9);
    ledcAttachPin(17, 10);
    ledcAttachPin(16, 11);

    ledcAttachPin(13, 12);
    ledcAttachPin(14, 13);
    ledcAttachPin(15, 14);
}

void led_update()
{
    if (espchrono::ago(lastRedraw) < 20ms)
        return;
    lastRedraw = espchrono::millis_clock::now();

    ChangePalettePeriodically();

    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors(startIndex);

    ledcAnalogWrite(0, leds[0].red);
    ledcAnalogWrite(1, leds[0].green);
    ledcAnalogWrite(2, leds[0].blue);

    ledcAnalogWrite(3, leds[1].red);
    ledcAnalogWrite(4, leds[1].green);
    ledcAnalogWrite(5, leds[1].blue);

    ledcAnalogWrite(6, leds[2].red);
    ledcAnalogWrite(7, leds[2].green);
    ledcAnalogWrite(8, leds[2].blue);

    ledcAnalogWrite(9, leds[3].red);
    ledcAnalogWrite(10, leds[3].green);
    ledcAnalogWrite(11, leds[3].blue);

    ledcAnalogWrite(12, leds[4].red);
    ledcAnalogWrite(13, leds[4].green);
    ledcAnalogWrite(14, leds[4].blue);
}

namespace {
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax)
{
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for( int i = 0; i < leds.size(); ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;

}

void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;

    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

void ChangePalettePeriodically()
{
    constexpr auto millis = []() -> unsigned int { return espchrono::millis_clock::now().time_since_epoch().count(); };

    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;

    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}
} // namespace
