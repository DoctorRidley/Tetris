/*
 * Controller.c
 *
 * Created: 6/3/2017 1:32:58 PM
 * Author : Benjamin Koshy
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

#define B		0x8000
#define Y		0x4000
#define Select	0x2000
#define Start	0x1000
#define Up		0x0800
#define Down	0x0400
#define Left	0x0200
#define Right	0x0100
#define A		0x0080
#define X		0x0040
#define L		0x0020
#define R		0x0010

#include <avr/io.h>
#include <io.h>
#include <bit.h>

unsigned short buttons;

void Get_Input() {
	unsigned char i = 0;
	PORTA &= 0xF9; // Set Latch and Clock to low
	PORTA |= 0x02; // Set Latch to high
	PORTA &= 0xF9; // Set Latch to low
	buttons = 0;
	buttons = SetBit(buttons, 15, ~PINA & 0x01); // read the first one (B)
	for (i = 0; i < 15; i++) {
		PORTA &= 0xFB; //
		PORTA |= 0x04; // Set Clock to high
		buttons = SetBit(buttons, 14 - i, ~PINA & 0x01);
		PORTA &= 0xF9; // Set Clock to low
	}
}

#endif