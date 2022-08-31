/***************************************************************************//**
  @file     leds.c
  @brief    leds driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                        INCLUDE HEADER FILES                                *
 ******************************************************************************/

#include "leds.h"
#include "board.h"
/*******************************************************************************
 *            CONSTANT AND MACRO DEFINITIONS USING #DEFINE                    *
 ******************************************************************************/

//  #define PIN_LED_RED     PORTNUM2PIN(PB,22)	// PTB22
//  #define PIN_LED_GREEN   PORTNUM2PIN(PE,26)	// PTE26
//  #define PIN_LED_BLUE    PORTNUM2PIN(PB,21)	// PTB21

/*******************************************************************************
 *                  ENUMERATIONS AND STRUCTURES AND TYPEDEFS                    *
 ******************************************************************************/

//----------- COLORES DEL LED ------------
typedef enum{
    LED_ROJO,
    LED_VERDE,
    LED_AZUL
}colored_led_t;
//-----------------------------------------

/*******************************************************************************
 *      STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE            *
 ******************************************************************************/


static bool leds[NUM_LEDS];

  //static int refresh_rate = 2;        //tasa de refresco del timer
  //static tim_id_t rate_id;            //VERRRRRRRRRRR

static void callback_leds();

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initLEDs()
{
  gpioMode(PIN_LED_RED, OUTPUT);
  gpioMode(PIN_LED_GREEN, OUTPUT);
  gpioMode(PIN_LED_BLUE, OUTPUT);


  for(int i=0; i<NUM_LEDS; i++)
	{
		clear_led(i);
	}

  //rate_id = timerGetId();
	//timerStart(rate_id, TIMER_MS2TICKS(my_refresh_rate), TIM_MODE_PERIODIC, &callback_leds);

}


void set_led(int led_number)       //set  true
{
  if(led_number == LED_AZUL)
  {

  }
  else if (led_number == LED_AZUL)
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
  gpioWrite (LED_CONF_1, leds_selector[index].led_bit_0);     //escribo en el primer selector, lo que vale el bit 0
  gpioWrite (LED_CONF_2, leds_selector[index].led_bit_1);     //escribo en el segundo selector, lo que vale el bit 1
	index++;
  index = (index == NUM_LEDS)? 0 : index;                     //si es mayor a la cantidad de leds, vuelvo el counter a 0
}
