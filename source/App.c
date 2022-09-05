/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "timer.h"
#include "display.h"
#include "leds.h"
#include "encoder.h"
#include "card_reader.h"
#include "button.h"
#include "user.h"
#include "message.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//----------- ESTADOS DEL MENU -----------
typedef enum{
    ESTADO_INIT,
    ESTADO_ID,
    ESTADO_PASS,
    ESTADO_BRILLO,
    ESTADO_VERIFICAR
} estadosDelMenu_t;
//----------------------------------------

//----------- EVENTOS DEL MENU -----------
typedef enum{
    EVENTO_NONE,
    EVENTO_IZQ,
    EVENTO_DER,
    EVENTO_CLICK,
    EVENTO_CLICK_2,
    EVENTO_CLICK_3,
    EVENTO_CLICK_LONG,
    EVENTO_TARJETA,
    EVENTO_MSG
} eventosDelMenu_t;
//----------------------------------------

//----------- COLORES DEL LED ------------
typedef enum{
    LED1,
    LED2,
    LED3
} colored_led_t;
//-----------------------------------------



#define MAX_UNIT_ID         8
#define MIN_UNIT_PASS		4
#define MAX_UNIT_PASS       5

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

#define MSG_TIME            500

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static estadosDelMenu_t idle(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_id(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento);
static estadosDelMenu_t verificar_estado (void);
static void reset_all (void);
static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos);
static void show_message(digit_t *msg_ptr, uint8_t msg_len);
static void show_pass(digit_t *pass_ptr, uint8_t pass_len);
static void show_enter(digit_t *input_ptr, uint8_t input_len);
static void show_brightness();

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/

// ESTADOS
static estadosDelMenu_t estado = ESTADO_INIT;
static estadosDelMenu_t ultimo_estado = ESTADO_INIT;

// ID 
static uint8_t id[] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t posicion_id = 0;
//static int id_buffer[MAX_UNIT_ID];

// CONTRASENA
static uint8_t pass[] = {0, 0, 0, 0, 0};
static uint8_t posicion_pass = 0;

