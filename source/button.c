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

static void event_coming(bool C);
void callback_click(void);
void get_current_values(void);
void callback_button(void);

/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
int click_counter = 0;               //Cantidad de clicks
bool last_state_button = false;      //el switch arranca en false
bool current_C;                      //Valor actual de C
bool button_status;                         //Estado del button
buttonEvent_t button_event;          //Eveneto del button
tim_id_t button_timer;               //timer
tim_id_t click_timer;                //timer

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initButton() {
	timerInit();                               //Inicializo Timer
	button_timer = timerGetId();

    //Pins Modes//
	gpioMode(PIN_C, INPUT);             //modo del pin

    button_event = NONE_CLICK;               //Se inicializa con el evento nulo (que no hay)
    button_status = false;                     //Variable de cambio en falso

    //Periodic Interuption ---> button_callback (1ms)
	timerStart(button_timer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, &callback_button);
}

bool buttonGetStatus(){            //Si hay un evento, devolveme true, sino devolveme un false
	if(button_status){
		button_status = false;
		return true;
	}
    else{
		return false;
	}
}

buttonEvent_t buttonGetEvent(){          //Getter del evento del button
	return button_event;
}

bool buttonSetStatus(bool change_state){            //Setter para que la app me lo pueda cambiar
	button_status = change_state;
	return button_status;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void event_coming(bool C){

    // Veo si hubo cambió (flanco descendente)
    bool current_state_button = (current_C == ON);
    if(!last_state_button && current_state_button){         //si el estados de ambos son distintos, entonces hubo un cambio (se pulso el botton)
        click_counter += 1;
        if(click_counter == 1){
            click_timer = timerGetId();                 //inicializo timer
            timerStart(click_timer, TIMER_MS2TICKS(500), TIM_MODE_SINGLESHOT, callback_click);     //inicializo el timer
        }
        else if(click_counter == 2){
            timerRestart(click_timer);     //inicializo el timer
        }
        else{
            timerFinish(click_timer);
        }
    }
    last_state_button = current_state_button;               //cambio variable para que quede arriba
}


void callback_button(void){                         //el callback
    get_current_values();                                   //Me fijo valores actuales de los pines de A, B y C
    event_coming(current_C);     //Me fijo si hubo un cambio en A o en B
}

void callback_click(void){                           //el callback
    buttonEvent_t turn = NONE_CLICK;
    if(click_counter == 1){         //si se apreto una vez
        turn = CLICK;
        button_status = true;            //hubo un cambio
        click_counter = 0;
    }
    else if(click_counter == 2){         //si se apreto dos veces
        turn = CLICK_2;
        button_status = true;              //hubo un cambio
        click_counter = 0;
    }
    else{
        turn = CLICK_3;             //si se apreto mas de dos veces, se asume como 3
        button_status = true;              //hubo un cambio
        click_counter = 0;
    }
    button_event = turn;
}

void get_current_values(void){       //Me fijo valor actual del pin
    current_C = gpioRead(PIN_C);
}

/*******************************************************************************
 ******************************************************************************/
