/*
 * PWM2.c
 *
 * Created: 4/9/2025 4:30:03 PM
 *  Author: luisp
 */ 
#include <avr/io.h>
#include "PWM2.h"

void PWM2CONFIG() {
	//CONFIGURAMOS 0CR2A COMO LA SALIDA EN PB3
	DDRB |= (1 << PORTB3);
	
	//CONFIGURAMOS FASTPWM NO INVERTIDO
	TCCR2A = (1 << COM2A1)| (1 << WGM21)| (1 << WGM20)  ; 
	
	
	
	TCCR2B |= (1 << CS22);  //PRESCALER DE 64 
}