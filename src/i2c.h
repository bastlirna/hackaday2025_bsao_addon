#pragma once

#include <stdint.h>

void i2c_setup();
void i2c_reply(const uint8_t *buff, uint8_t size);