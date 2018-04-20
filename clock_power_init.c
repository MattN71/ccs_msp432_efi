/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/* Standard Includes */
#include <stdint.h>
#include "common.h"


void clock_power_init(void) {
        /* Halting WDT */
       MAP_WDT_A_holdTimer();

       // Configuring pins for peripheral/crystal usage
       MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION); //HFXT
       MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ, GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION); //LFXT

       //Set crystal values from launchpad
       MAP_CS_setExternalClockSourceFrequency(32000, 48000000);

       /* Starting HFXT in non-bypass mode without a timeout. Before we start
       * we have to change VCORE to 1 to support the 48MHz frequency
       * Active Mode, VCORE 1, DCDC Power Supply */
       MAP_PCM_setPowerState(PCM_AM_DCDC_VCORE1);

       MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
       MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
       MAP_CS_startHFXT(false);
       MAP_CS_startLFXT(CS_LFXT_DRIVE3);

       /* Initializing the clock source as follows:
        *      MCLK = HFXT/1 = 48MHz
        *      SMCLK = HFXT/1 = 48MHz
        *      HSMCLK = HFXT/4 = 12Mhz
        *      ACLK = LFXT/1 = 32kHz
        *      BCLK  = LFXT/1 = 32kHz
        */


       MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
       MAP_CS_initClockSignal(CS_HSMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
       MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);

       MAP_CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
       MAP_CS_initClockSignal(CS_BCLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

}
