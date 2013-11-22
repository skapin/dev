#ifndef __I2C_H__
#define __I2C_H__

#define I2C_PROTOCOL_VERSION 1


#if defined(ARDUINO) && ARDUINO >= 100
#define I2C_WRITE write
#define I2C_READ read
#include "Arduino.h"
#else
#include "WProgram.h"
#define I2C_WRITE send
#define I2C_READ receive
#endif


#include <inttypes.h>

/** We use an union to send float through I2C/TWI. Union allow us to share memory cell between Float and byte array.
 * So we use union as reinterpret_cast. Kinda classic but usefull.
 * Wire() class can send byte array and string but no float...
 * **/
#define FB_SIZE 2


#define I2C_SETUP           1
#define I2C_SET             2
#define I2C_GET             3
#define I2C_ALL             4
#define I2C_PING            5
#define I2C_PONG            6
#define I2C_RESET           7
#define I2C_VERSION         8
#define I2C_INIT            9

#define MASTER_ID           1


#include "Pin.h"
#include "Update.h"

#include <Wire.h>

void i2c_set_vpin_value( Pin* pin_values, uint8_t pin, int value);
void i2c_set_vpin_type( Pin* pin_values, uint8_t pin, int type);
void i2c_send_action( uint8_t sender, uint8_t dest, uint8_t action );

#endif
