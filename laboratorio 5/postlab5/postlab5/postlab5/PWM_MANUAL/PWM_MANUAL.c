/*
 * PWM_MANUAL.c
 *
 * Created: 4/22/2025 12:55:25 AM
 *  Author: luisp
 */ 
#include <avr/io.h>
#include "PWM_MANUAL.h"
void initTIMER0(){
	TCCR0A = 0x00; //modo normal
	TCCR0B = (1 << CS01)| (1 << CS00) ;// Prescaler
	TCNT0 = 50; //
	TIMSK0 |= (1 << TOIE0); // Habilitar interrupción por overflow
}