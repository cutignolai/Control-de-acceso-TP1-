/***************************************************************************//**
   @file    leds.h
  @brief    leds driver
  @author   Micho
 ******************************************************************************/

#ifndef _LEDS_H_
#define _LEDS_H_

/*******************************************************************************
 *                          INCLUDE HEADER FILES                                *
 ******************************************************************************/

#include "PDRV_GPIO.h"

/*******************************************************************************
 *              CONSTANT AND MACRO DEFINITIONS USING #DEFINE                    *
 ******************************************************************************/

#define NUM_LEDS 3

/*******************************************************************************
 *                  ENUMERATIONS AND STRUCTURES AND TYPEDEFS                    *
 ******************************************************************************/

/*******************************************************************************
 *              VARIABLE PROTOTYPES WITH GLOBAL SCOPE                           *
 ******************************************************************************/

/*******************************************************************************
 *                  FUNCTION PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

void initLeds();
void dispClearLED(int n);
void dispSetLED(int n);
void dispToggleLED(int n);


/*******************************************************************************
 ******************************************************************************/


#endif /* _LEDS_H_ */