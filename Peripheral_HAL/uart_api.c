#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "Peripheral_HAL/uart_api.h"


static bool writeBaud();
static void InitUART();
static void EUSCIA0_IRQHandler(void);
static void beginSend();

static uint32_t baudRate = 0;

int32_t sent = 0;


//Buffers for UART
static volatile uint8_t receiveBuf[64];
static volatile uint8_t transmitBuf[64];

//Indexes for buffers
static volatile uint_fast8_t recHead8b = 0;
static volatile uint_fast8_t recTail8b = 0;
static volatile uint_fast8_t sendHead8b = 0;
static volatile uint_fast8_t sendTail8b = 0;
#define recHead (recHead8b & 0x3F)
#define recTail (recTail8b & 0x3F)
#define sendHead (sendHead8b & 0x3F)
#define sendTail (sendTail8b & 0x3F)

enum {initial, idle, active} uartState = initial;

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
        uartConfig.secondModReg = 0; //37?
        break;
    case 115200:
        uartConfig.clockPrescalar = 6;
        uartConfig.firstModReg = 8;
        uartConfig.secondModReg = 0;
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
            if ( sendTail != sendHead) {
                EUSCI_A0->TXBUF = transmitBuf[sendHead]; //Load first byte
                sendHead8b++;
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
    if (sendHead - sendTail != 1) {
        transmitBuf[sendTail] = data; //Add to buffer
        sendTail8b++; //Increment index
        if (uartState == initial || uartState == idle) { //If UART is idle,
            beginSend();
            uartState = active;
        }
        return true;
    } else {
        return false;
    }
}

static void beginSend() {
    EUSCI_A0->TXBUF = transmitBuf[sendHead]; //Load first byte
    sendHead8b++;
}

bool println(uint8_t data) {
    if(!print(data)) {
        return false;
    }
    while(!print(10)){}
    while(!print(13)){}
    return true;
}

