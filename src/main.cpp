#include <Arduino.h>
#include "leds.h"
#include "i2c.h"
#include "i2c_cb.h"

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

i2c_error_t i2c_set_mode_cb(i2c_msg_set_mode_t *msg)
{
  if (msg->mode >= leds_animation_type_length)
  {
    return i2err_invalid_data_value;
  }

  leds_setTypeTo(static_cast<leds_animation_type_t>(msg->mode));

  return i2err_ok;
}

i2c_error_t i2c_get_mode_cb()
{
  i2c_msg_set_mode_t msg = {
      .mode = leds_getType(),
  };
  i2c_reply((const uint8_t *)&msg, sizeof(msg));
  return i2err_ok;
}

i2c_error_t i2c_reset_cb()
{
  return i2err_ok;
}

i2c_error_t i2c_set_led_cb(i2c_msg_set_led_t *msg)
{
  if (msg->id >= NLEDS)
  {
    return i2err_invalid_data_value;
  }

  leds_setColor(msg->id, msg->r, msg->g, msg->b);

  return i2err_ok;
}

void setup()
{
// This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined(__AVR_ATtiny85__)
  if (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  leds_setup();
  i2c_setup();
}

void loop()
{
  leds_loop();
}
