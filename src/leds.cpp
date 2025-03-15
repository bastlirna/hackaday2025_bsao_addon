#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "leds.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 8 // LED pin on the ATTiny

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLEDS, PIN, NEO_GRB + NEO_KHZ800);

void colorWipe(uint32_t c, uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);

static struct
{
  volatile leds_animation_type_t expected_type;
  volatile leds_animation_type_t current_type;
  volatile leds_animation_req_type_t mode;
} state;

void leds_setTypeTo(leds_animation_type_t ct)
{
  state.mode = ct == anim_none ? anim_auto : anim_manual;
  state.expected_type = ct;
}

void leds_setColor(uint8_t id, uint8_t r, uint8_t g, uint8_t b)
{
  state.mode = anim_manual;
  state.expected_type = anim_none;

  strip.setPixelColor(id, strip.Color(r, g, b));
  strip.show();
}

leds_animation_type_t leds_getType()
{
  if (state.mode == anim_auto)
  {
    return anim_none;
  }
  return state.expected_type;
}

void leds_setup()
{

  pinMode(PIN, OUTPUT);
  digitalWrite(PIN, LOW);
  delay(10);

  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  state.current_type = anim_rainbowCycle;
  state.expected_type = anim_rainbowCycle;
  state.mode = anim_auto;
}

void leds_loop()
{

  switch (state.mode)
  {
  case anim_auto:
    state.expected_type = static_cast<leds_animation_type_t>(state.expected_type + 1);
    if (state.expected_type == leds_animation_type_length)
    {
      state.expected_type = static_cast<leds_animation_type_t>(anim_none + 1);
    }
    state.current_type = state.expected_type;
    break;
  case anim_manual:
    break;
  }

  switch (state.expected_type)
  {
  case anim_colorWipe:
    //  Some example procedures showing how to display to the pixels:
    colorWipe(strip.Color(255, 0, 0), 50); // Red
    colorWipe(strip.Color(0, 255, 0), 50); // Green
    colorWipe(strip.Color(0, 0, 255), 50); // Blue
    break;
  case anim_theaterChase:
    //   Send a theater pixel chase in...
    theaterChase(strip.Color(127, 127, 127), 50); // White
    theaterChase(strip.Color(127, 0, 0), 50);     // Red
    theaterChase(strip.Color(0, 0, 127), 50);     // Blue
    break;
  case anim_rainbow:
    rainbow(20);
    break;
  case anim_rainbowCycle:
    rainbowCycle(20);
    break;
  case anim_theaterChaseRainbow:
    theaterChaseRainbow(50);
    break;
  }
  state.current_type = state.expected_type;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    if (state.expected_type != state.current_type)
    {
      return;
    }
  }
}

void rainbow(uint8_t wait)
{
  uint16_t i, j;

  leds_animation_type_t current = state.current_type;
  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
    if (state.expected_type != state.current_type)
    {
      return;
    }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;
  leds_animation_type_t current = state.current_type;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
    if (state.expected_type != state.current_type)
    {
      return;
    }
  }
}

// Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait)
{
  leds_animation_type_t current = state.current_type;
  for (int j = 0; j < 10; j++)
  { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c); // turn every third pixel on
      }
      strip.show();

      delay(wait);
      if (state.expected_type != state.current_type)
      {
        return;
      }

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); // turn every third pixel off
      }
    }
  }
}

// Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
  leds_animation_type_t current = state.current_type;
  for (int j = 0; j < 256; j++)
  { // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, Wheel((i + j) % 255)); // turn every third pixel on
      }
      strip.show();

      delay(wait);
      if (state.expected_type != state.current_type)
      {
        return;
      }

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); // turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}