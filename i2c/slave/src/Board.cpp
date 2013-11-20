#include "Board.h"

uint8_t last_check = 0;

Board::Board()
{
    this->pin_values={};
    this->connected = false;
    this->check_state = BOARD_CHECK_OK;
}

void check_boards_connected()
{
}
