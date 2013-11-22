#ifndef __IC2_SLAVE__
#define __IC2_SLAVE__


#include <SPI.h>

#include <Wire.h>

#include "Configuration.h"
#include "i2c.h"
#include "Board.h"



/* ---------FUNCTIONS------- */
void operator delete(void * p); 
void * operator new(size_t size); 

class Board;

extern Board board;

void i2c_update_pin_value( int pin, int value);
void i2c_update_pin_type( int pin, uint8_t type);
int i2c_read_pin_value( int pin );
uint8_t i2c_read_pin_type( int pin );

void init_pin_table();
void i2c_send_version( uint8_t sender, int dest );

#define WRITE_PIN(i, t) i2c_update_pin_value(  i, t)
#define READ_PIN(i) i2c_read_pin_value( i )
#define READ_PIN_MODE(i) i2c_read_pin_type( i )
#define PIN_MODE(i, v) i2c_update_pin_type( i, v)




#endif // __IC2_SLAVE__
