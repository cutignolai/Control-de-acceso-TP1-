/***************************************************************************//**
  @file     board.h
  @brief    Board management
  @author   Micho
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs

#define PIN_LED_RED     PORTNUM2PIN(PB,22)	  // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)	  // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21)	  // PTB21
#define LED_ACTIVE      LOW

// PINES DIGITALES
//PLACA
#define DIO_1           PORTNUM2PIN(PC, 3)	    // PTC3
#define DIO_2           PORTNUM2PIN(PC, 2)	    // PTC2
#define DIO_3           PORTNUM2PIN(PA, 2)	    // PTA2
#define DIO_4           PORTNUM2PIN(PB, 23)	    // PTB23
#define DIO_5           PORTNUM2PIN(PB, 18)	    // PTB18
#define DIO_6           PORTNUM2PIN(PB, 9)	    // PTB9
#define DIO_7           PORTNUM2PIN(PC, 17)	    // PTC17
#define DIO_8           PORTNUM2PIN(PC, 16)	    // PTC16
#define DIO_9           PORTNUM2PIN(PC, 7)	    // PTC7
#define DIO_10          PORTNUM2PIN(PC, 5)	    // PTC5
#define DIO_11          PORTNUM2PIN(PC, 0)	    // PTC0
#define DIO_12          PORTNUM2PIN(PC, 9)	    // PTC9
#define DIO_13          PORTNUM2PIN(PC, 8)	    // PTC8
#define DIO_14          PORTNUM2PIN(PC, 1)	    // PTC1
#define DIO_15          PORTNUM2PIN(PB, 19)	    // PTB19
//CARD LECTOR
#define DIO_16          PORTNUM2PIN(PB,11)	    // PTB11
#define DIO_17          PORTNUM2PIN(PC,11)	    // PTC11
#define DIO_18          PORTNUM2PIN(PC,10)	    // PTC10
//TEST PIN
#define DIO_19          PORTNUM2PIN(PB,10)	    // PTC10


/*******************************************************************************
 ******************************************************************************/

#endif // _BOARD_H_
