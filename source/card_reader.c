/***************************************************************************//**
  @file     card_reader.c
  @brief    card reader services
  @authors   Guillermo OM, Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "MK64F12.h"
#include "gpio.h"
#include "card_reader.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
 //Pins from the card reader
#define PIN_CR_DATA    PORTNUM2PIN(PB,2)        //Card reader data entry, LS bit comes out first
#define PIN_CR_CLOCK    PORTNUM2PIN(PB,3)       //Card reader clock entry, data changes on positive edge
#define PIN_CR_ENABLE    PORTNUM2PIN(PB,10)     //Card reader enable, low while card is sliding

//Characters from the card
#define SS ';'
#define FS '='
#define ES '?'

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
typedef enum {NO_ERROR,UNFINISHED_MESSAGE,DATA_AFTER_LRC,SS_EXPECTED,FS_EXPECTED, ES_EXPECTED, PARITY_ERROR, UNEXPECTED_CHARACTER, BAD_LRC, TO_ERROR}ERROR_TYPE;

static ERROR_TYPE error_type;


static bool init;      //True after call to initCardReader()

typedef struct{
    uint8_t parity      :1;
	uint8_t data        :4;
	uint8_t loaded_bits :3;
} card_char;

static card_char current_char;
static char* Track;

static int* ID;
static int* stored_ID;
static int track_index;

static bool clock_received = false;
static bool data_was_stored;
/*******************************************************************************
 *                      GLOBAL FUNCTION PROTOTYPES
 ******************************************************************************/
int getError(void);
bool CardReaderIsReady (void);
int* getID (void);
void readCard (void);
void initCardReader(void);
void resetReader (void);
static void irq_enable (void);
static void irq_clk_falling_edge (void);
static void pin2data (bool pin);
bool validateParity (void);
bool validateChar (uint8_t new_char);
void add2track (uint8_t new_char);
void add2ID (int index, uint8_t new_char);
void validate_LRC(uint8_t new_char);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
int getError (void){
	return error_type;
}

bool CardReaderIsReady (void){
	if (error_type == NO_ERROR)
	        return data_was_stored;  //in case data was stored, but an unexpected error happened
	    else
	        return false;
}

int* getID (void){
    if (error_type == NO_ERROR)
        return stored_ID;           //In case the function is called and a card is being slided
    else
        return NULL;
}


void readCard (void){
	while((gpioRead(PIN_CR_ENABLE) == 0) && (error_type == NO_ERROR)){ //card is sliding and no error was detected

		if (clock_received){                    //was a falling edge was detected?
			clock_received = false;             //flag down to expect next edge
			pin2data(!gpioRead(PIN_CR_DATA));   //thus, it proceeds to read incomming data
		}

		}
		if (track_index != 40){     //It finished reading & there are less characters than expected
			error_type = UNFINISHED_MESSAGE;
		}

		for (int i = 0; i<=40; i++){
			stored_ID[i] = ID[i];
		}
		data_was_stored = true;     //finished reading, new ID stored
}

void initCardReader(void){
    if(!init){ //to avoid more than one call at a time to this FSM
        init = true;
        error_type = NO_ERROR;
        current_char.parity = 0b0;
		current_char.data = 0x0;
		current_char.loaded_bits = 0;
        track_index = 0; 

        gpioMode(PIN_CR_DATA, INPUT);
        gpioMode(PIN_CR_CLOCK, INPUT);
        gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, irq_clk_falling_edge);
        gpioMode(PIN_CR_ENABLE, INPUT);
        gpioIRQ(PIN_CR_ENABLE, GPIO_IRQ_MODE_FALLING_EDGE, irq_enable);

        clock_received = false;
        data_was_stored = false;
    }
}

void resetReader (void){
    init = true;
    error_type = NO_ERROR;
    current_char.parity = 0b0;
	current_char.data = 0x0;
	current_char.loaded_bits = 0;
    track_index = 0;
    clock_received = false;
    data_was_stored = false;
}

/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void irq_enable (void) {   
    readCard();						//stores data until main asks for it
}
static void irq_clk_falling_edge (void) {   
    clock_received = true;         //for the FSM to ask if it's expecting data
}

static void pin2data (bool pin){
    if (current_char.loaded_bits <= 4){
        current_char.data |= pin<<current_char.loaded_bits;
        current_char.loaded_bits++;
    }
    else if (current_char.loaded_bits > 4){
    	current_char.parity |= pin;
    	        if (validateParity()){
    	            uint8_t validated_char = (current_char.data & 0x0F) | '0';
    	            if (validateChar(validated_char))
    	                add2track(validated_char);
    	            current_char.parity = 0b0;
    	            current_char.data = 0x0;
    	            current_char.loaded_bits = 0;
    	        }
    	        else{
    	            error_type = PARITY_ERROR;
    	        }
    }
    else{
    	//do nothing
    }
}

bool validateParity (void){
    int counter = 0;
    bool one;
    for (int i = 0; i<4; i++){
        one = current_char.data&(1<<i);
        if (one)
            counter++;
    }
    if (current_char.parity == !(one%2))
        return true;
    else
        return false;
}

bool validateChar (uint8_t new_char){
    if ((track_index <= 20) || (track_index == 38))
    {
        if (new_char >= '0' && new_char <= '9')
            return true;
        else if (new_char == SS)
            return true;
        else if (new_char == FS)
            return true;
        else if (new_char == ES)
            return true;
        else{
            error_type = UNEXPECTED_CHARACTER;
            return false;
        }
    }
    else
        return true;        //It doesn't check AD, DD or LRC characters as they may not be those listed above
}

void add2track (uint8_t new_char){
    if (track_index == 0){                      //SS
        if (new_char == SS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            error_type = SS_EXPECTED;
        }
    }  
    else if (track_index <= 19){                //PAN
        if (new_char >= '0' && new_char <= '9'){
            Track[track_index] = new_char;
            add2ID(track_index, new_char);
            track_index++;
        }
        else{
            error_type = UNEXPECTED_CHARACTER;
        }        
    }
    else if (track_index == 20){                //FS
        if (new_char == FS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            error_type = FS_EXPECTED;
        }
    }
    else if (track_index <= 37){                //AD & DD
        Track[track_index] = new_char;
        track_index++;                
    }
    else if (track_index == 38){                //ES
        if (new_char == ES){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            error_type = ES_EXPECTED;
        }
    }
    else if (track_index == 39){                //LRC
        validate_LRC(new_char);
        Track[track_index] = new_char;
        track_index++;   
    }
    else{
        error_type = DATA_AFTER_LRC;            //There's no other data expected from here
    }
}

void add2ID (int index, uint8_t new_char){
    if (index > 11){                            //takes the last 8 digits of the PAN
        int converted_char = (new_char - '0');   //Converts from char to int the current number
        ID[index-12] = converted_char;
    }
}

void validate_LRC(uint8_t new_char){
	uint8_t LRC = current_char.data;
    uint8_t xor_char = 0;
	for (int i = 0; i<track_index;i++)
		xor_char ^= Track[i];
	if((uint8_t)(LRC & 0x0F) == (uint8_t)(xor_char & 0x0F))
		Track[39] = new_char;
	else{
		error_type = BAD_LRC;
	}
}

/*******************************************************************************
 ******************************************************************************/
