#ifndef __BOARD_H__
#define __BOARD_H__

#include <inttypes.h>
#include "Pin.h"


#define BOARD_CHECK_OK    0
#define BOARD_CHECK_WAIT_PONG   1
#define BOARD_CHECK_DELAY   10*5 //total time is : 100ms * board_check_timer = 5sec

class Board
{
    public:
    Board();
    Pin pin_values[PINS_PER_BOARD];
    uint8_t connected;
    uint8_t check_state;    
};


void check_boards_connected();

#endif
