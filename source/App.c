/***************************************************************************//**
  @file     App.c
  @brief    Display Test
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "display.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_UNIT_ID         8
#define MAX_UNIT_PASS       4

#define READY               true
#define NOT_READY           !READY

#define USUARIO_VALIDO      true
#define USUARIO_INVALIDO    !USUARIO_VALIDO

#define ENTER_CLICK         1
#define BRILLO_CLICK        2
#define SUPR_CLICK          3
 
#define SI                  true
#define NO                  false

#define ACTIVADO            true
#define DESACTIVADO         false

#define GET_SEL0(x) (x & 1)
#define GET_SEL1(x) ((x >> 1) & 1)

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void reset_all (void);
bool CardReaderIsReady();


/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
pin_t pin_arr[] = {DIO_1, DIO_3, DIO_5, DIO_7, DIO_10, DIO_11, DIO_13, DIO_15, DIO_2, DIO_4};


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	initDisplay();

	digit_t msg[] = {IDX_H, IDX_O, IDX_L, IDX_A, IDX_CLEAR, IDX_C, IDX_H, IDX_I, IDX_C, IDX_O, IDX_S};
	// setStaticMode();
	// loadBuffer(&msg[0], 4);
	setScrollMode();
  	loadBuffer(&msg[0], 11);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	// int j;
	// for (j = 0; j < 4; j++){
	// 	gpioWrite(DIO_2, GET_SEL0(j));
	// 	gpioWrite(DIO_4, GET_SEL1(j));
	// 	int i;
	// 	for (i = 0; i < 8; i++){
	// 		gpioWrite(pin_arr[i], 1);
	// 	}
	// }


  // setScrollMode();
  // loadBuffer(&msg[0], 4);

  // digit_t msg2[] = {IDX_C, IDX_O, IDX_C, IDX_A, IDX_C, IDX_O, IDX_L, IDX_A};
  // loadBuffer(&msg2[0], 8);

  // bool blink_arr = { false, true, false, false};
  // setBlinkingDigits( &blink_arr );
  // setBlinkMode();
  // setBufferIndex(2);
  // showLastDigits(true);

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/

