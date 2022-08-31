/***************************************************************************//**
  @file     encoder.c
  @brief    Encoder Driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                              INCLUDE HEADER FILES                            *
 ******************************************************************************/

#include "board.h"
#include "encoder.h"
//#include "timers_drv.h"


/*******************************************************************************
 *                 CONSTANT AND MACRO DEFINITIONS USING #DEFINE                 *
 ******************************************************************************/

//#define PIN_A 		PORTNUM2PIN(Puerto, num)
//#define PIN_B 		PORTNUM2PIN(Puerto, num)
//#define PIN_C 	    PORTNUM2PIN(Puerto, num)
#define ON      	LOW         //Activo bajo


/*******************************************************************************
 *                              ENUMS AND STRUCTURES                            *
 ******************************************************************************/

enum states {START, LEFT_1, LEFT_2, LEFT_3, RIGHT_1, RIGHT_2, RIGHT_3};           //FSM


/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static encoderEvent_t event_coming(bool A, bool B);
static void callback_encoder(void);
static void click_timer(void);
static void get_current_values(void);


/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
static int click_counter = 0;       //Cantidad de clicks
static bool flag_timer_finish = false;      //Timer de clicks termino
static bool flag_timer_is_running = false;  //Timer de clicks esta corriendo
static bool current_A;        //Valor actual de A
static bool current_B;        //Valor actual de B
static bool current_C;        //Valor actual de C
static bool status;                         //Estado del encoder (para la FSM)
static encoderEvent_t encoder_event;       //Eveneto del encoder
static tim_id_t encoder_timer;             //timer



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initEncoder() {
	initTimers();                               //Inicializo Timer
	encoder_timer = timerGetId();

    //Pins Modes//
	gpioMode(PIN_A, INPUT);
	gpioMode(PIN_B, INPUT);
	gpioMode(PIN_C, INPUT);

    encoder_event = NONE;               //Se inicializa con el evento nulo (que no hay)
    status = false;                     //Variable de cambio en falso

    //Periodic Interuption ---> encoder_callback (1ms)
	timerStart(encoder_timer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, callback_encoder);
}

bool encoderGetStatus(){            //Si hay un evento, devolveme true, sino devolveme un false
	if(status){
		status = false;
		return true;
	}
    else{
		return false;
	}
}

encoderEvent_t encoderGetEvent(){          //Getter del evento del encoder
	return encoder_event;
}

bool encoderSetStatus(bool change_state){            //Setter para que la app me lo pueda cambiar
	status = change_status;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static encoderEvent_t event_coming(bool A, bool B){         //FSM: check if the user switch left or right

    static enum states current_state = START;
    static bool last_state_button = false;                  //el switch arranca en false

    encoderEvent_t turn = NONE;                             //Todavía no hay giro ni nada

    // Veo si hubo cambió (flanco descendente o ascendente)
    bool current_state_button = (current_C == ON);
    if(!last_state_button && current_state_button){         //si el estados de ambos son distintos, entonces hubo un cambio (se pulso el botton)
        click_counter += 1;
        if(click_counter == 1){
            //inicializo timer 
            flag_timer_is_running = true;
        }
    }
    last_state_button = current_state_button;               //cambio variable para que quede arriba

    if(flag_timer_finish){                     //finalizo el timer
        if(click_counter == 1){         //si se apreto una vez
            turn = CLICK;
            status = true;            //hubo un cambio
            click_counter = 0;
        }
        else if(contador == 2){         //si se apreto dos veces
            turn = CLICK_2;
            status = true;              //hubo un cambio
            click_counter = 0;
        }
        else{
            turn = CLICK_3;             //si se apreto mas de dos veces, se asume como 3
            status = true;              //hubo un cambio
            click_counter = 0;
        }
        flag_timer_finish = false;
    }

    //FSM del encoder
    if(!flag_timer_is_running){

        switch(current_state){          //RECORDAR QUE ES ACTIVO BAJO!!!!

            case START:                 //INICIO: DERECHA O IZQUIERDA
                    if (A && !B){  
                        current_state = RIGHT_1;        //CON B EN 0 Y A EN 1 VOY PARA LA DERECHA
                    }
                    if (!A && B){
                        current_state = LEFT_1;         //CON A EN 0 Y B EN 1 VOY PARA LA IZQUIERDA
                    }
            break;

            //IQUIERDA
            case LEFT_1:                //SI SE DETECTÓ QUE GIRO PARA LA IZQUIERDA
                    if(!A && !B){
                        current_state = LEFT_2;         //SI AMBOS SON 0, ENTONCES PASO AL PROXIMO ESTADO
                    }
                    else if(!A && B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;          //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;
            case LEFT_2:                //SIGUE PARA LA IZQUIERDA
                    if(A && !B){
                        current_state = LEFT_3;         //SI B ES 0 Y A ES 1, ENTONCES PASO AL PROXIMO ESTADO
                    }
                    else if(!A && !B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;          //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;
            case LEFT_3:                //SIGUE PARA LA IZQUIERDA
                    if(A && B){                     
                        current_state = START;          //VOLVI A ESTAR EN 1. QUE LA PERILLA ESTA EN REPOSO, ENTONCES VUELVO AL INICIO
                        turn = LEFT;              //UNA VEZ QUE SE HIZO EL MOVIMIENTO COMPLETO, CONFIRMO QUE ES IZQUIERDA
                        status = true;
                    } 
                    else if(A && !B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;              //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;

            //DERECHA
            case RIGHT_1:               //SI YA SE DETECTÓ QUE GIRO PARA LA DERECHA
                    if(!A && !B){
                        current_state = RIGHT_2;        //SI AMBOS SON 0, ENTONCES PASO AL PROXIMO ESTADO
                    }
                    else if(A && !B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;          //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;
            case RIGHT_2:                   //SIGUE PARA LA DERECHA
                    if(!A && B){
                        current_state = RIGHT_3;        //SI A ES 0 Y B ES 1, ENTONCES PASO AL PROXIMO ESTADO
                    }
                    else if(!A && !B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;         //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;
            case RIGHT_3:                   //SIGUE PARA LA DERECHA
                    if(A && B){                     //VOLVI A ESTAR EN 1. QUE LA PERILLA ESTA EN REPOSO, ENTONCES VUELVO AL INICIO
                        current_state = START;
                        turn = RIGHT;         //UNA VEZ QUE SE HIZO EL MOVIMIENTO COMPLETO, CONFIRMO QUE ES DERECHA
                        status = true;
                    } 
                    else if(!A && B){
                        //SIGUE EL MISMO ESTADO
                    }
                    else{
                        current_state = START;         //SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                    }
            break;
            default: break;
        }
    }
    return turn;                              //DEVUELVO EL RESULTADO: IZQ O DERECHA
}


static void callback_encoder(void){                         //el callback
    get_current_values();                                   //Me fijo valores actuales de los pines de A, B y C
    encoder_event = event_coming(current_A, current_B);     //Me fijo si hubo un cambio en A o en B
}

static void click_timer(void){                              //el callback
    flag_timer_finish = true;                               //termino el tiempo del click timer
    flag_timer_is_running = false;                          //ya no corre mas
}

static void get_current_values(void){       //Me fijo valor actual del pin
	current_A = gpioRead(PIN_A);
	current_B = gpioRead(PIN_B);
    current_C = gpioRead(PIN_C);
}

/*******************************************************************************
 ******************************************************************************/