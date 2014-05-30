/*
    This file is part of Polybox.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "eps.h"

Board boards[NUM_BOARD];
bool send_entries_flag = false;
int next_send_pin=0;
volatile uint16_t restart_counter =0 ;
volatile bool has_token = false;

///< ACK system
bool send_ack = false;
uint8_t i2c_destination = 0;
uint8_t i2c_current_buffer_size = 0;
uint8_t i2c_buffer[BUFFER_LENGTH];
uint32_t i2c_ack_resend_count = 0;
uint32_t i2c_timer = 0;
uint8_t i2c_ack_reply_destination = 1;

void(* reset_board) (void) = 0; 

void setup_slave_master(  ) {
    Wire.begin( BOARD_ID );          // join i2c bus
    Wire.onReceive(i2cReceiveEvent); // register event  
}

void eps_manage()
{
//	eps_check_ack();
	
	uint8_t pong_flag = i2c_timer % PING_PONG_DELAY;
	uint8_t update_flag = i2c_timer % SEND_UPDATE_DELAY;
	uint8_t get_flag = i2c_timer % GET_DELAY;
	
	i2c_timer++;
	for ( uint8_t i =0 ; i<NUM_BOARD ; ++i )
	{
		// If board connected, do.
		//hardware check to do.
		if ( boards[i].connected )
		{
	/*		if ( pong_flag )
			{*/
				boards[i].manage_ping_pong();
			//}
			/** If the board has the token, send Board updates. **/
			if ( has_token )
			{
				eps_send_board_update(i);
				has_token = false;
			}
			/*if ( get_flag )
			{ 
				eps_send_action( i+1, EPS_GET );
			}*/
			
			 /** Does we need to send all pin value to the I2C bus ? **/
			if ( send_entries_flag )
			{
				eps_send_entries(i);
				send_entries_flag = false;
			}
		}
		else
		{
			// Process HandShake states
			if ( i2c_timer % HANDSHAKE_TIME )
			{
				boards[i].manage_status( );
			}
		}
	}	
}
void eps_check_ack()
{
	if ( send_ack == true )
	{
		eps_send_action( i2c_ack_reply_destination, EPS_ACK ) ;
		send_ack = false;
	}
}

uint8_t vpin2bpin(int vpin)
{
    return vpin%PINS_PER_BOARD;
}
 uint8_t vpin2board(int vpin)
{
    return vpin/PINS_PER_BOARD;
}
void printAllPin()
{
	for ( uint8_t i =0 ; i<NUM_BOARD ; ++i )
	{
		Serial.print("Board");
		Serial.println(i);
		for ( uint8_t h = 0 ; h<PINS_PER_BOARD ; ++h )
		{
			Serial.print(h);
			Serial.print(":");
			Serial.print( boards[i].pin_values[h]->value);
			Serial.print("  ");
		}
	}
}
void eps_ack_add_data( uint8_t data)
{
    i2c_buffer[i2c_current_buffer_size] = data;	
    i2c_current_buffer_size++;
}
void eps_ack_reset()
{
	i2c_current_buffer_size = 0;
}
uint8_t eps_ack_is_waiting()
{
	return i2c_current_buffer_size;
}
void eps_ack_resend()
{
	Wire.beginTransmission( i2c_destination );
	for( uint8_t i = 0; i < i2c_current_buffer_size ; ++i )
	{ 
		Wire.I2C_WRITE( i2c_buffer[i] );
	}
	Wire.endTransmission();
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
        if ( IS_DIGITAL(boards[board_n].pin_values[real_pin]->type) ) // digital
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
    boards[vpin2board(pin)].pin_values[real_pin]->type = type ;
}

void eps_send_action( uint8_t dest, uint8_t action )
{
	///< ACK
	i2c_destination = dest;
    
    Wire.beginTransmission( dest );
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( action );
    Wire.endTransmission();
}

