#include "eps.h"

Board boards[NUM_BOARD];
bool send_entries_flag = false;
int next_send_pin=0;

void(* reset_board) (void) = 0; 

void setup_slave_master(  ) {
    Wire.begin( BOARD_ID );          // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
}
uint8_t vpin2bpin(int vpin)
{
    return vpin%PINS_PER_BOARD;
}
 uint8_t vpin2board(int vpin)
{
    return vpin/PINS_PER_BOARD;
}

// READ
int eps_read_vpin_value( int pin )
{
    return boards[vpin2board(pin)].read_bpin( vpin2bpin(pin) );
}
 uint8_t eps_read_vpin_type( int pin )
{
    return boards[vpin2board(pin)].read_bpin_type( vpin2bpin(pin) );
}

int board_read_bpin_value( uint8_t b, uint8_t pin )
{
    return boards[b].read_bpin( pin );
}
 uint8_t board_read_bpin_type( uint8_t b, uint8_t pin )
{
    return boards[b].read_bpin_type( pin );
}

// WRITE

void eps_set_vpin_value( int pin, int value) {
    uint8_t real_pin = vpin2bpin(pin);
    uint8_t board_n = vpin2board(pin);
    boards[board_n].write_bpin( real_pin, value );
    if ( pin >= PINS_PER_BOARD ) // start virtual pin (i.e other Arduino board)
    {
        boards[board_n].pin_update_queue.push( Update{real_pin, EPS_SET} );
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
    }//pin_update_queue.push( Update{pin, eps_SET} );
}

void eps_write_vpin_type( int pin, uint8_t type) {
    uint8_t real_pin = vpin2bpin(pin);
    if ( pin >= PINS_PER_BOARD ) // start virtual pin (i.e other Arduino board)
    // So we need to update the fifo queue.
    {
        boards[vpin2board(pin)].pin_update_queue.push( Update{real_pin, EPS_SETUP} );
    }
    else
    {
        pinMode(real_pin, type & PIN_TYPE_IO_MASK );
    }
    boards[vpin2board(pin)].pin_values[real_pin].type = type ;
}

void eps_send_action( uint8_t dest, uint8_t action )
{
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( action );
    Wire.endTransmission();
}

void eps_send_version( int dest )
{
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( EPS_VERSION );
    Wire.I2C_WRITE( EPS_PROTOCOL_VERSION );
    Wire.endTransmission();
}

void eps_send_board_update(uint8_t dest) 
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
            if ( up.type == EPS_SET ) {
                value = boards[dest].read_bpin(up.pin);
                Wire.I2C_WRITE( (byte) ( ( value & 0xFF00)>>8 ) );
                Wire.I2C_WRITE( (byte) ( value & 0x00FF ) );
                count = count+2+2;
            }
            else if ( up.type == EPS_SETUP )
            {
                Wire.I2C_WRITE( boards[dest].read_bpin_type(up.pin) );
                count = count+2+1;
            }
        }
        Wire.endTransmission();
    }
}


byte eps_send_entries(uint8_t dest)
{
    uint8_t j;
    Wire.beginTransmission( dest+1 ); // Open th I2C link
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( EPS_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each "for" loop
    {
        if ( ( boards[0].read_bpin_type(next_send_pin)  & PIN_TYPE_IO_MASK ) == PIN_TYPE_INPUT ) 
        {
            Wire.I2C_WRITE( next_send_pin );
            Wire.I2C_WRITE( (byte) ((boards[0].pin_values[next_send_pin].value & 0xFF00) >> 8)  );
            Wire.I2C_WRITE( (byte) (boards[0].pin_values[next_send_pin].value & 0x00FF) );
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

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void i2cReceiveEvent(int howMany)
{
    #ifdef IS_MASTER
    //Serial.print(" [ISP ");
    int sender = Wire.I2C_READ() -1;
    byte action = Wire.I2C_READ();

    if ( action == EPS_SET )
    {
        byte pin = 0;
        int value = 0;
        while ( Wire.available() )
        {
            pin = Wire.I2C_READ();
            value = Wire.I2C_READ() << 8;
            value += Wire.I2C_READ();
            boards[sender].write_bpin( pin, value );
        }
    }
    else if (action == EPS_PONG && boards[sender].check_state==BOARD_WAIT_PONG )
    {
        boards[sender].check_state = BOARD_OK;
    }
    else if (action == EPS_RESET )
    {
        reset_board();
    }
    else if (action == EPS_INIT )
    {
        boards[sender].check_state = BOARD_OK;
        boards[sender].connected = true;
    }
    else if (action == EPS_VERSION )
    {
        while ( Wire.available() )
        {
            uint8_t value = Wire.I2C_READ();
            if ( value < EPS_MIN_VERSION_REQUIRE )
            {
                boards[sender].check_state = BOARD_BAD_VERSION;
            }
            else
            {
                boards[sender].check_state = BOARD_WAIT_INIT;
            }
        }
    }
    #else
    byte pin = 0;
    int value = 0;
    int sender = Wire.I2C_READ() -1;
    byte action = Wire.read();
    if ( action == EPS_GET && board.connected ) 
    {
        Serial.print(" GET ");
        send_entries_flag = true;
    }
    else if ( action == EPS_SET ) 
    {
        Serial.print(" SET ");
        if ( Wire.available() )
        {
            pin = Wire.I2C_READ();
            value = Wire.I2C_READ()<<8;
            value += Wire.I2C_READ();
            WRITE_PIN( pin, value );            
        }
    }
    else if ( action == EPS_SETUP ) 
    {
        Serial.print(" SETUP ");
        while ( Wire.available() )
        {
            pin = Wire.I2C_READ();
            value = Wire.I2C_READ();
            PIN_MODE( pin, value );  
            Serial.print(pin);
        }
    }
    else if ( action == EPS_ALL ) 
    {
        Serial.print(" all ");
        while ( Wire.available() )
        {
            action = Wire.I2C_READ();
            pin = Wire.read();
            if ( action == EPS_SET)
            {
                value = Wire.I2C_READ() << 8;
                value += Wire.I2C_READ();
                WRITE_PIN( pin, value );            
            }
            else if ( action == EPS_SETUP)
            {
                value = Wire.I2C_READ();
                PIN_MODE( pin, value );    
            }
        }
    }
    else if ( action == EPS_PING && board.connected ) 
    {
        Serial.print(" PING ");
        board.check_state = BOARD_WAIT_PONG;
    }
    else if ( action == EPS_RESET ) 
    {
        Serial.print(" RESET ");
        reset_board();
    }
    else if ( action == EPS_INIT ) 
    {
        Serial.print(" INIT ");
        board.check_state = BOARD_WAIT_INIT;
    }
    else if ( action == EPS_VERSION ) 
    {
        Serial.print(" VERSION");
        board.connected = false;
        if ( Wire.available() )
        {
            value = Wire.read();
            if ( value > EPS_PROTOCOL_VERSION )
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
    #endif
}


#ifdef IS_MASTER
void operator delete(void * p) // or delete(void *, std::size_t)
{
  free(p);
}
void * operator new(size_t size)
{
  return malloc(size);
}
#endif
