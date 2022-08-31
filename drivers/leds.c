/***************************************************************************//**
  @file     leds.c
  @brief    Leds driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                        INCLUDE HEADER FILES                                *
 ******************************************************************************/

#include "leds.h"
<<<<<<< Updated upstream
<<<<<<< HEAD
#include "timer.h"

=======
#include "board.h"
>>>>>>> 8f353db74644faa259b72305d4753deabc6f16af
=======
//#include "timers_drv.h"

>>>>>>> Stashed changes
/*******************************************************************************
 *            CONSTANT AND MACRO DEFINITIONS USING #DEFINE                    *
 ******************************************************************************/

//#define LED_CONF_1 PORTNUM2PIN(Puerto,num)
//#define LED_CONF_2 PORTNUM2PIN(Puerto,num)

/*******************************************************************************
 *                  ENUMERATIONS AND STRUCTURES AND TYPEDEFS                    *
 ******************************************************************************/
<<<<<<< Updated upstream
<<<<<<< HEAD
typedef struct led_selector_t{
  bool led_bit_0;
	bool led_bit_1;
}led_selector_t;
=======

//----------- COLORES DEL LED ------------
typedef enum{
    LED_ROJO,
    LED_VERDE,
    LED_AZUL
}colored_led_t;
//-----------------------------------------
>>>>>>> 8f353db74644faa259b72305d4753deabc6f16af
=======
typedef struct led_selector_t
{
	bool led_bit_0;
	bool led_bit_1;
}led_selector_t;
>>>>>>> Stashed changes

/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static void callback_leds();

/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
static bool leds[NUM_LEDS];
<<<<<<< Updated upstream
<<<<<<< HEAD
static led_selector_t led_selector[] = {
  {LOW,LOW},    //00 --> NINGUN LED
  {LOW,HIGH},   //01 --> LED0
  {HIGH,LOW},   //10 --> LED1
  {HIGH,HIGH}   //11 --> LED
};
static int index = 0;
static tim_id_t leds_timer;            
=======

  //static int refresh_rate = 2;        //tasa de refresco del timer
  //static tim_id_t rate_id;            //VERRRRRRRRRRR

static void callback_leds();
>>>>>>> 8f353db74644faa259b72305d4753deabc6f16af
=======
static led_selector_t led_selector[] = {
  {LOW,LOW},    //00 --> NINGUN LED
  {LOW,HIGH},   //01 --> LED1
  {HIGH,LOW},   //10 --> LED2
  {HIGH,HIGH}   //11 --> LED3
  };
  static int index = 0;
  //static int refresh_rate = 2;        //tasa de refresco del timer
  //static tim_id_t rate_id;            //VERRRRRRRRRRR

  static void callback_leds();
>>>>>>> Stashed changes

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initLeds()
{
  gpioMode(LED_CONF_1, OUTPUT);
	gpioMode(LED_CONF_2, OUTPUT);

<<<<<<< Updated upstream
<<<<<<< HEAD
  for(int i = 0; i < NUM_LEDS; i++)
=======

  for(int i=0; i<NUM_LEDS; i++)
>>>>>>> 8f353db74644faa259b72305d4753deabc6f16af
=======
  for(int i=0; i<DISP_LEDS_NUM;i++)
>>>>>>> Stashed changes
	{
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
  gpioWrite (LED_CONF_1, leds_selector[index].led_bit_0);     //escribo en el primer selector, lo que vale el bit 0
  gpioWrite (LED_CONF_2, leds_selector[index].led_bit_1);     //escribo en el segundo selector, lo que vale el bit 1
	index++;
  index = (index == NUM_LEDS)? 0 : index;                     //si es mayor a la cantidad de leds, vuelvo el counter a 0
}
