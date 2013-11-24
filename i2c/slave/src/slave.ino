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
#include "i2c_slave.h"

#define bool boolean


int next_send_pin;
bool send_entries_flag = false;
uint8_t timer_check_pin = 0;

void(* resetFunc) (void) = 0; 

void setup_slave_master(  ) {
    Wire.begin( board_id );                // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
    next_send_pin = 0;
}
void setup() 
{
    Serial.begin(19200);   
    setup_slave_master( ); // get an random id number. This ID in in a huge range, and doesnt allow reserved ID(for already attribut board)
    init_pin_table();
    
    delay( DELAY_START_UP );
}

void i2c_send_entries()
{
    uint8_t j;
    Wire.beginTransmission( MASTER_ID ); // Open th I2C link
    Wire.write( board_id );
    Wire.write( I2C_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each "for" loop
    {
        if ( (READ_PIN_MODE(next_send_pin)  & PIN_TYPE_IO_MASK ) == PIN_TYPE_INPUT ) 
        {
            Wire.write( next_send_pin );
            Wire.write( (byte) ((board.pin_values[next_send_pin].value & 0xFF00) >> 8)  );
            Wire.write( (byte) (board.pin_values[next_send_pin].value & 0x00FF) );
        }
        next_send_pin = (next_send_pin+1) % PINS_PER_BOARD;      
        if ( next_send_pin == 0 )
        {
            send_entries_flag = false;
        }
    }
    Wire.endTransmission();
    
}

void loop() {
    check_board();
    //Serial.print(".");
    if ( check_state == BOARD_OFF )
    {
        Serial.print(" OFF ");
        delay( DELAY_OFF );
    }
    else if (check_state == BOARD_W8_MASTER )
    {
        delay( DELAY_INIT );
    }
    else
    {
     //   Serial.print(".");
        // Master want us to send entries values.
        if ( send_entries_flag )
        {
            i2c_send_entries();
        }
        ++timer_check_pin;
        if ( timer_check_pin >= DELAY_CHECK_PIN )
        {
            check_pins_update();
            timer_check_pin = 0;
        }
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
        check_state = BOARD_WAIT_PONG;
    }
    else if ( action == I2C_RESET ) 
    {
        Serial.print(" RESET ");
        resetFunc();
    }
    else if ( action == I2C_INIT ) 
    {
        Serial.print(" INIT ");
        check_state = BOARD_WAIT_INIT;
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
                check_state = BOARD_BAD_VERSION;
            }
            else
            {
                Serial.print(" GOOD VERSION ");
                check_state = BOARD_WAIT_VERSION;
            }
        }
    }
    else
    {
        Serial.print(" else? ");
        Serial.print( action );        
    }
}



