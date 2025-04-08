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
uint8_t transistores = 0;
uint8_t unidades = 0;
uint8_t decenas = 0;
uint8_t unidades_contador = 0;
uint8_t decenas_contador = 0;
uint8_t conversion = 0;
uint8_t bandera_alarma = 0;
const uint8_t tabla[] = {
	0b01111110, // 0
	0b00110000, // 1
	0b01101101, // 2
	0b01111001, // 3
	0b00110011, // 4
	0b01011011, // 5
	0b01011111, // 6
	0b01110000, // 7
	0b01111111, // 8
	0b01111011, // 9
	0b01110111, // A
	0b00011111, // B
	0b01001110, // C
	0b00111101, // D
	0b01001111, // E
	0b01000111, // F
};
/****************************************/
// Function prototypes
void setup();
void initADC();
void showDisplays();
void alarma();
/****************************************/
// Main Function
int main(void)
{
	setup();
	while (1)
	{
		//PORTD = contador;
		showDisplays();
		alarma();
		//PORTC = 0b00000010;
		//PORTD = 0xff;
	}
}
/****************************************/
// NON-Interrupt subroutines
void setup()
{
	cli();
	//CONFIGURAR presecaler general del CLK A 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	//PORTC COMO SALIDA DE LOS TRANSISTORES
	DDRC = 0XFF;
	PORTC = 0X00;
	//PORTD COMO SALIDA
	DDRD = 0XFF;
	UCSR0B = 0X00;
	PORTD = 0X00;
	//PORTB COMO ENTRADA DE LOS BOTONES pb2,pb3 y SALIDA de la alarma en pb4
	DDRB = 0b00010000;
	PORTB = 0b11101111; //ACTIVO PULL UPS
	//interrupciones de timer0
	TCCR0A	= 0X00;	TCCR0B	= (1 << CS01) | (1 << CS00); //prescaler de 64	TCNT0	= 178; //cada 5ms	TIMSK0	= (1 << TOIE0);
	//INTERRUPCIONES PINCHANGE
	PCICR = (1 << PCIE0); // Habilitar PCINT en PORTB
	PCMSK0 = (1 << PCINT3)|(1 << PCINT2);
	initADC();
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

void showDisplays(){
	switch (transistores){
		case 0:
		//aca enseno display decenas
		PORTC = 0x04; //0b0000 1000
		PORTD = tabla[decenas];
		break;
		case 1:
		//ense;o display unidades
		PORTC = 0x08; //0b0001 0000
		
		PORTD = tabla[unidades];
		
		break;
		case 2:
		//pc1 en cero para dar tierra a los leds
		PORTC = 0b00000010;
		PORTD = contador;
		break;
		
		default:
		transistores = 0;
		break;
	}
}
void alarma(){
	if (ADCH >= contador){
		PORTB |= (1 << PORTB4);
	}else{
		PORTB &= ~(1 << PORTB4);
	}
}
/****************************************/
// Interrupt routines
ISR(ADC_vect){
	unidades = ADCH & 0x0F;
	decenas = (ADCH >> 4) & 0x0F;  // Desplazar 4 bits a la derecha y limpiar bits altos
	ADCSRA |= (1 << ADSC); //EMPEZAR EL ADC
	
}
ISR(TIMER0_OVF_vect)
{
	TCNT0	= 178;
	transistores++;
	if (transistores == 3){
		transistores = 0;
	}
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


