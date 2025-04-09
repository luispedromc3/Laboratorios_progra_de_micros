/*
 * prelab5.c
 *
 * Created: 4/9/2025 11:28:47 AM
 * Author: Luis Pedro Monterroso
 * Description: 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM1/PWM1.h"
/****************************************/
// Function prototypes
void setup();
void initADC();
void PMW1CONFIG(uint16_t top, uint16_t prescaler);
void CICLODETRABAJO(uint16_t VAL, uint16_t LIMITE_INF, uint16_t LIMITE_SUP);
/****************************************/
// Main Function
int main(void)
{
	setup();
	PMW1CONFIG(312,64);
	while (1)
	{
		CICLODETRABAJO(ADCH,7,37);
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup() {
	cli();
	//CONFIGURAR presecaler general del CLK A 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//adc
	initADC();
	//salida pwm en d6
	DDRB |= (1 << DDB1);
	
	sei();
}
void initADC(){
	ADMUX = 0;
	ADMUX |= (1 << REFS0) ; //VCC COMO COMPARADOR
	ADMUX |= (1 << ADLAR) ; //JUSTIFICADO A LA IZQUIERDA USAR HIGH
	ADMUX |= (1 << MUX2)|(1 << MUX1); //ADC6
	ADCSRA = 0;
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //PRESCALER DE 128
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE); //ACTIVAR INTERRUPCIONES
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
}
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
	
}
