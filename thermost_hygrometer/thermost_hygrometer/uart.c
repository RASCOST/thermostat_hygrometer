#include "uart.h"

#define FOSC 8000000
#define BAUDRATE 19200
#define UBRR_VAL ((FOSC/(16L*BAUDRATE))-1)

#define IS_BUFF_EMPTY (UCSR0A & (1 << UDRE0))
#define N_ELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define FALSE 0
#define TRUE 1

void initUART(void) {
    UBRR0H = (uint8_t) (UBRR_VAL >> 8);
    UBRR0L = (uint8_t) (UBRR_VAL);
    UCSR0B |= (1 << TXEN0); // only transmitter next version of file will include preprocessor directives to choose
    UCSR0C |= (1 << UCSZ00); // frame format: 8data, 1 stop bit
}

uint8_t txByteData(uint8_t data) {
    while (!IS_BUFF_EMPTY);
    UDR0 = data;

}

void txMultiByteData(char* data, uint8_t length) {
    for (uint8_t idx = 0; idx < length; idx++) {
        //while (!txByteData(data[idx]));
        txByteData(data[idx]);
    }
}

void txString(char *str ) {
    uint8_t idx = 0;
    
    while (str[idx] != '\0') {
        txByteData(str[idx]);
        idx++;
    }
}