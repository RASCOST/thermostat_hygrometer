#include "uart.h"

#define FOSC 800000UL
#define BAUDRATE 19200
#define UBRR_VALUE (FOSC/(16*BAUDRATE)) - 1

#define IS_BUFF_EMPTY UCSR0A & ~(1 << UDRE0)
#define N_ELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define FALSE 0
#define TRUE 1

void initUART(uint16_t baudRate) {
    UBRR0H = (uint8_t)(baudRate >> 8);
    UBRR0L = (uint8_t)(baudRate);
    UCSR0B |= (1 << TXEN0); // only transmitter next version of file will include preprocessor directives to choose
    UCSR0C |= (1 << UCSZ00); // frame format: 8data, 1 stop bit
}

uint8_t txByteData(uint8_t data) {
    if (IS_BUFF_EMPTY) {
        UDRE0 = data;
        return TRUE;
    }
    
    return FALSE;
}

void txMultiByteData(uint8_t* data, uint8_t length) {
    for (uint8_t idx = 0; idx < length; idx++) {
        while (!txByteData(data[idx]));
    }
}