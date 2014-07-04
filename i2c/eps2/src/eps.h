#ifndef __eps_H__
#define __eps_H__

#define EPS_PROTOCOL_VERSION 1
#define EPS_MIN_VERSION_REQUIRE   1

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


#define EPS_SETUP           1
#define EPS_SET             2
#define EPS_GET             3
#define EPS_ALL             4
#define EPS_PING            5
#define EPS_PONG            6
#define EPS_RESET           7
#define EPS_VERSION         8
#define EPS_INIT            9

#define MASTER_ID           1

#include "Configuration_eps.h"
#include "Board.h"
#include <Wire.h>

class Board;

#define WRITE_VPIN( p, v) eps_set_vpin_value( p, v)
#define READ_VPIN( p ) eps_read_vpin_value( p )
#define READ_VPIN_MODE( p ) eps_read_vpin_type( p )
#define VPIN_MODE( p, v) eps_write_vpin_type( p, v)

extern Board boards[NUM_BOARD];
extern bool send_entries_flag;

uint8_t vpin2bpin(int vpin);
uint8_t vpin2board(int vpin);

// READ
int board_read_bpin_value( uint8_t b, uint8_t pin );
uint8_t board_read_bpin_type( uint8_t b, uint8_t pin );

int eps_read_vpin_value( int pin );
uint8_t eps_read_vpin_type( int pin );

// WRITE
void eps_set_vpin_value( int pin, int value);
void eps_write_vpin_type( int pin, uint8_t type);

void eps_send_action( uint8_t dest, uint8_t action );
void eps_send_version( int dest );

void eps_send_board_update(uint8_t dest);
byte eps_send_entries(uint8_t dest);

void i2cReceiveEvent(int howMany);
void setup_slave_master();


#ifdef IS_MASTER // welll...arduino dont know new/delete...but it's ok for INO...
 void operator delete(void * p);
 void * operator new(size_t size); 
#endif


#endif
