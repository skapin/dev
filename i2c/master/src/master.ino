// I2C multi-device communication
// by D.K.Merriman, based on Sketches by:

// Wire Slave Master (transmitter)
// by Nicholas Zambetti <http://www.zambetti.com>

// 20090329

#include <Wire.h>
#include "../lib/QueueList/QueueList.h"
#include "../../common/src/i2c.h"

#define LED_OUT 13
#define Receiver 4	// Who I want to talk to




int x;
int MyAddress;
char String1[20];
QueueList <Update> pin_update_queue;


void update_pin_value( int pin, int value) {
    set_pin_value( pin, value );
    pin_update_queue.push( Update{pin, I2C_SET} );

}

void update_pin_type( int pin, int type) {
    set_pin_type( pin, type );
    pin_update_queue.push( Update{pin, I2C_SETUP} );
}


void setup_salve_master() {
    Wire.begin( MASTER_ID );                // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
}

void setup()
{
    setup_salve_master();
    Serial.begin(19200); 
}

void i2c_send_get( int slave_number ) 
{
    Wire.beginTransmission( slave_number );
    Wire.write( I2C_GET );
    Wire.endTransmission();
    Serial.print("[MASTER] I2C send end <-");
    Serial.flush();
}

void i2c_send_update() 
{
    if ( !pin_update_queue.isEmpty() ) 
    {
        byte count = 2;
        Wire.beginTransmission( 4 ); // Open th I2C link
        //Wire.write( MASTER_ID );
        while ( !pin_update_queue.isEmpty() && count < BUFFER_LENGTH-2) 
        {
            Update up = pin_update_queue.pop();
            Wire.write( up.set_setup );
            Wire.write( up.pin );
            if ( up.set_setup == I2C_SET ) {
                Wire.write( pin_values[up.pin].value & 0xFF00 );
                Wire.write( pin_values[up.pin].value & 0x00FF );
                count = count+4;
            }
            else if ( up.set_setup == I2C_SETUP ) {
                Wire.write( pin_values[up.pin].type );
                count = count+3;
            }
        }
        Wire.endTransmission();
    }
    
}

void loop()

{
    Serial.print("loop <-");
    for( int i =0; i<PIN_MAX_NUMBER ; ++i ) // Loop to send all entries
    {
        Serial.print(pin_values[i].value);
        Serial.print(".");
        Serial.flush();
    }
    update_pin_value(3,9000);
    i2c_send_update();
    
    i2c_send_get( 4 );
  
    
    delay(10);
    
}


void i2cReceiveEvent(int howMany)
{
    //Serial.print(" [ISP ");    
    int sender = Wire.read();
    byte action = Wire.read();
    
    if ( action == I2C_GET ) 
    {
        
    }
    else if ( action == I2C_SET ) 
    {
      //  Serial.print("***");
        byte pin = 0;
        int value = 0;
        while ( Wire.available() )
        {
            pin = Wire.read();
            value = Wire.read() << 8;
            value += Wire.read();
            //check si c'ets bien une pin INPUT (eviter la latence induite entre slave/master sur la modif pinMode 
            // (si une pin ets devenu output entre tmeps apr exemple)
            // faire : type == INPUT
            set_pin_value( pin, value );
        }
    }
    else if ( action == I2C_SETUP ) 
    {
        int pin = 0;
        int value = 0;
        while ( Wire.available() )
        {
            pin = Wire.read();
            value = Wire.read();
            set_pin_type( pin, value );
        }
    }
}
