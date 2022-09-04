/***************************************************************************//**
  @file     button.c
  @brief    Button Driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                              INCLUDE HEADER FILES                            *
 ******************************************************************************/

#include "board.h"
#include "button.h"
#include "timer.h"


/*******************************************************************************
 *                 CONSTANT AND MACRO DEFINITIONS USING #DEFINE                 *
 ******************************************************************************/

#define PIN_C   DIO_10
#define ON      LOW         //Activo bajo ---> Prendido
#define OFF     HIGH        //Activo bajo ---> Apagado


/*******************************************************************************
 *                              ENUMS AND STRUCTURES                            *
 ******************************************************************************/


/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static buttonEvent_t event_coming(bool C);
static void callback_click(void);
static void callback_button(void);
// static void callback_click_long(void);
static void get_current_values(void);


/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
static int click_counter = 0;               //Cantidad de clicks
// static int long_click_counter = 0;          //Click mantenido                   //branch
static bool last_state_button = false;      //el switch arranca en false        
// static bool long_click = false;             //el switch arranca en false        //branch
static bool current_C;                      //Valor actual de C
static bool status;                         //Estado del button
static buttonEvent_t turn = NONE_CLICK;
static buttonEvent_t button_event;          //Eveneto del button
static tim_id_t button_timer;               //timer
static tim_id_t click_timer;                //timer
// static tim_id_t click_long_timer;           //timer                             //branch



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initButton() {
	timerInit();                               //Inicializo Timer

    //Pins Modes//
	gpioMode(PIN_C, INPUT);             //modo del pin
    
    // Led para mostrar cuando se ejecuta una interrupcion
    // gpioMode(PIN_LED_BLUE, OUTPUT);
    // gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);

    button_event = NONE_CLICK;               //Se inicializa con el evento nulo (que no hay)
    status = false;                     //Variable de cambio en falso

    //Periodic Interuption ---> button_callback (1ms)
	button_timer = timerGetId();
	timerStart(button_timer, TIMER_MS2TICKS(PERIODIC_BUTTON_TIME), TIM_MODE_PERIODIC, callback_button);

    click_timer = timerGetId();
    timerCreate(click_timer, TIMER_MS2TICKS(SINGLESHOT_CLICK_TIME), TIM_MODE_SINGLESHOT, callback_click);

}

bool buttonGetStatus(){            //Si hay un evento, devolveme true, sino devolveme un false
	if(status){
		status = false;
		return true;
	}
    else{
		return false;
	}
}

buttonEvent_t buttonGetEvent(){          //Getter del evento del button
	return button_event;
}

bool buttonSetStatus(bool change_status){            //Setter para que la app me lo pueda cambiar
	status = change_status;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static buttonEvent_t event_coming(bool C){         //FSM: check if the user switch left or right

    // Veo si hubo cambió (flanco descendente)
    bool current_state_button = (current_C == ON);
    if(!last_state_button && current_state_button){         //si el estados de ambos son distintos, entonces hubo un cambio (se pulso el botton)
        click_counter += 1;
        if(click_counter == 1){
            // long_click = true;                          //branch
            timerRestart(click_timer);     //inicializo el timer (de clicks futuros)
        }
        else if(click_counter == 2){
            timerRestart(click_timer);     //inicializo el timer
        }
        else{
            timerFinish(click_timer);
        }
    }
    /*
    if(click_counter == 1){     //branch
        if (last_state_button && current_state_button && long_click){       //si sigue en el mismo estado, entonces puede ser un long click
            click_long_timer = timerGetId();                 //inicializo timer
            timerStart(click_long_timer, TIMER_MS2TICKS(PERIODIC_LONG_CLICK_TIME), TIM_MODE_PERIODIC, callback_click_long);     //pregunto cada 10 ms si sigo ahi, y sumo counter long
        }
        else if (last_state_button && !current_state_button && long_click){     //si no fue click long, entonces fue click normal
            long_click = false;
            long_click_counter = 0;
            timerFinish(click_long_timer);     //termino el timer
        }
    }   //branch
    */
    last_state_button = current_state_button;               //cambio variable para que quede arriba
    return turn;                                            //DEVUELVO EL RESULTADO: IZQ O DERECHA
}


static void callback_button(void){ 
    get_current_values();                                   //Me fijo valores actuales de los pines de A, B y C
    button_event = event_coming(current_C);                 //Me fijo si hubo un cambio en C
}

static void callback_click(void){ 
    /*
    if(long_click_counter >= MAX_LONG_CLICK){       //branch
        turn = CLICK_LONG;              //si se apreto mucho tiempo, tengo un click sostenido
        status = true;                  //hubo un cambio
        click_counter = 0;
        long_click_counter = 0;
    }
    */

    if(click_counter == 1){         //si se apreto una vez
        turn = CLICK;
        status = true;            //hubo un cambio
        click_counter = 0;
        // long_click_counter = 0;
    }
    else if(click_counter == 2){         //si se apreto dos veces
        turn = CLICK_2;
        status = true;              //hubo un cambio
        click_counter = 0;
        // long_click_counter = 0;
    }
    else if(click_counter == 3){
        turn = CLICK_3;             //si se apreto mas de dos veces, se asume como 3
        status = true;              //hubo un cambio
        click_counter = 0;
        // long_click_counter = 0;
    }
    button_event = turn;
}

static void get_current_values(void){       //Me fijo valor actual del pin
    current_C = gpioRead(PIN_C);
}

/*
static void callback_click_long(void){ 
    gpioWrite(PIN_LED_BLUE, LED_ACTIVE);                     //el callback
    long_click_counter += PERIODIC_BUTTON_TIME;             //sumo counter del click long
    gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
}       //branch
*/

/*******************************************************************************
 ******************************************************************************/
