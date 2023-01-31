/*
 * lcd.c
 *
 * Created: 15-03-2015 10:04:26
 *  Author: RASC
 */
#define F_CPU 8000000UL
#include "lcd.h"

#define BIT( x ) ( 1 << x )
#define SET_RS( x , y ) ( x |= BIT( y ) )
#define CLEAR_RS( x, y ) ( x &= ~ BIT( y ) )
#define SET_EN( x , y ) ( x |= BIT( y ) )
#define CLEAR_EN( x, y ) ( x &= ~ BIT( y ) )
#define LCD PORTC
#define WRITE_LCD( x ) ( LCD = ( LCD & 0xF0 ) | ( x & 0x0F )  )
#define CMD PORTD
#define EN PORTD2
#define RS PORTD3

static void lcdCmd(uint8_t cmd)
{
    WRITE_LCD(cmd);
	//clear RS LCD instruction mode
	CLEAR_RS(CMD, RS);
	//enable LCD
	SET_EN(CMD, EN);
	CLEAR_EN(CMD, EN);
    _delay_ms(1);	
}

static void lcdData(uint8_t data)
{
    WRITE_LCD(data);
	//set RS LCD data mode
	SET_RS(CMD, RS);
	//enable LCD
	SET_EN(CMD, EN);
	CLEAR_EN(CMD, EN);
    _delay_ms(1);
}

void lcdPrint(char *msg)
{
	
	for(uint8_t i=0; msg[i] !='\0'; i++) {
		lcdData(msg[i] >> 4);
		lcdData(msg[i]);
		_delay_ms(1);
	}
}

void lcdMoveCursor(uint8_t row, uint8_t column)
{
	uint8_t temp = 0;
	
	if(1 == row) {
		temp = 0x80 + column;
		
		lcdCmd(temp >> 4);
		lcdCmd(temp);
	}
	
	if(2 == row) {
		temp = 0xC0 + column;
		
		lcdCmd(temp >> 4);
		lcdCmd(temp);
	}
}

void lcdCursorLeftShift()
{
	lcdCmd(0x01);
	lcdCmd(0x00);
}

void lcdClear()
{
	//Clear
	lcdCmd(0x00);
	lcdCmd(0x01);
	_delay_ms(1);
	//Home
	lcdCmd(0x00);
	lcdCmd(0x02);
}

void lcdCursorBlink()
{
	lcdCmd(0x00);
	lcdCmd(0x0F);
}

void lcdClearCursor()
{
	lcdCmd(0x00);
	lcdCmd(0x0C);
}

void lcdInit( void )
{
	_delay_ms(20);

	lcdCmd(0x03);
	_delay_ms(10);
	
	lcdCmd(0x03);
	_delay_ms(10);
	
	lcdCmd(0x03);
	_delay_ms(10);
	
	//enable four-bit mode
	lcdCmd(0x02);
	_delay_ms(1);
	
	//Set Interface
	lcdCmd(0x02); //4 bit mode
	lcdCmd(0x08); //N lines
	_delay_ms(1);
	
	//Display no cursor - no blink
	lcdCmd(0x00);
	lcdCmd(0x0C);
	_delay_ms(1);
    
    //Set Cursor Move Direction
    lcdCmd(0x00);
    lcdCmd(0x06);
    _delay_ms(1);
	
	//Clear
	lcdCmd(0x00);
	lcdCmd(0x01);
	_delay_ms(1);
	
	//LCD ON
	lcdCmd(0x00);
	lcdCmd(0x0C);
    /*_delay_ms(20);
    lcdCmd(0x03);
    _delay_ms(5);
    lcdCmd(0x03);
    _delay_ms(1);
    lcdCmd(0x03);
    _delay_ms(1);
    lcdCmd(0x02); // 4 bit mode
    _delay_ms(1);
    lcdCmd(0x02); // Function Set 001DL => DL=0 => 0010
    lcdCmd(0x08); // Function Set NFXX => NF=10 => 1000
    _delay_ms(1);
    lcdCmd(0x00); // Display OFF
    lcdCmd(0x08); // Display OFF
    _delay_ms(1);
    lcdCmd(0x00); // Display ON
    lcdCmd(0x0C); // Display ON
    _delay_ms(1);
    lcdCmd(0x00); // Entry Set Mode 0000
    lcdCmd(0x06); // Entry Set Mode 01I/DS => I/D=1 S=0 => 0110
    _delay_ms(1);*/
}
