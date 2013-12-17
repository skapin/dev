
#ifndef __COUNTER_PIN_H__
#define __COUNTER_PIN_H__


 

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
 
#include "Pin.h"

class CounterPin : public Pin
{
	public :
    byte lastState;
    byte updated;
    
	CounterPin(): Pin(){
		lastState = LOW;
		updated = false;
		};
	//virtual ~CounterPin(){};
};
#endif
