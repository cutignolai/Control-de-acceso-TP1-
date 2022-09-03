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

#define GET_SEL0(x) (x & 1)
#define GET_SEL1(x) ((x >> 1) & 1)

#define GET_SIZE(x, y)      ( ( sizeof (x) ) / ( sizeof (y) ) )

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void reset_all (void);
bool CardReaderIsReady();
void bright_callback();
static void show_input(digit_t *input_ptr, uint8_t input_ptr_len, uint8_t pos);

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
pin_t pin_arr[] = {DIO_1, DIO_3, DIO_5, DIO_7, DIO_10, DIO_11, DIO_13, DIO_15, DIO_2, DIO_4};
tim_id_t intensity_timer;
uint8_t b = 0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	initDisplay();

	// /***** TEST SCROLL *****/
	// digit_t msg[] = {IDX_H, IDX_O, IDX_L, IDX_A, IDX_CLEAR, IDX_C, IDX_H, IDX_I, IDX_C, IDX_O, IDX_S};
	// setScrollMode();
  	// loadBuffer(&msg[0], 11);

	// /***** TEST STATIC *****/
	// digit_t msg[] = {IDX_H, IDX_O, IDX_L, IDX_A};
	// setStaticMode();
	// loadBuffer(&msg[0], 4);

	// /***** TEST BRILLO *****/
	// digit_t msg[] = {IDX_8, IDX_8, IDX_8, IDX_8};
	// setBrightness(BRIGHTNESS_LOW);
	// intensity_timer = timerGetId();
	// timerStart(intensity_timer, TIMER_MS2TICKS(5000), TIM_MODE_PERIODIC, bright_callback);

	// /***** LEDS PARA TEST *****/
	// gpioMode(PIN_LED_RED, OUTPUT);
    // gpioMode(PIN_LED_GREEN, OUTPUT);
    // gpioWrite(PIN_LED_RED, !LED_ACTIVE);
    // gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);

	// /***** TEST BLINK *****/
  	// digit_t pass[] = { IDX_DASH, IDX_DASH, IDX_DASH, IDX_5 };
  	// loadBuffer(&pass[0], 4);
	// bool blink_arr[] = { false, false, false, true};
	// setBlinkingDigits( &blink_arr[0] );
	// setBlinkMode();

	// /***** TEST BUFFER IDX *****/		
	// //setBufferIndex(2);
	// showLastDigits(true);

    /***** TEST SHOW INPUT *****/
    digit_t msg[] = {1, 2, 3};
    show_input(&msg[0], GET_SIZE(msg, msg[0]), GET_SIZE(msg, msg[0]) - 1);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	/***** TEST PINES *****/
	// int j;
	// for (j = 0; j < 4; j++){
	// 	gpioWrite(DIO_2, GET_SEL0(j));
	// 	gpioWrite(DIO_4, GET_SEL1(j));
	// 	int i;
	// 	for (i = 0; i < 8; i++){
	// 		gpioWrite(pin_arr[i], 1);
	// 	}
	// }

	//			:)

	// /***** TEST BRILLO *****/
	// if (b == 3){
	// 	timerStop(intensity_timer);
	// 	setBrightness(BRIGHTNESS_LOW);
	// 	gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	// 	gpioWrite(PIN_LED_RED, LED_ACTIVE);
	// 	b++;
	// }

}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void bright_callback(){
	b++;
	upBrightness();
	if (gpioRead(PIN_LED_GREEN) == LED_ACTIVE){
		gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	} else {
		gpioWrite(PIN_LED_GREEN, LED_ACTIVE);
	}
}

static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos)
{
    bool blink[] = {false, false, false, false};
    if(pos<DISPLAY_LEN)
    {
        blink[pos] = true;
    }
    else
    {
        blink[DISPLAY_LEN-1] = true;
    }

    setBlinkingDigits(&blink[0]);
    showLastDigits(true);
    loadBuffer(input_ptr, input_len);
    setBlinkMode();
}

static void show_message(digit_t *msg_ptr, uint8_t msg_len)
{
    loadBuffer(msg_ptr, msg_len);
    setScrollMode();
}

/*******************************************************************************
 ******************************************************************************/
