#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "eparremote.h"
#include "special.h"

const uint8_t cf[] PROGMEM = "CENTER:";
const uint8_t cw[] PROGMEM = "SPAN:";
const uint8_t codet[] PROGMEM = "CODE: ";
const uint8_t zonet[] PROGMEM = "ZONE:";
const uint8_t xmit[] PROGMEM = "TRANSMIT";

const uint8_t r1[] PROGMEM = "REL1:";
const uint8_t r2[] PROGMEM = "REL2:";

const uint8_t on[] PROGMEM = "ON";
const uint8_t off[] PROGMEM = "OFF";

const uint8_t *onoff[2] = {off,on};

const uint8_t za[] PROGMEM = "A(00)";
const uint8_t zb[] PROGMEM = "B(01)";
const uint8_t zc[] PROGMEM = "C(10)";
const uint8_t ztp[] PROGMEM = "TP(11)";

const uint8_t *zonen[4] = {za,zb,zc,ztp};

const uint8_t xmit1[] PROGMEM = "RELAY 1";
const uint8_t xmit2[] PROGMEM = "RELAY 2";
const uint8_t xmit3[] PROGMEM = "DONE ! ";

void cur_rel() {
	com(0b10000101 + reli*64);		// Ligne 1 ou 2
}

void cur_span() {
	if (digitedit > 2) {
		com(0b11000000 + (3-digitedit));	// Ligne 2
	} else {
		com(0b11000000 + (4-digitedit));	// Ligne 2
	}
}

void cur_code() {
	com(0b11000000 + (7-bitedit));	// Ligne 2 + 7-bit
}

void cur_freq() {
	if (digitedit > 3) {
		com(0b11000000 + (5-digitedit));	// Ligne 2, decalage point
	} else {
		com(0b11000000 + (6-digitedit));	// Ligne 2
	}
}

void cur_zone() {
	com(0b11000000);			// Ligne 2
}

void FE_DIGIT() {
	selected ^= 1;
	if (selected) {
		com(0b00001101);		// Cligno on
	} else {
		com(0b00001110);		// Curseur on
	}
}

void FE_REL() {
	relay[reli] ^= 1;			// Inverser etat relais
	disp();						// Maj affichage
	cur_rel();
}

void FE_CODE() {
	citycode ^= (1<<bitedit);	// Inverser etat bit
	disp();						// Maj affichage
	cur_code();
}

void FE_ZONE() {
}

void FE_XMIT() {
}

void FC_FREQ() {
	cur_freq();
	com(0b00001110);	// Curseur on
}

void FC_SPAN() {
	digitedit = 0;
	cur_span();
	com(0b00001110);	// cursor on
}

void FC_REL() {
	cur_rel();
	com(0b00001101);	// Cligno on
}

void FC_CODE() {
	cur_code();
	com(0b00001101);	// Cligno on
}

void FC_ZONE() {
	cur_zone();
	com(0b00001101);	// Cligno on
}

void FC_XMIT() {
	com(0b11000000);	// Ligne 2: "RELAIS 1"
	wrt(xmit1);
	EPAR_TX(citycode,zone,RELAY1,relay[0],NORMAL,REPEAT);
	com(0b11000000);	// Ligne 2: "RELAIS 2"
	wrt(xmit2);
	EPAR_TX(citycode,zone,RELAY2,relay[1],NORMAL,REPEAT);
	com(0b11000000);	// Ligne 2: "TERMINE!"
	wrt(xmit3);
	update(0,POWERDOWN);
	editing = 0;
}

void FLE_FREQ() {
	if (selected) {
		fr -= power[digitedit];	// Decrementer frequence
		if (fr < FREQ_MIN) fr = FREQ_MIN;	// Limite basse
		disp();
	} else {
		if (digitedit == 5) {	// Navig chiffre frequence
			digitedit = 0;
		} else {
			digitedit++;
		}
	}
	cur_freq();
}

void FLE_SPAN() {
	if (selected) {
		ec -= power[digitedit+1];
		if (ec < ECART_MIN) ec = ECART_MIN;
		disp();
	} else {
		if (digitedit == 3) {
			digitedit = 0;
		} else {
			digitedit++;
		}
	}
	cur_span();
}

void FLE_REL() {
	reli ^= 1;				// Choisir l'autre relais
	cur_rel();
}

void FLE_CODE() {
	if (bitedit == 7) {		// Navig bit
		bitedit = 0;
	} else {
		bitedit++;
	}
	cur_code();
}

void FLE_ZONE() {
	if (zone==0) {			// Selection zone, limite basse et warp
		zone=3;
	} else {
		zone--;
	}
	disp();
	cur_zone();
}

void FLE_XMIT() {
}

void FRE_FREQ() {
	if (selected) {
		fr += power[digitedit];	// Incrementer frequence
		if (fr > FREQ_MAX) fr = FREQ_MAX;	// Limite haute
		disp();
	} else {
		if (digitedit == 0) {	// Navig chiffre frequence
			digitedit = 5;
		} else {
			digitedit--;
		}
	}
	cur_freq();
}

void FRE_SPAN() {
	if (selected) {
		ec += power[digitedit+1];
		if (ec > ECART_MAX) ec = ECART_MAX;
		disp();
	} else {
		if (digitedit == 0) {
			digitedit = 3;
		} else {
			digitedit--;
		}
	}
	cur_span();
}

void FRE_REL() {
	reli ^= 1;				// Choisir l'autre relais
	cur_rel();
}

void FRE_CODE() {
	if (bitedit == 0) {		// Navig bit
		bitedit = 7;
	} else {
		bitedit--;
	}
	cur_code();
}

void FRE_ZONE() {
	if (zone==3) {			// Selection zone, limite haute et warp
		zone=0;
	} else {
		zone++;
	}
	disp();
	cur_zone();
}

void FRE_XMIT() {
}

void FD_FREQ() {
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(cf);
	dtostrf(fr,3,4,test);
	com(0b11000000);		// Ligne 2
	wrtr(test);
}

void FD_SPAN() {
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(cw);
	dtostrf(ec,3,3,test);
	com(0b11000000);		// Ligne 2
	wrtr(test);
}

void FD_REL() {
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(r1);
	wrt(onoff[relay[0]]);
	com(0b11000000);		// Ligne 2
	wrt(r2);
	wrt(onoff[relay[1]]);
}

void FD_CODE() {
	uint8_t chr, bits;
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(codet);
	wrtc(hex(citycode>>4));	// Code en hexa
	wrtc(hex(citycode & 0xF));
	com(0b11000000);		// Ligne 2
	for (bits=0;bits<8;bits++) {		// Affichage code en binaire
		if ((citycode << bits) & 0x80) {
			chr = '1';
		} else {
			chr = '0';
		}
		wrtc(chr);
	}
}

void FD_ZONE() {
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(zonet);
	com(0b11000000);		// Ligne 1
	wrt(zonen[zone]);
}

void FD_XMIT() {
	com(0b00000001);		// Clear
	com(0b10000000);		// Ligne 1
	wrt(xmit);
}

void FS_FREQ() {
	eeprom_write_block((const void*)&fr, (void*)&EEfr, sizeof(double));
}

void FS_SPAN() {
	eeprom_write_block((const void*)&ec, (void*)&EEec, sizeof(double));
}

void FS_REL() {
}

void FS_CODE() {
	//eeprom_write_byte(&EEcode,citycode);
}

void FS_ZONE() {
	eeprom_write_byte(&EEzone,zone);
}

void FS_XMIT() {
}
