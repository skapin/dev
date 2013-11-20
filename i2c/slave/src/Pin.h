#ifndef __PIN_H__
#define __PIN_H__

#define PINS_PER_BOARD      56

#define PIN_TYPE_INPUT      INPUT
#define PIN_TYPE_OUTPUT     OUTPUT
#define PIN_TYPE_DISABLE    0
#define PIN_NOONE           NULL

typedef struct Pin {
    uint8_t type;
    int value;
} Pin;



#endif
