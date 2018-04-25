/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include "common.h"
#include "Peripheral_HAL/uart_api.h"
#include "Peripheral_HAL/timer32_api.h"


//test commit #2

/* Statics */
static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;
extern int counter;
extern uint8_t transmitBuf[64];

int main(void)
{
    //Initialize clock and power system
    clock_power_init();
    init_timer32();

    /*
     *  Getting all of the frequency values of the CLK sources using the
     * convenience functions */
    aclk = MAP_CS_getACLK();
    mclk = MAP_CS_getMCLK();
    smclk = MAP_CS_getSMCLK();
    hsmclk = MAP_CS_getHSMCLK();
    bclk = MAP_CS_getBCLK();

    UARTsetBaud(9600); //Init UART to 57600 baud


    char message[] = "Test \n\r";
    uint32_t len = sizeof(message)/sizeof(message[0]);
    while (1)
    {
        blockDelayMS(500);
        forcePrint((uint8_t *)message, len);
    }
}
