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

#include "Configuration.h"
#include "Board.h"
#include <Wire.h>

class Board;


extern Board boards[NUM_BOARD];

uint8_t vpin2bpin(int vpin);
uint8_t vpin2board(int vpin);

int board_read_bpin_value( uint8_t b, uint8_t pin );
uint8_t board_read_bpin_type( uint8_t b, uint8_t pin );

void i2c_send_action( uint8_t dest, uint8_t action );
void i2c_send_version( int dest );

void i2c_send_board_update(uint8_t dest);
byte i2c_send_entries(uint8_t dest);

#endif
