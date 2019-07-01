#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "eparremote.h"

// Passe 4 bits au LCD
void clockLCD(uint8_t arg) {
	_delay_us(1000);
	PORTC |= _BV(E);
	_delay_us(100);
	PORTD = arg | PULLUPS;
	_delay_us(100);
	PORTC &= ~_BV(E);
	_delay_us(100);
}

// Passe 8 bits au LCD
void com(uint8_t command) {
	clockLCD(command & 0xF0);
	clockLCD(command << 4);
	_delay_ms(1);
}

// Ecrit le string en flash
void wrt(const uint8_t *FlashLoc) {
	uint8_t i, chr;
	PORTC |= _BV(RS);
	for(i=0;(chr = (uint8_t)pgm_read_byte(&FlashLoc[i]));i++) {
		com(chr);
	}
	PORTC &= ~_BV(RS);
}

// Ecrit le string en RAM
void wrtr(char *cbuf) {
	uint8_t i, chr;
	PORTC |= _BV(RS);
	for(i=0;(chr = cbuf[i]);i++) {
		com(chr);
	}
	PORTC &= ~_BV(RS);
}

// Ecrit le caractere
void wrtc(char chr) {
	PORTC |= _BV(RS);
	com(chr);
	PORTC &= ~_BV(RS);
}

// Conversion hexa/ascii
uint8_t hex(uint8_t num) {
	if (num > 9) {
		num += 0x37;
	} else {
		num += 0x30;
	}
	return num;
}
