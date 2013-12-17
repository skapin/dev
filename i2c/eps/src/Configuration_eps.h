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

#ifndef BOARD_ID
        #error "You must define a board number. You can do it though make command."
#endif

#define NUM_BOARD           1
#define PINS_PER_BOARD      56
#define DELAY_MAIN_LOOP     10//µs

#define DELAY_START_UP      1000 //ms  time to wait at the end of setup(), to be sure the main board is ready !
#define DELAY_OFF           1000*30 // = 30sec
#define DELAY_INIT          100
#define DELAY_CHECK_PIN_SLOW     1000 // *timer ( DELAY_MAIN_LOOP )
#define DELAY_CHECK_PIN_FAST     10 // *timer ( DELAY_MAIN_LOOP )


