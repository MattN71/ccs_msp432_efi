#ifndef UART_API_H
#define UART_API_H

#include <stdint.h>
#include <stdbool.h>

//Six-bit type
#define SIX_BIT(x) ((x) & 0x3F)

uint32_t UARTgetBaud();
bool UARTsetBaud(uint32_t baud);
bool print(uint8_t data);
bool println(uint8_t data);


#endif /* UART_API_H */
