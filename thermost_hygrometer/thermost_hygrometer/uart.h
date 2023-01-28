#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void initUART(void);
uint8_t txByteData(uint8_t data);
void txMultiByteData(uint8_t* data, uint8_t length);
void txString(char *str );
#endif //UART_H_