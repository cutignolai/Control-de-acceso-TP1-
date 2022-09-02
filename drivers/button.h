/***************************************************************************//**
  @file     button.h
  @brief    Button Driver
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

#define MAX_LONG_CLICK 500            //ms
#define PERIODIC_BUTTON_TIME 1        //ms
#define PERIODIC_LONG_CLICK_TIME 10   //ms      //branch
#define SINGLESHOT_CLICK_TIME 700     //ms



/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/

typedef enum{CLICK,
	CLICK_2,
  CLICK_3,
  CLICK_LONG,       //branch
	NONE_CLICK
}buttonEvent_t;



/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/


void initButton();                           //Inicialización del encoder

bool buttonGetStatus();                      //Me devuelve si hubo un cambio en la perilla

bool buttonSetStatus(bool change_state);     //Me setea el status para que no quede en evento

buttonEvent_t buttonGetEvent();             //Me devuelve si fue un giro hacia la derecha, izquierda, click o nada aun




/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_H_