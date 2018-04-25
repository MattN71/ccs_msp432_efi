#include "Peripheral_HAL/timer32_api.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define TIMER_A0_FREQ (48000000 / 256)


void init_timer32() {
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
    MAP_Timer32_setCount(TIMER32_0_BASE, UINT32_MAX);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);
}

void blockDelayMS(uint32_t milliseconds) {
    uint32_t ticks = (milliseconds * 1875) / 10;
    uint32_t start = MAP_Timer32_getValue(TIMER32_0_BASE);

    uint32_t present = start;
    while( start - present < ticks ){
        present = MAP_Timer32_getValue(TIMER32_0_BASE);
    }
}