// MESSAGE COMPLETE
static bool ha_hecho_click = NO;
static bool user_is_ready = false;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    timerInit();
    initDisplay();
    initEncoder();
    initLeds();
    initButton(); 
    initCardReader();

    messageSetStatus(ACTIVADO);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{

	eventosDelMenu_t evento = EVENTO_NONE;

    // Analizo si hubo un evento
    if(CardReaderIsReady())
    {
		evento = EVENTO_TARJETA;
	}
    else if(encoderGetStatus())
    {
		evento = encoderGetEvent();	
        encoderSetStatus(DESACTIVADO);
        
	}
    else if(buttonGetStatus())
    {
		evento = buttonGetEvent();	
        buttonSetStatus(DESACTIVADO);
	}
    else if(messageHandlerStatus())
    {
    	evento = messageGetEvent();
    	messageSetStatus(DESACTIVADO);
    }

    // Si hubo un evento, veo en que estado de mi FSM estoy y le envio el evento
	if(evento != EVENTO_NONE)
    {
		switch(estado){
            case ESTADO_INIT:
                estado = idle(evento);
                break;
            case ESTADO_ID:
                estado = modificar_id(evento);
                break;
            case ESTADO_PASS:
                estado = modificar_pass(evento);
                break;
            case ESTADO_BRILLO:
				estado = modificar_brillo(evento);
                break;
            case ESTADO_VERIFICAR:
                estado = verificar_estado();
                break;
            default: break;
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static estadosDelMenu_t idle(eventosDelMenu_t evento)
{
	digit_t msg[] = {IDX_I, IDX_d, IDX_L, IDX_E};
	estadosDelMenu_t proximo_estado = ESTADO_INIT;

    switch(evento)
    {
        case EVENTO_DER:
        case EVENTO_IZQ:
        case EVENTO_CLICK:

            proximo_estado = ESTADO_ID;
            messageSetStatus(ACTIVADO);

            break;

        case EVENTO_CLICK_2:
        case EVENTO_CLICK_3:

            proximo_estado = ESTADO_BRILLO;
            messageSetStatus(ACTIVADO);

            break;
            

        case EVENTO_TARJETA:
            //id_buffer = getIdTarjeta();
            //hacer for
            proximo_estado = ESTADO_PASS;
            messageSetStatus(ACTIVADO);
            // show_input(&id[0], posicion_id + 1, posicion_id);
            // posicion_id = 0;
            break;
        
        case EVENTO_MSG:
            show_message(&msg[0], 4);
            break;
        
        default:
            break;

    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_id(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_ID;
    digit_t msg[] = {IDX_U, IDX_S, IDX_e, IDX_r};

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_id <= MAX_UNIT_ID)
            {
                id[posicion_id] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (id[posicion_id] > 9)
                {
                    id[posicion_id] = 0;
                }
                
                show_input(&id[0], posicion_id + 1, posicion_id);
            }
            break;

        case EVENTO_IZQ:
            if(posicion_id <= MAX_UNIT_ID)
            {
                id[posicion_id] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (id[posicion_id] > 9)
                {
                    id[posicion_id] = 9;
                }

                show_input(&id[0], posicion_id + 1, posicion_id);
            }
            break;

        case EVENTO_CLICK:
        case EVENTO_CLICK_LONG:
            
            ha_hecho_click = SI;

            posicion_id += 1;

            if(posicion_id >= MAX_UNIT_ID)
            {
                posicion_id = 0;
                proximo_estado = ESTADO_PASS;
                messageSetStatus(ACTIVADO);
            }

            //show_enter(&id[0], posicion_id);
            show_input(&id[0], posicion_id + 1, posicion_id);

            break;

        case EVENTO_CLICK_2:

            if( ha_hecho_click == NO )
            {
                // Guardo el estado actual para luego retomar desde aca
                ultimo_estado = ESTADO_ID;
                proximo_estado = ESTADO_BRILLO;
                messageSetStatus(ACTIVADO);
            }

            else if (ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                id[posicion_id] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_id -= 1;
                id[posicion_id] = 0;
                if (!posicion_id) {
                	ha_hecho_click = NO;
                }
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
        
        case EVENTO_CLICK_3:

            if( ha_hecho_click == SI )
            {
                reset_all();
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
            

        case EVENTO_TARJETA:
            //id_buffer = getIdTarjeta();
            //hacer for
            proximo_estado = ESTADO_PASS;
            messageSetStatus(ACTIVADO);
            // show_input(&id[0], posicion_id + 1, posicion_id);
            // posicion_id = 0;
            break;

        case EVENTO_MSG:
            show_message(&msg[0], 4);
            break;
        
        default:
            break;
    
    
    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_PASS;
    digit_t msg[] = {IDX_P, IDX_A, IDX_S, IDX_S};

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_pass < MAX_UNIT_PASS)
            {
                pass[posicion_pass] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] > 9)
                {
                    pass[posicion_pass] = 0;
                }

                show_pass(&pass[0], posicion_pass + 1);
            }
            break;

        case EVENTO_IZQ:
            if(posicion_pass < MAX_UNIT_PASS)
            {
                pass[posicion_pass] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] > 9)
                {
                    pass[posicion_pass] = 9;
                }

                show_pass(&pass[0], posicion_pass + 1);
            }
            break;

      
        case EVENTO_CLICK:
            
            ha_hecho_click = SI;
            {
                posicion_pass += 1;

                if(posicion_pass >= MAX_UNIT_PASS)
                {
                    //posicion_pass = 0;
                    proximo_estado = ESTADO_VERIFICAR;
                    user_is_ready = READY;
                    messageSetStatus(ACTIVADO);
                }
            }

            show_pass(&pass[0], posicion_pass + 1);

            break;

        case EVENTO_CLICK_2:


            if(ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                id[posicion_pass] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_pass -= 1;
                id[posicion_pass] = 0;

                show_pass(&pass[0], posicion_pass + 1);
            }

            break;

        case EVENTO_CLICK_3:

			reset_all();
			proximo_estado = ESTADO_INIT;

            break;

        case EVENTO_CLICK_LONG:
        	if(posicion_pass >= MIN_UNIT_PASS - 1)
			{
				proximo_estado = ESTADO_VERIFICAR;
				user_is_ready = READY;
	            messageSetStatus(ACTIVADO);
			} else {
				ha_hecho_click = SI;
				posicion_pass += 1;
				show_pass(&pass[0], posicion_pass + 1);
			}
        	break;
    
        case EVENTO_MSG:
            show_message(&msg[0], 4);
            break;

        default:
            break;
    
    }

    return proximo_estado;

}	

static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_BRILLO;
    digit_t msg[] = {IDX_b, IDX_r, IDX_I, IDX_g, IDX_h, IDX_t, IDX_n, IDX_e, IDX_S, IDX_S};

    // Mostrar un caracter
    //showPasswordDigit(0, 0, MAX_UNIT_PASS);

    switch(evento)
    {
        case EVENTO_DER:

            upBrightness();
            show_brightness();
            break;

        case EVENTO_IZQ:
            
            downBrightness();
            show_brightness();
            break;

        case EVENTO_CLICK:
        case EVENTO_CLICK_LONG:
            
            proximo_estado = ultimo_estado;
            messageSetStatus(ACTIVADO);
            break;

        case EVENTO_MSG:
            show_message(&msg[0], 10);    
            break;
    
        default:
            break;
    }

	return proximo_estado;
}

