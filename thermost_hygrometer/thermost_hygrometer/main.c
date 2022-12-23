/*
 * thermost_hygrometer.c
 *
 * Created: 20/02/2022 10:51:47
 * Author : RASC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lcd.h"

#define F_CPU 8000000UL
#define DHT11 2

volatile unsigned char tick = 0;

enum states{WAITING, RESPONSE, START_TRANS} state;
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
void enableDTH11Interrupt(void) {
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT10);
}

void startComDHT11(void) {
    setDTH11PortCOUT();
    PORTC &= ~(1 << DHT11); // start signal pull-down serial bus
    _delay_ms(20);
    PORTC |= (1 << DHT11); // pull-up serial bus
    setDTH11PortCIN(); // wait DTH11
    startTimer0();
}

void readDHT11(void) {
  char number[2];
  // Start communication
  startComDHT11();

  while (tick <= 30) {
    itoa(tick, number, 10);
  }

  //stopTimer0();
  tick = 0;

}

/***********************
   Interrupts handlers
 **********************/
ISR(TIMER0_OVF_vect) {
  tick++;
}

ISR(PCINT1) {
    
}

void initSystem(void) {
	initPORTC();
	initPORTD();
	//setupTimer0();
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
		//readDHT11();
        //PORTC ^= (1 << DHT11);
       // _delay_ms(1000);
       // PORTC ^= (1 << DHT11);
       // _delay_ms(1000);
    }
	
	return 0;
}

