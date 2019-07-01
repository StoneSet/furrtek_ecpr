/*
Telecommande EPAR v1.1
07/10/2012 CC by Furrtek
ATMega8 8Mhz RC
*/

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "special.h"
#include "lcd.h"
#include "eparremote.h"
#include "navig.h"

typedef void (*FuncPtr)();
FuncPtr FPtr;

uint8_t editing=0;		// Bool valeur en edition
uint8_t selected=0;		// Bool chiffre choisi
uint8_t reli=0;			// # relais en edition
uint8_t bitedit=0;		// # bit en edition
uint8_t digitedit=0;	// # chiffre en edition

uint8_t relay[2] = {0,0};	// Etat relais
uint8_t menu = MENU_CODE;	// Menu (au demarrage)
double fr;					// Valeur frequence centrale (MHz)
double ec;					// Valeur ecart frequence (MHz)
uint8_t citycode;
uint8_t zone;

uint8_t EEMEM EEcode;		// Sauvegardes en EEPROM
uint8_t EEMEM EEzone;
uint8_t EEMEM EEmenu;
uint32_t EEMEM EEfr;
uint32_t EEMEM EEec;

const uint8_t fwa[] PROGMEM = "TLC EPAR";	// Splash pour se la peter
const uint8_t fwb[] PROGMEM = "Ver. 1.1";
const uint8_t fwc[] PROGMEM = "Furrtek\"";
const uint8_t fwd[] PROGMEM = "07/10/12";

// Table de handlers pour les menus
const FuncPtr FTableEditing[] PROGMEM = {FE_DIGIT,FE_DIGIT,FE_REL,FE_CODE,FE_ZONE,FE_XMIT};
const FuncPtr FTableChoose[] PROGMEM = {FC_FREQ,FC_SPAN,FC_REL,FC_CODE,FC_ZONE,FC_XMIT};
const FuncPtr FTableLeftEditing[] PROGMEM = {FLE_FREQ,FLE_SPAN,FLE_REL,FLE_CODE,FLE_ZONE,FLE_XMIT};
const FuncPtr FTableRightEditing[] PROGMEM = {FRE_FREQ,FRE_SPAN,FRE_REL,FRE_CODE,FRE_ZONE,FRE_XMIT};
const FuncPtr FTableDisp[] PROGMEM = {FD_FREQ,FD_SPAN,FD_REL,FD_CODE,FD_ZONE,FD_XMIT};
const FuncPtr FTableSave[] PROGMEM = {FS_FREQ,FS_SPAN,FS_REL,FS_CODE,FS_ZONE,FS_XMIT};

const double power[6] = {0.0001,0.001,0.01,0.1,1,10};	// Puissances de 10

void disp() {
	// MAJ affichage LCD
	FPtr=(FuncPtr)pgm_read_word(&FTableDisp[menu]);
	FPtr();
}

int main(void) {
	WDTCR = (1<<WDCE) | (1<<WDE);	// Watchdog off (dangereux)
	WDTCR = 0x00;

	DDRB = 0b00001111;	// Outputs vers AD9850
	DDRD = 0b11110000;	// Data LCD
	DDRC = 0b00000011;	// Control LCD

	PORTD = 0b00100000 | PULLUPS;	// Mode 8 bits
	_delay_us(100);
	PORTC |= _BV(E);
	_delay_us(100);
	PORTC &= ~_BV(E);
	_delay_us(100);

	com(0b00101000);		// 2 lignes
	com(0b00000001);		// Clear
	com(0b00000010);		// Home
	com(0b00001100);		// Ecran on, cursor off, blink off

	com(0b10000000);		// Ligne 1
	wrt(fwa);
	com(0b11000000);		// Ligne 2
	wrt(fwb);

	_delay_ms(1000);

	com(0b10000000);		// Ligne 1
	wrt(fwc);
	com(0b11000000);		// Ligne 2
	wrt(fwd);

	_delay_ms(1000);

	PORTB = _BV(AD9850_RESET);	// Init mode serie AD9850
	_delay_ms(100);
	PORTB = 0;
	_delay_ms(100);

	PORTB = _BV(AD9850_W_CLK);
	_delay_ms(1);
	PORTB = 0;
	_delay_ms(1);

	PORTB = _BV(AD9850_FQ_UD);
	_delay_ms(1);
	PORTB = 0;
	_delay_ms(1);

	update(0,POWERDOWN);	// DDS au dodo

	// Recup sauvegarde EEPROM + sanitizing
	//menu = eeprom_read_byte(&EEmenu);
	//if (menu > MENU_LAST) menu = MENU_FIRST;
	citycode = eeprom_read_byte(&EEcode);
	zone = eeprom_read_byte(&EEzone);
	if (zone > 3) zone = 3;

	eeprom_read_block((void*)&ec, (const void*)&EEec, sizeof(double));
	if ((((*(uint32_t*)&ec) & 0x7fffffff) > 0x7f800000)||(ec < ECART_MIN)||(ec > ECART_MAX)) ec = 0.003;
	eeprom_read_block((void*)&fr, (const void*)&EEfr, sizeof(double)); 
	if ((((*(uint32_t*)&fr) & 0x7fffffff) > 0x7f800000)||(fr < FREQ_MIN)||(fr > FREQ_MAX)) fr = 31;

	disp();

	for (;;) {
		while ((PIND & PULLUPS) == PULLUPS) {};
		_delay_ms(50);
		if (!bit_is_set(PIND,PD0)) {
			// Gauche
			while(!bit_is_set(PIND,PD0)) {};
			if (editing) {
				// Modif valeur
				FPtr=(FuncPtr)pgm_read_word(&FTableLeftEditing[menu]);
				FPtr();
			} else {
				// Menu precedent
				if (menu == MENU_FIRST) {
					menu = MENU_LAST;
				} else {
					menu--;
				}
				disp();
			}
		} else if (!bit_is_set(PIND,PD3)) {
			// Droite
			while(!bit_is_set(PIND,PD3)) {};
			if (editing) {
				// Modif valeur
				FPtr=(FuncPtr)pgm_read_word(&FTableRightEditing[menu]);
				FPtr();
			} else {
				// Menu suivant
				if (menu == MENU_LAST) {
					menu = MENU_FIRST;
				} else {
					menu++;
				}
				disp();
			}
		} else if (!bit_is_set(PIND,PD1)) {
			// Edition
			while(!bit_is_set(PIND,PD1)) {};
			if (editing) {
				// Modif valeur
				FPtr=(FuncPtr)pgm_read_word(&FTableEditing[menu]);
				FPtr();
			} else {
				// Selection valeur
				eeprom_write_byte(&EEmenu,menu);
				editing = 1;
				digitedit = 0;
				FPtr=(FuncPtr)pgm_read_word(&FTableChoose[menu]);
				FPtr();
			}
		} else if (!bit_is_set(PIND,PD2)) {
			// Validation
			while(!bit_is_set(PIND,PD2)) {};
			editing = 0;
			selected = 0;
			com(0b00001100);		// blink et cursor off
			FPtr=(FuncPtr)pgm_read_word(&FTableSave[menu]);
			FPtr();
		}
	}

  return 0;
}
