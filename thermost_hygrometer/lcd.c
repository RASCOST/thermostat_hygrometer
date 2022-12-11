/*
 * lcd.c
 *
 * Created: 15-03-2015 10:04:26
 *  Author: RASC
 */ 
#include "lcd.h"

#define BIT( x ) ( 1 << x )
#define SET_RS( x , y ) ( x |= BIT( y ) )
#define CLEAR_RS( x, y ) ( x &= ~ BIT( y ) )
#define SET_EN( x , y ) ( x |= BIT( y ) )
#define CLEAR_EN( x, y ) ( x &= ~ BIT( y ) )
#ifndef LCD
	#define LCD PORTA
#endif
#define WRITE_LCD( x ) ( LCD = ( LCD & 0xF0 ) | ( x & 0x0F )  )
#ifndef
	#define RS 0
#endif
#ifndef EN
	#define EN 1
#endif

void lcdCmd( uint8_t cmd )
{
	//clear RS LCD instruction mode
	CLEAR_RS( LCD, RS);
	//enable LCD
	SET_EN( LCD, EN);
	WRITE_LCD( cmd );
	CLEAR_EN( LCD, EN);	
}

void lcdData( uint8_t data )
{
	//set RS LCD data mode
	SET_RS( LCD, RS);
	//enable LCD
	SET_EN( LCD, EN);
	WRITE_LCD( data );
	CLEAR_EN( LCD, EN);	
}

void lcdPrint( uint8_t *msg )
{
	
	for( uint8_t i = 0; msg[ i ] != '\0'; i++ )
	{
		lcdData(  msg[i] >> 4 );
		lcdData(  msg[i] );
		_delay_ms( 1 );
	}
}

void lcdMoveCursor( uint8_t row, uint8_t column )
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
	lcdCmd( 0x10 );
	lcdCmd( 0x00 );
}

void lcdClear()
{
	//Clear
	lcdCmd( 0x00 );
	lcdCmd( 0x10 );
	_delay_ms( 1 );
	//Home
	lcdCmd( 0x00 );
	lcdCmd( 0x02 );
}

void lcdCursorBlink()
{
	lcdCmd( 0x00 );
	lcdCmd( 0xF0 );
}

void lcdClearCursor()
{
	lcdCmd( 0x00 );
	lcdCmd( 0xC0 );
}

void lcdInit( void )
{
	_delay_ms( 20 );

	lcdCmd( 0x30 );
	_delay_ms( 10 );
	
	lcdCmd( 0x30 );
	_delay_ms( 10 );
	
	lcdCmd( 0x30 );
	_delay_ms( 10 );
	
	//enable four-bit mode
	lcdCmd( 0x20 );
	_delay_ms( 1 );
	
	//Set Interface
	lcdCmd( 0x20 ); //4 bit mode
	lcdCmd( 0x80 ); //N lines
	_delay_ms( 1 );
	
	//Enable Display/Cursor
	lcdCmd( 0x00 );
	lcdCmd( 0x80 );
	_delay_ms( 1 );
	
	//Clear
	lcdCmd( 0x00 );
	lcdCmd( 0x10 );
	_delay_ms( 1 );
	
	//Set Cursor Move Direction
	lcdCmd( 0x00 );
	lcdCmd( 0x60 );
	_delay_ms( 1 );
	
	//LCD ON
	lcdCmd( 0x00 );
	lcdCmd( 0xC0 );
}
