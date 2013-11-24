#ifndef __BOARD_H__
#define __BOARD_H__

#include <inttypes.h>
#include "Pin.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "QueueList.h"
#include "Update.h"

#include "i2c_slave.h"

#define BOARD_WAIT_VERSION    0
#define BOARD_WAIT_INIT    1
#define BOARD_WAIT_PONG   2
#define BOARD_OK    3
#define BOARD_BAD_VERSION   4
#define BOARD_OFF    5
#define BOARD_INIT    6
#define BOARD_W8_MASTER    7

#define BOARD_CHECK_DELAY   10*5 //total time is : 100ms * board_check_timer = 5sec

extern uint8_t board_id;
extern uint8_t check_state;


class Board
{
    public:
    Board();
    Pin pin_values[PINS_PER_BOARD];
    uint8_t connected;
    uint8_t check_state;
    QueueList<Update> pin_update_queue;      
};


void check_boards_connected();
void check_pins_update();
void check_board();

#endif
