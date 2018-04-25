#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "outputs.h"

// To implement more cylinders later
#define NUM_CYLINDERS 1

#define COIL_1_PORT GPIO_PORT_P8
#define COIL_1_PORTNUM P8
#define COIL_1_PIN GPIO_PIN6

#define FUEL_1_PORT GPIO_PORT_P8
#define FUEL_1_PORTNUM P8
#define FUEL_1_PIN GPIO_PIN7


//Function pointers to be used
void (*const coilOneOn)(void) = coilOneOn_f;
void (*const coilOneOff)(void) = coilOneOff_f;
void (*const injectorOneOn)(void) = injectorOneOn_f;
void (*const injectorOneOff)(void) = injectorOneOff_f;


void init_outputs(void) {
    GPIO_setAsOutputPin(COIL_1_PORT, COIL_1_PIN);
    GPIO_setAsOutputPin(FUEL_1_PORT, FUEL_1_PIN);
}


void coilOneOn_f(void) {
    COIL_1_PORTNUM->OUT |= COIL_1_PIN;
}

void coilOneOff_f(void) {
    COIL_1_PORTNUM->OUT &= ~COIL_1_PIN;
}

void injectorOneOn_f(void) {
    FUEL_1_PORTNUM->OUT |= FUEL_1_PIN;
}

void injectorOneOff_f(void) {
    FUEL_1_PORTNUM->OUT &= ~FUEL_1_PIN;
}


