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
#include "timer.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define DISPLAY_LEN     4
#define SEGMENTS        8
#define ON      1
#define OFF     0
#define SEG_ON  1
#define SEG_OFF 0
#define MAX_BRIGHTNESS  4
#define BLINK_T         500
#define SCROLL_T        500
#define BUFFER_MAX_LEN  16
#define MAX_REFRESH_T   30

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

#define N_PINS  10

/**** DIGITS ****/
#define DIG0    0
#define DIG1    1
#define DIG2    2
#define DIG3    3

#define GET_SEL0(x) (x & 1)
#define GET_SEL1(x) ((x >> 1) & 1)

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
typedef enum {
    DISPLAY_CLEAR = 0,
    DISPLAY_SCROLL = 1,
    DISPLAY_BLINK = 2,
    DISPLAY_STATIC = 3
} display_states_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void refresh_display();
void set_segment(uint8_t seg, uint8_t set);
void set_display(digit_t arr[]);
void set_digit(digit_t number, uint8_t position);
void set_buffer(digit_t *dig_arr, uint8_t arr_len);
void clear_buffer();
void blink_digits();
void scroll_buffer();
void display_blink(digit_t* arr);
void clear_display();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static digit_t char_arr[] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, DASH, LOW_DASH, POINT, CLEAR};
static uint8_t seg_arr[] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, SEGDP};

static display_states_t display_state = CLEAR;

static digit_t buffer[BUFFER_MAX_LEN];
static uint8_t buffer_len;         // Cantidad de caracteres seteados del buffer
static uint8_t buffer_idx;         // Caracter a partir del cual mostrar

static uint8_t brightness = 1;

static uint8_t scroll_idx;

static bool show_last = false;

static bool blink = false;
static bool blinking_digits[DISPLAY_LEN] = { false, false, false, false};

// TIMERS
static tim_id_t refresh_timer;
static tim_id_t blink_timer;
static tim_id_t scroll_timer;



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// INIT
void initDisplay(){
    
    // INIT PINS
    pin_t pin_arr[] = {DIO_1, DIO_3, DIO_5, DIO_7, DIO_9, DIO_11, DIO_13, DIO_15, DIO_2, DIO_4};
    uint8_t i;
    for(i = 0; i < N_PINS; i++){
        gpioMode(pin_arr[i], OUTPUT);
    }

    // CREATE TIMERS
    refresh_timer = timerGetId();
    timerCreate(refresh_timer, TIMER_MS2TICKS(MAX_REFRESH_T/brightness), TIM_MODE_PERIODIC, refresh_display);
    blink_timer = timerGetId();
    timerCreate(refresh_timer, TIMER_MS2TICKS(BLINK_T), TIM_MODE_PERIODIC, blink_digits);
    scroll_timer = timerGetId();
    timerCreate(refresh_timer, TIMER_MS2TICKS(BLINK_T), TIM_MODE_PERIODIC, scroll_buffer);

    // INIT 7 SEGMENTS
    clear_buffer();

    // START DISPLAY
    timerActivate(refresh_timer);
}                     

// void showMessage(uint8_t *message, uint8_t message_len){
//     if (display_state != DISPLAY_MESSAGE){
//         timerReset(blink_timer);
//         timerReset(scroll_timer);
//         if (buffer_len > DISPLAY_LEN){
//             timerActivate(scroll_timer);
//         }
//     }
//     if (message_len > BUFFER_MAX_LEN) { message_len = BUFFER_MAX_LEN; }
//     set_buffer(message, message_len);
//     scroll_idx = 0;
//     display_state = DISPLAY_MESSAGE;
// }

void setClearMode(){
    if (display_state != DISPLAY_CLEAR){
        timerReset(blink_timer);
        timerReset(scroll_timer);
    }
    display_state = DISPLAY_CLEAR;
}

void setScrollMode(){
    if (display_state != DISPLAY_SCROLL){
        timerReset(blink_timer);
        timerRestart(scroll_timer);
        scroll_idx = 0;
    }
    display_state = DISPLAY_SCROLL;
}

void setStaticMode(){
    if (display_state != DISPLAY_STATIC){
        timerReset(blink_timer);
        timerReset(scroll_timer);
    }
    display_state = DISPLAY_STATIC;
}

void setBlinkMode(){
    if (display_state != DISPLAY_BLINK){
        timerReset(blink_timer);
        timerReset(scroll_timer);
        blink = false;
        timerActivate(blink_timer);
    }
    display_state = DISPLAY_BLINK;
}

void loadBuffer(digit_t *msg, uint8_t msg_len){
    if (msg_len > BUFFER_MAX_LEN) { msg_len = BUFFER_MAX_LEN; }
    set_buffer(msg, msg_len);
    if (show_last) { auto_set_buffer_index(); }
}

