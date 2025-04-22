/*
 * PWM1.c
 *
 * Created: 4/9/2025 11:38:02 AM
 *  Author: luisp
 */ 
#include <avr/io.h>
#include "PWM1.h"

void PMW1CONFIG(uint16_t top, uint16_t prescaler) {
	//CONFIGURAMOS 0CR1A COMO LA SALIDA EN PB1
	DDRB |= (1 << DDB1);
	
	//CONFIGURAMOS FASTPWM
	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13);
	
	
	ICR1 = top;
	
	switch (prescaler) {
		case 1:
		TCCR1B |= (1 << CS10);
		break;
		case 8:
		TCCR1B |= (1 << CS11);
		break;
		case 64:
		TCCR1B |= (1 << CS11) | (1 << CS10);
		break;
		case 256:
		TCCR1B |= (1 << CS12);
		break;
		case 1024:
		TCCR1B |= (1 << CS12) | (1 << CS10);
		break;
		default:
		//64 POR DEFECTO
		TCCR1B |= (1 << CS11) | (1 << CS10);
		break;
	}
}

