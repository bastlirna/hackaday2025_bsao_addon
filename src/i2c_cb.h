#pragma once

#include "i2c_types.h"

i2c_error_t i2c_set_mode_cb(i2c_msg_set_mode_t *msg);
i2c_error_t i2c_set_led_cb(i2c_msg_set_led_t *msg);
i2c_error_t i2c_get_mode_cb();
i2c_error_t i2c_reset_cb();
i2c_error_t i2c_get_version_cb();