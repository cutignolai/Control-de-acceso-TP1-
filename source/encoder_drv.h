/***************************************************************************//**
  @file     encoder_drv.h
  @brief    Encoder Driver
  @author   Micho
 ******************************************************************************/
#ifndef _EDRV_H_
#define _EDRV_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/




/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/




/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef enum{LEFT,
	RIGHT,
	CLICK,
	NONE
}encoderEvent_t;



/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/


void initEncoder();                           //Inicialización del encoder

bool encoderGetStatus();                      //Me devuelve si hubo un cambio en la perilla

bool encoderSetStatus(bool change_state);     //Me setea el status para que no quede en evento

encoderEvent_t encoderGetEvent();             //Me devuelve si fue un giro hacia la derecha, izquierda, click o nada aun




/*******************************************************************************
 ******************************************************************************/

#endif // _EDRV_H_