void showLastDigits(bool b){
    show_last = b;
    if (show_last) { autoSetBufferIndex(); }
}

void setBufferIndex(uint8_t idx){
    if (idx < BUFFER_MAX_LEN - DISPLAY_LEN){
        buffer_idx = idx;
    }
}

void setBlinkingDigits(bool* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        blinking_digits[i] = *(arr + i);
    }
}


// void showLastBlinking(uint8_t input_idx){
//     if (display_state != DISPLAY_BLINK){
//         timerReset(blink_timer);
//         timerReset(scroll_timer);
//         if (buffer_len != BUFFER_MAX_LEN){ 
//             buffer[buffer_len] = char_arr[input_idx];
//             buffer_len++; 
//         } else {
//             buffer[buffer_len - 1] = char_arr[input_idx];
//         }
//         blink = false;
//         timerActivate(blink_timer);
//     }
//     else {
//         buffer[buffer_len - 1] = char_arr[input_idx];
//     }
//     display_state = DISPLAY_BLINK;
// }

// NO VA EN CAPA DRIVERS
// void showPasswordDigit(uint8_t char_idx, uint8_t position){
//     if (display_state != DISPLAY_PASS){
//         timerReset(blink_timer);
//         timerReset(scroll_timer);
//         blink = false;
//         timerActivate(blink_timer);
//     }
//     display_state = DISPLAY_PASS;
//     digit_t arr[] = {LOW_DASH, LOW_DASH, LOW_DASH, LOW_DASH};
//     if ( position == DISPLAY_LEN){ position = DISPLAY_LEN - 1; }
//     arr[position] = char_arr[char_idx];
//     set_buffer(&arr[0], DISPLAY_LEN);
//     // init timer
// }

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

// TODO:
// BRILLO
void refresh_display(){

    switch (display_state){

    case DISPLAY_SCROLL:
        if (buffer_len <= DISPLAY_LEN){
            set_display(&buffer[0]);
        } else {
            scroll_message();
        }
        break;

    case DISPLAY_BLINK:
        display_blink(&buffer[buffer_idx]);
        break;

    case DISPLAY_STATIC:
        set_display(&buffer[buffer_idx]);
        break;
    // case DISPLAY_PASS:
    //     display_pass();
    //     break;

    case DISPLAY_CLEAR:
    default:
        clear_display();
        break;
    }
}

void clear_display(){
    clear_buffer();
    buffer_idx = 0;
}

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

void auto_set_buffer_index(){
    if (buffer_len > DISPLAY_LEN){
        buffer_idx = buffer_len - DISPLAY_LEN;
    } else {
        buffer_idx = 0;
    }
}

void scroll_buffer(){
    digit_t* idx = &buffer[0];
    if (scroll_idx > buffer_len){ scroll_idx = 0; }
    if (scroll_idx + DISPLAY_LEN <= buffer_len){
        set_display(idx + scroll_idx);
    } else {
        uint8_t i;
        for (i = scroll_idx; i < buffer_len; i++){
            set_digit(buffer[i], i - scroll_idx);
        }
        set_digit(CLEAR, i - scroll_idx);
        for (i = i + 1; i < scroll_idx + DISPLAY_LEN; i++){
            set_digit(buffer[i - buffer_len - 1], i - scroll_idx);
        }
    }
}

// void display_blink(){
//     digit_t* idx;
//     if (buffer_len < DISPLAY_LEN){
//         idx = &buffer[0];
//     } else {
//         idx = &buffer[buffer_len - DISPLAY_LEN]; 
//     }
//     uint8_t i;
//     for (i = 0; i < DISPLAY_LEN; i++){
//         if ( blink && ( i == buffer_len - 1 || ( buffer_len > DISPLAY_LEN - 1 && i == DISPLAY_LEN - 1))){
//             set_digit(CLEAR, i);
//         } else {
//             set_digit(*(idx + i), i);
//         }
//     }
// }

void display_blink(digit_t* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        if (blink && blinking_digits[i]){
            set_digit(CLEAR, i);
        } else {
            set_digit(*(arr + i), i);
        }
    }
}

// void display_pass(){
//     uint8_t i;
//     for (i = 0; i < DISPLAY_LEN; i++){
//         if ( i == buffer_len - 1 ){
//             if (blink){
//                 set_digit(CLEAR, i);
//             } else {
//                 set_digit(buffer[buffer_len - 1], i);
//             }
//         } else if (i < buffer_len - 1){
//             set_digit(DASH, i);
//         } else {
//             set_digit(CLEAR, i);
//         }
//     }
// }

// callback
void scroll_buffer(){
    scroll_idx++;
}

void blink_digits(){
    blink = !blink;
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
