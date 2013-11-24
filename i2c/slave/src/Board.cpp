#include "Board.h"

uint8_t last_check = 0;
uint8_t board_id=2;
uint8_t check_state = BOARD_W8_MASTER;

Board::Board()
{
    this->pin_values={};
    this->connected = false;
 //   this->check_state = BOARD_OK;
}

void check_boards_connected()
{
}


void check_pins_update()
{
    int value=0;
    for ( uint8_t i = 0; i < PINS_PER_BOARD ; ++i )
    {
        // we process only INPUT type pin. Output are controled by master.
        if ( board.pin_values[i].GET_IO_TYPE == PIN_TYPE_INPUT )
        {
            if ( IS_DIGITAL( board.pin_values[i].type ) )
            {
                value = digitalRead( i );
                if ( READ_PIN( i ) != value )
                {
                    board.pin_update_queue.push( Update{ i, I2C_SET} );
                    WRITE_PIN( i, value );
                }
            }
        }
    }
}


void check_board() 
{
    switch (check_state )
    {
        case BOARD_WAIT_PONG :
        {
            check_state = BOARD_OK;
            i2c_send_action( board_id, MASTER_ID, I2C_PONG );
        }
        break;
        case BOARD_WAIT_VERSION :
        {
            check_state = BOARD_W8_MASTER;
            i2c_send_version( board_id, MASTER_ID );
        }
        break;
        case BOARD_WAIT_INIT :
        {
            check_state = BOARD_OK;
            board.connected = true;
            i2c_send_action( board_id, MASTER_ID, I2C_INIT );
        }
        break;
        case BOARD_BAD_VERSION :
        {
            check_state = BOARD_OFF;
            board.connected = false;
            i2c_send_version( board_id, MASTER_ID );
        }
        break;
        
        default :
        break;
    }
}
