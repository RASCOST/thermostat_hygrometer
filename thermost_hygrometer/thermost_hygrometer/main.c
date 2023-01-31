/*
 * thermost_hygrometer.c
 *
 * Created: 20/02/2022 10:51:47
 * Author : RASC
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/cpufunc.h>

#include "lcd.h"
#include "uart.h"

#define PD_DHT11 4

#define SET_DHT11PIN_INPUT (DDRD &= ~(1 << PD_DHT11))
#define SET_DHT11PIN_OUTPUT (DDRD |= (1 << PD_DHT11))
#define N_ELEMS(x)  (sizeof(x) / sizeof((x)[0]))

const uint8_t weights[8] PROGMEM = {128, 64, 32, 16, 8, 4, 2 , 1};
    
enum states{START, WAIT_RESP_LOW, WAIT_RESP_HIGH, START_TRANS, RCV_DATA};
volatile enum states state = START;

volatile uint8_t data_ticks = 0;
   
/**********
   PORTC
**********/
void initPORTC(void) {
    DDRC = 0xFF; // D3 D2 D1 D0 -> outputs  
}

/**********
   PORTD
**********/
void initPORTD(void) {
    DDRD = 0x0C; // EN, RS
}

void setDTH11PortDIN(void) {
    DDRD &=  ~(1 << PD_DHT11); // set PORTD4 as input
}

void setDTH11PortDOUT(void) {
    DDRD |= (1 << PD_DHT11); // set PORTD4 as output
}

/***********
   Timer 0
 **********/
void setupTimer0(void) {
  TCCR0A &=  (~(1 << WGM01) | ~(1 << WGM00)); // Set NORMAL MODE
  TCCR0B &= ~(1 << WGM02);
}

void startTimer0(void) {
  TCCR0B |= (1 << CS01); // Prescaler 8
}

void stopTimer0(void) {
  TCCR0B  &= ~(1 << CS01);
}

/****************
* Utils
****************/
void charToAscii(uint8_t number, char* ascii)
{    
    if(10 <= number)
    {
        ascii[0] = ((uint8_t) (number / 10)) + 0X30; //0X30 is the ASCII value of zero
        ascii[1] = (number - ((ascii[0]-0X30) * 10)) + 0X30;
    }
    else
    {
        ascii[0] = 0X30;
        ascii[1] = 0X30 + number;
    }
}

/*********
   DHT11
 ********/
int8_t convertDHT11Response(uint8_t* bits) {
    uint8_t number = 0;
    
    for (uint8_t n=0; n < 8; n++) {
        number += bits[n] * weights[n];
    }
    
    return number;
}

void enableDTH11Interrupt(void) {
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20);
    sei();
}

void disableDHT11Interrupt(void) {
    PCICR &= ~(1 << PCIE2);
    PCMSK2 &= ~(1 << PCINT20);
    cli();
}


uint8_t getHygroValue(uint8_t bits[]) {
    uint8_t value = 0;
    
    for (uint8_t idx=0; idx < 8; idx++) {
        if (bits[idx] == 1)
            value += pgm_read_byte(&(weights[idx]));
    }
    
    return value;
}

uint8_t getTempValue(uint8_t bits[]) {
    uint8_t value = 0;
    
    for (uint8_t idx=16; idx < 24; idx++) {
        if (bits[idx] == 1)
        value += pgm_read_byte(&(weights[idx-16]));
    }
    
    return value;
}

void readDHT11(void) {
    uint8_t bits[40];
    char ascii[2];
    
    // Start communication
    SET_DHT11PIN_OUTPUT;
    _delay_ms(20);
    SET_DHT11PIN_INPUT;
    enableDTH11Interrupt();
    
    // Receiving data
    for (uint8_t bit=0; bit < 40; bit++) {
        //txString("in data\n");
        while(data_ticks == 0);
        
        if (data_ticks > 30) {
            bits[bit] = 1;
        } else {
            bits[bit] = 0;
        }
        
        data_ticks = 0;
    }

    // Finished Receiving data
    disableDHT11Interrupt();
    stopTimer0();
    state = START;
    
    charToAscii(getHygroValue(bits), ascii);
    //txMultiByteData(ascii, 2);
    lcdMoveCursor(2, 3);
    lcdPrint(ascii);
    charToAscii(getTempValue(bits)-2, ascii);
    //txMultiByteData(ascii, 2);
    lcdMoveCursor(1, 3);
    lcdPrint(ascii);
}

/***********************
   Interrupts handlers
 **********************/
ISR(PCINT2_vect) {
    
    switch(state) {
        case START:
            if (!(PIND & (1<<PD_DHT11))) {
                state = WAIT_RESP_LOW;
            }
            break;
            
        case WAIT_RESP_LOW:
            if ((PIND & (1<<PD_DHT11))) {
               state = WAIT_RESP_HIGH;
            }
           break;
           
        case WAIT_RESP_HIGH:
            if (!(PIND & (1<<PD_DHT11))) {
                state = START_TRANS;
            }
            break;
        
        case START_TRANS:
            if ((PIND & (1<<PD_DHT11))) {
                startTimer0();
                state = RCV_DATA;
            }
            break;
        
        case RCV_DATA:
            if (!(PIND & (1<<PD_DHT11))) {
                stopTimer0();
                state = START_TRANS;
                data_ticks = TCNT0;
                TCNT0 = 0;
            }
            break;
    }
}

/**********************

**********************/
void initSystem(void) {
	initPORTC();
	initPORTD();
    initUART();
	setupTimer0();
	lcdInit();
	lcdMoveCursor(1, 0);
	lcdPrint("Thermost");
	lcdMoveCursor(2, 0);
	lcdPrint("Hygrome");
}

int main(void)
{
	initSystem();
    //txString("Hello from Thermostat and Hygrometer\n");
    _delay_ms(5000);
    lcdClear();
    //lcdMoveCursor(1, 0);
    lcdPrint("T: ");
    lcdMoveCursor(2, 0);
    lcdPrint("H: ");
    while (1) 
    {
        readDHT11();
        _delay_ms(5000);
    }
	
	return 0;
}

