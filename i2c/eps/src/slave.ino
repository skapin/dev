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
#define DELAY_CHECK_PIN_SLOW	1000 // *timer ( DELAY_MAIN_LOOP )
#define DELAY_CHECK_PIN_FAST	10 // *timer ( DELAY_MAIN_LOOP )

#define SERIAL_BAUDRATE			19200

/***********************************************************************
 * 
 * Variables
 * 
 * ********************************************************************/
 
uint8_t timer_check_pin_slow = 0;
uint8_t timer_check_pin_fast = 0;
volatile byte timer_eps_update = 0 ;// 10ms*10 = 100ms; use base counter



/***********************************************************************
 * 
 * Fonctions
 * 
 * ********************************************************************/
void setup() 
{
    Serial.begin( SERIAL_BAUDRATE );   
    setup_slave_master( ); 
    
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
        delay( DELAY_INIT );
    }
    else
    {
     //   Serial.print(".");
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
        
        //Send queued pin value to master via i2c
        eps_send_board_update( 0 );
        //__asm__("nop\n\t"); 
        // wait a bit
        delayMicroseconds( DELAY_MAIN_LOOP );
    }
}



