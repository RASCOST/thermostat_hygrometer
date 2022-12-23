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
#define LCD PORTD
#define WRITE_LCD( x ) ( LCD = ( LCD & 0xF0 ) | ( x & 0x0F )  )
#define CMD PORTC
#define EN PORTC0
#define RS PORTC1

static void lcdCmd(uint8_t cmd)
{
	//clear RS LCD instruction mode
	CLEAR_RS(CMD, RS);
	//enable LCD
	SET_EN(CMD, EN);
	WRITE_LCD(cmd);
	CLEAR_EN(CMD, EN);	
}

static void lcdData(uint8_t data)
{
	//set RS LCD data mode
	SET_RS(CMD, RS);
	//enable LCD
	SET_EN(CMD, EN);
	WRITE_LCD(data);
	CLEAR_EN(CMD, EN);	
}

void lcdPrint(char *msg)
{
	
	for( uint8_t i = 0; msg[ i ] != '\0'; i++ )
	{
		lcdData(  msg[i] >> 4 );
		lcdData(  msg[i] );
		_delay_ms( 1 );
	}
}

void lcdMoveCursor(uint8_t row, uint8_t column)
{
	uint8_t temp = 0;
	
	if( 1 == row )
	{
		temp = 0x80 + column;
		
		lcdCmd( temp >> 4 );
		lcdCmd( temp );
	}
	
	if( 2 == row )
	{
		temp = 0xC0 + column;
		
		lcdCmd( temp >> 4 );
		lcdCmd( temp );
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

	lcdCmd( 0x03 );
	_delay_ms(10);
	
	lcdCmd(0x03);
	_delay_ms(10);
	
	lcdCmd(0x03);
	_delay_ms(10);
	
	//enable four-bit mode
	lcdCmd( 0x02 );
	_delay_ms( 1 );
	
	//Set Interface
	lcdCmd(0x02); //4 bit mode
	lcdCmd(0x08); //N lines
	_delay_ms(1);
	
	//Enable Display/Cursor
	lcdCmd(0x00);
	lcdCmd(0x0C);
	_delay_ms(1);
	
	//Clear
	lcdCmd(0x00);
	lcdCmd(0x01);
	_delay_ms( 1 );
	
	//Set Cursor Move Direction
	lcdCmd(0x00);
	lcdCmd(0x06);
	_delay_ms( 1 );
	
	//LCD ON
	lcdCmd(0x00);
	lcdCmd(0x0C);
}