static estadosDelMenu_t verificar_estado (void)
{
    estadosDelMenu_t proximo_estado = ESTADO_ID;
    set_led(LED1);
    
    // traducir de arreglo a string de numeros
    uint8_t id_char[MAX_UNIT_ID];
    for(int i = 0 ; i < MAX_UNIT_ID ; i++)
    {
    	id_char[i] = (char)(id[i]);
    }

    uint8_t pass_char[posicion_pass + 1];
        for(int i = 0 ; i < posicion_pass + 1; i++)
        {
        	pass_char[i] = (char)(pass[i]);
        }

    if( checkUser(id_char, pass_char, posicion_pass) )
    {
    	digit_t msg[] = {IDX_O, IDX_P, IDX_e, IDX_n};
		show_message(&msg[0], 4);
		clear_led(LED1);
		clear_led(LED2);
		set_led(LED3);
		timerDelay(TIMER_MS2TICKS(5000));
		// poner timer
    } 
    else
    {
    	digit_t msg[] = {IDX_C, IDX_L, IDX_O, IDX_S, IDX_E, IDX_d};
		show_message(&msg[0], 6);
		clear_led(LED1);
		clear_led(LED3);
		set_led(LED2);
		timerDelay(TIMER_MS2TICKS(5000));
    }
    
    reset_all();
    //showMessage(id, MAX_UNIT_ID);

    return proximo_estado;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void reset_all (void)
{
    // RESETEO ID
    for (int i = 0; i < MAX_UNIT_ID; i++)
    {
        id[i] = 0;
    }   
    posicion_id = 0;

    // RESETEO PASSWORD
    for (int i = 0; i < MAX_UNIT_PASS; i++)
    {
        pass[i] = 0;
    }   
    posicion_pass = 0;

    // RESETEO ESTADOS
    estado = ESTADO_INIT;
    ultimo_estado = ESTADO_INIT;

    // RESETEO INTERFAZ
    setClearMode();
    //poner timer
    //clear_led(TODOS);
    ha_hecho_click = NO;

    messageSetStatus(ACTIVADO);

}

static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos){

    bool blink[] = {false, false, false, false};
    if (pos < DISPLAY_LEN) {
        blink[pos] = true;
    }
    else {
        blink[DISPLAY_LEN-1] = true;
    }

    setBlinkingDigits(&blink[0]);
    showLastDigits(true);
    loadBuffer(input_ptr, input_len);
    setBlinkMode();
}

static void show_message(digit_t *msg_ptr, uint8_t msg_len){

    loadBuffer(msg_ptr, msg_len);
    setScrollMode();
} 

static void show_enter(digit_t *input_ptr, uint8_t input_len){

	loadBuffer(input_ptr, input_len);
    showLastDigits(true);
    setStaticMode();
}

static void show_pass(digit_t *pass_ptr, uint8_t pass_len){

    digit_t pass[] = { IDX_DASH, IDX_DASH, IDX_DASH, IDX_DASH, IDX_DASH };
    pass[pass_len - 1] = *(pass_ptr + pass_len - 1);
    show_input(&pass[0], pass_len, pass_len - 1);

}

static void show_brightness(){
    switch (getBrightnessState()) {
        digit_t bmsg[BRIGHTNESS_HIGH + 1];
    case BRIGHTNESS_LOW:
        bmsg[0] = IDX_b;
        bmsg[1] = BRIGHTNESS_LOW + 1;
        show_message(&bmsg[0], 2);
        clear_led(LED1);
        clear_led(LED2);
        set_led(LED3);
        break;
    
    case BRIGHTNESS_MEDIUM:
        bmsg[0] = IDX_b;
        bmsg[1] = IDX_CLEAR;
        bmsg[2] = BRIGHTNESS_MEDIUM + 1;
        show_message(&bmsg[0], 3);
        clear_led(LED1);
        set_led(LED2);
        set_led(LED3);
        break;

    case BRIGHTNESS_HIGH:
        bmsg[0] = IDX_b;
        bmsg[1] = IDX_CLEAR;
        bmsg[2] = IDX_CLEAR;
        bmsg[3] = BRIGHTNESS_HIGH + 1;
        show_message(&bmsg[0], 4);
        set_led(LED1);
        set_led(LED2);
        set_led(LED3);
        break;

    default:
        break;
    }
}

/*******************************************************************************
 ******************************************************************************/

