#pragma once

#include <stdint.h>

typedef enum
{
    i2cmd_status = 0x00,
    i2cmd_set_mode = 0x01,
    i2cmd_set_led = 0x02,
    i2cmd_reset = 0x05,
} i2c_command_t;

typedef enum
{
    i2err_ok = 0x00,
    i2err_err_msg_to_short = 0xa0,
    i2err_err_msg_to_long = 0xa1,
    i2err_invalid_data_value = 0x10,
    i2err_unk_cmd = 0xf0,
} i2c_error_t;

typedef struct
{
    uint8_t version : 4;
    uint8_t reserved : 4;
    uint8_t err;
} __attribute__((packed, aligned(1))) i2c_reply_status_t;

typedef struct
{
    uint8_t mode;
} __attribute__((packed, aligned(1))) i2c_msg_set_mode_t;

typedef struct
{
    uint8_t id;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed, aligned(1))) i2c_msg_set_led_t;