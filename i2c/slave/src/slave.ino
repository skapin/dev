
#include <inttypes.h>
#include <Wire.h>

#include "Configuration.h"
#include "i2c_slave.h"

#define bool boolean

uint8_t board_id=2;
int next_send_pin;
bool send_entries_flag = false;
bool send_pong_flag = false;


void setup_salve_master() {
    Wire.begin( board_id );                // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
    next_send_pin = 0;
}
void setup() {
    setup_salve_master();
    Serial.begin(19200);           
    init_pin_table();
}

void i2c_send_entries()
{
    int j;
    Wire.beginTransmission( MASTER_ID ); // Open th I2C link
    Wire.write( board_id );
    Wire.write( I2C_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each "for" loop
    {
        if ( READ_PIN_MODE(next_send_pin) == PIN_TYPE_INPUT ) 
        {
            Wire.write( next_send_pin );
            Wire.write( board.pin_values[next_send_pin].value & 0xFF00 );
            Wire.write( board.pin_values[next_send_pin].value & 0x00FF );
        }
        next_send_pin = (next_send_pin+1) % PINS_PER_BOARD;      
    }
    Wire.endTransmission();
    send_entries_flag = false;
}

void loop() {
    Serial.print("Loop.");
    // Master want us to send entries values.
    if ( send_entries_flag )
    {
        //i2c_send_entries();
    }
    if ( send_pong_flag )
    {
        send_pong_flag = false;
        i2c_send_action( board_id, MASTER_ID, I2C_PONG );
    }
    delay( DELAY_MAIN_LOOP );
    
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void i2cReceiveEvent(int howMany) {
    byte pin = 0;
    int value = 0;
    byte action = Wire.read();
    if ( action == I2C_GET ) 
    {
        Serial.print(" GET ");
        send_entries_flag = true;
    }
    else if ( action == I2C_SET ) 
    {
        while ( Wire.available() )
        {
            pin = Wire.read();
            value = Wire.read() << 8;
            value += Wire.read();
            WRITE_PIN( pin, value );
        }
    }
    else if ( action == I2C_ALL ) 
    {
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
    else if ( action == I2C_PING ) 
    {
        Serial.print(" PING ");
        send_pong_flag = true;
    }
}
