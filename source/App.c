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
#include "display.h"
#include "leds.h"
#include "encoder.h"
#include "card_reader.h"
#include "button.h"
#include "user.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//----------- ESTADOS DEL MENU -----------
typedef enum{
    ESTADO_ID,
    ESTADO_PASS,
    ESTADO_BRILLO,
    ESTADO_VERIFICAR
}estadosDelMenu_t;
//----------------------------------------

//----------- EVENTOS DEL MENU -----------
typedef enum{
    EVENTO_NONE,
    EVENTO_IZQ,
    EVENTO_DER,
    EVENTO_CLICK,
    EVENTO_CLICK_2,
    EVENTO_CLICK_3,
    EVENTO_TARJETA
}eventosDelMenu_t;
//----------------------------------------

//----------- COLORES DEL LED ------------
typedef enum{
    LED1,
    LED2,
    LED3
}colored_led_t;
//-----------------------------------------



#define MAX_UNIT_ID         8
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

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static estadosDelMenu_t modificar_id(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento);
static estadosDelMenu_t verificar_estado (void);
static void reset_all (void);
static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos);
static void show_message(digit_t *msg_ptr, uint8_t msg_len);
static void show_pass(digit_t *pass_ptr, uint8_t pass_len);
static void show_enter(digit_t *input_ptr, uint8_t input_len);
// static void showMessage(digit_t *input_ptr, uint8_t input_ptr_len, uint8_t pos);


/*******************************************************************************
 * VARIABLES
 ******************************************************************************/

// ESTADOS
static estadosDelMenu_t estado = ESTADO_ID;
static estadosDelMenu_t ultimo_estado = ESTADO_ID;

// ID 
static uint8_t id[] = {0, 0, 0, 0, 0, 0, 0, 0};
static int posicion_id = 0;
//static int id_buffer[MAX_UNIT_ID];

// CONTRASENA
static uint8_t pass[] = {0, 0, 0, 0};
static int posicion_pass = 0;

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
    initDisplay();
    initEncoder();
    initLeds();
    initButton(); 
    initCardReader();

    set_led(LED1);
    set_led(LED3);

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

    // Si hubo un evento, veo en que estado de mi FSM estoy y le envio el evento
	if(evento != EVENTO_NONE)
    {
		switch(estado){
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

static estadosDelMenu_t modificar_id(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_ID;

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
            
            ha_hecho_click = SI;

            posicion_id += 1;

            if(posicion_id >= MAX_UNIT_ID)
            {
                posicion_id = 0;
                proximo_estado = ESTADO_PASS;
            }

            //show_enter(&id[0], posicion_id);
            show_input(&id[0], posicion_id + 1, posicion_id);

            break;

        case EVENTO_CLICK_2:

            if(ha_hecho_click == NO)
            {
                // Guardo el estado actual para luego retomar desde aca
                ultimo_estado = ESTADO_ID;
                proximo_estado = ESTADO_BRILLO;
            }

            else if (ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                id[posicion_id] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_id -=1;
                id[posicion_id] = 0;
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
        
        case EVENTO_CLICK_3:

            if(ha_hecho_click == SI)
            {
                reset_all();
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
            

        case EVENTO_TARJETA:
            //id_buffer = getIdTarjeta();
            //hacer for
            proximo_estado = ESTADO_PASS;
            show_input(&id[0], posicion_id + 1, posicion_id);
            posicion_id = 0;
            break;
        
        default:
            break;
    
    
    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_PASS;

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_pass <= MAX_UNIT_PASS)
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
            if(posicion_pass <= MAX_UNIT_PASS)
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
            
            ha_hecho_click == SI;
            {
                posicion_pass += 1;

                if(posicion_pass > MAX_UNIT_PASS)
                {
                    posicion_pass = 0;
                    proximo_estado = ESTADO_VERIFICAR;
                    user_is_ready = READY;
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
                posicion_pass -=1;
                id[posicion_pass] = 0;

                show_pass(&pass[0], posicion_pass + 1);
            }

            break;

        case EVENTO_CLICK_3:

            if(ha_hecho_click == SI)
            {
                reset_all();
            }

            break;

        default:
            break;
    
    
    }

    return proximo_estado;

}	

static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_BRILLO;

    // Mostrar un caracter
    //showPasswordDigit(0, 0, MAX_UNIT_PASS);

    switch(evento)
    {
        case EVENTO_DER:

            //bright_up();
            break;

        case EVENTO_IZQ:

            //bright_down();
            break;

        case EVENTO_CLICK:
            
            proximo_estado = ultimo_estado;
            break;
    
        default:
            break;
    }

    // A futuro:
    
    // mostrar 1 caracter con brillo 1
    // mostrar 1 caracter con brillo 2
    // mostrar 1 caracter con brillo 3
    // mostrar 1 caracter con brillo 4

    // evento derecha: +1 en el contador, con el punto
    // evento izquierdas: -1 en el contador, con el punto
    // evento click: seleccionar en el con contador (setear con brillo X)

	return proximo_estado;
}

static estadosDelMenu_t verificar_estado (void)
{
    estadosDelMenu_t proximo_estado = ESTADO_ID;
    set_led(LED1);
    
    // traducir de arreglo a string de numeros

    char id_char[MAX_UNIT_ID];
    for(int i = 0 ; i < MAX_UNIT_ID ; i++)
    {
    	id_char[i] = (char)(id[i]);
    }

    char pass_char[MAX_UNIT_ID];
        for(int i = 0 ; i < MAX_UNIT_PASS ; i++)
        {
        	pass_char[i] = (char)(pass[i]);
        }

    if( checkUser(id_char, pass_char) == USUARIO_VALIDO)
    {
        set_led(LED2);
    } 
    else
    {
        set_led(LED3);
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
    estado = ESTADO_ID;
    ultimo_estado = ESTADO_ID;

    // RESETEO INTERFAZ
    setClearMode();
    //poner timer
    //clear_led(TODOS);
    ha_hecho_click = NO;

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
    printf("%i\n", *(input_ptr + 1));
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

    digit_t pass[] = { IDX_DASH, IDX_DASH, IDX_DASH, IDX_DASH};
    pass[pass_len - 1] = *(pass_ptr + pass_len - 1);
    show_input(&pass[0], pass_len, pass_len - 1);

}

/*******************************************************************************
 ******************************************************************************/

