#include "Board.h"


Board::Board()
{
    this->pin_values = {};
    this->connected = false;
    this->check_state = BOARD_W8_MASTER;
}
Board::~Board()
{
    
}

void Board::check_connected( uint8_t dest )
{  
    switch ( check_state )
    {   
        case BOARD_WAIT_VERSION :
            eps_send_version( dest );
        break;         
        case BOARD_WAIT_INIT :
            eps_send_action( dest, EPS_INIT );
        break;
        case BOARD_WAIT_PONG :
            connected = false ;
            check_state = BOARD_WAIT_VERSION;
        break;            
        case BOARD_OK :
            check_state = BOARD_WAIT_PONG;
            eps_send_action( dest, EPS_PING );
        break;
        case BOARD_W8_MASTER:
            #ifdef IS_MASTER
                check_state = BOARD_WAIT_VERSION;
            #endif
        default:
        break;
    }
}


void Board::check_pins_update()
{
    int value=0;
    for ( uint8_t i = 0; i < PINS_PER_BOARD ; ++i )
    {
        // we process only INPUT type pin. Output are controled by master.
        if ( pin_values[i].GET_IO_TYPE == PIN_TYPE_INPUT )
        {
            if ( IS_DIGITAL( pin_values[i].type ) )
            {
                value = digitalRead( i );
                if ( read_bpin( i ) != value )
                {
                    pin_update_queue.push( Update{ i, EPS_SET } );
                    write_bpin( i, value );
                }
            }
        }
    }
}
void Board::init_pin_table()
{
    for( int i =0; i<PINS_PER_BOARD ; ++i ) // Loop to send all entries
    {
        write_bpin_type( i, PIN_TYPE_INPUT );
        write_bpin( i, 0 );
    }
}
// READ
int Board::read_bpin( uint8_t pin )
{
    return pin_values[pin].value;
}
uint8_t Board::read_bpin_type( uint8_t pin )
{
    return pin_values[pin].type;
}
// WRITE
int Board::write_bpin(  uint8_t pin, int value )
{
    pin_values[pin].value = value;
}
uint8_t Board::write_bpin_type( uint8_t pin, uint8_t type )
{
    pin_values[pin].type;
}

void Board::process_state( uint8_t dest ) 
{
    switch ( check_state )
    {
        case BOARD_WAIT_PONG :
        {
            check_state = BOARD_OK;
            eps_send_action( dest, EPS_PONG );
        }
        break;
        case BOARD_WAIT_VERSION :
        {
            check_state = BOARD_W8_MASTER;
            eps_send_version( dest );
        }
        break;
        case BOARD_WAIT_INIT :
        {
            check_state = BOARD_OK;
            connected = true;
            eps_send_action( dest, EPS_INIT );
        }
        break;
        case BOARD_BAD_VERSION :
        {
            check_state = BOARD_OFF;
            connected = false;
            eps_send_version( dest );
        }
        break;
        
        default :
        break;
    }
}
