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
#define PIN_CR_DATA      PORTNUM2PIN(PB,11)        //Card reader data entry, LS bit comes out first
#define PIN_CR_CLOCK     PORTNUM2PIN(PC,11)      //Card reader clock entry, data changes on positive edge
#define PIN_CR_ENABLE    PORTNUM2PIN(PC,10)     //Card reader enable, low while card is sliding

//Characters from the card
#define SS ';'
#define FS '='
#define ES '?'

//Maximum data size
#define MAX_DATA 200

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static bool init;   //True after call to initCardReader()

static ERROR_TYPE error_type;

typedef struct{
    uint8_t parity      :1;
	uint8_t data        :4;
	uint8_t loaded_bits :3;
} card_char;
static card_char current_char;

static char Track[40]; 
static uint8_t track_index;

static uint8_t stored_ID[] = {0, 0, 0, 0, 0, 0, 0, 0};

static uint8_t data[200];
static uint8_t index;

static bool data_pin;

static bool data_was_stored;
static bool volatile enable_interrupt;
static bool SS_arrived;

/*******************************************************************************
 *         FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void irq_enable (void);
static void irq_clk_falling_edge (void);
static void readCard (void);
static void pin2data (bool pin, uint8_t index);
static void orderData (uint8_t pin);
static bool validateParity (void);
static void validate_LRC(uint8_t new_char);
static void add2track (uint8_t new_char);
static void add2ID (uint8_t index, uint8_t new_char);
void printall(void);


void write_data (bool pin);
void print_data (void);
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initCardReader(void){
    if(!init){  //to avoid more than one call at a time to the Card Reader program
        init = true;
        error_type = NO_ERROR;
        current_char.parity = 0b0;
		current_char.data = 0x0;
		current_char.loaded_bits = 0;
        track_index = 0; 
        index = 0;

        gpioMode(PIN_CR_DATA, INPUT);
        gpioMode(PIN_CR_CLOCK, INPUT);
        gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, irq_clk_falling_edge);
        gpioMode(PIN_CR_ENABLE, INPUT);
        gpioIRQ(PIN_CR_ENABLE, GPIO_IRQ_MODE_BOTH_EDGES, irq_enable);

        NVIC_EnableIRQ(PORTC_IRQn);

        data_was_stored = false;
        enable_interrupt = false;
        SS_arrived = false;

    }
}

void resetReader (void){
    init = true;
    error_type = NO_ERROR;
    data_pin = false;
    index = 0;

    data_was_stored = false;
    enable_interrupt = false;
    SS_arrived = false;

    current_char.parity = 0b0;
	current_char.data = 0x0;
	current_char.loaded_bits = 0;

    track_index = 0;

    uint8_t i;
    for (i = 0; i<8; i++){
		stored_ID[i] = 0;
	}
    for (i = 0; i<40; i++){
		Track[i] = '0';
	}
    for (i = 0; i<200; i++){
        data[i] = 0;
    }

}

bool CardReaderIsReady (void){
	 return data_was_stored;
}

ERROR_TYPE getError (void){
	return error_type;
}

uint8_t* getData (void){
    if (getError() == NO_ERROR)
        return &data[0]; 
    else
        return NULL;
}

uint8_t* processData (void){
    track_index = 0;
    current_char.parity = 0b0;
	current_char.data = 0x0;
	current_char.loaded_bits = 0;
    uint8_t data_index;
    for (data_index = 0; data_index < MAX_DATA; data_index++){      //the 200 bits of the array
        orderData(data[data_index]);
    }
    return &stored_ID[0];
}


/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void irq_enable () {
    enable_interrupt = !gpioRead(PIN_CR_ENABLE);        
}
static void irq_clk_falling_edge () {	
    if (enable_interrupt && (!data_was_stored)){
        readCard();   
    }
}

static void readCard (void){

    data_pin = !gpioRead(PIN_CR_DATA);
    if(index<MAX_DATA){
        pin2data(data_pin, index);          //thus, it proceeds to read incomming data
        
    }
    else if (index == MAX_DATA){
        data_was_stored = true;
    }
    else{
        //do nothing
    }
}

static void pin2data (bool pin, uint8_t index){
    if(!SS_arrived){
        if (pin == 1){
        	SS_arrived = true;
            write_data(pin);       
        }
    }
    else{       
        write_data(pin);
    }
}

void write_data (bool pin){         
    data[index] = pin;
    index++;
}


static void orderData (uint8_t pin){
    if (current_char.loaded_bits < 4){
            if (pin == 1)
                current_char.data |= 0b1<<current_char.loaded_bits;
            else
                current_char.data |= 0b0<<current_char.loaded_bits;
            current_char.loaded_bits++;
        }
        else if (current_char.loaded_bits >= 4){
    	    current_char.parity |= pin;
             
    	    if (track_index <= 38){
                if (validateParity()){
                    uint8_t new_char = (current_char.data & 0x0F) | '0';
                    add2track(new_char);
                }
                else{
                    error_type = PARITY_ERROR;
                }
            }
            else{                                                       //LRC doesn't check parity!
                uint8_t new_char = (current_char.data & 0x0F) | '0';
                add2track(new_char);
            }
            current_char.data = 0x0;
            current_char.loaded_bits = 0;
            current_char.parity = 0;       
        }
        else{
    	//do nothing
        }
}

static bool validateParity (void){
    uint8_t counter = 0;
    uint8_t zero;                                   //how many zeros does it have?
    uint8_t i;
    for (i = 0; i<4; i++){
        zero = (current_char.data & (1<<i)) & 0x0F;
        if (zero == 0x00)
            counter++;
    }                                           
    if ((!current_char.parity) == (counter%2))
        return true;
    else
        return false;
}

static void validate_LRC(uint8_t new_char){
	uint8_t LRC = current_char.data;
    uint8_t xor_char = 0;
    uint8_t i;
	for (i = 0; i<track_index;i++)
		xor_char ^= Track[i];
	if((uint8_t)(LRC & 0x0F) == (uint8_t)(xor_char & 0x0F))
		Track[39] = new_char;
	else{
		error_type = BAD_LRC;
	}
}


static void add2track (uint8_t new_char){
    if (track_index == 0){                      //SS
        if (new_char == SS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            Track[track_index] = new_char;
            track_index++;
            error_type = SS_EXPECTED;
        }
    }  
    else if (track_index <= 16){                //PAN
        if (new_char >= '0' && new_char <= '9'){
            Track[track_index] = new_char;
            add2ID(track_index, new_char);
            track_index++;
        }
        else{
            Track[track_index] = new_char;
            add2ID(track_index, new_char);
            track_index++;
            error_type = UNEXPECTED_CHARACTER;
        }        
    }
    else if (track_index == 17){                //FS
        if (new_char == FS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            Track[track_index] = new_char;
            track_index++;
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
            Track[track_index] = new_char;
            track_index++;
            error_type = ES_EXPECTED;
        }
    }
    else{                                       //LRC
        validate_LRC(new_char);
        Track[track_index] = new_char;
        track_index++;   
    }
   
}

static void add2ID (uint8_t index, uint8_t new_char){
    if (index > 8){                                 //takes the last 8 digits of the PAN
        uint8_t converted_char = (new_char - '0');   //Converts from char to uint8_t the current number
        stored_ID[index-9] = converted_char;
    }
}


/*******************************************************************************
 ******************************************************************************/

void printall(void){
    uint8_t i;
    printf("\n ID: ");
    for (i=0; i<8; i++){
        printf("%d", stored_ID[i]);
    }
    printf("\n Track: ");
    for (i=0; i<40; i++){
        printf("%c", Track[i]);
    }
    printf("Error type: %d", getError());
    /*printf("\n Data: ");
    for (i=0; i<200; i++){
        printf("%d", data[i]);
    }*/
}


/*******************************************************************************
 ******************************************************************************/

