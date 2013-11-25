#include "i2c.h"

Board boards[NUM_BOARD];
int next_send_pin=0;

uint8_t vpin2bpin(int vpin)
{
    return vpin%PINS_PER_BOARD;
}
 uint8_t vpin2board(int vpin)
{
    return vpin/PINS_PER_BOARD;
}

int i2c_read_vpin_value( int pin )
{
    return boards[vpin2board(pin)].read_bpin( vpin2bpin(pin) );
}
 uint8_t i2c_read_vpin_type( int pin )
{
    return boards[vpin2board(pin)].read_bpin_type( vpin2bpin(pin) );
}

// READ
int board_read_bpin_value( uint8_t b, uint8_t pin )
{
    return boards[b].read_bpin( pin );
}
 uint8_t board_read_bpin_type( uint8_t b, uint8_t pin )
{
    return boards[b].read_bpin_type( pin );
}

// WRITE

void i2c_set_pin_value( int pin, int value) {
    uint8_t real_pin = vpin2bpin(pin);
    uint8_t board_n = vpin2board(pin);
    boards[board_n].write_bpin( real_pin, value );
    if ( pin >= PINS_PER_BOARD ) // start virtual pin (i.e other Arduino board)
    {
        boards[board_n].pin_update_queue.push( Update{real_pin, I2C_SET} );
    }
    else
    {
        if ( IS_DIGITAL(boards[board_n].pin_values[real_pin].type) ) // digital
        {
            digitalWrite( real_pin, value );
        }
        else
        {
            analogWrite( real_pin, value );
        }
    }//pin_update_queue.push( Update{pin, I2C_SET} );
}

void i2c_send_action( uint8_t dest, uint8_t action )
{
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( action );
    Wire.endTransmission();
}

void i2c_send_version( int dest )
{
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( I2C_VERSION );
    Wire.I2C_WRITE( VERSION );
    Wire.endTransmission();
}

void i2c_send_board_update(uint8_t dest) 
{
    if ( ! boards[dest].pin_update_queue.isEmpty() ) 
    {
        Update up;
        byte count=0;
        int value=0;
        count = 2;
        Wire.beginTransmission( dest+1 ); // Open th I2C link (i.e master)
        while ( !boards[dest].pin_update_queue.isEmpty() && count < BUFFER_LENGTH-2) 
        {
            up = boards[dest].pin_update_queue.pop();
            Wire.I2C_WRITE( up.type );
            Wire.I2C_WRITE( up.pin );
            if ( up.type == I2C_SET ) {
                value = boards[dest].read_bpin(up.pin);
                Wire.I2C_WRITE( (byte) ( ( value & 0xFF00)>>8 ) );
                Wire.I2C_WRITE( (byte) ( value & 0x00FF ) );
                count = count+2+2;
            }
            else if ( up.type == I2C_SETUP )
            {
                Wire.I2C_WRITE( boards[dest].read_bpin_type(up.pin) );
                count = count+2+1;
            }
        }
        Wire.endTransmission();
    }
}


byte i2c_send_entries(uint8_t dest)
{
    uint8_t j;
    Wire.beginTransmission( dest+1 ); // Open th I2C link
    Wire.write( BOARD_ID );
    Wire.write( I2C_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each "for" loop
    {
        if ( ( boards[0].read_bpin_type(next_send_pin)  & PIN_TYPE_IO_MASK ) == PIN_TYPE_INPUT ) 
        {
            Wire.write( next_send_pin );
            Wire.write( (byte) ((boards[0].pin_values[next_send_pin].value & 0xFF00) >> 8)  );
            Wire.write( (byte) (boards[0].pin_values[next_send_pin].value & 0x00FF) );
        }
        next_send_pin = (next_send_pin+1) % PINS_PER_BOARD;      
        if ( next_send_pin == 0 )
        {
            Wire.endTransmission();
            return false;
        }
    }
    Wire.endTransmission();
    return true;
}


