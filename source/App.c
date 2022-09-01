/***************************************************************************//**
  @file     App.c
  @brief    Timer test
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "timer.h"

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

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void timer_callback();

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
uint8_t sec = 0;
tim_id_t sec_timer;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    timerInit(); 
    sec_timer = timerGetId();
    timerStart(sec_timer, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, timer_callback);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

    if (sec == 10){
        timerStop(sec_timer);
        printf("Termino");
    }
	//nada

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void timer_callback(){
    sec++;
    printf("sec: %i\n", sec/1000);
}

/*******************************************************************************
 ******************************************************************************/

