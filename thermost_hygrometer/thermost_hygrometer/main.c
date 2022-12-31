/*
 * thermost_hygrometer.c
 *
 * Created: 20/02/2022 10:51:47
 * Author : RASC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "lcd.h"

#define F_CPU 8000000UL
#define DHT11 2

const uint8_t weights[8] PROGMEM = {128, 64, 32, 16, 8, 4, 2 , 1};
    
volatile enum states{WAITING, RESPONSE, START_TRANS, RECEIVE}state;
volatile uint8_t resTime = 0;
   
/**********
   PORTC
**********/
void initPORTC(void) {
	DDRC = 0x07; // EN, RS and DHT11 is output when start communication 
}

void setDTH11PortCIN(void) {
	DDRC &=  ~(1 << DHT11);
	//DDRC &= 0xFB;
}

void setDTH11PortCOUT(void) {
	DDRC = (DDRC & 0xFB) | (1 << DHT11);
	//DDRC = (DDRC & 0xFB) | (DDRC | 0x04);
}

/**********
   PORTD
**********/
void initPORTD(void) {
	DDRD = 0xFF; // D3 D2 D1 D0 -> outputs
}

/***********
   Timer 0
 **********/
void setupTimer0() {
  TCCR0A &=  ~(1 << WGM01) | ~(1 << WGM00); // Set NORMAL MODE
  TCCR0B &= ~(1 << WGM02);
}

void startTimer0() {
  TCCR0B |= (1 << CS01); // Prescaler 8
}

void stopTimer0() {
  TCCR0B  &= ~(1 << CS01);
}

/*********
   DHT11
 ********/
uint8_t convertDHT11Response(void) {
    if (resTime >= 23 && resTime <= 33) {
        resTime = 0;
        return 0;
    } else if (resTime >= 65 && resTime <= 75) {
        resTime = 0;
        return 1;
    }
}

void enableDTH11Interrupt(void) {
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT10);
    sei();
}

void disableDHT11Interrupt(void) {
    PCICR &= ~(1 << PCIE1);
    PCMSK1 &= ~(1 << PCINT10);
    cli();
}

void startComDHT11(void) {
    setDTH11PortCOUT();
    PORTC &= ~(1 << DHT11); // start signal pull-down serial bus
    _delay_ms(20);
    PORTC |= (1 << DHT11); // pull-up serial bus
    setDTH11PortCIN(); // wait DTH11
    while ((PINC & (1 << PINC2)));
    while (!(PINC & (1 << PINC2)));
    while ((PINC & (1 << PINC2)));
    while (!(PINC & (1 << PINC2)));
    enableDTH11Interrupt();
    //startTimer0();
}

uint8_t getHygroValue(uint8_t bits[]) {
    uint8_t value = 0;
    
    for (uint8_t idx=0; idx < 8; idx++) {
        if (bits[idx] == 1)
            value += pgm_read_byte(&(weights[idx]));
    }
    
    return value;
}

void readDHT11(void) {
    uint8_t bits[40];
    uint8_t rec = 0;
    uint8_t value = 0;
    char hygro[8];
    
    // Start communication
    startComDHT11();

    while (rec < 40) {
        value = convertDHT11Response();
        
        if (value != -1){
            bits[rec] = value;
            rec++;
        }            
          
    }
  
    // Received all data from DHT11
    disableDHT11Interrupt();
    stopTimer0();
    TCNT0 = 0;
    lcdMoveCursor(2, 0);
    sprintf(hygro, "H: %d", getHygroValue(bits));
    lcdPrint(hygro);
    setDTH11PortCOUT();
}

/***********************
   Interrupts handlers
 **********************/
ISR(PCINT1_vect) {
    char test[8];
    if ((PINC & (1 << PINC2))) {
        TCNT0 = 0;
        startTimer0();
    } else {
        stopTimer0();
        resTime = TCNT0;
        lcdMoveCursor(2, 0);
        sprintf(test, "T: %d", TCNT0);
        lcdPrint(test);
    }   
}

/**********************

**********************/
void initSystem(void) {
	initPORTC();
	initPORTD();
	setupTimer0();
	lcdInit();
	lcdMoveCursor(0, 0);
	lcdPrint("Thermostat");
	//lcdMoveCursor(1, 0);
	//lcdPrint("Hydrometer");
}

int main(void)
{
	initSystem();
	//startTimer0();
	
    while (1) 
    {
        _delay_ms(5000);
		readDHT11();
        //PORTC ^= (1 << DHT11);
       // _delay_ms(1000);
       // PORTC ^= (1 << DHT11);
       // _delay_ms(1000);
    }
	
	return 0;
}

