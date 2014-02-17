/***********************************************************
 * @file slave.ino
 * @author Florian Boudinet <florian.boudinet@gmail.com>
 * 
 * Sketch built for ino tool. 
 * This sketch setup an I2C slave board to speak with a master.
 * It's a part of Polybox-Firmware.
 * 
 * *********************************************************/
#include <inttypes.h>
#include <Wire.h>

#include "Configuration_eps.h"
#include "eps.h" // Extension Pin System

/***********************************************************************
 * 
 * Configuration && Define
 * 
 * ********************************************************************/
 
#define bool boolean

#define DELAY_MAIN_LOOP    		10//µs, used at the end of each Loop(). Base Timer

#define DELAY_START_UP			1000 	//ms  time to wait at the end of setup(), to be sure the main board is ready !
#define DELAY_OFF				1000*30 // = 30sec
#define DELAY_INIT				100
/** delay for slow pin(default). Check if values changed and push then 
 * to the updateQueue **/
#define DELAY_CHECK_PIN_SLOW	1000 // *timer ( DELAY_MAIN_LOOP )
/** delay for fast pin. Check if values changed and push then 
 * to the updateQueue **/
#define DELAY_CHECK_PIN_FAST	10 // *timer ( DELAY_MAIN_LOOP )
/** psuh delay for analog input (use ADC) **/
#define DELAY_PROCESS_ANALOG	10000 // *timer ( DELAY_MAIN_LOOP )

/** we sum X time each analog value, to smooth result (average result) **/
#define SERIAL_BAUDRATE			115200

#define ANALOG_SUM				5

/***********************************************************************
 * 
 * Variables
 * 
 * ********************************************************************/
uint8_t need_check = true ;
uint8_t timer_check_pin_slow = 0;
uint8_t timer_check_pin_fast = 0;
uint8_t timer_process_analog = 0;
uint8_t current_analog = 0; // current analog pin
uint8_t current_analog_sum_number = 0; // we sum X time each analog value, to smooth result (average result)
volatile byte timer_eps_update = 0 ;// 10ms*10 = 100ms; use base counter


/***********************************************************************
 * 
 * Fonctions
 * 
 * ********************************************************************/
void setup_analog_timer()
{
    TCCR0A = 0;  // Setup analog interrupt
    OCR0B = 64;
    TIMSK0 |= (1<<OCIE0B);
}

void setup() 
{
    Serial.begin( SERIAL_BAUDRATE );   
    setup_slave_master( ); 
    pinMode(3,OUTPUT);
  //  setup_analog_timer();
    
    // Read and init ADCC for 1st time (dummy value)
    delay( DELAY_START_UP );
}


void loop() {
    board.process_state( MASTER_ID );
    if ( board.check_state == BOARD_OFF )
    {
        Serial.print(" OFF ");
        delay( DELAY_OFF );
    }
    else if ( board.check_state == BOARD_W8_MASTER )
    {
		Serial.print(" W8 ");
		digitalWrite(3, HIGH);
        delay( DELAY_INIT );
    }
    else
    {
        // Master want us to send entries values.
        if ( send_entries_flag )
        {
            send_entries_flag = eps_send_entries(0);
        }
        ++timer_check_pin_fast;
        
        //Check FAST pin ?
        if ( timer_check_pin_slow >= DELAY_CHECK_PIN_FAST )
        {
            board.check_pins_update( PIN_TYPE_FAST_CHECK);
            timer_check_pin_fast = 0;
        }
        //Check SLOW (classic) pin ?
        ++timer_check_pin_slow;
        if ( timer_check_pin_slow >= DELAY_CHECK_PIN_SLOW )
        {
            board.check_pins_update( );
            timer_check_pin_slow = 0;
        }
        // push analog update.
        if ( timer_process_analog >= DELAY_PROCESS_ANALOG )
        {
			board.process_analog();
			timer_process_analog  = 0;
		}
        
        //Send queued pin value to master via i2c
        eps_send_board_update( 0 );
        //__asm__("nop\n\t"); 
        // wait a bit
        delayMicroseconds( DELAY_MAIN_LOOP );
    }
}


/**
This timer is called 3906 timer per second. It is used to read Analog Input (using ADC)
*/
ISR( TIMER0_COMPB_vect )
{
/*	if ( need_check )
	{
		current_analog++;//next value to conv.
		if ( current_analog > 15 ) // number of ANALOG pin
		{
			current_analog = 0;
		}
		if ( IS_ANALOG(board.pin_values[current_analog]->type) )
		{
			#if defined(ADCSRB) && defined(MUX5)
			if(current_analog & 8)  // Reading channel 0-7 or 8-15?
				ADCSRB |= _BV(MUX5);
			else
				ADCSRB &= ~_BV(MUX5);
			#endif
			ADMUX = (ADMUX & ~(0x1F)) | (current_analog & 7);
			ADCSRA |= _BV(ADSC);  // start next conversion
			need_check = false;
		}	
	}
	else
	{
		if( (ADCSRA & _BV(ADSC)) == 0 )  // Conversion finished ?
		{
			board.pin_values[current_analog]->value = ADCW;
			need_check = true;
		}
	}*/
}
