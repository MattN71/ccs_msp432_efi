#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "Peripheral_HAL/uart_api.h"


static bool writeBaud();
static void InitUART();
static void EUSCIA0_IRQHandler(void);
static void beginSend();

static uint32_t baudRate = 0;

//Buffers for UART
static uint8_t receiveBuf[64];
uint8_t transmitBuf[64];

//Indexes for buffers
struct {
    uint_fast8_t recHead : 6;
    uint_fast8_t recTail : 6;
    uint_fast8_t sendHead : 6;
    uint_fast8_t sendTail : 6;
} bufferPointer = {0,0,0,0};

static enum {initial, idle, active} uartState = initial;

static eUSCI_UART_Config uartConfig = {           // 8E1
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 12MHz
    19,                                           // UCBR   = 19
    8,                                            // UCBRF  = 8
    0xAA,                                         // UCBRS  = 0xAA
    EUSCI_A_UART_EVEN_PARITY,                     // Even Parity
    EUSCI_A_UART_LSB_FIRST,                       // LSB First
    EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
    EUSCI_A_UART_MODE,                            // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
    };

static bool writeBaud() {
    if (MAP_CS_getSMCLK() != 12000000) {
        return false; //Not supported
    }
    switch (baudRate) { //Update uart config when changing baud rate
    case 9600:
        uartConfig.clockPrescalar = 78;
        uartConfig.firstModReg = 2;
        uartConfig.secondModReg = 0;
        break;
    case 57600:
        uartConfig.clockPrescalar = 13;
        uartConfig.firstModReg = 0;
        uartConfig.secondModReg = 37; //37 or 0?
        break;
    case 115200:
        uartConfig.clockPrescalar = 6;
        uartConfig.firstModReg = 8;
        uartConfig.secondModReg = 32;
        break;
    default:
        return false; //Not supported
    }
    InitUART();
    return true;
}

static void InitUART() {
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); //RX, TX pins
    MAP_UART_registerInterrupt(EUSCI_A0_BASE, &EUSCIA0_IRQHandler);
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_INTERRUPT);
    //Other available interrupts: EUSCI_A_UART_STARTBIT_INTERRUPT, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT
}

uint32_t UARTgetBaud() {
    return baudRate;
}

bool UARTsetBaud(uint32_t baud) {
    uint32_t tempBaud = baudRate; //Save previous baudRate setting
    baudRate = baud; //Update baudRate
    if ( writeBaud() ) { //Try to write to eUSCI
        return true; //return true if successful
    } else {
        baudRate = tempBaud; //Restore old value and return false if not
        return false;
    }
}


static void EUSCIA0_IRQHandler(void) {
    uint16_t iv = EUSCI_A0->IV; //Read automatically clear highest priority interrupt
    switch (iv) {
        case 0x00: // Vector 0: No interrupts
            break;
        case 0x02: // Vector 2: UCRXIFG / RX buffer full
            break;
        case 0x04: // Vector 4: UCTXIFG / Transmit buffer empty
            bufferPointer.sendHead++; //Increment head once send is successful
            if ( SIX_BIT(bufferPointer.sendTail) != SIX_BIT(bufferPointer.sendHead)) {
                EUSCI_A0->TXBUF = transmitBuf[SIX_BIT(bufferPointer.sendHead)]; //Load first/next byte
                MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
                uartState = active;
            } else {
                uartState = idle;
            }
            break;
   /*   case 0x06: // Vector 6: UCSTTIFG / Start bit received
            break;
        case 0x08: // Vector 8: UCTXCPTIFG / Transmit complete
            break;     */
        default:
            break;
    }
}


/* Attempts to add char to send buffer. Returns true if successful, returns false if buffer is full */
bool print(uint8_t data) {
    if (SIX_BIT( SIX_BIT(bufferPointer.sendHead) - SIX_BIT(bufferPointer.sendTail) ) != 1) {
        transmitBuf[SIX_BIT(bufferPointer.sendTail)] = data; //Add to buffer
        bufferPointer.sendTail++; //Increment index
        if (uartState == initial || uartState == idle) { //If UART is idle,
            beginSend();
            uartState = active;
        }
        return true;
    } else {
        return false; //Buffer is full
    }
}

static void beginSend() {
    EUSCI_A0->TXBUF = transmitBuf[bufferPointer.sendHead]; //Load first byte
}

bool println(uint8_t data) {
    if(!print(data)) {
        return false;
    }
    while( !print(10) );
    while( !print(13) );
    return true;
}

//Simple blocking function that sends a string
void forcePrint(uint8_t *start, uint32_t length) {
    size_t i;
    for (i = 0; i < length; i++) {
        while( !print(start[i]) ); //Wait until added to buffer successfully
    }
    while(uartState != idle); //Wait until entire buffer has been shifted out;
}
