#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "eparremote.h"

#define Ut 1800 	// 1800 ok
#define Espace 63 	// 63 ok

// MAJ AD9850
void update(uint32_t freq, uint8_t ph) {
	uint8_t bits;

	for (bits=0;bits<32;bits++) {
		if ((freq>>bits) & 1) {
			PORTB |= _BV(AD9850_D7);
		} else {
			PORTB &= ~_BV(AD9850_D7);
		}
		//_delay_us(10);
		PORTB |= _BV(AD9850_W_CLK);
		_delay_us(10);
		PORTB &= ~_BV(AD9850_W_CLK);
		_delay_us(10);
	}

	for (bits=0;bits<8;bits++) {
		if ((ph>>bits) & 1) {
			PORTB |= _BV(AD9850_D7);
		} else {
			PORTB &= ~_BV(AD9850_D7);
		}
		//_delay_us(10);
		PORTB |= _BV(AD9850_W_CLK);
		_delay_us(10);
		PORTB &= ~_BV(AD9850_W_CLK);
		_delay_us(10);
	}

	PORTB |= _BV(AD9850_FQ_UD);		// Valide
	_delay_us(10);
	PORTB &= ~_BV(AD9850_FQ_UD);
	_delay_us(10);
}

void EPAR_TX(uint8_t ccode, uint8_t zone, uint8_t relay, uint8_t state, uint8_t reverse, uint8_t rep) {
	uint8_t bits,c;
	uint16_t mot;

	PORTB = _BV(AD9850_RESET);
	_delay_ms(100);
	PORTB = 0;
	_delay_ms(100);

	mot = ccode | (zone << 8) | (relay << 10) | (state << 11);

	for (c=0;c<rep;c++) {
		Up();
		_delay_us(Ut);
		for (bits=0;bits<12;bits++) {
			Down();
			_delay_us(Ut);
			if ((mot>>bits) & 1) {
				_delay_us(Ut);
				Up();
			} else {
				Up();
				_delay_us(Ut);
			}
			_delay_us(Ut);
		}
		Down();
		_delay_ms(Espace);
	}
}

void Up() {
	update(((fr-(ec/2))*4294967296)/125,0);
}

void Down() {
	update(((fr+(ec/2))*4294967296)/125,0);
}
