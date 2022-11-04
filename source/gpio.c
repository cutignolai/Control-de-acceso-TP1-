#include "gpio.h"

static GPIO_Type* gpioPtrs[] = GPIO_BASE_PTRS;
static PORT_Type* portPtrs[] = PORT_BASE_PTRS;
static uint32_t simMasks[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK };
static SIM_Type* sim_ptr = SIM;
static pinIrqFun_t isr_Matrix [PORTS_CNT][PINS_CNT];


/* * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */



__ISR__ PORTD_IRQHandler (void){

	uint8_t i = 0;
	while ( !((PORTD -> ISFR) & (1<<i)) && (i < PINS_CNT)) //atiende primero a la interrupcion que viene de un pin con numero mas bajo
	{
		i++;
	}
/*
    0b1000: Interrupt/DMA request disabled
    0b1001: Interrupt when logic 0
    0b1010: Interrupt on rising-edge
    0b1011: Interrupt on falling-edge
    0b1100: Interrupt on either edge
    0b1101: Interrupt when logic 1
*/
static const uint32_t IRQC_MODES[] = {0b1000,0b1001,0b1010,0b1011,0b1100,0b1101};


static pinIrqFun_t CALLBACKS[ARRAY_SIZE];

static void IRQHandler(int32_t port);

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void gpioMode (pin_t pin, uint8_t mode)
{

    // Verifico que se ingresaron los datos correctos
    if ( pin > PORTNUM2PIN(PE,31) ) return false;

    // Obtengo el puerto y el numero de pin
    uint32_t port = PIN2PORT(pin);
    int num_pin = PIN2NUM(pin);

    //----------------------- Clock Enable ----------------------------
    //                      12.2.12 (pag. 323)
    SIM->SCGC5 |= CLK_CONTROL(port,1);
    //-----------------------------------------------------------------

    //-------------------- PORT configuration -------------------------
    //                      11.5.1 (pag. 288)

	PORT_Type* port_ptr = PORT_PTRS[port];

    // Vacio el puerto:
	port_ptr->PCR[num_pin] = 0x0;                   // Tener cuidado, se puso todo en 0!

    // MUX:
    port_ptr->PCR[num_pin] |= PORT_PCR_MUX(0b001);  // 001 Alternative 1 (GPIO).

    // DSE:
    port_ptr->PCR[num_pin] |= PORT_PCR_DSE(0b1);    // 1 High drive strength is configured on the corresponding pin, if pin is configured as a digital output.

    // SRE:
    // Ver final del codigo

    // PS & PE:
    if ((mode==INPUT_PULLDOWN)||(mode==INPUT_PULLUP))
    {
		port_ptr->PCR[num_pin] |= PORT_PCR_PE(0b1);     // 1 Internal pullup or pulldown resistor is enabled on the corresponding pin, if the pin is configured as a digital input.
        if(mode==INPUT_PULLDOWN)
        {
            port_ptr->PCR[num_pin] |= PORT_PCR_PS(0b0); // 0 Internal pulldown resistor is enabled on the corresponding pin, if the corresponding PE field is set.
        }
        else if(mode==INPUT_PULLUP)
        {
            port_ptr->PCR[num_pin] |= PORT_PCR_PS(0b1); // 1 Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE field is set.
        }
	}
    //-----------------------------------------------------------------

    //----------------------- GPIO configuration ----------------------
    //                        55.2.6 (pag. 1803)
    GPIO_Type* gpio_ptr = GPIO_PTRS[port];
    if(mode == OUTPUT)
    {
        gpio_ptr->PDDR |= 1 << PIN2NUM(pin);
    }
    else if ((mode==INPUT_PULLDOWN)||(mode==INPUT_PULLUP))
    {
        gpio_ptr->PDDR |= 0 << PIN2NUM(pin);
    }
    //-----------------------------------------------------------------

    return true;

	isr_Matrix[PD][i]();
	PORT_ClearInterruptFlag(PORTNUM2PIN(PD,i));
}

__ISR__ PORTA_IRQHandler (void){

	uint8_t i = 0;
	while ( !((PORTA -> ISFR) & (1<<i)) && (i < PINS_CNT)) //atiende primero a la interrupcion que viene de un pin con numero mas bajo
	{
		i++;
	}

	isr_Matrix[PA][i]();
	PORT_ClearInterruptFlag(PORTNUM2PIN(PA,i));
void gpioWrite (pin_t pin, bool value) {
    if ( !(pin > PORTNUM2PIN(PE,31)) ){
		uint32_t new_value =  (uint32_t)(1 << PIN2NUM(pin));
		GPIO_Type* gpio_ptr = GPIO_PTRS[PIN2PORT(pin)];
		if(value) {
			gpio_ptr->PSOR = new_value;
		}
		else {
			gpio_ptr->PCOR = new_value;
		}
    }
}

__ISR__ PORTB_IRQHandler (void){

	uint8_t i = 0;
	while ( !((PORTB -> ISFR) & (1<<i)) && (i < PINS_CNT)) //atiende primero a la interrupcion que viene de un pin con numero mas bajo
	{
		i++;
	}

	isr_Matrix[PB][i]();
	PORT_ClearInterruptFlag(PORTNUM2PIN(PB,i));
}

__ISR__ PORTE_IRQHandler (void){

	uint8_t i = 0;
	while ( !((PORTE -> ISFR) & (1<<i)) && (i < PINS_CNT)) //atiende primero a la interrupcion que viene de un pin con numero mas bajo
	{
		i++;
	}

	isr_Matrix[PE][i]();
	PORT_ClearInterruptFlag(PORTNUM2PIN(PE,i));
void gpioToggle (pin_t pin) {   //no hay mucha magia, es lo mismo que el Write pero sin el if
    if ( !(pin > PORTNUM2PIN(PE,31)) ){
    	GPIO_Type* gpio_ptr = GPIO_PTRS[PIN2PORT(pin)];
    	gpio_ptr->PTOR = (uint32_t)(1 << PIN2NUM(pin));
    }
}


__ISR__ PORTC_IRQHandler (void){

	uint8_t i = 0;
	while ( !((PORTC -> ISFR) & (1<<i)) && (i < PINS_CNT)) //atiende primero a la interrupcion que viene de un pin con numero mas bajo
	{
		i++;
	}

	isr_Matrix[PC][i]();
	PORT_ClearInterruptFlag(PORTNUM2PIN(PC,i));
}

void gpioMode (pin_t pin, uint8_t mode){

	sim_ptr->SCGC5 |= simMasks[PIN2PORT(pin)]; // activo clock gating
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	GPIO_Type *gpio = gpioPtrs[PIN2PORT(pin)];

	uint32_t num = PIN2NUM(pin); // num es el numero de pin

	// connect to gpio (hay un PCR por pin)
	port->PCR[num] = 0x00;
	port->PCR[num] |= PORT_PCR_MUX(1);
	//port->PCR[num] |= PORT_PCR_DSE(1);
	port->PCR[num] |= PORT_PCR_IRQC(0);
	// PCR solo -> uint32_t array[32]
	// PCR[num] -> uint32_t

	switch(mode){
		case INPUT:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			break;
		case OUTPUT:
			gpio->PDDR |= 1<<num;// seteamos el pin como output
			break;
		case INPUT_PULLUP:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] |= HIGH<<0; //PULL SELECT en 1 (PULLUP)
			break;
		case INPUT_PULLDOWN:
			gpio->PDDR &= ~ (1<<num); // seteamos el pin como input
			port->PCR[num] |= HIGH<<1; //PULL ENABLE en 1
			port->PCR[num] &= ~(HIGH); //PULL SELECT en 0 (PULLDOWN)
			break;
	}

}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	if(value == HIGH){
		gpio->PDOR |= (1<<num);
	}else{
		gpio->PDOR &= ~ (1<<num);
	}
}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	if( (gpio->PTOR & (1<<num) ) == 1<<num){ //si esta prendido el toggle
		gpio->PTOR &= ~ (1<<num); //lo apagamos
	}else{
		gpio->PTOR |= (1<<num);
	}
}

