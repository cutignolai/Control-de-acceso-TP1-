/***************************************************************************//**
  @file     button.h
  @brief    Button Driver
  @author   Micho
 ******************************************************************************/
#ifndef _BUTTON_H_
#define _BUTTON_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/




/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/




/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef enum{CLICK,
	CLICK_2,
    CLICK_3,
	NONE_CLICK
}buttonEvent_t;



/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/


void initButton(void);                           //Inicialización del encoder

bool buttonGetStatus();                      //Me devuelve si hubo un cambio en la perilla

bool buttonSetStatus(bool change_state);     //Me setea el status para que no quede en evento

buttonEvent_t buttonGetEvent();             //Me devuelve si fue un giro hacia la derecha, izquierda, click o nada aun




/*******************************************************************************
 ******************************************************************************/

#endif // _BUTTON_H_
