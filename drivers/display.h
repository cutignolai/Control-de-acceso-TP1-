/***************************************************************************//**
  @file     display.h
  @brief    Display Driver
  @author   Olivia De Vincenti
 ******************************************************************************/
#ifndef _ENCODER_H_
#define _ENCODER_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "gpio.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define ID_MESSAGE_LONG     8
#define PASS_MESSAGE_LONG   5

/********** CHAR INDEX **********/
#define IDX_0       0
#define IDX_1       1
#define IDX_2       2
#define IDX_3       3
#define IDX_4       4
#define IDX_5       5
#define IDX_6       6
#define IDX_7       7
#define IDX_8       8
#define IDX_9       9
#define IDX_DASH        10
#define IDX_LOW_DASH    11
#define IDX_POINT       12
#define IDX_CLEAR       13

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef uint8_t digit_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/


/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/
//------------------------ GLOBAL ------------------------
// INIT
void initDisplay();                          

// SETTERS
/**
 * @brief Limpia el display, deja todos lo dígitos vacíos
 */
void clearDisplay();



/**
 * @brief Muestra en el display un mensaje de longitud variable entre 1 y 16 dígitos
 * 
 * @param message puntero al arreglo que contiene el mensaje. ESTE ARREGLO TIENE QUE CONTENER LOS CHAR IDX
 * @param message_len longitud del mensaje
 */
void showMessage(uint8_t* message, uint8_t message_len);


/**
 * @brief Muestra en el display el dígito indicado en la posición indicada, rellena las posiciones anteriores con '-' (DASH)
 * 
 * @param char_idx char idx del caracter a mostrar
 * @param position posición en la que mostrar el caracter
 */
void showPasswordDigit(uint8_t char_idx, uint8_t position);

/* 
--------------------------------------------------------- 
   Se encarga de subir y bajar el brillo
--------------------------------------------------------- 
*/
void bright_up(void);
void bright_down(void);


//------------------------ LOCAL ------------------------


/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_H_