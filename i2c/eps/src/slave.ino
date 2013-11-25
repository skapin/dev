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

#include "Configuration.h"
#include "i2c.h"

#define WRITE_PIN(p,v) boards[0].write_bpin(p,v)
#define PIN_MODE(p,t) boards[0].write_bpin_type(p,t)

#define bool boolean
#define board boards[0]

bool send_entries_flag = false;
uint8_t timer_check_pin = 0;
volatile byte timer_i2c_update = 0 ;// 10ms*10 = 100ms; use base counter

void(* resetFunc) (void) = 0; 

void setup_slave_master(  ) {
    Wire.begin( BOARD_ID );                // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
}
void setup() 
{
    Serial.begin(19200);   
    setup_slave_master( ); 
    
    delay( DELAY_START_UP );
}


void loop() {
    board.process_state( MASTER_ID );
    //Serial.print(".");
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
            send_entries_flag = i2c_send_entries(0);
        }
        ++timer_check_pin;
        if ( timer_check_pin >= DELAY_CHECK_PIN )
        {
            board.check_pins_update();
            timer_check_pin = 0;
        }
        i2c_send_board_update( 0 );
        __asm__("nop\n\t"); 
        //delay( DELAY_MAIN_LOOP );
    }
}
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void i2cReceiveEvent(int howMany) {
    byte pin = 0;
    int value = 0;
    byte action = Wire.read();
    if ( action == I2C_GET && board.connected ) 
    {
        Serial.print(" GET ");
        send_entries_flag = true;
    }
    else if ( action == I2C_SET ) 
    {
        Serial.print(" SET ");
        if ( Wire.available() )
        {
            pin = Wire.read();
            value = Wire.read()<<8;
            value += Wire.read();
            WRITE_PIN( pin, value );            
        }
    }
    else if ( action == I2C_SETUP ) 
    {
        Serial.print(" SETUP ");
        while ( Wire.available() )
        {
            pin = Wire.read();
            value = Wire.read();
            PIN_MODE( pin, value );  
            Serial.print(pin);
        }
    }
    else if ( action == I2C_ALL ) 
    {
        Serial.print(" all ");
        while ( Wire.available() )
        {
            action = Wire.read();
            pin = Wire.read();
            if ( action == I2C_SET)
            {
                value = Wire.read() << 8;
                value += Wire.read();
                WRITE_PIN( pin, value );            
            }
            else if ( action == I2C_SETUP)
            {
                value = Wire.read();
                PIN_MODE( pin, value );    
            }
        }
    }
    else if ( action == I2C_PING && board.connected ) 
    {
        Serial.print(" PING ");
        board.check_state = BOARD_WAIT_PONG;
    }
    else if ( action == I2C_RESET ) 
    {
        Serial.print(" RESET ");
        resetFunc();
    }
    else if ( action == I2C_INIT ) 
    {
        Serial.print(" INIT ");
        board.check_state = BOARD_WAIT_INIT;
    }
    else if ( action == I2C_VERSION ) 
    {
        Serial.print(" VERSION");
        board.connected = false;
        if ( Wire.available() )
        {
            value = Wire.read();
            if ( value > VERSION )
            {
                Serial.print(" BAD VERSION ");
                board.check_state = BOARD_BAD_VERSION;
            }
            else
            {
                Serial.print(" GOOD VERSION ");
                board.check_state = BOARD_WAIT_VERSION;
            }
        }
    }
    else
    {
        Serial.print(" else? ");
        Serial.print( action );        
    }
}



