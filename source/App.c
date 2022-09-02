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


/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void red_callback();
void blue_callback();

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
uint8_t sec = 0;
tim_id_t red_timer;
tim_id_t blue_timer;
uint8_t red_state;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	sec = 0;
	timerInit();
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);
	red_timer = timerGetId();
	blue_timer = timerGetId();
	red_state = LED_ACTIVE;
	gpioWrite(PIN_LED_RED, red_state);
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
	timerStart(red_timer, TIMER_MS2TICKS(1000), TIM_MODE_PERIODIC, red_callback);
	timerStart(blue_timer, TIMER_MS2TICKS(5000), TIM_MODE_SINGLESHOT, blue_callback);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	if (sec == 10){
		timerStop(red_timer);
		gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	}

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void red_callback(){
    sec++;
    red_state = !red_state;
    gpioWrite(PIN_LED_RED, red_state);
    //printf("sec: %i\n", sec/1000);
}

void blue_callback(){
	gpioWrite(PIN_LED_BLUE, LED_ACTIVE);
}

/*******************************************************************************
 ******************************************************************************/

