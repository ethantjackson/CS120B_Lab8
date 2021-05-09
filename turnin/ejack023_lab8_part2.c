/*	Author: ejack023
 *	 *	Lab Section: 023
 *	  *	Assignment: Lab #8  Exercise #1
 *	   *
 *	    *	I acknowledge all content contained herein, excluding template or example
 *	     *	code, is my own original work.
 *	      *
 *	       *	Demo Link: https://www.youtube.com/watch?v=j_-byomxXDI&ab_channel=EthanJackson
 *	        */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	
	if (frequency != current_frequency) {
		if(!frequency) {TCCR3B &= 0x08;}
		else {TCCR3B |= 0x03;}
			
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
			
		else if (frequency > 31250) {OCR3A = 0x0000;}
			
		else {OCR3A = (short)(8000000 / (128*frequency)) - 1;}
			
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

unsigned char btn = 0x00;
unsigned char note = 0;
unsigned char on = 0;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25}; 
enum SM1_STATES {SM1_START, SM1_WAIT, SM1_ON_OFF, SM1_ON_OFF_RELEASE, SM1_UP, SM1_UP_RELEASE, SM1_DOWN, SM1_DOWN_RELEASE} SM1_STATE;
void Tick_Fct() {
	btn = ~PINA;
	switch(SM1_STATE) {
		case SM1_START:
			SM1_STATE = SM1_WAIT;
			break;
		case SM1_WAIT:
			if (btn == 0x01) { SM1_STATE = SM1_ON_OFF; }
			else if (btn == 0x02) { SM1_STATE = SM1_UP; }
			else if (btn == 0x04) { SM1_STATE = SM1_DOWN; }
			break;
		case SM1_ON_OFF:
			SM1_STATE = SM1_ON_OFF_RELEASE;
			break;
		case SM1_ON_OFF_RELEASE:
			if (!btn) SM1_STATE = SM1_WAIT;
			break;
		case SM1_UP:
			SM1_STATE = SM1_UP_RELEASE;
			break;
		case SM1_UP_RELEASE:
		    if (!btn) SM1_STATE = SM1_WAIT;
			break;
		case SM1_DOWN:
			SM1_STATE = SM1_DOWN_RELEASE;
			break;
		case SM1_DOWN_RELEASE:
			if (!btn) SM1_STATE = SM1_WAIT;
			break;
		default:
			SM1_STATE = SM1_WAIT;
			break;
	}
	switch(SM1_STATE) {
		case SM1_START:
			break;
		case SM1_WAIT:
			break;
		case SM1_ON_OFF:
			if (!on) on = 1;
			else on = 0;
			
			if (on) {PWM_on(); set_PWM(notes[note]);}
			else PWM_off();
			break;
		case SM1_ON_OFF_RELEASE:
			break;
		case SM1_UP:
			if (note < 7) {
				++note;
				if (on)
				set_PWM(notes[note]);
			}
			break;
		case SM1_UP_RELEASE:
			break;
		case SM1_DOWN:
			if (note > 0) {
				--note;
				if(on)
				set_PWM(notes[note]);
			}
			break;
		case SM1_DOWN_RELEASE:
			break;
		default:
			break;
	}
};


int main(void)
{
	DDRA = 0X00; PORTA = 0XFF;
	DDRB = 0x40; PORTB = 0x00;
	PWM_off();
	set_PWM(notes[note]);
	SM1_STATE = SM1_START;
	while(1) {
		Tick_Fct();
	}
}

