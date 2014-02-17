/**
 * Configuration file.
 * Variable you can adjust, like delay timer.
 * 
 * **/
#ifndef __CONFIGURATION_EPS_H__
#define __CONFIGURATION_EPS_H__

#ifndef BOARD_ID
  #error "You must define a board number. You can do it though make command."
#endif
#if BOARD_ID == -1
  #error "You must define a board number. You can do it though make command."
#endif

#define IS_SLAVE			1          
//#define IS_MASTER			1

#ifdef IS_SLAVE
 #define board boards[0]
// #define WRITE_PIN(p,v) boards[0].write_bpin(p,v)   <--- / NOOB !
// #define PIN_MODE(p,t) boards[0].write_bpin_type(p,t)
#endif


#define NUM_BOARD			1
#define PINS_PER_BOARD		(54+14)
#define PIN_ANALOG			54

#endif
