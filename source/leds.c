/***************************************************************************//**
  @file     leds.c
  @brief    leds driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                        INCLUDE HEADER FILES                                *
 ******************************************************************************/

#include "leds.h"
#include "timer.h"
#include "board.h"

/*******************************************************************************
 *            CONSTANT AND MACRO DEFINITIONS USING #DEFINE                    *
 ******************************************************************************/

#define LED_CONF_1  DIO_12
#define LED_CONF_2  DIO_14

/*******************************************************************************
 *                  ENUMERATIONS AND STRUCTURES AND TYPEDEFS                    *
 ******************************************************************************/
typedef struct {
	bool led_bit_0;
	bool led_bit_1;
} led_selector_t;

/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static void callback_leds();

/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
bool leds[NUM_LEDS];
led_selector_t led_selector[] = {
  {  LOW, LOW  },  //00 --> NINGUN LED
  {  LOW, HIGH },  //01 --> LED1
  { HIGH, LOW  },  //10 --> LED2
  { HIGH, HIGH }   //11 --> LED3
};
uint8_t index = 0;
tim_id_t leds_timer;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initLeds(){
  gpioMode(LED_CONF_1, OUTPUT);
	gpioMode(LED_CONF_2, OUTPUT);

  for(int i = 0; i < NUM_LEDS; i++){
		clear_led(i);
	}

  leds_timer = timerGetId();
  //Periodic Interuption ---> leds_callback (4ms)
	timerStart(leds_timer, TIMER_MS2TICKS(4), TIM_MODE_PERIODIC, &callback_leds);

}

void set_led(int l)       //set  true
{
	leds[l] = true;
}

void clear_led(int l)     //set false
{
	leds[l] = false;
}

void toggle_led(int l)    //not leds
{
	leds[l] = !leds[l];
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static void callback_leds()         //callback
{
  if(leds[index]){
    gpioWrite (LED_CONF_1, led_selector[index].led_bit_0);     //escribo en el primer selector, lo que vale el bit 0
    gpioWrite (LED_CONF_2, led_selector[index].led_bit_1);     //escribo en el segundo selector, lo que vale el bit 1
  }
  else{
    gpioWrite (LED_CONF_1, led_selector[LOW].led_bit_0);     //escribo en el primer selector, lo que vale el bit 0
    gpioWrite (LED_CONF_2, led_selector[LOW].led_bit_1);     //escribo en el segundo selector, lo que vale el bit 1
  }
  index++;
  index = (index == NUM_LEDS)? 0 : index;                     //si es mayor a la cantidad de leds, vuelvo el counter a 0
}
