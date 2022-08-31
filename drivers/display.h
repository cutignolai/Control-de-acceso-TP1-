/***************************************************************************//**
  @file     encoder.h
  @brief    Encoder Driver
  @author   Micho
 ******************************************************************************/
#ifndef _ENCODER_H_
#define _ENCODER_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define ID_MESSAGE_LONG     8
#define PASS_MESSAGE_LONG   4

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/

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


/* 
--------------------------------------------------------- 
   Se encarga de recibir un arreglo de enteros 
   y el largo del arreglo
   
   Ejemplo:
            message = [1, 2, 2, 3, 1, 0, 0, 1]
            long_message = ID_MESSAGE_LONG 
--------------------------------------------------------- 
*/
void setLongString(int message[], int long_message);


/* 
--------------------------------------------------------- 
   Se encarga de recibir un valor, la posicion y el largo
   (el resto de los numeros los oculta)
   
   Ejemplo:
            message = 1
            position = 3 

            mensaje: _ _ 1 
--------------------------------------------------------- 
*/
void setShortChar(int number, int position, int password_long);


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