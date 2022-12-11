/*
 * thermost_hygrometer.c
 *
 * Created: 20/02/2022 10:51:47
 * Author : rasc
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define DHT11 PC2
#define EN PC0
#define RS PC1

volatile unsigned char tick = 0;

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

int main(void)
{
	setupTimer0();
	startTimer0();
	
    while (1) 
    {
		readDHT11();
    }
}

