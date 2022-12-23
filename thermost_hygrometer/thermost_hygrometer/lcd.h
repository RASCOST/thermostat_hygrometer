/*
 * lcd.h
 *
 * Created: 15-03-2015 10:04:02
 * Author: RASC
 *
 * Modified: 11-12-2002
 *  - changed char to uint8_t in line 20
 */

#ifndef LCD_H_
#define LCD_H_

#include <util/delay.h>
#include <avr/io.h>

static void lcdCmd( uint8_t cmd );
static void lcdData( uint8_t data );
void lcdPrint( char *msg );
void lcdMoveCursor( uint8_t line, uint8_t row );
void lcdCursorLeftShift();
void lcdClear();
void lcdCursorBlink();
void lcdClearCursor();
void lcdInit( void );

#endif /* LCD_H_ */