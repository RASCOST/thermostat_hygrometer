/*
 * thermost_hygrometer.c
 *
 * Created: 20/02/2022 10:51:47
 * Author : rasc
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

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



/***********************
   Interrupts handlers
 **********************/
ISR(TIMER0_OVF_vect) {
  tick++;
}

int main(void)
{
    /* Replace with your application code */
	setupTimer0();
	startTimer0();
	
    while (1) 
    {
		//readDHT11();
    }
}

