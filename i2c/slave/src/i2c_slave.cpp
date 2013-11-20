#include "i2c_slave.h"

union fb_tag {
      byte b[2];
      int f;
} fb;


volatile byte timer_i2c_update = 0 ;// 10ms*10 = 100ms; use base counter

Board board;

 int i2c_read_pin_value( int pin )
{
    return board.pin_values[pin].value;
}
 uint8_t i2c_read_pin_type( int pin )
{
    return board.pin_values[pin].type;
}

void i2c_update_pin_value( int pin, int value) {
    i2c_set_vpin_value( board.pin_values, pin, value );
    //pin_update_queue.push( Update{pin, I2C_SET} );

}
void i2c_update_pin_type(  int pin, uint8_t type) {
    pinMode(pin, type );
    i2c_set_vpin_type( board.pin_values, pin, type );
}

void init_pin_table()
{
    for( int i =0; i<PINS_PER_BOARD ; ++i ) // Loop to send all entries
    {
        PIN_MODE( i, PIN_TYPE_INPUT );
        WRITE_PIN( i, i*2 );
    }
}

