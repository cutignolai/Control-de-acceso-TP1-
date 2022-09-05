/***************************************************************************//**
  @file     button.h
  @brief    Button Driver
  @author   Ignacio Cutignola
 ******************************************************************************/
#ifndef _USER_H_
#define _USER_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define MAX_ID         8
#define MAX_PASS       5



/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef struct Users {
	uint8_t id[MAX_ID];
	uint8_t pass[MAX_PASS];
} User_t;



/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/



/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/

bool checkUser(uint8_t id[], uint8_t pass[], uint8_t pass_len);




/*******************************************************************************
 ******************************************************************************/

#endif // _USER_H_
