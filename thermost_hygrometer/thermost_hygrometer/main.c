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

#define DHT11 PINC2
#define EN PINC0
#define RS PINC1

volatile unsigned char tick = 0;

/**********
   PORTC
**********/
void initPORTC(void) {
	DDRC = 0x07; // EN, RS and DHT11 is output when start communication 
}

void setDTH11PortCIN(void) {
	DDRC &= 0xFB
}

void setDTH11PortCOUT(void) {
	DDRC = (DDRC & 0xFB) | (DDRC | 0x04);
}

/***********
   Timer 0
 **********/
void setupTimer0() {
  TCCR0A |= (1 << WGM01); // Set CTC mode
  OCR0A = 0x0F; // 1 us
  TIMSK0 |= (1 << OCIE0A);// Interrupt compare
  sei();
}

void startTimer0() {
  TCCR0B |= (1 << CS00); // No prescaling
}

void stopTimer0() {
  TCCR0B  |= (0 << CS00);
}

/*********
   DHT11
 ********/
void readDHT11() {
  char number[2];
  // Start communication
  //digitalWrite(dht11, LOW);

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

void initSystem(void) {
	setupTimer0();
	lcdInit();
	lcdMoveCursor(0, 0);
	lcdPrint("Thermostat");
	lcdMoveCursor(1, 0);
	lcdPrint("Thermostat");
}

int main(void)
{
	initSystem();
	//startTimer0();
	
    while (1) 
    {
		//readDHT11();
    }
}

