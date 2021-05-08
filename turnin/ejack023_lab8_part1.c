/*	Author: ejack023
 *	 *	Lab Section: 023
 *	  *	Assignment: Lab #8  Exercise #1
 *	   *
 *	    *	I acknowledge all content contained herein, excluding template or example
 *	     *	code, is my own original work.
 *	      *
 *	       *	Demo Link: https://www.youtube.com/watch?v=13p0KkZdjQA&ab_channel=EthanJackson
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
enum SM1_STATES {SM1_START, SM1_WAIT, SM1_C4, SM1_D4, SM1_E4} SM1_STATE;
void Tick_Fct() {
	btn = ~PINA;
	switch(SM1_STATE) {
		case SM1_START:
			SM1_STATE = SM1_WAIT;
			break;
		case SM1_WAIT:
			if (btn == 0x01) { PWM_on(); SM1_STATE = SM1_C4; }
			else if (btn == 0x02) { PWM_on(); SM1_STATE = SM1_D4; }
			else if (btn == 0x04) { PWM_on(); SM1_STATE = SM1_E4; }
			break;
		case SM1_C4:
			if (btn != 0x01) SM1_STATE = SM1_WAIT;
			break;
		case SM1_D4:
			if (btn != 0x02) SM1_STATE = SM1_WAIT;
			break;
		case SM1_E4:
			if (btn != 0x04) SM1_STATE = SM1_WAIT;
			break;
		default:
			SM1_STATE = SM1_WAIT;
			break;
	}
	switch(SM1_STATE) {
		case SM1_START:
			break;
		case SM1_WAIT:
			PWM_off();
			break;
		case SM1_C4:
			set_PWM(261.63);
			break;
		case SM1_D4:
			set_PWM(293.66);
			break;
		case SM1_E4:
			set_PWM(329.63);
			break;
		default:
			break;
	}
};


int main(void)
{
	DDRA = 0X00; PORTA = 0XFF;
	DDRB = 0x40; PORTB = 0x00;
	PWM_on();
	SM1_STATE = SM1_START;
	while(1) {
		Tick_Fct();
	}
}

