/**
 * @file slave.ino
 * @author Florian Boudinet <florian.boudinet@gmail.com>
 * 
 * Sketch built for ino tool. 
 * This sketch setup an I2C slave board to speak with a master.
 * It's a part of Polybox-Firmware.
 * 
 * **/
#include <inttypes.h>
#include <Wire.h>

#include "Configuration_eps.h"
#include "eps.h"

#define bool boolean

uint8_t timer_check_pin = 0;
volatile byte timer_eps_update = 0 ;// 10ms*10 = 100ms; use base counter

void setup() 
{
    Serial.begin(19200);   
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
        ++timer_check_pin;
        if ( timer_check_pin >= DELAY_CHECK_PIN )
        {
            board.check_pins_update();
            timer_check_pin = 0;
        }
        eps_send_board_update( 0 );
        __asm__("nop\n\t"); 
        //delay( DELAY_MAIN_LOOP );
    }
}