void eps_send_version( int dest )
{
	i2c_destination = dest;
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
        ///< ACK
		i2c_destination = dest+1;	
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
    ///< ACK
	i2c_destination = dest+1;
    Wire.I2C_WRITE( BOARD_ID );
    Wire.I2C_WRITE( EPS_SET );
    for( j =0; j< ((BUFFER_LENGTH-2)/3) ; ++j ) // Loop to send all entries. TWI buffer is only 32 Byte. We send 3 byte during each "for" loop
    {
        if ( ( boards[0].read_bpin_type(next_send_pin)  & PIN_TYPE_IO_MASK ) == PIN_TYPE_INPUT ) 
        {
            Wire.I2C_WRITE( next_send_pin );
            Wire.I2C_WRITE( (byte) ((boards[0].pin_values[next_send_pin]->value & 0xFF00) >> 8)  );
            Wire.I2C_WRITE( (byte) (boards[0].pin_values[next_send_pin]->value & 0x00FF) );
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
     //   reset_board();
    }
    else if (action == EPS_INIT )
    {
        boards[sender].check_state = BOARD_OK;
        boards[sender].connected = true;
        eps_ack_reset(); ///< Reset the ACK flag.
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
        eps_ack_reset();
    }
    
    #else
    byte pin = 0;
    int value = 0;
    int sender = Wire.I2C_READ() -1;
    byte action = Wire.read();
    
    ///< ACK system
	i2c_ack_reply_destination = sender + 1 ;
	send_ack = true;
	
    if ( action == EPS_GET && board.connected ) 
    {
        send_entries_flag = true;
        Serial.print(" ***GET*** ");
    }
    else if ( action == EPS_SET ) 
    {
        if ( Wire.available() )
        {
            pin = Wire.I2C_READ();
            value = Wire.I2C_READ()<<8;
            value += Wire.I2C_READ();
            WRITE_VPIN( pin, value );            
        }
    }
    else if ( action == EPS_SETUP ) 
    {
        while ( Wire.available() )
        {
            pin = Wire.I2C_READ();
            value = Wire.I2C_READ();
            VPIN_MODE( pin, value );  
            Serial.print(pin);
        }
    }
    else if ( action == EPS_ALL ) 
    {
		Serial.print(" ***ALL/UP*** ");
        while ( Wire.available() )
        {
            action = Wire.I2C_READ();
            pin = Wire.I2C_READ();
            if ( action == EPS_SET)
            {
                value = Wire.I2C_READ() << 8;
                value += Wire.I2C_READ();
                WRITE_VPIN( pin, value );
                
            }
            else if ( action == EPS_SETUP)
            {
                value = Wire.I2C_READ();
                VPIN_MODE( pin, value );    
            }
        }
        
    }
    else if ( action == EPS_PING && board.connected ) 
    {
        board.check_state = BOARD_WAIT_PONG;
    }
    else if ( action == EPS_RESET ) 
    {
		Serial.print(" RESET BOARD ");
        //reset_board();
    }
    else if ( action == EPS_INIT ) 
    {
		Serial.print(" INIT ");
        board.check_state = BOARD_WAIT_INIT;
    }
    else if ( action == EPS_TOKEN ) 
    {
		Serial.print(" TOKEN ");
        has_token = true;
    }
    else if ( action == EPS_VERSION ) 
    {
		Serial.print(" VERS ");
		restart_counter++;
        board.connected = false;
        if ( Wire.available() )
        {
            value = Wire.read();
            if ( value > EPS_PROTOCOL_VERSION )
            {
                board.check_state = BOARD_BAD_VERSION;
            }
            else
            {
                board.check_state = BOARD_WAIT_VERSION;
            }
        }
    }
    else if ( action == EPS_ACK )
    {
		send_ack = false; ///< no need ack for an ACK...
	//	eps_ack_reset(); ///< Reset the ACK flag.
		Serial.print(" ACK ");
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
