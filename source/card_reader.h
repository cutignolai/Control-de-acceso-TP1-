/***************************************************************************//**
  @file     card_reader.h
  @brief    card reader services
  @authors  Guillermo OM, Pedro DL
 ******************************************************************************/

#ifndef _CARD_READER_
#define _CARD_READER_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
 

/**
 * @brief Initializes the card reader
 */
void initCardReader(void);

/**
 * @brief Checks if there's some information available regarding the card
 * @return If the information is ready
 */
bool CardReaderIsReady (void);

/**
 * @brief Asks for the ID stored 
 * @return An array of 8 int in case there was data. If nor a NULL
 */
int* getID (void);

/**
 * @brief Resets the card reader data in case it's necessary
 */
void resetReader (void);

/*******************************************************************************
 ******************************************************************************/

#endif // _CARD_READER_
