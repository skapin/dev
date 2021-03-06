/**
 * Configuration file.
 * Variable you can adjust, like delay timer.
 * 
 * **/
 
#define IS_SLAVE            1          
//#define IS_MASTER            1

#ifdef IS_SLAVE
 #define board boards[0]
 #define WRITE_PIN(p,v) boards[0].write_bpin(p,v)
 #define PIN_MODE(p,t) boards[0].write_bpin_type(p,t)
#endif

#define BOARD_ID            2
#define NUM_BOARD           1
#define PINS_PER_BOARD      56
#define DELAY_MAIN_LOOP     100//ms

#define DELAY_START_UP      1000 //ms  time to wait at the end of setup(), to be sure the main board is ready !
#define DELAY_OFF           1000*60 // = 1min
#define DELAY_INIT          100
#define DELAY_CHECK_PIN     100 // *timer (so ms)


