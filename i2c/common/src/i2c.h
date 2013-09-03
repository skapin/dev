

#define PIN_MAX_NUMBER      64

#define PIN_TYPE_INPUT      INPUT
#define PIN_TYPE_OUTPUT     OUTPUT
#define PIN_TYPE_DISABLE    0
#define PIN_NOONE           NULL

#define I2C_SETUP           1
#define I2C_SET             2
#define I2C_GET             3
#define I2C_ALL             4

#define MASTER_ID           1

typedef struct Pin {
    byte type;
    int value;
} Pin;

typedef struct Update {
    byte pin;
    byte set_setup;
} Update;
    
Pin pin_values[PIN_MAX_NUMBER]= {0};


void set_pin_value( byte pin, int value)
{
    pin_values[pin].value = value;
}

void set_pin_type( byte pin, int type)
{
    pin_values[pin].type = type;
}

void pinMode_ext( byte pin, int type)
{
    pinMode( pin, type );
    set_pin_type( pin, type );
}

void set_pin_Ext( byte pin, int value)
{
    set_pin_value( pin, value );
}
