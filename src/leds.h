#pragma once

#define NLEDS 12 // Total number of addressable leds

typedef enum
{
    anim_auto = 0,
    anim_manual
} leds_animation_req_type_t;

typedef enum
{
    anim_none = 0,
    anim_colorWipe,
    anim_theaterChase,
    anim_rainbow,
    anim_rainbowCycle,
    anim_theaterChaseRainbow,
    leds_animation_type_length
} leds_animation_type_t;

void leds_setup();
void leds_loop();
void leds_setTypeTo(leds_animation_type_t ct);
void leds_setColor(uint8_t id, uint8_t r, uint8_t g, uint8_t b);
leds_animation_type_t leds_getType();
