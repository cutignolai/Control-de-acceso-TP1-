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
    EVENTO_CLICK_MANTENIDO,
    EVENTO_TARJETA
}eventosDelMenu_t;
//----------------------------------------

//----------- COLORES DEL LED ------------
typedef enum{
    ROJO,
    AMARILLO,
    VERDE
}colored_led_t;
//----------------------------------------


#define MAX_UNIT_ID         5
#define MAX_UNIT_PASS       5

#define READY               true
#define NOT_READY           !READY'

#define USUARIO_VALIDO      true
#define USUARIO_INVALIDO    !USUARIO_VALIDO

/*******************************************************************************
 * ENUMS AND STRUCTURES
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES
 ******************************************************************************/

static estadosDelMenu_t estado = ESTADO_ID;
static estadosDelMenu_t last_estado = ESTADO_ID;

static int id[MAX_UNIT_ID] = [0, 0, 0, 0];
static int pass[MAX_UNIT_PASS] = [0, 0, 0, 0];
static int posicion_id = 0;
static int posicion_pass = 0;

static bool user_is_ready = false;




/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    // initDisplay();
    //initEncoder();
    // initLEDs();
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
		evento = encoderGetEvent();	// Es valido igualarlo ya que en el enum se definen los mismos índices
	}

    // Si hubo un evento, veo en que estado de mi FSM estoy y le envio el evento
	if(event0 != EVENTO_NONE){
		switch(estado){
		case ESTADO_ID:
			estado = modificar_id(event);
			break;
		case ESTADO_PASS:
			estado = modificar_pass(event);
			break;
		case ESTADO_BRILLO:
			estado = modificar_brillo(event);
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
	menuState_t proximo_state = ESTADO_ID;

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion <= 3)
            {
                num[posicion] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (num[posicion] > 9)
                {
                    num[posicion] = 0;
                }
            }
            break;

        case EVENTO_IZQ:
            if(posicion <= 3)
            {
                num[posicion] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (num[posicion] < 0)
                {
                    num[posicion] = 9;
                }
            }
            break;

        case EVENTO_CLICK:
            posicion += 1;

            if(posicion > 3)
            {
                posicion = 0;
                proximo_estado = ESTADO_PASS;
            }
            break;
    
        case EVENTO_CLICK_2:

            if(user_is_ready == !READY)
            {
                // Guardo el estado actual para luego retomar desde aca
                last_estado = ESTADO_ID;
                proximo_estado = ESTADO_BRILLO;
            }

        case EVENTO_CLICK_3:

            // Dejo en 0 el digito en donde estaba
            num[posicion] = 0;

            // Me ubico en el ultimo digito ingresado y lo pongo en 0
            posicion -=1;
            num[posicion] = 0;


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
            if(posicion_pass <= 3)
            {
                pass[posicion_pass] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] > 9)
                {
                    pass[posicion_pass] = 0;
                }
            }
            break;

        case EVENTO_IZQ:
            if(posicion_pass <= 3)
            {
                pass[posicion_pass] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] < 0)
                {
                    pass[posicion_pass] = 9;
                }
            }
            break;

        case EVENTO_CLICK:
            posicion_pass += 1;

            if(posicion_pass > 3)
            {
                posicion_pass = 0;
                proximo_estado = ESTADO_VERIFICAR;
                user_is_ready = READY
            }
            break;
    
        case EVENTO_CLICK_2:
            if(user_is_ready == !READY)
            {
                last_estado = ESTADO_PASS;
                proximo_estado = ESTADO_BRILLO;
            }
            break;
        
        case EVENTO_CLICK_3:

            // Dejo en 0 el digito en donde estaba
            num[posicion] = 0;

            // Me ubico en el ultimo digito ingresado y lo pongo en 0
            posicion -=1;
            num[posicion] = 0;


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

            subir_brillo();
            break;

        case EVENTO_IZQ:

            bajar_brillo();
            break;

        case EVENTO_CLICK:
            
            estado = last_estado;
            break;
    
        default:
            break;
    
    
    }

	return proximo_estado;
}


static estadosDelMenu_t verificar_estado (void)
{
    // Desactivar interrupciones
    encender_led(AMARILLO);
    // traducir de arreglo a string de numeros
    if( checkUser(id, pass) == USUARIO_VALIDO)
    {
        encender_led(VERDE);
    } 
    else
    {
        encender_led(ROJO);
    }
    
    reset_all();
}

void reset_all (void)
{
    id = [0,0,0,0,0];
    pass = [0,0,0,0,0];
    posicion_id = 0;
    posicion_pass = 0;
    estado = ESTADO_ID;
}

/*******************************************************************************
 ******************************************************************************/
