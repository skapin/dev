/**
 * Configuration file.
 * Variable you can adjust, like delay timer.
 * 
 * **/
 
#define VERSION             1
#ifndef BOARD_ID
	#error "You must define a board number. You can do it though make command."
#endif
#define NUM_BOARD           1
#define PINS_PER_BOARD      56
#define DELAY_MAIN_LOOP     100//ms

#define DELAY_START_UP      1000 //ms  time to wait at the end of setup(), to be sure the main board is ready !
#define DELAY_OFF           1000*60 // = 1min
#define DELAY_INIT          100
#define DELAY_CHECK_PIN     100 // *timer (so ms)


