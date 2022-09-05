/***************************************************************************//**
  @file     message.h
  @brief    Message Handler
  @author   Olivia De Vincenti
 ******************************************************************************/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

/*******************************************************************************
 *                          INCLUDE HEADER FILES                                *
 ******************************************************************************/

#include "gpio.h"
#include "display.h"

/*******************************************************************************
 *              CONSTANT AND MACRO DEFINITIONS USING #DEFINE                    *
 ******************************************************************************/



/*******************************************************************************
 *                  ENUMERATIONS AND STRUCTURES AND TYPEDEFS                    *
 ******************************************************************************/
typedef enum{
    NO_MSG = 0,
    a,
    b,
    c,
    d,
    e,
    f,
    g,
    MSG_PENDING
} messageEvent_t;
/*******************************************************************************
 *              VARIABLE PROTOTYPES WITH GLOBAL SCOPE                           *
 ******************************************************************************/

/*******************************************************************************
 *                  FUNCTION PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

bool messageHandlerStatus();

messageEvent_t messageGetEvent();

bool messageSetStatus(bool change_state);

/*******************************************************************************
 ******************************************************************************/


#endif /* _MESSAGE_H_ */