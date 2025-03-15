#include <Arduino.h>
#include <Wire.h>
#include "i2c.h"
#include "i2c_types.h"
#include "i2c_cb.h"
#include "leds.h"

#define I2C_SLAVE_ADDRESS 0x2c // the 7-bit address (remember to change this when adapting this example)

static struct
{
    uint8_t txBuff[8];
    uint8_t txBuffLength;
    i2c_error_t lastError;
} state;

static void send_status(i2c_error_t err);
static i2c_error_t read_message(uint8_t *msg, uint8_t max_msg_size);

void I2C_RxHandler(int numBytes)
{
    if (Wire.available())
    {
        uint8_t buffer[8];
        uint8_t expectedSize = 0;
        i2c_command_t cmd = static_cast<i2c_command_t>(Wire.read());
        switch (cmd)
        {
        case i2cmd_set_mode:
            expectedSize = sizeof(i2c_msg_set_mode_t);
            break;
        case i2cmd_set_led:
            expectedSize = sizeof(i2c_msg_set_led_t);
            break;
        case i2cmd_status:
        case i2cmd_reset:
            break;
        default:
            state.lastError = i2err_unk_cmd;
            return;
        }

        if (expectedSize > 0 && Wire.available())
        {
            // Write command
            i2c_error_t err = read_message(buffer, expectedSize);
            if (err != i2err_ok)
            {
                state.lastError = err;
                return;
            }

            switch (cmd)
            {
            case i2cmd_set_mode:
                state.lastError = i2c_set_mode_cb((i2c_msg_set_mode_t *)buffer);
                break;
            case i2cmd_set_led:
                state.lastError = i2c_set_led_cb((i2c_msg_set_led_t *)buffer);
                break;
            default:
                return;
            }
        }
        else
        {
            // Read command
            switch (cmd)
            {
            case i2cmd_set_mode:
                state.lastError = i2c_get_mode_cb();
                break;
            case i2cmd_reset:
                state.lastError = i2c_reset_cb();
                break;
            case i2cmd_status:
                send_status(state.lastError);
                return;
            default:
                return;
            }
        }
    }
}

void I2C_TxHandler(void)
{
    if (state.txBuffLength > 0)
    {
        Wire.write((uint8_t *)state.txBuff, state.txBuffLength);
        state.txBuffLength = 0;
    }
}

static i2c_error_t read_message(uint8_t *msg, uint8_t max_msg_size)
{
    uint8_t i = 0;
    while (Wire.available())
    {
        if (i >= max_msg_size)
        {
            return i2err_err_msg_to_long;
        }
        msg[i++] = Wire.read();
    }

    if (i == 0)
    {
        return i2err_err_msg_to_short;
    }

    return i2err_ok;
}

static void send_status(i2c_error_t err)
{
    i2c_reply_status_t *msg = (i2c_reply_status_t *)state.txBuff;
    msg->err = err;
    msg->version = 1;
    msg->reserved = 0;
    state.txBuffLength = sizeof(i2c_reply_status_t);
}

void i2c_reply(const uint8_t *buff, uint8_t size)
{
    if (size > sizeof(state.txBuff))
    {
        size = sizeof(state.txBuff);
    }
    memcpy(state.txBuff, buff, size);
    state.txBuffLength = sizeof(i2c_reply_status_t);
}

void i2c_setup()
{
    Wire.begin(I2C_SLAVE_ADDRESS); // Initialize I2C (Slave Mode: address=0x55 )
    Wire.onReceive(I2C_RxHandler);
    Wire.onRequest(I2C_TxHandler);
}
