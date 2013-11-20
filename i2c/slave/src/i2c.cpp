
#include "i2c.h"


void i2c_set_vpin_value( Pin* pin_values, uint8_t pin, int value)
{
    pin_values[pin].value = value;
}

void i2c_set_vpin_type( Pin* pin_values, uint8_t pin, int type)
{
    pin_values[pin].type = type;
}

void i2c_send_action( uint8_t sender, uint8_t dest, uint8_t action )
{
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( sender );
    Wire.I2C_WRITE( action );
    Wire.endTransmission();
}
