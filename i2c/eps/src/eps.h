#ifndef __eps_H__
#define __eps_H__

#define eps_PROTOCOL_VERSION 1


#if defined(ARDUINO) && ARDUINO >= 100
#define eps_WRITE write
#define eps_READ read
#include "Arduino.h"
#else
#include "WProgram.h"
#define eps_WRITE send
#define eps_READ receive
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


extern Board boards[NUM_BOARD];

uint8_t vpin2bpin(int vpin);
uint8_t vpin2board(int vpin);

int board_read_bpin_value( uint8_t b, uint8_t pin );
uint8_t board_read_bpin_type( uint8_t b, uint8_t pin );

void eps_send_action( uint8_t dest, uint8_t action );
void eps_send_version( int dest );

void eps_send_board_update(uint8_t dest);
byte eps_send_entries(uint8_t dest);

#endif
