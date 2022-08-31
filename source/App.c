/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "board.h"
#include "gpio.h"
#include "display.h"
#include "leds.h"
#include "encoder.h"
#include "button.h"


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
    ROJO,
    VERDE,
    AZUL,
    TODOS
}colored_led_t;
//-----------------------------------------



#define MAX_UNIT_ID         8
#define MAX_UNIT_PASS       4

#define READY               true
#define NOT_READY           !READY'

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



/*******************************************************************************
 * VARIABLES
 ******************************************************************************/

// ESTADOS
static estadosDelMenu_t estado = ESTADO_ID;
static estadosDelMenu_t ultimo_estado = ESTADO_ID;

// ID 
static int id[MAX_UNIT_ID] = [0, 0, 0, 0, 0, 0, 0, 0];
static int posicion_id = 0;

// CONTRASENA
static int pass[MAX_UNIT_PASS] = [0, 0, 0, 0];
static int posicion_pass = 0;

// MESSAGE COMPLETE
static bool ha_hecho_click = NO;




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
    initLEDs();
    initButton(); 
    // initCardReader();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	menuEvent_t evento = EVENT_NONE;

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
	if(event0 != EVENTO_NONE){
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
	menuState_t proximo_estado = ESTADO_ID;

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_id <= MAX_UNIT_ID)
            {
                num[posicion_id] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (num[posicion_id] > 9)
                {
                    num[posicion_id] = 0;
                }
                
                setLongString(id, MAX_UNIT_ID);
            }
            break;

        case EVENTO_IZQ:
            if(posicion_id <= MAX_UNIT_ID)
            {
                num[posicion_id] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (num[posicion_id] < 0)
                {
                    num[posicion_id] = 9;
                }

                setLongString(id, MAX_UNIT_ID);
            }
            break;

        case EVENTO_CLICK:
            
            ha_hecho_click = SI

            posicion_id += 1;

            if(posicion_id > MAX_UNIT_ID)
            {
                posicion_id = 0;
                proximo_estado = ESTADO_PASS;
            }

            setLongString(id, MAX_UNIT_ID);

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
                num[posicion_id] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_id -=1;
                num[posicion_id] = 0;
            }

            setLongString(id, MAX_UNIT_ID);



            break;
        
        case EVENTO_CLICK_3:

            if(ha_hecho_click == SI)
            {
                reset_all();
            }

            setLongString(id, MAX_UNIT_ID);

            break;
            


        default:
            break;
    
    
    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento)
{
	menuState_t proximo_estado = ESTADO_PASS;

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

                setShortChar(pass[posicion_pass], posicion_pass, MAX_UNIT_PASS)
            }
            break;

        case EVENTO_IZQ:
            if(posicion_pass <= MAX_UNIT_PASS)
            {
                pass[posicion_pass] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] < 0)
                {
                    pass[posicion_pass] = 9;
                }

                setShortChar(pass[posicion_pass], posicion_pass, MAX_UNIT_PASS)
            }
            break;

      
        case EVENTO_CLICK:
            
            if(ha_hecho_click == SI)
            {
                posicion_pass += 1;

                if(posicion_pass > MAX_UNIT_PASS)
                {
                    posicion_pass = 0;
                    proximo_estado = ESTADO_VERIFICAR;
                    user_is_ready = READY
                }
            }
            

            setShortChar(pass[posicion_pass], posicion_pass, MAX_UNIT_PASS)

        case EVENTO_CLICK_2:


            if(ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                num[posicion_pass] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_pass -=1;
                num[posicion_pass] = 0;

                setShortChar(pass[posicion_pass], posicion_pass, MAX_UNIT_PASS)
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

}	

static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento)
{
	menuState_t proximo_estado = ESTADO_BRILLO;

    switch(evento)
    {
        case EVENTO_DER:

            bright_up();
            break;

        case EVENTO_IZQ:

            bright_down();
            break;

        case EVENTO_CLICK:
            
            proximo_estado = ultimo_estado;
            break;
    
        default:
            break;
    
    
    }

	return proximo_estado;
}


static estadosDelMenu_t verificar_estado (void)
{
    // Desactivar interrupciones
    set_led(AZUL);
    // traducir de arreglo a string de numeros
    if( checkUser(id, pass) == USUARIO_VALIDO)
    {
        set_led(VERDE);
    } 
    else
    {
        set_led(ROJO);
    }
    
    reset_all();
    setLongString(id);
}

void reset_all (void)
{
    // RESETEI ID
    id = [0, 0, 0, 0, 0, 0, 0, 0];
    posicion_id = 0;

    // RESETEO PASSWORD
    pass = [0, 0, 0, 0];
    posicion_pass = 0;

    // RESETEO ESTADOS
    estado = ESTADO_ID;
    ultimo_estado = ESTADO_ID;

    // RESETEO INTERFAZ
    setLongString(id, MAX_UNIT_ID);
    clear_led(TODOS);
    ha_hecho_click = NO;

}

/*******************************************************************************
 ******************************************************************************/
