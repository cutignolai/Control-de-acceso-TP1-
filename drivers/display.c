/***************************************************************************//**
  @file     display.h
  @brief    Display Driver
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "gpio.h"
#include "./source/board.h"
#include "display.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define DISPLAY_LEN     4
#define SEGMENTS        8
#define ON      1
#define OFF     0
#define SEG_ON  1
#define SEG_OFF 0
#define MAX_BRIGHTNESS  10
#define BLINK_FREQ      500
#define BUFFER_LEN      8

/******* PINS *******/
#define SEGA    DIO_1
#define SEGB    DIO_3
#define SEGC    DIO_5
#define SEGD    DIO_7
#define SEGE    DIO_9
#define SEGF    DIO_11
#define SEGG    DIO_13
#define SEGDP   DIO_15

#define SEL0    DIO_2
#define SEL1    DIO_4

/*** DIGIT MASKS ***/
#define DIG0    0b00
#define DIG1    0b01
#define DIG2    0b10
#define DIG3    0b11

#define GET_SEL0(x) (x & 1)
#define GET_SEL1(x) ((x >> 1) & 1)

/********** SEGMENT MASKS **********/
#define MASK_SEGA   0x01 //0b00000001
#define MASK_SEGB   0x02 //0b00000010
#define MASK_SEGC   0x04 //0b00000100
#define MASK_SEGD   0x08 //0b00001000
#define MASK_SEGE   0x10 //0b00010000
#define MASK_SEGF   0x20 //0b00100000
#define MASK_SEGG   0x40 //0b01000000
#define MASK_SEGDP  0x80 //0b10000000

/************ CHAR MASKS ************/
        //     aaaa
        //    f    b     Bits: DP g f e d c b a
        //    f    b
        //     gggg
        //    e    c
        //    e    c
        //     dddd    DP
                          //DPgfedcba
#define NUM_0       0x3F //0b00111111
#define NUM_1       0x06 //0b00000110
#define NUM_2       0x5B //0b01011011
#define NUM_3       0x4F //0b01001111
#define NUM_4       0x66 //0b01100110
#define NUM_5       0x6D //0b01101101
#define NUM_6       0x7D //0b01111101
#define NUM_7       0x07 //0b00000111
#define NUM_8       0x7F //0b01111111
#define NUM_9       0x67 //0b01100111
#define DASH        0x40 //0b01000000
#define LOW_DASH    0x08 //0b00001000
#define POINT       0x80 //0b10000000
#define CLEAR       0x00 //0b00000000

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
// typedef struct {
// 	digit_t     segA    : 1;
//     digit_t     segB    : 1;
//     digit_t     segC    : 1;
//     digit_t     segD    : 1;
//     digit_t     segE    : 1;
//     digit_t     segF    : 1;
//     digit_t     segG    : 1;
//     digit_t     segDP   : 1;
// } segments_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

digit_t char_arr[] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, DASH, LOW_DASH, POINT, CLEAR};
uint8_t seg_arr[] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, SEGDP};
uint8_t digit_arr[] = {DIG0, DIG1, DIG2, DIG3};
digit_t display[] = {0, 0, 0, 0};
digit_t buffer[16];
uint8_t buffer_len;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void set_segment(uint8_t seg, uint8_t set);
void set_display(digit_t arr[]);
void set_digit(digit_t number, uint8_t position);
void set_buffer(digit_t *dig_arr, uint8_t arr_len);
void clear_buffer();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// INIT
void initDisplay(){

}                     

// SETTERS
void clearDisplay(){

    digit_t arr[] = {CLEAR, CLEAR, CLEAR, CLEAR};
    set_display(&arr);
}

void showMessage(uint8_t *message, uint8_t message_len){
    set_buffer(message, message_len);
}

void showPasswordDigit(uint8_t char_idx, uint8_t position){
    digit_t arr[] = {LOW_DASH, LOW_DASH, LOW_DASH, LOW_DASH};
    if( position == 4){ position = 3; }
    arr[position] = char_arr[char_idx];
    set_buffer(&arr[0], DISPLAY_LEN);
}

/* 
--------------------------------------------------------- 
   Se encarga de subir y bajar el brillo
--------------------------------------------------------- 
*/
void bright_up(void);
void bright_down(void);


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void set_display(digit_t* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        set_digit(*(arr + i), i);
    }
}

void set_digit(digit_t number, uint8_t position){

    digit_t character = char_arr[number];
    
    // Select digit
    gpioWrite(SEL0, GET_SEL0(position));
    gpioWrite(SEL1, GET_SEL1(position));

    uint8_t idx;
    for (idx = 0; idx < SEGMENTS; idx++){
        uint8_t set = character & 1;
        set_segment(seg_arr[idx], set);
        character = character >> 1;
    }

}

void set_segment(uint8_t seg, uint8_t set){
    gpioWrite(seg, set);
}

void set_buffer(digit_t *dig_arr, uint8_t arr_len){
    uint8_t i;
    clear_buffer();
    for (i = 0; i < arr_len; i++){
        buffer[i] = *(dig_arr + i);
    }
    buffer_len = arr_len;
}

void clear_buffer(){
    uint8_t i;
    for (i = 0; i < buffer_len; i++){
        buffer[i] = CLEAR;
    }
    buffer_len = 0;
}

/*******************************************************************************
 ******************************************************************************/