/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin){
	uint32_t port_name = PIN2PORT(pin);
	uint32_t num = PIN2NUM(pin);
	GPIO_Type *gpio = gpioPtrs[port_name];
	return ( (1<<num) & gpio->PDIR ) == 1<<num ;
}
/*
void gpioEnableInterrupts(pin_t pin){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	port->PCR[PIN2NUM(pin)] |= (1<<24);
	bool aux = (port->PCR[PIN2NUM(pin)] & (1<<24)) == (1<<24);
}
*/


bool gpioIRQ(pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun){

	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	port->PCR[PIN2NUM(pin)] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[PIN2NUM(pin)] |= PORT_PCR_IRQC(irqMode+8);
	isr_Matrix[PIN2PORT(pin)][PIN2NUM(pin)] = irqFun;

	return true;
}


void PORT_ClearInterruptFlag (pin_t pin){
	PORT_Type *port = portPtrs[PIN2PORT(pin)];
	port->PCR[PIN2NUM(pin)] |= PORT_PCR_ISF_MASK;
}
/*******************************************************************************
 ******************************************************************************/
static void IRQHandler(int32_t port) {

	PORT_Type* port_ptr = PORT_PTRS[port];
	uint32_t ISFR = port_ptr->ISFR;
	for(int pin = 0; pin<PINS_PER_PORT; pin++) {
		if (ISFR>>pin & 0b1) {
			port_ptr->ISFR |= 1<<pin; //w1c
			(*CALLBACKS[PINS_PER_PORT*port + pin])();
		}
	}
}

__ISR__ PORTA_IRQHandler(void) { IRQHandler(PA); }
__ISR__ PORTB_IRQHandler(void) { IRQHandler(PB); }
__ISR__ PORTC_IRQHandler(void) { IRQHandler(PC); }
__ISR__ PORTD_IRQHandler(void) { IRQHandler(PD); }
__ISR__ PORTE_IRQHandler(void) { IRQHandler(PE); }


//----------------------------------------- Comentarios extras -------------------------------------------

/*
    SRE:
    El SRE no se configura porque necesitamos que este en 0. Recordemos que segun Nico, el SRE es como un
    "freno de mano", si esta en Fast(0) reacciona rapido, si esta en Slow(1) tarda mucho en accionarse. Nostotros
    necesitaDAmos que sea rapido
*/
