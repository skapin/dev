

#include <Wire.h>
#include "../../common/src/i2c.h"

#define LED_OUT 13
#define SLAVE_ID 4	// who am I?
#define bool boolean


int x;
int Source;
int next_send_pin;

char String1[20];
bool send_entries_flag = false;


void init_pin_table()
{
    for( int i =0; i<PIN_MAX_NUMBER ; ++i ) // Loop to send all entries
    {
        pinMode_ext( i, PIN_TYPE_INPUT );
        set_pin_value( i, i*2 );
    }
}

void setup_salve_master() {
    Wire.begin(SLAVE_ID);                // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
    next_send_pin = 0;
}
void setup() {
    setup_salve_master();
    Serial.begin(19200);           
    pinMode(LED_OUT, OUTPUT);
    Source = 0;
    init_pin_table();
}

void i2c_send_entries()
{
    int i,j;
    Serial.print("[loop] start send entries ->");
    Serial.flush();
    Wire.beginTransmission( MASTER_ID ); // Open th I2C link
    Wire.write( SLAVE_ID );
    Wire.write( I2C_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each for loop
    {
        if ( pin_values[i].type == PIN_TYPE_INPUT ) 
        {
            Wire.write( next_send_pin );
            Wire.write( pin_values[next_send_pin].value & 0xFF00 );
            Wire.write( pin_values[next_send_pin].value & 0x00FF );
        }
        next_send_pin = (next_send_pin+1) % PIN_MAX_NUMBER;      
    }
    Wire.endTransmission();
    send_entries_flag = false;
}

void loop() {
    for( int i =0; i<PIN_MAX_NUMBER ; ++i ) // Loop to send all entries
    {
        Serial.print(pin_values[i].value);
        Serial.print(".");
        Serial.flush();
    }
    // Master want us to send entries values.
    if ( send_entries_flag )
    {
        i2c_send_entries();
    }
    delay(10);
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
            set_pin_Ext( pin, value );
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
                set_pin_Ext( pin, value );            
            }
            else if ( action == I2C_SETUP)
            {
                value = Wire.read();
                pinMode_ext( pin, value );    
            }
        }
    }
}
