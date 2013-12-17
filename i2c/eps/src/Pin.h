#ifndef __PIN_H__
#define __PIN_H__

/**
 * ATMEGA 2560
 * 
 * Size of struct :
 * uint8_t : 8bits
 * int : 16bits
 * => 24bits => 3 Bytes
 * 
 * **/
 

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
 
#define PINS_PER_BOARD      56

#define PIN_TYPE_INPUT      INPUT //1
#define PIN_TYPE_OUTPUT     OUTPUT //0
#define PIN_TYPE_DIGITAL    0
#define PIN_TYPE_ANALOG     2
#define PIN_TYPE_DISABLE    4

#define PIN_TYPE_FAST_CHECK 8
#define PIN_TYPE_SLOW_CHECK 0

#define PIN_TYPE_COUNTER	16 
#define PIN_TYPE_STANDARD	0

#define PIN_NOONE           NULL

#define PIN_TYPE_IO_MASK   ( PIN_TYPE_INPUT | PIN_TYPE_OUTPUT )
#define IS_DIGITAL(n) ( n & PIN_TYPE_IO_MASK ) == PIN_TYPE_DIGITAL
#define IS_ANALOG(n) ( n & PIN_TYPE_IO_MASK ) ==  PIN_TYPE_ANALOG
#define IS_STANDARD(n) ( n & PIN_TYPE_COUNTER ) ==  PIN_TYPE_STANDARD
#define IS_COUNTER(n) ( n & PIN_TYPE_COUNTER ) ==  PIN_TYPE_COUNTER
#define GET_IO_TYPE type & PIN_TYPE_IO_MASK


class Pin
{
	public :
	
	uint8_t type;
    int value;
    
	Pin(){
		type = PIN_TYPE_OUTPUT;
		value = 0;
		};
//	virtual ~Pin(){};
		
	
};
#endif
