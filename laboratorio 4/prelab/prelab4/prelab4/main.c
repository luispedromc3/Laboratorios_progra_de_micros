/*
*
* prelab4.c
*
* Created: 4/1/2025 4:59:00 PM
* Author: Luis Pedro Monterroso
* Description: contador binario de 8 bits
*/
/****************************************/
// Encabezado (Libraries)
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
uint8_t contador = 0;
/****************************************/
// Function prototypes
void setup();
/****************************************/
// Main Function
int main(void)
{
	setup();
	while (1)
	{
		PORTD = contador;
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup()
{
	cli();
	//CONFIGURAR presecaler general del CLK A 1MHz
	CLKPR = (1 << CLKPCE);
	//CLKPR = (1 << CLKPS2);
	//PORTD COMO SALIDA 
	DDRD = 0XFF;
	UCSR0B = 0X00;
	PORTD = 0X00;
	//PORTB COMO ENTRADA DE LOS BOTONES pb2 y pb3
	DDRB = 0X00;
	PORTB = 0XFF; //ACTIVO PULL UPS
	//interrupciones de timer0
	TCCR0A	= 0X00;	TCCR0B	= (1 << CS01) | (1 << CS00); //prescaler de 64	TCNT0	= 178; //cada 5ms	TIMSK0	= (1 << TOIE0);
	//INTERRUPCIONES PINCHANGE
	PCICR = (1 << PCIE0); // Habilitar PCINT en PORTB
	PCMSK0 = (1 << PCINT3)|(1 << PCINT2);											
	sei();
}
/****************************************/
// Interrupt routines
ISR(TIMER0_OVF_vect)
{
	TCNT0	= 178;
}
ISR(PCINT0_vect)
{
	if (!(PINB & (1 << PB2))) { // Si PB2 es presionado (LOW)
		contador++;
	}
	if (!(PINB & (1 << PB3))) { // Si PB3 es presionado (LOW)
		contador--;
	}
}

